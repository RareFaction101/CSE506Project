
#include <String>
#include "Processor.h"
#include "Cache.h"
#include <fstream>
#include <Vector>


class Simulator{
  std::vector<Processor> Processors;

  // the busRd is set to True when the processor issues a read, but the data is not present in the cache, otherwise false;
  // the size should be the same as the processor
  std::vector<bool> busRd;

  // the busRdX is set to True when the processor issues a write, telling others that the processor is intent to modify the cache line, otherwise false;
  // the size should be the same as the processor
  std::vector<bool> busRdX;

  // the mode should be either assigned to "MESI" or "MSI"
  std::string mode;

  //default size is 4;
  //default mode is MSI;
  //should also try to initialize the processor and cache
  Simulator();

  //mode should either accept "MESI" or "MSI";
  Simulator(int numOfProcessors, std::string mode);

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