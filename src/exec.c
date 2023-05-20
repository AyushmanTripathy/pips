#include <stdlib.h>
#include <stdio.h>
#include "exec.h"

extern void printError(char *, int);
extern void printTokenTree(Token *, int);
extern void freeToken(Token *);

extern Token * createToken(int, char *, int);
extern Token * trueBooleanToken;
extern Token * falseBooleanToken;

extern Function ** functions;
extern FunctionPointer ** defs;

Token * execScope(Token *, Variable **);
Token * execStatment(Token * , Variable **);
Token * execFunction(Function *, Token * t);

Token * handleConditional(Token * t, Variable ** vars) {
  int done = 0;
  while (t != NULL) {
    if (!done) {
      if (t->type == -23) {
        execScope((t->childTokens)[0], vars);
        return t->next;
      } else if (t->type == -21 || t->type == -22) {
        Token * condition = execStatment((t->childTokens)[0], vars);
        if (condition->int_data != 1) { 
          execScope((t->childTokens)[1], vars);
          done = 1;
        }
      } else return t;
    }
    t = t->next;
  }
  return t;
}

Token * execScope(Token * t, Variable ** vars) {
  while(t != NULL) {
    // IF statements
    if (t->type == -21) {
      t = handleConditional(t, vars);
      if (t == NULL) return NULL;
    }
    else if (t->type == -22 || t->type == -23)
      printError("conditionals need to begin with if statments", 1);
    else execStatment(t, vars);
    t = t->next;
  }
  return NULL;
}

Token * execStatment(Token * t, Variable ** vars) {
  if (t == NULL) printError("NULL execution", 3);
  if (t->data == NULL) printError("Nameless Function execution.", 3);
  if (t->data[0] == '%') {
    Variable * v = getVariable(vars, t->childTokens[0]->data);
    return createToken(v->type, NULL, v->int_data);
  }

  Token ** children = t->childTokens;
  Tokens childrenCopy = (Tokens) malloc(t->childTokensCount * sizeof(Token *));
  int * copied = (int *) malloc(t->childTokensCount * sizeof(int));
  for (int i = 0; i < t->childTokensCount; i++) {
    if (children[i]->type == -10) {
      childrenCopy[i] = execStatment(children[i], vars);
      copied[i] = 1;
    } else childrenCopy[i] = children[i];
  }
  FunctionPointer * fp = getFromFunctionPointers(defs, t->data);
  if (fp == NULL) {
    Function * fn = getFromFunctions(functions, t->data);
    if(fn == NULL) printError("Function not found!", 3);
    return execFunction(fn, t);
  }

  Token * output = (fp->pointer)(childrenCopy, t->childTokensCount);
  for (int i = 0; i < t->childTokensCount; i++) {
    if (copied[i] == 1) freeToken(childrenCopy[i]);
  }
  free(childrenCopy);
  return output;
}

Token * execFunction(Function * fn, Token * t) {
  Variable ** vars = initVariables();
  if (fn->paramsCount != 0) {
    if (fn->paramsCount != t->childTokensCount)
      printError("Not sufficient arguments for function.", 3);
    for (int i = 0; i < fn->paramsCount; i++) {
      char * name = fn->params[i]->data;
      Token * child = t->childTokens[i];
      addVariable(vars, name, child->type, child->int_data);
    }
  }
  execScope(fn->execSeq, vars);
  freeVariables(vars);
  return NULL;
}

void execGlobal(Token * execSeq) {
  Variable ** vars = initVariables();
  execScope(execSeq, vars);
  freeVariables(vars);
}
