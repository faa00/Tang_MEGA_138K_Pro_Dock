/*
 * ******************************************************************************************
 * File		: demo_i2c.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: I2C demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This example program shows the usage of the I2C interface for simulating master
 * to access slave EEPROM device. It demonstrates the I2C master to write a 10 bytes
 * data pattern to the simulate I2C slave EEPROM device and then read back to check
 * the result.
 *
 * Scenario:
 *
 * Firstly initialize the I2C slave development board, slave will act as EEPROM device
 * and wait master to write data. Secondly initialize the I2C master development board,
 * master will write 10 bytes data pattern {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE,
 * 0xF0, 0x5A, 0xA5} to slave and then read back to check the result. If consistent,
 * mater UART will output test pass message.
 ********************************************************************************************
* */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running I2C demo
#if RUN_DEMO_I2C

// ************ Includes ************ //
#include "Driver_I2C.h"
#include "platform.h"
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********** //
// Select I2C master mode or slave mode
// For user defined
#define I2C_MASTER_MODE_TEST         1							// Enable/disable I2C master mode
#define I2C_SLAVE_MODE_TEST          !(I2C_MASTER_MODE_TEST)	// Enable/disable I2C slave mode

// Select I2C 10bit or 7bit address mode
// For user defined
#define I2C_10BIT_MODE               0							// Enable/disable I2C 10bit address mode
#define I2C_7BIT_MODE                !(I2C_10BIT_MODE)			// Enable/disable I2C 7bit address mode


// Simulate I2C slave EEPROM device
// Simulate 10 bit I2C slave address
// User defined
#if I2C_10BIT_MODE
#define SIM_EEPROM_I2C_ADDR          (0x3FA)
#else
#define SIM_EEPROM_I2C_ADDR          (0x60)
#endif

#define I2C_SLAVE_ADDR               SIM_EEPROM_I2C_ADDR

// Simulate I2C EEPROM offset address 0x0 ~ 0x400
// Max memory locations available
#define EEPROM_MAX_ADDR              1024

// Max bytes to write in one step
#define EEPROM_MAX_WRITE             16

#define MAX_XFER_SZ                  256

#define MEMSET(s, c, n)              __builtin_memset ((s), (c), (n))
#define MEMCPY(des, src, n)          __builtin_memcpy((des), (src), (n))

#define FAKE_CPUFREQ_S		PCLKFREQ		// PCLK
#define SLAVE_LATENCY_MS	100
#define DELAY_CYCLE		    (SLAVE_LATENCY_MS * (FAKE_CPUFREQ_S/1000))

// I2C slave TX/RX flag
typedef enum _SLAVE_TRX_FLAG_ID
{
	I2C_SLAVE_TX_NONE = 0,
	I2C_SLAVE_RX_NONE = 0,
	I2C_SLAVE_TX_START,
	I2C_SLAVE_TX_COMPLETE,
	I2C_SLAVE_RX_START,
	I2C_SLAVE_RX_COMPLETE,
} SLAVE_TRX_FLAG_ID;

extern AE350_DRIVER_I2C Driver_I2C;					// I2C
static AE350_DRIVER_I2C* pDrv_I2C = &Driver_I2C;	// A pointer to I2C


/*************************************** Master ****************************************/
#if I2C_MASTER_MODE_TEST
// Test I2C master mode

static uint16_t Device_Addr = I2C_SLAVE_ADDR;
static uint8_t wr_buf[EEPROM_MAX_WRITE + 2] = {0};

// 2 byte address since EEPROM_MAX_ADDR is 1024
static volatile uint16_t EEPROM_Addr = 0x0216;

// 10 byte data
static volatile uint8_t EEPROM_Data[10] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x5A, 0xA5};
static volatile uint8_t Tmp[10] = {0};

// Master transmit
int32_t i2c_master_tx(uint16_t addr, const uint8_t* buf, uint32_t len)
{
	// 2 bytes for simulate I2C EEPROM offset address 0x0 ~ 0x400
	wr_buf[0] = (uint8_t)(addr >> 8);
	wr_buf[1] = (uint8_t)(addr & 0xFF);

	MEMCPY(&wr_buf[2], &buf[0], len);

	pDrv_I2C->MasterTransmit(Device_Addr, wr_buf, len + 2, false);
	while(pDrv_I2C->GetStatus().busy);

	if(pDrv_I2C->GetDataCount() != (len + 2))
	{
		return -1;
	}

	return 0;
}

// Master receive
int32_t i2c_master_rx(uint16_t addr, uint8_t* buf, uint32_t len)
{
	uint8_t a[2] = {0};

	a[0] = (uint8_t)(addr >> 8);
	a[1] = (uint8_t)(addr & 0xFF);

	// xfer_pending is true => no stop condition, still busy
	// since xfer_pending is true, no stop condition and busy bit will always be pull high
	// 10-bit slave address must set STOP bit
	// tell EEPROM which EEPROM offset address will be read
	pDrv_I2C->MasterTransmit(Device_Addr, a, 2, true);
	while(pDrv_I2C->GetStatus().busy);

	if(pDrv_I2C->GetDataCount() != 2)
	{
		return -1;
	}

	// Read-request to read the data from the EEPROM offset address
	pDrv_I2C->MasterReceive(Device_Addr, buf, len, false);
	while(pDrv_I2C->GetStatus().busy);

	if(pDrv_I2C->GetDataCount() != len)
	{
		return -1;
	}

	return 0;
}

// Initializes I2C master mode
int32_t i2c_master_init(void)
{
	int32_t status = AE350_DRIVER_OK;

	// Default slave mode
	status = pDrv_I2C->Initialize(NULL);
	if(status != AE350_DRIVER_OK)
	{
		return status;
	}

	// Clear
	status = pDrv_I2C->Control(AE350_I2C_BUS_CLEAR, 0);
	if(status != AE350_DRIVER_OK)
	{
		return status;
	}

	// Power full
	status = pDrv_I2C->PowerControl(AE350_POWER_FULL);
	if(status != AE350_DRIVER_OK)
	{
		return status;
	}

	// Speed: standard
	status = pDrv_I2C->Control(AE350_I2C_BUS_SPEED, AE350_I2C_BUS_SPEED_STANDARD);
	if(status != AE350_DRIVER_OK)
	{
		return status;
	}

	Device_Addr = I2C_SLAVE_ADDR;

	// Simulate I2C slave EEPROM device
	// Simulate I2C slave address
#if I2C_10BIT_MODE
	status = pDrv_I2C->Control(AE350_I2C_OWN_ADDRESS, (Device_Addr | AE350_I2C_ADDRESS_10BIT));
#else
	status = pDrv_I2C->Control(AE350_I2C_OWN_ADDRESS, (Device_Addr));
#endif
	if(status != AE350_DRIVER_OK)
	{
		return status;
	}

	return status;
}

// Master mode
int i2c_master_mode(void)
{
	volatile uint32_t time_start = 0;
	volatile uint32_t time_end = 0;
	int32_t status = 0, i = 0;

	// Initializes UART
	uart_init(38400);	// Baud rate is 38400

	printf("\r\nIt's a I2C Master Mode demo.\r\n");
	printf("\r\n====Please hardwire 2 test board=====\r\n\r\n");

	if(pDrv_I2C)
	{
		printf("I2C master mode init .....\r\n");

		status = i2c_master_init();
		if(status != AE350_DRIVER_OK)
		{
			printf("I2C init error.....\r\n");
			while(1);
		}

		printf("I2C FIFO master tx .....\r\n");

		status = i2c_master_tx(EEPROM_Addr, (uint8_t*)&EEPROM_Data, sizeof(EEPROM_Data));

		time_start = DEV_PLMT->MTIME[0];			//
		time_end = DEV_PLMT->MTIME[0];				//

		printf("I2C FIFO master rx .....\r\n");

		// Waiting slave to complete the sending setting
		while (time_end < (time_start + DELAY_CYCLE))
		{
			time_end = DEV_PLMT->MTIME[0];			//
		}

		status = i2c_master_rx(EEPROM_Addr, (uint8_t*)&Tmp, sizeof(Tmp));

		for(i = 0; i < 10; i++)
		{
			// Error hit
			if(EEPROM_Data[i] != Tmp[i])
			{
				printf("I2C FIFO master test fail.....\r\n");
				while(1);
			}
		}

		printf("I2C FIFO master test pass .....\r\n");

		// Exit demo
		pDrv_I2C->Uninitialize();
	}
	else
	{
		// pDrv_I2C is NULL
	}

	return 0;
}

/*************************************** Master ****************************************/


/*************************************** Slave ****************************************/
#elif I2C_SLAVE_MODE_TEST
// Test I2C slave mode

// Simulate I2C slave EEPROM device
static volatile uint16_t offset_addr = 0;
static volatile uint8_t slave_eeprom[EEPROM_MAX_ADDR] = {0};
static volatile int slave_tx = I2C_SLAVE_TX_NONE;
static volatile int slave_rx = I2C_SLAVE_RX_NONE;

// Callback function
void i2c_callback(uint32_t event)
{
	// Callback function body
	if(event & AE350_I2C_EVENT_TRANSFER_INCOMPLETE)
	{
		if(event & AE350_I2C_EVENT_SLAVE_RECEIVE)
		{
			// Slave address hit is the entry for slave mode driver
			// to detect slave RX/TX action depend on master TX/RX action
			slave_rx = I2C_SLAVE_RX_START;
		}
		else if(event & AE350_I2C_EVENT_SLAVE_TRANSMIT)
		{
			// Slave address hit is the entry for slave mode driver
			// to detect slave RX/TX action depend on master TX/RX action
			slave_tx = I2C_SLAVE_TX_START;
		}
	}
	else if(event & AE350_I2C_EVENT_TRANSFER_DONE)
	{
		if(slave_rx == I2C_SLAVE_RX_START)
		{
			slave_rx = I2C_SLAVE_RX_COMPLETE;
		}

		if(slave_tx == I2C_SLAVE_TX_START)
		{
			slave_tx = I2C_SLAVE_TX_COMPLETE;
		}
	}
	else if(event & AE350_I2C_EVENT_ADDRESS_NACK)
	{
		if(slave_tx == I2C_SLAVE_TX_START)
		{
			slave_tx = I2C_SLAVE_TX_COMPLETE;
		}
	}
}

// Slave transmit
int32_t i2c_slave_tx(void)
{
	uint16_t r_addr = offset_addr;

	do
	{
		pDrv_I2C->SlaveTransmit((uint8_t*)&slave_eeprom[r_addr], 1);
		r_addr++;

		while(pDrv_I2C->GetStatus().busy);

		// Error hit
		if((r_addr - offset_addr) > MAX_XFER_SZ)
		{
			while(1);
		}
	}while(slave_tx != I2C_SLAVE_TX_COMPLETE);

	return 0;
}

// Slave receive
int32_t i2c_slave_rx(void)
{
	uint32_t data_count = 0;
	uint8_t tmp[2] = {0};
	uint16_t w_addr = 0;

	// Master device issue write-request w/ flash address,
	// then issue read-request to read flash data from the address
	offset_addr = 0;

	// A new I2C data transaction(start-address-data-stop)
	data_count = pDrv_I2C->GetDataCount();

	// Error hit
	if(data_count > MAX_XFER_SZ)
	{
		while(1);
	}

	if(data_count > 0)
	{
		if(data_count >= 2)
		{
			// The first 2 bytes are flash address
			// High byte
			pDrv_I2C->SlaveReceive(&tmp[0], 1);
			// Low byte
			pDrv_I2C->SlaveReceive(&tmp[1], 1);

			w_addr = offset_addr = ((tmp[0] << 8) | tmp[1]);
			pDrv_I2C->SlaveReceive((uint8_t*)&slave_eeprom[w_addr], data_count - 2);
			w_addr += (data_count - 2);
		}
		else
		{
			pDrv_I2C->SlaveReceive((uint8_t*)&slave_eeprom[0], data_count);
		}
	}
	// I2C data transaction w/o pay load data
	else
	{
	}

	return 0;
}

// Initializes I2C slave mode
int32_t i2c_slave_init(void)
{
	int32_t status = AE350_DRIVER_OK;

	// Default slave mode
	status = pDrv_I2C->Initialize(i2c_callback);
	if(status != AE350_DRIVER_OK)
	{
		return status;
	}

	// Clear
	status = pDrv_I2C->Control(AE350_I2C_BUS_CLEAR, 0);
	if(status != AE350_DRIVER_OK)
	{
		return status;
	}

	// Power full
	status = pDrv_I2C->PowerControl(AE350_POWER_FULL);
	if(status != AE350_DRIVER_OK)
	{
		return status;
	}

	// Speed: standard
	status = pDrv_I2C->Control(AE350_I2C_BUS_SPEED, AE350_I2C_BUS_SPEED_STANDARD);
	if(status != AE350_DRIVER_OK)
	{
		return status;
	}

	// Simulate I2C slave EEPROM device
	// Simulate I2C slave address
#if I2C_10BIT_MODE
	status = pDrv_I2C->Control(AE350_I2C_OWN_ADDRESS, (I2C_SLAVE_ADDR | AE350_I2C_ADDRESS_10BIT));
#else
	status = pDrv_I2C->Control(AE350_I2C_OWN_ADDRESS, (I2C_SLAVE_ADDR));
#endif
	if(status != AE350_DRIVER_OK)
	{
		return status;
	}

	return status;
}

// Slave mode
int i2c_slave_mode(void)
{
	int32_t status = 0;

	// Initializes UART
	uart_init(38400);	// Baud rate is 38400

	printf("\r\nIt's a I2C Slave Mode demo.\r\n");
	printf("\r\n====Please hardwire 2 test board=====\r\n\r\n");

	if(pDrv_I2C)
	{
		printf("I2C slave mode init .....\r\n");

		status = i2c_slave_init();
		if(status != AE350_DRIVER_OK)
		{
			while(1);
		}

		do
		{
			if(slave_rx == I2C_SLAVE_RX_COMPLETE)
			{
				// A new I2C data transaction(start-address-data-stop)
				i2c_slave_rx();
				slave_rx = I2C_SLAVE_RX_NONE;
			}

			if(slave_tx == I2C_SLAVE_TX_START)
			{
				// A new I2C data transaction(start-address-data-stop)
				i2c_slave_tx();
				slave_tx = I2C_SLAVE_TX_NONE;
			}
		}while(1);
	}
	else
	{
		// pDrv_I2C is NULL
	}

	return 0;
}
#endif

/*************************************** Slave ****************************************/


// Application entry function
int demo_i2c(void)
{
#if I2C_MASTER_MODE_TEST
	// Master mode
	i2c_master_mode();
#elif I2C_SLAVE_MODE_TEST
	// Slave mode
	i2c_slave_mode();
#endif

	return 0;
}

#endif	/* RUN_DEMO_I2C */
