/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:     warmboot.c
 * Purpose:     Sample Warm Boot cache in the Linux filesystem.
 */


#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/scache.h>

/* Required for WB to DRAM (and not a file) */
#ifdef BCM_PETRA_SUPPORT
    #include <soc/dpp/error.h>
    #include <soc/dpp/drv.h>
    #include <soc/dpp/ARAD/arad_dram.h>
    #include <bcm_int/dpp/error.h>
#endif

#ifdef BCM_WARM_BOOT_SUPPORT

#if (defined(LINUX) && !defined(__KERNEL__)) || defined(UNIX)

#include <stdio.h>

static FILE *scache_fp[SOC_MAX_NUM_DEVICES];
                                    /* Non-NULL indicates file opened */
static sal_mutex_t file_lock[SOC_MAX_NUM_DEVICES];
#ifndef NO_FILEIO
static char *scache_nm[SOC_MAX_NUM_DEVICES];
                                    /* Non-NULL indicates file selected */
#endif

STATIC int
appl_scache_file_read_func(int unit, uint8 *buf, int offset, int nbytes)
{
    size_t result;

    if (!scache_fp[unit]) {
        return SOC_E_UNIT;
    }
    if (sal_mutex_take(file_lock[unit],sal_mutex_FOREVER))
    {
        cli_out("Unit %d: Mutex take failed\n", unit);
        return SOC_E_FAIL;
    }
    if (0 != fseek(scache_fp[unit], offset, SEEK_SET)) {
        return SOC_E_FAIL;
    }

    result = fread(buf, 1, nbytes, scache_fp[unit]);
    if (result != nbytes) {
        sal_mutex_give(file_lock[unit]);
        return SOC_E_MEMORY;
    }
    if ( sal_mutex_give(file_lock[unit]))
    {
        cli_out("Unit %d: Mutex give failed\n", unit);
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
appl_scache_file_write_func(int unit, uint8 *buf, int offset, int nbytes)
{
    size_t result;

    if (!scache_fp[unit]) {
        return SOC_E_UNIT;
    }
    if (sal_mutex_take(file_lock[unit],sal_mutex_FOREVER))
    {
        cli_out("Unit %d: Mutex take failed\n", unit);
        return SOC_E_FAIL;
    }
    if (0 != fseek(scache_fp[unit], offset, SEEK_SET)) {
        return SOC_E_FAIL;
    }

    result = fwrite(buf, 1, nbytes, scache_fp[unit]);
    if (result != nbytes) {
        if ( sal_mutex_give(file_lock[unit]))
        {
            cli_out("Unit %d: Mutex give failed\n", unit);
            return SOC_E_FAIL;
        }
        return SOC_E_MEMORY;
    }
    fflush(scache_fp[unit]);
    if ( sal_mutex_give(file_lock[unit]))
    {
        cli_out("Unit %d: Mutex give failed\n", unit);
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

int
appl_scache_file_close(int unit)
{
#ifndef NO_FILEIO
    if (!scache_nm[unit]) {
    cli_out("Unit %d: Scache file is not set\n", unit);
    return -1;
    }
    if (sal_mutex_take(file_lock[unit],sal_mutex_FOREVER))
    {
        cli_out("Unit %d: Mutex take failed\n", unit);
        return SOC_E_FAIL;
    }
    if (scache_fp[unit]) {
    sal_fclose(scache_fp[unit]);
    scache_fp[unit] = 0;
    }
    sal_free(scache_nm[unit]);
    scache_nm[unit] = NULL;
    sal_mutex_destroy(file_lock[unit]);
    file_lock[unit]=0;
#endif
    return 0;
}

int
appl_scache_file_open(int unit, int warm_boot, char *filename)
{
#ifndef NO_FILEIO
    if (scache_nm[unit]) {
    appl_scache_file_close(unit);
        scache_nm[unit] = NULL;
    }
    if (NULL == filename) {
        return 0; /* No scache file */
    }
    file_lock[unit] = sal_mutex_create("schan-file");
    if (sal_mutex_take(file_lock[unit],sal_mutex_FOREVER))
    {
        cli_out("Unit %d: Mutex take failed\n", unit);
        return SOC_E_FAIL;
    }

    if ((scache_fp[unit] =
         sal_fopen(filename, warm_boot ? "r+" : "w+")) == 0) {
    cli_out("Unit %d: Error opening scache file %s\n", unit,filename);
    return -1;
    }
    scache_nm[unit] =
        sal_strncpy((char *)sal_alloc(sal_strlen(filename) + 1, __FILE__),
                   filename, sal_strlen(filename));
    {
        if (soc_switch_stable_register(unit,
                                   &appl_scache_file_read_func,
                                   &appl_scache_file_write_func,
                                   NULL, NULL) < 0) {
            cli_out("Unit %d: soc_switch_stable_register failure\n", unit);
            return -1;
        }
    }
     if ( sal_mutex_give(file_lock[unit]))
    {
        cli_out("Unit %d: Mutex give failed\n", unit);
        return SOC_E_FAIL;
    }
#endif /* NO_FILEIO */
    return 0;
}

#else /* (defined(LINUX) && !defined(__KERNEL__)) || defined(UNIX) */

int
appl_scache_file_close(int unit)
{
    return 0;
}

int
appl_scache_file_open(int unit, int warm_boot)
{
    return 0;
}

#endif /* (defined(LINUX) && !defined(__KERNEL__)) || defined(UNIX) */

/*
 * Function:
 *     appl_warm_boot_event_handler_cb
 *
 * Purpose:
 *     Warm boot event handler call back routine for test purpose.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     event    - (IN) Switch event type
 *     argX     - (IN) Event Arguments
 *     userdata - (IN) user supplied data
 * Returns:
 *     BCM_E_XXX
 */
void
appl_warm_boot_event_handler_cb(int unit, soc_switch_event_t event,
                               uint32 arg1, uint32 arg2, uint32 arg3,
                               void *userdata)
{
    switch (event) {
        case SOC_SWITCH_EVENT_STABLE_FULL:
        case SOC_SWITCH_EVENT_STABLE_ERROR:
        case SOC_SWITCH_EVENT_UNCONTROLLED_SHUTDOWN:
            assert(0);
        case SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE:
            break;
        default:
            break;
    }
    return;
}

#ifdef BCM_PETRA_SUPPORT
int
_arad_user_buffer_dram_read_cb(
    int unit, 
    uint8 *buf, 
    int offset, 
    int nbytes) {

    int rv = BCM_E_NONE;

    rv = handle_sand_result(soc_arad_user_buffer_dram_read(
         unit, 
         0x0, 
         buf, 
         offset, 
         nbytes));

    return rv;
}

int
_arad_user_buffer_dram_write_cb(
    int unit, 
    uint8 *buf, 
    int offset, 
    int nbytes) {
    SOC_SAND_IF_ERR_RETURN(
       soc_arad_user_buffer_dram_write(
         unit, 
         0x0, 
         buf, 
         offset, 
         nbytes));

    return BCM_E_NONE;
}

/* 
 * Write function signature to register for the application provided
 * stable for Level 2 Warm Boot in user buffer (dram)
 */

int
appl_scache_user_buffer(int unit)
{
    if (soc_switch_stable_register(unit,
                                   &_arad_user_buffer_dram_read_cb,
                                   &_arad_user_buffer_dram_write_cb,
                                   NULL, NULL) < 0) {
        cli_out("Unit %d: soc_switch_stable_register failure\n", unit);
        return -1;
    }

    return 0;
}
#endif /* BCM_PETRA_SUPPORT */

#else /* BCM_WARM_BOOT_SUPPORT */
typedef int _warmboot_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_WARM_BOOT_SUPPORT */


