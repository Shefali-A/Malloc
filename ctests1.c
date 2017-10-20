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
    "alloc: block returned is aligned"
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
 * From spec if hl_ init: Do not assume that heapptr is 8-byte aligned. 
 * make sure that the heaps created allocate, release, and resize properly with non 8 byte aligned heapptr
 */

int test06() {

	char heap[HEAP_SIZE];
    int heap_created_f = hl_init(heap + 1, HEAP_SIZE);

    void *block = hl_alloc(heap + 1, ARRAY_LEN);



    if (heap_created_f) {
        return SUCCESS;
    }
    return FAILURE;


}

/* Your test.
 * From spec of hl_release: If blockptr has the value 0 (NULL), the function should do nothing.
 * making sure you don't segfault
 * make it full, release with blckptr 0 and then try filling again, should not work
 */
int test07() {

	char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    void *block = hl_alloc(heap, 0.6*HEAP_SIZE);

    // How do you do this? what does release return?
    //Do we need the *here for arguments
    hl_release(heap, 0);

    void *new_block = hl_alloc(heap, 0.6*HEAP_SIZE);

    if (new_block == NULL) {
    return SUCCESS;
    }

    return FAILURE;

}

/* Your test.
 * From spec of hl_alloc: Blocks of memory can be of size 0.
 * CHECK THIS!!!!!! IS IT ROBUST?!?!?!!??!?!?!?!!?
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
 * TESTING RELEASE
 * fill it and then release one and and try to alloc again - should work if release worked
 * should we also have a test that allocates more than 1 block, frees 1 block, and the allocates at the same blockptr
 */
int test09() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    void *block = hl_alloc(heap, (int) 0.6*HEAP_SIZE);

    // How do you do this? what does release return?
    //Do we need the *here for arguments
    hl_release(heap, block);

    void *new_block = hl_alloc(heap, (int) 0.6*HEAP_SIZE);

    if (new_block != NULL) {
    	return SUCCESS;
    }

    return FAILURE;
    
}

/* Your test.
 * hl_resize: the return value should be 8 byte aligned
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
    block1 = hl_alloc(&array, ARRAY_LEN);
    block2 = hl_alloc(&array, ARRAY_LEN);
    block3 = hl_alloc(&array, ARRAY_LEN);
    block4 = hl_alloc(&array, ARRAY_LEN);
    block5 = hl_alloc(&array, ARRAY_LEN);
    block6 = hl_alloc(&array, ARRAY_LEN);
    block7 = hl_alloc(&array, ARRAY_LEN);

    newblock = hl_resize(&array, block, ARRAY_LEN);
    newblock1 = hl_resize(&array, block1, ARRAY_LEN + 1);
    newblock2 = hl_resize(&array, block2, ARRAY_LEN + 2);
    newblock3 = hl_resize(&array, block3, ARRAY_LEN + 3);
    newblock4 = hl_resize(&array, block4, ARRAY_LEN + 4);
    newblock5 = hl_resize(&array, block5, ARRAY_LEN + 5);
    newblock6 = hl_resize(&array, block6, ARRAY_LEN + 6);
    newblock7 = hl_resize(&array, block7, ARRAY_LEN + 7);

    aligned_f = !((unsigned long)newblock & (ALIGNMENT - 1));
    aligned_f1 = !((unsigned long)newblock1 & (ALIGNMENT - 1));
    aligned_f2 = !((unsigned long)newblock2 & (ALIGNMENT - 1));
    aligned_f3 = !((unsigned long)newblock3 & (ALIGNMENT - 1));
    aligned_f4 = !((unsigned long)newblock4 & (ALIGNMENT - 1));
    aligned_f5 = !((unsigned long)newblock5 & (ALIGNMENT - 1));
    aligned_f6 = !((unsigned long)newblock6 & (ALIGNMENT - 1));
    aligned_f7 = !((unsigned long)newblock7 & (ALIGNMENT - 1));

    if (aligned_f && aligned_f1 && aligned_f2 && aligned_f3 && aligned_f4 && aligned_f5 && aligned_f6 && aligned_f7) {
        return SUCCESS;
    }

    return FAILURE;
}


/* Your test.
 * for hl_resize: if the blockptr has the value 0 (NULL), the function should behave like hl_alloc
 * DONE I THINK!
 */
int test11() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    void *new_block1 = hl_resize(heap, NULL, ARRAY_LEN);

    void *new_block2 = hl_resize(heap, NULL, ARRAY_LEN * HEAP_SIZE);

    if ((new_block1 == NULL) && (new_block2 != NULL)) {
        return FAILURE;
    }
    return SUCCESS;

}

/* Your test.
 */
int test12() {

    return SUCCESS;

}

/* Your test.
 */
int test13() {

    return SUCCESS;

}
