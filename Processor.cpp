#include "Processor.h"
#include <iostream>

int Processor::ID = 0;
uint8_t extractBits(uint8_t);

Processor::Processor(){
    this->pid = this->ID++;

    int cacheSize = 16;
    int blockSize = 4;

    // Resize the outer vector (rows)
    this->caches.resize(4);
    for (auto& cache : caches) {
        cache.resize(6);
    }


    // Loop through each row and resize the inner vector (columns)
    for (size_t i = 0; i < this->caches.size(); ++i) {
        this->caches[i][0] = 0;
        std::fill(this->caches[i].begin() + 1, this->caches[i].end(), 0);
    }
}

void Processor::operation(const int &c, const uint8_t &address, const uint8_t &data, AtomicBus *atomicBus, RAM *ram){
    uint8_t tag = address >> 4;
    int cacheIndex = static_cast<unsigned int>(extractBits(address));
    std::cout << cacheIndex << "<-CacheIndex" << std::endl;
    std::cout << static_cast<unsigned int>(tag) << "<-tag" << std::endl;
    // read
    if (c == 0){
        if (this->caches[cacheIndex][1] == tag && this->caches[cacheIndex][0] == 0b00000000){
            //check cache hit / check tag
            atomicBus->busRd.at(this->pid) = false;
            std::cout<< "cache hit!" << std::endl;
        }
        else{
            //cache miss - busRd
            std::cout << this->pid << std::endl;

            atomicBus->busRd.at(this->pid) = true;
            atomicBus->busRdAddr.at(this->pid) = address;
            //request data from RAM

            std::vector<uint8_t> temp = ram->memRead(address);

            for(int i = 2; i < this->caches[cacheIndex].size();i++)
                this->caches[cacheIndex][i] = temp[i-2];

            this->caches[cacheIndex][0] = 1;
        }
    }
};

uint8_t extractBits(uint8_t value) {
    return (value >> 2) & 0x03;
}