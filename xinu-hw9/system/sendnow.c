/**
 * @file sendnow.c
 * @provides sendnow.
 *
 */
/* Embedded Xinu, Copyright (C) 2020.   All rights reserved. */

#include <xinu.h>

/**
 * Send a message to anoher thread
 * @param pid proc id of recipient
 * @param msg contents of message
 * @return OK on sucess, SYSERR on failure
 */

syscall sendnow(int pid, message msg)
{
	register pcb *ppcb;

	/* TODO:
 	* - Acquire Lock and release when appropriate
 	* - PID & Process Error Checking
 	* - Deposit Message, raise flag
 	* -  If receving message is blocked, ready process
 	* - Return OK
 	*/

	// PID error checking
	if(isbadpid(pid))	{
		return SYSERR;
	}
	ppcb = &proctab[pid];

	// Error Checking
	if(ppcb->msg_var.hasMessage == TRUE)	{
		return SYSERR;
	}


	lock_acquire(ppcb->msg_var.core_com_lock);	// Lock acquire
	ppcb->msg_var.msgin = msg;	// Deposit message
	ppcb->msg_var.hasMessage = TRUE;	// Raise flag
	

	if(ppcb->state == PRRECV)	{
		ready(pid, RESCHED_YES, ppcb->core_affinity);
	}

	lock_release(ppcb->msg_var.core_com_lock);
	return OK;
}
