#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "uthread.h"
#include "kernel/riscv.h"

struct uthread process_threads[MAX_UTHREADS] = {0};

struct uthread garbage;
struct uthread *current_thread = &garbage;

void switch_to_thread(struct uthread *next) {
	printf("inside switch_to_thread()\n");
	struct uthread *temp = current_thread;
	current_thread = next;
	uswtch(&temp->context, &next->context);
}

int uthread_create(void (*start_func)(), enum sched_priority priority) {
	struct uthread *free_spot;

	// searching for a free spot
	for (free_spot = process_threads; free_spot < &process_threads[MAX_UTHREADS]; ++free_spot) {
		// acquire(&free_spot->lock);
		if (free_spot->state == FREE) {
			break;
		}
		// release(&free_spot->lock);
	}

	if (free_spot == &process_threads[MAX_UTHREADS]) return -1;

	// found a free spot
	memset(&free_spot->context, 0, sizeof(struct context));

	free_spot->context.ra = (uint64)start_func;
	free_spot->context.sp = (uint64)free_spot->ustack + STACK_SIZE;
	free_spot->priority = priority;
	free_spot->state = RUNNABLE;

	// release(&free_spot->lock);
	return 0;
}


void uthread_yield() {
	printf("inside uthread_yield()\n");
	struct uthread *next_to_run = current_thread;
	// acquire(&current_thread->lock);
	struct uthread *iter;
	for (iter = process_threads; iter < &process_threads[MAX_UTHREADS]; ++iter) {
		// acquire(&iter->lock);
		printf("yield loop %d, state=%d, priority=%d\n",
			iter - process_threads,
			iter->state,
			iter->priority
		);

		printf("runnable=%c, priority greater=%c\n",
			iter->state == RUNNABLE ? 'T': 'F', 
			(int)iter->priority > (int)next_to_run->priority ? 'T': 'F'
		);

		if (iter->state == RUNNABLE && (int)iter->priority > (int)next_to_run->priority) {
			printf("updating next_to_run\n");
			next_to_run = iter;
		}

		printf("next_to_run: state=%d, priority=%d\n",
			// iter - process_threads,
			next_to_run->state,
			next_to_run->priority
		);
		// release(&iter->lock);
	}

	// if (iter == &process_threads[MAX_UTHREADS]) {
	// 	printf("didn't find threads to run, calling uhtread_exit()\n");
	// 	uthread_exit();
	// }

	if (next_to_run != current_thread) switch_to_thread(next_to_run);
}


void uthread_exit() {
	printf("inside uthread_exit()\n");
	current_thread->state = FREE;

	printf("before searching for first runnable\n");
	// searching table for any runnable thread
	struct uthread *any_runnable;
	for (any_runnable = process_threads; any_runnable < &process_threads[MAX_UTHREADS]; ++any_runnable) {
		if (any_runnable->state == RUNNABLE) {
			break;
		}
	}
	printf("after searching for first runnable\n");

	// if no runnable thread found - exit
	if (any_runnable == &process_threads[MAX_UTHREADS]) {
		printf("no runnable threads found, calling exit(0)\n");
		exit(0);
	}

	printf("before searching for a max priority next thread\n");
	// search table for thread with max priority
	struct uthread *next_to_run = any_runnable;
	for (struct uthread *iter = process_threads; iter < &process_threads[MAX_UTHREADS]; ++iter) {
		if (iter->state == RUNNABLE && (int)iter->priority > (int)next_to_run->priority) {
			next_to_run = iter;
		}
	}
	printf("after searching for a max priority next thread\n");
	printf("same thread=%c\n", next_to_run == current_thread?'T':'F');

	if (next_to_run != current_thread) switch_to_thread(next_to_run);
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
	printf("inside uthread_start_all()\n");

	static char started = 0;
	if (started) return -1;
	started = 1;

	garbage = (struct uthread){{0}, 0, {0}, -1};

	uthread_yield();

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

