#include "ustack.h"
#include "user.h" // for sbrk
#include "kernel/util.h"

typedef struct header {
	struct header *previous_block;
	unsigned size;
} Header;


static Header *stack_head = NULL;
static uint currently_available_heap = 0; // until the end of the last allocated page


void *ustack_malloc(uint requested_heap_size) {
	debug_print("in ustack_malloc, currently_available_heap=%d, requested_heap_size=%d", currently_available_heap, requested_heap_size);

	if (requested_heap_size > MAX_ALLOCATION) {
		return (void *)-1;
	}

	Header *new_block;
	uint requested_size_w_header = requested_heap_size + sizeof(Header); // size of buffer with metadata header

	debug_print("\trequested_size_w_header=%d", requested_size_w_header);

	// if enough is available from previous allocations
	if (currently_available_heap >= requested_size_w_header) {
		// at this point stack_head != NULL
		currently_available_heap -= requested_size_w_header;
		new_block = END_OF_BLOCK(stack_head);
	}
	else {
		void *free_memory = sbrk(PGSIZE);

		// if unable to allocate more memory; sbrk returns (void *) -1 on error
		if (free_memory == (void *)(-1)) {
			return (void *)-1;
		}
		
		currently_available_heap += PGSIZE - requested_size_w_header;

		new_block = (Header *)free_memory;
	}

	new_block->size = requested_heap_size;
	debug_print("\tupdating size to %d", requested_heap_size);
	new_block->previous_block = stack_head;

	stack_head = new_block;

	debug_print("returning from ustack_malloc, currently_available_heap=%d", currently_available_heap);
	return WITHOUT_HEADER(new_block);
}


int ustack_free(void) {
	debug_print("in ustack_free, currently_available_heap=%d", currently_available_heap);

	if (stack_head == NULL) {
		return -1;
	}

	unsigned length_freed = stack_head->size;

	debug_print("\tswitching stack_head from %p to %p", stack_head, stack_head->previous_block);
	stack_head = stack_head->previous_block;

	currently_available_heap += length_freed + sizeof(Header);

	if (currently_available_heap >= PGSIZE) {
		debug_print("\tin ustack_free if");
		sbrk(-PGSIZE);
		currently_available_heap -= PGSIZE;
	}

	debug_print("returning from ustack_free, currently_available_heap=%d, length_freed=%d", currently_available_heap, length_freed);
	return length_freed;
}