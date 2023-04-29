#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];
extern void forkret(void);


void kthreadinit(struct proc *p)
{

	initlock(&p->thread_ids_lock, "thread ids lock");

	for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
	{
	initlock(&kt->lock, "thread lock");
	kt->state = UNUSED;
	kt->process = p;

	// WARNING: Don't change this line!
	// get the pointer to the kernel stack of the kthread
	kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
	}
}


struct kthread *mykthread()
{
	return cpus[cpuid()].kernel_thread;
}


struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
	return p->base_trapframes + ((int)(kt - p->kthread));
}


// TODO: delte this after you are done with task 2.2
// void allocproc_help_function(struct proc *p) {
//   p->kthread->trapframe = get_kthread_trapframe(p, p->kthread);

//   p->context.sp = p->kthread->kstack + PGSIZE;
// }


int alloc_thread_id(struct proc *process) {
	int id;

	acquire(&process->thread_ids_lock);

	id = process->next_thread_id;
	++process->next_thread_id;

	release(&process->thread_ids_lock);

	return id;
}


/* static */ struct kthread* allocate_kernel_thread(struct proc *process) {

	// acquire locks?

	struct kthread *iter = process->kthread;
	struct kthread *table = process->kthread;

	for ( ; iter < &table[NKT]; ++iter) {
		acquire(&iter->lock);

		if (iter->state == KUNUSED) {
			break;
		}

		release(&iter->lock);
	}

	if (iter == &table[NKT]) return 0;

	iter->thread_id = alloc_thread_id(process);
	iter->state = USED;

	iter->trapframe = get_kthread_trapframe(process, iter);

	memset(&iter->context, 0, sizeof(iter->context));

	iter->context.ra = (uint64)forkret;
	iter->context.sp = iter->kstack + PGSIZE;
	iter->process = process;

	return iter;
}


/* static */ void free_kernel_thread(struct kthread *kernel_thread) {
	memset(kernel_thread, 0, sizeof(struct kthread));
	// *kernel_thread = {0};

	kernel_thread->state = UNUSED;
}