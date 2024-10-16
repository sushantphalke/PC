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

#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 16

__global__ void gpu_matrix_mult(int *a, int *b, int *c, int m, int n, int k) {
    int row = blockIdx.y * blockDim.y + threadIdx.y; 
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;
    if (col < k && row < m) {
        for (int i = 0; i < n; i++) {
            sum += a[row * n + i] * b[i * k + col];
        }
        c[row * k + col] = sum;
    }
} 

int main() {
    int m, n, k;
    scanf("%d %d %d", &m, &n, &k);

    int *h_a, *h_b, *h_c;
    cudaMallocHost((void**)&h_a, sizeof(int) * m * n);
    cudaMallocHost((void**)&h_b, sizeof(int) * n * k);
    cudaMallocHost((void**)&h_c, sizeof(int) * m * k);

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            h_a[i * n + j] = 1;
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < k; ++j) {
            h_b[i * k + j] = 2;
        }
    }

    // float gpu_elapsed_time_ms;
    // cudaEvent_t start, stop;
    // cudaEventCreate(&start);
    // cudaEventCreate(&stop);

    int *d_a, *d_b, *d_c;
    cudaMalloc((void**)&d_a, sizeof(int) * m * n);
    cudaMalloc((void**)&d_b, sizeof(int) * n * k);
    cudaMalloc((void**)&d_c, sizeof(int) * m * k);

    cudaMemcpy(d_a, h_a, sizeof(int) * m * n, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, sizeof(int) * n * k, cudaMemcpyHostToDevice);

    unsigned int grid_rows = (m + BLOCK_SIZE - 1) / BLOCK_SIZE;
    unsigned int grid_cols = (k + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dim3 dimGrid(grid_cols, grid_rows);
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);

    // cudaEventRecord(start, 0);
    gpu_matrix_mult<<<dimGrid, dimBlock>>>(d_a, d_b, d_c, m, n, k);    
    cudaMemcpy(h_c, d_c, sizeof(int) * m * k, cudaMemcpyDeviceToHost);
    // cudaEventRecord(stop, 0);
    // cudaEventSynchronize(stop);
    // cudaEventElapsedTime(&gpu_elapsed_time_ms, start, stop);

    // printf("Time elapsed on matrix multiplication of %dx%d . %dx%d on GPU: %f ms.\n", m, n, n, k, gpu_elapsed_time_ms);

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < k; ++j) {
            printf("%d ", h_c[i * k + j]);
        }
        printf("\n");
    }

    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    cudaFreeHost(h_a);
    cudaFreeHost(h_b);
    cudaFreeHost(h_c);
    //cudaEventDestroy(start);
    //cudaEventDestroy(stop);

    return 0;
}




































// #include <stdio.h>
// #include <stdlib.h>
// #include <assert.h>

// #define BLOCK_SIZE 16
// __global__ void gpu_matrix_mult(int *a,int *b, int *c, int m, int n, int k)
// { 
//     int row = blockIdx.y * blockDim.y + threadIdx.y; 
//     int col = blockIdx.x * blockDim.x + threadIdx.x;
//     int sum = 0;
//     if( col < k && row < m) 
//     {
//         for(int i = 0; i < n; i++) 
//         {
//             sum += a[row * n + i] * b[i * k + col];
//         }
//         c[row * k + col] = sum;
//     }
// } 

// int main(){
//     int m, n, k;
//     /* Fixed seed for illustration */
//     // srand(3333);
//     printf("please type in m n and k\n");
//     scanf("%d %d %d", &m, &n, &k);

//     // allocate memory in host RAM, h_cc is used to store CPU result
//     int *h_a, *h_b, *h_c, *h_cc;
//     cudaMallocHost((void **) &h_a, sizeof(int)*m*n);
//     cudaMallocHost((void **) &h_b, sizeof(int)*n*k);
//     cudaMallocHost((void **) &h_c, sizeof(int)*m*k);
//     cudaMallocHost((void **) &h_cc, sizeof(int)*m*k);

//     // random initialize matrix A
//     for (int i = 0; i < m; ++i) {
//         for (int j = 0; j < n; ++j) {
//             // h_a[i * n + j] = rand() % 1024;
//             h_a[i * n + j] = 1;
//         }
//     }

//     // random initialize matrix B
//     for (int i = 0; i < n; ++i) {
//         for (int j = 0; j < k; ++j) {
//             // h_b[i * k + j] = rand() % 1024;
//             h_b[i * k + j] = 2;
//         }
//     }

//     float gpu_elapsed_time_ms;
//     // some events to count the execution time
//     cudaEvent_t start, stop;
//     cudaEventCreate(&start);
//     cudaEventCreate(&stop);

//     // start to count execution time of GPU version
//     cudaEventRecord(start, 0);
//     // Allocate memory space on the device 
//     int *d_a, *d_b, *d_c;
//     cudaMalloc((void **) &d_a, sizeof(int)*m*n);
//     cudaMalloc((void **) &d_b, sizeof(int)*n*k);
//     cudaMalloc((void **) &d_c, sizeof(int)*m*k);

//     // copy matrix A and B from host to device memory
//     cudaMemcpy(d_a, h_a, sizeof(int)*m*n, cudaMemcpyHostToDevice);
//     cudaMemcpy(d_b, h_b, sizeof(int)*n*k, cudaMemcpyHostToDevice);

//     unsigned int grid_rows = (m + BLOCK_SIZE - 1) / BLOCK_SIZE;
//     unsigned int grid_cols = (k + BLOCK_SIZE - 1) / BLOCK_SIZE;
//     dim3 dimGrid(grid_cols, grid_rows);
//     dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
   
//     // Launch kernel 
//         gpu_matrix_mult<<<dimGrid, dimBlock>>>(d_a, d_b, d_c, m, n, k);    
    
//     // Transefr results from device to host 
//     cudaMemcpy(h_c, d_c, sizeof(int)*m*k, cudaMemcpyDeviceToHost);
//     cudaThreadSynchronize();
//     // time counting terminate
//     cudaEventRecord(stop, 0);
//     cudaEventSynchronize(stop);


//     cudaEventElapsedTime(&gpu_elapsed_time_ms, start, stop);
//     printf("Time elapsed on matrix multiplication of %dx%d . %dx%d on GPU: %f ms.\n\n", m, n, n, k, gpu_elapsed_time_ms);



//     for (int i = 0; i < n; ++i) {
//         for (int j = 0; j < k; ++j) {
//              printf(" %d _ ",h_c[i * k + j] );
//         }
//         printf("\n");
//     }

//         printf( "time == %f \n", gpu_elapsed_time_ms);//"all results are correct!!!, speedup = %f\n", cpu_elapsed_time_ms /
    


//     // free memory
//     cudaFree(d_a);
//     cudaFree(d_b);
//     cudaFree(d_c);
//     cudaFreeHost(h_a);
//     cudaFreeHost(h_b);
//     cudaFreeHost(h_c);
//     cudaFreeHost(h_cc);
//     return 0;
// }
