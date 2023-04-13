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
		char exit_msg[32]; // as1ts3

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

// clamp between min and max, inclusive
/* as1ts5 */ void clamp_int(int *variable, int min, int max) {
	*variable =
		(*variable > max) ? max
		: (*variable < min) ? min
		: *variable;
}

/* as1ts5 */ uint64
sys_set_ps_priority(void)
{
	int priority;
	argint(0, &priority);

	clamp_int(&priority, 1, 10);

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

/* as1ts6 */ uint64
sys_set_cfs_priority(void)
{
	int priority;
	argint(0, &priority);

	if (priority > LOW || priority < HIGH) return -1;

	struct proc *current_proc = myproc();

	acquire(&current_proc->lock);
	current_proc->cfs_priority = priority;
	release(&current_proc->lock);

	return 0;
}

/* as1ts6 */ uint64
sys_get_cfs_priority(void)
{
	int target_pid;
	argint(0, &target_pid);

	uint64 destination_address;
	argaddr(1, &destination_address);

	struct proc *target_proc = find_proc_by_pid(target_pid);

	uint64 return_array[4];

	if (target_proc != NULL) { // if found and locked proc
		return_array[0] = target_proc->cfs_priority;
		return_array[1] = target_proc->rtime;
		return_array[2] = target_proc->stime;
		return_array[3] = target_proc->retime;

		release(&target_proc->lock);
	}
	else { // proc not found
		for (int i = 0; i < 4; ++i) {
			return_array[i] = -1;
		}
	}

	return copyout(
		myproc()->pagetable, 
		destination_address,
		(char*)return_array, 
		4*sizeof(uint64)
	);
}


/* as1ts7*/ uint64 
sys_set_policy(void) {
	int policy;
	argint(0, &policy);

	if (policy < 0 || policy > 2) return -1;

	acquire(&sched_policy_lock);
	sched_policy = policy;
	release(&sched_policy_lock);

	return 0;
}