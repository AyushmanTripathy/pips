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

#endif
