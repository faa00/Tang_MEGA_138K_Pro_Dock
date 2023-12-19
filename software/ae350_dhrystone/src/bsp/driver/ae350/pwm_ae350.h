/*
 * ******************************************************************************************
 * File		: pwm_ae350.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: PWM driver definitions
 * ******************************************************************************************
 */

#ifndef __PWM_AE350_H__
#define __PWM_AE350_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_PWM.h"
#include "platform.h"


// Definitions  -----------------------------------------------------------------------------

// Number of PWM channels
#define PWM_NUMBER_OF_CHANNELS           (4)

#define PWM_EXTCLK_FREQ                  (32768)
#define PWM_APBCLK_FREQ                  PCLKFREQ

// PIT Channel Control register
#define PIT_CH_CTRL_PWMPARK_LOW          (0U << 4)
#define PIT_CH_CTRL_PWMPARK_HIGH         (1U << 4)
#define PIT_CH_CTRL_EXTCLK               (0U << 3)
#define PIT_CH_CTRL_APBCLK               (1U << 3)
#define PIT_CH_CTRL_MODE_PWM             (4U << 0)

// PWM MAX duty length
#define PWM_DUTY_MAX                     255

// PWM flags
#define PWM_FLAG_INITIALIZED             (1U << 0)
#define PWM_FLAG_POWERED                 (1U << 1)
#define PWM_FLAG_CHANNEL_ACTIVED(CHN)    (1U << (8 + CHN))

// PWM Information (Run-Time)
typedef struct _PWM_INFO
{
	AE350_PWM_SignalEvent_t   	cb_event;      						// Event callback
	uint32_t                	flags;         						// PWM driver flags
	uint32_t                	clk[PWM_NUMBER_OF_CHANNELS];
	uint32_t                	freq[PWM_NUMBER_OF_CHANNELS];       // PWM channel frequency
	uint32_t                	duty[PWM_NUMBER_OF_CHANNELS];       // PWM channel duty cycle (0 ~ 255)
} PWM_INFO;

// PWM Resources definitions
typedef struct
{
	AE350_PWM_CAPABILITIES    capabilities;  // Capabilities
	PIT_RegDef				  *reg;          // Pointer to PIT register
	PWM_INFO                  *info;         // Run-Time Information
} const PWM_RESOURCES;


#endif /* __PWM_AE350_H__ */
