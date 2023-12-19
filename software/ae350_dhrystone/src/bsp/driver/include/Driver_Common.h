/*
 * ******************************************************************************************
 * File		: Driver_Common.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Common driver definitions
 * ******************************************************************************************
 */

#ifndef __DRIVER_COMMON_H__
#define __DRIVER_COMMON_H__


// Includes ---------------------------------------------------------------------------------
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


// Definitions  -----------------------------------------------------------------------------

// Major and minor revision
#define AE350_DRIVER_VERSION_MAJOR_MINOR(major,minor) (((major) << 8) | (minor))

/**
\brief Driver Version
*/
typedef struct _AE350_DRIVER_VERSION
{
	uint16_t api;                         // API version
	uint16_t drv;                         // Driver version
} AE350_DRIVER_VERSION;

/* General return codes */
#define AE350_DRIVER_OK                 0 // Operation succeeded
#define AE350_DRIVER_ERROR             -1 // Unspecified error
#define AE350_DRIVER_ERROR_BUSY        -2 // Driver is busy
#define AE350_DRIVER_ERROR_TIMEOUT     -3 // Timeout occurred
#define AE350_DRIVER_ERROR_UNSUPPORTED -4 // Operation not supported
#define AE350_DRIVER_ERROR_PARAMETER   -5 // Parameter error
#define AE350_DRIVER_ERROR_SPECIFIC    -6 // Start of driver specific errors

/**
\brief General power states
*/ 
typedef enum _AE350_POWER_STATE
{
	AE350_POWER_OFF,                      // Power off: no operation possible
	AE350_POWER_LOW,                      // Low power mode: retain state, detect and signal wake-up events
	AE350_POWER_FULL                      // Power on: full operation at maximum performance
} AE350_POWER_STATE;


#endif /* __DRIVER_COMMON_H__ */
