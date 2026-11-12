CC = gcc
FLAGS = -Og -g -Wall
LIBFLAGS = -c
INCLUDE = -I. -Iinclude

UTILS = spinlock list string
ALLOCS = slaballocator
ALL = $(UTILS) $(ALLOCS) $(LIBS)

ALLOCOBJS = bin/slab_allocator.o
ALLOBJS = $(UTILOBJS) $(ALLOCOBJS) $(LIBOBJS)

slaballocator:
	$(CC) $(FLAGS) $(LIBFLAGS) $(INCLUDE) src/slab_allocator.c -o bin/slab_allocator.o

spinlock: 
	$(CC) $(FLAGS) $(LIBFLAGS) $(INCLUDE) util/spinlock.c -o bin/spinlock.o

list:
	$(CC) $(FLAGS) $(LIBFLAGS) $(INCLUDE) util/list.c -o bin/list.o

string: 
	$(CC) $(FLAGS) $(LIBFLAGS) $(INCLUDE) util/string.c -o bin/string.o

buddyallocator:
	$(CC) $(FLAGS) $(LIBFLAGS) $(INCLUDE) src/buddy_allocator.c -o bin/buddy_allocator.o

mm: $(UTILS) $(ALLOCS)
	$(CC) $(FLAGS) $(LIBFLAGS) $(INCLUDE) $(ALLOCOBJS) sys/mem/mem.c -o bin/mem.o

test: $(UTILS) $(ALLOCS) buddyallocator mm
	$(CC) $(FLAGS) $(INCLUDE) test/mm_test.c bin/slab_allocator.o bin/buddy_allocator.o bin/mem.o bin/spinlock.o bin/list.o bin/string.o -o bin/mm_test

run: test buddyallocator slaballocator spinlock list string mm
	./bin/mm_test

clean:
	rm -rf bin/*
