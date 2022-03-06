#include <iostream>
#include <iomanip>
#include <chrono> // required for timing runtime.
#include <vector> // semi-required depending on gcc version. For using vectors.
#include <stdlib.h> // used for abs() function.
#include "mpi.h"


using namespace std;

// Global consts.

// Total size for each axis of the matrix. eg. 1000 x 1000.
const unsigned int SIZE = 5 + 2;

// Difference from the last run to consider an element in steady state.
const float STEADY_STATE = 0.001;

// The OFFSET of each value of the head applies at a given element.
const float OFFSET = 100 / ((float)SIZE - 1);

// Total elements of the plate.
const int TOTAL_ELEMENTS = (SIZE - 2) * (SIZE - 2);

const int MASTER_ID = 1;
const int WORKER_ID = 2; 



void printMatrix(vector<float> matrix);
void setupMatrixBorder(vector<float>& matrix);

void averageArray(vector<float>& matrix);
void averageRow(vector<float>& matrix, int row);

int countSteadyState(vector<float>& matrix1, vector<float>& matrix2);


int main(int argc, char *argv[]) {

	int rank, numProcess, rc, tag, offset, rows;
	double t1,t2;
  
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcess);

    cout << "Rank: " << rank << endl;

    // Define vectors
    vector<float> plate(SIZE * SIZE);
    vector<float> prev(SIZE * SIZE);

    if (rank == 0) {


        // Print the values of the consts defined at the begining.
        cout << "Offset is " << OFFSET << endl;
        cout << "Total elements of the plate is " << TOTAL_ELEMENTS << endl;
        cout << endl;

        setupMatrixBorder(plate);

        rows = SIZE / (numProcess - 1);
		offset = 0;
		tag = MASTER_ID;


        MPI_Send(&plate[(2 - 1) * SIZE], (3 * SIZE), MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
        //MPI_Recv(&plate[(2) * SIZE], (SIZE), MPI_FLOAT, 1, WORKER_ID, MPI_COMM_WORLD, &status);

        //cout << "master:" << endl;
        //printMatrix(plate);



    }
    if (rank > 0) {

        MPI_Recv(&plate[(2 - 1) * SIZE], (3 * SIZE), MPI_FLOAT, 0, MASTER_ID, MPI_COMM_WORLD, &status);
        averageRow(plate, 2);

        //MPI_Send(&plate[(2) * SIZE], (SIZE), MPI_FLOAT, 0, MASTER_ID, MPI_COMM_WORLD);
        printMatrix(plate);
        
    }

    MPI_Finalize();
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
 * averageArray - Perform the Jacobi iteration across the array
 * 
 * matrix - the matrix to perform the iteration
/***********************************************************/

void averageRow(vector<float>& matrix, int row) {

    for (int col = 1; col < SIZE - 1; col++) {
        matrix[(row * SIZE) + col] = 0.25 * (matrix[((row + 1) * SIZE) + col] +
        matrix[((row - 1) * SIZE) + col] + matrix[(row * SIZE) + col + 1] +
        matrix[(row * SIZE) + col - 1]);
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

            if ( abs(matrix1.at((row * SIZE) + col) - matrix1.at((row * SIZE) + col) < STEADY_STATE ) ) totalSteadyState++;
        }
    }

    return totalSteadyState;

}


