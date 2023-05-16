#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

TokenNode * createTokenNode(char * str, int type) {
  TokenNode * t = (TokenNode *) malloc(sizeof(TokenNode)); 
  t->data = str;

  if (!type) {
    if (str[0] >= '0' && str[0] <= '9') type = -1;
    else type = isKeyword(str); 
  } 

  t->type = type;
  t->next = NULL;
  return t;
}

Token * createToken(int type, char * data, int childCount) {
  Token * t = (Token *) malloc(sizeof(Token));

  if (type == -1) {
    t->type = -1;
    t->int_data = childCount == 0 ? atoi(data): childCount;
    t->data = NULL;
    t->childTokens = NULL;
    t->childTokensCount = 0;
    return t;
  }

  if (childCount > 0) 
    t->childTokens = (Token **) malloc(childCount * sizeof(Token *));
  else t->childTokens = NULL;
  t->type = type;
  t->data = data;
  t->int_data = 0;
  t->childTokensCount = childCount;
  return t;
}

void printTokenNode(TokenNode * n) {
  while(n != NULL) {
    if (n->data) printf("(%s)-> ", n-> data);
    else printf("[%d]-> ", n->type);
    n = n->next;
  }
  printf("\n");
}

void printTokenTree(Token * n, int depth) {
  if (n == NULL) return;
  for (int i = 1; i < depth; i++) printf("  ");
  if (depth != 0) printf("|-");
  if(n->type == -10) printf("[%s]\n", n->data);
  else if(n->type == -1) printf("%d\n", n->int_data);
  else printf("%s\n", n->data);

  if (n->childTokensCount != 0) {
    Token ** childrens = n->childTokens;
    for (int i = 0; i < n->childTokensCount; i++) {
      printTokenTree(childrens[i] , depth + 1);
    }
  }

  if (n->next == NULL) return;
  printTokenTree(n->next, depth);
}

Token * execStatment(Token * t, Function ** functions, FunctionPointer ** defs);
Token * execFunction(Function * fn, Function ** functions, FunctionPointer ** defs) {
  Token * iterator = fn->execSeq;
  while(iterator != NULL) {
    execStatment(iterator, functions, defs);
    iterator = iterator->next;
  }
  return NULL;
}

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

int main(int argc, char *argv[]) {
  FunctionPointer ** defs = initFunctionPointers();
  addToFunctionPointers(defs, "add", &add);
  addToFunctionPointers(defs, "print", &print);
  addToFunctionPointers(defs, "subtract", &print);

  Function ** functions = initFunctionHashMap();
  Function * global = parseFile(argv[1], functions);
  //execFunction(global, functions, defs);
}
