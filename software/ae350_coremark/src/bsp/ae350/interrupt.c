/*
 * ******************************************************************************************
 * File		: interrupt.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Platform level external interrupt handler
 * ******************************************************************************************
 */

// No Vectored PLIC

// Includes ---------------------------------------------------------------------------------
#include <stdio.h>
#include "platform.h"


// Definitions ------------------------------------------------------------------------------

// Interrupt handler function pointer
typedef void (*isr_func)(void);

// Default interrupt handler functions
void default_irq_handler(void)
{
	printf("Default interrupt handler\r\n");
}


// Interrupt handler function declarations
void rtc_period_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));	// RTC period interrupt
void rtc_alarm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));	// RTC alarm interrupt
void pit_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// PIT interrupt
void gp0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// GP0 interrupt
void spi_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// SPI interrupt
void i2c_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// I2C interrupt
void gpio_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// GPIO interrupt
void uart1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// UART1 interrupt
void uart2_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// UART2 interrupt
void dma_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// DMA interrupt
void gp1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// GP1 interrupt
void gp2_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// GP2 interrupt
void gp3_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// GP3 interrupt
void gp4_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// GP4 interrupt
void gp5_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// GP5 interrupt
void gp6_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// GP6 interrupt
void gp7_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// GP7 interrupt
void gp8_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// GP8 interrupt
void gp9_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));			// GP9 interrupt
void gp10_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// GP10 interrupt
void gp11_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// GP11 interrupt
void gp12_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// GP12 interrupt
void gp13_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// GP13 interrupt
void gp14_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// GP14 interrupt
void gp15_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// GP15 interrupt
void standby_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// SMU2Core standby interrupt
void wakeup_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));		// SMU2Core wake up interrupt


// Interrupt handler functions entry
const isr_func irq_handler[] =
{
	default_irq_handler,		// 0
	rtc_period_irq_handler,		// 1
	rtc_alarm_irq_handler,		// 2
	pit_irq_handler,			// 3
	gp0_irq_handler,			// 4
	spi_irq_handler,			// 5
	i2c_irq_handler,			// 6
	gpio_irq_handler,			// 7
	uart1_irq_handler,			// 8
	uart2_irq_handler,			// 9
	dma_irq_handler,			// 10
	gp1_irq_handler,			// 11
	gp2_irq_handler,			// 12
	gp3_irq_handler,			// 13
	gp4_irq_handler,			// 14
	gp5_irq_handler,			// 15
	gp6_irq_handler,			// 16
	gp7_irq_handler,			// 17
	gp8_irq_handler,			// 18
	gp9_irq_handler,			// 19
	gp10_irq_handler,			// 20
	gp11_irq_handler,			// 21
	gp12_irq_handler,			// 22
	gp13_irq_handler,			// 23
	gp14_irq_handler,			// 24
	gp15_irq_handler,			// 25
	standby_irq_handler,		// 26
	wakeup_irq_handler,			// 27
	default_irq_handler,		// 28
	default_irq_handler,		// 29
	default_irq_handler,		// 30
	default_irq_handler			// 31
};


// Machine external interrupt handler
void mext_interrupt(unsigned int irq_source)
{
	/* Enable interrupts in general to allow nested */
	HAL_MIE_ENABLE();

	/* Do interrupt handler */
	irq_handler[irq_source]();

	__nds__plic_complete_interrupt(irq_source);

	/* Disable interrupt in general to restore context */
	HAL_MIE_DISABLE();
}
