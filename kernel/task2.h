#include "types.h"

void add_user_page(uint64);
void remove_user_page(uint64);
void swap_in_user_page(uint64);
void swap_out_user_page();
void initialize_user_pages(struct proc* process);

void copy_paging_info(struct proc*, struct proc*);
void zero_out_paging_info(struct proc* p);
