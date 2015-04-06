/*
 * Eve Virtual machine
 * An experimental vitual machine, to use as a target for the eve programmig language.
 * Author: Chouri Soulaymen (http://sorakun.legtux.org) email : sorakun@legtux.org
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "iU.h"
#include "eve_vm.h"

void push(int value)
{
    printf("Pushing %d, current stack size: %d\n", value, stack.size+1);
    stack.mem = (int*)realloc(stack.mem, (stack.size+1)*sizeof(int));
    stack.mem[stack.size] = value;
    stack.size++;
}

int pop()
{
    printf("current stack size: %d\n", stack.size);
    int tmp = stack.mem[stack.size-1];
    stack.mem = (int*)realloc(stack.mem, (stack.size-1)*sizeof(int));
    stack.size--;
    return tmp;
}

void fetch(tProgram * p)
{
    p->pc++;
}


void eval(tProgram * p)
{
    int * inst = NULL;
    inst = p->code[ p->pc ];

    if(inst == NULL)
        printf("ERROR\n");

    switch( inst[0] )
    {
    case RET:
        p->running = 0;
        break;

    case RVLOAD:
        regs[ inst[1] ] = inst[2];
        break;

    case AVLOAD:
    {
        printf("Moving to address %d value %d dataCount = %d.\n", inst[1], inst[2], dataCount);
        if (inst[1] >= dataCount)
        {
            dataSegment = (tDataSeg*)realloc(dataSegment, (inst[1]+1)*sizeof(tDataSeg));
            dataSegment[inst[1]].mem = (void*)malloc(sizeof(int));
            int * x = (int*)dataSegment[inst[1]].mem;
            *x = inst[2];
            dataCount = inst[1]+1;
        }
        else
        {
            int * x = (int*)dataSegment[inst[1]].mem;
            *x = inst[2];
        }
        break;
    }

    case RALOAD:
    {
        int * x = (int*)dataSegment[ inst[2] ].mem;
        regs[ inst[1] ] = *x;
        break;
    }

    case ARLOAD:
    {
        int * x = (int*)dataSegment[ inst[1] ].mem;
        *x = regs[ inst[1] ];
        break;
    }

    case RRLOAD:
        regs[ inst[1] ] = regs[ inst[2] ];
        break;

    case AALOAD:
    {
        if (inst[1] >= dataCount)
        {
            dataSegment = (tDataSeg*)realloc(dataSegment, (inst[1]+1)*sizeof(tDataSeg));
            dataSegment[inst[1]].mem = (void*)malloc(sizeof(int));
            dataSegment[inst[1]].mem = dataSegment[inst[2]].mem;
            dataCount = inst[1]+1;
        }
        else
        {
            dataSegment[inst[1]].mem = dataSegment[inst[2]].mem;
        }
        break;
    }

    case PUSH:
        push( regs[ inst[1] ] );
        break;

    case POP:
        regs[ inst[1] ] = pop();
        break;

        /*
            case CALL:
              run(&p->app->programs[inst[1]]); break;
        */
    case LD:
    {
        int * x = ((int*)dataSegment[ regs[ A ] ].mem + regs[B]);
        regs[ R ] =  *x;
        break;
    }

    case ST:
    {
        printf("Storing %d in [%d][%d]\n", inst[1], regs[A], regs[B]);
        int * x = ((int*)dataSegment[ regs[ A ] ].mem + regs[B]);
        *x =  inst[1];
        break;
    }

    case EXTEND:
    {
        dataSegment[ regs[ A ] ].mem = realloc(dataSegment[ inst[1] ].mem, inst[2] * sizeof(int));
        break;
    }

    case CMP:
    {
        if (regs[ A ] == regs[ B ])
        {
            flags[ ZF ] = 1;
            flags[ GF ] = 0;
            flags[ LF ] = 0;
        }
        else if (regs[ A ] > regs[ B ])
        {
            flags[ ZF ] = 0;
            flags[ GF ] = 1;
            flags[ LF ] = 0;
        }
        else
        {
            flags[ ZF ] = 0;
            flags[ GF ] = 0;
            flags[ LF ] = 1;
        }
        break;
    }

    case JE:
        if (flags[ ZF ])
            p->pc = inst[ 1 ];
        break;

    case JNE:
        if (!flags[ ZF ])
            p->pc = inst[ 1 ];
        break;

    case JG:
        if (flags[ GF ])
            p->pc = inst[ 1 ];
        break;

    case JL:
        if (flags[ LF ])
            p->pc = inst[ 1 ];
        break;

    case JUMP:
        p->pc = inst[ 1 ];
        break;

    case RINC:
        regs[ inst[ 1 ] ] ++;
        break;

    case RDEC:
        regs[ inst[ 1 ] ] --;
        break;

    case ADD:
        regs[ R ] = regs[ A ] + regs[ B ];
        break;

    case SUB:
        regs[ R ] = regs[ A ] - regs[ B ];
        break;

    case MULT:
        regs[ R ] = regs[ A ] * regs[ B ];
        break;

    case DEV:
        regs[ R ] = regs[ A ] / regs[ B ];
        break;

    case PRINT_REG:
        printf("%d\n", regs[ inst[1] ]);


    case PRINTC:
        printf("%c\n", regs[ R ]);
        break;

    case PRINTS:
        printf("%s", (char*)dataSegment[ inst[1] ].mem);
        break;

    case CLEAN_RGS:
        regs[A] = regs[B] = regs[R] = 0;
        break;
    }
}

void showRegs()
{
    int i;
    printf( "regs = " );
    for( i=0; i<REG_COUNT; i++ )
        printf( "%d ", regs[ i ] );

    printf( "flags(%d, %d, %d).\n", flags[0], flags[1], flags[2]);
}

void run(tProgram * p)
{
    p->running = 1;
    p->pc = 0;
    while( p->running )
    {
        //showRegs();
        eval(p);
        fetch(p);
    }
    showRegs();
}

/*
 * For quick String defenition
 * Returns memory address of the allocated and initialized space
 */
int defineString(const char * str)
{
    dataSegment = (tDataSeg*)realloc(dataSegment, (dataCount+1)*sizeof(tDataSeg));
    dataSegment[dataCount].mem = (void*)malloc(sizeof(char)*(strlen(str)+1)); /* +1 for \0.*/
    strcpy((char*)dataSegment[dataCount].mem, str);

    dataCount++;
}
