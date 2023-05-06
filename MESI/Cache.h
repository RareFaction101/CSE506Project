#include <vector>

class Cache{
    public:

    int cacheSize; //in bytes
    int blockSize; //4-bytes per block

    //char is one byte

    //default cacheSize is 16 bytes
    //default blockSize if 4-bytes per block

    //so total of 4 blocks
    //each block contains 4 bytes, so size of 4 + 2 = 6, we need additional space for state and tag

    //for ex.
    // 2bits state + 2bits tag (we are using 16 blocks of ram, so log2(16) - los2(4) = 2) + 1byte+1byte+1byte+1byte
    // ['M', tag, 1bytedata,1bytedata,1bytedata,1bytedata]
    // ['I', tag, 1bytedata,1bytedata,1bytedata,1bytedata]
    // ['I', tag, 1bytedata,1bytedata,1bytedata,1bytedata]
    // ['S', tag, 1bytedata,1bytedata,1bytedata,1bytedata]

    // initialize the size of caches based on the value of cacheSize and blockSize
    // also, initilize the content of caches to 0, and the state to 'I';
    Cache();
};