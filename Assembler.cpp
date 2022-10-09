#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "onegin.h"
#include "stack.h"
#include "logging.h"
#include "assembler.h"
//#include <TXLib.h>

typedef int elem_t;

const size_t MAX_LEN_CMD = 30;

const char* INPUT_FILE_NAME = "user_code.txt";

int main()
{
    type_buf_char      user_code         = {NULL, 0, 0};
    type_buf_structs   arr_structs       = {NULL, 0   };

    read_file(INPUT_FILE_NAME, &user_code, &arr_structs);

    char* ptr_asm_code = NULL;

    ptr_asm_code = allocate_array(char, user_code.Num_lines * 2 * sizeof(int));

    Assert(ptr_asm_code == NULL);

    char* cmd[MAX_LEN_CMD] = {};

    for (size_t i = 0; i < user_code.Num_lines; i++)
    {
        sscanf((arr_structs.Ptr)[i].Loc, " %s", cmd);

        log("Command found: %s\n", cmd);
    }

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
