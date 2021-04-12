/**
 * @file recv.c
 * @provides recv.
 *
 * $Id: receive.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * recv - wait for a mesage and return it
 * @return message
 */

message recv(void)
{
	register pcb *ppcb;
	int senderpid = -1;
	message msg;
	ppcb = &proctab[currpid[getcpuid()]];
	
	/* TODO:
 	* - Remember to acquire lock and release lock when interacitng with the msg structure
 	* - Check for Message. If no message, put in blocking state and reschedule
 	*   			If there is message, retrive message
 	* - Now, check queue for wating send processes,
 	*   If stuff in queue  collect message for next time recv is called and ready send process
 	*   else, reset message flag
 	*   return collected message
 	*/	
	
	// Acquire lock
	lock_acquire(ppcb->msg_var.core_com_lock);

	// Check for message
	if(ppcb->msg_var.hasMessage == FALSE )	{
		ppcb->state = PRRECV; // put in blocking state
		resched();
	}

	msg = ppcb->msg_var.msgin; // retrieve message
	ppcb->msg_var.hasMessage = FALSE;	// Update flag
	
	// Check for more messages from queue
	senderpid = dequeue(ppcb->msg_var.msgqueue); // Get sender PID

	if(senderpid != EMPTY)	{
		pcb *msgProc;
		msgProc = &proctab[senderpid]; // Get sender pcb

		ppcb->msg_var.msgin = msgProc->msg_var.msgout; // Deposit message
		ppcb->msg_var.hasMessage = TRUE; // Set message flag
		msgProc->msg_var.msgout = NULL;
		
		ready(senderpid, RESCHED_NO, msgProc->core_affinity); // Ready send process
	}
	else	{
		ppcb->msg_var.hasMessage = FALSE; // Reset message flag
	}

	// Release lock
	lock_release(ppcb->msg_var.core_com_lock);

	return msg;
}
