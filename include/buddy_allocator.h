#ifndef __BUDDY_ALLOCATOR_H__
#define __BUDDY_ALLOCATOR_H__

#include "slab_allocator.h"
#include "util/spinlock.h"

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif // PAGE_SIZE
#define MAX_CHUNK_SIZE 65536

struct chunk;

typedef struct chunk {
    void *addr;
    struct chunk *next;
} chunk_t;

typedef struct free_list {
    chunk_t **lists;
} free_list_t;


/******************
 * Global structs *
 ******************/
chunk_t *all_chunks;
free_list_t *free_list;

slab_allocator_t buddy_slab_allocator;
slab_allocator_t chunk_slab_allocator;

spinlock_t mm_buddy_mutex;

/*************************
 * Function declarations *
 *************************/

/*
 * Construct the mm subsystem using a buddy allocator
 */
void buddy_init();

/*
 * Allocate "size" bytes in physical memory from the buddy allocation
 * structure. Return a pointer to the allocated bytes
 */
void *buddy_obj_alloc(long size);

/*
 * Given a pointer and a size, free the associated bytes
 */
void buddy_obj_free(long size, void *obj);

/*
 * Allocate "npages" of contiguous memory and return a pointer to those pages
 */
void *page_alloc_n(int npages);

/*
 * Free the bytes associated with a pointer given the number of pages
 * associated with that pointer
 */
void page_free_n(void *ptr, int npages);

#endif // __BUDDY_ALLOCATOR_H__
