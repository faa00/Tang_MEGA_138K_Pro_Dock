/*
 * ******************************************************************************************
 * File		: demo_pwm.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: PWM demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This example program shows the usage of the PWM interface for driving
 * a speaker or buzzer. It demonstrates the PWM output channel 1 to output
 * different frequency and duty cycle signal to the speaker or buzzer to
 * play the notes.
 *
 * Scenario:
 *
 * After initialized, the UART will send out the instruction messages and
 * then wait for receiving data. If the number 1 ~ 7 is received (i.e.
 * the '1' ~ '7' key is pressed on PC side under terminal emulation program),
 * the corresponding 'Do' ~ 'Si' frequency PWM signal is output to
 * speaker to play the note. That is, '1' for 'Do' note, '2' for 'Re' note
 * ... and so on. If the "space" is received, the PWM stop output. And if
 * the "carriage return" is received, the program will exits. Otherwise,
 * it will bypass the received data and continue to wait for next receiving.
 ********************************************************************************************
 * */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running PWM demo
#if RUN_DEMO_PWM

// ************ Includes *********** //
#include "Driver_PWM.h"
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********* // 
#define PWM_OUPUT_CHANNEL		1	 // PWM channel 1

#define NOTE_DO                 262  //523
#define NOTE_RE                 294  //587
#define NOTE_MI                 330  //659
#define NOTE_FA                 349  //698
#define NOTE_SO                 392  //784
#define NOTE_LA                 440  //880
#define NOTE_SE                 494  //988

/* PWM Driver */
extern AE350_DRIVER_PWM Driver_PWM;			// PWM

static const unsigned int tones[] =
{
    NOTE_DO, NOTE_RE, NOTE_MI, NOTE_FA, NOTE_SO, NOTE_LA, NOTE_SE
};


// Application entry function
int demo_pwm(void)
{
    static AE350_DRIVER_PWM *PWMdrv = &Driver_PWM;
    char pwm_num = PWM_OUPUT_CHANNEL;
    char note;

    /* Initializes UART */
    uart_init(38400);	// Baud rate is 38400

    printf("\r\nIt's a Pulse Width Modulator demo.\r\n\r\n");

    /* Initializes the PWM driver */
    PWMdrv->Initialize(NULL);

    /* Power up the PWM peripheral */
    PWMdrv->PowerControl(AE350_POWER_FULL);

    /* Active the PWM output channel */
    PWMdrv->Control(AE350_PWM_ACTIVE_CONFIGURE |
    		        AE350_PWM_PARK_LOW |
					AE350_PWM_CLKSRC_EXTERNAL, pwm_num);

    printf("Let's play piano...\r\n");
    printf("[1 2 3 4 5 6 7] ==> Play [Do Re Mi Fa So La Si]\r\n");
    printf("[Space]         ==> Mute\r\n");
    printf("[Enter]         ==> Quit\r\n");

    while(1)
    {
        /* Input a character from UART */
    	note = uart_getc();

        if (note == 13)
        {
        	/* If input Enter, quit */
        	printf("Input a [Enter]\r\n");

            PWMdrv->PowerControl(AE350_POWER_OFF);
            printf("\r\nQUIT!");

            break;
        }
        else if (note > 48 && note < 56)
        {
        	/* If input 1 ~ 7, play note */
        	printf("Input a [%d]\r\n", note);

        	PWMdrv->SetFreq(pwm_num, tones[note - 49]);
            PWMdrv->Output(pwm_num, 255/2);  /* Output 50% duty cycle */
        }
        else if (note == 32)
        {
            /* If input Space, mute */
        	printf("Input a [Space]\r\n");

            PWMdrv->Output(pwm_num, 0);      /* Output 0% duty cycle to disable */
        }
    }

    return 0;
}

#endif	/* RUN_DEMO_PWM */
