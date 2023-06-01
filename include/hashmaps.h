#ifndef HASHMAP_H
#define HASHMAP_H

#include "types.h"

Function ** initFunctionHashMap();
void addToFunctions(Function **, Function *);
Function * getFromFunctions(Function **, char *);
void freeFunctionHashMap(Function **);

FunctionPointer ** initFunctionPointers();
void addToFunctionPointers(FunctionPointer **, char *, Token* (*)(Token **, int));
FunctionPointer * getFromFunctionPointers(FunctionPointer **, char *);
void freeFunctionPointers(FunctionPointer **);

Variable ** initVariables();
Variable * getVariable(Variable **, char *);
void addVariable(Variable **, char *, int, int);
int setVariable(Variable **, char *, int, int);
void mutateVariable(Variable **, char *, int, int);
void freeVariables(Variable ** );

#endif
