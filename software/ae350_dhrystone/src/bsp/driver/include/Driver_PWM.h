/*
 * ******************************************************************************************
 * File		: Driver_PWM.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: PWM driver definitions
 * ******************************************************************************************
 */

#ifndef __DRIVER_PWM_H__
#define __DRIVER_PWM_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_Common.h"


// Definitions ------------------------------------------------------------------------------

#define AE350_PWM_API_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,02)  /* API version */


/****** PWM Control Codes *****/

#define AE350_PWM_CONTROL_Msk                 (0xFFUL)

#define AE350_PWM_ACTIVE_CONFIGURE            (0x01UL)   // Configure PWM ; arg1 = PWM channel number

/*----- PWM Control Codes: Configure Parameters: PARK Mode -----*/
#define AE350_PWM_PARK_BITS_Pos               8
#define AE350_PWM_PARK_BITS_Msk               (1UL << AE350_PWM_PARK_BITS_Pos)
#define AE350_PWM_PARK_LOW                    (0UL << AE350_PWM_PARK_BITS_Pos)    // Park LOW (default)
#define AE350_PWM_PARK_HIGH                   (1UL << AE350_PWM_PARK_BITS_Pos)    // Park HIGH

/*----- PWM Control Codes: Configure Parameters: Clock Source -----*/
#define AE350_PWM_CLKSRC_Pos                  9
#define AE350_PWM_CLKSRC_Msk                  (1UL << AE350_PWM_CLKSRC_Pos)
#define AE350_PWM_CLKSRC_SYSTEM               (0UL << AE350_PWM_CLKSRC_Pos)       // System clock (default)
#define AE350_PWM_CLKSRC_EXTERNAL             (1UL << AE350_PWM_CLKSRC_Pos)       // External clock


/***********************************************************************
\brief PWM Status
************************************************************************/
typedef struct _AE350_PWM_STATUS
{
	uint32_t configured    : 8;               // PWM channel state: 1=Configured, 0=Un-configured
	uint32_t output        : 8;               // PWM channel output: 1=Enable, 0=Disable
} AE350_PWM_STATUS;


/****** PWM Event *****/
#define AE350_PWM_EVENT_ONESTEP_COMPLETE      (1UL << 0)  // One step complete


// Function documentation
/***********************************************************************
  \fn          void AE350_PWM_SignalEvent (uint32_t event)
  \brief       Signal PWM Events.
  \param[in]   event  \ref PWM_events notification mask
  \return      none
************************************************************************/
typedef void (*AE350_PWM_SignalEvent_t) (uint32_t ch_num, uint32_t event);  // Pointer to \ref AE350_PWM_SignalEvent : Signal PWM Event.


/***********************************************************************
\brief PWM Device Driver Capabilities.
************************************************************************/
typedef struct _AE350_PWM_CAPABILITIES
{
	uint32_t channels               : 4;  // Supports PWM channel numbers
	uint32_t event_onestep_complete : 1;  // Signal one step complete event: \ref AE350_PWM_EVENT_ONESTEP_COMPLETE
} AE350_PWM_CAPABILITIES;


/***********************************************************************
\brief Access structure of the PWM Driver.
************************************************************************/
typedef struct _AE350_DRIVER_PWM
{
	AE350_DRIVER_VERSION     (*GetVersion)      (void);                              // Get driver version
	AE350_PWM_CAPABILITIES   (*GetCapabilities) (void);                              // Get driver capabilities
	int32_t                  (*Initialize)      (AE350_PWM_SignalEvent_t cb_event);  // Initialize PWM interface
	int32_t                  (*Uninitialize)    (void);                              // Uninitialized PWM interface
	int32_t                  (*PowerControl)    (AE350_POWER_STATE state);           // Control PWM interface power
	int32_t                  (*Control)         (uint32_t control, uint32_t arg);    // Control PWM interface
	int32_t                  (*SetFreq)         (uint8_t pwm, uint32_t freq);        // Set the frequency
	int32_t                  (*Output)          (uint8_t pwm, uint8_t duty);         // Start PWM output
	AE350_PWM_STATUS         (*GetStatus)       (void);                              // Get PWM status
} const AE350_DRIVER_PWM;


#endif /* __DRIVER_PWM_H__ */
