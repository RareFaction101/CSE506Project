#include "Cache.h"
#include "AtomicBus.h"
#include "RAM.h"
#include <cstdint>

class Processor{
    public:

    // Helper variable
    static int ID;
    
    // processor ID
    int pid;

    // Cache Hit Tracker Variables
    int totalCacheHitRead = 0;
    int totalCacheMissRead = 0;
    int totalCacheHitWrite = 0;
    int totalCacheMissWrite = 0;

    // 2D Caches
    std::vector<std::vector<uint8_t>> caches;

    // This constructor initializes a Processor object. It assigns a unique ID to the processor and sets the cache size and block size. The cache is represented as a two-dimensional vector, where the first dimension corresponds to cache lines and the second dimension contains cache coherence state, tag, and data elements. The constructor initializes all elements in the cache to 0.
    Processor();

    // Performs a processor operation based on the value of 'c'.
// c: The operation type (0: read, 1: write, default: snooping)
// address: The memory address for read or write operations (default: -1, unused for snooping)
// data: The data to be written for write operations (default: -1, unused for read and snooping)
// atomicBus: A pointer to an AtomicBus object for read, write, and snooping operations (default: nullptr)
// ram: A pointer to a RAM object for snooping operations (default: nullptr, unused for read and write)
    void operation(const int &c = 2, const uint8_t &address = -1, const uint8_t &data = -1, AtomicBus *atomicBus = nullptr, RAM *ram = nullptr);

    // Performs a read operation on the processor, checking the cache for a cache hit or miss.
// If a cache hit occurs, the busRd signal for the processor is set to false, and the cache hit counter for read operations is incremented.
// If a cache miss occurs, the busRd signal and busRdAddr for the processor are set, and the cache miss counter for read operations is incremented.
// address: The memory address for the read operation
// atomicBus: A pointer to an AtomicBus object for managing bus signals and addresses
    void PrRd(const uint8_t &address = -1, AtomicBus *atomicBus = nullptr);

// Performs a write operation on the processor, checking the cache for a cache hit or miss.
// If a cache hit occurs, the busRd signal for the processor is set to false, the busRdX signal and busRdXAddr are set for the processor,
// the cache hit counter for write operations is incremented, the cache coherence state is changed to Modified (M), and the cache line is updated with the new data.
// If a cache miss occurs, the cache miss counter for write operations is incremented.
// address: The memory address for the write operation
// data: The data to be written
// atomicBus: A pointer to an AtomicBus object for managing bus signals and addresses
    void PrWr(const uint8_t &address = -1, const uint8_t &data = -1, AtomicBus *atomicBus = nullptr);

    void Snooping(AtomicBus *atomicBus = nullptr, RAM *ram = nullptr);

    void busResponse(const int &c = 2, const uint8_t &address = -1, const uint8_t &data = -1, AtomicBus *atomicBus = nullptr, RAM *ram = nullptr);
};