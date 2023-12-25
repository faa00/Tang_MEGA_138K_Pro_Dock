/*
 * ******************************************************************************************
 * File		: i2c_ae350.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: I2C driver definitions
 * ******************************************************************************************
 */

#ifndef __I2C_AE350_H__
#define __I2C_AE350_H__


// Includes ---------------------------------------------------------------------------------
#include "Driver_I2C.h"
#include "dma_ae350.h"
#include "platform.h"


// Definitions  -----------------------------------------------------------------------------
// Select APB clock
#define APB_CLK_200MHZ		0	// APB clock is 200MHz
#define APB_CLK_125MHZ		0	// APB clock is 125MHz
#define APB_CLK_100MHZ		1	// APB clock is 100MHz
#define APB_CLK_50MHZ		0	// APB clock is 50MHz


#ifndef BIT
#define BIT(n)                      ((unsigned int) 1 << (n))
#define BITS2(m,n)                  (BIT(m) | BIT(n))

/* bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                   (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */

// n-byte FIFO, where n is the number 2, 4, 8 or 16.
#define IIC_FIFO_DEPTH_N      		(16)

// ID revision register bit define (RO)
#define IIC_ID                		BITS(12,31)     // ID number for I2C
#define IIC_MAJOR             		BITS(4,11)      // Enable alarm wake up signal
#define IIC_MINOR             		BITS(0,3)       // Enable alarm interrupt

// Configuration Register (RO)
#define FIFO_SZ_MSK                 BITS(0,1)       // FIFO size mask
#define FIFO_SZ_2                   (0x0)           // 2 bytes
#define FIFO_SZ_4                   (0x1)           // 4 bytes
#define FIFO_SZ_8                   (0x2)           // 8 bytes
#define FIFO_SZ_16                  (0x3)           // 16 bytes
#define MAX_XFER_SZ                 (256)           // 256 bytes

// Interrupt Enable Register (RW)
#define IEN_ALL                     BITS(0,9)       // All Interrupts mask.
#define IEN_CMPL                    BIT(9)          // Completion Interrupt.
#define IEN_BYTE_RECV               BIT(8)          // Byte Receive Interrupt.
#define IEN_BYTE_TRANS              BIT(7)          // Byte Transmit Interrupt.
#define IEN_START                   BIT(6)          // START Condition Interrupt.
#define IEN_STOP                    BIT(5)          // STOP Condition Interrupt.
#define IEN_ARB_LOSE                BIT(4)          // Arbitration Lose Interrupt.
#define IEN_ADDR_HIT                BIT(3)          // Address Hit Interrupt.
#define IEN_FIFO_HALF               BIT(2)          // FIFO Half Interrupt.
#define IEN_FIFO_FULL               BIT(1)          // FIFO Full Interrupt.
#define IEN_FIFO_EMPTY              BIT(0)          // FIFO Empty Interrupt.

// Status Register (RW)
#define STATUS_W1C_ALL              BITS(3,9)       // All Interrupts status write 1 clear mask.
#define STATUS_LINE_SDA             BIT(14)         // Current status of the SDA line on the bus.
#define STATUS_LINE_SCL             BIT(13)         // Current status of the SCL line on the bus.
#define STATUS_GEN_CALL             BIT(12)         // The address of the current transaction is a general call address.
#define STATUS_BUS_BUSY             BIT(11)         // The bus is busy.
#define STATUS_ACK                  BIT(10)         // The type of the last received/transmitted acknowledge bit.
#define STATUS_CMPL                 BIT(9)          // Transaction Completion
#define STATUS_BYTE_RECV            BIT(8)          // A byte of data has been received
#define STATUS_BYTE_TRANS           BIT(7)          // A byte of data has been transmitted.
#define STATUS_START                BIT(6)          // A START Condition or a repeated TART condition has been transmitted/received.
#define STATUS_STOP                 BIT(5)          // A STOP Condition has been transmitted/received.
#define STATUS_ARB_LOSE             BIT(4)          // The controller has lost the bus arbitration (master mode only).
#define STATUS_ADDR_HIT             BIT(3)          // Master: indicates that a slave has responded to the transaction
													// Slave: indicates that a transaction is targeting the controller (including the General Call).
#define STATUS_FIFO_HALF            BIT(2)          // Indicates that the FIFO is half-full or half-empty.
#define STATUS_FIFO_FULL            BIT(1)          // The FIFO is full.
#define STATUS_FIFO_EMPTY           BIT(0)          // The FIFO is empty.

// Address Register (RW)
#define SLAVE_ADDR_MSK              BITS(0,9)       // The slave address.

// Data Register (RW)
#define DATA_MSK                    BITS(0,7)       // Write this register to put one byte of data to the FIFO, Read this register to get one byte of data from the FIFO.

// Control Register (RW)
#define CTRL_PHASE_START            BIT(12)         // Enable this bit to send a START condition at the beginning of transaction, master mode only.
#define CTRL_PHASE_ADDR             BIT(11)         // Enable this bit to send the address after START condition, master mode only.
#define CTRL_PHASE_DATA             BIT(10)         // Enable this bit to send the data after Address phase, master mode only.
#define CTRL_PHASE_STOP             BIT(9)          // Enable this bit to send a STOP condition at the end of a transaction, master mode only.
#define CTRL_DIR                    BIT(8)          // Transaction direction
#define CTRL_DATA_COUNT             BITS(0,7)       // Data counts in bytes.

// Command Register (RW)
#define CMD_MSK                     BITS(0,2)       // Action command mask
#define CMD_NO_ACT                  (0x0)           // No action
#define CMD_ISSUE_TRANSACTION       (0x1)           // Issue a data transaction (Master only)
#define CMD_ACK                     (0x2)           // Respond with an ACK to the received byte
#define CMD_NACK                    (0x3)           // Respond with a NACK to the received byte
#define CMD_CLEAR_FIFO              (0x4)           // Clear the FIFO
#define CMD_RESET_I2C               (0x5)           /* Reset the I2C controller (abort current
                                                     * transaction, set the SDA and SCL line to the
                                                     * open-drain mode, reset the Status Register and
                                                     * the Interrupt Enable Register, and empty the FIFO)
                                                     */

// Setup Register (RW)
#define SETUP_T_SUDAT               BITS(24,28)     // T_SUDAT defines the data setup time before releasing the SCL.
#define SETUP_T_SP                  BITS(21,23)     // T_SP defines the pulse width of spikes that must be suppressed by the input filter.
#define SETUP_T_HDDAT               BITS(16,20)     // T_SUDAT defines the data hold time after SCL goes LOW.
#define SETUP_T_SCL_RATIO           BIT(13)         // The LOW period of the generated SCL clock is defined by the combination of T_SCLRatio and T_SCLHi values
#define SETUP_T_SCLHI               BITS(4,12)      // The HIGH period of generated SCL clock is defined by T_SCLHi.
#define SETUP_DMA_EN                BIT(3)          // Enable the direct memory access mode data transfer.
#define SETUP_MASTER                BIT(2)          // Configure this device as a master or a slave.
#define SETUP_ADDRESSING            BIT(1)          // I2C addressing mode: 10-bit or 7-bit addressing mode
#define SETUP_I2C_EN                BIT(0)          // Enable the I2C controller.


#if APB_CLK_200MHZ
// APB clock == 200MHz
// Computed as:
// APB clock period (tpclk) == 1/200MHz = 5ns
// TPM = 1
// I2C mode: standard-mode, fast-mode, fast-mode-plus, all based on APB clock (200MHZ)
// Standard-mode
#define SETUP_T_SUDAT_STD           (17)            // Data setup time, (2 * 5ns) + (2 + 5 + T_SUDAT) * 5ns * (1 + 1) >= 250ns, Minimum 250ns
#define SETUP_T_SP_STD              (5)             // Spikes time, T_SP * 5ns * (1 + 1) <= 50ns, Max 50ns
#define SETUP_T_HDDAT_STD           (22)            // Data hold time, (2 * 5ns) + (2 + 5 + T_HDDAT) * 5ns * (1 + 1) >= 300ns, Minimum 300ns
#define SETUP_T_SCL_RATIO_STD       (2)             // Ratio, (2 * 5ns) + (2 + 5 + 392 * ratio) * 5ns * (1 + 1) >= 4700ns, SCL Low Minimum 4700ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_STD           (392)           // SCL high period, (2 * 5ns) + (2 + 5 + T_SCLHI) * 5ns * (1 + 1) >= 4000ns, SCL High Minimum 4000ns
// Fast-mode
#define SETUP_T_SUDAT_FAST          (2)             // Data setup time, (2 * 5ns) + (2 + 5 + T_SUDAT) * 5ns * (1 + 1) >= 100ns, Minimum 100ns
#define SETUP_T_SP_FAST             (5)             // Spikes time, T_SP * 5ns * (1 + 1) <=  50ns, Max 50ns
#define SETUP_T_HDDAT_FAST          (22)            // Data hold time, (2 * 5ns) + (2 + 5 + T_HDDAT) * 5ns * (1 + 1) >= 300ns, Minimum 300ns
#define SETUP_T_SCL_RATIO_FAST      (3)             // Ratio, (2 * 5ns) + (2 + 5 + 52 * ratio) * 5ns * (1 + 1) >= 1300ns, SCL Low Minimum 1300ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST          (52)            // SCL high period, (2 * 5ns) + (2 + 5 + T_SCLHI) * 5ns * (1 + 1) >= 600ns, SCL High Minimum 600ns
// Fast-mode plus
#define SETUP_T_SUDAT_FAST_P        (0)             // Data setup time, (2 * 5ns) + (2 + 5 + T_SUDAT) * 5ns * (1 + 1) >= 50ns, Minimum 50ns
#define SETUP_T_SP_FAST_P           (5)             // Spikes time, T_SP * 5ns * (1 + 1) <= 50ns, Max 50ns
#define SETUP_T_HDDAT_FAST_P        (0)             // Data hold time, (2 * 5ns) + (2 + 5 + T_HDDAT) * 5ns * (1 + 1) >= 0ns, Minimum 0ns
#define SETUP_T_SCL_RATIO_FAST_P    (3)             // Ratio, (2 * 5ns) + (2 + 5 + 18 * ratio) * 5ns * (1 + 1) >= 500ns, SCL Low Minimum 500ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST_P        (18)            // SCL high period, (2 * 5ns) + (2 + 5 + T_SCLHI) * 5ns * (1 + 1) >= 260ns, SCL High Minimum 260ns

#elif APB_CLK_125MHZ
// APB clock == 125MHz
// Computed as:
// APB clock period (tpclk) == 1/125MHz = 8ns
// TPM = 1
// I2C mode: standard-mode, fast-mode, fast-mode-plus, all based on APB clock (125MHZ)
// Standard-mode
#define SETUP_T_SUDAT_STD           (10)            // Data setup time, (2 * 8ns) + (2 + 3 + T_SUDAT) * 8ns * (1 + 1) >= 250ns, Minimum 250ns
#define SETUP_T_SP_STD              (3)             // Spikes time, T_SP * 8ns * (1 + 1) <= 50ns, Max 50ns
#define SETUP_T_HDDAT_STD           (13)            // Data hold time, (2 * 8ns) + (2 + 3 + T_HDDAT) * 8ns * (1 + 1) >= 300ns, Minimum 300ns
#define SETUP_T_SCL_RATIO_STD       (2)             // Ratio, (2 * 8ns) + (2 + 3 + 243 * ratio) * 8ns * (1 + 1) >= 4700ns, SCL Low Minimum 4700ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_STD           (243)           // SCL high period, (2 * 8ns) + (2 + 3 + T_SCLHI) * 8ns * (1 + 1) >= 4000ns, SCL High Minimum 4000ns
// Fast-mode
#define SETUP_T_SUDAT_FAST          (1)             // Data setup time, (2 * 8ns) + (2 + 3 + T_SUDAT) * 8ns * (1 + 1) >= 100ns, Minimum 100ns
#define SETUP_T_SP_FAST             (3)             // Spikes time, T_SP * 8ns * (1 + 1) <=  50ns, Max 50ns
#define SETUP_T_HDDAT_FAST          (13)            // Data hold time, (2 * 8ns) + (2 + 3 + T_HDDAT) * 8ns * (1 + 1) >= 300ns, Minimum 300ns
#define SETUP_T_SCL_RATIO_FAST      (3)             // Ratio, (2 * 8ns) + (2 + 3 + 32 * ratio) * 8ns * (1 + 1) >= 1300ns, SCL Low Minimum 1300ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST          (32)            // SCL high period, (2 * 8ns) + (2 + 3 + T_SCLHI) * 8ns * (1 + 1) >= 600ns, SCL High Minimum 600ns
// Fast-mode plus
#define SETUP_T_SUDAT_FAST_P        (0)             // Data setup time, (2 * 8ns) + (2 + 3 + T_SUDAT) * 8ns * (1 + 1) >= 50ns, Minimum 50ns
#define SETUP_T_SP_FAST_P           (3)             // Spikes time, T_SP * 8ns * (1 + 1) <= 50ns, Max 50ns
#define SETUP_T_HDDAT_FAST_P        (0)             // Data hold time, (2 * 8ns) + (2 + 3 + T_HDDAT) * 8ns * (1 + 1) >= 0ns, Minimum 0ns
#define SETUP_T_SCL_RATIO_FAST_P    (3)             // Ratio, (2 * 8ns) + (2 + 3 + 11 * ratio) * 8ns * (1 + 1) >= 500ns, SCL Low Minimum 500ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST_P        (11)            // SCL high period, (2 * 8ns) + (2 + 3 + T_SCLHI) * 8ns * (1 + 1) >= 260ns, SCL High Minimum 260ns

#elif APB_CLK_100MHZ
// APB clock == 100MHz
// Computed as:
// APB clock period (tpclk) == 1/100MHz = 10ns
// TPM = 0
// I2C mode: standard-mode, fast-mode, fast-mode-plus, all based on APB clock (100MHZ)
// Standard-mode
#define SETUP_T_SUDAT_STD           (16)            // Data setup time, (2 * 10ns) + (2 + 5 + T_SUDAT) * 10ns * (0 + 1) >= 250ns, Minimum 250ns
#define SETUP_T_SP_STD              (5)             // Spikes time, T_SP * 10ns * (0 + 1) <= 50ns, Max 50ns
#define SETUP_T_HDDAT_STD           (21)            // Data hold time, (2 * 10ns) + (2 + 5 + T_HDDAT) * 10ns * (0 + 1) >= 300ns, Minimum 300ns
#define SETUP_T_SCL_RATIO_STD       (1)             // Ratio, (2 * 10ns) + (2 + 5 + 491 * ratio) * 10ns * (0 + 1) >= 4700ns, SCL Low Minimum 4700ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_STD           (491)           // SCL high period, (2 * 10ns) + (2 + 5 + T_SCLHI) * 10ns * (0 + 1) >= 4000ns, SCL High Minimum 4000ns
// Fast-mode
#define SETUP_T_SUDAT_FAST          (1)             // Data setup time, (2 * 10ns) + (2 + 5 + T_SUDAT) * 10ns * (0 + 1) >= 100ns, Minimum 100ns
#define SETUP_T_SP_FAST             (5)             // Spikes time, T_SP * 10ns * (0 + 1) <=  50ns, Max 50ns
#define SETUP_T_HDDAT_FAST          (21)            // Data hold time, (2 * 10ns) + (2 + 5 + T_HDDAT) * 10ns * (0 + 1) >= 300ns, Minimum 300ns
#define SETUP_T_SCL_RATIO_FAST      (3)             // Ratio, (2 * 10ns) + (2 + 5 + 51 * ratio) * 10ns * (0 + 1) >= 1300ns, SCL Low Minimum 1300ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST          (51)            // SCL high period, (2 * 10ns) + (2 + 5 + T_SCLHI) * 10ns * (0 + 1) >= 600ns, SCL High Minimum 600ns
// Fast-mode plus
#define SETUP_T_SUDAT_FAST_P        (0)             // Data setup time, (2 * 10ns) + (2 + 5 + T_SUDAT) * 10ns * (0 + 1) >= 50ns, Minimum 50ns
#define SETUP_T_SP_FAST_P           (5)             // Spikes time, T_SP * 10ns * (0 + 1) <= 50ns, Max 50ns
#define SETUP_T_HDDAT_FAST_P        (0)             // Data hold time, (2 * 10ns) + (2 + 5 + T_HDDAT) * 10ns * (0 + 1) >= 0ns, Minimum 0ns
#define SETUP_T_SCL_RATIO_FAST_P    (3)             // Ratio, (2 * 10ns) + (2 + 5 + 17 * ratio) * 10ns * (0 + 1) >= 500ns, SCL Low Minimum 500ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST_P        (17)            // SCL high period, (2 * 10ns) + (2 + 5 + T_SCLHI) * 10ns * (0 + 1) >= 260ns, SCL High Minimum 260ns

#elif APB_CLK_50MHZ
// APB clock == 50MHz
// Computed as:
// APB clock period (tpclk) == 1/50MHz = 20ns
// TPM = 0
// I2C mode: standard-mode, fast-mode, fast-mode-plus, all based on APB clock (50MHZ)
// Standard-mode
#define SETUP_T_SUDAT_STD           (7)             // Data setup time, (2 * 20ns) + (2 + 2 + T_SUDAT) * 20ns * (0 + 1) >= 250ns, Minimum 250ns
#define SETUP_T_SP_STD              (2)             // Spikes time, T_SP * 20ns * (0 + 1) <= 50ns, Max 50ns
#define SETUP_T_HDDAT_STD           (9)             // Data hold time, (2 * 20ns) + (2 + 2 + T_HDDAT) * 20ns * (0 + 1) >= 300ns, Minimum 300ns
#define SETUP_T_SCL_RATIO_STD       (3)             // Ratio, (2 * 20ns) + (2 + 2 + 194 * ratio) * 20ns * (0 + 1) >= 4700ns, SCL Low Minimum 4700ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_STD           (194)           // SCL high period, (2 * 20ns) + (2 + 2 + T_SCLHI) * 20ns * (0 + 1) >= 4000ns, SCL High Minimum 4000ns
// Fast-mode
#define SETUP_T_SUDAT_FAST          (0)             // Data setup time, (2 * 20ns) + (2 + 2 + T_SUDAT) * 20ns * (0 + 1) >= 100ns, Minimum 100ns
#define SETUP_T_SP_FAST             (2)             // Spikes time, T_SP * 20ns * (0 + 1) <=  50ns, Max 50ns
#define SETUP_T_HDDAT_FAST          (9)             // Data hold time, (2 * 20ns) + (2 + 2 + T_HDDAT) * 20ns * (0 + 1) >= 300ns, Minimum 300ns
#define SETUP_T_SCL_RATIO_FAST      (3)             // Ratio, (2 * 20ns) + (2 + 2 + 24 * ratio) * 20ns * (0 + 1) >= 1300ns, SCL Low Minimum 1300ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST          (24)            // SCL high period, (2 * 20ns) + (2 + 2 + T_SCLHI) * 20ns * (0 + 1) >= 600ns, SCL High Minimum 600ns
// Fast-mode plus
#define SETUP_T_SUDAT_FAST_P        (0)             // Data setup time, (2 * 20ns) + (2 + 2 + T_SUDAT) * 20ns * (0 + 1) >= 50ns, Minimum 50ns
#define SETUP_T_SP_FAST_P           (2)             // Spikes time, T_SP * 20ns * (0 + 1) <= 50ns, Max 50ns
#define SETUP_T_HDDAT_FAST_P        (0)             // Data hold time, (2 * 20ns) + (2 + 2 + T_HDDAT) * 20ns * (0 + 1) >= 0ns, Minimum 0ns
#define SETUP_T_SCL_RATIO_FAST_P    (3)             // Ratio, (2 * 20ns) + (2 + 2 + 7 * ratio) * 20ns * (0 + 1) >= 500ns, SCL Low Minimum 500ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST_P        (7)             // SCL high period, (2 * 20ns) + (2 + 2 + T_SCLHI) * 20ns * (0 + 1) >= 260ns, SCL High Minimum 260ns

#endif


#define PARA_IGNORE                 (0)

typedef enum _I2C_CTRL_REG_ITEM_DIR
{
	I2C_MASTER_TX = 0x0,
	I2C_MASTER_RX = 0x1,
	I2C_SLAVE_TX = 0x1,
	I2C_SLAVE_RX = 0x0,
} I2C_CTRL_REG_ITEM_DIR;

// I2C driver running state
typedef enum _I2C_DRIVER_STATE
{
	I2C_DRV_NONE = 0x0,
	I2C_DRV_INIT = BIT(0),
	I2C_DRV_POWER = BIT(1),
	I2C_DRV_CFG_PARAM = BIT(2),
	I2C_DRV_MASTER_TX = BIT(3),
	I2C_DRV_MASTER_RX = BIT(4),
	I2C_DRV_SLAVE_TX = BIT(5),
	I2C_DRV_SLAVE_RX = BIT(6),
	I2C_DRV_MASTER_TX_CMPL = BIT(7),
	I2C_DRV_MASTER_RX_CMPL = BIT(8),
	I2C_DRV_SLAVE_TX_CMPL = BIT(9),
	I2C_DRV_SLAVE_RX_CMPL = BIT(10),
} I2C_DRIVER_STATE;

#define MEMSET(s, c, n)         __builtin_memset ((s), (c), (n))
#define MEMCPY(des, src, n)     __builtin_memcpy((des), (src), (n))

// I2C information
typedef struct _I2C_INFO
{
	AE350_I2C_SignalEvent_t         Sig_Evt_CB;
	AE350_I2C_CAPABILITIES          I2C_Cap;
	volatile I2C_DRIVER_STATE       Driver_State;
	AE350_POWER_STATE               Pwr_State;
	uint8_t*                        middleware_rx_buf;
	uint8_t*                        middleware_tx_buf;
	/* flags for flow control */
	// Salve mode only
	uint32_t                        last_rx_data_count;
	// Xfer_Data_Rd_Buf[] for middle ware read out, salve mode only
	uint32_t                        middleware_rx_ptr;
	// Salve mode only
	uint8_t                         nack_assert;
	uint32_t                        FIFO_Depth;
	uint32_t                        Slave_Addr;
	uint32_t                        Xfer_Wt_Num;
	uint32_t                        Xfer_Rd_Num;
	// Write pointer
	uint32_t                        Xfered_Data_Wt_Ptr;
	// Read pointer
	uint32_t                        Xfered_Data_Rd_Ptr;
	// Read pointer overwrite
	uint32_t                        Xfered_Data_Rd_Ptr_Ow;
	uint8_t                         Xfer_Data_Rd_Buf[MAX_XFER_SZ];
	uint32_t                        Slave_Rx_Cmpl_Ctrl_Reg_Val;
	volatile AE350_I2C_STATUS       Status;
	uint32_t                        Xfer_Cmpl_Count;
} I2C_INFO;

// I2C DMA
typedef const struct _I2C_DMA
{
	uint8_t                 channel;          // DMA Channel
	uint8_t                 reqsel;           // DMA request selection
	DMA_SignalEvent_t       cb_event;         // DMA Event callback
} I2C_DMA;

// I2C Resource Configuration
typedef struct
{
	I2C_RegDef*		reg;                // I2C register interface
	I2C_DMA*		dma_tx;				// DMA transmit
	I2C_DMA*		dma_rx;				// DMA receive
	I2C_INFO*		info;               // Run-Time control information
} const I2C_RESOURCES;


#endif /* __I2C_AE350_H__ */
