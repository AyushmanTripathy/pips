#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"

extern Strings * strings;
extern TokenNode* createTokenNode(char *, int);
extern Token* createToken(int, char *, int);
extern void addToFunctions(Function **, Function *);

extern void printTokenTree(Token *, int);
extern void printTokenNode(TokenNode *);
extern void printError(char *, int);

void freeTokenNode(TokenNode * n) {
  TokenNode * next;
  while(n != NULL) {
    next = n->next;
    free(n);
    n = next;
  }
}

TokenNode * parseLine(Line * l) {
  TokenNode head = { next : NULL };
  TokenNode * t = &head;

  char * str = l->data;
  int len = strlen(str), start = 0;
  for(int i = 0; i < len; i++) {
    if (isSymbol(str[i])) {
      char * s = trimStr(sliceStr(str, start, i));
      if (s != NULL) {
        t->next = createTokenNode(s, 0);
        t = t->next;
      }
      t->next = createTokenNode(NULL, (int) str[i]);
      start = i + 1;
    } else if (str[i] == ' ' ) {
      char * s = trimStr(sliceStr(str, start, i + 1));
      if (s != NULL) t->next = createTokenNode(s, 0);
      start = i;
    } else if (i + 1 == len) {
      char * s = trimStr(sliceStr(str, start, i + 1));
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
  if (isBlock != 0) printError(" ] Missing!", 1);
  return count;
}

Token * analyseTokenNode(TokenNode * t) {
  int childCount = getChildCount(t);
  if (t->type > 0) printError("Expected Function.", 1);
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
          if (t->next == NULL) printError("Empty pipe.", 1);
          else if (t->next->type == (int) '|') printError("Double pipe.", 1);
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
      } else if (t->type == 0) {
        Token * pass = createToken(-10, "%", 1);
        pass->childTokens[0] = createToken(t->type, t->data, 0);
        childrens[i++] = pass;
      } else childrens[i++] = createToken(t->type, t->data, 0);
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
    else printError("Name is required while declaring function", 0);
  } else printError(": missing while declaring function", 0);

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
        printError("Name Token expected as function parameter", 1);
      params[i] = createToken(iterator->type, iterator->data, 0);
      iterator = iterator->next;
    }
    fn->params = params;
  }

  fn->execSeq = NULL;
  if (iterator->type != 58) printError(": missing while declaring function",0);
  if (iterator->next == NULL) return fn;
  fn->execSeq = analyseTokenNode(iterator->next);
  return fn;
}

Token * parseIfStatment(TokenNode * n, int isElif) {
  Token * ifToken = isElif ? createToken(-22, "ELIF", 2): createToken(-21, "IF", 2);

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
  
  if (colonPointer == NULL) printError(": missing in conditional statment.", 0);
  else if (colonPointer->next != NULL) {
    (ifToken->childTokens)[1] = analyseTokenNode(colonPointer->next);
  } else (ifToken->childTokens)[1] = NULL;

  if (n->next != NULL) {
    TokenNode * passFunc = createTokenNode("bool", 0);
    passFunc->next = n->next;
    (ifToken->childTokens)[0] = analyseTokenNode(passFunc);
    // for freeing passFunc TokenNode
    n = passFunc;
  } else printError("condition missing in conditional statment.", 0);

  return ifToken;
}

Token * parseElseStatment(TokenNode * n) {
  Token * elseToken = createToken(-23, "ELSE", 1);

  TokenNode * colonPointer = NULL;
  while(n->next != NULL)  {
    if ((n->next)->type == 58) {
      colonPointer = n->next;
      break;
    }
    n = n->next;
  }
  
  if (colonPointer == NULL) printError(": missing in else statment.", 0);
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
  if (paramsCount == 0) printError("Empty Definition.", 1);
  paramsCount *= 2;

  def->params = (Token **) malloc(paramsCount * sizeof(Token *));
  def->paramsCount = -1 * paramsCount;

  int index = 0;
  while(line != NULL) {
    if (expectedIndent != line->indentation) break;
    TokenNode * n = parseLine(line);
    TokenNode * pattern = n;
    if (n->next == NULL) printError("Invalid pattern.", 1);
    while(n->next->type != 58) {
      n = n->next;
      if (n->next == NULL) printError("Expected :.", 1);
    }
    TokenNode * execSeq = n->next->next;
    if (execSeq == NULL) printError("Tokens expected after :.", 1);
    free(n->next);
    n->next = NULL;

    def->params[index++] = parseDefPattern(pattern);
    def->params[index++] = analyseTokenNode(execSeq);
    line = line->next;
    freeTokenNode(pattern);
    freeTokenNode(execSeq);
  }
  return def;
}

Token * classifyScopes(Line * line, Function ** functions) {
  int indent = line->indentation; 
 
  Function * fn = NULL;
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
            if (indent != 0) 
              printError("Functions cannot be defined in non global scope.", 3);
            fn = parseFunction(n);
            if(fn->execSeq == NULL )
              fn->execSeq = classifyScopes(line->next, functions);
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
              else printError("Empty conditional block", 1);
            }
            break;
          case -248:
            if (curr == NULL) printError("if statement expected.", 1);
            curr->next = parseIfStatment(n, 1);
            curr = curr->next;
            if ((curr->childTokens)[1] == NULL)
              if (line->next->indentation > indent)
              (curr->childTokens)[1] = classifyScopes(line->next, functions);
              else printError("Empty conditional block", 1);
            break;
          case -247:
            if (curr == NULL) printError("if statement expected.", 1);
            curr->next = parseElseStatment(n);
            curr = curr->next;
            if ((curr->childTokens)[0] == NULL)
              if (line->next->indentation > indent)
              (curr->childTokens)[0] = classifyScopes(line->next, functions);
              else printError("Empty conditional block", 1);
            break;
          case -246:
            if (n->next == NULL || n->next->data == NULL)
              printError("name is required to define a function.", 1);
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
  Line * line = readSourceCode(path);

  strings = (Strings *) malloc(sizeof(Strings));
  strings->length = 0;
  line = cleanseLines(line, strings);

  if (line == NULL) return 0;
  if (line->indentation > 0) printError("Cannot start from non global scope.", 0);

  Token * global = classifyScopes(line, functions);
  freeLines(line);
  return global;
}
