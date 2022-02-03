#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>

using namespace std;

const unsigned int ROW = 3, COL = 3;

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

int main() {
	
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	freopen("project1_1.txt","w",stdout);

    vector<double> result(ROW * COL);

    vector<double> matrix_x = {
          2.33,   4.53, -98.40,
         28.54, -75.60,  23.44,
        102.33,   3.30, -45.01,
	};

	vector<double> matrix_y = {
         46.730,   43.60,  92.1,
         -2.233, -116.80, -11.4,
        156.800,   13.44,  67.8,
	};

	chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	mulMat(matrix_x, matrix_y, result);

	chrono::steady_clock::time_point end = chrono::steady_clock::now();

    printResult(result);

	cout << "Time difference = " << chrono::duration_cast<std::chrono::microseconds> (end - begin).count() << "[µs]" << std::endl;
    cout << "Time difference = " << chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;


}