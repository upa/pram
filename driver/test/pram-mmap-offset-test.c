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
#include <sys/ioctl.h>

#include "../pram.h"


int main(int argc, char **argv)
{
	int fd, n;
	unsigned long offset;
	size_t p2pmem_size = 1024 * 1024;
	size_t slot_size = 1024 * 8;
	void *slots[p2pmem_size / slot_size];
	struct pram_addr_info pai;

	/* open pram char dev */
	fd = open("/dev/pram", O_RDWR);
	if (fd < 0) {
		perror("open /dev/pram failed");
		return -1;
	}
	
	for (n = 0; n < 8; n++) {

		offset = slot_size * n;

		slots[n] = mmap(0, slot_size, PROT_READ | PROT_WRITE,
			      MAP_LOCKED | MAP_SHARED, fd, offset);
		if (slots[n] == MAP_FAILED) {
			fprintf(stderr, "mmap failed, offset=%ld: %s\n",
				offset, strerror(errno));
			goto err_out;
		}

		pai.offset = offset;
		if (ioctl(fd, PRAMIO_PADDR, &pai) < 0) {
			fprintf(stderr,
				"ioctl(PRAMIO_PADDR) failed for %p: %s\n",
				slots[n],
				strerror(errno));
			goto err_out;
		}
		printf("mmap success, "
		       "offset=%6ld, vaddr is %p, paddr is 0x%lx\n",
		       offset, slots[n], pai.paddr);
	}

err_out:
	close(fd);
	
	return 0;
}
