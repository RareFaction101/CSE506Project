#include "Processor.h"
#include <iostream>

int Processor::ID = 0;
uint8_t extractBits(uint8_t);

Processor::Processor() {
    // Assign a unique ID to the processor and increment the static ID counter
    this->pid = this->ID++;

    // Set the cache size and block size
    int cacheSize = 16;
    int blockSize = 4;

    // Resize the outer vector (caches) to have 4 elements (cache lines)
    this->caches.resize(4);

    // Resize each inner vector (cache line) to have 6 elements (metadata and data)
    for (auto& cache : caches) {
        cache.resize(6);
    }

    // Initialize the cache lines
    for (size_t i = 0; i < this->caches.size(); ++i) {
        // Set the first element of each cache line to 0 (e.g., a tag or status bit)
        this->caches[i][0] = 0;

        // Set the remaining elements of each cache line to 0 (e.g., data or other metadata)
        std::fill(this->caches[i].begin() + 1, this->caches[i].end(), 0);
    }
}

void Processor::operation(const int &c, const uint8_t &address, const uint8_t &data, AtomicBus *atomicBus, RAM *ram) {
    // Check the operation type based on the value of 'c'
    if (c == 0) {
        // If 'c' is 0, perform a read operation
        Processor::PrRd(address, atomicBus);
    } else if (c == 1) {
        // If 'c' is 1, perform a write operation
        Processor::PrWr(address, data, atomicBus);
    } else {
        // If 'c' is any other value, perform a snooping operation
        Processor::Snooping(atomicBus, ram);
    }
}

void Processor::PrRd(const uint8_t &address, AtomicBus *atomicBus) {
    // Extract the tag and cache index from the address
    uint8_t tag = address >> 4;
    int cacheIndex = static_cast<unsigned int>(extractBits(address));
    int blockIndex = static_cast<unsigned int>(address & 0x03);

    // Check if the cache line at 'cacheIndex' has the same tag and a valid state (non-zero)
    if (this->caches[cacheIndex][1] == tag && this->caches[cacheIndex][0] != 0) {
        // Cache hit - set the busRd signal for this processor to false
        atomicBus->busRd.at(this->pid) = false;

        // Increment the cache hit counter for read operations
        totalCacheHitRead++;
    } else {
        // Cache miss - set the busRd signal and busRdAddr for this processor
        atomicBus->busRd.at(this->pid) = true;

        // Send a bus request for the missing address
        atomicBus->busRdAddr.at(this->pid) = address;

        // Increment the cache miss counter for read operations
        totalCacheMissRead++;
    }
}

void Processor::PrWr(const uint8_t &address, const uint8_t &data, AtomicBus *atomicBus) {
    // Extract the tag, cache index, and block index from the address
    uint8_t tag = address >> 4;
    int cacheIndex = static_cast<unsigned int>(extractBits(address));
    int blockIndex = static_cast<unsigned int>(address & 0x03);

    // Check if the cache line at 'cacheIndex' has the same tag and a valid state (non-zero)
    if (this->caches[cacheIndex][1] == tag && this->caches[cacheIndex][0] != 0) {
        // Cache hit - set the busRd signal for this processor to false
        atomicBus->busRd.at(this->pid) = false;

        // Send a bus request with intent to modify the cache line
        atomicBus->busRdX.at(this->pid) = true;
        atomicBus->busRdXAddr.at(this->pid) = address;

        // Increment the cache hit counter for write operations
        totalCacheHitWrite++;

        // Change the cache coherence state to Modified (M)
        this->caches[cacheIndex][0] = 2;

        // Update the cache line with the new data
        this->caches[cacheIndex][2 + blockIndex] = data;
    } else {
        // Cache miss - set the busRdX signal and busRdXAddr for this processor
        atomicBus->busRdX.at(this->pid) = true;

        // Send a bus request for the missing address
        atomicBus->busRdXAddr.at(this->pid) = address;

        // Cache miss - increment the cache miss counter for write operations
        totalCacheMissWrite++;
    }
}

void Processor::Snooping(AtomicBus *atomicBus, RAM *ram) {
    // Iterate through each bus request
    for (size_t i = 0; i < atomicBus->busRd.size(); i++) {
        // Extract cache indices and tags for bus read and bus write requests
        int snoopedCacheIndexRd = static_cast<unsigned int>(extractBits(atomicBus->busRdAddr[i]));
        int snoopedCacheIndexRdX = static_cast<unsigned int>(extractBits(atomicBus->busRdXAddr[i]));

        uint8_t tagRd = atomicBus->busRdAddr[i] >> 4;
        uint8_t tagRdX = atomicBus->busRdXAddr[i] >> 4;

        // Snooping bus read request
        if (atomicBus->busRd[i]) {
            if (this->caches[snoopedCacheIndexRd][1] == tagRd) {

                // if current state is M, change it to S
                if (this->caches[snoopedCacheIndexRd][0] == 2){
                    this->caches[snoopedCacheIndexRd][0] = 1;

                    // Flush the cache line to RAM
                    std::vector<uint8_t> temp(4);
                    for (size_t i = 0; i < temp.size(); i++) {
                        temp[i] = this->caches[snoopedCacheIndexRd][i + 2];
                    }
                    ram->writeToMem(atomicBus->busRdAddr[i], temp);
                }

                // if current state is S, change it to S
                else if (this->caches[snoopedCacheIndexRd][0] == 1){
                    this->caches[snoopedCacheIndexRd][0] = 1;
                }
            }
        }

        // Snooping bus write request
        if (atomicBus->busRdX[i]) {
            // If the cache line is in Shared (S) state, change to Invalid (I) state
            std::cout << "Snooping processor..." << i << std::endl;
            if (this->caches[snoopedCacheIndexRdX][1] == tagRdX) {

                // if current state is S, change it to I
                if (this->caches[snoopedCacheIndexRdX][0] == 1){
                    this->caches[snoopedCacheIndexRdX][0] = 0;
                }

                // if current stat is M, change it to M and flush
                else if(this->caches[snoopedCacheIndexRdX][0] == 2){
                    this->caches[snoopedCacheIndexRdX][0] = 0;

                    // Flush the cache line to RAM
                    std::vector<uint8_t> temp(4);
                    for (size_t i = 0; i < temp.size(); i++) {
                        temp[i] = this->caches[snoopedCacheIndexRdX][i + 2];
                    }
                    ram->writeToMem(atomicBus->busRdXAddr[i], temp);
                }
            }
        }
    }
}

void Processor::busResponse(const int &c, const uint8_t &address, const uint8_t &data, AtomicBus *atomicBus, RAM *ram){

    uint8_t tag = address >> 4;
    int cacheIndex = static_cast<unsigned int>(extractBits(address));
    int blockIndex = static_cast<unsigned int>(address & 0x03);

    if (c == 0){
        //checking current state
        if (this->caches[cacheIndex][0] == 0){

            //read from main mem
            std::vector<uint8_t> temp = ram->memRead(address);
            for(int i = 2; i < this->caches[cacheIndex].size();i++)
                this->caches[cacheIndex][i] = temp[i-2];

            //changing to state S
            this->caches[cacheIndex][0] = 1;

            //changing the tag
            this->caches[cacheIndex][1] = tag;
        }
    }else if (c == 1){
        //checking current state
        if (this->caches[cacheIndex][0] == 0){

            //read from main mem
            std::vector<uint8_t> temp = ram->memRead(address);
            for(int i = 2; i < this->caches[cacheIndex].size();i++)
                this->caches[cacheIndex][i] = temp[i-2];

            //chaging the state to M
            this->caches[cacheIndex][0] = 2;

            // Update the cache line with the new data
            this->caches[cacheIndex][2 + blockIndex] = data;

            //changing the tag
            this->caches[cacheIndex][1] = tag;
        }
    }
}

uint8_t extractBits(uint8_t value) {
    return (value >> 2) & 0x03;
}