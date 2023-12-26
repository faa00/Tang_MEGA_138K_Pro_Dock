/*
 * ******************************************************************************************
 * File		: demo_hsp.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Hardware stack protection demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows hardware stack protection and recording mechanism.
 *
 * Scenario:
 *
 * We use HANOI tower game to illustrate the hardware stack protection features.
 * The main function will enable the hardware stack operating scheme of recording
 * mechanism first to get the top of stack for HANOI towers moves using
 * recursion. And then, switch to enable the hardware stack operating scheme of
 * stack protection mechanism to do the HANOI towers moves again by reducing
 * the stack size bound to generate the stack overflow exception trap.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running hardware stack protection demo
#if RUN_DEMO_HSP

// ************ Includes ************ //
#include "platform.h"
#include "uart.h"
#include <stdio.h>

#ifdef CFG_GCOV
#include <stdlib.h>
#endif


// ********** Definitions ********** //

/* HSP feature configuration in MMSC_CFG */
#define MISC_HSP                        (1UL << 5)

/* Machine mode MHSP_CTL */
#define MHSP_CTL_OVF_EN                 (1UL << 0)
#define MHSP_CTL_UDF_EN                 (1UL << 1)
#define MHSP_CTL_SCHM_TOS               (1UL << 2)
#define MHSP_CTL_SCHM_DETECT            (0UL << 2)
#define MHSP_CTL_U_EN                   (1UL << 3)
#define MHSP_CTL_S_EN                   (1UL << 4)
#define MHSP_CTL_M_EN                   (1UL << 5)

/* How many disks for HANOI tower */
#define HANOI_DISKS                     4

/* String buffer size */
#define STRBUF_SIZE                     256


// Stack overflow exception handler
long except_handler(long cause, long epc, long *reg)
{
	printf("\r\n[Exception] : ");

	/* Show exception error type. */
	/* MCAUSE.Exccode[9:0] is exception/interrupt code for CLIC */
	switch (cause & 0x3FF)
	{
	case TRAP_M_STACKOVF:
		printf("Stack Overflow (sp = 0x%x)\r\n", (unsigned int)__nds__get_current_sp());
		break;
	default:
		/* Unhandled Trap */
		printf("Unhandled Trap : mcause = 0x%x, mepc = 0x%x\r\n", (unsigned int)cause, (unsigned int)epc);
		break;
	}

#ifdef CFG_GCOV
	exit(0);
#else
	while(1);
#endif
}

//HANOI tower game
static void towers(int num, char frompeg, char topeg, char auxpeg, int *move)
{
	/* consume the stack */
	char buf[STRBUF_SIZE];

	if (num == 1)
	{
		sprintf(buf, "\r\nMove disk 1 from %c to %c", frompeg, topeg);
		printf("%s", buf);
		(*move)++;
		return;
	}

	towers(num - 1, frompeg, auxpeg, topeg, move);

	sprintf(buf, "\r\nMove disk %d from %c to %c", num, frompeg, topeg);
	printf("%s", buf);

	(*move)++;
	towers(num - 1, auxpeg, topeg, frompeg, move);
}

// Application entry function
int demo_hsp(void)
{
	int disks, moves;
	unsigned long tos;
	extern long _stack;

	// Initializes UART
	uart_init(UART_SPEED);		// Baud rate is UART_SPEED

	printf("\r\nIt's a Hardware Stack Protection and Recording Mechanism demo.\r\n");

	/*
	 * Check whether the CPU configured with Hardware Stack protection/recording or not.
	 * The MMSC_CFG bit 5 indicates this.
	 */
	if (!(read_csr(NDS_MMSC_CFG) & MISC_HSP))
	{
		printf("CPU does NOT support Hardware Stack protection/recording.\r\n");
		goto error;
	}

	printf("\r\nThe initial stack pointer : 0x%x\r\n", (unsigned int)(long)&_stack);

	/*
	 * Following demos the Hardware Stack recording.
	 */
	printf("[Hardware Stack Recording]\r\n");

	/* Disable Hardware Stack protection first. */
	write_csr(NDS_MHSP_CTL, 0);

	printf("\r\nHANOI Tower with %d disks : \r\n", HANOI_DISKS);

	/* Set SP bound to maximum */
	write_csr(NDS_MSP_BOUND, -1);

	/* Enable MHSP_CTL : Top stack recording, machine mode enabled */
	set_csr(NDS_MHSP_CTL, MHSP_CTL_OVF_EN | MHSP_CTL_SCHM_TOS | MHSP_CTL_M_EN);

	/* HANOI Tower */
	disks = HANOI_DISKS;
	moves = 0;
	towers(disks, 'A', 'C', 'B', &moves);

	printf("\r\nHANOI(%d) = %d moves\r\n\r\n", disks, moves);

	/* Get latest updated from the SP_BOUND register */
	tos = read_csr(NDS_MSP_BOUND);

	printf("Top of Stack : 0x%x \r\n", (unsigned int)tos);

	/*
	 * Following demos the Hardware Stack overflow detection.
	 */
	printf("[Hardware Stack Overflow Detection]\r\n");

	/* Disable Hardware Stack protection first. */
	write_csr(NDS_MHSP_CTL, 0);

	/*
	 * Set SP bound to stack overflow value.
	 * (Large than top of stack and 512 byte alignment)
	 */
	tos = ((tos + 512) & ~511);
	write_csr(NDS_MSP_BOUND, tos);

	printf("Set stack top bound : 0x%x\r\n", (unsigned int)tos);

	printf("Retest...\r\nHANOI Tower with %d disks : \r\n", HANOI_DISKS);

	/* Enable MHSP_CTL : Overflow detection, machine mode enabled */
	set_csr(NDS_MHSP_CTL, MHSP_CTL_OVF_EN | MHSP_CTL_SCHM_DETECT | MHSP_CTL_M_EN);

	/*
	 * Retest the HANOI tower moves again
	 * Since the SP bound is set to the value large than top of stack needed
	 * in HANOI tower moves, the stack overflow exception will be generated.
	 */
	disks = HANOI_DISKS;
	moves = 0;
	towers(disks, 'A', 'C', 'B', &moves);

	// It is right : Enter except handler

	/* Never to here */
	printf("ERROR : Hardware Stack Protection failed\r\n");

error:
	return 1;
}

#endif	/* RUN_DEMO_HSP */
