#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "hashmaps.h"

extern short int hashmap_size;
extern short int hashmap_r;
extern short int variable_hashmap_size;
extern short int variable_hashmap_r;

extern void error(char *, short int);
extern void freeTokenTree(Token *);
extern void freeToken(Token *);

// HASHMAP
short int hashFunc(char * str, short int size, short int r) {
  short int hash = 0;
  for (int i = 0; str[i] != '\0'; i++)
    hash = ((r * hash) + (short int) str[i]) % size;
  return hash;
}

Function ** initFunctionHashMap() {
  Function ** buckets = (Function **) malloc(hashmap_size * sizeof(Function *));
  for (short int i = 0; i < hashmap_size; i++) buckets[i] = NULL;
  return buckets;
}

void addToFunctions(Function ** buckets, Function * fn) {
  short int hashKey = hashFunc(fn->name, hashmap_size, hashmap_r);
  if (buckets[hashKey] == NULL) buckets[hashKey] = fn;
  else {
    Function * iterator = buckets[hashKey];
    while(iterator->next != NULL) iterator = iterator->next;
    iterator->next = fn;
  }
}

Function * getFromFunctions(Function ** buckets, char * name) {
  short int hashKey = hashFunc(name, hashmap_size, hashmap_r);
  if (buckets[hashKey] != NULL) {
    Function * iterator = buckets[hashKey];
    while(strcmp(iterator->name, name) != 0) {
      iterator = iterator->next;
      if (iterator == NULL) return NULL; 
    }
    return iterator;
  } else return NULL; 
}

void freeFunction(Function * fn) {
  if (fn->next != NULL) freeFunction(fn->next);

  if (fn->paramsCount < 0) {
    fn->paramsCount *= -1;
    for (short int i = 0; i < fn->paramsCount; i++)
      freeTokenTree(fn->params[i]);
  } else {
    freeTokenTree(fn->execSeq);
    for (short int i = 0; i < fn->paramsCount; i++)
      freeToken(fn->params[i]);
  }
  free(fn->params);
  free(fn->name);
  free(fn);
}
 
void freeFunctionHashMap(Function ** buckets) {
  for (short int i = 0; i < hashmap_size; i++) {
    if (buckets[i] != NULL) freeFunction(buckets[i]);
  }
  free(buckets);
}

FunctionPointer ** initFunctionPointers() {
  FunctionPointer ** arr 
    = (FunctionPointer **) malloc(hashmap_size * sizeof(FunctionPointer));
  for (short int i = 0; i < hashmap_size; i++) arr[i] = NULL;
  return arr;
}

void addToFunctionPointers(FunctionPointer ** map, char * key, Token* (*fn)(Tokens, short int)) {
  short int hashKey = hashFunc(key, hashmap_size, hashmap_r);
  
  FunctionPointer * fp = (FunctionPointer *) malloc(sizeof(FunctionPointer));
  fp->next = NULL;
  fp->key = key;
  fp->pointer = fn;
  
  if (map[hashKey] == NULL) map[hashKey] = fp;
  else {
    FunctionPointer * iterator = map[hashKey];
    while (iterator->next != NULL) iterator = iterator->next;
    iterator->next = fp;
  }
}

FunctionPointer * getFromFunctionPointers(FunctionPointer ** map, char * key) {
  short int hashKey = hashFunc(key, hashmap_size, hashmap_r);
  
  if (map[hashKey] != NULL) {
    FunctionPointer * iterator = map[hashKey];
    while(strcmp(iterator->key, key) != 0) {
      iterator = iterator->next;
      if (iterator == NULL) return NULL; 
    }
    return iterator;
  } else return NULL; 
}

void freeFunctionPointer(FunctionPointer * fp) {
  if (fp->next != NULL) freeFunctionPointer(fp->next);
  free(fp);
}

void freeFunctionPointers(FunctionPointer ** map) {
  for (short int i = 0; i < hashmap_size; i++) {
    if (map[i] != NULL) freeFunctionPointer(map[i]);
  }
  free(map);
}

void addVariable(Variable ** map, char * name, char type, int int_data) {
  short int hashKey = hashFunc(name, variable_hashmap_size, variable_hashmap_r);
  
  Variable * v = (Variable *) malloc(sizeof(Variable));
  v->next = NULL;
  v->key = name;
  v->int_data = int_data;
  v->type = type;
  
  if (map[hashKey] == NULL) map[hashKey] = v;
  else {
    Variable * iterator = map[hashKey];
    while (iterator->next != NULL) iterator = iterator->next;
    iterator->next = v;
  }
}

short int setVariable(Variable ** map, char * name, char type, int int_data) {
  short int hashKey = hashFunc(name, variable_hashmap_size, variable_hashmap_r);
  
  Variable * v = (Variable *) malloc(sizeof(Variable));
  v->next = NULL;
  v->key = name;
  v->int_data = int_data;
  v->type = type;
  
  if (map[hashKey] == NULL) map[hashKey] = v;
  else {
    Variable * iterator = map[hashKey];
    while (1) {
      if (strcmp(iterator->key, name) == 0) return 1;
      if (iterator->next == NULL) break;
      iterator = iterator->next;
    }
    iterator->next = v;
  }
  return 0;
}

void mutateVariable(Variable ** map, char * name, char type, int int_data) {
  short int hashKey = hashFunc(name, variable_hashmap_size, variable_hashmap_r);
  Variable * iterator = map[hashKey];
  while (iterator != NULL) {
    if (strcmp(iterator->key, name) == 0) {
      iterator->type = type;
      iterator->int_data = int_data;
      return;
    }
    iterator = iterator->next;
  }
  setVariable(map, name, type, int_data);
}

Variable ** initVariables() {
  Variable ** arr = (Variable **) malloc(variable_hashmap_size * sizeof(Variable));
  for (short int i = 0; i < variable_hashmap_size; i++) arr[i] = NULL;
  addVariable(arr, "True", -3, 1);
  addVariable(arr, "False", -3, 0);
  addVariable(arr, "Null", -5, 0);
  return arr;
}

void freeVariable(Variable * v) {
  if (v->next != NULL) freeVariable(v->next);
  free(v);
}

void freeVariables(Variable ** map) {
  for (short int i = 0; i < variable_hashmap_size; i++) {
    if (map[i] != NULL) freeVariable(map[i]);
  }
  free(map);
}

Variable * getVariable(Variable ** map, char * name) {
  short int hashKey = hashFunc(name, variable_hashmap_size, variable_hashmap_r);
  
  if (map[hashKey] != NULL) {
    Variable * iterator = map[hashKey];
    while(strcmp(iterator->key, name) != 0) {
      iterator = iterator->next;
      if (iterator == NULL) error("Variable not defined.", 3); 
    }
    return iterator;
  } else error("Variable not defined.", 3);
}
