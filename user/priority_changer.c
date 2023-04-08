#include "kernel/types.h"
// #include "kernel/stat.h"
#include "user/user.h"

void try_change_to(int priority) {
	printf("changing priority to %d\n", priority);
	set_ps_priority(priority);
	printf("current priority: %d\n", get_ps_priority());
}

int
main(int argc, char *argv[]) {
	printf("current priority: %d\n", get_ps_priority());
	
	try_change_to(11);
	try_change_to(0);
	try_change_to(10);
	try_change_to(1);
	try_change_to(2);
	try_change_to(9);

	return 0;
}