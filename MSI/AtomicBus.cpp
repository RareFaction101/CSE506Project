#include "AtomicBus.h"

AtomicBus::AtomicBus(){
    busRd.resize(4);
    busRdX.resize(4);
    busRdAddr.resize(4);
    busRdXAddr.resize(4);
}

AtomicBus::AtomicBus(int numOfProcessors){
    busRd.resize(numOfProcessors);
    busRdX.resize(numOfProcessors);
    busRdAddr.resize(numOfProcessors);
    busRdXAddr.resize(numOfProcessors);
}