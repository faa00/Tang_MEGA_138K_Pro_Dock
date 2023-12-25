/*
 * ******************************************************************************************
 * File		: wdt_ae350.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: WDT driver definitions
 * ******************************************************************************************
 */

#ifndef __WDT_AE350_H__
#define __WDT_AE350_H__

// Includes ---------------------------------------------------------------------------------
#include "Driver_WDT.h"
#include "platform.h"


// Definitions  -----------------------------------------------------------------------------

/* 0x10 Control Register */
#define WDT_CTRL_RSTTIME_POW_2_7        0x000
#define WDT_CTRL_RSTTIME_POW_2_8        0x100
#define WDT_CTRL_RSTTIME_POW_2_9        0x200
#define WDT_CTRL_RSTTIME_POW_2_10       0x300
#define WDT_CTRL_RSTTIME_POW_2_11       0x400
#define WDT_CTRL_RSTTIME_POW_2_12       0x500
#define WDT_CTRL_RSTTIME_POW_2_13       0x600
#define WDT_CTRL_RSTTIME_POW_2_14       0x700

#define WDT_CTRL_INTTIME_POW_2_6        0x000
#define WDT_CTRL_INTTIME_POW_2_8        0x010
#define WDT_CTRL_INTTIME_POW_2_10       0x020
#define WDT_CTRL_INTTIME_POW_2_11       0x030
#define WDT_CTRL_INTTIME_POW_2_12       0x040
#define WDT_CTRL_INTTIME_POW_2_13       0x050
#define WDT_CTRL_INTTIME_POW_2_14       0x060
#define WDT_CTRL_INTTIME_POW_2_15       0x070

#define WDT_CTRL_INTTIME_POW_2_17       0x080
#define WDT_CTRL_INTTIME_POW_2_19       0x090
#define WDT_CTRL_INTTIME_POW_2_21       0x0A0
#define WDT_CTRL_INTTIME_POW_2_23       0x0B0
#define WDT_CTRL_INTTIME_POW_2_25       0x0C0
#define WDT_CTRL_INTTIME_POW_2_27       0x0D0
#define WDT_CTRL_INTTIME_POW_2_29       0x0E0
#define WDT_CTRL_INTTIME_POW_2_31       0x0F0

#define WDT_CTRL_RSTEN                  0x8
#define WDT_CTRL_INTEN                  0x4
#define WDT_CTRL_APBCLK                 0x2
#define WDT_CTRL_EXTCLK                 0x0
#define WDT_CTRL_EN                     0x1

/* 0x14 Restart Register */
#define WDT_RESTART_NUM                 0xcafe

/* 0x18 Write Enable Register */
#define WDT_WREN_NUM                    0x5aa5

/* 0x1C Status Register */
#define WDT_ST_INTEXPIRED               0x1
#define WDT_ST_INTEXPIRED_CLR           0x1   // w1c bit

// WDT flags
#define WDT_FLAG_INITIALIZED            (1U << 0)
#define WDT_FLAG_POWERED                (1U << 1)

// WDT information (Run-Time)
typedef struct _WDT_INFO
{
	AE350_WDT_SignalEvent_t   cb_event;       // Event callback
	uint32_t                  flags;          // WDT driver flags
} WDT_INFO;

// WDT resources definitions
typedef struct
{
	AE350_WDT_CAPABILITIES     capabilities;  // Capabilities
	WDT_RegDef                 *reg;          // Pointer to WDT register
	WDT_INFO                   *info;         // Run-Time information
} const WDT_RESOURCES;


#endif /* __WDT_AE350_H__ */
