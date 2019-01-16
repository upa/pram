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

static uintptr_t phy_addr(void* virt) {
    int fd;
    long pagesize;
    off_t ret;
    ssize_t rc;
    uintptr_t entry = 0;

    fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0)
	    printf("open /proc/self/pagemap: %s", strerror(errno));

    pagesize = sysconf(_SC_PAGESIZE);

    ret = lseek(fd, (uintptr_t)virt / pagesize * sizeof(uintptr_t), SEEK_SET);
    if (ret < 0)
	    printf("lseek for /proc/self/pagemap: %s\n", strerror(errno));


    rc = read(fd, &entry, sizeof(entry));
    if (rc < 1 || entry == 0)
	    printf("read for /proc/self/pagemap: %s\n", strerror(errno));

    close(fd);

    return (entry & 0x7fffffffffffffULL) * pagesize +
           ((uintptr_t)virt) % pagesize;
}

int main(int argc, char **argv)
{
	int fd;
	void *p2pmem;
	size_t p2pmem_size = 1024 * 1024;
	uintptr_t paddr;
	char *p;

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

	paddr = phy_addr(p2pmem);

	printf("mmap success, p2pmem is %p, paddr is %ld\n", p2pmem, paddr);

	printf("put 01, 02, 03 ... 0xFF from start\n");
	int n;
	for (n = 0; n < 1024 * 512; n++) {
		p = p2pmem + n;
		*p = n;
	}

err_out:
	close(fd);
	
	return 0;
}
