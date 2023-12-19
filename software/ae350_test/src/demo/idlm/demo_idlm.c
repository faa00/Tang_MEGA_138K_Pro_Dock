/*
 * ******************************************************************************************
 * File		: demo_idlm.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Access ILM/DLM demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows how to access local memory (ILM/DLM) by means of slave port.
 * It uses different CPU and BUS address to move data from/to local memory.
 *
 * Scenario:
 *
 * After checking the ILM and DLM configuration, the program will start to move data from
 * ROM/DDR to ILM/DLM, and move data from ILM/DLM to DDR,  and then identify it. First, it
 * will copy some ROM/DDR data to ILM/DLM and read data back to do data comparing.
 * Second, it will copy some ILM/DLM data to DDR and then do data comparing.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running access ILM/DLM demo
#if RUN_DEMO_IDLM

// *********** Includes ************ //
#include "platform.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>


// ********** Definitions ********** //

/* Data pattern in DLM */
static char dlm_pattern[1024] = { -1 };


// Get local memory size
static unsigned int get_lm_size(unsigned int lm_cfg)
{
	unsigned int lmsz;

	lmsz = (lm_cfg >> 15) & 0x1F;

	if (!lmsz || lmsz > 20)
	{
		lmsz = 0;                   // Reserved size! treat as 0!
	}
	else
	{
		lmsz = 1 << (9 + lmsz);		// ILM and DLM are both 64KB
	}

	return lmsz;
}

// Application entry function
int demo_idlm(void)
{
	unsigned int micm_cfg, mdcm_cfg;
	static unsigned int g_ilm_size, g_dlm_size;
	unsigned int data_size;
	char *src, *dst;
	int i;

	// Initializes UART
	uart_init(38400);		// Baud rate is 38400

	printf("\r\nIt's a ILM/DLM Access by Slave Port demo.\r\n\r\n");

	/* Check if ILM/DLM is enable */
	if (!(read_csr(NDS_MILMB) & 0x1) || !(read_csr(NDS_MDLMB) & 0x1))
	{
		printf("Local memory does NOT enable.\r\n");
		printf("Please manually enable ILM/DLM before loading ELF.\r\n");
		return 0;
	}

	/* Check ILM */
	micm_cfg = __nds__mfsr(NDS_MICM_CFG);
	printf("Check ILM: MICM_CFG = 0x%x\r\n", micm_cfg);

	if ((micm_cfg >> 12) & 0x7)
	{
		printf("The system has ILM...\r\n");
	}
	else
	{
		printf("The system has no ILM...\r\n");
		return 0;
	}

	/* Check DLM */
	mdcm_cfg = __nds__mfsr(NDS_MDCM_CFG);
	printf("Check DLM: MDCM_CFG = 0x%x\r\n", mdcm_cfg);

	if ((mdcm_cfg >> 12) & 0x7)
	{
		printf("The system has DLM...\r\n");
	}
	else
	{
		printf("The system has no DLM...\r\n");
		return 0;
	}

	/* Get ILM/DLM size */
	g_ilm_size = get_lm_size(micm_cfg);		// ILM size
	g_dlm_size = get_lm_size(mdcm_cfg);		// DLM size
	printf("ILM Size = 0x%x (%dKB), DLM Size = 0x%x (%dKB)\r\n", g_ilm_size, g_ilm_size/1024, g_dlm_size, g_dlm_size/1024);

	/* Move data from ROM to ILM */
	dst = (char *)(ILM_BASE);				// DLM address
	src = (char *)(SPIMEM_BASE);          	// ROM bus address
	data_size = g_ilm_size;
	printf("\r\nMove %d data from ROM to ILM\r\n", data_size);
	memcpy(dst, src, data_size);

	/* Check data */
	printf("Checking data... ");
	if (memcmp(dst, src, data_size) != 0)
	{
		printf("ERROR.\r\n");
	}
	else
	{
		printf("OK.\r\n");
	}

	/* Move data from DDR to DLM */
	dst = (char *)(DLM_BASE);				// DLM address
	src = (char *)(DDRMEM_BASE);          	// DDR bus address
	data_size = g_dlm_size/2;
	printf("\r\nMove %d data from DDR to DLM\r\n", data_size);
	memcpy(dst, src, data_size);

	/* Check data */
	printf("Checking data... ");
	if (memcmp(dst, src, data_size) != 0)
	{
		printf("ERROR.\r\n");
	}
	else
	{
		printf("OK.\r\n");
	}

	/* Move data from ILM to DDR by slave port */
	dst = (char *)(DDRMEM_BASE+0x10000);		// DDR bus address
	src = (char *)(ILM_BASE);          			// ILM address
	data_size = g_ilm_size/64;
	printf("\r\nMove %d data from ILM to DDR\r\n", data_size);
	memcpy(dst, src, data_size);

	/* Check data */
	printf("Checking data... ");
	if (memcmp(dst, src, data_size) != 0)
	{
		printf("ERROR.\r\n");
	}
	else
	{
		printf("OK.\r\n");
	}

	/* Move data from DLM to DDR by slave port */
	src = (char *)(dlm_pattern);
	data_size = sizeof(dlm_pattern);
	for (i = 0; i < data_size; i++)
	{
		src[i] = 0x5a;
	}
	printf("\r\nMove %d data from DLM to DDR\r\n", data_size);
	memcpy(dst, src, data_size);

	printf("Checking data... ");
	if (memcmp(dst, src, data_size) != 0)
	{
		printf("ERROR.\r\n");
	}
	else
	{
		printf("OK.\r\n");
	}

	printf("\r\nAccess ILM/DLM by Slave Port Completed.\r\n");

	return 0;
}

#endif	/* RUN_DEMO_IDLM */
