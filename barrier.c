#include <mpi.h>
#include <stdio.h>
#include <unistd.h>  // For sleep function

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Initialize MPI

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the process
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the number of processes

    printf("Process %d is starting...\n", rank);
    
    // Simulate some work with sleep
    sleep(rank);  // Each process sleeps for its rank number of seconds

    printf("Process %d reached the barrier...\n", rank);
    
    // Synchronize all processes at this point
    MPI_Barrier(MPI_COMM_WORLD);

    printf("Process %d passed the barrier and is continuing...\n", rank);

    // Each process performs some more work after the barrier
    if (rank == 0) {
        printf("Process %d is doing some additional work...\n", rank);
    }

    MPI_Finalize();  // Finalize MPI
    return 0;
}
