#include "specificators.h"

//int
const char* specificator1 (int /* value*/)
{
    return "%11d";
}

const char* specificator2 (int /* value*/)
{
    return "%p";
}

//char
const char* specificator1 (char /* value*/)
{
    return "%11d";
}

const char* specificator2 (char /* value*/)
{
    return "%c";
}
