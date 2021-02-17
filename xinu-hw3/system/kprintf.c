/**
 * @file kprintf.c
 * COSC3250 - Project 3
 * Provides I/O related bit communication functions for a Synchronous Serial Driver for embedded OSystems.
 * @authors Carl Barcenas, Anthony Nicholas
 * In collaboration with: Mike Awadallah, Brendan Wilke
 * Instructor Sabirat Rubiya
 * TA-BOT:MAILTO carlanthony.barcenas@marquette.edu anthony.nicholas@marquette.edu
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>

#define UNGETMAX 10             /* Can un-get at most 10 characters. */

static unsigned char ungetArray[UNGETMAX];

/**
 * Synchronously read a character from a UART.  This blocks until a character is
 * available.  The interrupt handler is not used.
 *
 * @return
 *      The character read from the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */
syscall kgetc(void)
{
    volatile struct pl011_uart_csreg *regptr;
    /* Pointer to the UART control and status registers.  */
    regptr = (struct pl011_uart_csreg *)0x3F201000;

	unsigned char c;
	int i = 0;

    // 	     First, check the unget buffer for a character.
    //       Otherwise, check UART flags register, and
    //       once the receiver is not empty, get character c.
	if(ungetArray[0] != '\0')
	{
		c = ungetArray[0];
		for(i = 1; i < UNGETMAX; i++)
		{
			// Stops loop if ungetArray ends
			if(ungetArray[i] == '\0')
			{
				ungetArray[i-1] = '\0';
				break;
			}
			// Shift array left
			ungetArray[i-1] = ungetArray[i];
		}
		return (int)c;		
	}
	else
	{
		while(regptr->fr & PL011_FR_RXFE)
		{
			// Do nothing
		}
		
		c = regptr->dr;

		return (int)c;
	}
	
	
    return SYSERR;
}

/**
 * kcheckc - check to see if a character is available.
 * @return true if a character is available, false otherwise.
 */
syscall kcheckc(void)
{
    volatile struct pl011_uart_csreg *regptr;
    regptr = (struct pl011_uart_csreg *)0x3F201000;

    //  Check the unget buffer and the UART for characters.
	if(ungetArray[0] != '\0') //Check if unget buffer or UART has a character
	{
		return 1;
	}
	else if(regptr->fr & PL011_FR_RXFE) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
    return SYSERR;
}

/**
 * kungetc - put a serial character "back" into a local buffer.
 * @param c character to unget.
 * @return c on success, SYSERR on failure.
 */
syscall kungetc(unsigned char c)
{
    //  Check for room in unget buffer, put the character in or discard.
	int i = 0;
	for(i = 0; i < UNGETMAX; i++)
	{
		if(ungetArray[i] == '\0')
		{
			ungetArray[i] = c;
			return c;
		}
	}

    return SYSERR;
}


/**
 * Synchronously write a character to a UART.  This blocks until the character
 * has been written to the hardware.  The interrupt handler is not used.
 *
 * @param c
 *      The character to write.
 *
 * @return
 *      The character written to the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */
syscall kputc(uchar c)
{
    volatile struct pl011_uart_csreg *regptr;

    /* Pointer to the UART control and status registers.  */
    regptr = (struct pl011_uart_csreg *)0x3F201000;
    // Check UART flags register.
    // Once the Transmitter FIFO is not full, send character c.

	while((regptr->fr & PL011_FR_TXFE)!=PL011_FR_TXFE) // Loop through flag register until it is empty
	{
		// Do nothing
	}	
	
	// Sends character c
	regptr->dr = c;
	return (int)c;

    return SYSERR;
}

/**
 * kernel printf: formatted, synchronous output to SERIAL0.
 *
 * @param format
 *      The format string.  Not all standard format specifiers are supported by
 *      this implementation.  See _doprnt() for a description of supported
 *      conversion specifications.
 * @param ...
 *      Arguments matching those in the format string.
 *
 * @return
 *      The number of characters written.
 */
syscall kprintf(const char *format, ...)
{
    int retval;
    va_list ap;

    va_start(ap, format);
    retval = _doprnt(format, ap, (int (*)(int, int))kputc, 0);
    va_end(ap);
    return retval;
}
