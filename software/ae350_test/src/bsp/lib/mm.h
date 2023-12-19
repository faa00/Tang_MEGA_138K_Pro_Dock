/*
 * ******************************************************************************************
 * File		: mm.h
 * Author 	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Memory management
 * ******************************************************************************************
 */

#ifndef __MM_H__
#define __MM_H__


// Declarations -----------------------------------------------------------------------------

extern void mem_init(void);											// Initialize
extern void* mem_malloc(unsigned int size);							// Allocate
extern void mem_free(void *ptr);									// Free
extern void mem_set(void *s, unsigned char c, unsigned int count);	// Set
extern void mem_cpy(void *des, void *src, unsigned int n);			// Copy
extern int mem_cmp(void *des, void *src, unsigned int n);			// Compare 1: not equal; 0: equal


#endif	/* __MM_H__ */
