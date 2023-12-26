/*
 * ******************************************************************************************
 * File		: demo_plmt.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Platform level machine timer demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows how to active machine related interrupts.
 * It enables machine timer, machine software interrupts.
 *
 * The machine timer interrupt:
 * The machine timer interrupt period is set to 2 seconds. If the interrupt
 * happens then the handler sets the machine software trigger flag.
 *
 * The machine software interrupt:
 * The machine software interrupt is activated when machine software trigger flag
 * is set by machine timer interrupt.
 *
 * The main function:
 * The main function checks the flags status changed by machine timer interrupt
 * to trigger the machine software interrupts. And press key1, key2, key3 to light led1,
 * led2, led3 at any time.
 *
 * Scenario:
 *
 * The machine software interrupt is activated to output machine timer is alive message
 * every 2 seconds by setting the trigger flag from machine timer interrupt handler.
 * Press key1, key2, key3 to light led1, led2, led3 at any time.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running Machine Timer demo
#if RUN_DEMO_PLMT

// ************ Includes ************ //
#include "Driver_GPIO.h"
#include "uart.h"
#include "platform.h"
#include <stdio.h>


// ********** Definitions ********** //

// On the DK-START-GW5AT138 V2.0
// LED: GPIO_/2/1/0
#define GPIO_OUTPUT_LED  0x7
// KEY: GPIO_5/4/3
#define GPIO_INPUT_KEY   0x38

#define MACHINE_TIMER_PERIOD			(2 * OSCFREQ)          // 2 seconds

/* The flag to trigger Machine software interrupt */
static volatile char trigger_mswi_flag = 0;

extern AE350_DRIVER_GPIO Driver_GPIO;		// GPIO
int gpio_demo(void);
void __attribute__((weak)) gpio_led_ctrl(int light);
void gpio_callback(uint32_t event);

void mtime_handler(void);
void mswi_handler(void);
void setup_mtimer(void);
void setup_mswi(void);
void trigger_mswi(void);


// Application entry function
int demo_plmt(void)
{
	// Initializes UART
	uart_init(UART_SPEED);		// Baud rate is UART_SPEED

	printf("\r\nIt's a Platform Level Machine Timer demo.\r\n\r\n");

	/* Disable the external and machine timer and software interrupts until setup is done */
	HAL_MEIP_DISABLE();		// Disable Machine External interrupt
	HAL_MTIME_DISABLE();	// Disable Machine Timer interrupt
	HAL_MSWI_DISABLE();		// Disable Machine Software interrupt

	/* Setup machine timer and software interrupt handler */
	setup_mtimer();
	setup_mswi();

	/* Enable the Machine-External/Timer/SWI bit in MIE */
	HAL_MEIP_ENABLE();		// Enable Machine External interrupt
	HAL_MTIME_ENABLE();		// Enable Machine Timer interrupt
	HAL_MSWI_ENABLE();		// Enable Machine Software interrupt

	/* Enable interrupts in general. */
	HAL_MIE_ENABLE();

	// Press key1, key2, key3 to light led1, led2, led3
	gpio_demo();

	int mtimer_cnt = 0;

	while(1)
	{
		// Wait machine timer interrupt handler
		// to set machine software interrupt handler flag
		if(trigger_mswi_flag)
		{
			trigger_mswi();
			printf("mtimer count: %4d\r\n", mtimer_cnt);
			mtimer_cnt++;
			printf("\r\n");
		}
	}

	return 0;
}

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

	/*
	 * Set the flag to trigger Machine software interrupt.
	 * The Machine SWI interrupt handler will output message to indicate
	 * machine timer is alive.
	 */
	trigger_mswi_flag = 1;

	/* Re-enable the timer interrupt. */
	HAL_MTIME_ENABLE();
}

// Machine software interrupt handler
void mswi_handler(void)
{
	/* Machine SWI is cleared */
	HAL_MSWI_CLEAR();

	/*
	 * It is triggered by machine timer interrupt handler.
	 * Output messages to indicate machine timer is alive.
	 */
	if(trigger_mswi_flag)
	{
		/* Clear UART output message trigger flag */
		trigger_mswi_flag = 0;

		printf("Message triggered from Machine Timer interrupt handler.\r\n");
	}
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

// Setup machine software interrupt
void setup_mswi(void)
{
	/* Initializes Machine SWI */
	/* Machine SWI is connected to PLIC_SW source 1 */
	HAL_MSWI_INITIAL();

	printf("Setup machine software interrupt handler.\r\n");
}

// Trigger machine software interrupt
void trigger_mswi(void)
{
	/* Set PLIC_SW source 1 to trigger machine software interrupt handler */
	HAL_MSWI_PENDING();
}

// Initializes GPIO and set direction of GPIO
int gpio_demo(void)
{
	AE350_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;

	// Initializes GPIO
	GPIO_Dri->Initialize(gpio_callback);

	// Set GPIO direction
	GPIO_Dri->SetDir(GPIO_OUTPUT_LED, AE350_GPIO_DIR_OUTPUT);	// Led output
	GPIO_Dri->SetDir(GPIO_INPUT_KEY, AE350_GPIO_DIR_INPUT);		// Key input

	// Set switches interrupt mode to negative edge and enable it
	GPIO_Dri->Control(AE350_GPIO_SET_INTR_NEGATIVE_EDGE | AE350_GPIO_INTR_ENABLE, GPIO_INPUT_KEY, 0, 0);

	// Set the interrupt level 1 for the GPIO
	HAL_INTERRUPT_SET_LEVEL(IRQ_GPIO_SOURCE, 1);

	// Enable interrupt GPIO source
	HAL_INTERRUPT_ENABLE(IRQ_GPIO_SOURCE);

	printf("Setup GPIO key and led.\r\n\r\n");

	return 0;
}

// GPIO callback
void gpio_callback(uint32_t event)
{
	switch (event)
	{
	case AE350_GPIO_EVENT_PIN3:
		// Press key1, led1 is on
		gpio_led_ctrl(0);
		printf("Press key1 and led1 is on.\r\n\r\n");
		break;
	case AE350_GPIO_EVENT_PIN4:
		// Press key2, led2 is on
		gpio_led_ctrl(1);
		printf("Press key2 and led2 is on.\r\n\r\n");
		break;
	case AE350_GPIO_EVENT_PIN5:
		// Press key3, led3 is on
		gpio_led_ctrl(2);
		printf("Press key3 and led3 is on.\r\n\r\n");
		break;
	default:
		break;
	}
}

// Control LED
void __attribute__((weak)) gpio_led_ctrl(int light)
{
	AE350_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;
	GPIO_Dri->Write(GPIO_OUTPUT_LED, 1);

	if(0 == light)
	{
		GPIO_Dri->Write(0x06, 0);
	}
	else if(1 == light)
	{
		GPIO_Dri->Write(0x05, 0);
	}
	else if(2 == light)
	{
		GPIO_Dri->Write(0x03, 0);
	}
	else
	{
		GPIO_Dri->Write(0x07, 0);
	}
}

#endif	/* RUN_DEMO_PLMT */
