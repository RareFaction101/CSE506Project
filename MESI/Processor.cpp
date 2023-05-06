#include "Processor.h"
#include <iostream>
#include <cmath>

int Processor::ID = 0;

int computeCacheIndexBits(int cacheSize, int blockSize) {
    int numCacheBlocks = cacheSize / blockSize;
    int numCacheSets = numCacheBlocks;
    int indexBits = static_cast<int>(std::log2(numCacheSets));
    return indexBits;
}
int computeBlockOffsetBits(int blockSize) {
    int offsetBits = static_cast<int>(std::log2(blockSize));
    return offsetBits;
}
int extractCacheIndex(uint64_t address, int cacheSize, int blockSize) {
    int indexBits = computeCacheIndexBits(cacheSize, blockSize);
    int offsetBits = computeBlockOffsetBits(blockSize);

    // Create a mask to extract the cache index bits
    unsigned int indexMask = (1 << indexBits) - 1;

    // Shift the address to the right by the number of offsetBits to remove them
    // Then apply the mask to extract the cache index bits
    return (address >> offsetBits) & indexMask;
}
int extractBlockOffset(uint64_t address, int blockSize) {
    int offsetBits = computeBlockOffsetBits(blockSize);

    // Create a mask to extract the block offset bits
    unsigned int offsetMask = (1 << offsetBits) - 1;

    // Apply the mask to extract the block offset bits from the memory address
    return address & offsetMask;
}
int extractTagBits(uint64_t address, int cacheSize, int blockSize){
    int indexBits = computeCacheIndexBits(cacheSize, blockSize);
    int offsetBits = computeBlockOffsetBits(blockSize);

    // Shift the address to the right by the sum of indexBits and offsetBits to remove them
    // The remaining bits represent the tag bits
    return address >> (indexBits + offsetBits);
}

Processor::Processor() {
    // Assign a unique ID to the processor and increment the static ID counter
    this->pid = this->ID++;

    // Set the cache size and block size
    this->cacheSize = 16;
    this->cacheBlockSize = 4;

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
Processor::Processor(int cacheSize, int cacheBlockSize){
    // Assign a unique ID to the processor and increment the static ID counter
    this->pid = this->ID++;

    // Resize the outer vector (caches) to have 4 elements (cache lines)
    this->cacheSize = cacheSize;
    this->cacheBlockSize = cacheBlockSize;

    int cacheLineSize = ceil((double)this->cacheSize / (double)this->cacheBlockSize);

    this->caches.resize(cacheLineSize);

    // Resize each inner vector (cache line) to have 6 elements (metadata and data)
    for (auto& cache : caches) {
        cache.resize(2+cacheBlockSize);
    }

    // Initialize the cache lines
    for (size_t i = 0; i < this->caches.size(); ++i) {
        std::fill(this->caches[i].begin(), this->caches[i].end(), 0);
    }
}
void Processor::operation(const int &c, const uint64_t &address, const uint8_t &data, AtomicBus *atomicBus, RAM *ram) {
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
void Processor::PrRd(const uint64_t &address, AtomicBus *atomicBus) {
    
    // Extract the tag and cache index from the address
    int cacheIndex = extractCacheIndex(address,this->cacheSize,this->cacheBlockSize);
    int blockIndex = extractBlockOffset(address,this->cacheBlockSize);
    uint64_t tag = extractTagBits(address,this->cacheSize,this->cacheBlockSize);

    // tag hit
    if (this->caches[cacheIndex][1] == tag) {

        // if S state
        if (this->caches[cacheIndex][0] == 1){

            // Change the cache coherence state to Shared (S)
            this->caches[cacheIndex][0] = 1;

            // Increment the cache hit counter for read operations
            totalCacheHitRead++;
        }

        // if M state
        else if (this->caches[cacheIndex][0] == 2){

            // Change the cache coherence state to Modified (M)
            this->caches[cacheIndex][0] = 2;

            // Increment the cache hit counter for read operations
            totalCacheHitRead++;
        }

        // if E state
        else if (this->caches[cacheIndex][0] == 3){

            // Change the cache coherence state to Exclusive (E)
            this->caches[cacheIndex][0] = 3;

            // Increment the cache hit counter for read operations
            totalCacheHitRead++;
        }

        // if I state
        else {

            atomicBus->busRd.at(this->pid) = true;

            // Send a bus request for the missing address
            atomicBus->busRdAddr.at(this->pid) = address;

            totalCacheMissRead++;
        }
    }
    // tag miss 
    else {
        atomicBus->busRd.at(this->pid) = true;

        // Send a bus request for the missing address
        atomicBus->busRdAddr.at(this->pid) = address;

        totalCacheMissRead++;
    }
}
void Processor::PrWr(const uint64_t &address, const uint8_t &data, AtomicBus *atomicBus) {
    // Extract the tag, cache index, and block index from the address
    int cacheIndex = extractCacheIndex(address,this->cacheSize,this->cacheBlockSize);
    int blockIndex = extractBlockOffset(address,this->cacheBlockSize);
    uint64_t tag = extractTagBits(address,this->cacheSize,this->cacheBlockSize);

    // tag hit
    if (this->caches[cacheIndex][1] == tag) {

        // if S state
        if (this->caches[cacheIndex][0] == 1){
            // Change the cache coherence state to Modified (M)
            this->caches[cacheIndex][0] = 2;

            // Send a bus request with intent to modify the cache line
            atomicBus->busRdX.at(this->pid) = true;
            atomicBus->busRdXAddr.at(this->pid) = address;

            // Increment the cache hit counter for write operations
            totalCacheHitWrite++;

            // Update the cache line with the new data
            this->caches[cacheIndex][2 + blockIndex] = data;
        }

        // if M state
        else if (this->caches[cacheIndex][0] == 2){
            // Change the cache coherence state to Modified (M)
            this->caches[cacheIndex][0] = 2;

            // Increment the cache hit counter for write operations
            totalCacheHitWrite++;

            // Update the cache line with the new data
            this->caches[cacheIndex][2 + blockIndex] = data;
        }

        // if E state
        else if (this->caches[cacheIndex][0] == 3){
            // Change the cache coherence state to Modified (M)
            this->caches[cacheIndex][0] = 2;

            // Increment the cache hit counter for write operations
            totalCacheHitWrite++;

            // Update the cache line with the new data
            this->caches[cacheIndex][2 + blockIndex] = data;
        }

        // if I state
        else{
            // Cache miss - set the busRdX signal and busRdXAddr for this processor
            atomicBus->busRdX.at(this->pid) = true;

            // Send a bus request for the missing address
            atomicBus->busRdXAddr.at(this->pid) = address;

            // Cache miss - increment the cache miss counter for write operations
            totalCacheMissWrite++;
        }
    } 

    // tag miss
    else {
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
        int snoopedCacheIndexRd = static_cast<unsigned int>(extractCacheIndex(atomicBus->busRdAddr[i],this->cacheSize,this->cacheBlockSize));
        int snoopedCacheIndexRdX = static_cast<unsigned int>(extractCacheIndex(atomicBus->busRdXAddr[i],this->cacheSize,this->cacheBlockSize));

        uint64_t tagRd = extractTagBits(atomicBus->busRdAddr[i],this->cacheSize,this->cacheBlockSize);
        uint64_t tagRdX = extractTagBits(atomicBus->busRdXAddr[i],this->cacheSize,this->cacheBlockSize);

        // Snooping bus read request
        if (atomicBus->busRd[i]) {

            // tag hit
            if (this->caches[snoopedCacheIndexRd][1] == tagRd) {

                // if current state is M
                if (this->caches[snoopedCacheIndexRd][0] == 2){

                    // Change the cache coherence state to Shared (S)
                    this->caches[snoopedCacheIndexRd][0] = 1;

                    // Flush the cache line to RAM
                    std::vector<uint8_t> temp(this->cacheBlockSize);
                    for (size_t i = 0; i < temp.size(); i++) {
                        temp[i] = this->caches[snoopedCacheIndexRd][i + 2];
                    }
                    ram->writeToMem(atomicBus->busRdAddr[i], temp);
                }

                // if current state is E
                else if (this->caches[snoopedCacheIndexRd][0] == 3){

                    // Change the cache coherence state to Shared (S)
                    this->caches[snoopedCacheIndexRd][0] = 1;
                }

                // if current state is S
                else if (this->caches[snoopedCacheIndexRd][0] == 1){

                    // Change the cache coherence state to Shared (S)
                    this->caches[snoopedCacheIndexRd][0] = 1;
                } 
            }
        }

        // Snooping bus write request
        if (atomicBus->busRdX[i]) {
    
            //tag hit
            if (this->caches[snoopedCacheIndexRdX][1] == tagRdX) {

                // if current state is M
                if (this->caches[snoopedCacheIndexRdX][0] == 2){

                    // Change the cache coherence state to Invalid (I)
                    this->caches[snoopedCacheIndexRdX][0] = 0;

                    // Flush the cache line to RAM
                    std::vector<uint8_t> temp(this->cacheBlockSize);
                    for (size_t i = 0; i < temp.size(); i++) {
                        temp[i] = this->caches[snoopedCacheIndexRdX][i + 2];
                    }
                    ram->writeToMem(atomicBus->busRdXAddr[i], temp);
                }

                // if current state is E
                else if (this->caches[snoopedCacheIndexRdX][0] == 3){

                    // Change the cache coherence state to Invalid (I)
                    this->caches[snoopedCacheIndexRdX][0] = 0;
                }

                // if current state is S
                else if (this->caches[snoopedCacheIndexRdX][0] == 1){

                    // Change the cache coherence state to Invalid (I)
                    this->caches[snoopedCacheIndexRdX][0] = 0;
                }
            }
        }
    }
}
void Processor::busResponse(const int &c, const uint64_t &address, const uint8_t &data, AtomicBus *atomicBus, RAM *ram){

    int cacheIndex = extractCacheIndex(address,this->cacheSize,this->cacheBlockSize);
    int blockIndex = extractBlockOffset(address,this->cacheBlockSize);
    uint64_t tag = extractTagBits(address,this->cacheSize,this->cacheBlockSize);

    if (c == 0){
        //checking current state
        if (this->caches[cacheIndex][0] == 0){
            
            // changing state
            bool assertedCache = std::any_of(
                atomicBus->shareState.begin(), atomicBus->shareState.end(), [](int addr) { return addr == 1; });
            
            this->caches[cacheIndex][0] = (assertedCache) ? 1 : 3;

            //read from main mem
            std::vector<uint8_t> temp = ram->memRead(address);
            for(int i = 2; i < this->caches[cacheIndex].size();i++)
                this->caches[cacheIndex][i] = temp[i-2];

            //changing the tag
            this->caches[cacheIndex][1] = tag;
        }
    }else if (c == 1){
        //checking current state
        if (this->caches[cacheIndex][0] == 0){
            // Change the cache coherence state to Modified (M)
            this->caches[cacheIndex][0] = 2;

            //read from main mem
            std::vector<uint8_t> temp = ram->memRead(address);
            for(int i = 2; i < this->caches[cacheIndex].size();i++)
                this->caches[cacheIndex][i] = temp[i-2];

            // Update the cache line with the new data
            this->caches[cacheIndex][2 + blockIndex] = data;

            //changing the tag
            this->caches[cacheIndex][1] = tag;
        }
    }
}