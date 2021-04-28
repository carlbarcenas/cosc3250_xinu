/**
 * @file     xsh_test.c
 * @provides xsh_test
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */
// COLLAB WITH BRENDAN WILKE

#include <xinu.h>

/**
 * Shell command (test) is testing hook.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */

command xsh_test(int nargs, char *args[])
{
    //TODO: Test your O/S.
    printf("howwy\r\n");
    printf("This is where you should put some testing code.\n");
	int i, blk, tempfd;

	if(nargs == 2)	{
		i = atoi(args[1]);
	}

	switch(i)	{
	// Test Case 1: Basic block get and delete
	case 1:
		printf("INITIAL FREELIST IMAGE: \r\n");
		printFreeList();

		printf("\r\nAFTER GETTING BLOCK: \r\n");
		blk = sbGetBlock(supertab);
		printFreeList();

		printf("\r\nAFTER FREEING PREVIOUSLY ACQUIRED BLOCK: \r\n");
		sbFreeBlock(supertab, blk);
		printFreeList();
		break;

	// Test Case 2: No header block test case
	case 2:
		printf("WARNING: ONLY USE ON FRESH .dat DISKS\r\n");
		printf("INITIAL FREELIST IMAGE: \r\n");
		printFreeList();

		int blocks[254];

		printf("\r\nGETTING ALL BLOCKS: \r\n");
		for(i = 0; i <= 253; i++)	{
			blocks[i] = sbGetBlock(supertab);
		}
		printFreeList();

		printf("\r\nFREE BLOCK 255, SHOULD MAKE NEW HEADER: \r\n");
		sbFreeBlock(supertab, blocks[248]);
		printFreeList();

		printf("\r\nFREE REST OF BLOCKS: \r\n");
		for(i = 0; i <= 252; i++)	{
			sbFreeBlock(supertab, blocks[i]);
		}
		printFreeList();
	break;

	// Test Case 3: Create and Delete File
	case 3:
		printf("INITIAL FILE DIRECTORY IMAGE: \r");
		xsh_diskstat(0, NULL);

		printf("CREATE A FILE: \r\n");
		tempfd = fileCreate("bungus");
		xsh_diskstat(0, NULL);

		printf("DELETE FILE: \r\n");
		fileDelete(tempfd);
		xsh_diskstat(0, NULL);

		int mulfds[3];

		printf("CREATE MULTIPLE: \r");
		mulfds[0] = fileCreate("bingo");
		mulfds[1] = fileCreate("bango");
		mulfds[2] = fileCreate("bongo");
		xsh_diskstat(0, NULL);

		printf("DELETE BANGO BONGO \r");
		fileDelete(mulfds[1]);
		fileDelete(mulfds[2]);
		xsh_diskstat(0, NULL);

		printf("DELETE BINGO FOR YOUR BANGO BONGO: \r");
		fileDelete(mulfds[0]);
		xsh_diskstat(0, NULL);
	break;

	default:
		printf("Default \r\n");
	break;

	}

    return OK;
}

