#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"

extern short int tab_space_count;

extern void error(char *, short int);
extern char * trimStr(char *);
extern short int calcSpaces(char *);

void printLines(Line * line) {
  while (line != NULL) {
    printf("%s \n", line->data);
    line = line->next;
  }
}

void freeLine(Line * l) {
  l->next = NULL;
  free(l->data);
  free(l);
}

void freeLines(Line * l) {
  if (l == NULL) return;
  if (l->next != NULL) freeLines(l->next);
  freeLine(l);
}

Line * readFile(char * path) {
  // reading source code
  char buff[255];
  FILE *source = fopen(path, "r");
  Line * src = NULL;
  Line * head = NULL;

  if (source == NULL) error("Invalid File Name", 0);
  while(fgets(buff, 255, source) != NULL) {
    // check for empty lines
    char * data = trimStr(buff);
    if (data == NULL) continue;

    Line * node = (Line *) malloc(sizeof(Line));
    node->next = NULL;
    node->indentation = calcSpaces(buff) / tab_space_count;
    node->data = data;

    if (src == NULL) src = head = node;
    else {
      head->next = node;
      head = node;
    }
  }
  fclose(source);

  if (src == NULL) error("input file is empty.", 1);
  return src;
}

void appendString(Strings * strs, char * s) {
  char ** newArr = (char **) malloc((strs->length + 1) * sizeof(char *));
  for (int i = 0; i < strs->length; i++) {
    newArr[i] = strs->data[i];
  }
  newArr[strs->length] = s;
  if (strs->length != 0) free(strs->data);
  strs->length++;
  strs->data = newArr;
}

short int getStringLength(char * str, int x) {
  for (int i = x; 1; i++) {
    if (str[i] == '\0') return -1;
    else if (str[i] == '"') return i - x;
  }
}

Line * cleanseLines(Line * head, Strings * strs) {
  short int isQuote = 0, escapedQuote = 0, isComment = 0; 
  char * string = NULL;
  short int stringLength = 0;

  Line * prev = NULL;
  Line * l = head;

  while (l != NULL) {
    int len = strlen(l->data), index = 0;
    char * newStr = (char *) malloc((len + 1) * sizeof(char));
    char * str = l->data;

    for(int i = 0; i < len; i++) {
      if (isComment == 1) continue;
      if (escapedQuote == 1) {
        isQuote = isQuote ? 0 : 1;
        escapedQuote = 0;
      }
      if (str[i] == '"') {
        if (isQuote) {
          escapedQuote = 1;
          newStr[index++] = '#';
          newStr[index++] = (char) 33 + strs->length;
          appendString(strs, string);
          string = NULL;
          stringLength = 0;
        } else {
          short int len = getStringLength(str, i + 1);
          if (len == -1) {
            freeLines(head);
            free(newStr);
            error("Expected \".", 1);
          }
          string = (char *) malloc((len + 1) * sizeof(char));
          string[len] = '\0';
          isQuote = 1;
        }
      } else if (str[i] == '#' || str[i] == '/') isComment = 1;
      else if (isQuote == 1) string[stringLength++] = str[i];

      // handle quotes
      if (isQuote == 1);
      else if (isComment == 1);
      else newStr[index++] = str[i]; 
    }
    newStr[index] = '\0';
    isComment = 0;
  
    // check for empty lines
    if (strlen(newStr) == 0) {
      if (prev == NULL) head = l->next;
      else prev->next = l->next;
      Line * next = l->next;
      freeLine(l);
      free(newStr);
      l = next;
    } else {
      free(str);
      l->data = newStr;
      prev = l;
      l = l->next;
    }
  }
  return head;
}
