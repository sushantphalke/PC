#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Initialize MPI

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the process
    MPI_Status status;
    MPI_Request request;

    if (rank == 0) {
        // Process 0 sends data using MPI_Bsend
        int buffer_size = MPI_BSEND_OVERHEAD + sizeof(int);  // Buffer size
        void* buffer = malloc(buffer_size);
        MPI_Buffer_attach(buffer, buffer_size);  // Attach the buffer

        int data = 10;  // Data to send
        printf("Process 0: Sending data %d using MPI_Bsend\n", data);
        MPI_Bsend(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);  // Buffered send to Process 1

        MPI_Buffer_detach(&buffer, &buffer_size);  // Detach the buffer
        free(buffer);  // Free the buffer
    } else if (rank == 1) {
        // Process 1 sends data using MPI_Ssend (Synchronous send)
        int data = 20;  // Data to send
        printf("Process 1: Sending data %d using MPI_Ssend\n", data);
        MPI_Ssend(&data, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);  // Synchronous send to Process 2
    } else if (rank == 2) {
        // Process 2 sends data using MPI_Isend (Non-blocking send)
        int data = 30;  // Data to send
        printf("Process 2: Sending data %d using MPI_Isend\n", data);
        MPI_Isend(&data, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, &request);  // Non-blocking send to Process 3

        // Do some work here while sending happens in the background
        MPI_Wait(&request, &status);  // Wait for non-blocking send to complete
    } else if (rank == 3) {
        // Process 3 receives messages from Process 0, 1, and 2
        int received_data;
        
        // Receiving data from Process 0 (Bsend)
        MPI_Recv(&received_data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        printf("Process 3: Received data %d from Process 0 (Bsend)\n", received_data);

        // Receiving data from Process 1 (Ssend)
        MPI_Recv(&received_data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        printf("Process 3: Received data %d from Process 1 (Ssend)\n", received_data);

        // Receiving data from Process 2 (Isend)
        MPI_Recv(&received_data, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
        printf("Process 3: Received data %d from Process 2 (Isend)\n", received_data);
    }

    MPI_Finalize();  // Finalize MPI
    return 0;
}
