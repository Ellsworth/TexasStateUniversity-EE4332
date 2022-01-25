#include <iostream>
// #include <chrono>

using namespace std;

void printVector(int v[]) {

    cout << endl;
    //cout << "Scalar: " << k << endl;
    cout << "Vector ";
    for (int i = 0; i < 4; i++) {
        cout << " " << v[i];
    }
    
}

int main() {
    int v[4] = {1, 3, 4, 5};

    int k = 4;

    printVector(v);

    for (int i = 0; i < k; i++) {
        v[i] = v[i] * 4;
    }

    printVector(v);


    cout << endl;

}
