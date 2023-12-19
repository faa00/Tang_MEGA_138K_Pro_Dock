/*
 * ******************************************************************************************
 * File		: Driver_SPI.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: SPI driver definitions
 * ******************************************************************************************
 */

#ifndef __DRIVER_SPI_H__
#define __DRIVER_SPI_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_Common.h"


// Definitions  -----------------------------------------------------------------------------

#define AE350_SPI_API_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,01)  /* API version */


/****** SPI Control Codes *****/

#define AE350_SPI_CONTROL_Pos              0
#define AE350_SPI_CONTROL_Msk             (0xFFUL << AE350_SPI_CONTROL_Pos)

/*----- SPI Control Codes: Mode -----*/
#define AE350_SPI_MODE_INACTIVE           (0x00UL << AE350_SPI_CONTROL_Pos)      	// SPI inactive
#define AE350_SPI_MODE_MASTER             (0x01UL << AE350_SPI_CONTROL_Pos)      	// SPI Master (output on MOSI, input on MISO); arg = bus speed in bps
#define AE350_SPI_MODE_SLAVE              (0x02UL << AE350_SPI_CONTROL_Pos)      	// SPI Slave  (output on MISO, input on MOSI)
#define AE350_SPI_MODE_MASTER_SIMPLEX     (0x03UL << AE350_SPI_CONTROL_Pos)      	// SPI Master (output/input on MOSI); arg = bus speed in bps
#define AE350_SPI_MODE_SLAVE_SIMPLEX      (0x04UL << AE350_SPI_CONTROL_Pos)      	// SPI Slave  (output/input on MISO)

/*----- SPI Control Codes: Mode Parameters: Frame Format -----*/
#define AE350_SPI_FRAME_FORMAT_Pos         8
#define AE350_SPI_FRAME_FORMAT_Msk        (7UL << AE350_SPI_FRAME_FORMAT_Pos)
#define AE350_SPI_CPOL0_CPHA0             (0UL << AE350_SPI_FRAME_FORMAT_Pos)    	// Clock polarity 0, clock phase 0 (default)
#define AE350_SPI_CPOL0_CPHA1             (1UL << AE350_SPI_FRAME_FORMAT_Pos)    	// Clock polarity 0, clock phase 1
#define AE350_SPI_CPOL1_CPHA0             (2UL << AE350_SPI_FRAME_FORMAT_Pos)    	// Clock polarity 1, clock phase 0
#define AE350_SPI_CPOL1_CPHA1             (3UL << AE350_SPI_FRAME_FORMAT_Pos)    	// Clock polarity 1, clock phase 1
#define AE350_SPI_TI_SSI                  (4UL << AE350_SPI_FRAME_FORMAT_Pos)    	// Texas instruments frame format
#define AE350_SPI_MICROWIRE               (5UL << AE350_SPI_FRAME_FORMAT_Pos)    	// National microwire frame format

/*----- SPI Control Codes: Mode Parameters: Data Bits -----*/
#define AE350_SPI_DATA_BITS_Pos            12
#define AE350_SPI_DATA_BITS_Msk           (0x3FUL << AE350_SPI_DATA_BITS_Pos)
#define AE350_SPI_DATA_BITS(n)            (((n) & 0x3F) << AE350_SPI_DATA_BITS_Pos) // Number of data bits

/*----- SPI Control Codes: Mode Parameters: Bit Order -----*/
#define AE350_SPI_BIT_ORDER_Pos            18
#define AE350_SPI_BIT_ORDER_Msk           (1UL << AE350_SPI_BIT_ORDER_Pos)
#define AE350_SPI_MSB_LSB                 (0UL << AE350_SPI_BIT_ORDER_Pos)      	// SPI bit order from MSB to LSB (default)
#define AE350_SPI_LSB_MSB                 (1UL << AE350_SPI_BIT_ORDER_Pos)     	 	// SPI bit order from LSB to MSB

/*----- SPI Control Codes: Mode Parameters: Slave Select Mode -----*/
#define AE350_SPI_SS_MASTER_MODE_Pos       19
#define AE350_SPI_SS_MASTER_MODE_Msk      (3UL << AE350_SPI_SS_MASTER_MODE_Pos)
#define AE350_SPI_SS_MASTER_UNUSED        (1UL << AE350_SPI_SS_MASTER_MODE_Pos) 	// SPI Slave select when Master: not used
#define AE350_SPI_SS_MASTER_SW            (2UL << AE350_SPI_SS_MASTER_MODE_Pos) 	// SPI Slave select when Master: software controlled
#define AE350_SPI_SS_MASTER_HW_OUTPUT     (0UL << AE350_SPI_SS_MASTER_MODE_Pos) 	// SPI Slave select when Master: hardware controlled output (default)
#define AE350_SPI_SS_MASTER_HW_INPUT      (3UL << AE350_SPI_SS_MASTER_MODE_Pos) 	// SPI Slave select when Master: hardware monitored input

#define AE350_SPI_SS_SLAVE_MODE_Pos        21
#define AE350_SPI_SS_SLAVE_MODE_Msk       (1UL << AE350_SPI_SS_SLAVE_MODE_Pos)
#define AE350_SPI_SS_SLAVE_HW             (0UL << AE350_SPI_SS_SLAVE_MODE_Pos)  	// SPI Slave select when Slave: hardware monitored (default)
#define AE350_SPI_SS_SLAVE_SW             (1UL << AE350_SPI_SS_SLAVE_MODE_Pos)  	// SPI Slave select when Slave: software controlled

/*----- SPI Control Codes: Mode Parameters: Transfer Format Phase -----*/
#define AE350_SPI_TRANSFER_FORMAT_Pos      22
#define AE350_SPI_TRANSFER_FORMAT_Msk     (1UL << AE350_SPI_TRANSFER_FORMAT_Pos)
#define AE350_SPI_SLV_DATA_ONLY_TRANSFER  (1UL << AE350_SPI_TRANSFER_FORMAT_Pos)	// (Slave mode only) SPI transaction only consists of data without command and address phase.

/*----- SPI Control Codes: Miscellaneous Controls  -----*/
#define AE350_SPI_SET_BUS_SPEED           (0x10UL << AE350_SPI_CONTROL_Pos)     	// Set bus speed in bps; arg = value
#define AE350_SPI_GET_BUS_SPEED           (0x11UL << AE350_SPI_CONTROL_Pos)     	// Get bus speed in bps
#define AE350_SPI_SET_DEFAULT_TX_VALUE    (0x12UL << AE350_SPI_CONTROL_Pos)     	// Set default transmit value; arg = value
#define AE350_SPI_CONTROL_SS              (0x13UL << AE350_SPI_CONTROL_Pos)     	// Control Slave select; arg: 0=inactive, 1=active
#define AE350_SPI_ABORT_TRANSFER          (0x14UL << AE350_SPI_CONTROL_Pos)     	// Abort current data transfer
#define AE350_SPI_TX_HEADER_LENGTH		  (0x20UL << AE350_SPI_CONTROL_Pos)

/****** SPI Slave Select Signal definitions *****/
#define AE350_SPI_SS_INACTIVE              0                                    	// SPI Slave select signal inactive
#define AE350_SPI_SS_ACTIVE                1                                    	// SPI Slave select signal active

/****** SPI specific error codes *****/
#define AE350_SPI_ERROR_MODE              (AE350_DRIVER_ERROR_SPECIFIC - 1)     	// Specified mode not supported
#define AE350_SPI_ERROR_FRAME_FORMAT      (AE350_DRIVER_ERROR_SPECIFIC - 2)     	// Specified frame format not supported
#define AE350_SPI_ERROR_DATA_BITS         (AE350_DRIVER_ERROR_SPECIFIC - 3)     	// Specified number of data bits not supported
#define AE350_SPI_ERROR_BIT_ORDER         (AE350_DRIVER_ERROR_SPECIFIC - 4)     	// Specified bit order not supported
#define AE350_SPI_ERROR_SS_MODE           (AE350_DRIVER_ERROR_SPECIFIC - 5)     	// Specified Slave select mode not supported
#define AE350_SPI_ERROR_HEADER_LEN		  (AE350_DRIVER_ERROR_SPECIFIC - 6)


/*****************************************************************************
\brief SPI Status
******************************************************************************/
typedef struct _AE350_SPI_STATUS
{
	uint32_t busy       : 1;              // Transmitter/Receiver busy flag
	uint32_t data_lost  : 1;              // Data lost: Receive overflow / Transmit underflow (cleared on start of transfer operation)
	uint32_t mode_fault : 1;              // Mode fault detected; optional (cleared on start of transfer operation)
} AE350_SPI_STATUS;


/****** SPI Event *****/
#define AE350_SPI_EVENT_TRANSFER_COMPLETE (1UL << 0)  // Data transfer completed
#define AE350_SPI_EVENT_DATA_LOST         (1UL << 1)  // Data lost: Receive overflow / Transmit underflow
#define AE350_SPI_EVENT_MODE_FAULT        (1UL << 2)  // Master mode fault (SS deactivated when Master)



/******************************************************************************
  \fn          void AE350_SPI_SignalEvent (uint32_t event)
  \brief       Signal SPI Events.
  \param[in]   event \ref SPI_events notification mask
  \return      none
*******************************************************************************/
typedef void (*AE350_SPI_SignalEvent_t) (uint32_t event);  // Pointer to \ref AE350_SPI_SignalEvent : Signal SPI Event.


/******************************************************************************
\brief SPI Driver Capabilities.
*******************************************************************************/
typedef struct _AE350_SPI_CAPABILITIES
{
	uint32_t simplex          : 1;        // Supports simplex mode (Master and Slave)
	uint32_t ti_ssi           : 1;        // Supports TI synchronous serial interface
	uint32_t microwire        : 1;        // Supports microwire interface
	uint32_t event_mode_fault : 1;        // Signal mode fault event: \ref AE350_SPI_EVENT_MODE_FAULT
} AE350_SPI_CAPABILITIES;


/******************************************************************************
\brief Access structure of the SPI Driver.
*******************************************************************************/
typedef struct _AE350_DRIVER_SPI
{
	AE350_DRIVER_VERSION   (*GetVersion)      (void);                             	// Get driver version
	AE350_SPI_CAPABILITIES (*GetCapabilities) (void);                             	// Get driver capabilities
	int32_t                (*Initialize)      (AE350_SPI_SignalEvent_t cb_event);   // Initialize SPI interface
	int32_t                (*Uninitialize)    (void);                             	// Uninitialized SPI interface
	int32_t                (*PowerControl)    (AE350_POWER_STATE state);            // Control SPI interface power
	int32_t                (*Send)            (const void *data, uint32_t num);   	// Start sending data to SPI interface
	int32_t                (*Receive)         (void *data, uint32_t num);   		// Start receiving data from SPI interface
	int32_t                (*Transfer)        (const void *data_out,
                                               void *data_in,
                                               uint32_t num);                  		// Start sending/receiving data to/from SPI
	uint32_t               (*GetDataCount)    (void);                             	// Get transferred data count
	int32_t                (*Control)         (uint32_t control, uint32_t arg);   	// Control SPI interface
	AE350_SPI_STATUS       (*GetStatus)       (void);                             	// Get SPI status
} const AE350_DRIVER_SPI;


#endif /* __DRIVER_SPI_H__ */
