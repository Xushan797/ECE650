#include "my_malloc.h"





size_t total_size = 0;
size_t total_free_size = 0;
block_h * head = NULL;
int flag = 0;
int flag1 = 0;
int f = 0;
void printFree() {
    block_h * cur = head;
    printf("%ld\n", sizeof(block_h));
    printf("%d, try",flag);
    printf("%d, try",flag1);
    printf("%d, try",f);
    while(cur) {
        printf("this:%p, prev:%p, next:%p, size:%ld\n", cur, cur->prev, cur->next, cur->size);
        cur = cur->next;
    }
}


void *allocate_new_block(size_t size){

    total_size += size + sizeof(block_h);
    block_h * head_block = sbrk(size + sizeof(block_h));
    head_block->size = size;
    head_block->next = NULL;
    head_block->prev = NULL;
    return (void *)head_block;
}
block_h *find_first_free_block(size_t size){
    if (!head){
        return NULL;
    }
    block_h *cur = head;
    do {
        if (cur->size >= size){
            return cur;
        }
        else{
            cur = cur->next;
        }
    } while (cur);
    return cur;
}
void add_free_list(block_h * block){
    if(!head){
        head = block;
        return;
    }
    else{
        block_h * cur = head;
        do {
            if (block < cur && !cur->prev){
                head = block;
                block->next = cur;
                cur->prev = block;
                block->prev = NULL;
                return;
            }
            else if (cur < block &&  !cur->next){
                cur->next = block;
                block->prev = cur;
                block->next = NULL;
                return;
            } else if(cur < block && cur->next > block){
                block->next = cur->next;
                block->prev = cur;
                cur->next = block;
                block->next->prev = block;
                return;
            } else{
                cur = cur->next;
            }
        }while(cur);
    }
}
void delete_free_list(block_h * block){

    if(!block->next && !block->prev){
        head = NULL;
        block->next = NULL;
        block->prev = NULL;
    }
    else if(!block->next){
        block->prev->next = NULL;
        block->prev = NULL;
    }
    else if(!block->prev){
        head = block->next;
        block->next->prev = NULL;
        block->next = NULL;
    }
    else{
        block->next->prev = block->prev;
        block->prev->next = block->next;
        block->next = NULL;
        block->prev = NULL;
    }
}
void *split(block_h * cur,size_t size){
    block_h * free_block =(block_h *)((void*)cur + size + sizeof(block_h));
    add_free_list(free_block);
    delete_free_list(cur);
    free_block->size = cur->size - size - sizeof(block_h);
    cur->size = size;
    return (void *)cur;
}




void *ff_malloc(size_t size){
    block_h * cur = find_first_free_block(size);
    if (cur){
        if (cur->size - sizeof(block_h) > size ){
            //printf("split");
            total_free_size -= size + sizeof (block_h);
            return split(cur, size) + sizeof(block_h);
        }
        else{
            //printf("not split\n");
            delete_free_list(cur);
            total_free_size -= cur->size + sizeof (block_h);
            return (void *)cur + sizeof(block_h);
        }
    }
    else{
        return allocate_new_block(size) + sizeof(block_h);
    }
}

void merge_free_block(void *ptr){
    //printf("0 merge");
    block_h * cur = ptr;
    block_h * prev = cur->prev;
    flag1 += 1;
    f = 0;
//    //printf("%p   %p\n", cur, prev);
//    if(cur->next && ((char *) + cur->size + sizeof(block_h) == (char *)cur->next)) {
//        cur->size += sizeof(block_h) + cur->next->size;
//        delete_free_list(cur->next);
//        //p->next->next = NULL;
//        //p->next->prev = NULL;
//    }
//    if ((cur->prev != NULL) &&
//
//        ((char *)cur->prev + cur->prev->size + sizeof(block_h) == (char *)cur)) {
//        cur->prev->size += sizeof(block_h) + cur->size;
//        delete_free_list(cur);
//        //p->next = NULL;
//        //p->prev = NULL;
//    }
    if(cur->next && cur->next == (void*)ptr + cur->size + sizeof(block_h)) {
        flag += 1;
        block_h * next = cur->next->next;
        cur->size += cur->next->size +sizeof(block_h);
        cur->next = next;
        if(next) {
            next->prev = cur;
        }
    }
    if(prev && prev == (void*)ptr - prev->size - sizeof(block_h)) {
        f= 1;
        prev->size += cur->size +sizeof(block_h);
        prev->next = cur->next;
        if (cur->next) {
            cur->next->prev = prev;
        }
    }



}

void ff_free(void *ptr){
    block_h * cur = (block_h *)((void *)ptr - sizeof(block_h));
    total_free_size += cur->size +sizeof (block_h);
    add_free_list(cur);
    merge_free_block(cur);
}


block_h *find_best_free_block(size_t size){
    if(!head){
        return NULL;
    }
    block_h *cur = head;
    block_h * best_fit_block = NULL;
    do {
        if (cur->size >= size){
//            printf("cur is not null");
//            printf("%d\n",cur->is_free);
            if(cur->size == size){
                return cur;
            }
            if (!best_fit_block ){
            best_fit_block = cur;
            } else if(cur->size < best_fit_block->size){
                best_fit_block = cur;
            }
            cur = cur->next;

        }
        else{
            cur = cur->next;
        }
    } while (cur);
//    printf("cur is null");
    return best_fit_block;
}

void *bf_malloc(size_t size){
    block_h * cur = find_best_free_block(size);
    if (cur){
        if (cur->size - sizeof(block_h) > size ){
            //printf("split");
            total_free_size -= size + sizeof (block_h);
            return split(cur, size) + sizeof(block_h);
        }
        else{
            //printf("not split\n");
            delete_free_list(cur);
            total_free_size -= cur->size + sizeof (block_h);
            return (void *)cur + sizeof(block_h);
        }
    }
    else{
        return allocate_new_block(size) + sizeof(block_h);
    }
}
void bf_free(void *ptr){
    block_h * cur = (block_h *)(ptr - sizeof(block_h));
    total_free_size += cur->size + sizeof (block_h);
    add_free_list(cur);
    merge_free_block(cur);
}

unsigned long get_data_segment_size(){
    return total_size;
}
unsigned long get_data_segment_free_space_size(){
    return total_free_size;
}
