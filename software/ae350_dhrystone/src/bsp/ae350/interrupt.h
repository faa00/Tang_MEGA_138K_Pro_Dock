/*
 * ******************************************************************************************
 * File		: interrupt.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Platform level external interrupt handler
 * ******************************************************************************************
 */

// No Vectored PLIC

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__


#ifdef __cplusplus
extern "C" {
#endif


// Definitions ------------------------------------------------------------------------------

#define USE_PLIC

/*
 * Define 'NDS_PLIC_BASE' and 'NDS_PLIC_SW_BASE' before include platform
 * PLIC/PLIC_SW related intrinsic functions.
 */
#define NDS_PLIC_BASE        PLIC_BASE
#define NDS_PLIC_SW_BASE     PLIC_SW_BASE
#include "plic.h"

/*
 * CPU Machine timer control
 */
#define HAL_MTIMER_INITIAL()
#define HAL_MTIME_ENABLE()                              set_csr(NDS_MIE, MIP_MTIP)
#define HAL_MTIME_DISABLE()                             clear_csr(NDS_MIE, MIP_MTIP);

/*
 * CPU Machine SWI control
 *
 * Machine SWI (MSIP) is connected to PLIC_SW source 1.
 */
#define HAL_MSWI_INITIAL()                              \
{                                                       \
    __nds__plic_sw_set_priority(1, 1);                  \
    __nds__plic_sw_enable_interrupt(1);                 \
}
#define HAL_MSWI_ENABLE()                               set_csr(NDS_MIE, MIP_MSIP)
#define HAL_MSWI_DISABLE()                              clear_csr(NDS_MIE, MIP_MSIP)
#define HAL_MSWI_PENDING()                              __nds__plic_sw_set_pending(1)
#define HAL_MSWI_CLEAR()                                __nds__plic_sw_claim_interrupt()

/*
 * Platform defined interrupt controller access
 *
 * This uses the non vectored PLIC scheme.
 */
#define HAL_MIE_ENABLE()                                set_csr(NDS_MSTATUS, MSTATUS_MIE)               // Enable general interrupt
#define HAL_MIE_DISABLE()                               clear_csr(NDS_MSTATUS, MSTATUS_MIE)             // Disable general interrupt
#define HAL_MEIP_ENABLE()                               set_csr(NDS_MIE, MIP_MEIP)                      // Enable external interrupt
#define HAL_MEIP_DISABLE()                              clear_csr(NDS_MIE, MIP_MEIP)                    // Disable external interrupt
#define HAL_INTERRUPT_ENABLE(vector)                    __nds__plic_enable_interrupt(vector)
#define HAL_INTERRUPT_DISABLE(vector)                   __nds__plic_disable_interrupt(vector)
#define HAL_INTERRUPT_THRESHOLD(threshold)              __nds__plic_set_threshold(threshold)
#define HAL_INTERRUPT_SET_LEVEL(vector, level)          __nds__plic_set_priority(vector, level)


/*
 * Machine external interrupt handler function declaration.
 */
extern void mext_interrupt(unsigned int irq_source);


#ifdef __cplusplus
}
#endif


#endif	/* __INTERRUPT_H__ */
