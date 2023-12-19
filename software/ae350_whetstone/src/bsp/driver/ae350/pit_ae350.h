/*
 * ******************************************************************************************
 * File		: pit_ae350.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: PIT as simple timer driver definitions
 * ******************************************************************************************
 */

#ifndef __PIT_AE350_H__
#define __PIT_AE350_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_PIT.h"
#include "platform.h"


// Definitions  -----------------------------------------------------------------------------

// Number of PIT as simple timer channels
#define PIT_NUMBER_OF_CHANNELS          (4)

#define PIT_CHNCTRL_CLK_EXTERNAL        (0 << 3)
#define PIT_CHNCTRL_CLK_PCLK            (1 << 3)
#define PIT_CHNCTRL_MODEMASK            0x07
#define PIT_CHNCTRL_TMR_32BIT           1
#define PIT_CHNCTRL_TMR_16BIT           2
#define PIT_CHNCTRL_TMR_8BIT            3
#define PIT_CHNCTRL_PWM                 4
#define PIT_CHNCTRL_MIXED_16BIT         6
#define PIT_CHNCTRL_MIXED_8BIT          7


#endif		/* __PIT_AE350_H__ */
