/*
 * ******************************************************************************************
 * File		: demo_plic.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Platform level interrupt controller demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows how to active machine and PLIC related interrupts.
 * It enables machine timer, machine software, PLIC PIT as simple timer interrupts.
 *
 * The machine timer interrupt:
 * The machine timer interrupt period is set to 2 seconds. If the interrupt
 * happens then the handler sets the machine software trigger flag.
 *
 * The PLIC PIT as simple timer interrupt:
 * The PLIC PIT as simple timer interrupt period is set to 200 milliseconds. If the interrupt
 * happens then the handler sets the machine software trigger flag.
 *
 * The machine software interrupt:
 * The machine software interrupt is activated when machine software trigger flag
 * is set by machine timer interrupt or PLIC PIT as simple timer interrupt.
 * It will output UART message to show machine timer or PLIC PIT as simple timer is alive.
 *
 * The main function:
 * The main function checks the flag status changed by machine timer or PLIC PIT as simple timer
 * interrupt to trigger the machine software interrupt.
 * This is no vectored interrupt.
 *
 * Scenario:
 *
 * The machine software interrupt is activated to output machine timer is alive message
 * every 2 seconds by setting the trigger flag from machine timer interrupt handler.
 * And the machine software interrupt is active to output PLIC PIT as simple timer is alive
 * message every 200ms by setting the trigger flag from PLIC PIT as simple timer interrupt
 * handler.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running platform level interrupt controller demo
#if RUN_DEMO_PLIC

// ************ Includes ************ //
#include "Driver_PIT.h"
#include "platform.h"
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********** //

#define MACHINE_TIMER_PERIOD            (2 * OSCFREQ)			// 2 seconds
#define PIT_TIMER_PERIOD                ((2 * OSCFREQ)/10)		// 0.2 seconds

/* The flag to trigger machine software interrupt handler */
static volatile char trigger_mswi_flag = 0;

/* The flag to trigger UART output message of mswi */
static volatile char trigger_mswi_uart_msg_flag = 0;

/* The flag to trigger UART output message of PIT as simple timer */
static volatile char trigger_pit_uart_msg_flag = 0;

extern AE350_DRIVER_PIT Driver_PIT;		// PIT as simple timer


// Machine timer interrupt handler
void mtime_handler(void)
{
	/* Disable the timer interrupt to prevent re-entry */
	HAL_MTIME_DISABLE();

	/* Reset the timer to specified period */
	// [63:0]: [63:32]=[1], [31:0]=[0]
	unsigned long long mtime = (((unsigned long long)(DEV_PLMT->MTIME[1])) << 32) | (DEV_PLMT->MTIME[0]);	// [63:0]
	mtime += MACHINE_TIMER_PERIOD;
	DEV_PLMT->MTIMECMP0[0] = (unsigned int)(mtime);			// [31:0]
	DEV_PLMT->MTIMECMP0[1] = (unsigned int)(mtime >> 32);	// [63:32]

	/* Enable interrupts in general to allow nested */
	HAL_MIE_ENABLE();

	/*
	 * Set the flag to trigger machine software interrupt.
	 * The machine SWI interrupt handler will output message to indicate
	 * machine timer is alive.
	 */
	trigger_mswi_flag = 1;
	trigger_mswi_uart_msg_flag = 1;

	/* Re-enable the timer interrupt. */
	HAL_MTIME_ENABLE();
}

// Machine software interrupt handler
void mswi_handler(void)
{
	/* Clear Machine SWI interrupt */
	HAL_MSWI_CLEAR();

	/*
	 * It is triggered by machine timer interrupt handler.
	 * Output messages to indicate machine timer is alive.
	 */
	if(trigger_mswi_uart_msg_flag)
	{
		/* Clear UART output message trigger flag */
		trigger_mswi_uart_msg_flag = 0;

		printf("\r\nMessage triggered from Machine Timer interrupt handler.\r\n\r\n");
	}

	/*
	 * It is triggered by PIT as simple timer interrupt handler.
	 * Output messages to indicate PLIC PIT as simple timer is alive.
	 */
	if(trigger_pit_uart_msg_flag)
	{
		/* Clear UART output message trigger flag */
	 	trigger_pit_uart_msg_flag = 0;

		printf("Message triggered from PLIC PIT as simple timer interrupt handler.\r\n");
	}
}

// PIT as simple timer interrupt handler
// This is pit_irq_handler
void pit_timer_irq_handler(void)
{
	AE350_DRIVER_PIT *DrvPIT = &Driver_PIT;

	/* Clear PIT as simple timer interrupt status */
	DrvPIT->Control(AE350_PIT_TIMER_INTR_CLEAR, 0);

	/*
	 * Set the flag to trigger machine software interrupt.
	 * The machine SWI interrupt handler will output message to indicate
	 * the PLIC PIT as simple timer is alive.
	 */
	trigger_mswi_flag = 1;
	trigger_pit_uart_msg_flag = 1;
}

// Setup machine timer
void setup_mtimer(void)
{
	HAL_MTIMER_INITIAL();

	/* Active machine timer */
	// [63:0]: [63:32]=[1], [31:0]=[0]
	unsigned long long mtime = (((unsigned long long)(DEV_PLMT->MTIME[1])) << 32) | (DEV_PLMT->MTIME[0]);	// [63:0]
	mtime += MACHINE_TIMER_PERIOD;
	DEV_PLMT->MTIMECMP0[0] = (unsigned int)(mtime);			// [31:0]
	DEV_PLMT->MTIMECMP0[1] = (unsigned int)(mtime >> 32);	// [63:32]

	printf("Setup machine timer.\r\n");
}

// Setup machine software interrupt handler
void setup_mswi(void)
{
	/* Initializes Machine SWI */
	/* Machine SWI is connected to PLIC_SW source 1 */
	HAL_MSWI_INITIAL();

	printf("Setup machine software interrupt handler.\r\n");
}

// Setup PIT as simple timer
void setup_pit_timer(unsigned int period)
{
	AE350_DRIVER_PIT *DrvPIT = &Driver_PIT;

	/* Setup system PIT as simple timer */
	// Initializes
	DrvPIT->Initialize();
	// Set period
	DrvPIT->SetPeriod(0, period);
	// Enable interrupt
	DrvPIT->Control(AE350_PIT_TIMER_INTR_ENABLE, 0);

	/* Priority level must be set > 0 to trigger the interrupt */
	HAL_INTERRUPT_SET_LEVEL(IRQ_PIT_SOURCE, 1);

	/* Enable PLIC interrupt PIT as simple timer source */
	HAL_INTERRUPT_ENABLE(IRQ_PIT_SOURCE);

	/* Start PIT as simple timer */
	DrvPIT->Control(AE350_PIT_TIMER_START, 0);

	printf("Setup pit timer.\r\n");
}

// Trigger machine software interrupt handler
void trigger_mswi(void)
{
	/* Trigger machine software interrupt handler */
	HAL_MSWI_PENDING();
}

// Application entry function
int demo_plic(void)
{
	// Initializes UART
	uart_init(UART_SPEED);		// Baud rate is UART_SPEED

	printf("\r\nIt's a No Vectored Platform Level Interrupt Controller demo.\r\n\r\n");

	/* Disable the Machine External/Timer/Software interrupts until setup is done */
	HAL_MEIP_DISABLE();		// Disable Machine External interrupts
	HAL_MTIME_DISABLE();	// Disable Machine Timer interrupts
	HAL_MSWI_DISABLE();		// Disable Machine Software interrupts

	/* Setup machine timer and software interrupt handler */
	setup_mtimer();
	setup_mswi();

	/* Setup PLIC PIT as simple timer interrupt handler */
	setup_pit_timer(PIT_TIMER_PERIOD);

	printf("\r\n");

	/* Enable the Machine External/Timer/Software interrupt bit in MIE. */
	HAL_MEIP_ENABLE();		// Enable Machine External interrupts
	HAL_MTIME_ENABLE();		// Enable Machine Timer interrupts
	HAL_MSWI_ENABLE();		// Enable Machine Software interrupts

	/* Enable interrupts in general. */
	HAL_MIE_ENABLE();

	/*
	 * Check the flags to trigger machine software interrupt to
	 * output messages to indicate the system is alive.
	 */
	while(1)
	{
		if(trigger_mswi_flag)
		{
			/* Clear machine software trigger flag */
			trigger_mswi_flag = 0;
			trigger_mswi();
		}
	}

	return 0;
}

#endif	/* RUN_DEMO_PLIC */
