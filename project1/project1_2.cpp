#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>

using namespace std;

const unsigned int ROW = 3, COL = 3;

const double RNG_MIN = -1999.999, RNG_MAX = 1999.999;

void mulMat(vector<double> mat1, vector<double> mat2, vector<double>& result) {

	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			result[(3 * i) + j] = 0;

			for (int k = 0; k < ROW; k++) {
				result[(3 * i) + j] += mat1[(3 * i) + k] * mat2[(3 * k) + j];
			}
            
		}

	}
}

void printResult(vector<double> result) {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {


            if (result[(3 * i) + j] > 0) cout << " ";
			cout << result[(3 * i) + j] << " ";
            
		}

		cout << endl;
	}
}

double getRandomNumber(double min, double max) {
	return min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (max - min)));
}

void fillMatrixWithRandom(unsigned int max, double rng_min, double rng_max, vector<double>& matrix) {
	for (int i = 0; i < max; i++) {
		matrix[i] = getRandomNumber(rng_min, rng_max);
	}
}

int main() {
	
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	srand (static_cast <unsigned> (time(0)));

	freopen("project1_2.txt","w",stdout);

    vector<double> result(ROW * COL);
    vector<double> matrix_x(ROW * COL);
	vector<double> matrix_y(ROW * COL);

	fillMatrixWithRandom(ROW * COL, RNG_MIN, RNG_MAX, matrix_x);
	fillMatrixWithRandom(ROW * COL, RNG_MIN, RNG_MAX, matrix_y);

	printResult(matrix_x);
	cout << endl;
	printResult(matrix_y);
	cout << endl;

	
	chrono::steady_clock::time_point matrix_begin = chrono::steady_clock::now();

	mulMat(matrix_x, matrix_y, result);

	chrono::steady_clock::time_point matrix_end = chrono::steady_clock::now();

    printResult(result);
	cout << endl;

	chrono::steady_clock::time_point end = chrono::steady_clock::now();

	cout << "Total Program Runtime: " << chrono::duration_cast<std::chrono::microseconds> (end - begin).count() << "[µs]" << std::endl;	
	cout << "Matrix Multiplication: " << chrono::duration_cast<std::chrono::microseconds> (matrix_end - matrix_begin).count() << "[µs]" << std::endl;
    //cout << "Time difference = " << chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

}