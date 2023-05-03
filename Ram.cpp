#include "RAM.h"

RAM::RAM(){
    this->memorySize = 64;
    this->blockSize = 4;

    this->ram.resize(16);
    for (auto ramLine: ram){
        ram.resize(7);
    }

    for (auto ramLine: ram){
        std::fill(ramLine.begin(),ramLine.end(),'0');
    }
}