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
	// { DETERMINE CORRECT FREELIST }
	int i;
	int cpuid = -1;
	ulong startaddr, endaddr;

	for(i = 0; i <= 4; i++)	{ // Loop through 4 core values
		startaddr = freelist[i].base;	// Get start address of freelist
		endaddr = freelist[i].base + freelist[i].bound;  // Get end address of freelist
		if( block > startaddr  &&  block < endaddr )	{ // Check if block addr within freelist address range
			cpuid = i;	// Set cpuid
			break;		// Escape loop
		}
	}
	if( cpuid == -1 )	{	// Error checking
		return SYSERR;
	}

	// { ACQUIRE MEMORY LOCK }
	lock_acquire(freelist[cpuid].memlock);

	// { FIND WHERE MEMBLOCK GOES IN FREELIST }
	if( block > freelist[cpuid].base && block < freelist[cpuid].head )	{ // First, check if behind head memblk
		prev = NULL;
		next = freelist[cpuid].head;
	}
	else	{	// If no space behind head memblk, loop through freelist
		prev = freelist[cpuid].head;
		next = prev->next;
		while(1) {
			if( block > prev && block < next )	{	// Found where block goes
				break;					// break out of loop
			}
			else	{ // Continue loop
				prev = next;
				next = next->next;
			}
		}
	}


	restore(im);
    return OK;
}
