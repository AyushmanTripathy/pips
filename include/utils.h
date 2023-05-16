#ifndef UTILS_H
#define UTILS_H

// IS FUNCTIONS
int isKeyword(char * s);
int isSymbol(char s);
int isTrimable(char s);

char * getErrorCode(int code);
void printError(char * s, int code);

// HASHMAP
int hashFunc(char * str);

// STRING FUNCTIONS
char * sliceStr(char * str, int start, int end);
char * trimStr(char * str);
int calcSpaces(char * str);
int nextQuote(char * str, int index);

#endif
