#ifndef ONEGIN_H_INCLUDED
#define ONEGIN_H_INCLUDED

//#define TX_COMPILED

#include <stdio.h>
//#include <TXLib.h>
#include <string.h>
#include <stdlib.h>

//#include "text_macros.h"

const int MAX_LEN_LINE = 30;

typedef struct prop_line
{
    char* Loc;
    size_t Len;
}
type_prop_line;

typedef struct buffer_char
{
    char* Ptr; //  ptr_text
    size_t Size;
    unsigned long int Num_lines;
}
type_buf_char;

typedef struct buffer_ptrs
{
    type_prop_line** Ptr;
    size_t Size;
}
type_buf_ptrs;

typedef struct buffer_structs
{
    type_prop_line* Ptr;
    size_t Size;
}
type_buf_structs;

enum TYPE_SORT
{
    MERGE = 0,
    QSORT = 1
};

int handle_cmd_args(int argc, char** argv, TYPE_SORT* ptr_type_sort);

int read_file(const char* filename,
              type_buf_char* ptr_text_buf,
              type_buf_structs* ptr_arr_structs);

int text_to_buffer(FILE* file, type_buf_char* ptr_text_buf);

FILE* open_file_rmode(const char* filename);

int make_pointers_to_lines(type_buf_char* ptr_text_buf,
                           type_buf_structs* ptr_arr_structs);

int create_array_structs(type_buf_char*    ptr_text_buf,
                     type_buf_structs* ptr_arr_structs);

int get_file_size(FILE* file);

int count_lines(type_buf_char* ptr_text_buf);

bool isletter(char sym);

int test_comparison();

int str_compare(type_prop_line* ptr_line1, type_prop_line* ptr_line2);

int str_compare_reverse(type_prop_line* ptr_line1,
                        type_prop_line* ptr_line2);

int sort_text(type_buf_ptrs* ptr_arr_structs,
              TYPE_SORT type_sort,
              int (*comparator)(const void* line1, const void* line2));

int merge_sort_all(type_buf_ptrs* ptr_arr_structs,
                   int (*comparator)(const void* line1, const void* line2));

int single_merge(type_buf_ptrs* arr,
                 type_buf_ptrs* temp,
                 type_prop_line** ptr_start,
                 type_prop_line** ptr_end,
                 int (*comparator)(const void* line1, const void* line2));

int Merge(type_buf_ptrs* arr,
          type_buf_ptrs* temp,
          type_prop_line** ptr1,
          type_prop_line** ptr2,
          type_prop_line** ptr2_end,
          int (*comparator)(const void* line1, const void* line2));

int print_arr_ptrs(type_buf_structs* ptr_arr_structs);

int sort_and_write(char* filename,
               type_buf_char* ptr_text_buf,
               type_buf_structs* ptr_arr_structs,
               type_buf_ptrs* ptr_arr_adrs);

FILE* open_Wfile(char* filename);

int putting_buf_text_to_file(type_buf_ptrs* ptr_arr_structs, FILE* w_file);

int put_line(FILE* w_file, type_prop_line* ptr_line);

int put_buffer(FILE* w_file, type_buf_structs* ptr_arr_structs);

static int is_line_empty(char* ptr_line);

int comparator_straight(const void* line1, const void* line2);

int comparator_reverse(const void* line1, const void* line2);

int print_help(char* filename);

int end_of_line(char sym);

#endif // ONEGIN_H_INCLUDED
