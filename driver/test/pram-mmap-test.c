#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int fd;
	void *p2pmem;
	size_t p2pmem_size = 1024 * 1024;


	/* open pram char dev */
	fd = open("/dev/pram", O_RDWR);
	if (fd < 0) {
		perror("open /dev/pram failed");
		return -1;
	}
	
	p2pmem = mmap(0, p2pmem_size, PROT_READ | PROT_WRITE,
		      MAP_LOCKED | MAP_SHARED, fd, 0);
	if (p2pmem == MAP_FAILED) {
		perror("mmap failed");
		goto err_out;
	}

	printf("mmap success, p2pmem is %p\n", p2pmem);

err_out:
	close(fd);
	
	return 0;
}
