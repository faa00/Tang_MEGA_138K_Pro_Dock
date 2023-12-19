/*
 * ******************************************************************************************
 * File		: uart_ae350.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: UART driver definitions
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "uart_ae350.h"


// Variables  -------------------------------------------------------------------------------

#define AE350_UART_DRV_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if ((!DRV_UART1) && (!DRV_UART2))
	#error "UART not enabled in config.h!"
#endif

// Driver version
static const AE350_DRIVER_VERSION uart_driver_version = { AE350_UART_API_VERSION, AE350_UART_DRV_VERSION };

// Trigger level definitions
// Can be user defined by C preprocessor
#ifndef UART1_TRIG_LVL
#define UART1_TRIG_LVL            UART_TRIG_LVL_1
#endif
#ifndef UART2_TRIG_LVL
#define UART2_TRIG_LVL            UART_TRIG_LVL_1
#endif

// UART1
#if (DRV_UART1)
static UART_INFO uart1_info = {0};

#if (DRV_UART1_DMA_TX_EN == 1)
void uart1_dma_tx_event (uint32_t event);
static UART_DMA uart1_dma_tx = {DRV_UART1_DMA_TX_CH,
                                DRV_UART1_DMA_TX_REQID,
                                uart1_dma_tx_event};
#endif
#if (DRV_UART1_DMA_RX_EN == 1)
void uart1_dma_rx_event (uint32_t event);
static UART_DMA uart1_dma_rx = {DRV_UART1_DMA_RX_CH,
                                DRV_UART1_DMA_RX_REQID,
                                uart1_dma_rx_event};
#endif

static const UART_RESOURCES uart1_resources =
{
	{
		// Capabilities
		1,  // Supports UART (Asynchronous) mode
		0,  // Supports Synchronous Master mode
		0,  // Supports Synchronous Slave mode
		0,  // Supports UART Single-wire mode
		0,  // Supports UART IrDA mode
		0,  // Supports UART Smart Card mode
		0,	// Smart Card Clock generator
		0,  // RTS Flow Control available
		0,  // CTS Flow Control available
		0,  // Transmit completed event: \ref AE350_UART_EVENT_TX_COMPLETE
#if ((DRV_UART1_DMA_RX_EN == 1) || (UART1_TRIG_LVL == UART_TRIG_LVL_1))
		0,  // Signal receive character timeout event: \ref AE350_UART_EVENT_RX_TIMEOUT
#else
		1,  // Signal receive character timeout event: \ref AE350_UART_EVENT_RX_TIMEOUT
#endif
		0,  // RTS Line: 0=not available, 1=available
		0,  // CTS Line: 0=not available, 1=available
		0,  // DTR Line: 0=not available, 1=available
		0,  // DSR Line: 0=not available, 1=available
		0,  // DCD Line: 0=not available, 1=available
		0,  // RI Line: 0=not available, 1=available
		0,  // Signal CTS change event: \ref AE350_UART_EVENT_CTS
		0,  // Signal DSR change event: \ref AE350_UART_EVENT_DSR
		0,  // Signal DCD change event: \ref AE350_UART_EVENT_DCD
		0,  // Signal RI change event:  \ref AE350_UART_EVENT_RI
	},
	DEV_UART1,
	IRQ_UART1_SOURCE,
	UART1_TRIG_LVL,
#if (DRV_UART1_DMA_TX_EN == 1)
	&uart1_dma_tx,
#else
	NULL,
#endif
#if (DRV_UART1_DMA_RX_EN == 1)
	&uart1_dma_rx,
#else
	NULL,
#endif
	&uart1_info
};
#endif	// DRV_UART1

// UART2
#if (DRV_UART2)
static UART_INFO uart2_info = {0};

#if (DRV_UART2_DMA_TX_EN == 1)
void uart2_dma_tx_event (uint32_t event);
static UART_DMA uart2_dma_tx = {DRV_UART2_DMA_TX_CH,
                                DRV_UART2_DMA_TX_REQID,
                                uart2_dma_tx_event};
#endif
#if (DRV_UART2_DMA_RX_EN == 1)
void uart2_dma_rx_event (uint32_t event);
static UART_DMA uart2_dma_rx = {DRV_UART2_DMA_RX_CH,
                                DRV_UART2_DMA_RX_REQID,
                                uart2_dma_rx_event};
#endif

static const UART_RESOURCES uart2_resources =
{
	{
		// Capabilities
		1,  // Supports UART (Asynchronous) mode
		0,  // Supports Synchronous Master mode
		0,  // Supports Synchronous Slave mode
		0,  // Supports UART Single-wire mode
		0,  // Supports UART IrDA mode
		0,  // Supports UART Smart Card mode
		0,  // Smart Card Clock generator
		0,  // RTS Flow Control available
		0,  // CTS Flow Control available
		0,  // Transmit completed event: \ref AE350_UART_EVENT_TX_COMPLETE
#if ((DRV_UART2_DMA_RX_EN == 1) || (UART2_TRIG_LVL == UART_TRIG_LVL_1))
		0,  // Signal receive character timeout event: \ref AE350_UART_EVENT_RX_TIMEOUT
#else
		1,  // Signal receive character timeout event: \ref AE350_UART_EVENT_RX_TIMEOUT
#endif
		0,  // RTS Line: 0=not available, 1=available
		0,  // CTS Line: 0=not available, 1=available
		0,  // DTR Line: 0=not available, 1=available
		0,  // DSR Line: 0=not available, 1=available
		0,  // DCD Line: 0=not available, 1=available
		0,  // RI Line: 0=not available, 1=available
		0,  // Signal CTS change event: \ref AE350_UART_EVENT_CTS
		0,  // Signal DSR change event: \ref AE350_UART_EVENT_DSR
		0,  // Signal DCD change event: \ref AE350_UART_EVENT_DCD
		0,  // Signal RI change event:  \ref AE350_UART_EVENT_RI
	},
	DEV_UART2,
	IRQ_UART2_SOURCE,
	UART2_TRIG_LVL,
#if (DRV_UART2_DMA_TX_EN == 1)
	&uart2_dma_tx,
#else
	NULL,
#endif
#if (DRV_UART2_DMA_RX_EN == 1)
	&uart2_dma_rx,
#else
	NULL,
#endif
	&uart2_info
};
#endif	//DRV_UART2


// Definitions  -----------------------------------------------------------------------------

/****************************************************************************************
  \fn          int32_t uart_set_baudrate (uint32_t         baudrate,
                                          UART_RESOURCES   *uartx)
  \brief       Set baudrate dividers
  \param[in]   baudrate  Uart baudrate
  \param[in]   uartx     Pointer to UART resources)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*****************************************************************************************/
int32_t uart_set_baudrate (uint32_t baudrate, UART_RESOURCES *uartx)
{
	uint32_t div;

	// Over sampling is fixed to 16, and round off the divider value:
	// divider = (UCLKFREQ / (16.0 * baud rate)) + 0.5;
	// And below is a variant which doesn't use FP operation.
	// It is in RDS_V511
	div = ((UCLKFREQ + 8 * baudrate) / (16 * baudrate));

	uartx->reg->LCR = 0;
	uartx->reg->LCR = 3;
	uartx->reg->LCR |= UARTC_LCR_DLAB;
	uartx->reg->DLL = (div >> 0) & 0xff;
	uartx->reg->DLM = (div >> 8) & 0xff;
	// Reset DLAB bit
	uartx->reg->LCR &= (~UARTC_LCR_DLAB);

	uartx->info->baudrate = baudrate;

	return 0;
}

/*****************************************************************************************
  \fn          uint32_t uart_rxline_irq_handler (UART_RESOURCES *uartx)
  \brief       Receive line interrupt handler
  \param[in]   uartx     Pointer to UART resources
  \return      Rx Line event mask
******************************************************************************************/
static uint32_t uart_rxline_irq_handler (UART_RESOURCES *uartx)
{
	uint32_t lsr, event;

	#define UARTC_LSR_LINE_INT      (UARTC_LSR_OE | UARTC_LSR_PE | UARTC_LSR_FE | UARTC_LSR_BI)
	event = 0U;
	lsr   = uartx->reg->LSR  & UARTC_LSR_LINE_INT;

	// OverRun error
	if (lsr & UARTC_LSR_OE)
	{
		uartx->info->rx_status.rx_overflow = 1U;
		event |= AE350_UART_EVENT_RX_OVERFLOW;
	}

	// Parity error
	if (lsr & UARTC_LSR_PE)
	{
		uartx->info->rx_status.rx_parity_error = 1U;
		event |= AE350_UART_EVENT_RX_PARITY_ERROR;
	}

	// Break detected
	if (lsr & UARTC_LSR_BI)
	{
		uartx->info->rx_status.rx_break = 1U;
		event |= AE350_UART_EVENT_RX_BREAK;
	}
	// Framing error
	else
	{
		if (lsr & UARTC_LSR_FE)
		{
			uartx->info->rx_status.rx_framing_error = 1U;
			event |= AE350_UART_EVENT_RX_FRAMING_ERROR;
		}
	}

	return event;
}

// Function Prototypes
static int32_t uart_receive (void *data, uint32_t num, UART_RESOURCES *uartx);


// UART Driver functions

/*****************************************************************************************
  \fn          AE350_DRIVER_VERSION uart_get_version (void)
  \brief       Get driver version.
  \return      \ref AE350_DRIVER_VERSION
******************************************************************************************/
static AE350_DRIVER_VERSION uart_get_version (void)
{
	return uart_driver_version;
}

/*****************************************************************************************
  \fn          AE350_UART_CAPABILITIES uart_get_capabilities (UART_RESOURCES *uartx)
  \brief       Get driver capabilities
  \param[in]   uartx     Pointer to UART resources
  \return      \ref AE350_UART_CAPABILITIES
******************************************************************************************/
static AE350_UART_CAPABILITIES uart_get_capabilities (UART_RESOURCES *uartx)
{
	return uartx->capabilities;
}

/*****************************************************************************************
  \fn          int32_t uart_initialize (AE350_UART_SignalEvent_t  cb_event
                                        UART_RESOURCES            *uartx)
  \brief       Initialize UART Interface.
  \param[in]   cb_event  Pointer to \ref AE350_UART_SignalEvent
  \param[in]   uartx     Pointer to UART resources
  \return      \ref execution_status
******************************************************************************************/
static int32_t uart_initialize (AE350_UART_SignalEvent_t cb_event, UART_RESOURCES *uartx)
{
	if (uartx->info->flags & UART_FLAG_INITIALIZED)
	{
		// Driver is already initialized
		return AE350_DRIVER_OK;
	}

	// Initialize UART Run-time Resources
	uartx->info->cb_event = cb_event;

	uartx->info->rx_status.rx_busy          = 0U;
	uartx->info->rx_status.rx_overflow      = 0U;
	uartx->info->rx_status.rx_break         = 0U;
	uartx->info->rx_status.rx_framing_error = 0U;
	uartx->info->rx_status.rx_parity_error  = 0U;

	uartx->info->xfer.send_active           = 0U;
	uartx->info->xfer.tx_def_val            = 0U;

	// DMA Initialize
	if (uartx->dma_tx || uartx->dma_rx)
	{
		dma_initialize ();
	}

	uartx->info->flags = UART_FLAG_INITIALIZED;

	return AE350_DRIVER_OK;
}

/*****************************************************************************************
  \fn          int32_t uart_uninitialize (UART_RESOURCES *uartx)
  \brief       Uninitialized UART Interface.
  \param[in]   uartx     Pointer to UART resources
  \return      \ref execution_status
******************************************************************************************/
static int32_t uart_uninitialize (UART_RESOURCES *uartx)
{
	// DMA Un-initialize
	if (uartx->dma_tx || uartx->dma_rx)
	{
		dma_uninitialize ();
	}

	// Reset UART status flags
	uartx->info->flags = 0U;

	return AE350_DRIVER_OK;
}

/*****************************************************************************************
  \fn          int32_t uart_power_control (AE350_POWER_STATE state,
                                           UART_RESOURCES    *uartx)
  \brief       Control UART Interface Power.
  \param[in]   state  Power state
  \param[in]   uartx  Pointer to UART resources
  \return      \ref execution_status
******************************************************************************************/
static int32_t uart_power_control (AE350_POWER_STATE state, UART_RESOURCES *uartx)
{
	uint32_t val;

	switch (state)
	{
	case AE350_POWER_OFF:
		// Disable UART IRQ
		__nds__plic_disable_interrupt(uartx->irq_num);

		// If DMA mode - disable TX DMA channel
		if ((uartx->dma_tx) && (uartx->info->xfer.send_active != 0U))
		{
			dma_channel_disable (uartx->dma_tx->channel);
		}

		// If DMA mode - disable DMA channel
		if ((uartx->dma_rx) && (uartx->info->rx_status.rx_busy))
		{
			dma_channel_disable (uartx->dma_rx->channel);
		}

		// Clear driver variables
		uartx->info->rx_status.rx_busy          = 0U;
		uartx->info->rx_status.rx_overflow      = 0U;
		uartx->info->rx_status.rx_break         = 0U;
		uartx->info->rx_status.rx_framing_error = 0U;
		uartx->info->rx_status.rx_parity_error  = 0U;
		uartx->info->xfer.send_active           = 0U;

		uartx->info->flags &= ~UART_FLAG_POWERED;

		break;

	case AE350_POWER_LOW:
		return AE350_DRIVER_ERROR_UNSUPPORTED;

	case AE350_POWER_FULL:
		if ((uartx->info->flags & UART_FLAG_INITIALIZED) == 0U)
		{
			return AE350_DRIVER_ERROR;
		}

		if ((uartx->info->flags & UART_FLAG_POWERED) != 0U)
		{
			return AE350_DRIVER_OK;
		}

		// Disable interrupts
		uartx->reg->IER = 0U;

		// Configure FIFO Control register
		// Set trigger level
		val = (uartx->trig_lvl & UARTC_FCR_RXFIFO_TRGL_MASK) | UARTC_FCR_FIFO_EN;

		if (uartx->dma_rx || uartx->dma_tx)
		{
			val |= UARTC_FCR_DMA_EN;
		}

		uartx->reg->FCR = val;

		// Enable modem lines status interrupts (only UART2)
		if (uartx->capabilities.cts || uartx->capabilities.dcd ||
			uartx->capabilities.dsr || uartx->capabilities.ri)
		{
			uartx->reg->IER |= UARTC_IER_MS;
		}

		// Clear driver variables
		uartx->info->rx_status.rx_busy          = 0U;
		uartx->info->rx_status.rx_overflow      = 0U;
		uartx->info->rx_status.rx_break         = 0U;
		uartx->info->rx_status.rx_framing_error = 0U;
		uartx->info->rx_status.rx_parity_error  = 0U;

		uartx->info->mode                       = 0U;
		uartx->info->flags                      = 0U;
		uartx->info->xfer.send_active           = 0U;

		uartx->info->flags = UART_FLAG_POWERED | UART_FLAG_INITIALIZED;

		// Priority must be set > 0 to trigger the interrupt
		__nds__plic_set_priority(uartx->irq_num, 1);

		// Enable PLIC interrupt PIT source
		__nds__plic_enable_interrupt(uartx->irq_num);

		// Enable the Machine-External bit in MIE
		set_csr(NDS_MIE, MIP_MEIP);

		// Enable GIE
		set_csr(NDS_MSTATUS, MSTATUS_MIE);

		break;

	default:
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	return AE350_DRIVER_OK;
}

/******************************************************************************************
  \fn          int32_t uart_send (const void             *data,
                                        uint32_t         num,
                                        UART_RESOURCES   *uartx)
  \brief       Start sending data to UART transmitter.
  \param[in]   data  Pointer to buffer with data to send to UART transmitter
  \param[in]   num   Number of data items to send
  \param[in]   uartx Pointer to UART resources
  \return      \ref execution_status
*******************************************************************************************/
static int32_t uart_send (const void *data, uint32_t num, UART_RESOURCES *uartx)
{
	int32_t stat, val;

	if ((data == NULL) || (num == 0U))
	{
		// Invalid parameters
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	if ((uartx->info->flags & UART_FLAG_CONFIGURED) == 0U)
	{
		// UART is not configured (mode not selected)
		return AE350_DRIVER_ERROR;
	}

	if (uartx->info->xfer.send_active != 0U)
	{
		// Send is not completed yet
		return AE350_DRIVER_ERROR_BUSY;
	}

	// Set Send active flag
	uartx->info->xfer.send_active = 1U;

	// Save transmit buffer info
	uartx->info->xfer.tx_buf = (uint8_t *)data;
	uartx->info->xfer.tx_num = num;
	uartx->info->xfer.tx_cnt = 0U;

	// DMA mode
	if (uartx->dma_tx)
	{
		// Configure DMA channel
		stat = dma_channel_configure (uartx->dma_tx->channel,
									  (uint32_t)(long)data,
									  (uint32_t)(long)(&(uartx->reg->THR)),
									  num,
									  DMA_CH_CTRL_SBINF(DMA_INF_IDX1) |
									  DMA_CH_CTRL_DBINF(DMA_INF_IDX1) |
									  DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
									  DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE) |
									  DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE) |
									  DMA_CH_CTRL_DMODE_HANDSHAKE |
									  DMA_CH_CTRL_SRCADDR_INC |
									  DMA_CH_CTRL_DSTADDR_FIX |
									  DMA_CH_CTRL_DSTREQ(uartx->dma_tx->reqsel)	|
									  DMA_CH_CTRL_INTABT |
									  DMA_CH_CTRL_INTERR |
									  DMA_CH_CTRL_INTTC	|
									  DMA_CH_CTRL_ENABLE,
									  uartx->dma_tx->cb_event);

		if (stat == -1)
		{
			return AE350_DRIVER_ERROR;
		}
	}
	// Interrupt mode
	else
	{
		// Fill TX FIFO
		if (uartx->reg->LSR & UARTC_LSR_THRE)
		{
			val = 16U;
			while ((val--) && (uartx->info->xfer.tx_cnt != uartx->info->xfer.tx_num))
			{
				uartx->reg->THR = uartx->info->xfer.tx_buf[uartx->info->xfer.tx_cnt++];
			}
		}

		// Enable transmit holding register empty interrupt
		uartx->reg->IER |= UARTC_IER_THRE;
	}

	return AE350_DRIVER_OK;
}

/******************************************************************************************
  \fn          int32_t uart_receive (void            *data,
                                     uint32_t         num,
                                     UART_RESOURCES  *uartx)
  \brief       Start receiving data from UART receiver.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \param[in]   uartx Pointer to UART resources
  \return      \ref execution_status
*******************************************************************************************/
static int32_t uart_receive (void *data, uint32_t num, UART_RESOURCES *uartx)
{
	int32_t stat;

	if ((data == NULL) || (num == 0U))
	{
		// Invalid parameters
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	if ((uartx->info->flags & UART_FLAG_CONFIGURED) == 0U)
	{
		// UART is not configured (mode not selected)
		return AE350_DRIVER_ERROR;
	}

	// Check if receiver is busy
	if (uartx->info->rx_status.rx_busy == 1U)
	{
		return AE350_DRIVER_ERROR_BUSY;
	}

	// Set RX busy flag
	uartx->info->rx_status.rx_busy = 1U;

	// Save number of data to be received
	uartx->info->xfer.rx_num = num;

	// Clear RX statuses
	uartx->info->rx_status.rx_break          = 0U;
	uartx->info->rx_status.rx_framing_error  = 0U;
	uartx->info->rx_status.rx_overflow       = 0U;
	uartx->info->rx_status.rx_parity_error   = 0U;

	// Save receive buffer info
	uartx->info->xfer.rx_buf = (uint8_t *)data;
	uartx->info->xfer.rx_cnt =              0U;

	// DMA mode
	if (uartx->dma_rx)
	{
		stat = dma_channel_configure (uartx->dma_rx->channel,
									  (uint32_t)(long)&uartx->reg->RBR,
									  (uint32_t)(long)data,
									  num,
									  DMA_CH_CTRL_SBINF(DMA_INF_IDX1) |
									  DMA_CH_CTRL_DBINF(DMA_INF_IDX1) |
									  DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
									  DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE) |
									  DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE) |
									  DMA_CH_CTRL_SMODE_HANDSHAKE |
									  DMA_CH_CTRL_SRCADDR_FIX |
									  DMA_CH_CTRL_DSTADDR_INC |
									  DMA_CH_CTRL_SRCREQ(uartx->dma_rx->reqsel)	|
									  DMA_CH_CTRL_INTABT |
									  DMA_CH_CTRL_INTERR |
									  DMA_CH_CTRL_INTTC |
									  DMA_CH_CTRL_ENABLE,
									  uartx->dma_rx->cb_event);

		if (stat == -1)
		{
			return AE350_DRIVER_ERROR;
		}
	}
	// Interrupt mode
	else
	{
		// Enable receive data available interrupt
		uartx->reg->IER |= UARTC_IER_RDR;
	}

	return AE350_DRIVER_OK;
}

/******************************************************************************************
  \fn          int32_t uart_transfer (const void             *data_out,
                                            void             *data_in,
                                            uint32_t          num,
                                            UART_RESOURCES   *uartx)
  \brief       Start sending/receiving data to/from UART transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to UART transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from UART receiver
  \param[in]   num       Number of data items to transfer
  \param[in]   uartx     Pointer to UART resources
  \return      \ref execution_status
*******************************************************************************************/
static int32_t uart_transfer (const void *data_out, void *data_in, uint32_t	num, UART_RESOURCES	*uartx)
{
	if ((data_out == NULL) || (data_in == NULL) || (num == 0U))
	{
		// Invalid parameters
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	if ((uartx->info->flags & UART_FLAG_CONFIGURED) == 0U)
	{
		// UART is not configured
		return AE350_DRIVER_ERROR;
	}

	// Only in synchronous mode
	return AE350_DRIVER_ERROR;
}

/******************************************************************************************
  \fn          uint32_t uart_get_txcount (UART_RESOURCES *uartx)
  \brief       Get transmitted data count.
  \param[in]   uartx     Pointer to UART resources
  \return      number of data items transmitted
*******************************************************************************************/
static uint32_t uart_get_txcount (UART_RESOURCES *uartx)
{
	uint32_t cnt;

	if (uartx->dma_tx)
	{
		cnt = dma_channel_get_count (uartx->dma_tx->channel);
	}
	else
	{
		cnt = uartx->info->xfer.tx_cnt;
	}

	return cnt;
}

/******************************************************************************************
  \fn          uint32_t uart_get_rxcount (UART_RESOURCES *uartx)
  \brief       Get received data count.
  \param[in]   uartx     Pointer to UART resources
  \return      number of data items received
*******************************************************************************************/
static uint32_t uart_get_rxcount (UART_RESOURCES *uartx)
{
	uint32_t cnt;

	if (uartx->dma_rx)
	{
		cnt = dma_channel_get_count (uartx->dma_rx->channel);
	}
	else
	{
		cnt = uartx->info->xfer.rx_cnt;
	}

	return cnt;
}

/******************************************************************************************
  \fn          int32_t uart_control (uint32_t          control,
                                     uint32_t          arg,
                                     UART_RESOURCES   *uartx)
  \brief       Control UART Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \param[in]   uartx    Pointer to UART resources
  \return      common \ref execution_status and driver specific \ref uart_execution_status
*******************************************************************************************/
static int32_t uart_control (uint32_t control, uint32_t arg, UART_RESOURCES  *uartx)
{
	uint32_t val, mode;
	uint32_t lcr, mcr;

	if ((uartx->info->flags & UART_FLAG_POWERED) == 0U)
	{
		// UART not powered
		return AE350_DRIVER_ERROR;
	}

	lcr      = 0U;
	mcr      = 0U;

	switch (control & AE350_UART_CONTROL_Msk)
	{
	// Control TX
	case AE350_UART_CONTROL_TX:
		// Check if TX line available
		if (arg)
		{
			uartx->info->flags |= UART_FLAG_TX_ENABLED;
		}
		else
		{
			uartx->info->flags &= ~UART_FLAG_TX_ENABLED;
		}

		return AE350_DRIVER_OK;

	// Control RX
	case AE350_UART_CONTROL_RX:
		// RX Line interrupt enable (overrun, framing, parity error, break)
		if (arg)
		{
			uartx->info->flags |= UART_FLAG_RX_ENABLED;
			uartx->reg->IER |= UARTC_IER_RLS;
		}
		else
		{
			uartx->info->flags &= ~UART_FLAG_RX_ENABLED;
			uartx->reg->IER &= ~UARTC_IER_RLS;
		}

		return AE350_DRIVER_OK;

	// Control break
	case AE350_UART_CONTROL_BREAK:
		if (arg)
		{
			if (uartx->info->xfer.send_active != 0U)
			{
				return AE350_DRIVER_ERROR_BUSY;
			}

			uartx->reg->LCR |= UARTC_LCR_SETBREAK;

			// Set Send active flag
			uartx->info->xfer.send_active = 1U;
		}
		else
		{
			uartx->reg->LCR &= ~UARTC_LCR_SETBREAK;

			// Clear Send active flag
			uartx->info->xfer.send_active = 0U;
		}

		return AE350_DRIVER_OK;

	// Abort Send
	case AE350_UART_ABORT_SEND:
		// Disable transmit holding register empty interrupt
		uartx->reg->IER &= ~UARTC_IER_THRE;

		// Set trigger level
		val  = (uartx->trig_lvl & UARTC_FCR_RXFIFO_TRGL_MASK) |	UARTC_FCR_FIFO_EN;
		if (uartx->dma_rx || uartx->dma_tx)
		{
			val |= UARTC_FCR_DMA_EN;
		}

		// Transmit FIFO reset
		val |= UARTC_FCR_TFIFO_RESET;
		uartx->reg->FCR = val;

		// If DMA mode - disable DMA channel
		if ((uartx->dma_tx) && (uartx->info->xfer.send_active != 0U))
		{
			dma_channel_disable (uartx->dma_tx->channel);
		}

		// Clear Send active flag
		uartx->info->xfer.send_active = 0U;

		return AE350_DRIVER_OK;

	// Abort receive
	case AE350_UART_ABORT_RECEIVE:
		// Disable receive data available interrupt
		uartx->reg->IER &= ~UARTC_IER_RDR;

		// Set trigger level
		val  = (uartx->trig_lvl & UARTC_FCR_RXFIFO_TRGL_MASK) |	UARTC_FCR_FIFO_EN;
		if (uartx->dma_rx || uartx->dma_tx)
		{
			val |= UARTC_FCR_DMA_EN;
		}

		// Receive FIFO reset
		val |= UARTC_FCR_RFIFO_RESET;
		uartx->reg->FCR = val;

		// If DMA mode - disable DMA channel
		if ((uartx->dma_rx) && (uartx->info->rx_status.rx_busy))
		{
			dma_channel_disable (uartx->dma_rx->channel);
		}

		// Clear RX busy status
		uartx->info->rx_status.rx_busy = 0U;

		return AE350_DRIVER_OK;

	// Abort transfer
	case AE350_UART_ABORT_TRANSFER:
		// Disable transmit holding register empty and receive data available interrupts
		uartx->reg->IER &= ~(UARTC_IER_THRE | UARTC_IER_RDR);

		// If DMA mode - disable DMA channel
		if ((uartx->dma_tx) && (uartx->info->xfer.send_active != 0U))
		{
			dma_channel_disable (uartx->dma_tx->channel);
		}

		if ((uartx->dma_rx) && (uartx->info->rx_status.rx_busy))
		{
			dma_channel_disable (uartx->dma_rx->channel);
		}

		// Set trigger level
		val  = (uartx->trig_lvl & UARTC_FCR_RXFIFO_TRGL_MASK) | UARTC_FCR_FIFO_EN;
		if (uartx->dma_rx || uartx->dma_tx)
		{
			val |= UARTC_FCR_DMA_EN;
		}

		// Transmit and receive FIFO reset
		val |= UARTC_FCR_TFIFO_RESET | UARTC_FCR_RFIFO_RESET;
		uartx->reg->FCR = val;

		// Clear busy statuses
		uartx->info->rx_status.rx_busy = 0U;
		uartx->info->xfer.send_active  = 0U;

		return AE350_DRIVER_OK;

	default:
		break;
	}

	switch (control & AE350_UART_CONTROL_Msk)
	{
	case AE350_UART_MODE_ASYNCHRONOUS:
		mode = AE350_UART_MODE_ASYNCHRONOUS;
		break;

	case AE350_UART_MODE_SYNCHRONOUS_MASTER:
	case AE350_UART_MODE_SYNCHRONOUS_SLAVE:
	case AE350_UART_MODE_IRDA:
	case AE350_UART_MODE_SMART_CARD:
	case AE350_UART_MODE_SINGLE_WIRE:
		return AE350_UART_ERROR_MODE;

	// Default TX value
	case AE350_UART_SET_DEFAULT_TX_VALUE:
		uartx->info->xfer.tx_def_val = arg;
		return AE350_DRIVER_OK;

	// Unsupported command
	default:
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	// Check if Receiver/Transmitter is busy
	if (uartx->info->rx_status.rx_busy ||
	   (uartx->info->xfer.send_active != 0U))
	{
		return AE350_DRIVER_ERROR_BUSY;
	}

	// UART data bits
	switch (control & AE350_UART_DATA_BITS_Msk)
	{
	case AE350_UART_DATA_BITS_5:
		lcr |= UARTC_LCR_BITS5;
		break;
	case AE350_UART_DATA_BITS_6:
		lcr |= UARTC_LCR_BITS6;
		break;
	case AE350_UART_DATA_BITS_7:
		lcr |= UARTC_LCR_BITS7;
		break;
	case AE350_UART_DATA_BITS_8:
		lcr |= UARTC_LCR_BITS8;
		break;
	default:
		return AE350_UART_ERROR_DATA_BITS;
	}

	// UART parity
	switch (control & AE350_UART_PARITY_Msk)
	{
	case AE350_UART_PARITY_NONE:
		break;
	case AE350_UART_PARITY_EVEN:
		lcr |= UARTC_LCR_PARITY_EVEN;
		break;
	case AE350_UART_PARITY_ODD:
		lcr |= UARTC_LCR_PARITY_ODD;
		break;
	default:
		return (AE350_UART_ERROR_PARITY);
	}

	// UART stop bits
	switch (control & AE350_UART_STOP_BITS_Msk)
	{
	case AE350_UART_STOP_BITS_1:
		break;
	case AE350_UART_STOP_BITS_2:
		lcr |= UARTC_LCR_STOP2;
		break;
	default:
		return AE350_UART_ERROR_STOP_BITS;
	}

	mcr = uartx->reg->MCR & ~UARTC_MCR_AFE;

	// UART flow control
	switch (control & AE350_UART_FLOW_CONTROL_Msk)
	{
	case AE350_UART_FLOW_CONTROL_NONE:
		break;
	case AE350_UART_FLOW_CONTROL_RTS:
		if (uartx->capabilities.flow_control_rts)
		{
			mcr |= UARTC_MCR_AFE;
		}
		else
		{
			return AE350_UART_ERROR_FLOW_CONTROL;
		}
		break;
	case AE350_UART_FLOW_CONTROL_CTS:
		if (uartx->capabilities.flow_control_cts)
		{
			mcr |= UARTC_MCR_AFE;
		}
		else
		{
			return AE350_UART_ERROR_FLOW_CONTROL;
		}
		break;
	case AE350_UART_FLOW_CONTROL_RTS_CTS:
		if (uartx->capabilities.flow_control_rts &&
			uartx->capabilities.flow_control_cts)
		{
			mcr |= UARTC_MCR_AFE;
		}
		else
		{
			return AE350_UART_ERROR_FLOW_CONTROL;
		}
		break;
	default:
		return AE350_UART_ERROR_FLOW_CONTROL;
	}

	// UART baud rate
	if (uart_set_baudrate (arg, uartx) == -1)
	{
		return AE350_UART_ERROR_BAUDRATE;
	}

	// Configuration is OK - Mode is valid
	uartx->info->mode = mode;

	// Configure MCR register
	uartx->reg->MCR = (uartx->reg->MCR & ~UARTC_MCR_AFE) | mcr;

	// Configure Line control register
	uartx->reg->LCR = ((uartx->reg->LCR & (UARTC_LCR_SETBREAK | UARTC_LCR_DLAB)) | lcr);

	// Set configured flag
	uartx->info->flags |= UART_FLAG_CONFIGURED;

	return AE350_DRIVER_OK;
}

/******************************************************************************************
  \fn          AE350_UART_STATUS uart_get_status (UART_RESOURCES *uartx)
  \brief       Get UART status.
  \param[in]   uartx     Pointer to UART resources
  \return      UART status \ref AE350_UART_STATUS
*******************************************************************************************/
static AE350_UART_STATUS uart_get_status (UART_RESOURCES *uartx)
{
	AE350_UART_STATUS stat;

	stat.tx_busy          = (uartx->reg->LSR & UARTC_LSR_TEMT ? (0U) : (1U));
	stat.rx_busy          = uartx->info->rx_status.rx_busy;
	stat.tx_underflow     = 0U;
	stat.rx_overflow      = uartx->info->rx_status.rx_overflow;
	stat.rx_break         = uartx->info->rx_status.rx_break;
	stat.rx_framing_error = uartx->info->rx_status.rx_framing_error;
	stat.rx_parity_error  = uartx->info->rx_status.rx_parity_error;

	return stat;
}

/******************************************************************************************
  \fn          int32_t uart_set_modem_control (AE350_UART_MODEM_CONTROL  control,
                                               UART_RESOURCES            *uartx)
  \brief       Set UART Modem Control line state.
  \param[in]   control   \ref AE350_UART_MODEM_CONTROL
  \param[in]   uartx     Pointer to UART resources
  \return      \ref execution_status
*******************************************************************************************/
static int32_t uart_set_modem_control (AE350_UART_MODEM_CONTROL control, UART_RESOURCES *uartx)
{
	if ((uartx->info->flags & UART_FLAG_CONFIGURED) == 0U)
	{
		// UART is not configured
		return AE350_DRIVER_ERROR;
	}

	if (control == AE350_UART_RTS_CLEAR)
	{
		if (uartx->capabilities.rts)
		{
			uartx->reg->MCR &= ~UARTC_MCR_RTS;
		}
		else
		{
			return AE350_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	if (control == AE350_UART_RTS_SET)
	{
		if (uartx->capabilities.rts)
		{
			uartx->reg->MCR |=  UARTC_MCR_RTS;
		}
		else
		{
			return AE350_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	if (control == AE350_UART_DTR_CLEAR)
	{
		if (uartx->capabilities.dtr)
		{
			uartx->reg->MCR &= ~UARTC_MCR_DTR;
		}
		else
		{
			return AE350_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	if (control == AE350_UART_DTR_SET)
	{
		if (uartx->capabilities.dtr)
		{
			uartx->reg->MCR |=  UARTC_MCR_DTR;
		}
		else
		{
			return AE350_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	return AE350_DRIVER_OK;
}

/******************************************************************************************
  \fn          AE350_UART_MODEM_STATUS uart_get_modem_status (UART_RESOURCES *uartx)
  \brief       Get UART Modem Status lines state.
  \param[in]   uartx     Pointer to UART resources
  \return      modem status \ref AE350_UART_MODEM_STATUS
*******************************************************************************************/
static AE350_UART_MODEM_STATUS uart_get_modem_status (UART_RESOURCES *uartx)
{
	AE350_UART_MODEM_STATUS modem_status;
	uint32_t msr;

	if (uartx->info->flags & UART_FLAG_CONFIGURED)
	{
		msr = uartx->reg->MSR;

		if (uartx->capabilities.cts)
		{
			modem_status.cts = (msr & UARTC_MSR_CTS ? (1U) : (0U));
		}
		else
		{
			modem_status.cts = 0U;
		}

		if (uartx->capabilities.dsr)
		{
			modem_status.dsr = (msr & UARTC_MSR_DSR ? (1U) : (0U));
		}
		else
		{
			modem_status.dsr = 0U;
		}

		if (uartx->capabilities.ri )
		{
			modem_status.ri  = (msr & UARTC_MSR_RI  ? (1U) : (0U));
		}
		else
		{
			modem_status.ri  = 0U;
		}

		if (uartx->capabilities.dcd)
		{
			modem_status.dcd = (msr & UARTC_MSR_DCD ? (1U) : (0U));
		}
		else
		{
			modem_status.dcd = 0U;
		}

	}
	else
	{
		modem_status.cts = 0U;
		modem_status.dsr = 0U;
		modem_status.ri  = 0U;
		modem_status.dcd = 0U;
	}

	return modem_status;
}

/******************************************************************************************
  \fn          void uart_irq_handler (UART_RESOURCES *uartx)
  \brief       UART Interrupt handler.
  \param[in]   uartx     Pointer to UART resources
*******************************************************************************************/
static void uart_irq_handler (UART_RESOURCES *uartx)
{
	uint32_t iir, event, val, i;

	event = 0U;
	iir   = uartx->reg->IIR;

	if ((iir & UARTC_IIR_NONE) == 0U)
	{
		// Transmit holding register empty
		if ((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_THRE)
		{
			val = 16U;
			while ((val --) && (uartx->info->xfer.tx_num != uartx->info->xfer.tx_cnt))
			{
				// Write data to TX FIFO
				uartx->reg->THR = uartx->info->xfer.tx_buf[uartx->info->xfer.tx_cnt];
				uartx->info->xfer.tx_cnt++;
			}

			// Check if all data is transmitted
			if (uartx->info->xfer.tx_num == uartx->info->xfer.tx_cnt)
			{
				// Disable THRE interrupt
				uartx->reg->IER &= ~UARTC_IER_THRE;

				// Clear TX busy flag
				uartx->info->xfer.send_active = 0U;

				// Set send complete event
				event |= AE350_UART_EVENT_SEND_COMPLETE;
			}
		}

		// Receive line status
		if ((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_RLS)
		{
			event |= uart_rxline_irq_handler(uartx);
		}

		// Receive data available and Character time-out indicator interrupt
		if (((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_RDA)  ||
			((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_RTO))
		{
			switch (uartx->trig_lvl)
			{
			case UART_TRIG_LVL_1:
				i = 1U;
				break;
			case UART_TRIG_LVL_4:
				i = 3U;
				break;
			case UART_TRIG_LVL_8:
				i = 7U;
				break;
			case UART_TRIG_LVL_14:
				i = 13U;
				break;
			default:
				i = 1U;
				break;
			}

			// Get available data from RX FIFO
			while ((uartx->reg->LSR & UARTC_LSR_RDR) && (i--))
			{
				// Check RX line interrupt for errors
				event |= uart_rxline_irq_handler(uartx);

				// Read data from RX FIFO into receive buffer
				uartx->info->xfer.rx_buf[uartx->info->xfer.rx_cnt] = uartx->reg->RBR;

				uartx->info->xfer.rx_cnt++;

				// Check if requested amount of data is received
				if (uartx->info->xfer.rx_cnt == uartx->info->xfer.rx_num)
				{
					// Disable RDA interrupt
					uartx->reg->IER &= ~UARTC_IER_RDR;

					// Clear RX busy flag and set receive transfer complete event
					uartx->info->rx_status.rx_busy = 0U;
					event |= AE350_UART_EVENT_RECEIVE_COMPLETE;

					break;
				}
			}
		}

		// Character time-out indicator
		if ((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_RTO)
		{
			// Signal RX Time-out event, if not all requested data received
			if (uartx->info->xfer.rx_cnt != uartx->info->xfer.rx_num)
			{
				event |= AE350_UART_EVENT_RX_TIMEOUT;
			}
		}

		// Modem interrupt
		if ((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_MODEM)
		{
			// Save modem status register
			val = uartx->reg->MSR;

			// CTS state changed
			if ((uartx->capabilities.cts) && (val & UARTC_MSR_DCTS))
			{
				event |= AE350_UART_EVENT_CTS;
			}

			// DSR state changed
			if ((uartx->capabilities.dsr) && (val & UARTC_MSR_DDSR))
			{
				event |= AE350_UART_EVENT_DSR;
			}

			// Ring indicator
			if ((uartx->capabilities.ri)  && (val & UARTC_MSR_TERI))
			{
				event |= AE350_UART_EVENT_RI;
			}

			// DCD state changed
			if ((uartx->capabilities.dcd) && (val & UARTC_MSR_DDCD))
			{
				event |= AE350_UART_EVENT_DCD;
			}

		}
	}

	if ((uartx->info->cb_event != NULL) && (event != 0U))
	{
		uartx->info->cb_event (event);
	}
}

#if (((DRV_UART1) && (DRV_UART1_DMA_TX_EN == 1)) || \
     ((DRV_UART2) && (DRV_UART2_DMA_TX_EN == 1)))
/******************************************************************************************
  \fn          void uart_dma_tx_event (uint32_t event, UART_RESOURCES *uartx)
  \brief       UART Interrupt handler.
  \param[in]   uartx     Pointer to UART resources
  \param[in]   event     DMA_EVENT_TERMINAL_COUNT_REQUEST / DMA_EVENT_ERROR
*******************************************************************************************/
static void uart_dma_tx_event (uint32_t event, UART_RESOURCES *uartx)
{
	switch (event)
	{
	case DMA_EVENT_TERMINAL_COUNT_REQUEST:
		uartx->info->xfer.tx_cnt = uartx->info->xfer.tx_num;
		// Clear TX busy flag
		uartx->info->xfer.send_active = 0U;

		// Set send complete event for asynchronous transfers
		if (uartx->info->cb_event)
		{
			uartx->info->cb_event (AE350_UART_EVENT_SEND_COMPLETE);
		}

		break;

	case DMA_EVENT_ERROR:
		break;
	default:
		break;
	}
}
#endif


#if (((DRV_UART1) && (DRV_UART1_DMA_RX_EN == 1)) || \
     ((DRV_UART2) && (DRV_UART2_DMA_RX_EN == 1)))
/******************************************************************************************
  \fn          void uart_dma_rx_event (uint32_t event, UART_RESOURCES *uartx)
  \brief       UART Interrupt handler.
  \param[in]   event     DMA_EVENT_TERMINAL_COUNT_REQUEST / DMA_EVENT_ERROR
  \param[in]   uartx     Pointer to UART resources
*******************************************************************************************/
static void uart_dma_rx_event (uint32_t event, UART_RESOURCES *uartx)
{
	switch (event)
	{
	case DMA_EVENT_TERMINAL_COUNT_REQUEST:
		uartx->info->xfer.rx_cnt    = uartx->info->xfer.rx_num;
		uartx->info->rx_status.rx_busy = 0U;

		if (uartx->info->cb_event)
		{
			uartx->info->cb_event (AE350_UART_EVENT_RECEIVE_COMPLETE);
		}

		break;

	case DMA_EVENT_ERROR:
		break;
	default:
		break;
	}
}
#endif


// UART1
#if (DRV_UART1)

// UART1 driver wrapper functions
static AE350_UART_CAPABILITIES uart1_get_capabilities (void)
{
	return uart_get_capabilities (&uart1_resources);
}

static int32_t uart1_initialize (AE350_UART_SignalEvent_t cb_event)
{
	return uart_initialize (cb_event, &uart1_resources);
}

static int32_t uart1_uninitialize (void)
{
	return uart_uninitialize(&uart1_resources);
}

static int32_t uart1_power_control (AE350_POWER_STATE state)
{
	return uart_power_control (state, &uart1_resources);
}

static int32_t uart1_send (const void *data, uint32_t num)
{
	return uart_send (data, num, &uart1_resources);
}

static int32_t uart1_receive (void *data, uint32_t num)
{
	return uart_receive (data, num, &uart1_resources);
}

static int32_t uart1_transfer (const void *data_out, void *data_in, uint32_t num)
{
	return uart_transfer (data_out, data_in, num, &uart1_resources);
}

static uint32_t uart1_get_txcount (void)
{
	return uart_get_txcount (&uart1_resources);
}

static uint32_t uart1_get_rxcount (void)
{
	return uart_get_rxcount (&uart1_resources);
}

static int32_t uart1_control (uint32_t control, uint32_t arg)
{
	return uart_control (control, arg, &uart1_resources);
}

static AE350_UART_STATUS uart1_get_status (void)
{
	return uart_get_status (&uart1_resources);
}

static int32_t uart1_set_modem_control (AE350_UART_MODEM_CONTROL control)
{
	return uart_set_modem_control (control, &uart1_resources);
}

static AE350_UART_MODEM_STATUS uart1_get_modem_status (void)
{
	return uart_get_modem_status (&uart1_resources);
}

#if (DRV_UART1_DMA_TX_EN == 1)
void uart1_dma_tx_event (uint32_t event)
{
	uart_dma_tx_event(event, &uart1_resources);
}
#endif

#if (DRV_UART1_DMA_RX_EN == 1)
void uart1_dma_rx_event (uint32_t event)
{
	uart_dma_rx_event(event, &uart1_resources);
}
#endif

// UART1 interrupt handler
void uart1_irq_handler (void)
{
	uart_irq_handler (&uart1_resources);
}


// UART1 driver control block
AE350_DRIVER_UART Driver_UART1 =
{
	uart_get_version,			// Get version
	uart1_get_capabilities,		// Get capabilities
	uart1_initialize,			// Initializes
	uart1_uninitialize,			// Uninitialized
	uart1_power_control,		// Power control
	uart1_send,					// Send data
	uart1_receive,				// Receive data
	uart1_transfer,				// Transfer data
	uart1_get_txcount,			// Get transmitted data count
	uart1_get_rxcount,			// Get received data count
	uart1_control,				// Control
	uart1_get_status,			// Get status
	uart1_set_modem_control,	// Set modem control
	uart1_get_modem_status		// Get modem control
};

#endif	// DRV_UART1


// UART2
#if (DRV_UART2)

// UART2 driver wrapper functions
static AE350_UART_CAPABILITIES uart2_get_capabilities (void)
{
	return uart_get_capabilities (&uart2_resources);
}

static int32_t uart2_initialize (AE350_UART_SignalEvent_t cb_event)
{
	return uart_initialize (cb_event, &uart2_resources);
}

static int32_t uart2_uninitialize (void)
{
	return uart_uninitialize(&uart2_resources);
}

static int32_t uart2_power_control (AE350_POWER_STATE state)
{
	return uart_power_control (state, &uart2_resources);
}
static int32_t uart2_send (const void *data, uint32_t num)
{
	return uart_send (data, num, &uart2_resources);
}

static int32_t uart2_receive (void *data, uint32_t num)
{
	return uart_receive (data, num, &uart2_resources);
}

static int32_t uart2_transfer (const void *data_out, void *data_in, uint32_t num)
{
	return uart_transfer (data_out, data_in, num, &uart2_resources);
}

static uint32_t uart2_get_txcount (void)
{
	return uart_get_txcount (&uart2_resources);
}

static uint32_t uart2_get_rxcount (void)
{
	return uart_get_rxcount (&uart2_resources);
}

static int32_t uart2_control (uint32_t control, uint32_t arg)
{
	return uart_control (control, arg, &uart2_resources);
}

static AE350_UART_STATUS uart2_get_status (void)
{
	return uart_get_status (&uart2_resources);
}

static int32_t uart2_set_modem_control (AE350_UART_MODEM_CONTROL control)
{
	return uart_set_modem_control (control, &uart2_resources);
}

static AE350_UART_MODEM_STATUS uart2_get_modem_status (void)
{
	return uart_get_modem_status (&uart2_resources);
}

#if (DRV_UART2_DMA_TX_EN == 1)
void uart2_dma_tx_event (uint32_t event)
{
	uart_dma_tx_event(event, &uart2_resources);
}
#endif

#if (DRV_UART2_DMA_RX_EN == 1)
void uart2_dma_rx_event (uint32_t event)
{
	uart_dma_rx_event(event, &uart2_resources);
}
#endif

// UART2 interrupt handler
void uart2_irq_handler (void)
{
	uart_irq_handler (&uart2_resources);
}


// UART2 driver control block
AE350_DRIVER_UART Driver_UART2 =
{
	uart_get_version,			// Get version
	uart2_get_capabilities,		// Get capabilities
	uart2_initialize,			// Initializes
	uart2_uninitialize,			// Uninitialized
	uart2_power_control,		// Power control
	uart2_send,					// Send data
	uart2_receive,				// Receive data
	uart2_transfer,				// Transfer data
	uart2_get_txcount,			// Get transmitted data count
	uart2_get_rxcount,			// Get received data count
	uart2_control,				// Control
	uart2_get_status,			// Get status
	uart2_set_modem_control,	// Set modem control
	uart2_get_modem_status		// Get modem status
};

#endif	// DRV_UART2
