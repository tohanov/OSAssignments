#include "types.h"
#include "riscv.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"


bool outlier_process() {
	assert(myproc() != NULL );

	return 
		myproc() == NULL 
		|| strncmp(myproc()->name,"sh", 3) == 0 
		|| strncmp(myproc()->name,"initcode", 9) == 0
		|| strncmp(myproc()->name,"init", 5) == 0;
}


void swap_out() {
	debug_print("in swap_out");
	

	// TODO
}


bool slot_empty(Page_metadata *slot) {
	// if slot page not present
	return /* slot->pte == NULL ||  */!(slot->pte & PTE_V);
}


Page_metadata * get_empty_page_slot() {
	debug_print("in get_empty_page_slot");

	struct proc *process = myproc();

	for (int i = 0; i < MAX_TOTAL_PAGES; ++i) {
		debug_print("\tin loop i=%d", i);

		if (slot_empty(&process->user_pages[i])) {
			return &process->user_pages[i];
		}
	}

	error_print("returning NULL, should be unreachable");
	return NULL;
}


void add_user_page(uint64 virtual_address, uint64 physical_address) {
	struct proc *process = myproc();
	
	if (outlier_process()) {
		if (myproc() != NULL) {
			debug_print("in add_user_page, OUTLIER, name=%s, returning", myproc()->name);
		}
		else {
			debug_print("in add_user_page, OUTLIER, returning");
		}
		return;
	}

	debug_print("in add_user_page, num_user_pages=%d", process->num_user_pages);
	
	if (process->num_user_pages >= MAX_TOTAL_PAGES) {
		panic("process->num_user_pages >= MAX_TOTAL_PAGES");
	}

	if (process->num_user_pages >= MAX_PSYC_PAGES) {
		swap_out();
	}

	Page_metadata *free_page_slot = get_empty_page_slot();

	*free_page_slot = (Page_metadata){
		// .allocated = 1,
		.pte = *(walk(process->pagetable, virtual_address, 0)),
		.virtual_address = virtual_address,
		.physical_address = walkaddr(process->pagetable, virtual_address),
		.offset_in_swapfile = (free_page_slot - process->user_pages) * PGSIZE,
	};

	++process->num_user_pages;
}


void remove_user_page(uint64 virtual_address, uint64 physical_address) {
	// struct proc *process = myproc();

	if (outlier_process()) {
		if (myproc() != NULL) {
			debug_print("in add_user_page, OUTLIER, name=%s, returning", myproc()->name);
		}
		else {
			debug_print("in add_user_page, OUTLIER, returning");
		}
		return;
	}

	// TODO

	--myproc()->num_user_pages;
	
	// if (process->num_user_pages >= MAX_TOTAL_PAGES) {
	// 	panic("process->num_user_pages >= MAX_TOTAL_PAGES");
	// }

	// if (process->num_user_pages >= MAX_PSYC_PAGES) {
	// 	swap_out();
	// }

	// Page_metadata *free_page_slot = get_free_page_slot();

	// *free_page_slot = (Page_metadata){
	// 	.allocated = 0,
	// 	.pte = 0,
	// 	.va = 0,
	// 	.offset_in_swapfile = 0,
	// };
}


void initialize_user_pages(struct proc* process) {
	debug_print("in initialize_user_pages");
	process->num_user_pages = 0;
	process->next_to_swap_out = process->user_pages;

	for (Page_metadata *page_metadata = process->user_pages;
		page_metadata < &process->user_pages[MAX_TOTAL_PAGES];
		++page_metadata) {

		*page_metadata = (Page_metadata){
			// .allocated = 0,
			.pte = NULL & ~PTE_V & ~PTE_PG, // invalid (not present) and not swapped out
			.virtual_address = NULL,
			.physical_address = NULL,
			.offset_in_swapfile = (page_metadata - process->user_pages) * PGSIZE,
		};

	}
}