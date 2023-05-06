#pragma once
#include <vector>

class AtomicBus{
    public:
    // the busRd is set to True when the processor issues a read, but the data is not present in the cache, otherwise false;
  // the size should be the same as the processor
  std::vector<bool> busRd;
  std::vector<uint8_t> busRdAddr;

  // the busRdX is set to True when the processor issues a write, telling others that the processor is intent to modify the cache line, otherwise false;
  // the size should be the same as the processor
  std::vector<bool> busRdX;
  std::vector<uint8_t> busRdXAddr;

  std::vector<bool> shareState;

    //sets the size of busRd and busRdX
    AtomicBus();

    AtomicBus(int numOfProcessors);
};