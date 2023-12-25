/*
 * ******************************************************************************************
 * File		: wdt_ae350.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: WDT driver definitions
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "wdt_ae350.h"


// Definitions  -----------------------------------------------------------------------------

#define AE350_WDT_DRV_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if (!DRV_WDT)
	#error "WDT not enabled in config.h!"
#endif

// Driver version
static const AE350_DRIVER_VERSION wdt_driver_version = { AE350_WDT_API_VERSION, AE350_WDT_DRV_VERSION };

// WDT information
#if (DRV_WDT)
static WDT_INFO wdt_info = {0};
#endif	// DRV_WDT

// WDT resources
static const WDT_RESOURCES wdt =
{
	{
	1,
	1,
	},
	DEV_WDT,
	&wdt_info
};


/****************************************************************************
  \fn          AE350_DRIVER_VERSION wdt_get_version (void)
  \brief       Get driver version.
  \return      \ref AE350_DRIVER_VERSION
*****************************************************************************/
static AE350_DRIVER_VERSION wdt_get_version(void)
{
	return wdt_driver_version;
}

/*****************************************************************************
  \fn          AE350_WDT_CAPABILITIES wdt_get_capabilities (void)
  \brief       Get driver capabilities
  \param[in]   wdt Pointer to WDT resources
  \return      \ref AE350_WDT_CAPABILITIES
******************************************************************************/
static AE350_WDT_CAPABILITIES wdt_get_capabilities(void)
{
	return wdt.capabilities;
}

/*****************************************************************************
  \fn          int32_t wdt_initialize (AE350_WDT_SignalEvent_t cb_event)
  \brief       Initialize WDT Interface.
  \param[in]   cb_event  Pointer to \ref AE350_WDT_SignalEvent
  \return      \ref execution_status
******************************************************************************/
static int32_t wdt_initialize(AE350_WDT_SignalEvent_t cb_event)
{
	if (wdt.info->flags & WDT_FLAG_INITIALIZED)
	{
		// Driver is already initialized
		return AE350_DRIVER_OK;
	}

	wdt.info->cb_event = cb_event;
	wdt.info->flags = WDT_FLAG_INITIALIZED;

	return AE350_DRIVER_OK;
}

/*****************************************************************************
  \fn          int32_t wdt_uninitialize (void)
  \brief       Uninitialized WDT Interface.
  \return      \ref execution_status
******************************************************************************/
static int32_t wdt_uninitialize(void)
{
	// Reset WDT status flags
	wdt.info->flags = 0U;
	wdt.info->cb_event = NULL;

	return AE350_DRIVER_OK;
}

/*****************************************************************************
  \fn          void AE350_WDT_RestartTimer (void)
  \brief       Restart WDT Timer
  \return      none
******************************************************************************/
static void wdt_restart_timer(void)
{
	wdt.reg->WREN = WDT_WREN_NUM;
	wdt.reg->RESTART = WDT_RESTART_NUM;
}

/******************************************************************************
  \fn          void AE350_WDT_ClearIrq (void)
  \brief       Clear WDT IRQ Expired status
  \return      none
*******************************************************************************/
static void wdt_clear_irq_status(void)
{
	wdt.reg->ST = WDT_ST_INTEXPIRED_CLR;
}

/******************************************************************************
  \fn          int32_t wdt_control (uint32_t control, uint32_t arg)
  \brief       Control WDT Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      common \ref execution_status and driver specific
  	  	  	  	  	  \ref wdt_execution_status
*******************************************************************************/
static int32_t wdt_control(uint32_t control, uint32_t arg)
{
	if ((wdt.info->flags & WDT_FLAG_INITIALIZED) == 0U)
	{
		// WDT is not initialized
		return AE350_DRIVER_ERROR;
	}

	uint32_t time_period = arg;
	uint32_t ctrl = 0;

	// Enable WDT RST/INT stages.
	ctrl |= (WDT_CTRL_RSTEN | WDT_CTRL_INTEN);
	// Set RST time to minimum.
	ctrl |= WDT_CTRL_RSTTIME_POW_2_7;

	switch (control & AE350_WDT_CLKSRC_Msk)
	{
		case AE350_WDT_CLKSRC_APB:
			ctrl |= WDT_CTRL_APBCLK;
			break;
		case AE350_WDT_CLKSRC_EXTERNAL:
			ctrl |= WDT_CTRL_EXTCLK;
			break;
		default:
			break;
	}

	switch (time_period)
	{
		case AE350_WDT_TIME_POW_2_6:
			ctrl |= WDT_CTRL_INTTIME_POW_2_6;
			break;
		case AE350_WDT_TIME_POW_2_8:
			ctrl |= WDT_CTRL_INTTIME_POW_2_8;
			break;
		case AE350_WDT_TIME_POW_2_10:
			ctrl |= WDT_CTRL_INTTIME_POW_2_10;
			break;
		case AE350_WDT_TIME_POW_2_11:
			ctrl |= WDT_CTRL_INTTIME_POW_2_11;
			break;
		case AE350_WDT_TIME_POW_2_12:
			ctrl |= WDT_CTRL_INTTIME_POW_2_12;
			break;
		case AE350_WDT_TIME_POW_2_13:
			ctrl |= WDT_CTRL_INTTIME_POW_2_13;
			break;
		case AE350_WDT_TIME_POW_2_14:
			ctrl |= WDT_CTRL_INTTIME_POW_2_14;
			break;
		case AE350_WDT_TIME_POW_2_15:
			ctrl |= WDT_CTRL_INTTIME_POW_2_15;
			break;
		default:
			return AE350_WDT_ERROR_TIME_PREIOD;
	}

	wdt_restart_timer();
	wdt_clear_irq_status();

	wdt.reg->WREN = WDT_WREN_NUM;
	wdt.reg->CTRL = ctrl;

	return AE350_DRIVER_OK;
}

/******************************************************************************
  \fn          void wdt_enable (void)
  \brief       Enable WDT Timer
  \return      none
*******************************************************************************/
static void wdt_enable(void)
{
	uint32_t ctrl = wdt.reg->CTRL;

	ctrl |= WDT_CTRL_EN;
	wdt.reg->WREN = WDT_WREN_NUM;
	wdt.reg->CTRL = ctrl;
}

/******************************************************************************
  \fn          void wdt_disable (void)
  \brief       Disable WDT Timer
  \return      none
*******************************************************************************/
static void wdt_disable(void)
{
	uint32_t ctrl = wdt.reg->CTRL;

	ctrl &= ~WDT_CTRL_EN;
	wdt.reg->WREN = WDT_WREN_NUM;
	wdt.reg->CTRL = ctrl;
}

/*******************************************************************************
  \fn          AE350_WDT_STATUS wdt_get_status (void)
  \brief       Get WDT status.
  \return      WDT status \ref AE350_WDT_STATUS
********************************************************************************/
static AE350_WDT_STATUS wdt_get_status(void)
{
	AE350_WDT_STATUS status = {0};

	uint32_t status_reg = wdt.reg->ST;

	if( status_reg & WDT_ST_INTEXPIRED )
	{
		status.irq_expired = 1;
	}

	return status;
}


// WDT driver control block
AE350_DRIVER_WDT Driver_WDT =
{
	wdt_get_version,			// Get version
	wdt_get_capabilities,		// Get capabilities
	wdt_initialize,				// Initializes
	wdt_uninitialize,			// Uninitialized
	wdt_control,				// Control
	wdt_enable,					// Enable
	wdt_disable,				// Disable
	wdt_restart_timer,			// Restart timer
	wdt_clear_irq_status,		// Clear interrupt status
	wdt_get_status				// Get status
};


/* NMI interrupt handler */
void nmi_handler(void)
{
	AE350_WDT_STATUS status = wdt_get_status();

	if( status.irq_expired )
	{
		wdt.info->cb_event(0);
	}

	// Wait WDT reset signal to reset system.
	while(1);
}
