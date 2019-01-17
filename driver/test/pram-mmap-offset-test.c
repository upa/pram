#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char **argv)
{
	int fd, n;
	size_t p2pmem_size = 1024 * 1024;
	size_t slot_size = 1024 * 8;
	void *slots[p2pmem_size / slot_size];

	/* open pram char dev */
	fd = open("/dev/pram", O_RDWR);
	if (fd < 0) {
		perror("open /dev/pram failed");
		return -1;
	}
	
	for (n = 0; n < 8; n++) {

		slots[n] = mmap(0, slot_size, PROT_READ | PROT_WRITE,
			      MAP_LOCKED | MAP_SHARED, fd, slot_size * n);
		if (slots[n] == MAP_FAILED) {
			fprintf(stderr, "mmap failed, offset=%ld: %s\n",
				slot_size * n, strerror(errno));
			goto err_out;
		}
		printf("mmap success, offset=%ld, p2pmem is %p\n",
		       slot_size * n, slots[n]);
	}

err_out:
	close(fd);
	
	return 0;
}
