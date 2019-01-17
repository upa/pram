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

#define COUNT 1000

int main(int argc, char **argv)
{
	void *mem;
	int n, size, count = COUNT;
	struct timeval start, end;
	unsigned long elapsed_us[COUNT];
	unsigned long avg, sum, min, max;


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
	

	for (n = 0; n < count; n++) {
		gettimeofday(&start, NULL);
		*((unsigned long *)(mem + count)) = count;
		gettimeofday(&end, NULL);

		if (end.tv_usec < start.tv_usec) {
			end.tv_usec += 1000000;
			end.tv_sec -= 1;
		}

		elapsed_us[n] = ((end.tv_sec - start.tv_sec) * 1000000 +
				 end.tv_usec - start.tv_usec);
	}

	
	sum = 0;
	min = elapsed_us[0];
	max = elapsed_us[0];
	for (n = 0; n < count; n++) {

		sum += elapsed_us[n];

		if (min > elapsed_us[n])
			min = elapsed_us[n];
		if (max < elapsed_us[n])
			max = elapsed_us[n];
	}

	avg = max / count;

	printf("%lu byte write, avg=%lu, min=%lu, max=%lu\n",
	       sizeof(unsigned long), avg, min, max);
	       

	return 0;
}
