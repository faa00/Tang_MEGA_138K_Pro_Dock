/*
 * ******************************************************************************************
 * File		: gpio_ae350.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: GPIO driver definitions
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "gpio_ae350.h"


// Variables  -------------------------------------------------------------------------------

#define AE350_GPIO_DRV_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if ((!DRV_GPIO))
	#error "GPIO not enabled in config.h!"
#endif

// Driver version
static const AE350_DRIVER_VERSION gpio_driver_version = {AE350_GPIO_API_VERSION, AE350_GPIO_DRV_VERSION};

static GPIO_INFO gpio_info = {0};


// Definitions  -----------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------
// Functions  : Set GPIO interrupt mode
// Parameters : intr_mode : GPIO interrupt mode
//              mask      : GPIO ports to be used
// ------------------------------------------------------------------------------------------
static void gpio_set_intr_mode(uint32_t intr_mode, uint32_t mask)
{
	uint32_t i;

	for (i = 0; i < GPIO_MAX_PIN_NUM; i++)
	{
		if (mask & (0x1 << i))
		{
			if ((i >= 0) & (i < 8))
			{
				DEV_GPIO->INTRMODE0 &= ~(7 << (i * 4));					// Clear pin-0~7's interrupt mode
				DEV_GPIO->INTRMODE0 |= (intr_mode << (i * 4));			// Set pin-0~7's interrupt mode
			}
			else if ((i >= 8) & (i < 16))
			{
				DEV_GPIO->INTRMODE1 &= ~(7 << ((i-8) * 4));				// Clear pin-8~15's interrupt mode
				DEV_GPIO->INTRMODE1 |= (intr_mode << ((i-8) * 4));		// Set pin-8~15's interrupt mode
			}
			else if ((i >= 16) & (i < 24))
			{
				DEV_GPIO->INTRMODE2 &= ~(7 << ((i-16) * 4));			// Clear pin-16~23's interrupt mode
				DEV_GPIO->INTRMODE2 |= (intr_mode << ((i-16) * 4));		// Set pin-16~23's interrupt mode
			}
			else if ((i >= 24) & (i < 32))
			{
				DEV_GPIO->INTRMODE3 &= ~(7 << ((i-24) * 4));			// Clear pin-24~31's interrupt mode
				DEV_GPIO->INTRMODE3 |= (intr_mode << ((i-24) * 4));		// Set pin-24~31's interrupt mode
			}
		}
	}
}

// ------------------------------------------------------------------------------------------
// Functions : Get GPIO driver version
// Returns   : GPIO driver version
// ------------------------------------------------------------------------------------------
AE350_DRIVER_VERSION gpio_get_version(void)
{
	return gpio_driver_version;
}

// ------------------------------------------------------------------------------------------
// Functions  : Initializes GPIO
// Parameters : cb_event : GPIO ports interrupt event
// Returns    : General return codes
// ------------------------------------------------------------------------------------------
int32_t gpio_initialize(AE350_GPIO_SignalEvent_t cb_event)
{
    // Disable all interrupts
	DEV_GPIO->INTREN = 0;

    // Write 1 to clear interrupt status
	DEV_GPIO->INTRSTATUS = 0xFFFFFFFF;

   // Priority must be set > 0 to trigger the interrupt
	__nds__plic_set_priority(IRQ_GPIO_SOURCE, 1);

	// Enable PLIC interrupt GPIO source
	__nds__plic_enable_interrupt(IRQ_GPIO_SOURCE);

	// Enable the Machine-External bit in MIE
	set_csr(NDS_MIE, MIP_MEIP);

	// Enable GIE
	set_csr(NDS_MSTATUS, MSTATUS_MIE);

	// Interrupt event
	gpio_info.cb_event = cb_event;

	return AE350_DRIVER_OK;
}

// ------------------------------------------------------------------------------------------
// Functions : Uninitialized GPIO
// Returns   : General return codes
// ------------------------------------------------------------------------------------------
int32_t gpio_uninitialize(void)
{
	// Disable all interrupts
	DEV_GPIO->INTREN = 0;

    // Write 1 to clear interrupt status
	DEV_GPIO->INTRSTATUS = 0xFFFFFFFF;

	// Disable PLIC interrupt GPIO source
	__nds__plic_disable_interrupt(IRQ_GPIO_SOURCE);

	gpio_info.cb_event = NULL;

	return AE350_DRIVER_OK;
}

// ------------------------------------------------------------------------------------------
// Functions  : Write a value into a pin
// Parameters : pin_num : A GPIO pin
//              val     : The value written into the pin
// ------------------------------------------------------------------------------------------
void gpio_pinwrite(uint32_t pin_num, int32_t val)
{
	val ? (DEV_GPIO->DOUTSET = 1UL << pin_num) : (DEV_GPIO->DOUTCLEAR = (1UL << pin_num));
}

// ------------------------------------------------------------------------------------------
// Functions  : Read a value from a pin
// Parameters : pin_num : A GPIO pin
// Returns    : The value read from the pin
// ------------------------------------------------------------------------------------------
uint8_t gpio_pinread(uint32_t pin_num)
{
	return ((DEV_GPIO->DATAIN & (1UL << pin_num)) ? (1) : (0));
}

// ------------------------------------------------------------------------------------------
// Functions  : Write a value into GPIO ports
// Parameters : mask : GPIO ports to be used
//              val  : The value written into GPIO ports
// ------------------------------------------------------------------------------------------
void gpio_write(uint32_t mask, uint32_t val)
{
	val ? (DEV_GPIO->DOUTSET = mask) : (DEV_GPIO->DOUTCLEAR = mask);
}

// ------------------------------------------------------------------------------------------
// Functions : Read a value from GPIO ports
// Returns   : The value read from GPIO ports
// ------------------------------------------------------------------------------------------
uint32_t gpio_read(void)
{
	return (DEV_GPIO->DATAIN);
}

// ------------------------------------------------------------------------------------------
// Functions  : Set GPIO direction
// Parameters : mask : GPIO ports to be used
//              dir  : Direction input/output
// ------------------------------------------------------------------------------------------
void gpio_setdir(uint32_t mask, int32_t dir)
{
	dir ? (DEV_GPIO->CHANNELDIR |= mask) : (DEV_GPIO->CHANNELDIR &= ~mask);
}

// ------------------------------------------------------------------------------------------
// Functions  : Control command for GPIO interrupt, pull, de-bounce
// Parameters : mode : Interrupt, pull, de-bounce mode or enable/disable
//              mask  : GPIO ports to be used
//				clksel : De-bounce clock source selection (extclk/pclk)
//				scale  : GPIO pre-scale base
// Returns    : General return code
// ------------------------------------------------------------------------------------------
int32_t gpio_control(uint32_t mode, uint32_t mask, uint32_t clksel, uint32_t scale)
{
	/* Interrupt --------------------------------------------------------------- */
	// Set interrupt mode
	if (mode & AE350_GPIO_SET_INTR_HIGH_LEVEL)
	{
		gpio_set_intr_mode(GPIO_INTR_HIGH_LEVEL, mask);
	}
	else if (mode & AE350_GPIO_SET_INTR_LOW_LEVEL)
	{
		gpio_set_intr_mode(GPIO_INTR_LOW_LEVEL, mask);
	}
	else if (mode & AE350_GPIO_SET_INTR_NEGATIVE_EDGE)
	{
		gpio_set_intr_mode(GPIO_INTR_NEGATIVE_EDGE, mask);
	}
	else if (mode & AE350_GPIO_SET_INTR_POSITIVE_EDGE)
	{
		gpio_set_intr_mode(GPIO_INTR_POSITIVE_EDGE, mask);
	}
	else if (mode & AE350_GPIO_SET_INTR_DUAL_EDGE)
	{
		gpio_set_intr_mode(GPIO_INTR_DUAL_EDGE, mask);
	}

	// Interrupt disable/enable
	if (mode & AE350_GPIO_INTR_DISABLE)
	{
		DEV_GPIO->INTREN &= ~mask;
	}
	else if (mode & AE350_GPIO_INTR_ENABLE)
	{
		DEV_GPIO->INTREN |= mask;
	}

	// Interrupt clear status
	if (mode & AE350_GPIO_INTR_CLEAR)
	{
		DEV_GPIO->INTRSTATUS |= mask;
	}


	/* Pull ------------------------------------------------------------------- */
	// Pull disable/enable
	if (mode & AE350_GPIO_PULL_DISABLE)
	{
		DEV_GPIO->PULLEN &= ~mask;			// 0x0 : disable pull
	}
	else if (mode & AE350_GPIO_PULL_ENABLE)
	{
		DEV_GPIO->PULLEN |= mask;			// 0x1 : enable pull
	}

	// Pull type
	if (mode & AE350_GPIO_SET_PULL_UP)
	{
		DEV_GPIO->PULLTYPE &= ~mask;		// 0x0 : pull up
	}
	else if (mode & AE350_GPIO_SET_PULL_DOWN)
	{
		DEV_GPIO->PULLTYPE |= mask;			// 0x1 : pull down
	}


	/* De-bounce --------------------------------------------------------------- */
	// De-bounce disable/enable
	if (mode & AE350_GPIO_DB_DISABLE)
	{
		DEV_GPIO->DEBOUNCEEN &= ~mask;		// 0x0 : disable de-bounce
	}
	else if (mode & AE350_GPIO_DB_ENABLE)
	{
		DEV_GPIO->DEBOUNCEEN |= mask;		// 0x1 : enable de-bounce
	}

	// De-bounce clock source
	if (mode & AE350_GPIO_SET_DB_CLKSRC)
	{
		DEV_GPIO->DEBOUNCECTRL &= 0x7FFFFFFF;						// Clear [31] to 0
		//clksel : AE350_GPIO_DB_CLKSRC_EXT or AE350_GPIO_DB_CLKSRC_P
		DEV_GPIO->DEBOUNCECTRL |= ((clksel << 31) & 0x80000000);	// Set [31] to clksel (extclk or pclk)
	}

	// De-bounce pre-scale
	if (mode & AE350_GPIO_SET_DB_PRESCALE)
	{
		DEV_GPIO->DEBOUNCECTRL &= 0xFFFFFF00;						// Clear [7:0] to 0
		DEV_GPIO->DEBOUNCECTRL |= (scale & 0x000000FF);				// Set [7:0] to scale
	}

	return AE350_DRIVER_OK;
}

// ------------------------------------------------------------------------------------------
// Functions  : Get GPIO configuration status
// Parameters : cfg_info : Configuration status
// Returns    : General return codes
// ------------------------------------------------------------------------------------------
int32_t gpio_get_status(AE350_CFG_STATUS* cfg_info)
{
	uint32_t cfg_reg = DEV_GPIO->CFG;

	// Get pull option
	cfg_info->pull_opt = (cfg_reg & 0x80000000) >> 31;			// [31] pull option

	// Get interrupt option
	cfg_info->intr_opt = (cfg_reg & 0x40000000) >> 30;			// [30] interrupt option

	// Get de-bounce option
	cfg_info->debounce_opt = (cfg_reg & 0x20000000) >> 29;		// [29] de-bounce option

	// Get channel number
	cfg_info->channel_num = cfg_reg & 0x0000003F;				// [5:0] channel number

	return AE350_DRIVER_OK;
}

// ------------------------------------------------------------------------------------------
// Functions : GPIO interrupt handler
// ------------------------------------------------------------------------------------------
void gpio_irq_handler(void)
{
	// Trigger interrupt by event
	uint32_t status;
	uint32_t i;

	status = DEV_GPIO->INTRSTATUS;

	// Write 1 to clear interrupt status
	DEV_GPIO->INTRSTATUS = status;

	for (i = 0; i < GPIO_MAX_PIN_NUM; i++)
	{
		// If a pin has an interrupt, trigger interrupt event
		if (status & (0x1 << i))
		{
			gpio_info.cb_event((1UL << i));
		}
	}
}


// GPIO driver control block
AE350_DRIVER_GPIO Driver_GPIO =
{
	gpio_get_version,				// Get version
	gpio_initialize,				// Initializes
	gpio_uninitialize,				// Uninitialized
	gpio_pinwrite,					// Write a pin
	gpio_pinread,					// Read a pin
	gpio_write,						// Write data
	gpio_read,						// Read data
	gpio_setdir,					// Set direction
	gpio_control,					// Control
	gpio_get_status					// Status
};
