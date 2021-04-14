/**
 * @file testcases.c
 * @provides testcases
 *
 *
 * Modified by:	
 *
 * TA-BOT:MAILTO carlanthony.barcenas@marquette.edu anthony.nicholas@marquette.edu
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

/* Function prototypes */
void print_sem(semaphore sem);
uchar getc(void);
void putc(uchar);


/* Test process to use for testing semaphores. */
process testSemWait(semaphore sem)
{
    wait(sem);
    printf("process %d after wait()\r\n", currpid[getcpuid()]);
    return 0;
}

process loopylarry(semaphore sem)
{
	for(int i = 0; i <= 10; i++)	{
		printf("%d\r\n", i);
		if(i == 5)	{
			wait(sem);
		}
	}
	return 0;
}

process interruptingClyde(semaphore sem)
{
	printf("Interrupting Clyds is interrupting this loop\r\n");
	printf("        _^_^_\r\n");
	printf("       (<> <>)\r\n");
	printf("       (  >  )\r\n");
	printf("        | o |\r\n");
	printf("        \\   /\r\n");
	printf("         --- \r\n");

	signal(sem);
	return 0;
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    uchar c;
    semaphore testsem;

    enable();

    printf(" a: getc() testing.\r\n");
    printf("b: putc() testing.\r\n");
    printf("c: printf() testing.\r\n");
    printf("d: basic semaphore signal(n)/wait testing\r\n");
    printf("e: Semaphore lock testing\r\n");
    printf("f: Semaphore Ordering Test\r\n");

    printf("===TEST BEGIN===\r\n");
    
	// TODO: Test your operating system!
    c = getc();
    switch (c)
    {

    case 'a':
	printf("Getc Works\r\n");
	break;

    case 'b':
	putc(c);
	printf("If 'b' shows, putc works");
	break;

    case 'c':
	printf("3.6... Not great, not terrible. We did everything right.\r\n");
	break;

    case 'd':
	testsem = semcreate(1);
	printf("Initial semaphore image: \r\n");
	print_sem(testsem);

	printf("Sending signal to semaphore...\r\n");
	signal(testsem);
	print_sem(testsem);

	printf("Telling semaphore to wait...\r\n");
	wait(testsem);
	print_sem(testsem);

	printf("Sending 5 signals to semaphore...\r\n");
	signaln(testsem, 5);
	print_sem(testsem);

	printf("Freeing semaphore...\r\n");
	semfree(testsem);
	print_sem(testsem);
	break;

    case 'e':
	testsem = semcreate(1); // Initialize semaphore with count = 1
	
	// wait(testsem); BECAUSE TESTSEMWAIT() ALREADY HAS WAIT(), DO NOT NEED THIS
	// Critical section
	ready(create((void*)testSemWait, INITSTK, PRIORITY_HIGH, "t0", 1, testsem),
		RESCHED_YES, 0);	// testSemWait on core 0.
	ready(create((void*)testSemWait, INITSTK, PRIORITY_HIGH, "t1", 1, testsem),
                RESCHED_YES, 1);        // testSemWait on core 1.
	printf("This should print before the 2nd 'process after wait' statement...\r\n");
	signal(testsem); // t0 finishes critical section
	semfree(testsem);	
	break;

    case 'f':
	testsem = semcreate(0);
	printf("Loop should be interrupted by Interrupting Clyde\r\n");
	
	ready(create((void*)loopylarry, INITSTK, PRIORITY_HIGH, "t0", 1, testsem),
                RESCHED_YES, 0);
	ready(create((void*)interruptingClyde, INITSTK, PRIORITY_HIGH, "t1", 1, testsem),
                RESCHED_YES, 1);	
	break;

    default:
        break;
    }

    while (numproc > 5)  // Four null procs plus this test proc.
    {
        resched();
    }

    printf("\r\n===TEST END===\r\n");
    printf("\r\n\r\nAll user processes have completed.\r\n\r\n");

    while (1);
    return;
}

void print_sem(semaphore sem)
{
    struct sement *semptr;
    semptr = &semtab[sem];

    printf("\r\n%d->state: %s\r\n", sem,
           (semptr->state == 0x01) ? "SFREE" : "SUSED");
    printf("%d->count: %d\r\n", sem, semptr->count);
    printf("%d->queue: %s\r\n\r\n", sem,
           (isempty(semptr->queue)) ? "EMPTY" : "NONEMPTY");
}
