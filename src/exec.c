#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "exec.h"

extern void error(char *,short int);
extern void printTokenTree(Token *, int);
extern void freeToken(Token *);
extern void freeTokenTree(Token *);

extern Token * createToken(int, char *, int);
extern Token * nullToken;
extern Token * trueBooleanToken;
extern Token * falseBooleanToken;

extern Strings * strings;
extern Function ** functions;
extern FunctionPointer ** defFunctions;

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
      error("conditionals need to begin with if statments", 1);

    Token * output = execStatment(t, vars);
    if (output->type == -15) return output;
    t = t->next;
  }
  return nullToken;
}

Token * set(Tokens t, int l, Variable ** vars) {
  if (l != 2) error("set function requires 2 arguments", 3);
  if (t[0]->type != -2) error("set function", 4);
  char * key = strings->data[t[0]->int_data];
  int isMutation = setVariable(vars, key, t[1]->type, t[1]->int_data);
  if (isMutation == 1) error("Mutation", 3);
  return nullToken;
}

Token * execStatment(Token * t, Variable ** vars) {
  if (t == NULL) error("NULL execution", 3);
  if (t->data == NULL) error("Nameless Function execution.", 3);

  if (t->data[0] == '%') {
    Variable * v = getVariable(vars, t->childTokens[0]->data);
    if (v->type == -3) {
      if (v->int_data == 0) return falseBooleanToken;
      else return trueBooleanToken;
    } else if (v->type == -5) return nullToken;
    return createToken(v->type, NULL, v->int_data);
  }

  Token ** children = t->childTokens;
  Tokens childrenCopy = (Tokens) malloc(t->childTokensCount * sizeof(Token *));
  int * copied = (int *) malloc(t->childTokensCount * sizeof(int));
  for (int i = 0; i < t->childTokensCount; i++) copied[i] = 0;

  for (int i = 0; i < t->childTokensCount; i++) {
    if (children[i]->type == -10) {
      childrenCopy[i] = execStatment(children[i], vars);
      copied[i] = 1;
    } else childrenCopy[i] = children[i];
  }

  Token * output;
  FunctionPointer * fp = getFromFunctionPointers(defFunctions, t->data);
  if (fp == NULL) {
    Function * fn = getFromFunctions(functions, t->data);
    if(fn != NULL)
      output = execFunction(fn, childrenCopy, t->childTokensCount);
    else if (strcmp(t->data, "set") == 0)
      output = set(childrenCopy, t->childTokensCount, vars);
    else error("Function not found!", 3);
  } else {
    if (childrenCopy[0] == NULL) printf("self calling %s\n", t->data);
    output = (fp->pointer)(childrenCopy, t->childTokensCount);
  }

  // freeing childrens
  for (int i = 0; i < t->childTokensCount; i++) {
    if (copied[i] == 1) freeToken(childrenCopy[i]);
  }

  free(childrenCopy);
  free(copied);
  return output;
}

int matchPattern(Token * pattern, Tokens children, int childCount) {
  int i = 0;
  while(pattern != NULL) {
    if (childCount == i) return 0;
    Token * child = children[i];
    switch (pattern->type) {
      case -1:
        if (pattern->int_data != child->int_data) return 0;
        break;
    }
    i++;
    pattern = pattern->next;
  }
  if (i == childCount) return 1;
  return 0;
}

Token * execDef(Function * fn, Tokens children, int childCount) {
  int patternCount = fn->paramsCount * -1 / 2;
  Token * execSeq = NULL;
  Token * param;
  for (int index = 0; index < patternCount; index++) {
    Token * pattern = fn->params[index * 2];
    if (matchPattern(pattern, children, childCount) == 1) {
      execSeq = fn->params[(index * 2) + 1];
      param = pattern;
      break;
    }
  }
 if (execSeq == NULL) error("No pattern matched", 3);
 Variable ** vars = initVariables();
 int i = 0;
 while (param != NULL) {
  if (param->type == 0) {
    Token * child = children[i];
    addVariable(vars, param->data, child->type, child->int_data);
  }
  i++;
  param = param->next;
 }
 Token * output = execStatment(execSeq, vars);
 freeVariables(vars);
 if (output->type == -15) {
    Token * returnValue;
    if (output->childTokensCount == 0) returnValue = nullToken;
    else returnValue = output->childTokens[0];
    freeToken(output);
    return returnValue;
  }
 return output;
}

Token * execFunction(Function * fn, Tokens children, int childCount) {
  if (fn->paramsCount < 0)
    return execDef(fn, children, childCount);

  Variable ** vars = initVariables();
  if (fn->paramsCount != 0) {
    if (fn->paramsCount != childCount)
      error("Not sufficient arguments for function.", 3);
    for (int i = 0; i < fn->paramsCount; i++) {
      char * name = fn->params[i]->data;
      Token * child = children[i];
      addVariable(vars, name, child->type, child->int_data);
    }
  }
  Token * output = execScope(fn->execSeq, vars);
  freeVariables(vars);

  if (output->type == -15) {
    Token * returnValue;
    if (output->childTokensCount == 0) returnValue = nullToken;
    else returnValue = output->childTokens[0];
    freeToken(output);
    return returnValue;
  }
  return nullToken;
}

int execGlobal(Token * execSeq) {
  Variable ** vars = initVariables();
  Token * output = execScope(execSeq, vars);
  freeVariables(vars);
  int code = 0;
  if (output->type == -15) {
    if (output->childTokensCount == 0);
    else code = output->childTokens[0]->int_data;
    freeTokenTree(output);
  }
  return code;
}
