/*
 * ******************************************************************************************
 * File		: demo.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: AE350 drivers demo
 * ******************************************************************************************
 */

#ifndef __DEMO_H__
#define __DEMO_H__


#ifdef __cplusplus
extern "C" {
#endif

#define UART_SPEED	115200

// Definitions ------------------------------------------------------------------------------
// Control demo
// External peripherals demo
#define RUN_DEMO_GPIO			0	// Run GPIO demo
#define RUN_DEMO_UART			1	// Run UART demo
#define RUN_DEMO_I2C			0	// Run I2C demo
#define RUN_DEMO_PWM			0	// Run PWM demo
#define RUN_DEMO_RTC			0	// Run RTC demo
#define RUN_DEMO_SPI			0	// Run SPI demo
#define RUN_DEMO_WDT_PIT		0	// Run WDT demo and PIT as simple timer

// SoC feature demo
#define RUN_DEMO_PLMT			0	// Run PLMT demo
#define RUN_DEMO_PRINTF			0	// Run overwrite printf() demo
#define RUN_DEMO_SCANF			0	// Run Overwrite scanf() demo
#define RUN_DEMO_HSP            	0   // Run hardware stack protection demo
#define RUN_DEMO_PFM            	0   // Run hardware performance monitor demo
#define RUN_DEMO_PLIC			0	// Run platform level interrupt controller demo
#define RUN_DEMO_POWERBRAKE     	0   // Run hardware performance throttling mechanism demo
#define RUN_DEMO_WFI			0	// Run wait for interrupt demo
#define RUN_DEMO_CACHE			0	// Run L1 cache demo
#define RUN_DEMO_CACHE_LOCK		0	// Run L1 cache lock demo
#define RUN_DEMO_IDLM			0	// Run access ILM/DLM demo
#define RUN_DEMO_MM			0	// Run memory management demo
#define RUN_DEMO_INTR			0	// Run multiple peripherals interrupts demo

// Board feature demo
#define RUN_DEMO_LED			1	// Run waterfall led demo


// Declarations -----------------------------------------------------------------------------

// GPIO demo
#if RUN_DEMO_GPIO
int demo_gpio(void);
#endif

// UART demo
#if RUN_DEMO_UART
int demo_uart(void);
#endif

// I2C demo
#if RUN_DEMO_I2C
int demo_i2c(void);
#endif

// PWM demo
#if RUN_DEMO_PWM
int demo_pwm(void);
#endif

// RTC demo
#if RUN_DEMO_RTC
int demo_rtc(void);
#endif

// SPI demo
#if RUN_DEMO_SPI
int demo_spi(void);
#endif

// WDT and PIT as simple timer demo
#if RUN_DEMO_WDT_PIT
int demo_wdt_and_pit(void);
#endif

// Machine Timer demo
#if RUN_DEMO_PLMT
int demo_plmt(void);
#endif

// Overwrite printf() demo
#if RUN_DEMO_PRINTF
int demo_printf(void);
#endif

// Overwrite scanf() demo
#if RUN_DEMO_SCANF
int demo_scanf(void);
#endif

// Hardware stack protection demo
#if RUN_DEMO_HSP
int demo_hsp(void);
#endif

// Hardware performance monitor demo
#if RUN_DEMO_PFM
int demo_pfm(void);
#endif

// Platform level interrupt controller demo
#if RUN_DEMO_PLIC
int demo_plic(void);
#endif

// Hardware performance throttling mechanism demo
#if RUN_DEMO_POWERBRAKE
int demo_powerbrake(void);
#endif

// Wait for interrupt demo
#if RUN_DEMO_WFI
int demo_wfi(void);
#endif

// L1 cache demo
#if RUN_DEMO_CACHE
int demo_cache(void);
#endif

// L1 cache lock demo
#if RUN_DEMO_CACHE_LOCK
int demo_cache_lock(void);
#endif

// Access ILM/DLM demo
#if RUN_DEMO_IDLM
int demo_idlm(void);
#endif

// Memory management demo
#if RUN_DEMO_MM
int demo_mm(void);
#endif

// Multiple peripherals interrupts demo
#if RUN_DEMO_INTR
int demo_intr(void);
#endif

// Waterfall led demo
#if RUN_DEMO_LED
int demo_led(void);
#endif


#ifdef __cplusplus
}
#endif


#endif	/* __DEMO_H__ */
