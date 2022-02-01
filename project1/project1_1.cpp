#include <iostream>
#include <iomanip>
#include <chrono>

using namespace std;

const int ROW = 3;
const int COL = 3;

void mulMat(double mat1[][COL], double mat2[][COL]) {
	double result[ROW][COL];

	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			result[i][j] = 0;

			for (int k = 0; k < ROW; k++) {
				result[i][j] += mat1[i][k] * mat2[k][j];
			}

            if (result[i][j] > 0) cout << " ";
			cout << result[i][j] << " ";
            
		}

		cout << endl;
	}
}

int main(void) {

	double matrix_x[ROW][COL] = {
        { 02.33,   4.53, -98.40},
        { 28.54, -75.60,  23.44},
        {102.33,   3.30, -45.01},
	};

	double matrix_y[ROW][COL] = {
        { 46.730,   43.60,  92.1},
        { -2.233, -116.80, -11.4},
        {156.800,   13.44,  67.8},
	};

	chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	mulMat(matrix_x, matrix_y);

	chrono::steady_clock::time_point end = chrono::steady_clock::now();

	cout << "Time difference = " << chrono::duration_cast<std::chrono::microseconds> (end - begin).count() << "[µs]" << std::endl;
    cout << "Time difference = " << chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

	return 0;
}
