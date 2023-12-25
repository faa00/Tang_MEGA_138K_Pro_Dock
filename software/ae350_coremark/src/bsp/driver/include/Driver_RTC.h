/*
 * ******************************************************************************************
 * File		: Driver_RTC.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: RTC driver definitions
 * ******************************************************************************************
 */

#ifndef __DRIVER_RTC_H__
#define __DRIVER_RTC_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_Common.h"


// Definitions ------------------------------------------------------------------------------

// API version
#define AE350_RTC_API_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,02)

// RTC status
typedef struct _AE350_RTC_STATUS
{
    unsigned int Status;
} AE350_RTC_STATUS;

// RTC control ID
typedef enum _AE350_RTC_CTRL_ID
{
	AE350_RTC_CTRL_NONE,
	AE350_RTC_CTRL_EN,
	AE350_RTC_CTRL_ALARM_WAKEUP,
	AE350_RTC_CTRL_ALARM_INT,
	AE350_RTC_CTRL_DAY_INT,
	AE350_RTC_CTRL_HOUR_INT,
	AE350_RTC_CTRL_MIN_INT,
	AE350_RTC_CTRL_SEC_INT,
	AE350_RTC_CTRL_HSEC_INT,
} AE350_RTC_CTRL_ID;

// RTC event ID
#define AE350_RTC_EVENT_ALARM_INT     (1UL << 0)
#define AE350_RTC_EVENT_DAY_INT       (1UL << 1)
#define AE350_RTC_EVENT_HOUR_INT      (1UL << 2)
#define AE350_RTC_EVENT_MIN_INT       (1UL << 3)
#define AE350_RTC_EVENT_SEC_INT       (1UL << 4)
#define AE350_RTC_EVENT_HSEC_INT      (1UL << 5)

// Callback function
typedef void (*AE350_RTC_SignalEvent_t) (uint32_t event);

// RTC driver time format.
typedef struct _AE350_RTC_TIME
{
    uint32_t day;
    uint32_t hour;
    uint32_t min;
    uint32_t sec;
} AE350_RTC_TIME;

// RTC driver date format.
typedef struct _AE350_RTC_ALARM
{
    uint32_t hour;
    uint32_t min;
    uint32_t sec;
} AE350_RTC_ALARM;

// Access structure of the RTC driver.
typedef struct _AE350_DRIVER_RTC
{
	AE350_DRIVER_VERSION (*GetVersion)   (void);									// Get version
    int32_t              (*Initialize) 	 (AE350_RTC_SignalEvent_t cb_event);		// Initializes
    int32_t              (*Uninitialize) (void);									// Uninitialized
    int32_t              (*PowerControl) (AE350_POWER_STATE state);					// Power control
    int32_t              (*SetTime)      (AE350_RTC_TIME* stime);					// Set time
    int32_t              (*GetTime)      (AE350_RTC_TIME* stime);					// Get time
    int32_t              (*SetAlarm)     (AE350_RTC_ALARM* salarm);					// Set alarm
    int32_t              (*GetAlarm)     (AE350_RTC_ALARM* salarm);					// Get alarm
    int32_t              (*Control)      (uint32_t control, uint32_t arg);			// Control
    AE350_RTC_STATUS     (*GetStatus)    (void);									// Get status
} const AE350_DRIVER_RTC;


#endif /* __DRIVER_RTC_H__ */
