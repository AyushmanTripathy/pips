#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

Token * nullToken;
Token * trueBooleanToken;
Token * falseBooleanToken;

Function ** functions;
FunctionPointer ** defs;

void printError(char * s, int code) {
  char * c = getErrorCode(code);
  if (c == NULL) printf("[ERROR] %s\n", s);
  else printf("[%s ERROR] %s\n", c, s);
  exit(1);
}

TokenNode * createTokenNode(char * str, int type) {
  TokenNode * t = (TokenNode *) malloc(sizeof(TokenNode)); 
  t->data = str;

  if (!type) {
    if (isNumber(str)) type = -1;
    else type = isKeyword(str); 
  } 

  t->type = type;
  t->next = NULL;
  return t;
}

Token * createToken(int type, char * data, int childCount) {
  Token * t = (Token *) malloc(sizeof(Token));
  t->next = NULL;

  if (type == -1) {
    t->type = -1;
    t->int_data = data == NULL ? childCount: atoi(data);
    t->data = NULL;
    t->childTokens = NULL;
    t->childTokensCount = 0;
    if (data != NULL) free(data);
    return t;
  } else if (type == -3) {
    t->type = -3;
    t->int_data = childCount;
    t->data = NULL;
    t->childTokens = NULL;
    t->childTokensCount = 0;
    if (data != NULL) free(data);
    return t;
  }

  if (childCount > 0) 
    t->childTokens = (Tokens) malloc(childCount * sizeof(Token *));
  else t->childTokens = NULL;
  t->type = type;
  t->data = data;
  t->int_data = 0;
  t->childTokensCount = childCount;
  return t;
}

void freeToken(Token * t) {
  if (t->type == -3 || t->type == -5) return;
  free(t->childTokens);
  //if (t->data != NULL) free(t->data);
  free(t);
}

void freeTokenTree(Token * t) {
  for (int i = 0; i < t->childTokensCount; i++)
    freeTokenTree(t->childTokens[i]);
  if (t->next != NULL)
    freeTokenTree(t->next);
  freeToken(t);
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
  else if(n->type == -3) printf("%s\n", n->int_data ? "True" : "False");
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

int main(int argc, char *argv[]) {
  functions = initFunctionHashMap();
  Token * global = parseFile(argv[1], functions);
  printTokenTree(global, 0);
  printf("------------------\n");

  nullToken = createToken(-5, NULL, 0);
  trueBooleanToken = createToken(-3, NULL, 1);
  falseBooleanToken = createToken(-3, NULL, 0);

  defs = initFunctionPointers();
  addToFunctionPointers(defs, "pass", &pass);
  addToFunctionPointers(defs, "bool", &boolFunc);
  addToFunctionPointers(defs, "add", &add);
  addToFunctionPointers(defs, "print", &print);
  addToFunctionPointers(defs, "neg", &neg);

  execGlobal(global);
  freeFunctionPointers(defs);
  freeFunctionHashMap(functions);
  freeTokenTree(global);
  freeToken(nullToken);
  free(falseBooleanToken);
  free(trueBooleanToken);
}
