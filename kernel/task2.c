#include "types.h"
#include "riscv.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"


bool outlier_process(struct proc *process) {
	// assert_print(myproc() != NULL );

	bool condition = process == NULL;
	condition = condition || strncmp(process->name,"sh", 3) == 0;
	condition = condition || strncmp(process->name,"initcode", 9) == 0;
	condition = condition || strncmp(process->name,"init", 5) == 0;

	return condition;
}


bool slot_empty(Page_metadata *slot) {
	// if slot page not present
	// assert_print(!(slot->pte_flags & PTE_V));
	// assert_print((slot->pte_flags & PTE_PG));
	// error_print("");

	return 
		/* slot->pte == 0 ||  */
		(!(slot->pte_flags & PTE_V))
		&& (!(slot->pte_flags & PTE_PG));
		// && (slot->pte_flags & PTE_U);
}


Page_metadata * get_next_to_swap_out() {
	#if SWAP_ALGO == NFUA
		debug_print("in get_next_to_swap_out");

		struct proc *process = myproc();
		Page_metadata *page_min_counter = process->user_pages;

		for (Page_metadata *iterator = process->user_pages + 1; iterator < &process->user_pages[MAX_TOTAL_PAGES]; ++iterator) {
			debug_print("\tin loop i=%d", iterator - process->user_pages);

			if (!slot_empty(iterator) && page_min_counter->counter > iterator->counter) {
				page_min_counter = iterator;
			}
		}

		debug_print("\treturn slot %d", page_min_counter - process->user_pages);
		return page_min_counter;

	#elif SWAP_ALGO == LAPA
		debug_print("in get_next_to_swap_out");

		struct proc *process = myproc();
		Page_metadata *page_min_num_ones = process->user_pages;

		for (Page_metadata *iterator = process->user_pages + 1; iterator < &process->user_pages[MAX_TOTAL_PAGES]; ++iterator) {
			debug_print("\tin loop i=%d", iterator - process->user_pages);

			if (!slot_empty(iterator)
				&& (sum_bits(page_min_num_ones->counter) > sum_bits(iterator->counter) 
					|| (sum_bits(page_min_num_ones->counter) == sum_bits(iterator->counter)
						&& page_min_num_ones->counter > iterator->counter))
			) {
				page_min_num_ones = iterator;
			}
		}

		debug_print("\treturn slot %d", page_min_num_ones - process->user_pages);
		return page_min_num_ones;

	#elif SWAP_ALGO == SCFIFO
		struct proc *process = myproc();

		int i = process->clock_handle - process->user_pages;
		Page_metadata *scfifo_result;
		
		for ( ; ; i = (i+1)%MAX_TOTAL_PAGES) {
			debug_print("\tin loop i=%d", i);

			pte_t *pte = walk(process->pagetable, process->user_pages[i].virtual_address, 0);

			if ( !slot_empty(&process->user_pages[i]) && !(*pte & PTE_A)) {
				scfifo_result = &process->user_pages[i];
				break;
			}

			// *pte &= ~PTE_A;
			mark_pte_not_accessed(pte);
		}

		return scfifo_result;
	#endif

	error_print("returning NULL, should be unreachable");
	return NULL;
	
	// debug_print("in get_next_to_swap_out");

	// struct proc *process = myproc();

	// for (int i = process->clock_handle - process->user_pages; 
	// 	i != process->clock_handle - process->user_pages - 1; 
	// 	i = (i+1)%MAX_TOTAL_PAGES) {
		
	// 	debug_print("\tin loop i=%d", i);

	// 	if ( ! slot_empty(&process->user_pages[i])) {
	// 		debug_print("\treturn slot %d", i);
	// 		return &process->user_pages[i];
	// 	}
	// }

	// error_print("returning NULL, should be unreachable");
	// return NULL;
}

void mark_invalid(Page_metadata *page) {
	page->pte_flags &= ~PTE_V;
	page->pte_flags &= ~PTE_PG;
}


bool is_swapped_out(Page_metadata *page) {
	assert_print(!(page->pte_flags & PTE_V));
	assert_print((page->pte_flags & PTE_PG));

	return 
		!(page->pte_flags & PTE_V) 
		&& (page->pte_flags & PTE_PG);
}


void mark_swapped_out(Page_metadata *page) {
	page->pte_flags &= ~PTE_V;
	page->pte_flags |= PTE_PG;
}


void mark_pte_swapped_out(pte_t *pte_ptr) {
	*pte_ptr &= ~PTE_V;
	*pte_ptr |= PTE_PG;
}


void mark_pte_swapped_in(pte_t *pte_ptr) {
	*pte_ptr |= PTE_V;
	*pte_ptr &= ~PTE_PG;
}


void mark_swapped_in(Page_metadata *page) {
	page->pte_flags |= PTE_V;
	page->pte_flags &= ~PTE_PG;
}


void swap_out_user_page() {
	debug_print("in swap_out_user_page");

	struct proc *process = myproc();

	if (process->swapFile == NULL) {
		createSwapFile(process);
	}

	Page_metadata *to_swap_out = get_next_to_swap_out();

	// write to swap file
	uint64 physical_address = walkaddr(process->pagetable, to_swap_out->virtual_address);
	/* int ret_val =  */writeToSwapFile(process, (char *)physical_address, to_swap_out->offset_in_swapfile, PGSIZE);

	// debug_print("\twriteToSwapFile wrote %d bytes", ret_val);

	// kfree
	kfree((void *)physical_address);

	// mark not present
	mark_pte_swapped_out(walk(process->pagetable, to_swap_out->virtual_address, 0));
	mark_swapped_out(to_swap_out);
}


Page_metadata *find_slot_by_va(uint64 virtual_address) {
	debug_print("in find_slot_by_va");

	struct proc *process = myproc();

	for (int i = 0; i < MAX_TOTAL_PAGES; ++i) {
		debug_print("\tin loop i=%d", i);

		assert_print(
			virtual_address != process->user_pages[i].virtual_address
			|| (virtual_address == process->user_pages[i].virtual_address 
			&& !slot_empty(&process->user_pages[i]))
		);

		if (virtual_address == process->user_pages[i].virtual_address 
			/* && !slot_empty(&process->user_pages[i]) */) {
			debug_print("\treturn slot %d", i);

			return &process->user_pages[i];
		}
	}

	error_print("returning NULL, should be unreachable");
	return NULL;
}


void swap_in_user_page(uint64 virtual_address) {
	#if SWAP_ALGO != NONE
		debug_print("in swap_in_user_page");

		struct proc *process = myproc();

		if (process->swapFile == NULL) {
			panic("process->swapFile == NULL");
		}

		Page_metadata *to_swap_in = find_slot_by_va(virtual_address);

		assert_print(is_swapped_out(to_swap_in));

		void *new_physical_address = kalloc();

		pte_t *pte_ptr = walk(process->pagetable, virtual_address, 0);
		*pte_ptr = PA2PTE(new_physical_address) | to_swap_in->pte_flags;
		mark_pte_swapped_in(pte_ptr);

		mark_swapped_in(to_swap_in);

		readFromSwapFile(process, new_physical_address, to_swap_in->offset_in_swapfile, PGSIZE);
	#endif
	virtual_address = virtual_address; // for compilation to pass
}

/*
#if SWAP_ALGO != NONE
		warning_print("in swap_in_user_page");

		struct proc *process = myproc();

		if (process->swapFile == NULL) {
			panic("process->swapFile == NULL");
		}

		Page_metadata *to_swap_in = find_slot_by_va(virtual_address);

		assert_print(is_swapped_out(to_swap_in));

		warning_print("\tbefore reading from file");
		pte_t *pte_ptr = walk(process->pagetable, virtual_address, 1);
		warning_print("\tafter reading from file");
		
		*pte_ptr |= to_swap_in->pte_flags;
		
		mark_swapped_in(to_swap_in);
		mark_pte_swapped_in(pte_ptr);

		uint64 new_physical_address = walkaddr(process->pagetable, virtual_address);// kalloc();
		readFromSwapFile(process, (void *)new_physical_address, to_swap_in->offset_in_swapfile, PGSIZE);

		#if SWAP_ALGO == NFUA
			to_swap_in->counter = 0,
		#elif SWAP_ALGO == LAPA
			to_swap_in->counter = 0xFFFFFFFF,
		#endif
	#endif
	virtual_address = virtual_address; // for compilation to pass
*/


Page_metadata * get_empty_page_slot() {
	debug_print("in get_empty_page_slot");

	struct proc *process = myproc();

	for (int i = 0; i < MAX_TOTAL_PAGES; ++i) {
		debug_print("\tin loop i=%d", i);

		if (slot_empty(&process->user_pages[i])) {
			debug_print("\treturn slot %d", i);

			return &process->user_pages[i];
		}
	}

	error_print("returning NULL, should be unreachable");
	return NULL;
}


void add_user_page(uint64 virtual_address) {
	#if SWAP_ALGO != NONE
		debug_print("in add_user_page");
		
		struct proc *process = myproc();

		if (outlier_process(process)) {
			if (process != NULL) {
				debug_print("\tOUTLIER, name=%s, returning", process->name);
			}
			else {
				debug_print("\tOUTLIER, returning");
			}
			return;
		}

		debug_print("\tnum_user_pages=%d", process->num_user_pages);
		
		if (process->num_user_pages >= MAX_TOTAL_PAGES) {
			panic("process->num_user_pages >= MAX_TOTAL_PAGES");
		}

		if (process->num_user_pages >= MAX_PSYC_PAGES) {
			swap_out_user_page();
		}

		Page_metadata *free_page_slot = get_empty_page_slot();

		debug_print("\treturned pte = %x", * (walk(process->pagetable, virtual_address, 0)));
		debug_print("\tisolated pte flags = %x", PTE_FLAGS( * (walk(process->pagetable, virtual_address, 0))));

		*free_page_slot = (Page_metadata) {
			// .allocated = 1,
			.pte_flags = PTE_FLAGS( * (walk(process->pagetable, virtual_address, 0))),
			.virtual_address = virtual_address,
			// .physical_address = walkaddr(process->pagetable, virtual_address),
			// .offset_in_swapfile = (free_page_slot - process->user_pages) * PGSIZE,
		};

		++process->num_user_pages;
	#endif
}

// 💩💩💩💩💩💩💩💩💩💩💩💩💩💩💩💩💩💩💩💩💩

void remove_user_page(uint64 virtual_address) {
	#if SWAP_ALGO != NONE
		debug_print("inside remove_user_page");

		struct proc *process = myproc();

		// assumes kfree has been called for the page

		if (outlier_process(process)) {
			if (process != NULL) {
				debug_print("\tOUTLIER, name=%s, returning", process->name);
			}
			else {
				debug_print("\tOUTLIER, returning");
			}
			return;
		}

		Page_metadata *to_remove = find_slot_by_va(virtual_address);

		mark_invalid(to_remove);

		--process->num_user_pages;
		
		// if (process->num_user_pages >= MAX_TOTAL_PAGES) {
		// 	panic("process->num_user_pages >= MAX_TOTAL_PAGES");
		// }

		// if (process->num_user_pages >= MAX_PSYC_PAGES) {
		// 	swap_out_user_page();
		// }

		// Page_metadata *free_page_slot = get_free_page_slot();

		// *free_page_slot = (Page_metadata){
		// 	.allocated = 0,
		// 	.pte = 0,
		// 	.va = 0,
		// 	.offset_in_swapfile = 0,
		// };
	#endif
}


void initialize_user_pages(struct proc* process) {
	#if SWAP_ALGO != NONE
		debug_print("in initialize_user_pages");

		if (outlier_process(process)) {
			if (process != NULL) {
				debug_print("\tOUTLIER, name=%s, returning", process->name);
			}
			else {
				debug_print("\tOUTLIER, returning");
			}
			return;
		}

		debug_print("\tNORMAL, name=%s", process->name);

		process->swapFile = NULL;

		process->num_user_pages = 0;
		
		#if SWAP_ALGO == SCFIFO
			process->clock_handle = process->user_pages;
		#endif

		debug_print("\tbefore loop");

		for (Page_metadata *page_metadata = process->user_pages;
			page_metadata < &process->user_pages[MAX_TOTAL_PAGES];
			++page_metadata) {

			*page_metadata = (Page_metadata) {
				// .allocated = 0,
				.pte_flags = 0 & ~PTE_V & ~PTE_PG, // invalid (not present) and not swapped out
				.virtual_address = NULL,
				// .physical_address = NULL,
				.offset_in_swapfile = (page_metadata - process->user_pages) * PGSIZE,
				
				#if SWAP_ALGO == NFUA
					.counter = 0,
				#elif SWAP_ALGO == LAPA
					.counter = 0xFFFFFFFF,
				#endif
			};

		}

		debug_print("\tafter loop");
	#endif
}


void zero_out_paging_info(struct proc* process) {
	#if SWAP_ALGO != NONE
		debug_print("in zero_out_paging_info");

		if (outlier_process(process)) {
			if (process != NULL) {
				debug_print("\tOUTLIER, name=%s, returning", process->name);
			}
			else {
				debug_print("\tOUTLIER, returning");
			}
			return;
		}

		process->num_user_pages = 0;

		if (process->swapFile == NULL) {
			#ifdef DEBUG_PRINT
				int return_value = createSwapFile(process);
				debug_print("createSwapFile returned %d", return_value);
			#else
				createSwapFile(process);
			#endif
		}

		debug_print("\twriting zeros to file");
		removeSwapFile(process);
		createSwapFile(process);

		debug_print("\twriting zeros to user_pages");
		for(int i = 0; i < MAX_TOTAL_PAGES; ++i){
			Page_metadata *page = &process->user_pages[i];
			page->pte_flags = 0;
			page->virtual_address = NULL;
		}

		// for(int i = 0; i < MAX_TOTAL_PAGES; ++i){
		// 	Page_metadata *source_page = &source->user_pages[i];
		// 	Page_metadata *destination_page = &destination->user_pages[i];

		// 	char *swapping_buffer = kalloc();
		// 	int num_read_bytes = readFromSwapFile(source, swapping_buffer, source_page->offset_in_swapfile, PGSIZE);
		// 	writeToSwapFile(destination, swapping_buffer, destination_page->offset_in_swapfile, num_read_bytes);
		// 	kfree((void*)swapping_buffer);

		// 	destination_page->pte_flags = source_page->pte_flags;
		// 	destination_page->virtual_address = source_page->virtual_address;
		// 	// destination_page->offset_in_swapfile = source_page->offset_in_swapfile;
		// }

		// char buffer[PGSIZE];
		// writeToSwapFile(process, buffer, 0, PGSIZE);
	#endif
}


void copy_paging_info(struct proc *source, struct proc *destination) {
	#if SWAP_ALGO != NONE
		debug_print("in copy_paging_info");
	
		if (outlier_process(source) || outlier_process(destination)) {
			debug_print("\tone of the processes is an OUTLIER, returning");
			return;
		}

		if (destination->swapFile == NULL) {
			createSwapFile(destination);
		}

		if (source->swapFile == NULL) {
			return;
		}

		for(int i = 0; i < MAX_TOTAL_PAGES; ++i){
			Page_metadata *source_page = &source->user_pages[i];
			Page_metadata *destination_page = &destination->user_pages[i];

			char *swapping_buffer = kalloc();
			int num_read_bytes = readFromSwapFile(source, swapping_buffer, source_page->offset_in_swapfile, PGSIZE);
			writeToSwapFile(destination, swapping_buffer, destination_page->offset_in_swapfile, num_read_bytes);
			kfree((void*)swapping_buffer);

			destination_page->pte_flags = source_page->pte_flags;
			destination_page->virtual_address = source_page->virtual_address;
			// destination_page->offset_in_swapfile = source_page->offset_in_swapfile;
		}

		// char buffer[PGSIZE];

		// readFromSwapFile(source, buffer, 0, PGSIZE);
		// debug_print("\tbytesRead=%d", readFromSwapFile(source, buffer, 0, PGSIZE));

		// writeToSwapFile(destination, buffer, 0, /* bytesRead */PGSIZE);
	#endif
}