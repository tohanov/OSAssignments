#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_kthread_create(void)
{
	uint64 function_ptr;
	argaddr(0, &function_ptr);

	uint64 stack_ptr;
	argaddr(1, &stack_ptr);

	int stack_size;
	argint(2, &stack_size);

	return kthread_create((void *)function_ptr, (void *)stack_ptr, stack_size);
}

uint64
sys_kthread_id(void)
{
	return mykthread()->thread_id;
}

uint64
sys_kthread_kill(void)
{
	int ktid;
	argint(0, &ktid);

	return kthread_kill(ktid);
}

uint64
sys_kthread_exit(void)
{
	// static char called = 0;
	// if (called != 0) return 0;
	// called = 1;

	int status;
	argint(0, &status);

	kthread_exit(status);

	return 0;
}

uint64
sys_kthread_join(void)
{
	int ktid;
	argint(0, &ktid);
	
	uint64 status_ptr;
	argaddr(1, &status_ptr);

	return kthread_join(ktid, (int *)status_ptr);
}
