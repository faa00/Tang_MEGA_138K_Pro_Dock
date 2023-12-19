/* IMPORT */

/* Includes: */
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "stub.h"
#include "uart.h"


/* Definitions: */

__attribute__((used))
ssize_t _read(int fd, void* ptr, size_t len)
{
	char c;
	int i;

	for (i = 0; i < len; i++)
	{
		c = uart_getc();
		*(uint8_t *)(ptr++) = c;
		if (c == '\r')
		{
			break;
		}
	}

	return (len - i);
}
