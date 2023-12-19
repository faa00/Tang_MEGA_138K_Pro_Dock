/*
 * ******************************************************************************************
 * File		: rtc_ae350.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: RTC driver definitions
 * ******************************************************************************************
 */

#ifndef __RTC_AE350_H__
#define __RTC_AE350_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_RTC.h"
#include "platform.h"


// Definitions ------------------------------------------------------------------------------

#ifndef BIT
#define BIT(n)                      ((unsigned int) 1 << (n))
#define BITS2(m,n)                  (BIT(m) | BIT(n) )

/* Bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                   (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */

#define RTC_HALF_SECOND_SUPPORT   	(0)
#define RTC_DAY_BITS              	(5)        	   // for counting up to 2^5 = 32 days

// ID revision register bit define
#define RTC_ID                		BITS(12,31)    // ID number for RTC
#define RTC_MAJOR             		BITS(4,11)     // Major number for RTC
#define RTC_MINOR             		BITS(0,3)      // Minor number for RTC

// Counter register bit define
// Day passed after RTC enabled, BITS(17,21), counting up to 2^5 = 32 days
#define CNTR_DAY                    BITS(17,(17 + (RTC_DAY_BITS - 1)))
#define CNTR_HOUR                   BITS(12,16)    // Hour field of current time; range: 0 ~ 23
#define CNTR_MIN                    BITS(6,11)     // Minute field of current time; range: 0 ~ 59
#define CNTR_SEC                    BITS(0,5)      // Second field of current time; range: 0 ~ 59

// Alarm register bit define
#define ALARM_HOUR                  BITS(12,16)    // Hour field of current time; range: 0 ~ 23
#define ALARM_MIN                   BITS(6,11)     // Minute field of current time; range: 0 ~ 59
#define ALARM_SEC                   BITS(0,5)      // Second field of current time; range: 0 ~ 59

// Control register bit define
#define CTRL_RTC_EN                 BIT(0)         // Enable RTC
#define CTRL_ALARM_WAKEUP           BIT(1)         // Enable alarm wake up signal
#define CTRL_ALARM_INT              BIT(2)         // Enable alarm interrupt
#define CTRL_DAY                    BIT(3)         // Enable day interrupt
#define CTRL_HOUR                   BIT(4)         // Enable hour interrupt
#define CTRL_MIN                    BIT(5)         // Enable minute interrupt
#define CTRL_SEC                    BIT(6)         // Enable second interrupt
#define CTRL_HSEC                   BIT(7)         // Enable half-second interrupt

// Status register bit define
#define STATUS_ALARM_INT            BIT(2)         // Alarm interrupt status
#define STATUS_DAY                  BIT(3)         // Day interrupt status
#define STATUS_HOUR                 BIT(4)         // Hour interrupt status
#define STATUS_MIN                  BIT(5)         // Minute interrupt status
#define STATUS_SEC                  BIT(6)         // Second interrupt status
#define STATUS_HSEC                 BIT(7)         // Half-second interrupt status
#define STATUS_WRITEDONE            BIT(16)        // The synchronization progress of RTC register updates

// Digit trimming register bit define
#define TRIM_SEC_TRIM				BITS(0,4)		// Digital trimming value for the rest of seconds
#define TRIM_SEC_SIGN				BIT(7)			// Sign bit for the trimming value for the rest of seconds
#define TRIM_MIN_TRIM				BITS(8:12)		// Digital trimming value for the second on the minute boundary
#define TRIM_MIN_SIGN				BIT(15)			// Sign bit for the trimming value for the second on the minute boundary
#define TRIM_HOUR_TRIM				BITS(16,20)		// Digital trimming value for the second on the hour boundary
#define TRIM_HOUR_SIGN				BIT(23)			// Sign bit for the trimming value for the second on the hour boundary
#define TRIM_DAY_TRIM				BITS(24,28)		// Digital trimming value for the second on the day boundary
#define TRIM_DAY_SIGN				BIT(31)			// Sign bit for the trimming value for the second on the day boundary

// RTC information
typedef struct _RTC_INFO
{
	AE350_RTC_SignalEvent_t   Sig_Evt_CB;
	AE350_RTC_TIME            Set_Rtc_Time;
	AE350_RTC_ALARM           Set_Rtc_Alarm;
	AE350_POWER_STATE         Pwr_State;
} RTC_INFO;


#endif /* __RTC_AE350_H__ */
