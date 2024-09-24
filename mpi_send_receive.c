#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Initialize the MPI environment

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank (ID) of the process

    if (rank == 0) {
        // Code for Process 0 (Sender)
        int data = 100;  // Data to send
        printf("Process 0: Sending data %d to Process 1\n", data);
        MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);  // Send data to Process 1
    } else if (rank == 1) {
        // Code for Process 1 (Receiver)
        int received_data;
        MPI_Recv(&received_data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  // Receive data from Process 0
        printf("Process 1: Received data %d from Process 0\n", received_data);
    }

    MPI_Finalize();  // Finalize the MPI environment
    return 0;
}
