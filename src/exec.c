#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "exec.h"

extern int selfMaximumCallCount;

extern void error(char *,short int);
extern void printTokenTree(Token *, short int);
extern void freeToken(Token *);
extern void freeTokenTree(Token *);

extern Token * createToken(short int, char *, int);
extern Token * copyToken(Token *);
extern Token * nullToken;
extern Token * trueBooleanToken;
extern Token * falseBooleanToken;

extern Strings * strings;
extern Function ** functions;
extern FunctionPointer ** defFunctions;

Token * execScope(Token *, Variable **);
Token * execStatment(Token * , Variable **);
Token * execFunction(Function *, Tokens, short int);

Token * handleConditional(Token * t, Variable ** vars) {
  short int done = 0;
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

Token * set(Tokens t, short int l, Variable ** vars) {
  if (l != 2) error("set function requires 2 arguments", 3);
  if (t[0]->type != -2) error("set function", 4);
  char * key = strings->data[t[0]->int_data];
  short int isMutation = setVariable(vars, key, t[1]->type, t[1]->int_data);
  if (isMutation == 1) error("Mutation", 3);
  return nullToken;
}

Token * execStatment(Token * t, Variable ** vars) {
  if (t == NULL) error("NULL execution", 5);
  if (t->data == NULL) error("Nameless Function execution.", 5);
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
  short int * copied = (short int *) malloc(t->childTokensCount * sizeof(short int));
  for (short int i = 0; i < t->childTokensCount; i++) copied[i] = 0;

  for (short int i = 0; i < t->childTokensCount; i++) {
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
    else if (strcmp(t->data, "self") == 0) {
      Token * self = createToken(-13, NULL, t->childTokensCount);
      free(self->childTokens);
      for (short int i = 0; i < t->childTokensCount; i++) {
        if (copied[i] == 0) childrenCopy[i] = copyToken(childrenCopy[i]);
      }
      self->childTokens = childrenCopy;
      free(copied);
      return self;
    } else error("Function not found!", 3);
  } else {
    if (childrenCopy[0] == NULL) printf("self calling %s\n", t->data);
    output = (fp->pointer)(childrenCopy, t->childTokensCount);
  }

  // freeing childrens
  for (short int i = 0; i < t->childTokensCount; i++) {
    if (copied[i] == 1 && childrenCopy[i]->type != -13) freeToken(childrenCopy[i]);
  }

  free(childrenCopy);
  free(copied);
  return output;
}

short int matchPattern(Token * pattern, Tokens children, int childCount) {
  short int i = 0;
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
 
Token * selfDef(Function * fn, Variable ** vars, Token * t) {
  short int patternCount = fn->paramsCount * -1;
  Token * execSeq = NULL;
  Token * param;

  Token * output;
  Token ** children = t->childTokens;
  int childCount = t->childTokensCount;
  t->childTokens = NULL;
  t->childTokensCount = 0;
  freeToken(t);

  int count = selfMaximumCallCount;
  while(1) {
    if (count == 0) error("Self maximum call count exceeded", 3);
    else count--;

    short int match;
    for (int index = 0; index < patternCount; index++) {
      Token * pattern = fn->params[index]->childTokens[0];
      if (matchPattern(pattern, children, childCount) == 1) {
        execSeq = fn->params[index]->childTokens[1];
        param = pattern;
        match = index;
        break;
      }
    }
    if (execSeq == NULL) error("No pattern matched", 3);
    short int i = 0;
    while (param != NULL) {
      if (param->type == 0) 
        mutateVariable(vars, param->data, children[i]->type, children[i]->int_data);
      i++;
      param = param->next;
    }
    for (short int i = 0; i < childCount; i++) freeToken(children[i]);
    free(children);

    param = fn->params[match];
    while(param->next != NULL) {
      param = param->next;
      Token * condition = execStatment(param->childTokens[0], vars);
      if (condition->type != -3) error("Guard condition should be boolean", 3);
      if (condition->int_data == 1) {
        execSeq = param->childTokens[1];
        break;
      }
    }
    output = execStatment(execSeq, vars);

    if (output->type == -15) {
      Token * returnValue;
      if (output->childTokensCount == 0) returnValue = nullToken;
      else if (output->childTokens[0]->type == -13) 
        returnValue = output->childTokens[0];
      else returnValue = output->childTokens[0];
      freeToken(output);
      output = returnValue;
    }
    if (output->type == -13) {
      children = output->childTokens;
      childCount = output->childTokensCount;
      output->childTokens = NULL;
      free(output);
    } else break;
  }
  freeVariables(vars);
  return output;
}

Token * execDef(Function * fn, Tokens children, int childCount) {
  short int patternCount = fn->paramsCount * -1;
  Token * execSeq = NULL;
  Token * param;
  short int match;
  for (short int index = 0; index < patternCount; index++) {
    Token * pattern = fn->params[index]->childTokens[0];
    if (matchPattern(pattern, children, childCount) == 1) {
      execSeq = fn->params[index]->childTokens[1];
      param = pattern;
      match = index;
      break;
    }
  }
  if (execSeq == NULL) error("No pattern matched", 3);
  Variable ** vars = initVariables();
  short int i = 0;
  while (param != NULL) {
   if (param->type == 0) {
     Token * child = children[i];
     addVariable(vars, param->data, child->type, child->int_data);
   }
   i++;
   param = param->next;
  }
  // check for gaurds
  param = fn->params[match];
  while(param->next != NULL) {
    param = param->next;
    Token * condition = execStatment(param->childTokens[0], vars);
    if (condition->type != -3) error("Guard condition should be boolean", 3);
    if (condition->int_data == 1) {
      execSeq = param->childTokens[1];
      break;
    }
  }
  Token * output = execStatment(execSeq, vars);
  if (output->type == -15) {
     Token * returnValue;
     if (output->childTokensCount == 0) returnValue = nullToken;
     else if (output->childTokens[0]->type == -13) 
       returnValue = output->childTokens[0];
     else returnValue = output->childTokens[0];
     freeToken(output);
     output = returnValue;
  }
  if (output->type == -13) return selfDef(fn, vars, output);
  freeVariables(vars);
  return output;
}

Token * selfFunction(Function * fn, Variable ** vars, Token * t) {
  Token * output;
  Token ** children = t->childTokens;
  int childCount = t->childTokensCount;
  t->childTokens = NULL;
  t->childTokensCount = 0;
  freeToken(t);

  int count = selfMaximumCallCount;
  while(1) {
    if (count == 0) error("Self maximum call count exceeded", 3);
    else count--;

    // setting vars
    if (fn->paramsCount != 0) {
      if (fn->paramsCount != childCount)
        error("Invalid no of arguments for self.", 3);
      for (int i = 0; i < childCount; i++) {
        char * name = fn->params[i]->data;
        Token * child = children[i];
        mutateVariable(vars, name, child->type, child->int_data);
      }
    }
    for (short int i = 0; i < childCount; i++) freeToken(children[i]);
    free(children);

    output = execScope(fn->execSeq, vars);

    if (output->type == -15) {
      Token * returnValue;
      if (output->childTokensCount == 0) returnValue = nullToken;
      else returnValue = output->childTokens[0];
      freeToken(output);
      if (returnValue->type == -13) {
        children = returnValue->childTokens;
        childCount = returnValue->childTokensCount;
        free(returnValue);
      } else {
        output = returnValue;
        break;
      }
    } else break;
  }

  freeVariables(vars);
  return output;
}

Token * execFunction(Function * fn, Tokens children, short int childCount) {
  if (fn->paramsCount < 0)
    return execDef(fn, children, childCount);
  
  Variable ** vars = initVariables();
  if (fn->paramsCount != 0) {
    if (fn->paramsCount != childCount)
      error("Invalid no of arguments for function.", 3);
    for (short int i = 0; i < fn->paramsCount; i++) {
      char * name = fn->params[i]->data;
      Token * child = children[i];
      addVariable(vars, name, child->type, child->int_data);
    }
  }

  Token * output = execScope(fn->execSeq, vars);

  if (output->type == -15) {
    Token * returnValue;
    if (output->childTokensCount == 0) returnValue = nullToken;
    else returnValue = output->childTokens[0];
    freeToken(output);
    if (returnValue->type == -13) return selfFunction(fn, vars, returnValue);
    else {
      freeVariables(vars);
      return returnValue;
    }
  }
  freeVariables(vars);
  return nullToken;
}

short int execGlobal(Token * execSeq) {
  Variable ** vars = initVariables();
  Token * output = execScope(execSeq, vars);
  freeVariables(vars);
  short int code = 0;
  if (output->type == -15) {
    if (output->childTokensCount == 0);
    else code = output->childTokens[0]->int_data;
    freeTokenTree(output);
  }
  return code;
}
