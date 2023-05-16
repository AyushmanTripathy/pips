#include "defs.h"
#include <stdlib.h>
#include <stdio.h>

extern Token * createToken(int, char *, int);
extern void printError(char *, int);

Token * pass(Token * t) {
  return t;
}

Token * add(Token * t) {
  int sum = 0;
  Token ** args = t->childTokens;
  for (int i = 0; i < t->childTokensCount; i++) {
    if (args[i]->type != -1) printError("add function", 4);
    sum += args[i]->int_data;
  }
  return createToken(-1, NULL, sum);
}

Token * subtract(Token * t) {
  int sum = 0;
  Token ** args = t->childTokens;
  for (int i = 0; i < t->childTokensCount; i++) {
    if (args[i]->type != -1) printError("subtract function", 4);
    else if (i == 0) sum = args[i]->int_data;
    sum = sum - args[i]->int_data;
  }
  return createToken(-1, NULL, sum);
}

Token * print(Token * t) {
  Token ** args = t->childTokens;
  for (int i = 0; i < t->childTokensCount; i++) {
    if (args[i]->type == -1) printf("%d ", args[i]->int_data);
    else printf("%s ", args[i]->data);
  }
  printf("\n");
  return createToken(-5, NULL, 0);
}
