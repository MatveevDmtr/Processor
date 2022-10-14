#include "cpu.h"

/*static char* CpuErrorNames[] {
    "Wrong signature",
    "Undefined command",
    "Can't divide by zero",
    "Invalid reg index",
    "Invalid RAM index"
};*/


const char* CPU_SIGNATURE  = "MDA";
const char* NAME_FILE_CODE = "ASM.txt";

const size_t RAM_SIZE = 100;

const size_t REG_SIZE = 10;

int Run()
{
    log("\n----------CPU----------\n");

    CPU cpu = {NULL, NULL, NULL, CPU_SIGNATURE, 0, 0};

    stack_t stk1 = StructStackInit(stk1);

    StackCtor(&stk1);

    log("stk1 created\n");

    getCode(&cpu);

    log("Code was read\n");

    if (Execute(&cpu, &stk1))
    {
        print_log(FRAMED, "EXECUTION ERROR");
    }

    log("Finish running\n");

    return 0;
}

int Execute(CPU* cpu, stack_t* stk)
{
    Assert(cpu == NULL);
    Assert(stk == NULL);

    log("ip: %d, Size: %d\n", cpu->ip, cpu->Size);

    while (cpu->ip < cpu->Size)
    {
        log("cmd_code: %d, cmd_code * mask: %d\n", cpu->code[cpu->ip],
            cpu->code[cpu->ip] & CMD_MASK);

        switch(cpu->code[cpu->ip] & CMD_MASK)
        {
            #include "commands.h"

        default:

            print_log(FRAMED, "COMMAND ERROR: Unexpected command");

            log("ERROR: Unexpected command, ip: %zd\n", cpu->ip);

            return UNDEFINED_CMD;
        }

        cpu->ip++;
        StackDump(stk);
    }

    log("end of cycle\n");



    #undef DEF_CMD

    return 0;

}

void SkipNewLines()
{
    char ch = 0;

    while((ch = getchar()) != '\n')            continue;
}

void PushArg(CPU* cpu, stack_t* stk)
{
    log("start push\n");

    size_t cmd_ip = cpu->ip++;

    if (cpu->code[cmd_ip] & ARG_RAM)
    {
        int index = 0;

        if (cpu->code[cmd_ip] & ARG_REG)
        {
            index += cpu->regs[cpu->code[(cpu->ip)++]];
        }

        if (cpu->code[cmd_ip] & ARG_IMMED)
        {
            index += cpu->code[cpu->ip++];
        }

        StackPush(stk, cpu->RAM[index]);
    }
    else
    {
        int arg   = 0;

        if (cpu->code[cmd_ip] & ARG_REG)
        {
            arg += cpu->regs[cpu->code[(cpu->ip)++]];
        }

        if (cpu->code[cmd_ip] & ARG_IMMED)
        {
            arg += cpu->code[cpu->ip++];
        }

        StackPush(stk, arg);
    }

    cpu->ip--;

    //hueta
    /*int shift = 0;

    if (cpu->code[cpu->ip] & ARG_REG)
    {
        log("register in push\n");

        arg += cpu->regs[cpu->code[cpu->ip + 1]];

        shift++;
    }

    if (cpu->code[cpu->ip] & ARG_IMMED)
    {
        log("immediate in push\n");

        arg += cpu->code[cpu->ip+1+shift];

        shift++;
    }

    if (cpu->code[cpu->ip] & ARG_RAM)
    {
        log("ram in push\n");

        arg = cpu->RAM[arg];
    }

    log("arg is %d\n", arg);

    cpu->ip += shift;

    StackPush(stk, arg); */

    log("finish push\n");
}

int PopArg(CPU* cpu, stack_t* stk)
{
    log("in pop\n");

    int arg = StackPop(stk);

    size_t cmd_ip = cpu->ip++;

    int index = 0;

    if (cpu->code[cmd_ip] & ARG_RAM)
    {
        if (cpu->code[cmd_ip] & ARG_REG)
        {
            index += cpu->regs[cpu->code[(cpu->ip)++]];
        }

        if (cpu->code[cmd_ip] & ARG_IMMED)
        {
            index += cpu->code[cpu->ip++];
        }

        cpu->RAM[index] = arg;
    }
    else
    {
        if (cpu->code[cmd_ip] & ARG_REG)
        {
            cpu->regs[cpu->code[(cpu->ip)++]] = arg;
        }

        if (cpu->code[cmd_ip] & ARG_IMMED)
        {
            print_log(FRAMED, "SYNTAX ERROR: Invalid usage of function pop");
        }
    }
    cpu->ip--;

    //hueta

    /*if (cpu->code[ptr_cmd] & ARG_REG)
    {
        index += cpu->code[(cpu->ip)++];
    }

    if (cpu->code[ptr_cmd] & ARG_IMMED)
    {
        index += cpu->code[cpu->ip++];
    }

    if (cpu->code[cpu->ip] & ARG_RAM)
    {
        if (index >= RAM_SIZE)
        {
            print_log(FRAMED, "ERROR: invalid index of RAM");

            return INVALID_MEM_INDEX;
        }

        cpu->RAM[index] = arg;

    }
    else
    {
        if (index >= REG_SIZE)
        {
            printf("ERROR: invalid index of REGS\n");

            exit(INVALID_REG_INDEX);
        }
        cpu->regs[index] = arg;
    }

    cpu->ip += shift;
    */

    log("pop done\n");

}

void CpuCtor(CPU* cpu)
{
    cpu->code = (int*)(calloc(cpu->Size, sizeof(int)));
    Assert(cpu->code == nullptr);

    cpu->RAM = (int*)(calloc(RAM_SIZE, sizeof(int)));
    Assert(cpu->RAM == nullptr);

    cpu->regs = (int*)(calloc(REG_SIZE, sizeof(int)));
    Assert(cpu->regs == nullptr);
}

int checkSign(CPU* cpu, FILE* file_asm)
{
    char asm_sign[MAX_LEN_SIGN] = {};

    int num_read_lines = fscanf(file_asm, "%s", asm_sign);

    log("Lines read: %d\n", num_read_lines);

    log("sign read: %s, sign cpu: %s\n", asm_sign, cpu->signature);

    if (strcmp(asm_sign, cpu->signature))
    {
        print_log(FRAMED, "SIGNATURE ERROR");

        return WRONG_SIGNATURE;
    }

    return 0;
}

int getCode(CPU* cpu)
{
    FILE* file_asm = open_file_rmode(NAME_FILE_CODE);

    Assert(file_asm == NULL);

    if (checkSign(cpu, file_asm))    return WRONG_SIGNATURE;

    fscanf(file_asm, "\n");

    fread(&cpu->Size, sizeof(int), 1, file_asm);

    log("Size already read: %d\n", cpu->Size);

    CpuCtor(cpu);

    fscanf(file_asm, "\n");

    fread(cpu->code, sizeof(int), cpu->Size, file_asm);

    printf("\n");

    fclose(file_asm);

}
