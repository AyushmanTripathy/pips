#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern Token * createToken(int, char *, int);
extern void printError(char *, int);

extern Token * nullToken;
extern Token * trueBooleanToken;
extern Token * falseBooleanToken;

Token * pass(Token * t) {
  return t;
}

Token * boolFunc(Token * t) {
  printf("bool returned \n" );
  if (t->childTokensCount < 1)
    printError("bool functions requires atleast one argument.", 3);

  t = (t->childTokens)[0];
  switch (t->type) {
    case -1:
      if (t->int_data == 0) return falseBooleanToken;
      else return trueBooleanToken;
    case -2:
      if (strcmp(t->data, "") == 0) return falseBooleanToken;
      else return trueBooleanToken;
    case -3: return t;
    case -5: return falseBooleanToken;
  }
  printError("boolFunc collapse", 3);
}

Token * add(Token * t) {
  return createToken(-1, NULL, 0);
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
    else if(args[i]->type == -3) {
      printf("%s ", args[i]->int_data ? "True":"False" );
    }
    else printf("%s ", args[i]->data);
  }
  printf("\n");
  return nullToken;
}
