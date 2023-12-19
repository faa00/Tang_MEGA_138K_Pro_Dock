/*
 * ******************************************************************************************
 * File		: demo_gpio.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: GPIO demo
 * ******************************************************************************************
 */

/*
 *************************************************************************************************
 * This example program shows the usage of the GPIO interface.
 * It demonstrates the GPIO to output/input application for 7-segments/switches.
 *
 * Scenario:
 *
 * After initialized, the demo program will set GPIO direction for two 7-segments(output)
 * and ten switches(input). And then set the switches interrupt trigger mode to negative
 * edge and enable corresponding interrupts. After all setting is done, the program will
 * enter to a infinite loop, waiting for user to press switch1 ~ switch10 to trigger interrupts.
 * And when interrupt occurs, the two 7-segments will show the corresponding value 0 ~ 9.
 *************************************************************************************************
* */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running GPIO demo
#if RUN_DEMO_GPIO

// *********** Includes *********** //
#include "Driver_GPIO.h"
#include "platform.h"
#include "uart.h"
#include <stdio.h>


// ********** Declarations ******** //
void __attribute__((weak)) writesegment(int num);	// Display segment number 0~9


// ********** Definitions ********* //
extern AE350_DRIVER_GPIO Driver_GPIO;		// GPIO

#define GPIO_SW_USED_MASK     0xffff		// Pin 0~15 switches(input)
#define GPIO_7SEG_USED_MASK   0xffff0000	// pin 16~31 two 7-segments(output)

#define GPIO_7SEG1_OFFSET     16			// Pin 16~23 7-segment 1
#define GPIO_7SEG2_OFFSET     24			// Pin 24~31 7-segment 2

// Common cathode
uint8_t seven_segment_value[10] =
{
	0x3f,	// 0
	0x06,	// 1
	0x5b,	// 2
	0x4f,	// 3
	0x66,	// 4
	0x6d,	// 5
	0x7d,	// 6
	0x07,	// 7
	0x7f,	// 8
	0x6f	// 9
};


// Do 7-segment
void __attribute__((weak)) writesegment(int num)
{
	AE350_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;

	GPIO_Dri->Write(GPIO_7SEG_USED_MASK, 1);
	GPIO_Dri->Write(seven_segment_value[num] << GPIO_7SEG1_OFFSET, 0);
	GPIO_Dri->Write(seven_segment_value[num] << GPIO_7SEG2_OFFSET, 0);

	printf("7-segment number is %d\r\n", num);
}

// GPIO input callback event
void gpio_callback(uint32_t event)
{
	switch (event)
	{
		case AE350_GPIO_EVENT_PIN0:
			// Set 7-segment number 0
			writesegment(0);
			break;
		case AE350_GPIO_EVENT_PIN1:
			// Set 7-segment number 1
			writesegment(1);
			break;
		case AE350_GPIO_EVENT_PIN2:
			// Set 7-segment number 2
			writesegment(2);
			break;
		case AE350_GPIO_EVENT_PIN3:
			// Set 7-segment number 3
			writesegment(3);
			break;
		case AE350_GPIO_EVENT_PIN4:
			// Set 7-segment number 4
			writesegment(4);
			break;
		case AE350_GPIO_EVENT_PIN5:
			// Set 7-segment number 5
			writesegment(5);
			break;
		case AE350_GPIO_EVENT_PIN6:
			// Set 7-segment number 6
			writesegment(6);
			break;
		case AE350_GPIO_EVENT_PIN7:
			// Set 7-segment number 7
			writesegment(7);
			break;
		case AE350_GPIO_EVENT_PIN8:
			// Set 7-segment number 8
			writesegment(8);
			break;
		case AE350_GPIO_EVENT_PIN9:
			// Set 7-segment number 9
			writesegment(9);
			break;
		default:
			break;
	}
}

// Application entry function
int demo_gpio(void)
{
	AE350_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;

	// Initializes UART
	uart_init(38400);	// Baudrate is 38400

	printf("\r\nIt's a GPIO 7-Segments and Switches demo.\r\n");

	// Initializes GPIO
	GPIO_Dri->Initialize(gpio_callback);

	// Set GPIO direction (7-segments: output, switches: input)
	GPIO_Dri->SetDir(GPIO_7SEG_USED_MASK, AE350_GPIO_DIR_OUTPUT);	// Pin 16~31 is output
	GPIO_Dri->SetDir(GPIO_SW_USED_MASK, AE350_GPIO_DIR_INPUT);		// Pin 0~15 is input

	// Set switches interrupt mode to negative edge and enable it
	GPIO_Dri->Control(AE350_GPIO_SET_INTR_NEGATIVE_EDGE | AE350_GPIO_INTR_ENABLE, GPIO_SW_USED_MASK, 0, 0);

	// Set the interrupt level 1 for the GPIO
	HAL_INTERRUPT_SET_LEVEL(IRQ_GPIO_SOURCE, 1);

	// Enable interrupt GPIO source
	HAL_INTERRUPT_ENABLE(IRQ_GPIO_SOURCE);

	printf("\r\nPlease press SW1-SW10 and seven-segments will show the corresponding value...\r\n\r\n");

	// Press SW1 to SW10 to trigger GPIO pin 0-9 input interrupt handler.

	return 0;
}

#endif	/* RUN_DEMO_GPIO */
