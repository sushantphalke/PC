#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Initialize MPI

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the process

    MPI_Request request;  // MPI request object for non-blocking communication
    MPI_Status status;    // Status object to check completion

    if (rank == 0) {
        // Process 0 sends data non-blocking
        int data = 100;  // Data to send
        printf("Process 0: Sending data %d to Process 1\n", data);

        MPI_Isend(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);  // Non-blocking send

        // Do other work here while sending happens in the background...

        MPI_Wait(&request, &status);  // Wait for the send to complete
        printf("Process 0: Data send completed\n");
    } else if (rank == 1) {
        // Process 1 receives data non-blocking
        int received_data;

        MPI_Irecv(&received_data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);  // Non-blocking receive

        // Do other work here while receiving happens in the background...

        MPI_Wait(&request, &status);  // Wait for the receive to complete
        printf("Process 1: Received data %d from Process 0\n", received_data);
    }

    MPI_Finalize();  // Finalize MPI
    return 0;
}
