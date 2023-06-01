#include <stdio.h>
#include <stdlib.h>

#include "main.h"

Strings * strings = NULL;
Function ** functions = NULL;
FunctionPointer ** defFunctions = NULL;

Token * nullToken;
Token * falseBooleanToken;
Token * trueBooleanToken;

void freeStrings(Strings * strs) {
  if (strs->length != 0) {
    for (int i = 0; i < strs->length; i++) free(strs->data[i]);
    free(strs->data);
  }
  free(strs);
}

void freeToken(Token * t) {
  if (t->type == -3 || t->type == -5) return;
  free(t->childTokens);
  if (t->data != NULL) free(t->data);
  free(t);
}

void freeTokenTree(Token * t) {
  if (t == NULL) return;
  for (int i = 0; i < t->childTokensCount; i++)
    freeTokenTree(t->childTokens[i]);
  if (t->next != NULL)
    freeTokenTree(t->next);
  freeToken(t);
}

void freeMemory() {
  if (nullToken != NULL) {
    free(nullToken);
    free(trueBooleanToken);
    free(falseBooleanToken);
  }
  if (strings != NULL) freeStrings(strings);
  if (functions != NULL) freeFunctionHashMap(functions);
  if (defFunctions != NULL) freeFunctionPointers(defFunctions);
}

Token * createToken(int type, char * data, int childCount) {
  Token * t = (Token *) malloc(sizeof(Token));
  t->next = NULL;

  switch (type) {
    case -1:
      t->type = -1;
      t->int_data = data == NULL ? childCount: atoi(data);
      t->data = NULL;
      t->childTokens = NULL;
      t->childTokensCount = 0;
      if (data != NULL) free(data);
      return t;
    case -2:
      t->type = -2;
      t->data = NULL;
      t->childTokens = NULL;
      t->childTokensCount = 0;
      t->int_data = childCount;
      return t;
    case -3:
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

Token * copyToken(Token * t) {
  Token * c = (Token *) malloc(sizeof(Token));
  c->next = NULL;
  c->childTokensCount = 0;
  c->childTokens = NULL;
  c->data = t->data;
  c->type = t->type;
  c->int_data = t->int_data;
  return c;
}

void printTokenNode(TokenNode * n) {
  while(n != NULL) {
    if (n->data) printf("(%s)-> ", n-> data);
    else printf("[%d]-> ", n->type);
    n = n->next;
  }
  printf("\n");
}

void error(char * msg, short int code) {
  printf("[ ");
  switch (code) {
    case 0: break;
    case 1: printf("PARSING");
            break;
    case 3: printf("RUNTIME");
            break;
    case 4: printf("TYPE");
            break;
    default: error("Unknown error code", 0);
  }
  printf(" ERROR ] %s\n", msg);
  freeMemory();
  exit(1);
}

void printTokenTree(Token * n, int depth) {
  if (depth > 10) {
    printf("print overload\n");
    exit(1);
  }
  if (n == NULL) return;
  for (int i = 1; i < depth; i++) printf("  ");
  if (depth != 0) printf("|-");

  switch (n->type) {
    case -1: printf("%d", n->int_data);
             break;
    case -2: printf("\"%s\"", strings->data[n->int_data]);
             break;
    case -3: printf("%s", n->int_data ? "True" : "False");
             break;
    case -5: printf("Null");
             break;
    case -21: printf("IF");
              break;
    case -22: printf("ELIF");
              break;
    case -23: printf("ELSE");
              break;
    case -10: printf("[%s]", n->data);
              break;
    default: printf("%s", n->data);
  }
  printf("\n");

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
  if (argc < 2) error("No Input Specified.", 0);

  functions = initFunctionHashMap();
  Token * global = parseFile(argv[1], functions);
  printTokenTree(global, 0);
  printf("-------------------\n");
  defFunctions = initFunctionPointers();
  addDefaultFunctions(defFunctions);

  nullToken = createToken(-5, NULL, 0);
  falseBooleanToken = createToken(-3, NULL, 0);
  trueBooleanToken = createToken(-3, NULL, 1);

  int exitCode = execGlobal(global);
  printf("-------------------\n");
  freeTokenTree(global);
  freeMemory();
  return exitCode;
}
