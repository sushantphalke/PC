#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 16

__global__ void conv(int *input, int *output, int *filter, int input_rows, int input_cols, int filter_size, int padding) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < input_rows && col < input_cols) {
        int sum = 0;
        for (int i = 0; i < filter_size; i++) {
            for (int j = 0; j < filter_size; j++) {
                int input_row = row + i - padding;
                int input_col = col + j - padding;
                if (input_row >= 0 && input_row < input_rows && input_col >= 0 && input_col < input_cols) {
                    sum += input[input_row * input_cols + input_col] * filter[i * filter_size + j];
                }
            }
        }
        output[row * input_cols + col] = sum;
    }
}

int main() {
    int input_rows = 16;
    int input_cols = 8;
    int filter_size = 3;
    int padding = 2; // Use 1 for no padding and 2 for padding of 2

    int *h_input, *h_filter, *h_output;
    //int output_rows = input_rows + 2 * padding; // Output size with padding
    //int output_cols = input_cols + 2 * padding;

    // Allocate host memory
    cudaMallocHost((void**)&h_input, sizeof(int) * input_rows * input_cols);
    cudaMallocHost((void**)&h_filter, sizeof(int) * filter_size * filter_size);
    cudaMallocHost((void**)&h_output, sizeof(int) * input_rows * input_cols); // Output same size as input

    // Initialize input matrix and filter
    for (int i = 0; i < input_rows; i++)
        for (int j = 0; j < input_cols; j++)
            h_input[i * input_cols + j] = 1; // Input matrix

    for (int i = 0; i < filter_size; i++)
        for (int j = 0; j < filter_size; j++)
            h_filter[i * filter_size + j] = 1; // Filter matrix

    // Device memory allocation
    int *d_input, *d_filter, *d_output;
    cudaMalloc((void**)&d_input, sizeof(int) * input_rows * input_cols);
    cudaMalloc((void**)&d_filter, sizeof(int) * filter_size * filter_size);
    cudaMalloc((void**)&d_output, sizeof(int) * input_rows * input_cols);

    // Copy input and filter to device memory
    cudaMemcpy(d_input, h_input, sizeof(int) * input_rows * input_cols, cudaMemcpyHostToDevice);
    cudaMemcpy(d_filter, h_filter, sizeof(int) * filter_size * filter_size, cudaMemcpyHostToDevice);

    // Define grid and block dimensions
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid((input_cols + BLOCK_SIZE - 1) / BLOCK_SIZE, (input_rows + BLOCK_SIZE - 1) / BLOCK_SIZE);

    // Launch convolution kernel
    conv<<<dimGrid, dimBlock>>>(d_input, d_output, d_filter, input_rows, input_cols, filter_size, padding);

    // Copy result back to host
    cudaMemcpy(h_output, d_output, sizeof(int) * input_rows * input_cols, cudaMemcpyDeviceToHost);

    // Print output
    printf("Convolution Result:\n");
    for (int i = 0; i < input_rows; i++) {
        for (int j = 0; j < input_cols; j++) {
            printf("%d ", h_output[i * input_cols + j]);
        }
        printf("\n");
    }

    // Free device and host memory
    cudaFree(d_input);
    cudaFree(d_filter);
    cudaFree(d_output);
    cudaFreeHost(h_input);
    cudaFreeHost(h_filter);
    cudaFreeHost(h_output);

    return 0;
}
