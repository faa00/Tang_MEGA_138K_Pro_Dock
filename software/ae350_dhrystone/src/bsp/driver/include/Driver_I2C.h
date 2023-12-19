/*
 * ******************************************************************************************
 * File		: Driver_I2C.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: I2C driver definitions
 * ******************************************************************************************
 */

#ifndef __DRIVER_I2C_H__
#define __DRIVER_I2C_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_Common.h"


// Definitions  -----------------------------------------------------------------------------

#define AE350_I2C_API_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,02)  /* API version */


/****** I2C Control Codes *****/

#define AE350_I2C_OWN_ADDRESS             (0x01)      	// Set own slave address; arg = address
#define AE350_I2C_BUS_SPEED               (0x02)      	// Set bus speed; arg = speed
#define AE350_I2C_BUS_CLEAR               (0x03)      	// Execute bus clear: send nine clock pulses
#define AE350_I2C_ABORT_TRANSFER          (0x04)      	// Abort master/slave transmit/receive

/*----- I2C Bus Speed -----*/
#define AE350_I2C_BUS_SPEED_STANDARD      (0x01)      	// Standard speed (100kHz)
#define AE350_I2C_BUS_SPEED_FAST          (0x02)      	// Fast speed     (400kHz)
#define AE350_I2C_BUS_SPEED_FAST_PLUS     (0x03)      	// Fast+ speed    (  1MHz)
#define AE350_I2C_BUS_SPEED_HIGH          (0x04)      	// High speed     (3.4MHz)


/****** I2C Address Flags *****/
#define AE350_I2C_ADDRESS_10BIT           0x0400      	// 10-bit address flag
#define AE350_I2C_ADDRESS_GC              0x8000      	// General call flag


/***************************************************************************
\brief I2C Status
****************************************************************************/
typedef struct _AE350_I2C_STATUS
{
	uint32_t busy             : 1;        				// Busy flag
	uint32_t mode             : 1;        				// Mode: 0=Slave, 1=Master
	uint32_t direction        : 2;        				// Direction: 0=non, 1=Receiver, 2=Transmitter
	uint32_t general_call     : 1;        				// General call indication (cleared on start of next Slave operation)
	uint32_t arbitration_lost : 1;        				// Master lost arbitration (cleared on start of next Master operation)
	uint32_t bus_error        : 1;        				// Bus error detected (cleared on start of next Master/Slave operation)
} AE350_I2C_STATUS;


/****** I2C Event *****/
#define AE350_I2C_EVENT_TRANSFER_DONE       (1UL << 0)  // Master/Slave Transmit/Receive finished
#define AE350_I2C_EVENT_TRANSFER_INCOMPLETE (1UL << 1)  // Master/Slave Transmit/Receive incomplete transfer
#define AE350_I2C_EVENT_SLAVE_TRANSMIT      (1UL << 2)  // Slave Transmit operation requested
#define AE350_I2C_EVENT_SLAVE_RECEIVE       (1UL << 3)  // Slave Receive operation requested
#define AE350_I2C_EVENT_ADDRESS_NACK        (1UL << 4)  // Address not acknowledged from Slave
#define AE350_I2C_EVENT_GENERAL_CALL        (1UL << 5)  // General call indication
#define AE350_I2C_EVENT_ARBITRATION_LOST    (1UL << 6)  // Master lost arbitration
#define AE350_I2C_EVENT_BUS_ERROR           (1UL << 7)  // Bus error detected (START/STOP at illegal position)
#define AE350_I2C_EVENT_BUS_CLEAR           (1UL << 8)  // Bus clear finished


// Function documentation
/****************************************************************************
  \fn          void AE350_I2C_SignalEvent (uint32_t event)
  \brief       Signal I2C Events.
  \param[in]   event  \ref I2C_events notification mask
*****************************************************************************/
typedef void (*AE350_I2C_SignalEvent_t) (uint32_t event);  // Pointer to \ref AE350_I2C_SignalEvent : Signal I2C Event.


/****************************************************************************
\brief I2C Driver Capabilities.
*****************************************************************************/
typedef struct _AE350_I2C_CAPABILITIES
{
	uint32_t address_10_bit : 1;          // Supports 10-bit addressing
} AE350_I2C_CAPABILITIES;


/****************************************************************************
\brief Access structure of the I2C Driver.
*****************************************************************************/
typedef struct _AE350_DRIVER_I2C
{
	AE350_DRIVER_VERSION   (*GetVersion)     (void);                                                                // Get driver version
	AE350_I2C_CAPABILITIES (*GetCapabilities)(void);                                                                // Get driver capabilities
	int32_t                (*Initialize)     (AE350_I2C_SignalEvent_t cb_event);                                    // Initialize I2C interface
	int32_t                (*Uninitialize)   (void);                                                                // Uninitialized I2C interface
	int32_t                (*PowerControl)   (AE350_POWER_STATE state);                                             // Control I2C interface power
	int32_t                (*MasterTransmit) (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending); // Start transmitting data as I2C Master
	int32_t                (*MasterReceive)  (uint32_t addr,       uint8_t *data, uint32_t num, bool xfer_pending); // Start receiving data as I2C Master
	int32_t                (*SlaveTransmit)  (               const uint8_t *data, uint32_t num);                    // Start transmitting data as I2C Slave
	int32_t                (*SlaveReceive)   (                     uint8_t *data, uint32_t num);                    // Start receiving data as I2C Slave
	uint32_t               (*GetDataCount)   (void);                                                                // Get transferred data count
	int32_t                (*Control)        (uint32_t control, uint32_t arg);                                      // Control I2C interface
	AE350_I2C_STATUS       (*GetStatus)      (void);                                                                // Get I2C status
} const AE350_DRIVER_I2C;


#endif /* __DRIVER_I2C_H__ */
