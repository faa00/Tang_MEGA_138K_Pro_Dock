/*
 * ******************************************************************************************
 * File		: Driver_SMU.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: SMU driver definitions
 * ******************************************************************************************
 */

#ifndef __DRIVER_SMU_H__
#define __DRIVER_SMU_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_Common.h"


// Definitions  -----------------------------------------------------------------------------

// System ID and Revision Register (SYSTEMVER)
#define AE350_SYSTEMVER_DEFAULT		        0x41453500		// Default system ID and revision

// Board ID Register (BOARDID)
#define AE350_BOARDID_DEFAULT		        0x0174b010		// Default board ID

// Wake up and Reset Status Register (WRSR)
// W1C: write 1 to clear
#define AE350_SMU_WRSR_DBG		            (1UL << 10)		// Wake up by debug requests
#define AE350_SMU_WRSR_ALM		            (1UL << 9 )		// Wake up by RTC alarm events
#define AE350_SMU_WRSR_EXT		            (1UL << 8 )		// Wake up by external events
#define AE350_SMU_WRSR_SW		            (1UL << 4 )		// Software reset
#define AE350_SMU_WRSR_WDT		            (1UL << 3 )		// Watch dog reset
#define AE350_SMU_WRSR_HW		            (1UL << 2 )		// Hardware reset
#define AE350_SMU_WRSR_MPOR		            (1UL << 1 )		// MPD power on reset
#define AE350_SMU_WRSR_APOR		            (1UL << 0 )		// AOPD power on reset

// SMU Command Register (SMUCR)
// SMU command
#define AE350_SMU_SMUCR_SOFT_RESET      	0x3c			// Software reset to reset the whole system
#define AE350_SMU_SMUCR_STANDBY		        0x55			// Standby command that triggers the standby request to the processor
#define AE350_SMU_SMUCR_POWER_OFF	        0x5a			// Power off the main power domain

// Wake up and Reset Mask Register (WRMASK)
#define AE350_SMU_WRMASK_WIMASK		        (1UL << 8 )		// Whether external events will trigger wake up
#define AE350_SMU_WRMASK_ALMMASK	        (1UL << 9 )		// Whether RTC events will trigger wake up
#define AE350_SMU_WRMASK_DBGMASK	        (1UL << 10)		// Whether debug requests will trigger wake up

// Clock Enable Register (CER)
#define AE350_SMU_CER_PIT		            (1UL << 10)		// PIT clock enable
#define AE350_SMU_CER_WDT		            (1UL << 9 )		// WDT clock enable
#define AE350_SMU_CER_IIC		            (1UL << 8 )		// I2C clock enable
#define AE350_SMU_CER_GPIO		            (1UL << 7 )		// GPIO clock enable
#define AE350_SMU_CER_SPI		            (1UL << 6 )		// SPI clock enable
#define AE350_SMU_CER_UART2		            (1UL << 4 )		// UART2 clock enable
#define AE350_SMU_CER_UART1		            (1UL << 3 )		// UART1 clock enable
#define AE350_SMU_CER_PCLK		            (1UL << 2 )		// Main APB bus clock enable
#define AE350_SMU_CER_HCLK		            (1UL << 1 )		// AHB bus clock enable
#define AE350_SMU_CER_CORE		            (1UL << 0 )		// Processor clock enable

// Clock Ratio Register (CRR)
#define AE350_SMU_CRR_CCLKSEL_OSCH			(0x0)	        // OSCH
#define AE350_SMU_CRR_CCLKSEL_OSCH_DIV2		(0x1)	        // Divide OSCH by 2
#define AE350_SMU_CRR_HPCLKSEL_1_1_1_1		(0x0)	        // 1:1:1:1
#define AE350_SMU_CRR_HPCLKSEL_1_1_1_1D2	(0x1)	        // 1:1:1:1/2
#define AE350_SMU_CRR_HPCLKSEL_1_1_1_1D4	(0x2)	        // 1:1:1:1/4
#define AE350_SMU_CRR_HPCLKSEL_1_1_1D2_1D2	(0x3)	        // 1:1:1/2:1/2
#define AE350_SMU_CRR_HPCLKSEL_1_1_1D2_1D4	(0x4)	        // 1:1:1/2:1/4

// Power Control Register (PCR)
#define AE350_SMU_PCR_DVFS			        (0x1)	        // Dynamic voltage frequency scaling
#define AE350_SMU_PCR_LIGHTSLEEP	        (0x2)	        // Light sleep
#define AE350_SMU_PCR_DEEPSLEEP		        (0x3)	        // Deep sleep 

// Power Interrupt Register (PIR)
#define AE350_SMU_PIR_EN			        (0x1)	        // Enable power interrupt


#endif      /* __DRIVER_SMU_H__ */
