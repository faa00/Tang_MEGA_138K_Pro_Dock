/*
 * ******************************************************************************************
 * File		: spi_ae350.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: SPI driver definitions
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "spi_ae350.h"


// Definitions  -----------------------------------------------------------------------------

#define AE350_SPI_DRV_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,9)

// SPI clock for AE350_SOC is 66MHz
#define SPI_CLK    66000000

// SPI transfer operation
#define SPI_SEND                      0x0
#define SPI_RECEIVE                   0x1
#define SPI_TRANSFER                  0x2

#if (!DRV_SPI)
	#error "SPI not enabled in config.h!"
#endif

// Driver version
static const AE350_DRIVER_VERSION spi_driver_version = {AE350_SPI_API_VERSION, AE350_SPI_DRV_VERSION};

// SPI
#if (DRV_SPI)
static SPI_INFO spi_info = {0};

#if (DRV_SPI_DMA_TX_EN == 1)
void spi_dma_tx_event(uint32_t event);
static SPI_DMA spi_dma_tx = {DRV_SPI_DMA_TX_CH, DRV_SPI_DMA_TX_REQID, spi_dma_tx_event};
#endif

#if (DRV_SPI_DMA_RX_EN == 1)
void spi_dma_rx_event(uint32_t event);
static SPI_DMA spi_dma_rx = {DRV_SPI_DMA_RX_CH, DRV_SPI_DMA_RX_REQID, spi_dma_rx_event};
#endif

// SPI resources
static const SPI_RESOURCES spi_resources =
{
	{0},
	DEV_SPI,
	IRQ_SPI_SOURCE,
#if (DRV_SPI_DMA_TX_EN == 1)
	&spi_dma_tx,
#else
	NULL,
#endif
#if (DRV_SPI_DMA_RX_EN == 1)
	&spi_dma_rx,
#else
	NULL,
#endif
	&spi_info
};
#endif	// DRV_SPI


// Get SPI revision number
static inline uint32_t spix_ip_revision_number(SPI_RESOURCES *spi)
{
	return spi->reg->IDREV & (SPI_IDREV_MAJOR_MASK | SPI_IDREV_MINOR_MASK);
}

/* SPI_REVISION_NUM(major, minor): Generate revision number from major and minor number. */
#define SPI_REVISION_NUM(major, minor)  (((major << SPI_IDREV_MAJOR_BIT) & SPI_IDREV_MAJOR_MASK) | (minor & SPI_IDREV_MINOR_MASK))

/*
 * SPI FIFO depth to 128, and add slave data-only transfer feature.
 */
#define SPI_IP_HAS_FIFO_DEPTH_128(spi)  (spix_ip_revision_number(spi) > SPI_REVISION_NUM(4, 6))


// Get TX FIFO size
static uint32_t spix_get_txfifo_size(SPI_RESOURCES *spi)
{
	if (SPI_IP_HAS_FIFO_DEPTH_128(spi))
	{
		return 2 << ((spi->reg->CONFIG >> 4) & 0xf);
	}
	else
	{
		return 2 << ((spi->reg->CONFIG >> 4) & 0x3);
	}
}

static void spix_polling_spiactive(SPI_RESOURCES *spi)
{
	while ((spi->reg->STATUS) & 0x1);
}

// Get capabilities
static AE350_SPI_CAPABILITIES spix_get_capabilities(SPI_RESOURCES *spi)
{
	return spi->capabilities;
}

// Initializes SPI
static int32_t spix_initialize(AE350_SPI_SignalEvent_t cb_event, SPI_RESOURCES *spi)
{
	if (spi->info->flags & SPI_FLAG_INITIALIZED)
	{
		return AE350_DRIVER_OK;
	}

	// Initialize SPI run-time resources
	spi->info->cb_event          = cb_event;

	spi->info->status.busy       = 0U;
	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->xfer.rx_buf       = 0U;
	spi->info->xfer.tx_buf       = 0U;
	spi->info->xfer.tx_buf_limit = 0U;
	spi->info->xfer.rx_cnt       = 0U;
	spi->info->xfer.tx_cnt       = 0U;

	spi->info->mode              = 0U;
	spi->info->record_rx_buf     = 0U;
	spi->info->is_header         = 0U;
	spi->info->txfifo_size = spix_get_txfifo_size(spi);

	spi->info->flags = SPI_FLAG_INITIALIZED;  // SPI is initialized

	spi->info->xfer.dma_tx_complete = 0;
	spi->info->xfer.dma_rx_complete = 0;

	return AE350_DRIVER_OK;
}

// Uninitialized
static int32_t spix_uninitialize(SPI_RESOURCES *spi)
{
	spi->info->flags = 0U;                    // SPI is uninitialized

	return AE350_DRIVER_OK;
}

// Control
static int32_t spix_power_control(AE350_POWER_STATE state, SPI_RESOURCES *spi)
{
	switch (state)
	{
		case AE350_POWER_OFF:
			// Disable PLIC interrupt SPI0 source
			__nds__plic_disable_interrupt(spi->irq_num);

			// DMA un-initialize
			if (spi->dma_tx || spi->dma_rx)
			{
				dma_uninitialize();

				if (spi->dma_tx && (spi->info->status.busy != 0))
				{
					dma_channel_disable(spi->dma_tx->channel);
				}

				if (spi->dma_rx && (spi->info->status.busy != 0))
				{
					dma_channel_disable(spi->dma_rx->channel);
				}
			}

			// Reset SPI and TX/RX FIFOs
			spi->reg->CTRL = (TXFIFORST | RXFIFORST | SPIRST);

			// Clear SPI run-time resources
			spi->info->status.busy       = 0U;
			spi->info->status.data_lost  = 0U;
			spi->info->status.mode_fault = 0U;

			spi->info->xfer.rx_buf       = 0U;
			spi->info->xfer.tx_buf       = 0U;
			spi->info->xfer.rx_cnt       = 0U;
			spi->info->xfer.tx_cnt       = 0U;

			spi->info->mode              = 0U;

			spi->info->flags &= ~SPI_FLAG_POWERED;

			break;

		case AE350_POWER_LOW:
			return AE350_DRIVER_ERROR_UNSUPPORTED;

		case AE350_POWER_FULL:
			if ((spi->info->flags & SPI_FLAG_INITIALIZED) == 0U)
			{
				return AE350_DRIVER_ERROR;
			}

			if ((spi->info->flags & SPI_FLAG_POWERED) != 0U)
			{
				return AE350_DRIVER_OK;
			}

			// DMA initialize
			if (spi->dma_tx || spi->dma_rx)
			{
				dma_initialize();
			}

			// Reset SPI and TX/RX FIFOs
			spi->reg->CTRL = (TXFIFORST | RXFIFORST | SPIRST);

			// Clear SPI run-time resources
			spi->info->status.busy       = 0U;
			spi->info->status.data_lost  = 0U;
			spi->info->status.mode_fault = 0U;

			spi->info->xfer.rx_buf       = 0U;
			spi->info->xfer.tx_buf       = 0U;
			spi->info->xfer.rx_cnt       = 0U;
			spi->info->xfer.tx_cnt       = 0U;

			spi->info->mode              = 0U;

			// Priority must be set > 0 to trigger the interrupt
			__nds__plic_set_priority(spi->irq_num, 1);

			// Enable PLIC interrupt SPI0 source
			__nds__plic_enable_interrupt(spi->irq_num);

			// Enable the Machine-External bit in MIE
			set_csr(NDS_MIE, MIP_MEIP);

			// Enable GIE
			set_csr(NDS_MSTATUS, MSTATUS_MIE);

			spi->info->flags |= SPI_FLAG_POWERED;   // SPI is powered

			break;

		default:
			return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	return AE350_DRIVER_OK;
}

// Send data
static int32_t spix_send(const void *data, uint32_t num, SPI_RESOURCES *spi)
{
	int32_t stat;

	if ((data == NULL) || (num == 0U))
	{
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
	{
		return AE350_DRIVER_ERROR;
	}

	if (spi->info->status.busy)
	{
		return AE350_DRIVER_ERROR_BUSY;
	}

	// Set busy flag
	spi->info->status.busy       = 1U;

	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->xfer.tx_buf   = (uint8_t *)data;
	spi->info->xfer.tx_cnt   = 0U;

	spi->info->xfer.transfer_op = SPI_SEND;

	// Wait prior transfer finish
	spix_polling_spiactive(spi);

	if ((spi->info->mode & AE350_SPI_TRANSFER_FORMAT_Msk) != AE350_SPI_SLV_DATA_ONLY_TRANSFER)
	{
		// Set transfer mode to write only and transfer count for write data
		spi->reg->TRANSCTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->TRANSCTRL |= (SPI_TRANSMODE_WRONLY | WR_TRANCNT(num));
	}
	else
	{
		// SlvDataOnly mode should set TransMode to 0
		spi->reg->TRANSCTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->TRANSCTRL |= (SPI_TRANSCTRL_SLV_DATA_ONLY | WR_TRANCNT(num));
	}

	// Set TX FIFO threshold to 2
	spi->reg->CTRL = TXTHRES(2);

	// Prepare info that ISR needed
	spi->info->xfer.txfifo_refill = spi->info->txfifo_size - 2; // TX FIFO threshold = 2

	if (spi->info->data_bits <= 8)
	{
		// Data bits = 1....8
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num));
		spi->info->src_width = 1;
	}
	else if (spi->info->data_bits <= 16)
	{
		// Data bits = 9....16
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num * 2));
		spi->info->src_width = 2;
	}
	else
	{
		// Data bits = 17....32
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num * 4));
		spi->info->src_width = 4;
	}

	// DMA mode
	if (spi->dma_tx)
	{
		// Enable TX DMA
		spi->reg->CTRL |= TXDMAEN;

		// Configure DMA channel
		stat = dma_channel_configure(spi->dma_tx->channel,
					     (uint32_t)(long)spi->info->xfer.tx_buf,
					     (uint32_t)(long)(&(spi->reg->DATA)),
					     num,
					     DMA_CH_CTRL_SBINF(DMA_INF_IDX1) |
					     DMA_CH_CTRL_DBINF(DMA_INF_IDX1) |
					     DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
					     // DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
					     DMA_CH_CTRL_SWIDTH(spi->info->src_width/2) |
					     DMA_CH_CTRL_DWIDTH(spi->info->src_width/2) |
					     DMA_CH_CTRL_DMODE_HANDSHAKE |
					     DMA_CH_CTRL_SRCADDR_INC |
					     DMA_CH_CTRL_DSTADDR_FIX |
					     DMA_CH_CTRL_DSTREQ(spi->dma_tx->reqsel) |
					     DMA_CH_CTRL_INTABT |
					     DMA_CH_CTRL_INTERR |
					     DMA_CH_CTRL_INTTC |
					     DMA_CH_CTRL_ENABLE,
					     spi->dma_tx->cb_event);

		if (stat == -1)
		{
			return AE350_DRIVER_ERROR;
		}

		// Enable interrupts
		spi->reg->INTREN = SPI_ENDINT;
	}
	else
	{
		// Interrupt mode

		// Enable interrupts
		spi->reg->INTREN = (SPI_TXFIFOINT | SPI_ENDINT);

		// Enable TX FIFO under run interrupt when slave mode
		if ((spi->info->mode & AE350_SPI_CONTROL_Msk) == AE350_SPI_MODE_SLAVE)
		{
			spi->reg->INTREN |= SPI_TXFIFOOURINT;
		}
	}

	// Trigger transfer when SPI master mode
	if ((spi->info->mode & AE350_SPI_CONTROL_Msk) == AE350_SPI_MODE_MASTER)
	{
		spi->reg->CMD = 0U;
	}

	return AE350_DRIVER_OK;
}

// Receive data
static int32_t spix_receive(void *data, uint32_t num, SPI_RESOURCES *spi)
{
	int32_t stat;

	if ((data == NULL) || (num == 0U))
	{
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
	{
		return AE350_DRIVER_ERROR;
	}

	if (spi->info->status.busy)
	{
		return AE350_DRIVER_ERROR_BUSY;
	}

	// Set busy flag
	spi->info->status.busy       = 1U;

	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->xfer.rx_buf = (uint8_t *)data;
	spi->info->xfer.rx_cnt = 0U;

	spi->info->xfer.transfer_op = SPI_RECEIVE;

	// Wait prior transfer finish
	spix_polling_spiactive(spi);

	if ((spi->info->mode & AE350_SPI_TRANSFER_FORMAT_Msk) != AE350_SPI_SLV_DATA_ONLY_TRANSFER)
	{
		// Set transfer mode to read only and transfer count for read data
		spi->reg->TRANSCTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->TRANSCTRL |= (SPI_TRANSMODE_RDONLY | RD_TRANCNT(num));
	}
	else
	{
		// SlvDataOnly mode should set TransMode to 0
		spi->reg->TRANSCTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->TRANSCTRL |= (SPI_TRANSCTRL_SLV_DATA_ONLY | RD_TRANCNT(num));
	}

	// Set RX FIFO threshold to 2
	spi->reg->CTRL = RXTHRES(1);

	if (spi->info->data_bits <= 8)
	{
		// Data bits = 1....8
		spi->info->src_width = 1;
	}
	else if (spi->info->data_bits <= 16)
	{
		// Data bits = 9....16
		spi->info->src_width = 2;
	}
	else
	{
		// Data bits = 17....32
		spi->info->src_width = 4;
	}

	// DMA mode
	if (spi->dma_rx)
	{
		// Enable RX DMA
		spi->reg->CTRL |= RXDMAEN;

		// Configure DMA channel
		stat = dma_channel_configure(spi->dma_rx->channel,
					     (uint32_t)(long)(&(spi->reg->DATA)),
					     (uint32_t)(long)spi->info->xfer.rx_buf,
					     num,
					     DMA_CH_CTRL_SBINF(DMA_INF_IDX1) |
					     DMA_CH_CTRL_DBINF(DMA_INF_IDX1) |
					     DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
					     // DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
					     DMA_CH_CTRL_SWIDTH(spi->info->src_width/2) |
					     DMA_CH_CTRL_DWIDTH(spi->info->src_width/2) |
					     DMA_CH_CTRL_SMODE_HANDSHAKE |
					     DMA_CH_CTRL_SRCADDR_FIX |
					     DMA_CH_CTRL_DSTADDR_INC |
					     DMA_CH_CTRL_SRCREQ(spi->dma_rx->reqsel) |
					     DMA_CH_CTRL_INTABT |
					     DMA_CH_CTRL_INTERR |
					     DMA_CH_CTRL_INTTC |
					     DMA_CH_CTRL_ENABLE,
					     spi->dma_rx->cb_event);

		if (stat == -1)
		{
			return AE350_DRIVER_ERROR;
		}

		// Enable interrupts
		spi->reg->INTREN = SPI_ENDINT;
	}
	else
	{
		// Interrupt mode

		// Enable interrupts
		spi->reg->INTREN = (SPI_RXFIFOINT | SPI_ENDINT);

		// Enable RX FIFO overrun interrupt when slave mode
		if ((spi->info->mode & AE350_SPI_CONTROL_Msk) == AE350_SPI_MODE_SLAVE)
		{
			spi->reg->INTREN |= SPI_RXFIFOOORINT;
		}
	}

	// Trigger transfer when SPI master mode
	if ((spi->info->mode & AE350_SPI_CONTROL_Msk) == AE350_SPI_MODE_MASTER)
	{
		spi->reg->CMD = 0U;
	}

	return AE350_DRIVER_OK;
}

// Transfer data
static int32_t spix_transfer(const void *data_out, void *data_in, uint32_t num, SPI_RESOURCES *spi)
{
	uint32_t dma_rx_num = 0;
	int32_t stat;

	if ((data_out == NULL) || (data_in == NULL) || (num == 0U))
	{
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
	{
		return AE350_DRIVER_ERROR;
	}

	if (spi->info->status.busy)
	{
		return AE350_DRIVER_ERROR_BUSY;
	}

	if (spi->info->tx_header_len > num)
	{
		return AE350_SPI_ERROR_HEADER_LEN;
	}

	// Set busy flag
	spi->info->status.busy       = 1U;

	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->xfer.rx_buf = (uint8_t *)data_in;
	spi->info->xfer.tx_buf = (uint8_t *)data_out;
	spi->info->xfer.rx_cnt = 0U;
	spi->info->xfer.tx_cnt = 0U;

	spi->info->xfer.transfer_op = SPI_TRANSFER;

	// Wait prior transfer finish
	spix_polling_spiactive(spi);

	if ((spi->info->mode & AE350_SPI_TRANSFER_FORMAT_Msk) != AE350_SPI_SLV_DATA_ONLY_TRANSFER)
	{
		// Set transfer mode to write and read at the same time and transfer count for write/read data
		spi->reg->TRANSCTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->TRANSCTRL |= (SPI_TRANSMODE_WRnRD | WR_TRANCNT(num) | RD_TRANCNT(num));
	}
	else
	{
		// SlvDataOnly mode should set TransMode to 0
		spi->reg->TRANSCTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->TRANSCTRL |= (SPI_TRANSCTRL_SLV_DATA_ONLY | WR_TRANCNT(num) | RD_TRANCNT(num));
	}

	if (spi->info->data_bits <= 8)
	{
		// Data bits = 1....8
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num));
		spi->info->src_width = 1;
	}
	else if (spi->info->data_bits <= 16)
	{
		// Data bits = 9....16
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num * 2));
		spi->info->src_width = 2;
	}
	else
	{
		// Data bits = 17....32
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num * 4));
		spi->info->src_width = 4;
	}

	if ((spi->info->tx_header_len > 0) && (spi->info->mode & AE350_SPI_CONTROL_Msk))
	{
		// If there is TX header, only setting DMA to receive the header at first time
		dma_rx_num = spi->info->tx_header_len;
		spi->info->record_rx_buf = (uint8_t *)(spi->info->xfer.rx_buf);
		spi->info->is_header = 1;
		spi->info->data_num = num - spi->info->tx_header_len;
	}
	else
	{
		dma_rx_num = num;
	}

	spi->reg->CTRL &= 0x0;

	// Set TX FIFO threshold and RX FIFO threshold to 2
	spi->reg->CTRL |= (TXTHRES(2) | RXTHRES(1));

	// Prepare info that ISR needed
	spi->info->xfer.txfifo_refill = spi->info->txfifo_size - 2; // TX FIFO threshold = 2

	// DMA mode
	if (spi->dma_tx || spi->dma_rx)
	{
		if (spi->dma_tx)
		{
			// Enable TX DMA
			spi->reg->CTRL |= TXDMAEN;

			// Configure DMA channel
			stat = dma_channel_configure(spi->dma_tx->channel,
						      (uint32_t)(long)spi->info->xfer.tx_buf,
						      (uint32_t)(long)(&(spi->reg->DATA)),
						      num,
						      DMA_CH_CTRL_SBINF(DMA_INF_IDX1) |
						      DMA_CH_CTRL_DBINF(DMA_INF_IDX1) |
						      DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
						      // DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
						      DMA_CH_CTRL_SWIDTH(spi->info->src_width/2) |
						      DMA_CH_CTRL_DWIDTH(spi->info->src_width/2) |
						      DMA_CH_CTRL_DMODE_HANDSHAKE |
						      DMA_CH_CTRL_SRCADDR_INC |
						      DMA_CH_CTRL_DSTADDR_FIX |
						      DMA_CH_CTRL_DSTREQ(spi->dma_tx->reqsel) |
						      DMA_CH_CTRL_INTABT |
						      DMA_CH_CTRL_INTERR |
						      DMA_CH_CTRL_INTTC |
						      DMA_CH_CTRL_ENABLE,
						      spi->dma_tx->cb_event);
			if (stat == -1)
			{
				return AE350_DRIVER_ERROR;
			}
		}

		if (spi->dma_rx)
		{
			// Enable RX DMA
			spi->reg->CTRL |= RXDMAEN;

			// Configure DMA channel
			stat = dma_channel_configure(spi->dma_rx->channel,
						     (uint32_t)(long)(&(spi->reg->DATA)),
						     (uint32_t)(long)spi->info->xfer.rx_buf,
						     dma_rx_num,
						     DMA_CH_CTRL_SBINF(DMA_INF_IDX0) |
						     DMA_CH_CTRL_DBINF(DMA_INF_IDX0) |
						     DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
						     // DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
						     DMA_CH_CTRL_SWIDTH(spi->info->src_width/2) |
						     DMA_CH_CTRL_DWIDTH(spi->info->src_width/2) |
						     DMA_CH_CTRL_SMODE_HANDSHAKE |
						     DMA_CH_CTRL_SRCADDR_FIX |
						     DMA_CH_CTRL_DSTADDR_INC |
						     DMA_CH_CTRL_SRCREQ(spi->dma_rx->reqsel) |
						     DMA_CH_CTRL_INTABT |
						     DMA_CH_CTRL_INTERR |
						     DMA_CH_CTRL_INTTC |
						     DMA_CH_CTRL_ENABLE,
						     spi->dma_rx->cb_event);
			if (stat == -1)
			{
				return AE350_DRIVER_ERROR;
			}
		}

		// Enable interrupts
		spi->reg->INTREN = SPI_ENDINT;
	}
	else
	{
		// Interrupt mode

		// Enable interrupts
		spi->reg->INTREN = (SPI_TXFIFOINT | SPI_RXFIFOINT | SPI_ENDINT);

		// Enable TX FIFO under run and RX FIFO overrun interrupt when slave mode
		if ((spi->info->mode & AE350_SPI_CONTROL_Msk) == AE350_SPI_MODE_SLAVE)
		{
			spi->reg->INTREN |= (SPI_TXFIFOOURINT | SPI_RXFIFOOORINT);
		}
	}

	// Trigger transfer when SPI master mode
	if ((spi->info->mode & AE350_SPI_CONTROL_Msk) == AE350_SPI_MODE_MASTER)
	{
		spi->reg->CMD = 0;
	}

	return AE350_DRIVER_OK;
}

// Get data count
static int32_t spix_get_data_count(SPI_RESOURCES *spi)
{
	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
	{
		return 0U;
	}

	switch (spi->info->xfer.transfer_op)
	{
		case SPI_SEND:
		case SPI_TRANSFER:
			if (spi->dma_tx)
			{
				return (dma_channel_get_count(spi->dma_tx->channel));
			}
			else
			{
				return (spi->info->xfer.tx_cnt);
			}
		case SPI_RECEIVE:
			if (spi->dma_rx)
			{
				return (dma_channel_get_count(spi->dma_rx->channel));
			}
			else
			{
				return (spi->info->xfer.rx_cnt);
			}
		default:
			return AE350_DRIVER_OK;
	}
}

// Control
static int32_t spix_control(uint32_t control, uint32_t arg, SPI_RESOURCES *spi)
{
	uint32_t sclk_div;

	if (!(spi->info->flags & SPI_FLAG_POWERED))
	{
		return AE350_DRIVER_ERROR;
	}

	if ((control & AE350_SPI_CONTROL_Msk) == AE350_SPI_ABORT_TRANSFER)
	{
		// Abort SPI transfer
		// Disable SPI interrupts
		spi->reg->INTREN = 0;

		// Clear SPI run-time resources
		spi->info->status.busy = 0U;

		spi->info->xfer.rx_buf = 0U;
		spi->info->xfer.tx_buf = 0U;
		spi->info->xfer.rx_cnt = 0U;
		spi->info->xfer.tx_cnt = 0U;

		spi->info->mode        = 0U;

		return AE350_DRIVER_OK;
	}

	if (spi->info->status.busy)
	{
		return AE350_DRIVER_ERROR_BUSY;
	}

	switch (control & AE350_SPI_CONTROL_Msk)
	{
		case AE350_SPI_MODE_MASTER_SIMPLEX:  // SPI master (output/input on MOSI); arg = bus speed in bps
		case AE350_SPI_MODE_SLAVE_SIMPLEX:   // SPI slave (output/input on MISO
		case AE350_SPI_SET_DEFAULT_TX_VALUE: // Set default Transmit value; arg = value
		case AE350_SPI_CONTROL_SS:           // Control slave select; arg = 0:inactive, 1:active
			return AE350_SPI_ERROR_MODE;

		case AE350_SPI_MODE_INACTIVE:		 // SPI inactive
			spi->info->mode &= ~AE350_SPI_CONTROL_Msk;
			spi->info->mode |= AE350_SPI_MODE_INACTIVE;

			spi->info->flags &= ~SPI_FLAG_CONFIGURED;

			return AE350_DRIVER_OK;

		case AE350_SPI_MODE_MASTER:
			// SPI master (output on MOSI, input on MISO); arg = bus speed in bps
			// Set master mode and disable data merge mode
			spi->reg->TRANSFMT &= ~(SPI_MERGE | SPI_SLAVE);

			spi->info->mode &= ~AE350_SPI_CONTROL_Msk;
			spi->info->mode |= AE350_SPI_MODE_MASTER;

			spi->info->flags |= SPI_FLAG_CONFIGURED;

			goto set_speed;

		case AE350_SPI_MODE_SLAVE:
			// SPI slave (output on MISO, input on MOSI)
			// Set slave mode and disable data merge mode
			spi->reg->TRANSFMT &= ~SPI_MERGE;
			spi->reg->TRANSFMT |= SPI_SLAVE;

			spi->info->mode &= ~AE350_SPI_CONTROL_Msk;
			spi->info->mode |= AE350_SPI_MODE_SLAVE;

			spi->info->flags |= SPI_FLAG_CONFIGURED;

			break;

		case AE350_SPI_SET_BUS_SPEED:
			// Set bus speed in bps; arg = value
			set_speed:
				if (arg == 0U)
				{
					return AE350_DRIVER_ERROR;
				}

				sclk_div = (SPI_CLK / (2 * arg)) - 1;

				spi->reg->TIMING &= ~0xff;
				spi->reg->TIMING |= sclk_div;

				if ((control & AE350_SPI_CONTROL_Msk) == AE350_SPI_SET_BUS_SPEED)
				{
					return AE350_DRIVER_OK;
				}

			break;

		case AE350_SPI_GET_BUS_SPEED:
			// Get bus speed in bps
			sclk_div = spi->reg->TIMING & 0xff;
			return (SPI_CLK / ((sclk_div + 1) * 2));

		case AE350_SPI_TX_HEADER_LENGTH:
			spi->info->tx_header_len = (uint8_t)arg;
			return AE350_DRIVER_OK;

		default:
			return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	// SPI slave select mode for master
	if ((spi->info->mode & AE350_SPI_CONTROL_Msk) ==  AE350_SPI_MODE_MASTER)
	{
		switch (control & AE350_SPI_SS_MASTER_MODE_Msk)
		{
			case AE350_SPI_SS_MASTER_HW_OUTPUT:
				break;
			case AE350_SPI_SS_MASTER_UNUSED:
			case AE350_SPI_SS_MASTER_SW:
			case AE350_SPI_SS_MASTER_HW_INPUT:
				return AE350_SPI_ERROR_SS_MODE;
			default:
				break;
		}
	}

	// SPI slave select mode for slave
	if ((spi->info->mode & AE350_SPI_CONTROL_Msk) == AE350_SPI_MODE_SLAVE)
	{
		switch (control & AE350_SPI_SS_SLAVE_MODE_Msk)
		{
			case AE350_SPI_SS_SLAVE_HW:
				break;
			case AE350_SPI_SS_SLAVE_SW:
				return AE350_SPI_ERROR_SS_MODE;
			default:
				break;
		}
	}

	// SPI data-only transfer mode for slave
	if ((spi->info->mode & AE350_SPI_CONTROL_Msk) == AE350_SPI_MODE_SLAVE)
	{
		if ((control & AE350_SPI_TRANSFER_FORMAT_Msk) == AE350_SPI_SLV_DATA_ONLY_TRANSFER)
		{
			spi->info->mode &= ~AE350_SPI_TRANSFER_FORMAT_Msk;
			spi->info->mode |= AE350_SPI_SLV_DATA_ONLY_TRANSFER;
		}
	}

	// Set SPI frame format
	switch (control & AE350_SPI_FRAME_FORMAT_Msk)
	{
		case AE350_SPI_CPOL0_CPHA0:
			spi->reg->TRANSFMT &= ~(SPI_CPOL | SPI_CPHA);
			break;
		case AE350_SPI_CPOL0_CPHA1:
			spi->reg->TRANSFMT &= ~SPI_CPOL;
			spi->reg->TRANSFMT |= SPI_CPHA;
			break;
		case AE350_SPI_CPOL1_CPHA0:
			spi->reg->TRANSFMT |= SPI_CPOL;
			spi->reg->TRANSFMT &= ~SPI_CPHA;
			break;
		case AE350_SPI_CPOL1_CPHA1:
			spi->reg->TRANSFMT |= SPI_CPOL;
			spi->reg->TRANSFMT |= SPI_CPHA;
			break;
		case AE350_SPI_TI_SSI:
		case AE350_SPI_MICROWIRE:
		default:
			return AE350_SPI_ERROR_FRAME_FORMAT;
	}

	// Set number of data bits
	spi->info->data_bits = ((control & AE350_SPI_DATA_BITS_Msk) >> AE350_SPI_DATA_BITS_Pos);

	if ((spi->info->data_bits < 1U) || (spi->info->data_bits > 32U))
	{
		return AE350_SPI_ERROR_DATA_BITS;
	}
	else
	{
		spi->reg->TRANSFMT &= ~(DATA_BITS_MSK);
		spi->reg->TRANSFMT |= DATA_BITS(spi->info->data_bits);
	}

	// Set SPI bit order
	if ((control & AE350_SPI_BIT_ORDER_Msk) == AE350_SPI_LSB_MSB)
	{
		spi->reg->TRANSFMT |= SPI_LSB;
	}
	else
	{
		spi->reg->TRANSFMT &= ~SPI_LSB;
	}

	return AE350_DRIVER_OK;
}

// Get status
static AE350_SPI_STATUS spix_get_status(SPI_RESOURCES *spi)
{
	AE350_SPI_STATUS status;

	status.busy       = spi->info->status.busy;
	status.data_lost  = spi->info->status.data_lost;
	status.mode_fault = spi->info->status.mode_fault;

	return (status);
}

// Interrupt handler
static void spix_irq_handler(SPI_RESOURCES *spi)
{
	uint32_t i, j, status;
	uint32_t data = 0;
	uint32_t rx_num = 0;
	uint32_t event = 0;

	// Read status register
	status = spi->reg->INTRST;

	if ((status & SPI_RXFIFOOORINT) || (status & SPI_TXFIFOOURINT))
	{
		// TX FIFO under run or RX FIFO overrun interrupt status
		spi->info->status.data_lost = 1U;

		event |= AE350_SPI_EVENT_DATA_LOST;
	}

	if (status & SPI_TXFIFOINT)
	{
		for (i = 0; i < spi->info->xfer.txfifo_refill; i++)
		{
			data = 0;
			if (spi->info->xfer.tx_buf < spi->info->xfer.tx_buf_limit)
			{
				// Handle the data frame format
				if (spi->info->data_bits <= 8)
				{
					// Data bits = 1....8
					data = *spi->info->xfer.tx_buf;
					spi->info->xfer.tx_buf++;
				}
				else if (spi->info->data_bits <= 16)
				{
					// Data bits = 9....16
					for (j = 0; j < 2; j++)
					{
						data |= *spi->info->xfer.tx_buf << j * 8;
						spi->info->xfer.tx_buf++;
					}
				}
				else
				{
					// Data bits = 17....32
					for (j = 0; j < 4; j++)
					{
						data |= *spi->info->xfer.tx_buf << j * 8;
						spi->info->xfer.tx_buf++;
					}
				}

				spi->reg->DATA = data;
				spi->info->xfer.tx_cnt++;
			}
			else
			{
				spi->reg->INTREN &= ~SPI_TXFIFOINT;
			}
		}
	}

	if (status & SPI_RXFIFOINT)
	{
		// Get number of valid entries in the RX FIFO
		if (SPI_IP_HAS_FIFO_DEPTH_128(spi))
		{
			rx_num = (((spi->reg->STATUS >> 8) & 0x2f) | ((spi->reg->STATUS & (0x3 << 24)) >> 18));
		}
		else
		{
			rx_num = (spi->reg->STATUS >> 8) & 0x1f;
		}

		for (i = rx_num; i > 0; i--)
		{
			data = spi->reg->DATA;
			spi->info->xfer.rx_cnt++;

			// Handle the data frame format
			if (spi->info->data_bits <= 8)
			{
				*spi->info->xfer.rx_buf = data & 0xff;
				spi->info->xfer.rx_buf++;
			}
			else if (spi->info->data_bits <= 16)
			{
				for (j = 0; j < 2; j++)
				{
					*spi->info->xfer.rx_buf = (data >> j * 8) & 0xff;
					spi->info->xfer.rx_buf++;
				}
			}
			else
			{
				for (j = 0; j < 4; j++)
				{
					*spi->info->xfer.rx_buf = (data >> j * 8) & 0xff;
					spi->info->xfer.rx_buf++;
				}
			}

			// If there is a TX header on spi_transfer, drop the dummy data from slave when master sending header.
			if ((spi->info->is_header == 1) && (spi->info->xfer.rx_cnt == spi->info->tx_header_len))
			{
				spi->info->is_header = 0;
				spi->info->xfer.rx_buf = spi->info->record_rx_buf;
				spi->info->xfer.rx_cnt = 0;
			}
		}
	}

	if (status & SPI_ENDINT)
	{
		// Disable SPI interrupts
		spi->reg->INTREN = 0;

		if (spi->dma_tx && ((spi->info->xfer.transfer_op == SPI_SEND) || (spi->info->xfer.transfer_op == SPI_TRANSFER)))
		{
			if (!spi->info->xfer.dma_tx_complete)
			{
				dma_channel_disable(spi->dma_tx->channel);
			}

			spi->info->xfer.dma_tx_complete = 0;
		}

		if (spi->dma_rx && ((spi->info->xfer.transfer_op == SPI_RECEIVE) || (spi->info->xfer.transfer_op == SPI_TRANSFER)))
		{
			if (!spi->info->xfer.dma_rx_complete)
			{
				dma_channel_disable(spi->dma_rx->channel);
			}

			spi->info->xfer.dma_rx_complete = 0;
		}

		// Clear TX/RX FIFOs
		spi->reg->CTRL = (TXFIFORST | RXFIFORST);

		spi->info->status.busy = 0U;

		event |= AE350_SPI_EVENT_TRANSFER_COMPLETE;
	}

	// Clear interrupt status
	spi->reg->INTRST = status;
	// Make sure "write 1 clear" take effect before return
	spi->reg->INTRST;

	if ((spi->info->cb_event != NULL) && (event != 0))
	{
		spi->info->cb_event(event);
	}
}

// DMA TX event
#if ((DRV_SPI) && (DRV_SPI_DMA_TX_EN == 1))
static void spix_dma_tx_event (uint32_t event, SPI_RESOURCES * spi)
{
	switch (event)
	{
		case DMA_EVENT_TERMINAL_COUNT_REQUEST:
			spi->info->xfer.dma_tx_complete = 1;
			break;
		case DMA_EVENT_ERROR:
		default:
			break;
	}
}
#endif

// DMA RX event
#if ((DRV_SPI) && (DRV_SPI_DMA_RX_EN == 1))
static void spix_dma_rx_event (uint32_t event, SPI_RESOURCES * spi)
{
	switch (event)
	{
		case DMA_EVENT_TERMINAL_COUNT_REQUEST:
			if (spi->info->is_header == 0)
			{
				spi->info->xfer.dma_rx_complete = 1;
			}
			else
			{
				// Setting another DMA transfer to cover the dummy data from slave when master is sending header.
				// Configure DMA channel
				dma_channel_configure(spi->dma_rx->channel,
						       (uint32_t)(long)(&(spi->reg->DATA)),
						       (uint32_t)(long)spi->info->xfer.rx_buf,
						       spi->info->data_num,
						       DMA_CH_CTRL_SBINF(DMA_INF_IDX0) |
						       DMA_CH_CTRL_DBINF(DMA_INF_IDX0) |
						       DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
						       // DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
						       DMA_CH_CTRL_SWIDTH(spi->info->src_width/2) |
						       DMA_CH_CTRL_DWIDTH(spi->info->src_width/2) |
						       DMA_CH_CTRL_SMODE_HANDSHAKE |
						       DMA_CH_CTRL_SRCADDR_FIX |
						       DMA_CH_CTRL_DSTADDR_INC |
						       DMA_CH_CTRL_SRCREQ(spi->dma_rx->reqsel) |
						       DMA_CH_CTRL_INTABT |
						       DMA_CH_CTRL_INTERR |
						       DMA_CH_CTRL_INTTC |
						       DMA_CH_CTRL_ENABLE,
						       spi->dma_rx->cb_event);
				spi->info->is_header = 0;
			}

			break;

		case DMA_EVENT_ERROR:
			break;

		default:
			break;
	}
}
#endif


#if (DRV_SPI)

// Get version
static AE350_DRIVER_VERSION spi_get_version(void)
{
	return spi_driver_version;
}

// Get capabilities
static AE350_SPI_CAPABILITIES spi_get_capabilities(void)
{
	return spix_get_capabilities(&spi_resources);
}

// Initializes
static int32_t spi_initialize(AE350_SPI_SignalEvent_t cb_event)
{
	return spix_initialize(cb_event, &spi_resources);
}

// Uninitialized
static int32_t spi_uninitialize (void)
{
	return spix_uninitialize(&spi_resources);
}

// Power control
static int32_t spi_power_control(AE350_POWER_STATE state)
{
	return spix_power_control(state, &spi_resources);
}

// Send data
static int32_t spi_send(const void *data, uint32_t num)
{
	return spix_send(data, num, &spi_resources);
}

// Receive data
static int32_t spi_receive(void *data, uint32_t num)
{
	return spix_receive(data, num, &spi_resources);
}

// Transfer data
static int32_t spi_transfer(const void *data_out, void *data_in, uint32_t num)
{
	return spix_transfer(data_out, data_in, num, &spi_resources);
}

// Get data count
static uint32_t spi_get_data_count(void)
{
	return spix_get_data_count(&spi_resources);
}

// Control command
static int32_t spi_control(uint32_t control, uint32_t arg)
{
	return spix_control(control, arg, &spi_resources);
}

// Get status
static AE350_SPI_STATUS spi_get_status(void)
{
	return spix_get_status(&spi_resources);
}

// DMA TX event
#if (DRV_SPI_DMA_TX_EN == 1)
void spi_dma_tx_event (uint32_t event)
{
	spix_dma_tx_event(event, &spi_resources);
}
#endif

#if (DRV_SPI_DMA_RX_EN == 1)
void spi_dma_rx_event (uint32_t event)
{
	spix_dma_rx_event(event, &spi_resources);
}
#endif

// SPI interrupt handler
void spi_irq_handler(void)
{
	spix_irq_handler(&spi_resources);
}


// SPI driver control block
AE350_DRIVER_SPI Driver_SPI =
{
	spi_get_version,			// Get version
	spi_get_capabilities,		// Get capabilities
	spi_initialize,				// Initializes
	spi_uninitialize,			// Uninitialized
	spi_power_control,			// Power control
	spi_send,					// Send data
	spi_receive,				// Receive data
	spi_transfer,				// Transfer data
	spi_get_data_count,			// Get data count
	spi_control,				// Control
	spi_get_status				// Get status
};

#endif	// DRV_SPI
