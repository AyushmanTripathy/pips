#ifndef CONFIG_H
#define CONFIG_H

int tab_space_count = 2;

int hashmap_size = 113;
int hashmap_r = 31;
int variable_hashmap_size = 29;
int variable_hashmap_r = 3;

int selfMaximumCallCount = 100;

const char keywords[][5] = { "fn", "if", "elif", "else", "def" };
const int keywordsLength = 5; 

#endif
