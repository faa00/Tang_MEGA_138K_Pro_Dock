/*
 * ******************************************************************************************
 * File		: demo_cache.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: L1 cache demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows hardware cache mechanism.
 *
 * Scenario:
 *
 * We demonstrates doing runtime code patching on a cacheable memory
 * to complete data coherence. First, we check the cache feature and
 * enable I/D cache if feature existing. Then we modify the value of
 * a global variable named g_selfmodify within D cache memory and use
 * fence.i instruction to do memory coherence. Final we check the
 * correctness of g_selfmodify to complete this demo.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running L1 cache demo
#if RUN_DEMO_CACHE

// ************ Includes ************ //
#include "platform.h"
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********** //

/* CSR NDS_ICM_CFG */
#define ISET_MSK                                ((1ULL << 2) | (1ULL << 1) | (1ULL << 0))
#define IWAY_MSK                                ((1ULL << 5) | (1ULL << 4) | (1ULL << 3))
#define ISIZE_MSK                               ((1ULL << 8) | (1ULL << 7) | (1ULL << 6))

/* CSR NDS_DCM_CFG */
#define DSET_MSK                                ((1ULL << 2) | (1ULL << 1) | (1ULL << 0))
#define DWAY_MSK                                ((1ULL << 5) | (1ULL << 4) | (1ULL << 3))
#define DSIZE_MSK                               ((1ULL << 8) | (1ULL << 7) | (1ULL << 6))

/* CSR NDS_MCACHE_CTL */
#define DC_WARND_MSK                            (0x3 << 13)
#define DC_COHEN_MSK                            (0x1 << 19)
#define DC_COHSTA_MSK                           (0x1 << 20)

/* CSR NDS_MMSC_CFG */
#define CCTLCSR_MSK                             (1ULL << 16)
#define VCCTL_MSK                               ((1ULL << 18) | (1ULL << 19))

/* CCTL Command Machine mode */
/* Allow S/U mode */
#define CCTL_L1D_VA_INVAL                       0
/* Allow S/U mode */
#define CCTL_L1D_VA_WB                          1
/* Allow S/U mode */
#define CCTL_L1D_VA_WBINVAL                     2
#define CCTL_L1D_VA_LOCK                        3
#define CCTL_L1D_VA_UNLOCK                      4
#define CCTL_L1D_WBINVAL_ALL                    6
#define CCTL_L1D_WB_ALL                         7
/* Allow S/U mode */
#define CCTL_L1I_VA_INVAL                       8
#define CCTL_L1I_VA_LOCK                        11
#define CCTL_L1I_VA_UNLOCK                      12
#define CCTL_L1D_IX_INVAL                       16
#define CCTL_L1D_IX_WB                          17
#define CCTL_L1D_IX_WBINVAL                     18
#define CCTL_L1D_IX_RTAG                        19
#define CCTL_L1D_IX_RDATA                       20
#define CCTL_L1D_IX_WTAG                        21
#define CCTL_L1D_IX_WDATA                       22
#define CCTL_L1D_INVAL_ALL                      23
#define CCTL_L1I_IX_INVAL                       24
#define CCTL_L1I_IX_RTAG                        27
#define CCTL_L1I_IX_RDATA                       28
#define CCTL_L1I_IX_WTAG                        29
#define CCTL_L1I_IX_WDATA                       30

#define BUF_SIZE                                0x100


typedef void (*fun_ptr)(void*);

volatile unsigned int g_selfmodify = 0;

// Enable I-cache and D-cache
int enableIDCache(void)
{
	unsigned int iset, iway, isize, dset, dway, dsize;
	unsigned long icm_cfg = 0, dcm_cfg = 0;

	/* Check if support CCTL feature */
	if (read_csr(NDS_MMSC_CFG) & CCTLCSR_MSK)
	{
		printf("MCU supports CCTL operation\r\n");

		if (read_csr(NDS_MMSC_CFG) & VCCTL_MSK)
		{
			printf("MCU supports CCTL auto-increment\r\n");
		}
		else
		{
			printf("MCU does NOT support CCTL auto-increment\r\n");
		}

	}
	else
	{
		printf("MCU supports FENCE operation\r\n");
	}

	/* Enable I/D cache HW prefetcher and D-cache write-around (threshold: 4 cache lines) as default */
	clear_csr(NDS_MCACHE_CTL, DC_WARND_MSK);
	set_csr(NDS_MCACHE_CTL, (0x1 << 13) | (0x1 << 10) | (0x1 << 9));

	/* Get ICache ways, sets, line size */
	icm_cfg = read_csr(NDS_MICM_CFG);
	if ((icm_cfg & ISET_MSK) < 7)
	{
		iset = (unsigned int)(1 << ((icm_cfg & ISET_MSK) + 6));
	}
	else
	{
		iset = 0;
	}
	printf("The L1C ICache sets = %u\r\n", iset);

	iway = (unsigned int)(((icm_cfg & IWAY_MSK) >> 3) + 1);
	printf("The L1C ICache ways = %u\r\n", iway);

	if (((icm_cfg & ISIZE_MSK) >> 6) && (((icm_cfg & ISIZE_MSK) >> 6) <= 5))
	{
		isize = (unsigned int)(1 << (((icm_cfg & ISIZE_MSK) >> 6) + 2));
	}
	else if (((icm_cfg & ISIZE_MSK) >> 6) >= 6)
	{
		printf("Warning L1C i cacheline size is reserved value\r\n");
		isize = 0;
	}
	else
	{
		isize = 0;
	}

	printf("The L1C ICache line size = %u\r\n", isize);
	if (isize == 0)
	{
		printf("This MCU doesn't have L1C ICache.\r\n");
		return 0;
	}
	else
	{
		/* Enable L1C ICache */
		set_csr(NDS_MCACHE_CTL, (0x1 << 0));
	}

	/* Get DCache ways, sets, line size  */
	dcm_cfg = read_csr(NDS_MDCM_CFG);
	if ((dcm_cfg & DSET_MSK) < 7)
	{
		dset = (unsigned int)(1 << ((dcm_cfg & DSET_MSK) + 6));
	}
	else
	{
		dset = 0;
	}
	printf("The L1C DCache sets = %u\r\n", dset);

	dway = (unsigned int)(((dcm_cfg & DWAY_MSK) >> 3) + 1);
	printf("The L1C DCache ways = %u\r\n", dway);

	if (((dcm_cfg & DSIZE_MSK) >> 6) && (((dcm_cfg & DSIZE_MSK) >> 6) <= 5))
	{
		dsize = (unsigned int)(1 << (((dcm_cfg & DSIZE_MSK) >> 6) + 2));
	}
	else if (((dcm_cfg & DSIZE_MSK) >> 6) >= 6)
	{
		printf("Warning L1C d cacheline size is reserved value\r\n");
		dsize = 0;
	}
	else
	{
		dsize = 0;
	}

	printf("The L1C DCache line size = %u\r\n", dsize);
	if (dsize == 0)
	{
		printf("This MCU doesn't have L1C DCache.\r\n");
		return 0;
	}
	else
	{
		/* Enable L1C DCache */
		set_csr(NDS_MCACHE_CTL, (0x1 << 1));
	}

	if (read_csr(NDS_MCACHE_CTL) & 0x1)
	{
		printf("Enable L1C I cache\r\n");
	}

	if (read_csr(NDS_MCACHE_CTL) & 0x2)
	{
		printf("Enable L1C D cache\r\n");
	}

	if (!(read_csr(NDS_MCACHE_CTL) & 0x3))
	{
		printf("Can't enable L1C I/D cache\r\n");
	}

	return 1;
}

// Cache operation
void cacheOp(void* va, long size)
{
	printf("I/D cache flush\r\n");

	if(read_csr(NDS_MMSC_CFG) & CCTLCSR_MSK)
	{
		unsigned int i, tmp = 0, isize = 0, dsize = 0;

		isize = (unsigned int)(1 << (((read_csr(NDS_MICM_CFG) & ISIZE_MSK) >> 6) + 2));
		dsize = (unsigned int)(1 << (((read_csr(NDS_MDCM_CFG) & DSIZE_MSK) >> 6) + 2));

		/* Check whether the MCU configured with CCTL auto-incremented feature. */
		if ((read_csr(NDS_MMSC_CFG) & VCCTL_MSK))
		{
			unsigned long final_va = (unsigned long)(va + size);
			unsigned long next_va = (unsigned long)va;

			/* Write only once at the beginning, it will be updated by CCTL command CSR write operation */
			write_csr(NDS_MCCTLBEGINADDR, (unsigned long)va);

			/* L1C DCache write back and invalidate */
			while (next_va < final_va)
			{
				/* Write back and invalid one cache line each time */
				write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_VA_WBINVAL);

				/* Read back from BEGINADDR csr for next va */
				next_va = read_csr(NDS_MCCTLBEGINADDR);
			}

			/* Write only once at the beginning, it will be updated by CCTL command CSR write operation */
			write_csr(NDS_MCCTLBEGINADDR, (unsigned long)va);

			/* L1C ICache invalidate */
			while (next_va < final_va)
			{
				/* Invalid one cache line each time */
				write_csr(NDS_MCCTLCOMMAND, CCTL_L1I_VA_INVAL);

				/* Read back from BEGINADDR csr for next va */
				next_va = read_csr(NDS_MCCTLBEGINADDR);
			}
		}
		else
		{
			/* L1C DCache write back and invalidate */
			for (i = 0; tmp < size; i++)
			{
				/* Write back and invalid one cache line each time */
				write_csr(NDS_MCCTLBEGINADDR, (unsigned long)(va + (i * dsize)));
				tmp += dsize;
				write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_VA_WBINVAL);
			}

			/* L1C ICache invalidate */
			for (i = 0; tmp < size; i++)
			{
				/* Invalid one cache line each time */
				write_csr(NDS_MCCTLBEGINADDR, (unsigned long)(va + (i * isize)));
				tmp += isize;
				write_csr(NDS_MCCTLCOMMAND, CCTL_L1I_VA_INVAL);
			}
		}
	}
	else
	{
		/* FENCE.I go data writeback w/o data invalid but instruction invalid.
		   As src code is copied to dst and go to execute dst instruction,
		   you should use FENCE.I instead of FENCE */
		__nds__fencei();
	}
}

void __attribute__((naked)) dummyTextContext(void* var)
{
	/* To avoid the influence of GCOV, keep the code to one inline
	   assembly */
	asm volatile (
		"injectionStart:"
		"li t0, %0;"
		"sw t0, 0(a0);"			/* *var = 7777777 */
		"ret;"
		"injectionEnd:"
		:
		: "i" (7777777)
		: "t0"
	);
}

void selfModifyCode(void)
{
	extern char injectionStart, injectionEnd;
	char buf[BUF_SIZE];
	fun_ptr fun = (fun_ptr)buf;

	char *pSrc, *pDes;
	unsigned int size, des_size = sizeof(buf), i;

	printf("Execute original self modify code.\r\n");
	pDes = (char*)&buf;
	pSrc = (char*)&injectionStart;
	size = (unsigned int)((unsigned long)&injectionEnd - (unsigned long)&injectionStart);

	if (des_size < size)
	{
		printf("It will overwrite the next context. please reserve more space to copy\r\n");
	}

	for (i = 0; i < size; i++)
	{
		*pDes++ = *pSrc++;
	}

	/* Call Icache invalidate and Dcache writeback, invalidate */
	cacheOp((void*)fun, BUF_SIZE);

	/* Invoke copied code in buf */
	fun((void*)&g_selfmodify);
}

// Application entry function
int demo_cache(void)
{
	// Initializes UART
	uart_init(38400);	// Baud rate is 38400

	printf("\r\nIt's a L1 Cache demo.\r\n\r\n");

	if(enableIDCache())
	{
		selfModifyCode();
		if (g_selfmodify == 7777777)
		{
			printf("Run selfModifyCode Pass.\r\n");
		}
		else
		{
			printf("Run selfModifyCode Fail.\r\n");
		}

		printf("L1 Cache Completed.\r\n");
	}

	return 0;
}

#endif	/* RUN_DEMO_CACHE */
