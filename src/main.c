#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "main.h"

Strings * strings = NULL;
Function ** functions = NULL;
FunctionPointer ** defFunctions = NULL;

double ** numbers = NULL;
short int numberCurrentIndex = 0;

Token * nullToken;
Token * falseBooleanToken;
Token * trueBooleanToken;

double ** initNumbers() {
  double ** nums = (double **) malloc(number_packet_count * sizeof(double *));
  for (short int i = 0; i < number_packet_count; i++)
    nums[i] = NULL;
  return nums;
}

void freeNumbers(double ** num) {
  int packetIndex = 0;
  for (short int x = 0; x < number_packet_count; x++) {
    if (num[x] != NULL) free(num[x]);
  }
  free(num);
}

void freeNumber(short int index) {
  printf("freeing %d\n", index);
  numbers[index / number_packet_size]
    [index % number_packet_size] = FLT_EPSILON;
}

void freeStrings(Strings * strs) {
  if (strs->length != 0) {
    for (short int i = 0; i < strs->length; i++) free(strs->data[i]);
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
  for (short int i = 0; i < t->childTokensCount; i++)
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
  if (numbers != NULL) freeNumbers(numbers);
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
    case 5: printf("INTERNAL");
            break;
    default: error("Unknown error code", 0);
  }
  printf(" ERROR ] %s\n", msg);
  freeMemory();
  exit(1);
}

double getNumber(short int index) {
  short int x = index / number_packet_size;
  short int i = index % number_packet_size;
  return numbers[x][i]; 
}

short int addNumber(double num) {
  short int x = numberCurrentIndex / number_packet_size;
  if (x >= number_packet_count)
    error("Number overflow", 5);
  if (numbers[x] == NULL) 
    numbers[x] = (double *) malloc(number_packet_size * sizeof(double));
  short int i = numberCurrentIndex  % number_packet_size;
  numbers[x][i] = num;
  numberCurrentIndex++;
  return numberCurrentIndex - 1;
}

Token * createToken(char type, char * data, int childCount) {
  Token * t = (Token *) malloc(sizeof(Token));
  t->next = NULL;

  switch (type) {
    case -1:
      t->type = -1;
      if (data == NULL) t->int_data = childCount;
      else t->int_data = addNumber(strtof(data, NULL));
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

void printTokenTree(Token * n, short int depth) {
  if (depth > 10) {
    printf("print overload\n");
    exit(1);
  }
  if (n == NULL) return;
  for (short int i = 1; i < depth; i++) printf("  ");
  if (depth != 0) printf("|-");

  switch (n->type) {
    case -1: printf("%lf", getNumber(n->int_data));
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
    for (short int i = 0; i < n->childTokensCount; i++) {
      printTokenTree(childrens[i] , depth + 1);
    }
  }

  if (n->next == NULL) return;
  printTokenTree(n->next, depth);
}

int main(int argc, char *argv[]) {
  if (argc < 2) error("No Input Specified.", 0);

  numbers = initNumbers();
  addNumber(0.0);
  functions = initFunctionHashMap();

  Token * global = parseFile(argv[1], functions);
  defFunctions = initFunctionPointers();
  addDefaultFunctions(defFunctions);

  nullToken = createToken(-5, NULL, 0);
  falseBooleanToken = createToken(-3, NULL, 0);
  trueBooleanToken = createToken(-3, NULL, 1);

  short int exitCode = execGlobal(global);
  freeTokenTree(global);
  freeMemory();
  return exitCode;
}
