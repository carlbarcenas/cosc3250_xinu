/**
 * @file     ctxsw.s
 * @provides ctxsw
 *
 * COSC 3250 / COEN 4820 Assignment 4
 *
 * COSC 3250 - Project 5
 * Context Switch
 * @authors Carl Barcenas, Anthony Nicholas
 * Instructor Sabirat Rubiya
 * TA-BOT:MAILTO carlanthony.barcenas@marquette.edu anthony.nicholas@marquette.edu
 *
/* Embedded XINU, Copyright (C) 2013.  All rights reserved. */

#include <arm.h>

.text
	.align 4
	.globl	ctxsw


/**
 * @fn void ctxsw(&oldregs, &newregs)
 *
 * Switch context (values in registers) to another process, saving the
 * current processes information. This function will not return as normally
 * thought as it will load in the stack pointer for a different process and
 * jump to that location and begin executing code.
 *
 * @param  &oldstack address of outgoing stack save area
 * @param  &newstack address of incoming stack save area
 * @return special case -- see above
 */
ctxsw:
	.func ctxsw

//in collaboration with: Tyrell To, Simran Bhalla, Justin Ethithara

	// TODO: Save callee-save ("non-volatile") registers.
	str	R4, [R0, #PREG_R4*4]
	str	R5, [R0, #PREG_R5*4]
	str 	R6, [R0, #PREG_R6*4]
	str 	R7, [R0, #PREG_R7*4]
	str 	R8, [R0, #PREG_R8*4]
	str 	R9, [R0, #PREG_R9*4]
	str 	R10,[R0, #PREG_R10*4]
	str 	R11,[R0, #PREG_R11*4]
	str	SP, [R0, #PREG_SP*4]
	str	LR, [R0, #PREG_PC*4]

  //move R1 into IP (intra procedure scratch register)
	mov	IP, R1

	// TODO: Restore callee-save ("non-volatile") registers.
	ldr	R0, [IP, #PREG_R0*4]
	ldr	R1, [IP, #PREG_R1*4]
	ldr	R2, [IP, #PREG_R2*4]
	ldr 	R3, [IP, #PREG_R3*4]
	ldr 	R4, [IP, #PREG_R4*4]
	ldr 	R5, [IP, #PREG_R5*4]
	ldr 	R6, [IP, #PREG_R6*4]
	ldr 	R7, [IP, #PREG_R7*4]
	ldr 	R8, [IP, #PREG_R8*4]
	ldr 	R9, [IP, #PREG_R9*4]
	ldr 	R10,[IP, #PREG_R10*4]
	ldr 	R11,[IP, #PREG_R11*4]
	ldr 	SP, [IP, #PREG_SP*4]
	ldr 	LR, [IP, #PREG_LR*4]

	// TODO: Jump to next function.
	ldr	PC, [IP, #PREG_PC*4]

	.end ctxsw
