/*
 * ******************************************************************************************
 * File		: demo_pfm.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Hardware performance monitor demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo shows how to use basic RISC-V profiling counters, 'mcycle' (counts the number
 * of processor cycles) and 'minstret' (counts the number of retired instructions).
 * Note that both are writable as well.
 *
 * Scenario:
 *
 * The main function uses a simple program (factorial) to measure the performance by 'mcycle'
 * and 'minstret' hardware performance monitor. It demos two measure methodology. One is
 * using counter differences and the other is clearing counters to use values directly.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running hardware performance monitor demo
#if RUN_DEMO_PFM

// ************ Includes ************ //
#include "platform.h"
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********** //

/*
 * The 'mcycle' counter is 64-bit counter. But RV32 access
 * it as two 32-bit registers, so we check for rollover
 * with this routine as suggested by the RISC-V Privileged
 * Architecture Specification.
 */
__attribute__((always_inline))
static inline unsigned long long rdmcycle(void)
{
#if __riscv_xlen == 32
	do
	{
		unsigned long hi = read_csr(NDS_MCYCLEH);
		unsigned long lo = read_csr(NDS_MCYCLE);

		if (hi == read_csr(NDS_MCYCLEH))
		{
			return ((unsigned long long)hi << 32) | lo;
		}
	} while(1);
#else
	return read_csr(NDS_MCYCLE);
#endif
}

/*
 * The 'minstret' counter is 64-bit counter. But RV32 access
 * it as two 32-bit registers, same as for 'mcycle'.
 */
__attribute__((always_inline))
static inline unsigned long long rdminstret(void)
{
#if __riscv_xlen == 32
	do
	{
		unsigned long hi = read_csr(NDS_MINSTRETH);
		unsigned long lo = read_csr(NDS_MINSTRET);

		if (hi == read_csr(NDS_MINSTRETH))
		{
			return ((unsigned long long)hi << 32) | lo;
		}
	} while(1);
#else
	return read_csr(NDS_MINSTRET);
#endif
}

/* Simple factorial program to measure the performance. */
int factorial(int i)
{
	int result = 1;

	for (int ii = 1; ii <= i; ii++)
	{
		result = result * i;
	}

	return result;
}

// Application entry function
int demo_pfm(void)
{
	unsigned long long before_cycle, before_instret;
	unsigned long long after_cycle, after_instret;
	volatile int sequence = 100;    // Prevent optimize repeated 'factorial' function calls

	// Initializes UART
	uart_init(38400);		// Baud rate is 38400

	printf("\r\nIt's a Hardware Performance Monitor demo.\r\n");

	/*
	 * Check whether the CPU configured with Performance monitoring counters.
	 * The mmsc_cfg.NOPMC bit 0 indicates this.
	 */
	if (read_csr(NDS_MMSC_CFG) & (1 << 22))
	{
		printf("CPU does NOT support PFM.\r\n");
		while(1);
	}

	printf("\r\nRun factorial(%d) mathematics ...\r\n", sequence);

	printf("\r\nDemo 1: Using Counter Differences.\r\n");

	for (int ii = 0; ii < 3; ii++)
	{
		before_cycle = rdmcycle();
		before_instret = rdminstret();

		__attribute__((unused)) volatile int result = factorial(sequence);

		after_cycle = rdmcycle();
		after_instret = rdminstret();

		printf("Loop %d: Retired %d instructions in %d cycles\r\n", ii, (unsigned int)(after_instret - before_instret), (unsigned int)(after_cycle - before_cycle));
	}

	printf("\r\nDemo 2: Clearing Counters, Using Values Directly.\r\n");

 	for (int ii = 0; ii < 3; ii++)
 	{
		write_csr(NDS_MCYCLE,  0);
		write_csr(NDS_MINSTRET, 0);
#if __riscv_xlen == 32
		write_csr(NDS_MCYCLEH, 0);
		write_csr(NDS_MINSTRETH, 0);
#endif
		__attribute__((unused)) volatile int result = factorial(sequence);

		after_cycle = rdmcycle();
		after_instret = rdminstret();

		printf("Loop %d: Retired %d instructions in %d cycles\r\n", ii, (unsigned int)(after_instret), (unsigned int)(after_cycle));
	}

 	printf("\r\nHardware Performance Monitor Demo Completed.\r\n");

	return 0;
}

#endif	/* RUN_DEMO_PFM */
