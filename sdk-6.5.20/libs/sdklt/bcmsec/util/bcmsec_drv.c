/*! \file bcmsec_drv.c
 *
 * BCMSEC Driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmsec/bcmsec_drv.h>
#include <bcmissu/issu_api.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmsec/bcmsec_utils.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_DRV

/*******************************************************************************
* Local definitions
 */

static bcmsec_drv_t *bcmsec_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmsec_drv_set(int unit, bcmsec_drv_t *drv)
{
    bcmsec_drv[unit] = drv;

    return SHR_E_NONE;
}

int
bcmsec_drv_get(int unit, bcmsec_drv_t **drv)
{
    SHR_FUNC_ENTER(unit);
    *drv = bcmsec_drv[unit];
    if (*drv == NULL) {
       SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_port_info_get(int unit, int lport, sec_pt_port_info_t *info)
{
    bcmsec_drv_t *drv;
    int pport;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmsec_drv_get(unit, &drv));
    sal_memset(info, 0, sizeof(sec_pt_port_info_t));
    SHR_IF_ERR_EXIT
        (bcmsec_port_phys_port_get(unit, lport, &pport));
    SHR_IF_ERR_EXIT
        (drv->port_info_get(unit, pport, info));
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_is_blktype_sec(int unit, int blktype)
{
    bcmsec_drv_t *drv;

    if (SHR_SUCCESS(bcmsec_drv_get(unit, &drv)) &&
        drv->blktype_is_sec) {
        return (drv->blktype_is_sec(unit, blktype));
    }
    return FALSE;
}

int
bcmsec_is_sec_supported(int unit)
{
    bcmsec_drv_t *drv;

    if (SHR_SUCCESS(bcmsec_drv_get(unit, &drv)) &&
        drv->is_sec_supported) {
        return (drv->is_sec_supported(unit));
    }
    return FALSE;
}
