#include "kernel/types.h"
// #include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
	int a = atoi(argv[1]);
	
	printf("Trying to set policy to %d: ", a);
	if ((a = set_policy(a)) < 0) exit(a, "Error");
	
	exit(a, "Success");
}