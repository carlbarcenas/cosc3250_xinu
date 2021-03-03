/**
 * @file create.c
 * @provides create, newpid, userret
 *
 * COSC 3250 / COEN 4820 Assignment 5
 * Initializes a new Process Control Block
 * @authors Carl Barcenas, Anthony Nicholas
 * NOTE: Collaborated with some groups for some parts
 * Instructor Sabirat Rubya
=======
 * COSC 3250 / COEN 4820 Assignment 4
 *
 * COSC 3250 - Project 4
 * description
 * @authors Carl Barcenas, Anthony Nicholas
 * Instructor Sabirat Rubiya
 * TA-BOT:MAILTO carlanthony.barcenas@marquette.edu anthony.nicholas@marquette.edu
 *
 * Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <arm.h>
#include <xinu.h>

/* Assembly routine for atomic operations */
extern int _atomic_increment_post(int *);
extern int _atomic_increment_limit(int *, int);

static pid_typ newpid(void);
void userret(void);
void *getstk(ulong);

/**
 * Create a new process to start running a function.
 * @param funcaddr address of function that will begin in new process
 * @param ssize    stack size in bytes
 * @param name     name of the process, used for debugging
 * @param nargs    number of arguments that follow
 * @return the new process id
 */
syscall create(void *funcaddr, ulong ssize, char *name, ulong nargs, ...)
{
	ulong *saddr;               /* stack address                */
	ulong pid;                  /* stores new process id        */
	pcb *ppcb;                  /* pointer to proc control blk  */
	ulong i;
	va_list ap;                 /* points to list of var args   */
	ulong pads = 0;             /* padding entries in record.   */
	void INITRET(void);

	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (ulong)(ssize + 3) & 0xFFFFFFFC;
	/* round up to even boundary    */
	saddr = (ulong *)getstk(ssize);     /* allocate new stack and pid   */
	pid = newpid();
	/* a little error checking      */
	if ((((ulong *)SYSERR) == saddr) || (SYSERR == pid))
	{
		return SYSERR;
	}

	_atomic_increment_post(&numproc);

	ppcb = &proctab[pid];
	/* setup PCB entry for new proc */
	ppcb->state = PRSUSP;

  	// TODO: Setup PCB entry for new process. note, state already done
	ppcb->stklen = ssize;
	// TODO: Setup PCB entry for new process. note, state already done
	ppcb->stklen = ssize; //set size of stklen
	ppcb->stkbase = (void *)saddr; //WAS STACK MAGIC, TRYING saddr
	ppcb->core_affinity = -1;
	strncpy(ppcb->name, name, strlen(name)); 

	/* Initialize stack with accounting block. */
	*saddr = STACKMAGIC;
	*--saddr = pid;
	*--saddr = ppcb->stklen;
	*--saddr = (ulong)ppcb->stkbase;

	/* Handle variable number of arguments passed to starting function   */
	if (nargs)
	{
		pads = ((nargs - 1) / 4) * 4;
	}
	/* If more than 4 args, pad record size to multiple of native memory */
	/*  transfer size.  Reserve space for extra args                     */
	for (i = 0; i < pads; i++)
	{
		*--saddr = 0;
	}

	// TODO: Initialize process context.
	for (i=0; i < PREGS; i++)
	{
		saddr--;
		if(i==0)	{
			*saddr = (ulong)funcaddr;
		}
		else if(i==1)	{
			*saddr = (ulong)&userret;
		}
		else	{
			*saddr = 0;
		}

	}
	ppcb->regs[PREG_LR] = (int)userret;
	ppcb->regs[PREG_PC] = (int)funcaddr;


	// TODO:  Place arguments into activation record.
	//        See K&R 7.3 for example using va_start, va_arg and
	//        va_end macros for variable argument functions.
	//        may need a loop? max of 4 args?
	//	  in collaboration with Justin Ethitara, Tyrell To, Simran Bhalla
	va_start(ap, nargs);
	if(nargs > 0 && nargs <= 4)	// When nargs < 4
	{
		for(i=0; i < nargs; i++) // Place args into regs[0] to regs[3]
		{
			*saddr = va_arg(ap,int);
			ppcb->regs[i] = *saddr;
			saddr++;
		}
		saddr = saddr - nargs; // Shift saddr back to bottom of stack
	}
	else if(nargs > 4)	// When nargs > 4
	{
		for(i=0; i < 4; i++)
		{
			*saddr = va_arg(ap, int);
			ppcb->regs[i] = (int) *saddr;	// for first 4 args, place into regs[0] to regs[3]
			saddr++;
		}
		saddr = saddr + 11; // shift saddr up to pads
		for(i=0; i<pads; i++)
		{
			saddr++;
			*saddr = va_arg(ap, int);
		}
		saddr = saddr - 3;	// shift saddr back down to Arg4
		
	}
	va_end(ap);


	ppcb->regs[PREG_SP] = (int)saddr;


	return pid;
}

/**
 * Obtain a new (free) process id.
 * @return a free process id, SYSERR if all ids are used
 */
static pid_typ newpid(void)
{
	pid_typ pid;                /* process id to return     */
	static pid_typ nextpid = 0;

	for (pid = 0; pid < NPROC; pid++)
	{                           /* check all NPROC slots    */
		//        nextpid = (nextpid + 1) % NPROC;
		_atomic_increment_limit(&nextpid, NPROC);
		if (PRFREE == proctab[nextpid].state)
		{
			return nextpid;
		}
	}
	return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
	uint cpuid = getcpuid();
	kill(currpid[cpuid]);
}
