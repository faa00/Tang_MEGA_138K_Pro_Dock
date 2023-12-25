/*
 * ******************************************************************************************
 * File		: pit_ae350.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: PIT as simple timer driver definitions
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "pit_ae350.h"


// Definitions  -----------------------------------------------------------------------------

#define AE350_PIT_DRV_VERSION AE350_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if (!DRV_PIT)
	#error "PIT not enabled in config.h!"
#endif

// Driver version
static const AE350_DRIVER_VERSION pit_timer_driver_version = {AE350_PIT_API_VERSION, AE350_PIT_DRV_VERSION};


// Get version
static AE350_DRIVER_VERSION pit_timer_get_version(void)
{
	return pit_timer_driver_version;
}

static void pit_timer_init_irqchip(void)
{
	/* Set PIT priority to lowest */
	/* Priority must be set > 0 to trigger the interrupt */
	__nds__plic_set_priority(IRQ_PIT_SOURCE, 1);

	/* Enable HW# (PIT) */
	__nds__plic_enable_interrupt(IRQ_PIT_SOURCE);

	/* Enable the Machine-External bit in MIE */
	set_csr(NDS_MIE, MIP_MEIP);

	/* Enable GIE (Machine Mode) */
	set_csr(NDS_MSTATUS, MSTATUS_MIE);
}

// Initializes PIT as simple timer
static int32_t pit_timer_init(void)
{
	/* Disable PIT */
	DEV_PIT->CHNEN = 0;

	/* Set PIT control mode */
	DEV_PIT->CHANNEL[0].CTRL = (PIT_CHNCTRL_TMR_32BIT | PIT_CHNCTRL_CLK_PCLK);
	DEV_PIT->CHANNEL[1].CTRL = (PIT_CHNCTRL_TMR_32BIT | PIT_CHNCTRL_CLK_PCLK);
	DEV_PIT->CHANNEL[2].CTRL = (PIT_CHNCTRL_TMR_32BIT | PIT_CHNCTRL_CLK_PCLK);
	DEV_PIT->CHANNEL[3].CTRL = (PIT_CHNCTRL_TMR_32BIT | PIT_CHNCTRL_CLK_PCLK);

	/* Clear and disable interrupt */
	DEV_PIT->INTEN = 0;
	DEV_PIT->INTST = -1;

	pit_timer_init_irqchip();

	return AE350_DRIVER_OK;
}

// Read time
static uint32_t pit_timer_read(uint32_t tmr)
{
	if (tmr >= PIT_NUMBER_OF_CHANNELS)
	{
		return 0;
	}

	return (DEV_PIT->CHANNEL[tmr].RELOAD - DEV_PIT->CHANNEL[tmr].COUNTER);
}

// Control
static int32_t pit_timer_control(uint32_t mode, uint32_t tmr)
{
	if (tmr >= PIT_NUMBER_OF_CHANNELS)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	if (mode & AE350_PIT_TIMER_START)
	{
		// Start timer
		DEV_PIT->CHNEN |= (0x1 << (4 * (tmr)));
	}
	else if (mode & AE350_PIT_TIMER_STOP)
	{
		// Stop timer
		DEV_PIT->CHNEN &= ~(0x1 << (4 * (tmr)));
	}
	else if (mode & AE350_PIT_TIMER_INTR_ENABLE)
	{
		// Enable interrupt
		DEV_PIT->INTEN |= (0x1 << (4 * (tmr)));
	}
	else if (mode & AE350_PIT_TIMER_INTR_DISABLE)
	{
		// Disable interrupt
		DEV_PIT->INTEN &= ~(0x1 << (4 * (tmr)));
	}
	else if (mode & AE350_PIT_TIMER_INTR_CLEAR)
	{
		// Clear interrupt
		DEV_PIT->INTST = 0xF << (4 * (tmr));
	}

	return AE350_DRIVER_OK;
}

// Set period
static int32_t pit_timer_set_period(uint32_t tmr, uint32_t period)
{
	if (tmr >= PIT_NUMBER_OF_CHANNELS)
	{
		return AE350_DRIVER_ERROR_UNSUPPORTED;
	}

	DEV_PIT->CHANNEL[tmr].RELOAD = period;

	return AE350_DRIVER_OK;
}

// Get status
static uint32_t pit_timer_get_status(uint32_t tmr)
{
	return (DEV_PIT->INTST & (0xF << (4 * (tmr))));
}

// Get (m)s tick
static uint32_t pit_timer_get_tick(uint32_t mode, uint32_t sec)
{
	if (mode & AE350_PIT_TIMER_SEC_TICK)
	{
		// Second
		return sec * PCLKFREQ;
	}
	else if (mode & AE350_PIT_TIMER_MSEC_TICK)
	{
		// Microsecond
		return sec * (PCLKFREQ / 1000);
	}
	else
	{
		return 0;
	}
}


// PIT as simple timer driver control block
AE350_DRIVER_PIT Driver_PIT =
{
	pit_timer_get_version,		// Get version
	pit_timer_init,				// Initializes
	pit_timer_read,				// Read time
	pit_timer_control,			// Control
	pit_timer_set_period,		// Set period
	pit_timer_get_status,		// Get status
	pit_timer_get_tick			// Get tick
};
