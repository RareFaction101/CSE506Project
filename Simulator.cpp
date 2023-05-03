#include "Simulator.h"
#include <sstream>
#include <iostream>

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
    this->atomicBus = new atomicBus();
}

void Simulator::simulationBegin(const std::string& filename){
    std::vector<int> data(4);
    std::ifstream file(filename);

    if (!file.is_open()){
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    std::string line;
    while (std::getline(file,line)){
        //read inputs
        std::istringstream iss(line);

        for (size_t i = 0; i < 4; ++i){
            iss >> data.at(i);
        }

        int pid = data.at(0);
        int action = data.at(1);
        int address = data.at(2);
        int data = data.at(3);

        // loop through processors, place some of the processor on action queue, and some on snooping queue
        for (const auto & processor: this->processors){
            if(processor->pid == pid)
                ActionQ.push(processor);
            else
                SnoopingQ.push(processor);
        }

        // start processing action queue
        while(!ActionQ.empty()){
            ActionQ.front()->operation(action address,data);
            ActionQ.pop();
        }
        
        // start processing snooping queue
        while(!SnoopingQ.empty()){
            SnoopingQ.front()->operation(action address,data);
            SnoopingQ.pop();
        }

        // snoopi

        //clear data vector
        data.clear();
    }

    file.close();
}

void Simulator::report(){
    for (const auto& processor: Processors){
        std::cout << "Processor: " << processor->pid << "\n";

        std::cout << "Cache Report: \n";

        for (const auto& row : processor->cache->caches) {
            for (int i = 0; i < row.size(); i++) {
                if (i == 0)
                    std::cout << "State:";
                else if (i == 1)
                    std::cout << "Tag:";
                else if (i == 2)
                    std::cout << "Data:";
                std::cout << row[i] << ' ';
            }
            std::cout << std::endl;
        }
        std::cout << '\n';
    }
}

int main(){
    Simulator *simulator = new Simulator();
    simulator->report();
}