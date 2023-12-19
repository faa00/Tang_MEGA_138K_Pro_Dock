/*
 * ******************************************************************************************
 * File		: platform.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Platform device definitions
 * ******************************************************************************************
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__


#ifdef __cplusplus
extern "C" {
#endif


// Includes ---------------------------------------------------------------------------------
#include "config.h"
#include "core_v5.h"
#include "ae350.h"
#include "interrupt.h"


// Definitions ------------------------------------------------------------------------------

/*****************************************************************************
 * Peripheral device HAL declaration
 ****************************************************************************/
#define DEV_PLMT             AE350_PLMT		// Machine Timer
#define DEV_DMA              AE350_DMA		// DMAC
#define DEV_SMU              AE350_SMU		// SMU
#define DEV_UART1            AE350_UART1	// UART1
#define DEV_UART2            AE350_UART2	// UART2
#define DEV_PIT              AE350_PIT		// PIT
#define DEV_WDT              AE350_WDT		// WDT
#define DEV_RTC              AE350_RTC		// RTC
#define DEV_GPIO             AE350_GPIO		// GPIO
#define DEV_I2C              AE350_I2C		// I2C
#define DEV_SPI              AE350_SPI		// SPI


#ifdef __cplusplus
}
#endif


#endif	/* __PLATFORM_H__ */
