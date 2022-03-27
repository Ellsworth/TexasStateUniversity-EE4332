/******************************************************************************
** Erich Scott Ellsworth, wqi7
** EE4332 – MP4
** Filename: project4.cpp
** Due: 3/28/22
**
** Objective:
** Perform the jacobi iteration on an array using OpenMPI.
**
*******************************************************************************/

#include <iostream>
#include <iomanip>
#include <chrono> // required for timing runtime.
#include <vector> // semi-required depending on gcc version. For using vectors.
#include <stdlib.h> // used for abs() function.
#include <fstream> // Required for outputting to a text file.

#include <omp.h>
#include "mpi.h"

using namespace std;

// Global consts.

// Total size for each axis of the matrix. eg. 7000 x 7000.
const unsigned int SIZE = 7000;

// Difference from the last run to consider an element in steady state.
const float STEADY_STATE = 0.001;

// Maximum runs to do. if we reach this number, finish.
const float MAX_ITERATIONS = 2500;

// The OFFSET of each value of the head applies at a given element.
const float OFFSET = 100 / ((float)SIZE - 1);

// Total elements of the plate.
const int TOTAL_ELEMENTS = (SIZE - 2) * (SIZE - 2);

const int MASTER_ID = 1;
const int WORKER_ID = 2;

void printMatrix(vector<float> matrix);
void setupMatrixBorder(vector<float>& matrix);

void averageRow(vector<float>& matrix, int rowstart, int rowend);

int countSteadyState(vector<float>& matrix1, vector<float>& matrix2, int rowstart, int rowend);


int main(int argc, char *argv[]) {

    // Pipe stdout to output file.
    //freopen("project3_2.txt","w", stdout);


	int rank, numProcess, rc, tag, offset, rows;
    int startrow, endrow, iterations = 0;
    int steadystate = 0, sum_steadystate = 0, isSteadyState = 0;

    // Start the clock.
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    // Init OpenMPI
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcess);

    // Define vectors
    vector<float> plate(SIZE * SIZE);
    vector<float> prev(SIZE * SIZE);

    // Calculate how many rows each process is responsible for. 
    const int rowsPerProcess = SIZE / numProcess;

    // Find the start and end rows;    
    startrow = (rank * rowsPerProcess);
    endrow = (startrow + rowsPerProcess);
    if (rank == numProcess - 1) endrow = SIZE - 1;

    cout << rank << ": Rows " << startrow << "-" << endrow << endl;

    /**
     * Master process
     */
    if (rank == 0) {

            // setup the plate.
            setupMatrixBorder(plate);

            do {
            
            // for ever loop, iterate this by one.
            iterations++;

            // Reset the steady state counter.
            sum_steadystate = 0;

            // Send the plate.
            MPI_Send(&plate[((SIZE * SIZE) / 2) - SIZE], ((SIZE * rowsPerProcess) + SIZE), MPI_FLOAT, 1, MASTER_ID, MPI_COMM_WORLD);

            // Set the previous plate to the current.
            prev = plate;

            // Perform the calculation.
            averageRow(plate, 1, (SIZE / 2));

            // RX the worker's data.
            MPI_Recv(&plate[((SIZE * SIZE) / 2)], ((SIZE * rowsPerProcess)), MPI_FLOAT, 1, WORKER_ID, MPI_COMM_WORLD, &status);
            
            // RX the # of steady state units in the worker's half.
            MPI_Recv(&steadystate, 1, MPI_INT, 1, WORKER_ID, MPI_COMM_WORLD, &status);

            // Add the worker and our steady state count to sum_steadystate.
            sum_steadystate += steadystate;

            sum_steadystate += countSteadyState(plate, prev, 1, (SIZE / 2) - 1);

            // If we are in steady state, or past the max iterations, set the
            //      isSteadyState flag to 1.
            if (sum_steadystate > TOTAL_ELEMENTS - 1) isSteadyState = 1;
            if (iterations > MAX_ITERATIONS - 1) isSteadyState = 1;

            // Let the worker know to keep going or not.
            MPI_Send(&isSteadyState, 1, MPI_INT, 1, MASTER_ID, MPI_COMM_WORLD);

        } while (!isSteadyState);

    }

    // Worker code.
    if (rank > 0) {

        while (!isSteadyState) {

        // reset the counter
        sum_steadystate = 0;

        // RX the plate from master.
        MPI_Recv(&plate[((SIZE * SIZE) / 2) - SIZE], ((SIZE * rowsPerProcess) + SIZE), MPI_FLOAT, 0, MASTER_ID, MPI_COMM_WORLD, &status);

        prev = plate;

        // Perform the calculation.
        averageRow(plate, SIZE / 2, SIZE - 2);

        // Count the number of our elements that are in steady state.
        steadystate = countSteadyState(plate, prev, SIZE / 2, SIZE - 2);

        // Send back the computed plate.
        MPI_Send(&plate[((SIZE * SIZE) / 2)], ((SIZE * rowsPerProcess)), MPI_FLOAT, 0, WORKER_ID, MPI_COMM_WORLD);

        // Send the total elements in state state.
        MPI_Send(&steadystate, 1, MPI_INT, 0, WORKER_ID, MPI_COMM_WORLD);

        // RX if we need to keep going or not.
        MPI_Recv(&isSteadyState, 1, MPI_INT, 0, MASTER_ID, MPI_COMM_WORLD, &status);

        }

    }

    // Cleanly terminate all of the workers and the message passing interface.
    MPI_Finalize();

    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    if (rank == 0) {

    printMatrix(plate);

    // Stop the clock and print our time.
    
    cout << "Total iterations: " << iterations << endl;

    cout << "Total Program Runtime: " << chrono::duration_cast<std::chrono::microseconds> (end - begin).count() << "[µs]" << std::endl;
    }
}

/***********************************************************
 * printMatrix - Prints a vector matrix.
 * 
 * matrix - the matrix to print
 * iterations - the total number of iterations
/***********************************************************/

void printMatrix(vector<float> matrix) {

    ofstream fout;
    fout.open("project4.txt");

    // Go thru each element and write to fout.
    for (int row = 0; row < SIZE; row++) {

        for (int col = 0; col < SIZE; col++) {
            fout << matrix[(row * SIZE) + col] << " ";
        }

        fout << endl;

    }

    // Close the file when we are done. Not 100% required, but good form.

    fout.close();

}

/***********************************************************
 * setupMatrixBorder - Sets up the heat values at the border off the matrix
 * 
 * matrix - the matrix to setup the border
/***********************************************************/

void setupMatrixBorder(vector<float>& matrix) {

    fill(matrix.begin(), matrix.end(), 0.5F);

    int i, row;

    #pragma omp parallel for private(i, row) shared(matrix)

    // set the top and right sides to zero.
    for (i = 0; i < SIZE; i++) {
        matrix.at(i) = 0;
        matrix.at(SIZE - 1 + (SIZE * i)) = 0;
    }

    // Setup the heat source.
    for (row = SIZE; row > 0; row--) {

        matrix[row * SIZE] = OFFSET * row;
        matrix[(SIZE * (SIZE - 1)) + row] = 100 - (OFFSET * row);
    }

    #pragma omp barrier // prevent race condition.

    return;
}

/***********************************************************
 * averageArray - Perform the Jacobi iteration across the array
 * 
 * matrix - the matrix to perform the iteration
 * rowstart - the upper row bounds of iteration
 * rowwend - the lower row bounds of the iteration
/***********************************************************/

void averageRow(vector<float>& matrix, int rowstart, int rowend) {

    int row, col;

    #pragma omp parallel for private(row, col) shared(matrix)

    for (row = rowstart; row < rowend + 1; row++) {
        for (col = 1; col < SIZE - 1; col++) {

            matrix[(row  * SIZE) + col] = 0.25 * (matrix[((row + 1) * SIZE) + col] +
                matrix[((row - 1) * SIZE) + col] + matrix[(row * SIZE) + col + 1] +
                matrix[(row * SIZE) + col - 1]);

        }
    }

    #pragma omp barrier // prevent race condition.

    return;
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
    int row, col;

    #pragma omp parallel for reduction (+:totalSteadyState)

    for (row = rowstart; row < rowend + 1; row++) {
        for (col = 1; col < SIZE - 1; col++) {
            
            // Is there at least a STEADY_STATE difference between the two?
            if ( abs(matrix1.at((row * SIZE) + col) - matrix2.at((row * SIZE) + col) < STEADY_STATE ) ) {
                totalSteadyState++;
            }
        }
    }

    #pragma omp barrier // prevent race condition.

    return totalSteadyState;

}
