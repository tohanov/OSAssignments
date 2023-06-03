#include "types.h"
#include "riscv.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"


void mark_pte_not_accessed(pte_t *pte_ptr) {
	*pte_ptr &= ~PTE_A;
}

void update_data_structure(struct proc *process) {
	#if (SWAP_ALGO == NFUA) || (SWAP_ALGO == LAPA)
		// debug_print("inside update_data_structure");

		if (outlier_process(process)) {
			if (process != NULL) {
				// debug_print("\tOUTLIER, name=%s, returning", process->name);
			}
			else {
				// debug_print("\tOUTLIER, returning");
			}
			return;
		}

		for (Page_metadata *iterator = process->user_pages + 1; iterator < &process->user_pages[MAX_TOTAL_PAGES]; ++iterator) {
			// debug_print("\tin loop i=%d", iterator - process->user_pages);
			pte_t *pte = walk(process->pagetable, iterator->virtual_address, 0);
			
			iterator->counter >>= 1;
			iterator->counter |= (*pte & PTE_A) << (63-6);

			// *pte &= ~PTE_A;
			mark_pte_not_accessed(pte);
		}
	#endif
}


uint8 sum_bits(uint64 number) {
	uint8 counter = 0;
	
	while (number != 0) {
		counter += number%2;
		number >>= 1;
	}

	return counter;
}