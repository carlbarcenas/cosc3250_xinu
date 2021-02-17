/**
 * @file testcases.c
 * @provides testcases
 *
 * $Id: testcases.c 175 2008-01-30 01:18:27Z brylow $
 *
 * Modified by: CARL BARCENAS
 *
 * and ANTHONY NICHOLAS
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

devcall putc(int dev, char c)
{
    return 0;
}


/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;
    int i;
    char teststring[10];

    kprintf("===TEST BEGIN===\r\n");

    c = kgetc();
    switch (c)
    {

        // Test your operating system!
	case 'a': 
		// kgetc() test: Will print the statement if kgetc() can read the character 'a'
		kprintf("Please help me I am a man trapped in a Raspberry Pi 3.\r\n");
		break;

	case 'b':
		// kputc(uchar c) test: Puts the characters of "HELLO" into dr and prints out "HELLO"
		kputc('H');
		kputc('E');
		kputc('L');
		kputc('L');
		kputc('O');
		break;

	case 'c': 
		// kcheckc() test: Will print out statement if unget buffer IS EMPTY and data register NOT EMPTY
		// By default, unget buffer is empty
		kputc('a');
		if(kcheckc() == 1)
		{
			kprintf("kcheckc() works for an EMPTY unget buffer and a NOT EMPTY dataregister\r\n");
		}
		else
		{
			kprintf("fix ya cut g\r\n");
		}
		break;

	case 'd': // kungetc() test: Push a small string into unget array and print.
		for(i = 0; i < 5; i++)
		{
			kungetc('A' + i);
			teststring[i] = kgetc();
		}
		kprintf("%s\r\n", teststring);
		break;

	case 'e': // kcheckc() test: Will print out statement if unget buffer NOT EMPTY and data register IS EMPTY
		kungetc('A');
		kputc('\0');
		if(kcheckc() == 1) {
			kprintf("kcheckc() works for a NOT EMPTY unget buffer and an EMPTY dataregister\r\n");
		}
		else
		{
			kprintf("fix ya cut g\r\n");
		}
		break;

	case 'f': // kcheckc() test: Testing if kcheckc() will return 0 for EMPTY dataregister and EMPTY unget buffer
		kputc('\0');
		if(kcheckc()==0)
		{
			kprintf("kcheckc() works for empty cases");
		}
		break;		

    default:
        kprintf("Hello Xinu World!\r\n");
    }


    kprintf("\r\n===TEST END===\r\n");
    return;
}
