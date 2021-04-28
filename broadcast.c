/*
 *   Send / Recv
 *
 *   José Monteiro, DEI / IST
 *
 *   Last modification: 2 November 2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char *argv[]) {

    MPI_Status status;
    int id, p,
	i, rounds;
    double* aux=malloc(1000*sizeof (double));
    for(int iter=0;i<1000;i++){
        aux[i]=iter;
    }
    

    MPI_Init (&argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);


    MPI_Barrier (MPI_COMM_WORLD);
    
    secs = - MPI_Wtime();
    if(!id){
    for(int p_aux=1;p_aux<p;p_aux++){
    MPI_Send(&aux, 1000, MPI_DOUBLE, p_aux, 123, MPI_COMM_WORLD);
    }
    MPI_Recv(&aux, 1000, MPI_DOUBLE, 0, 123, MPI_COMM_WORLD, &status);
    
    MPI_Barrier (MPI_COMM_WORLD);
    
    secs=MPI_Wtime() - secs;
    if(!id){
        printf("send: %lf ",secs)
    }
    
    secs=MPI_Wtime();
    
    if(!id){
        MPI_Bcast(&aux, 1000, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    
    MPI_Recv(&aux, 1000, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        
    MPI_Barrier (MPI_COMM_WORLD);
    
    secs=MPI_Wtime() - secs;
    if(!id){
        printf("send: %lf ",secs)
    }
    
   
   MPI_Barrier (MPI_COMM_WORLD);
   MPI_Finalize();
   return 0;
}
