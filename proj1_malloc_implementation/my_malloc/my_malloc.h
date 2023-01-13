#ifndef MY_MALLOC_MY_MALLOC2_H
#define MY_MALLOC_MY_MALLOC2_H
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
typedef struct block_header{
    size_t size;
    struct block_header * prev;
    struct block_header * next;
} block_h;

void *init_list(size_t size);
block_h *find_first_free_block(size_t size);
void *split(block_h * cur,size_t size);
void *allocate_new_block(size_t size);
void merge_free_block(void *ptr);

void *ff_malloc(size_t size);
void ff_free(void *ptr);


block_h *find_best_free_block(size_t size);
void *bf_malloc(size_t size);
void bf_free(void *ptr);

unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();

void printFree();

#endif //MY_MALLOC_MY_MALLOC2_H
