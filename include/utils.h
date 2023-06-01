#ifndef UTILS_H
#define UTILS_H

// IS FUNCTIONS
int isKeyword(char * s);
int isNumber(char * s);
int isSymbol(char s);
int isTrimable(char s);

char * getErrorCode(int code);

// STRING FUNCTIONS
char * mallocStr(char *);
char * sliceStr(char * str, int start, int end);
char * trimStr(char * str);
int calcSpaces(char * str);
int nextQuote(char * str, int index);

#endif
