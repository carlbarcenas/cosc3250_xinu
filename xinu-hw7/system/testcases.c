/**
 * @file testcases.c
 * @provides testcases
 *
 *
 * Modified by:	
 *
 * TA-BOT:MAILTO 
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

// Print the queue of the process
void prQueue(qid_typ q)	{

	short head, tail, next, extent;
	kprintf("Queue (0x%08X) = \r\n", q);

	// Assign head, tail, etc
	head = queuehead(q);
	tail = queuetail(q);
	next = head;

	extent = NQENT;

	while((next < NQENT) && (next != tail) && extent)	{
		kprintf("    [%3d|%3d:%3d]\r\n", next, queuetab[next].prev, queuetab[next].next);

 		next = queuetab[next].next;
		extent--;
	}
	if(!extent)	{
		kprintf("	...Loop detected in the queue\r\n");
	}
	else	{
		kprintf("    [%3d|%3d:%3d]\r\n", tail, queuetab[tail].prev, queuetab[tail].next);
	}
}


void printpid(int times)
{
    int i = 0;
    uint cpuid = getcpuid();

    enable();
    for (i = 0; i < times; i++)
    {
        kprintf("This is process %d\r\n", currpid[cpuid]);
        udelay(1);
    }
}

void loopylarry(void)
{	
	uint cpuid = getcpuid();

	enable();
	while(1)	{
		kprintf("This is process %d\r\n", currpid[cpuid]);
		udelay(1);
	}
}

void printFreelist(int core)	{
	memblk *curr;
	curr = freelist[core].head;

	kprintf("-=-=-=-=-START PRINT-=-=-=-=-\r\n");
	while(curr != NULL)	{
		kprintf("\r\nAddr: %d \r\n", curr);
		kprintf("Length: %d \r\n", curr->length);
		kprintf("End Addr: %d \r\n", ((ulong)(curr) + (curr->length)));
		kprintf("-----------------\r\n");
		curr = curr->next;
	}
	kprintf("-=-=-=-=-END PRINT-=-=-=-=-\r\n");
	
}



/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    uchar c;

    kprintf("===TEST BEGIN===\r\n");
    kprintf("0) Test priority scheduling\r\n");
    kprintf("1) Priorities in Isolation \r\n");
    kprintf("2) Getmem Testing\r\n");
    kprintf("3) Freemem Testing\r\n");
    kprintf("4) Malloc Testing \r\n");
    kprintf("5) Free Testing \r\n");
    kprintf("6) freelist[cpuid].base vs &freelist[cpuid]\r\n");
    kprintf("7) Coalescing Testing \r\n");
    kprintf("\'A\') Aging / Starvation testcase\r\n");
    kprintf("\'P\') Preemption testcase\r\n");

    kprintf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n\n\n\n");

    // TODO: Test your operating system!
	ulong *blk;
	ulong *blk2;
	ulong *blk3;


    c = kgetc();
    switch (c)
    {
    case '0':
        // Run 3 processes with varying priorities
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-A", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_MED, "PRINTER-B", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_LOW, "PRINTER-C", 1,
               5), RESCHED_YES, 0);
        break;


    case '1':
	// Testing priorities in isolation
	prQueue(readylist[0][PRIORITY_LOW]);

	kprintf("Adding 10 PRIORITY_LOW to queue\r\n");

	proctab[10].priority = PRIORITY_LOW;
	ready(10, 0, 0);

	prQueue(readylist[0][PRIORITY_LOW]);

	kill(10);
	break;

    case '2':
	// TODO: getmem testing
	printFreelist(getcpuid());

	getmem((ulong)64);
	kprintf("---------Getting 64 bytes-------\r\n");
	
	printFreelist(getcpuid());

	getmem((ulong)256);
	kprintf("---------Getting 256 bytes-------\r\n");

	printFreelist(getcpuid());
	break;

    case '3':
	printFreelist(getcpuid());

	blk = getmem(64);
	kprintf("\n\n----------Getting 64 bytes--------\r\n\n");

	printFreelist(getcpuid());

	freemem(blk, 64);
	kprintf("\n\n----------Freeing 64 bytes--------\r\n\n");

	printFreelist(getcpuid());
	break;

    case '4':
	printFreelist(0);
	malloc(1024);
	kprintf("\n\n---------MALLOC(1024)-----------\r\n\n");
	printFreelist(0);
	break;

    case '5':
	printFreelist(0);
	blk = malloc(1024);
	kprintf("\n\n---------MALLOC(1024)----------\r\n\n");
	printFreelist(0);
	free((void*) blk);
	kprintf("\n\n----------FREE--------------\r\n\n");
	printFreelist(0);
	break;

    case '6':
	kprintf(".base: %d \r\n&: %d \r\n", (ulong)freelist[getcpuid()].base, (ulong)&freelist[getcpuid()]);
	break;

    case '7':
	kprintf("Initial Case: \r\n");
	printFreelist(0);

	kprintf("context: 3 blocks attained from getmem, blk=1024, blk2=2048, blk3=512\r\n");
	blk = getmem(1024);
	blk2 = getmem(2048);
	blk3 = getmem(512);
	printFreelist(0);

	kprintf("\nFREEING BLK2\r\n");
	freemem(blk2, 2048);
	printFreelist(0);

	kprintf("\nFREEING BLK3, should coalesce\r\n");
	freemem(blk3, 512);
	printFreelist(0);

	kprintf("\nFREEING BLK, freelist should not have 2 entities\r\n");
	freemem(blk, 1024);
	printFreelist(0);
	
	break;

    case 'a':
    case 'A':
#if AGING
        // AGING TESTCASE
        kprintf("AGING is enabled.\r\n");
	kprintf("At some point, process 6/7 should print above process 4/5.\r\n");

        // TODO: Create a testcase that demonstrates aging 
	ready(create
              ((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-A", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-B", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_MED, "PRINTER-C", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_MED, "PRINTER-D", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_LOW, "PRINTER-E", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_LOW, "PRINTER-F", 1,
               5), RESCHED_YES, 0);



#else
        // STARVING TESTCASE
        kprintf("\r\nAGING is not currently enabled.\r\n");
	kprintf("Processes should be in order of 4/5(high), 6/7(med), 8/9(low).\r\n");

        // TODO: Create a testcase that demonstrates starvation
	ready(create
              ((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-A", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_HIGH, "PRINTER-B", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_MED, "PRINTER-C", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_MED, "PRINTER-D", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_LOW, "PRINTER-E", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)printpid, INITSTK, PRIORITY_LOW, "PRINTER-F", 1,
               5), RESCHED_YES, 0);


#endif
        break;

    case 'p':
    case 'P':
#if PREEMPT
        // PREEMPTION TESTCASE
        kprintf("\r\nPreemption is enabled.\r\n");

        // TODO: Create a testcase that demonstrates preemption
	ready(create
              ((void *)loopylarry, INITSTK, PRIORITY_LOW, "PRINTER-A", 1,
             	5), RESCHED_NO, 0);
        ready(create
              ((void *)loopylarry, INITSTK, PRIORITY_MED, "PRINTER-B", 1,
               5), RESCHED_NO, 0);
        ready(create
              ((void *)loopylarry, INITSTK, PRIORITY_HIGH, "PRINTER-C", 1,
               5), RESCHED_YES, 0);


#else
        kprintf("\r\nPreemption is not currently enabled...\r\n");
#endif
        break;

    default:
        break;
    }

    kprintf("\r\n===TEST END===\r\n");
    return;
}
