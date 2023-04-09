#include "kernel/types.h"
// #include "kernel/stat.h"
#include "user/user.h"

void run_test(int sleep_seconds) {
	uint64 sum = 0;

	for (int i = 1; i <= 10; ++i) {
		for (int i = 1; i <= 1e5; ++i) {
			sum ^= i; // so the loop doesn't get optimized-out
		}
		// sleep(5);
		sleep(1);
	}

	int current_pid = getpid();

	uint64 cfs_details[4];
	get_cfs_priority(current_pid, cfs_details);

	sleep(sleep_seconds);

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
	if (fork() == 0) { // start with normal and fork
		run_test(0);
		return 0;
	}

	set_cfs_priority(0); // high 
	if (fork() == 0) {
		run_test(10);
		return 0;
	}
	
	set_cfs_priority(2); // low
	if (fork() == 0) {
		run_test(20);
		return 0;
	}
	
	// set_cfs_priority(2); // low
	// if (fork() == 0) {
	// 	run_test(30);
	// 	return 0;
	// }
	
	// set_cfs_priority(2); // low
	// if (fork() == 0) {
	// 	run_test(40);
	// 	return 0;
	// }
	
	// set_cfs_priority(2); // low
	// if (fork() == 0) {
	// 	run_test(50);
	// 	return 0;
	// }
	
	// set_cfs_priority(2); // low
	// if (fork() == 0) {
	// 	run_test(60);
	// 	return 0;
	// }
	
	// set_cfs_priority(2); // low
	// if (fork() == 0) {
	// 	run_test(70);
	// 	return 0;
	// }
	
	// set_cfs_priority(2); // low
	// if (fork() == 0) {
	// 	run_test(80);
	// 	return 0;
	// }
	
	// set_cfs_priority(2); // low
	// if (fork() == 0) {
	// 	run_test(90);
	// 	return 0;
	// }
	
	while (wait(0, 0) != -1) {}

	return 0;
}