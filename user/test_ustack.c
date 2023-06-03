#include "ustack.h"
#include "user.h"
#include "kernel/util.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
	int initial_num_pages = get_num_pages();
	printf("initially has %d pages\n", initial_num_pages);
	
	int num_allocations = 0;
	char *buffer = ustack_malloc(MAX_ALLOCATION);
	++num_allocations;
	// int length_written = strlen("hi there :)") + 1;
	strcpy(buffer, "hi there :)");

	while (get_num_pages() - initial_num_pages < MAX_PSYC_PAGES + 1) {
		++num_allocations;
		ustack_malloc(MAX_ALLOCATION);
		printf("after allocation #%d has %d pages\n", num_allocations, get_num_pages());
	}

	printf("before print of buffer\n");
	printf("\tbuffer=%p\n", buffer);
	printf("%s\n", buffer);
	printf("after print of buffer\n");

	int num_frees = 0;
	while (get_num_pages() > initial_num_pages) {
		++num_frees;
		ustack_free();
		printf("after free #%d has %d pages, initial_num_pages=%d\n", num_frees, get_num_pages(), initial_num_pages);
	}

	assert_print(num_frees == num_allocations);

	printf("\nallocating 17 pages and quitting to see if clearing pagetable succeeds\n");
	int num_allocations2 = 0;
	while (get_num_pages() - initial_num_pages < 17) {
		++num_allocations2;
		ustack_malloc(MAX_ALLOCATION);
		printf("after #%d allocation has %d pages\n", num_allocations2, get_num_pages());
	}

	return 0;
}