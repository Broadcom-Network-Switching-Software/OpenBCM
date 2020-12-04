/*! \file bcmcfg_comp_scanner_default_config_get.c
 *
 * BCMCFG component default config get.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg_types.h>
#include <bcmcfg/bcmcfg_internal.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_COMP


/*******************************************************************************
 * Public Functions
 */

int
bcmcfg_comp_scanner_default_config_get(const bcmcfg_comp_scanner_t *scanner,
                                       size_t size,
                                       void *config)
{
    uint32_t *src;
    size_t data_size;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!scanner ||
        !config) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    data_size = scanner->data_size;
    if (size != data_size) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    src = scanner->default_data;
    if (!src) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memcpy(config, src, size);

exit:
    SHR_FUNC_EXIT();
}

