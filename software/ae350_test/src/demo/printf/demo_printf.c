/*
 * ******************************************************************************************
 * File		: demo_printf.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Overwrite printf() demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows how to use STD printf()/sprintf() function to redirect
 * the output of UART port or a buffer by overwriting printf()/sprintf() functions.
 *
 * Scenario:
 *
 * Use STD printf() function to output message (string, character, integer, hex...) to UART.
 * Use STD sprintf() function to output message to a buffer.
 * Control output of UART1 or UART2 port in uart.h
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running overwriting printf() demo
#if RUN_DEMO_PRINTF

// ************ Includes *********** //
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********** //
int demo_printf(void)
{
	char *ptr = "Hello world!";
	int i = 5;
	unsigned int bs = sizeof(int)*8;
	int mi;
	char buf[80];

	/* Initializes UART */
	uart_init(38400);		// Baud rate is 38400

	mi = (1 << (bs-1)) + 1;

	printf("\r\nIt's a printf() demo.\r\n\r\n");

	printf("%s\r\n", ptr);
	printf("%d = 5\r\n", i);
	printf("%d = - max integer\r\n", mi);
	printf("char %c = 'a'\r\n", 'a');
	printf("hex %x = ff\r\n", 0xff);
	printf("hex %02x = 00\r\n", 0);
	printf("signed %d = unsigned %u = hex %x\r\n", -3, -3, -3);
	printf("%d %s(s)", 0, "message");
	printf("\r\n");
	printf("%d %s(s) with %%\r\n", 0, "message");

	sprintf(buf, "justif: \"%-10s\"\r\n", "left");
	printf("%s", buf);

	sprintf(buf, "justif: \"%10s\"\r\n", "right");
	printf("%s", buf);

	sprintf(buf, " 3: %04d zero padded\r\n", 3);
	printf("%s", buf);

	sprintf(buf, " 3: %-4d left justif.\r\n", 3);
	printf("%s", buf);

	sprintf(buf, " 3: %4d right justif.\r\n", 3);
	printf("%s", buf);

	sprintf(buf, "-3: %04d zero padded\r\n", -3);
	printf("%s", buf);

	sprintf(buf, "-3: %-4d left justif.\r\n", -3);
	printf("%s", buf);

	sprintf(buf, "-3: %4d right justif.\r\n", -3);
	printf("%s", buf);

	return 0;
}

#endif	/* RUN_DEMO_PRINTF */
