#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>

using namespace std;

/*
 * Helper functions
 */

bool isPowerOfTwo (int x) {
    return (x > 0) && ((x & (x - 1)) == 0);
}

bool isnum (char x) {
    return x <= '9' && x >= '0';
}

/*
 * Data structures used
 */

const int MAX = 1e7;

long long memory[MAX];

long long t = 0;
int cache_size, block_size, cache_associativity, num_sets, num_blocks;

// stats
int accesses = 0;
int reads = 0;
int read_hits = 0;
int read_misses = 0;
int writes = 0;
int write_hits = 0;
int write_misses = 0;

int combine (int tag, int set_number) {
    return set_number * num_sets + tag;
}

// returns tag, set_number
pair<int, int> unpack (int mem_loc) {
    return make_pair(mem_loc % num_sets, mem_loc / num_sets);
}

struct Block {

    // valid bit
    bool valid;

    // dirty bit
    bool dirty;
    
    // value stored in block
    long long value;

    // tag value
    int tag;

    // timeout countdown
    long long time;

    // empty constructor
    Block() : valid(false), dirty(false), value(0), tag(0), time(-1) {}

    Block(int value, int tag) : valid(true), dirty(false), value(value), tag(tag), time(-1) {}

    void print(int s) {
        cout << "Valid: " << (valid ? "Yes" : "No") << endl;
        cout << "Dirty: " << (dirty ? "Yes" : "No") << endl;
        cout << "Value: " << (value) << endl;
        cout << "Tag: " << tag << endl;
        cout << "Original memory location: " << combine(tag, s) << endl;
        cout << "Countdown: " << time << endl;
    }
};

struct Set {
    
    // number of blocks in the set
    int size;

    // index
    int index;

    // data
    vector<Block> set;

    // pointer to first element after high and low priority groups
    int high, low;
    
    // constructor for a given set size
    Set(int set_size) : size(set_size), set(set_size), high(0), low(0), index(-1) {}

    void print() {
        cout << endl;
        cout << "Set # " << index << endl;
        cout << "High priority elements: " << high << endl;
        cout << "Low priority elements: " << low - high << endl;
        cout << "Blocks" << endl;
        for (int i = 0; i < low; ++i) {
            cout << "Block # " << i << endl;
            set[i].print(index);
        }
    }

    // this is all invalid for now
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

    // loc is the tag of the memory location and not the memory location
    void read (int loc) {
        
        accesses++;
        reads++;
        
        // first search for element 
        int found_index = -1;
        for (int i = 0; i < low; ++i) {
            if (loc == set[i].tag) {
                found_index = i;
                break;
            }
        }
        
        if (found_index == -1) read_misses++;
        else read_hits++;

        if (found_index < high && found_index != -1) {
            // found it in high priority
            for (int i = found_index - 1; i >= 0; --i) {
                swap(set[i + 1], set[i]);
            }
            set[0].time = t;
        } else if (found_index < low && found_index != -1) {
            // found in low priority
            high++;
            for (int i = found_index - 1; i >= 0; --i) {
                swap(set[i + 1], set[i]);
            }
            set[0].time = t;
        } else {
            // not found
            int mem_loc = combine(loc, index);
            // fetch element from memory
            long long value = memory[mem_loc];
            if (low < size) {
                // cache is not full
                // set the last block of the set to the just-read block
                set[low] = Block(value, loc);
                // bring it to the front of the low priority blocks
                for (int i = low - 1; i >= high; --i) {
                    swap(set[i + 1], set[i]);
                }
                ++low;
            } else {
                // cache is full
                // need to replace the first element of the low priority, or if all the elements have high priority, replace the last element of the high priority
                Block block_to_replace;
                int index_to_replace;
                if (low == high) {
                    // all elements have high priority
                    --high;
                    block_to_replace = set[low - 1];
                    index_to_replace = high;
                } else {
                    // some elements have low priority, so choose the last element and shift all elements to the right
                    block_to_replace = set[low - 1];
                    for (int i = low - 2; i >= high; --i) {
                        swap(set[i + 1], set[i]);
                    }
                    index_to_replace = high;
                }

                // if dirty, then write back
                if (block_to_replace.dirty) {
                    int write_back_loc = combine(block_to_replace.tag, index);
                    memory[write_back_loc] = block_to_replace.value;
                }

                set[index_to_replace] = Block(value, loc);
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
        
        accesses++;
        writes++;

        int found_index = -1;
        for (int i = 0; i < low; ++i) {
            if (loc == set[i].tag) {
                found_index = i;
                break;
            }
        }

        if (found_index == -1) write_misses++;
        else write_hits++;

        if (found_index != -1) {
            
            set[found_index].value = data;
            set[found_index].time = t;

            if (!set[found_index].dirty) {
                // set dirty bit to true and then write to this position
                set[found_index].dirty = 1;
            }

            if (found_index < high) {
            } else if (found_index < low) {
                ++high;
            } else {
                assert(false);
            }

            for (int i = found_index - 1; i >= 0; --i) {
                swap(set[i + 1], set[i]);
            }

        } else {
            // not found
            // so basically check if there is more space in cache or not
            // if there is, then just put it in the low priority space in the front
            // else replace the least used element in low priority (or in high priority if low priority is empty) with this element,
            // and to do that, write back the old thing, and copy paste the data here, and set the dirty bit to true
            if (low < size) {
                set[low] = Block(data, loc);
                set[low].dirty = true;
                for (int i = low - 1; i >= high; --i) {
                    swap(set[i + 1], set[i]);
                }
                ++low;
            } else {
                // need to replace last element with this element
                Block block_to_replace;
                int index_to_replace;
                if (low == high) {
                    // all elements have high priority
                    --high;
                    block_to_replace = set[low - 1];
                    index_to_replace = high;
                } else {
                    // some elements have low priority, so choose the last element and shift all elements to the right
                    block_to_replace = set[low - 1];
                    for (int i = low - 2; i >= high; --i) {
                        swap(set[i + 1], set[i]);
                    }
                    index_to_replace = high;
                }

                // if dirty, then write back
                if (block_to_replace.dirty) {
                    int write_back_loc = combine(block_to_replace.tag, index);
                    memory[write_back_loc] = block_to_replace.value;
                }

                set[index_to_replace] = Block(data, loc);
                set[index_to_replace].dirty = true;
            }
        }
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
        for (int i = 0; i < set_size; ++i) {
            sets[i].index = i;
        }
    }

};

// Design Principle - We try to emulate the real implementation of a processor as much as possible, so our design choices will be motivated by the real principles used, and invalid inputs according to those principles will be discarded (for example, we check if the inputs are powers of two etc)

/*
 * Main function
 */

int main (int argc, char* argv[]) {

    cout << setprecision(10) << fixed; 

    if (argc > 1) {
        freopen(argv[1], "r", stdin);
    } else {
        cerr << "Pass an input file name to the program as the first command-line argument\n";
        return 0;
    }

    cin >> cache_size >> block_size >> cache_associativity >> t;
    cout << "Cache size: " << cache_size << endl
         << "Block size: " << block_size << endl
         << "Cache associativity: " << cache_associativity << endl
         << "Timeout: " << t << endl;
    // input validation 
    // t = 0 doesn't make sense because then the high priority group would be empty by vacuousness
    // cache_size = 0 doesn't make sense because empty cache is useless
    // block size can't be zero because division by 0 doesn't make sense

    assert (cache_size > 0);
    assert (block_size > 0);
    assert (cache_associativity > 0);
    assert (t > 0);

    // block size, cache size and cache associativity need to be powers of two, to allow as close an implementation to a real cache as possible

    assert (isPowerOfTwo(block_size));
    assert (isPowerOfTwo(cache_size));
    assert (isPowerOfTwo(cache_associativity));

    // cache size should be divisible by the block size
    assert (cache_size % block_size == 0);
    num_blocks = cache_size / block_size;

    // number of blocks should be divisible by cache associativity
    assert (num_blocks % cache_associativity == 0);
    num_sets = num_blocks / cache_associativity;

    cout << "Number of sets: " << num_sets << endl;

    Cache cache(num_sets, num_blocks);

    string line;
    while (cin >> line) {

        cout << "-----------------------" << endl;

        /*for (auto &c : line) {
            if (c == ',') c = ' ';
        }*/
        long long location = 0, data = 0;
        char choice;

        // input starts

        location = stoi(line.substr(0, line.size() - 1));
        cin >> line;
        choice = line[0];
        if (choice == 'W') {
            cin >> line;
            data = stoi(line);
        }

        cout << "Location: " << location << endl
             << "Choice: " << choice << endl
             << "Data: " << data << endl;

        // input ends
        
        int set_index, tag;
        tie(tag, set_index) = unpack(location);

        if (choice == 'R') {
            cache.sets[set_index].read(tag);
        } else if (choice == 'W') {
            cache.sets[set_index].write(tag, data);
        } else {
            cerr << "Wrong instruction, terminating.\n";
            return 0;
        }
        // now for every set in cache, just push all the timeout elements from high priority to low priority

        // decrease time globally

        for (auto &set : cache.sets) {
            for (int i = 0; i < set.high; ++i) {
                --set.set[i].time;
            }
        }

        for (auto &set : cache.sets) {
            // check only the last element of the set
            if (set.high >= 1) {
                if (set.set[set.high - 1].time < 0) {
                    --set.high;
                }
            }
        }

        cout << "Printing cache\n";

        for (auto &set : cache.sets) {
            set.print();
        }

    }

    return 0;

}
