
CC = gcc
INCLUDE :=
LDFLAGS := 
CFLAGS := -g -Wall $(INCLUDE)

PROGNAME = pram-mmap-test

all: $(PROGNAME)

.c.o:
	$(CC) $< -o $@

clean:
	rm -rf *.o
	rm -rf $(PROGNAME)

