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
	}s;
	// aligning end of header block to actual memory block with a 16 byte alignment
	ALIGN stub;
};
typedef union header header_t;

// header and tail pointer to keep track of the list
header_t  *head, *tail;

// Thread lock mechanism to prevent two or more threads from conucrrently accessing memory
// before any action acquire this lock and after you're done, just release this lock
pthread_mutex_t global_malloc_lock;

