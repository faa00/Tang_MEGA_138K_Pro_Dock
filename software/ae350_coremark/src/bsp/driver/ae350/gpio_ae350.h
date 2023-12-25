/*
 * ******************************************************************************************
 * File		: gpio_ae350.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: GPIO driver definitions
 * ******************************************************************************************
 */

#ifndef __GPIO_AE350_H__
#define __GPIO_AE350_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_GPIO.h"
#include "platform.h"


// Definitions  -----------------------------------------------------------------------------

// GPIO interrupt mode
#define GPIO_INTER_NONE				0x0			// None
#define GPIO_INTR_HIGH_LEVEL        0x2			// High level
#define GPIO_INTR_LOW_LEVEL         0x3			// Low level
#define GPIO_INTR_NEGATIVE_EDGE     0x5			// Negative edge
#define GPIO_INTR_POSITIVE_EDGE     0x6			// Positive edge
#define GPIO_INTR_DUAL_EDGE         0x7			// Dual edge

// GPIO pin number
#define GPIO_MAX_PIN_NUM            32


// GPIO information
typedef struct _GPIO_INFO
{
	AE350_GPIO_SignalEvent_t cb_event;  		// Event callback
} GPIO_INFO;


#endif /* __GPIO_AE350_H__ */
