/*! \file bcmcth_mirror_drv.c
 *
 * BCMCTH Mirror Driver APIs.
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

#include <bcmcth/bcmcth_mirror_drv.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MIRROR

/*! Device specific attach function type. */
typedef bcmcth_mirror_drv_t *(*bcmcth_mirror_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_mirror_drv_get },
static struct {
    bcmcth_mirror_drv_get_f drv_get;
} mirror_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/* Array of device variant specific data for mirror */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_mirror_data_attach },
static struct {
    cth_mirror_data_attach_f attach;
} list_variant_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

static bcmcth_mirror_drv_t *mirror_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmcth_mirror_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    mirror_drv[unit] = mirror_drv_get[dev_type].drv_get(unit);

    /* Hook up variant-specific data */
    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        return SHR_E_UNIT;
    }
    list_variant_attach[variant].attach(mirror_drv[unit]);

    return SHR_E_NONE;
}

int
bcmcth_mirror_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    if (mirror_drv[unit] && mirror_drv[unit]->init) {
        SHR_IF_ERR_EXIT(
            mirror_drv[unit]->init(unit, warm, mirror_drv[unit]->data));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mirror_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (mirror_drv[unit] && mirror_drv[unit]->deinit) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_drv[unit]->deinit(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mirror_encap_op(
    int unit,
    bcmcth_mirror_encap_op_t encap_op,
    bcmcth_mirror_encap_t *encap)
{
    SHR_FUNC_ENTER(unit);

    if (mirror_drv[unit] && mirror_drv[unit]->encap_op) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_drv[unit]->encap_op(unit, encap_op, encap));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mirror_encap_ins_attr_op(
    int unit,
    bcmcth_mirror_encap_op_t encap_op,
    bcmcth_mirror_encap_ins_attr_t *encap)
{
    SHR_FUNC_ENTER(unit);

    if (mirror_drv[unit] && mirror_drv[unit]->encap_ins_attr_op) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_drv[unit]->encap_ins_attr_op(unit, encap_op, encap));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mirror_data_get(
    int unit,
    const bcmcth_mirror_data_t **data)
{
    if (data == NULL) {
        return SHR_E_PARAM;
    }

    if (mirror_drv[unit] && mirror_drv[unit]->data) {
        *data = mirror_drv[unit]->data;
        return SHR_E_NONE;
    }

    return SHR_E_INIT;
}

int
bcmcth_mirror_ctr_flex_sflow_write(int unit,
                                   bcmcth_mirror_flex_sflow_stat_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (mirror_drv[unit] && mirror_drv[unit]->ctr_flex_sflow_write) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_drv[unit]->ctr_flex_sflow_write(unit, info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mirror_ctr_flex_sflow_read(int unit,
                                  bcmcth_mirror_flex_sflow_stat_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (mirror_drv[unit] && mirror_drv[unit]->ctr_flex_sflow_read) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_drv[unit]->ctr_flex_sflow_read(unit, info));
    }

exit:
    SHR_FUNC_EXIT();
}
