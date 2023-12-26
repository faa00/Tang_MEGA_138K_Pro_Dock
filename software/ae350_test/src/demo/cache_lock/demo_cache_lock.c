/*
 * ******************************************************************************************
 * File		: demo_cache_lock.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: L1 cache lock demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows hardware cache lock mechanism.
 *
 * Scenario:
 *
 * We demonstrate doing runtime code patching on a cacheable memory
 * to complete data coherence and illustrate how to lock the patch
 * code into I cache. First, we check the cache feature and enable
 * I/D cache if feature existing. Then we run "A" patch code to modify
 * the value of a global variable named g_selfmodify and use CCTL
 * instructions to do memory coherence without lock the patch code.
 * Next, we run nop instructions with 32K size to flush the "A" patch
 * code from I cache. Final we run another "B" patch code to modify
 * the g_selfmodify and check the correctness of g_selfmodify to
 * complete the unlock scenario. Oppositely, as we go the lock scenario,
 * we run and lock the "A" patch code into I cache, next run nop instructions
 * with 32K size to flush the "A" patch code, final run another "B" patch code
 * to modify the g_selfmodify and check the correctness of g_selfmodify to
 * complete the lock scenario.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running L1 cache lock demo
#if RUN_DEMO_CACHE_LOCK

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
#define ILCK_MSK                                (1ULL << 9)
#define DLCK_MSK                                (1ULL << 9)

/* BRPE branch target buffer enable in MMISC_CTL */
#define BRPE_EN                                 (1UL << 3)

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

#define ICACHE_SIZE_32K                         (2 * 512 * 1 * 32)

#define SC_UNLOCK                               1
#define SC_LOCK                                 2
#define SC_DONE                                 3

#define MEMSET(s, c, n)                         __builtin_memset ((s), (c), (n))


typedef void (*fun_ptr)(void*);

char buf_32k_[ICACHE_SIZE_32K] __attribute__ ((aligned(1024)));
volatile unsigned int g_selfmodify = 0;


// Check I-cache
static void invalid_icache(unsigned long* va, int size)
{
	unsigned int i;
	unsigned int isize = (unsigned int)(1 << (((read_csr(NDS_MICM_CFG) & ISIZE_MSK) >> 6) + 2));

	/* Check whether the MCU configured with CCTL auto-incremented feature. */
	if ((read_csr(NDS_MMSC_CFG) & VCCTL_MSK))
	{
		unsigned long final_va = (unsigned long)(va + size);
		unsigned long next_va = (unsigned long)va;

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
		/* L1C ICache invalidate */
		for (i = 0; size > 0; i++)
		{
			/* Invalid one cache line each time */
			write_csr(NDS_MCCTLBEGINADDR, (unsigned long)(va + (i * isize)));
			size -= isize;
			write_csr(NDS_MCCTLCOMMAND, CCTL_L1I_VA_INVAL);
		}
	}
}

// Lock I-cache
static void lock_icache(unsigned long* va, unsigned int size)
{
	unsigned int i;
	unsigned long mcctldata;
	unsigned int isize = (unsigned int)(1 << (((read_csr(NDS_MICM_CFG) & ISIZE_MSK) >> 6) + 2));

	/* Check whether the MCU configured with CCTL auto-incremented feature. */
	if ((read_csr(NDS_MMSC_CFG) & VCCTL_MSK))
	{
		unsigned long final_va = (unsigned long)(va + size);
		unsigned long next_va = (unsigned long)va;

		/* Write only once at the beginning, it will be updated by CCTL command CSR write operation */
		write_csr(NDS_MCCTLBEGINADDR, (unsigned long)va);

		/* L1C ICache lock */
		while (next_va < final_va)
		{
			/* Lock one cache line each time */
			write_csr(NDS_MCCTLCOMMAND, CCTL_L1I_VA_LOCK);
			mcctldata = read_csr(NDS_MCCTLDATA);
			if (!mcctldata)
			{
				printf("ICache lock fail!, va = 0x%08lx\r\n", next_va);
				return;
			}

			/* Read back from BEGINADDR csr for next va */
			next_va = read_csr(NDS_MCCTLBEGINADDR);
		}
	}
	else
	{
		/* L1C ICache lock */
		for (i = 0; size > 0; i++)
		{
			/* Lock one cache line each time */
			write_csr(NDS_MCCTLBEGINADDR, (unsigned long)(va + (i * isize)));
			size -= isize;
			write_csr(NDS_MCCTLCOMMAND, CCTL_L1I_VA_LOCK);
			mcctldata = read_csr(NDS_MCCTLDATA);
			if (!mcctldata)
			{
				printf("ICache lock fail!, va = 0x%08lx\r\n", ((unsigned long)va + (i * isize)));
				return;
			}
		}
	}
}

// Flush all D-cache
static void flush_all_dcache(void)
{
	/* L1C DCache writeback and invalidate all */
	write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_WBINVAL_ALL);
}

// Enable cache
static int enable_cache(void)
{
	/* Check whether the MCU configured with L1C cache. */
	if (!((read_csr(NDS_MICM_CFG) & ISIZE_MSK) >> 6) || !((read_csr(NDS_MDCM_CFG) & DSIZE_MSK) >> 6))
	{
		return 0;
	}

	/* Enable I/D cache HW prefetcher and D-cache write-around (threshold: 4 cache lines) as default */
	clear_csr(NDS_MCACHE_CTL, DC_WARND_MSK);
	set_csr(NDS_MCACHE_CTL, (0x1 << 13) | (0x1 << 10) | (0x1 << 9));

	/* Enable L1C cache */
	set_csr(NDS_MCACHE_CTL, (0x1 << 1) | (0x1 << 0));

	return 1;
}

void dummy_text_context(void* var0)
{
	/* To avoid the influence of GCOV, keep the code to one inline
	   assembly */
	asm volatile (
		"inject_start_77:"                  /* inject_start_77: */
		"li t0, %[input0];"                 /* g_selfmodify = 7777777 */
		"sw t0, 0(a0);"
		"ret;"
		"inject_end_77:"                    /* inject_end_77: */
		:
		: [input0]"i" (7777777)
		: "t0"
	);

	/* fill buf_32k w/ NOP instruction */
	asm volatile (
		"inject_nop_start:"                 /* inject_nop_start: */
		"nop;"                              /* nop is 2bytes on RV32/RV64 */
		"nop;"
		"inject_nop_end:"                   /* inject_nop_end: */
		"inject_jump_start:"                /* inject_jump_start: */
		"ret;"
		"inject_jump_end:"                  /* inject_jump_end: */
		:
		:
		:
	);

	asm volatile (
		"inject_start_88:"                  /* inject_start_88: */
		"li t0, %[input0];"                 /* g_selfmodify = 88888888 */
		"sw t0, 0(a0);"
		"ret;"
		"inject_end_88:"                    /* inject_end_88: */
		:
		: [input0]"i" (8888888)
		: "t0"
	);
}

static int cache_scenario(unsigned int sc)
{
	extern char inject_start_77, inject_end_77,   \
	            inject_start_88, inject_end_88,   \
	            inject_nop_start, inject_nop_end, \
	            inject_jump_start, inject_jump_end;

	char *pSrc, *pDes;
	unsigned short NOP = 0; /* NOP is 2bytes on RV32/RV64 */
	unsigned short* pUSDes;
	char buf_32k[ICACHE_SIZE_32K] __attribute__ ((aligned(1024)));
	fun_ptr fun_32k = (fun_ptr)buf_32k, fun_32k_ = (fun_ptr)buf_32k_;
	unsigned long i, size, des_32k_size = sizeof(buf_32k), des_32k_size_ = sizeof(buf_32k_);

	MEMSET(&buf_32k[0], 0, sizeof(buf_32k));

	if (sc == SC_UNLOCK)
	{
		printf("\r\n==============demo cache unlock===============\r\n");
		printf("Run unlock self modify code, expect g_selfmodify=8888888.....\r\n");
	}
	else if (sc == SC_LOCK)
	{
		printf("\r\n==============demo cache lock=================\r\n");
		printf("Run lock self modify code, expect g_selfmodify=7777777.....\r\n");
	}

	/* Step 1 : Run 32K NOP code to full-fill Icache to let the LRU algorithm of cache replacement more fare */
	pDes = (char*)&buf_32k_;
	pSrc = (char*)&inject_nop_start;
	size = (unsigned long)&inject_nop_end - (unsigned long)&inject_nop_start;

	if (des_32k_size_ < size)
	{
		printf("It will overwrite the next context. please reserve more space to copy\r\n");
		return -1;
	}

	/* NOP is 2bytes on RV32/RV64 */
	pUSDes = (unsigned short*)pDes;
	NOP = *(unsigned short*)pSrc;

	MEMSET(&buf_32k_[0], 0, sizeof(buf_32k_));

	/* Fill buf_32k_ w/ NOP instruction */
	for (i = 0; i < (des_32k_size_ / sizeof(unsigned short)); i++)
	{
		*pUSDes++ = NOP;
	}

	pSrc = (char*)&inject_jump_start;
	size = (unsigned long)&inject_jump_end - (unsigned long)&inject_jump_start;

	pDes += (des_32k_size_ - size);

	/* Fill end of buf_32k_ w/ JUMP instruction */
	for (i = 0; i < size; i++)
	{
		*pDes++ = *pSrc++;
	}

	/* Disable BTB feature to prevent self-modify code with illegal instruction be pulled into cache first. */
	/* Only invalid Dcache, can not invalid Icache. */
	flush_all_dcache();

	/* First run 32k NOP code to full-fill Icache */
	fun_32k_(NULL);

	/* Step 2 : Run selfmodify code to set g_selfmodify = 7777777 */
	g_selfmodify = 0;
	pDes = (char*)&buf_32k;
	pSrc = (char*)&inject_start_77;
	size = (unsigned long)&inject_end_77 - (unsigned long)&inject_start_77;

	if (des_32k_size < size)
	{
		printf("It will overwrite the next context. please reserve more space to copy\r\n");
		return -1;
	}

	for (i = 0; i < size; i++)
	{
		*pDes++ = *pSrc++;
	}

	/* Call Icache invalidate and Dcache writeback/invalidate */
	flush_all_dcache();
	invalid_icache((unsigned long*)fun_32k, size);

	if (sc == SC_LOCK)
	{
		/* Fill & lock fun_32k code to Icache line */
		lock_icache((unsigned long*)fun_32k, size);
	}

	printf("Run self modify code to set g_selfmodify = 7777777.....\r\n");

	/* Invoke the copied code w/ self modify code to reside in Icache, g_selfmodify = 7777777 */
	fun_32k((void*)&g_selfmodify);

	/* Step 3 : Run 32K NOP code to do cache replacement of selfmodify code */
	printf("Run 32k NOP buf to full-fill Icache to flush the unlocked Icache line of self modify code.....\r\n");

	/* Run 32k NOP code to full-fill Icache to flush the unlocked Icache line of self_modify code. */
	/* Invoke the copied code w/ NOP to reside in Icache. */
	fun_32k_(NULL);

	/* Step 4 : Run selfmodify code to set g_selfmodify = 8888888 */
	pDes = (char*)&buf_32k;
	pSrc = (char*)&inject_start_88;
	size = (unsigned long)&inject_end_88 - (unsigned long)&inject_start_88;

	if (des_32k_size < size)
	{
		printf("It will overwrite the next context. please reserve more space to copy\r\n");
		return -1;
	}

	MEMSET(&buf_32k[0], 0, sizeof(buf_32k));

	for (i = 0; i < size; i++)
	{
		*pDes++ = *pSrc++;
	}

	/* Only invalid Dcache, can not invalid Icache */
	flush_all_dcache();

	printf("Run self modify code to set g_selfmodify = 8888888.....\r\n");

	/* Invoke the copied code w/ self modify code to reside in Icache, g_selfmodify = 8888888 */
	fun_32k((void*)&g_selfmodify);

	printf("The g_selfmodify = %u\r\n", g_selfmodify);

	return 0;
}

// Run scenario
static void run_scenario(void)
{
	cache_scenario(SC_UNLOCK);

	if (g_selfmodify == 7777777)
	{
		printf("Run cache unlock scenario FAIL.\r\n");
	}
	else if (g_selfmodify == 8888888)
	{
		printf("Run cache unlock scenario PASS.\r\n");
	}

	cache_scenario(SC_LOCK);

	if (g_selfmodify == 7777777)
	{
		printf("Run cache lock scenario PASS.\r\n");
	}
	else if (g_selfmodify == 8888888)
	{
		printf("Run cache lock scenario FAIL.\r\n");
	}
}

// Application entry function
int demo_cache_lock(void)
{
	// Initializes UART
	uart_init(UART_SPEED);		// Baud rate is UART_SPEED

	printf("\r\nIt's a L1 Cache Lock Mechanism demo.\r\n\r\n");

	/* Disable BTB feature to prevent self-modify code with illegal instruction be pulled into cache first */
	clear_csr(NDS_MMISC_CTL, BRPE_EN);

	if (enable_cache())
	{
		/* Check whether the MCU configured with cache lock feature. */
		if ((read_csr(NDS_MICM_CFG) & ILCK_MSK) && (read_csr(NDS_MDCM_CFG) & DLCK_MSK))
		{
			printf("MCU supports cache lock feature.\r\n");
		}
		else
		{
			printf("MCU does NOT support cache lock feature.\r\n");
			while(1);
		}

		/* Check whether the MCU configured with CCTL feature. */
		if (!(read_csr(NDS_MMSC_CFG) & CCTLCSR_MSK))
		{
			printf("MCU does NOT support CCTL feature.\r\n");
			while(1);
		}

		run_scenario();

		printf("\r\nDemo L1 Cache Lock Completed.\r\n");
	}
	else
	{
		printf("\r\nMCU does NOT support cache.\r\n");
		while(1);
	}

	return 0;
}

#endif	/* RUN_DEMO_CACHE_LOCK */
