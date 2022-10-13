#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

#include "onegin.h"
#include "stack.h"
#include "logging.h"
#include "assembler.h"
//#include <TXLib.h>

enum CMD_NAMES
{
    CMD_HLT  = 0 ,
    CMD_PUSH = 1 ,
    CMD_POP  = 2 ,
    CMD_ADD  = 3 ,
    CMD_SUB  = 4 ,
    CMD_MUL  = 5 ,
    CMD_DIV  = 6 ,
    CMD_IN   = 7 ,
    CMD_OUT  = 8 ,
    CMD_JUMP = 9 ,
    CMD_JB   = 10,
    CMD_JBE  = 11,
    CMD_JA   = 12,
    CMD_JAE  = 13,
    CMD_JE   = 14,
    CMD_JNE  = 15,
    CMD_NAME = 16,
    CMD_LAB  = 17
};

typedef int elem_t;

const size_t MAX_LEN_CMD    = 30;

const size_t MAX_NUM_LABELS = 30;

const char* INPUT_FILE_NAME = "user_code.txt";

elem_t PutArg(size_t       cmd_code,
              char*        ptr_arg,
              int*         ptr_asm,
              size_t*      ptr_ip,
              label_field* labels);

int HandleRegs(int* ptr_asm, size_t* ptr_ip, char* reg_name);

int UserCodeToASM(type_buf_char*    ptr_user_code,
                  type_buf_structs* ptr_arr_structs,
                  label_field* labels,
                  int* ptr_asm);

int FuncName(char*        ptr_arg,
             int*         ptr_asm,
             size_t*      ptr_ip,
             label_field* labels);

int FuncJump(char*        ptr_arg,
             int*         ptr_asm,
             size_t*      ptr_ip,
             label_field* labels);

int FuncLab(char*        ptr_arg,
            int*         ptr_asm,
            size_t*      ptr_ip,
            label_field* labels);

size_t IdentifyNumLabel(char*        ptr_arg,
                         int*         ptr_asm,
                         size_t*      ptr_ip,
                         label_field* labels);

enum ARG_TYPES
{
    ARG_IMMED = (1 << 5),
    ARG_REG   = (1 << 6),
    ARG_RAM   = (1 << 7)
};

int main()
{
    type_buf_char      user_code         = {NULL, 0, 0};
    type_buf_structs   arr_structs       = {NULL, 0   };

    read_file(INPUT_FILE_NAME, &user_code, &arr_structs);

    int* ptr_asm = NULL;

    ptr_asm = allocate_array(int, user_code.Num_lines * 3);

    Assert(ptr_asm == NULL);

    label_field labels[MAX_NUM_LABELS] = {{87, ""}};

    UserCodeToASM(&user_code, &arr_structs, labels, ptr_asm);

    UserCodeToASM(&user_code, &arr_structs, labels, ptr_asm);

    return 0;
}

int UserCodeToASM(type_buf_char*    ptr_user_code,
                  type_buf_structs* ptr_arr_structs,
                  label_field* labels,
                  int* ptr_asm)
{
    char   cmd[MAX_LEN_CMD] = {};

    size_t read_res = 0;

    size_t cmd_code = 0;

    size_t ip = 0;

    char*  cursor = NULL;

    for (size_t i = 0; i < ptr_user_code->Num_lines; i++)
    {
        cursor = (ptr_arr_structs->Ptr)[i].Loc;

        sscanf(cursor, "%s", cmd);

        cursor += strlen((const char*)cmd);

        log("Command found: %s\n", cmd);

        if (!strcmp(cmd, "push"))
        {
            cmd_code = CMD_PUSH;
        }
        else if (!strcmp(cmd, "jump"))
        {
            cmd_code = CMD_JUMP;

            FuncJump(cursor, ptr_asm, &ip, labels);
        }
        else if (!strcmp(cmd, "name"))
        {
            cmd_code = CMD_NAME;

            FuncName(cursor, ptr_asm, &ip, labels);
        }
        else if (!strcmp(cmd, "lab"))
        {
            cmd_code = CMD_LAB;

            FuncLab(cursor, ptr_asm, &ip, labels);
        }

        if (cmd_code == CMD_PUSH || cmd_code == CMD_POP)
        {
            PutArg(cmd_code, cursor, ptr_asm, &ip, labels);
        }
    }

    for (size_t i = 0; i < ip; i++)
    {
        log("elem of asm: %d\n", ptr_asm[i]);
    }

    return 0;
}

int SkipSpace(char** cursor)
{
    while (**cursor == ' ') {(*cursor)++;}

    return 0;
}

int SearchLabelByName(label_field* labels, char* name)
{
    size_t i = 0;

    while (i < MAX_NUM_LABELS)
    {
        if (!strcmp(labels[i].Name, name))     return i;

        i++;
    }

    print_log(FRAMED, "LABEL ERROR: No such label found");

    return -1;
}

int FuncLab(char*        ptr_arg,
            int*         ptr_asm,
            size_t*      ptr_ip,
            label_field* labels)
{
    size_t num_label = IdentifyNumLabel(ptr_arg, ptr_asm, ptr_ip, labels);

    labels[num_label].Value = *ptr_ip + 1;

    return 0;
}

int FuncName(char*        ptr_arg,
             int*         ptr_asm,
             size_t*      ptr_ip,
             label_field* labels)
{
    log("case name\n");

    SkipSpace(&ptr_arg);

    size_t num_label = -1;

    char label_name[MAX_LEN_LABEL_NAME] = {};

    if (!sscanf(ptr_arg, "%s", label_name))
    {
        print_log(FRAMED, "LABEL ERROR: Unreadable name");

        return -1;
    }

    SkipSpace(&ptr_arg);

    ptr_arg += strlen((const char*)label_name);

    if (!sscanf(ptr_arg, "%d", &num_label))
    {
        print_log(FRAMED, "LABEL ERROR: Unreadable name");

        return -1;
    }

    strcpy(labels[num_label].Name, label_name);

    return 0;
}

int FuncJump(char*        ptr_arg,
             int*         ptr_asm,
             size_t*      ptr_ip,
             label_field* labels)
{
    log("case jump\n");

    size_t num_label = IdentifyNumLabel(ptr_arg, ptr_asm, ptr_ip, labels);

    ptr_asm[(*ptr_ip)++] = CMD_JUMP;

    ptr_asm[(*ptr_ip)++] = labels[num_label].Value;

    return 0;
}

size_t IdentifyNumLabel(char*        ptr_arg,
                         int*         ptr_asm,
                         size_t*      ptr_ip,
                         label_field* labels)
{
    SkipSpace(&ptr_arg);

    size_t num_label = -1;

    if (!isdigit(*ptr_arg))
    {
        char label_name[MAX_LEN_LABEL_NAME] = {};

        if (!sscanf(ptr_arg, "%s", label_name))
        {
            print_log(FRAMED, "LABEL ERROR: Unreadable name");

            return -1;
        }

        num_label = SearchLabelByName(labels, label_name);
    }
    else
    {
        if (!sscanf(ptr_arg, "%d", &num_label))
        {
            print_log(FRAMED, "LABEL ERROR: Unreadable digit");

            return -1;
        }
    }

    Assert(num_label == -1);

    if (num_label == -1)
    {
        return -1;
    }

    return num_label;
}

elem_t PutArg(size_t       cmd_code,
              char*        ptr_arg,
              int*         ptr_asm,
              size_t*      ptr_ip,
              label_field* labels)
{
    Assert(ptr_arg == NULL);

    log("cmd_code: %d\n", cmd_code);

    int  arg         = 0;

    int  read_res    = 0;

    int  reg_num     = 0;

    char reg_name[5] = {};

    SkipSpace(&ptr_arg);

    log("ptr_arg: %p, first sym to scan: %c\n", ptr_arg, *ptr_arg);

    //txDump(ptr_arg);

    read_res = sscanf(ptr_arg, "[%d+%[a-z]]", &arg, reg_name);

    if (!read_res)
        read_res = sscanf(ptr_arg, "[%[a-z]+%d]", &reg_name, &arg);

    if (read_res == 2)
    {
        log("case [d+rcx]\n");

        ptr_asm[(*ptr_ip)++] = cmd_code + ARG_IMMED + ARG_REG + ARG_RAM;

        HandleRegs(ptr_asm, ptr_ip, reg_name);

        ptr_asm[(*ptr_ip)++] = arg;

        return arg;
    }

    read_res = sscanf(ptr_arg, "%d+%[a-z]", &arg, reg_name);

    if (!read_res)
    {
        read_res = sscanf(ptr_arg, "%[a-z]+%d", reg_name, &arg);
    }

    if (read_res == 2)
    {
        log("case d+rcx\n");

        ptr_asm[(*ptr_ip)++] = cmd_code + ARG_IMMED + ARG_REG;

        HandleRegs(ptr_asm, ptr_ip, reg_name);

        ptr_asm[(*ptr_ip)++] = arg;

        return arg;
    }

    read_res = sscanf(ptr_arg, "[%d]", &arg);

    if (read_res == 1)
    {
        log("case [d]\n");

        ptr_asm[(*ptr_ip)++] = cmd_code + ARG_IMMED + ARG_RAM;

        ptr_asm[(*ptr_ip)++] = arg;

        return NULL;
    }

    read_res = sscanf(ptr_arg, "%[a-z]", reg_name);

    if (read_res == 1)
    {
        log("case rcx\n");

        ptr_asm[(*ptr_ip)++] = cmd_code + ARG_REG;

        HandleRegs(ptr_asm, ptr_ip, reg_name);

        return NULL;
    }

    read_res = sscanf(ptr_arg, "%d", &arg);

    if (read_res == 1)
    {
        log("simple-dimple\n");

        ptr_asm[(*ptr_ip)++] = cmd_code + ARG_IMMED;

        ptr_asm[(*ptr_ip)++] = arg;

        return arg;
    }

    print_log(FRAMED, "Syntax Error: Invalid Argument");

    return -1;
}

/*int ReadByFormat(char* mask1, char* mask2, ...)
{
    va_list args;

    va_start (args, format);

    va_arg(args, int);

    va_arg(args, char*);

    read_res = sscanf(ptr_arg, mask1, &arg, reg_name);

    if (!read_res)
    {
        read_res = sscanf(ptr_arg, mask2, &reg_name, &arg);
    }

    fflush(LOG_FILE);

    va_end(args);
}*/

int HandleRegs(int* ptr_asm, size_t* ptr_ip, char* reg_name)
{
    if (reg_name[0] != 'r' || reg_name[2] != 'x')
    {
        print_log(FRAMED, "Syntax Error: name of register is invalid");
    }

    int reg_num = *(reg_name + 1) - 'a' + 1;

    log("reg_num: %d\n\n", reg_num);

    ptr_asm[(*ptr_ip)++] = reg_num;

    return 0;
}

int read_file(const char* filename,
              type_buf_char* ptr_text_buf,
              type_buf_structs* ptr_arr_structs)
{
    FILE* text_file = open_file_rmode(filename);

    if (!text_file)    return -1;

    ptr_text_buf->Size = safe_def_int(get_file_size(text_file) + 1, NULL + 1);

    log("Size of file is: %d bytes.\n", ptr_text_buf->Size);

    ptr_text_buf->Ptr = allocate_array(char, ptr_text_buf->Size);

    text_to_buffer(text_file, ptr_text_buf);

    fclose(text_file);

    make_pointers_to_lines(ptr_text_buf,
                           ptr_arr_structs);

    //txDump(ptr_text_buf->Ptr);

    return 0;
}

int make_pointers_to_lines(type_buf_char* ptr_text_buf,
                           type_buf_structs* ptr_arr_structs)
{
    ptr_text_buf   ->Num_lines = count_lines(ptr_text_buf);

    ptr_arr_structs->Ptr       = allocate_array(type_prop_line , ptr_text_buf->Num_lines);

    ptr_arr_structs->Size      = ptr_text_buf->Num_lines;

    create_array_structs(ptr_text_buf, ptr_arr_structs);
}

FILE* open_file_rmode(const char* filename)
{
    FILE* r_file = fopen(filename, "rb");

    if(r_file == NULL)
    {
        print_log(FRAMED, "File not found");

        log("File %s not found\n", filename);

        return NULL;
    }

    return r_file;
}

int text_to_buffer(FILE* file, type_buf_char* ptr_text_buf)
{
    size_t num_read_sym = fread(ptr_text_buf->Ptr,
                                sizeof(char),
                                ptr_text_buf->Size - 1,
                                file);

    Assert(num_read_sym == 0);

    log("buffer size: %d\n"
        "fread number of symbols: %d\n", ptr_text_buf->Size, num_read_sym);

    if (ptr_text_buf->Size - 1 != num_read_sym)
    {
        printf("Program dies from cringe encoding\n");

        return -1;
    }

    return 0;
}

int create_array_structs(type_buf_char*    ptr_text_buf,
                         type_buf_structs* ptr_arr_structs)
{
    unsigned long int index_line = 0;

    char* ptr_prev_line = ptr_text_buf->Ptr;

    for (size_t i = 0; i < ptr_text_buf->Size; i++)
    {
        if (end_of_line((ptr_text_buf->Ptr)[i]))
        {
            if (!is_line_empty(ptr_prev_line))
            {
                (ptr_arr_structs->Ptr)[index_line] = {ptr_prev_line,
                                                     (ptr_text_buf->Ptr) + i - ptr_prev_line};

                //(ptr_arr_adrs->Ptr)[index_line] = (ptr_arr_structs->Ptr) + index_line;

                index_line++;
            }

            ptr_prev_line = (ptr_text_buf->Ptr) + i + 1;
        }
    }

    return 1;
}

int count_lines(type_buf_char* ptr_text_buf)
{
    Assert(ptr_text_buf == NULL);

    size_t num_lines = 0;

    char* ptr_prev_line = ptr_text_buf->Ptr;

    for(size_t i = 0; i < ptr_text_buf->Size; i++)
    {
        if (*(ptr_text_buf->Ptr + i) == '\n')
        {
            if (!is_line_empty(ptr_prev_line))
            {
                num_lines++;
            }
            ptr_prev_line = ptr_text_buf->Ptr + i + 1;
        }
    }

    if (!is_line_empty(ptr_prev_line))
    {
        num_lines++;
    }

    ptr_text_buf->Num_lines = num_lines;

    log("Num lines: %d\n", num_lines);

    return num_lines;
}

static int is_line_empty(char* ptr_line)
{
    while (!end_of_line(*ptr_line))
    {
        if (isletter(*ptr_line))
        {
            return 0;
        }
        ptr_line++;
    }
    return 1;
}


int get_file_size(FILE* file)
{
    Assert(file == NULL);

    struct stat buf;

    int errcode = fstat(fileno(file), &buf);

    Assert(errcode != NULL);

    printf("File size = %d\n", buf.st_size);

    return buf.st_size;
}

bool isletter(char sym)
{
    if (('a' <= sym && sym <= 'z') ||
        ('A' <= sym && sym <= 'Z') ||
        ('à' <= sym && sym <= 'ÿ') ||
        ('À' <= sym && sym <= 'ß'))
    {
        return true;
    }

    return false;
}

int end_of_line(char sym)
{
    return (sym == '\0' ||
            sym == '\n' ||
            sym == '\r') ?
            true : false;
}

/*int MakeAsmArray(char* asm_arr, )

int name_to_asm(char* name)
{
    if (strcmp(name, "push"))                   return
}*/
