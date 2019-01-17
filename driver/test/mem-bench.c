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
#include <sys/time.h>

enum {
	dram,
	pram,
};

void *alloc_mem(int type, size_t size)
{
		int fd;
	void *mem;

	switch(type) {
	case dram:
		mem = malloc(size);
		if (mem == NULL) {
			perror("malloc");
			exit(ENOBUFS);
		}
		break;

	case pram:
		/* open pram char dev */
		fd = open("/dev/pram", O_RDWR);
		if (fd < 0) {
			perror("open /dev/pram failed");
			exit(EINVAL);
		}
	
		mem = mmap(0, size, PROT_READ | PROT_WRITE,
			      MAP_LOCKED | MAP_SHARED, fd, 0);
		if (mem == MAP_FAILED) {
			perror("mmap failed");
			close(fd);
			exit(EINVAL);
		}
		break;
	}

	return mem;
}


#define MAX_MEM_SIZE (1024 * 1024)

void usage(void)
{
	printf("usage: mem-bench [SIZE(byte)] [dram|pram]\n");
}

int main(int argc, char **argv)
{
	void *mem;
	int n, m, size, count = 10000;
	unsigned long *p;
	unsigned long one = 0xFFFFFFFFFFFFFFFF;
	unsigned long zero = 0x0;
	struct timeval start, end;
	double elapsed_us;
	double throughput;	/* byte per sec */

	if (argc < 3) {
		usage();
		return 1;
	}

	size = atoi(argv[1]);

	if (size < 0 || size > MAX_MEM_SIZE) {
		printf("size must be greater than 0, less than %d\n",
		       MAX_MEM_SIZE);
		return 1;
	}
	
	if (strncmp(argv[2], "pram", 4) == 0)
		mem = alloc_mem(pram, size);
	else if (strncmp(argv[2], "dram", 4) == 0)
		mem = alloc_mem(dram, size);
	else {
		printf("invalid memory type '%s'\n", argv[2]);
		return 1;
	}
	
	gettimeofday(&start, NULL);
	for (n = 0; n < count; n += 2) {
		for (m = 0; m < size; m += sizeof(unsigned long)) {
			p = (unsigned long *)(mem + m);
			*p = one;
		}
		for (m = 0; m < size; m += sizeof(unsigned long)) {
			p = (unsigned long *)(mem + m);
			*p = zero;
		}
	}
	gettimeofday(&end, NULL);
	
	if (end.tv_usec < start.tv_usec) {
		end.tv_usec += 1000000;
		end.tv_sec -= 1;
	}

	elapsed_us = (double)((end.tv_sec - start.tv_sec) * 1000000 +
		      end.tv_usec - start.tv_usec);

	throughput = (double)(size * count) / elapsed_us * 1000000;

	printf("%d Bytes in %fus, %.2lf B/sec, %.2lf GB/sec\n",
	       size * count, elapsed_us / 1000000, throughput,
	       throughput / 1024 / 1024 / 1024);

	return 0;
}
