/*
 * ******************************************************************************************
 * File		: define.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: CoreMark predefine
 * ******************************************************************************************
 */

#ifndef __DEFINE_H__
#define __DEFINE_H__

/* Includes: */
#include "platform.h"


/* Definitions: */

#define CPU_MHz            (CPUFREQ/MHz)

// It's suggested to set ITERATIONS as 3500 to run benchmark on real board
// To speed up CoreMark simulation, the ITERATIONS could be set as 350
#ifndef  CFG_SIMU
#ifdef CFG_CACHE_ENABLE
#define ITERATIONS 35000	// With cache
#else
#define ITERATIONS 3500		// No cache
#endif
#else
#define ITERATIONS 350		// Simulation
#endif


#endif	/* __DEFINE_H__ */
