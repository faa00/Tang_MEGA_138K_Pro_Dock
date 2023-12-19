/*
 * ******************************************************************************************
 * File		: uart.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: UART predefine
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "uart.h"
#include "platform.h"


// Definitions ------------------------------------------------------------------------------

// Define UART1 or UART2 used in printf()
#if UART1_USED_IN_PRINTF
#define	DEV_UART	DEV_UART1	// UART1
#else
#define	DEV_UART	DEV_UART2	// UART2
#endif

// Baud rate computed
#define BAUD_RATE(n)            ((UCLKFREQ + 8 * (n)) / (16 * (n)))

// Initializes UART
void uart_init(unsigned int baudrate)
{
	/* Set DLAB to 1 */
	DEV_UART->LCR |= 0x80;

	/* Set DLL for baud rate */
	DEV_UART->DLL = (BAUD_RATE(baudrate) >> 0) & 0xff;
	DEV_UART->DLM = (BAUD_RATE(baudrate) >> 8) & 0xff;

	/* LCR: length 8, no parity, 1 stop bit. */
	DEV_UART->LCR = 0x03;

	/* FCR: enable FIFO, reset TX and RX. */
	DEV_UART->FCR = 0x07;
}

// Input a character by UART
int uart_getc(void)
{
	#define SERIAL_LSR_RDR 0x01

	while ((DEV_UART->LSR & SERIAL_LSR_RDR) == 0);

	return DEV_UART->RBR;
}

// Print a character by UART
void uart_putc(int c)
{
	#define SERIAL_LSR_THRE 0x20

	while ((DEV_UART->LSR & SERIAL_LSR_THRE) == 0);

	DEV_UART->THR = c;
}

// Print a string by UART
int uart_puts(const char *s)
{
	int len = 0;

	while (*s)
	{
		uart_putc(*s);

		if (*s == '\n')
		{
			uart_putc('\r');
		}

		s++;
		len++;
	}

	return len;
}

// Overwrite function for printf()
int outbyte(int c)
{
	uart_putc(c);

	if (c =='\n')
	{
		uart_putc('\r');
	}

	return c;
}
