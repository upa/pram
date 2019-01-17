
CC = gcc
INCLUDE :=
LDFLAGS := 
CFLAGS := -g -Wall $(INCLUDE)

PROGNAME = pram-mmap-test pram-mmap-offset-test	\
	   mem-bench mem-bench2

all: $(PROGNAME)

.c.o:
	$(CC) $< -o $@

clean:
	rm -rf *.o
	rm -rf $(PROGNAME)

