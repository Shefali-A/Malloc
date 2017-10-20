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
 *
 */

typedef struct header{
  unsigned int size;
  bool free;
  struct header* next;
} header;

typedef struct footer{
  unsigned int size;
  bool free;
} footer;

typedef struct heap_header{
	struct header *firstfree;
} heap_header;

typedef struct allocheader{
  unsigned int size;
  bool free;
} allocheader;


/* Useful shorthand: casts a pointer to a (char *) before adding */
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))

int hl_init(void *heap_ptr, unsigned int heap_size) {

	heap_header *heapmeta = (heap_header *)heap_ptr;
	void *end_heapmeta = (void *) ADD_BYTES(heapmeta, sizeof(heapmeta));

	//starting = first free head pointer
	header *starting = end_heapmeta;

	if ((uintptr_t)end_heapmeta%8 != 0) {
		starting = (header *) ADD_BYTES(starting, (8 - ((char)end_heapmeta%8)));
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
	footer *firstfreefoot = (footer *) ADD_BYTES(starting, (sizeof(header) + true_heap_size));
	starting->size = true_heap_size + 2;
	starting->free = true;
	firstfreefoot->size = true_heap_size + 2;
	firstfreefoot->free = true;

	return 1;

#ifdef PRINT_DEBUG
    printf("heap starts at addr %p\n", heap_ptr);
    printf("8 bytes later is %p\n", ADD_BYTES(heap_ptr, 8));
#endif

}

void *hl_alloc(void *heap_ptr, unsigned int payload_size) {

	heap_header *heapmeta = (heap_header *)heap_ptr;

	header *current = heapmeta->firstfree;

	uintptr_t current_size = current->size + sizeof(header) - sizeof(allocheader);
	uintptr_t actual_size = payload_size;
	if (payload_size%8 != 0) {
		actual_size += 8 - (payload_size%8);
	}

	while (current_size < actual_size) {
		current = current->next;
		if ((current_size < actual_size) && (((current->size)%4 == 2) || ((current->size)%4 == 3))) {
			return 0;
		}
		current_size = current->size + sizeof(header) - sizeof(allocheader);
	}

	footer *newfoot = (footer *) ADD_BYTES(current, (sizeof(allocheader) + (char)actual_size));

	header *newfreehead = (header *) ADD_BYTES(newfoot, sizeof(footer));
	newfreehead->size = current->size - sizeof(header) - sizeof(footer) - (char)actual_size;
	newfreehead->next = current->next;
	newfreehead->free = true;
	footer *newfreefoot = (footer *) ADD_BYTES(newfreehead, (char)(newfreehead->size));
	newfreefoot->size = newfreehead->size;
	newfreefoot->free = true;

	allocheader *newhead = (allocheader *)current;
	newfoot->size = (char)actual_size;
	newfoot->free = false;
	newhead->size = (char)actual_size;
	newhead->free = false;

	if (current == heapmeta->firstfree) {
		heapmeta->firstfree = newfreehead;
	}

	void *return_pointer = (void *) ADD_BYTES(newhead, sizeof(allocheader));

	//HOW TO MODIFY THE LAST BLOCK AND ADD 2???
	//have to change the previous free block's next pointer

    return return_pointer;

}

void hl_release(void *heap_ptr, void *payload_ptr) {

	heap_header *heapmeta = (heap_header *)heap_ptr;

	if (payload_ptr == 0) {
		return;
	}

	allocheader *release_header = (allocheader *) ADD_BYTES(payload_ptr, - sizeof(allocheader));
	uintptr_t release_header_size = release_header->size;

	header *new_release_header = (header *)release_header;
	new_release_header->size = release_header_size - (sizeof(header) - sizeof(allocheader));
	new_release_header->free = true;
	new_release_header->next = ????????????;

	footer *new_release_footer = (footer *) ADD_BYTES(payload_ptr, new_release_header->size);
	new_release_footer->size = new_release_header->size;
	new_release_footer->free = true;

	allocheader *next_header = (allocheader *) ADD_BYTES(new_release_footer, sizeof(footer));
	if (next_header->free == true) {
		header *next_header = (header *) ADD_BYTES(new_release_footer, sizeof(footer));
		uintptr_t next_header_size = next_header->size;
		footer *next_footer = (footer *) ADD_BYTES(next_header, next_header_size);
		uintptr_t total_size = new_release_header->size + sizeof(header) + sizeof(footer) + (char)next_header_size;
		new_release_header->size = total_size;
		new_release_header->next = next_header->next;
		new_release_footer = next_footer;
		new_release_footer->size = new_release_header->size;
	}

	footer *prev_footer = (footer *) ADD_BYTES(new_release_header, (- sizeof(footer)));
	if (prev_footer->free == true) {
		uintptr_t prev_footer_size = prev_footer->size;
		header *prev_header = (header *) ADD_BYTES(prev_footer, (- prev_footer_size - sizeof(header)));
		uintptr_t total_size = new_release_header->size + sizeof(header) + sizeof(footer) + (char)prev_footer_size;
		new_release_footer->size = total_size;
		new_release_header = prev_header;
		new_release_header->size = new_release_footer->size;
		new_release_header->next = prev_header->next;
	}

	//To do:
	//Next

}

void *hl_resize(void *heap_ptr, void *payload_ptr, unsigned int new_size) {

	heap_header *heapmeta = (heap_header *)heap_ptr;

	if (new_size%8 != 0) {
		new_size += 8 - (new_size%8);
	}

	if (payload_ptr == 0) {
		return hl_alloc(heap_ptr, new_size);
	}

	allocheader *resize_block_head = payload_ptr;
	uintptr_t old_size = resize_block_head->size;
	footer *resize_block_foot = (footer *) ADD_BYTES(payload_ptr, (sizeof(allocheader) + old_size));

	void *return_pointer = (void *) ADD_BYTES(resize_block_head, sizeof(allocheader));

	uintptr_t minimum_space = sizeof(allocheader) + sizeof(footer);
	uintptr_t difference = old_size - new_size;
	if ((old_size >= new_size) && (difference >= minimum_space)) {
		resize_block_foot = (footer *) ADD_BYTES(payload_ptr, (sizeof(allocheader) + new_size));
		resize_block_head->size = new_size;
		resize_block_foot->size = new_size;

		//making the remaing space a free block
		header *new_free_head = (header *) ADD_BYTES(resize_block_foot, sizeof(footer));
		new_free_head->size = difference - minimum_space;
		new_free_head->free = true;
		new_free_head->next = ????????????????;
		footer *new_free_foot = (footer *) ADD_BYTES(new_free_foot, (sizeof(header) + new_free_head->size));
		new_free_foot->size = new_free_head->size;
		new_free_foot->free = true;

		//coalescing
		allocheader *next_header = (allocheader *) ADD_BYTES(new_release_footer, sizeof(footer));
		if (next_header->free == true) {
			uintptr_t next_header_size = next_header->size;
			footer *next_footer = (footer *) ADD_BYTES(next_header, next_header_size);
			uintptr_t total_size = new_free_head->size + sizeof(header) + sizeof(footer) + (char)next_header_size;
			new_free_head->size = total_size;
			new_free_head->next = next_header->next;
			new_free_foot = next_footer;
			new_free_foot->size = new_free_head->size;
		}
	}

	header *firstfreepointer = heapmeta->firstfree;
	uintptr_t firstfreesize = firstfreepointer->size + sizeof(header) - sizeof(allocheader);

	while (firstfreesize < new_size) {
		firstfreepointer = firstfreepointer->next;
		if ((firstfreesize < new_size) && (((firstfreepointer->size)%4 == 2) || ((firstfreepointer->size)%4 == 3))) {
			return 0;
		}
		firstfreesize = firstfreepointer->size + sizeof(header) - sizeof(allocheader);
	}

	if (old_size < new_size) {
		new_block_head = firstfreepointer;
		memcpy(new_block_head + sizeof(allocheader), resize_block_head + sizeof(allocheader), old_size);
		resize_block_head = firstfreepointer;
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