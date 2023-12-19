/*
 * ******************************************************************************************
 * File		: ae350.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Interrupt sources, registers, addresses and devices definitions
 * ******************************************************************************************
 */

#ifndef __AE350_H__
#define __AE350_H__


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * System clock
 ****************************************************************************/
#define KHz                     1000				/* KHz */
#define MHz                     1000000				/* MHz */

#define OSCFREQ                 (100 * MHz)
#define CPUFREQ                 (800 * MHz)			/* CPU core	: max 800MHz */
#define RTCFREQ                 (32768    )			/* RTC		: 32.768KHz  */
#define HCLKFREQ                (OSCFREQ  )			/* AHB bus	: max 200MHz */
#define PCLKFREQ                (OSCFREQ  )			/* APB bus	: max 200MHz */
#define UCLKFREQ                (OSCFREQ  )			/* UART                  */


/*****************************************************************************
 * PLIC Interrupt source definitions
 ****************************************************************************/
#define IRQ_RTCPERIOD_SOURCE    1			/* RTC period interrupt       */
#define IRQ_RTCALARM_SOURCE     2			/* RTC alarm interrupt        */
#define IRQ_PIT_SOURCE          3			/* PIT interrupt              */
#define IRQ_GP0_SOURCE			4			/* User interrupt             */
#define IRQ_SPI_SOURCE          5			/* SPI interrupt              */
#define IRQ_I2C_SOURCE          6			/* I2C interrupt              */
#define IRQ_GPIO_SOURCE         7			/* GPIO interrupt             */
#define IRQ_UART1_SOURCE        8			/* UART1 interrupt            */
#define IRQ_UART2_SOURCE        9			/* UART2 interrupt            */
#define IRQ_DMA_SOURCE          10			/* DMA interrupt              */
#define	IRQ_GP1_SOURCE			11			/* User interrupt             */
#define IRQ_GP2_SOURCE			12			/* User interrupt             */
#define IRQ_GP3_SOURCE			13			/* User interrupt             */
#define IRQ_GP4_SOURCE			14			/* User interrupt             */
#define IRQ_GP5_SOURCE			15			/* User interrupt             */
#define IRQ_GP6_SOURCE			16			/* User interrupt             */
#define IRQ_GP7_SOURCE			17			/* User interrupt             */
#define IRQ_GP8_SOURCE			18			/* User interrupt             */
#define IRQ_GP9_SOURCE			19			/* User interrupt             */
#define IRQ_GP10_SOURCE			20			/* User interrupt             */
#define IRQ_GP11_SOURCE			21			/* User interrupt             */
#define IRQ_GP12_SOURCE			22			/* User interrupt             */
#define IRQ_GP13_SOURCE			23			/* User interrupt             */
#define IRQ_GP14_SOURCE			24			/* User interrupt             */
#define IRQ_GP15_SOURCE			25			/* User interrupt             */
#define IRQ_STANDBY_SOURCE      26			/* SMU standby interrupt      */
#define IRQ_WAKEUP_SOURCE       27			/* SMU wake up interrupt      */


#ifndef __ASSEMBLER__

/*****************************************************************************
 * Device Specific Peripheral Registers structures
 ****************************************************************************/
#define __I                     volatile const	/* 'read only' permissions      */
#define __O                     volatile        /* 'write only' permissions     */
#define __IO                    volatile        /* 'read / write' permissions   */


/*****************************************************************************
 * PLMT (Platform Level Machine Timer) - AE350
 ****************************************************************************/
typedef struct
{
	__IO unsigned int MTIME[2];            /* 0x00~0x04 Machine Time Register 		   [1][0]: [63:32][31:0] */
	__IO unsigned int MTIMECMP0[2];        /* 0x08~0x0C Machine Time Compare0 Register [1][0]: [63:32][31:0] */
	__IO unsigned int MTIMECMP1[2];        /* 0x10~0x14 Machine Time Compare1 Register [1][0]: [63:32][31:0] */
	__IO unsigned int MTIMECMP2[2];        /* 0x18~0x1C Machine Time Compare2 Register [1][0]: [63:32][31:0] */
	__IO unsigned int MTIMECMP3[2];        /* 0x20~0x24 Machine Time Compare3 Register [1][0]: [63:32][31:0] */
} PLMT_RegDef;

/*****************************************************************************
 * DMAC (Direct Memory Access Controller) - AE350
 ****************************************************************************/
typedef struct
{
	__IO unsigned int CTRL;                 /* DMA Channel Control Register                        */
	__IO unsigned int TRANSIZE;             /* DMA Channel Transfer Size Register                  */
	__IO unsigned int SRCADDRL;             /* DMA Channel Source Address(low part) Register       */
	__IO unsigned int SRCADDRH;             /* DMA Channel Source Address(high part) Register      */
	__IO unsigned int DSTADDRL;             /* DMA Channel Destination Address Register(low part)  */
	__IO unsigned int DSTADDRH;             /* DMA Channel Destination Address Register(high part) */
	__IO unsigned int LLPL;                 /* DMA Channel Linked List Pointer Register(low part)  */
	__IO unsigned int LLPH;                 /* DMA Channel Linked List Pointer Register(high part) */
} DMA_CHANNEL_REG;

typedef struct
{
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register    */
	     unsigned int RESERVED0[3];         /* 0x04~0x0C Reserved               */
	__I  unsigned int DMACFG;               /* 0x10 DMA Configure Register      */
	     unsigned int RESERVED1[3];         /* 0x14~0x1C Reserved               */
	__IO unsigned int DMACTRL;              /* 0x20 DMA Control Register        */
	__O  unsigned int CHABORT;              /* 0x24 DMA Channel Abort Register  */
	     unsigned int RESERVED2[2];         /* 0x28~0x2C Reserved               */
	__IO unsigned int INTSTATUS;            /* 0x30 Interrupt Status Register   */
	__I  unsigned int CHEN;			        /* 0x34 Channel Enable Register     */
	     unsigned int RESERVED3[2];         /* 0x38~0x3C Reserved               */
	DMA_CHANNEL_REG   CHANNEL[8];           /* 0x40~0x5C Channel #n Registers   */
} DMA_RegDef;

/*****************************************************************************
 * SMU (System Management Unit) - AE350
 ****************************************************************************/
typedef struct
{
	__I  unsigned int SYSTEMVER;            /* 0x00 System ID and Revision Register             */
	__I  unsigned int BOARDID;              /* 0x04 Board ID Register                           */
	__I  unsigned int SYSTEMCFG;            /* 0x08 SYSTEM Configuration Register               */
	     unsigned int RESERVED0;            /* 0x0C Reserved                                    */
	__IO unsigned int WRSR;                 /* 0x10 Wake up and Reset Status Register           */
	__IO unsigned int SMUCR;                /* 0x14 SMU Command Register                        */
	     unsigned int RESERVED1;            /* 0x18 Reserved                                    */
	__IO unsigned int WRMASK;               /* 0x1C Wake up and Reset Mask Register             */
	__IO unsigned int CER;                  /* 0x20 Clock Enable Register                       */
	__IO unsigned int CRR;                  /* 0x24 Clock Ratio Register                        */
	     unsigned int RESERVED2[2];         /* 0x28~0x2C Reserved                               */
	__IO unsigned int URREG0;               /* 0x30 User Defined Register                       */
	__IO unsigned int URREG1;               /* 0x34 User Defined Register                       */
	     unsigned int RESERVED3[2];         /* 0x38~0x3C Reserved                               */
	__IO unsigned int SCRATCH;              /* 0x40 Scratch Register                            */
	__IO unsigned int HARTS_RESET_REG;      /* 0x44 HARTs Reset Register                        */
	     unsigned int RESERVED4[2];         /* 0x48 ~ 0x4C Reserved                             */
	__IO unsigned int HART0_RESET_VECTOR;   /* 0x50 HART0 Reset Vector Register                 */
	__IO unsigned int HART1_RESET_VECTOR;   /* 0x54 HART1 Reset Vector Register                 */
	__IO unsigned int HART2_RESET_VECTOR;   /* 0x58 HART2 Reset Vector Register                 */
	__IO unsigned int HART3_RESET_VECTOR;   /* 0x5C HART3 Reset Vector Register                 */
	     unsigned int RESERVED5[40];        /* 0x60~0xFC Reserved                               */
	__IO unsigned int PMCR;                 /* 0x100 Power Mode Cycle Register                  */
	__IO unsigned int PMDIR;                /* 0x104 Power Mode Disable Initialization Register */
	__IO unsigned int PDSR;                 /* 0x108 Power Domain Select Register               */
	__IO unsigned int PSR;                  /* 0x10C Power Status Register                      */
	__IO unsigned int PWRR;                 /* 0x110 Power Wake up Record Register              */
	__IO unsigned int PWER;                 /* 0x114 Power Wake up Enable Register              */
	__IO unsigned int PCR;                  /* 0x118 Power Control Register                     */
	__IO unsigned int PIR;                  /* 0x11C Power Interrupt Register                   */
} SMU_RegDef;

/*****************************************************************************
 * UARTx (Universal Asynchronous Receiver/Transmitter) - AE350
 ****************************************************************************/
typedef struct
{
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register           */
	     unsigned int RESERVED0[3];         /* 0x04~0x0C Reserved                      */
	__I  unsigned int CFG;                  /* 0x10 Hardware Configure Register        */
	__IO unsigned int OSCR;                 /* 0x14 Over Sample Control Register       */
	     unsigned int RESERVED1[2];         /* 0x18~0x1C Reserved                      */
	union
	{
		__I  unsigned int RBR;              /* 0x20 Receiver Buffer Register           */
		__O  unsigned int THR;              /* 0x20 Transmitter Holding Register       */
		__IO unsigned int DLL;              /* 0x20 Divisor Latch LSB                  */
	};
	union
	{
		__IO unsigned int IER;              /* 0x24 Interrupt Enable Register          */
		__IO unsigned int DLM;              /* 0x24 Divisor Latch MSB                  */
	};
	union
	{
		__I  unsigned int IIR;              /* 0x28 Interrupt Identification Register  */
		__O  unsigned int FCR;              /* 0x28 FIFO Control Register              */
	};
	__IO unsigned int LCR;                  /* 0x2C Line Control Register              */
	__IO unsigned int MCR;                  /* 0x30 Modem Control Register             */
	__IO unsigned int LSR;                  /* 0x34 Line Status Register               */
	__IO unsigned int MSR;                  /* 0x38 Modem Status Register              */
	__IO unsigned int SCR;                  /* 0x3C Scratch Register                   */
} UART_RegDef;

/*****************************************************************************
 * PIT (Programmable Interval Timer) as Simple Timer - AE350
 * PWM (Pulse Width Modulator) - AE350
 ****************************************************************************/
/*
 * PIT as simple timer or as PWM
 *
 * 32-bit Timer 0
 * 16-bit Timer 0, 16-bit Timer 1
 *  8-bit Timer 0,  8-bit Timer 1, 8-bit Timer 2, 8-bit Timer3
 *                                                             16-bit PWM
 * 16-bit Timer 0,                                              8-bit PWM
 *  8-bit Timer 0,  8-bit Timer 1,                              8-bit PWM
 */
typedef struct
{
	__IO unsigned int CTRL;                 /* PIT Channel Control Register */
	__IO unsigned int RELOAD;               /* PIT Channel Reload Register  */
	__IO unsigned int COUNTER;              /* PIT Channel Counter Register */
	     unsigned int RESERVED[1];			/* Reserved                     */
} PIT_CHANNEL_REG;

typedef struct
{
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register  */
	     unsigned int RESERVED[3];          /* 0x04~0x0C Reserved             */
	__I  unsigned int CFG;                  /* 0x10 Configuration Register    */
	__IO unsigned int INTEN;                /* 0x14 Interrupt Enable Register */
	__IO unsigned int INTST;                /* 0x18 Interrupt Status Register */
	__IO unsigned int CHNEN;                /* 0x1C Channel Enable Register   */
	PIT_CHANNEL_REG   CHANNEL[4];           /* 0x20~0x50 Channel #n Registers */
} PIT_RegDef;

/*****************************************************************************
 * WDT (Watchdog timer) - AE350
 ****************************************************************************/
typedef struct
{
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register */
	     unsigned int RESERVED[3];          /* 0x04~0x0C Reserved            */
	__IO unsigned int CTRL;                 /* 0x10 Control Register         */
	__O  unsigned int RESTART;              /* 0x14 Restart Register         */
	__O  unsigned int WREN;                 /* 0x18 Write Enable Register    */
	__IO unsigned int ST;                   /* 0x1C Status Register          */
} WDT_RegDef;

/*****************************************************************************
 * RTC (Real Time Clock) - AE350
 ****************************************************************************/
typedef struct
{
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register */
	     unsigned int RESERVED[3];          /* 0x04~0x0C Reserved            */
	__IO unsigned int CNTR;                 /* 0x10 Counter Register         */
	__IO unsigned int ALARM;                /* 0x14 Alarm Register           */
	__IO unsigned int CTRL;                 /* 0x18 Control Register         */
	__IO unsigned int STATUS;               /* 0x1C Status Register          */
	__IO unsigned int TRIM;					/* 0x20 Digit Trimming Register  */
} RTC_RegDef;

/*****************************************************************************
 * GPIO (General Purpose I/O) - AE350
 ****************************************************************************/
typedef struct
{
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register        */
	     unsigned int RESERVED0[3];         /* 0x04~0x0C Reserved                   */
	__I  unsigned int CFG;                  /* 0x10 Configuration Register          */
	     unsigned int RESERVED1[3];         /* 0x14~0x1C Reserved                   */
	__I  unsigned int DATAIN;               /* 0x20 Channel Data-in Register        */
	__IO unsigned int DATAOUT;              /* 0x24 Channel Data-out Register       */
	__IO unsigned int CHANNELDIR;           /* 0x28 Channel Direction Register      */
	__O  unsigned int DOUTCLEAR;            /* 0x2C Channel Data-out Clear Register */
	__O  unsigned int DOUTSET;              /* 0x30 Channel Data-out Set Register   */
	     unsigned int RESERVED2[3];         /* 0x34~0x3C Reserved                   */
	__IO unsigned int PULLEN;               /* 0x40 Pull Enable Register            */
	__IO unsigned int PULLTYPE;             /* 0x44 Pull Type Register              */
	     unsigned int RESERVED3[2];         /* 0x48~0x4C Reserved                   */
	__IO unsigned int INTREN;               /* 0x50 Interrupt Enable Register       */
	__IO unsigned int INTRMODE0;            /* 0x54 Interrupt Mode Register (0~7)   */
	__IO unsigned int INTRMODE1;            /* 0x58 Interrupt Mode Register (8~15)  */
	__IO unsigned int INTRMODE2;            /* 0x5C Interrupt Mode Register (16~23) */
	__IO unsigned int INTRMODE3;            /* 0x60 Interrupt Mode Register (24~31) */
	__IO unsigned int INTRSTATUS;           /* 0x64 Interrupt Status Register       */
         unsigned int RESERVED4[2];         /* 0x68~0x6C Reserved                   */
	__IO unsigned int DEBOUNCEEN;           /* 0x70 De-bounce Enable Register       */
	__IO unsigned int DEBOUNCECTRL;         /* 0x74 De-bounce control register      */
	     unsigned int RESERVED5[2];			/* 0x78~0x7C Reserved                   */
} GPIO_RegDef;

/*****************************************************************************
 * I2C (Inter-Integrated Circuit Master/Slave) - AE350
 ****************************************************************************/
typedef struct
{
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register             */
	     unsigned int RESERVED[3];          /* 0x04~0x0C Reserved                        */
	__I  unsigned int CFG;                  /* 0x10 Configuration Register               */
	__IO unsigned int INTEN;                /* 0x14 Interrupt Enable Register            */
	__IO unsigned int STATUS;               /* 0x18 Status Register                      */
	__IO unsigned int ADDR;                 /* 0x1C Address Register                     */
	__IO unsigned int DATA;                 /* 0x20 Data Register                        */
	__IO unsigned int CTRL;                 /* 0x24 Control Register                     */
	__IO unsigned int CMD;                  /* 0x28 Command Register                     */
	__IO unsigned int SETUP;                /* 0x2C Setup Register                       */
	__IO unsigned int TPM;                  /* 0x30 Timing Parameter Multiplier Register */
} I2C_RegDef;

/*****************************************************************************
 * SPI (Serial Peripheral Interface) - AE350
 ****************************************************************************/
typedef struct
{
	__I  unsigned int IDREV;                /* 0x00 ID and Revision Register           */
	     unsigned int RESERVED0[3];         /* 0x04~0x0C Reserved                      */
	__IO unsigned int TRANSFMT;             /* 0x10 SPI Transfer Format Register       */
	__IO unsigned int DIRECTIO;             /* 0x14 SPI Direct IO Control Register     */
	     unsigned int RESERVED1[2];         /* 0x18~0x1C Reserved                      */
	__IO unsigned int TRANSCTRL;            /* 0x20 SPI Transfer Control Register      */
	__IO unsigned int CMD;                  /* 0x24 SPI Command Register               */
	__IO unsigned int ADDR;                 /* 0x28 SPI Address Register               */
	__IO unsigned int DATA;                 /* 0x2C SPI Data Register                  */
	__IO unsigned int CTRL;                 /* 0x30 SPI Control Register               */
	__I  unsigned int STATUS;               /* 0x34 SPI Status Register                */
	__IO unsigned int INTREN;               /* 0x38 SPI Interrupt Enable Register      */
	__O  unsigned int INTRST;               /* 0x3C SPI Interrupt Status Register      */
	__IO unsigned int TIMING;               /* 0x40 SPI Interface Timing Register      */
	     unsigned int RESERVED2[3];         /* 0x44~0x4C Reserved                      */
	__IO unsigned int MEMCTRL;              /* 0x50 SPI Memory Access Control Register */
	     unsigned int RESERVED3[3];         /* 0x54 ~ 0x5C Reserved                    */
	__IO unsigned int SLVST;                /* 0x60 SPI Slave Status Register          */
	__I  unsigned int SLVDATACNT;           /* 0x64 SPI Slave Data Count Register      */
	     unsigned int RESERVED4[5];         /* 0x68~0x78 Reserved                      */
	__I  unsigned int CONFIG;               /* 0x7C Configuration Register             */
} SPI_RegDef;


/*****************************************************************************
 * Memory Map
 ****************************************************************************/
#define _IO_(addr)              (addr)

#define ILM_BASE                0xA0000000          /* Local Memory Slave Port : ILM */
#define DLM_BASE                0xA0200000          /* Local Memory Slave Port : DLM */
#define SPIMEM_BASE             0x80000000          /* ROM/FLASH                     */
#define DDRMEM_BASE             0x00000000          /* DDR/SRAM                      */

#define PLMT_BASE               _IO_(0xE6000000)    /* Machine Timer                 */
#define SMU_BASE                _IO_(0xF0100000)    /* SMU                           */
#define UART1_BASE              _IO_(0xF0200000)    /* UART1                         */
#define UART2_BASE              _IO_(0xF0300000)    /* UART2                         */
#define PIT_BASE                _IO_(0xF0400000)    /* PIT                           */
#define WDT_BASE                _IO_(0xF0500000)    /* WDT                           */
#define RTC_BASE                _IO_(0xF0600000)    /* RTC                           */
#define GPIO_BASE               _IO_(0xF0700000)    /* GPIO                          */
#define I2C_BASE                _IO_(0xF0A00000)    /* I2C                           */
#define DMAC_BASE               _IO_(0xF0C00000)    /* DMAC                          */
#define SPI_BASE                _IO_(0xF0F00000)    /* SPI                           */

#define APB_EXT_BASE            _IO_(0xF8000000)	/* Extended APB Slave            */
#define AHB_EXT_BASE            _IO_(0xE8000000)	/* Extended AHB Slave            */

#define PLIC_BASE               _IO_(0xE4000000)	/* PLIC                          */
#define PLIC_SW_BASE            _IO_(0xE6400000)	/* PLIC-SWINT                    */

#define BMC_BASE                _IO_(0xC0000000)	/* BMC                           */
#define AHBDEC_BASE             _IO_(0xE0000000)	/* AHB Decoder                   */
#define PLDM_BASE               _IO_(0xE6800000)	/* Debug Module                  */
#define APBBRG_BASE             _IO_(0xF0000000)	/* APB Bridge                    */


/*****************************************************************************
 * Peripheral device declaration
 ****************************************************************************/
#define AE350_PLMT              ((PLMT_RegDef *) PLMT_BASE  )		/* Machine TImer	*/
#define AE350_DMA               ((DMA_RegDef *)  DMAC_BASE  )		/* DMAC				*/
#define AE350_SMU               ((SMU_RegDef *)  SMU_BASE   )		/* SMU				*/
#define AE350_UART1             ((UART_RegDef *) UART1_BASE )		/* UART1			*/
#define AE350_UART2             ((UART_RegDef *) UART2_BASE )		/* UART2			*/
#define AE350_PIT               ((PIT_RegDef *)  PIT_BASE   )		/* PIT				*/
#define AE350_WDT               ((WDT_RegDef *)  WDT_BASE   )		/* WDT				*/
#define AE350_RTC               ((RTC_RegDef *)  RTC_BASE   )		/* RTC				*/
#define AE350_GPIO              ((GPIO_RegDef *) GPIO_BASE  )		/* GPIO				*/
#define AE350_I2C               ((I2C_RegDef *)  I2C_BASE   )		/* I2C				*/
#define AE350_SPI               ((SPI_RegDef *)  SPI_BASE   )		/* SPI				*/

#endif	/* __ASSEMBLER__ */


#ifdef __cplusplus
}
#endif


#endif	/* __AE350_H__ */
