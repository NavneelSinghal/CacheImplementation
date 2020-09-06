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

## Testing

The following types of testing has been carried out:

### Corner cases and input validation

1. Corner case testing for the case when a high priority block needs to be replaced
2. Testing whether the data is inside the input range or not (both positive and negative)
3. Testing whether sizes are not powers of two or not
4. Testing for invalid inputs (spacing and commas etc)

### Stress testing 

### Testing on simple real-life programs

### Testing for optimal parameters

