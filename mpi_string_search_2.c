#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 1024
#define PATTERN "pattern we are looking for" // Change this to the pattern you want to search for

void find_pattern_in_chunk(char *chunk, int start_line, int rank) {
    char *line = strtok(chunk, "\n");
    int line_number = start_line;

    while (line != NULL) {
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

    double start_time = MPI_Wtime(); // Start timing

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

        int line_count = 0;
        while (fgets(chunk, CHUNK_SIZE, file) != NULL) {
            int dest = line_count % (size - 1) + 1; // Distribute to worker processes
            MPI_Send(&line_count, 1, MPI_INT, dest, 0, MPI_COMM_WORLD); // Send starting line number
            MPI_Send(chunk, CHUNK_SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
            line_count++;
        }
        fclose(file);

        // Send termination messages
        for (int i = 1; i < size; i++) {
            int termination_line = -1; // Indicate termination
            char termination_message[CHUNK_SIZE] = "";
            MPI_Send(&termination_line, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(termination_message, CHUNK_SIZE, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else {
        while (1) {
            int start_line;
            MPI_Recv(&start_line, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(chunk, CHUNK_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
            if (start_line == -1) {
                break; // Termination condition
            }
            find_pattern_in_chunk(chunk, start_line, rank);
        }
    }

    free(chunk);
    
    double end_time = MPI_Wtime(); // End timing
    double total_time = end_time - start_time;

    if (rank == 0) {
        printf("Total execution time: %f seconds\n", total_time);
    }

    MPI_Finalize();
    return 0;
}
