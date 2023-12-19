/*
 * ******************************************************************************************
 * File		: Driver_PIT.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: PIT as simple timer driver definitions
 * ******************************************************************************************
 */

#ifndef __DRIVER_PIT_H__
#define __DRIVER_PIT_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_Common.h"


// Definitions ------------------------------------------------------------------------------

#define AE350_PIT_API_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,02)  /* API version */

// Interrupt handler
// Use PIT interrupt handler as PIT as simple timer interrupt handler
#define pit_timer_irq_handler pit_irq_handler

// PIT as simple timer control command
#define AE350_PIT_TIMER_START			(1UL << 0)		// Start timer
#define AE350_PIT_TIMER_STOP			(1UL << 1)		// Stop timer
#define AE350_PIT_TIMER_INTR_ENABLE		(1UL << 2)		// Enable timer interrupt
#define AE350_PIT_TIMER_INTR_DISABLE	(1UL << 3)		// Disable timer interrupt
#define AE350_PIT_TIMER_INTR_CLEAR		(1UL << 4)		// Clear timer interrupt

#define	AE350_PIT_TIMER_SEC_TICK		(1UL << 0)		// Second tick
#define AE350_PIT_TIMER_MSEC_TICK		(1UL << 1)		// Microsecond tick


// Access structure of the PIT as simple timer driver
typedef struct _AE350_DRIVER_PIT
{
	AE350_DRIVER_VERSION     (*GetVersion)     		(void);								// Get version
	int32_t                  (*Initialize)     		(void);								// Initializes
	uint32_t				 (*Read)				(uint32_t tmr);						// Read time
	int32_t                  (*Control)        		(uint32_t mode, uint32_t tmr);		// Control
	int32_t					 (*SetPeriod)			(uint32_t tmr, uint32_t period);	// Set period
	uint32_t				 (*GetStatus)			(uint32_t tmr);						// Get status
	uint32_t				 (*GetTick)				(uint32_t mode, uint32_t sec);		// Get (m)s tick
} const AE350_DRIVER_PIT;


#endif		/* __DRIVER_PIT_H__ */
