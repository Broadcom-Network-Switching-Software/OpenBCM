/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*! \file range.c
 *
 * BCM Range module multiplexing.
 */
#include <bcm/types.h>

#include <bcm_int/ltsw/mbcm/range.h>
#include <bcm_int/ltsw/mbcm/mbcm.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_RANGE

static mbcm_ltsw_range_drv_t mbcm_ltsw_range_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_range_drv_set(
    int unit,
    mbcm_ltsw_range_drv_t *drv)
{
    mbcm_ltsw_range_drv_t *local = &mbcm_ltsw_range_drv[unit];

    SHR_FUNC_ENTER(unit);

    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                       (mbcm_ltsw_drv_t *)drv,
                       sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_range_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_range_drv[unit].range_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_range_tbls_info_init(
    int unit,
    bcmint_range_tbls_info_t **tbls_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_range_drv[unit].range_tbls_info_init(unit, tbls_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_range_rtype_add(
        int unit,
        bcmlt_entry_handle_t range_check_lt,
        bcmint_range_tbls_info_t *tbls_info,
        bcm_range_config_t *range_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_range_drv[unit].range_rtype_add(unit,
                                                   range_check_lt,
                                                   tbls_info,
                                                   range_config));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_range_rtype_get(
        int unit,
        bcmlt_entry_handle_t range_check_lt,
        bcmint_range_tbls_info_t *tbls_info,
        bcm_range_config_t *range_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_range_drv[unit].range_rtype_get(unit,
                                                   range_check_lt,
                                                   tbls_info,
                                                   range_config));

exit:
    SHR_FUNC_EXIT();
}
