#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

TokenNode * createTokenNode(char * str, int type) {
  TokenNode * t = (TokenNode *) malloc(sizeof(TokenNode)); 
  t->data = str;

  if (!type) {
    if (str[0] >= '0' && str[0] <= '9') type = -1;
    else type = isKeyword(str); 
  } 

  t->type = type;
  t->next = NULL;
  return t;
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

Token * createToken(int type, char * data, int childCount) {
  Token * t = (Token *) malloc(sizeof(Token));

  if (type == -1) {
    t->type = -1;
    t->int_data = childCount == 0 ? atoi(data): childCount;
    t->data = NULL;
    t->childTokens = NULL;
    t->childTokensCount = 0;
    return t;
  }

  if (childCount > 0) 
    t->childTokens = (Token **) malloc(childCount * sizeof(Token *));
  else t->childTokens = NULL;
  t->type = type;
  t->data = data;
  t->int_data = 0;
  t->childTokensCount = childCount;
  return t;
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
  if (isBlock != 0) printError("] Missing!", 0);
  return count;
}

Token * analyseTokenNode(TokenNode * t) {
  int childCount = getChildCount(t);
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
          if (t->next == NULL) printError("Empty pipe.", 0);
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
      childrens[i++] = createToken(t->type, t->data, 0);
    }
    t = t->next;
  }
  return root;
}

void printTokenNode(TokenNode * n) {
  while(n != NULL) {
    if (n->data) printf("(%s)-> ", n-> data);
    else printf("[%d]-> ", n->type);
    n = n->next;
  }
  printf("\n");
}

void printTokenTree(Token * n, int depth) {
  if (n == NULL) return;
  for (int i = 1; i < depth; i++) printf("  ");
  if (depth != 0) printf("|-");
  if(n->type == -10) printf("[%s]\n", n->data);
  else if(n->type == -1) printf("%d\n", n->int_data);
  else printf("%s\n", n->data);

  if (n->childTokensCount != 0) {
    Token ** childrens = n->childTokens;
    for (int i = 0; i < n->childTokensCount; i++) {
      printTokenTree(childrens[i] , depth + 1);
    }
  }

  if (n->next == NULL) return;
  printTokenTree(n->next, depth);
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
      params[i] = createToken(-12, iterator->data, 0);
      iterator = iterator->next;
    }
  }

  fn->execSeq = NULL;
  fn->linker = NULL;
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
    TokenNode * passFunc = createTokenNode("pass", 0);
    passFunc->next = n->next;
    (ifToken->childTokens)[0] = analyseTokenNode(passFunc);
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
            fn = parseFunction(n);
            if(fn->execSeq == NULL )
              fn->execSeq = classifyScopes(line->next, functions);
            addToFunctions(functions, fn);
            break;
          case -249:
            curr->next = parseIfStatment(n, 0);
            curr = curr->next;
            if ((curr->childTokens)[1] == NULL)
              (curr->childTokens)[1] = classifyScopes(line->next, functions);
            break;
          case -248:
            curr->next = parseIfStatment(n, 1);
            curr = curr->next;
            if ((curr->childTokens)[1] == NULL)
              (curr->childTokens)[1] = classifyScopes(line->next, functions);
            break;
          case -247:
            curr->next = parseElseStatment(n);
            curr = curr->next;
            if ((curr->childTokens)[0] == NULL)
              (curr->childTokens)[0] = classifyScopes(line->next, functions);
            break;
        }
      } else {
        if (head == NULL) {
          curr = analyseTokenNode(n); 
          curr->next = NULL;
          head = curr;
        } else {
          printTokenNode(n);
          curr->next = analyseTokenNode(n);
          curr = curr->next;
        }
      }
      free(n);
    }
    line = line->next;
  }
  return head;
}

Token * execStatment(Token * t, Function ** functions, FunctionPointer ** defs);
Token * execFunction(Function * fn, Function ** functions, FunctionPointer ** defs) {
  Token * iterator = fn->execSeq;
  while(iterator != NULL) {
    execStatment(iterator, functions, defs);
    iterator = iterator->next;
  }
  return NULL;
}

Token * execStatment(Token * t, Function ** functions, FunctionPointer ** defs) {
  Token ** children = t->childTokens;
  for (int i = 0; i < t->childTokensCount; i++) {
    if ((children[i])->type == -10) 
      children[i] = execStatment(children[i], functions, defs);
  }
  FunctionPointer * fp = getFromFunctionPointers(defs, t->data);
  if (fp == NULL) {
    Function * fn = getFromFunctions(functions, t->data);
    if(fn == NULL) printError("Function not found!", 3);
    return execFunction(fn, functions, defs);
  }
  return (fp->pointer)(t);
}

Function * classifyGlobalScope(Line * line, Function ** functions) {
  Function * global = (Function *) malloc(sizeof(Function));
  global->name = "global";
  global->paramsCount = 0;
  global->execSeq = classifyScopes(line, functions);
  return global;
}

int main(int argc, char *argv[]) {
  Line * line = readSourceCode(argv[1]);
  line = cleanseLines(line);
  if (line == NULL) return 0;
  if (line->indentation > 0) printError("Cannot start from non global scope.", 0);

  FunctionPointer ** defs = initFunctionPointers();
  addToFunctionPointers(defs, "add", &add);
  addToFunctionPointers(defs, "print", &print);
  addToFunctionPointers(defs, "subtract", &print);

  Function ** functions = initFunctionHashMap();
  Function * global = classifyGlobalScope(line, functions);
  freeLines(line);

  printTokenTree(global->execSeq, 0);
  //execFunction(global, functions, defs);
}
