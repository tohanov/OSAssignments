#include "kernel/types.h"
#include "kernel/riscv.h"

#define MAX_ALLOCATION 512 // bytes

#define WITHOUT_HEADER(block) ((void *)block + sizeof(Header))
#define END_OF_BLOCK(block) ((void *)block + sizeof(Header) + block->size)

void *ustack_malloc(uint len);

int ustack_free(void);