#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern Token * createToken(int, char *, int);
extern void printError(char *, int);

extern Token * nullToken;
extern Token * trueBooleanToken;
extern Token * falseBooleanToken;

Token * pass(Tokens t, int l) {
  return t[0];
}

// BOOL FUNCTIONS
Token * boolFunc(Tokens t, int l) {
  if (l < 1)
    printError("bool functions requires atleast one argument.", 3);

  switch (t[0]->type) {
    case -1:
      if (t[0]->int_data == 0) return falseBooleanToken;
      else return trueBooleanToken;
    case -2:
      if (strcmp(t[0]->data, "") == 0) return falseBooleanToken;
      else return trueBooleanToken;
    case -3: return t[0];
    case -5: return falseBooleanToken;
    case 0:
      if (strcmp(t[0]->data, "True")) return trueBooleanToken;
      else if (strcmp(t[0]->data, "False")) return falseBooleanToken;
  }
  printError("boolFunc collapse", 3);
}

// INT FUNCTIONS
Token * add(Tokens t, int l) {
  int sum = 0;
  for(int i = 0; i < l; i++) {
    if (t[i]->type != -1) printError("Add function", 4);
    else sum += t[i]->int_data;
  }
  return createToken(-1, NULL, sum);
}

Token * neg(Tokens t, int l) {
  if (l != 1)
    printError("neg functions requires one argument.", 3);
  if (t[0]->type != -1) printError("Neg Function", 4);
  return createToken(-1, NULL, -1 * t[0]->int_data);
}

// VOID FUNCTIONS
Token * print(Tokens t, int l) {
  for (int i = 0; i < l; i++) {
    if (t[i]->type == -1) printf("%d ", t[i]->int_data);
    else if(t[i]->type == -3) {
      printf("%s ", t[i]->int_data ? "True":"False" );
    }
    else printf("%s ", t[i]->data);
  }
  printf("\n");
  return nullToken;
}
