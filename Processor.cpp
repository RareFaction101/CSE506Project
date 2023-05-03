#include "Processor.h"

int Processor::ID = 0;

Processor::Processor(){
    this->pid = this->ID++;
    this->cache = new Cache();
}