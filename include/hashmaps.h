#ifndef HASHMAP_H
#define HASHMAP_H

#include "types.h"

typedef struct Function_tag {
  char * name;
  int paramsCount;
  Token ** params;
  Token * execSeq;
  Token * linker;
  struct Function_tag * next;
} Function;

typedef struct FunctionPointer_tag {
  char * key;
  Token * (* pointer)(Token *);
  struct FunctionPointer_tag * next;
} FunctionPointer;

Function ** initFunctionHashMap();
void addToFunctions(Function **, Function *);
Function * getFromFunctions(Function **, char *);

FunctionPointer ** initFunctionPointers();
void addToFunctionPointers(FunctionPointer **, char *, Token* (*)(Token*));
FunctionPointer * getFromFunctionPointers(FunctionPointer **, char *);

#endif
