#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"

extern int keywordsLength;
extern char keywords[][5];
extern void error(char *, int);

// IS FUNCTIONS
int isKeyword(char * s) {
  for (int i = 0; i < keywordsLength; i++) {
    if (!strcmp(s, keywords[i])) return -250 + i;
  }
  return 0;
}

int isNumber(char * str) {
  int i = 0;
  if (str[i] == '-') i++;

  while(str[i] != '\0') {
    if (!(str[i] >= '0' && str[i] <= '9')) {
      if(i == 0) return 0;
      else error("Invalid Name Token", 1);
    }
    i++;
  }
  return 1;
}

int isSymbol(char s) {
  switch (s) {
    case ':': return 1;
    case ']': return 1;
    case '[': return 1;
    case '|': return 1;
  }
  return 0;
}

int isTrimable(char s) {
  switch (s) {
    case ' ': return 1;
    case '\t': return 1;
    case '\n': return 1;
    case '\v': return 1;
    case '\f': return 1;
    case '\r': return 1;
  }
  return 0;
}

// STRING FUNCTIONS
char * mallocStr(char * s) {
  int len = strlen(s);
  char * str = (char *) malloc((len + 1) * sizeof(char));
  for(int i = 0; i < len; i++) str[i] = s[i];
  str[len] = '\0';
  return str;
}

char * sliceStr(char * str, int start, int end) {
  char * newStr = (char *) malloc((end - start + 1) * sizeof(char));
  for (int i = 0; (i + start) < end; i++) newStr[i] = str[start + i];
  newStr[end - start] = '\0';
  return newStr;
}

char * trimStr(char * str) {
  int length = strlen(str), index = 0;

  int startCount = 0, endCount = 0;
  for (int i = 0; i < length; i++) {
    if (isTrimable(str[i])) startCount++;
    else break;
  }
  for(int i = length - 1; i >= 0; i--) {
    if (isTrimable(str[i])) endCount++;
    else break;
  }
  if (startCount == length) return NULL;
  char * newStr = (char *) malloc((length - startCount - endCount + 1) * sizeof(char));
  endCount = length - endCount;
  for (int i = startCount; i < endCount; i++) newStr[index++] = str[i];
  newStr[index] = '\0'; 
  return newStr;
}

int calcSpaces(char * str) {
  int count = 0, len = strlen(str);
  for(int i = 0; i < len; i++) {
    if (str[i] == ' ') count++;
    else break;
  }
  return count;
}

int nextQuote(char * str, int index) {
  int counter = 0;
  for (int i = index; str[i] != '"'; i++) {
    if (str[i] == '\0') error("Unescaped Quote", 1);
    counter++;
  }
  return counter;
}
