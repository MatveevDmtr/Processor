#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <string.h>
#include <sys\stat.h>
#include <ctype.h>
#include <stdio.h>
#include "stack.h"
#include "Cpu_Config.h"
#include "logging.h"

#define DEF_CMD(name, num, arg, ...) \
    case num:                        \
    __VA_ARGS__                      \
    break;


const int CMD_MASK = 31; /*11111*/

const size_t MAX_LEN_SIGN = 3;

enum CPU_ERRORS
{
    WRONG_SIGNATURE   = 1,
    UNDEFINED_CMD     = 2,
    ZERO_DIVISION     = 3,
    INVALID_REG_INDEX = 4,
    INVALID_MEM_INDEX = 5
};


typedef struct CPU_struct
{
    int*        code;
    int*        regs;
    int*        RAM;
    const char* signature;
    size_t      ip;
    size_t      Size;
} CPU;

int  Execute(CPU* cpu, stack_t* stk);

void SkipNewLines();

void PushArg(CPU* cpu, stack_t* stk);

int PopArg(CPU* cpu, stack_t* stk);

void CpuCtor(CPU* cpu);

int  checkSign(CPU* cpu, FILE* a_code);

int  getCode(CPU* cpu);

FILE* open_file_rmode(const char* filename);

int Run();

#endif //guard
