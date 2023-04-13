#include "kernel/types.h"
// #include "kernel/stat.h"
#include "user/user.h"

void run_test(int sleep_time) {
	for (int i = 1; i <= 1e6; ++i) {
		// computation to slow-down execution of process
		for (int j= 1; j <= 1000; ++j) {}
		
		if (i % (int)1e5 == 0) sleep(1);
	}

	int current_pid = getpid();

	uint64 cfs_details[4];
	get_cfs_priority(current_pid, cfs_details);

	sleep(sleep_time*5);

	printf("pid=%d, cfs_priority=%d, rtime=%d, stime=%d, retime=%d\n", 
		current_pid,
		cfs_details[0],
		cfs_details[1],
		cfs_details[2],
		cfs_details[3]
	);
}

int
main(int argc, char *argv[])
{
	const int CHILD_NUM = 6;

	for (int i = 0; i < CHILD_NUM; ++i) {
		set_cfs_priority(i % 3);

		if (fork() == 0) {
			run_test(i);
			return 0;
		}
	}
	
	while (wait(0, 0) != -1) {}

	return 0;
}