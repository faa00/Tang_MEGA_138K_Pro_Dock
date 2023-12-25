/*
 * ******************************************************************************************
 * File		: clic.h
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Core local interrupt controller
 * ******************************************************************************************
 */

#ifndef __CLIC_H__
#define __CLIC_H__


#ifdef __cplusplus
extern "C" {
#endif


// Definitions -------------------------------------------------------------------------------

/* A fixed parameter of the CLIC (CLICINTCTLBITS) defines how many total bits are present for the
   level plus priority in the clicintctl registers , with 2 <= CLICINTCTLBITS <= 8 */
#define CLICINTCTLBITS                                     8

/* cliccfg register - 8 bits  */
#define CLIC_CFG_OFFSET                                    (0x00000000UL)

/* clicinfo register - 32 bits  */
#define CLIC_INFO_OFFSET                                   (0x00000004UL)

/* mintthresh.uth register - 8 bits  */
#define CLIC_THRESHOLD_UTH_OFFSET                          (0x00000008UL)

/* mintthresh.sth register - 8 bits  */
#define CLIC_THRESHOLD_STH_OFFSET                          (0x00000009UL)

/* mintthresh.mth register - 8 bits  */
#define CLIC_THRESHOLD_MTH_OFFSET                          (0x0000000BUL)

/* clicintip register - 8 bits per source */
#define CLIC_INTIP_OFFSET                                  (0x00001000UL)
#define CLIC_INTIP_SHIFT_PER_SOURCE                        2

/* clicintie register - 8 bits per source */
#define CLIC_INTIE_OFFSET                                  (0x00001001UL)
#define CLIC_INTIE_SHIFT_PER_SOURCE                        2

/* clicintattr register - 8 bits per source */
#define CLIC_INTATTR_OFFSET                                (0x00001002UL)
#define CLIC_INTATTR_SHIFT_PER_SOURCE                      2

/* clicintctl register - 8 bits per source */
#define CLIC_INTCTL_OFFSET                                 (0x00001003UL)
#define CLIC_INTCTL_SHIFT_PER_SOURCE                       2

/* cliccfg register - 8 bits  */
#define CLICCFG_offNMBITS                                  (5)
#define CLICCFG_offNLBITS                                  (1)
#define CLICCFG_mskNMBITS                                  (0x3 << CLICCFG_offNMBITS)
#define CLICCFG_mskNLBITS                                  (0xF << CLICCFG_offNLBITS)

/* clicintattr register - 8 bits  */
#define CLICATTR_offMODE                                   (6)
#define CLICATTR_offTRIG                                   (1)
#define CLICATTR_offSHV                                    (0)
#define CLICATTR_mskMODE                                   (0x3 << CLICATTR_offMODE)
#define CLICATTR_mskTRIG                                   (0x3 << CLICATTR_offTRIG)
#define CLICATTR_mskSHV                                    (0x1 << CLICATTR_offSHV)
#define NDS_CLIC_TRIG_LEVEL                                0x0
#define NDS_CLIC_TRIG_EDGE_POSITIVE                        0x1
#define NDS_CLIC_TRIG_EDGE_NEGATIVE                        0x3


#ifdef NDS_CLIC_BASE

__attribute__((always_inline)) static inline void __nds__clic_set_cliccfg (unsigned char nmbits, unsigned char nlbits)
{
	volatile unsigned char *cfg_ptr = (volatile unsigned char *)(NDS_CLIC_BASE + CLIC_CFG_OFFSET);

	/* The 4-bit cliccfg.nlbits WARL field indicates how many bits encode the level at which the
	   interrupt is taken. Valid values are 0 - 8.*/
	if (nlbits >= CLICINTCTLBITS)
	{
		nlbits = CLICINTCTLBITS;
	}

	*cfg_ptr = (((*cfg_ptr) & (~(CLICCFG_mskNMBITS | CLICCFG_mskNLBITS))) | (nmbits << CLICCFG_offNMBITS) | (nlbits << CLICCFG_offNLBITS));
}

__attribute__((always_inline)) static inline unsigned char __nds__clic_get_cliccfg (void)
{
	volatile unsigned char *cfg_ptr = (volatile unsigned char *)(NDS_CLIC_BASE + CLIC_CFG_OFFSET);

	return *cfg_ptr;
}

__attribute__((always_inline)) static inline unsigned int __nds__clic_get_info (void)
{
	volatile unsigned int *info_ptr = (volatile unsigned int *)(NDS_CLIC_BASE + CLIC_INFO_OFFSET);

	return *info_ptr;
}

/* mintthresh.mth register only includes interrupt level, the format is:
        Bits                Field
        7 -: nlbits         level
        7-nlbits : 0        reserved
*/
__attribute__((always_inline)) static inline void __nds__clic_set_mmode_threshold (unsigned char mth)
{
	/* The interrupt-level threshold (mintthresh) is a new read/write CSR, which holds an 8-bit field
	   for each privilege mode (i.e. mth, hsth, sth, uth), holding its current threshold level, and add
	   some support CSR addresses that allow the field to be read/written independently */
	/* The current hart's effective interrupt level would then be:
	   effective_level = max( minstatus.mil, mintthresh.mth ) */
	volatile unsigned char *thresh_m_ptr = (volatile unsigned char *)(NDS_CLIC_BASE + CLIC_THRESHOLD_MTH_OFFSET);
	volatile unsigned char *cfg_ptr = (volatile unsigned char *)(NDS_CLIC_BASE + CLIC_CFG_OFFSET);

	unsigned char nlbits = (((*cfg_ptr) & CLICCFG_mskNLBITS) >>  CLICCFG_offNLBITS);

	/* non-existent bits for threshold encoding, HW treat as reserved */
	mth <<= (8 - nlbits);

	*thresh_m_ptr = mth;
}

__attribute__((always_inline)) static inline void __nds__clic_set_pending (unsigned int source)
{
	volatile unsigned char *pend_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                         	      CLIC_INTIP_OFFSET +
                                                         	      (source << CLIC_INTIP_SHIFT_PER_SOURCE));

	*pend_ptr = 1;
}

__attribute__((always_inline)) static inline void __nds__clic_enable_interrupt (unsigned int source)
{
	volatile unsigned char *enable_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                                    CLIC_INTIE_OFFSET +
                                                                    (source << CLIC_INTIE_SHIFT_PER_SOURCE));

	*enable_ptr = 1;
}

__attribute__((always_inline)) static inline void __nds__clic_disable_interrupt (unsigned int source)
{
	volatile unsigned char *enable_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                                    CLIC_INTIE_OFFSET +
                                                                    (source << CLIC_INTIE_SHIFT_PER_SOURCE));

	*enable_ptr = 0;
}

/* Attribute register includes trig, shv, mode fields, the format is:
        Bits           Field
        7:6            mode
        5:3            reserved
        2:1            trig
        0              shv
*/
__attribute__((always_inline)) static inline void __nds__clic_set_mode (unsigned int source, unsigned char mode)
{
	volatile unsigned char *mode_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                                  CLIC_INTATTR_OFFSET +
                                                                  (source << CLIC_INTATTR_SHIFT_PER_SOURCE));

	*mode_ptr = ((*mode_ptr & (~ CLICATTR_mskMODE)) | (mode << CLICATTR_offMODE));
}

__attribute__((always_inline)) static inline unsigned char __nds__clic_get_mode (unsigned int source)
{
	volatile unsigned char *mode_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                                  CLIC_INTATTR_OFFSET +
                                                                  (source << CLIC_INTATTR_SHIFT_PER_SOURCE));

	return ((*mode_ptr & CLICATTR_mskMODE) >> CLICATTR_offMODE);
}

__attribute__((always_inline)) static inline void __nds__clic_set_trig (unsigned int source, unsigned char trig)
{
	volatile unsigned char *trig_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                                  CLIC_INTATTR_OFFSET +
                                                                  (source << CLIC_INTATTR_SHIFT_PER_SOURCE));

	*trig_ptr = ((*trig_ptr & (~ CLICATTR_mskTRIG)) | (trig << CLICATTR_offTRIG));
}

__attribute__((always_inline)) static inline unsigned char __nds__clic_get_trig (unsigned int source)
{
	volatile unsigned char *trig_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                                  CLIC_INTATTR_OFFSET +
                                                                  (source << CLIC_INTATTR_SHIFT_PER_SOURCE));

	return ((*trig_ptr & CLICATTR_mskTRIG) >> CLICATTR_offTRIG);
}

__attribute__((always_inline)) static inline void __nds__clic_set_shv (unsigned int source, unsigned char shv)
{
	volatile unsigned char *shv_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                                 CLIC_INTATTR_OFFSET +
                                                                 (source << CLIC_INTATTR_SHIFT_PER_SOURCE));

	*shv_ptr = ((*shv_ptr & (~ CLICATTR_mskSHV)) | (shv << CLICATTR_offSHV));
}

__attribute__((always_inline)) static inline unsigned char __nds__clic_get_shv (unsigned int source)
{
	volatile unsigned char *shv_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                                 CLIC_INTATTR_OFFSET +
                                                                 (source << CLIC_INTATTR_SHIFT_PER_SOURCE));

	return ((*shv_ptr & CLICATTR_mskSHV) >> CLICATTR_offSHV);
}

/* Control register includes level, priority fields, the format is:
        Bits                                          Field
        7 -: nlbits                                   level
        (7-nlbits) -: (CLICINTCTLBITS-nlbits)         priority
        0 +: 8-CLICINTCTLBITS                         reserved #every bit is hardwared to 1
*/
__attribute__((always_inline)) static inline void __nds__clic_set_level (unsigned int source, unsigned char level)
{
	volatile unsigned char *cfg_ptr = (volatile unsigned char *)(NDS_CLIC_BASE + CLIC_CFG_OFFSET);
	volatile unsigned char *level_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
								       	   	   	   	   	   	   	   CLIC_INTCTL_OFFSET +
	                                                               (source << CLIC_INTCTL_SHIFT_PER_SOURCE));

	unsigned char nlbits = (((*cfg_ptr) & CLICCFG_mskNLBITS) >>  CLICCFG_offNLBITS);

	/* non-existent bits for level encoding, HW treat as 1 */
	*level_ptr = (((*level_ptr << nlbits) >> nlbits) | (level << (8 - nlbits)));
}

__attribute__((always_inline)) static inline unsigned char __nds__clic_get_level (unsigned int source)
{
	volatile unsigned char *cfg_ptr = (volatile unsigned char *)(NDS_CLIC_BASE + CLIC_CFG_OFFSET);
	volatile unsigned char *level_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
								       	   	   	   	   	   	   	   CLIC_INTCTL_OFFSET +
	                                                               (source << CLIC_INTCTL_SHIFT_PER_SOURCE));

	unsigned char nlbits = (((*cfg_ptr) & CLICCFG_mskNLBITS) >>  CLICCFG_offNLBITS);

	return (*level_ptr >> (8 - nlbits));
}

__attribute__((always_inline)) static inline void __nds__clic_set_priority (unsigned int source, unsigned char priority)
{
	volatile unsigned char *cfg_ptr = (volatile unsigned char *)(NDS_CLIC_BASE + CLIC_CFG_OFFSET);
	volatile unsigned char *priority_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                                      CLIC_INTCTL_OFFSET +
                                                                      (source << CLIC_INTCTL_SHIFT_PER_SOURCE));

	unsigned char nlbits = (((*cfg_ptr) & CLICCFG_mskNLBITS) >>  CLICCFG_offNLBITS);

	if (!(CLICINTCTLBITS - nlbits))
	{
		return;
	}

	/* non-existent bits for priority encoding, HW treat as 1 */
	*priority_ptr = (((*priority_ptr >> (8 - nlbits)) << (8 - nlbits)) | (priority << (8 - CLICINTCTLBITS)));
}

__attribute__((always_inline)) static inline unsigned char __nds__clic_get_priority (unsigned int source)
{
	volatile unsigned char *cfg_ptr = (volatile unsigned char *)(NDS_CLIC_BASE + CLIC_CFG_OFFSET);
	volatile unsigned char *priority_ptr = (volatile unsigned char *)(NDS_CLIC_BASE +
                                                                      CLIC_INTCTL_OFFSET +
                                                                      (source << CLIC_INTCTL_SHIFT_PER_SOURCE));

	unsigned char nlbits = (((*cfg_ptr) & CLICCFG_mskNLBITS) >>  CLICCFG_offNLBITS);

	if (!(CLICINTCTLBITS - nlbits))
	{
		return 0;
	}

	return (((*priority_ptr << nlbits) >> nlbits) >> (8 - CLICINTCTLBITS));
}

#endif	// NDS_CLIC_BASE


#ifdef __cplusplus
}
#endif


#endif	/* __CLIC_H__ */
