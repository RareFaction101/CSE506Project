#include <vector>

class AtomicBus{
    std::vector<bool> BusRd;
    std::vector<bool> BusRdX;

    //sets the size of busRd and busRdX
    AtomicBus();

    // this function takes address as input, returns the data from the main memory
    int memRead(const int &address, const int &RAM);

    // this function write the data to the particular RAM address
    void writeToMem(const int &address, const int &data, const int&RAM);
};