/*
 * ******************************************************************************************
 * File		: read.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Overwrite scanf() _read
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "uart.h"


// Definitions ------------------------------------------------------------------------------

/*
 * Retarget function for scanf()
 */
extern int _read (int file, char * ptr, int len);
int _read (int file, char * ptr, int len)
{
	char c;
	int i;

	for (i = 0; i < len; i++)
	{
		c = uart_getc();
		*ptr++ = c;
		if (c == '\r')
		{
			break;
		}
	}

	return (len - i);
}
