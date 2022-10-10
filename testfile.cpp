#include <stdio.h>


int main1()
{
    int a = 0;

    char line[5] = {};

    int read_res = scanf("[%d+%[a-z]s]", &a, line);

    printf("read_res: %d, a: %d, line: %s", read_res, a, line);
}
