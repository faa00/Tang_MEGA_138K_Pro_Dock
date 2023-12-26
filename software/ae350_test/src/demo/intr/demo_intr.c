/*
 * ******************************************************************************************
 * File		: demo_intr.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Multiple peripherals interrupts demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows how to use multiple peripherals interrupts.
 *
 * Scenario:
 *
 * The main function will enable the GPIO buttons and setup PIT timer. The PIT timer ISR is
 * triggered every 4 seconds. When GPIO buttons pressed, the GPIO ISR will display some
 * messages.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running multiple peripherals interrupts demo
#if RUN_DEMO_INTR

// ************ Includes ************ //
#include "Driver_PIT.h"
#include "Driver_GPIO.h"
#include "platform.h"
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********** //

extern AE350_DRIVER_PIT	Driver_PIT;		// PIT timer
extern AE350_DRIVER_GPIO Driver_GPIO;	// GPIO


#define	GPIO_INT_PRIORITY	1	// GPIO interrupt priority
#define	PIT_INT_PRIORITY	2	// PIT interrupt priority

#define PIT_TIMER_PERIOD	(4 * OSCFREQ)	// 4 secs

#define PIT_TIMER0	0	// PIT timer 0
#define PIT_TIMER1	1	// PIT timer 1
#define PIT_TIMER2	2	// PIT timer 2
#define PIT_TIMER3	3	// PIT timer 3

// On the DK-START-GW5AT138 V2.0
// Key: GPIO_3
#define GPIO_USED_MASK	0x8	// SW1


/* Declarations of GPIO and PIT */
static void setup_pit_timer(unsigned int period);
static void setup_gpio(void);


// Setup PIT timer
static void setup_pit_timer(unsigned int period)
{
	AE350_DRIVER_PIT *DrvPIT = &Driver_PIT;

	/* Setup system PIT timer */

	// Initializes PIT
	DrvPIT->Initialize();

	// Set period
	DrvPIT->SetPeriod(PIT_TIMER0, period);

	// Enable interrupt
	DrvPIT->Control(AE350_PIT_TIMER_INTR_ENABLE, PIT_TIMER0);

	// Set the interrupt level 2 for the PIT timer
	HAL_INTERRUPT_SET_LEVEL(IRQ_PIT_SOURCE, PIT_INT_PRIORITY);

	/* Enable PLIC interrupt PIT timer source */
	HAL_INTERRUPT_ENABLE(IRQ_PIT_SOURCE);

	/* Start PIT timer */
	DrvPIT->Control(AE350_PIT_TIMER_START, PIT_TIMER0);

	printf("Setup PIT timer.\r\n");
}

// PIT timer interrupt handler
void pit_timer_irq_handler(void)
{
	AE350_DRIVER_PIT *DrvPIT = &Driver_PIT;

	/* Clear PIT timer interrupt status */
	DrvPIT->Control(AE350_PIT_TIMER_INTR_CLEAR, PIT_TIMER0);

	printf("It's in PIT timer interrupt.\r\n");
}


// GPIO callback
void gpio_callback(uint32_t event)
{
	switch (event)
	{
		case AE350_GPIO_EVENT_PIN3:
			//SW1
			printf("It's in GPIO interrupt.\r\n");
			break;
		default:
			break;
	}
}

// Setup GPIO
static void setup_gpio(void)
{
	AE350_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;

	// Initializes GPIO
	GPIO_Dri->Initialize(gpio_callback);

	// Set GPIO direction
	GPIO_Dri->SetDir(GPIO_USED_MASK, AE350_GPIO_DIR_INPUT);		// Key input

	// Set switches interrupt mode to negative edge and enable it
	GPIO_Dri->Control(AE350_GPIO_SET_INTR_NEGATIVE_EDGE | AE350_GPIO_INTR_ENABLE, GPIO_USED_MASK, 0, 0);

	// Set the interrupt level 1 for the GPIO
	HAL_INTERRUPT_SET_LEVEL(IRQ_GPIO_SOURCE, GPIO_INT_PRIORITY);

	// Enable interrupt GPIO source
	HAL_INTERRUPT_ENABLE(IRQ_GPIO_SOURCE);

	printf("Setup GPIO.\r\n");
}

// Application entry function
int demo_intr(void)
{
	// Initializes UART
	uart_init(UART_SPEED);	// Baud rate is UART_SPEED

	printf("\r\nIt's a Multiple Peripherals Interrupts demo.\r\n\r\n");

	// Disable Machine External interrupts
	HAL_MEIP_DISABLE();

	// Setup PIT timer
	setup_pit_timer(PIT_TIMER_PERIOD);

	// Setup GPIO
	setup_gpio();

	// Enable Machine External interrupts
	HAL_MEIP_ENABLE();

	// Enable interrupts in general.
	HAL_MIE_ENABLE();

	return 0;
}

#endif	/* RUN_DEMO_INTR */
