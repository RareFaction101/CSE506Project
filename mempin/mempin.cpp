#include <string>
#include <cstdint>
#include <vector>
#include <sstream>
#include <cstring>

#include "pin.H"

#include "mempin.h"

int main(int argc, char* argv[]) {
    KNOB<std::string> knob_output_path(
        KNOB_MODE_WRITEONCE,
        "pintool",
        "o",
        "mempin_trace.txt",
        "The output path of trace file"
    );
    int pin_err_code = PIN_Init(argc, argv);
    if (pin_err_code) {
        LOG(
            (std::ostringstream() << "main: pin got incorrect arguments, exit with " << pin_err_code << '\n').str()
        );
        return pin_err_code;
    }
    LOG("main: pin initialized\n");
    std::string* output_path = new std::string(knob_output_path.Value());
    std::vector<mempin::MemRecord>* records = new std::vector<mempin::MemRecord>();
    mempin::OutputTraceArg* output_trace_arg = new mempin::OutputTraceArg {output_path, records};
    INS_AddInstrumentFunction(mempin::instruct_callpack, records);
    PIN_AddFiniFunction(mempin::instruct_finish_callback, output_trace_arg);
    LOG("main: program will start\n");
    PIN_StartProgram();
    return 0;
}

namespace mempin {

VOID instruct_callpack(INS inst, VOID* data) {
    UINT32 mem_op_count = INS_MemoryOperandCount(inst);
    for (UINT32 mem_op_i = 0; mem_op_i < mem_op_count; ++mem_op_i) {
        if (INS_MemoryOperandIsRead(inst, mem_op_i))
            INS_InsertPredicatedCall(
                inst, IPOINT_BEFORE, 
                reinterpret_cast<AFUNPTR>(instruct_mem_callpack),
                IARG_MEMORYOP_EA, mem_op_i,
                IARG_MEMORYOP_SIZE, mem_op_i,
                IARG_BOOL, static_cast<BOOL>(false),
                IARG_PTR, data,
                IARG_END
            );
        if (INS_MemoryOperandIsWritten(inst, mem_op_i))
            INS_InsertPredicatedCall(
                inst, IPOINT_BEFORE, 
                reinterpret_cast<AFUNPTR>(instruct_mem_callpack),
                IARG_MEMORYOP_EA, mem_op_i,
                IARG_MEMORYOP_SIZE, mem_op_i,
                IARG_BOOL, static_cast<BOOL>(true),
                IARG_PTR, data,
                IARG_END
            );
    }
}

VOID instruct_mem_callpack(
    ADDRINT addr,
    UINT32 size,
    BOOL write_en,
    VOID* data
) {
    OS_THREAD_ID tid = PIN_GetTid();
    int cpu = get_cpu();
    std::vector<MemRecord>& records = 
        *(reinterpret_cast<std::vector<MemRecord>*>(data));
    records.push_back({
        addr_align_8(reinterpret_cast<void*>(addr)),
        static_cast<int>(tid),
        cpu,
        write_en
    });
}

VOID instruct_finish_callback(INT32 code, VOID* data) {
    LOG(
        (std::ostringstream() << "instruct_finish_callback: program exit with code " << code << '\n').str()
    );
    OutputTraceArg* output_trace_arg = (reinterpret_cast<OutputTraceArg*>(data));
    std::string str = mem_records_to_str(*(output_trace_arg->records));
    LOG("instruct_finish_callback: writing record to file\n");
    write_file(output_trace_arg->path->c_str(), str);
    delete output_trace_arg->path;
    delete output_trace_arg->records;
    delete output_trace_arg;
    LOG("instruct_finish_callback: wrote trace file, exit\n");
}

std::string mem_records_to_str(const std::vector<MemRecord>& records) {
    LOG("mem_records_to_str: start converting mem record to string\n");
    std::ostringstream ss;
    for (size_t i = 0, len = records.size(); i < len; ++i) {
        const MemRecord& rec = records[i];
        ss  << rec.cpu << ' '
            << (rec.write_en ? '1' : '0') << ' '
            << reinterpret_cast<uintptr_t>(rec.addr) << ' '
            << '0';
        if (i != len - 1)
            ss << '\n';
    }
    return ss.str();
}

void* addr_align_8(void* addr) {
    uintptr_t addr_int = reinterpret_cast<uintptr_t>(addr);
    addr_int >>= 3; 
    addr_int <<= 3;
    return reinterpret_cast<void*>(addr_int);
}

int get_cpu() {
    std::string cpu_info = read_whole_file("/proc/cpuinfo");
    // 0: key; 
    // 1: tabs before colon; 2: spaces after colon;
    // 3: value
    int status = 0;  
    size_t key_start = 0, key_end = 0;
    size_t value_start = 0;
    for (size_t i = 0, len = cpu_info.size(); i <= len; ++i) {
        if (status == 0 && cpu_info[i] == '\t') {
            key_end = i;
            status = 1;
        }
        else if (status == 1 && cpu_info[i] == ':')
            status = 2;
        else if (status == 2 && cpu_info[i] != ' ') {
            value_start = i;
            status = 3;
        }
        else if (status == 3 && (cpu_info[i] == '\n' || cpu_info[i] == '\0')) {
            if (cpu_info.compare(key_start, key_end - key_start, "processor"))
                return atoi(&cpu_info[value_start]);
            status = 0;
        }
    }
    return 0;
}

std::string read_whole_file(const char* path) {
    const USIZE SIZE_STEP = 1024;
    NATIVE_FD fd;
    USIZE total_size = 0;
    USIZE size;
    std::string str;
    OS_RETURN_CODE err_code;
    err_code = OS_OpenFD(path, OS_FILE_OPEN_TYPE_READ, 0, &fd);
    if (err_code.generic_err == OS_RETURN_CODE_FILE_QUERY_FAILED) {
        LOG("read_whole_file: fail to open file\n");
        return str;
    }
    do {
        str.resize(total_size + SIZE_STEP);
        size = SIZE_STEP;
        err_code = OS_ReadFD(fd, &size, &str[total_size]);
        if (err_code.generic_err == OS_RETURN_CODE_FILE_READ_FAILED) {
            LOG("read_whole_file: fail to read file\n");
            break;
        }
        total_size += size;
    } while (size == SIZE_STEP);
    OS_CloseFD(fd);
    str.resize(total_size);
    return str;
}

void write_file(const char* path, const std::string& data) {
    NATIVE_FD fd;
    OS_RETURN_CODE err_code;
    err_code = OS_OpenFD(
        path,
        OS_FILE_OPEN_TYPE_WRITE | OS_FILE_OPEN_TYPE_CREATE,
        (
            OS_FILE_PERMISSION_TYPE_WRITE_USER | OS_FILE_PERMISSION_TYPE_READ_USER | 
            OS_FILE_PERMISSION_TYPE_WRITE_GROUP | OS_FILE_PERMISSION_TYPE_READ_GROUP |
            OS_FILE_PERMISSION_TYPE_READ_OTHERS
        ),
        &fd
    );
    if (err_code.generic_err == OS_RETURN_CODE_FILE_QUERY_FAILED) {
        LOG("write_file: fail to open file\n");
        return;
    }
    USIZE size = data.size();
    err_code = OS_WriteFD(fd, data.c_str(), &size);
    OS_CloseFD(fd);
    if (err_code.generic_err == OS_RETURN_CODE_FILE_WRITE_FAILED)
        LOG("write_file: fail to read file\n");
}

} // namespace mempin