#include "buddy_allocator.h"

#include "sys/mem/mem.h"

#include "util/ktypes.h"
#include "util/string.h"

/*
 * TODO: implement me!
 *
 * Hints:
 *   - we need to construct the free list, how many lists should reside in
 *     the structure?
 *   - to allocate the "chunks" and the free list, we need to reserve space in
 *     memory as a slab... how big will these components be?
 *   - once we have allocated space for the "mm" subsystem components, the
 *     amount of free space in memory has changed... bare this in mind when
 *     constructing your metadata!
 */
void buddy_init() {

}

/*
 * TODO: implement me!
 *
 * Hints:
 *   - given a size, how do we know where to allocate from?
 *   - if we don't have any available chunks, how do we find another one?
 */
void *buddy_obj_alloc(long size) {
    return NULL;
}

/*
 * TODO: implement me!
 *
 * Hint: this function should handle the inverse cases of the "alloc" function!
 *       if there are cases that need to be handled in that function, what
 *       would the inverse cases look like?
 */
void buddy_obj_free(long size, void *obj) {

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
