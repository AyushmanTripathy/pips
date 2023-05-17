#ifndef HASHMAP_H
#define HASHMAP_H

#include "types.h"

Function ** initFunctionHashMap();
void addToFunctions(Function **, Function *);
Function * getFromFunctions(Function **, char *);

FunctionPointer ** initFunctionPointers();
void addToFunctionPointers(FunctionPointer **, char *, Token* (*)(Token*));
FunctionPointer * getFromFunctionPointers(FunctionPointer **, char *);

Variable ** initVariables();
Variable * getVariable(Variable **, char *, int);
void addVariable(Variable **, char *, int, int, int);
#endif
