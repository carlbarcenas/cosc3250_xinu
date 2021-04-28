/* sbFreeBlock.c - sbFreeBlock */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*                                                                 */
/* and                                                             */
/*                                                                 */
/*                                                                 */

#include <kernel.h>
#include <device.h>
#include <memory.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 * sbFreeBlock - Add a block back into the free list of disk blocks.
 *------------------------------------------------------------------------
 */
devcall sbFreeBlock(struct superblock *psuper, int block)
{
    // TODO: Add the block back into the filesystem's list of
    //  free blocks.  Use the superblock's locks to guarantee
    //  mutually exclusive access to the free list, and write
    //  the changed free list segment(s) back to disk.
    //
    //  TODO 2: (See sbGetBlock)
    //  Error Checking
    //  Calculate fd
    //  Set up location of where to put block back in free list
    //  Grab Lock
    //  Put block back into the file system's list of free blocks (deal with every case)
    //  Release Lock
    //
	struct freeblock *freeblk, *free2;
	struct dirblock *swizzle;
	struct dentry *phw;
	int result, i;
	int diskfd;

	// Error Checking
	if (NULL == psuper)	{
		return SYSERR;
	}
	if (NULL == block)	{
		return SYSERR;
	}
	phw = psuper->sb_disk;
	if (NULL == phw)	{
		return SYSERR;
	}

	// Calculate fd
	diskfd = phw - devtab;

	// Set up location of where to put block back in free list
	freeblk = psuper->sb_freelst;

	// Grab Lock
	wait(psuper->sb_dirlock);

	// Put block back into the file system's list of free blocks
	// 3 Cases: No header freeblk, full block, and regular freelist addition
	// CASE: No header freeblk
	if(freeblk == NULL)	{
		// Allocate and initialize a new block to use as a header
		freeblk = psuper->sb_freelst = malloc(sizeof(struct freeblock));
		freeblk->fr_blocknum = block;
		freeblk->fr_count = 0;
		freeblk->fr_next = NULL;

		// Swish swizzly
		swizzle = psuper->sb_dirlst;
		psuper->sb_dirlst = (struct dirblock *)swizzle->db_blocknum;

		// Write
		seek(diskfd, psuper->sb_blocknum);
		if (SYSERR == write(diskfd, freeblk, sizeof(struct superblock)))	{
			signal(psuper->sb_dirlock);
			return SYSERR;
		}

		// Unswish swizzlin
		psuper->sb_dirlst = swizzle;
		signal(psuper->sb_freelock);
		return OK;
	}

	// Go to end of freelist
	while(freeblk->fr_next != NULL)	{		
		freeblk = freeblk->fr_next;
	}

	// CASE: full block
	if(freeblk->fr_count >= FREEBLOCKMAX)	{
		// Allocate and initialize block
		free2 = psuper->sb_freelst = malloc(sizeof(struct freeblock));
		free2->fr_blocknum = block;
                free2->fr_count = 0;
                free2->fr_next = NULL;

		// Add to end of freelist, write
		freeblk->fr_next = free2;
		if(SYSERR == write(diskfd, free2, sizeof(struct superblock)))	{
			signal(psuper->sb_dirlock);
			return SYSERR;
		}
		
		signal(psuper->sb_dirlock);
		return OK;
	}

	// CASE: not full block
	if(freeblk->fr_count < FREEBLOCKMAX)	{
		freeblk = freeblk->fr_free[block];
		freeblk->fr_count++;

		// Write
		seek(diskfd, freeblk->fr_blocknum);
		if(SYSERR == write(diskfd, freeblk, sizeof(struct superblock)))	{
			signal(psuper->sb_dirlock);
			return SYSERR;
		}

		signal(psuper->sb_dirlock);
		return OK;
	}


	// Release Lock
	signal(psuper->sb_dirlock);
    	return SYSERR;
}
