#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"

extern Strings * strings;

extern void error(char *, int);
extern Token* createToken(int, char *, int);
extern void freeTokenTree(Token *);

extern void addToFunctions(Function **, Function *);
extern void printTokenTree(Token *, int);
extern void printTokenNode(TokenNode *);

TokenNode * createTokenNode(char * str, int type) {
  TokenNode * t = (TokenNode *) malloc(sizeof(TokenNode)); 
  t->data = str;

  if (!type) {
    if (isNumber(str)) type = -1;
    else {
      type = isKeyword(str); 
      if (type != 0) {
        free(str);
        t->data = NULL;
      }
    }
  } 

  t->type = type;
  t->next = NULL;
  return t;
}

void freeTokenNode(TokenNode * n) {
  TokenNode * next;
  while(n != NULL) {
    next = n->next;
    free(n);
    n = next;
  }
}

char * getTrimedString(char * str, int start, int end) {
  char * sliced = sliceStr(str, start, end);
  char * trimed = trimStr(sliced);
  free(sliced);
  return trimed;
}

TokenNode * parseLine(Line * l) {
  TokenNode head = { next : NULL };
  TokenNode * t = &head;

  char * str = l->data;
  int len = strlen(str), start = 0;
  for(int i = 0; i < len; i++) {
    if (isSymbol(str[i])) {
      char * s = getTrimedString(str, start, i);
      if (s != NULL) {
        t->next = createTokenNode(s, 0);
        t = t->next;
      }
      t->next = createTokenNode(NULL, (int) str[i]);
      start = i + 1;
    } else if (str[i] == ' ' ) {
      char * s = getTrimedString(str, start, i + 1);
      if (s != NULL) t->next = createTokenNode(s, 0);
      start = i;
    } else if (i + 1 == len) {
      char * s = getTrimedString(str, start, i + 1);
      if (s != NULL) t->next = createTokenNode(s, 0);
      start = i;
    }
    if(t->next != NULL) t = t->next;
  }
  return head.next; 
}

int getChildCount(TokenNode * t) {
  int count = -1;
  int isBlock = 0;
  while (t != NULL) {
    if (t->type > 0) {
      switch ((char) t->type) {
        case '|':
          if (isBlock != 0) break; 
          return count + 1;
        case '[':
          if (isBlock == 0) count++;
          isBlock++;
          break;
        case ']':
          if (!isBlock) return count;
          else isBlock--;
          break;
      }
    } else if (!isBlock) count++;
    t = t->next;
  }
  if (isBlock != 0) error(" ] Missing!", 1);
  return count;
}

Token * analyseTokenNode(TokenNode * t) {
  int childCount = getChildCount(t);
  if (t->type > 0) error("Expected Function.", 1);
  Token * root = createToken(-10, t->data, childCount);
  t = t->next;
  if (t == NULL) return root;

  int i = 0, isBlock = 0; 
  Token ** childrens = root->childTokens;
  while(t != NULL) {
    if (t->type > 0) {
      switch ((char) t->type) {
        case '|':
          if (isBlock != 0) break;
          if (t->next == NULL) error("Empty pipe.", 1);
          else if (t->next->type == (int) '|') error("Double pipe.", 1);
          childrens[i++] = analyseTokenNode(t->next);
          return root;
        case '[':
          if (isBlock == 0)
            childrens[i++] = analyseTokenNode(t->next); 
          isBlock++;
          break;
        case ']':
          isBlock--;
          if(isBlock < 0) return root;
          break;
      }
    } else if (isBlock == 0) {
      // variables
      if (t->data[0] == '#') {
        int index = ((int) t->data[1]) - 33;
        childrens[i++] = createToken(-2, NULL, index);
        free(t->data);
        t->data = NULL;
      } else if (t->type == 0) {
        Token * pass = createToken(-10, mallocStr("%"), 1);
        pass->childTokens[0] = createToken(t->type, t->data, 0);
        childrens[i++] = pass;
      } else {
        childrens[i++] = createToken(t->type, t->data, 0);
        t->data = NULL;
      }
    }
    t = t->next;
  }
  return root;
}

Function * parseFunction(TokenNode * n) {
  Function * fn = (Function *) malloc(sizeof(Function));
  fn->next = NULL;

  // get name
  if (n->next != NULL)  {
    n = n->next;
    if (n->data != NULL && !n->type) fn->name = n->data;
    else error("Name is required while declaring function", 0);
  } else error(": missing while declaring function", 0);

  // get params
  TokenNode * iterator = n;
  int paramsCount = -1;
  while(iterator != NULL && iterator->type != 58) {
    paramsCount++;
    iterator = iterator->next;
  }
  fn->paramsCount = paramsCount;
  if  (paramsCount == 0) fn->params = NULL;
  else {
    Token ** params = (Token **) malloc(sizeof(Token));
    iterator = n->next;
    for(int i = 0; i < paramsCount; i++) {
      if (iterator->type != 0)
        error("Name Token expected as function parameter", 1);
      params[i] = createToken(iterator->type, iterator->data, 0);
      iterator = iterator->next;
    }
    fn->params = params;
  }

  fn->execSeq = NULL;
  if (iterator->type != 58) error(": missing while declaring function",0);
  if (iterator->next == NULL) return fn;
  fn->execSeq = analyseTokenNode(iterator->next);
  return fn;
}

Token * parseIfStatment(TokenNode * n, int isElif) {
  Token * ifToken = isElif ?
    createToken(-22, NULL, 2):
    createToken(-21, NULL, 2);

  TokenNode * colonPointer = NULL;
  TokenNode * iterator = n;

  while(iterator->next != NULL)  {
    if ((iterator->next)->type == 58) {
      colonPointer = iterator->next;
      iterator->next = NULL;
      break;
    }
    iterator = iterator->next;
  }
  
  if (colonPointer == NULL) error(": missing in conditional statment.", 0);
  else if (colonPointer->next != NULL) {
    (ifToken->childTokens)[1] = analyseTokenNode(colonPointer->next);
  } else (ifToken->childTokens)[1] = NULL;
  freeTokenNode(colonPointer);

  if (n->next != NULL) {
    TokenNode * passFunc = createTokenNode(mallocStr("bool"), 0);
    passFunc->next = n->next;
    (ifToken->childTokens)[0] = analyseTokenNode(passFunc);
    // for freeing passFunc TokenNode
    passFunc->next = NULL;
    freeTokenNode(passFunc);
  } else error("condition missing in conditional statment.", 0);

  return ifToken;
}

Token * parseElseStatment(TokenNode * n) {
  Token * elseToken = createToken(-23, NULL,1);

  TokenNode * colonPointer = NULL;
  while(n->next != NULL)  {
    if ((n->next)->type == 58) {
      colonPointer = n->next;
      break;
    }
    n = n->next;
  }
  
  if (colonPointer == NULL) error(": missing in else statment.", 0);
  else if (colonPointer->next != NULL) {
    (elseToken->childTokens)[0] = analyseTokenNode(colonPointer->next);
  } else (elseToken->childTokens)[0] = NULL;
  return elseToken;
}

Token * parseDefPattern(TokenNode * n) {
  Token * head = NULL;
  Token * curr = NULL;
  while (n != NULL) {
    if (curr == NULL) {
      curr = createToken(n->type, n->data, 0);
      head = curr;
    } else {
      curr->next = createToken(n->type, n->data, 0);
      curr = curr->next;
    }
    n = n->next;
  }
  return head;
}

Function * parseDef(char * name, Line * line,int expectedIndent) {
  Function * def = (Function *) malloc(sizeof(Function));
  def->name = name;
  def->next = NULL;
  def->execSeq = NULL;

  int paramsCount = 0;
  Line * iterator = line;
  while (iterator != NULL) {
    if (expectedIndent == iterator->indentation) paramsCount++;
    else if (expectedIndent > iterator->indentation) break;
    iterator = iterator->next;
  }
  if (paramsCount == 0) error("Empty Definition.", 1);
  def->params = (Token **) malloc(paramsCount * sizeof(Token *));
  def->paramsCount = -1 * paramsCount;

  int index = 0;
  Token * guardPosition = NULL;
  while(line != NULL) {
    if (expectedIndent > line->indentation) break;
    TokenNode * n = parseLine(line);
    TokenNode * pattern = n;
    if (n->next == NULL) error("Invalid pattern.", 1);
    while(n->next->type != 58) {
      n = n->next;
      if (n->next == NULL) error("Expected :.", 1);
    }
    TokenNode * execSeq = n->next->next;
    if (execSeq == NULL) error("Tokens expected after :.", 1);
    free(n->next);
    n->next = NULL;

    if (expectedIndent == line->indentation) {
      Token * patternToken = createToken(-12, NULL, 2);
      patternToken->childTokens[0] = parseDefPattern(pattern);
      patternToken->childTokens[1] = analyseTokenNode(execSeq);
      def->params[index++] = patternToken;
      guardPosition = patternToken;
    } else {
      Token * guard = createToken(-12, NULL, 2);
      guard->childTokens[0] = analyseTokenNode(pattern);
      guard->childTokens[1] = analyseTokenNode(execSeq);
      guardPosition->next = guard;
      guardPosition = guard;
    }
    line = line->next;
    freeTokenNode(pattern);
    freeTokenNode(execSeq);
  }
  return def;
}

void classifyScopesError(char * msg, TokenNode * n, Token * t) {
  freeTokenNode(n);
  freeTokenTree(t);
  error(msg, 1);
}

Token * classifyScopes(Line * line, Function ** functions) {
  int indent = line->indentation; 
 
  Token * head = NULL;
  Token * curr = NULL;
  while(line != NULL) {
    if (indent > line->indentation) return head;
    else if (indent < line->indentation);
    else {
      TokenNode * n = parseLine(line);
      if (n->type <= -200) {
        switch (n->type) {
          case -250:
            Function * fn = NULL;
            if (indent != 0) 
              classifyScopesError("Functions declared in non global scope.", n, head);
            fn = parseFunction(n);
            if(fn->execSeq == NULL) {
              if (line->next->indentation <= indent) 
                classifyScopesError("Empty Function.", n, head);
              fn->execSeq = classifyScopes(line->next, functions);
            }
            addToFunctions(functions, fn);
            break;
          case -249:
            if (head == NULL) curr = parseIfStatment(n, 0);
            else {
              curr->next = parseIfStatment(n, 0);
              curr = curr->next;
            }
            if ((curr->childTokens)[1] == NULL) {
              if (line->next->indentation > indent)
              (curr->childTokens)[1] = classifyScopes(line->next, functions);
              else classifyScopesError("Empty conditional block", n, head);
            }
            break;
          case -248:
            if (curr == NULL) classifyScopesError("if statement expected.", n, head);
            curr->next = parseIfStatment(n, 1);
            curr = curr->next;
            if ((curr->childTokens)[1] == NULL)
              if (line->next->indentation > indent)
              (curr->childTokens)[1] = classifyScopes(line->next, functions);
              else classifyScopesError("Empty conditional block", n, head);
            break;
          case -247:
            if (curr == NULL) classifyScopesError("if statement expected.", n, head);
            curr->next = parseElseStatment(n);
            curr = curr->next;
            if ((curr->childTokens)[0] == NULL)
              if (line->next->indentation > indent)
              (curr->childTokens)[0] = classifyScopes(line->next, functions);
              else classifyScopesError("Empty conditional block", n, head);
            break;
          case -246:
            if (n->next == NULL || n->next->data == NULL)
              classifyScopesError("name is required to define a function.", n, head);
            Function * def = parseDef(n->next->data, line->next, indent + 1);
            addToFunctions(functions, def);
            break;
        }
      } else {
        if (curr == NULL) {
          curr = analyseTokenNode(n); 
          curr->next = NULL;
        } else {
          curr->next = analyseTokenNode(n);
          curr = curr->next;
        }
      }
      if (head == NULL) head = curr;
      freeTokenNode(n);
    }
    line = line->next;
  }
  return head;
}

Token * parseFile(char * path, Function ** functions) {
  Line * line = readFile(path);

  strings = (Strings *) malloc(sizeof(Strings));
  strings->length = 0;
  line = cleanseLines(line, strings);

  if (line == NULL) return 0;
  if (line->indentation > 0) error("Cannot start from non global scope.", 0);

  Token * global = classifyScopes(line, functions);
  freeLines(line);
  return global;
}
