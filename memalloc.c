/*
	Program name: Memory allocator
	Description: A simple memory allocator in C which has the following functions
				malloc(), calloc(), realloc(), free()
	Author: pokixdx (Anchit Mhatre)
*/

#include <unistd.h>
#include <pthread.h>

typedef char ALIGN[16];

// Creating a header to attach to every newly allocated memory block
union header
{

	struct
	{
		size_t size;
		unsigned is_free;
		// linked list implementation to keep track of non-contiguous memory blocks
		struct header_t *next;
	} s;
	// aligning end of header block to actual memory block with a 16 byte alignment
	ALIGN stub;
};
typedef union header header_t;

// header and tail pointer to keep track of the list
header_t *head, *tail;

// Thread lock mechanism to prevent two or more threads from conucrrently accessing memory
// before any action acquire this lock and after you're done, just release this lock
pthread_mutex_t global_malloc_lock;

// Check whether any memory blocks of given size are free in the linked list
// first fit approach used
header_t *get_free_block(size_t size)
{
	header_t *curr = head;
	while (curr)
	{
		// check if there is a free block which can accomodate requested size
		if (curr->s.is_free && curr->s.size >= size)
		{
			return curr;
			curr = curr->s.next;
		}
	}
	return NULL;
}

// Allocating memory and returning pointer to the allocated memory
void *malloc(size_t size)
{
	size_t total_size;
	void *block;
	header_t *header;

	// if size is 0 then we return NULL
	if (!size)
		return NULL;

	// acquiring lock for valid size
	pthread_mutex_lock(&global_malloc_lock);
	header = get_free_block(size);
	if (header)
	{
		header->s.is_free = 0;
		pthread_mutex_unlock(&global_malloc_lock);
		// hiding headers and pointing to first byte of memory block
		return (void *)(header + 1);
	}
	total_size = sizeof(header_t) + size;

	// extending heap if no sufficiently large free block is found
	block = sbrk(total_size);
	if (block == (void *)-1)
	{
		pthread_mutex_unlock(&global_malloc_lock);
		return NULL;
	}

	// Filling header with requested memory size
	header = block;
	header->s.size = size;
	header->s.is_free = 0;
	header->s.next = NULL;

	if (!head)
		head = header;
	if (tail)
		tail->s.next = header;
	tail = header;

	// releasing global lock
	pthread_mutex_unlock(&global_malloc_lock);

	// not exposing header to the caller
	return (void *)(header + 1);
}