/* IMPORT */

/* Includes: */
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "platform.h"
#include "stub.h"


/* Definitions: */

extern int _put_char(int ch) __attribute__((weak));

__attribute__((used))
ssize_t _write(int fd, const void* ptr, size_t len)
{
	const uint8_t * current = (const uint8_t *) ptr;

	for (size_t jj = 0; jj < len; jj++)
	{
		_put_char(current[jj]);

		if (current[jj] == '\n')
		{
			_put_char('\r');
		}
	}

	return len;
}

__attribute__((used))
int _put_char(int ch)
{
	#define SERIAL_LSR_THRE         0x20
	while ((DEV_UART2->LSR & SERIAL_LSR_THRE) == 0) ;
	DEV_UART2->THR = ch;
    return ch;
}
