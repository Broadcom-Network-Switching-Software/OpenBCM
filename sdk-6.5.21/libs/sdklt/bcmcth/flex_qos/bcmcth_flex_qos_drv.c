/*! \file bcmcth_flex_qos_drv.c
 *
 * BCMCTH Flex QoS Driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <shr/shr_ha.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_conf.h>

#include <bcmcth/bcmcth_flex_qos_drv.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCMCTH_FLEX_QOS


#define BSL_LOG_MODULE BSL_LS_BCMCTH_FLEX_QOS

/*! Device specific attach function type. */
typedef bcmcth_flex_qos_info_drv_t *(*bcmcth_flex_qos_info_drv_get_f)(int unit);

/*! Flex QoS ingress info table drive get. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_flex_qos_ing_info_drv_get},
static struct {
    bcmcth_flex_qos_info_drv_get_f drv_get;
} flex_qos_ing_info_drv_get[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};

/*! Flex QoS egress info table drive get. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_flex_qos_egr_info_drv_get},
static struct {
    bcmcth_flex_qos_info_drv_get_f drv_get;
} flex_qos_egr_info_drv_get[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};

static bcmcth_flex_qos_info_drv_t
*flex_qos_ing_info_drv[BCMDRD_CONFIG_MAX_UNITS];
static bcmcth_flex_qos_info_drv_t
*flex_qos_egr_info_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */
/*
 * Flex Digest driver initialization routine.
 */
int
bcmcth_flex_qos_drv_init(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;
    bcmcth_info_table_t data;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    variant = bcmlrd_variant_get(unit);

    /* Perform ingress info table setup */
    data.table_id = 0xFFFFFFFF;
    flex_qos_ing_info_drv[unit] =
        flex_qos_ing_info_drv_get[variant].drv_get(unit);

    if (flex_qos_ing_info_drv[unit] == NULL) {
        SHR_EXIT();
    }

    if (flex_qos_ing_info_drv[unit] && flex_qos_ing_info_drv[unit]->get_data) {
        SHR_IF_ERR_EXIT(flex_qos_ing_info_drv[unit]->get_data(unit, &data));
    } else {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(info_tbl_init(unit, &data, warm));

    SHR_IF_ERR_EXIT(bcmcth_flex_qos_ing_info_imm_register(unit,data.table_id));

    /* Perform egress info table setup */
    data.table_id = 0xFFFFFFFF;
    flex_qos_egr_info_drv[unit] =
        flex_qos_egr_info_drv_get[variant].drv_get(unit);

    if (flex_qos_egr_info_drv[unit] && flex_qos_egr_info_drv[unit]->get_data) {
        SHR_IF_ERR_EXIT(flex_qos_egr_info_drv[unit]->get_data(unit, &data));
    } else {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(info_tbl_init(unit, &data, warm));

    SHR_IF_ERR_EXIT(bcmcth_flex_qos_egr_info_imm_register(unit, data.table_id));

exit:
    SHR_FUNC_EXIT();
}

