#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Initialize MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the number of processes

    const char* filename = "mpi_io_example.dat";
    MPI_File fh;  // File handle

    // Step 1: Writing data to the file
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);

    int data = rank + 1;  // Each process will write its rank + 1
    MPI_File_write(fh, &data, 1, MPI_INT, MPI_STATUS_IGNORE);  // Write data to the file

    MPI_File_close(&fh);  // Close the file after writing

    // Synchronize all processes before reading
    MPI_Barrier(MPI_COMM_WORLD);

    // Step 2: Reading data from the file
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    int read_data;
    MPI_File_read(fh, &read_data, 1, MPI_INT, MPI_STATUS_IGNORE);  // Read data from the file
    printf("Process %d read value %d from file\n", rank, read_data);  // Print the read value

    MPI_File_close(&fh);  // Close the file after reading

    MPI_Finalize();  // Finalize MPI
    return 0;
}
