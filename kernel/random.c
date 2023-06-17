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

static uint8 lfsr_seed;

// Linear feedback shift register
// Returns the next pseudo-random number
// The seed is updated with the returned value
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


//
// user write()s to the console go here.
//
int
randomwrite(int user_src, uint64 src, int n)
{
	printf("inside randomwrite");

	if (n != 1 || either_copyin(&lfsr_seed, user_src, src, 1) == -1)
		return -1;

	return RANDOMWRITE_SUCCESS;
//   int i;

//   for(i = 0; i < n; i++){
//     char c;
//     if(either_copyin(&c, user_src, src+i, 1) == -1)
//       break;
//     uartputc(c);
//   }

//   return i;
}


int
randomread(int user_dst, uint64 dst, int n)
{
	printf("inside randomread");
	
	int i = 0;
	for ( ; i < n; ++i) {
		uint8 cbuf = next_random_number();
		
		if(either_copyout(user_dst, dst, &cbuf, 1) == -1)
			return i;
	}

	return i;
//   uint target;
//   int c;
//   char cbuf;

//   target = n;
//   acquire(&cons.lock);
//   while(n > 0){
//     // wait until interrupt handler has put some
//     // input into cons.buffer.
//     while(cons.r == cons.w){
//       if(killed(myproc())){
//         release(&cons.lock);
//         return -1;
//       }
//       sleep(&cons.r, &cons.lock);
//     }

//     c = cons.buf[cons.r++ % INPUT_BUF_SIZE];

//     if(c == C('D')){  // end-of-file
//       if(n < target){
//         // Save ^D for next time, to make sure
//         // caller gets a 0-byte result.
//         cons.r--;
//       }
//       break;
//     }

//     // copy the input byte to the user-space buffer.
//     cbuf = c;
//     if(either_copyout(user_dst, dst, &cbuf, 1) == -1)
//       break;

//     dst++;
//     --n;

//     if(c == '\n'){
//       // a whole line has arrived, return to
//       // the user-level read().
//       break;
//     }
//   }
//   release(&cons.lock);

//   return target - n;
}

void
randominit(void)
{
	//   initlock(&cons.lock, "cons");

	//   uartinit();

	// connect read and write system calls
	// to consoleread and consolewrite.

	lfsr_seed = 0x2A;

	// TODO maybe use file->off to store seed

	devsw[RANDOM].read = randomread;
	devsw[RANDOM].write = randomwrite;
}