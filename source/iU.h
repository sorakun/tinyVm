/*
 * iU Calculating laguage
*/
#include "eve_vm.h"

#ifndef iU_H
#define iU_H



#define MAX_TOKEN_SIZE 50

typedef char * string;

typedef enum
{
  Add, Substract, Multiply, Devide,  Power, Sine, Cos, XVariable, Constant, Assignment
} TNodeType;

typedef struct TOpType
{
  TNodeType op;
  int priority; // The higher, the more prior.
} TOpType;

typedef struct TNodePointer
{
  TNodeType node_type;

  struct TNodePointer * LeftChild;
  struct TNodePointer * RightChild;
// struct TNodePointer * UnaryChild;

  char varName[MAX_TOKEN_SIZE];

  double ConstantData;
} TNodePointer;

typedef struct Table
{
  int size;
  string * input;
} Table;

typedef struct TVariable
{
  char name[MAX_TOKEN_SIZE];
  double value;
} TVariable;



#endif

