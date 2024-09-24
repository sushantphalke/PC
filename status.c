#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Initialize MPI

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank (ID) of the process

    if (rank == 0) {
        // Code for Process 0 (Sender)
        int data = 123;  // Data to send
        printf("Process 0: Sending data %d to Process 1\n", data);
        MPI_Send(&data, 1, MPI_INT, 1, 99, MPI_COMM_WORLD);  // Send to Process 1 with tag 99
    } else if (rank == 1) {
        // Code for Process 1 (Receiver)
        int received_data;
        MPI_Status status;  // Create an MPI_Status object to store message details

        MPI_Recv(&received_data, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);  // Receive from Process 0

        // After receiving, we can check details in the status object
        int source = status.MPI_SOURCE;   // Get the source of the message
        int tag = status.MPI_TAG;         // Get the message tag
        int count;
        MPI_Get_count(&status, MPI_INT, &count);  // Get the actual number of elements received

        printf("Process 1: Received data %d from Process %d with tag %d, count %d\n", 
               received_data, source, tag, count);
    }

    MPI_Finalize();  // Finalize MPI
    return 0;
}
