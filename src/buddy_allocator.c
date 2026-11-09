#include "buddy_allocator.h"

#include "sys/mem/mem.h"

#include "util/ktypes.h"
#include "util/string.h"

// 4K, 8K, 16K, 32K, 64K
#define NUM_ORDERS 5

// round up to a chunk. if we land exactly on a size, step up so
// theres a little extra room in the block
static int order_for_size(long size)
{
    int order = 0;
    long bytes = PAGE_SIZE;

    while (bytes <= size) {
        if (order + 1 >= NUM_ORDERS) {
            break;
        }
        bytes *= 2;
        order++;
    }
    return order;
}

// all_chunks holds descriptors nobody is using right now
static chunk_t *grab_chunk()
{
    chunk_t *c;

    if (all_chunks == NULL) {
        return NULL;
    }
    c = all_chunks;
    all_chunks = c->next;
    c->next = NULL;
    return c;
}

static void drop_chunk(chunk_t *c)
{
    c->addr = NULL;
    c->next = all_chunks;
    all_chunks = c;
}

static void push_chunk(int order, chunk_t *c)
{
    c->next = free_list->lists[order];
    free_list->lists[order] = c;
}

static chunk_t *pop_chunk(int order)
{
    chunk_t *c;

    c = free_list->lists[order];
    if (c == NULL) {
        return NULL;
    }
    free_list->lists[order] = c->next;
    c->next = NULL;
    return c;
}

// pull a free block with this address off the list, if we have one
static chunk_t *pull_chunk(int order, void *addr)
{
    chunk_t *prev;
    chunk_t *cur;

    prev = NULL;
    cur = free_list->lists[order];
    while (cur != NULL) {
        if (cur->addr == addr) {
            if (prev == NULL) {
                free_list->lists[order] = cur->next;
            } else {
                prev->next = cur->next;
            }
            cur->next = NULL;
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }
    return NULL;
}


void buddy_init()
{
    long chunk_count;
    long i;
    char *limit;
    char *cursor;
    unsigned long aligned;
    slab_allocator_t lists_slab;

    // mem_init has to run first so dram exists
    if (dram == NULL || mem_size <= 0) {
        return;
    }
    slab_init();
    limit = (char *)dram + mem_size;

    slab_allocator_init(&buddy_slab_allocator, (int)sizeof(free_list_t));
    slab_allocator_init(&chunk_slab_allocator, (int)sizeof(chunk_t));

    free_list = slab_obj_alloc(buddy_slab_allocator);
    slab_allocator_init(&lists_slab, (int)(sizeof(chunk_t *) * NUM_ORDERS));
    free_list->lists = slab_obj_alloc(lists_slab);
    for (i = 0; i < NUM_ORDERS; i++) {
        free_list->lists[i] = NULL;
    }

    // one descriptor per page. has to happen before we mark the free region
    // because this moves free_ptr
    chunk_count = mem_size / PAGE_SIZE;
    all_chunks = NULL;
    for (i = 0; i < chunk_count; i++) {
        chunk_t *c;

        if ((char *)free_ptr + (long)sizeof(chunk_t) > limit) {
            break;
        }
        c = slab_obj_alloc(chunk_slab_allocator);
        c->addr = NULL;
        c->next = all_chunks;
        all_chunks = c;
    }

    aligned = (unsigned long)free_ptr;
    aligned = (aligned + PAGE_SIZE - 1) & ~((unsigned long)PAGE_SIZE - 1);
    cursor = (char *)aligned;

    // biggest chunks that still fit, including the leftover tail
    while (cursor < limit) {
        int order = NUM_ORDERS - 1;
        int placed = 0;

        while (order >= 0) {
            long block = (long)PAGE_SIZE << order;
            chunk_t *c;

            if (cursor + block <= limit) {
                c = grab_chunk();
                c->addr = cursor;
                push_chunk(order, c);
                cursor += block;
                placed = 1;
                break;
            }
            order--;
        }
        if (!placed) {
            break;
        }
    }
}


void *buddy_obj_alloc(long size)
{
    int order;
    int cur;
    chunk_t *block;
    void *addr;

    if (size <= 0 || free_list == NULL) {
        return NULL;
    }
    if (size > MAX_CHUNK_SIZE) {
        return NULL;
    }

    order = order_for_size(size);
    cur = order;
    while (cur < NUM_ORDERS && free_list->lists[cur] == NULL) {
        cur++;
    }
    if (cur == NUM_ORDERS) {
        return NULL;
    }

    block = pop_chunk(cur);
    while (cur > order) {
        chunk_t *buddy;
        long half;

        cur--;
        half = (long)PAGE_SIZE << cur;
        buddy = grab_chunk();
        buddy->addr = (char *)block->addr + half;
        push_chunk(cur, buddy);
    }

    addr = block->addr;
    drop_chunk(block);
    return addr;
}

// inverse of alloc: merge with the buddy when that block is free
void buddy_obj_free(long size, void *obj)
{
    int order;
    void *addr;
    chunk_t *c;

    if (obj == NULL || size <= 0 || free_list == NULL) {
        return;
    }
    if (size > MAX_CHUNK_SIZE) {
        return;
    }

    order = order_for_size(size);
    addr = obj;

    // buddy sits right after this block. if its free, fold them together
    while (order < NUM_ORDERS - 1) {
        long bytes;
        void *buddy_addr;
        chunk_t *buddy;

        bytes = (long)PAGE_SIZE << order;
        buddy_addr = (char *)addr + bytes;
        buddy = pull_chunk(order, buddy_addr);
        if (buddy == NULL) {
            break;
        }
        drop_chunk(buddy);
        order++;
    }

    c = grab_chunk();
    c->addr = addr;
    push_chunk(order, c);
}

/*
 * TODO: implement me!
 *
 * Hint: this function behaves similar to "obj_alloc" but what happens if
 *       npages is larger than MAX_CHUNK_SIZE?
 */
void *page_alloc_n(int npages) {
    return NULL;
}

/*
 * TODO: implement me!
 *
 * Hint: this function should handle the inverse case of the "alloc" function!
 */
void page_free_n(void *ptr, int npages) {

}
