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
	str 	R0, [R0,4*PREG_R0]
	str 	R0, [R0,4*PREG_R1]
	str 	R0, [R1,4*PREG_R2]
	str 	R0, [R2,4*PREG_R3]
	str 	R0, [R3,4*PREG_R4]
	str 	R0, [R4,4*PREG_R5]
	str 	R0, [R5,4*PREG_R6]
	str 	R0, [R6,4*PREG_R7]
	str 	R0, [R7,4*PREG_R8]
	str 	R0, [R8,4*PREG_R9]
	str 	R0, [R9,4*PREG_R10]
	str 	R0, [R10,4*PREG_R11]
	str 	R0, [R11,4*PREG_IP]
	str 	R0, [R12,4*PREG_SP]
	str 	R0, [R13,4*PREG_LR]
	str 	R0, [R14,4*PREG_PC]
	mov 	R12,R0
	//NOTE: If only storing non-vol regs, why are we also storing R0 to R3?
	//I think callee-save is only R4 to R11

	// TODO: Restore callee-save ("non-volatile") registers.
	ldr R0, [R1,4*PREG_R0]
	ldr R1, [R1,4*PREG_R1]
	ldr R2, [R1,4*PREG_R2]
	ldr R3, [R1,4*PREG_R3]
	ldr R4, [R1,4*PREG_R4]
	ldr R5, [R1,4*PREG_R5]
	ldr R6, [R1,4*PREG_R6]
	ldr R7, [R1,4*PREG_R7]
	ldr R8, [R1,4*PREG_R8]
	ldr R9, [R1,4*PREG_R9]
	ldr R10, [R1,4*PREG_R10]
	ldr R11, [R1,4*PREG_R11]
	ldr R13, [R1,4*PREG_IP]
	ldr R14, [R1,4*PREG_LR]
	ldr R15, [R1,4*PREG_PC]

	// TODO: Jump to next function.

	.end ctxsw
