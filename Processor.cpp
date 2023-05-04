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
    int blockIndex = static_cast<unsigned int>(address & 0x03);
    // std::cout << cacheIndex << "<-CacheIndex" << std::endl;
    // std::cout << static_cast<unsigned int>(tag) << "<-tag" << std::endl;
    // read
    if (c == 0){
        //cache hit policy
        if (this->caches[cacheIndex][1] == tag && this->caches[cacheIndex][0] != 0){
            //check cache hit / check tag
            atomicBus->busRd.at(this->pid) = false;

            //increment cache hit
            totalCacheHitRead++;
        }
        else{
            //cache miss - busRd
            std::cout << this->pid << std::endl;

            //send bus request
            atomicBus->busRd.at(this->pid) = true;
            atomicBus->busRdAddr.at(this->pid) = address;
        }
        // write
    }else if(c == 1){
        // cache hit case
        if (this->caches[cacheIndex][1] == tag && this->caches[cacheIndex][0] != 0){
            //check cache hit / check tag
            atomicBus->busRd.at(this->pid) = false;

            //send bus request with intent to modify
            atomicBus->busRdX.at(this->pid) = true;
            atomicBus->busRdXAddr.at(this->pid) = address;

            //increment cache hit
            totalCacheHitWrite++;

            //changing the state to M
            this->caches[cacheIndex][0] = 2;

            //update cache line
            this->caches[cacheIndex][2+blockIndex] = data;
        }
        // snooping
    }else{
        //check each request
        for (size_t i = 0; i < atomicBus->busRd.size(); i++){
            // snooping bus read request
            int snoopedCacheIndexRd = static_cast<unsigned int>(extractBits(atomicBus->busRdAddr[i]));
            int snoopedCacheIndexRdX = static_cast<unsigned int>(extractBits(atomicBus->busRdXAddr[i]));

            uint8_t tagRd = atomicBus->busRdAddr[i] >> 4;
            uint8_t tagRdX = atomicBus->busRdXAddr[i] >> 4;

            // snooping bus read request
            if (atomicBus->busRd[i]){
                // if is M state, issue flush
                if (this->caches[snoopedCacheIndexRd][1] == tagRd && this->caches[snoopedCacheIndexRd][0] == 2){
                    // change to S state
                    this->caches[snoopedCacheIndexRd][0] = 1;
                    // flush
                    std::vector<uint8_t> temp(4);
                    for (size_t i = 0; i < temp.size(); i++){
                        temp[i] = this->caches[snoopedCacheIndexRd][i+2];
                    }
                    ram->writeToMem(address,temp);
                }
            }
            // snooping bus write request
            if (atomicBus->busRdX[i]){
                // if is S state
                std::cout << "Snooping processor..." << i << std::endl;
                if (this->caches[snoopedCacheIndexRdX][1] == tagRdX && this->caches[snoopedCacheIndexRdX][0] == 1){
                    // change to I state
                    this->caches[snoopedCacheIndexRdX][0] = 0;
                }
            }
        }
    }
};

void Processor::busResponse(const int &c, const uint8_t &address, const uint8_t &data, AtomicBus *atomicBus, RAM *ram){

    uint8_t tag = address >> 4;
    int cacheIndex = static_cast<unsigned int>(extractBits(address));
    int blockIndex = static_cast<unsigned int>(address & 0x03);

    if (c == 0){
        //read from main mem
        std::vector<uint8_t> temp = ram->memRead(address);
        for(int i = 2; i < this->caches[cacheIndex].size();i++)
            this->caches[cacheIndex][i] = temp[i-2];

        //chaging the state to S
        this->caches[cacheIndex][0] = 1;

        //changing the tag
        this->caches[cacheIndex][1] = tag;
    }
}

uint8_t extractBits(uint8_t value) {
    return (value >> 2) & 0x03;
}