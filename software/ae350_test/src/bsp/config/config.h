/*
 * ******************************************************************************************
 * File		: config.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Configurations
 * ******************************************************************************************
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

// Definitions -------------------------------------------------------------------------------

// Users can configure the defines in this area
// to match different environment settings


/* -------------------------------- AE350 core configurations -------------------------------- */

// Simulation select
//#define CFG_SIMU		// Do simulation on SID

// Code coverage select
//#define CFG_GCOV		// Do code coverage support

// L1 cache select
#define CFG_CACHE_ENABLE

// Build mode select
// The BUILD_MODE can be specified to BUILD_XIP/BUILD_BURN/BUILD_LOAD only.
/*
 * BUILD_LOAD : ae350-ilm.sag	: debugging in ILM
 *            : ae350-ddr.sag	: debugging in DDR memory
 * BUILD_BURN : ae350-ddr.sag	: booting in Flash memory and running in DDR memory
 * BUILD_XIP  : ae350-xip.sag	: booting and running in Flash memory
 *
 */
#define BUILD_MODE	BUILD_BURN


// The following is predefined settings
// Please do not modify them
#define	BUILD_LOAD	1		// The program is loaded by GDB or eBIOS
#define	BUILD_BURN	2		// The program is burned to the flash, but run in RAM
#define	BUILD_XIP	3		// The program is burned to the flash, and run in flash (xip linker script files must be used)

#if BUILD_MODE == BUILD_LOAD
#define CFG_LOAD
#endif
#if BUILD_MODE == BUILD_BURN
#define CFG_BURN
#endif
#if BUILD_MODE == BUILD_XIP
#define CFG_XIP
#endif




/* -------------------------------- AE350 subsystem configurations -------------------------------- */

/* GPIO (General purpose input and output ) */
// Configuration settings for Driver GPIO
#define DRV_GPIO						1		// Enable/disable GPIO	1: enable; 0: disable

/* I2C (Inter-integrated circuit interface) */
// Configuration settings for Driver I2C
#define DRV_I2C                         1		// Enable/disable I2C	1: enable; 0: disable
// I2C with DMA
// TX
#define DRV_I2C_DMA_TX_EN               0		// Enable/disable I2C DMA TX	1: enable; 0: disable
#define DRV_I2C_DMA_TX_CH               2		// DMA channel number of I2C TX
#define DRV_I2C_DMA_TX_REQID            8		// DMA request id of I2C TX
// RX
#define DRV_I2C_DMA_RX_EN               0		// Enable/disable I2C DMA RX	1: enable; 0: disable
#define DRV_I2C_DMA_RX_CH               3		// DMA channel number of I2C RX
#define DRV_I2C_DMA_RX_REQID            8		// DMA request id of I2C RX

/* UART1 (Universal asynchronous receiver transmitter) */
// Configuration settings for Driver UART1
#define DRV_UART1                       1		// Enable/disable UART1	1: enable; 0: disable
// UART1 with DMA
// TX
#define DRV_UART1_DMA_TX_EN             1		// Enable/disable UART1 DMA TX	1: enable; 0: disable
#define DRV_UART1_DMA_TX_CH             0		// DMA channel number of UART1 TX
#define DRV_UART1_DMA_TX_REQID          4		// DMA request id of UART1 TX
// RX
#define DRV_UART1_DMA_RX_EN             1		// Enable/disable UART1 DMA RX	1: enable; 0: disable
#define DRV_UART1_DMA_RX_CH             1		// DMA channel number of UART1 RX
#define DRV_UART1_DMA_RX_REQID          5		// DMA request id of UART1 RX

/* UART2 (Universal asynchronous receiver transmitter) */
// Configuration settings for Driver UART2
#define DRV_UART2                       1		// Enable/disable UART2	1: enable; 0: disable
// UART2 with DMA
// TX
#define DRV_UART2_DMA_TX_EN             1		// Enable/disable UART2 DMA TX	1: enable; 0: disable
#define DRV_UART2_DMA_TX_CH             0		// DMA channel number of UART2 TX
#define DRV_UART2_DMA_TX_REQID          6		// DMA request id of UART2 TX
// RX
#define DRV_UART2_DMA_RX_EN             1		// Enable/disable UART2 DMA RX	1: enable; 0: disable
#define DRV_UART2_DMA_RX_CH             1		// DMA channel number of UART2 RX
#define DRV_UART2_DMA_RX_REQID          7		// DMA request id of UART2 RX

/* SPI (Serial peripheral interface) */
// Configuration settings for Driver SPI
#define DRV_SPI                         1		// Enable/disable SPI	1: enable; 0: disable
// SPI with DMA
// TX
#define DRV_SPI_DMA_TX_EN               0		// Enable/disable SPI DMA TX	1: enable; 0: disable
#define DRV_SPI_DMA_TX_CH               0		// DMA channel number of SPI TX
#define DRV_SPI_DMA_TX_REQID            2		// DMA request id of SPI TX
// RX
#define DRV_SPI_DMA_RX_EN               0		// Enable/disable SPI DMA RX	1: enable; 0: disable
#define DRV_SPI_DMA_RX_CH               1		// DMA channel number of SPI RX
#define DRV_SPI_DMA_RX_REQID            3		// DMA request id of SPI RX

/* PWM (Pulse width modulator) */
// Configuration settings for Driver PWM
#define DRV_PWM                         1		// Enable/disable PWM	1: enable; 0: disable

/* RTC (Real-time clock) */
// Configuration settings for Driver RTC
#define DRV_RTC				            1		// Enable/disable RTC	1: enable; 0: disable

/* WDT (Watch dog timer) */
// Configuration settings for Driver WDT
#define DRV_WDT                         1		// Enable/disable WDT	1: enable; 0: disable

/* PIT as Simple Timer */
// Configuration settings for Driver PIT as Simple Timer
#define DRV_PIT							1		// Enable/disable PIT as Simple Timer	1: enable; 0: disable


#endif // __CONFIG_H__
