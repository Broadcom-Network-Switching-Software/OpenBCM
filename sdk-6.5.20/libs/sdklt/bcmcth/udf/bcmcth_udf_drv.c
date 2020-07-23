/*! \file bcmcth_udf_drv.c
 *
 * BCMCTH UDF Protocol Driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmcth/bcmcth_udf_drv.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmcth/bcmcth_udf_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_UDF

/*! Device specific attach function type. */
typedef bcmcth_udf_drv_t *(*bcmcth_udf_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_udf_drv_get },
static struct {
    bcmcth_udf_drv_get_f drv_get;
} udf_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/* Array of device variant specific for UDF pointers. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_udf_var_info_attach },
static struct {
    udf_var_info_attach_f attach;
} udf_variant_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

static bcmcth_udf_drv_t *udf_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */
int
bcmcth_udf_var_info_attach(int unit)
{
    bcmlrd_variant_t variant;
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific software setup */
    udf_drv[unit] = udf_drv_get[dev_type].drv_get(unit);

    if (udf_drv[unit] && udf_drv[unit]->drv_init) {
        SHR_IF_ERR_EXIT
        (udf_variant_attach[variant].attach(udf_drv[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_udf_drv_init(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT(bcmcth_udf_imm_register(unit));

    /* Perform device-specific software setup */
    udf_drv[unit] = udf_drv_get[dev_type].drv_get(unit);

    if (udf_drv[unit] && udf_drv[unit]->drv_init) {
        SHR_IF_ERR_EXIT(udf_drv[unit]->var_info_init(unit, warm,
                                                     &(udf_drv[unit]->var_info)));
    }

    if (udf_drv[unit] && udf_drv[unit]->drv_init) {
        SHR_IF_ERR_EXIT(udf_drv[unit]->drv_init(unit, warm));
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_udf_deinit(int unit)
{
    if (udf_drv[unit] && udf_drv[unit]->drv_deinit) {
        return udf_drv[unit]->drv_deinit(unit);
    }

    return SHR_E_NONE;
}

int bcmcth_udf_policy_create(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_sid,
    bcmcth_udf_policy_info_t *lt_entry,
    uint32_t update)
{
    SHR_FUNC_ENTER(unit);

    /* Perform device-specific software setup */
    if (udf_drv[unit] && udf_drv[unit]->drv_policy_create) {
        SHR_IF_ERR_VERBOSE_EXIT
            (udf_drv[unit]->drv_policy_create(unit, trans_id, lt_sid, lt_entry,
                                              update));
    }

exit:
    SHR_FUNC_EXIT();

}

int bcmcth_udf_policy_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_sid,
    bcmcth_udf_policy_info_t *lt_entry)
{
    SHR_FUNC_ENTER(unit);

    /* Perform device-specific software setup */
    if (udf_drv[unit] && udf_drv[unit]->drv_policy_delete) {
        SHR_IF_ERR_VERBOSE_EXIT
            (udf_drv[unit]->drv_policy_delete(unit, trans_id, lt_sid, lt_entry));
    }

exit:
    SHR_FUNC_EXIT();

}

int bcmcth_udf_field_mux_select_create(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_sid,
    bcmcth_udf_field_mux_select_info_t *lt_entry)
{
    SHR_FUNC_ENTER(unit);
    if (udf_drv[unit] && udf_drv[unit]->drv_field_mux_select_create) {
        SHR_IF_ERR_VERBOSE_EXIT
            (udf_drv[unit]->drv_field_mux_select_create(unit, trans_id, lt_sid, lt_entry));
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmcth_udf_field_mux_select_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_sid,
    bcmcth_udf_field_mux_select_info_t *lt_entry)
{
    SHR_FUNC_ENTER(unit);

    /* Perform device-specific software setup */
    if (udf_drv[unit] && udf_drv[unit]->drv_field_mux_select_delete) {
        SHR_IF_ERR_VERBOSE_EXIT
            (udf_drv[unit]->drv_field_mux_select_delete(unit, trans_id, lt_sid, lt_entry));
    }

exit:
    SHR_FUNC_EXIT();

}

int bcmcth_udf_policy_info_get(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Perform device-specific software setup */
    if (udf_drv[unit] && udf_drv[unit]->drv_policy_info_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (udf_drv[unit]->drv_policy_info_get(unit, warm));
    }

exit:
    SHR_FUNC_EXIT();

}

