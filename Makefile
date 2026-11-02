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

buddyallocator:
	$(CC) $(FLAGS) $(LIBFLAGS) $(INCLUDE) src/buddy_allocator.c -o bin/buddy_allocator.o

mm: $(UTILS) $(ALLOCS)
	$(CC) $(FLAGS) $(LIBFLAGS) $(INCLUDE) $(ALLOCOBJS) src/mm.c -o bin/mm.o

clean:
	rm -rf bin/*
