#include "Simulator.h"
#include <sstream>
#include <iostream>
#include <string>

Simulator::Simulator(){
    // fix size for processor
    this->Processors.resize(4);

    // cache coherence protocal
    this->mode = "MSI";

    // initialize each individual processor
    for (size_t i = 0; i < this->Processors.size(); ++i) {
        this->Processors[i] = new Processor();
    }

    // initialize RAM
    this->ram = new RAM();

    // initialize atomicBus
    this->atomicBus = new AtomicBus();
}

Simulator::Simulator(int numOfProcessors,
  int cacheSize, int cacheBlockSize, int ramSize, int ramBlockSize){
    // fix size for processor
    this->Processors.resize(numOfProcessors);

    // cache coherence protocal
    this->mode = "MSI";

    // initialize each individual processor
    for (size_t i = 0; i < this->Processors.size(); ++i) {
        this->Processors[i] = new Processor(cacheSize,cacheBlockSize);
    }

    // initialize RAM
    this->ram = new RAM(ramSize,ramBlockSize);

    // initialize atomicBus
    this->atomicBus = new AtomicBus(numOfProcessors);
}

void Simulator::simulationBegin(const std::string& filename){
    std::vector<int> instructions(4);
    std::ifstream file(filename);

    if (!file.is_open()){
        std::cerr << "Error opening file: " << filename << std::endl;
    }
    
    std::string line;
    while (std::getline(file,line)){
        //read inputs
        std::istringstream iss(line);

        for (size_t i = 0; i < 4; ++i){
            iss >> instructions[i];
        }

        int pid = instructions.at(0);
        int action = instructions.at(1);
        int address = instructions.at(2);
        int data = instructions.at(3);

        // loop through processors, place some of the processor on action queue, and some on snooping queue
        for (const auto & processor: this->Processors){
            if(processor->pid == pid){
                RequestQ.push(processor);
                ResponseQ.push(processor);
            }
            else
                SnoopingQ.push(processor);
        }

        // start processing request queue
        while(!RequestQ.empty()){
            RequestQ.front()->operation(action,address,data,this->atomicBus,this->ram);
            RequestQ.pop();
        }
        
        // start processing snooping queue
        while(!SnoopingQ.empty()){
            SnoopingQ.front()->operation(2,address,data,this->atomicBus,this->ram);
            SnoopingQ.pop();
        }

        // start processing response queue
        while(!ResponseQ.empty()){
            ResponseQ.front()->busResponse(action,address,data,this->atomicBus,this->ram);
            ResponseQ.pop();
        }

        //initialize atomicBus
        std::fill(atomicBus->busRd.begin(),atomicBus->busRd.end(),false);
        std::fill(atomicBus->busRdAddr.begin(),atomicBus->busRdAddr.end(),false);
        std::fill(atomicBus->busRdX.begin(),atomicBus->busRdX.end(),false);
        std::fill(atomicBus->busRdXAddr.begin(),atomicBus->busRdXAddr.end(),false);

        //generate report
        report();
    }

    file.close();
}

void Simulator::report(){
    for (const auto& processor: Processors){
        std::cout << "Processor: " << processor->pid << "\n";

        std::cout << "Cache Report: \n";

        for (const auto& row : processor->caches) {
            for (int i = 0; i < row.size(); i++) {
                if (i == 0)
                    std::cout << "State:";
                else if (i == 1)
                    std::cout << "Tag:";
                else if (i == 2)
                    std::cout << "Data:";
                
                std::cout << static_cast<unsigned int>(row[i]) << ' ';
            }
            std::cout << std::endl;
        }
        std::cout << '\n';
    }

    std::cout << "RAM Report: \n";
    for (const auto& ramLine: this->ram->ram){
        for (int i = 0; i < ramLine.size(); i++){
            std::cout << static_cast<unsigned int>(ramLine[i]) << ' ';
        }
        std::cout << std::endl;
    }

    std::cout << "Request report: \n\n";
    int x = 0;
    std::cout << "       BusRd: ";
    for (const auto& rd: this->atomicBus->busRd)
        std::cout << "Pid " << x++ << ":  " << rd << "  ";
    std::cout << std::endl;

    x = 0;
    std::cout << "      BusRdX: ";
    for (const auto& rd: this->atomicBus->busRdX)
        std::cout << "Pid " << x++ << ":  " << rd << "  ";
    std::cout << std::endl;

    x = 0;
    std::cout << "Shared State: ";
    for (const auto& rd: this->atomicBus->shareState)
        std::cout << "Pid " << x++ << ":  " << rd << "  ";
    std::cout << std::endl;

    for (const auto& processor: Processors){
        std::cout << "Processor: " << processor->pid << "\n";

        std::cout << "Cache Hit Count (Read): " << processor->totalCacheHitRead << "\n";
        std::cout << "Cache Hit Miss Count (Read): " << processor->totalCacheMissRead << "\n";

        std::cout << "Cache Hit Count (Write): " << processor->totalCacheHitWrite << "\n";
        std::cout << "Cache Hit Miss Count (Write): " << processor->totalCacheMissWrite << "\n";
    }
}

int main(){
    Simulator *simulator = new Simulator(5,16,4,64,4);
    
    simulator->simulationBegin("test.txt");
}