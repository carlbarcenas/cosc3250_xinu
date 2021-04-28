/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*                                                                 */
/* and                                                             */
/*                                                                 */
/*                                                                 */
/*
 * TA-BOT:MAILTO carlanthony.barcenas@marquette.edu anthony.nicholas@marquette.edu
 * */

#include <kernel.h>
#include <memory.h>
#include <file.h>

/*------------------------------------------------------------------------
 * fileDelete - Delete a file.
 *------------------------------------------------------------------------
 */
devcall fileDelete(int fd)
{
    // TODO: Unlink this file from the master directory index,
    //  and return its space to the free disk block list.
    //  Use the superblock's locks to guarantee mutually exclusive
    //  access to the directory index.
    //
    // TODO 2: (See file create)
    // Error Checking
    // Grab lock
    // Use sbFreeBlock to free up block
    // Overwrite the block on disk
    // Release Lock
    

	// Error checking ??
	if  ((NULL == supertab) || (NULL == filetab))	{
		return SYSERR;
	}

	// Grab Lock
	wait(supertab->sb_dirlock);

	// Use sbFreeBlock to free the block  
	// Use filetab[fd] to get fn_blocknum??
	supertab->sb_dirlst->db_fnodes[fd].fn_state = FILE_FREE;
	sbFreeBlock(supertab, supertab->sb_dirlst->db_fnodes[fd].fn_blocknum);

	// Overwrite the block on disk
	filetab[fd].fn_state = FILE_FREE;
	seek(DISK0, supertab->sb_dirlst->db_blocknum);
	write(DISK0, supertab->sb_dirlst, sizeof(struct dirblock));


	// Release Lock
	signal(supertab->sb_dirlock);

    return OK;
}
