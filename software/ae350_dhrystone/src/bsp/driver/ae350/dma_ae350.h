/*
 * ******************************************************************************************
 * File		: dma_ae350.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: DMA driver definitions
 * ******************************************************************************************
 */

#ifndef __DMA_AE350_H__
#define __DMA_AE350_H__


// Includes ---------------------------------------------------------------------------------
#include <stdint.h>
#include "platform.h"


// Definitions  -----------------------------------------------------------------------------

// Number of DMA channels
#define DMA_NUMBER_OF_CHANNELS           ((uint8_t) 8)

// GPDMA events
#define DMA_EVENT_TERMINAL_COUNT_REQUEST (1)
#define DMA_EVENT_ERROR                  (2)
#define DMA_EVENT_ABORT                  (3)

// Source burst size in source and destination definitions
#define DMA_BSIZE_1                      (0x0)    // Burst size = 1
#define DMA_BSIZE_2                      (0x1)    // Burst size = 2
#define DMA_BSIZE_4                      (0x2)    // Burst size = 4
#define DMA_BSIZE_8                      (0x3)    // Burst size = 8
#define DMA_BSIZE_16                     (0x4)    // Burst size = 16
#define DMA_BSIZE_32                     (0x5)    // Burst size = 32
#define DMA_BSIZE_64                     (0x6)    // Burst size = 64
#define DMA_BSIZE_128                    (0x7)    // Burst size = 128
#define DMA_BSIZE_256                    (0x8)    // Burst size = 128
#define DMA_BSIZE_512                    (0x9)    // Burst size = 128
#define DMA_BSIZE_1024                   (0xa)    // Burst size = 128

// Width in source transfer width and destination transfer width definitions
#define DMA_WIDTH_BYTE                   (0x0)    // Width = 1 byte   byte
#define DMA_WIDTH_HALFWORD               (0x1)    // Width = 2 bytes  half word
#define DMA_WIDTH_WORD                   (0x2)    // Width = 4 bytes  word
#define DMA_WIDTH_DWORD                  (0x3)    // Width = 8 bytes  double word
#define DMA_WIDTH_QWORD                  (0x4)    // Width = 16 bytes quadrant word
#define DMA_WIDTH_EWORD                  (0x5)    // Width = 32 bytes eight word

// Bus interface index
#define DMA_INF_IDX0                     (0x0)    // Interface = 0
#define DMA_INF_IDX1                     (0x1)    // Interface = 1

// DMA Channel Control register definition
#define DMA_CH_CTRL_SBINFIDX_POS         (        31)
#define DMA_CH_CTRL_SBINF(n)             ((n)  << DMA_CH_CTRL_SBINFIDX_POS) 							// [31] SrcBusINFIDX
#define DMA_CH_CTRL_SBINF_MASK           (   1 << DMA_CH_CTRL_SBINFIDX_POS) 							// [31] SrcBusINFIDX
#define DMA_CH_CTRL_DBINFIDX_POS         (        30)
#define DMA_CH_CTRL_DBINF(n)             ((n)  << DMA_CH_CTRL_DBINFIDX_POS) 							// [30] DstBusINFIDX
#define DMA_CH_CTRL_DBINF_MASK           (   1 << DMA_CH_CTRL_DBINFIDX_POS) 							// [30] SrcBusINFIDX
#define DMA_CH_CTRL_PRIORITY_HIGH        (   1 << 29)													// [29] Priority
#define DMA_CH_CTRL_SBSIZE_POS           (        24)
#define DMA_CH_CTRL_SBSIZE_MASK          (0x0f << DMA_CH_CTRL_SBSIZE_POS)								// [27:24] SrcBurstSize
#define DMA_CH_CTRL_SBSIZE(n)            (((n) << DMA_CH_CTRL_SBSIZE_POS) & DMA_CH_CTRL_SBSIZE_MASK)	// [27:24] SrcBurstSize
#define DMA_CH_CTRL_SWIDTH_POS           (        21)
#define DMA_CH_CTRL_SWIDTH_MASK          (0x07 << DMA_CH_CTRL_SWIDTH_POS)								// [23:21] SrcWidth
#define DMA_CH_CTRL_SWIDTH(n)            (((n) << DMA_CH_CTRL_SWIDTH_POS) & DMA_CH_CTRL_SWIDTH_MASK)	// [23:21] SrcWidth
#define DMA_CH_CTRL_DWIDTH_POS           (        18)
#define DMA_CH_CTRL_DWIDTH_MASK          (0x07 << DMA_CH_CTRL_DWIDTH_POS)								// [20:18] DstWidth
#define DMA_CH_CTRL_DWIDTH(n)            (((n) << DMA_CH_CTRL_DWIDTH_POS) & DMA_CH_CTRL_DWIDTH_MASK)	// [20:18] DstWidth
#define DMA_CH_CTRL_SMODE_HANDSHAKE      (   1 << 17)													// [17] SrcMode
#define DMA_CH_CTRL_DMODE_HANDSHAKE      (   1 << 16)													// [16] DstMode
#define DMA_CH_CTRL_SRCADDRCTRL_POS      (        14)
#define DMA_CH_CTRL_SRCADDRCTRL_MASK     (0x03 << DMA_CH_CTRL_SRCADDRCTRL_POS)							// [15:14] SrcAddrCtrl
#define DMA_CH_CTRL_SRCADDR_INC          (   0 << DMA_CH_CTRL_SRCADDRCTRL_POS)							// 0x0: Increment address
#define DMA_CH_CTRL_SRCADDR_DEC          (   1 << DMA_CH_CTRL_SRCADDRCTRL_POS)							// 0x1: Decrement address
#define DMA_CH_CTRL_SRCADDR_FIX          (   2 << DMA_CH_CTRL_SRCADDRCTRL_POS)							// 0x2: Fixed address
#define DMA_CH_CTRL_DSTADDRCTRL_POS      (        12)
#define DMA_CH_CTRL_DSTADDRCTRL_MASK     (0x03 << DMA_CH_CTRL_DSTADDRCTRL_POS)							// [13:12] DstAddrCtrl
#define DMA_CH_CTRL_DSTADDR_INC          (   0 << DMA_CH_CTRL_DSTADDRCTRL_POS)							// 0x0: Increment address
#define DMA_CH_CTRL_DSTADDR_DEC          (   1 << DMA_CH_CTRL_DSTADDRCTRL_POS)							// 0x1: Decrement address
#define DMA_CH_CTRL_DSTADDR_FIX          (   2 << DMA_CH_CTRL_DSTADDRCTRL_POS)							// 0x2: Fixed address
#define DMA_CH_CTRL_SRCREQ_POS           (        8)
#define DMA_CH_CTRL_SRCREQ_MASK          (0x0F << DMA_CH_CTRL_SRCREQ_POS)								// [11:8] SrcReqSel
#define DMA_CH_CTRL_SRCREQ(n)            (((n) << DMA_CH_CTRL_SRCREQ_POS) & DMA_CH_CTRL_SRCREQ_MASK)	// [11:8] SrcReqSel
#define DMA_CH_CTRL_DSTREQ_POS           (        4)
#define DMA_CH_CTRL_DSTREQ_MASK          (0x0F << DMA_CH_CTRL_DSTREQ_POS)								// [7:4] DstReqSel
#define DMA_CH_CTRL_DSTREQ(n)            (((n) << DMA_CH_CTRL_DSTREQ_POS) & DMA_CH_CTRL_DSTREQ_MASK)	// [7:4] DstReqSel
#define DMA_CH_CTRL_INTABT               (   0 << 3)													// [3] IntAbtMask
#define DMA_CH_CTRL_INTERR               (   0 << 2)													// [2] IntErrMask
#define DMA_CH_CTRL_INTTC                (   0 << 1)													// [1] IntTCMask
#define DMA_CH_CTRL_ENABLE               (   1 << 0)													// [0] Enable


// Declarations  ---------------------------------------------------------------------------

/**********************************************************************
  \fn          void DMA_SignalEvent_t (uint32_t event)
  \brief       Signal DMA Events.
  \param[in]   event  DMA Event mask
  \return      none
*********************************************************************/
typedef void (*DMA_SignalEvent_t) (uint32_t event);

/**********************************************************************
  \fn          int32_t dma_initialize (void)
  \brief       Initialize DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
extern int32_t dma_initialize (void);

/**********************************************************************
  \fn          int32_t dma_uninitialize (void)
  \brief       Uninitialized DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
extern int32_t dma_uninitialize (void);

/**********************************************************************
  \fn          int32_t dma_channel_configure (uint8_t            ch,
                                              uint32_t           src_addr,
                                              uint32_t           dst_addr,
                                              uint32_t           size,
                                              uint32_t           control,
                                              DMA_SignalEvent_t  cb_event)
  \brief       Configure DMA channel for next transfer
  \param[in]   ch        Channel number (0..7)
  \param[in]   src_addr  Source address
  \param[in]   dest_addr Destination address
  \param[in]   size      Amount of data to transfer
  \param[in]   control   Channel control
  \param[in]   cb_event  Channel callback pointer
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
extern int32_t dma_channel_configure (	uint8_t				ch,
										uint32_t			src_addr,
										uint32_t			dst_addr,
										uint32_t			size,
										uint32_t			control,
										DMA_SignalEvent_t	cb_event);

/**********************************************************************
  \fn          int32_t dma_channel_enable (uint8_t ch)
  \brief       Enable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
extern int32_t dma_channel_enable (uint8_t ch);

/**********************************************************************
  \fn          int32_t dma_channel_disable (uint8_t ch)
  \brief       Disable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
extern int32_t dma_channel_disable (uint8_t ch);

/**********************************************************************
  \fn          uint32_t dma_channel_get_status (uint8_t ch)
  \brief       Check if DMA channel is enabled or disabled
  \param[in]   ch Channel number (0..7)
  \returns     Channel status
   - \b  1: channel enabled
   - \b  0: channel disabled
*********************************************************************/
extern uint32_t dma_channel_get_status (uint8_t ch);

/**********************************************************************
  \fn          uint32_t dma_channel_get_count (uint8_t ch)
  \brief       Get number of transferred data
  \param[in]   ch Channel number (0..7)
  \returns     Number of transferred data
*********************************************************************/
extern uint32_t dma_channel_get_count (uint8_t ch);

/**********************************************************************
  \fn          uint32_t dma_channel_abort (uint8_t ch)
  \brief       Abort ch transfer
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*********************************************************************/
extern int32_t dma_channel_abort (uint8_t ch);


#endif /* __DMA_AE350_H__ */
