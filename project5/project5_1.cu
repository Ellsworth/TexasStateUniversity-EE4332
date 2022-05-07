/******************************************************************************
** Erich Scott Ellsworth, wqi7
** EE4332 – MP#5
** Filename: project1_3.cpp
** Due: 4/18/22
**
** Objective:
** Multiply two random matrices using CUDA
**
*******************************************************************************/

// Includes. Need the CUDA framework to be installed.
#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>
#include <time.h>

// Size of the tiles on GPU. DO NOT TOUCH. MICROARCH DEPENDANT.
#define TILE_SIZE 32

#define ARRAY_SIZE 2000        // Size of the array in each direction. row x col
#define RNG_RANGE_MIN -199.99  // Min value for the RNG.
#define RNG_RANGE_MAX 199.99   // Max value for the RNG.

/***********************************************************
 * matrixMultiply: Multiplies two matrices using CUDA.
 *
 * A & B : matrices to multiply.
 * C : matrix of the result. Passed by reference
 *
 * https://en.wikipedia.org/wiki/Matrix_multiplication
/***********************************************************/
__global__ void matrixMultiply(float * A, float * B, float * C)  {

    //tile size arrays - to the GPU's shared memory region
    __shared__ float sA[TILE_SIZE][TILE_SIZE];
    __shared__ float sB[TILE_SIZE][TILE_SIZE];

    // ids of thread	
    int Row = blockDim.y*blockIdx.y + threadIdx.y;
    int Col = blockDim.x*blockIdx.x + threadIdx.x;
    float Cvalue = 0.0;

    // init to zero 
    sA[threadIdx.y][threadIdx.x] = 0.0;
    sB[threadIdx.y][threadIdx.x] = 0.0;

    for (int k = 0; k < (((ARRAY_SIZE - 1)/ TILE_SIZE) + 1); k++)  {
        
        // copy data to the tile from matrix A
        if ( (Row < ARRAY_SIZE) && (threadIdx.x + (k*TILE_SIZE)) < ARRAY_SIZE) {
        sA[threadIdx.y][threadIdx.x] = A[(Row*ARRAY_SIZE) + threadIdx.x + (k*TILE_SIZE)];
        }
        
        else  {
        sA[threadIdx.y][threadIdx.x] = 0.0; // if outside the boundaries of the matrix
        }
        
        // Move the data from the tile to matrix B.
        if ( Col < ARRAY_SIZE && (threadIdx.y + k*TILE_SIZE) < ARRAY_SIZE) {
            sB[threadIdx.y][threadIdx.x] = B[(threadIdx.y + k*TILE_SIZE)*ARRAY_SIZE + Col];
        }
        else {
            sB[threadIdx.y][threadIdx.x] = 0.0;
        }
       
        // making sure all threads have arrived before multiplicatoins
        __syncthreads();

        // multiplying elements present in the current tile
        for (int j = 0; j < TILE_SIZE; ++j) {
            Cvalue += sA[threadIdx.y][j] * sB[j][threadIdx.x];
        }
    }
    
    // Saving final result to Matrix C
    if (Row < ARRAY_SIZE && Col < ARRAY_SIZE) {
           C[Row * ARRAY_SIZE + Col] = Cvalue;
    }
}

/***********************************************************
 * printResult: Prints a matrix.
 *
 * matrix: The matrix to be printed.
/***********************************************************/

void printMatrix(float * matrix) {

    for (int row = 0; row < ARRAY_SIZE; row++) {

        for (int col = 0; col < ARRAY_SIZE; col++) {

            printf("%f  ",*(matrix+(row * ARRAY_SIZE) + col));

        }
        printf("\n");
    }

}

/***********************************************************
 * !!! WARNING !!!
 * 
 * DO NOT USE THIS FUNCTION FOR CRYPTOGRAPHY OR ANY USE REQUIRING VERIFIABLY
 * RANDOM NUMBERS. THE BUILT IN PSUEDO-RNG rand() IN C IS FLAWED AND KNOWN TO 
 * HAVE ERRATA.
 * 
 * For more info read: https://codeforces.com/blog/entry/61587
 * 
 * getRandomNumber: Returns a pseudorandom float.
 *
 * min - the smallest number in the range to generate
 * max - the largest number in the range to generate
/***********************************************************/

float getRandomNumber(float min, float max) {
    return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

/***********************************************************
 * fillMatrixWithRandom - Fills a matrix with random numbers.
 * 
 * max - how many times to iterate thru the array. eg. 5: fill index 0-4.
 * rng_min - the smallest number in the range to generate
 * rng_max - the largest number in the range to generate
/***********************************************************/

void fillMatrixWithRandom(float rng_min, float rng_max, float * matrix) {
    
    for (int i = 0; i < ARRAY_SIZE * ARRAY_SIZE; i++) {
        matrix[i] = getRandomNumber(rng_min, rng_max);
    }
    
}


int main(int argc, char ** argv) {

    clock_t start = clock();

    float * hostA; // The A matrix
    float * hostB; // The B matrix
    float * hostC; // The output C matrix
    float * deviceA;
    float * deviceB;
    float * deviceC;


    hostA = (float *) malloc(sizeof(float) * ARRAY_SIZE * ARRAY_SIZE);
    hostB = (float *) malloc(sizeof(float) * ARRAY_SIZE * ARRAY_SIZE);
    hostC = (float *) malloc(sizeof(float) * ARRAY_SIZE * ARRAY_SIZE);

    fillMatrixWithRandom(RNG_RANGE_MIN, RNG_RANGE_MAX, hostA);
    fillMatrixWithRandom(RNG_RANGE_MIN, RNG_RANGE_MAX, hostB);
    
    // Allocating GPU memory
    cudaMalloc((void **)&deviceA, sizeof(float) *ARRAY_SIZE * ARRAY_SIZE);
    cudaMalloc((void **)&deviceB, sizeof(float) *ARRAY_SIZE * ARRAY_SIZE);
    cudaMalloc((void **)&deviceC, sizeof(float) *ARRAY_SIZE * ARRAY_SIZE);

    // Copy memory to the GPU
    cudaMemcpy(deviceA, hostA, sizeof(float) * ARRAY_SIZE * ARRAY_SIZE, cudaMemcpyHostToDevice);
    cudaMemcpy(deviceB, hostB, sizeof(float) * ARRAY_SIZE * ARRAY_SIZE, cudaMemcpyHostToDevice);

    // Initialize the grid and block dimensions
    dim3 dimGrid((ARRAY_SIZE/TILE_SIZE) + 1, (ARRAY_SIZE/TILE_SIZE) + 1, 1);	//Number of Blocks required
    dim3 dimBlock(TILE_SIZE, TILE_SIZE, 1); //Number of threads in each block

    matrixMultiply<<<dimGrid, dimBlock>>>(deviceA, deviceB, deviceC);

    // Another way to sync theads from the GPU
    cudaDeviceSynchronize();

    // Copy the results in GPU memory back to the CPU
    cudaMemcpy(hostC, deviceC, sizeof(float) * ARRAY_SIZE * ARRAY_SIZE, cudaMemcpyDeviceToHost);

    // Print the resulting matrix.
    printMatrix(hostC);

    // Free the GPU memory
    cudaFree(deviceA);
    cudaFree(deviceB);
    cudaFree(deviceC);
    
    // Free the Pointer Memory
    free(hostA);
    free(hostB);
    free(hostC);

    // Stop the total runtime clock.
    clock_t stop = clock();
    
    // Print timing data.
    printf("Elapsed: %f seconds\n", (double)(stop - start) / CLOCKS_PER_SEC);

    return 0;
}