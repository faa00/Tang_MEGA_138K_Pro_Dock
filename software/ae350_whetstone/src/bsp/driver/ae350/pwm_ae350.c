/*
 * ******************************************************************************************
 * File		: pwm_ae350.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: PWM driver definitions
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "pwm_ae350.h"


// Definitions  -----------------------------------------------------------------------------

#define AE350_PWM_DRV_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if (!DRV_PWM)
	#error "PWM not enabled in config.h!"
#endif

// Driver version
static const AE350_DRIVER_VERSION pwm_driver_version = {AE350_PWM_API_VERSION, AE350_PWM_DRV_VERSION};

// PWM information
static PWM_INFO pwm_info = {0};

// PWM resources
static const PWM_RESOURCES pwm =
{
	{
	  PWM_NUMBER_OF_CHANNELS,  /* PWM channel numbers */
	  0                        /* Signal one step complete event */
	},
	DEV_PIT,
	&pwm_info
};

// Output
static void pwmout(uint8_t ch_num, uint32_t freq, uint32_t duty)
{
	uint32_t period, high_cycle, low_cycle;

	period = pwm.info->clk[ch_num] / freq;
	high_cycle = ((period * duty) / PWM_DUTY_MAX) - 1;
	low_cycle = period - high_cycle - 1;

	// Disable corresponding PWM channel bit
	pwm.reg->CHNEN &= ~(1 << ((ch_num << 2) + 3));

	pwm.reg->CHANNEL[ch_num].RELOAD = (high_cycle << 16) | (low_cycle);

	// Enable corresponding PWM channel bit
	pwm.reg->CHNEN |= (1 << ((ch_num << 2) + 3));
}

/****************************************************************************
  \fn          AE350_DRIVER_VERSION pwm_get_version (void)
  \brief       Get driver version.
  \return      \ref AE350_DRIVER_VERSION
*****************************************************************************/
static AE350_DRIVER_VERSION pwm_get_version(void)
{
	return pwm_driver_version;
}

/*****************************************************************************
  \fn          AE350_PWM_CAPABILITIES pwm_get_capabilities (void)
  \brief       Get driver capabilities
  \return      \ref AE350_PWM_CAPABILITIES
******************************************************************************/
static AE350_PWM_CAPABILITIES pwm_get_capabilities(void)
{
	return pwm.capabilities;
}

/*****************************************************************************
  \fn          int32_t pwm_initialize (AE350_PWM_SignalEvent_t cb_event)
  \brief       Initialize PWM Interface.
  \param[in]   cb_event  Pointer to \ref AE350_PWM_SignalEvent
  \return      \ref execution_status
******************************************************************************/
static int32_t pwm_initialize(AE350_PWM_SignalEvent_t cb_event)
{
	uint32_t ch_num;

	if (pwm.info->flags & PWM_FLAG_INITIALIZED)
	{
		// Driver is already initialized
		return AE350_DRIVER_OK;
	}

	pwm.info->cb_event = cb_event;

	for (ch_num = 0U; ch_num < PWM_NUMBER_OF_CHANNELS; ch_num++)
	{
		pwm.info->clk[ch_num] = 0;
		pwm.info->freq[ch_num] = 0;
		pwm.info->duty[ch_num] = 0;
	}

	pwm.info->flags = PWM_FLAG_INITIALIZED;

	return AE350_DRIVER_OK;
}

/*****************************************************************************
  \fn          int32_t pwm_uninitialize (void)
  \brief       Uninitialized PWM Interface.
  \return      \ref execution_status
******************************************************************************/
static int32_t pwm_uninitialize(void)
{
	// Reset PWM status flags
	pwm.info->flags = 0U;

	pwm.info->cb_event = NULL;

	return AE350_DRIVER_OK;
}

/*****************************************************************************
  \fn          int32_t pwm_power_control (AE350_POWER_STATE state)
  \brief       Control PWM Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
******************************************************************************/
static int32_t pwm_power_control(AE350_POWER_STATE  state)
{
	uint32_t ch_num, ch_mask;

	switch (state)
	{
	case AE350_POWER_OFF:
		for (ch_num = 0U, ch_mask = 0U; ch_num < PWM_NUMBER_OF_CHANNELS; ch_num++)
		{
			if (pwm.info->duty[ch_num])
			{
				ch_mask |= (1 << ((ch_num << 2) + 3));

				pwm.info->clk[ch_num] = 0;
				pwm.info->freq[ch_num] = 0;
				pwm.info->duty[ch_num] = 0;
			}
		}

		// Disable corresponding PWM channel bit
		pwm.reg->CHNEN &= ~(ch_mask);
		pwm.info->flags &= ~PWM_FLAG_POWERED;

		break;

	case AE350_POWER_LOW:
		return AE350_DRIVER_ERROR_UNSUPPORTED;

	case AE350_POWER_FULL:
		if ((pwm.info->flags & PWM_FLAG_INITIALIZED) == 0U)
		{
			return AE350_DRIVER_ERROR;
		}
		if ((pwm.info->flags & PWM_FLAG_POWERED) != 0U)
		{
			return AE350_DRIVER_OK;
		}

		pwm.info->flags = PWM_FLAG_POWERED | PWM_FLAG_INITIALIZED;

		break;

	default:
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	return AE350_DRIVER_OK;
}

/*****************************************************************************
  \fn          int32_t pwm_control (uint32_t control, uint32_t arg)
  \brief       Control PWM Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      common \ref execution_status and driver specific \ref pwm_execution_status
******************************************************************************/
static int32_t pwm_control(uint32_t control, uint32_t arg)
{
	uint32_t ctrl, clk;

	if ((pwm.info->flags & PWM_FLAG_POWERED) == 0U)
	{
		// PWM not powered
		return AE350_DRIVER_ERROR;
	}

	switch (control & AE350_PWM_CONTROL_Msk)
	{
	case AE350_PWM_ACTIVE_CONFIGURE:
		if (arg >= pwm.capabilities.channels)
		{
			// PWM channel is not existed
			return AE350_DRIVER_ERROR;
		}

		ctrl = PIT_CH_CTRL_MODE_PWM;

		// PWM park value
		if (control & AE350_PWM_PARK_HIGH)
		{
			ctrl |= PIT_CH_CTRL_PWMPARK_HIGH;
		}

		// PWM clock source
		if (control & AE350_PWM_CLKSRC_EXTERNAL)
		{
			clk = PWM_EXTCLK_FREQ;
		}
		else
		{
			ctrl |= PIT_CH_CTRL_APBCLK;
			clk = PWM_APBCLK_FREQ;
		}

		pwm.reg->CHANNEL[arg].CTRL = ctrl;
		pwm.info->clk[arg] = clk;

		pwm.info->flags |= PWM_FLAG_CHANNEL_ACTIVED(arg);

		return AE350_DRIVER_OK;

	default:
		// Unsupported command
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}
}

/*****************************************************************************
  \fn          int32_t pwm_set_freq (uint8_t ch_num, uint32_t freq)
  \brief       Set PWM Interface frequency
  \param[in]   ch_num  PWM channel number
  \param[in]   freq    Frequency
  \return      \ref execution_status
******************************************************************************/
static int32_t pwm_set_freq(uint8_t ch_num, uint32_t freq)
{
	if (ch_num >= pwm.capabilities.channels)
	{
		// PWM channel is not existed
		return AE350_DRIVER_ERROR;
	}

	if ((pwm.info->flags & PWM_FLAG_CHANNEL_ACTIVED(ch_num)) == 0U)
	{
		// PWM channel is not activated
		return AE350_DRIVER_ERROR;
	}

	// PWM duty cycle is NOT zero, output is enabled
	if (pwm.info->duty[ch_num])
	{
		pwmout(ch_num, freq, pwm.info->duty[ch_num]);
	}

	pwm.info->freq[ch_num] = freq;

	return AE350_DRIVER_OK;
}

/*****************************************************************************
  \fn          int32_t pwm_output (uint8_t ch_num, uint8_t duty)
  \brief       Output PWM pulse
  \param[in]   ch_num  PWM channel number
  \param[in]   duty    Duty length (0 ~ 255)
  \return      \ref execution_status
******************************************************************************/
static int32_t pwm_output(uint8_t ch_num, uint8_t duty)
{
	if (ch_num >= pwm.capabilities.channels)
	{
		// PWM channel is not existed
		return AE350_DRIVER_ERROR;
	}

	if ((pwm.info->flags & PWM_FLAG_CHANNEL_ACTIVED(ch_num)) == 0U)
	{
		// PWM channel is not activated
		return AE350_DRIVER_ERROR;
	}

	// Overflow Max. duty value (0 ~ 255)
	if (duty > PWM_DUTY_MAX)
	{
		return AE350_DRIVER_ERROR;
	}

	pwm.info->duty[ch_num] = duty;

	if (duty)
	{
		pwmout(ch_num, pwm.info->freq[ch_num], duty);
	}
	else
	{
		// Duty is zero.
		//Disable corresponding PWM channel bit
		pwm.reg->CHNEN &= ~(1 << ((ch_num << 2) + 3));
	}

	return AE350_DRIVER_OK;
}

/*****************************************************************************
  \fn          AE350_PWM_STATUS pwm_get_status (void)
  \brief       Get PWM status.
  \return      PWM status \ref AE350_PWM_STATUS
******************************************************************************/
static AE350_PWM_STATUS pwm_get_status (void)
{
	AE350_PWM_STATUS stat;
	uint32_t ch_num;

	stat.configured = (pwm.info->flags >> 8) & 0xFF;

	for (ch_num = stat.output = 0U; ch_num < PWM_NUMBER_OF_CHANNELS; ch_num++)
	{
		if (pwm.info->duty[ch_num])
		{
			stat.output |= (1 << ch_num);
		}
	}

	return stat;
}


// PWM driver control block
AE350_DRIVER_PWM Driver_PWM =
{
	pwm_get_version,		// Get version
	pwm_get_capabilities,	// Get capabilities
	pwm_initialize,			// Initializes
	pwm_uninitialize,		// Uninitialized
	pwm_power_control,		// Power control
	pwm_control,			// Control
	pwm_set_freq,			// Set the frequency
	pwm_output,				// Start output
	pwm_get_status			// Get status
};
