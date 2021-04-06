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

void receiveMsg(void)	{
	message recvMesg = recv();
	kprintf("Message: %d\r\n", recvMesg);
}



/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    uchar c;
    register pcb *ppcb;
    int result;
    pid_typ testpid;

    kprintf("===TEST BEGIN===\r\n");
    kprintf("1) \r\n");
    kprintf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n\n\n\n");



    c = kgetc();
    switch (c)
    {
    case '0':
	testpid = create((void*)receiveMsg, INITSTK, PRIORITY_LOW, "RECV", 0);
	ready(testpid, RESCHED_YES, getcpuid());
	ppcb = &proctab[testpid];
	ppcb->core_affinity = 0;
	result = sendnow(testpid, 0x5);
	kprintf("Result: %d\r\n", result);
	kill(testpid);
        break;


    default:
        break;
    }

    kprintf("\r\n===TEST END===\r\n");
    return;
}
