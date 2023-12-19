/*
 * ******************************************************************************************
 * File		: dma_ae350.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: DMA driver definitions
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "dma_ae350.h"

// If enable L1 cache
#ifdef CFG_CACHE_ENABLE
#include "cache.h"
#endif


// Variables ---------------------------------------------------------------------------------

// DMA channel information
typedef struct
{
	uint32_t           SrcAddr;
	uint32_t           DstAddr;
	uint32_t           Size;
	uint32_t           Cnt;
	DMA_SignalEvent_t  cb_event;
} DMA_Channel_Info;

static uint32_t channel_active = 0U;
static uint32_t init_cnt       = 0U;

static DMA_Channel_Info channel_info[DMA_NUMBER_OF_CHANNELS];
#define DMA_CHANNEL(n)  ((DMA_CHANNEL_REG *)&(DEV_DMA->CHANNEL[n]))

#ifdef CFG_CACHE_ENABLE
#define DRAM_START   0x00000000U
#define DRAM_END     0x7FFFFFFFU
#define IS_ADDR_IN_RAM(addr) ((addr >= DRAM_START) && (addr <= DRAM_END))
#endif

#ifdef CFG_CACHE_ENABLE
#define DMA_DCACHE_WRITEBACK(start, size)        ae350_dma_writeback_range(start, size)
#define DMA_DCACHE_INVALID(start, size)          ae350_dma_invalidate_range(start, size)
#define DMA_DCACHE_INVALID_AFTER(start, size)    ae350_dma_invalidate_range2(start, size)
#else
#define DMA_DCACHE_WRITEBACK(start, size)        NULL
#define DMA_DCACHE_INVALID(start, size)          NULL
#define DMA_DCACHE_INVALID_AFTER(start, size)    NULL
#endif


// Definitions ------------------------------------------------------------------------------

/**********************************************************************
  \fn          int32_t set_channel_active_flag (uint8_t ch)
  \brief       Protected set of channel active flag
  \param[in]   ch        Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
__inline static int32_t set_channel_active_flag (uint8_t ch)
{
	uint8_t gie = (read_csr(NDS_MSTATUS) & (1 << 3));

	if (gie)
	{
		/* Disable global interrupt for core */
		clear_csr(NDS_MSTATUS, MSTATUS_MIE);
	}

	if (channel_active & (1U << ch))
	{
		if (gie)
		{
			/* Enable interrupts in general. */
			set_csr(NDS_MSTATUS, MSTATUS_MIE);
		}

		return -1;
	}
	channel_active |= (1U << ch);

	if (gie)
	{
		/* Enable interrupts in general. */
		set_csr(NDS_MSTATUS, MSTATUS_MIE);
	}

	return 0;
}

/**********************************************************************
  \fn          void clear_channel_active_flag (uint8_t ch)
  \brief       Protected clear of channel active flag
  \param[in]   ch        Channel number (0..7)
*********************************************************************/
__inline static void clear_channel_active_flag (uint8_t ch)
{
	uint8_t gie = (read_csr(NDS_MSTATUS) & (1 << 3));

	if (gie)
	{
		/* Disable global interrupt for core */
		clear_csr(NDS_MSTATUS, MSTATUS_MIE);
	}

  	channel_active &= ~(1U << ch);

	if (gie)
	{
		/* Enable interrupts in general. */
		set_csr(NDS_MSTATUS, MSTATUS_MIE);
	}
}

/**********************************************************************
  \fn          int32_t dma_initialize (void)
  \brief       Initialize DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
int32_t dma_initialize (void)
{
	uint32_t ch_num;

	init_cnt++;

	// Check if already initialized
	if (init_cnt > 1U)
	{
		return 0;
	}

	// Reset DMA
	DEV_DMA->DMACTRL = 1U;

	// Reset all DMA channels
	for (ch_num = 0U; ch_num < DMA_NUMBER_OF_CHANNELS; ch_num++)
	{
		DMA_CHANNEL(ch_num)->CTRL     = 0U;
		channel_info[ch_num].SrcAddr  = 0U;
		channel_info[ch_num].DstAddr  = 0U;
		channel_info[ch_num].Size     = 0U;
		channel_info[ch_num].Cnt      = 0U;
	}

	// Clear all DMA interrupt flags
	DEV_DMA->INTSTATUS = 0xFFFFFF;

	// Priority must be set > 0 to trigger the interrupt
	__nds__plic_set_priority(IRQ_DMA_SOURCE, 1);

	// Enable PLIC interrupt DMA source
	__nds__plic_enable_interrupt(IRQ_DMA_SOURCE);

	// Enable the Machine-External bit in MIE
	set_csr(NDS_MIE, MIP_MEIP);

	// Enable GIE
	set_csr(NDS_MSTATUS, MSTATUS_MIE);

	return 0;
}

/**********************************************************************
  \fn          int32_t dma_uninitialize (void)
  \brief       Uninitialized DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
int32_t dma_uninitialize (void)
{
	// Check if DMA is initialized
	if (init_cnt == 0U)
	{
		return -1;
	}

	init_cnt--;
	if (init_cnt != 0U)
	{
		return 0;
	}

	// Disable and Clear DMA IRQ
	__nds__plic_disable_interrupt(IRQ_DMA_SOURCE);

	return 0;
}

/**********************************************************************
  \fn          int32_t dma_channel_configure (uint8_t              ch,
                                              uint32_t             src_addr,
                                              uint32_t             dst_addr,
                                              uint32_t             size,
                                              uint32_t             control,
                                              DMA_SignalEvent_t    cb_event)
  \brief       Configure DMA channel for next transfer
  \param[in]   ch        Channel number (0..7)
  \param[in]   src_addr  Source address
  \param[in]   dst_addr  Destination address
  \param[in]   size      Amount of data to transfer
  \param[in]   control   Channel control
  \param[in]   cb_event  Channel callback pointer
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
int32_t dma_channel_configure (	uint8_t            ch,
								uint32_t           src_addr,
								uint32_t           dst_addr,
								uint32_t           size,
								uint32_t           control,
								DMA_SignalEvent_t  cb_event)
{
	DMA_CHANNEL_REG* dma_ch;

	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS)
	{
		return -1;
	}

	// Set Channel active flag
	if (set_channel_active_flag (ch) == -1)
	{
		return -1;
	}

#ifdef CFG_CACHE_ENABLE
	if ((control & DMA_CH_CTRL_DMODE_HANDSHAKE) && IS_ADDR_IN_RAM(src_addr))
	{
		DMA_DCACHE_WRITEBACK(src_addr, size);
	}

	if ((control & DMA_CH_CTRL_SMODE_HANDSHAKE) && IS_ADDR_IN_RAM(dst_addr))
	{
		DMA_DCACHE_INVALID(dst_addr, size);
	}
#endif

	// Save callback pointer
	channel_info[ch].cb_event = cb_event;

	dma_ch = DMA_CHANNEL(ch);

	// Reset DMA Channel configuration
	dma_ch->CTRL = 0U;

	// Clear DMA interrupts status
	DEV_DMA->INTSTATUS = (1U << (16+ch));
	DEV_DMA->INTSTATUS = (1U << (8+ch));
	DEV_DMA->INTSTATUS = (1U << ch);

	// Link list not supported
	dma_ch->LLPL = 0U;
	dma_ch->LLPH = 0U;

	channel_info[ch].Size = size;
	if (size > 0x3FFFFFU)
	{
		// Max DMA transfer size = 4M
		size = 0x3FFFFFU;
	}

	dma_ch->TRANSIZE = size;
	// Set Source and Destination address
	dma_ch->SRCADDRL = src_addr;
	dma_ch->DSTADDRL = dst_addr;
	dma_ch->SRCADDRH = 0U;
	dma_ch->DSTADDRH = 0U;

	if (!(control & DMA_CH_CTRL_SRCADDR_FIX))
	{
		if (control & DMA_CH_CTRL_SRCADDR_DEC)
		{
			// Source address decrement
			src_addr -= (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
		}
		else
		{
			// Source address increment
			src_addr += (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
		}
	}

	if (!(control & DMA_CH_CTRL_DSTADDR_FIX))
	{
		if (control & DMA_CH_CTRL_DSTADDR_DEC)
		{
			// Source address decrement
			dst_addr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
		else
		{
			// Destination address increment
			dst_addr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
	}

	// Save channel information
	channel_info[ch].SrcAddr = src_addr;
	channel_info[ch].DstAddr = dst_addr;
	channel_info[ch].Cnt     = size;

	// Compiler barrier to ensure channel_info[ch] is completed before setup DMA CTRL register.
	// It is in RDS_V511 IDE.
	asm volatile("" ::: "memory");

	dma_ch->CTRL = control;

	if ((control & DMA_CH_CTRL_ENABLE) == 0U)
	{
		// Clear Channel active flag
		clear_channel_active_flag (ch);
	}

	return 0;
}

/**********************************************************************
  \fn          int32_t dma_channel_enable (uint8_t ch)
  \brief       Enable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
int32_t dma_channel_enable (uint8_t ch)
{
	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS)
	{
		return -1;
	}

	// Set Channel active flag
	if (set_channel_active_flag (ch) == -1)
	{
		return -1;
	}

	DMA_CHANNEL(ch)->CTRL |= DMA_CH_CTRL_ENABLE;

	return 0;
}

/**********************************************************************
  \fn          int32_t dma_channel_disable (uint8_t ch)
  \brief       Disable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
int32_t dma_channel_disable (uint8_t ch)
{
	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS)
	{
		return -1;
	}

#ifdef CFG_CACHE_ENABLE
	uint32_t dst_addr = channel_info[ch].DstAddr;
	uint32_t cnt = channel_info[ch].Cnt;
	uint32_t size = channel_info[ch].Size;
	uint32_t control = DMA_CHANNEL(ch)->CTRL;

	if (!(control & DMA_CH_CTRL_DSTADDR_FIX))
	{
		if (control & DMA_CH_CTRL_DSTADDR_DEC)
		{
			// Source address increment to complement
			dst_addr += (cnt << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
		else
		{
			// Destination address decrement to complement
			dst_addr -= (cnt << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
	}

	if((control & DMA_CH_CTRL_SMODE_HANDSHAKE) && IS_ADDR_IN_RAM(dst_addr))
	{
		DMA_DCACHE_INVALID_AFTER(dst_addr, size);
	}
#endif

	// Clear Channel active flag
	clear_channel_active_flag (ch);

	DMA_CHANNEL(ch)->CTRL &= ~DMA_CH_CTRL_ENABLE;

	return 0;
}

/**********************************************************************
  \fn          int32_t dma_channel_abort (uint8_t ch)
  \brief       Disable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
int32_t dma_channel_abort (uint8_t ch)
{
	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS)
	{
		return -1;
	}

#ifdef CFG_CACHE_ENABLE
	uint32_t dst_addr = channel_info[ch].DstAddr;
	uint32_t cnt = channel_info[ch].Cnt;
	uint32_t size = channel_info[ch].Size;
	uint32_t control = DMA_CHANNEL(ch)->CTRL;

	if (!(control & DMA_CH_CTRL_DSTADDR_FIX))
	{
		if (control & DMA_CH_CTRL_DSTADDR_DEC)
		{
			// Source address increment to complement
			dst_addr += (cnt << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
		else
		{
			// Destination address decrement to complement
			dst_addr -= (cnt << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
	}

	if((control & DMA_CH_CTRL_SMODE_HANDSHAKE) && IS_ADDR_IN_RAM(dst_addr))
	{
		DMA_DCACHE_INVALID_AFTER(dst_addr, size);
	}
#endif

	// Clear Channel active flag
	clear_channel_active_flag (ch);

	// abort DMA ch transfer
	DEV_DMA->CHABORT = (1U << ch);

	DMA_CHANNEL(ch)->CTRL     = 0U;
	channel_info[ch].SrcAddr  = 0U;
	channel_info[ch].DstAddr  = 0U;
	channel_info[ch].Size     = 0U;
	channel_info[ch].Cnt      = 0U;

	// Clear DMA interrupts status
	DEV_DMA->INTSTATUS = (1U << (16+ch));
	DEV_DMA->INTSTATUS = (1U << (8+ch));
	DEV_DMA->INTSTATUS = (1U << ch);

	return 0;
}

/**********************************************************************
  \fn          uint32_t dma_channel_get_status (uint8_t ch)
  \brief       Check if DMA channel is enabled or disabled
  \param[in]   ch Channel number (0..7)
  \returns     Channel status
   - \b  1: channel enabled
   - \b  0: channel disabled
*********************************************************************/
uint32_t dma_channel_get_status (uint8_t ch)
{
	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS)
	{
		return 0U;
	}

	if (channel_active & (1 << ch))
	{
		return 1U;
	}
	else
	{
		return 0U;
	}
}

/**********************************************************************
  \fn          uint32_t dma_channel_get_count (uint8_t ch)
  \brief       Get number of transferred data
  \param[in]   ch Channel number (0..7)
  \returns     Number of transferred data
*********************************************************************/
uint32_t dma_channel_get_count (uint8_t ch)
{
	// Check if channel is valid
	if (ch >= DMA_NUMBER_OF_CHANNELS)
	{
		return 0;
	}

	return (channel_info[ch].Cnt - (DMA_CHANNEL(ch)->TRANSIZE & 0x3FFFFF));
}

/**********************************************************************
  \fn          void dma_irq_handler (void)
  \brief       DMA interrupt handler
*********************************************************************/
void dma_irq_handler (void)
{
	uint32_t ch, size;
	DMA_CHANNEL_REG * dma_ch;

	for (ch = 0; ch < DMA_NUMBER_OF_CHANNELS; ch++)
	{
		if ((DEV_DMA->INTSTATUS & (1U << ch)) == 0)
		{
			dma_ch = DMA_CHANNEL(ch);

			// Terminal count request interrupt
			if (DEV_DMA->INTSTATUS & (1U << (16 + ch)))
			{
				// Clear interrupt flag
				DEV_DMA->INTSTATUS = (1U << (16 + ch));

				if (channel_info[ch].Cnt != channel_info[ch].Size)
				{
					// Data waiting to transfer
					uint32_t control;

					size = channel_info[ch].Size - channel_info[ch].Cnt;
					// Max DMA transfer size = 4M
					if (size > 0x3FFFFFU)
					{
						size = 0x3FFFFFU;
					}

					channel_info[ch].Cnt += size;
					control = dma_ch->CTRL;

					if (!(control & DMA_CH_CTRL_SRCADDR_FIX))
					{
						dma_ch->SRCADDRL = channel_info[ch].SrcAddr;
						dma_ch->SRCADDRH = 0U;
						if (control & DMA_CH_CTRL_SRCADDR_DEC)
						{
							// Source address decrement
							channel_info[ch].SrcAddr -= (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
						}
						else
						{
							// Source address increment
							channel_info[ch].SrcAddr += (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
						}
					}
					if (!(control & DMA_CH_CTRL_DSTADDR_FIX))
					{
						dma_ch->DSTADDRL = channel_info[ch].DstAddr;
						dma_ch->DSTADDRH = 0U;
						if (control & DMA_CH_CTRL_DSTADDR_DEC)
						{
							// Source address decrement
							channel_info[ch].DstAddr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						}
						else
						{
							// Destination address increment
							channel_info[ch].DstAddr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						}
					}

					// Set transfer size
					dma_ch->TRANSIZE = size;
					// Enable DMA Channel
					dma_ch->CTRL |= DMA_CH_CTRL_ENABLE;
				}
				else
				{
					// All Data has been transferred

#ifdef CFG_CACHE_ENABLE
					uint32_t dst_addr = channel_info[ch].DstAddr;
					uint32_t control = dma_ch->CTRL;
					uint32_t size = channel_info[ch].Size;

					if (!(control & DMA_CH_CTRL_DSTADDR_FIX))
					{
						if (control & DMA_CH_CTRL_DSTADDR_DEC)
						{
							// Source address increment to complement
							dst_addr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						}
						else
						{
							// Destination address decrement to complement
							dst_addr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						}
					}

					if((control & DMA_CH_CTRL_SMODE_HANDSHAKE) && IS_ADDR_IN_RAM(dst_addr))
					{
						DMA_DCACHE_INVALID_AFTER(dst_addr, size);
					}
#endif
					// Clear Channel active flag
					clear_channel_active_flag (ch);

					// Signal Event
					if (channel_info[ch].cb_event)
					{
						channel_info[ch].cb_event(DMA_EVENT_TERMINAL_COUNT_REQUEST);
					}
				}
			}
			else
			{
				// DMA error interrupt
				if (DEV_DMA->INTSTATUS & (1U << ch))
				{
					dma_ch->CTRL = 0U;
					// Clear Channel active flag
					clear_channel_active_flag (ch);

					// Clear interrupt flag
					DEV_DMA->INTSTATUS = (1U << ch);

					// Signal Event
					if (channel_info[ch].cb_event)
					{
						channel_info[ch].cb_event(DMA_EVENT_ERROR);
					}
				}
				// DMA abort interrupt
				else if (DEV_DMA->INTSTATUS & (1U << (8 + ch)))
				{
					dma_ch->CTRL = 0U;
					// Clear Channel active flag
					clear_channel_active_flag (ch);

					// Clear interrupt flag
					DEV_DMA->INTSTATUS = (1U << (8 + ch));

					// Signal Event
					if (channel_info[ch].cb_event)
					{
						channel_info[ch].cb_event(DMA_EVENT_ABORT);
					}
				}
			}
		}
	}
}
