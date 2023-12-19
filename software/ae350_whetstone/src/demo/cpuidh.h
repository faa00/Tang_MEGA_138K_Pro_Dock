/*
 * ******************************************************************************************
 * File		: cpuidh.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Whetstone timer.
 * ******************************************************************************************
 */

#ifndef __CPUIDH_H__
#define __CPUIDH_H__

/* Includes: */
#include "define.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Declarations: */

extern volatile SPDP  theseSecs;
extern volatile SPDP  startSecs;
extern volatile SPDP  secs;

extern void start_time(void);
extern void end_time(void);
extern SPDP time(void);


#ifdef __cplusplus
};
#endif


#endif	/* __CPUIDH_H__ */
