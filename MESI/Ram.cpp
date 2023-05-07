#include "RAM.h"
#include <iostream>
#include <cmath>

int computeBlockOffsetBits1(int blockSize) {
    int offsetBits = std::log2(blockSize);
    return offsetBits;
}

RAM::RAM() : RAM(64, 4) {
    // this->memorySize = 64;
    // this->blockSize = 4;

    // this->ram.resize(16);
    // for (auto & ramLine: this->ram){
    //     ramLine.resize(4);
    // }

    // int k = 0;
    // for (size_t i = 0; i < ram.size(); i++){
    //     for (size_t j = 0; j < ram[j].size(); j++){
    //         ram[i][j] = k++;
    //     }
    // }
}

RAM::RAM(int ramSize, int ramBlock){

    this->memorySize = ramSize;
    this->blockSize = ramBlock;

    // int ramLineSize = ceil((double)this->memorySize / (double)this->blockSize);

    // this->ram.resize(ramLineSize);
    // for (auto & ramLine: this->ram){
    //     ramLine.resize(this->blockSize);
    // }

    // int k = 0;
    // for (size_t i = 0; i < ram.size(); i++){
    //     for (size_t j = 0; j < ram[j].size(); j++){
    //         ram[i][j] = k++;
    //     }
    // }
}

std::vector<uint8_t> RAM::memRead(const uint64_t &address){

    // int paddress = address >> computeBlockOffsetBits1(this->blockSize);
    
    // std::vector<uint8_t> data(this->blockSize);

    // for (int i = 0; i < data.size(); i++)
    //     data[i] = ram[paddress][i];

    // return data;
    return std::vector<uint8_t>();
}

void RAM::writeToMem(const uint64_t &address, const std::vector<uint8_t> &data){
    // int paddress = address >> computeBlockOffsetBits1(this->blockSize);

    // for (size_t i = 0; i < data.size(); i++)
    //     ram[paddress][i] = data[i];
}