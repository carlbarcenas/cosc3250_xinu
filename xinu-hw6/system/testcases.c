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
void ostest_printQueue(qid_typ q)	{

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

void longprocess()	{
	int i = 0;
	kprintf("Looping...\r\n");

	for(i = 0; i < 60000; i++)	{

	}

}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    uchar c;

    kprintf("===TEST BEGIN===\r\n");
    kprintf("0) Test priority scheduling\r\n");
    kprintf("\'A\') Aging / Starvation testcase\r\n");
    kprintf("\'P\') Preemption testcase\r\n");

    // TODO: Test your operating system!

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
	ostest_printQueue(readylist[0][PRIORITY_LOW]);

	kprintf("Adding 10 PRIORITY_LOW to queue\r\n");

	proctab[10].priority = PRIORITY_LOW;
	ready(10, 0, 0);

	ostest_printQueue(readylist[0][PRIORITY_LOW]);

	kill(10);
	break;

    case 'a':
    case 'A':
#if AGING
        // AGING TESTCASE
        kprintf("AGING is enabled.\r\n");

        // TODO: Create a testcase that demonstrates aging 
	kprintf("eat my cheeks you darn hippie");

#else
        // STARVING TESTCASE
        kprintf("\r\nAGING is not currently enabled.\r\n");

        // TODO: Create a testcase that demonstrates starvation


#endif
        break;

    case 'p':
    case 'P':
#if PREEMPT
        // PREEMPTION TESTCASE
        kprintf("\r\nPreemption is enabled.\r\n");

        // TODO: Create a testcase that demonstrates preemption


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
