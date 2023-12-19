/*
 * ******************************************************************************************
 * File		: define.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Whetstone predefine.
 * ******************************************************************************************
 */

#ifndef __DEFINE_H__
#define __DEFINE_H__

/* Includes: */
#include "platform.h"


/* Definitions: */

#define CPU_MHz            (CPUFREQ/MHz)


/* Predefined macro __riscv_flen is equal to:
 * 64: if v5d toolchain is used
 * 32: if v5f toolchain is used
 */
#if __riscv_flen == 32
	#define SPDP float
	#define Precision "Single"

	#define atan	atanf
	#define sin 	sinf
	#define cos 	cosf
	#define sqrt	sqrtf
	#define exp 	expf
	#define log 	logf
#else // __riscv_flen == 64
    #define SPDP double
    #define Precision "Double"
#endif


#endif	/* __DEFINE_H__ */
