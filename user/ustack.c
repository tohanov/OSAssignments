#include "ustack.h"
#include <unistd.h> // for sbrk

#define MAX_ALLOCATION 512 // bytes
// #define NULL 0

#define WITHOUT_HEADER(block) ((void *)block + sizeof(Header))
#define END_OF_BLOCK(block) ((void *)block + sizeof(Header) + block->size)

// typedef long Align;

/* static */ union header {
	union header *previous_block;
	unsigned size;

		// char* deallocated;

	// struct {
	// } metadata;

	// Align x;
};

typedef union header Header;

static Header *stack_head = NULL;
// static long long heap_size = 0; // total heap allocated by sbrk
// static long long user_heap_size = 0; // total heap requested by user
static uint currently_available_heap = 0; // until the end of the last allocated page

void *ustack_malloc(uint requested_heap_size) {
	if (requested_heap_size > MAX_ALLOCATION) {
		return (void *)-1;
	}

	Header *new_block;
	uint requested_size_w_header = requested_heap_size + sizeof(Header); // size of buffer with metadata header

	if (/* heap_size - user_heap_size */currently_available_heap >= requested_size_w_header) { // if enough is available from previous allocations
		// at this point stack_head != NULL

		// user_heap_size += requested_size_w_header;
		currently_available_heap -= requested_size_w_header;
		new_block = END_OF_BLOCK(stack_head);
	}
	else {
		void *free_memory = sbrk(PGSIZE);
		// case: unable to allocate more memory; sbrk returns (void *) -1 on error
		if (free_memory == (void *)(-1)) {
			return (void *)-1;
		}
		
		// heap_size += PGSIZE - (heap_size - user_heap_size);
		// user_heap_size += requested_size_w_header;
		currently_available_heap += PGSIZE - requested_size_w_header;

		new_block = (Header *)free_memory;
	}

	new_block->size = requested_heap_size;
	new_block->previous_block = stack_head;

	stack_head = new_block;

	return WITHOUT_HEADER(new_block);
}


int ustack_free(void) {
	if (stack_head == NULL) {
		return -1;
	}

	unsigned length_freed = stack_head->size;
	stack_head = stack_head->previous_block;

	currently_available_heap += length_freed;

	if (currently_available_heap >= PGSIZE) {
		sbrk(-PGSIZE);
		currently_available_heap -= PGSIZE;
	}

	return length_freed;
}