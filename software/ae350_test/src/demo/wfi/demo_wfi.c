/*
 * ******************************************************************************************
 * File		: demo_wfi.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Wait for interrupt demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows how to let CPU core enter standby and wake up.
 *
 * Scenario:
 *
 * The CPU core enter into standby mode as execute WFI instruction. The CPU core
 * wake up as push GPIO buttons SW1 ~ SW3 and print UART message "Wake up from standby mode".
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running wait for interrupt demo
#if RUN_DEMO_WFI

// ************ Includes ************ //
#include "Driver_GPIO.h"
#include "platform.h"
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********** //

// On the DK-START-GW5AT138 V2.0
// Key: GPIO_5/4/3
#define GPIO_USED_MASK       0x38    	// Which GPIOs to input

extern AE350_DRIVER_GPIO Driver_GPIO;	// GPIO


// GPIO callback
void gpio_callback(uint32_t event)
{
	// Don't do anything.
	switch (event)
	{
		default:
			break;
	}
}

// Setup GPIO
void setup_gpio(void)
{
	AE350_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;

	// Initializes GPIO
	GPIO_Dri->Initialize(gpio_callback);

	// Set GPIO direction
	GPIO_Dri->SetDir(GPIO_USED_MASK, AE350_GPIO_DIR_INPUT);		// SW1 ~ SW3 input

	// Set switches interrupt mode to negative edge and enable it
	GPIO_Dri->Control(AE350_GPIO_SET_INTR_NEGATIVE_EDGE | AE350_GPIO_INTR_ENABLE, GPIO_USED_MASK, 0, 0);

	// Set the interrupt level 1 for the GPIO
	HAL_INTERRUPT_SET_LEVEL(IRQ_GPIO_SOURCE, 1);

	// Enable interrupt GPIO source
	HAL_INTERRUPT_ENABLE(IRQ_GPIO_SOURCE);
}

// Application entry function
int demo_wfi(void)
{
	// Initializes UART
	uart_init(38400);		// Baud rate is 38400

	printf("\r\nIt's a Wait-for-Interrupt demo.\r\n\r\n");

	/*
	 * Set interrupt level threshold to 0.
	 * Interrupt level 0 corresponds to regular execution (no interrupt) outside of an interrupt handler.
	 * Levels 1 - 255 correspond to interrupt handler levels.
	 */
	HAL_INTERRUPT_THRESHOLD(0);

	/* Disable global interrupt for core to execute next
	 * instruction of standby when wake up */
	HAL_MIE_DISABLE();

	/* Disable the machine PLIC interrupts until setup is done */
	HAL_MEIP_DISABLE();

	/* Setup external GPIO input interrupt to wake up from standby */
	setup_gpio();

	/* Enable the Machine-External bit in MIE */
	HAL_MEIP_ENABLE();

	/* Enter standby mode */
	printf("Entering StandBy mode.....................\r\n");
	printf("And press SW1 ~ SW3 trying to wake it up zzzzzzzzzzzzzzzzzzzzzzzzzZZZZZZZZZzzzzzzz...\r\n");

	// Execute WFI instruction to wake up
	asm ("wfi");

	printf("Wake up from standby mode..................\r\n");

	return 0;
}

#endif	/* RUN_DEMO_WFI */
