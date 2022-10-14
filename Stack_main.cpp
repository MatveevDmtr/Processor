#ifndef STACK_MAIN_CPP_INCLUDED
#define STACK_MAIN_CPP_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <TXLib.h>

#include "logging.h"
#include "StackConfig.h"
#include "stack.h"
#include "GetPoison.h"
#include "specificators.h"




#ifndef STACK_MODE

    #define STACK_MODE USER_MODE

#endif

enum ERRCODES
{
     OK                 , // 0
     SEGFAULT           , // 1
     ZOMBIE             , // 2
     NULLPTR            , // 3
     PTRPOISONED        , // 4
     SIZEPOISONED       , // 5
     NEGCAP             , // 6
     CAPPOISONED        , // 7
     LBIRDSTACK         , // 8
     RBIRDSTACK         , // 9
     LBIRD_STACK_NULLPTR, // 10
     RBIRD_STACK_NULLPTR, // 11
     LBIRDSTRUCT        , // 12
     RBIRDSTRUCT        , // 13
     INV_STACK_HASH     , // 14
     INV_STRUCT_HASH    , // 15
     FILEERROR          , // 16
     DEBUGINFOERROR     , // 17
     REALLOCERROR       , // 18
     STACKOVERFLOW      , // 19
     CONSTR_ERROR       , // 20
     DESTR_ERROR        , // 21
     DECODEERROR          // 22
};

/*int main3()
{
    log("Log file opened\n");

    fprintf(stderr, "Startuem\n");

    stack_t stk1 = StructStackInit(stk1);

    log("Stk1 initialized\n");

    StackDump(&stk1);

    StackCtor(&stk1);

    StackDump(&stk1);

    log("Stack created\n");

    for (size_t i = 0; i < 100; i++)
    {
        StackPush(&stk1, i);

        if (i % 10 == 0)
        {
            StackDump(&stk1);
        }
    }

    for (size_t i = 0; i < 100; i++)
    {
        StackPop(&stk1);

        if (i % 10 == 0)
        {
            StackDump(&stk1);
        }
    }

    StackPop(&stk1);

    StackDump(&stk1);

    StackDtor(&stk1);
}
*/

stack_t StructureStackInit(const char* name,
                           const char* func,
                           const char* file,
                           int line)
{
    elem_t*            Ptr             = NULL;
    size_t             Size            = 0;
    size_t             Capacity        = 0;

    UnsignedLL*        PtrStackCannary = NULL;

    stack_t temp_stk = {
        .StructLeftCannary = LEFT_CANNARY,
        .Ptr               = getPoison(Ptr),
        .Size              = getPoison(Size),
        .Capacity          = getPoison(Capacity),
        .DeadInside        = 1,
        .PtrStackLeftBird  = getPoison(PtrStackCannary),
        .PtrStackRightBird = getPoison(PtrStackCannary),

    #if STACK_MODE >= HASH_MODE
        .StackHashSum         = 0,
        .StructHashSum        = 0,
    #endif
    };

#if STACK_MODE >= HARDDEBUG_MODE
    temp_stk.debug_info.Orig_name    = name;
    temp_stk.debug_info.Func_calling = func;
    temp_stk.debug_info.File_calling = file;
    temp_stk.debug_info.Line_created = line;
#endif

    temp_stk.StructRightCannary  = RIGHT_CANNARY;

    return temp_stk;
}

UnsignedLL FindErrors(stack_t* stk)
{
    UnsignedLL sum_errcodes = 0;

    if (stk == NULL || stk == getPoison(stk))           sum_errcodes += 1 << SEGFAULT;
    if (stk->DeadInside)                                sum_errcodes += 1 << ZOMBIE;
    if (stk->Ptr <= NULL)                               sum_errcodes += 1 << NULLPTR;
    if (stk->Ptr == (elem_t*)getPoison(stk->Ptr))       sum_errcodes += 1 << PTRPOISONED;
    if (stk->Size == getPoison(stk->Size))              sum_errcodes += 1 << SIZEPOISONED;
    if (stk->Capacity <= 0)                             sum_errcodes += 1 << NEGCAP;
    if (stk->Capacity == getPoison(stk->Capacity))      sum_errcodes += 1 << CAPPOISONED;
    if (stk->StructLeftCannary  != LEFT_CANNARY)        sum_errcodes += 1 << LBIRDSTRUCT;
    if (stk->StructRightCannary != RIGHT_CANNARY)       sum_errcodes += 1 << RBIRDSTRUCT;

    if (stk->PtrStackLeftBird   == NULL ||
        stk->PtrStackLeftBird   == getPoison(stk->PtrStackLeftBird))
    {
                                                        sum_errcodes += 1 << LBIRD_STACK_NULLPTR;
    }
    else if (*stk->PtrStackLeftBird  != LEFT_CANNARY)   sum_errcodes += 1 << LBIRDSTACK;

    if (stk->PtrStackRightBird  == NULL ||
        stk->PtrStackRightBird  == getPoison(stk->PtrStackRightBird))
    {
                                                        sum_errcodes += 1 << RBIRD_STACK_NULLPTR;
    }
    else if (*stk->PtrStackRightBird != RIGHT_CANNARY)  sum_errcodes += 1 << RBIRDSTACK;

    if (stk->Size > stk->Capacity)                      sum_errcodes += 1 << STACKOVERFLOW;

#if STACK_MODE >= HASH_MODE
    if (!(sum_errcodes & (1 << NEGCAP)) && !(sum_errcodes & (1 << CAPPOISONED)))
    {
        UnsignedLL stack_hash_sum = CalculateGNUHash(stk->PtrStackLeftBird,
                                                     stk->Capacity * sizeof(elem_t) + 2 * sizeof(bird_t));

        if (stack_hash_sum != stk->StackHashSum)
        {
            sum_errcodes += 1 << INV_STACK_HASH;
        }
        if (!stack_hash_sum)
        {
            stk->StackHashSum = stack_hash_sum;
        }
    }

    UnsignedLL prev_struct_hash_sum = stk->StructHashSum;

    stk->StructHashSum              = 0;

    UnsignedLL struct_hash_sum      = CalculateGNUHash(stk, sizeof(stack_t));

    if (struct_hash_sum != prev_struct_hash_sum)
    {
        sum_errcodes += 1 << INV_STRUCT_HASH;
    }

    if (struct_hash_sum > 0)
    {
        stk->StructHashSum = struct_hash_sum;
    }
    else
    {
        stk->StructHashSum = prev_struct_hash_sum;
    }
#endif

    if (sum_errcodes == 0)
    {
         sum_errcodes = (1 << OK);
    }

    return sum_errcodes;
}

int DecodeErrors(UnsignedLL sum_errcodes)
{
    if (sum_errcodes <= 0)
    {
        print_log(FRAMED, "DECODE ERROR: Invalid sum of error codes");

        return DECODEERROR;
    }
    size_t max_errcode = (size_t) floor(log2 ((double)sum_errcodes));

    for (size_t errcode = 0; errcode <= max_errcode; errcode++)
    {
        if (sum_errcodes & (1 << errcode))
        {
            LogError(errcode);
        }
    }

    return 0;
}

int LogCritError(int errcode, const char* func, int line)
{
    switch (errcode)
    {
        case OK:
            //print_log(FRAMED, "Everything OK: Stack is all right and can be used");
            break;

        case SEGFAULT:
            print_crit_errors("SEGMENTATION FAULT: Invalid Pointer to Structure of Stack", func, line);
            break;

        case ZOMBIE:
            print_crit_errors("DEADINSIDE ERROR: Stack doesn't exist", func, line);
            break;

        case NULLPTR:
            print_crit_errors("POINTER ERROR: Stack Pointer (pointer to buffer) is NULL", func, line);
            break;

        case PTRPOISONED:
            print_crit_errors("POINTER ERROR: Invalid Stack Pointer (pointer to buffer)", func, line);
            break;

        case SIZEPOISONED:
            print_crit_errors("SIZE ERROR: Stack Size is poisoned", func, line);
            break;

        case NEGCAP:
            print_crit_errors("CAPACITY ERROR: Stack Capacity has a Negative Value", func, line);
            break;

        case CAPPOISONED:
            print_crit_errors("CAPACITY ERROR: Stack Capacity is poisoned", func, line);
            break;

        case LBIRDSTACK:
            print_crit_errors("CANNARY ERROR: Left Stack Cannary is invalid", func, line);
            break;

        case RBIRDSTACK:
            print_crit_errors("CANNARY ERROR: Right Stack Cannary is invalid", func, line);
            break;

        case LBIRD_STACK_NULLPTR:
            print_crit_errors("CANNARY ERROR: Ptr to Left Stack Cannary is invalid", func, line);
            break;

        case RBIRD_STACK_NULLPTR:
            print_crit_errors("CANNARY ERROR: Ptr to Right Stack Cannary is invalid", func, line);
            break;

        case LBIRDSTRUCT:
            print_crit_errors("CANNARY ERROR: Left Structure Cannary is invalid", func, line);
            break;

        case RBIRDSTRUCT:
            print_crit_errors("CANNARY ERROR: Right Structure Cannary is invalid", func, line);
            break;

        case INV_STACK_HASH:
            print_crit_errors("HASH ERROR: Stack HashSum is invalid", func, line);
            break;

        case INV_STRUCT_HASH:
            print_crit_errors("HASH ERROR: Structure HashSum is invalid", func, line);
            break;

        case STACKOVERFLOW:
            print_crit_errors("STACK OVERFLOW ERROR: Size of Stack is bigger than its Capacity", func, line);
            break;

        case CONSTR_ERROR:
            print_crit_errors("CONSTRUCTION ERROR: Trying to Construct an existing stack", func, line);
            break;

        case DESTR_ERROR:
            print_crit_errors("DESTRUCTION ERROR: Trying to Destruct a Dead stack", func, line);
            break;

        default:
            print_crit_errors("DECODE ERROR: Unexpected Error Code", func, line);
            return 1;
    }

    return 0;
}

UnsignedLL StackVerify(stack_t* stk)
{
    UnsignedLL sum_errcodes = FindErrors(stk);

    if (sum_errcodes < 1)
    {
        print_log(FRAMED, "SEARCH OF ERRORS ERROR: Invalid Sum of Error Codes");

        return -1;
    }

    DecodeErrors(sum_errcodes);

    return sum_errcodes;
}

#if STACK_MODE >= HARDDEBUG_MODE
void DumpEmExit()
{
    log("\n---------- EMERGENCY FINISH DUMP ----------\n\n");
}

int FuckingDump(stack_t* stk,
                const char* funcname,
                const char* filename,
                int         line)
{
    log("\n\n++++++++++ Start Dump ++++++++++\n");

    if (stk == NULL)
    {
        LogError(SEGFAULT);

        DumpEmExit();

        return SEGFAULT;
    }

    if (funcname == NULL || filename == NULL || line == 0)
    {
        log("STACKDUMP CALL ERROR: StackDump can't recognize parameters of call\n");
    }
    else
    {
        log("\n%s at %s (line %d):\n", funcname, filename, line);
    }

    log("Stack [0x%p] ", stk->Ptr);

    UnsignedLL sum_errcodes = FindErrors(stk);

    bool is_ok = (sum_errcodes == 1);

    if (is_ok)
    {
        print_log(FRAMED, "OK");
    }
    else
    {
        print_log(FRAMED, "ERROR");
    }

    log("Originally '%s' from %s at %s (line %d): \n\n",
                       stk->debug_info.Orig_name,
                       stk->debug_info.Func_calling,
                       stk->debug_info.File_calling,
                       stk->debug_info.Line_created);

    if (stk->PtrStackLeftBird   == NULL ||
        stk->PtrStackLeftBird   == getPoison(stk->PtrStackLeftBird))
    {
        LogError(LBIRD_STACK_NULLPTR);
    }
    else if (*stk->PtrStackLeftBird  != LEFT_CANNARY)
    {
        LogError(LBIRDSTACK);

        log("INVALID StackLeftCannary:     %x\n", *stk->PtrStackLeftBird);
    }

    if (stk->PtrStackRightBird  == NULL ||
        stk->PtrStackRightBird  == getPoison(stk->PtrStackRightBird))
    {
        LogError(RBIRD_STACK_NULLPTR);
    }
    else if (*stk->PtrStackRightBird != RIGHT_CANNARY)
    {
        LogError(RBIRDSTACK);

        log("INVALID StackRightCannary:     %x\n", *stk->PtrStackRightBird);
    }

    log("Pointer:              %p\n"
        "Size:                 %d\n"
        "Capacity:             %d\n"
        "DeadInside:           %d\n"
        "PtrStackLeftCannary:  %p\n"
        "PtrStackRightCannary: %p\n"
        "StackHashSum:         %d\n"
        "StructHashSum:        %d\n\n",
        stk->Ptr,
        stk->Size,
        stk->Capacity,
        stk->DeadInside,
        stk->PtrStackLeftBird,
        stk->PtrStackRightBird,
        stk->StackHashSum,
        stk->StructHashSum);

    if (is_ok)
    {
        log("StackLeftCannary:      %x\n", *stk->PtrStackLeftBird);

        log("StackRightCannary:     %x\n", *stk->PtrStackRightBird);
    }

    if (stk->Capacity <= 0 || stk->Capacity == getPoison(stk->Capacity))
    {
        LogError(NEGCAP);

        DumpEmExit();

        return CAPPOISONED;
    }

    if (stk->Ptr == NULL || stk->Ptr == getPoison(stk->Ptr))
    {
        log("POINTER ERROR: Bad Stack Pointer (pointer to buffer)");

        DumpEmExit();

        return PTRPOISONED;
    }

    log("{\n");

    size_t numbers_in_capacity = (size_t) ceil(log10 ((double) stk->Capacity));

    elem_t curr_elem = 0;

    const char* specif1 = specificator1(stk->Ptr[0]);
    const char* specif2 = specificator2(stk->Ptr[0]);

    const char* null_specif1 = specificator1(INT_CONST);
    const char* null_specif2 = specificator2(INT_CONST);

    for (size_t i = 0; i < stk->Capacity; i++)
    {
        curr_elem = (stk->Ptr)[i];

        log("\t data[%0*d] = ", numbers_in_capacity, i);

        if (curr_elem != NULL)
        {
            log(specif1, curr_elem);
            log(" = ");
            log(specif2, curr_elem);
        }
        else
        {
            log(null_specif1, curr_elem);
            log(" = ");
            log(null_specif2, curr_elem);
        }
        if (curr_elem == getPoison(curr_elem))
        {
            log(" (POISONED)");
        }
        log("\n");
    }

    log("}\n\n");

    log("\n---------- Finish Dump ----------\n\n");

    return 0;
};
#endif

#if STACK_MODE >= HASH_MODE
UnsignedLL CalculateGNUHash(void* start_ptr, size_t num_bytes)
{
    if (start_ptr == NULL || start_ptr == getPoison(start_ptr))
    {
        log("POINTER ERROR: Invalid start pointer for Calculating HashSum\n");

        return NULLPTR;
    }

    UnsignedLL hash_sum = START_HASH;
    for (size_t i = 0; i < num_bytes; i++)
    {
        hash_sum = hash_sum * 33 + ((char*)start_ptr)[i];
    }

    return hash_sum;
}

static int UpdateHash(stack_t* stk)
{
    if (stk == NULL || stk == getPoison(stk))
    {
        LogError(SEGFAULT);

        return SEGFAULT;
    }

    stk->StackHashSum     = CalculateGNUHash(stk->PtrStackLeftBird,
                                             stk->Capacity * sizeof(elem_t) + 2 * sizeof(bird_t));
    stk->StructHashSum    = 0;
    stk->StructHashSum    = CalculateGNUHash(stk, sizeof(stack_t));

    return 0;
}
#endif


static int StackResize(stack_t* stk, size_t new_capacity)
{
    if (StackVerify(stk) > 1)
    {
        log("Trying to Resize an invalid stack\n");

        return getPoison(INT_CONST);
    }

    char* temp_ptr = (char*) realloc((char*)stk->PtrStackLeftBird, new_capacity * sizeof(elem_t) + 2 * sizeof(bird_t));

    if (temp_ptr == NULL)
    {
        print_log(FRAMED, "REALLOCATION ERROR: Can't Find Such Amount of Dynamic Memory");

        return REALLOCERROR;
    }

    stk->PtrStackLeftBird  = (bird_t*) temp_ptr;

    stk->Ptr               = (elem_t*)(temp_ptr + sizeof(bird_t));

    stk->PtrStackRightBird = (bird_t*)(temp_ptr + sizeof(bird_t) + new_capacity * sizeof(elem_t));

    *stk->PtrStackRightBird = RIGHT_CANNARY;

    elem_t poison_for_stack = getPoison(stk->Ptr[0]);

    if (new_capacity > stk->Capacity)
    {
        for (size_t i = stk->Size; i < new_capacity; i++)
        {
            stk->Ptr[i] = poison_for_stack;
        }
    }

    stk->Capacity = new_capacity;

#if STACK_MODE >= HASH_MODE
    UpdateHash(stk);
#endif

    StackVerify(stk);

    return 0;
}

int StackCtor(stack_t* stk)
{
    if (stk == NULL || stk == getPoison(stk))
    {
        LogError(SEGFAULT);

        return SEGFAULT;
    }

    if (!(stk->DeadInside))
    {
        LogError(CONSTR_ERROR);

        return CONSTR_ERROR;
    }

    char* temp_ptr = (char*) calloc(MIN_LEN_STACK * sizeof(elem_t) + 2 * sizeof(bird_t), sizeof(char));

    if (temp_ptr == NULL)
    {
        LogError(REALLOCERROR);

        return REALLOCERROR;
    }

    stk->PtrStackLeftBird   = (bird_t*) temp_ptr;

    stk->Ptr                = (elem_t*)(temp_ptr + sizeof(bird_t));

    stk->PtrStackRightBird  = (bird_t*)(temp_ptr + sizeof(bird_t) + MIN_LEN_STACK * sizeof(elem_t));

    *stk->PtrStackLeftBird  = LEFT_CANNARY;
    *stk->PtrStackRightBird = RIGHT_CANNARY;

    stk->DeadInside         = 0;
    stk->Size               = 0;
    stk->Capacity           = MIN_LEN_STACK;

    stk->StructLeftCannary  = LEFT_CANNARY;
    stk->StructRightCannary = RIGHT_CANNARY;

#if STACK_MODE >= HASH_MODE
    UpdateHash(stk);
#endif

    StackVerify(stk);

    return 0;
}

int StackDtor(stack_t* stk)
{
    if (stk->DeadInside)
    {
        LogError(DESTR_ERROR);

        return DESTR_ERROR;
    }

    if (StackVerify(stk) > 1)
    {
        log("Trying to Destruct an invalid stack\n");

        return getPoison(INT_CONST);
    }

    elem_t poison_for_buf = getPoison(stk->Ptr[0]);

    for (size_t i = 0; i < stk->Capacity; i++)
    {
        (stk->Ptr)[i] = poison_for_buf;
    }

    stk->Ptr           = getPoison(stk->Ptr);
    stk->Size          = getPoison(stk->Size);
    stk->DeadInside    = 1;

#if STACK_MODE >= HASH_MODE
    stk->StackHashSum  = CalculateGNUHash(stk->PtrStackLeftBird,
                                          stk->Capacity * sizeof(elem_t) + 2 * sizeof(bird_t));
#endif

    stk->Capacity      = getPoison(stk->Capacity);

#if STACK_MODE >= HASH_MODE
    stk->StructHashSum = 0;
    stk->StructHashSum = CalculateGNUHash(stk, sizeof(stk));
#endif

    *stk->PtrStackLeftBird  = getPoison(*stk->PtrStackLeftBird);

    stk->StructLeftCannary  = getPoison(stk->StructLeftCannary );
    stk->StructRightCannary = getPoison(stk->StructRightCannary);

    *stk->PtrStackRightBird = getPoison(*stk->PtrStackRightBird);

    free(stk->Ptr);

    return 0;
}

int StackPush(stack_t* stk, elem_t elem)
{
    if (StackVerify(stk) > 1)
    {
        log("Trying to Push to an invalid stack\n");

        return getPoison(INT_CONST);
    }

    if (stk->Size >= stk->Capacity)
    {
        StackResize(stk, stk->Capacity * 2);
    }
    stk->Ptr[stk->Size] = elem;

    stk->Size++;

#if STACK_MODE >= HASH_MODE
    UpdateHash(stk);
#endif

    StackVerify(stk);

    return 0;
}

elem_t StackPop(stack_t* stk)
{
    if (StackVerify(stk) > 1)
    {
        log("Trying to Pop from an invalid stack\n");

        return getPoison(stk->Ptr[0]);
    }

    if (stk->Size < 1)
    {
        log("Trying to Pop from an empty stack (size = 0)\n");

        return getPoison(stk->Ptr[0]);
    }

    elem_t elem = stk->Ptr[stk->Size - 1];

    stk->Ptr[stk->Size - 1] = getPoison(stk->Ptr[0]);

    stk->Size--;

#if STACK_MODE >= HASH_MODE
    UpdateHash(stk);
#endif

    if (stk->Capacity >= 2 * MIN_LEN_STACK && stk->Size <= stk->Capacity / 4)
    {
        log("Resize to %d\n", stk->Capacity / 2);

        StackResize(stk, stk->Capacity / 2);
    }

#if STACK_MODE >= HASH_MODE
    UpdateHash(stk);
#endif

#endif // STACK_MAIN_CPP_INCLUDED

    StackVerify(stk);

    return elem;
}
