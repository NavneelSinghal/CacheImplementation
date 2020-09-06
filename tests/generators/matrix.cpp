#include <iostream>
#include <random>
#include <cassert>

using namespace std;

const int N = 16;
int a[N][N];
int b[N][N];

void print_write (int i, int j) {
    cout << (i * N + j) << ", W, " << a[i][j] << '\n';
}

void print_read (int i, int j) {
    cout << (i * N + j) << ", R\n";
}

int main(int argc, char* argv[]) {
    
    assert(argc == 5);
    for (int i = 1; i < 5; ++i) {
        cout << argv[i] << '\n';
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            a[i][j] = rand() % 100;
            print_write(i, j);
        }
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < N; ++k) {
                print_read(i, k);
                print_read(k, j);
                print_read(N + i, j);
                b[i][j] += a[i][k] * a[k][j];
                print_write(N + i, j);
            }
        }
    }
}
