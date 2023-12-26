/*
 * ******************************************************************************************
 * File		: demo_rtc.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: RTC demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This example program shows the usage of the RTC interface for time interrupt.
 * It demonstrates the second interrupt as elapsed 1 second and the alarm interrupt
 * as rtc_time count equal to alarm_time.
 *
 * Scenario:
 *
 * After initialized, the demo program will set the rtc_time to 0 and the alarm_time
 * 1 minute. As 1 second elapsed, the program output UART message with "Second interrupt done".
 * Similarly, as the rtc_time count equal to the alarm_time, the program will
 * output UART message with "Alarm interrupt done".
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running RTC demo
#if RUN_DEMO_RTC

// ************ Includes ************ //
#include "Driver_RTC.h"
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********** //
// RTC demo flag
typedef enum _DEMO_RTC_FLAG_ID
{
    DEMO_RTC_NONE,
    DEMO_RTC_ALARM_INT_DONE,
    DEMO_RTC_DAY_INT_DONE,
    DEMO_RTC_HOUR_INT_DONE,
    DEMO_RTC_MIN_INT_DONE,
    DEMO_RTC_SEC_INT_DONE,
    DEMO_RTC_HSEC_INT_DONE,
} DEMO_RTC_FLAG_ID;

// Volatile is necessary for while(!(g_Demo_RTC_Flag == eRTC_DEMO_SEC_INT_DONE));
volatile uint32_t g_Demo_RTC_Flag = DEMO_RTC_NONE;
extern AE350_DRIVER_RTC Driver_RTC;

#define ALARM_DELTA_3S     (3)


// Alarm interrupt operation
void Alarm_Int(void)
{
    AE350_DRIVER_RTC* pDrv_RTC = &Driver_RTC;

    printf("Alarm interrupt operation.....\r\n");

    // Disable alarm interrupt
    pDrv_RTC->Control(AE350_RTC_CTRL_ALARM_INT, 0);

    g_Demo_RTC_Flag = DEMO_RTC_ALARM_INT_DONE;
}

// Day interrupt operation
void Day_Int(void)
{
    AE350_DRIVER_RTC* pDrv_RTC = &Driver_RTC;

    printf("Day interrupt operation.....\r\n");

    // Disable day interrupt
    pDrv_RTC->Control(AE350_RTC_CTRL_DAY_INT, 0);

    g_Demo_RTC_Flag = DEMO_RTC_DAY_INT_DONE;
}

// Hour interrupt operation
void Hour_Int(void)
{
    AE350_DRIVER_RTC* pDrv_RTC = &Driver_RTC;

    printf("Hour interrupt operation.....\r\n");

    // Disable hour interrupt
    pDrv_RTC->Control(AE350_RTC_CTRL_HOUR_INT, 0);

    g_Demo_RTC_Flag = DEMO_RTC_HOUR_INT_DONE;
}

// Minute interrupt operation
void Min_Int(void)
{
    AE350_DRIVER_RTC* pDrv_RTC = &Driver_RTC;

    printf("Minute interrupt operation.....\r\n");

    // Disable MIN interrupt
    pDrv_RTC->Control(AE350_RTC_CTRL_MIN_INT, 0);

    g_Demo_RTC_Flag = DEMO_RTC_MIN_INT_DONE;
}

// Second interrupt operation
void Sec_Int(void)
{
    AE350_DRIVER_RTC* pDrv_RTC = &Driver_RTC;

    printf("Second interrupt operation.....\r\n");

    // Disable second interrupt
    pDrv_RTC->Control(AE350_RTC_CTRL_SEC_INT, 0);

    g_Demo_RTC_Flag = DEMO_RTC_SEC_INT_DONE;
}

// Half second interrupt operation
void Hsec_Int(void)
{
    AE350_DRIVER_RTC* pDrv_RTC = &Driver_RTC;

    printf("Half second interrupt operation.....\r\n");

    // Disable half-second interrupt
    pDrv_RTC->Control(AE350_RTC_CTRL_HSEC_INT, 0);

    g_Demo_RTC_Flag = DEMO_RTC_HSEC_INT_DONE;
}

// RTC callback event
void My_RTC_Callback(uint32_t event)
{
    // Callback function body
    if(event & AE350_RTC_EVENT_ALARM_INT)
    {
    	// Alarm interrupt event
        Alarm_Int();
    }

    if(event & AE350_RTC_EVENT_DAY_INT)
    {
    	// Day interrupt event
        Day_Int();
    }

    if(event & AE350_RTC_EVENT_HOUR_INT)
    {
    	// Hour interrupt event
        Hour_Int();
    }

    if(event & AE350_RTC_EVENT_MIN_INT)
    {
    	// Minute interrupt event
        Min_Int();
    }

    if(event & AE350_RTC_EVENT_SEC_INT)
    {
    	// Second interrupt event
        Sec_Int();
    }

    if(event & AE350_RTC_EVENT_HSEC_INT)
    {
    	// Half interrupt event
        Hsec_Int();
    }
}

// Application entry function
int demo_rtc(void)
{
    AE350_DRIVER_RTC*     pDrv_RTC = &Driver_RTC;
    AE350_RTC_TIME        rRTC_Time = {0,0,0,0};
    AE350_RTC_ALARM       rRTC_Alarm = {0,0,0};

    // Initializes UART
    uart_init(UART_SPEED);	// Baud rate is UART_SPEED

    printf("\r\nIt's a Real Time Clock demo.\r\n\r\n");

    // Start demo
    g_Demo_RTC_Flag = DEMO_RTC_NONE;

    if(pDrv_RTC)
    {
    	// Initializes RTC
        pDrv_RTC->Initialize(My_RTC_Callback);
        pDrv_RTC->PowerControl(AE350_POWER_FULL);

        pDrv_RTC->SetTime(&rRTC_Time);

        // -- Half second interrupt event
        pDrv_RTC->Control(AE350_RTC_CTRL_HSEC_INT, 1);			// Enable half second interrupt event
        pDrv_RTC->Control(AE350_RTC_CTRL_EN, 1);				// Enable RTC

        while(!(g_Demo_RTC_Flag == DEMO_RTC_HSEC_INT_DONE));	// Waiting half second interrupt event completed
        printf("Half Second interrupt done.....\r\n\r\n");

        // -- Second interrupt event
        pDrv_RTC->Control(AE350_RTC_CTRL_SEC_INT, 1);			// Enable second interrupt event
        pDrv_RTC->Control(AE350_RTC_CTRL_EN, 1);				// Enable RTC

        while(!(g_Demo_RTC_Flag == DEMO_RTC_SEC_INT_DONE));		// Waiting second interrupt event completed
        printf("Second interrupt done.....\r\n\r\n");

        // -- Alarm interrupt event
        printf("After 3 seconds elapsed, you will see alarm interrupt done message.....\r\n");

        pDrv_RTC->PowerControl(AE350_POWER_FULL);

        // Get current RTC time
        pDrv_RTC->GetTime(&rRTC_Time);
        rRTC_Alarm.hour = rRTC_Time.hour;
        rRTC_Alarm.min= rRTC_Time.min;
        rRTC_Alarm.sec= rRTC_Time.sec + ALARM_DELTA_3S;
        pDrv_RTC->SetAlarm(&rRTC_Alarm);

        pDrv_RTC->Control(AE350_RTC_CTRL_ALARM_INT, 1);			// Enable alarm interrupt event
        pDrv_RTC->Control(AE350_RTC_CTRL_EN, 1);				// Enable RTC

        while(!(g_Demo_RTC_Flag == DEMO_RTC_ALARM_INT_DONE));	// Waiting alarm interrupt event completed
        printf("Alarm interrupt done.....\r\n\r\n");

        // -- Minute interrupt event

        // -- Hour interrupt event

        // -- Day interrupt event

        // Exit RTC
        pDrv_RTC->Uninitialize();
    }
    else
    {
        // pDrv_RTC is NULL
    }

    return 0;
}

#endif	/* RUN_DEMO_RTC */
