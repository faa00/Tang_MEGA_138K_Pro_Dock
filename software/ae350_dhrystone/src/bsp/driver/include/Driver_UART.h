/*
 * ******************************************************************************************
 * File		: Driver_UART.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: UART driver definitions
 * ******************************************************************************************
 */

#ifndef __DRIVER_UART_H__
#define __DRIVER_UART_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_Common.h"


// Definitions  -----------------------------------------------------------------------------

#define AE350_UART_API_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,02)  /* API version */


/****** UART Control Codes *****/

#define AE350_UART_CONTROL_Pos                0
#define AE350_UART_CONTROL_Msk               (0xFFUL << AE350_UART_CONTROL_Pos)

/*----- UART Control Codes: Mode -----*/
#define AE350_UART_MODE_ASYNCHRONOUS         (0x01UL << AE350_UART_CONTROL_Pos)   // UART (Asynchronous); arg = Baudrate
#define AE350_UART_MODE_SYNCHRONOUS_MASTER   (0x02UL << AE350_UART_CONTROL_Pos)   // Synchronous Master (generates clock signal); arg = Baud rate
#define AE350_UART_MODE_SYNCHRONOUS_SLAVE    (0x03UL << AE350_UART_CONTROL_Pos)   // Synchronous Slave (external clock signal)
#define AE350_UART_MODE_SINGLE_WIRE          (0x04UL << AE350_UART_CONTROL_Pos)   // UART single-wire (half-duplex); arg = Baud rate
#define AE350_UART_MODE_IRDA                 (0x05UL << AE350_UART_CONTROL_Pos)   // UART IrDA; arg = Baud rate
#define AE350_UART_MODE_SMART_CARD           (0x06UL << AE350_UART_CONTROL_Pos)   // UART smart card; arg = Baud rate

/*----- UART Control Codes: Mode Parameters: Data Bits -----*/
#define AE350_UART_DATA_BITS_Pos              8
#define AE350_UART_DATA_BITS_Msk             (7UL << AE350_UART_DATA_BITS_Pos)
#define AE350_UART_DATA_BITS_5               (5UL << AE350_UART_DATA_BITS_Pos)    // 5 data bits
#define AE350_UART_DATA_BITS_6               (6UL << AE350_UART_DATA_BITS_Pos)    // 6 data bit
#define AE350_UART_DATA_BITS_7               (7UL << AE350_UART_DATA_BITS_Pos)    // 7 data bits
#define AE350_UART_DATA_BITS_8               (0UL << AE350_UART_DATA_BITS_Pos)    // 8 data bits (default)
#define AE350_UART_DATA_BITS_9               (1UL << AE350_UART_DATA_BITS_Pos)    // 9 data bits

/*----- UART Control Codes: Mode Parameters: Parity -----*/
#define AE350_UART_PARITY_Pos                 12
#define AE350_UART_PARITY_Msk                (3UL << AE350_UART_PARITY_Pos)
#define AE350_UART_PARITY_NONE               (0UL << AE350_UART_PARITY_Pos)       // No parity (default)
#define AE350_UART_PARITY_EVEN               (1UL << AE350_UART_PARITY_Pos)       // Even parity
#define AE350_UART_PARITY_ODD                (2UL << AE350_UART_PARITY_Pos)       // Odd parity

/*----- UART Control Codes: Mode Parameters: Stop Bits -----*/
#define AE350_UART_STOP_BITS_Pos              14
#define AE350_UART_STOP_BITS_Msk             (3UL << AE350_UART_STOP_BITS_Pos)
#define AE350_UART_STOP_BITS_1               (0UL << AE350_UART_STOP_BITS_Pos)    // 1 stop bit (default)
#define AE350_UART_STOP_BITS_2               (1UL << AE350_UART_STOP_BITS_Pos)    // 2 stop bits
#define AE350_UART_STOP_BITS_1_5             (2UL << AE350_UART_STOP_BITS_Pos)    // 1.5 stop bits
#define AE350_UART_STOP_BITS_0_5             (3UL << AE350_UART_STOP_BITS_Pos)    // 0.5 stop bits

/*----- UART Control Codes: Mode Parameters: Flow Control -----*/
#define AE350_UART_FLOW_CONTROL_Pos           16
#define AE350_UART_FLOW_CONTROL_Msk          (3UL << AE350_UART_FLOW_CONTROL_Pos)
#define AE350_UART_FLOW_CONTROL_NONE         (0UL << AE350_UART_FLOW_CONTROL_Pos) // No flow control (default)
#define AE350_UART_FLOW_CONTROL_RTS          (1UL << AE350_UART_FLOW_CONTROL_Pos) // RTS flow control
#define AE350_UART_FLOW_CONTROL_CTS          (2UL << AE350_UART_FLOW_CONTROL_Pos) // CTS flow control
#define AE350_UART_FLOW_CONTROL_RTS_CTS      (3UL << AE350_UART_FLOW_CONTROL_Pos) // RTS/CTS flow control

/*----- UART Control Codes: Mode Parameters: Clock Polarity (Synchronous mode) -----*/
#define AE350_UART_CPOL_Pos                   18
#define AE350_UART_CPOL_Msk                  (1UL << AE350_UART_CPOL_Pos)
#define AE350_UART_CPOL0                     (0UL << AE350_UART_CPOL_Pos)         // CPOL = 0 (default)
#define AE350_UART_CPOL1                     (1UL << AE350_UART_CPOL_Pos)         // CPOL = 1

/*----- UART Control Codes: Mode Parameters: Clock Phase (Synchronous mode) -----*/
#define AE350_UART_CPHA_Pos                   19
#define AE350_UART_CPHA_Msk                  (1UL << AE350_UART_CPHA_Pos)
#define AE350_UART_CPHA0                     (0UL << AE350_UART_CPHA_Pos)         // CPHA = 0 (default)
#define AE350_UART_CPHA1                     (1UL << AE350_UART_CPHA_Pos)         // CPHA = 1

/*----- UART Control Codes: Miscellaneous Controls  -----*/
#define AE350_UART_SET_DEFAULT_TX_VALUE      (0x10UL << AE350_UART_CONTROL_Pos)   // Set default transmit value (Synchronous Receive only); arg = value
#define AE350_UART_SET_IRDA_PULSE            (0x11UL << AE350_UART_CONTROL_Pos)   // Set IrDA pulse in ns; arg: 0=3/16 of bit period
#define AE350_UART_SET_SMART_CARD_GUARD_TIME (0x12UL << AE350_UART_CONTROL_Pos)   // Set smart card guard Time; arg = number of bit periods
#define AE350_UART_SET_SMART_CARD_CLOCK      (0x13UL << AE350_UART_CONTROL_Pos)   // Set smart card clock in Hz; arg: 0=Clock not generated
#define AE350_UART_CONTROL_SMART_CARD_NACK   (0x14UL << AE350_UART_CONTROL_Pos)   // Smart card NACK generation; arg: 0=disabled, 1=enabled
#define AE350_UART_CONTROL_TX                (0x15UL << AE350_UART_CONTROL_Pos)   // Transmitter; arg: 0=disabled, 1=enabled
#define AE350_UART_CONTROL_RX                (0x16UL << AE350_UART_CONTROL_Pos)   // Receiver; arg: 0=disabled, 1=enabled
#define AE350_UART_CONTROL_BREAK             (0x17UL << AE350_UART_CONTROL_Pos)   // Continuous break transmission; arg: 0=disabled, 1=enabled
#define AE350_UART_ABORT_SEND                (0x18UL << AE350_UART_CONTROL_Pos)   // Abort \ref AE350_UART_Send
#define AE350_UART_ABORT_RECEIVE             (0x19UL << AE350_UART_CONTROL_Pos)   // Abort \ref AE350_UART_Receive
#define AE350_UART_ABORT_TRANSFER            (0x1AUL << AE350_UART_CONTROL_Pos)   // Abort \ref AE350_UART_Transfer


/****** UART specific error codes *****/
#define AE350_UART_ERROR_MODE                (AE350_DRIVER_ERROR_SPECIFIC - 1)     // Specified mode not supported
#define AE350_UART_ERROR_BAUDRATE            (AE350_DRIVER_ERROR_SPECIFIC - 2)     // Specified baud rate not supported
#define AE350_UART_ERROR_DATA_BITS           (AE350_DRIVER_ERROR_SPECIFIC - 3)     // Specified number of data bits not supported
#define AE350_UART_ERROR_PARITY              (AE350_DRIVER_ERROR_SPECIFIC - 4)     // Specified parity not supported
#define AE350_UART_ERROR_STOP_BITS           (AE350_DRIVER_ERROR_SPECIFIC - 5)     // Specified number of stop bits not supported
#define AE350_UART_ERROR_FLOW_CONTROL        (AE350_DRIVER_ERROR_SPECIFIC - 6)     // Specified flow control not supported
#define AE350_UART_ERROR_CPOL                (AE350_DRIVER_ERROR_SPECIFIC - 7)     // Specified clock polarity not supported
#define AE350_UART_ERROR_CPHA                (AE350_DRIVER_ERROR_SPECIFIC - 8)     // Specified clock phase not supported


/***************************************************************************
\brief UART Status
****************************************************************************/
typedef struct _AE350_UART_STATUS
{
	uint32_t tx_busy          : 1;        // Transmitter busy flag
	uint32_t rx_busy          : 1;        // Receiver busy flag
	uint32_t tx_underflow     : 1;        // Transmit data underflow detected (cleared on start of next send operation)
	uint32_t rx_overflow      : 1;        // Receive data overflow detected (cleared on start of next receive operation)
	uint32_t rx_break         : 1;        // Break detected on receive (cleared on start of next receive operation)
	uint32_t rx_framing_error : 1;        // Framing error detected on receive (cleared on start of next receive operation)
	uint32_t rx_parity_error  : 1;        // Parity error detected on receive (cleared on start of next receive operation)
} AE350_UART_STATUS;

/****************************************************************************
\brief UART Modem Control
*****************************************************************************/
typedef enum _AE350_UART_MODEM_CONTROL
{
	AE350_UART_RTS_CLEAR,                  // Deactivate RTS
	AE350_UART_RTS_SET,                    // Activate RTS
	AE350_UART_DTR_CLEAR,                  // Deactivate DTR
	AE350_UART_DTR_SET                     // Activate DTR
} AE350_UART_MODEM_CONTROL;

/****************************************************************************
\brief UART Modem Status
*****************************************************************************/
typedef struct _AE350_UART_MODEM_STATUS
{
	uint32_t cts : 1;                     // CTS state: 1=Active, 0=Inactive
	uint32_t dsr : 1;                     // DSR state: 1=Active, 0=Inactive
	uint32_t dcd : 1;                     // DCD state: 1=Active, 0=Inactive
	uint32_t ri  : 1;                     // RI  state: 1=Active, 0=Inactive
} AE350_UART_MODEM_STATUS;


/****** UART Event *****/
#define AE350_UART_EVENT_SEND_COMPLETE       (1UL << 0)  // Send completed; however UART may still transmit data
#define AE350_UART_EVENT_RECEIVE_COMPLETE    (1UL << 1)  // Receive completed
#define AE350_UART_EVENT_TRANSFER_COMPLETE   (1UL << 2)  // Transfer completed
#define AE350_UART_EVENT_TX_COMPLETE         (1UL << 3)  // Transmit completed (optional)
#define AE350_UART_EVENT_TX_UNDERFLOW        (1UL << 4)  // Transmit data not available (Synchronous Slave)
#define AE350_UART_EVENT_RX_OVERFLOW         (1UL << 5)  // Receive data overflow
#define AE350_UART_EVENT_RX_TIMEOUT          (1UL << 6)  // Receive character timeout (optional)
#define AE350_UART_EVENT_RX_BREAK            (1UL << 7)  // Break detected on receive
#define AE350_UART_EVENT_RX_FRAMING_ERROR    (1UL << 8)  // Framing error detected on receive
#define AE350_UART_EVENT_RX_PARITY_ERROR     (1UL << 9)  // Parity error detected on receive
#define AE350_UART_EVENT_CTS                 (1UL << 10) // CTS state changed (optional)
#define AE350_UART_EVENT_DSR                 (1UL << 11) // DSR state changed (optional)
#define AE350_UART_EVENT_DCD                 (1UL << 12) // DCD state changed (optional)
#define AE350_UART_EVENT_RI                  (1UL << 13) // RI  state changed (optional)


// Function documentation
/****************************************************************************
  \fn          void AE350_UART_SignalEvent (uint32_t event)
  \brief       Signal UART Events.
  \param[in]   event  \ref UART_events notification mask
  \return      none
*****************************************************************************/
typedef void (*AE350_UART_SignalEvent_t) (uint32_t event);  // Pointer to \ref AE350_UART_SignalEvent : Signal UART Event.


/****************************************************************************
\brief UART Device Driver Capabilities.
*****************************************************************************/
typedef struct _AE350_UART_CAPABILITIES
{
	uint32_t asynchronous       : 1;      // Supports UART (Asynchronous) mode
	uint32_t synchronous_master : 1;      // Supports synchronous Master mode
	uint32_t synchronous_slave  : 1;      // Supports synchronous Slave mode
	uint32_t single_wire        : 1;      // Supports UART single-wire mode
	uint32_t irda               : 1;      // Supports UART IrDA mode
	uint32_t smart_card         : 1;      // Supports UART smart card mode
	uint32_t smart_card_clock   : 1;      // Smart card clock generator available
	uint32_t flow_control_rts   : 1;      // RTS flow control available
	uint32_t flow_control_cts   : 1;      // CTS flow control available
	uint32_t event_tx_complete  : 1;      // Transmit completed event: \ref AE350_UART_EVENT_TX_COMPLETE
	uint32_t event_rx_timeout   : 1;      // Signal receive character timeout event: \ref AE350_UART_EVENT_RX_TIMEOUT
	uint32_t rts                : 1;      // RTS line: 0=not available, 1=available
	uint32_t cts                : 1;      // CTS line: 0=not available, 1=available
	uint32_t dtr                : 1;      // DTR line: 0=not available, 1=available
	uint32_t dsr                : 1;      // DSR line: 0=not available, 1=available
	uint32_t dcd                : 1;      // DCD line: 0=not available, 1=available
	uint32_t ri                 : 1;      // RI line: 0=not available, 1=available
	uint32_t event_cts          : 1;      // Signal CTS change event: \ref AE350_UART_EVENT_CTS
	uint32_t event_dsr          : 1;      // Signal DSR change event: \ref AE350_UART_EVENT_DSR
	uint32_t event_dcd          : 1;      // Signal DCD change event: \ref AE350_UART_EVENT_DCD
	uint32_t event_ri           : 1;      // Signal RI change event: \ref AE350_UART_EVENT_RI
} AE350_UART_CAPABILITIES;


/****************************************************************************
\brief Access structure of the UART Driver.
*****************************************************************************/
typedef struct _AE350_DRIVER_UART
{
	AE350_DRIVER_VERSION     (*GetVersion)      (void);                              					// Get driver version
	AE350_UART_CAPABILITIES  (*GetCapabilities) (void);                              					// Get driver capabilities
	int32_t                  (*Initialize)      (AE350_UART_SignalEvent_t cb_event);  					// Initialize UART interface
	int32_t                  (*Uninitialize)    (void);                              					// Uninitialized UART interface
	int32_t                  (*PowerControl)    (AE350_POWER_STATE state);             					// Control UART interface power
	int32_t                  (*Send)            (const void *data, uint32_t num);    					// Start sending data to UART transmitter
	int32_t                  (*Receive)         (      void *data, uint32_t num);    					// Start receiving data from UART receiver
	int32_t                  (*Transfer)        (const void *data_out, void *data_in, uint32_t    num);	// Start sending/receiving data to/from UART
	uint32_t                 (*GetTxCount)      (void);                              					// Get transmitted data count
	uint32_t                 (*GetRxCount)      (void);                              					// Get received data count
	int32_t                  (*Control)         (uint32_t control, uint32_t arg);    					// Control UART interface
	AE350_UART_STATUS        (*GetStatus)       (void);                              					// Get UART status
	int32_t                  (*SetModemControl) (AE350_UART_MODEM_CONTROL control);   					// Set UART modem control line state
	AE350_UART_MODEM_STATUS  (*GetModemStatus)  (void);                              					// Get UART modem status lines state
} const AE350_DRIVER_UART;


#endif /* __DRIVER_UART_H__ */
