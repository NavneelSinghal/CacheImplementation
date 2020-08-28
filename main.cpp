#include <bits/stdc++.h>

using namespace std;

bool isPowerOfTwo (int x) {
    return (x > 0) && ((x & (x - 1)) == 0);
}

int main (int argc, char* argv[]) {

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    cout << setprecision(10) << fixed;

    if (argc > 1) {
        freopen(argv[1], "r", stdin);
    } else {
        cerr << "Pass an input file name to the program as the first command-line argument\n";
        return 0;
    }

    int cache_size, block_size, cache_associativity, t;
    cin >> cache_size >> block_size >> cache_associativity >> t;

    // input validation (what if t = 0?)

    assert (cache_size > 0);
    assert (block_size > 0);
    assert (cache_associativity > 0);
    assert (t > 0);

    assert (isPowerOfTwo(block_size)); // block size should be a power of two
    assert (isPowerOfTwo(cache_size)); // cache size should be a power of two
    assert (isPowerOfTwo(cache_associativity)); // cache associativity should be a power of two
    
    assert (cache_size % block_size == 0);

    int num_blocks = cache_size / block_size;

    assert (num_blocks % cache_associativity == 0);

    int num_sets = num_blocks / cache_associativity;



}
