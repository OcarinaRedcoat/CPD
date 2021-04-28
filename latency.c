#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char *argv[]) {

    MPI_Status status;
    int id, p,
	i, rounds;
    double secs;

    MPI_Init (&argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    MPI_Barrier (MPI_COMM_WORLD);
    secs = - MPI_Wtime();
    MPI_Send(&secs, 1, MPI_DOUBLE, 1, 123, MPI_COMM_WORLD);
    
    MPI_Recv(&secs, 1, MPI_DOUBLE, 0, 123, MPI_COMM_WORLD, &status);
    
    printf("send latency: %d", MPI_Wtime() - secs);
   
   MPI_Barrier (MPI_COMM_WORLD);
   MPI_Finalize();
   return 0;
