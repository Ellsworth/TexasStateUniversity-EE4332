/******************************************************************************
** Erich Scott Ellsworth, wqi7
** EE4332 – MP#1
** Filename: project1_2.cpp
** Due: 2/7/22
**
** Objective:
** Multiply two random matrices.
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

// The value of the random numbers in the matrices.
const double RNG_MIN = -1999.999, RNG_MAX = 1999.999;

/***********************************************************
 * mulMat: Multiplies two matrices
 *
 * mat1 & mat2: vector matrices to multiply.
 * result: vector matrix of the result. Passed by reference
 *
 * https://en.wikipedia.org/wiki/Matrix_multiplication
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
 * printResult: Prints a matrix.
 *
 * result: vector matrix of the matrix to be printed.
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

/***********************************************************
 * !!! WARNING !!!
 * 
 * DO NOT USE THIS FUNCTION FOR CRYPTOGRAPHY OR ANY USE REQUIRING VERIFIABLY
 * RANDOM NUMBERS. THE BUILT IN PSUEDO-RNG rand() IN C++ IS FLAWED AND KNOWN TO 
 * HAVE ERRATA.
 * 
 * For more info read: https://codeforces.com/blog/entry/61587
 * 
 * getRandomNumber: Returns a pseudorandom double.
 *
 * min - the smallest number in the range to generate
 * max - the largest number in the range to generate
/***********************************************************/

double getRandomNumber(double min, double max) {
	return min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (max - min)));
}

/***********************************************************
 * fillMatrixWithRandom - Fills a matrix with random numbers.
 * 
 * max - how many times to iterate thru the array. eg. 5: fill index 0-4.
 * rng_min - the smallest number in the range to generate
 * rng_max - the largest number in the range to generate
/***********************************************************/

void fillMatrixWithRandom(unsigned int max, double rng_min, double rng_max, vector<double>& matrix) {
	for (int i = 0; i < max; i++) {
		matrix[i] = getRandomNumber(rng_min, rng_max);
	}
}

int main() {
	// Start the total runtime clock.	
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	// Seed the rng.
	srand (time(NULL));

	// Pipe stdout to output file.
	freopen("project1_2.txt","w",stdout);

	// Create and pre-allocate vector for the result.
    vector<double> result(ROW * COL);
    vector<double> matrix_x(ROW * COL);
	vector<double> matrix_y(ROW * COL);

	// Fill the matrices with random numbers.
	fillMatrixWithRandom(ROW * COL, RNG_MIN, RNG_MAX, matrix_x);
	fillMatrixWithRandom(ROW * COL, RNG_MIN, RNG_MAX, matrix_y);

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
    //cout << "Time difference = " << chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

}