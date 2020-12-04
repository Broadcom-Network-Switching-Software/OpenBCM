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
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_conf.h>

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


/*! Device specific attach function type for port info. */
typedef bcmcth_mon_info_drv_t *(*bcmcth_mon_info_drv_get_f)(int unit);

/*! Define drv functions for MON_REDIRECT_PROFILE info base variant. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
bcmcth_mon_info_drv_t *_bc##_cth_mon_redirect_profile_info_drv_get(int unit) {return NULL;}
#include <bcmdrd/bcmdrd_devlist.h>

/*! MON_REDIRECT_PROFILE info table drive get. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_mon_redirect_profile_info_drv_get},
static struct {
    bcmcth_mon_info_drv_get_f drv_get;
} mon_redirect_profile_info_drv_get[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};

static bcmcth_mon_info_drv_t
*mon_redirect_profile_info_drv[BCMDRD_CONFIG_MAX_UNITS];

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

bcmcth_mon_flowtracker_fifo_drv_t *
bcmcth_mon_ft_fifo_drv_get(int unit)
{
    if (mon_drv[unit]) {
        if (mon_drv[unit]->ft_drv) {
            return mon_drv[unit]->ft_drv->ft_fifo_drv;
        }
    }
    return NULL;
}

bcmcth_mon_flowtracker_intr_drv_t *
bcmcth_mon_ft_intr_drv_get(int unit)
{
    if (mon_drv[unit]) {
        if (mon_drv[unit]->ft_drv) {
            return mon_drv[unit]->ft_drv->ft_intr_drv;
        }
    }
    return NULL;
}

int
bcmcth_mon_drv_init(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;
    bcmcth_info_table_t data;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    mon_drv[unit] = mon_drv_get[dev_type].drv_get(unit);

    /* Perform MON_REDIRECT_PROFILE_INFO table setup */
    variant = bcmlrd_variant_get(unit);

    data.table_id = 0xFFFFFFFF;
    mon_redirect_profile_info_drv[unit] =
        mon_redirect_profile_info_drv_get[variant].drv_get(unit);

    if (mon_redirect_profile_info_drv[unit] == NULL) {
        SHR_EXIT();
    }
    if (mon_redirect_profile_info_drv[unit]->get_data) {
        SHR_IF_ERR_EXIT
            (mon_redirect_profile_info_drv[unit]->get_data(unit, &data));
    } else {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(info_tbl_init(unit, &data, warm));

exit:
    SHR_FUNC_EXIT();
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
    bcmcth_mon_ft_sw_resources_free(unit);
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
