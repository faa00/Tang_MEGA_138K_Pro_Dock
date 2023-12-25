/*
 * ******************************************************************************************
 * File		: delay.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Delay predefine
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "platform.h"
#include "delay.h"


// Definitions ------------------------------------------------------------------------------

/*
 * The mcycle counter is 64-bit counter. But RV32 access
 * it as two 32-bit registers, so we check for rollover
 * with this routine as suggested by the RISC-V Privileged
 * Architecture Specification.
 */

unsigned int get_timer_freq(void)
{
	return (unsigned int)CPUFREQ;
}

unsigned long long get_timer_value(void)
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

// Simple delay microsecond
void simple_delay_ms(unsigned int delay_ms)
{
	for(delay_ms=(OSCFREQ>>15)*delay_ms; delay_ms != 0; delay_ms--);
}
