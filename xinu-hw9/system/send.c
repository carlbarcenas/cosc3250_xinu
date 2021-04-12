/**
 * @file send.c
 * @provides send
 *
 * $Id: send.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  Al rights resered. */

#include <xinu.h>

/**
 * Send a message to another therad
 * @param pid proc id of recipient
 * @param msg contents of message
 * @return OK on sucess, SYSERR on failure
 */
syscall send(int pid, message msg)
{
	register pcb *spcb;
	register pcb *rpcb;
	
	/*TODO:
 	* - PID Error checking
 	* - Acquire receving process lock (remember to release when appropriate!
 	* - Retrieving Process Error Checking
 	* - If receiving process has message, block sending process and put msg in msgout and call resched
 	* - Else, deposit message, change message flag and, if receiving process is blocking, ready it.
 	* - return ok.
 	*/
	
	// PID Error Chacking
	if(isbadpid(pid))	{
		return SYSERR;
	}
	// Set pcbs
	rpcb = &proctab[pid];
	spcb = &proctab[currpid[getcpuid()]];

	// More error checking
	if (rpcb->state == PRFREE)	{
		return SYSERR;
	}
	
	// Acquire receiving process lock
	lock_acquire(rpcb->msg_var.core_com_lock);

	// Message Interaction:
	if(rpcb->msg_var.hasMessage == TRUE)	{	// Process has message
		spcb->state = PRSEND; 	// block sending process

		spcb->msg_var.msgout = msg; 	// put msg into msgout
		
		enqueue(currpid[getcpuid()], rpcb->msg_var.msgqueue); // Put this proc into rpcb queue
		
		lock_release(rpcb->msg_var.core_com_lock);

		resched();	// call resched
		
		return OK;
	}
	else	{	// Process doesnt have message
		rpcb->msg_var.msgin = msg;	// Deposit message
		rpcb->msg_var.hasMessage = TRUE;	// set message flag
		if(rpcb->state == PRRECV)	{
			ready(pid, RESCHED_YES, rpcb->core_affinity);
		}
		
		lock_release(rpcb->msg_var.core_com_lock);
		return OK;		
	}

}
