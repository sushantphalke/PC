#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Initialize MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the number of processes

    // Define a new group that includes processes 0 and 1
    MPI_Group world_group, new_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);  // Get the group from the world communicator

    int ranks[2] = {0, 1};  // Ranks to include in the new group
    MPI_Group_incl(world_group, 2, ranks, &new_group);  // Create a new group with ranks 0 and 1

    // Create a new communicator from the new group
    MPI_Comm new_comm;
    MPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);  // Create the communicator

    // Check if the new communicator is valid (only processes in the group will have a valid communicator)
    if (new_comm != MPI_COMM_NULL) {
        int value = rank + 1;  // Each process will send its rank + 1
        int gathered_values[2];  // Array to gather values in the new communicator

        // All processes in the new communicator will perform a gather operation
        MPI_Gather(&value, 1, MPI_INT, gathered_values, 1, MPI_INT, 0, new_comm);

        // Only process 0 in the new communicator will print the gathered values
        if (rank == 0) {
            printf("Process 0 gathered values: %d %d\n", gathered_values[0], gathered_values[1]);
        }
    }

    // Free the groups and communicators
    MPI_Group_free(&world_group);
    MPI_Group_free(&new_group);
    MPI_Comm_free(&new_comm);

    MPI_Finalize();  // Finalize MPI
    return 0;
}
