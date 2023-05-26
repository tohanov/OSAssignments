#include "ustack.h"
#include "user.h"

int main(int argc, char *argv[]) {
	// int allocations_counter = 0;

	printf("initially has %d pages\n", get_num_pages());
	// printf("get_num_pages() < 16: %d\n", (int)get_num_pages() < 16);
	
	ustack_malloc(MAX_ALLOCATION);
	printf("after 1 allocation has %d pages\n", get_num_pages());

	ustack_free();
	printf("after 1 free has %d pages\n", get_num_pages());
	

	// while (get_num_pages() < 16) {
	// 	++allocations_counter;
	// 	printf("allocation #%d\n", allocations_counter);

	// 	ustack_malloc(MAX_ALLOCATION);
	// }

	// printf("allocated %d times\n", allocations_counter);

	return 0;
}