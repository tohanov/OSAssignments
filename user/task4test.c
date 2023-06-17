#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


#define num_chars 100

int test_random(uint8 *buf, int num) {
	int random_fd = open("random", O_RDWR);

	return read(random_fd, buf, num);
}


int find(uint8 *buf, int len, uint8 to_find) {
	for (int i = 0; i < len; ++i) {
		if (buf[i] == to_find)
			return i;
	}
	return -1;
}


void print_buf(uint8 *buf, int len) {
	printf("[");
	for (int i = 0; i < len; ++i) {
		printf("0x%x, ", buf[i]);
	}
	printf("]\n");
}


int main() {
	uint8 *buf1 = malloc(num_chars);
	uint8 *buf2 = malloc(num_chars);

	if (fork() != 0) {
		/* int read_bytes =  */test_random(buf1, num_chars);
		wait(0);

		// printf("wait finished\n");

		// printf("read %d random bytes\n", read_bytes);

		printf("buf1=");
		print_buf(buf1, num_chars);

		int i1 = find(buf1, num_chars, 0x2a);
		printf("i1=%d\n", i1);
	}
	else {
		/* int read_bytes =  */test_random(buf2, 255 - num_chars);
		// // printf("read %d random bytes\n", read_bytes);
		// printf("child finished\n");

		printf("buf2=");
		print_buf(buf2, 255 - num_chars);

		int i2 = find(buf2, 255 - num_chars, 0x2a);
		printf("i2=%d\n", i2);
	}

	// int file_fd = open("new_file", O_RDWR | O_CREATE);
	// // int random_fd = open("random", O_RDWR);
	// printf("file_fd=%d\n", file_fd);

	// write(file_fd, "abc", 3);
	// printf("seek=%d\n", seek(file_fd, -10, SEEK_CUR));
	// write(file_fd, "def", 3);
	// close(file_fd);

	// file_fd = open("new_file", O_RDWR);
	// char buf[200];
	// read(file_fd, buf, 10);

	// printf("%s\n", buf);
	return 0;
}