/*
 * ******************************************************************************************
 * File		: uart.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: UART predefine
 * ******************************************************************************************
 */

#ifndef __UART_H__
#define __UART_H__


// Definitions -------------------------------------------------------------------------------

#define UART1_USED_IN_PRINTF  0		// UART1 used in printf(); otherwise UART2


// Declarations ------------------------------------------------------------------------------

extern void uart_init(unsigned int baudrate);		// Initializes UART
extern int uart_getc(void);							// Input a character by UART
extern void uart_putc(int c);						// Print a character by UART
extern int uart_puts(const char *s);				// Print a string by UART
extern int outbyte(int c);							// Overwrite function for printf()


#endif	/* __UART_H__ */
