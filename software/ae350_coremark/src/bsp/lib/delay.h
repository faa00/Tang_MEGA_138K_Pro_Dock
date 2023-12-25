/*
 * ******************************************************************************************
 * File		: delay.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Delay predefine
 * ******************************************************************************************
 */

#ifndef __DELAY_H__
#define __DELAY_H__


// Declarations ------------------------------------------------------------------------------

extern void simple_delay_ms(unsigned int delay_ms);		// Simple delay microsecond
extern unsigned int get_timer_freq(void);
extern unsigned long long get_timer_value(void);


#endif	/* __DELAY_H__ */
