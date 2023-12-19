/*
 * ******************************************************************************************
 * File		: cache.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: L1 cache
 * ******************************************************************************************
 */

#ifndef __CACHE_H__
#define __CACHE_H__


// Includes ----------------------------------------------------------------------------------
#include "core_v5.h"


// Definitions -------------------------------------------------------------------------------

// L1 cache information
struct _cache_info
{
	unsigned char is_init;				// Initialized flag
	unsigned long cacheline_size;		// L1 cache line size
};

extern struct _cache_info cache_info;

extern void get_cache_info(void);

// Get L1 cache line size
static inline __attribute__((always_inline)) unsigned long cache_line_size()
{
	if (!cache_info.is_init)
	{
		get_cache_info();
	}

	return cache_info.cacheline_size;
}


/* L1 cache operations */
// I-Cache
extern void ae350_icache_invalidate_range(unsigned long start, unsigned long size);
extern void ae350_icache_invalidate_all(void);

// D-Cache
extern void ae350_dcache_writeback_range(unsigned long start, unsigned long size);
extern void ae350_dcache_invalidate_range(unsigned long start, unsigned long size);
extern void ae350_dcache_flush_range(unsigned long start, unsigned long size);
extern void ae350_dcache_flush_all(void);

/* DMA-specific operations */
extern void ae350_dma_writeback_range(unsigned long start, unsigned long size);
extern void ae350_dma_invalidate_range(unsigned long start, unsigned long size);
extern void ae350_dma_invalidate_range2(unsigned long start, unsigned long size);


#endif /* __CACHE_H__ */
