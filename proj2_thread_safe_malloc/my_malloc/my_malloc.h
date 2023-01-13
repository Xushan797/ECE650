#ifndef MY_MALLOC_MY_MALLOC2_H
#define MY_MALLOC_MY_MALLOC2_H
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

typedef struct block_header{
    size_t size;
    struct block_header * prev;
    struct block_header * next;
} block_h;

void *split(block_h * cur,size_t size, block_h ** head);
void *allocate_new_block(size_t size, int flag);
void merge_free_block(void *ptr);



block_h *find_best_free_block(size_t size,block_h ** head);
void *bf_malloc(size_t size,block_h ** head, int flag);
void bf_free(void *ptr,block_h ** head);

unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();

void printFree();


//Thread Safe malloc/free: locking version
void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

//Thread Safe malloc/free: non-locking version 
void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);
#endif //MY_MALLOC_MY_MALLOC2_H
