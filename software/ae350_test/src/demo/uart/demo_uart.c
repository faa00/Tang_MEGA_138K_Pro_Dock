/*
 * ******************************************************************************************
 * File		: demo_uart.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: UART demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This example program shows the usage of the UART interface for asynchronous
 * communication. It demonstrates the UART to send/receive to/from a PC via
 * a terminal emulation program (HyperTerm, TeraTerm).
 *
 * Scenario:
 *
 * After initialized, the UART will send out the string "Press Enter to receive
 * a message" and then wait for receiving data. If the "carriage return" is received
 * (i.e. the 'Enter' key is pressed on PC side under terminal emulation program),
 * the UART will send out the "Hello World!" welcome message. Otherwise, it will
 * bypass the received data and continue to wait for next receiving.
 *
 * Note: Don't enable DMA in RTE_Device.h, it could be crashed.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running UART demo
#if RUN_DEMO_UART

// *********** Includes ************ //
#include "Driver_UART.h"


// ********** Definitions ********** //
// UART1 or UART2 is used
#define UART1_IS_USED	0	// 1: UART1 is used; 0: UART2 is used

/* UART Driver */
#if UART1_IS_USED
extern AE350_DRIVER_UART Driver_UART1;		// UART1
#else
extern AE350_DRIVER_UART Driver_UART2;		// UART2
#endif

static volatile char uart_event_complete = 0;

static void wait_complete(void)
{
	while (!uart_event_complete);
	uart_event_complete = 0;
}

// UART callback event
void myUART_callback(uint32_t event)
{
    switch (event)
    {
    	case AE350_UART_EVENT_RECEIVE_COMPLETE:
    	case AE350_UART_EVENT_TRANSFER_COMPLETE:
    	case AE350_UART_EVENT_SEND_COMPLETE:
    	case AE350_UART_EVENT_TX_COMPLETE:
    		/* Success: continue task */
    		uart_event_complete = 1;
    		break;

    	case AE350_UART_EVENT_RX_TIMEOUT:
    		while(1);
    		break;

    	case AE350_UART_EVENT_RX_OVERFLOW:
    	case AE350_UART_EVENT_TX_UNDERFLOW:
    		while(1);
    		break;
    	default:
    		while(1);
    		break;
    }
}

// Application entry function
int demo_uart(void)
{
#if UART1_IS_USED
	static AE350_DRIVER_UART *UARTdrv = &Driver_UART1;	//UART1
#else
    static AE350_DRIVER_UART *UARTdrv = &Driver_UART2;	//UART2
#endif

    char cmd = 0;

    /* Initializes the UART driver */
    UARTdrv->Initialize(myUART_callback);

    /* Power up the UART peripheral */
    UARTdrv->PowerControl(AE350_POWER_FULL);

    /* Configure the UART to 38400 Bits/second */
    UARTdrv->Control(AE350_UART_MODE_ASYNCHRONOUS |			// Mode			: asynchronous
    				 AE350_UART_DATA_BITS_8 |				// Data bits	: 8-bit
					 AE350_UART_PARITY_NONE |				// Parity		: none
					 AE350_UART_STOP_BITS_1 |				// Stop bit:    : 1-bit
					 AE350_UART_FLOW_CONTROL_NONE, 115200);	// Flow control : none
    														// Baud rate    : 38400

    /* Enable Receiver and Transmitter lines */
    UARTdrv->Control (AE350_UART_CONTROL_TX, 1);			// Transmit
    UARTdrv->Control (AE350_UART_CONTROL_RX, 1);			// Received

    UARTdrv->Send("\r\nPress Enter to receive a message", 34);
    wait_complete();

    // Press character "Enter"
    // Print "Hello World!"
    while (1)
    {
    	// Receive 'G'
        UARTdrv->Receive(&cmd, 1);           /* Get a character 'G' from UART */
        wait_complete();
        if (cmd == 'G')                      /* 'G', send greeting  */
        {
        	UARTdrv->Send("\r\nHello World!", 14);
        	wait_complete();
        }
    }

    return 0;
}

#endif	/* RUN_DEMO_UART */
