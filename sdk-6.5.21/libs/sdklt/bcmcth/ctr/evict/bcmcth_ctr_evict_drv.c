/*! \file bcmcth_ctr_evict_drv.c
 *
 * BCMCTH CTR_EVICT Driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_ctr_evict_drv.h>
#include <bcmcth/bcmcth_imm_util.h>

/*******************************************************************************
 * Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREVICT

/*! Device specific attach function type. */
typedef bcmcth_ctr_evict_drv_t *(*bcmcth_ctr_evict_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,\
                             _cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_ctr_evict_drv_get },
static struct {
    bcmcth_ctr_evict_drv_get_f drv_get;
} ctr_evict_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_ctr_evict_drv_t *ctr_evict_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Private functions
 */
static bcmcth_ctr_evict_drv_t *
bcmcth_ctr_evict_drv_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return ctr_evict_drv[unit];
}

/*******************************************************************************
* Public functions
 */
int
bcmcth_ctr_evict_entry_update(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t trans_id,
                              ctr_control_entry_t *entry)
{
    bcmcth_ctr_evict_drv_t *evict = bcmcth_ctr_evict_drv_get(unit);

    if (evict && evict->evict_entry_update_fn) {
        return evict->evict_entry_update_fn(unit, ltid, trans_id, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_evict_dev_init(int unit, bool warm)
{
    bcmcth_ctr_evict_drv_t *evict = bcmcth_ctr_evict_drv_get(unit);

    if (!warm) {
        /* Chain the mem_id for eviction eligible tables */
        if (evict && evict->evict_init_fn) {
            return evict->evict_init_fn(unit);
        }
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_evict_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    ctr_evict_drv[unit] = ctr_evict_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_ctr_evict_drv_cleanup(int unit)
{
    if (ctr_evict_drv[unit]) {
        ctr_evict_drv[unit] = NULL;
    }

    return SHR_E_NONE;
}
