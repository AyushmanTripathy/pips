#ifndef CONFIG_H
#define CONFIG_H

short const int tab_space_count = 2;
short const int number_packet_count = 5;
short const int number_packet_size = 30;

short const int hashmap_size = 113;
short const int hashmap_r = 31;
short const int variable_hashmap_size = 29;
short const int variable_hashmap_r = 3;

const int selfMaximumCallCount = 100;

const char keywords[][5] = { "fn", "if", "elif", "else", "def" };
const short int keywordsLength = 5; 

#endif
