#ifndef TYPES_H
#define TYPES_H

typedef struct Token_tag {
  char type;
  char * data;
  int int_data;
  short int childTokensCount;
  struct Token_tag * next;
  struct Token_tag ** childTokens;
} Token;

typedef Token ** Tokens;

typedef struct TokenNode_tag {
  char type;
  char * data;
  struct TokenNode_tag * next;
} TokenNode;

typedef struct Variable_tag {
  char * key;
  char type;
  int int_data;
  struct Variable_tag * next;
} Variable;

typedef struct {
  char ** data;
  short int length;
} Strings;

typedef struct Function_tag {
  char * name;
  short int paramsCount;
  Token ** params;
  Token * execSeq;
  struct Function_tag * next;
} Function;

typedef struct FunctionPointer_tag {
  char * key;
  Token * (* pointer)(Token **, short int);
  struct FunctionPointer_tag * next;
} FunctionPointer;

#endif
