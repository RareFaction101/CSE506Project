#pragma once
#include <vector>
#include <cstdint>

class RAM{
    public:
    int memorySize; //in bytes
    int blockSize; //4-bytes per block

    std::vector<std::vector<uint8_t>> ram; //char is one byte

    //default memorySize is 64 bytes
    //default blockSize if 4-bytes per block

    //so total of 16 blocks
    //each block contains 4 bytes, so size of 4 + 3 = 7, we need additional space for tag, cache index, and block offset

    // for ex.
    // 2 bits tag + 2 bits cache index + 2 bits block offset + 1 bytedata + 1 bytedata + 1 bytedata + 1bytedata
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

    // this function takes address as input, returns the data from the main memory
    std::vector<uint8_t> memRead(const uint8_t &address);

    // this function write the data to the particular RAM address
    void writeToMem(const uint8_t &address, const std::vector<uint8_t> &data);
};