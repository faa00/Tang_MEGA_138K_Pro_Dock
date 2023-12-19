/*
 * ******************************************************************************************
 * File		: main.c
 * Author 	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Main functions
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"


// Definitions ------------------------------------------------------------------------------
int main(void)
{
	//
	// User application codes
	//

	// Run UART demo
#if RUN_DEMO_UART
	demo_uart();
#endif

	// Run GPIO demo
#if RUN_DEMO_GPIO
	demo_gpio();
#endif

	// Run I2C demo
#if RUN_DEMO_I2C
	demo_i2c();
#endif

	// Run PWM demo
#if RUN_DEMO_PWM
	demo_pwm();
#endif

	// Run RTC demo
#if RUN_DEMO_RTC
	demo_rtc();
#endif

	// Run SPI demo
#if RUN_DEMO_SPI
	demo_spi();
#endif

	// Run WDT and PIT as simple timer demo
#if RUN_DEMO_WDT_PIT
	demo_wdt_and_pit();
#endif

	// Run Machine Timer demo
#if RUN_DEMO_PLMT
	demo_plmt();
#endif

	// Run overwrite printf() demo
#if RUN_DEMO_PRINTF
	demo_printf();
#endif

	// Run overwrite scanf() demo
#if RUN_DEMO_SCANF
	demo_scanf();
#endif

	// Run hardware stack protection demo
#if RUN_DEMO_HSP
	demo_hsp();
#endif

	// Run hardware performance monitor demo
#if RUN_DEMO_PFM
	demo_pfm();
#endif

	// Run platform level interrupt controller demo
#if RUN_DEMO_PLIC
	demo_plic();
#endif

	// Run hardware performance throttling mechanism demo
#if RUN_DEMO_POWERBRAKE
	demo_powerbrake();
#endif

	// Run wait for interrupt demo
#if RUN_DEMO_WFI
	demo_wfi();
#endif

	// Run L1 cache demo
#if RUN_DEMO_CACHE
	demo_cache();
#endif

	// Run L1 cache lock demo
#if RUN_DEMO_CACHE_LOCK
	demo_cache_lock();
#endif

	// Run access ILM/DLM demo
#if RUN_DEMO_IDLM
	demo_idlm();
#endif

	// Run memory management demo
#if RUN_DEMO_MM
	demo_mm();
#endif

	// Run multiple peripherals interrupts demo
#if RUN_DEMO_INTR
	demo_intr();
#endif

    // Run waterfall led demo
#if RUN_DEMO_LED
    demo_led();
#endif

	//
	// User application codes
    //

	return 0;
}
