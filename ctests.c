#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "heaplib.h"

#define HEAP_SIZE 1024
#define ARRAY_LEN 16

// TODO: Add test descriptions as you add more tests...
const char* testDescriptions[] = {
    "init: heap should be created when enough space",
    "init: heap should not be created when not enough space",
    "alloc: block should be allocated when enough space",
    "alloc: block should not be allocated when not enough space",
    "alloc: block returned is aligned",
    "alloc, init: blocks should not overlap and alloc should work when heapptr is not 8-byte aligned",
    "release: function should do nothing when blockptr is 0 (NULL)",
    "alloc: alloc works when block_size is 0",
    "release: release should allow multiple blocks to alloc which would otherwise not fit in the heap without being released",
    "init, alloc, resize: pointers returned by alloc and resize should be 8-byte aligned even if heapptr is not 8-byte aligned",
    "resize: resize should behave like alloc when the blockptr is 0 (NULL)",
    "resize: contents of allocated block should be preserved after resize and resizing a block larger than the heap should return NULL ptr",
    "resize: resize should work when resizing block to a smaller size and a larger size",
};

/* Checks whether a "heap" is created when there IS enough space.
 * THIS TEST IS COMPLETE.
 */
int test01() {

    char heap[HEAP_SIZE];

    int heap_created_f = hl_init(heap, HEAP_SIZE);

    if (heap_created_f) {
        return SUCCESS;
    }
    return FAILURE;
}

/* Checks whether a "heap" is created when there IS NOT enough space.
 * THIS TEST IS NOT COMPLETE. heaplame.c does not pass this test. Feel free to fix it!
 * Lab 12 TODO: COMPLETE THIS TEST!
 */
int test02() {

    char heap[HEAP_SIZE];

    //not enough space means not enough space for meta data (at minimum)

    int heap_created_f = hl_init(heap, 1);

    if (heap_created_f) {
        return FAILURE;
    }
    return SUCCESS;

}

/* Checks whether a block can be allocated when there is enough space.
 * THIS TEST IS NOT COMPLETE.
 * Lab 12 TODO: COMPLETE THIS TEST!
 */
int test03() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    void *block = hl_alloc(heap, ARRAY_LEN);

    if (block == NULL) {
        return FAILURE;
    }
    return SUCCESS;
}

/* Checks whether a block can be allocated when there is NOT enough space.
 * THIS TEST IS COMPLETE. heaplame.c does not pass this test. Feel free to fix it!
 */
int test04() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // should NOT work
    void *block = hl_alloc(heap, ARRAY_LEN * HEAP_SIZE);

    if (block == NULL) {
        return SUCCESS;
    }
    return FAILURE;
}

/* Checks whether hl_alloc returns a pointer that has the correct
 * alignment.
 * THIS TEST IS NOT COMPLETE. heaplame.c does not pass this test. Feel free to fix it!
 * LAB 12 TODO: COMPLETE THIS TEST! (it is not robust)
 */
int test05() {

    char array[HEAP_SIZE];
    void *block;
    void *block1;
    void *block2;
    void *block3;
    void *block4;
    void *block5;
    void *block6;
    void *block7;
    bool aligned_f = false;
    bool aligned_f1 = false;
    bool aligned_f2 = false;
    bool aligned_f3 = false;
    bool aligned_f4 = false;
    bool aligned_f5 = false;
    bool aligned_f6 = false;
    bool aligned_f7 = false;

    hl_init(&array, HEAP_SIZE - 1);

    block = hl_alloc(&array, ARRAY_LEN); // doesn't really matter how much we allocate here
    block1 = hl_alloc(&array, ARRAY_LEN+1);
    block2 = hl_alloc(&array, ARRAY_LEN+2);
    block3 = hl_alloc(&array, ARRAY_LEN+3);
    block4 = hl_alloc(&array, ARRAY_LEN+4);
    block5 = hl_alloc(&array, ARRAY_LEN+5);
    block6 = hl_alloc(&array, ARRAY_LEN+6);
    block7 = hl_alloc(&array, ARRAY_LEN+7);


    // you may find this helpful. feel free to remove
#ifdef PRINT_DEBUG
    printf("blockptr = %16lx\n", (unsigned long)block);
    printf("mask =     %16lx\n", (unsigned long)(ALIGNMENT -1));
    printf("---------------------------\n");
    printf("ANDED =    %16lx\n", (unsigned long)block & (ALIGNMENT - 1));
    printf("!ANDED (ALIGNED) =   %6d\n", !((unsigned long)block & (ALIGNMENT - 1)));
#endif

    aligned_f = !((unsigned long)block & (ALIGNMENT - 1));
    aligned_f1 = !((unsigned long)block1 & (ALIGNMENT - 1));
    aligned_f2 = !((unsigned long)block2 & (ALIGNMENT - 1));
    aligned_f3 = !((unsigned long)block3 & (ALIGNMENT - 1));
    aligned_f4 = !((unsigned long)block4 & (ALIGNMENT - 1));
    aligned_f5 = !((unsigned long)block5 & (ALIGNMENT - 1));
    aligned_f6 = !((unsigned long)block6 & (ALIGNMENT - 1));
    aligned_f7 = !((unsigned long)block7 & (ALIGNMENT - 1));

    if (aligned_f && aligned_f1 && aligned_f2 && aligned_f3 && aligned_f4 && aligned_f5 && aligned_f6 && aligned_f7) {
        return SUCCESS;
    }

    return FAILURE;
}

/* Your test.
 * (1) Checks whether two blocks overlap in the same heap when alloc is called twice, and
 * (2) Checks whether alloc works with a heapptr that is not 8-byte aligned. 
 */

int test06() {

    bool test1 = false;
    bool test2 = false;

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    void *block = hl_alloc(heap, 10);

    void *new_block = hl_alloc(heap, 8);

    if ((new_block >= block+16) || (new_block+8 <= block)) {
        test1 = true;
    }

    char heap2[HEAP_SIZE];
    
    int heap_created_f = hl_init(heap2 + 1, HEAP_SIZE);

    void *block2 = hl_alloc(heap2 + 1, ARRAY_LEN);

    if (heap_created_f && (block2 != NULL)) {
        test2 = true;
    }

    if (test1 && test2) {
        return SUCCESS;
    }

    return FAILURE;

}


/* Your test.
 * Checks to make sure that when the blockptr is 0 (NULL), the function does nothing
 */
int test07() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    void *block = hl_alloc(heap, HEAP_SIZE/2);

    hl_release(heap, 0);

    void *new_block = hl_alloc(heap, HEAP_SIZE/2);

    if ((block != NULL) && (new_block == NULL)) {
    return SUCCESS;
    }

    return FAILURE;

}

/* Your test.
 * Checks to make sure alloc works even if block_size is 0
 */
int test08() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    void *block = hl_alloc(heap, 0);

    if (block == NULL) {
        return FAILURE;
    }

    return SUCCESS;

}

/* Your test.
 * Checks to see whether release works correctly by allocating a block, releasing it, and allocating a block to see if
 * there is enough space in memory after release
 */
int test09() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    void *block = hl_alloc(heap, HEAP_SIZE/2);

    hl_release(heap, block);

    void *new_block = hl_alloc(heap, HEAP_SIZE/2);

    if (new_block != NULL) {
        return SUCCESS;
    }

    return FAILURE;
    
}

/* Your test.
 * Checks whether when heapptr is not 8 byte aligned, pointers returned by alloc and resize are still 8 byte aligned
 * (Also checks when blockptr is NULL for resize)
 */
int test10() {

    char array[HEAP_SIZE];
    void *block;
    void *block1;
    void *block2;
    void *block3;
    void *block4;
    void *block5;
    void *block6;
    void *block7;
    void *newblock;
    void *newblock1;
    void *newblock2;
    void *newblock3;
    void *newblock4;
    void *newblock5;
    void *newblock6;
    void *newblock7;
    void *newblock8;
    bool aligned_f = false;
    bool aligned_f1 = false;
    bool aligned_f2 = false;
    bool aligned_f3 = false;
    bool aligned_f4 = false;
    bool aligned_f5 = false;
    bool aligned_f6 = false;
    bool aligned_f7 = false;
    bool aligned_f8 = false;
    bool aligned_f9 = false;
    bool aligned_f10 = false;
    bool aligned_f11 = false;
    bool aligned_f12 = false;
    bool aligned_f13 = false;
    bool aligned_f14 = false;
    bool aligned_f15 = false;
    bool aligned_f16 = false;

    char *array2 = array;

    if (!((unsigned long)array2 & (ALIGNMENT - 1))) {
        array2 += 1;
    }

    hl_init(array2, HEAP_SIZE - 1);

    block = hl_alloc(array2, ARRAY_LEN); // doesn't really matter how much we allocate here
    block1 = hl_alloc(array2, ARRAY_LEN + 3);
    block2 = hl_alloc(array2, ARRAY_LEN + 7);
    block3 = hl_alloc(array2, ARRAY_LEN + 2);
    block4 = hl_alloc(array2, ARRAY_LEN + 4);
    block5 = hl_alloc(array2, ARRAY_LEN - 1);
    block6 = hl_alloc(array2, ARRAY_LEN - 6);
    block7 = hl_alloc(array2, ARRAY_LEN + 5);

    newblock = hl_resize(array2, block, ARRAY_LEN);
    newblock1 = hl_resize(array2, block1, ARRAY_LEN + 1);
    newblock2 = hl_resize(array2, block2, ARRAY_LEN + 2);
    newblock3 = hl_resize(array2, block3, ARRAY_LEN + 3);
    newblock4 = hl_resize(array2, block4, ARRAY_LEN + 4);
    newblock5 = hl_resize(array2, block5, ARRAY_LEN + 5);
    newblock6 = hl_resize(array2, block6, ARRAY_LEN + 6);
    newblock7 = hl_resize(array2, block7, ARRAY_LEN + 7);
    newblock8 = hl_resize(array2, NULL, ARRAY_LEN - 3);

    aligned_f = !((unsigned long)block & (ALIGNMENT - 1));
    aligned_f1 = !((unsigned long)block1 & (ALIGNMENT - 1));
    aligned_f2 = !((unsigned long)block2 & (ALIGNMENT - 1));
    aligned_f3 = !((unsigned long)block3 & (ALIGNMENT - 1));
    aligned_f4 = !((unsigned long)block4 & (ALIGNMENT - 1));
    aligned_f5 = !((unsigned long)block5 & (ALIGNMENT - 1));
    aligned_f6 = !((unsigned long)block6 & (ALIGNMENT - 1));
    aligned_f7 = !((unsigned long)block7 & (ALIGNMENT - 1));
    aligned_f8 = !((unsigned long)newblock & (ALIGNMENT - 1));
    aligned_f9 = !((unsigned long)newblock1 & (ALIGNMENT - 1));
    aligned_f10 = !((unsigned long)newblock2 & (ALIGNMENT - 1));
    aligned_f11 = !((unsigned long)newblock3 & (ALIGNMENT - 1));
    aligned_f12 = !((unsigned long)newblock4 & (ALIGNMENT - 1));
    aligned_f13 = !((unsigned long)newblock5 & (ALIGNMENT - 1));
    aligned_f14 = !((unsigned long)newblock6 & (ALIGNMENT - 1));
    aligned_f15 = !((unsigned long)newblock7 & (ALIGNMENT - 1));
    aligned_f16 = !((unsigned long)newblock8 & (ALIGNMENT - 1));

    if (aligned_f && aligned_f1 && aligned_f2 && aligned_f3 && aligned_f4 && aligned_f5 && aligned_f6 && aligned_f7 && 
        aligned_f8 && aligned_f9 && aligned_f10 && aligned_f11 && aligned_f12 && aligned_f13 && aligned_f14 && aligned_f15 &&
        aligned_f16) {
        return SUCCESS;
    }

    return FAILURE;
}


/* Your test.
 * Checks to see whether hl_resize behaves like hl_alloc when the blockptr has the value 0 (NULL)
 */
int test11() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);
    void *block = hl_alloc(heap, ARRAY_LEN);

    hl_release(heap, block);

    void *new_block = hl_resize(heap, NULL, ARRAY_LEN);

    if (new_block == NULL) {
        return FAILURE;
    }

    return SUCCESS;

}

/* Your test.
 * (1) Checks that the contents of the block are preserved after resize, and
 * (2) Checks that the ptr returned is NULL when resizing the block to a size much larger than the heap size
 */
int test12() {

    bool test1 = false;
    bool test2 = false;

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    int *block = hl_alloc(heap, ARRAY_LEN);

    block[0] = 9;
    block[1] = 5;
    block[2] = 19;
    block[3] = 32;

    int *new_block = hl_resize(heap, block, ARRAY_LEN/2);

    if ((new_block[0] == block[0]) && (new_block[1] == block[1])) {
        test1 = true;
    }

    int *new_block2 = hl_resize(heap, new_block, ARRAY_LEN * HEAP_SIZE);

    if (new_block2 == NULL) {
        test2 = true;
    }

    if (test1 && test2) {
        return SUCCESS;
    }

    return FAILURE;

}

/* Your test.
 * (1) Checks that resize works when resizing the block to a smaller size, and
 * (2) Checks that resize works when resizing the block to a larger size
 */
int test13() {

    bool test1 = false;
    bool test2 = false;

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    void *block = hl_alloc(heap, HEAP_SIZE * 3/4);

    hl_resize(heap, block, ARRAY_LEN);

    void *new_block = hl_alloc(heap, HEAP_SIZE * 1/4);

    if (new_block != NULL) {
        test1 = true;
    }


    char heap2[HEAP_SIZE];

    hl_init(heap2, HEAP_SIZE);

    void *block2 = hl_alloc(heap2, HEAP_SIZE * 1/4);

    hl_resize(heap2, block2, HEAP_SIZE * 1/2);

    void *new_block2 = hl_alloc(heap2, HEAP_SIZE * 1/2);

    if (new_block2 == NULL) {
        test2 = true;
    }

    if (test1 && test2) {
        return SUCCESS;
    }

    return FAILURE;

}

int test14() {

    char heap[HEAP_SIZE*2];

    hl_init(heap, HEAP_SIZE*2);

    void *pointers[10000];
    int i;

    for (i=0; i < 1000; i++) {
        pointers[i] = hl_alloc(heap,10);
    }

    for (i=0; i<1000; i+=2) {

        hl_release(heap, pointers[i]);
        pointers[i] = NULL;
    }

    for (i=0; i<1000; i+=3) {
        hl_alloc(heap, 7);
    }

    return SUCCESS;
}
