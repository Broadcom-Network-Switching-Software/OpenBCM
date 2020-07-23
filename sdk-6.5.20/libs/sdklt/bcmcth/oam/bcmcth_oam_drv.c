/*! \file bcmcth_oam_drv.c
 *
 * BCMCTH OAM Driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmcth/bcmcth_oam_drv.h>
#include <bcmcth/bcmcth_oam_imm.h>
#include <bcmcth/bcmcth_oam_bfd.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_OAM

/*! Device specific attach function type. */
typedef bcmcth_oam_drv_t *(*bcmcth_oam_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_oam_drv_get },
static struct {
    bcmcth_oam_drv_get_f drv_get;
} oam_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_oam_drv_t *oam_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

bcmcth_oam_drv_t *
bcmcth_oam_drv_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return oam_drv[unit];
}

bcmcth_oam_bfd_drv_t *
bcmcth_oam_bfd_drv_get(int unit)
{
    if (oam_drv[unit]) {
        return oam_drv[unit]->bfd_drv;
    }
    return NULL;
}

int
bcmcth_oam_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    oam_drv[unit] = oam_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_oam_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    if (oam_drv[unit] && oam_drv[unit]->oam_init) {
        SHR_IF_ERR_EXIT(oam_drv[unit]->oam_init(unit, warm));
    }

    if (bcmcth_oam_bfd_drv_get(unit) != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_init(unit, warm));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (oam_drv[unit] && oam_drv[unit]->oam_deinit) {
        SHR_IF_ERR_VERBOSE_EXIT(oam_drv[unit]->oam_deinit(unit));
    }

    bcmcth_oam_bfd_deinit(unit);

exit:
    SHR_FUNC_EXIT();
}
