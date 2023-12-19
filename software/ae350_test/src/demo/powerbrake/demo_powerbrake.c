/*
 * ******************************************************************************************
 * File		: demo_powerbrake.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Hardware performance throttling mechanism demo
 * ******************************************************************************************
 */

/*
 ********************************************************************************************
 * This demo program shows hardware performance throttling mechanism.
 *
 * Scenario:
 *
 * We go the number game(0 => 9) under MCU configured in lowest or highest performance
 * throttling.
 * As run game with MCU configured in lowest performance, the cycle count is larger than
 * that with MCU configured in highest performance.
 ********************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "demo.h"

// If running hardware performance throttling mechanism demo
#if RUN_DEMO_POWERBRAKE

// ************ Includes ************ //
#include "platform.h"
#include "uart.h"
#include <stdio.h>


// ********** Definitions ********** //

/* PFT feature configuration in MMSC_CFG */
#define MISC_PFT                (1UL << 4)

/* PFT enable in MXSTATUS */
#define PFT_EN                  (1UL << 0)

/* Machine mode MPFT_CTL */
#define T_LEVEL(N)              ((N) << 4)
#define T_LEVEL_MSK             (0xF << 4)
#define T_LEVEL_HIGHEST         T_LEVEL(0)
#define T_LEVEL_LOWEST          T_LEVEL(15)
#define FAST_INT                (1UL << 8)

/* SWI PFT mode flag */
#define SWI_FAST_INT            0
#define SWI_NORMAL              1

#define LOOP_DELAY_COUNT        0x3000

/* Interrupt priority */
#define IRQ_PRIORITY_LOW        1
#define IRQ_PRIORITY_HIGH       2

/* Interrupt SHV mode selection */
#define IRQ_NONSHV              0
#define IRQ_SHV                 1


unsigned long long consumed_cycles_lowest = 0;
unsigned long long consumed_cycles_highest = 0;
volatile unsigned int swi_flag = 0;
volatile unsigned long long consumed_cycles_swi_fast_int = 0;
volatile unsigned long long consumed_cycles_swi_normal = 0;
volatile unsigned int isr_sync = 0;


__attribute__((always_inline))
static inline unsigned long long rdmcycle(void)
{
#if __riscv_xlen == 32
	do
	{
		unsigned long hi = read_csr(NDS_MCYCLEH);
		unsigned long lo = read_csr(NDS_MCYCLE);

		if (hi == read_csr(NDS_MCYCLEH))
		{
			return ((unsigned long long)hi << 32) | lo;
		}
	} while(1);
#else
	return read_csr(NDS_MCYCLE);
#endif
}

static void __attribute__((no_execit, no_profile_instrument_function)) loop_delay(unsigned int n)
{
	register unsigned int i = 0;

	for(i = 0; i < n; i++)
	{
		asm volatile ("nop");
	}
}

// Number game
static unsigned long long num_game(void)
{
	unsigned int i = 0;
	unsigned long long before_cycle_cnt, after_cycle_cnt;
	unsigned long long consumed_cycles = 0;

	/* Cycle counts start */
	before_cycle_cnt = rdmcycle();

	for(i = 0; i < 10; i++)
	{
		printf("%d", i);

		loop_delay(LOOP_DELAY_COUNT);

		printf(".");

		loop_delay(LOOP_DELAY_COUNT);

		printf(".");

		loop_delay(LOOP_DELAY_COUNT);

		printf(".");

		loop_delay(LOOP_DELAY_COUNT);
	}

	printf("\r\n");

	/* Cycle counts stop */
	after_cycle_cnt = rdmcycle();

	/* Consumed delta cycle counts */
	consumed_cycles = after_cycle_cnt - before_cycle_cnt;

	printf("consumed cycles: %u.\r\n", (unsigned int)consumed_cycles);

	return consumed_cycles;
}

// Do machine software interrupt handler
void mswi_handler(void)
{
	printf("It's machine software interrupt handler.\r\n");

	/* Clear Machine SWI (MSIP) */
	HAL_MSWI_CLEAR();

	printf("Go number game in ISR: 0 --> 9.\r\n");

	if(swi_flag == SWI_FAST_INT)
	{
		consumed_cycles_swi_fast_int = num_game();
	}
	else if(swi_flag == SWI_NORMAL)
	{
		consumed_cycles_swi_normal = num_game();
	}

	isr_sync = 1;
}

// Setup machine software interrupt handler
static void setup_mswi(void)
{
	/* Initial Machine software interrupt (MSIP). */
	/* Machine SWI is connected to PLIC_SW source 1 */
	HAL_MSWI_INITIAL();

	printf("\r\nSetup machine software interrupt handler.\r\n");
}

// Trigger machine software interrupt handler
static void trigger_mswi(void)
{
	printf("\r\nTrigger machine software interrupt handler.\r\n");

	/* Trigger Machine software interrupt handler */
	HAL_MSWI_PENDING();
}

// Application entry function
int demo_powerbrake(void)
{
	// Initializes UART
	uart_init(38400);		// Baud rate is 38400

	printf("\r\nIt's a Hardware Performance Throttling Mechanism demo.\r\n");

	/*
	 * Set interrupt level threshold to 0.
	 * Interrupt level 0 corresponds to regular execution (no interrupt) outside of an interrupt handler.
	 * Levels 1 - 255 correspond to interrupt handler levels.
	 */
	HAL_INTERRUPT_THRESHOLD(0);

	/* Disable global interrupt for core */
	HAL_MIE_DISABLE();

	/*
	 * Check whether the MCU configured with power brake or not.
	 * The MMSC_CFG bit 4 indicates this.
	 */
	if (!(read_csr(NDS_MMSC_CFG) & MISC_PFT))
	{
		printf("MCU does NOT support power brake.\r\n");
		goto error;
	}

	printf("\r\n================================================================================\r\n");
	printf("[Part 1: Run the number game with LOWEST/HIGHEST performance]\r\n");
	printf("\nMCU is configured to LOWEST performance (T_LEVEL: 15)\r\n");

	/* Disable performance throttling */
	clear_csr(NDS_MXSTATUS, PFT_EN);

	/* Set Throttling Level to lowest performance */
	write_csr(NDS_MPFT_CTL, T_LEVEL_LOWEST);

	/* Enable performance throttling */
	set_csr(NDS_MXSTATUS, PFT_EN);

	printf("Go number game: 0 --> 9.\r\n");
	consumed_cycles_lowest = num_game();

	printf("\r\nMCU is configured to HIGHEST performance (T_LEVEL: 0)\r\n");

	/* Disable performance throttling */
	clear_csr(NDS_MXSTATUS, PFT_EN);

	/* Set Throttling Level to highest performance */
	write_csr(NDS_MPFT_CTL, T_LEVEL_HIGHEST);

	/* Enable performance throttling */
	set_csr(NDS_MXSTATUS, PFT_EN);

	printf("Go number game: 0 --> 9.\r\n");
	consumed_cycles_highest = num_game();

	if(consumed_cycles_lowest > consumed_cycles_highest)
	{
		printf("\r\nConsumed cycles (LOWEST performance) > Consumed cycles (HIGHEST performance) => OK\r\n");
	}
	else
	{
		printf("\r\nConsumed cycles (HIGHEST performance) > Consumed cycles (LOWEST performance) => Fail\r\n");
		goto error;
	}

	printf("\r\n================================================================================\r\n");
	printf("[Part 2: Run the number game in ISR while ENABLE/DISABLE fast interrupt mode (FAST_INT)]\r\n");

	/* Disable Machine software interrupt. */
	HAL_MSWI_DISABLE();

	/* Setup Machine software interrupt handler */
	setup_mswi();

	/* Enable Machine software interrupt */
	HAL_MSWI_ENABLE();

	/* Enable global interrupt for core */
	HAL_MIE_ENABLE();

	printf("\r\nMCU is configured to LOWEST performance (T_LEVEL: 15)\r\n");
	printf("<ENABLE fast interrupt mode>\r\n");

	/* Disable performance throttling */
	clear_csr(NDS_MXSTATUS, PFT_EN);

	/* Set Throttling Level to lowest performance with fast interrupt response*/
	write_csr(NDS_MPFT_CTL, T_LEVEL_LOWEST | FAST_INT);

	/* Enable performance throttling */
	set_csr(NDS_MXSTATUS, PFT_EN);

 	swi_flag = SWI_FAST_INT;
	trigger_mswi();		// Trigger machine software interrupt handler
	while(!isr_sync){};
	isr_sync = 0;

	printf("\r\n<DISABLE fast interrupt mode>\r\n");

	/* Disable performance throttling */
	clear_csr(NDS_MXSTATUS, PFT_EN);

	/* Set Throttling Level to lowest performance without fast interrupt response*/
	write_csr(NDS_MPFT_CTL, T_LEVEL_LOWEST);

	/* Enable performance throttling */
	set_csr(NDS_MXSTATUS, PFT_EN);

	swi_flag = SWI_NORMAL;
	trigger_mswi();		// Trigger machine software interrupt handler
	while(!isr_sync){};
	isr_sync = 0;

	if(consumed_cycles_swi_normal > consumed_cycles_swi_fast_int)
	{
		printf("\r\nConsumed cycles(DISABLE FAST_INT) > Consumed cycles(ENABLE FAST_INT) => OK\r\n");
	}
	else
	{
		printf("\r\nConsumed cycles(ENABLE FAST_INT) > Consumed cycles(DISABLE FAST_INT) => Fail\r\n");
		goto error;
	}

	printf("\r\nDemo Hardware Performance Throttling Mechanism PASS!\r\n");

	/* Disable performance throttling */
	clear_csr(NDS_MXSTATUS, PFT_EN);

	return 0;

error:
	printf("\r\nDemo Hardware Performance Throttling Mechanism FAIL!\r\n");

	/* Disable performance throttling */
	clear_csr(NDS_MXSTATUS, PFT_EN);

	return 1;
}

#endif	/* RUN_DEMO_POWERBRAKE */
