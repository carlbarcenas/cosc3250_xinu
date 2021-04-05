/**
 * @file freemem.c
 *
 *
 * TA-BOT:MAILTO carlanthony.barcenas@marquette.edu anthony.nicholas@marquette.edu
 * Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Frees a block of heap-allocated memory.
 *
 * @param memptr
 *      Pointer to memory block allocated with memget().
 *
 * @param nbytes
 *      Length of memory block, in bytes.  (Same value passed to memget().)
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail
 *      because of memory corruption or specifying an invalid memory block.
 */
syscall freemem(void *memptr, ulong nbytes)
{
    register struct memblock *block, *next, *prev;
    irqmask im;
    ulong top;

    /* make sure block is in heap */
    if ((0 == nbytes)
        || ((ulong)memptr < (ulong)memheap)
        || ((ulong)memptr > (ulong)platform.maxaddr))
    {
        return SYSERR;
    }

    block = (struct memblock *)memptr;
    nbytes = (ulong)roundmb(nbytes);

    im = disable();

	/* TODO:
     *      - Determine correct freelist to return to
     *        based on block address
     *      - Acquire memory lock (memlock)
     *      - Find where the memory block should
     *        go back onto the freelist (based on address)
     *      - Find top of previous memblock
     *      - Make sure block is not overlapping on prev or next blocks
     *      - Coalesce with previous block if adjacent
     *      - Coalesce with next block if adjacent
     */
// -=-=-{ DETERMINE CORRECT FREELIST }-=-=-
	int i;
        int cpuid = -1;
        ulong startaddr, endaddr;

        for(i = 0; i <= 3; i++) { // Loop through 4 core values
                startaddr = freelist[i].base;   // Get start address of freelist
                endaddr = freelist[i].base + freelist[i].bound;  // Get end address of freelist
		if( (ulong)block >= startaddr  &&  (ulong)block <= endaddr )      {
                // Check if block addr within freelist address range
                        cpuid = i;      // Set cpuid
                        break;          // Escape loop
                }
        }
        if( cpuid == -1 )       {       // Error checking
                restore(im);
                return SYSERR;
        }	


// -=-=-{ ACQUIRE MEMORY LOCK }-=-=-
	lock_acquire(freelist[cpuid].memlock);

// -=-=-{ FIND WHERE MEMBLOCK GOES IN FREELIST }-=-=-
// 	// Initialize prev, next
	prev = (memblk *)&freelist[cpuid];
	next = freelist[cpuid].head;
	while( block > next )	{		// Stop loop when in between prev and next
							//stops when block is behind next
		prev = next;
		next = next->next;
	}

// -=-=-{ FIND TOP OF PREVIOUS MEMBLOCK }-=-=-
	if( (ulong)prev == (ulong)&freelist[cpuid] )	{ // If desired block is behind head of freelist
		top = NULL;
	}
	else	{
		top = (ulong)prev + (prev->length);
	}

// -=-=-{ CHECK OVERLAP }-=-=-
	// Check overlap with previous block
	if( (ulong)block < top )	{ 
		lock_release(freelist[cpuid].memlock);
		restore(im);
		return SYSERR;
	}
	// Check overlap with next block
	else if( ( next != NULL ) && (((ulong)block + block->length) > (ulong)next) )	{ 
		lock_release(freelist[cpuid].memlock);
		restore(im);
		return SYSERR;
	}
	
	// Place block into freelist if no overlap
	/*if(top == NULL)	{	// DELETEME?
		freelist[cpuid].head = block;
	}*/
	prev->next = block;
	block->next = next;
	block->length = nbytes;
	

// -=-=-{ COALESCE }-=-=-
	// Calculate the top addresses of prev and block
	ulong prevTop = (ulong)prev + (prev->length);
	ulong blockTop = (ulong)block + (block->length);

	if((ulong)block == prevTop) { // Coalesce with prev block
		prev->length += block->length;
		prev->next = next;
		block = prev;
	}
	
	if(blockTop  == (ulong)next)	{ // Coalesce with next block
		block->next = next->next;
		block->length += next->length;
	}
	

// Return
	freelist[cpuid].length += nbytes; // Resize freelist
	lock_release(freelist[cpuid].memlock);
	restore(im);
	return OK;
}
