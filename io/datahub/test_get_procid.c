#include "hdf5.h"
#include "time.h"
#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "get_procid.h"
#include "getopt.h"
#define NAME_MAX 255

char filename[NAME_MAX];
char dataset[NAME_MAX];
char cb_buffer_size[NAME_MAX];
char cb_nodes[NAME_MAX];

struct sendrecv{
  int *src;  //rank id(s) of data source
  int total; // total srcs
};
int main(int argc, char ** argv)
{
 int rank,nproc,nid;
 MPI_Status status;
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 MPI_Comm_size(MPI_COMM_WORLD, &nproc);
 int haswell[2];
 int knl[2];
 int hasmax=0;
 int knlmax=0;
 int c;
 MPI_Info info;
 hid_t   file_id2, dset_id2, plist_id3, plist_id4, dataspace_id2,result_space, result_memspace_id;
 hsize_t result_offset[2], result_count[2], result_memspace_size[2], my_z_size;
 hsize_t dims2[2], dims_x, dims_y, dims_z;

 double *data_t;
 int i,j,k;

 strncpy(filename, "./test.h5", NAME_MAX);
 strncpy(cb_buffer_size,"16777216", NAME_MAX);
 strncpy(cb_nodes, "16", NAME_MAX);
 dims_x = 2000;
 dims_y = 1000;
 strncpy(dataset,"temperatures",NAME_MAX);
 int col=1;//collective write


 while ((c = getopt (argc, argv, "f:b:k:n:x:y:z:l:m:")) != -1)
    switch (c)
      {
      case 'f':
        strncpy(filename, optarg, NAME_MAX);
        break;
      case 'b':
        strncpy(cb_buffer_size,optarg, NAME_MAX);
        break;
      case 'n':
        strncpy(cb_nodes, optarg, NAME_MAX);
        break;
      case 'x':
        dims_x = strtoull(optarg, NULL, 10);
        break;
      case 'y':
        dims_y = strtoull(optarg, NULL, 10);
        break;
      case 'k':
        col = strtol(optarg,NULL, 10);
        break;
      case 'l':
        hasmax = strtol(optarg, NULL, 10);
        break;
      case 'm':
        knlmax = strtol(optarg, NULL, 10);
      default:
        break;
      }


//Locate Nid and Color Partitions

 haswell[0]=0;          //min id on haswell
 haswell[1]=hasmax;        //max id on haswell
 knl[0]=hasmax+1;            //min id on knl
 knl[1]=knlmax;           //max id on knl

 int color[1];
 int color_list[nproc];
 memset(color_list,0,sizeof(int)*nproc);
 MPI_Comm comm_kid=rank_split(MPI_COMM_WORLD, haswell, knl, color, color_list); //Split Haswell and Knl partitions, return their  colors,i.e., either haswell or knl
 int global_color_list[nproc];
 MPI_Reduce(color_list,global_color_list,nproc,MPI_INT,MPI_MAX,0,MPI_COMM_WORLD); // get the global color list
 int rank_kid,nproc_kid;
 MPI_Comm_rank(comm_kid,&rank_kid);
 MPI_Comm_size(comm_kid,&nproc_kid);
 //printf(" Rank: %10d, Rank_kid: %10d, Total_Kids: %10d, Total: %10d\n ",rank,rank_kid,nproc_kid,nproc);

 float file_size = (double)dims_x*(double)dims_y*(double)sizeof(double)/1024.0/1024.0/1024.0;
 

//Process Load Balancing
//Partition along X

 result_offset[1] = 0;
 result_offset[0] =  (dims_x / nproc) * rank; //later need to take care edge case
 result_count[0] = dims_x / nproc;
 result_count[1] = dims_y;

//Data Allocation and Generation for each rank:
 data_t = (double *)malloc(result_count[0] * result_count[1] * sizeof(double));
 if(data_t==NULL){
  printf("malloc error \n");
  MPI_Abort(MPI_COMM_WORLD,711);
 }
 srand((unsigned int)time(NULL));
 double my_size = ((double)dims_x * (double)dims_y *sizeof(double)) / 1024.0 / 1024.0 / 1024.0;
 for (i = 0; i < result_count[0]; i++){
  for(j = 0; j < result_count[1]; j++){
      data_t[i*result_count[1]+j] = ((double)rand()/(double)(RAND_MAX)) * i+0.001;
  }
 }


//Compute Bandwidth

//Re-distribute Data among colored groups
//Get the exact ranks id that belongs to each partition:
int * rank_has = (int *) malloc(1*sizeof(int));
int * rank_knl = (int *) malloc(1*sizeof(int));
int i_rank_has=0;
int i_rank_knl=0;
for(i=0;i<nproc;i++){
 if (global_color_list[i]==1){  //rank is on haswell partition
    rank_has[i_rank_has]=i;
    i_rank_has+=1;
    rank_has=realloc(rank_has,(i_rank_has+1)*sizeof(int)); //increase array size
 }
 else{
    rank_knl[i_rank_knl]=i;
    i_rank_knl+=1;
    rank_knl=realloc(rank_knl,(i_rank_knl+1)*sizeof(int)); //increase array size
 }
}
//Realloc Memory on Haswell
//if i_rank_has == i_rank_knl: just double current size
//if i_rank_has <  i_rank_knl: round-robin elarge the buffer size
//if i_rank_has >  i_rank_knl: loop over knl
/*

struct sendrecv{
  int *src;  //rank id(s) of data source
  int total=0; // total srcs
};

*/
//calculate recieve and send
struct sendrecv * isr=(struct sendrecv *)malloc(1*sizeof(struct sendrecv));
isr->src=(int *)malloc(sizeof(int));
isr->total=0;
printf("rank:%d\n",rank);
if (i_rank_knl<=i_rank_has){ // safe case, haswell nodes are enough to handle knl workload
 for (i=0;i<i_rank_knl;i++){ // 1 to 1
  int cur_has_rank=rank_has[i];
  int cur_knl_rank=rank_knl[i];
 // printf("cur rank:%d,cur knl:%d, cur has:%d\n",rank,cur_knl_rank,cur_has_rank);
  if(cur_has_rank==rank){ // update haswell rank's sendrecv struct
    if(isr->total>0)
      isr->src=realloc(isr->src,(isr->total+1)*sizeof(int));
    isr->src[isr->total]=cur_knl_rank;
    isr->total+=1;    // update total receive on haswell's cur rank
  }
  if(cur_knl_rank==rank){
    *(isr->src)=cur_has_rank;
   // printf("updating knl's target:%d\n",cur_has_rank);
  }
 }
}
if(i_rank_knl>i_rank_has){ // haswell nodes are not enough to match 1 to 1 with knl, so each has rank handles multiple knl nodes' load
int rec_knl=i_rank_knl;
j=0; //pointer for haswell rank
for(i=0;i<rec_knl;i++,j++){
 int cur_knl_rank = rank_knl[i];
 if(j==i_rank_has) // 1 round end
   j=0;//reset to zero
 int cur_has_rank = rank_has[j];
 if(rank==cur_has_rank){ // update haswell's rank's sendrecv struct
   if(isr->total>0)
    isr->src=realloc(isr->src,(isr->total+1)*sizeof(int));
   isr->src[isr->total]=cur_knl_rank;
   isr->total+=1;
 }
 if(rank==cur_knl_rank){
   *(isr->src)=cur_has_rank;
 }
}
}
//Use previously calculated sendrecv info to realloc haswell bufefer
for(i=0;i<i_rank_has;i++){
 if(rank==i){
   if(isr->total>0)
     data_t=realloc(data_t,result_count[0]*result_count[1]*(1+isr->total)*sizeof(double));
 } 
}
MPI_Barrier(MPI_COMM_WORLD);
double t0 = MPI_Wtime();
//Communication
int size_block=result_count[0]*result_count[1];
//Simple Case, Send all to Haswell
//Two-sided communication: Specify source and destination, issue Send/recieve
MPI_Request sr_request;
MPI_Status sr_status;

for(i=0;i<isr->total;i++){
 printf("receiver:%d,1st sender:%d\n",rank,isr->src[0]);
 //nonblocking send receive
 MPI_Irecv(data_t+size_block*(i+1),size_block,MPI_DOUBLE,isr->src[i],123,MPI_COMM_WORLD,&sr_request);
}
if(isr->total==0){//sender side, i.e., knl partition
 printf("sender:%d,receiver:%d\n",rank,isr->src[0]);
 MPI_Isend(data_t,size_block,MPI_DOUBLE,*(isr->src),123,MPI_COMM_WORLD,&sr_request);
}

MPI_Wait(&sr_request,&status);
//One-sided communication: send round-robin to a datahub, receive round-robin from the datahub
 
MPI_Barrier(MPI_COMM_WORLD);
double t1 = MPI_Wtime();
//DO IO
 if(*color==1){//ask haswell to do io
   //Create new file and write result to this file
   plist_id3 = H5Pcreate(H5P_FILE_ACCESS);
   MPI_Info_create(&info);
   //MPI_Info_set(info, "cb_buffer_size", cb_buffer_size);
   //MPI_Info_set(info, "cb_nodes", cb_nodes);
   H5Pset_fapl_mpio(plist_id3, comm_kid, info);

   file_id2 = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, plist_id3);
   H5Pclose(plist_id3);
   dims2[0] = dims_x;//assume regular pattern, where size simply doubles along x dimension. 
   dims2[1] = dims_y;
   dataspace_id2 = H5Screate_simple(2, dims2, NULL);
   dset_id2 = H5Dcreate(file_id2,dataset, H5T_NATIVE_DOUBLE, dataspace_id2, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
   H5Sclose(dataspace_id2);
   result_space = H5Dget_space(dset_id2);
   result_count[0]*=isr->total+1;   
   H5Sselect_hyperslab(result_space, H5S_SELECT_SET, result_offset, NULL, result_count, NULL);

   result_memspace_size[0] = result_count[0];
   result_memspace_size[1] = result_count[1];
   result_memspace_id = H5Screate_simple(2, result_memspace_size, NULL);
   plist_id4 = H5Pcreate(H5P_DATASET_XFER);
   H5Pset_dxpl_mpio(plist_id4, H5FD_MPIO_COLLECTIVE);

   H5Dwrite(dset_id2, H5T_NATIVE_DOUBLE, result_memspace_id, result_space, plist_id4, data_t);
   H5Pclose(plist_id4);
   free(data_t);
   H5Sclose(result_space);
   H5Sclose(result_memspace_id);
   H5Dclose(dset_id2);
   H5Fclose(file_id2);
 }
 else if (*color==2){
   printf(" Rank: %10d, Rank_kid: %10d, Total_Kids: %10d, Total: %10d\n ",rank,rank_kid,nproc_kid,nproc);
 }
 MPI_Barrier(MPI_COMM_WORLD);
 double t2 = MPI_Wtime();
 if(rank==0)
 {
   printf("comm %.2fs,(%.2f); io %.2fs, (%.2f)\n",t1-t0,(t1-t0)/(t2-t0),t2-t1,(t2-t1)/(t2-t0));
   printf("data size: %.2fG at %s \n",file_size,filename); 
 }
 MPI_Finalize();
 return 0;
}
