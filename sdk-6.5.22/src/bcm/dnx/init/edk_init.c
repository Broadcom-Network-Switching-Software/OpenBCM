/** \file edk_init.c
 *
 * Define the functions required for EDK init, deinit and SW states management
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/init/edk_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/swstate/auto_generated/access/edk_access.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
/*
 * }
 */

#ifdef EDKHLIB
#define EDK_SEQ_MAX_BYTES    DNXC_SW_STATE_ALLOC_MAX_BYTES
int *edk_cache[EDK_SEQ_MAX];
extern int edk_main(int argc, char *argv[]);
int _edk_sdk_scache_get(int unit, int seq, int alloc, int size, char **scache)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_WARM_BOOT_SUPPORT
    int *scache_ptr;

    if ((size > EDK_SEQ_MAX_BYTES) || (seq >= EDK_SEQ_MAX)) {
        cli_out("edk_init:: _edk_sdk_scache_get ERROR in param\n");
        rv = BCM_E_PARAM;
        goto exit;
    }

    if (edk_cache[seq] == NULL) {
        edk_cache[seq] = sal_alloc(EDK_SEQ_MAX_BYTES, "EDK");
        if(edk_cache[seq] == NULL) {
            cli_out("edk_init:: _edk_sdk_scache_get ERROR in alloc\n");
            rv = BCM_E_MEMORY;
            goto exit;
        }
    }

    /* Read from the cache for warmboot */
    if ((alloc == 0) && (size == 0)) {
        scache_ptr = edk_cache[seq];
        size = EDK_SEQ_MAX_BYTES/sizeof(int);
        for (int idx = 0; idx < size; idx++) {
            if (edk_state.edk_seq.get(unit, seq, idx, scache_ptr) < 0) {
                cli_out("edk_init:: _edk_sdk_scache_get ERROR in read seq:%d idx:%d\n", seq, idx);
                rv = BCM_E_INTERNAL;
                goto exit;
            }
            scache_ptr++;
        }
    }

    *scache = (char *)edk_cache[seq];
    rv = BCM_E_NONE;

exit:
#endif
    return rv;
}

int _edk_sdk_scache_sync(int unit, int seq, int size, char *scache)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_WARM_BOOT_SUPPORT
    int *scache_ptr;

    if ((size > EDK_SEQ_MAX_BYTES) || (seq >= EDK_SEQ_MAX)) {
        cli_out("edk_init:: _edk_sdk_scache_sync ERROR in param check\n");
        rv = BCM_E_PARAM;
        goto exit;
    }

    size = size/sizeof(int);
    scache_ptr = (int *)scache;
    if (size) {
        /* Read from the cache for warmboot */
        for (int idx = 0; idx < size; idx++) {
            if (edk_state.edk_seq.set(unit, seq, idx, *scache_ptr) < 0) {
                cli_out("edk_init:: _edk_sdk_scache_set ERROR in write seq:%d idx:%d\n", seq, idx);
                rv = BCM_E_INTERNAL;
                goto exit;
            }
            scache_ptr++;
        }
    }
    rv = BCM_E_NONE;

exit:
#endif
    return rv;
}
#endif

shr_error_e
dnx_edk_state_init(int unit)
{
    int rv = BCM_E_NONE;
#ifdef EDKHLIB
    int size = EDK_SEQ_MAX_BYTES;

    for (int i = 0; i < EDK_SEQ_MAX; i++) {
        edk_cache[i] = NULL;
    }

    if (!sw_state_is_warm_boot(unit)) {

        /* Software state init */
        rv = edk_state.init(unit);
        if (BCM_FAILURE(rv)) {
            cli_out("edk_init:: dnx_edk_state_init ERROR in SW-State init\n");
            goto exit;
        }

        size = size/sizeof(int);
        for(int idx = 0; idx < EDK_SEQ_MAX; idx++) {
            if (edk_state.edk_seq.alloc(unit, idx, size) < 0) {
                cli_out("edk_init:: dnx_edk_state_init ERROR in SW-State alloc for idx:%d size:%d\n", idx, size);
                rv = BCM_E_INIT;
                goto exit;
            }
        }
    } else {
#ifdef BCM_WARM_BOOT_SUPPORT
        char* argv_list[2] = {"edk_main", "wb"};
        int argc = 2;
        uint8 is_init;

        /* Get the state of init, as during warmboot the framework will initialize */
        if (edk_state.is_init(unit, &is_init) < 0) {
            cli_out("edk_init:: dnx_edk_state_init ERROR in is_init() on WB\n");
            rv = BCM_E_INIT;
            goto exit;
        }

        /* Get the state of init */
        if (is_init != TRUE) {
            cli_out("edk_init:: dnx_edk_state_init ERROR SW-state NOT initialized on WB\n");
            rv = BCM_E_INIT;
            goto exit;
        }

        /* Warmboot EDK-Host */
        rv = edk_main(argc, argv_list);
        if (BCM_FAILURE(rv)) {
            cli_out("edk_init:: dnx_edk_state_init ERROR in edk_main(), ret:%d\n", rv);
            rv = BCM_E_INIT;
            goto exit;
        }
#endif  /* BCM_WARM_BOOT_SUPPORT */
    }

exit:
#endif  /* EDKHLIB */
    return rv;
}

shr_error_e
dnx_edk_state_deinit(int unit)
{
#ifdef EDKHLIB
    char* argv_list[2] = {"edk_main", "exit"};
    int argc = 2;

    (void)edk_main(argc, argv_list);
#endif
    return BCM_E_NONE;
}
