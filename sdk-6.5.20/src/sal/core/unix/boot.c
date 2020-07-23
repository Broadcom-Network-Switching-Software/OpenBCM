/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	boot.c
 * Purpose:	Kernel initialization
 */

#include <stdio.h>
#include <stdlib.h>


#include <soc/debug.h>



#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <sal/core/spl.h>
#include <sal/core/dpc.h>
#include <sal/core/sync.h>

/*
 * Function:
 *	sal_core_init
 * Purpose:
 *	Initialize the Kernel SAL
 * Returns:
 *	0 on success, -1 on failure
 */

int
sal_core_init(void)
{    
    sal_udelay(0);	/* Cause sal_udelay() to self-calibrate */

    sal_thread_main_set(sal_thread_self());

    if (sal_spl_init()) {
        return -1;
    }

    if (sal_dpc_init()) {
        return -1;
    }

    if (sal_global_lock_init()) {
        return -1;
    }

    
    if (sal_appl_global_lock_init()) {
        return -1;
    }

    return 0;
}

/*
 * Function:
 *  sal_core_cleanup
 * Purpose:
 *  Clean up resources created during sal_core_init
 * Returns:
 *  Nothing
 */
void
sal_core_cleanup(void)
{
    sal_dpc_term();

    return;
}

/*
 * Function:
 *	sal_boot_flags_get
 * Purpose:
 *	Return boot flags from startup
 *	Flags are set for PLISIM and NO_PROBE by default, and can be
 *	overridden by setting the environment variable SOC_BOOT_FLAGS.
 * Parameters:
 *	None
 * Returns:
 *	32-bit flags value, 0 if not supported or no flags set.
 */

static int	_sal_boot_flags_init = FALSE;
static uint32	_sal_boot_flags = 0;

uint32
sal_boot_flags_get(void)
{
    if (!_sal_boot_flags_init) {
	char *s = getenv("SOC_BOOT_FLAGS");
	if (s == NULL) {
#ifdef PLISIM
	    _sal_boot_flags = (BOOT_F_PLISIM | BOOT_F_NO_PROBE);
#ifdef LINUX_PLI_COMBO_BDE
            if (getenv("BCM_SIM_PATH") == NULL) {
                _sal_boot_flags = 0;
            }
#endif
#endif
	} else {
	    _sal_boot_flags = sal_ctoi(s, NULL);
	}
	_sal_boot_flags_init = TRUE;
    }

    return _sal_boot_flags;
}

/*
 * Function:
 *	sal_boot_flags_set
 * Purpose:
 *	Change boot flags
 * Parameters:
 *	flags - New boot flags
 */

void
sal_boot_flags_set(uint32 flags)
{
    _sal_boot_flags_init = TRUE;
    _sal_boot_flags = flags;
}

/*
 * Function:
 *	sal_boot_script
 * Purpose:
 *	Return name of boot script from startup
 * Parameters:
 *	None
 * Returns:
 *	Name of boot script, NULL if none
 */
char *
sal_boot_script(void)
{
    return getenv("SOC_BOOT_SCRIPT");
}


/*
 * Function:
 *      sal_os_name
 * Purpose:
 *      Provide a description of the underlying operating system
 * Parameters:
 *      None
 * Returns:
 *      String describing the OS
 */
const char* 
sal_os_name(void)
{
    return "Unix (Posix)"; 
}
