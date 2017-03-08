#ifndef GET_PROCID_H 
#define GET_PROCID_H

//split the ranks based on partition: haswell or knl
MPI_Comm rank_split(MPI_Comm , int * , int * ,int*, int *);

#endif
