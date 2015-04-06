/*
 * Eve Virtual machine
 * An experimental vitual machine, to use as a target for the eve programmig language.
 * Author: Chouri Soulaymen (http://sorakun.legtux.org) email : sorakun@legtux.org
 */

#ifndef EVEVM_H
#define EVEVM_H

/* Operations */

#define RET 0
#define AVLOAD 1
#define RVLOAD 2
#define RALOAD 3
#define ARLOAD 4
#define AALOAD 5
#define RRLOAD 6
#define PUSH   7
#define POP    8

#define LD 10
#define ST 11

#define EXTEND 20

#define ADD  100
#define SUB  101
#define MULT 102
#define DEV  103

#define FADD 104
#define FSUB 105
#define FMULT 106
#define FDEV 107

#define AND  110
#define OR   111
#define XOR  112
#define NOT  113


#define CMP 150

#define JE  151
#define JNE 152
#define JG  153
#define JL  154
#define JUMP 155
#define CALL 156

#define PRINT_REG 200
#define PRINTC    201
#define PRINTS	  202

#define RINC 250
#define RDEC 251

#define GOTO 300

#define CLEAN_RGS 400

/* Registers */

#define REG_COUNT 5

#define A 0
#define B 1
#define C 2
#define D 3
#define R 4

/* Flags */

#define ZF 0 /* Zero Flag */
#define LF 1 /* Lower Flag */
#define GF 2 /* Greater Flag */

int regs[ REG_COUNT ];

int flags[3];

typedef struct tDataSeg
{
    unsigned int size;
    void * mem;
} tDataSeg;

typedef struct tStack
{
    int * mem;
    int size;
} tStack;

tStack stack;



tDataSeg * dataSegment;
int dataCount;

struct tProgram;

typedef struct tApp
{
    struct tProgram * program;
    int pCount;
} tApp;

typedef struct tProgram
{
    char * name;
    int pc;
    int ** code;
    int instCount;
    int running;
    struct tApp * app;
} tProgram;

/*
 * For quick String defenition
 * Returns memory address of the allocated and initialized space
 */

void push(int value);
int pop();
int defineString(const char * str);
void fetch(tProgram * p);
void eval(tProgram * p);
void showRegs();
void run(tProgram * p);
void initProgram(tProgram * p);

#endif
