/*
 * ******************************************************************************************
 * File		: rtc_ae350.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: RTC driver definitions
 * ******************************************************************************************
 */


// Includes ---------------------------------------------------------------------------------
#include "rtc_ae350.h"


// Definitions ------------------------------------------------------------------------------

#define AE350_RTC_DRV_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if (!DRV_RTC)
	#error "RTC not enabled in config.h!"
#endif

// Driver version
static const AE350_DRIVER_VERSION rtc_driver_version =
{
	AE350_RTC_API_VERSION,
	AE350_RTC_DRV_VERSION
};

// RTC info
RTC_INFO g_rRtc_Info =
{
	NULL,
	{0},
	{0},
	AE350_POWER_OFF
};


// Get version
AE350_DRIVER_VERSION rtc_get_version(void)
{
	return rtc_driver_version;
}

// Initializes RTC
int32_t rtc_initialize(AE350_RTC_SignalEvent_t cb_event)
{
	uint32_t Tmp_C = 0, Tmp_S;

	Tmp_C = DEV_RTC->CTRL;
	Tmp_S = DEV_RTC->STATUS;

	// Disable all interrupt and clear interrupt status
	DEV_RTC->CTRL = (Tmp_C & (~ BITS(0,7)));
	DEV_RTC->STATUS = (Tmp_S | BITS(2,7));

	// Priority must be set > 0 to trigger the interrupt
	__nds__plic_set_priority(IRQ_RTCPERIOD_SOURCE, 1);
	__nds__plic_set_priority(IRQ_RTCALARM_SOURCE, 1);

	// Enable PLIC interrupt RTCPERIOD / RTCALARM source
	__nds__plic_enable_interrupt(IRQ_RTCPERIOD_SOURCE);
	__nds__plic_enable_interrupt(IRQ_RTCALARM_SOURCE);

	// Enable the Machine-External bit in MIE
	set_csr(NDS_MIE, MIP_MEIP);

	// Enable GIE
	set_csr(NDS_MSTATUS, MSTATUS_MIE);

	g_rRtc_Info.Sig_Evt_CB = cb_event;
	g_rRtc_Info.Pwr_State = AE350_POWER_FULL;

	return AE350_DRIVER_OK;
}

// Uninitialized
int32_t rtc_uninitialize(void)
{
	uint32_t Tmp_C = 0, Tmp_S;

	Tmp_C = DEV_RTC->CTRL;
	Tmp_S = DEV_RTC->STATUS;

	// Disable all interrupt and clear interrupt status
	DEV_RTC->CTRL = (Tmp_C & (~ BITS(0,7)));
	DEV_RTC->STATUS = (Tmp_S | BITS(2,7));

	// Enable PLIC interrupt RTCPERIOD / RTCALARM source
	__nds__plic_disable_interrupt(IRQ_RTCPERIOD_SOURCE);
	__nds__plic_disable_interrupt(IRQ_RTCALARM_SOURCE);

	g_rRtc_Info.Sig_Evt_CB = NULL;
	g_rRtc_Info.Pwr_State = AE350_POWER_OFF;

	return AE350_DRIVER_OK;
}

// Power control
int32_t rtc_power_control(AE350_POWER_STATE state)
{
	g_rRtc_Info.Pwr_State = state;

	switch (state)
	{
	case AE350_POWER_OFF:
		break;
	case AE350_POWER_LOW:
		break;
	case AE350_POWER_FULL:
		break;
	default:
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	return AE350_DRIVER_OK;
}

// Set time
int32_t rtc_set_time(AE350_RTC_TIME* stime)
{
	uint32_t Tmp_C = 0, Tmp_S;

	Tmp_S = DEV_RTC->STATUS;

	if (g_rRtc_Info.Pwr_State != AE350_POWER_FULL)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	do
	{
		Tmp_S = DEV_RTC->STATUS;
	// Wait until the WriteDone field of the Status Register equals 1
	} while (!(Tmp_S & STATUS_WRITEDONE));

	if ((stime->day >= (0x1 << RTC_DAY_BITS)) ||
		(stime->hour >= 24) ||
		(stime->min >= 60)  ||
		(stime->sec >= 60))
	{
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	// Keep the current set time for reference
	g_rRtc_Info.Set_Rtc_Time.day = stime->day;
	g_rRtc_Info.Set_Rtc_Time.hour = stime->hour;
	g_rRtc_Info.Set_Rtc_Time.min = stime->min;
	g_rRtc_Info.Set_Rtc_Time.sec = stime->sec;

	Tmp_C = (stime->day << 17) | (stime->hour << 12) | (stime->min << 6) | (stime->sec << 0);

	// Set to RTC HW IP
	DEV_RTC->CNTR = Tmp_C;

	return AE350_DRIVER_OK;
}

// Get time
int32_t rtc_get_time(AE350_RTC_TIME* stime)
{
	uint32_t Tmp = 0;

	if (g_rRtc_Info.Pwr_State != AE350_POWER_FULL)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	// Get from RTC HW IP
	Tmp = DEV_RTC->CNTR;

	// Return the current time queried from HW
	stime->day = ((Tmp & CNTR_DAY) >> 17);
	stime->hour = ((Tmp & CNTR_HOUR) >> 12);
	stime->min = ((Tmp & CNTR_MIN) >> 6);
	stime->sec = ((Tmp & CNTR_SEC) >> 0);

	if ((stime->day >= (0x1 << RTC_DAY_BITS)) ||
		(stime->hour >= 24) ||
		(stime->min >= 60)  ||
		(stime->sec >= 60))
	{
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	return AE350_DRIVER_OK;
}

// Set alarm
int32_t rtc_set_alarm(AE350_RTC_ALARM* salarm)
{
	uint32_t Tmp_C = 0, Tmp_S;

	Tmp_S = DEV_RTC->STATUS;

	if (g_rRtc_Info.Pwr_State != AE350_POWER_FULL)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	do
	{
		Tmp_S = DEV_RTC->STATUS;
	// Wait until the WriteDone field of the Status Register equals 1
	} while(!(Tmp_S & STATUS_WRITEDONE));

	if ((salarm->hour >= 24) ||
		(salarm->min >= 60)  ||
		(salarm->sec >= 60))
	{
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	// Keep the current alarm for reference
	g_rRtc_Info.Set_Rtc_Alarm.hour = salarm->hour;
	g_rRtc_Info.Set_Rtc_Alarm.min = salarm->min;
	g_rRtc_Info.Set_Rtc_Alarm.sec = salarm->sec;

	Tmp_C = (salarm->hour << 12) | (salarm->min << 6) | (salarm->sec << 0);

	// Set to RTC HW IP
	DEV_RTC->ALARM = Tmp_C;

	return AE350_DRIVER_OK;
}

// Get alarm
int32_t rtc_get_alarm(AE350_RTC_ALARM* salarm)
{
	uint32_t Tmp = 0;

	if (g_rRtc_Info.Pwr_State != AE350_POWER_FULL)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	// Get from RTC HW IP
	Tmp = DEV_RTC->ALARM;

	// Return the current alarm queried from HW
	salarm->hour = ((Tmp & ALARM_HOUR) >> 12);
	salarm->min = ((Tmp & ALARM_MIN) >> 6);
	salarm->sec = ((Tmp & ALARM_SEC) >> 0);

	if ((salarm->hour >= 24) ||
		(salarm->min >= 60)  ||
		(salarm->sec >= 60))
	{
		return AE350_DRIVER_ERROR_PARAMETER;
	}

	return AE350_DRIVER_OK;
}

// Control
int32_t rtc_control(uint32_t control, uint32_t arg)
{
	AE350_RTC_CTRL_ID eCtrlId = (AE350_RTC_CTRL_ID)control;
	uint32_t Tmp_C = 0, Tmp_S;

	Tmp_C = DEV_RTC->CTRL;
	Tmp_S = DEV_RTC->STATUS;

	if (g_rRtc_Info.Pwr_State != AE350_POWER_FULL)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	do
	{
		Tmp_S = DEV_RTC->STATUS;
	// Wait until the WriteDone field of the Status Register equals 1
	} while (!(Tmp_S & STATUS_WRITEDONE));

	switch (eCtrlId)
	{
	case AE350_RTC_CTRL_NONE:
		break;
	case AE350_RTC_CTRL_EN:
		if (arg == 1)
		{
			// RTC ON
			DEV_RTC->CTRL = (Tmp_C | CTRL_RTC_EN);
		}
		else if (arg == 0)
		{
			// RTC OFF
			DEV_RTC->CTRL = (Tmp_C & ~CTRL_RTC_EN);
		}

		break;

	case AE350_RTC_CTRL_ALARM_WAKEUP:
		if (arg == 1)
		{
			// INT ON
			DEV_RTC->CTRL = (Tmp_C | CTRL_ALARM_WAKEUP);
		}
		else if (arg == 0)
		{
			// INT OFF
			DEV_RTC->CTRL = (Tmp_C & ~CTRL_ALARM_WAKEUP);
		}

		break;

	case AE350_RTC_CTRL_ALARM_INT:
		// Write 1 clear
		DEV_RTC->STATUS = (Tmp_S | STATUS_ALARM_INT);

		if (arg == 1)
		{
			// INT ON
			DEV_RTC->CTRL = (Tmp_C | CTRL_ALARM_INT);
		}
		else if (arg == 0)
		{
			// INT OFF
			DEV_RTC->CTRL = (Tmp_C & ~CTRL_ALARM_INT);
		}

		break;

	case AE350_RTC_CTRL_DAY_INT:
		// Write 1 clear
		DEV_RTC->STATUS = (Tmp_S | STATUS_DAY);

		if (arg == 1)
		{
			// INT ON
			DEV_RTC->CTRL = (Tmp_C | CTRL_DAY);
		}
		else if (arg == 0)
		{
			// INT OFF
			DEV_RTC->CTRL = (Tmp_C & ~CTRL_DAY);
		}

		break;

	case AE350_RTC_CTRL_HOUR_INT:
		// Write 1 clear
		DEV_RTC->STATUS = (Tmp_S | STATUS_HOUR);

		if (arg == 1)
		{
			// INT ON
			DEV_RTC->CTRL = (Tmp_C | CTRL_HOUR);
		}
		else if (arg == 0)
		{
			// INT OFF
			DEV_RTC->CTRL = (Tmp_C & ~CTRL_HOUR);
		}

		break;

	case AE350_RTC_CTRL_MIN_INT:
		// Write 1 clear
		DEV_RTC->STATUS = (Tmp_S | STATUS_MIN);

		if (arg == 1)
		{
			// INT ON
			DEV_RTC->CTRL = (Tmp_C | CTRL_MIN);
		}
		else if (arg == 0)
		{
			// INT OFF
			DEV_RTC->CTRL = (Tmp_C & ~CTRL_MIN);
		}

		break;

	case AE350_RTC_CTRL_SEC_INT:
		// Write 1 clear
		DEV_RTC->STATUS = (Tmp_S | STATUS_SEC);

		if (arg == 1)
		{
			// INT ON
			DEV_RTC->CTRL = (Tmp_C | CTRL_SEC);
		}
		else if (arg == 0)
		{
			// INT OFF
			DEV_RTC->CTRL = (Tmp_C & ~CTRL_SEC);
		}

		break;

	case AE350_RTC_CTRL_HSEC_INT:
		// Write 1 clear
		DEV_RTC->STATUS = (Tmp_S | STATUS_HSEC);

		if (arg == 1)
		{
			// INT ON
			DEV_RTC->CTRL = (Tmp_C | CTRL_HSEC);
		}
		else if (arg == 0)
		{
			// INT OFF
			DEV_RTC->CTRL = (Tmp_C & ~CTRL_HSEC);
		}

		break;

	default:
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	return AE350_DRIVER_OK;
}

// Get status
AE350_RTC_STATUS rtc_get_status(void)
{
	AE350_RTC_STATUS  rRtc_Status;

	rRtc_Status.Status = DEV_RTC->STATUS;

	return rRtc_Status;
}

// Event
void rtc_signal_event(uint32_t event)
{
	// Invoke callback function
	if (g_rRtc_Info.Sig_Evt_CB)
	{
		g_rRtc_Info.Sig_Evt_CB(event);
	}
}

// RTC period interrupt handler
void rtc_period_irq_handler(void)
{
	uint32_t Evt_Id = 0;
	uint32_t Tmp_S, Tmp_W;

	// Only check day/hour/minute/second/half-second interrupt, not include alarm interrupt
	Tmp_W = Tmp_S = (DEV_RTC->STATUS & BITS(3,7));

	// Write 1 clear all interrupt status
	DEV_RTC->STATUS = Tmp_W;

	do
	{
		// Day interrupt
		if (Tmp_S & STATUS_DAY)
		{
			Evt_Id |= AE350_RTC_EVENT_DAY_INT;
		}
		// Hour interrupt
		if (Tmp_S & STATUS_HOUR)
		{
			Evt_Id |= AE350_RTC_EVENT_HOUR_INT;
		}
		// Minute interrupt
		if (Tmp_S & STATUS_MIN)
		{
			Evt_Id |= AE350_RTC_EVENT_MIN_INT;
		}
		// Second interrupt
		if (Tmp_S & STATUS_SEC)
		{
			Evt_Id |= AE350_RTC_EVENT_SEC_INT;
		}
		// Half second interrupt
		if (Tmp_S & STATUS_HSEC)
		{
			Evt_Id |= AE350_RTC_EVENT_HSEC_INT;
		}

		rtc_signal_event(Evt_Id);
	} while (0);
}

// RTC alarm interrupt handler
void rtc_alarm_irq_handler(void)
{
	uint32_t Evt_Id = 0;
	uint32_t Tmp_S, Tmp_W;

	// Only check alarm interrupt
	Tmp_W = Tmp_S = (DEV_RTC->STATUS & BIT(2));

	// Write 1 clear all interrupt status
	DEV_RTC->STATUS = Tmp_W;

	do
	{
		if (Tmp_S & STATUS_ALARM_INT)
		{
			Evt_Id |= AE350_RTC_EVENT_ALARM_INT;
		}

		rtc_signal_event(Evt_Id);
	} while (0);
}


// RTC driver control block
AE350_DRIVER_RTC Driver_RTC =
{
	rtc_get_version,		// Get version
	rtc_initialize,			// Initializes
	rtc_uninitialize,		// Uninitialized
	rtc_power_control,		// Power control
	rtc_set_time,			// Set time
	rtc_get_time,			// Get time
	rtc_set_alarm,			// Set alarm
	rtc_get_alarm,			// Get alarm
	rtc_control,			// Control
	rtc_get_status			// Get status
};
