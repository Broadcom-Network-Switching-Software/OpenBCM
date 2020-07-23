/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	boot.h
 * Purpose: 	Boot definitions
 */

#ifndef _SAL_BOOT_H
#define _SAL_BOOT_H

#include <sal/types.h>

/*
 * Boot flags.
 */

#define BOOT_F_NO_RC            0x1000
#define BOOT_F_NO_PROBE         0x2000
#define BOOT_F_NO_ATTACH        0x4000
#define BOOT_F_SHELL_ON_TRAP    0x8000 /* TTY trap char: no reboot; shell */
#define BOOT_F_QUICKTURN        0x10000
#define BOOT_F_PLISIM           0x20000
#define BOOT_F_RTLSIM           0x80000
#define BOOT_F_RELOAD           0x100000
#define BOOT_F_WARM_BOOT        0x200000
#define BOOT_F_BCMSIM           0x400000
#define BOOT_F_XGSSIM           0x800000
#define BOOT_F_NO_INTERRUPTS    0x1000000
#define BOOT_F_FAST_REBOOT      0x2000000
#define BOOT_F_I2C_ACCESS       0x4000000
#define BOOT_F_EARLY_DBG        0x8000000

extern uint32 sal_boot_flags_get(void);
extern void sal_boot_flags_set(uint32);
extern char *sal_boot_script(void);

#define sal_boot_flags  sal_boot_flags_get  /* Deprecated */

#define SAL_BOOT_SIMULATION  \
    (sal_boot_flags_get() & (BOOT_F_BCMSIM|BOOT_F_QUICKTURN|BOOT_F_PLISIM|BOOT_F_RTLSIM|BOOT_F_XGSSIM))
#define SAL_BOOT_QUICKTURN  \
    (sal_boot_flags_get() & BOOT_F_QUICKTURN)
#define SAL_BOOT_PLISIM  \
    (sal_boot_flags_get() & (BOOT_F_PLISIM|BOOT_F_RTLSIM))
#define SAL_BOOT_RTLSIM  \
    (sal_boot_flags_get() & BOOT_F_RTLSIM)
#define SAL_BOOT_BCMSIM  \
    (sal_boot_flags_get() & BOOT_F_BCMSIM)
#define SAL_BOOT_XGSSIM  \
    (sal_boot_flags_get() & BOOT_F_XGSSIM)

#define SAL_BOOT_NO_INTERRUPTS   \
    (sal_boot_flags_get() & BOOT_F_NO_INTERRUPTS)

#define SAL_BOOT_I2C_ACCESS   \
    (sal_boot_flags_get() & BOOT_F_I2C_ACCESS)

/*
 * Assertion Handling
 */

typedef void (*sal_assert_func_t)
     (const char* expr, const char* file, int line);

extern void sal_assert_set(sal_assert_func_t f);
extern void _default_assert(const char *expr, const char *file, int line);


/*
 * Init
 */
extern int sal_core_init(void);

/*
 * Clean up resources created during sal_core_init
 */
extern void sal_core_cleanup(void);

/*
 * Returns a string describing the current Operating System
 */
extern const char* sal_os_name(void); 


#endif	/* !_SAL_BOOT_H */


