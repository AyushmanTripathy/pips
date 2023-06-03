#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <stdio.h>

#include "defs.h"

extern Token * createToken(int, char *, int);
extern Token * copyToken(Token *);
extern void addToFunctionPointers(FunctionPointer **, char *, Token*(*)(Tokens, int));
extern void error(char *, short int);

extern Strings * strings;
extern Token * nullToken;
extern Token * trueBooleanToken;
extern Token * falseBooleanToken;

extern int addNumber(double);
extern double getNumber(int);

char comp(int a, int b) {
  double diff = getNumber(a) - getNumber(b);
  if (diff < 0) {
    if (diff * -1 < FLT_EPSILON) return 0;
  } else if (diff < FLT_EPSILON) return 0;

  if (diff > 0) return -1;
  return 1;
}

Token * errorFunc(Tokens t, int l) {
  Token * input = t[0];
  if (input->type != -2) error("error function", 4);
  error(strings->data[input->int_data], 5);
}

Token * pass(Tokens t, int l) {
  if (l > 1) error("Pass Overload.", 3);
  if (t[0]->type == -3 || t[0]->type == -5) return t[0];
  return copyToken(t[0]);
}

Token * returnFunc(Tokens t, int l) {
  if (l > 1) error("Return Overload.", 3);
  if (l == 0) return createToken(-15, NULL, 0);

  Token * output = createToken(-15, NULL, 1);
  if (t[0]->type == -3 || t[0]->type == -5 || t[0]->type == -13)
    output->childTokens[0] = t[0];
  else output->childTokens[0] = copyToken(t[0]);
  return output;
}

// BOOL FUNCTIONS
Token * Bool(Token * t) {
  switch (t->type) {
    case -1:
      if (comp(t->int_data, 0) == 0)
        return falseBooleanToken;
      else return trueBooleanToken;
    case -2:
      if (strcmp(t->data, "") == 0) return falseBooleanToken;
      else return trueBooleanToken;
    case -3: return t;
    case -5: return falseBooleanToken;
  }
  error("Bool collapse", 3);
}

Token * boolFunc(Tokens t, int l) {
  if (l != 1) error("bool function takes one argument.", 3);
  return Bool(t[0]);
}

Token * not(Tokens t, int l) {
  if (l != 1) error("not function takes one argument.", 3);
  Token * input = Bool(t[0]);
  if (input->int_data == 0) return trueBooleanToken;
  else return falseBooleanToken;
}

void checkInputForBoolFuncs(Tokens t, int l, char * name) {
  if (l != 2) error("comparision function takes two argument.", 3);
  if (t[0]->type != t[1]->type) error(name, 4);
}

Token * eq(Tokens t, int l) {
  checkInputForBoolFuncs(t, l, "eq function");
  if (t[0]->type == -3) {
    if (t[0]->int_data == t[1]->int_data) return trueBooleanToken;
    else return falseBooleanToken;
  } else if (t[0]->type == -1) {
    if (comp(t[0]->int_data, t[1]->int_data) == 0)
      return trueBooleanToken;
    else return falseBooleanToken;
  }
}

Token * lt(Tokens t, int l) {
  checkInputForBoolFuncs(t, l, "lt function");
  if (t[0]->type == -3) {
    if (t[0]->int_data < t[1]->int_data) return trueBooleanToken;
    else return falseBooleanToken;
  } else if (t[0]->type == -1) {
    if (comp(t[0]->int_data, t[1]->int_data) == 1)
      return trueBooleanToken;
    else return falseBooleanToken;
  }
}

Token * gt(Tokens t, int l) {
  checkInputForBoolFuncs(t, l, "gt function");
  if (t[0]->type == -3) {
    if (t[0]->int_data > t[1]->int_data) return trueBooleanToken;
    else return falseBooleanToken;
  } else if (t[0]->type == -1) {
    if (comp(t[0]->int_data, t[1]->int_data) == -1)
      return trueBooleanToken;
    else return falseBooleanToken;
  }
}

Token * and(Tokens t, int l) {
  checkInputForBoolFuncs(t, l, "and function");
  if (t[0]->type == -3) {
    if (t[0]->int_data && t[1]->int_data) return trueBooleanToken;
    else return falseBooleanToken;
  }
  error("and function", 4);
}

Token * or(Tokens t, int l) {
  checkInputForBoolFuncs(t, l, "or function");
  if (t[0]->type == -3) {
    if (t[0]->int_data || t[1]->int_data) return trueBooleanToken;
    else return falseBooleanToken;
  }
  error("or function", 4);
}

// INT FUNCTIONS
Token * sum(Tokens t, int l) {
  if (l < 1)
    error("sum function requires atleast one argument", 3);
  double sum = 0;
  for(short int i = 0; i < l; i++) {
    if (t[i]->type != -1) error("add function", 4);
    else sum += getNumber(t[i]->int_data);
  }
  return createToken(-1, NULL, addNumber(sum));
}

Token * add(Tokens t, int l) {
  if (l != 2) error("add function takes two argument.", 3);
  if (t[0]->type != t[1]->type || t[1]->type != -1)
    error("Add function", 4);
  return createToken(-1, NULL, addNumber(
          getNumber(t[0]->int_data) + getNumber(t[1]->int_data)
        ));
}

Token * multiply(Tokens t, int l) {
  if (l < 1)
    error("multiply function requires atleast one argument", 3);
  double sum = 1;
  for(short int i = 0; i < l; i++) {
    if (t[i]->type != -1) error("add function", 4);
    else sum *= getNumber(t[i]->int_data);
  }
  return createToken(-1, NULL, addNumber(sum));
}

Token * divide(Tokens t, int l) {
  if (l != 2) error("divide function takes two argument.", 3);
  if (t[0]->type != t[1]->type || t[1]->type != -1)
    error("Divide function", 4);
  return createToken(-1, NULL, addNumber(
          getNumber(t[0]->int_data) / getNumber(t[1]->int_data)
        ));
}

Token * neg(Tokens t, int l) {
  if (l != 1)
    error("neg function takes one argument.", 3);
  if (t[0]->type != -1) error("neg Function", 4);
  return createToken(-1, NULL, addNumber(-1 * getNumber(t[0]->int_data)));
}

Token * max(Tokens t, int l) {
  if (l <= 0)
    error("max function requires atleast one argument", 3);
  int maxId = t[0]->int_data;
  double max = getNumber(maxId);
  for (int i = 1; i < l; i++) {
    if (t[i]->type != -1) error("max function", 4);
    else {
      double x = getNumber(t[i]->int_data);
      if (x > max) {
        max = x;
        maxId = t[i]->int_data;
      }
    }
  }
  return createToken(-1, NULL, maxId);
}

Token * min(Tokens t, int l) {
  if (l <= 0)
    error("min function requires atleast one argument", 3);
  int minId = t[0]->int_data;
  double min = getNumber(minId);
  for (int i = 1; i < l; i++) {
    if (t[i]->type != -1) error("min function", 4);
    else {
      double x = getNumber(t[i]->int_data);
      if (x < min) {
        min = x;
        minId = t[i]->int_data;
      }
    }
  }
  return createToken(-1, NULL, minId);
}

// VOID FUNCTIONS
Token * print(Tokens t, int l) {
  for (int i = 0; i < l; i++) {
    switch (t[i]->type) {
      case  0: printf("%s ", t[i]->data);
      case -1: printf("%lf ", getNumber(t[i]->int_data));
               break;
      case -2: printf("%s ", strings->data[t[i]->int_data]);
               break;
      case -3: printf("%s ", t[i]->int_data ? "True":"False" );
               break;
      case -5: printf("Null ");
               break;
      default: error("print collapse", 0);
    }
  }
  printf("\n");
  return nullToken;
}

void addDefaultFunctions(FunctionPointer ** map) {
  addToFunctionPointers(map, "pass", &pass);
  addToFunctionPointers(map, "return", &returnFunc);
  addToFunctionPointers(map, "error", &errorFunc);

  addToFunctionPointers(map, "bool", &boolFunc);
  addToFunctionPointers(map, "not", &not);  
  addToFunctionPointers(map, "eq", &eq); 
  addToFunctionPointers(map, "lt", &lt); 
  addToFunctionPointers(map, "gt", &gt); 
  addToFunctionPointers(map, "and", &and);  
  addToFunctionPointers(map, "or", &or); 

  addToFunctionPointers(map, "sum", &sum);
  addToFunctionPointers(map, "add", &add);
  addToFunctionPointers(map, "multiply", &multiply);
  addToFunctionPointers(map, "divide", &divide);
  addToFunctionPointers(map, "neg", &neg);
  addToFunctionPointers(map, "max", &max);
  addToFunctionPointers(map, "min", &min);

  addToFunctionPointers(map, "print", &print);
}
