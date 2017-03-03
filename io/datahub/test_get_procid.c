#include "hdf5.h"
#include "time.h"
#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "get_procid.h"

#define NAME_MAX 255

char filename[NAME_MAX];
char dataset[NAME_MAX];

int main(int argc, char ** argv)
{
 int rank,nproc,nid;
 MPI_Status status;
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 MPI_Comm_size(MPI_COMM_WORLD, &nproc);
 int haswell[2];
 int knl[2];
 haswell[0]=0;//min id on haswell
 haswell[1]=170;//max id on haswell
 knl[0]=171; //min id on knl
 knl[1]=5000;//max id on knl
 int color[1];
 MPI_Comm comm_kid=rank_split(MPI_COMM_WORLD, haswell, knl, color);
 int rank_kid,nproc_kid;
 MPI_Comm_rank(comm_kid,&rank_kid);
 MPI_Comm_size(comm_kid,&nproc_kid);
 printf(" Rank: %10d, Rank_kid: %10d, Total_Kids: %10d, Total: %10d\n ",rank,rank_kid,nproc_kid,nproc);
 
 hid_t   file_id2, dset_id2, plist_id3, plist_id4, dataspace_id2,result_space, result_memspace_id;
 hsize_t result_offset[2], result_count[2], result_memspace_size[2], my_z_size;
 hsize_t dims2[2], dims_x, dims_y, dims_z;
 double *data_t;
 int i,j,k;
 int c;
 MPI_Info info;
 //opterr = 0;
 strncpy(filename, "./test.h5", NAME_MAX);
 //strncpy(cb_buffer_size,"16777216", NAME_MAX);
 //strncpy(cb_nodes, "16", NAME_MAX);
 dims_x = 2000;
 dims_y = 1000;
 strncpy(dataset,"temperatures",NAME_MAX);
 int col=1;//collective write
 
 float file_size = (double)dims_x*(double)dims_y*(double)sizeof(double)/1024.0/1024.0/1024.0;
 
 
 if(*color==1){//ask haswell to do io
   //Create new file and write result to this file
   plist_id3 = H5Pcreate(H5P_FILE_ACCESS);
   MPI_Info_create(&info);
   //MPI_Info_set(info, "cb_buffer_size", cb_buffer_size);
   //MPI_Info_set(info, "cb_nodes", cb_nodes);
   H5Pset_fapl_mpio(plist_id3, comm_kid, info);

   file_id2 = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, plist_id3);
   H5Pclose(plist_id3);
   dims2[0] = dims_x;
   dims2[1] = dims_y;
   dataspace_id2 = H5Screate_simple(2, dims2, NULL);
   dset_id2 = H5Dcreate(file_id2,dataset, H5T_NATIVE_DOUBLE, dataspace_id2, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
   H5Sclose(dataspace_id2);
  
   result_offset[1] = 0;
   //int rank_kidi=MPI_Group_rank(comm_kid, &rank_kid);
   result_offset[0] =  (dims_x / nproc_kid) * rank_kid;
   result_count[0] = dims_x / nproc_kid;
   result_count[1] = dims_y;
   result_space = H5Dget_space(dset_id2);
   H5Sselect_hyperslab(result_space, H5S_SELECT_SET, result_offset, NULL, result_count, NULL);

   result_memspace_size[0] = result_count[0];
   result_memspace_size[1] = result_count[1];
   result_memspace_id = H5Screate_simple(2, result_memspace_size, NULL);
   data_t = (double *)malloc(result_count[0] * result_count[1] * sizeof(double));
   srand((unsigned int)time(NULL));
   double my_size = ((double)dims_x * (double)dims_y *sizeof(double)) / 1024.0 / 1024.0 / 1024.0;
   for (i = 0; i < result_count[0]; i++){
    for(j = 0; j < result_count[1]; j++){
        data_t[i*result_count[1]+j] = ((double)rand()/(double)(RAND_MAX)) * i+0.001;
    }
   }
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
 
 MPI_Finalize();
 return 0;
}
