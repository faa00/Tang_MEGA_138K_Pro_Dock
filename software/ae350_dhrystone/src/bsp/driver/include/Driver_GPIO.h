/*
 * ******************************************************************************************
 * File		: Driver_GPIO.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: GPIO driver definitions
 * ******************************************************************************************
 */

#ifndef __DRIVER_GPIO_H__
#define __DRIVER_GPIO_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_Common.h"


// Definitions  -----------------------------------------------------------------------------

// API version
#define AE350_GPIO_API_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2, 02)

// GPIO direction
// Channel direction register
#define AE350_GPIO_DIR_INPUT			  0x0			// Input direction
#define AE350_GPIO_DIR_OUTPUT             0x1			// Output direction

// GPIO De-bounce clock source selection
#define AE350_GPIO_DB_CLKSRC_EXT		  0x0			// External clock source
#define AE350_GPIO_DB_CLKSRC_P			  0x1			// APB clock source

// Control command
// GPIO interrupt mode
#define AE350_GPIO_INTR_ENABLE         	  (1UL << 0)	// Enable interrupt
#define AE350_GPIO_INTR_DISABLE           (1UL << 1)	// Disable interrupt
#define AE350_GPIO_SET_INTR_LOW_LEVEL     (1UL << 2)	// Low level
#define	AE350_GPIO_SET_INTR_HIGH_LEVEL    (1UL << 3)	// High level
#define	AE350_GPIO_SET_INTR_NEGATIVE_EDGE (1UL << 4)	// Negative edge
#define	AE350_GPIO_SET_INTR_POSITIVE_EDGE (1UL << 5)	// Positive edge
#define	AE350_GPIO_SET_INTR_DUAL_EDGE     (1UL << 6)	// Dual edge
#define AE350_GPIO_INTR_CLEAR             (1UL << 7)	// Clear interrupt status

//GPIO pull mode
#define AE350_GPIO_PULL_ENABLE			  (1UL << 8)	// Enable pull
#define AE350_GPIO_PULL_DISABLE			  (1UL << 9)	// Disable pull
#define AE350_GPIO_SET_PULL_UP			  (1UL << 10)	// Pull up
#define AE350_GPIO_SET_PULL_DOWN		  (1UL << 11)	// Pull down

//GPIO de-bounce mode
#define AE350_GPIO_DB_ENABLE              (1UL << 12)	// Enable de-bounce
#define AE350_GPIO_DB_DISABLE             (1UL << 13)	// Disable de-bounce
#define AE350_GPIO_SET_DB_CLKSRC		  (1UL << 14)	// De-bounce clock source
#define AE350_GPIO_SET_DB_PRESCALE		  (1UL << 15)	// De-bounce pre-scale

// GPIO interrupt signal event ID
#define	AE350_GPIO_EVENT_PIN0             (1UL << 0)	// Pin 0 input interrupt event
#define	AE350_GPIO_EVENT_PIN1             (1UL << 1)	// Pin 1 input interrupt event
#define	AE350_GPIO_EVENT_PIN2             (1UL << 2)	// Pin 2 input interrupt event
#define	AE350_GPIO_EVENT_PIN3             (1UL << 3)	// Pin 3 input interrupt event
#define	AE350_GPIO_EVENT_PIN4             (1UL << 4)	// Pin 4 input interrupt event
#define AE350_GPIO_EVENT_PIN5             (1UL << 5)	// Pin 5 input interrupt event
#define	AE350_GPIO_EVENT_PIN6             (1UL << 6)	// Pin 6 input interrupt event
#define	AE350_GPIO_EVENT_PIN7             (1UL << 7)	// Pin 7 input interrupt event
#define	AE350_GPIO_EVENT_PIN8             (1UL << 8)	// Pin 8 input interrupt event
#define	AE350_GPIO_EVENT_PIN9             (1UL << 9)	// Pin 9 input interrupt event
#define	AE350_GPIO_EVENT_PIN10            (1UL << 10)	// Pin 10 input interrupt event
#define	AE350_GPIO_EVENT_PIN11            (1UL << 11)	// Pin 11 input interrupt event
#define	AE350_GPIO_EVENT_PIN12            (1UL << 12)	// Pin 12 input interrupt event
#define	AE350_GPIO_EVENT_PIN13            (1UL << 13)	// Pin 13 input interrupt event
#define	AE350_GPIO_EVENT_PIN14            (1UL << 14)	// Pin 14 input interrupt event
#define	AE350_GPIO_EVENT_PIN15            (1UL << 15)	// Pin 15 input interrupt event
#define	AE350_GPIO_EVENT_PIN16            (1UL << 16)	// Pin 16 input interrupt event
#define	AE350_GPIO_EVENT_PIN17            (1UL << 17)	// Pin 17 input interrupt event
#define	AE350_GPIO_EVENT_PIN18            (1UL << 18)	// Pin 18 input interrupt event
#define	AE350_GPIO_EVENT_PIN19            (1UL << 19)	// Pin 19 input interrupt event
#define	AE350_GPIO_EVENT_PIN20            (1UL << 20)	// Pin 20 input interrupt event
#define	AE350_GPIO_EVENT_PIN21            (1UL << 21)	// Pin 21 input interrupt event
#define	AE350_GPIO_EVENT_PIN22            (1UL << 22)	// Pin 22 input interrupt event
#define	AE350_GPIO_EVENT_PIN23            (1UL << 23)	// Pin 23 input interrupt event
#define	AE350_GPIO_EVENT_PIN24            (1UL << 24)	// Pin 24 input interrupt event
#define	AE350_GPIO_EVENT_PIN25            (1UL << 25)	// Pin 25 input interrupt event
#define	AE350_GPIO_EVENT_PIN26            (1UL << 26)	// Pin 26 input interrupt event
#define	AE350_GPIO_EVENT_PIN27            (1UL << 27)	// Pin 27 input interrupt event
#define	AE350_GPIO_EVENT_PIN28            (1UL << 28)	// Pin 28 input interrupt event
#define	AE350_GPIO_EVENT_PIN29            (1UL << 29)	// Pin 29 input interrupt event
#define	AE350_GPIO_EVENT_PIN30            (1UL << 30)	// Pin 30 input interrupt event
#define	AE350_GPIO_EVENT_PIN31            (1UL << 31)	// Pin 31 input interrupt event


// Configuration register status
typedef struct
{
	uint32_t pull_opt      : 1;			// Pull option
	uint32_t intr_opt      : 1;			// Interrupt option
	uint32_t debounce_opt  : 1;			// De-bounce option
	uint32_t channel_num   : 6;			// Channel number
} AE350_CFG_STATUS;


// Callback function
typedef void (*AE350_GPIO_SignalEvent_t) (uint32_t event);


// Access structure of the GPIO driver
typedef struct _AE350_DRIVER_GPIO
{
	AE350_DRIVER_VERSION     (*GetVersion)     		(void);																// Get version
	int32_t                  (*Initialize)     		(AE350_GPIO_SignalEvent_t cb_event);								// Initializes
	int32_t                  (*Uninitialize)   		(void);																// Uninitialized
    void                     (*PinWrite)       		(uint32_t pin_num, int32_t val);									// Write a pin
	uint8_t                  (*PinRead)        		(uint32_t pin_num);													// Read a pin
	void                     (*Write)         		(uint32_t mask, uint32_t val);										// Write data
	uint32_t                 (*Read)           		(void);																// Read data
	void                     (*SetDir)         		(uint32_t mask, int32_t dir);										// Set direction
	int32_t                  (*Control)        		(uint32_t mode, uint32_t mask, uint32_t clksel, uint32_t scale);	// Control
	int32_t					 (*GetStatus)			(AE350_CFG_STATUS* cfg_info);										// Get status
} const AE350_DRIVER_GPIO;


#endif /* __DRIVER_GPIO_H__ */
