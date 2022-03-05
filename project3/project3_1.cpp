#include <iostream>
#include <iomanip>
#include <chrono> // required for timing runtime.
#include <vector> // semi-required depending on gcc version. For using vectors.
#include <math.h> // used for abs() function.

using namespace std;

// Global consts.

// Total size for each axis of the matrix. eg. 1000 x 1000.
const unsigned int SIZE = 1000;

// Difference from the last run to consider an element in steady state.
const float STEADY_STATE = 0.001;

// The OFFSET of each value of the head applies at a given element.
const float OFFSET = 100 / ((float)SIZE - 1);

// Total elements of the plate.
const int TOTAL_ELEMENTS = (SIZE - 2) * (SIZE - 2);


void printMatrix(vector<float> matrix);
void setupMatrixBorder(vector<float>& matrix);
void averageArray(vector<float>& matrix);
int countSteadyState(vector<float>& matrix1, vector<float>& matrix2);

int main() {

    // Define vectors
    vector<float> plate(SIZE * SIZE);
    vector<float> prev(SIZE * SIZE);

    // Print the values of the consts defined at the begining.
    cout << "Offset is " << OFFSET << endl;
    cout << "Total elements of the plate is " << TOTAL_ELEMENTS << endl;
    cout << endl;

    // Plate is filled with zeros, need to setup heat values.
    setupMatrixBorder(plate);

    // While the number of steady state elements does not equal the total elements.
    do {
        // Set the current plate values to the priv, so we can later compare.
        prev = plate;

        // Do the Jacobi iteration.
        averageArray(plate);
    } while (countSteadyState(plate, prev) != TOTAL_ELEMENTS);

    
    printMatrix(plate);


}

/***********************************************************
 * printMatrix - Prints a vector matrix.
 * 
 * matrix - the matrix to print
/***********************************************************/

void printMatrix(vector<float> matrix) {

    for (int row = 0; row < SIZE; row++) {

        for (int col = 0; col < SIZE; col++) {
            cout << matrix[(row * SIZE) + col] << " ";
        }

        cout << endl;

    }

}

/***********************************************************
 * setupMatrixBorder - Sets up the heat values at the border off the matrix
 * 
 * matrix - the matrix to setup the border
/***********************************************************/

void setupMatrixBorder(vector<float>& matrix) {

    for (int row = SIZE; row > 0; row--) {

        matrix[row * SIZE] = OFFSET * row;
        matrix[(SIZE * (SIZE - 1)) + row] = 100 - (OFFSET * row);
    }
    
}

/***********************************************************
 * averageArray - Perform the Jacobi iteration across the array
 * 
 * matrix - the matrix to perform the iteration
/***********************************************************/

void averageArray(vector<float>& matrix) {

    for (int row = 1; row < SIZE - 1; row++) {
        for (int col = 1; col < SIZE - 1; col++) {

            matrix[(row * SIZE) + col] = 0.25 * (matrix[((row + 1) * SIZE) + col] +
            matrix[((row - 1) * SIZE) + col] + matrix[(row * SIZE) + col + 1] +
            matrix[(row * SIZE) + col - 1]);
        }
    }
}

/***********************************************************
 * countSteadyState - Count the number of elements in steady state.
 * 
 * matrix1 - The first matrix to compare.
 * matrix2 - The second matrix to compare.
 * 
 * returns: The total number of elements in steady state.
/***********************************************************/

int countSteadyState(vector<float>& matrix1, vector<float>& matrix2) {

    unsigned int totalSteadyState = 0;

    for (int row = 1; row < SIZE - 1; row++) {
        for (int col = 1; col < SIZE - 1; col++) {
            if (abs(matrix1[(row * SIZE) + col] - matrix2[(row * SIZE) + col]) < STEADY_STATE)
                totalSteadyState++;
        }
    }

    return totalSteadyState;

}