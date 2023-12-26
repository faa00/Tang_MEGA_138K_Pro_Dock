/*
 * ******************************************************************************************
 * File		: demo_wdt_pit.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: WDT and PIT as simple timer demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This example program shows the usage of WDT and PIT as simple timer interface for resetting
 * the lock-up system. It demonstrates WDT to reset system when WDT takes a timeout.
 *
 * Scenario:
 *
 * WDT is initialized and timer will periodically restart WDT.
 * After timer restart WDT 10 times, we will stop timer.
 * Then, the WDT timeout occurs, so it resets the system.
 *
 * If WDT has IRQ stage, then program receives NMI interrupt at timeout.
 * It prints error message, and waits for system reset later.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running WDT and PIT as simple timer demo
#if RUN_DEMO_WDT_PIT

// ************ Includes *********** //
#include "Driver_WDT.h"
#include "Driver_PIT.h"
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********** //
extern AE350_DRIVER_WDT Driver_WDT;		// WDT
extern AE350_DRIVER_PIT Driver_PIT;		// PIT as simple timer

volatile unsigned int counter = 0;
AE350_WDT_CAPABILITIES drv_capabilities;

/* PIT as simple timer functions by PIT as simple timer driver */
#define PIT_TIMER1 0
#define PIT_TIMER2 1
#define PIT_TIMER3 2
#define PIT_TIMER4 3


// Configure and start timer
static void set_timer_irq_period(unsigned int timer, int msec)
{
	static AE350_DRIVER_PIT *PITdrv = &Driver_PIT;

    unsigned int period = PITdrv->GetTick(AE350_PIT_TIMER_MSEC_TICK, msec);

    PITdrv->SetPeriod(timer, period);
    PITdrv->Control(AE350_PIT_TIMER_INTR_ENABLE, timer);
    PITdrv->Control(AE350_PIT_TIMER_START, timer);
}

/* Interrupt handlers */
void wdt_handler (uint32_t event)
{
    printf("WDT activated\r\n");
}

// PIT as simple timer interrupt handler
// This is pit_irq_handler
void pit_timer_irq_handler(void)
{
	static AE350_DRIVER_PIT *PITdrv = &Driver_PIT;	// PIT as simple timer

	// Clear PIT as simple timer interrupt
	PITdrv->Control(AE350_PIT_TIMER_INTR_CLEAR, PIT_TIMER1);

    static AE350_DRIVER_WDT *WDTdrv = &Driver_WDT;	// WDT

    // Restart WDT timer
    WDTdrv->RestartTimer();

    counter++;

    return;
}

/* Application entry function */
int demo_wdt_and_pit(void)
{
    static AE350_DRIVER_WDT *WDTdrv = &Driver_WDT;	// WDT
    static AE350_DRIVER_PIT *PITdrv = &Driver_PIT;	// PIT as simple timer

    drv_capabilities = WDTdrv->GetCapabilities();

    // Initializes UART
    uart_init(UART_SPEED);	// Baud rate is UART_SPEED

    // Initializes PIT as simple timer
    PITdrv->Initialize();

    printf("\r\nIt's a Watch Dog Timer and Programmable Interval Timer (as simple timer) demo.\r\n\r\n");

    if (drv_capabilities.external_timer)
    {
        printf("WDT will reset whole system if WDT doesn't restarted in 1 seconds.\r\n");
        printf("PIT (as simple timer) restart WDT every 0.5 seconds, so WDT doesn't reset system.\r\n\r\n");
    }
    else
    {
        printf("WDT will reset whole system if WDT doesn't restarted in 1.5 microseconds.\r\n");
        printf("PIT (as simple timer) restart WDT every 1 microsecond, so WDT doesn't reset system.\r\n\r\n");
    }

    if (drv_capabilities.external_timer)
    {
        set_timer_irq_period(PIT_TIMER1, 500); /* 500 microsecond = 0.5 second */
    }
    else
    {
        set_timer_irq_period(PIT_TIMER1, 1);   /*   1 microsecond           */
    }

    // Initializes WDT
    if (drv_capabilities.irq_stage)
    {
        WDTdrv->Initialize(wdt_handler);
    }
    else
    {
        WDTdrv->Initialize(NULL);
    }

    // Set WDT source clock
    if (drv_capabilities.external_timer)
    {
        /* Under external clock rate, pow(2, 15) ticks are 1 second. */
        WDTdrv->Control(AE350_WDT_CLKSRC_EXTERNAL, AE350_WDT_TIME_POW_2_15);
    }
    else
    {
        /* Under APB clock rate, pow(2, 15) ticks are nearly 1.5 microseconds. */
        WDTdrv->Control(AE350_WDT_CLKSRC_APB, AE350_WDT_TIME_POW_2_15);
    }

    // Start watch dog timer
    WDTdrv->RestartTimer();

    int factor, limit = 10;

    if (drv_capabilities.external_timer)
    {
        factor = 1;
    }
    else
    {
        factor = 1000;
    }

    int print_times = 0;
    int disable = 0;

    while (1)
    {
        if ((counter/factor) > print_times)
        {
            unsigned int local_counter = counter;
            printf("PIT (as simple timer) restart WDT (%d times), system still alive.\r\n", local_counter);

            print_times++;
        }

        if (print_times >= limit && !disable)
        {
            printf("\r\nThen, We disable PIT (as simple timer), so the whole system will be reset by WDT.\r\n\r\n");
            PITdrv->Control(AE350_PIT_TIMER_INTR_DISABLE, PIT_TIMER1);

            disable = 1;
        }
    }

    return 0;
}

#endif	/* RUN_DEMO_WDT */
