/*! \file bcmcth_ts_drv.c
 *
 * BCMCTH TimeSync Driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmcth/bcmcth_ts_drv.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TIMESYNC

/*! Device specific attach function type. */
typedef bcmcth_ts_drv_t *(*bcmcth_ts_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_ts_drv_get },
static struct {
    bcmcth_ts_drv_get_f drv_get;
} ts_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/* Array of device variant specific info of TS */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_ts_info_get },
static struct {
    cth_ts_info_get_f info_get;
} list_variant_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

static bcmcth_ts_drv_t *ts_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */

/*******************************************************************************
* Public functions
 */

bcmcth_ts_drv_t *
bcmcth_ts_drv_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return ts_drv[unit];
}

int
bcmcth_ts_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    ts_drv[unit] = ts_drv_get[dev_type].drv_get(unit);

    /* Hook up variant-specific data */
    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        return SHR_E_UNIT;
    }
    list_variant_attach[variant].info_get(ts_drv[unit]);

    return SHR_E_NONE;
}

int
bcmcth_ts_init(int unit, bool warm)
{
    const bcmlrd_map_t *map = NULL;
    int rv;
    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, DEVICE_TS_SYNCE_CLK_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ts_synce_sw_resources_alloc(unit, warm));
    }

    if (ts_drv[unit] && ts_drv[unit]->init) {
        SHR_IF_ERR_EXIT
            (ts_drv[unit]->init(unit, warm, ts_drv[unit]->info));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    bcmcth_ts_synce_free(unit);

    SHR_FUNC_EXIT();
}

bcmcth_ts_synce_drv_t *
bcmcth_ts_synce_drv_get(int unit)
{
    if (ts_drv[unit]) {
        return ts_drv[unit]->synce_drv;
    }
    return NULL;
}

bcmcth_ts_tod_drv_t *
bcmcth_ts_tod_drv_get(int unit)
{
    if (ts_drv[unit]) {
        return ts_drv[unit]->tod_drv;
    }
    return NULL;
}
