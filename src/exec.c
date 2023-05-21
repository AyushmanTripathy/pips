#include <stdlib.h>
#include <stdio.h>
#include "exec.h"

extern void printError(char *, int);
extern void printTokenTree(Token *, int);
extern void freeToken(Token *);

extern Token * createToken(int, char *, int);
extern Token * nullToken;
extern Token * trueBooleanToken;
extern Token * falseBooleanToken;

extern Function ** functions;
extern FunctionPointer ** defs;

Token * execScope(Token *, Variable **);
Token * execStatment(Token * , Variable **);
Token * execFunction(Function *, Tokens, int);

Token * handleConditional(Token * t, Variable ** vars) {
  int done = 0;
  while (t != NULL) {
    if (!done) {
      if (t->type == -23) {
        Token * output = execScope((t->childTokens)[0], vars);
        if (output->type == -15) return output;
        return t->next;
      } else if (t->type == -21 || t->type == -22) {
        Token * condition = execStatment((t->childTokens)[0], vars);
        if (condition->int_data == 1) { 
          Token * output = execScope((t->childTokens)[1], vars);
          if (output->type == -15) return output;
          done = 1;
        }
      } else return t;
    }
    t = t->next;
  }
  return NULL;
}

Token * execScope(Token * t, Variable ** vars) {
  while(t != NULL) {
    // IF statements
    if (t->type == -21) {
      t = handleConditional(t, vars);
      if (t == NULL) return nullToken;
      else if (t->type == -15) return t;
    } else if (t->type == -22 || t->type == -23)
      printError("conditionals need to begin with if statments", 1);
    else {
      Token * output = execStatment(t, vars);
      if (output->type == -15) return output;
    }
    t = t->next;
  }
  return nullToken;
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

  Token * output;
  FunctionPointer * fp = getFromFunctionPointers(defs, t->data);
  if (fp == NULL) {
    Function * fn = getFromFunctions(functions, t->data);
    if(fn == NULL) printError("Function not found!", 3);
    output = execFunction(fn, childrenCopy, t->childTokensCount);
  } else {
    if (childrenCopy[0] == NULL) printf("calling %s\n", t->data);
    output = (fp->pointer)(childrenCopy, t->childTokensCount);
  }
  // freeing childrens
  for (int i = 0; i < t->childTokensCount; i++) {
    if (copied[i] == 1) freeToken(childrenCopy[i]);
  }
  free(childrenCopy);
  return output;
}

Token * execFunction(Function * fn, Tokens children, int childCount) {
  Variable ** vars = initVariables();
  if (fn->paramsCount != 0) {
    if (fn->paramsCount != childCount)
      printError("Not sufficient arguments for function.", 3);
    for (int i = 0; i < fn->paramsCount; i++) {
      char * name = fn->params[i]->data;
      Token * child = children[i];
      addVariable(vars, name, child->type, child->int_data);
    }
  }
  Token * output = execScope(fn->execSeq, vars);
  freeVariables(vars);
  if (output->type == -15) {
    if (output->childTokensCount == 0) return nullToken;
    else return output->childTokens[0];
  }
  return nullToken;
}

void execGlobal(Token * execSeq) {
  Variable ** vars = initVariables();
  execScope(execSeq, vars);
  freeVariables(vars);
}
