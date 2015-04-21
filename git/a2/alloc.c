/***********************************************************************************\
|                      Assignment 2: Malloc the World                               |
| Name: Ryan DePrekel                                                               |
| Class: Dr. Trenary's CS2240                                                       |
| Date:  2/22/15 (tested and works with testers)                                    |
|                                                                                   |
|            TODO: Align Pointers                                                   |
|                  Split Blocks                                                     |
|                                                                                   |
|                                                                                   |
|                                                                                   |
\***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //Includes sbrk()

typedef struct mem_block
{
	size_t size;
	struct mem_block *next;
	int    is_free;
}mem_block;  

#define MEM_SIZE sizeof(mem_block)
#define align4(x) (((((x)-1) >> 2)<<2)+4)

void free (void* start_byte);
void* malloc(size_t size);
void* calloc(size_t nelem, size_t elsize);
void* realloc(void* ptr, size_t size);
mem_block *request_space(mem_block *last, size_t size);
mem_block *find_free_block(mem_block **last, size_t size);
mem_block *get_block(void* ptr);

int init_flag = 0;
/* start of the linked list of memory*/
void* mem_start = NULL;
void* last_valid_address;

/* what to do first if no memory is allocated */
/* not sure if needed using two different tutorials*/

/*
void init_malloc()
{
	//grabs last address from OS 

	last_valid_address = sbrk(0);

	//nothing to manage yet, set start to last valid.
	mem_start = last_valid_address;
	init_flag = 1;
}
*/

//Returns the address of block
mem_block *get_block(void* ptr)
{
	return (mem_block*)ptr - 1;
}

mem_block *find_free_block(mem_block **last, size_t size)
{
	mem_block *current = mem_start;
	while(current && !(current->is_free && current->size >= size))
	{
		*last = current;
		current = current->next;
	}
	return current;
}

mem_block *request_space(mem_block *last, size_t size)
{
	mem_block *new_block;
	void *request;
	new_block = sbrk(0);
	request = sbrk(size + MEM_SIZE);
	//assert((void*)block == request); //Not thread safe.
	if(request == (void*) -1)
	{
		return NULL; //failed sbrk.
	}

	if(last) // NULL on first request.
	{
		last->next = new_block;
	}

	new_block->size = size;
	new_block->next = NULL;
	new_block->is_free = 0;

	return new_block;
}


void* malloc(size_t size)
{
	mem_block *block;

	//TODO: align size? (pointers??)
	
	//Requested a size of memory that isn't valid
	if(size <= 0)
	{
		return NULL;
	}

	if(!mem_start) //First time used
	{
		block = request_space(NULL, size);
		if(!block)
		{
			return NULL;
		}
		mem_start = block;
	}

	else
	{
		mem_block *last = mem_start;
		block = find_free_block(&last, size);
		if(!block) 
		{
			block = request_space(last, size);
			if(!block)
			{
				return NULL;
			}
		}
		else
		{
			// TOOD: consider splitting block here.
			block->is_free = 0;
		}
	}

	return (block+1); //Returns a pointer to the region after block
}

/* frees a allocated block of memory */
void free (void* start_byte)
{
	if(!start_byte)
	{
		return;
	}

	//TODO: consisder merging blocks once splitting blocks is implemented.
	mem_block* block_ptr = get_block(start_byte);
	//assert(block_ptr->is_free == 0);
	block_ptr->is_free = 1;
}

void* calloc(size_t nelem, size_t elsize)
{
	size_t size = nelem * elsize;  //TODO: check for overflow.
	void *ptr = malloc(size);
	memset(ptr, 0, size);
	return ptr;
}

void* realloc(void* ptr, size_t size)
{
	mem_block* block_ptr;
	if(!ptr)
	{
		//Pointer is NULL. Should act like Malloc
		return malloc(size);
	}

	block_ptr = get_block(ptr);
	if(block_ptr->size >= size)
	{
		/* there is enough space for block, could free some once we implemetn splitting*/
		return ptr;
	}

	/* Time to realloc for real. Malloc new, free old, then copy old to new.*/
	void *new_ptr;
	new_ptr = malloc(size);
	if(!new_ptr){
		return NULL; //TODO: set errno on failure.
	}

	memcpy(new_ptr, ptr, block_ptr->size);
	free(ptr);
	return new_ptr;
}



