#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Initialize MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get process rank
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the number of processes

    int root = 0;  // Process 0 is the root

    // MPI_Bcast: Broadcasting a value from root to all processes
    int broadcast_value;
    if (rank == root) {
        broadcast_value = 100;  // Root process sets the value
        printf("Process %d broadcasting value %d to all processes\n", rank, broadcast_value);
    }
    
    // Broadcast the value to all processes
    MPI_Bcast(&broadcast_value, 1, MPI_INT, root, MPI_COMM_WORLD);
    printf("Process %d received broadcasted value %d\n", rank, broadcast_value);

    // MPI_Gather: Gathering values from all processes to the root
    int local_value = rank + 1;  // Each process sets its local value
    int gathered_values[size];   // Array to hold gathered values at root

    // Gather the values from all processes to the root process
    MPI_Gather(&local_value, 1, MPI_INT, gathered_values, 1, MPI_INT, root, MPI_COMM_WORLD);

    if (rank == root) {
        printf("Process %d gathered values: ", rank);
        for (int i = 0; i < size; i++) {
            printf("%d ", gathered_values[i]);
        }
        printf("\n");
    }

    // MPI_Reduce: Sum all the local values and send the result to the root
    int reduction_result;
    
    // Perform the reduction (sum operation)
    MPI_Reduce(&local_value, &reduction_result, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);

    if (rank == root) {
        printf("Process %d received reduction result (sum of all values): %d\n", rank, reduction_result);
    }

    MPI_Finalize();  // Finalize MPI
    return 0;
}
