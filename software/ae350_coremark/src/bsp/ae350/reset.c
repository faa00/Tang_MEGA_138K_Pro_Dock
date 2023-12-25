/*
 * ******************************************************************************************
 * File		: reset.c
 * Author	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Boot into main function through start up reset
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "config.h"

#ifdef CFG_GCOV
#include <stdlib.h>
#endif


// Declarations -----------------------------------------------------------------------------
extern void c_startup(void);
extern void system_init(void);
extern void __libc_init_array(void);
extern void __libc_fini_array(void);


// Definitions ------------------------------------------------------------------------------

__attribute__((weak)) void reset_handler(void)
{
	extern int main(void);

	/*
	 * Initialize LMA/VMA sections.
	 * Relocation for any sections that need to be copied from LMA to VMA.
	 */
	c_startup();

	/* Call platform specific hardware initialization */
	system_init();

	/* Do global constructors */
	__libc_init_array();

#ifdef CFG_GCOV
	/* Register global destructors to be called upon exit */
	atexit(__libc_fini_array);

	/* Entry function */
	exit(main());
#else
	/* Entry function */
	main();
#endif
}

/*
 * When compiling C++ code with static objects, the compiler inserts
 * a call to __cxa_atexit() with __dso_handle as one of the arguments.
 * The dummy versions of these symbols should be provided.
 */
void __cxa_atexit(void (*arg1)(void*), void* arg2, void* arg3)
{
}

__attribute__((weak)) void *__dso_handle = (void*) &__dso_handle;
