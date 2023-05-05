#ifndef CACHEPIN_H
#define CACHEPIN_H

#include <vector>
#include <string>

#include "pin.H"

namespace mempin {

struct MemRecord {
    void* addr;
    // size_t size;
    int tid;
    int cpu;
    bool write_en;
};

struct OutputTraceArg {
    const std::string* path;
    const std::vector<MemRecord>* records; 
};

void instruct_callpack(INS inst, VOID* data);
VOID instruct_mem_callpack(
    ADDRINT addr,
    UINT32 size,
    BOOL write_en,
    VOID* data
);
VOID instruct_finish_callback(INT32 code, VOID* data);
void* addr_align_8(void* addr);
std::string mem_records_to_str(const std::vector<MemRecord>& records);
int get_cpu();
std::string read_whole_file(const char* path);
void write_file(const char* path, const std::string& data);

}
#endif
