#include<mpi.h>
#include<stdio.h>
#include"get_procid.h"

MPI_Comm rank_split(MPI_Comm comm, int * haswell_id, int * knl_id,int * color)
{
  int rank;
  MPI_Comm_rank(comm, &rank);
  int nid;
  PMI_CNOS_Get_nid(rank,&nid);
  //int color=0;
  if (haswell_id[0]<= nid && nid <=haswell_id[1])
  {
     color[0] = 1;
     //printf("rank:%d,nid:%d,color:1,haswell:%d,%d\n",rank,nid,haswell_id[0],haswell_id[1]);	
  } 
  else if(knl_id[0]<= nid && nid <=knl_id[1])
  {
     color[0] = 2;
     //printf("rank:%d,nid:%d,color:2\n",rank,nid);
  }
  MPI_Comm comm_kids;
  MPI_Comm_split(comm, color[0], rank, &comm_kids);
  return comm_kids;
}


