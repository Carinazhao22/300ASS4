/*
Team Members:

Huiyi Zou
301355563

Hui Wu
301329435

Caijie Zhao
301354256

Yuanying Mu
301354135

*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */
struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;
  
  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in  .
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if(myMemory != NULL) 
		free(myMemory);     /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
    if(head!=NULL)
		free(head);

	myMemory = malloc(sz);

	/* TODO: Initialize memory management structure. */
	head = malloc(sizeof(struct memoryList));
	head->size=sz;
	head->last=head;
	head->next=head;
	head->alloc=0;
	head->ptr=myMemory;
	next=head;
	
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);
	struct memoryList *trav, *temp, *best_size;

	switch (myStrategy)
	{
	  	case NotSet: 
	            return NULL;
	  	case First:
	  	{
			trav=head;
			best_size=NULL;
			if(trav->alloc==0 && trav->size>=requested)
			{
				best_size=trav;
			}
			else
			{
				trav=trav->next;
				for(; trav!=head; trav=trav->next){
					if(trav->alloc==0 && trav->size>=requested)
					{
						best_size=trav;
				    	break;
					}
				}
			}
			break;
	  	}
			 

	  	case Best:
	    {
			trav=head;
			best_size=NULL;
			if(trav->alloc==0 && trav->size>=requested)
			{
				best_size=trav;
			}
			trav=trav->next;
			for(; trav!=head; trav=trav->next)
			{
				if(trav->alloc==0 && trav->size>=requested)
				{
					if(best_size==NULL)
					{
						best_size=trav;
					}
					if(best_size->size > trav->size)
					{
						best_size=trav;
					}
				}
			}
			break;
	  	}

	  	case Worst:
		{
			trav=head;
			best_size=NULL;
			if(trav->alloc==0 && trav->size>=requested)
			{
				best_size=trav;
			}
			trav=trav->next;
			for(; trav!=head; trav=trav->next)
			{
				if(trav->alloc==0 && trav->size>=requested)
				{
					if(best_size==NULL)
					{
						best_size=trav;
					}
					if(best_size->size < trav->size)
					{
						best_size=trav;
					}
				}
			}
			break;
	  	}

	  	case Next:
		{
			trav=next;
			best_size=NULL;
			if(trav->alloc==0 && trav->size>=requested)
			{
				best_size=trav;
			}
			else
			{
				trav=trav->next;
				for(; trav!=head; trav=trav->next)
				{
					if(trav->alloc==0 && trav->size>=requested)
					{
						best_size=trav;
				    	break;
					}
				}
			}
			break;
	  	}

	}
	  //update block
	if(best_size==NULL)
		return NULL; 

	best_size->alloc = 1;
	if(best_size->size > requested)
	{
		temp = malloc(sizeof(struct memoryList));
		temp->next = best_size->next;
		temp->next->last = temp;
		temp->size = best_size->size - requested;
		best_size->size = requested;
		best_size->next = temp;
		temp->last = best_size;
		temp->alloc = 0;
		temp->ptr = best_size->ptr + requested;
		next = temp;
	}
	else
	{
		next = best_size->next;
	}

	return best_size->ptr;	
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{ 
	struct memoryList *temp = head;
	do{
	   	if(temp->ptr==block)
	   	{
		   	temp->alloc=0;
		   	if(temp!=head)
		    {
				if(temp->last->alloc==0)
			  	{
					struct memoryList *fr = temp->last;
					temp->last->next = temp->next;
					temp->last->size += temp->size;
					temp->next->last = temp->last;
				  
					if(next == temp)
						next = temp->last;

					free(temp);
					temp = fr;
			  	}
			}
			if(temp->next!=head)
			{
				if(temp->next->alloc==0)
				{
					struct memoryList *fr = temp->next;
					temp->size += temp->next->size;
					temp->next = temp->next->next;
					temp->next->last=temp;

					if(next == fr)
						next = temp;

					free(fr);
			    }

			}
			break;
			
	    }
		else
		{
			temp=temp->next;
		}
	}while(temp!=head);

	return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when we refer to "memory" here, we mean the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	struct memoryList *trav = head;
	int count = 0;

	if(head->alloc==0)
		count++;

	trav = trav->next;
	for(; trav!=head; trav=trav->next)
	{
		if(trav->alloc==0)
			count++;
	}      
	return count;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	struct memoryList *trav = head;
	int count = 0;

	if(head->alloc==1)
		count+=head->size;

	trav = trav->next;
	for(; trav!=head; trav=trav->next)
	{
		if(trav->alloc==1)
			count+=trav->size;
	}      
	return count;
}

/* Number of non-allocated bytes */
int mem_free()
{
    struct memoryList *trav = head;
	int count = 0;

	if(head->alloc==0)
		count+=head->size;

	trav = trav->next;
	for(; trav!=head; trav=trav->next)
	{
		if(trav->alloc==0)
			count+=trav->size;
	}      
	return count;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	struct memoryList *trav = head;
	int count = 0;

	if(head->alloc==0)
		count=head->size;

	trav = trav->next;
	for(; trav!=head; trav=trav->next)
	{
		if(trav->alloc==0&&trav->size>count)
			count=trav->size;
	}      
	return count;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	struct memoryList *trav = head;
	int count = 0;

	if(head->alloc==0&&head->size<=size)
		count++;

	trav = trav->next;
	for(; trav!=head; trav=trav->next)
	{
		if(trav->alloc==0&&trav->size<=size)
			count++;
	}      
	return count;
}       

char mem_is_alloc(void *ptr)
{
    struct memoryList *trav = head;

	if(head->ptr==ptr)
		return head->alloc;

	trav = trav->next;
	for(; trav!=head; trav=trav->next) 
	{
		if(trav->ptr==ptr)
			return trav->alloc;
	}
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may ind them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
	return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,500);
	
	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);
	
	print_memory();
	print_memory_status();
	
}
