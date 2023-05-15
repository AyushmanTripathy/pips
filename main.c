#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int tab_space_count = 2;
int hashmap_size = 113;
int hashmap_r = 31;

const char keywords[][5] = { "fn", "if", "elif", "else" };
const int keywordsLength = 4;

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
  int int_data;
  int childTokensCount;
  struct Token_tag * next;
  struct Token_tag ** childTokens;
} Token;

typedef struct Function_tag {
  char * name;
  int paramsCount;
  Token ** params;
  Token * execSeq;
  Token * linker;
  struct Function_tag * next;
} Function;

typedef struct {
  int size;
  int r;
  Function ** buckets;
} FunctionHashMap;

typedef struct FunctionPointer_tag {
  char * key;
  Token * (* pointer)(Token *);
  struct FunctionPointer_tag * next;
} FunctionPointer;

void freeLine(Line * l) {
  l->next = NULL;
  free(l->data);
  free(l);
}

void freeLines(Line * l) {
  if (l == NULL) return;
  if (l->next != NULL) freeLines(l->next);
  freeLine(l);
}

char * getErrorCode(int code) {
  switch (code) {
    case 1: return "PARSING";
    case 2: return "";
    case 3: return "RUNTIME";
    case 4: return "TYPE";
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

// HASHMAP
int hashFunc(char * str, int size, int r) {
  int hash = 0;
  for (int i = 0; str[i] != '\0'; i++)
    hash = ((r * hash) + (int) str[i]) % size;
  return hash;
}

FunctionHashMap * initFunctionHashMap(int size, int r) {
  FunctionHashMap * map = (FunctionHashMap *) malloc(sizeof(FunctionHashMap));
  map->buckets = (Function **) malloc(size * sizeof(Function *));
  for (int i = 0; i < size; i++) (map->buckets)[i] = NULL;
  map->size = size;
  map->r = r;
  return map;
}

void addToFunctionHashMap(FunctionHashMap * map, Function * fn) {
  int hashKey = hashFunc(fn->name, map->size, map->r);
  Function ** buckets = map->buckets;
  if (buckets[hashKey] == NULL) buckets[hashKey] = fn;
  else {
    Function * iterator = buckets[hashKey];
    while(iterator->next != NULL) iterator = iterator->next;
    iterator->next = fn;
  }
}

Function * getFromFunctionHashMap(FunctionHashMap * map, char * name) {
  int hashKey = hashFunc(name, map->size, map->r);
  if ((map->buckets)[hashKey] != NULL) {
    Function * iterator = (map->buckets)[hashKey];
    while(strcmp(iterator->name, name) != 0) {
      iterator = iterator->next;
      if (iterator == NULL) return NULL; 
    }
    return iterator;
  } else return NULL; 
}

FunctionPointer ** initFunctionPointers() {
  FunctionPointer ** arr 
    = (FunctionPointer **) malloc(hashmap_size * sizeof(FunctionPointer));
  for (int i = 0; i < hashmap_size; i++) arr[i] = NULL;
  return arr;
}

void addToFunctionPointers(FunctionPointer ** map, char * key, Token* (*fn)(Token*)) {
  int hashKey = hashFunc(key, hashmap_size, hashmap_r);

  FunctionPointer * fp = (FunctionPointer *) malloc(sizeof(FunctionPointer));
  fp->next = NULL;
  fp->key = key;
  fp->pointer = fn;

  if (map[hashKey] == NULL) map[hashKey] = fp;
  else {
    FunctionPointer * iterator = map[hashKey];
    while (iterator->next != NULL) iterator = iterator->next;
    iterator->next = fp;
  }
}

FunctionPointer * getFromFunctionPointers(FunctionPointer ** map, char * key) {
  int hashKey = hashFunc(key, hashmap_size, hashmap_r);

  if (map[hashKey] != NULL) {
    FunctionPointer * iterator = map[hashKey];
    while(strcmp(iterator->key, key) != 0) {
      iterator = iterator->next;
      if (iterator == NULL) return NULL; 
    }
    return iterator;
  } else return NULL; 
}


// STRING METHODS
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

  if (source == NULL) printError("Invalid File Name", 0);
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

int nextQuote(char * str, int index) {
  int counter = 0;
  for (int i = index; str[i] != '"'; i++) {
    if (str[i] == '\0') printError("Unescaped Quote", 1);
    counter++;
  }
  return counter;
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
      } else if (str[i] == '#') isComment = 1;

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

// PRINT FUNCTIONS
void printLines(Line * line) {
  while (line != NULL) {
    printf("%s \n", line->data);
    line = line->next;
  }
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

Token * classifyScopes(Line * line, FunctionHashMap * functions) {
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
            addToFunctionHashMap(functions, fn);
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

// DEF FUNCTIONS
Token * pass(Token * t) {
  return t;
}

Token * add(Token * t) {
  int sum = 0;
  Token ** args = t->childTokens;
  for (int i = 0; i < t->childTokensCount; i++) {
    if (args[i]->type != -1) printError("add function", 4);
    sum += args[i]->int_data;
  }
  return createToken(-1, NULL, sum);
}

Token * subtract(Token * t) {
  int sum = 0;
  Token ** args = t->childTokens;
  for (int i = 0; i < t->childTokensCount; i++) {
    if (args[i]->type != -1) printError("subtract function", 4);
    else if (i == 0) sum = args[i]->int_data;
    sum = sum - args[i]->int_data;
  }
  return createToken(-1, NULL, sum);
}

Token * print(Token * t) {
  Token ** args = t->childTokens;
  for (int i = 0; i < t->childTokensCount; i++) {
    if (args[i]->type == -1) printf("%d ", args[i]->int_data);
    else printf("%s ", args[i]->data);
  }
  printf("\n");
  return createToken(-5, NULL, 0);
}

Token * execStatment(Token * t, FunctionHashMap * functions, FunctionPointer ** defs);

Token * execFunction(Function * fn, FunctionHashMap * functions, FunctionPointer ** defs) {
  Token * iterator = fn->execSeq;
  while(iterator != NULL) {
    execStatment(iterator, functions, defs);
    iterator = iterator->next;
  }
  return NULL;
}

Token * execStatment(Token * t, FunctionHashMap * functions, FunctionPointer ** defs) {
  Token ** children = t->childTokens;
  for (int i = 0; i < t->childTokensCount; i++) {
    if ((children[i])->type == -10) 
      children[i] = execStatment(children[i], functions, defs);
  }
  FunctionPointer * fp = getFromFunctionPointers(defs, t->data);
  if (fp == NULL) {
    Function * fn = getFromFunctionHashMap(functions, t->data);
    if(fn == NULL) printError("Function not found!", 3);
    return execFunction(fn, functions, defs);
  }
  return (fp->pointer)(t);
}

Function * classifyGlobalScope(Line * line, FunctionHashMap * functions) {
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

  FunctionHashMap * functions = initFunctionHashMap(hashmap_size, hashmap_r);
  Function * global = classifyGlobalScope(line, functions);
  freeLines(line);

  printTokenTree(global->execSeq, 0);
  execFunction(global, functions, defs);
}
