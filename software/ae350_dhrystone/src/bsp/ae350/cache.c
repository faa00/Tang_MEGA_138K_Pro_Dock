/*
 * ******************************************************************************************
 * File		: cache.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: L1 cache
 * ******************************************************************************************
 */

// Includes ----------------------------------------------------------------------------------
#include "cache.h"


// Definitions -------------------------------------------------------------------------------

/* CSR bit-field */

/* MICM_CFG, MDCM_CFG */
#define AE350_ISET					0x7
#define AE350_IWAY					0x38
#define AE350_ISIZE					0x1C0
#define AE350_DSET					0x7
#define AE350_DWAY					0x38
#define AE350_DSIZE					0x1C0

/* L1 CCTL Command */
#define CCTL_L1D_VA_INVAL			0
#define CCTL_L1D_VA_WB				1
#define CCTL_L1D_VA_WBINVAL			2
#define CCTL_L1D_VA_LOCK			3
#define CCTL_L1D_VA_UNLOCK			4
#define CCTL_L1D_WBINVAL_ALL		6
#define CCTL_L1D_WB_ALL				7
#define CCTL_L1I_VA_INVAL			8
#define CCTL_L1I_VA_LOCK			11
#define CCTL_L1I_VA_UNLOCK			12
#define CCTL_L1D_IX_INVAL			16
#define CCTL_L1D_IX_WB				17
#define CCTL_L1D_IX_WBINVAL			18
#define CCTL_L1D_IX_RTAG			19
#define CCTL_L1D_IX_RDATA			20
#define CCTL_L1D_IX_WTAG			21
#define CCTL_L1D_IX_WDATA			22
#define CCTL_L1D_INVAL_ALL			23
#define CCTL_L1I_IX_INVAL			24
#define CCTL_L1I_IX_RTAG			27
#define CCTL_L1I_IX_RDATA			28
#define CCTL_L1I_IX_WTAG			29
#define CCTL_L1I_IX_WDATA			30


/* Helper Macros */
#define ALWAYS_INLINE				inline __attribute__((always_inline))

/* Value of x rounded up to the next multiple of align. align must be power of 2. */
#define ROUND_UP(x, align)			(((x) + (align) - 1) & ~((align) - 1))

/* Value of x rounded down to the prev multiple of align. align must be power of 2. */
#define ROUND_DOWN(x, align)		((x) & ~((align) - 1))


/* Critical section APIs */
// GIE save
static ALWAYS_INLINE unsigned long GIE_SAVE(void)
{
	/* Disable global interrupt for core */
	return clear_csr(NDS_MSTATUS, MSTATUS_MIE) & MSTATUS_MIE;
}

// GIE restore
static ALWAYS_INLINE void GIE_RESTORE(unsigned long var)
{
	set_csr(NDS_MSTATUS, var);
}


/* Cache APIs */
struct _cache_info cache_info = {.is_init = 0};
enum cache_t {ICACHE, DCACHE};

// Get cache information
void get_cache_info(void)
{
	/* Critical Section */
	unsigned long saved_gie = GIE_SAVE();

	/* Check cache_info has been initialized again in the critical section. */
	if (!cache_info.is_init)
	{
		/* Get cache line size */
		// I-Cache size == D-Cache size
		unsigned long size = (read_csr(NDS_MDCM_CFG) & AE350_DSIZE) >> 6;
		if ((size > 0) && (size <= 5))
		{
			cache_info.cacheline_size = 1 << (size + 2);
		}
		else
		{
			cache_info.cacheline_size = 0;
		}

		/* Finish initialization */
		cache_info.is_init = 1;
	}

	GIE_RESTORE(saved_gie);
}

// Cache settings
static inline unsigned long cache_set(enum cache_t cache)
{
	if (cache == ICACHE)
	{
		// I-Cache
		return ((read_csr(NDS_MICM_CFG) & AE350_ISET) < 7) ? (unsigned long)(1 << ((read_csr(NDS_MICM_CFG) & AE350_ISET) + 6)) : 0;
	}
	else
	{
		// D-Cache
		return ((read_csr(NDS_MDCM_CFG) & AE350_DSET) < 7) ? (unsigned long)(1 << ((read_csr(NDS_MDCM_CFG) & AE350_DSET) + 6)) : 0;
	}
}

// Cache ways
static inline unsigned long cache_way(enum cache_t cache)
{
	if (cache == ICACHE)
	{
		// I-Cache
		return (unsigned long)(((read_csr(NDS_MICM_CFG) & AE350_IWAY) >> 3) + 1);
	}
	else
	{
		// D-Cache
		return (unsigned long)(((read_csr(NDS_MDCM_CFG) & AE350_DWAY) >> 3) + 1);
	}
}

/* Low-level Cache APIs */

/*
 * Note: Low-level CCTL functions may not be thread-safe if it uses more than
 * 1 CCTL register because IRQ can pollute CCTL register. Thus, caller needs to
 * protect thread-safety of them.
 */
static void ae350_l1c_icache_invalidate_range(unsigned long start, unsigned long size)
{
	unsigned long line_size = cache_line_size();

	unsigned long last_byte = start + size - 1;
	start = ROUND_DOWN(start, line_size);

	while (start <= last_byte)
	{
		write_csr(NDS_MCCTLBEGINADDR, start);
		write_csr(NDS_MCCTLCOMMAND, CCTL_L1I_VA_INVAL);
		start += line_size;
	}
}

/*
 * ae350_l1c_icache_invalidate_all(void)
 *
 * invalidate all L1 I-cached data.
 *
 * Note: CCTL L1 doesn't support i-cache invalidate all operation,
 * so this function emulates "Invalidate all" operation by invalidating
 * each cache line of cache (index-based CCTL).
 */
static void ae350_l1c_icache_invalidate_all(void)
{
	unsigned long line_size = cache_line_size();
	unsigned long end = cache_way(ICACHE) * cache_set(ICACHE) * line_size;

	for (int i = 0; i < end; i += line_size)
	{
		write_csr(NDS_MCCTLBEGINADDR, i);
		write_csr(NDS_MCCTLCOMMAND, CCTL_L1I_IX_INVAL);
	}
}

// L1 D-Cache write back range
static void ae350_l1c_dcache_writeback_range(unsigned long start, unsigned long size)
{
	unsigned long line_size = cache_line_size();

	unsigned long last_byte = start + size - 1;
	start = ROUND_DOWN(start, line_size);

	while (start <= last_byte)
	{
		write_csr(NDS_MCCTLBEGINADDR, start);
		write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_VA_WB);
		start += line_size;
	}
}

// L1 D-Cache invalidate range
static void ae350_l1c_dcache_invalidate_range(unsigned long start, unsigned long size)
{
	unsigned long line_size = cache_line_size();

	unsigned long last_byte = start + size - 1;
	start = ROUND_DOWN(start, line_size);

	while (start <= last_byte)
	{
		write_csr(NDS_MCCTLBEGINADDR, start);
		write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_VA_INVAL);
		start += line_size;
	}
}

// L1 D-Cache flush range
static void ae350_l1c_dcache_flush_range(unsigned long start, unsigned long size)
{
	unsigned long line_size = cache_line_size();

	unsigned long last_byte = start + size - 1;
	start = ROUND_DOWN(start, line_size);

	while (start <= last_byte)
	{
		write_csr(NDS_MCCTLBEGINADDR, start);
		write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_VA_WBINVAL);
		start += line_size;
	}
}

// L1 D-Cache flush all
static ALWAYS_INLINE void ae350_l1c_dcache_flush_all(void)
{
	write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_WBINVAL_ALL);
}


/* High-Level Cache APIs */
// I-Cache invalidate range
void ae350_icache_invalidate_range(unsigned long start, unsigned long size)
{
	unsigned long saved_gie = GIE_SAVE();
	ae350_l1c_icache_invalidate_range(start, size);	// L1 I-Cache
	GIE_RESTORE(saved_gie);
}

/*
 * ae350_icache_invalidate_all(void)
 *
 * invalidate all I-cached data.
 */
void ae350_icache_invalidate_all(void)
{
	unsigned long saved_gie = GIE_SAVE();
	ae350_l1c_icache_invalidate_all();	// L1 I-Cache
	GIE_RESTORE(saved_gie);
}

// D-Cache write back range
void ae350_dcache_writeback_range(unsigned long start, unsigned long size)
{
	unsigned long saved_gie = GIE_SAVE();
	ae350_l1c_dcache_writeback_range(start, size);	// L1 D-Cache
	GIE_RESTORE(saved_gie);
}

// D-Cache invalidate range
void ae350_dcache_invalidate_range(unsigned long start, unsigned long size)
{
	unsigned long saved_gie = GIE_SAVE();
	ae350_l1c_dcache_invalidate_range(start, size);	// L1 D-Cache
	GIE_RESTORE(saved_gie);
}

// D-Cache flush range
void ae350_dcache_flush_range(unsigned long start, unsigned long size)
{
	unsigned long saved_gie = GIE_SAVE();
	ae350_l1c_dcache_flush_range(start, size);	// L1 D-Cache
	GIE_RESTORE(saved_gie);
}

// D-Cache flush all
void ae350_dcache_flush_all(void)
{
	/*
	 * Note: To keep no data loss in full L1C invalidation, there
	 * shouldn't any store instruction to cacheable region between
	 * L1C write back and L1C invalidation.
	 */
	if (!cache_info.is_init)
	{
		/*
		 * Make sure get_cache_info() isn't called between CCTL
		 * operations because this function does some store
		 * instructions.
		 */
		get_cache_info();
	}

	unsigned long saved_gie = GIE_SAVE();
	ae350_l1c_dcache_flush_all();	// L1 D-Cache
	GIE_RESTORE(saved_gie);
}

// D-Cache invalidate address
static ALWAYS_INLINE void ae350_dcache_invalidate_addr(unsigned long addr)
{
	ae350_dcache_invalidate_range(addr, 1);
}

// D-Cache flush address
static ALWAYS_INLINE void ae350_dcache_flush_addr(unsigned long addr)
{
	ae350_dcache_flush_range(addr, 1);
}

// Unaligned cache line move
static ALWAYS_INLINE void unaligned_cache_line_move(unsigned char* src, unsigned char* dst, unsigned long len)
{
	int i;
	unsigned char* src_p = src;
	unsigned char* dst_p = dst;

	for (i = 0; i < len; ++i)
	{
		*(dst_p+i)=*(src_p+i);
	}
}

// D-Cache invalidate partial line
static void ae350_dcache_invalidate_partial_line(unsigned long start, unsigned long end)
{
	unsigned long line_size = cache_line_size();
	unsigned char buf[line_size];

	unsigned long aligned_start = ROUND_DOWN(start, line_size);
	unsigned long aligned_end   = ROUND_UP(end, line_size);
	unsigned long end_offset    = end & (line_size-1);

	/* Handle cache line unaligned */
	if (aligned_start < start)
	{
		unaligned_cache_line_move((unsigned char*)aligned_start, buf, start - aligned_start);
	}
	if (end < aligned_end)
	{
		unaligned_cache_line_move((unsigned char*)end, buf + end_offset, aligned_end - end);
	}

	ae350_dcache_invalidate_addr(start);

	/* Handle cache line unaligned */
	if (aligned_start < start)
	{
		unaligned_cache_line_move(buf, (unsigned char*)aligned_start, start - aligned_start);
	}
	if (end < aligned_end)
	{
		unaligned_cache_line_move(buf + end_offset, (unsigned char*)end, aligned_end - end);
	}
}

// DMA write back range
void ae350_dma_writeback_range(unsigned long start, unsigned long size)
{
	ae350_dcache_writeback_range(start, size);
}

/*
 * ae350_dma_invalidate_range(start, size)
 *
 * Invalidate D-Cache of specified unaligned region. It's also acceptable to
 * write back D-Cache instead of invalidation at unaligned boundary in region.
 */
void ae350_dma_invalidate_range(unsigned long start, unsigned long size)
{
	unsigned long saved_gie = GIE_SAVE();

	unsigned long line_size = cache_line_size();
	unsigned long end = start + size;
	unsigned long aligned_start = ROUND_UP(start, line_size);
	unsigned long aligned_end   = ROUND_DOWN(end, line_size);

	if (aligned_start > aligned_end)
	{
		ae350_dcache_flush_addr(start);
	}
	else
	{
		if (start < aligned_start)
		{
			ae350_dcache_flush_addr(start);
		}
		if (aligned_start < aligned_end)
		{
			ae350_dcache_invalidate_range(aligned_start, aligned_end - aligned_start);
		}
		if (aligned_end < end)
		{
			ae350_dcache_flush_addr(end);
		}
	}

	GIE_RESTORE(saved_gie);
}

/*
 * ae350_dma_invalidate_range2(start, size)
 *
 * Invalidate D-Cache of specified unaligned region. It's NOT acceptable to
 * write back D-Cache at unaligned boundary in region.
 */
void ae350_dma_invalidate_range2(unsigned long start, unsigned long size)
{
	unsigned long saved_gie = GIE_SAVE();

	unsigned long line_size = cache_line_size();
	unsigned long end = start + size;
	unsigned long aligned_start = ROUND_UP(start, line_size);
	unsigned long aligned_end   = ROUND_DOWN(end, line_size);

	if (aligned_start > aligned_end)
	{
		ae350_dcache_invalidate_partial_line(start, end);
	}
	else
	{
		if (start < aligned_start)
		{
			ae350_dcache_invalidate_partial_line(start, aligned_start);
		}
		if (aligned_start < aligned_end)
		{
			ae350_dcache_invalidate_range(aligned_start, aligned_end - aligned_start);
		}
		if (aligned_end < end)
		{
			ae350_dcache_invalidate_partial_line(aligned_end, end);
		}
	}

	GIE_RESTORE(saved_gie);
}
