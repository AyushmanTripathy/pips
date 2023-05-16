#ifndef EXECUTIONER_H
#define EXECUTIONER_H

#include "types.h"
#include "hashmaps.h"

Token * execStatment(Token *, Function **, FunctionPointer **);
Token * execFunction(Function *, Function **, FunctionPointer **);

#endif
