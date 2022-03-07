#include <iostream>
#include <iomanip>
#include <chrono> // required for timing runtime.
#include <vector> // semi-required depending on gcc version. For using vectors.
#include <stdlib.h> // used for abs() function.
#include "mpi.h"


using namespace std;

// Global consts.

// Total size for each axis of the matrix. eg. 1000 x 1000.
const unsigned int SIZE = 7000;

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
void averageRow(vector<float>& matrix, int rowstart, int rowend);

int countSteadyState(vector<float>& matrix1, vector<float>& matrix2, int rowstart, int rowend);


int main(int argc, char *argv[]) {

	int rank, numProcess, rc, tag, offset, rows;
    int startrow, endrow;
	double t1,t2;
  
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcess);

    int steadystate, sum_steadystate;

    //cout << "Rank: " << rank << endl;

    // Define vectors
    vector<float> plate(SIZE * SIZE);
    vector<float> prev(SIZE * SIZE);

    // Calculate how many rows each process is responsible for. 
    const int rowsPerProcess = (SIZE - 2) / (numProcess - 1);


    /**
     * Master process
     */
    if (rank == 0) {

        // Setup the heat source on the matrix
        setupMatrixBorder(plate);

        // Print the values of the consts defined at the begining.
        cout << "Rows pers process: " << rowsPerProcess << endl;
        cout << endl;

        do {
            
            sum_steadystate = 0;

            prev = plate;

            for (int process = 1; process < numProcess; process++) {

                startrow = (process - 1) * rowsPerProcess + 1;
                endrow = (process) * rowsPerProcess;


                if (process == numProcess - 1) endrow = SIZE - 2;

                cout << "Process " << rank << ": Process " << process << " will avg rows " << startrow
                    << "-" << endrow << endl;

                MPI_Send(&startrow, 1, MPI_INT, process, MASTER_ID, MPI_COMM_WORLD);
                MPI_Send(&endrow, 1, MPI_INT, process, MASTER_ID, MPI_COMM_WORLD);

                MPI_Send(&prev[startrow * SIZE], (SIZE * (endrow - startrow)), MPI_FLOAT, process, MASTER_ID, MPI_COMM_WORLD);


                MPI_Send(&plate[(startrow - 1) * SIZE], (SIZE * (endrow - startrow + 2)), MPI_FLOAT, process, MASTER_ID, MPI_COMM_WORLD);
                MPI_Recv(&plate[(startrow) * SIZE], (SIZE * (endrow - startrow + 1)), MPI_FLOAT, process, WORKER_ID, MPI_COMM_WORLD, &status);

                MPI_Recv(&steadystate, 1, MPI_INT, process, WORKER_ID, MPI_COMM_WORLD, &status);
                sum_steadystate += steadystate;
                
                cout << "Current steady state: " << sum_steadystate << endl;

            }
        } while(sum_steadystate < TOTAL_ELEMENTS - 1);

        printMatrix(plate);
        cout << endl;

    }
    if (rank > 0) {

        MPI_Recv(&startrow, 1, MPI_INT, 0, MASTER_ID, MPI_COMM_WORLD, &status);
        MPI_Recv(&endrow, 1, MPI_INT, 0, MASTER_ID, MPI_COMM_WORLD, &status);

        MPI_Recv(&prev[startrow * SIZE], (SIZE * (endrow - startrow)), MPI_FLOAT, 0, MASTER_ID, MPI_COMM_WORLD, &status);

        MPI_Recv(&plate[(startrow - 1) * SIZE], (SIZE * (endrow - startrow + 2)), MPI_FLOAT, 0, MASTER_ID, MPI_COMM_WORLD, &status);
        
        averageRow(plate, startrow, endrow);
        
        MPI_Send(&plate[startrow * SIZE], (SIZE * (endrow - startrow + 1)), MPI_FLOAT, 0, WORKER_ID, MPI_COMM_WORLD);

        steadystate = countSteadyState(plate, prev, startrow, endrow);

        MPI_Send(&steadystate, 1, MPI_INT, 0, WORKER_ID, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    if (rank == 0) {
    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    cout << "Total Program Runtime: " << chrono::duration_cast<std::chrono::microseconds> (end - begin).count() << "[µs]" << std::endl;
    }
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
 * rowstart - the upper bounds of iteration
 * rowwend - the lower bounds of the iteration
/***********************************************************/

void averageRow(vector<float>& matrix, int rowstart, int rowend) {

    for (int row = rowstart; row < rowend + 1; row++) {
        for (int col = 1; col < SIZE - 1; col++) {

            matrix[(row  * SIZE) + col] = 0.25 * (matrix[((row + 1) * SIZE) + col] +
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

int countSteadyState(vector<float>& matrix1, vector<float>& matrix2, int rowstart, int rowend) {

    unsigned int totalSteadyState = 0;

    for (int row = rowstart; row < rowend + 1; row++) {
        for (int col = 1; col < SIZE - 1; col++) {

            if ( abs(matrix1.at((row * SIZE) + col) - matrix1.at((row * SIZE) + col) < STEADY_STATE ) ) totalSteadyState++;
        }
    }

    return totalSteadyState;

}


