// Includes.
#include <iostream>
#include <iomanip>
#include <chrono> // required for timing runtime.
#include <vector> // semi-required depending on gcc version. For using vectors.
#include <random>

using namespace std;

#define SIZE 5
#define RNG_MIN -1999.999
#define RNG_MAX 1999.999

void fillMatrixWithRandom(float matrix[SIZE][SIZE]);
void printMatrix(float matrix[SIZE][SIZE]);
void multiplyMatrix(float matrixA[SIZE][SIZE], float matrixB[SIZE][SIZE], float matrixResult[SIZE][SIZE]);

int main() {

    // Setup the matrix.
    float matrixA[SIZE][SIZE], matrixB[SIZE][SIZE], matrixResult[SIZE][SIZE];

    // Fill the two matricies.
    fillMatrixWithRandom(matrixA);
    fillMatrixWithRandom(matrixB);

    // Do the multiplication.
    multiplyMatrix(matrixA, matrixB, matrixResult);

    // Print our result.
    printMatrix(matrixResult);

}

/***********************************************************
 * fillMatrixWithRandom - Fills a matrix with random numbers. Uses <random> for
 *      higher quality random numbers. Also is much faster than rand();
 * 
 * matrix - 2D matrix to fill.
/***********************************************************/

void fillMatrixWithRandom(float matrix[SIZE][SIZE]) {
    
    // Setup the RNG engine, using real-ish random numbers.
    random_device rd;

    // Default engine. Should be the hardware accelerated one, if it exists.
    default_random_engine eng(rd());

    // Uniform random numbers. RNG_MIN and RNG_MAX are defined.
    uniform_real_distribution<> distr(RNG_MIN, RNG_MAX);

    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            matrix[row][col] = distr(eng);
        }
    }
}

/***********************************************************
 * printMatrix: Prints a matrix.
 *
 * matrix: matrix to be printed.
/***********************************************************/

void printMatrix(float matrix[SIZE][SIZE]) {

    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            cout << matrix[row][col] << " ";
        }
        cout << endl;
    }

}

/***********************************************************
 * multiplyMatrix: Multiply two matricies.
 *
 * matrixA & matrixB: 2D matrix to multiply.
 * matrixResult - The result.
/***********************************************************/
void multiplyMatrix(float matrixA[SIZE][SIZE], float matrixB[SIZE][SIZE], float matrixResult[SIZE][SIZE]) {

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < SIZE; k++) {

                matrixResult[i][j] += matrixA[i][k] * matrixB[k][j];

            }
        }
    }

}