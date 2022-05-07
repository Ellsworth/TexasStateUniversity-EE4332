// Includes.
#include <iostream>
#include <iomanip>
#include <chrono> // required for timing runtime.
#include <vector> // semi-required depending on gcc version. For using vectors.
#include <random>

using namespace std;

#define SIZE 5
#define RNG_MIN -199.999
#define RNG_MAX 199.999

void fillMatrixWithRandom(vector<float> &matrix);
void printMatrix(vector<float> matrix);
void multiplyMatrix(vector<float> matrixA, vector<float> matrixB, vector<float> &matrixResult);

int main() {

    vector<float> matrixA(SIZE * SIZE), matrixB(SIZE * SIZE), matrixResult(SIZE * SIZE);

    fillMatrixWithRandom(matrixA);
    fillMatrixWithRandom(matrixB);

    multiplyMatrix(matrixA, matrixB, matrixResult);
    
    printMatrix(matrixResult);

}

/***********************************************************
 * fillMatrixWithRandom - Fills a matrix with random numbers. Uses <random> for
 *      higher quality random numbers. Also is much faster than rand();
 * 
 * matrix - 2D matrix to fill.
/***********************************************************/

void fillMatrixWithRandom(vector<float> &matrix) {
    
    // Setup the RNG engine, using real-ish random numbers.
    random_device rd;

    // Default engine. Should be the hardware accelerated one, if it exists.
    default_random_engine eng(rd());

    // Uniform random numbers. RNG_MIN and RNG_MAX are defined.
    uniform_real_distribution<> distr(RNG_MIN, RNG_MAX);

    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            matrix.at((row * SIZE) + col) = distr(eng);
        }
    }


}

/***********************************************************
 * printMatrix: Prints a matrix.
 *
 * matrix: matrix to be printed.
/***********************************************************/

void printMatrix(vector<float> matrix) {

    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            cout << matrix.at((row * SIZE) + col) << " ";
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
void multiplyMatrix(vector<float> matrixA, vector<float> matrixB, vector<float> &matrixResult) {

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < SIZE; k++) {

                matrixResult.at((i * SIZE) + j) += matrixA.at((i * SIZE) + k) * matrixB.at((k * SIZE) + j);

            }
        }
    }

}