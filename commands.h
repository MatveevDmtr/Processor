#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

DEF_CMD(HLT, 0, 0,
{
    printf("in hlt\n");
    return 0;
})

DEF_CMD(PUSH, 1, 1,
{
   PushArg(cpu, stk);
})

DEF_CMD(POP, 2, 1,
{
    PopArg(cpu, stk);
})

DEF_CMD(ADD, 3, 0,
{
    log("in add\n");

    int a = StackPop(stk);
    int b = StackPop(stk);

    StackPush(stk, a + b);
})

DEF_CMD(SUB, 4, 0,
{
    log("in sub\n");

    int a = StackPop(stk);
    int b = StackPop(stk);

    StackPush(stk, a - b);
})

DEF_CMD(MUL, 5, 0,
{
    log("in mul\n");

    int a = StackPop(stk);
    int b = StackPop(stk);

    StackPush(stk, a * b);
})

DEF_CMD(DIV, 6, 0,
{
    log("in div\n");

    int a = StackPop(stk);
    int b = StackPop(stk);

    if (b == 0)
    {
        printf("ZERO DIVISION ERROR: can't divide by zero\n");
        return ZERO_DIVISION;
    } else {
    StackPush(stk, a * b);
    }

})

DEF_CMD(IN, 7, 0,
{
    printf("in in\n");
    printf("enter a number");
    int value = 0;

    while (!scanf("%d", &value)) {
        printf("please, enter a number\n");
        SkipNewLines();
    }
    SkipNewLines();
    StackPush(stk, value);

})

DEF_CMD(OUT, 8, 0,
{
    log("in out\n");

    int a = StackPop(stk);

    printf("Out print is %d\n", a);
})

DEF_CMD(JUMP, 9, 1,
{
    cpu->ip = cpu->code[cpu->ip+1] - 1;
})


#endif // COMMANDS_H_INCLUDED
