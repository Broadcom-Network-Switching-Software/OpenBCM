/*! \file bcmcth_port_drv.c
 *
 * BCMCTH PORT driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmcth/bcmcth_port_drv.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCMCTH_PORT

/*! Device specific attach function type. */
typedef bcmcth_port_drv_t *(*bcmcth_port_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_port_drv_get },
static struct {
    bcmcth_port_drv_get_f drv_get;
} port_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_port_drv_t *port_drv[BCMDRD_CONFIG_MAX_UNITS];

/*! Device specific attach function type for port info. */
typedef bcmcth_port_info_drv_t *(*bcmcth_port_info_drv_get_f)(int unit);
/*! Define stub functions for PORT_ING_EGR_BLOCK_PROFILE info base variant. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
bcmcth_port_info_drv_t *_bc##_cth_port_ing_egr_block_profile_info_drv_get(int unit) {return NULL;}
#include <bcmdrd/bcmdrd_devlist.h>

/*! PORT_ING_EGR_BLOCK_PROFILE info table drive get. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_port_ing_egr_block_profile_info_drv_get},
static struct {
    bcmcth_port_info_drv_get_f drv_get;
} port_ing_egr_block_profile_info_drv_get[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};

static bcmcth_port_info_drv_t
*port_ing_egr_block_profile_info_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_port_drv_init(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;
    bcmcth_info_table_t data;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific software setup */
    port_drv[unit] = port_drv_get[dev_type].drv_get(unit);

    variant = bcmlrd_variant_get(unit);
    /* Perform ingress info table setup */
    data.table_id = 0xFFFFFFFF;
    port_ing_egr_block_profile_info_drv[unit] =
        port_ing_egr_block_profile_info_drv_get[variant].drv_get(unit);

    if (port_ing_egr_block_profile_info_drv[unit] == NULL) {
        SHR_EXIT();
    }
    if (port_ing_egr_block_profile_info_drv[unit]->get_data) {
        SHR_IF_ERR_EXIT
            (port_ing_egr_block_profile_info_drv[unit]->get_data(unit, &data));
    } else {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(info_tbl_init(unit, &data, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_port_bridge_set(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t lt_id,
                       shr_port_t port,
                       bool bridge)
{
    if (port_drv[unit] && port_drv[unit]->port_bridge_set) {
        return port_drv[unit]->port_bridge_set(unit,
                                               op_arg,
                                               lt_id,
                                               port,
                                               bridge);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_bridge_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t lt_id,
                       shr_port_t port,
                       bool *bridge)
{
    if (port_drv[unit] && port_drv[unit]->port_bridge_get) {
        return port_drv[unit]->port_bridge_get(unit,
                                               op_arg,
                                               lt_id,
                                               port,
                                               bridge);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_membership_policy_set(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t lt_id,
                                  shr_port_t port,
                                  port_membership_policy_info_t *lt_info)
{
    if (port_drv[unit] && port_drv[unit]->port_membership_policy_set) {
        return port_drv[unit]->port_membership_policy_set(unit,
                                                          op_arg,
                                                          lt_id,
                                                          port,
                                                          lt_info);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_membership_policy_get(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t lt_id,
                                  shr_port_t port,
                                  port_membership_policy_info_t *lt_info)
{
    if (port_drv[unit] && port_drv[unit]->port_membership_policy_get) {
        return port_drv[unit]->port_membership_policy_get(unit,
                                                          op_arg,
                                                          lt_id,
                                                          port,
                                                          lt_info);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_system_source_get(int unit,
                              int port,
                              int *system_source)
{
    if (port_drv[unit] && port_drv[unit]->port_system_source_get) {
        return port_drv[unit]->port_system_source_get(unit,
                                              port,
                                              system_source);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_system_dest_set(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmltd_sid_t lt_id,
                            bool set,
                            port_system_dest_entry_t *entry)
{
    if (port_drv[unit] && port_drv[unit]->port_system_dest_set) {
        return port_drv[unit]->port_system_dest_set(unit,
                                                    op_arg,
                                                    lt_id,
                                                    set,
                                                    entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_trunk_pctl_set(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t lt_id,
                           port_trunk_pctl_entry_t *entry)
{
    if (port_drv[unit] && port_drv[unit]->port_trunk_pctl_set) {
        return port_drv[unit]->port_trunk_pctl_set(unit,
                                                   op_arg,
                                                   lt_id,
                                                   entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_modid_get(int unit,
                      int port,
                      int *modid)
{
    if (port_drv[unit] && port_drv[unit]->port_modid_get) {
        return port_drv[unit]->port_modid_get(unit, port, modid);
    } else {
        return SHR_E_UNAVAIL;
    }
}

int
bcmcth_port_system_ingress_mirror_profile_set(int unit,
                                              const bcmltd_op_arg_t *op_arg,
                                              bcmltd_sid_t lt_id,
                                              int tbl_inst,
                                              uint16_t port_system_profile_id,
                                              uint32_t mirror_inst,
                                              bool mirror_enable)
{
    if (port_drv[unit] &&
        port_drv[unit]->port_system_ing_mirror_profile_set) {
        return port_drv[unit]->port_system_ing_mirror_profile_set(
                   unit, op_arg, lt_id, tbl_inst,
                   port_system_profile_id, mirror_inst, mirror_enable);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_egress_mirror_set(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              port_emirror_t *cfg,
                              bool mirror_enable)
{
    if (port_drv[unit] && port_drv[unit]->port_egress_mirror_set) {
        return port_drv[unit]->port_egress_mirror_set(unit,
                                                      op_arg,
                                                      lt_id,
                                                      cfg,
                                                      mirror_enable);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_ingress_mirror_set(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t lt_id,
                               port_imirror_t *cfg,
                               bool mirror_enable)
{
    if (port_drv[unit] && port_drv[unit]->port_ingress_mirror_set) {
        return port_drv[unit]->port_ingress_mirror_set(unit,
                                                       op_arg,
                                                       lt_id,
                                                       cfg,
                                                       mirror_enable);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_mirror_set(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id,
    bcmcth_port_mirror_op_t mirror_op,
    port_mirror_t *lt_info)
{
    if (port_drv[unit] && port_drv[unit]->port_mirror_set) {
        return port_drv[unit]->port_mirror_set(unit, op_arg, lt_id,
                                               mirror_op, lt_info);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_svp_ingress_mirror_set(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t lt_id,
                                   port_imirror_t *cfg,
                                   bool mirror_enable)
{
    if (port_drv[unit] && port_drv[unit]->port_svp_ingress_mirror_set) {
        return port_drv[unit]->port_svp_ingress_mirror_set(unit,
                                                           op_arg,
                                                           lt_id,
                                                           cfg,
                                                           mirror_enable);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_svp_ingress_mirror_get(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t lt_id,
                                   port_imirror_t *cfg,
                                   bool *mirror_enable)
{
    if (port_drv[unit] && port_drv[unit]->port_svp_ingress_mirror_get) {
        return port_drv[unit]->port_svp_ingress_mirror_get(unit,
                                                           op_arg,
                                                           lt_id,
                                                           cfg,
                                                           mirror_enable);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_init(int unit, bool warm)
{
    if (port_drv[unit] && port_drv[unit]->port_init) {
        return port_drv[unit]->port_init(unit, warm);
    }

    return SHR_E_NONE;
}

int
bcmcth_port_deinit(int unit)
{
    if (port_drv[unit] && port_drv[unit]->port_deinit) {
        return port_drv[unit]->port_deinit(unit);
    }

    return SHR_E_NONE;
}

