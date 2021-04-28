#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]){
  int me, nprocs;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &me); // process id
  
  printf("Task %d sends greatings from machine %d \n", nprocs, me);
  
  MPI_Finalize();
  
} 
