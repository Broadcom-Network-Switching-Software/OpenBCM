/*! \file bcmtm_drv.c
 *
 * BCMTM Driver APIs
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
#include <bcmissu/issu_api.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/generated/bcmtm_ha.h>
#include <bcmtm/generated/bcmtm_mc_ha.h>
#include <bcmtm/bcmtm_ha_internal.h>
#include <bcmtm/multicast/bcmtm_mc_group.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_device_info.h>
#include <bcmcth/bcmcth_tm_drv.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_DRV

/*******************************************************************************
* Local definitions
 */

static bcmtm_drv_t *bcmtm_drv[BCMDRD_CONFIG_MAX_UNITS];
static bcmtm_drv_info_t *bcmtm_drv_info[BCMDRD_CONFIG_MAX_UNITS];
static bcmtm_thd_info_t *bcmtm_thd_info[BCMDRD_CONFIG_MAX_UNITS];
static bcmtm_obm_thd_info_t *bcmtm_obm_thd_info[BCMDRD_CONFIG_MAX_UNITS];
static bcmtm_obm_usage_info_t *bcmtm_obm_usage[BCMDRD_CONFIG_MAX_UNITS];
static bcmtm_mc_dev_info_t *bcmtm_mc_dev_info[BCMDRD_CONFIG_MAX_UNITS];
static bcmtm_port_map_info_t bcmtm_port_map[BCMDRD_CONFIG_MAX_UNITS];
static bcmtm_pfc_ddr_info_t *bcmtm_pfc_ddr_info[BCMDRD_CONFIG_MAX_UNITS];
bcmtm_device_info_t bcmtm_device_info[BCMDRD_CONFIG_MAX_UNITS];

typedef bcmcth_tm_info_drv_t *(*bcmcth_tm_q_assignment_info_drv_get_f)(int unit);
typedef bcmcth_tm_info_drv_t *(*bcmcth_tm_cos_q_cpu_map_info_drv_get_f)(int unit);

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_tm_q_assignment_info_drv_get},
static struct {
    bcmcth_tm_q_assignment_info_drv_get_f drv_get;
} bcmtm_q_assignment_info_drv[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_tm_cos_q_cpu_map_info_drv_get},

static struct {
    bcmcth_tm_cos_q_cpu_map_info_drv_get_f drv_get;
} bcmtm_cos_q_cpu_map_info_drv[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};

/*******************************************************************************
* Public functions
 */

int
bcmtm_drv_set(int unit, bcmtm_drv_t *drv)
{
    bcmtm_drv[unit] = drv;

    return SHR_E_NONE;
}

int
bcmtm_drv_get(int unit, bcmtm_drv_t **drv)
{
    SHR_FUNC_ENTER(unit);
    *drv = bcmtm_drv[unit];
    if (*drv == NULL) {
       SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_drv_info_get(int unit, bcmtm_drv_info_t **drv_info)
{
    SHR_FUNC_ENTER(unit);
    *drv_info = bcmtm_drv_info[unit];
    if (*drv_info == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

void
bcmtm_port_map_get(int unit, bcmtm_port_map_info_t **port_map)
{
    *port_map = &(bcmtm_port_map[unit]);
}

int
bcmtm_port_map_set(int unit, bcmtm_port_map_info_t *port_map)
{
    SHR_FUNC_ENTER(unit);
    bcmtm_port_map[unit] = *port_map;
    SHR_FUNC_EXIT();
}

void
bcmtm_pfc_ddr_info_get(int unit, bcmtm_pfc_ddr_info_t **pfc_ddr_info)
{
    *pfc_ddr_info = bcmtm_pfc_ddr_info[unit];
}

int
bcmtm_pfc_ddr_info_set(int unit, bcmtm_pfc_ddr_info_t *pfc_ddr_info)
{
    SHR_FUNC_ENTER(unit);
    bcmtm_pfc_ddr_info[unit] = pfc_ddr_info;
    SHR_FUNC_EXIT();
}

int
bcmtm_drv_info_alloc(int unit, bool warm)
{
    bcmtm_drv_info_t *drv_info = NULL;
    uint32_t bcmtm_drv_info_sz = sizeof(bcmtm_drv_info_t);

    SHR_FUNC_ENTER(unit);
    drv_info = shr_ha_mem_alloc(unit,
                                BCMMGMT_TM_COMP_ID,
                                BCMTM_HA_DEV_DRV_INFO,
                                "bcmtmDrvInfo",
                                &bcmtm_drv_info_sz);
    SHR_NULL_CHECK(drv_info, SHR_E_MEMORY);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for Device drive info "
        "is created with size %0d\n"), bcmtm_drv_info_sz));

    if (!warm) {
        /* memset the allocated memory*/
        sal_memset(drv_info, 0, bcmtm_drv_info_sz);

        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_DEV_DRV_INFO, 0,
                                   sizeof(bcmtm_drv_info_t), 1,
                                   BCMTM_DRV_INFO_T_ID);
    }
    bcmtm_drv_info[unit] = drv_info;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_mc_dev_info_get(int unit, bcmtm_mc_dev_info_t **mc_dev_info)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmtm_mc_dev_info[unit]) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    *mc_dev_info = bcmtm_mc_dev_info[unit];
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_mc_dev_info_alloc(int unit, bool warm)
{
    bcmtm_mc_dev_info_t *mc_dev_info = NULL;
    uint32_t size = sizeof(bcmtm_mc_dev_info_t);

    SHR_FUNC_ENTER(unit);

    mc_dev_info = shr_ha_mem_alloc(unit,
                                   BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_DEV_MC_DEV_INFO,
                                   "bcmtmMulticastDevInfo",
                                   &size);
    SHR_NULL_CHECK(mc_dev_info, SHR_E_MEMORY);
    LOG_VERBOSE(BSL_LOG_MODULE,
         (BSL_META_U(unit, "BCMTM: Mutlicast device info is created with %d\n"),
                size));

    if (!warm) {
        /* memset the allocated memory */
        sal_memset(mc_dev_info, 0, size);
        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_DEV_MC_DEV_INFO, 0,
                                   size, 1, BCMTM_MC_DEV_INFO_T_ID);
    }
    bcmtm_mc_dev_info[unit] = mc_dev_info;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_thd_info_get(int unit, bcmtm_thd_info_t **thd_info)
{
    SHR_FUNC_ENTER(unit);
    if (!bcmtm_thd_info[unit]) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    *thd_info = bcmtm_thd_info[unit];
exit:
    SHR_FUNC_EXIT();
}



typedef enum {
    THD_PORT_MAX_BYTES = 0,
    THD_LOSSY_MAX_BYTES = 1,
    THD_LOSSY_LOW_MAX_BYTES = 2
} obm_thd_e;

int
bcmtm_obm_thd_get(int unit, uint32_t lport, int thd, uint32_t *val)
{
    bcmtm_obm_thd_info_t *thd_info = NULL;
    SHR_FUNC_ENTER(unit);
    if (!bcmtm_obm_thd_info[unit]) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    thd_info = bcmtm_obm_thd_info[unit];

    switch (thd) {
        case THD_PORT_MAX_BYTES:
            *val = thd_info->obm_port_max_bytes[lport];
            break;
        case THD_LOSSY_MAX_BYTES:
            *val = thd_info->obm_lossy_max_bytes[lport];
            break;
        case THD_LOSSY_LOW_MAX_BYTES:
            *val = thd_info->obm_lossy_low_max_bytes[lport];
            break;
        default:
            return SHR_E_PARAM;
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_thd_set(int unit, uint32_t lport, int thd, uint32_t val)
{
    bcmtm_obm_thd_info_t *thd_info = NULL;
    SHR_FUNC_ENTER(unit);
    if (!bcmtm_obm_thd_info[unit]) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    thd_info = bcmtm_obm_thd_info[unit];

    switch (thd) {
        case THD_PORT_MAX_BYTES:
            thd_info->obm_port_max_bytes[lport] = val;
            break;
        case THD_LOSSY_MAX_BYTES:
            thd_info->obm_lossy_max_bytes[lport] = val;
            break;
        case THD_LOSSY_LOW_MAX_BYTES:
            thd_info->obm_lossy_low_max_bytes[lport] = val;
            break;
        default:
            return SHR_E_PARAM;
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_thd_info_alloc(int unit, bool warm)
{
    bcmtm_thd_info_t *thd_info = NULL;
    uint32_t bcmtm_thd_info_sz = sizeof(bcmtm_thd_info_t);

    SHR_FUNC_ENTER(unit);
    thd_info = shr_ha_mem_alloc(unit,
                                BCMMGMT_TM_COMP_ID,
                                BCMTM_HA_DEV_THD_INFO,
                                "bcmtmThdInfo",
                                &bcmtm_thd_info_sz);
    SHR_NULL_CHECK(thd_info, SHR_E_PARAM);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for Device drive thd feature info "
        "is created with size %0d\n"), bcmtm_thd_info_sz));

    /* memset the allocated memory*/
    if (!warm) {
        sal_memset(thd_info, 0, bcmtm_thd_info_sz);

        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_DEV_THD_INFO, 0,
                                   sizeof(bcmtm_thd_info_t), 1,
                                   BCMTM_THD_INFO_T_ID);
    }
    /* save address of allocated memory. */
    bcmtm_thd_info[unit] = thd_info ;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_thd_info_alloc(int unit, bool warm)
{
    bcmtm_obm_thd_info_t *thd_info = NULL;
    uint32_t thd_info_sz = sizeof(bcmtm_obm_thd_info_t);

    SHR_FUNC_ENTER(unit);
    thd_info = shr_ha_mem_alloc(unit,
                                BCMMGMT_TM_COMP_ID,
                                BCMTM_HA_DEV_OBM_THD_INFO,
                                "bcmtmObmThdInfo",
                                &thd_info_sz);
    SHR_NULL_CHECK(thd_info, SHR_E_PARAM);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for Device drive OBM thd feature "
        "info is created with size %0d\n"), thd_info_sz));


    if (!warm) {
        sal_memset(thd_info, 0, thd_info_sz);

        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_DEV_OBM_THD_INFO, 0,
                                   sizeof(bcmtm_obm_thd_info_t), 1,
                                   BCMTM_OBM_THD_INFO_T_ID);
    }

    bcmtm_obm_thd_info[unit] = thd_info ;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_usage_pbmp_info_get(int unit, bcmtm_obm_usage_info_t **obm_usage_pbmp)
{
    SHR_FUNC_ENTER(unit);
    if (!bcmtm_obm_usage[unit]) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    *obm_usage_pbmp = bcmtm_obm_usage[unit];
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_usage_pbmp_info_alloc(int unit, bool warm)
{
    bcmtm_obm_usage_info_t *obm_usage_info = NULL;
    uint32_t pbmp_sz = 0;

    SHR_FUNC_ENTER(unit);

    pbmp_sz = sizeof(bcmtm_obm_usage_info_t);
    obm_usage_info = shr_ha_mem_alloc(unit,
                                BCMMGMT_TM_COMP_ID,
                                BCMTM_HA_DEV_OBM_USAGE_PBMP_INFO,
                                "bcmtmDevObmUsagePbmpInfo",
                                &pbmp_sz);
    SHR_NULL_CHECK(obm_usage_info, SHR_E_PARAM);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMTM: HA memory for OBM port usage bitmap info "
        "is created with size %0d\n"), pbmp_sz));

    /* memset the allocated memory*/
    if (!warm) {
        sal_memset(obm_usage_info, 0, pbmp_sz);

        bcmissu_struct_info_report(unit, BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_DEV_OBM_USAGE_PBMP_INFO, 0,
                                   pbmp_sz, 1, BCMTM_OBM_USAGE_INFO_T_ID);
    }
    bcmtm_obm_usage[unit] = obm_usage_info;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_q_assignment_info_drv_get(int unit, bcmcth_tm_info_drv_t **drv)
{
    bcmlrd_variant_t variant;
    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    *drv = bcmtm_q_assignment_info_drv[variant].drv_get(unit);
    if (*drv == NULL) {
        SHR_ERR_EXIT (SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_cos_q_cpu_map_info_drv_get(int unit, bcmcth_tm_info_drv_t **drv)
{
    bcmlrd_variant_t variant;
    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    *drv = bcmtm_cos_q_cpu_map_info_drv[variant].drv_get(unit);
    if (*drv == NULL) {
        SHR_ERR_EXIT (SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();


}
