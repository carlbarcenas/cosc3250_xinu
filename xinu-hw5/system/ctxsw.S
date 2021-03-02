/**
 * @file     ctxsw.s
 * @provides ctxsw
 *
 * COSC 3250 / COEN 4820 Assignment 4
 */
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

	// TODO: Save callee-save ("non-volatile") registers.
	str	R0, [R0, #PREG_R0*4]
	str 	R1, [R0, #PREG_R1*4]
	str 	R2, [R0, #PREG_R2*4]
	str 	R3, [R0, #PREG_R3*4]
	str 	R4, [R0, #PREG_R4*4]
	str 	R5, [R0, #PREG_R5*4]
	str 	R6, [R0, #PREG_R6*4]
	str 	R7, [R0, #PREG_R7*4]
	str 	R8, [R0, #PREG_R8*4]
	str 	R9, [R0, #PREG_R9*4]
	str 	R10, [R0, #PREG_R10*4]
	str 	R11, [R0, #PREG_R11*4]
	str 	R12, [R0, #PREG_IP*4]
	str 	R13, [R0, #PREG_SP*4]
	str 	R14, [R0, #PREG_LR*4]
	str 	R15, [R0, #PREG_PC*4]
	mov 	R12,R0 //moves R0 into IP, scratch register

	//NOTE: If only storing non-vol regs, why are we also storing R0 to R3?
	//I think callee-save is only R4 to R11
	// STORE R4 to R11 to [R0]? Each register 4 bytes

	// TODO: Restore callee-save ("non-volatile") registers.
	ldr 	R0, [R1,#PREG_R0*4]
	ldr 	R1, [R1,#PREG_R1*4]
	ldr 	R2, [R1,#PREG_R2*4]
	ldr 	R3, [R1,#PREG_R3*4]
	ldr 	R4, [R1,#PREG_R4*4]
	ldr 	R5, [R1,#PREG_R5*4]
	ldr 	R6, [R1,#PREG_R6*4]
	ldr 	R7, [R1,#PREG_R7*4]
	ldr 	R8, [R1,#PREG_R8*4]
	ldr 	R9, [R1,#PREG_R9*4]
	ldr 	R10, [R1,#PREG_R10*4]
	ldr 	R11, [R1,#PREG_R11*4]
	ldr 	R13, [R1,#PREG_IP*4]
	ldr 	R14, [R1,#PREG_LR*4]
	ldr 	R15, [R1,#PREG_PC*4]

	// TODO: Jump to next function.

	.end ctxsw
