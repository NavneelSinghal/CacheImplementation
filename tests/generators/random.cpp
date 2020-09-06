#include <iostream>
#include <random>
using namespace std;

int main() {
    int n = 100000;
    cout << 1024 << endl << 8 << endl << 2 << endl << 4 << endl;
    for (int i = 0; i < n; ++i) {
        if (rand() & 1) {
            cout << rand() % 1024 << ", W, " << rand() % 1024 << "\n";
        } else {
            cout << rand() % 1024 << ", R\n";
        }
    }
}
