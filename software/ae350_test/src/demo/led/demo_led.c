/*
 * ******************************************************************************************
 * File		: demo_led.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Waterfall led demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This example program shows the usage of the GPIO interface.
 * It demonstrates the waterfall led.
 *
 * Scenario:
 *
 * After initialized, the demo program will set GPIO direction to output for waterfall led.
 * The waterfall led is lighted each led, the UART output lighted led.
 *
 * Note: Only GPIO pin as output, there is no GPIO pin as input interrupt event, callback
 * function has no event to be handled.
 ********************************************************************************************
* */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running waterfall led demo
#if RUN_DEMO_LED

// *********** Includes *********** //
#include "Driver_GPIO.h"
#include "uart.h"
#include "delay.h"
#include "config.h"
#include <stdio.h>


// ********** Definitions ********* //
extern AE350_DRIVER_GPIO Driver_GPIO;		// GPIO

#define GPIO_LED_USED_MASK		0x7			// GPIO pins output as LED
#define NUM_LED				3			// LED number


// GPIO callback event
void gpio_callback(uint32_t event)
{
	switch (event)
	{
		// Don't have GPIO pins as interrupt event
		default:
			break;
	}
}

// Application entry function
int demo_led(void)
{
	AE350_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;
	uint8_t num = NUM_LED;
	uint32_t led_pin = 0;	// This led

	long begin_time = 0;
	long end_time = 0;
	long use_time = 0;

	// Initializes UART
	uart_init(115200);		// Baud rate is 115200

	printf("\r\nIt's a Waterfall Led demo.\r\n\r\n");

	// Initializes GPIO
	GPIO_Dri->Initialize(gpio_callback);

	// Set GPIO direction (GPIO pins as led: output)
	GPIO_Dri->SetDir(GPIO_LED_USED_MASK, AE350_GPIO_DIR_OUTPUT);

	// Waterfall led
	while(1)
	{

		begin_time = time();

#ifdef CFG_CACHE_ENABLE
		simple_delay_ms(50000);
#else
		simple_delay_ms(100);
#endif

		if( num == NUM_LED )
		{
			num = 0;
    			GPIO_Dri->Write(7, 1);
			printf("\n");
		} 
		else 
		{
		    // This led
		    led_pin = 0x1 << (num++);

		    // Other leds are off
		    GPIO_Dri->Write(led_pin, 0);

		    // This led is on
		    //GPIO_Dri->Write(num, 0);

		    end_time = time();

		    use_time = end_time - begin_time;

		    printf("led[%d] is on %d.\n", num, (int)use_time);
		}
	}

	return 0;
}

#endif	/* RUN_DEMO_LED */
