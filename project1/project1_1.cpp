/******************************************************************************
** Erich Scott Ellsworth, wqi7
** EE4332 – MP#1
** Filename: project1_1.cpp
** Due: 2/7/22
**
** Objective:
** Multiply two predefined matrices.
**
*******************************************************************************/

// Includes.
#include <iostream>
#include <iomanip>
#include <chrono> // required for timing runtime.
#include <vector> // semi-required depending on gcc version. For using vectors.

using namespace std;

// The size of the two matrices
const unsigned int ROW = 3, COL = 3;

/***********************************************************
** mulMat: Multiplies two matrices
**
** mat1 & mat2: vector matrices to multiply.
** result: vector matrix of the result. Passed by reference
**
** https://en.wikipedia.org/wiki/Matrix_multiplication
/***********************************************************/

void mulMat(vector<double> mat1, vector<double> mat2, vector<double>& result) {
    // Iterate row by row
    for (int i = 0; i < ROW; i++) {

        // For every row reset each value to zero. 
        for (int j = 0; j < COL; j++) {
            result[(3 * i) + j] = 0;

            // Do the multiplication operation.
            for (int k = 0; k < ROW; k++) {
                result[(3 * i) + j] += mat1[(3 * i) + k] * mat2[(3 * k) + j];
            }
            
        }

    }
}

/***********************************************************
** printResult: Prints a matrix.
**
** result: vector matrix of the matrix to be printed.
/***********************************************************/

void printResult(vector<double> result) {

    // Iterate for each row.
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {


            if (result[(3 * i) + j] > 0) cout << " ";
            cout << result[(3 * i) + j] << " ";
            
        }

        cout << endl;
    }
}

int main() {

    // Start the total runtime clock.    
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    // Pipe stdout to output file.
    freopen("project1_1.txt","w",stdout);

    // Create and pre-allocate vector for the result.
    vector<double> result(ROW * COL);

    // Matrices to multiply. Should _never_ be edited.
    vector<double> matrix_x = {
          2.33,   4.53, -98.40,
         28.54, -75.60,  23.44,
        102.33,   3.30, -45.01,
    };

    vector<double> matrix_y = {
         46.730,   43.60,  92.1,
         -2.233, -116.80, -11.4,
        156.800,   13.44,  67.8,
    };

    // Requirement 1. Print out the two matrices.
    printResult(matrix_x);
    cout << endl;
    printResult(matrix_y);
    cout << endl;

    // Start the multiplication clock.
    chrono::steady_clock::time_point matrix_begin = chrono::steady_clock::now();

    // Multiply the two matrices.
    mulMat(matrix_x, matrix_y, result);

    // Stop the multiplication clock.
    chrono::steady_clock::time_point matrix_end = chrono::steady_clock::now();

    printResult(result);
    cout << endl;

    // Stop the total runtime clock.
    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    // Print timing data.
    cout << "Total Program Runtime: " << chrono::duration_cast<std::chrono::microseconds> (end - begin).count() << "[µs]" << std::endl;    
    cout << "Matrix Multiplication: " << chrono::duration_cast<std::chrono::microseconds> (matrix_end - matrix_begin).count() << "[µs]" << std::endl;

}