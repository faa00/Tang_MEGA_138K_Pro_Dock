/*
 * ******************************************************************************************
 * File		: ae350.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Platform and system initialized, and start up
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "platform.h"


// Declarations -----------------------------------------------------------------------------
extern void reset_vector(void);


// Definitions ------------------------------------------------------------------------------
/* This must be a leaf function, no child function */
void __platform_init (void) __attribute__((naked));
void __platform_init(void)
{
	/* Do your platform low-level initial */

#ifdef CFG_CACHE_ENABLE
	/*
	 * Enable I/D cache with HW pre-fetcher,
	 * D-cache write-around (threshold: 4 cache lines),
	 * and CM (Coherence Manager).
	 */
	/* CSR: control and status register */
	clear_csr(NDS_MCACHE_CTL, (3 << 13));
	set_csr(NDS_MCACHE_CTL, (1 << 19) | (1 << 13) | (1 << 10) | (1 << 9) | (1 << 1) | (1 << 0));

	/* Check if CPU support CM or not. */
	if (read_csr(NDS_MCACHE_CTL) & (1 << 19))
	{
		/* Wait for cache coherence enabling completed */
		while((read_csr(NDS_MCACHE_CTL) & (1 << 20)) == 0);
	}
#endif

	__asm("ret");
}

/* Start up */
void c_startup(void)
{
#define MEMCPY(des, src, n)     __builtin_memcpy ((des), (src), (n))
#define MEMSET(s, c, n)         __builtin_memset ((s), (c), (n))

	/* Data section initialization */
	extern char  _edata, _end;
	unsigned int size;

	/* Start up and execute in ROM/Flash (0x80000000) */
#ifdef CFG_XIP
	extern char __data_lmastart, __data_start;

	/* Copy data section from LMA to VMA */
	size = &_edata - &__data_start;
	MEMCPY(&__data_start, &__data_lmastart, size);
#endif

	/* Clear BSS section */
	size = &_end - &_edata;
	MEMSET(&_edata, 0, size);
}

/* Initializes system */
void system_init(void)
{
	/*
	 * Do your system reset handler here
	 */
	/* Reset the CPU reset vector for this program. */
	/* Entry address upon processor reset */
	/* ROM/Flash address: 0x80000000 */
	/* Single core CPU, hart0, 0x50 */
	DEV_SMU->HART0_RESET_VECTOR = (unsigned int)(long)reset_vector;

	/* Enable PLIC features */
	if (read_csr(NDS_MMISC_CTL) & (1 << 1))
	{
		/* External PLIC interrupt is vectored */
		__nds__plic_set_feature(NDS_PLIC_FEATURE_PREEMPT | NDS_PLIC_FEATURE_VECTORED);
	}
	else
	{
		/* External PLIC interrupt is NOT vectored */
		__nds__plic_set_feature(NDS_PLIC_FEATURE_PREEMPT);
	}

	/* Enable misaligned access and non-blocking load */
	set_csr(NDS_MMISC_CTL, (1 << 8) | (1 << 6));
}
