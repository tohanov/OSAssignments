#include "ustack.h"
#include "user.h"
#include "kernel/util.h"

int main(int argc, char *argv[]) {
	// int allocations_counter = 0;

	int initial_num_pages = get_num_pages();
	int num_allocations = 0;
	printf("initially has %d pages\n", initial_num_pages);
	// printf("get_num_pages() < 16: %d\n", (int)get_num_pages() < 16);
	
	while (get_num_pages() - initial_num_pages < 2) {
		++num_allocations;
		ustack_malloc(MAX_ALLOCATION);
		printf("after #%d allocation has %d pages\n", num_allocations, get_num_pages());

	}

	int num_frees = 0;

	while (get_num_pages() > initial_num_pages) {
		++num_frees;
		ustack_free();
		printf("after #%d free has %d pages, initial_num_pages=%d\n", num_frees, get_num_pages(), initial_num_pages);

		// if (num_frees == 9) {
		// 	debug_print("get_num_pages()=%d, initial_num_pages=%d, breaking forcefully", get_num_pages(), initial_num_pages);
		// 	break;
		// }
	}
	

	// while (get_num_pages() < 16) {
	// 	++allocations_counter;
	// 	printf("allocation #%d\n", allocations_counter);

	// 	ustack_malloc(MAX_ALLOCATION);
	// }

	// printf("allocated %d times\n", allocations_counter);

	return 0;
}