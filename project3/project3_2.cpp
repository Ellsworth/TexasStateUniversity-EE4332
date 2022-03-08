#include <iostream>
#include <iomanip>
#include <chrono> // required for timing runtime.
#include <vector> // semi-required depending on gcc version. For using vectors.
#include <stdlib.h> // used for abs() function.
#include "mpi.h"


using namespace std;

// Global consts.

// Total size for each axis of the matrix. eg. 7000 x 7000.
const unsigned int SIZE = 700;

// Difference from the last run to consider an element in steady state.
const float STEADY_STATE = 0.01;

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

    // Pipe stdout to output file.
    //freopen("project3_2.txt","w", stdout);


	int rank, numProcess, rc, tag, offset, rows;
    int startrow, endrow, iterations = 0, isSteadyState = 0;
	double t1,t2;
  
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcess);

    int steadystate = 0, sum_steadystate = 0;
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

        // Do while the number of elements in steady states does not equal the
        //  total number of elements.


        do {
            
            // Reset steady state count.
            sum_steadystate = 0;

            // Set the prev array to the current plate.
            prev = plate;

            for (int process = 1; process < numProcess; process++) {


                // Calculate exactly what rows each workers needs.
                startrow = (process - 1) * rowsPerProcess + 1;
                endrow = (process) * rowsPerProcess;
                
                // Make sure the last process gets exactly the end of the array.
                if (process == numProcess - 1) endrow = SIZE - 2;

                // Send the data.
                //cout << "Sending startrow " << startrow << " to " << process << endl;
                MPI_Send(&startrow, 1, MPI_INT, process, MASTER_ID, MPI_COMM_WORLD);
                //cout << "Sending endrow " << endrow << " to " << process << endl;
                MPI_Send(&endrow, 1, MPI_INT, process, MASTER_ID, MPI_COMM_WORLD);
                
                //cout << "Sending array to " << process << endl;
                MPI_Send(&plate[(startrow - 1) * SIZE], (SIZE * (endrow - startrow + 2)), MPI_FLOAT, process, MASTER_ID, MPI_COMM_WORLD);
                //cout << "TX prev to " << process << endl;
                MPI_Send(&prev[(startrow - 1) * SIZE], (SIZE * (endrow - startrow + 2)), MPI_FLOAT, process, MASTER_ID, MPI_COMM_WORLD);


                MPI_Recv(&plate[(startrow) * SIZE], (SIZE * (endrow - startrow + 1)), MPI_FLOAT, process, WORKER_ID, MPI_COMM_WORLD, &status);
                //cout << rank << ": RX results from " << process << endl;

                MPI_Recv(&steadystate, 1, MPI_INT, process, WORKER_ID, MPI_COMM_WORLD, &status);
                //cout << rank << ": RX steady state count " << steadystate << " from " << process << endl;
                sum_steadystate += steadystate;
                
            }

            if (TOTAL_ELEMENTS <= sum_steadystate) isSteadyState = 1;


            // THIS MUST BE DONE LAST!
            for (int process = 1; process < numProcess; process++) {
                //cout << rank << ": TX isSteadyState to " << process << endl;
                MPI_Send(&isSteadyState, 1, MPI_INT, process, MASTER_ID, MPI_COMM_WORLD);
            }

             

            cout << "Complete: " << sum_steadystate << "/" << TOTAL_ELEMENTS << endl;
            cout << "Percent: " << (float)sum_steadystate / (float)TOTAL_ELEMENTS << "%" << endl;
            iterations++;

        } while(!isSteadyState);

        // Works with 2 nodes. Check when using 3.

        // When we are no longer ins steady state, print the array.
        printMatrix(plate);
        cout << endl;

    }

    // Worker code.
    if (rank > 0) {

        while (!isSteadyState) {

        //cout << "Worker " << rank << " reporting!" << endl;

        // Receive the start and end rows we are responsible for.
        
        MPI_Recv(&startrow, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        //cout << rank << ": RX startrow " << startrow << endl;
        MPI_Recv(&endrow, 1, MPI_INT, 0, MASTER_ID, MPI_COMM_WORLD, &status);
        //cout << rank << ": RX endrow " << endrow << endl;
        

        MPI_Recv(&plate[(startrow - 1) * SIZE], (SIZE * (endrow - startrow + 2)), MPI_FLOAT, 0, MASTER_ID, MPI_COMM_WORLD, &status);
        //cout << rank << ": RX array" << endl;


        MPI_Recv(&prev[(startrow - 1) * SIZE], (SIZE * (endrow - startrow + 2)), MPI_FLOAT, 0, MASTER_ID, MPI_COMM_WORLD, &status);
        //cout << rank << ": RX plate" << endl;

        averageRow(plate, startrow, endrow);

        // Send our results back to the the master.
        MPI_Send(&plate[startrow * SIZE], (SIZE * (endrow - startrow + 1)), MPI_FLOAT, 0, WORKER_ID, MPI_COMM_WORLD);
        //cout << rank << ": sent results back!" << endl;

        steadystate = countSteadyState(plate, prev, startrow, endrow);

        MPI_Send(&steadystate, 1, MPI_INT, 0, WORKER_ID, MPI_COMM_WORLD);
        //cout << rank << ": TX steady state count." << endl; 

        // THIS MUST BE DONE LAST!
        MPI_Recv(&isSteadyState, 1, MPI_INT, 0, MASTER_ID, MPI_COMM_WORLD, &status);
        //cout << rank << ": RX isSteadyState = " << isSteadyState << endl;

        if (isSteadyState) break;

        }
        
    }

    // Cleanly terminate all of the workers and the message passing interface.
    MPI_Finalize();

    if (rank == 0) {

    printMatrix(plate);


    // Stop the clock and print our time.
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

    fill(matrix.begin(), matrix.end(), 0.5F);

    for (int i = 0; i < SIZE; i++) {
        matrix.at(i) = 0;
        matrix.at(SIZE - 1 + (SIZE * i)) = 0;
    }

    for (int row = SIZE; row > 0; row--) {

        matrix[row * SIZE] = OFFSET * row;
        matrix[(SIZE * (SIZE - 1)) + row] = 100 - (OFFSET * row);
    }
}

/***********************************************************
 * averageArray - Perform the Jacobi iteration across the array
 * 
 * matrix - the matrix to perform the iteration
 * rowstart - the upper row bounds of iteration
 * rowwend - the lower row bounds of the iteration
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

            if ( abs(matrix1.at((row * SIZE) + col) - matrix2.at((row * SIZE) + col) < STEADY_STATE ) ) totalSteadyState++;
        }
    }

    return totalSteadyState;

}
