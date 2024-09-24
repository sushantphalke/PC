#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 1024
#define PATTERN "pattern we are looking for" // Change this to the pattern you want to search for

void find_pattern_in_chunk(char *chunk, int rank) {
    char *line = strtok(chunk, "\n");
    int line_number = 0;

    while (line != NULL) {
        // Check if the pattern is in the current line
        if (strstr(line, PATTERN) != NULL) {
            printf("Rank %d found pattern in line %d: %s\n", rank, line_number, line);
        }
        line = strtok(NULL, "\n");
        line_number++;
    }
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char *chunk = malloc(CHUNK_SIZE);
    if (chunk == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        FILE *file = fopen("input_large.txt", "r");
        if (file == NULL) {
            perror("Unable to open file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int chunk_count = 0;
        while (fgets(chunk, CHUNK_SIZE, file) != NULL) {
            // Send chunk to worker processes in a round-robin fashion
            int dest = chunk_count % (size - 1) + 1;
            MPI_Send(chunk, CHUNK_SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
            chunk_count++;
        }
        fclose(file);

        // Send termination messages
        for (int i = 1; i < size; i++) {
            char termination_message[CHUNK_SIZE] = "";
            MPI_Send(termination_message, CHUNK_SIZE, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Worker processes receive chunks
        while (1) {
            MPI_Recv(chunk, CHUNK_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
            if (strlen(chunk) == 0) {
                break; // Termination condition
            }
            find_pattern_in_chunk(chunk, rank);
        }
    }

    free(chunk);
    MPI_Finalize();
    return 0;
}
