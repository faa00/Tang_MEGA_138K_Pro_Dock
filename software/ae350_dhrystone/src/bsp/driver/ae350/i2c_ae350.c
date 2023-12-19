/*
 * ******************************************************************************************
 * File		: i2c_ae350.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: I2C driver definitions
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "i2c_ae350.h"


// Variables -- -----------------------------------------------------------------------------

#define AE350_I2C_DRV_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if ((!DRV_I2C))
	#error "I2C not enabled in config.h!"
#endif

// Driver version
static const AE350_DRIVER_VERSION DriverVersion =
{
	AE350_I2C_API_VERSION,
	AE350_I2C_DRV_VERSION
};

// Driver Capabilities
static const AE350_I2C_CAPABILITIES DriverCapabilities =
{
	1
};

#if (DRV_I2C)
// I2C Control Information
static I2C_INFO I2C_Info = {0};

#if (DRV_I2C_DMA_TX_EN == 1)
void i2c_dma_tx_event (uint32_t event);
static I2C_DMA i2c_dma_tx =
{
	DRV_I2C_DMA_TX_CH,
	DRV_I2C_DMA_TX_REQID,
	i2c_dma_tx_event
};
#endif

#if (DRV_I2C_DMA_RX_EN == 1)
void i2c_dma_rx_event (uint32_t event);
static I2C_DMA i2c_dma_rx =
{
	DRV_I2C_DMA_RX_CH,
	DRV_I2C_DMA_RX_REQID,
	i2c_dma_rx_event
};
#endif

// I2C Resources
static I2C_RESOURCES I2C_Resources =
{
	DEV_I2C,

#if (DRV_I2C_DMA_TX_EN == 1)
	&i2c_dma_tx,
#else
	NULL,
#endif

#if (DRV_I2C_DMA_RX_EN == 1)
	&i2c_dma_rx,
#else
	NULL,
#endif

	&I2C_Info
};
#endif


// Declarations --------------------------------------------------------------------------------
int32_t i2cx_control(uint32_t control, uint32_t arg0, uint32_t arg1, I2C_RESOURCES* i2c);
void i2cx_signalevent(uint32_t event, I2C_RESOURCES* i2c);
void i2cx_master_fifo_write(I2C_RESOURCES* i2c, uint8_t is_preceding);
void i2cx_slave_fifo_write(I2C_RESOURCES* i2c);
void i2cx_master_fifo_read(I2C_RESOURCES* i2c);
void i2cx_slave_fifo_read(I2C_RESOURCES* i2c, uint8_t is_fifo_full);


// Definitions ---------------------------------------------------------------------------------

// Initializes
int32_t i2cx_initialize(AE350_I2C_SignalEvent_t cb_event, I2C_RESOURCES* i2c)
{
	if (i2c->info->Driver_State & I2C_DRV_INIT)
	{
		return AE350_DRIVER_OK;
	}

	// Priority must be set > 0 to trigger the interrupt
	__nds__plic_set_priority(IRQ_I2C_SOURCE, 1);

	// Enable PLIC interrupt I2C source
	__nds__plic_enable_interrupt(IRQ_I2C_SOURCE);

	// Enable the Machine-External bit in MIE
	set_csr(NDS_MIE, MIP_MEIP);

	// Enable GIE
	set_csr(NDS_MSTATUS, MSTATUS_MIE);

	i2c->info->Sig_Evt_CB = cb_event;
	i2c->info->Pwr_State = AE350_POWER_FULL;
	i2c->info->Status.direction = 0;

	// DMA Initialize
	if (i2c->dma_tx || i2c->dma_rx)
	{
		dma_initialize();
	}

	i2c->info->Driver_State |= I2C_DRV_INIT;

#if APB_CLK_200MHZ
	// If APB clock = 200MHz (tpclk = 5ns), TPM = 1.
	i2c->reg->TPM = 1;
#elif APB_CLK_125MHZ
	// If APB clock = 125MHz (tpclk = 8ns), TPM = 1.
	i2c->reg->TPM = 1;
#elif APB_CLK_100MHZ
	// If APB clock = 100MHz (tpclk = 10ns), TPM = 0.
	i2c->reg->TPM = 0;
#elif APB_CLK_50MHZ
	// If APB clock = 50MHz (tpclk = 20ns), TPM = 0.
	i2c->reg->TPM = 0;
#endif

	return AE350_DRIVER_OK;
}

// Uninitialized
int32_t i2cx_uninitialize(I2C_RESOURCES* i2c)
{
	// DMA Uninitialized
	if (i2c->dma_tx || i2c->dma_rx)
	{
		dma_uninitialize();

		if (i2c->dma_tx && (i2c->info->Status.busy != 0))
		{
			dma_channel_disable(i2c->dma_tx->channel);
		}

		if (i2c->dma_rx && (i2c->info->Status.busy != 0))
		{
			dma_channel_disable(i2c->dma_rx->channel);
		}
	}

	// Disable and clear DMA IRQ
	__nds__plic_disable_interrupt(IRQ_I2C_SOURCE);

	i2c->info->Sig_Evt_CB = NULL;
	i2c->info->Pwr_State = AE350_POWER_OFF;

	// Clear and set driver state to none
	i2c->info->Driver_State = I2C_DRV_NONE;

	return AE350_DRIVER_OK;
}

// Power control
int32_t i2cx_power_control(AE350_POWER_STATE state, I2C_RESOURCES* i2c)
{
	uint32_t Tmp_C = 0, Tmp_S1 = 0;

	i2c->info->Pwr_State = state;

	switch (state)
	{
	case AE350_POWER_OFF:
		// DMA un-initialize
		if (i2c->dma_tx || i2c->dma_rx)
		{
			dma_uninitialize();

			if (i2c->dma_tx && (i2c->info->Status.busy != 0))
			{
				dma_channel_disable(i2c->dma_tx->channel);
			}

			if (i2c->dma_rx && (i2c->info->Status.busy != 0))
			{
				dma_channel_disable(i2c->dma_rx->channel);
			}
		}

		// Disable and clear DMA IRQ
		__nds__plic_disable_interrupt(IRQ_I2C_SOURCE);

		// I2C reset controller, including disable all I2C interrupts and clear FIFO
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_RESET_I2C);
		i2c->reg->CMD = Tmp_C;

		// I2C control disable
		Tmp_C = i2c->reg->SETUP;
		if (i2c->dma_tx || i2c->dma_rx)
		{
			Tmp_C &= (~ SETUP_DMA_EN);
		}
		Tmp_C &= (~ SETUP_I2C_EN);
		i2c->reg->SETUP = Tmp_C;

		i2c->info->Driver_State &= (~I2C_DRV_POWER);

		break;

	case AE350_POWER_LOW:
		break;

	case AE350_POWER_FULL:
		// I2C query FIFO depth
		// Read only FIFO size configured
		Tmp_S1 = i2c->reg->CFG;
		switch (Tmp_S1 & FIFO_SZ_MSK)
		{
		case FIFO_SZ_2:
		    i2c->info->FIFO_Depth = 2;
		    break;
		case FIFO_SZ_4:
		    i2c->info->FIFO_Depth = 4;
		    break;
		case FIFO_SZ_8:
		    i2c->info->FIFO_Depth = 8;
		    break;
		case FIFO_SZ_16:
		    i2c->info->FIFO_Depth = 16;
		    break;
		default:
			break;
		}

		// I2C reset controller, including disable all I2C interrupts and clear FIFO
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_RESET_I2C);
		i2c->reg->CMD = Tmp_C;

		// I2C setting: speed standard, slave mode(default)
		// FIFO(CPU) mode, 7-bit slave address, control enable
		i2c->reg->SETUP = 0x0;
		Tmp_C = i2c->reg->SETUP;
		if (i2c->dma_tx || i2c->dma_rx)
		{
			Tmp_C |= SETUP_DMA_EN;
		}
		Tmp_C |= ((SETUP_T_SUDAT_STD << 24) |
		          (SETUP_T_SP_STD  << 21) |
		          (SETUP_T_HDDAT_STD << 16) |
		          (SETUP_T_SCL_RATIO_STD << 13) |
		          (SETUP_T_SCLHI_STD << 4) |
		          SETUP_I2C_EN);
		i2c->reg->SETUP = Tmp_C;

		// I2C setting: enable completion interrupt and address hit interrupt
		Tmp_C = i2c->reg->INTEN;
		// DMA-slave mode still need complete interrupt
		Tmp_C |= (IEN_CMPL | IEN_ADDR_HIT);
		i2c->reg->INTEN = Tmp_C;

		// Clear status
		i2c->info->Status.busy = 0;
		// Define mode => 0:slave / 1:master
		i2c->info->Status.mode = 0;
		// Define direction => 0:non / 1:RX / 2:TX
		i2c->info->Status.direction = 0;
		i2c->info->Status.arbitration_lost = 0;
		i2c->info->Status.bus_error = 0;

		i2c->info->Driver_State = I2C_DRV_POWER;

		break;

	default:
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	return AE350_DRIVER_OK;
}

// Master mode know how many bytes to transmit
int32_t i2cx_master_transmit(uint32_t addr, const uint8_t* data, uint32_t num, bool xfer_pending, I2C_RESOURCES* i2c)
{
	uint32_t Tmp_C = 0;
	int32_t stat = 0;

	// Max 10-bit address(0x3FF), null data or number is no pay load for acknowledge polling
	// If no I2C pay load, set Phase_data=0x0
	if (addr > 0x3FF)
	{
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	if (i2c->info->Pwr_State != AE350_POWER_FULL)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	// Transfer operation in progress, or slave stalled
	if (i2c->info->Status.busy)
	{
		return AE350_DRIVER_ERROR_BUSY;
	}

	i2c->info->Status.busy = 1;
	// Define mode => 0:slave / 1:master
	i2c->info->Status.mode = 1;
	// Define direction => 0:non / 1:RX / 2:TX
	i2c->info->Status.direction = 2;
	i2c->info->Status.arbitration_lost = 0;
	i2c->info->Status.bus_error = 0;

	// Clear and set driver state to master TX before issue transaction
	i2c->info->Driver_State = I2C_DRV_MASTER_TX;

	// Step0
	// I2C reset controller, including disable all I2C interrupts and clear FIFO
	Tmp_C = i2c->reg->CMD;
	Tmp_C &= (~ CMD_MSK);
	Tmp_C |= (CMD_RESET_I2C);
	i2c->reg->CMD = Tmp_C;

	// Step1
	// I2C master, FIFO(CPU) mode, control enable
	Tmp_C = i2c->reg->SETUP;
	if (i2c->dma_tx)
	{
		Tmp_C |= SETUP_DMA_EN;
	}
	else
	{
		Tmp_C &= (~ SETUP_DMA_EN);
	}
	Tmp_C |= (SETUP_MASTER | SETUP_I2C_EN);
	i2c->reg->SETUP = Tmp_C;

	// I2C timing parameter is configured by middle ware w/ I2C_BUS_SPEED control code

	// Step2
	// I2C phase start enable, phase address enable, phase data enable, phase stop enable.
	// If I2C data transaction w/o I2C pay load, remember to clear data bit.
	// xfer_pending: Transfer operation is pending - Stop condition will not be generated.
	// The bus is busy when a START condition is on bus and it ends when a STOP condition is seen.
	// 10-bit slave address must set STOP bit.
	// I2C direction : master TX, set xfer data count.
	Tmp_C = i2c->reg->CTRL;
	Tmp_C &= (~ (CTRL_PHASE_STOP | CTRL_DIR | CTRL_DATA_COUNT));
	Tmp_C |= (CTRL_PHASE_START | CTRL_PHASE_ADDR | (!xfer_pending << 9) | (num & CTRL_DATA_COUNT));
	if (!num)
	{
		// Clear bit
		Tmp_C &= (~ CTRL_PHASE_DATA);
	}
	else
	{
		Tmp_C |= CTRL_PHASE_DATA;
	}
	i2c->reg->CTRL = Tmp_C;

	// Step3
	i2c->info->Slave_Addr = addr;
	i2c->info->Xfered_Data_Wt_Ptr = 0;
	i2c->info->Xfer_Wt_Num = num;
	i2c->info->Xfer_Cmpl_Count = 0;
	i2c->info->middleware_tx_buf = (uint8_t*)data;

	// I2C slave address, general call address = 0x0(7-bit or 10-bit)
	Tmp_C = i2c->reg->ADDR;
	Tmp_C &= (~ SLAVE_ADDR_MSK);
	Tmp_C |= (i2c->info->Slave_Addr & (SLAVE_ADDR_MSK));
	i2c->reg->ADDR = Tmp_C;

	// Step4
	// I2C Enable the Completion Interrupt, Enable/Disable the FIFO Empty Interrupt
	// I2C Enable the Arbitration Lose Interrupt, master mode only
	Tmp_C = i2c->reg->INTEN;
	if (i2c->dma_tx)
	{
		// DMA mode still need complete interrupt for status.busy timing,
		// but no need FIFO empty interrupt
		Tmp_C |= (IEN_CMPL | IEN_ARB_LOSE);
	}
	else
	{
		// I2C write a patch of data(FIFO_Depth) to FIFO,
		// it will be consumed empty if data is actually issued on I2C bus,
		// currently FIFO is not empty, will not trigger FIFO_EMPTY interrupt
		i2cx_master_fifo_write(i2c, 1);

		Tmp_C |= (IEN_CMPL | IEN_ARB_LOSE);
		if (num > 0)
		{
			// Enable
			Tmp_C |= IEN_FIFO_EMPTY;
		}
		else
		{
			// Disable
			Tmp_C &= (~ IEN_FIFO_EMPTY);
		}
	}
	i2c->reg->INTEN = Tmp_C;

	// Step5
	// I2C Write 0x1 to the Command register to issue the transaction
	Tmp_C = i2c->reg->CMD;
	Tmp_C &= (~ CMD_MSK);
	Tmp_C |= (CMD_ISSUE_TRANSACTION);
	i2c->reg->CMD = Tmp_C;

	// DMA mode
	if (i2c->dma_tx)
	{
		// Configure DMA channel
		stat = dma_channel_configure(i2c->dma_tx->channel,
		                             (uint32_t)(long)(&i2c->info->middleware_tx_buf[0]),
		                             (uint32_t)(long)(&(i2c->reg->DATA)),
		                             num,
		                             DMA_CH_CTRL_SBINF(DMA_INF_IDX1) |
		                             DMA_CH_CTRL_DBINF(DMA_INF_IDX1) |
		                             DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
 		                             DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE) |
 		                             DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE) |
		                             DMA_CH_CTRL_DMODE_HANDSHAKE |
		                             DMA_CH_CTRL_SRCADDR_INC |
		                             DMA_CH_CTRL_DSTADDR_FIX |
		                             DMA_CH_CTRL_DSTREQ(i2c->dma_tx->reqsel) |
		                             DMA_CH_CTRL_INTABT |
		                             DMA_CH_CTRL_INTERR |
		                             DMA_CH_CTRL_INTTC |
		                             DMA_CH_CTRL_ENABLE,
		                             i2c->dma_tx->cb_event);

		if (stat == -1)
		{
			return AE350_DRIVER_ERROR;
		}
	}

	// Master transmit no blocking operation, blocking operation is owned by middle ware

	return AE350_DRIVER_OK;
}

// Master mode know how many bytes to receive
int32_t i2cx_master_receive(uint32_t addr, uint8_t* data, uint32_t num, bool xfer_pending, I2C_RESOURCES* i2c)
{
	uint32_t Tmp_C = 0;
	int32_t stat = 0;

	// Max 10-bit address(0x3FF), null data or number is no pay load for acknowledge polling
	// If no I2C pay load, set Phase_data=0x0
	if (addr > 0x3FF)
	{
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	if (i2c->info->Pwr_State != AE350_POWER_FULL)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	// Transfer operation in progress, or slave stalled
	if (i2c->info->Status.busy)
	{
		return AE350_DRIVER_ERROR_BUSY;
	}

	i2c->info->Status.busy = 1;
	// Define mode => 0:slave / 1:master
	i2c->info->Status.mode = 1;
	// Define direction => 0:non / 1:RX / 2:TX
	i2c->info->Status.direction = 1;
	i2c->info->Status.arbitration_lost = 0;
	i2c->info->Status.bus_error = 0;

	// Clear and set driver state to master RX before issue transaction
	i2c->info->Driver_State = I2C_DRV_MASTER_RX;

	// Step0
	// I2C reset controller, including disable all I2C interrupts and clear FIFO
	Tmp_C = i2c->reg->CMD;
	Tmp_C &= (~ CMD_MSK);
	Tmp_C |= (CMD_RESET_I2C);
	i2c->reg->CMD = Tmp_C;

	// Step1
	// I2C master, FIFO(CPU) mode, control enable
	Tmp_C = i2c->reg->SETUP;
	if (i2c->dma_rx)
	{
		Tmp_C |= SETUP_DMA_EN;
	}
	else
	{
		Tmp_C &= (~ SETUP_DMA_EN);
	}
	Tmp_C |= (SETUP_MASTER | SETUP_I2C_EN);
	i2c->reg->SETUP = Tmp_C;

	// I2C timing parameter is configured by middle ware w/ I2C_BUS_SPEED control code

	// Step2
	// I2C phase start enable, phase address enable, phase data enable, phase stop enable.
	// If I2C data transaction w/o I2C pay load, remember to clear data bit.
	// xfer_pending: Transfer operation is pending - Stop condition will not be generated.
	// The bus is busy when a START condition is on bus and it ends when a STOP condition is seen.
	// 10-bit slave address must set STOP bit.
	// I2C direction : master RX, set xfer data count.
	Tmp_C = i2c->reg->CTRL;
	Tmp_C &= (~ (CTRL_PHASE_STOP | CTRL_DATA_COUNT));
	Tmp_C |= (CTRL_PHASE_START | CTRL_PHASE_ADDR | (!xfer_pending << 9) | (num & CTRL_DATA_COUNT) | CTRL_DIR);
	if (!num)
	{
		// Clear bit
		Tmp_C &= (~ CTRL_PHASE_DATA);
	}
	else
	{
		Tmp_C |= CTRL_PHASE_DATA;
	}
	i2c->reg->CTRL = Tmp_C;

	// Step3
	i2c->info->Slave_Addr = addr;
	i2c->info->Xfered_Data_Rd_Ptr = 0;
	i2c->info->Xfer_Rd_Num = num;
	i2c->info->Xfer_Cmpl_Count = 0;
	i2c->info->middleware_rx_buf = data;

	// I2C slave address, general call address = 0x0(7-bit or 10-bit)
	Tmp_C = i2c->reg->ADDR;
	Tmp_C &= (~ SLAVE_ADDR_MSK);
	Tmp_C |= (i2c->info->Slave_Addr & (SLAVE_ADDR_MSK));
	i2c->reg->ADDR = Tmp_C;

	// Step4
	// I2C Enable the Completion Interrupt, Enable the FIFO Full Interrupt
	// I2C Enable the Arbitration Lose Interrupt, master mode only
	Tmp_C = i2c->reg->INTEN;
	if (i2c->dma_rx)
	{
		// DMA mode still need complete interrupt for status.busy timing
		// but no need FIFO empty interrupt
		Tmp_C |= (IEN_CMPL | IEN_ARB_LOSE);
	}
	else
	{
		Tmp_C |= (IEN_CMPL | IEN_FIFO_FULL | IEN_ARB_LOSE);
	}
	i2c->reg->INTEN = Tmp_C;

	// Step5
	// I2C Write 0x1 to the Command register to issue the transaction
	Tmp_C = i2c->reg->CMD;
	Tmp_C &= (~ CMD_MSK);
	Tmp_C |= (CMD_ISSUE_TRANSACTION);
	i2c->reg->CMD = Tmp_C;

	// DMA mode
	if (i2c->dma_rx)
	{
		// Configure DMA channel
		stat = dma_channel_configure(i2c->dma_rx->channel,
		                             (uint32_t)(long)(&(i2c->reg->DATA)),
		                             (uint32_t)(long)(&i2c->info->middleware_rx_buf[0]),
		                             num,
		                             DMA_CH_CTRL_SBINF(DMA_INF_IDX1) |
		                             DMA_CH_CTRL_DBINF(DMA_INF_IDX1) |
		                             DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
		                             DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE) |
		                             DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE) |
		                             DMA_CH_CTRL_SMODE_HANDSHAKE |
		                             DMA_CH_CTRL_SRCADDR_FIX |
		                             DMA_CH_CTRL_DSTADDR_INC |
		                             DMA_CH_CTRL_SRCREQ(i2c->dma_rx->reqsel) |
		                             DMA_CH_CTRL_INTABT |
		                             DMA_CH_CTRL_INTERR |
		                             DMA_CH_CTRL_INTTC |
		                             DMA_CH_CTRL_ENABLE,
		                             i2c->dma_rx->cb_event);

		if (stat == -1)
		{
			return AE350_DRIVER_ERROR;
		}
	}

	// Master RX no blocking operation, blocking operation is owned by middle ware

	return AE350_DRIVER_OK;
}

// Slave mode unknown how many bytes to TX, so TX 1 byte each time until complete or nack_assert
int32_t i2cx_slave_transmit(const uint8_t* data, uint32_t num, I2C_RESOURCES* i2c)
{
	if (i2c->info->Pwr_State != AE350_POWER_FULL)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	// Transfer operation in progress, or master stalled
	if (i2c->info->Status.busy)
	{
		return AE350_DRIVER_ERROR_BUSY;
	}

	i2c->info->Status.busy = 1;
	// Define mode => 0:slave / 1:master
	i2c->info->Status.mode = 0;
	i2c->info->Status.bus_error = 0;

	// I2C xfer data count
	// If DMA is not enabled, DataCnt is the number of
	// bytes transmitted/received from the bus master.
	// It is reset to 0 when the controller is addressed
	// and then increased by one for each byte of data
	// transmitted/received

	// No I2C reset controller, no I2C clear FIFO
	// w/ minimal change I2C HW setting

	i2c->info->Xfered_Data_Wt_Ptr = 0;
	i2c->info->Xfer_Wt_Num = num;
	i2c->info->Xfer_Cmpl_Count = 0;
	i2c->info->middleware_tx_buf = (uint8_t*)data;

	i2cx_slave_fifo_write(i2c);

	return AE350_DRIVER_OK;
}

// Slave mode unknown how many bytes to RX, so RX FIFO-full byte each time until complete
int32_t i2cx_slave_receive(uint8_t* data, uint32_t num, I2C_RESOURCES* i2c)
{
	if (i2c->info->Pwr_State != AE350_POWER_FULL)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	// Since middle ware just read data from Xfer_Data_Rd_Buf,
	// no set busy flag, driver slave RX is independent of
	// middle ware slave RX, if set busy flag will affect
	// slave TX behavior
	// Define mode => 0:slave / 1:master
	i2c->info->Status.mode = 0;
	i2c->info->Status.bus_error = 0;

	// I2C xfer data count
	// If DMA is not enabled, DataCnt is the number of
	// bytes transmitted/received from the bus master.
	// It is reset to 0 when the controller is addressed
	// and then increased by one for each byte of data
	// transmitted/received

	// no I2C reset controller, no I2C clear FIFO since middle ware just read data from Xfer_Data_Rd_Buf
	// w/ minimal change I2C HW setting

	// Xfer_Data_Rd_Buf already read the data from hardware FIFO and keep,
	// currently middle ware able to take from the buffer
	// error hit
	if (num > MAX_XFER_SZ)
	{
		return AE350_DRIVER_ERROR;
	}
	MEMCPY(data, &i2c->info->Xfer_Data_Rd_Buf[i2c->info->middleware_rx_ptr], num);

	i2c->info->Xfer_Rd_Num = num;
	i2c->info->Xfer_Cmpl_Count = 0;
	i2c->info->middleware_rx_ptr += num;

	// Slave RX no blocking operation, blocking operation is owned by middle ware

	return AE350_DRIVER_OK;
}

// Get data count
uint32_t i2cx_get_datacount(I2C_RESOURCES* i2c)
{
	return (i2c->info->Xfer_Cmpl_Count);
}

// Control
int32_t i2cx_control(uint32_t control, uint32_t arg0, uint32_t arg1, I2C_RESOURCES* i2c)
{
	uint32_t Tmp_C = 0;
	int32_t status = AE350_DRIVER_OK;

	if (i2c->info->Pwr_State != AE350_POWER_FULL)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	switch (control)
	{
	// Middle ware use control code
	case AE350_I2C_OWN_ADDRESS:
		// Set slave addressing
		Tmp_C = i2c->reg->SETUP;
		if (arg0 & AE350_I2C_ADDRESS_10BIT)
		{
			// I2C 10-bit slave address
			Tmp_C |= SETUP_ADDRESSING;
		}
		else
		{
			// I2C 7-bit slave address
			Tmp_C &= (~ SETUP_ADDRESSING);
		}
		i2c->reg->SETUP = Tmp_C;

		// I2C slave address, general call address = 0x0(7-bit or 10-bit)
		Tmp_C = i2c->reg->ADDR;
		Tmp_C &= (~ SLAVE_ADDR_MSK);
		Tmp_C |= (arg0 & (SLAVE_ADDR_MSK));
		i2c->reg->ADDR = Tmp_C;

		break;

	case AE350_I2C_BUS_SPEED:
		Tmp_C = i2c->reg->SETUP;
		// Clear previous setting
   		Tmp_C &= (~ (SETUP_T_SUDAT | SETUP_T_SP | SETUP_T_HDDAT | SETUP_T_SCL_RATIO | SETUP_T_SCLHI));

		switch (arg0)
		{
		case AE350_I2C_BUS_SPEED_STANDARD:
			// I2C speed standard
			// Apply current setting
			Tmp_C |= ((SETUP_T_SUDAT_STD << 24) |
			(SETUP_T_SP_STD  << 21) |
			(SETUP_T_HDDAT_STD << 16) |
			(SETUP_T_SCL_RATIO_STD << 13) |
			(SETUP_T_SCLHI_STD << 4));
			break;
		case AE350_I2C_BUS_SPEED_FAST:
			// I2C speed fast
			// Apply current setting
			Tmp_C |= ((SETUP_T_SUDAT_FAST << 24) |
			(SETUP_T_SP_FAST  << 21) |
			(SETUP_T_HDDAT_FAST << 16) |
			(SETUP_T_SCL_RATIO_FAST << 13) |
			(SETUP_T_SCLHI_FAST << 4));
			break;
		case AE350_I2C_BUS_SPEED_FAST_PLUS:
			// I2C speed fast plus
			// Apply current setting
			Tmp_C |= ((SETUP_T_SUDAT_FAST_P << 24) |
			(SETUP_T_SP_FAST_P  << 21) |
			(SETUP_T_HDDAT_FAST_P << 16) |
			(SETUP_T_SCL_RATIO_FAST_P << 13) |
			(SETUP_T_SCLHI_FAST_P << 4));
			break;
		default:
			return AE350_DRIVER_ERROR_UNSUPPORTED;
		}

		// Apply
		i2c->reg->SETUP = Tmp_C;

		break;

	case AE350_I2C_BUS_CLEAR:
		// I2C reset controller, including disable all I2C interrupts and clear FIFO
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_RESET_I2C);
		i2c->reg->CMD = Tmp_C;

		break;

	case AE350_I2C_ABORT_TRANSFER:
		// I2C reset controller, including disable all I2C interrupts and clear FIFO
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_RESET_I2C);
		i2c->reg->CMD = Tmp_C;

		break;

	default:
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	i2c->info->Driver_State |= I2C_DRV_CFG_PARAM;

	return status;
}

// Get status
AE350_I2C_STATUS i2cx_getstatus(I2C_RESOURCES* i2c)
{
	return i2c->info->Status;
}

void i2cx_signalevent(uint32_t event, I2C_RESOURCES* i2c)
{
	// Invoke callback function
	if (i2c->info->Sig_Evt_CB)
	{
		i2c->info->Sig_Evt_CB(event);
	}
}

uint32_t check = 0;

void i2c_cmpl_handler(I2C_RESOURCES* i2c)
{
	uint32_t Tmp_S = 0, Tmp_S1 = 0, Tmp_C = 0;

	Tmp_S = i2c->reg->SETUP;

	// Master mode
	if (Tmp_S & SETUP_MASTER)
	{
		// I2C disable all Interrupts in the Interrupt Enable Register
		Tmp_C = i2c->reg->INTEN;
		Tmp_C &= (~ IEN_ALL);
		i2c->reg->INTEN = Tmp_C;
	}
	else
	{
		// I2C no disable all Interrupts in the Interrupt Enable Register,
		// keep previous setting for slave TX
	}

	// Check the DataCnt field of the Control Register
	// to know if all data are successfully transmitted.
	// -> Master: The number of bytes to transmit/receive.
	// 0 means 256 bytes. DataCnt will be decreased by one
	// for each byte transmitted/received.
	if ((i2c->info->Driver_State & I2C_DRV_MASTER_TX) || (i2c->info->Driver_State & I2C_DRV_MASTER_RX))
	{
		Tmp_S = i2c->reg->CTRL;

		if (i2c->info->Driver_State & I2C_DRV_MASTER_TX)
		{
			if (i2c->dma_tx)
			{
				i2c->info->Xfered_Data_Wt_Ptr = dma_channel_get_count(DRV_I2C_DMA_TX_CH);
				i2c->info->Xfer_Cmpl_Count = dma_channel_get_count(DRV_I2C_DMA_TX_CH);
			}
			else
			{
				i2c->info->Xfer_Cmpl_Count = i2c->info->Xfer_Wt_Num - (Tmp_S & CTRL_DATA_COUNT);
			}

			// Clear and set driver state to master TX complete
			i2c->info->Driver_State = I2C_DRV_MASTER_TX_CMPL;

			// Clear busy bit on i2c complete event as master DMA/CPU TX
			i2c->info->Status.busy = 0;
		}

		if (i2c->info->Driver_State & I2C_DRV_MASTER_RX)
		{
			if (i2c->dma_rx)
			{
				i2c->info->Xfered_Data_Rd_Ptr = dma_channel_get_count(DRV_I2C_DMA_RX_CH);
				i2c->info->Xfer_Cmpl_Count = dma_channel_get_count(DRV_I2C_DMA_RX_CH);

				// Clear and set driver state to master RX complete
				i2c->info->Driver_State = I2C_DRV_MASTER_RX_CMPL;
			}
			else
			{
				i2cx_master_fifo_read(i2c);

				i2c->info->Xfer_Cmpl_Count = i2c->info->Xfer_Rd_Num - (Tmp_S & CTRL_DATA_COUNT);

				// Clear and set driver state to master RX complete
				i2c->info->Driver_State = I2C_DRV_MASTER_RX_CMPL;

				// Clear busy bit on i2c complete event as master CPU RX
				i2c->info->Status.busy = 0;
			}
		}
	}

	// Check the DataCnt field of the Control Register
	// to know if all data are successfully transmitted.
	// -> Slave: the meaning of DataCnt depends on the
	// DMA mode:
	// If DMA is not enabled, DataCnt is the number of
	// bytes transmitted/received from the bus master.
	// It is reset to 0 when the controller is addressed
	// and then increased by one for each byte of data
	// transmitted/received.
	// If DMA is enabled, DataCnt is the number of
	// bytes to transmit/receive. It will not be reset to 0
	// when the slave is addressed and it will be
	// decreased by one for each byte of data transmitted/received.
	if ((i2c->info->Driver_State & I2C_DRV_SLAVE_TX) || (i2c->info->Driver_State & I2C_DRV_SLAVE_RX))
	{
		Tmp_S1 = i2c->reg->SETUP;
		Tmp_S = i2c->reg->CTRL;

		// I2C_FIFO mode
		if (!(Tmp_S1 & SETUP_DMA_EN))
		{
			if (i2c->info->Driver_State & I2C_DRV_SLAVE_TX)
			{
				// I2C Disable the Byte Transmit Interrupt in the Interrupt Enable Register
				Tmp_C = i2c->reg->INTEN;
				Tmp_C &= (~ IEN_BYTE_TRANS);
				i2c->reg->INTEN = Tmp_C;

				// Clear and set driver state to slave TX complete
				i2c->info->Driver_State = I2C_DRV_SLAVE_TX_CMPL;
			}

			if (i2c->info->Driver_State & I2C_DRV_SLAVE_RX)
			{
				i2cx_slave_fifo_read(i2c, 0);

				// I2C Disable the FIFO Full Interrupt in the Interrupt Enable Register
				Tmp_C = i2c->reg->INTEN;
				Tmp_C &= (~ IEN_FIFO_FULL);
				i2c->reg->INTEN = Tmp_C;

				// Key point for middle ware to query
				i2c->info->Xfer_Cmpl_Count = i2c->info->Xfered_Data_Rd_Ptr - i2c->info->middleware_rx_ptr;

				// Clear and set driver state to slave RX complete
				i2c->info->Driver_State = I2C_DRV_SLAVE_RX_CMPL;
			}

			// Clear busy bit on i2c complete event as slave CPU TX/RX
			i2c->info->Status.busy = 0;
		}
		// I2C_DMA mode
		else
		{
			if (i2c->info->Driver_State & I2C_DRV_SLAVE_TX)
			{
				// I2C Disable the Byte Transmit Interrupt in the Interrupt Enable Register
				Tmp_C = i2c->reg->INTEN;
				Tmp_C &= (~ IEN_BYTE_TRANS);
				i2c->reg->INTEN = Tmp_C;

		        // Key point for middle ware to query
				i2c->info->Xfered_Data_Wt_Ptr = dma_channel_get_count(DRV_I2C_DMA_TX_CH);
				i2c->info->Xfer_Cmpl_Count = dma_channel_get_count(DRV_I2C_DMA_TX_CH);

				// Clear and set driver state to slave TX complete
				i2c->info->Driver_State = I2C_DRV_SLAVE_TX_CMPL;

				// Clear busy bit on i2c complete event as slave DMA TX
				i2c->info->Status.busy = 0;
			}

			if (i2c->info->Driver_State & I2C_DRV_SLAVE_RX)
			{
				// Key point for middle ware to query
				i2c->info->Xfer_Cmpl_Count = dma_channel_get_count(DRV_I2C_DMA_RX_CH);

				// Abort DMA channel since MAX_XFER_SZ-read and expect complete in cmpl_handler
				dma_channel_abort(DRV_I2C_DMA_RX_CH);

				check = (Tmp_S & CTRL_DATA_COUNT);
				// Clear and set driver state to slave RX complete
				i2c->info->Driver_State = I2C_DRV_SLAVE_RX_CMPL;

				// Clear busy bit on i2c complete event as slave DMA RX since read MAX_XFER_SZ
				i2c->info->Status.busy = 0;
			}
		}

		// I2C clear FIFO first to prevent mistake i2cx_slave_fifo_read()
		// If the Completion Interrupt asserts, clear the FIFO and go next transaction.
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_CLEAR_FIFO);
		i2c->reg->CMD = Tmp_C;
	}

	i2c->info->Status.direction = 0;
}

// Basic FIFO write function
void i2cx_master_fifo_write(I2C_RESOURCES* i2c, uint8_t is_preceding)
{
	uint32_t i = 0, write_fifo_count = 0, Tmp_C = 0;

	write_fifo_count = ((i2c->info->Xfer_Wt_Num - i2c->info->Xfered_Data_Wt_Ptr) >= i2c->info->FIFO_Depth) ?
				i2c->info->FIFO_Depth : (i2c->info->Xfer_Wt_Num - i2c->info->Xfered_Data_Wt_Ptr);

	if (is_preceding)
	{
		write_fifo_count = 2;
	}

	// I2C write a patch of data(FIFO_Depth) to FIFO,
	// it will be consumed empty if data is actually issued on I2C bus
	for (i = 0; i < write_fifo_count; i++)
	{
		// I2C write data to FIFO through data port register
		i2c->reg->DATA = (i2c->info->middleware_tx_buf[i2c->info->Xfered_Data_Wt_Ptr] & (DATA_MSK));

		i2c->info->Xfered_Data_Wt_Ptr++;

		// If all data are pushed into the FIFO, disable the FIFO Empty Interrupt. Otherwise, repeat
		if (i2c->info->Xfered_Data_Wt_Ptr == i2c->info->Xfer_Wt_Num)
		{
			// I2C disable the FIFO Empty Interrupt in the Interrupt Enable Register
			Tmp_C = i2c->reg->INTEN;
			Tmp_C &= (~ IEN_FIFO_EMPTY);
			i2c->reg->INTEN = Tmp_C;
		}
	}
}

// Basic FIFO write function
void i2cx_slave_fifo_write(I2C_RESOURCES* i2c)
{
	uint32_t i = 0, write_fifo_count = 0, Tmp_C = 0;
	int32_t stat = 0;

	// Slave TX 1 byte each time, since no information got
	// about how many bytes of master RX should be,
	// check nack_assert to complete slave TX
	write_fifo_count = 1;

	// DMA mode
	if (i2c->dma_tx)
	{
		// If DMA is enabled, DataCnt is the number of
		// bytes to transmit/receive. It will not be reset to 0
		// when the slave is addressed and it will be
		// decreased by one for each byte of data transmitted/received.
		Tmp_C = i2c->reg->CTRL;
		Tmp_C &= (~ CTRL_DATA_COUNT);
		Tmp_C |= (write_fifo_count & CTRL_DATA_COUNT);
		i2c->reg->CTRL = Tmp_C;

		// Configure DMA channel
		stat = dma_channel_configure(i2c->dma_tx->channel,
		                             (uint32_t)(long)(&i2c->info->middleware_tx_buf[0]),
		                             (uint32_t)(long)(&(i2c->reg->DATA)),
		                             write_fifo_count,
		                             DMA_CH_CTRL_SBINF(DMA_INF_IDX1) |
		                             DMA_CH_CTRL_DBINF(DMA_INF_IDX1) |
		                             DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
		                             DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE) |
		                             DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE) |
		                             DMA_CH_CTRL_DMODE_HANDSHAKE |
		                             DMA_CH_CTRL_SRCADDR_INC |
		                             DMA_CH_CTRL_DSTADDR_FIX |
		                             DMA_CH_CTRL_DSTREQ(i2c->dma_tx->reqsel) |
		                             DMA_CH_CTRL_INTABT |
		                             DMA_CH_CTRL_INTERR |
		                             DMA_CH_CTRL_INTTC |
		                             DMA_CH_CTRL_ENABLE,
		                             i2c->dma_tx->cb_event);

		if (stat == -1)
		{
			return;
		}
	}
	else
	{
		// I2C write a patch of data(FIFO_Depth) to FIFO,
		// it will be consumed empty if data is actually issued on I2C bus
		for (i = 0; i < write_fifo_count; i++)
		{
			// I2C write data to FIFO through data port register
			i2c->reg->DATA = (i2c->info->middleware_tx_buf[i2c->info->Xfered_Data_Wt_Ptr] & (DATA_MSK));

			i2c->info->Xfered_Data_Wt_Ptr++;
			i2c->info->Xfer_Cmpl_Count++;
		}
	}
}

// Handle I2C FIFO empty
void i2cx_fifo_empty_handler(I2C_RESOURCES* i2c)
{
	if (i2c->info->Pwr_State != AE350_POWER_FULL)
	{
		return;
	}

	if (i2c->info->Driver_State & I2C_DRV_MASTER_TX)
	{
		i2cx_master_fifo_write(i2c, 0);
	}
	else if (i2c->info->Driver_State & I2C_DRV_SLAVE_TX)
	{
		// i2cx_slave_fifo_write(prI2C_Info);
	}
}

// Basic FIFO read function
void i2cx_master_fifo_read(I2C_RESOURCES* i2c)
{
	uint32_t i = 0, read_fifo_count = 0, Tmp_C = 0;

	read_fifo_count = ((i2c->info->Xfer_Rd_Num - i2c->info->Xfered_Data_Rd_Ptr) >= i2c->info->FIFO_Depth) ?
				i2c->info->FIFO_Depth : (i2c->info->Xfer_Rd_Num - i2c->info->Xfered_Data_Rd_Ptr);

	// I2C read a patch of data(FIFO_Depth) from FIFO,
	// it will be consumed empty if data is actually read out by driver
	for (i = 0; i < read_fifo_count; i++)
	{
		// I2C read data from FIFO through data port register
		i2c->info->middleware_rx_buf[i2c->info->Xfered_Data_Rd_Ptr] = (i2c->reg->DATA & (DATA_MSK));

		i2c->info->Xfered_Data_Rd_Ptr++;

		// If all data are read from the FIFO, disable the FIFO Full Interrupt. Otherwise, repeat
		if (i2c->info->Xfered_Data_Rd_Ptr == i2c->info->Xfer_Rd_Num)
		{
			// I2C disable the FIFO Full Interrupt in the Interrupt Enable Register
			Tmp_C = i2c->reg->INTEN;
			Tmp_C &= (~ IEN_FIFO_FULL);
			i2c->reg->INTEN = Tmp_C;
		}
	}
}

// Basic FIFO read function
void i2cx_slave_fifo_read(I2C_RESOURCES* i2c, uint8_t is_fifo_full)
{
	uint32_t i = 0, read_fifo_count = 0, curr_rx_data_count = 0;

	// Slave RX data count is accumulated and depend on
	// master TX data count of one transaction(start-address-data-stop),
	// possible larger than FIFO length(4 bytes)
	// slave: If DMA is not enabled(FIFO mode), DataCnt is the number of
	// bytes transmitted/received from the bus master.
	// It is reset to 0 when the controller is addressed
	// and then increased by one for each byte of data transmitted/received
	curr_rx_data_count = (i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val & CTRL_DATA_COUNT);

	// Error hit
	if (curr_rx_data_count > MAX_XFER_SZ)
	{
		while(1);
	}

	if (is_fifo_full)
	{
		read_fifo_count = i2c->info->FIFO_Depth;
		// Just only read FIFO in complete interrupt handler if FIFO is not full
	}
	else
	{
		read_fifo_count = curr_rx_data_count - i2c->info->last_rx_data_count;
	}

	// Error hit
	if (read_fifo_count > MAX_XFER_SZ)
	{
		while(1);
	}

	// I2C read a patch of data(FIFO_Depth) from FIFO,
	// it will be consumed empty if data is actually read out by driver
	for (i = 0; i < read_fifo_count; i++)
	{
		// I2C read data from FIFO through data port register
		i2c->info->Xfer_Data_Rd_Buf[i2c->info->Xfered_Data_Rd_Ptr] = (i2c->reg->DATA & (DATA_MSK));

		i2c->info->Xfered_Data_Rd_Ptr++;

		if (i2c->info->Xfered_Data_Rd_Ptr == MAX_XFER_SZ)
		{
			// Slave RX buffer overwrite
			i2c->info->Xfered_Data_Rd_Ptr_Ow++;
			i2c->info->Xfered_Data_Rd_Ptr = 0;
		}
	}

	i2c->info->last_rx_data_count = curr_rx_data_count;
}

void i2cx_fifo_full_handler(I2C_RESOURCES* i2c)
{
	if (i2c->info->Pwr_State != AE350_POWER_FULL)
	{
		return;
	}

	if (i2c->info->Driver_State & I2C_DRV_MASTER_RX)
	{
		i2cx_master_fifo_read(i2c);
	}
	else if (i2c->info->Driver_State & I2C_DRV_SLAVE_RX)
	{
		i2cx_slave_fifo_read(i2c, 1);
	}
}

void i2cx_slave_addr_hit_handler(I2C_RESOURCES* i2c)
{
	uint32_t Tmp_C = 0;
	int32_t stat = 0;

	if (i2c->info->Pwr_State != AE350_POWER_FULL)
	{
		return;
	}

	// Slave mode RX: if address hit, FIFO may not be full state
	if (i2c->info->Driver_State & I2C_DRV_SLAVE_RX)
	{
		// A new I2C data transaction(start-address-data-stop)
		i2c->info->Xfered_Data_Rd_Ptr_Ow = 0;
		i2c->info->Xfered_Data_Rd_Ptr = 0;
		// Key point to reset
		i2c->info->middleware_rx_ptr = 0;
		MEMSET(i2c->info->Xfer_Data_Rd_Buf, 0, sizeof(i2c->info->Xfer_Data_Rd_Buf));

		if (i2c->dma_rx)
		{
			i2c->info->last_rx_data_count = (i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val & CTRL_DATA_COUNT);

			// If DMA is enabled, DataCnt is the number of
			// bytes to transmit/receive. It will not be reset to 0
			// when the slave is addressed and it will be
			// decreased by one for each byte of data transmitted/received.
			Tmp_C = i2c->reg->CTRL;
			Tmp_C &= (~ CTRL_DATA_COUNT);
			Tmp_C |= (MAX_XFER_SZ & CTRL_DATA_COUNT);
			i2c->reg->CTRL = Tmp_C;

			// Configure DMA channel w/ MAX_XFER_SZ-read and expect complete in cmpl_handler
			stat = dma_channel_configure(i2c->dma_rx->channel,
			                             (uint32_t)(long)(&(i2c->reg->DATA)),
			                             (uint32_t)(long)(&i2c->info->Xfer_Data_Rd_Buf[i2c->info->Xfered_Data_Rd_Ptr]),
			                             MAX_XFER_SZ,
			                             DMA_CH_CTRL_SBINF(DMA_INF_IDX1) |
			                             DMA_CH_CTRL_DBINF(DMA_INF_IDX1) |
			                             DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
			                             DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE) |
			                             DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE) |
			                             DMA_CH_CTRL_SMODE_HANDSHAKE |
			                             DMA_CH_CTRL_SRCADDR_FIX |
			                             DMA_CH_CTRL_DSTADDR_INC |
			                             DMA_CH_CTRL_SRCREQ(i2c->dma_rx->reqsel) |
			                             DMA_CH_CTRL_INTABT |
			                             DMA_CH_CTRL_INTERR |
			                             DMA_CH_CTRL_INTTC |
			                             DMA_CH_CTRL_ENABLE,
			                             i2c->dma_rx->cb_event);

			if (stat == -1)
			{
				return;
			}

		}
		else
		{
			i2c->info->last_rx_data_count = 0;

			// I2C Enable the FIFO Full Interrupt in the Interrupt Enable Register
			Tmp_C = i2c->reg->INTEN;
			Tmp_C |= IEN_FIFO_FULL;
			i2c->reg->INTEN = Tmp_C;
		}
	}
	// Slave mode TX: if address hit, FIFO may not be empty state
	else if (i2c->info->Driver_State & I2C_DRV_SLAVE_TX)
	{
		// Master will generate nack_assert to notify slave TX complete
		i2c->info->nack_assert = 0;

		// I2C Enable the Byte Transmit Interrupt in the Interrupt Enable Register
		// for status.busy flag support
		Tmp_C = i2c->reg->INTEN;
		Tmp_C |= IEN_BYTE_TRANS;
		i2c->reg->INTEN = Tmp_C;
	}
}

// Interrupt handler
void i2cx_irq_handler(I2C_RESOURCES* i2c)
{
	uint32_t Tmp_S = 0;
	uint32_t Tmp_S1 = 0;
	uint32_t Tmp_W = 0;
	uint32_t Tmp_C = 0;
	uint32_t Evt_Id = 0;

	Tmp_W = Tmp_S = i2c->reg->STATUS;
	Tmp_S1 = i2c->reg->SETUP;
	Tmp_C = i2c->reg->CTRL;

	// Write 1 clear for those interrupts be able to W1C
	i2c->reg->STATUS = (Tmp_W & (STATUS_W1C_ALL & ~(STATUS_CMPL | STATUS_ADDR_HIT)));

	if ((Tmp_S & STATUS_CMPL) && (Tmp_S & STATUS_ADDR_HIT))
	{
		// Do address hit handler
		if (i2c->info->Status.direction == 0)
		{
			// Slave mode
			if (!(Tmp_S1 & SETUP_MASTER))
			{
				i2c->reg->STATUS = STATUS_ADDR_HIT;

				if ((((Tmp_C & CTRL_DIR) >> 8)) == I2C_SLAVE_TX)
				{
					i2c->reg->STATUS = STATUS_CMPL;
				}

				// Cleared on start of next Slave operation
				i2c->info->Status.general_call = 0;

				if (((Tmp_C & CTRL_DIR) >> 8) == I2C_SLAVE_RX)
				{
					i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val = Tmp_C;

					// Notify middle ware to do slave RX action
					Evt_Id |= AE350_I2C_EVENT_SLAVE_RECEIVE;
					i2c->info->Status.direction = 1;

					// Clear and set driver state to slave RX before data transaction
					i2c->info->Driver_State = I2C_DRV_SLAVE_RX;
				}
				else if (((Tmp_C & CTRL_DIR) >> 8) == I2C_SLAVE_TX)
				{
					// Notify middle ware to do slave TX action
					Evt_Id |= AE350_I2C_EVENT_SLAVE_TRANSMIT;
					i2c->info->Status.direction = 2;

					// Clear and set driver state to slave TX before data transaction
					i2c->info->Driver_State = I2C_DRV_SLAVE_TX;
				}

				// A new I2C data transaction(start-address-data-stop)
				i2cx_slave_addr_hit_handler(i2c);
			}

			Evt_Id |= AE350_I2C_EVENT_TRANSFER_INCOMPLETE;
		}
		else
		{
			i2c->reg->STATUS = STATUS_CMPL;

			// As I2C interrupt hit, quick store hardware already-transfered data count
			i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val = Tmp_C;

			i2c_cmpl_handler(i2c);

			Evt_Id |= AE350_I2C_EVENT_TRANSFER_DONE;
		}
	}
	else if (Tmp_S & STATUS_CMPL)
	{
		i2c->reg->STATUS = STATUS_CMPL;

		// As I2C interrupt hit, quick store hardware already-transfered data count
		i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val = Tmp_C;

		i2c_cmpl_handler(i2c);

		Evt_Id |= AE350_I2C_EVENT_TRANSFER_DONE;

	}
	else if (Tmp_S & STATUS_ADDR_HIT)
	{
		if (!(Tmp_S1 & SETUP_MASTER))
		{
			i2c->reg->STATUS = STATUS_ADDR_HIT;

			if ((((Tmp_C & CTRL_DIR) >> 8)) == I2C_SLAVE_TX)
			{
				i2c->reg->STATUS = STATUS_CMPL;
			}

			// Cleared on start of next Slave operation
			i2c->info->Status.general_call = 0;

			if (((Tmp_C & CTRL_DIR) >> 8) == I2C_SLAVE_RX)
			{
				i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val = Tmp_C;

				// Notify middle ware to do slave RX action
				Evt_Id |= AE350_I2C_EVENT_SLAVE_RECEIVE;
				i2c->info->Status.direction = 1;

				// Clear and set driver state to slave RX before data transaction
				i2c->info->Driver_State = I2C_DRV_SLAVE_RX;
			}
			else if (((Tmp_C & CTRL_DIR) >> 8) == I2C_SLAVE_TX)
			{
				// Notify middle ware to do slave TX action
				Evt_Id |= AE350_I2C_EVENT_SLAVE_TRANSMIT;
				i2c->info->Status.direction = 2;

				// Clear and set driver state to slave TX before data transaction
				i2c->info->Driver_State = I2C_DRV_SLAVE_TX;
			}

			// A new I2C data transaction(start-address-data-stop)
			i2cx_slave_addr_hit_handler(i2c);
		}
		Evt_Id |= AE350_I2C_EVENT_TRANSFER_INCOMPLETE;
	}
	else
	{
		Evt_Id |= AE350_I2C_EVENT_TRANSFER_INCOMPLETE;
	}

	if (Tmp_S & STATUS_FIFO_EMPTY)
	{
		i2cx_fifo_empty_handler(i2c);
	}

	if (Tmp_S & STATUS_FIFO_FULL)
	{
		// As I2C interrupt hit, quick store hardware already-transfered data count
		i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val = Tmp_C;

		i2cx_fifo_full_handler(i2c);
	}

	// For slave mode to complete slave TX
	if (!(Tmp_S & STATUS_ACK))
	{
		// Slave mode
		if (!(Tmp_S1 & SETUP_MASTER))
		{
			if (Tmp_S & STATUS_BYTE_RECV)
			{
				//...
			}
			else if (Tmp_S & STATUS_BYTE_TRANS)
			{
				// Notify middle ware to do slave TX complete
				Evt_Id |= AE350_I2C_EVENT_ADDRESS_NACK;

				i2c->info->nack_assert = 1;
			}
		}
	}

	// Indicates that the address of the current
	// transaction is a general call address.
	// This status is only valid in slave mode.
	// A new I2C data transaction(start-address-data-stop)
	// Slave mode
	if ((Tmp_S & STATUS_ADDR_HIT) && (Tmp_S & STATUS_GEN_CALL) && !(Tmp_S1 & SETUP_MASTER))
	{
		// Cleared on start of next Slave operation
		i2c->info->Status.general_call = 1;

		// I2C General Call slave address
		Evt_Id |= AE350_I2C_EVENT_GENERAL_CALL;

		i2c->info->Status.direction = 1;
	}

	// Master mode
	if ((Tmp_S & STATUS_ARB_LOSE) && (Tmp_S1 & SETUP_MASTER))
	{
		// Cleared on start of next Master operation
		i2c->info->Status.arbitration_lost = 1;

		Evt_Id |= AE350_I2C_EVENT_ARBITRATION_LOST;
	}

	// Slave mode
	if ((Tmp_S & STATUS_BYTE_TRANS) && !(Tmp_S1 & SETUP_MASTER))
	{
		// I2C clear FIFO first to prevent mistake i2cx_slave_fifo_read()
		// The Completion Interrupt asserts, clear the FIFO and go next transaction.
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_CLEAR_FIFO);
		i2c->reg->CMD = Tmp_C;

		// Set on start of next slave operation, cleared on slave TX 1 byte done or data transaction complete.
		i2c->info->Status.busy = 0;
	}

	i2cx_signalevent(Evt_Id, i2c);
}

// DMA TX
#if ((DRV_I2C) && (DRV_I2C_DMA_TX_EN == 1))
void i2cx_dma_tx_event (uint32_t event, I2C_RESOURCES* i2c)
{
	switch (event)
	{
	case DMA_EVENT_TERMINAL_COUNT_REQUEST:
		break;
	case DMA_EVENT_ERROR:
		break;
	default:
		break;
	}
}
#endif

// DMA RX
#if ((DRV_I2C) && (DRV_I2C_DMA_RX_EN == 1))
static void i2cx_dma_rx_event (uint32_t event, I2C_RESOURCES* i2c)
{
	switch (event)
	{
	case DMA_EVENT_TERMINAL_COUNT_REQUEST:
		// Clear busy bit on DMA complete event as master DMA RX
		i2c->info->Status.busy = 0;
		break;
	case DMA_EVENT_ERROR:
		break;
	default:
		break;
	}
}
#endif


#if (DRV_I2C)

// Get version
AE350_DRIVER_VERSION i2c_get_version(void)
{
	return DriverVersion;
}

// Get capabilities
AE350_I2C_CAPABILITIES i2c_get_capabilities(void)
{
	return DriverCapabilities;
}

// Initialized
int32_t i2c_initialize(AE350_I2C_SignalEvent_t cb_event)
{
	return  i2cx_initialize(cb_event, &I2C_Resources);
}

// Uninitialized
int32_t i2c_uninitialize(void)
{
	return  i2cx_uninitialize(&I2C_Resources);
}

// Power control
int32_t i2c_power_control(AE350_POWER_STATE state)
{
	return  i2cx_power_control(state, &I2C_Resources);
}

// Master transmit data
int32_t i2c_master_transmit(uint32_t addr, const uint8_t* data, uint32_t num, bool xfer_pending)
{
	return  i2cx_master_transmit(addr, data, num, xfer_pending, &I2C_Resources);
}

// Master receive data
int32_t i2c_master_receive(uint32_t addr, uint8_t* data, uint32_t num, bool xfer_pending)
{
	return  i2cx_master_receive(addr, data, num, xfer_pending, &I2C_Resources);
}

// Slave transmit data
int32_t i2c_slave_transmit(const uint8_t* data, uint32_t num)
{
	return  i2cx_slave_transmit(data, num, &I2C_Resources);
}

// Slave receive data
int32_t i2c_slave_receive(uint8_t* data, uint32_t num)
{
	return  i2cx_slave_receive(data, num, &I2C_Resources);
}

// Get data count
uint32_t i2c_get_datacount(void)
{
	return  i2cx_get_datacount(&I2C_Resources);
}

// Control command
int32_t i2c_control(uint32_t control, uint32_t arg)
{
	return  i2cx_control(control, arg, 0, &I2C_Resources);
}

// Get status
AE350_I2C_STATUS i2c_get_status(void)
{
	return  i2cx_getstatus(&I2C_Resources);
}

// DMA TX event
#if (DRV_I2C_DMA_TX_EN == 1)
void i2c_dma_tx_event (uint32_t event)
{
	i2cx_dma_tx_event(event, &I2C_Resources);
}
#endif

// DMA RX event
#if (DRV_I2C_DMA_RX_EN == 1)
void i2c_dma_rx_event (uint32_t event)
{
	i2cx_dma_rx_event(event, &I2C_Resources);
}
#endif

// I2C interrupt handler
void i2c_irq_handler(void)
{
	i2cx_irq_handler(&I2C_Resources);
}


// I2C driver control block
AE350_DRIVER_I2C Driver_I2C =
{
	i2c_get_version,		// Get version
	i2c_get_capabilities,	// Get capability
	i2c_initialize,			// Initializes
	i2c_uninitialize,		// Uninitialized
	i2c_power_control,		// Power control
	i2c_master_transmit,	// Master transmit data
	i2c_master_receive,		// Master receive data
	i2c_slave_transmit,		// Slave transmit data
	i2c_slave_receive,		// Slave receive data
	i2c_get_datacount,		// Get data count
	i2c_control,			// Control
	i2c_get_status			// Get status
};

#endif	// DRV_I2C
