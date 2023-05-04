#include "RAM.h"
#include <iostream>

uint8_t setFirstTwoBitsToZero(uint8_t);

RAM::RAM(){
    this->memorySize = 64;
    this->blockSize = 4;

    this->ram.resize(16);
    for (auto & ramLine: this->ram){
        ramLine.resize(4);
    }

    uint8_t k = 0;
    for (size_t i = 0; i < ram.size(); i++){
        for (size_t j = 0; j < ram[j].size(); j++){
            ram[i][j] = k++;
        }
    }
}

std::vector<uint8_t> RAM::memRead(const uint8_t &address){

    int paddress = static_cast<unsigned int>(setFirstTwoBitsToZero(address) >> 2);
    
    std::vector<uint8_t> data(4);

    for (int i = 0; i < data.size(); i++)
        data[i] = ram[paddress][i];
        
    return data;
}

void RAM::writeToMem(const uint8_t &address, const std::vector<uint8_t> &data){
    int paddress = static_cast<unsigned int>(setFirstTwoBitsToZero(address) >> 2);

    for (size_t i = 0; i < data.size(); i++){
        ram[paddress][i] = data[i];
    }
}

uint8_t setFirstTwoBitsToZero(uint8_t value) {
    uint8_t mask = ~(3); // Creates a mask with all bits set to 1 except the first two
    return value & mask; // Sets the first two bits to 0 using bitwise AND
}