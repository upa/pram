/* pram.h */

#ifndef _PRAM_H_
#define _PRAM_H_

struct pram_addr_info {
	unsigned long offset;	/* offset from the start of mmap reigion */
	unsigned long paddr;	/* paddr on PRAM device */
};

#define PRAMIO_PADDR _IOWR('i', 1, struct pram_addr_info)


#endif /* _PRAM_H_ */
