#include "ustack.h"
#include "user.h"
#include "kernel/util.h"

int main(int argc, char *argv[]) {
	int initial_num_pages = get_num_pages();
	printf("initially has %d pages\n", initial_num_pages);
	
	int num_allocations = 0;
	while (get_num_pages() - initial_num_pages < 17) {
		++num_allocations;
		ustack_malloc(MAX_ALLOCATION);
		printf("after #%d allocation has %d pages\n", num_allocations, get_num_pages());
	}

	int num_frees = 0;
	while (get_num_pages() > initial_num_pages) {
		++num_frees;
		ustack_free();
		printf("after #%d free has %d pages, initial_num_pages=%d\n", num_frees, get_num_pages(), initial_num_pages);
	}

	assert_print(num_frees == num_allocations);

	return 0;
}