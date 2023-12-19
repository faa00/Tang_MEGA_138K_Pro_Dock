/*
 * ******************************************************************************************
 * File		: define.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Definitions
 * ******************************************************************************************
 */

#ifndef __DEFINE_H__
#define __DEFINE_H__


// Includes ---------------------------------------------------------------------------------
#include "platform.h"


// Definitions ------------------------------------------------------------------------------

#define CPU_MHz						(CPUFREQ/MHz)

// The number of Dhrystone iterations
#ifdef CFG_SIMU
#define LOOP_COUNT					20000
#else
#define LOOP_COUNT					2000000
#endif

#define PLATFORM_TIMER				1

#define Mini_secs_Per_Second        1000
#define Micro_secs_Per_Second       1000000
#define Mini_secs_to_Micro_Second   1000


#endif	/* __DEFINE_H__ */
