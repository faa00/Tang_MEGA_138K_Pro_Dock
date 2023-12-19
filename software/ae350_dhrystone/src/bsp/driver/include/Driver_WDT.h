/*
 * ******************************************************************************************
 * File		: Driver_WDT.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: WDT driver definitions
 * ******************************************************************************************
 */

#ifndef __DRIVER_WDT_H__
#define __DRIVER_WDT_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_Common.h"


// Definitions  -----------------------------------------------------------------------------

#define AE350_WDT_API_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,02)  /* API version */

/****** WDT Control Codes *****/

#define AE350_WDT_CONTROL_Pos               0
#define AE350_WDT_CONTROL_Msk               (0xFFUL << AE350_WDT_CONTROL_Pos)

/*----- WDT Control Codes: Clock Source -----*/
#define AE350_WDT_CLKSRC_Pos                0
#define AE350_WDT_CLKSRC_Msk                (0x1UL << AE350_WDT_CLKSRC_Pos)
#define AE350_WDT_CLKSRC_APB                (0x1UL << AE350_WDT_CLKSRC_Pos)     // System clock
#define AE350_WDT_CLKSRC_EXTERNAL           (0x0UL << AE350_WDT_CLKSRC_Pos)     // External clock (default)

/*----- WDT Time Period -----*/
#define AE350_WDT_TIME_POW_2_6              (0x0UL)                           	// WDT timer interval    64 clock periods.
#define AE350_WDT_TIME_POW_2_8              (0x1UL)                           	// WDT timer interval   256 clock periods.
#define AE350_WDT_TIME_POW_2_10             (0x2UL)                           	// WDT timer interval  1024 clock periods.
#define AE350_WDT_TIME_POW_2_11             (0x3UL)                           	// WDT timer interval  2048 clock periods.
#define AE350_WDT_TIME_POW_2_12             (0x4UL)                           	// WDT timer interval  4096 clock periods.
#define AE350_WDT_TIME_POW_2_13             (0x5UL)                           	// WDT timer interval  8192 clock periods.
#define AE350_WDT_TIME_POW_2_14             (0x6UL)                           	// WDT timer interval 16384 clock periods.
#define AE350_WDT_TIME_POW_2_15             (0x7UL)                           	// WDT timer interval 32768 clock periods.

/****** WDT specific error codes *****/
#define AE350_WDT_ERROR_TIME_PREIOD         (AE350_DRIVER_ERROR_SPECIFIC - 1)   // Specified time period not supported

/**************************************************************************
\brief WDT Status
***************************************************************************/
typedef struct _AE350_WDT_STATUS
{
	uint32_t irq_expired  : 1;        			// WDT IRQ stage time expired (in WDT Interrupt)
} 	AE350_WDT_STATUS;

/****** WDT Event *****/
#define AE350_WDT_EVENT_INT_STAGE (1UL << 0)  	// WDT Interrupt event by IRQ stage time expired


/**************************************************************************
  \fn          void AE350_WDT_SignalEvent (uint32_t event)
  \brief       Signal WDT Events.
  \param[in]   event  \ref WDT_events notification mask
  \return      none
***************************************************************************/
typedef void (*AE350_WDT_SignalEvent_t) (uint32_t event);  // Pointer to \ref AE350_WDT_SignalEvent : Signal WDT Event.


/**************************************************************************
\brief WDT Device Driver Capabilities
***************************************************************************/
typedef struct _AE350_WDT_CAPABILITIES
{
	uint32_t external_timer : 1; 	// Support using external timer as clock source.
	uint32_t irq_stage      : 1; 	// Support interrupt stage before WDT reset.
} AE350_WDT_CAPABILITIES;


/**************************************************************************
\brief Access structure of the WDT Driver.
***************************************************************************/
typedef struct _AE350_DRIVER_WDT
{
  AE350_DRIVER_VERSION     (*GetVersion)      (void);                              // Get driver version
  AE350_WDT_CAPABILITIES   (*GetCapabilities) (void);                              // Get driver capabilities
  int32_t                  (*Initialize)      (AE350_WDT_SignalEvent_t cb_event);  // Initialize WDT interface
  int32_t                  (*Uninitialize)    (void);                              // Uninitialized WDT interface
  int32_t                  (*Control)         (uint32_t control, uint32_t arg);    // Control WDT interface
  void                     (*Enable)          (void);                              // Enable WDT timer
  void                     (*Disable)         (void);                              // Disable WDT timer
  void                     (*RestartTimer)    (void);                              // Restart WDT timer
  void                     (*ClearIrqStatus)  (void);                              // Clear WDT IRQ expired status
  AE350_WDT_STATUS         (*GetStatus)       (void);                              // Get WDT status
} const AE350_DRIVER_WDT;


#endif /* __DRIVER_WDT_H__ */
