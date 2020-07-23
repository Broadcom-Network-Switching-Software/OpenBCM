/*! \file bcmcth_mon_drv.c
 *
 * BCMCTH Monitor Driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcth/bcmcth_mon_drv.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/*! Device specific attach function type. */
typedef bcmcth_mon_drv_t *(*bcmcth_mon_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_mon_drv_get },
static struct {
    bcmcth_mon_drv_get_f drv_get;
} mon_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_mon_drv_t *mon_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

bcmcth_mon_drv_t *
bcmcth_mon_drv_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return mon_drv[unit];
}

bcmcth_mon_flowtracker_drv_t *
bcmcth_mon_ft_drv_get(int unit)
{
    if (mon_drv[unit]) {
        return mon_drv[unit]->ft_drv;
    }
    return NULL;
}

int
bcmcth_mon_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    mon_drv[unit] = mon_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_mon_init(int unit, bool warm)
{
    const bcmlrd_map_t *map = NULL;
    int rv;
    SHR_FUNC_ENTER(unit);

    if (mon_drv[unit] && mon_drv[unit]->mon_init) {
        SHR_IF_ERR_EXIT(mon_drv[unit]->mon_init(unit, warm));
    }

    rv = bcmlrd_map_get(unit, MON_FLOWTRACKER_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_sw_resources_alloc(unit, warm));
    }

    map = NULL;
    rv = bcmlrd_map_get(unit, MON_TELEMETRY_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_telemetry_sw_resources_alloc(unit, warm));
    }


    rv = bcmlrd_map_get(unit, MON_INBAND_TELEMETRY_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_int_uc_alloc(unit, warm));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (mon_drv[unit] && mon_drv[unit]->mon_deinit) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mon_drv[unit]->mon_deinit(unit));
    }

    bcmcth_mon_telemetry_sw_resources_free(unit);
    bcmcth_mon_ft_uc_free(unit);
    bcmcth_mon_int_uc_free(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_trace_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_trace_op_t op,
    bcmcth_mon_trace_t *entry)
{
    if (mon_drv[unit] && mon_drv[unit]->trace_op) {
        return mon_drv[unit]->trace_op(unit, trans_id, lt_id, op, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_mon_redirect_trace_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_redirect_trace_op_t op,
    bcmcth_mon_redirect_trace_t *entry)
{
    if (mon_drv[unit] && mon_drv[unit]->redirect_op) {
        return mon_drv[unit]->redirect_op(unit, trans_id, lt_id, op, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_mon_drop_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_drop_op_t op,
    bcmcth_mon_drop_t *entry)
{
    if (mon_drv[unit] && mon_drv[unit]->drop_op) {
        return mon_drv[unit]->drop_op(unit, trans_id, lt_id, op, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_mon_ctr_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_ctr_op_t op,
    bcmcth_mon_ctr_t *entry)
{
    if (mon_drv[unit] && mon_drv[unit]->ctr_op) {
        return mon_drv[unit]->ctr_op(unit, trans_id, lt_id, op, entry);
    }

    return SHR_E_NONE;
}

bcmcth_mon_int_drv_t *
bcmcth_mon_int_drv_get(int unit)
{
    if (mon_drv[unit]) {
        return mon_drv[unit]->int_drv;
    }
    return NULL;
}
