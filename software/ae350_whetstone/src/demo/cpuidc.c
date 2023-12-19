/*
 * ******************************************************************************************
 * File		: cpuidc.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Whetstone timer.
 * ******************************************************************************************
 */

/* Includes: */
#include <stdio.h>
#include "cpuidh.h"


/* Definitions: */

volatile SPDP  theseSecs = 0.0;
volatile SPDP  startSecs = 0.0;
volatile SPDP  secs;


/*
 * The mcycle counter is 64-bit counter. But RV32 access
 * it as two 32-bit registers, so we check for rollover
 * with this routine as suggested by the RISC-V Privileged
 * Architecture Specification.
*/
unsigned int get_core_freq(void)
{
	return (unsigned int)CPUFREQ;
}

unsigned long long rdmcycle(void)
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
	return (unsigned long long)read_csr(NDS_MCYCLE);
#endif
}

SPDP time(void)
{
	return (SPDP)rdmcycle()/get_core_freq();
}

void getSecs(void)
{
	theseSecs = time();

	return;
}

void start_time(void)
{
	getSecs();
	startSecs = theseSecs;

	return;
}

void end_time(void)
{
	getSecs();
	secs = theseSecs - startSecs;

	return;
}
