/*
 * ******************************************************************************************
 * File		: core_portme.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: CoreMark timer
 * ******************************************************************************************
 */

/* Includes:*/
#include <stdio.h>
#include <stdlib.h>

#include "coremark.h"
#include "define.h"
#include "delay.h"


/* Definitions: */

#if VALIDATION_RUN
volatile ee_s32 seed1_volatile=0x3415;
volatile ee_s32 seed2_volatile=0x3415;
volatile ee_s32 seed3_volatile=0x66;
#endif

#if PERFORMANCE_RUN
volatile ee_s32 seed1_volatile=0x0;
volatile ee_s32 seed2_volatile=0x0;
volatile ee_s32 seed3_volatile=0x66;
#endif

#if PROFILE_RUN
volatile ee_s32 seed1_volatile=0x8;
volatile ee_s32 seed2_volatile=0x8;
volatile ee_s32 seed3_volatile=0x8;
#endif

volatile ee_s32 seed4_volatile=ITERATIONS;
volatile ee_s32 seed5_volatile=0;

static CORE_TICKS t0, t1;


// Start time
void start_time(void)
{
	printf("\r\nThe time is from mcycle\r\n");

	t0 = get_timer_value();
}

// Stop time
void stop_time(void)
{
	t1 = get_timer_value();
}

CORE_TICKS get_time(void)
{
	return (CORE_TICKS)t1 - t0;
}

secs_ret time_in_secs(CORE_TICKS ticks)
{
	secs_ret delta = (secs_ret)ticks;
	secs_ret freq = (secs_ret)get_timer_freq();
	secs_ret val=delta / freq;

	return val;
}
