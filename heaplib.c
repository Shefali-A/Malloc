

#include <stdlib.h>
#include <stdio.h>
#include "heaplib.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

/* You must implement these functions according to the specification
 * given in heaplib.h. You can define any types you like to this file.
 *
 * Student 1 Name: Shefali Agarwal
 * Student 1 NetID: sa652
 * Student 2 Name: Nehal Rawat
 * Student 2 NetID: nr338
 *
 * Include a description of your approach here.
 * For out structs, we have a header for free block headers, a footer for all block, a heap_header for the heap meta data and an allocheader for allocated block headers.
 * For hl_init, after the our heap meta data, we 8-byte aligned the pointer at the end of the heap meta data and set the rest of the space as a free block.
 * For hl_alloc, we first find a free block that can fit the data in the new allocated block. Then, if there is enough free space, then we create a new smaller free block
 * otherwise, we allocate the entire free block to the new allocated block.
 * For hl_release, we make the current allocated block into a free block. If we have adjacent free blocks on either side of our newly freed block, then we coalesce.
 * For hl_resize, if our current size is larger than the new size, then we resize the block in the same location and if there is enough free space remaining, then we make
 * that into a free block and coalesce with the adjacent free block if it exists. If our current size is smalled than the new size, then we release the current block and move
 * the data over to another free block that has enough space, making it an allocated block now.
 */

typedef struct header{
  unsigned int size;
  bool free;
  bool padding;
  bool padding1;
  bool padding2;
  struct header* next;
  struct header* prev;
} header;

typedef struct footer{
  unsigned int size;
  bool free;
  bool padding;
  bool padding1;
  bool padding2;
} footer;

typedef struct heap_header{
	struct header *firstfree;
	unsigned int heapsize;
	unsigned int beforefirstfreehead;
} heap_header;

typedef struct allocheader{
  unsigned int size;
  bool free;
  bool padding;
  bool padding1;
  bool padding2;
} allocheader;

//fix bool as bool = 1 byte.


/* Useful shorthand: casts a pointer to a (char *) before adding */
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))

int hl_init(void *heap_ptr, unsigned int heap_size) {
	heap_header *heapmeta = (heap_header *)heap_ptr;
	void *end_heapmeta = (void *) ADD_BYTES(heapmeta, sizeof(heap_header));

	//starting = first free header pointer
	header *starting = (header *)end_heapmeta;

	if ((uintptr_t)end_heapmeta%8 != 0) {
		starting = (header *) ADD_BYTES(starting, (8 - (uintptr_t)end_heapmeta%8));
	}

	//before_firstfreehead = space between heap ptr and firstfree header
	uintptr_t before_firstfreehead = (uintptr_t)starting - (uintptr_t)heap_ptr;

	uintptr_t true_heap_size = heap_size - before_firstfreehead - sizeof(header) - sizeof(footer);
	if (true_heap_size%8 != 0) {
		true_heap_size -= (true_heap_size%8);
	}

	heap_size = true_heap_size + before_firstfreehead + sizeof(header) + sizeof(footer);

	if (heap_size < before_firstfreehead + sizeof(header) + sizeof(footer)) {
		return 0;
	}

	heapmeta->firstfree = starting; 
	// heapmeta->last = (footer *) ADD_BYTES(heap_ptr, (heap_size - sizeof(footer)));
	heapmeta->heapsize = heap_size - before_firstfreehead;
	heapmeta->beforefirstfreehead = before_firstfreehead;
	footer *firstfreefoot = (footer *) ADD_BYTES(starting, (sizeof(header) + true_heap_size));
	starting->size = true_heap_size;
	starting->free = true;
	starting->next = NULL;
	starting->prev = NULL;
	firstfreefoot->size = true_heap_size;
	firstfreefoot->free = true;

	return 1;

#ifdef PRINT_DEBUG
    printf("heap starts at addr %p\n", heap_ptr);
    printf("8 bytes later is %p\n", ADD_BYTES(heap_ptr, 8));
#endif

}

void *hl_alloc(void *heap_ptr, unsigned int payload_size) {

	heap_header *heapmeta = (heap_header *)heap_ptr;

	// printf("%s", "firstfree: ");
	// fflush(stdout);
	// printf("%p", heapmeta->firstfree);
	// fflush(stdout);
	
	if (payload_size == 0) {
		return NULL;
	}

	header *current = heapmeta->firstfree;

	// printf("%p", current);
	// fflush(stdout);

	if(current == NULL) {
		return 0;
	}

	//current_size is the new size available for allocation because the header is now smaller
	uintptr_t current_size = current->size + sizeof(header) - sizeof(allocheader); //firstfree size
	uintptr_t actual_size = payload_size;
	if (payload_size < 16) {
		actual_size = 16;
	}

	if (payload_size%8 != 0) {
		actual_size += 8 - (payload_size%8); //actual size is the size of the data
	}

	while (current_size < actual_size) { //traversing the free block until we get one that fits the actual_size
		current = current->next;
		if ((current == NULL) || (current >= ((header *)heapmeta + heapmeta->beforefirstfreehead + heapmeta->heapsize)) || ((uintptr_t)current <= heapmeta->beforefirstfreehead)) {
			return 0;
		}
		current_size = current->size + sizeof(header) - sizeof(allocheader);
		if ((current_size < actual_size) || (current->next == NULL)) {
			return 0;
		}
	}

	allocheader *newhead = (allocheader *)current;

	uintptr_t minimum_space = sizeof(header) + sizeof(footer);
	uintptr_t old_size = current_size + sizeof(allocheader) + sizeof(footer);
	uintptr_t new_size = actual_size + sizeof(allocheader) + sizeof(footer); //new_size is newly allocated block
	uintptr_t difference = old_size - new_size;
	if (difference >= minimum_space) {

		footer *newfoot = (footer *) ADD_BYTES(current, (sizeof(allocheader) + (uintptr_t)actual_size));

		header *newfreehead = (header *) ADD_BYTES(newfoot, sizeof(footer));
		newfreehead->size = current->size - sizeof(header) - sizeof(footer) - (uintptr_t)actual_size;
		newfreehead->next = heapmeta->firstfree;
		newfreehead->prev = NULL;
		heapmeta->firstfree = newfreehead;
		newfreehead->free = true;
		footer *newfreefoot = (footer *) ADD_BYTES(newfreehead, (sizeof(header) + (uintptr_t)(newfreehead->size)));
		newfreefoot->size = newfreehead->size;
		newfreefoot->free = true;

		if (current->prev != NULL) {
			current->prev->next = current->next;
		}

		if (current->next != NULL) {
			current->next->prev = current->prev;
		}

		newfoot->size = (uintptr_t)actual_size;
		newfoot->free = false;
		newhead->size = (uintptr_t)actual_size;
		newhead->free = false;
		
	}

	else {
		footer *newfoot = (footer *) ADD_BYTES(current, (sizeof(allocheader) + (uintptr_t)current_size));
		newfoot->size = current_size;
		newfoot->free = false;
		newhead->size = current_size;
		newhead->free = false;

		if (current->prev != NULL) {
			current->prev->next = current->next;
		}

		if (current->next != NULL) {
			current->next->prev = current->prev;
		}

		if(current == heapmeta->firstfree) {
			heapmeta->firstfree = current->next;
		}
	}

	void *return_pointer = (void *) ADD_BYTES(newhead, sizeof(allocheader));

	//HOW TO MODIFY THE LAST BLOCK AND ADD 2???

    return return_pointer;

}

void hl_release(void *heap_ptr, void *payload_ptr) {

	heap_header *heapmeta = (heap_header *)heap_ptr;

	// void *end_heapmeta = (void *) ADD_BYTES(heapmeta, sizeof(heapmeta));
	// header *starting = (header *)end_heapmeta;
	// if ((uintptr_t)end_heapmeta%8 != 0) {
	// 	starting = (header *) ADD_BYTES(starting, (8 - (uintptr_t)end_heapmeta%8));
	// }
	// uintptr_t before_firstfreehead = (uintptr_t)starting - (uintptr_t)heap_ptr;

	if (payload_ptr == 0) {
		return;
	}

	allocheader *release_header = (allocheader *) ADD_BYTES(payload_ptr, - sizeof(allocheader));
	uintptr_t release_header_size = release_header->size;

	header *new_release_header = (header *)release_header;
	new_release_header->size = release_header_size - (sizeof(header) - sizeof(allocheader));
	new_release_header->free = true;

	footer *new_release_footer = (footer *) ADD_BYTES(new_release_header, (sizeof(header) + new_release_header->size));
	new_release_footer->size = new_release_header->size;
	new_release_footer->free = true;

	allocheader *next_header = (allocheader *) ADD_BYTES(new_release_footer, sizeof(footer));

	if ((next_header->free == true) && (next_header >= ((allocheader *)heapmeta + heapmeta->beforefirstfreehead + heapmeta->heapsize))) {
		header *next_header = (header *) ADD_BYTES(new_release_footer, sizeof(footer));
		uintptr_t next_header_size = next_header->size;
		footer *next_footer = (footer *) ADD_BYTES(next_header, (sizeof(header) + next_header_size));
		uintptr_t total_size = new_release_header->size + sizeof(header) + sizeof(footer) + (uintptr_t)next_header_size;
		new_release_header->size = total_size; 
		if(next_header->prev != NULL) {
			next_header->prev->next = next_header->next;
		}
		if(next_header->next != NULL) {
			next_header->next->prev = next_header->prev;
		}
		new_release_footer = next_footer;
		new_release_footer->size = new_release_header->size;
	}

	footer *prev_footer = (footer *) ADD_BYTES(new_release_header, (- sizeof(footer)));

	if ((prev_footer->free == true) && ((uintptr_t)prev_footer <= heapmeta->beforefirstfreehead)) {
		uintptr_t prev_footer_size = prev_footer->size;
		header *prev_header = (header *) ADD_BYTES(prev_footer, (- prev_footer_size - sizeof(header)));
		uintptr_t total_size = new_release_header->size + sizeof(header) + sizeof(footer) + (uintptr_t)prev_footer_size;
		new_release_footer->size = total_size;
		new_release_header = prev_header;
		new_release_header->size = total_size;
		if(prev_header->prev != NULL) {
			prev_header->prev->next = prev_header->next;
		}
		if(prev_header->next != NULL) {
			prev_header->next->prev = prev_header->prev;
		}
	}

	new_release_header->next = heapmeta->firstfree;
	new_release_header->prev = NULL;
	heapmeta->firstfree = new_release_header;

	//We have never first told what next and prev are!

}

void *hl_resize(void *heap_ptr, void *payload_ptr, unsigned int new_size) {

	heap_header *heapmeta = (heap_header *)heap_ptr;

	if (new_size%8 != 0) {
		new_size += 8 - (new_size%8);
	}

	if (payload_ptr == 0) {
		return hl_alloc(heap_ptr, new_size);
	}

	allocheader *resize_block_head = (allocheader *) ADD_BYTES(payload_ptr, (- sizeof(allocheader)));
	uintptr_t old_size = resize_block_head->size;
	footer *resize_block_foot = (footer *) ADD_BYTES(payload_ptr, (sizeof(allocheader) + old_size));

	void *return_pointer = (void *) ADD_BYTES(resize_block_head, sizeof(allocheader));

	uintptr_t minimum_space = sizeof(header) + sizeof(footer);
	uintptr_t difference = old_size - new_size;
	if ((old_size >= new_size) && (difference >= minimum_space)) {
		resize_block_foot = (footer *) ADD_BYTES(payload_ptr, (sizeof(allocheader) + new_size));
		resize_block_head->size = new_size;
		resize_block_foot->size = new_size;

		//making the remaing space a free block
		header *new_free_head = (header *) ADD_BYTES(resize_block_foot, sizeof(footer));
		new_free_head->size = difference - minimum_space;
		new_free_head->free = true;
		footer *new_free_foot = (footer *) ADD_BYTES(new_free_head, (sizeof(header) + new_free_head->size));
		new_free_foot->size = new_free_head->size;
		new_free_foot->free = true;

		//coalescing
		header *next_header = (header *) ADD_BYTES(new_free_foot, sizeof(footer));
		if ((next_header->free == true) && (next_header >= ((header *)heapmeta + heapmeta->beforefirstfreehead + heapmeta->heapsize))) {
			uintptr_t next_header_size = next_header->size;
			footer *next_footer = (footer *) ADD_BYTES(next_header, (sizeof(header) + next_header_size));
			uintptr_t total_size = new_free_head->size + sizeof(header) + sizeof(footer) + (uintptr_t)next_header_size;
			new_free_head->size = total_size;
			if(next_header->prev != NULL) {
				next_header->prev->next = next_header->next;
			}
			if(next_header->next != NULL) {
				next_header->next->prev = next_header->prev;
			}
			new_free_foot = next_footer;
			new_free_foot->size = new_free_head->size;
		}

		new_free_head->next = heapmeta->firstfree;
		new_free_head->prev = NULL;
		heapmeta->firstfree = new_free_head;
	}

	header *firstfreepointer = heapmeta->firstfree;

	if (firstfreepointer == NULL) {
		return 0;
	}

	uintptr_t firstfreesize = firstfreepointer->size + sizeof(header) - sizeof(allocheader);

	while (firstfreesize < new_size) {
		firstfreepointer = firstfreepointer->next;
		if ((firstfreepointer == NULL) || (firstfreepointer >= ((header *)heapmeta + heapmeta->beforefirstfreehead + heapmeta->heapsize)) || ((uintptr_t)firstfreepointer <= heapmeta->beforefirstfreehead)) {
			return 0;
		}
		firstfreesize = firstfreepointer->size + sizeof(header) - sizeof(allocheader);
		if ((firstfreesize < new_size) && (firstfreepointer->next == NULL)) {
			return 0;
		}
	}

	if (old_size < new_size) { //wasting space because we are not taking care of extra free space in free block
		header *new_block = (header *)firstfreepointer;
		if(new_block->prev != NULL) {
			new_block->prev->next = new_block->next;
		}
		if(new_block->next != NULL) {
			new_block->next->prev = new_block->prev;
		}
		allocheader *new_block_head = (allocheader *)firstfreepointer;
		memcpy(new_block_head + sizeof(allocheader), resize_block_head + sizeof(allocheader), old_size);
		resize_block_head = (allocheader *)firstfreepointer;
		resize_block_head->size = new_size;
		resize_block_head->free = false;
		resize_block_foot = (footer *) ADD_BYTES(resize_block_head, (sizeof(allocheader) + new_size));
		resize_block_foot->size = new_size;
		resize_block_foot->free = false;

		//frees the previous block
		hl_release(heap_ptr, payload_ptr);

		return_pointer = (void *) ADD_BYTES(resize_block_head, sizeof(allocheader));
	}

    return return_pointer;

}