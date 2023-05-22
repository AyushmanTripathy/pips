#ifndef READER_H
#define READER_H

#include "types.h"

typedef struct Line_tag {
  char * data;
  int indentation;
  struct Line_tag * next;
} Line;

Line * readSourceCode(char *);
Line * cleanseLines(Line *, Strings *);

void freeLine(Line *);
void freeLines(Line *);
void printLines(Line *);

#endif
