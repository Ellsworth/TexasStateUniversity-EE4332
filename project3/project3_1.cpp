#include <iostream>
#include <iomanip>
#include <chrono> // required for timing runtime.
#include <vector> // semi-required depending on gcc version. For using vectors.

using namespace std;

const unsigned int SIZE = 10 + 1;
const short int offset = 100 / (SIZE - 1);

void printMatrix(vector<float> matrix);
void setupMatrixBorder(vector<float>& matrix);

int main() {

    vector<float> plate(SIZE * SIZE);

    //plate[(4 * SIZE) + 0] = 1;

    cout << "Offset is " << offset << endl;

    setupMatrixBorder(plate);
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

        matrix[row * SIZE] = offset * row;
        matrix[(SIZE * (SIZE - 1)) + row] = 100 - (offset * row);


    }

    
}