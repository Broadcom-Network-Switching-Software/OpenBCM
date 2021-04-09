/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * File: edk_init.c
 * Purpose: EDK support
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <appl/diag/shell.h>
#include <soc/defs.h>

#ifdef BCM_EDK_SUPPORT
#if (!defined(LINUX) && !defined(UNIX))
#error "EDK-Host cannot be built on non-Linux/UNIX platforms"
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif /* Linux/UNIX */

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif
#include <soc/mem.h>
#include <bcm_int/esw/switch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_WB_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION  BCM_WB_VERSION_1_0
#endif  /* BCM_WARM_BOOT_SUPPORT */

#ifdef EDKHLIB
int _edk_sdk_scache_get(int unit, int seq, int alloc, int size, char **scache)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_WARM_BOOT_SUPPORT
    soc_scache_handle_t scache_handle;
    uint8  *edk_scache;
    uint16 recovered_ver = 0;

    if (scache) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_EDK_HANDLE, seq);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, alloc, size,
                                      &edk_scache, BCM_WB_DEFAULT_VERSION,
                                      size ? NULL : &recovered_ver);
        if (BCM_FAILURE(rv)) {
            cli_out("edk_init:: EDK cache handle get ERROR\n");
            goto exit;
        }
        *scache = (char *)edk_scache;
    }
    rv = BCM_E_NONE;

exit:
#endif
    return rv;
}

int _edk_sdk_scache_sync(int unit, int seq, int size, char *scache)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_WARM_BOOT_SUPPORT
    soc_scache_handle_t scache_handle;
    uint8 *edk_scache;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_EDK_HANDLE, seq);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, size,
                             &edk_scache, BCM_WB_DEFAULT_VERSION,
                             NULL);
    if (BCM_FAILURE(rv)) {
        cli_out("edk_init:: EDK cache sync ERROR\n");
        goto exit;
    }

    sal_memcpy(edk_scache, scache, size);
    rv = BCM_E_NONE;

exit:
#endif  /* BCM_WARM_BOOT_SUPPORT */
    return rv;
}
#endif  /* EDKHLIB */

#else
typedef int _edk_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_EDK_SUPPORT */
