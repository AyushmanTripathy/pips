#ifndef TYPES_H
#define TYPES_H

typedef struct Token_tag {
  int type;
  char * data;
  int int_data;
  int childTokensCount;
  struct Token_tag * next;
  struct Token_tag ** childTokens;
} Token;

typedef struct TokenNode_tag {
  int type;
  char * data;
  struct TokenNode_tag * next;
} TokenNode;

typedef struct Variable_tag {
  char * key;
  int type;
  int int_data;
  struct Variable_tag * next;
} Variable;

typedef struct Function_tag {
  char * name;
  int paramsCount;
  Token ** params;
  Token * execSeq;
  Token * linker;
  struct Function_tag * next;
} Function;

typedef struct FunctionPointer_tag {
  char * key;
  Token * (* pointer)(Token *);
  struct FunctionPointer_tag * next;
} FunctionPointer;

#endif
