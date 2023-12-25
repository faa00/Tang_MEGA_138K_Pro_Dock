/*
 * ******************************************************************************************
 * File		: uart_ae350.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: UART driver definitions
 * ******************************************************************************************
 */

#ifndef __UART_AE350_H__
#define __UART_AE350_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_UART.h"
#include "dma_ae350.h"
#include "platform.h"


// Definitions  -----------------------------------------------------------------------------

/* IER Register (+0x24) */
#define UARTC_IER_RDR                   0x01 /* Data ready enable */
#define UARTC_IER_THRE                  0x02 /* THR empty enable */
#define UARTC_IER_RLS                   0x04 /* Receive line status enable */
#define UARTC_IER_MS                    0x08 /* Modem status enable */

/* IIR Register (+0x28) */
#define UARTC_IIR_NONE                  0x01 /* No interrupt pending */
#define UARTC_IIR_RLS                   0x06 /* Receive line status */
#define UARTC_IIR_RDA                   0x04 /* Receive data available */
#define UARTC_IIR_RTO                   0x0c /* Receive time out */
#define UARTC_IIR_THRE                  0x02 /* THR empty */
#define UARTC_IIR_MODEM                 0x00 /* Modem status */
#define UARTC_IIR_INT_MASK              0x0f /* Interrupt status bits mask */

#define UARTC_IIR_TFIFO_FULL            0x10 /* TX FIFO full */
#define UARTC_IIR_FIFO_EN               0xc0 /* FIFO mode is enabled, set when FCR[0] is 1 */

/* FCR Register (+0x28) */
#define UARTC_FCR_FIFO_EN               0x01 /* FIFO enable */
#define UARTC_FCR_RFIFO_RESET           0x02 /* RX FIFO reset */
#define UARTC_FCR_TFIFO_RESET           0x04 /* TT FIFO reset */
#define UARTC_FCR_DMA_EN                0x08 /* Select UART DMA mode */

#define UARTC_FCR_TFIFO16_TRGL1         0x00 /* TX 16-byte FIFO interrupt trigger level - 1 char  */
#define UARTC_FCR_TFIFO16_TRGL3         0x10 /* TX 16-byte FIFO interrupt trigger level - 3 char  */
#define UARTC_FCR_TFIFO16_TRGL9         0x20 /* TX 16-byte FIFO interrupt trigger level - 9 char  */
#define UARTC_FCR_TFIFO16_TRGL13        0x30 /* TX 16-byte FIFO interrupt trigger level - 13 char */

#define UARTC_FCR_RFIFO16_TRGL1         0x00 /* RX 16-byte FIFO interrupt trigger level - 1 char  */
#define UARTC_FCR_RFIFO16_TRGL4         0x40 /* RX 16-byte FIFO interrupt trigger level - 4 char  */
#define UARTC_FCR_RFIFO16_TRGL8         0x80 /* RX 16-byte FIFO interrupt trigger level - 8 char  */
#define UARTC_FCR_RFIFO16_TRGL14        0xc0 /* RX 16-byte FIFO interrupt trigger level - 14 char */

#define UARTC_FCR_FIFO_EN_MASK          0x01 /* FIFO enable */
#define UARTC_FCR_FIFO_EN_BIT           0
#define UARTC_FCR_RFIFO_RESET_MASK      0x02 /* RX FIFO reset */
#define UARTC_FCR_RFIFO_RESET_BIT       1
#define UARTC_FCR_TFIFO_RESET_MASK      0x04 /* TX FIFO reset */
#define UARTC_FCR_TFIFO_RESET_BIT       2
#define UARTC_FCR_DMA_EN_MASK           0x08 /* Select UART DMA mode */
#define UARTC_FCR_DMA_EN_BIT            3

#define UARTC_FCR_TXFIFO_TRGL_MASK      0x30 /* TX FIFO interrupt trigger level */
#define UARTC_FCR_TXFIFO_TRGL_SHIFT     4
#define UARTC_FCR_RXFIFO_TRGL_MASK      0xc0 /* RX FIFO interrupt trigger level */
#define UARTC_FCR_RXFIFO_TRGL_SHIFT     6

/* LCR Register (+0x2c) */
#define UARTC_LCR_BITS5                 0x00 /* 5 bits */
#define UARTC_LCR_BITS6                 0x01 /* 6 bits */
#define UARTC_LCR_BITS7                 0x02 /* 7 bits */
#define UARTC_LCR_BITS8                 0x03 /* 8 bits */
#define UARTC_LCR_STOP1                 0x00
#define UARTC_LCR_STOP2                 0x04

#define UARTC_LCR_PARITY_EN             0x08 /* Parity enable */
#define UARTC_LCR_PARITY_NONE           0x00 /* No parity check */
#define UARTC_LCR_PARITY_EVEN           0x18 /* Even parity */
#define UARTC_LCR_PARITY_ODD            0x08 /* Odd parity */

#define UARTC_LCR_SETBREAK              0x40 /* Set break condition */
#define UARTC_LCR_DLAB                  0x80 /* Divisor latch access bit */

/* MCR Register (+0x30) */
#define UARTC_MCR_DTR                   0x01 /* Data terminal ready */
#define UARTC_MCR_RTS                   0x02 /* Request to Send */
#define UARTC_MCR_OUT1                  0x04 /* Output1 */
#define UARTC_MCR_OUT2                  0x08 /* Output2 or global interrupt enable */
#define UARTC_MCR_LPBK                  0x10 /* Loop back mode */
#define UARTC_MCR_AFE                   0x20 /* Auto flow control */

/* LSR Register (+0x34) */
#define UARTC_LSR_RDR                   0x1  /* Data ready */
#define UARTC_LSR_OE                    0x2  /* Overrun error */
#define UARTC_LSR_PE                    0x4  /* Parity error */
#define UARTC_LSR_FE                    0x8  /* Framing error */
#define UARTC_LSR_BI                    0x10 /* Break interrupt */
#define UARTC_LSR_THRE                  0x20 /* THR/FIFO empty */
#define UARTC_LSR_TEMT                  0x40 /* THR/FIFO and TFR empty */
#define UARTC_LSR_ERRF                  0x80 /* FIFO data error */

/* MSR Register (+0x38) */
#define UARTC_MSR_DCTS                  0x1  /* Delta CTS */
#define UARTC_MSR_DDSR                  0x2  /* Delta DSR */
#define UARTC_MSR_TERI                  0x4  /* Trailing edge RI */
#define UARTC_MSR_DDCD                  0x8  /* Delta CD */
#define UARTC_MSR_CTS                   0x10 /* Clear to send */
#define UARTC_MSR_DSR                   0x20 /* Data set ready */
#define UARTC_MSR_RI                    0x40 /* Ring indicator */
#define UARTC_MSR_DCD                   0x80 /* Data carrier detect */


// UART flags
#define UART_FLAG_INITIALIZED           (1U << 0)
#define UART_FLAG_POWERED               (1U << 1)
#define UART_FLAG_CONFIGURED            (1U << 2)
#define UART_FLAG_TX_ENABLED            (1U << 3)
#define UART_FLAG_RX_ENABLED            (1U << 4)
#define UART_FLAG_SEND_ACTIVE           (1U << 5)

// UART TX FIFO trigger level
#define UART_TRIG_LVL_1                 (0x00U)
#define UART_TRIG_LVL_4                 (0x40U)
#define UART_TRIG_LVL_8                 (0x80U)
#define UART_TRIG_LVL_14                (0xC0U)

// UART transfer information (Run-Time)
typedef struct _UART_TRANSFER_INFO
{
	uint32_t                rx_num;        // Total number of data to be received
	uint32_t                tx_num;        // Total number of data to be send
	uint8_t                 *rx_buf;       // Pointer to in data buffer
	uint8_t                 *tx_buf;       // Pointer to out data buffer
	uint32_t                rx_cnt;        // Number of data received
	uint32_t                tx_cnt;        // Number of data sent
	uint8_t                 tx_def_val;    // Transmit default value (used in UART_SYNC_MASTER_MODE_RX)
	uint8_t                 send_active;   // Send active flag
} UART_TRANSFER_INFO;

// UART RX status
typedef struct _UART_RX_STATUS
{
	uint8_t rx_busy;                       // Receiver busy flag
	uint8_t rx_overflow;                   // Receive data overflow detected (cleared on start of next receive operation)
	uint8_t rx_break;                      // Break detected on receive (cleared on start of next receive operation)
	uint8_t rx_framing_error;              // Framing error detected on receive (cleared on start of next receive operation)
	uint8_t rx_parity_error;               // Parity error detected on receive (cleared on start of next receive operation)
} UART_RX_STATUS;

// UART information (Run-Time)
typedef struct _UART_INFO
{
	AE350_UART_SignalEvent_t cb_event;      // Event callback
	UART_RX_STATUS         rx_status;     	// Receive status flags
	UART_TRANSFER_INFO     xfer;          	// Transfer information
	uint8_t                 mode;          	// UART mode
	uint8_t                 flags;         	// UART driver flags
	uint32_t                baudrate;      	// Baud rate
} UART_INFO;

// UART DMA
typedef const struct _UART_DMA
{
	uint8_t                 channel;       	// DMA channel
	uint8_t                 reqsel;        	// DMA request selection
	DMA_SignalEvent_t       cb_event;      	// DMA event callback
} UART_DMA;

// UART resources definitions
typedef struct
{
	AE350_UART_CAPABILITIES   capabilities;  // Capabilities
	UART_RegDef		          *reg;          // Pointer to UART register
	uint32_t                  irq_num;       // UART IRQ Number
	uint32_t                  trig_lvl;      // FIFO trigger level
	UART_DMA                  *dma_tx;		 // DMA TX
	UART_DMA                  *dma_rx;		 // DMA RX
	UART_INFO                 *info;         // Run-Time information
} const UART_RESOURCES;


#endif /* __UART_AE350_H__ */
