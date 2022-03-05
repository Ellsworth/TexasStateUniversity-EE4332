#include <iostream>
#include <iomanip>
#include <chrono> // required for timing runtime.
#include <vector> // semi-required depending on gcc version. For using vectors.
#include <math.h>

using namespace std;

const unsigned int SIZE = 1000;
const float STEADY_STATE = 0.001;

const float OFFSET = 100 / ((float)SIZE - 1);
const int TOTAL_ELEMENTS = (SIZE - 2) * (SIZE - 2);


void printMatrix(vector<float> matrix);
void setupMatrixBorder(vector<float>& matrix);
void averageArray(vector<float>& matrix);
int countSteadyState(vector<float>& matrix1, vector<float>& matrix2);

int main() {

    

    vector<float> plate(SIZE * SIZE);
    vector<float> prev(SIZE * SIZE);

    //plate[(4 * SIZE) + 0] = 1;

    cout << "Offset is " << OFFSET << endl;
    cout << "Total elements of the plate is " << TOTAL_ELEMENTS << endl;
    cout << endl;

    setupMatrixBorder(plate);

    do {
        prev = plate;
        averageArray(plate);
    } while (countSteadyState(plate, prev) != TOTAL_ELEMENTS);

    
    printMatrix(plate);


}

void printMatrix(vector<float> matrix) {

    for (int row = 0; row < SIZE; row++) {

        for (int col = 0; col < SIZE; col++) {
            cout << matrix[(row * SIZE) + col] << " ";
        }

        cout << endl;

    }

}

void setupMatrixBorder(vector<float>& matrix) {

    for (int row = SIZE; row > 0; row--) {

        matrix[row * SIZE] = OFFSET * row;
        matrix[(SIZE * (SIZE - 1)) + row] = 100 - (OFFSET * row);
    }
    
}

void averageArray(vector<float>& matrix) {

    for (int row = 1; row < SIZE - 1; row++) {
        for (int col = 1; col < SIZE - 1; col++) {

            matrix[(row * SIZE) + col] = 0.25 * (matrix[((row + 1) * SIZE) + col] +
            matrix[((row - 1) * SIZE) + col] + matrix[(row * SIZE) + col + 1] +
            matrix[(row * SIZE) + col - 1]);
        }
    }
}

int countSteadyState(vector<float>& matrix1, vector<float>& matrix2) {

    unsigned int totalSteadyState = 0;

    for (int row = 1; row < SIZE - 1; row++) {
        for (int col = 1; col < SIZE - 1; col++) {
            if (abs(matrix1[(row * SIZE) + col] - matrix2[(row * SIZE) + col]) < STEADY_STATE)
                totalSteadyState++;
        }
    }

    //cout << "Steady state: " << totalSteadyState << endl;

    return totalSteadyState;

}