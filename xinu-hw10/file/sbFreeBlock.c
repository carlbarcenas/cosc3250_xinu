#include <kernel.h>
#include <device.h>
#include <memory.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 * sbFreeBlock - Add a block back into the free list of disk blocks.
 *------------------------------------------------------------------------
 * COLLAB WITH Brendan Wilke
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

	struct freeblock *freeblk, *free2, *swizzle;
	struct dirblock *swizzle2;
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
	wait(psuper->sb_freelock);

	// Put block back into the file system's list of free blocks
	// case: no header block, make a new one
	if(NULL == freeblk)     {
                // Initialize new block
                freeblk = psuper->sb_freelst = malloc(sizeof(struct freeblock));
                freeblk->fr_blocknum = block;
                freeblk->fr_count = 0;
                freeblk->fr_next = NULL;

                // Swish swizzly write
                swizzle = psuper->sb_freelst;
                swizzle2 = psuper->sb_dirlst;
                psuper->sb_freelst = (struct freeblock *)swizzle->fr_blocknum;
                psuper->sb_dirlst = (struct dirblock *)swizzle2->db_blocknum;

                // Seek and write the swizzle
                seek(diskfd, psuper->sb_blocknum);
                if (SYSERR == write(diskfd, psuper, sizeof(struct superblock)))        {
			signal(psuper->sb_freelock);
                        return SYSERR;
                }

                // Unswish swizzlin
                psuper->sb_freelst = swizzle;
                psuper->sb_dirlst = swizzle2;

		// Write block
		seek(diskfd, freeblk->fr_blocknum);
		if(SYSERR == write(diskfd, freeblk, sizeof(struct superblock)))	{
			signal(psuper->sb_freelock);
			return SYSERR;
		}

                // Release and return
                signal(psuper->sb_freelock);
                return OK;
        }

	// Go to end of freelist
        while(freeblk->fr_next != NULL) {
                freeblk = freeblk->fr_next;
        }

        // case: full fr_free[]/full block
        if(freeblk->fr_count >= FREEBLOCKMAX)   {
                free2 = malloc(sizeof(struct freeblock));
                free2->fr_blocknum = block;
                free2->fr_count = 0;
		freeblk->fr_next = free2;

		// Blueberry swishers
		swizzle = psuper->sb_freelst;
		swizzle2 = psuper->sb_dirlst;
		psuper->sb_freelst = (struct freeblock *)swizzle->fr_blocknum;
		psuper->sb_dirlst = (struct dirblock *)swizzle2->db_blocknum;

		// Seek and write the swizzle
		seek(diskfd, psuper->sb_blocknum);
		if(SYSERR == write(diskfd, psuper, sizeof(struct superblock)))	{
			signal(psuper->sb_freelock);
			return SYSERR;
		}

		// Unswish swizzle
		psuper->sb_freelst = swizzle;
		psuper->sb_dirlst = swizzle2;

		// Write block
		seek(diskfd, freeblk->fr_blocknum);
		if(SYSERR == write(diskfd, freeblk, sizeof(struct superblock)))	{
			signal(psuper->sb_freelock);
			return SYSERR;
		}

		// Signal and return
		signal(psuper->sb_freelock);
		return OK;
        }

	// case: regular case
	if(freeblk->fr_count < FREEBLOCKMAX)	{
		freeblk->fr_free[freeblk->fr_count] = block;
		freeblk->fr_count++;

		// Write
		seek(diskfd, freeblk->fr_blocknum);
		if(SYSERR == write(diskfd, freeblk, sizeof(struct superblock)))	{
			signal(psuper->sb_freelock);
			return SYSERR;
		}
		
		// Signal and return
		signal(psuper->sb_freelock);
		return OK;
	}

	// Release Lock and return
	signal(psuper->sb_freelock);
    	return SYSERR;
}
