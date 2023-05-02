#include "Cache.h"
#include "AtomicBus.h"

class Processor{
    int pid;
    Cache cache;
    int totalCacheHitRead = 0;
    int totalCacheMissRead = 0;
    int totalCacheHitWrite = 0;
    int totalCacheMissWrite = 0;

    // set the processor id
    // as well as initializing the cache block
    Processor(int pid);

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

    void operation(const char &c = 's', const int &address = -1, const int &data = -1, const AtomicBus &atomicBus);

};