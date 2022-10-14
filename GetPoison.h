#ifndef GETPOISON_H_INCLUDED
#define GETPOISON_H_INCLUDED

#include <math.h>

#include "logging.h"
#include "StackConfig.h"


const size_t BAAD_FOOD = 0xBAADF00D;


//start prototypes
stack_t* getPoison (stack_t* /* value */);

int getPoison (int /* value */);

size_t getPoison (size_t /* value */);

float getPoison (float /* value */);

double getPoison (double /* value */);

char getPoison (char /* value */);

char* getPoison (char* /* value */);

int* getPoison (int* /* value */);

double* getPoison (double* /* value */);

char** getPoison (char** /* value */);

void* getPoison (void* /* value */);

unsigned long long* getPoison (unsigned long long* /* value */);

unsigned long long getPoison (unsigned long long /* value */);
//finish prototypes

#endif
