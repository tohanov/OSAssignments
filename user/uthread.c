#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "uthread.h"
#include "kernel/riscv.h"

static struct uthread process_threads[MAX_UTHREADS] = {0};

static struct uthread garbage = (struct uthread){{0}, 0, {0}, -1};
static struct uthread *current_thread = &garbage;


void switch_to_thread(struct uthread *next) {
	// printf("inside switch_to_thread()\n");
	struct uthread *temp = current_thread;

	current_thread = next;
	next->state = RUNNING;

	uswtch(&temp->context, &next->context);
}


struct uthread *get_next_to_run(struct uthread *start_scan, enum sched_priority priority) {
	struct uthread *iterator = start_scan;

	do {
		iterator = process_threads + (iterator - process_threads + 1) % MAX_UTHREADS;

		if (iterator->state == RUNNABLE && iterator->priority == priority) {
			return iterator;
		}

	} while(iterator != start_scan);

	return 0;

	// for (iterator = start_scan; iterator != start_scan; 
	// 	iterator = process_threads + ((iterator+1)-process_threads) % MAX_UTHREADS) {
		// acquire(&iter->lock);
	// 	printf("yield loop %d, state=%d, priority=%d\n",
	// 		iterator - process_threads,
	// 		iterator->state,
	// 		iterator->priority
	// 	);

	// 	printf("runnable=%c, priority greater=%c\n",
	// 		iterator->state == RUNNABLE ? 'T': 'F', 
	// 		(int)iterator->priority > (int)next_to_run->priority ? 'T': 'F'
	// 	);

	// 	if (iterator->state == RUNNABLE && (int)iterator->priority > (int)next_to_run->priority) {
	// 		printf("updating next_to_run\n");
	// 		next_to_run = iterator;
	// 	}

	// 	printf("next_to_run: state=%d, priority=%d\n",
	// 		// iter - process_threads,
	// 		next_to_run->state,
	// 		next_to_run->priority
	// 	);
	// 	// release(&iter->lock);
	// }
}


void schedule_next(struct uthread *start_scan) {
	struct uthread *next_to_run;

	if ((next_to_run = get_next_to_run(start_scan, HIGH)) != 0 
		|| (next_to_run = get_next_to_run(start_scan, MEDIUM)) != 0
		|| (next_to_run = get_next_to_run(start_scan, LOW)) != 0) {
			// TODO see if need to assert different next thread than current
			switch_to_thread(next_to_run);
	}
	else exit(0);
}


int uthread_create(void (*start_func)(), enum sched_priority priority) {
	struct uthread *free_spot;

	// searching for a free spot
	for (free_spot = process_threads; free_spot < &process_threads[MAX_UTHREADS]; ++free_spot) {
		if (free_spot->state == FREE) {
			break;
		}
	}

	if (free_spot == &process_threads[MAX_UTHREADS]) return -1;

	// found a free spot
	memset(&free_spot->context, 0, sizeof(struct context));

	free_spot->context.ra = (uint64)start_func;
	free_spot->context.sp = (uint64)free_spot->ustack + STACK_SIZE;
	free_spot->priority = priority;
	free_spot->state = RUNNABLE;

	return 0;
}


void uthread_yield() {
	// printf("inside uthread_yield()\n");
	// struct uthread *next_to_run = current_thread;
	// acquire(&current_thread->lock);
	// struct uthread *iter;

	// FIXME do priority scheduling according to task
	//  change next thread's state to running

	current_thread->state = RUNNABLE;
	schedule_next(current_thread);
}


void uthread_exit() {
	// printf("inside uthread_exit()\n");
	current_thread->state = FREE;

	// printf("before searching for first runnable\n");
	// searching table for any runnable thread
	// struct uthread *any_runnable;
	// for (any_runnable = process_threads; any_runnable < &process_threads[MAX_UTHREADS]; ++any_runnable) {
	// 	if (any_runnable->state == RUNNABLE) {
	// 		break;
	// 	}
	// }
	// printf("after searching for first runnable\n");

	// if no runnable thread found - exit
	// if (any_runnable == &process_threads[MAX_UTHREADS]) {
	// 	printf("no runnable threads found, calling exit(0)\n");
	// 	exit(0);
	// }

	// printf("before searching for a max priority next thread\n");
	// search table for thread with max priority
	// struct uthread *next_to_run = any_runnable;
	// // FIXME round-robin
	// for (struct uthread *iter = process_threads; iter < &process_threads[MAX_UTHREADS]; ++iter) {
	// 	if (iter->state == RUNNABLE && (int)iter->priority > (int)next_to_run->priority) {
	// 		next_to_run = iter;
	// 	}
	// }
	// printf("after searching for a max priority next thread\n");
	// printf("same thread=%c\n", next_to_run == current_thread?'T':'F');
	// FIXME maybe not needed
	// if (next_to_run != current_thread) switch_to_thread(next_to_run);

	schedule_next(current_thread);
}


enum sched_priority uthread_set_priority(enum sched_priority priority) {
	enum sched_priority old_priority = current_thread->priority;

	current_thread->priority = priority;

	return old_priority;
}


enum sched_priority uthread_get_priority() {
	return current_thread->priority;
}


int uthread_start_all() {
	// printf("inside uthread_start_all()\n");

	static char started = 0;
	if (started) return -1;
	started = 1;

	// uthread_yield();
	schedule_next(&process_threads[MAX_UTHREADS - 1]);

	return 0;
}

// struct uthread {
//     char                ustack[STACK_SIZE];  // the thread's stack
//     enum tstate         state;          // FREE, RUNNING, RUNNABLE
//     struct context      context;        // uswtch() here to run process
//     enum sched_priority priority;       // scheduling priority
// };

struct uthread* uthread_self() {
	return current_thread;
}

