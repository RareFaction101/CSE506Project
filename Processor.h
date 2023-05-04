#include "Cache.h"
#include "AtomicBus.h"
#include "RAM.h"
#include <cstdint>

class Processor{
    public:

    static int ID;
    int pid;
    int totalCacheHitRead = 0;
    int totalCacheMissRead = 0;
    int totalCacheHitWrite = 0;
    int totalCacheMissWrite = 0;
    std::vector<std::vector<uint8_t>> caches;

    // set the processor id
    // as well as initializing the cache block
    Processor();

    // the processor will either one of the three things below:
    //  read
    //  write
    //  or snooping

    // if the operation is read, it first checks if the cache contains that copy of the data, and
    // increment either the variable totalCacheHitRead or totalCacheMissRead by 1
    // operation write should follows the similar manner

    // if the operation is neither read or write, the default operation will be used which is snooping,
    // it snoops the request made by other processor by checking the atomicBus

    // change the cache state accordingly in here as well.

    // input :
    // c will determine whether the operation is either read or write ('r' or 'w'), the default operation is 's'
    // address is byte address in the physical memory, see more details in cache class
    // data is just 32 bits data

    //expecting 6 bits address
    void operation(const int &c = 2, const uint8_t &address = -1, const uint8_t &data = -1, AtomicBus *atomicBus = nullptr, RAM *ram = nullptr);
};