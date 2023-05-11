#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int tab_space_count = 2;
const char keywords[3][5] = { "fn", "if", "else" };
const int keywordsLength = 3;

typedef struct Line_tag {
  char * data;
  int indentation;
  struct Line_tag * next;
} Line;

typedef struct TokenNode_tag {
  int type;
  char * data;
  struct TokenNode_tag * next;
} TokenNode;

typedef struct Token_tag {
  int type;
  char * data;
  int childTokensCount;
  struct Token_tag * next;
  struct Token_tag ** childTokens;
} Token;

typedef struct {
  char * name;
  int paramsCount;
  Token ** params;
  Token * execSeq;
  Token * linker;
} Function;

typedef struct {
  Token * head;
} Scope;

void freeLine(Line * l) {
  l->next = NULL;
  free(l->data);
  free(l);
}

char * getErrorCode(int code) {
  switch (code) {
    case 1: return "PARSING";
    case 2: return "";
    case 3: return "RUNTIME";
    case 4: return "";
    case 5: return "";
    case 6: return "";
  }
  return NULL;
}
void printError(char * s, int code) {
  char * c = getErrorCode(code);
  if (c == NULL) printf("[ERROR] %s\n", s);
  else printf("[%s ERROR] %s\n", c, s);
  exit(1);
}

char * sliceStr(char * str, int start, int end) {
  char * newStr = (char *) malloc((end - start) * sizeof(char));
  for (int i = 0; (i + start) < end; i++) newStr[i] = str[start + i];
  newStr[end - start] = '\0';
  return newStr;
}

// trim function
int isTrimable(char s) {
  switch (s) {
    case ' ': return 1;
    case '\t': return 1;
    case '\n': return 1;
    case '\v': return 1;
    case '\f': return 1;
    case '\r': return 1;
  }
  return 0;
}

char * trimStr(char * str) {
  int length = strlen(str), index = 0;

  int startCount = 0, endCount = 0;
  for (int i = 0; i < length; i++) {
    if (isTrimable(str[i])) startCount++;
    else break;
  }
  for(int i = length - 1; i >= 0; i--) {
    if (isTrimable(str[i])) endCount++;
    else break;
  }
  if (startCount == length) return NULL;
  char * newStr = (char *) malloc((length - startCount - endCount + 1) * sizeof(char));
  endCount = length - endCount;
  for (int i = startCount; i < endCount; i++) newStr[index++] = str[i];
  newStr[index] = '\0'; 
  return newStr;
}

int calcSpaces(char * str) {
  int count = 0, len = strlen(str);
  for(int i = 0; i < len; i++) {
    if (str[i] == ' ') count++;
    else break;
  }
  return count; 
}

Line *readSourceCode(char * path) {
  // reading source code
  char buff[255];
  FILE *source = fopen(path, "r");
  Line * src = NULL;
  Line * head = NULL;

  while(fgets(buff, 255, source) != NULL) {
    // check for empty lines
    char * data = trimStr(buff);
    if (data == NULL) continue;

    Line * node = (Line *) malloc(sizeof(Line));
    node->next = NULL;
    node->indentation = calcSpaces(buff) / tab_space_count;
    node->data = data;

    if (src == NULL) src = head = node;
    else {
      head->next = node;
      head = node;
    }
  }
  fclose(source);

  if (src == NULL) printError("input file is empty.", 1);
  return src;
}

Line * cleanseLines(Line * head) {
  int isQuote = 0, escapedQuote = 0, isComment = 0; 
  Line * prev = NULL;
  Line * l = head;

  while (l != NULL) {
    int len = strlen(l->data), index = 0;
    char * newStr = (char *) malloc(len * sizeof(char));
    char * str = l->data;

    for(int i = 0; i < len; i++) {
      if (escapedQuote == 1) {
        isQuote = isQuote ? 0 : 1;
        escapedQuote = 0;
      }
      if (str[i] == '"') {
        if (isQuote) escapedQuote = 1;
        else isQuote = 1;
      }
      else if (str[i] == '#') isComment = 1;

      // handle quotes
      if (isQuote == 1);
      else if (isComment == 1);
      else newStr[index++] = str[i]; 
    }
    newStr[index] = '\0';
    isComment = 0;
  
    // check for empty lines
    if (strlen(newStr) == 0) {
      if (prev == NULL) head = l->next;
      else prev->next = l->next;
      Line * next = l->next;
      freeLine(l);
      free(newStr);
      l = next;
    } else {
      free(str);
      l->data = newStr;
      prev = l;
      l = l->next;
    }
  }
  if (isQuote) printError("Quote not escapped.", 1);
  return head;
}

int isKeyword(char * s) {
  for (int i = 0; i < keywordsLength; i++) {
    if (!strcmp(s, keywords[i])) return -250 + i;
  }
  return 0;
}

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

int isSymbol(char s) {
  switch (s) {
    case ':': return 1;
    case ']': return 1;
    case '[': return 1;
    case '|': return 1;
    case '(': return 1;
    case ')': return 1;
  }
  return 0;
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

  if (childCount > 0) 
    t->childTokens = (Token **) malloc(childCount * sizeof(Token *));
  else t->childTokens = NULL;

  t->type = type;
  t->data = data;
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
  else printf("%s\n", n->data);

  if (n->type == -10) {
    Token ** childrens = n->childTokens;
    for (int i = 0; i < n->childTokensCount; i++) {
      printTokenTree(childrens[i] , depth + 1);
    }
  }
  if (n->type == -21) {
    Token ** childrens = n->childTokens;
    printTokenTree(childrens[0] , depth + 1);
    printTokenTree(childrens[1] , depth + 1);
  }
  if (n->next == NULL) return;
  printf("\n");
  printTokenTree(n->next, depth);
}

int analyseKeywords(TokenNode * t) {
  return 0;
}

Function * parseFunction(TokenNode * n) {
  Function * fn = (Function *) malloc(sizeof(Function));

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
      params[i] = createToken(-11, iterator->data, 0);
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

Token * parseIf(TokenNode * n) {
  Token * ifToken = createToken(-21, "IF", 2);

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
  
  if (colonPointer == NULL) printError(": missing in if statment.", 0);
  else if (colonPointer->next != NULL) {
    (ifToken->childTokens)[1] = analyseTokenNode(colonPointer->next);
  } else (ifToken->childTokens)[1] = NULL;

  if (n->next != NULL) {
    TokenNode * passFunc = createTokenNode("pass", 0);
    passFunc->next = n->next;
    (ifToken->childTokens)[0] = analyseTokenNode(passFunc);
  } else printError("condition missing in if statment.", 0);

  return ifToken;
}

Token * classifyScopes(Line * line) {
  int indent = line->indentation, isIf = 0; 
 
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
            break;
          case -249:
            curr->next = parseIf(n);
            curr = curr->next;

            if ((curr->childTokens)[1] == NULL) {
              // problem dectected here.
              Token * p = classifyScopes(line->next);
            }
            break;
        }
      } else {
        if (head == NULL) {
          curr = analyseTokenNode(n); 
          head = curr;
        } else {
          curr->next = analyseTokenNode(n);
          curr = curr->next;
        }
      }
    }
    line = line->next;
  }

  printf("---\n");
  return head;
}

int main(int argc, char *argv[]) {
  Line * line = readSourceCode(argv[1]);
  line = cleanseLines(line);
  if (line == NULL) return 0;

  if (line->indentation > 0) printError("Cannot start from non global scope.", 0);

  Token * global = classifyScopes(line);
}
