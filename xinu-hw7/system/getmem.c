/**
 * @file getmem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Allocate heap memory.
 *
 * @param nbytes
 *      Number of bytes requested.
 *
 * @return
 *      ::SYSERR if @p nbytes was 0 or there is no memory to satisfy the
 *      request; otherwise returns a pointer to the allocated memory region.
 *      The returned pointer is guaranteed to be 8-byte aligned.  Free the block
 *      with memfree() when done with it.
 */
void *getmem(ulong nbytes)
{
    register memblk *prev, *curr, *leftover;
    irqmask im;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (ulong)roundmb(nbytes);

    im = disable();

	/* TODO:
     *      - Use cpuid to use correct freelist
     *           ex: freelist[cpuid]
     *      - Acquire memory lock (memlock)
     *      - Traverse through the freelist
     *        to find a block that's suitable 
     *        (Use First Fit with simple compaction)
     *      - Release memory lock
     *      - return memory address if successful
     */
// -=-=-{ GET CPUID }-=-=-
	uint cpuid = getcpuid();
	curr = freelist[cpuid].head;
	prev = (memblk *)&freelist[cpuid]; //TODO


// -=-=-{ Acquire memory lock }-=-=-
	lock_acquire(freelist[cpuid].memlock);

// -=-=-{ Traverse through freelist }-=-=-
	while(curr != NULL)	{
		// If block is larger than nbytes, MOST LIKELY
		if(curr->length > nbytes)	{
			leftover = (memblk *)((ulong)curr + nbytes);	// Create new memblock
			leftover->next = curr->next;
			leftover->length = curr->length - nbytes;

			// Memblock element allocation
			prev->next = leftover;
			curr->length = nbytes;

			// Recalculate freelist length
			freelist[cpuid].length -= nbytes;

			lock_release(freelist[cpuid].memlock); // Release lock
			restore(im);	// DELETEME?
			return (void *)curr; // Return the address of the memblock			
		}
		// If block is exactly nbytes
		else if(curr->length == nbytes)	{
			prev->next = curr->next; // Remove block from freelist
			freelist[cpuid].length -= nbytes; // Resize freelist length
			
			lock_release(freelist[cpuid].memlock); // Release lock
			restore(im);	// DELETEME?
			return (void *)curr; // Return the address 
		}
		// If nbytes doesn't fit, continue traversal
		else	{
			prev = curr;
			curr = curr->next;
		}
	}
	
    // If the code gets this far, release lock
    lock_release(freelist[cpuid].memlock);
    restore(im);
    return (void *)SYSERR;
}
