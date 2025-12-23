CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.

all: test_stack test_heap

test_stack: stack.o test_stack.o
	$(CC) $(CFLAGS) -o test_stack stack.o test_stack.o

test_heap: heap.o test_heap.o
	$(CC) $(CFLAGS) -o test_heap heap.o test_heap.o

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

test_stack.o: test_stack.c stack.h
	$(CC) $(CFLAGS) -c test_stack.c

heap.o: heap.c heap.h
	$(CC) $(CFLAGS) -c heap.c

test_heap.o: test_heap.c heap.h
	$(CC) $(CFLAGS) -c test_heap.c

clean:
	rm -f *.o test_stack test_heap

