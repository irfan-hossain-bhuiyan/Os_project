CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.

all: test/test_stack test/test_heap


test/test_stack: stack.o test/test_stack.o
	$(CC) $(CFLAGS) -o test/test_stack stack.o test/test_stack.o


test/test_heap: heap.o test/test_heap.o
	$(CC) $(CFLAGS) -o test/test_heap heap.o test/test_heap.o


stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

test/test_stack.o: test/test_stack.c stack.h
	$(CC) $(CFLAGS) -c test/test_stack.c -o test/test_stack.o


heap.o: heap.c heap.h
	$(CC) $(CFLAGS) -c heap.c

test/test_heap.o: test/test_heap.c heap.h
	$(CC) $(CFLAGS) -c test/test_heap.c -o test/test_heap.o


clean:
	rm -f *.o test/*.o test/test_stack test/test_heap


