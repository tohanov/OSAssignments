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

	// added for assignments 
		char exit_msg[32];

		if (argstr(1, exit_msg, 32) < 0) { // copy from user space to kernel space
			exit_msg[31] = '\0';
		}
	// =====================

	argint(0, &n);
	exit(n, exit_msg);
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


	// added for assignments
	uint64 exit_msg; // as1ts2
	argaddr(1, &exit_msg); // as1ts2
	// =====================

  return wait(p, exit_msg);
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

/* as1ts2 */ uint64
sys_memsize(void)
{
  return myproc()->sz;
}

/* as1ts5 */ uint64
sys_set_ps_priority(void)
{
	int priority;
	argint(0, &priority);

	// clamp between 1 and 10, inclusive
	priority = (priority > 10) ? 10 : priority;
	priority = (priority < 1) ? 1 : priority;

	struct proc *current_proc = myproc();

	acquire(&current_proc->lock);
	current_proc->ps_priority = priority;
	release(&current_proc->lock);

	return 0; // required for compilation to pass
}

/* as1ts5 */ uint64
sys_get_ps_priority(void)
{
	return myproc()->ps_priority;
}