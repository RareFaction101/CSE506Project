
#include <string>
#include "Processor.h"
#include <fstream>
#include <vector>
#include "RAM.h"
#include "AtomicBus.h"
#include <queue>


class Simulator{
  public:

  std::vector<Processor*> Processors;
  RAM *ram;
  AtomicBus *atomicBus;

  std::queue<Processor*> RequestQ;
  std::queue<Processor*> SnoopingQ;
  std::queue<Processor*> ResponseQ;

  // the mode should be either assigned to "MESI" or "MSI"
  std::string mode;

  //default size is 4;
  //default mode is MSI;
  //should also try to initialize the processor and cache
  Simulator();

  //mode should either accept "MESI" or "MSI";
  Simulator(int numOfProcessors,
  int cacheSize, int cacheBlockSize, int ramSize, int ramBlockSize);

  // this simulation will read a test file using the following format
  //
  // [pid] [operation: "r" or "w"] [6-bits block address] [8-bit data optional] 
  //
  // for example:
  // 0 r 0 <- this means processor 0 issues a read at address 0, address is byte address, so if i see 0, 1, 2, or 3, it will be placed in the first block (block 0 of the RAM), because block 0 contains 4 bytes. Also, the data width is 1 byte long.

  // 1 r 3 4 <- this means processor 1 issues a write of 2 at address 3 (block 0)
  //
  // for now, we don't consider a case where no multiple processors can read or write at once for simplicity.
  // 
  // 
  //
  void simulationBegin(const std::string& filename);

  // this function gives out a report of total cache hit or miss for each individual processor
  // you may add other additional info you think is useful
  void report();
};