#include <vector>

class RAM{
    int memorySize; //in bytes
    int blockSize; //4-bytes per block

    std::vector<std::vector<char>> caches; //char is one byte

    //default memorySize is 64 bytes
    //default blockSize if 4-bytes per block

    //so total of 16 blocks
    //each block contains 4 bytes, so size of 4 + 3 = 7, we need additional space for tag, cache index, and block offset

    // for ex.
    // 2 bits tag + 2 bits cache index + 6 bits block offset + 1 bytedata + 1 bytedata + 1 bytedata + 1bytedata
    // 0: [2 bits tag, 2 bits cache index, 2 bits block offset,1bytedata,1bytedata,1bytedata, 1bytedata]
    // 1: [2 bits tag, 2 bits cache index, 2 bits block offset,1bytedata,1bytedata,1bytedata, 1bytedata]
    // 2: [2 bits tag, 2 bits cache index, 2 bits block offset,1bytedata,1bytedata,1bytedata, 1bytedata]
    // 3: [2 bits tag, 2 bits cache index, 2 bits block offset,1bytedata,1bytedata,1bytedata, 1bytedata]
    // .
    // .
    // .
    // 15: [2 bits tag, 2 bits cache index, 2 bits block offset,1bytedata,1bytedata,1bytedata, 1bytedata]

    // initialize the size of caches based on the value of cacheSize and blockSize
    // also, initilize the content of caches to 0, and the state to 'I';
    RAM(); 
};