#include "kernel/types.h"
// #include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
	printf("original process size = %d\n", memsize());

	void *allocatedBytes = malloc(20000);
	
	printf("process size after malloc() = %d\n", memsize());

	free(allocatedBytes);
	
	printf("process size after free() = %d\n", memsize());

	return 0;
}