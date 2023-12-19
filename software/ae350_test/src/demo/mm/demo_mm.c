/*
 * ******************************************************************************************
 * File		: demo_mm.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Memory management demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This example program shows the usage of the memory management.
 *
 * Scenario:
 *
 * After memory initialized, the demo program will allocate a memory to the character arrays.
 * Set an array values, copy this array to another array, compare these arrays. Finally,
 * free these memory.
 ********************************************************************************************
* */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running memory management demo
#if RUN_DEMO_MM

// *********** Includes *********** //
#include "uart.h"
#include "mm.h"
#include <stdio.h>


// ********** Definitions ********* //
#define M_SIZE	100


// Application entry function
int demo_mm(void)
{
	// Initializes UART
	uart_init(38400);	// Baud rate is 38400

	printf("\r\nIt's a Memory Management demo.\r\n\r\n");

	// Initializes memory
	mem_init();

	printf("allocate a...\r\n");
	unsigned char* a = (unsigned char*)mem_malloc(M_SIZE);

	printf("allocate b...\r\n");
	unsigned char* b = (unsigned char*)mem_malloc(M_SIZE);

	printf("\r\nset a...\r\n");
	mem_set(a, 0xAA, M_SIZE);

	printf("copy a to b...\r\n");
	mem_cpy(b, a, M_SIZE);

	printf("a :\r\n");
	for(int i = 0;i < M_SIZE;i++)
	{
		printf("%x ", a[i]);
	}
	printf("\r\n");

	printf("b :\r\n");
	for(int i = 0;i < M_SIZE;i++)
	{
		printf("%x ", b[i]);
	}
	printf("\r\n\r\n");

	if(mem_cmp(b, a, M_SIZE))
	{
		printf("compare a and b FAIL.\r\n");
	}
	else
	{
		printf("compare a and b PASS.\r\n");
	}

	mem_free(a);
	printf("\r\nfree a PASS.\r\n");

	mem_free(b);
	printf("free b PASS.\r\n");

	return 0;
}

#endif	/* RUN_DEMO_MM */
