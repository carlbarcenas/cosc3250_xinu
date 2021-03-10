/**
 * @file resched.c
 * @provides resched
 *
 * COSC 3250 / COEN 4820 Assignment 4
 *
 *
 * TA-BOT:MAILTO carlanthony.barcenas@marquette.edu anthony.nicholas@marquette.edu
 *
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

extern void ctxsw(void *, void *);
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate 
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */
syscall resched(void)
{
    int highest_prio;
    irqmask im;
    pcb *oldproc;               /* pointer to old process entry */
    pcb *newproc;               /* pointer to new process entry */

    pid_typ temp; // DELETEME?

    uint cpuid = getcpuid();

    oldproc = &proctab[currpid[cpuid]];

    im = disable();

#if AGING

    // TODO: Implement aging.
    //
    //       Reference include/clock.h to find more information
    //       about the quantums and how aging should behave.
	
	promote_medium[cpuid]--; // Decrement promote_medium

	if(promote_medium[cpuid] == 0)	{
		promote_medium[cpuid] = QUANTUM; // Reset quanta value
		promote_low[cpuid]--; // Decrement promote_low
		
		// Dequeue priority_med, enqueue to priority_high
		temp = dequeue(readylist[cpuid][PRIORITY_MED]);

		if(temp != EMPTY)	{
			enqueue(temp, readylist[cpuid][PRIORITY_HIGH]);
		}
	}
	
	if(promote_low[cpuid] == 0)	{
		promote_low[cpuid] = QUANTUM; // Reset quanta value
		
		// Check for empty
		temp = dequeue(readylist[cpuid][PRIORITY_LOW]);

		if(temp != EMPTY)	{
			enqueue(temp, readylist[cpuid][PRIORITY_MED]);
		}
	}

#endif

    /* place current process at end of ready queue */
    if (PRCURR == oldproc->state)
    {
        oldproc->state = PRREADY;
        enqueue(currpid[cpuid], readylist[cpuid][oldproc->priority]);
    }

    /* remove first process in highest priority ready queue */
    // determine queue to pick from
    if (nonempty(readylist[cpuid][PRIORITY_HIGH]))
    {
        highest_prio = PRIORITY_HIGH;
    }
    else if (nonempty(readylist[cpuid][PRIORITY_MED]))
    {
        highest_prio = PRIORITY_MED;
    }
    else
    {
        highest_prio = PRIORITY_LOW;
    }

    currpid[cpuid] = dequeue(readylist[cpuid][highest_prio]);
    newproc = &proctab[currpid[cpuid]];
    newproc->state = PRCURR;    /* mark it currently running    */

#if PREEMPT
    preempt[cpuid] = QUANTUM;
#endif

    ctxsw(&oldproc->regs, &newproc->regs);

    /* The OLD process returns here when resumed. */
    restore(im);
    return OK;
}
