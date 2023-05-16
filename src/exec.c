#include <stdlib.h>
#include "exec.h"

extern void printError(char *, int);

Token * execStatment(Token * t, Function ** functions, FunctionPointer ** defs) {
  Token ** children = t->childTokens;
  for (int i = 0; i < t->childTokensCount; i++) {
    if ((children[i])->type == -10) 
      children[i] = execStatment(children[i], functions, defs);
  }
  FunctionPointer * fp = getFromFunctionPointers(defs, t->data);
  if (fp == NULL) {
    Function * fn = getFromFunctions(functions, t->data);
    if(fn == NULL) printError("Function not found!", 3);
    return execFunction(fn, functions, defs);
  }
  return (fp->pointer)(t);
}

Token * execFunction(Function * fn, Function ** functions, FunctionPointer ** defs) {
  Token * iterator = fn->execSeq;
  while(iterator != NULL) {
    execStatment(iterator, functions, defs);
    iterator = iterator->next;
  }
  return NULL;
}
