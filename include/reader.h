#ifndef READER_H
#define READER_H

#include "types.h"

typedef struct Line_tag {
  char * data;
  int indentation;
  struct Line_tag * next;
} Line;

Line * readFile(char *);
void freeLines(Line *);
Line * cleanseLines(Line *, Strings *);

#endif
