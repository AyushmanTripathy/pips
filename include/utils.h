#ifndef UTILS_H
#define UTILS_H

// IS FUNCTIONS
char isKeyword(char * s);
short int isNumber(char * s);
short int isSymbol(char s);
short int isTrimable(char s);

char * getErrorCode(int code);

// STRING FUNCTIONS
char * mallocStr(char *);
char * sliceStr(char * str, int start, int end);
char * trimStr(char * str);
short int calcSpaces(char * str);
short int nextQuote(char * str, int index);

#endif
