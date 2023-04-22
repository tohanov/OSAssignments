#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "uthread.h"

struct uthread process_threads[MAX_UTHREADS] = {0};
struct uthread *current_thread = 0;

void switch_to_thread(struct uthread *next) {
	current_thread = next;
	uswtch(&current_thread->context, &next->context);
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

	free_spot->context.ra = start_func;
	free_spot->context.sp = free_spot->ustack;
	free_spot->priority = priority;
	free_spot->state = RUNNABLE;

	// release(&free_spot->lock);
	return 0;
}


void uthread_yield() {
	struct uthread *next_to_run = current_thread;
	// acquire(&current_thread->lock);

	for (struct uthread *iter = process_threads; iter < &process_threads[MAX_UTHREADS]; ++iter) {
		// acquire(&iter->lock);
		if (iter->state == RUNNABLE && iter->priority > next_to_run->priority) {
			next_to_run = iter;
		}
		// release(&iter->lock);
	}

	switch_to_thread(next_to_run);
}


void uthread_exit() {
	current_thread->state = FREE;

	// searching table for any runnable thread
	struct uthread *any_runnable;
	for (any_runnable = process_threads; any_runnable < &process_threads[MAX_UTHREADS]; ++any_runnable) {
		if (any_runnable->state == RUNNABLE) {
			break;
		}
	}

	// if no runnable thread found - exit
	if (any_runnable == &process_threads[MAX_UTHREADS]) exit(0);

	// search table for thread with max priority
	struct uthread *next_to_run = any_runnable;
	for (struct uthread *iter = process_threads; iter < &process_threads[MAX_UTHREADS]; ++iter) {
		if (iter->state == RUNNABLE && iter->priority > next_to_run->priority) {
			next_to_run = iter;
		}
	}

	switch_to_thread(next_to_run);
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
	static char started = 0;

	if (started) return -1;

	started = 1;

	uthread_yield();
}


struct uthread* uthread_self() {
	return current_thread;
}

