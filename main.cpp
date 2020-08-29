#include <bits/stdc++.h>

using namespace std;

/*
 * Helper functions
 */

bool isPowerOfTwo (int x) {
    return (x > 0) && ((x & (x - 1)) == 0);
}

/*
 * Data structures used
 */

const int MAX = 1e7;

long long memory[MAX];

long long time = 0;

struct Block {

    // valid bit
    bool valid;

    // dirty bit
    bool dirty;
    
    // value stored in block
    long long value;

    // tag value
    int tag;

    // previous access time
    long long time;

    // empty constructor
    Block() : valid(false), dirty(false), value(0), tag(0), time(-1) {}
};

struct Set {
    
    // number of blocks in the set
    int size;

    // index
    int index;

    // high and low priority blocks
    vector<Block> high, low;
    
    // constructor for a given set size
    Set(int set_size) : size(set_size), high(set_size >> 1), low(set_size >> 1), index(-1) {
        assert (set_size > 1);
        assert (!(set_size & 1));
    }

    // constructor for a given set size and high priority location size
    Set(int set_size, int high_size) : size(set_size), high(high_size), low(set_size - high_size), index(-1) {
        assert (0 < high_size && high_size < set_size);
    }

    // read
    // first check if it is inside cache
    // if it is there
    //   1. read it
    //   2. update access time for this element
    //   3. if in low priority, put it into high priority
    //   4. after looking at access times -
    //     a. if high priority is full, then write back the oldest element of high priority into memory (for pushing low priority elements into high priority elements)
    //     b. if low priority is full, just write back into the memory (in the end when you push elements from high to low priority after expiry)
    // else 
    //   1. access from the memory
    //   2. put it into low priority group
    //   3. for doing this, see b. of above
    // in the end, push old elements of high priority to low priority and keep doing b.
    // loc is the tag of the memory location
    void read (int loc) {
        
        int high_index = -1, low_index = -1;
        
        for (int i = 0; i < high.size(); ++i) {
            if (high[i].valid && high[i].tag == loc) {
                high_index = i;
                break;
            }
        }

        if (high_index != -1) {
            // found in high priority
            // simply update the time
            high[high_index].time = time;

        } else {
            
            for (int i = 0; i < low.size(); ++i) {
                if (low[i].valid && low[i].tag == loc) {
                    low_index = i;
                    break;
                }
            }
            
            if (low_index != -1) {
                // found in low priority
                // push this to high priority and update time of access
                // for doing this, if high priority is full then just find the oldest element in high priority and remove it from the cache, and write back to the memory if dirty bit is 1
            } else {
                // not found
            }
        }

    }

    // write
    // first check if it is inside cache
    // if it is there
    //   1. set the dirty bit to 1
    //   2. write to block
    //   3. if not into high priority, put it into high priority
    //   4. rearrange and all
    //     a.
    //     b.
    // else
    //   1. access from memory (and write to it?)
    //   2. put it into low priority group
    //   3. write to block and set dirty bit to 1
    //   4. rearrange and all
    //     a.
    //     b.

    void write (int loc, int data) {

    }

};

struct Cache {

    // number of sets in the cache
    int size;

    // sets
    vector<Set> sets;

    // constructor for a given cache size and set size
    Cache(int cache_size, int set_size) : size(cache_size) {
        assert (cache_size > 0);
        sets.assign(cache_size, Set(set_size));
    }

    // constructor for a given cache size, set size and high priority location size
    Cache(int cache_size, int set_size, int high_size) : size(cache_size) {
        assert (cache_size > 0);
        sets.assign(cache_size, Set(set_size, high_size));
        for (int i = 0; i < cache_size; ++i) {
            sets[i].index = i;
        }
    }
};

// Design Principle - We try to emulate the real implementation of a processor as much as possible, so our design choices will be motivated by the real principles used, and invalid inputs according to those principles will be discarded (for example, we check if the inputs are powers of two etc)

/*
 * Main function
 */

int main (int argc, char* argv[]) {

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    cout << setprecision(10) << fixed;

    float split_fraction = 0.5;

    if (argc > 2) {
        split_fraction = atof(argv[2]);
        if (split_fraction > 1 || split_fraction < 0) {
            cerr << "Choose a valid split fraction. Reverting to default fraction 0.5\n";
            split_fraction = 0.5;
        }
    }

    if (argc > 1) {
        freopen(argv[1], "r", stdin);
    } else {
        cerr << "Pass an input file name to the program as the first command-line argument\n";
        return 0;
    }

    int cache_size, block_size, cache_associativity, t;
    cin >> cache_size >> block_size >> cache_associativity >> t;

    // input validation 
    // t = 0 doesn't make sense because then the high priority group would be empty by vacuousness
    // cache_size = 0 doesn't make sense because empty cache is useless
    // block size can't be zero because division by 0 doesn't make sense
    // cache associativity needs to be at least two to accomodate both low priority and high priority groups

    assert (cache_size > 0);
    assert (block_size > 0);
    assert (cache_associativity > 1);
    assert (t > 0);

    // block size, cache size and cache associativity need to be powers of two, to allow as close an implementation to a real cache as possible

    assert (isPowerOfTwo(block_size));
    assert (isPowerOfTwo(cache_size));
    assert (isPowerOfTwo(cache_associativity));

    // cache size should be divisible by the block size
    assert (cache_size % block_size == 0);
    int num_blocks = cache_size / block_size;

    // number of blocks should be divisible by cache associativity
    assert (num_blocks % cache_associativity == 0);
    int num_sets = num_blocks / cache_associativity;

    int num_high = num_blocks * split_fraction;

    if (num_high == 0) num_high++;
    else if (num_high == num_blocks) num_high--;

    Cache cache(num_sets, num_blocks, num_high);

    int accesses = 0;
    int reads = 0;
    int writes = 0;
    int read_hits = 0;
    int read_misses = 0;
    int write_hits = 0;
    int write_misses = 0;

    string line;
    while (getline(stdin, line)) {

        long long location = 0, data = 0;
        char choice;

        // input starts
        int i = 0;
        int s = line.size();

        while (i < s && isspace(line[i])) ++i;

        while (i < s && isnum(line[i])) {
            location *= 10;
            location += line[i] - '0';
            ++i;
        }

        while (i < s && (isspace(line[i]) || line[i] == ',')) ++i;
        assert (i < s); // else wrong input
        choice = line[i];
        ++i;

        if (choice == 'W') {
            while (i < s && (isspace(line[i]) || line[i] == ',')) ++i;
            int sign = 1;
            if (i < s && line[i] == '-') sign = -1, ++i;
            while (i < s && isnum(line[i])) {
                data *= 10;
                data += line[i] - '0';
                ++i;
            }
            data *= sign;
        }

        // input ends

        ++time;

        if (choice == 'R') {

        } else if (choice == 'W') {

        } else {
            cerr << "Wrong instruction, terminating.\n";
            return 0;
        }
        // now for every set in cache, just push all the timeout elements from high priority to low priority

    }

    return 0;

}
