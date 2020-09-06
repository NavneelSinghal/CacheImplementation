# Assignment 12: Simulating Cache Memory

## Implementation

I have followed a modular design, with the blocks being the unit of granularity in the cache (as was clarified on Piazza).

A block contains the data, valid bit, dirty bit, tag, and the time remaining till downgrading (from higher priority to lower priority)

A set contains the set index, a fixed-size vector of blocks, and two integers high and low (which will be explained below).
I have implemented variable-sized partitions of high and low priority blocks (to ensure maximum usage of cache space to increase hit ratio), by following the following ordering:

First come all the high priority blocks, sorted in the decreasing order of arrival time (i.e. decreasing order of remaining time), then come all the low priority blocks, sorted in the decreasing order of arrival time.

To keep track of these two partitions, 'high' is the length of the first block, and 'low' is the length of both the blocks put together (i.e., in a sense they are the indices just after the high and the low priority blocks end).

Finally, a cache is a fixed-size vector of sets as defined above.

Now for the implementation:

1. Read instructions
When there is a read instruction to the cache, we first search for the tag in the cache.
If it is found, then it is either a high priority or a low priority element. We bubble it up to the first position and set the time remaining to T. If it was a low priority element, we simply need to add 1 to the high pointer to accomodate the new element.
Otherwise if it is not found, we check if there is any free space in the cache or not, and if there is, we copy the data from the memory to the cache and bubble it up to the most recent end of the low priority group. If there is no space, we perform a cache replacement. So for this, we check if there exists a low priority group or not (corner case). Then we suitably adjust 'high' to replace the cache element if there is no element in the low priority group or not. Otherwise we just bubble
up the element after replacing the lowest priority element (after writing it back if it was dirty or not).

2. Write instructions
As in the read instruction, we check if it exists, and if it does, then just bubble it up as in read after setting the dirty bit to true.
Otherwise, we again check if there is any free space in the cache or not, and if there is, we copy the data to the last element and bubble it up. We do the cache replacement in a similar way, and we also turn on the dirty bit of the new data written.

General input and output

The input and output is done using simple file I/O, and inputs are carefully validated before being used. In particular, one validation step that's taken is to check whether the data fits inside the block or not (data is assumed to be a signed integer for our purposes). The rest of those correspond to the design decisions about having sizes as powers of two only (to emulate a processor as close as possible, also clarified on Piazza).

## Running instructions

As in the Makefile, to compile the code, run g++ -o main main.cpp
To run the code, run ./main <filename>

The program can also be run in a verbose mode, so to run that, run 'make verbose' to compile the code in that mode then run the code.
Another mode, primarily for checking correctness of write back, is the mem mode, and just run 'make mem' to compile the code.
To use both these modes, just run 'make memverbose'

## Testing

The following types of testing has been carried out:

### Corner cases and input validation

1. Corner case testing for the case when a high priority block needs to be replaced
2. Testing whether the data is inside the input range or not (both positive and negative)
3. Testing whether sizes are not powers of two or not
4. Testing for invalid inputs (spacing and commas etc)

### Correctness testing

1. Testing whether the priority order is working correctly
2. Testing whether the replacement policy is working correctly
3. Testing whether the timeout is working correctly
4. Testing whether the sizes are variable or not

### Testing on large real-life programs for optimal parameters

I chose matrix multiplication as a program for testing out the cache efficiencies. The results were as follows:

1. Varying the cache size:
As cache size is increased, the hit ratio increases and it becomes essentially constant at larger cache sizes. For example, when everything else was kept constant (to 8, 2, 4), the following efficiencies were found:

Cache size      Hit ratio
16              0.477
64              0.478
256             0.683
1024            0.709
2048            0.925
4096            0.969
65536           0.969

2. Varying the block size:
The block size can only be at most 8 bytes, however on running a simulation with smaller data on the same reads and writes, we can note that the number of blocks decreases, so the effect is similar to that of keeping block size constant and decreasing the cache size (since the block size doesn't matter in our simulation).

3. Varying the idle time:
It was noted that there was a minor change in the performance, when everything else was constant (2048, 8, 2), and I ran the code on 17 * 17 matrix instead.

1 Hit ratio = 0.8722732060
2 Hit ratio = 0.8722732060
4 Hit ratio = 0.8722732060
8 Hit ratio = 0.8723233539
16 Hit ratio = 0.8723233539
32 Hit ratio = 0.8723233539
64 Hit ratio = 0.8724236498
128 Hit ratio = 0.8790431774
256 Hit ratio = 0.8795446567
512 Hit ratio = 0.8783411063
1024 Hit ratio = 0.8847600421
2048 Hit ratio = 0.8746301590

4. Varying the cache associativity:
Matrix multiplication has a peculiar property of decreasing the hit ratio on increasing the cache associativity than most other programs (reference: https://www.researchgate.net/publication/258870751_Performance_Drawbacks_for_Matrix_Multiplication_using_Set_Associative_Cache_in_GPU_devices), so I ran a random test case to determine effect of cache associativity on a program.

1 Hit ratio = 0.4970900000
2 Hit ratio = 0.4975300000
4 Hit ratio = 0.4980100000
8 Hit ratio = 0.4990000000
16 Hit ratio = 0.4980900000
32 Hit ratio = 0.4983800000

From here, as cache associativity increases, the hit ratio roughly increases as well
