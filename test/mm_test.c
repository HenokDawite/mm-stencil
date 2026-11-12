#include <stdio.h>
#include <assert.h>
#include "include/slab_allocator.h"
#include "include/buddy_allocator.h"
#include "sys/mem/mem.h"
#include "util/string.h"

// total free pages across all five free lists
static long count_free_pages(void){
    long total = 0;
    int o;
    chunk_t *c;

    for (o = 0; o < 5; o++) {                 // 5 = NUM_ORDERS
        for (c = free_list->lists[o]; c != NULL; c = c->next) {
            total += 1L << o;                 // order o block = 2^o pages
        }
    }
    return total;
}

//initializes, counts free pages, ensures it increments by one and decremeents when mem allocated and then freed
static int test_one(void){
    void *obj_one;
    void *obj_two;
    long before, during, after;

    if (mem_init() != 0) {
        return 1;                        
    }
    buddy_init();

    before = count_free_pages();

    obj_one = buddy_obj_alloc(PAGE_SIZE / sizeof(long));
    if (obj_one == NULL) {
        return 1;                             
    }
    during = count_free_pages();
    if (during != before - 1) {
        return 3;     
    }                       

    buddy_obj_free(PAGE_SIZE / sizeof(long), obj_one);
    after = count_free_pages();
    if (after != before) {
        return 4;                            
    }
    return 0;
}

//allovates consecutive blocks, then frees one block, then check that only one was freed then allocated again
static int test_two(void){
    void *head = NULL;    
    void **p;
    void *next;
    void *again;
    void *big;
    long before, count, i;

    if (mem_init() != 0) {
        return 1;
    }
    buddy_init();

    before = count_free_pages();

    //take every 64K block until no more space
    count = 0;
    for (;;) {
        p = page_alloc_n(16);
        if (p == NULL) {
            break;
        }
        *p = head;
        head = p;
        count++;
    }
    if (count == 0) {
        return 2;
    }
    //memory is full 
    if (page_alloc_n(16) != NULL) {
        return 3;
    }

    //free one block
    next = *(void **)head;
    page_free_n(head, 16);
    head = next;

    again = page_alloc_n(16); //reallocate
    if (again == NULL) {
        return 5;                       //freed block was not reusable
    }
    if (page_alloc_n(16) != NULL) {
        return 6;                       //only one block was freed
    }
    *(void **)again = head;
    head = again;
    
    return 0;
}
//allocate more pages than in one chunk (37) 
static int test_three(void){
    unsigned char *a;
    unsigned char *b;
    long before, i;

    if (mem_init() != 0) {
        return 1;
    }
    buddy_init();

    before = count_free_pages();
    //Blocks of 8 pages, allocate 40 and count freed
    a = page_alloc_n(37);
    if (a == NULL) {
        return 5;
    }
    if (count_free_pages() != before - 40) {
        return 6;
    }
    page_free_n(a, 37);
    if (count_free_pages() != before) {
        return 8;
    }

    return 0;

}

//builds on top of test_two, after freeing and reallocating once, will free all and reallocate two more times adjacent blocks
static int test_four(void){
    void *head = NULL;    
    void **p;
    void *next;
    void *again;
    void *big;
    long before, count, i;

    if (mem_init() != 0) {
        return 1;
    }
    buddy_init();

    before = count_free_pages();

    count = 0;
    //allocate all
    for (;;) {
        p = page_alloc_n(16);
        if (p == NULL) {
            break;
        }
        *p = head;
        head = p;
        count++;
    }
    if (count == 0) {
        return 2;
    }

    if (page_alloc_n(16) != NULL) {
        return 3;
    }

    //free one block and reallocate
    next = *(void **)head;
    page_free_n(head, 16);
    head = next;

    again = page_alloc_n(16);
    if (again == NULL) {
        return 4;                    
    }
    if (page_alloc_n(16) != NULL) {
        return 5;                       
    }
    *(void **)again = head;
    head = again;

    //free everything
    while (head != NULL) {
        next = *(void **)head;        
        page_free_n(head, 16);
        head = next;
    }
    if (count_free_pages() != before) {
        return 6;                  
    }

    //allocate everything again 
    for (i = 0; i < count; i++) {
        p = page_alloc_n(16);
        if (p == NULL) {
            return 7;
        }
        *p = head;
        head = p;
    }
    if (page_alloc_n(16) != NULL) {
        return 8;
    }

    //free it all again, try a multi-block request (alloc_large) if there is space
    while (head != NULL) {
        next = *(void **)head;
        page_free_n(head, 16);
        head = next;
    }
    if (count >= 2) {
        big = page_alloc_n(32);        
        if (big == NULL) {
            return 9;
        }
        page_free_n(big, 32);
    }
    if (count_free_pages() != before) {
        return 10;
    }
    return 0;
}
int main(void) {
    int result_one = test_one();
    
    if (result_one == 0) {
        printf("test_one: PASS\n");
    } else {
        printf("test_one: FAIL (%d)\n", result_one);
    }
    int result_two = test_two();
    if (result_two == 0) {
        printf("test_two: PASS\n");
    } else {
        printf("test_two: FAIL (%d)\n", result_two);
    }
    int result_three = test_three();
    if (result_three == 0) {
        printf("test_three: PASS\n");
    } else {
        printf("test_three: FAIL (%d)\n", result_three);
    }
    int result_four = test_four();
    if (result_four == 0) {
        printf("test_four: PASS\n");
    } else {
        printf("test_four: FAIL (%d)\n", result_four);
    }
    return 0;
}