#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "reader.h"
#include "utils.h"

Token * parseFile(char * name, Function ** functions);

#endif