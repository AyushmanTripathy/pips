#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "hashmaps.h"

extern int hashmap_size;
extern int hashmap_r;
extern void printError(char *, int);

// HASHMAP
int hashFunc(char * str) {
  int hash = 0;
  for (int i = 0; str[i] != '\0'; i++)
    hash = ((hashmap_r * hash) + (int) str[i]) % hashmap_size;
  return hash;
}

Function ** initFunctionHashMap() {
  Function ** buckets = (Function **) malloc(hashmap_size * sizeof(Function *));
  for (int i = 0; i < hashmap_size; i++) buckets[i] = NULL;
  return buckets;
}

void addToFunctions(Function ** buckets, Function * fn) {
  int hashKey = hashFunc(fn->name);
  if (buckets[hashKey] == NULL) buckets[hashKey] = fn;
  else {
    Function * iterator = buckets[hashKey];
    while(iterator->next != NULL) iterator = iterator->next;
    iterator->next = fn;
  }
}

Function * getFromFunctions(Function ** buckets, char * name) {
  int hashKey = hashFunc(name);
  if (buckets[hashKey] != NULL) {
    Function * iterator = buckets[hashKey];
    while(strcmp(iterator->name, name) != 0) {
      iterator = iterator->next;
      if (iterator == NULL) return NULL; 
    }
    return iterator;
  } else return NULL; 
}

FunctionPointer ** initFunctionPointers() {
  FunctionPointer ** arr 
    = (FunctionPointer **) malloc(hashmap_size * sizeof(FunctionPointer));
  for (int i = 0; i < hashmap_size; i++) arr[i] = NULL;
  return arr;
}

void addToFunctionPointers(FunctionPointer ** map, char * key, Token* (*fn)(Token*)) {
  int hashKey = hashFunc(key);
  
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
  int hashKey = hashFunc(key);
  
  if (map[hashKey] != NULL) {
    FunctionPointer * iterator = map[hashKey];
    while(strcmp(iterator->key, key) != 0) {
      iterator = iterator->next;
      if (iterator == NULL) return NULL; 
    }
    return iterator;
  } else return NULL; 
}

Variable ** initVariables() {
  Variable ** arr = (Variable **) malloc(hashmap_size * sizeof(Variable));
  for (int i = 0; i < hashmap_size; i++) arr[i] = NULL;
  return arr;
}

char * generateVariableKey(char * name, int scopeId) {
  int nameLen = strlen(name);
  int idLen = ((int) log10(scopeId)) + 1;
  char * str = (char *) malloc((nameLen + idLen + 1) * sizeof(char));

  for (int i = 0; i < idLen; i++) {
    str[i] = (char) ((scopeId % 10) + 48);
    scopeId /= 10;
  }
  for (int i = 0; i < nameLen; i++) str[i + idLen] = name[i];

  str[nameLen + idLen] = '\0';
  return str;
}

void addVariable(Variable ** map, char * name, int scopeId, int type, int int_data) {
  char * key = generateVariableKey(name, scopeId);
  int hashKey = hashFunc(key);
  
  Variable * v = (Variable *) malloc(sizeof(Variable));
  v->next = NULL;
  v->key = key;
  v->int_data = int_data;
  v->type = type;
  
  if (map[hashKey] == NULL) map[hashKey] = v;
  else {
    Variable * iterator = map[hashKey];
    while (iterator->next != NULL) iterator = iterator->next;
    iterator->next = v;
  }
}

Variable * getVariable(Variable ** map, char * name, int scopeId) {
  char * key = generateVariableKey(name, scopeId);
  int hashKey = hashFunc(key);
  
  if (map[hashKey] != NULL) {
    Variable * iterator = map[hashKey];
    while(strcmp(iterator->key, key) != 0) {
      iterator = iterator->next;
      if (iterator == NULL) printError("Variable not defined.", 3); 
    }
    free(key);
    return iterator;
  } else printError("Variable not defined.", 3);
}
