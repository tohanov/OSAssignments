#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/uthread.h"

void count()
{
    struct uthread *th = uthread_self();
    for (int i = 0; i < 10; i++) 
        {uthread_yield();}
    printf("Thread #something with priority %d finished\n",/* th->id, */th->priority);
    uthread_exit(); // thread func always needs a uthread exit call
    printf("Error: thread continues after uthread exit call (test_uthreads)\n");
}

void count_and_create()
{
    if (uthread_create(count, HIGH) == -1)
        {printf("Error: creation of new thread was unsuccessful (test_uthreads)");}
    struct uthread *th = uthread_self();
    for (int i = 0; i < 10; i++)
        {uthread_yield();}
    printf("Thread #something with priority %d finished\n", /*th->id, */th->priority);
    uthread_exit(); // thread func always needs a uthread exit call
    printf("Error: thread continues after uthread exit call (test_uthreads)\n");
}

int main()
{
    uthread_create(count, LOW);
    uthread_create(count, HIGH);
    uthread_create(count_and_create, MEDIUM); // create high priority thread during run
    uthread_create(count, LOW);
    if (uthread_create(count, HIGH) != -1)
        {printf("Error: expected uthread_create to return error (test_uthreads)\n");}
    int exit_v = uthread_start_all();
    printf("Error: bad exit message %d\n", exit_v);
    printf("Error: something went wrong shouldn't print this (test_uthreads)\n");
    return 0;
}
