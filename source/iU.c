/*
** iU Calculating Language **
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "iU.h"

// Disable this if you do not want debug messages.
// #define DEBUG_MODE

#ifdef DEBUG_MODE
#define debugf printf
#else
#define debugf IGNORE
#endif

Table parse_tree;
TNodePointer * Head, * Tree;

string * valArray; // = (double*)malloc(sizeof(double));
int valSize = 0;

TOpType * opArray; // = (TNodeType*)malloc(sizeof(TNodeType));
int opSize = 0;

int posOp = 0, posVal = 0;

string Operations[] =
{
  "+", "-", "/", "*", "^", "=", "cos", "sin"
};

nbOp = 8;

TVariable * varArray;
int varSize = 0;

int DEVIDE_BY_ZERO = 0;

void IGNORE(const char * s, ...){}

void throw_err(string msg)
{
  printf(msg);
}

string getOpName(TNodeType  op)
{
  switch(op)
    {
    case Add:
      return "Add";
    case Substract:
      return "Substract";
    case Multiply:
      return "Multiply";
    case Devide:
      return "Devide";
    case Power:
      return "Devide";
    case Assignment:
      return "Assignment";
    case Sine:
      return "Sine";
    case Cos:
      return "Cos";
    default:
      throw_err("invalid operation.");
    }
}

TNodeType getOpType(string op)
{
  if (strcmp(op, "+") == 0)
    return Add;
  else if (strcmp(op, "-") == 0)
    return Substract;
  else if (strcmp(op, "*") == 0)
    return Multiply;
  else if (strcmp(op, "/") == 0)
    return Devide;
  else if (strcmp(op, "^") == 0)
    return Power;
  else if (strcmp(op, "=") == 0)
    return Assignment;
  else if (strcmp(op, "sin") == 0)
    return Sine;
  else if (strcmp(op, "cos") == 0)
    return Cos;
  else if (isdigit(op[0]))
    return Constant;
  else
    throw_err("invalid operation founded!");
}

int op_is_unary(TNodeType op)
{
  return (op == Cos || op == Sine || op==Substract);
}

int priority_of(TNodeType op)
{
  switch (op)
    {
    case Assignment:
      return 0;
    case Add:
      return 1;
    case Substract:
      return 2;
    case Cos:
    case Sine:
      return 3;
    case Multiply:
    case Devide:
      return 4;
    case Power:
      return 5;
    }
}

int priority_satisfied(int i, int j)
{
  debugf("testing priority:\n");

  if (opArray[i].priority > opArray[j].priority)
    return 1;
  if (opArray[i].priority < opArray[j].priority)
    return 0;

  return (priority_of(opArray[i].op) > priority_of(opArray[j].op));

}

int isop(string op)
{
  int b = 1, i = 0;
  for(i = 0; (i < nbOp)&&((strcmp(op, Operations[i]) != 0)); i++);
  return (i < nbOp);
}

void iU_add_variable(char name[], double value)
{
  debugf("registering %s as %f\n", name, value);
  strcpy(varArray[varSize].name, name);
  varArray[varSize].value = value;
  varSize++;
  varArray = (TVariable*)realloc(varArray, (varSize+1)*sizeof(TVariable));
}

void iU_init()
{
  DEVIDE_BY_ZERO = 0;
  valSize = 0;
  opSize = 0;
  varArray = (TVariable*)malloc(sizeof(TVariable));
  varSize = 0;
  posOp = 0;
  posVal = 0;
  iU_add_variable("pi", 3.14);
  iU_add_variable("e", exp(1));
}

void iU_reinit()
{
  DEVIDE_BY_ZERO = 0;
  free(valArray);
  valSize = 0;
  free(opArray); // = (TNodeType*)malloc(sizeof(TNodeType));
  opSize = 0;
  posOp = 0;
  posVal = 0;
}

int iU_var_isdefined(char name[])
{
  int i;
  for(i=0; i<varSize; i++)
    {
      debugf("comparing %s with %s\n",name, varArray[i].name );
      if (strcmp(varArray[i].name, name) == 0)
        return 1;
    }
  return 0;
}

double iU_fetch_var(char name[])
{
  int i;
  for(i=0; i<varSize; i++)
    {
      debugf("comparing %s with %s\n",name, varArray[i].name );
      if (strcmp(varArray[i].name, name) == 0)
        return varArray[i].value;
    }
  throw_err("undefined variable.");
}

Table iU_tokenize(string arg)
{
  string inputS = arg;//string_remove_c(arg, ' ');

  string * input = (string*)malloc(sizeof(string));
  char   buffer[MAX_TOKEN_SIZE]; // token's max length is 50.
  int    i;
  int    strnum = 0;
  char *  next = buffer;
  char   c;
  int dot_exist;
  Table tmp;

  debugf("input: <%s>\n", inputS);
  debugf("parsing:\n");

  while ((c = *inputS++) != '\0')
    {
      input[strnum++] = next;
      input = (string*)realloc(input, (strnum+1)*sizeof(string));
      if (isdigit(c))
        {
          int dot_exist = 0;
          debugf("Digit: %c\n", c);
          *next++ = c;
          while (isdigit(*inputS)||*inputS == '.')
            {
              if(*inputS == '.')
                if (dot_exist == 0)
                  dot_exist= 1;
                else
                  throw_err("dot already exists!");

              c = *inputS++;
              debugf("Digit: %c\n", c);
              *next++ = c;
            }
          *next++ = '\0';
        }
      else if (isalpha(c))
        {
          debugf("alpha: %c\n", c);
          *next++ = c;
          while (isalpha(*inputS) || *inputS == '_'|| isdigit(*inputS))
            {
              c = *inputS++;
              debugf("alpha: %c\n", c);
              *next++ = c;
            }
          *next++ = '\0';
        }
      else if (isspace(c))
        {
          debugf("space: %c'\n'", c);
          *next++ = c;
          while (isalpha(*inputS) || *inputS == '_'|| isdigit(*inputS))
            {
              c = *inputS++;
              debugf("alpha: %c\n", c);
              *next++ = c;
            }
          *next++ = '\0';
        }
      else
        {
          debugf("Non-digit: %c\n", c);
          *next++ = c;
          *next++ = '\0';
        }
    }

  debugf("parsed:\n");
  for (i = 0; i < strnum; i++)
    {
      debugf("%d: <<%s>>\n", i, input[i]);
    }
  tmp.size = strnum;
  tmp.input = (string*)malloc(strnum * sizeof(string));

  for (i = 0; i<strnum; i++)
    {
      tmp.input[i] = (string)malloc(strlen(input[i]) * sizeof(char));
      strcpy(tmp.input[i], input[i]);
    }

  return tmp;
}

void iU_gen_parsetables()
{
  int i, p= 0;
  valArray = (string*)malloc(sizeof(string));
  opArray = (TOpType*)malloc(sizeof(TOpType));


  for (i = 0; i <= parse_tree.size-1; i++)
    {
      //debugf("%s", parse_tree.input[i]);
      if (isop(parse_tree.input[i]))
        {
          opArray[opSize].op = getOpType(parse_tree.input[i]);
          opArray[opSize].priority = p;
          opSize++;
          opArray = (TOpType*)realloc(opArray, (opSize+1)*sizeof(TOpType));
          if(op_is_unary(getOpType(parse_tree.input[i])))
            {
              valArray[valSize] = "0.0";
              valSize++;
              valArray = (string*)realloc(valArray, (valSize+1)*sizeof(string));
              debugf("(%s unary op)\n", valArray[valSize-1]);
            }
          switch(opArray[opSize-1].op)
            {
            case Add:
              debugf("[Add, priority: %d]\n", opArray[opSize-1].priority);
              break;
            case Substract:
              debugf("[Substract, priority: %d]\n", opArray[opSize-1].priority);
              break;
            case Multiply:
              debugf("[Multiply, priority: %d]\n", opArray[opSize-1].priority);
              break;
            case Devide:
              debugf("[Devide, priority: %d]\n", opArray[opSize-1].priority);
            case Power:
              debugf("[Power, priority: %d]\n", opArray[opSize-1].priority);
            case Assignment:
              debugf("[Assignment, priority: %d]\n", opArray[opSize-1].priority);
              break;
            }
        }
      else if (strcmp(parse_tree.input[i], "(") == 0 || strcmp(parse_tree.input[i], "[") == 0)
        {
          p++;
        }
      else if (strcmp(parse_tree.input[i], ")") == 0 || strcmp(parse_tree.input[i], "]") == 0)
        {
          p--;
        }
      else
        {
          valArray[valSize] = parse_tree.input[i];
          valSize++;
          valArray = (string*)realloc(valArray, (valSize+1)*sizeof(string));
          debugf("(%s)\n", valArray[valSize-1]);
        }
    }
  debugf("sizeof(val) = %d && sizeof(op) = %d\n", valSize, opSize);

  debugf("========================\n\n");
  for (i = 0; i< opSize; i++)
    {
      switch(opArray[i].op)
        {
        case Add:
          debugf("[Add, priority: %d]\n", opArray[i].priority);
          break;
        case Substract:
          debugf("[Substract, priority: %d]\n", opArray[i].priority);
          break;
        case Multiply:
          debugf("[Multiply, priority: %d]\n", opArray[i].priority);
          break;
        case Devide:
          debugf("[Devide, priority: %d]\n", opArray[i].priority);
          break;
        case Power:
          debugf("[Power, priority: %d]\n", opArray[i].priority);
          break;
        case Assignment:
          debugf("[Assignment, priority: %d]\n", opArray[i].priority);
          break;
        }
    }
}

/*
 * pfrom: starting position
 * pto: final position
 * path: right or left operand
 */

TNodePointer * iU_gen_parsetree(int pfrom, int pto)
{
  debugf(" ----- from: %d to %d -----\n", pfrom, pto);
  /* one signle operand to return */
  if (pto == pfrom)
    {
      TNodePointer * res = (TNodePointer*)malloc(sizeof(TNodePointer));
      // digit or X value
      if(isdigit(valArray[pfrom][0]))
        {
          debugf("val = %s\n", valArray[pfrom]);
          res->ConstantData = atoi(valArray[pfrom]);
          debugf("double val = %f\n", res->ConstantData);
          res->node_type = Constant;
        }
      else
        {
          //
          if (iU_var_isdefined(valArray[pfrom]))
            {
              debugf("defined\n")  ;
              res->ConstantData = iU_fetch_var(valArray[pfrom]);
              res->node_type = Constant;
            }
          else
            {
              strcpy(res->varName, valArray[pfrom]);
              res->node_type = XVariable;
            }
        }

      debugf("(feather reached %d)\n", atoi(valArray[pfrom]));
      return res;
    }

  int i, MinPos = pfrom;
  for(i = pfrom+1; i < pto; i++)
    {
      debugf("comaring %s with %s\n", getOpName(opArray[MinPos].op), getOpName(opArray[i].op));
      if (priority_satisfied(MinPos, i))
        {
          debugf("%s < %s\n", getOpName(opArray[MinPos].op), getOpName(opArray[i].op));
          MinPos = i;
        }
    }
  // we founded the least prior operation, now create the parse tree.
  int c;
  debugf("min pos = %d\n", MinPos);
  TNodePointer * x = (TNodePointer*)malloc(sizeof(TNodePointer));
  x->node_type = opArray[MinPos].op;
  x->LeftChild  = iU_gen_parsetree(pfrom, MinPos);
  x->RightChild = iU_gen_parsetree(MinPos+1, pto);
  return x;
}


int instCount;
tProgram ** prog;
int lineCount;

void genPOP()
{
  prog[instCount]->instCount = 4;
  lineCount += 4;
  prog[instCount]->code = calloc(4, sizeof(int*));
  prog[instCount]->code[0] = calloc(3, sizeof(int));
  prog[instCount]->code[0][0] = POP;
  prog[instCount]->code[0][1] = B;

  prog[instCount]->code[1] = calloc(3, sizeof(int));
  prog[instCount]->code[1][0] = POP;
  prog[instCount]->code[1][1] = A;
}

void iU_calculate_node(TNodePointer * NodePointer)
{

  prog = realloc(prog, (instCount+1)*sizeof(tProgram*));
  prog[instCount] = calloc(1, sizeof(tProgram));

  switch(NodePointer->node_type)
  {
    case Add:
      genPOP();
      prog[instCount]->code[2] = calloc(3, sizeof(int));
      prog[instCount]->code[2][0] = ADD;
      prog[instCount]->code[3] = calloc(3, sizeof(int));
      prog[instCount]->code[3][0] = PUSH;
      prog[instCount]->code[3][1] = R;
      instCount++;
      iU_calculate_node(NodePointer->LeftChild);
      iU_calculate_node(NodePointer->RightChild);

      return;

    case Substract: // Unary operator! 1 - 2 = 1 + 0 - 2
      genPOP();
      prog[instCount]->code[2] = calloc(3, sizeof(int));
      prog[instCount]->code[2][0] = SUB;
      prog[instCount]->code[3] = calloc(3, sizeof(int));
      prog[instCount]->code[3][0] = PUSH;
      prog[instCount]->code[3][1] = R;
      instCount++;
      iU_calculate_node(NodePointer->LeftChild);
      iU_calculate_node(NodePointer->RightChild);

      return;

    case Multiply:
      genPOP();
      prog[instCount]->code[2] = calloc(3, sizeof(int));
      prog[instCount]->code[2][0] = MULT;

      prog[instCount]->code[3] = calloc(3, sizeof(int));
      prog[instCount]->code[3][0] = PUSH;
      prog[instCount]->code[3][1] = R;

      instCount++;

      iU_calculate_node(NodePointer->LeftChild);
      iU_calculate_node(NodePointer->RightChild);
      return;

    case Devide:
      genPOP();
      prog[instCount]->code[2] = calloc(3, sizeof(int));
      prog[instCount]->code[2][0] = DEV;
      prog[instCount]->code[3] = calloc(3, sizeof(int));
      prog[instCount]->code[3][0] = PUSH;
      prog[instCount]->code[3][1] = R;
      instCount++;
      iU_calculate_node(NodePointer->LeftChild);
      iU_calculate_node(NodePointer->RightChild);

      return;
/*
    case Power:
      debugf("node type: Power\n");
      return pow(iU_calculate_node(NodePointer->LeftChild), iU_calculate_node(NodePointer->RightChild));
*/
    case Constant:
      prog[instCount]->instCount = 2;
      lineCount+=2;
      prog[instCount]->code = calloc(2, sizeof(int*));
      prog[instCount]->code[0] = calloc(3, sizeof(int));
      prog[instCount]->code[0][0] = RVLOAD;
      prog[instCount]->code[0][1] = A;
      prog[instCount]->code[0][2] = (int)NodePointer->ConstantData;
      prog[instCount]->code[1] = calloc(3, sizeof(int));
      prog[instCount]->code[1][0] = PUSH;
      prog[instCount]->code[1][1] = A;
      instCount ++;
      return;
/*
    case Assignment:
      debugf("node type: Variable %s\n", NodePointer->LeftChild->varName);
      iU_add_variable(NodePointer->LeftChild->varName, NodePointer->RightChild->ConstantData);
      return NodePointer->RightChild->ConstantData;

    case XVariable:
      throw_err("undefined variable!\n");
*/
    }
}



/* Main Program */
int main()
{
  string cmd;
  instCount = 0;
  cmd = malloc(20*sizeof(char));
  iU_init();
  printf("iU Calculating language, v1.0:\n");

  do
    {
        lineCount = 0;
        stack.mem = NULL;
    stack.size = 0;

    dataSegment = NULL;
    dataCount = 0;

      instCount = 0;
      printf(">>");
      scanf("%s", cmd);
      iU_init();
      parse_tree = iU_tokenize(cmd); //4*2+3-3*3
      iU_gen_parsetables();
      prog = calloc(1, sizeof(tProgram  *));
      iU_calculate_node(iU_gen_parsetree(0, valSize-1));
      tProgram * p = calloc(1, sizeof(tProgram));

      int i = instCount-1;
      debugf("done, %d program\n", instCount);
      int line = 0;
      p->code = calloc(lineCount, sizeof(int*));
      for(; i >= 0; i --)
      {
          int j = 0;
          for(; j < prog[i]->instCount; j++)
          {
              int k = 0;
              p->code[line] = calloc(3, sizeof(int));
              for (; k < 3; k++)
              {
                  p->code[line][k] = prog[i]->code[j][k];
                  printf("%d, ", p->code[line][k]);
              }
              printf("\n");
              line++;
          }
      }
      p->code = realloc(p->code, (line+3)*sizeof(int*));
      p->code[line] = calloc(3, sizeof(int));
      p->code[line][0] = POP;
      p->code[line][1] = A;
      p->code[line+1] = calloc(3, sizeof(int));
      p->code[line+1][0] = PRINT_REG;
      p->code[line+1][1] = A;
      p->code[line+2] = calloc(3, sizeof(int));
      p->code[line+2][0] = RET;
      run(p);
      iU_reinit();

      free(prog);
    }
  while(1);
  return 0;
}
