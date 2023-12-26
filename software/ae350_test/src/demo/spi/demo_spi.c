/*
 * ******************************************************************************************
 * File		: demo_spi.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: SPI demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This example program shows the usage of the SPI interface for master/slave
 * communication. It demonstrates the SPI master to send/receive data to/from
 * a slave.
 *
 * This example program needs to connect two development boards, one is as master,
 * another is as slave.
 *
 * Scenario:
 *
 * After initialized, sets up the SPI master to 8-bit data length and bus SCLK
 * frequency to 33MHz. After setting, master will send 8-byte data(value=0x0~0x7)
 * to slave, and then receive the data back from slave.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running SPI demo
#if RUN_DEMO_SPI

// ************ Includes ************ //
#include "Driver_SPI.h"
#include "platform.h"
#include "uart.h"
#include "delay.h"
#include <stdio.h>


// ********** Definitions ********** //

// For user defined
// Select SPI master mode or slave mode
#define SPI_MASTER_MODE     1						// Enable/disable SPI master mode
#define SPI_SLAVE_MODE      !(SPI_MASTER_MODE)		// Enable/disable SPI slave mode
// Data transmission direction
#define MASTER2SLAVE        0						// Send data from master to slave
#define SLAVE2MASTER        !(MASTER2SLAVE)			// Send data from slave to master

// SPI commands
#define SPI_READ    0x0b
#define SPI_WRITE   0x51
#define SPI_DUMMY   0xff

#define MTIME					PLMT_BASE
#define FAKE_CPUFREQ_S			PCLKFREQ
#define SLAVE_LATENCY_MS		0.5
#define DELAY_CYCLE				(SLAVE_LATENCY_MS * (FAKE_CPUFREQ_S/1000))

#define TEST_DATA_SIZE      	8
#define TOTAL_TRANSFER_SIZE 	(TEST_DATA_SIZE + 2) // Total transfer size is cmd(1) + dummy(1) + data(TEST_DATA_SIZE)

static volatile char spi_event_transfer_complete = 0;

extern AE350_DRIVER_SPI Driver_SPI;		// SPI


static void wait_complete(void)
{
	while (!spi_event_transfer_complete);
	printf("%d \r\n", spi_event_transfer_complete);
	spi_event_transfer_complete = 0;
}

// SPI callback
void spi_callback(uint32_t event)
{
	switch (event)
	{
		case AE350_SPI_EVENT_TRANSFER_COMPLETE:
			spi_event_transfer_complete = 1;
			break;
		case AE350_SPI_EVENT_DATA_LOST:
			while(1);
			break;
		default:
			while(1);
			break;
	}
}

#if SPI_MASTER_MODE
// Test SPI master mode

#if MASTER2SLAVE
static void spi_prepare_data_out(uint8_t *data_out, uint8_t cmd, uint32_t dummy_cnt, uint8_t *w_data, uint32_t wcnt)
{
	uint8_t *ptr = data_out;
	uint32_t i;

	// Set 1-byte command
	*ptr++ = cmd;

	// Set n-byte dummy
	for (i = 0; i < dummy_cnt; i++)
	{
		*ptr++ = SPI_DUMMY;
	}

	// Set n-byte w_data
	for (i = 0; i < wcnt; i++)
	{
		*ptr++ = *w_data++;
	}
}
#endif	/* MASTER2SLAVE */

int spi_master_mode (void)
{
	uint8_t data_out[TOTAL_TRANSFER_SIZE] = {0};
	int i;

	AE350_DRIVER_SPI *SPI_Dri = &Driver_SPI;

	// Initializes UART
	uart_init(UART_SPEED);	// Baud rate is UART_SPEED

	printf("\r\nIt's a SPI Master Mode demo.\r\n");
	printf("\r\n====Please hardwire 2 test board=====\r\n\r\n");

	// Initializes SPI
	SPI_Dri->Initialize(spi_callback);

	// Power full the SPI peripheral
	SPI_Dri->PowerControl(AE350_POWER_FULL);

	// Configure the SPI to master, 8-bit data length and bus speed to 1MHz
	SPI_Dri->Control(AE350_SPI_MODE_MASTER |
			 	  	 AE350_SPI_CPOL0_CPHA0 |
					 AE350_SPI_MSB_LSB |
					 AE350_SPI_DATA_BITS(8), 1000000);
	SPI_Dri->Control(AE350_SPI_TX_HEADER_LENGTH , 2);

	printf("Master write/read test...\r\n");

#if MASTER2SLAVE
	uint8_t w_data[TEST_DATA_SIZE] = {0};

	// Prepare write data
	for (i = 0; i < TEST_DATA_SIZE; i++)
	{
		w_data[i] = i;
	}

	spi_prepare_data_out(data_out, SPI_WRITE, 1, w_data, TEST_DATA_SIZE);

	// Write data to slave
	SPI_Dri->Send(data_out, TOTAL_TRANSFER_SIZE);
	wait_complete();

	for(int i = 0; i < TOTAL_TRANSFER_SIZE; i++)
	{
		printf("data_out[%d]=%d \r\n",i, data_out[i]);
	}
#elif SLAVE2MASTER
	uint8_t data_in[TEST_DATA_SIZE] = {0};

    for (i = 0; i < TOTAL_TRANSFER_SIZE; i++)
    {
    	data_out[i] = 0;
    }

	simple_delay_ms(200);

	SPI_Dri->Transfer(data_out, data_in, TOTAL_TRANSFER_SIZE);
	wait_complete();

	for (i = 0; i < TEST_DATA_SIZE; i++)
	{
		printf("data_in[%d]=%d \r\n", i, data_in[i]);
	}
#endif

	return 0;
}

#elif SPI_SLAVE_MODE
// Test SPI slave mode

int spi_slave_mode (void)
{
	uint8_t data[TEST_DATA_SIZE] = {0};

	AE350_DRIVER_SPI *SPI_Dri = &Driver_SPI;

	// Initializes UART
    uart_init(UART_SPEED);	// Baud rate is UART_SPEED

	printf("\r\nIt's a SPI Slave Mode demo.\r\n");
	printf("\r\n====Please hardwire 2 test board=====\r\n\r\n");

	// Initializes SPI
	SPI_Dri->Initialize(spi_callback);

	// Power full the SPI peripheral
	SPI_Dri->PowerControl(AE350_POWER_FULL);

	// Configure the SPI to slave, 8-bit data length
	SPI_Dri->Control(AE350_SPI_MODE_SLAVE |
			 	 	 AE350_SPI_CPOL0_CPHA0 |
					 AE350_SPI_MSB_LSB |
					 AE350_SPI_DATA_BITS(8), 0);

	printf("Slave write/read test...\r\n");

#if MASTER2SLAVE
	//Read data from master
	SPI_Dri->Receive(data, TEST_DATA_SIZE);
	wait_complete();

	for(int i = 0; i < TEST_DATA_SIZE; i++)
	{
		printf("data[%d]=%d \r\n", i, data[i]);
	}
#elif SLAVE2MASTER
	for (int i = 0; i < TEST_DATA_SIZE; i++)
	{
		data[i] = i+1;
	}

	// Write data to master
	SPI_Dri->Send(data, TEST_DATA_SIZE);
	wait_complete();
	for(int i = 0; i < TEST_DATA_SIZE; i++)
	{
		printf("data[%d]=%d \r\n", i, data[i]);
	}
#endif

	return 0;
}

#endif


// Application entry function
int demo_spi(void)
{
#if SPI_MASTER_MODE
	// Master mode
	spi_master_mode();
#elif SPI_SLAVE_MODE
	// Slave mode
	spi_slave_mode();
#endif

	return 0;
}

#endif	/* RUN_DEMO_SPI*/
