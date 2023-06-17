#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

#define RANDOMWRITE_SUCCESS 1

struct spinlock lfsr_lock;
static uint8 lfsr_seed;


uint8 lfsr_char(uint8 lfsr)
{
	uint8 bit;
	bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 0x01;
	lfsr = (lfsr >> 1) | (bit << 7);
	return lfsr;
}


uint8 next_random_number(void) {
	uint8 next = lfsr_char(lfsr_seed);
	lfsr_seed = next;
	return next;
}


int
randomwrite(int user_src, uint64 src, int n)
{
	acquire(&lfsr_lock);
	if (n != 1 || either_copyin(&lfsr_seed, user_src, src, 1) == -1) {
		release(&lfsr_lock);
		return -1;
	}
	release(&lfsr_lock);

	return RANDOMWRITE_SUCCESS;
}


int
randomread(int user_dst, uint64 dst, int n)
{
	int i = 0;
	for ( ; i < n; ++i) {
		acquire(&lfsr_lock);
		uint8 byte_buf = next_random_number();
		release(&lfsr_lock);
		
		if(either_copyout(user_dst, dst + i, &byte_buf, 1) == -1)
			return i;
	}

	return i;
}

void
randominit(void)
{
	initlock(&lfsr_lock, "lfsr_lock");

	lfsr_seed = 0x2A;

	devsw[RANDOM].read = randomread;
	devsw[RANDOM].write = randomwrite;
}