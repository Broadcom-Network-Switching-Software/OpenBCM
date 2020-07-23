/*! \file bcmcfg_comp_scanner_config_get.c
 *
 * BCMCFG component config get.
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
bcmcfg_comp_scanner_config_get(const bcmcfg_comp_scanner_t *scanner,
                               size_t size,
                               void *config)
{
    uint32_t **src;
    uint32_t *default_src;
    size_t data_size;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!scanner ||
        !config) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    src = scanner->data;
    default_src = scanner->default_data;
    data_size = scanner->data_size;
    if (size != data_size) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (!*src) {
        sal_memcpy(config, default_src, size);
    } else {
        sal_memcpy(config, *src, size);
    }

exit:
    SHR_FUNC_EXIT();
}

