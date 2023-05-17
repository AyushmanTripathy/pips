#include <stdlib.h>
#include <stdio.h>
#include "exec.h"

extern void printError(char *, int);
extern void printTokenTree(Token *, int);

extern Token * nullToken;
extern Function ** functions;
extern FunctionPointer ** defs;

Token * execScope(Token *);

Token * handleConditional(Token * t) {
  int done = 0;
  while (t != NULL) {
    if (!done) {
      if (t->type == -23) {
        execScope((t->childTokens)[0]);
        return t->next;
      } else if (t->type == -21 || t->type == -22) {
        Token * condition = execStatment((t->childTokens)[0]);
        if (condition->int_data != 1) { 
          execScope((t->childTokens)[1]);
          done = 1;
        }
      } else return t;
    }
    t = t->next;
  }
  return t;
}


Token * execScope(Token * t) {
  while(t != NULL) {
    // IF statements
    if (t->type == -21) {
      t = handleConditional(t);
      if (t == NULL) return NULL;
    }
    else if (t->type == -22 || t->type == -23)
      printError("conditionals need to begin with if statments", 1);
    else execStatment(t);
    t = t->next;
  }
  return NULL;
}

Token * execStatment(Token * t) {
  Token ** children = t->childTokens;
  for (int i = 0; i < t->childTokensCount; i++) {
    if ((children[i])->type == -10) 
      children[i] = execStatment(children[i]);
  }
  FunctionPointer * fp = getFromFunctionPointers(defs, t->data);
  if (fp == NULL) {
    Function * fn = getFromFunctions(functions, t->data);
    if(fn == NULL) printError("Function not found!", 3);
    return execFunction(fn);
  }
  return (fp->pointer)(t);
}

Token * execFunction(Function * fn) {
  execScope(fn->execSeq);
  return NULL;
}
