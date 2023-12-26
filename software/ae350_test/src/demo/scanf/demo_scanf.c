/*
 * ******************************************************************************************
 * File		: demo_scanf.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Overwrite scanf() demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows how to use STD scanf() function to redirect
 * the input of UART port by overwriting scanf() functions.
 *
 * Scenario:
 *
 * Use STD scanf() function to input message (string, integer, hex...) to UART.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running overwriting scanf() demo
#if RUN_DEMO_SCANF

// ************ Includes *********** //
#include "uart.h"
#include "delay.h"
#include <stdio.h>


// ********** Definitions ********** //
int demo_scanf(void)
{
	char s[256];
	unsigned int c;
	int n;

	// Initializes UART
	uart_init(UART_SPEED);		// Baud rate is UART_SPEED

	printf("\r\nIt's a scanf() demo.\r\n\r\n");

	simple_delay_ms(1000);
	printf("Enter a string : ");
	scanf("%s", s);
	printf("%s\r\n", s);

	simple_delay_ms(1000);
	printf("Enter a hex : ");
	scanf("%x", &c);
	printf("0x%x\r\n", c);

	simple_delay_ms(1000);
	printf("Enter an integer : ");
	scanf("%d", &n);
	printf("%d\r\n", n);

	simple_delay_ms(1000);
	printf("\r\nDemo scanf() PASS.\r\n");

	return 0;
}

#endif	/* RUN_DEMO_SCANF */
