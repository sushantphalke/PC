#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Allocate memory for a chunk of the file
    int chunk_size = 100; // Example chunk size
    char *chunk = (char*)malloc(chunk_size * sizeof(char));

    // Each process reads a chunk of the file
    MPI_File file;
    MPI_File_open(MPI_COMM_WORLD, "input.txt", MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
    MPI_File_read_at(file, rank * chunk_size, chunk, chunk_size, MPI_CHAR, MPI_STATUS_IGNORE);
    MPI_File_close(&file);

    // Search for the pattern in the chunk
    char *pattern = "searchPattern";
    if (strstr(chunk, pattern)) {
        printf("Pattern found in process %d\n", rank);
    }

    free(chunk);
    MPI_Finalize();
    return 0;
}
