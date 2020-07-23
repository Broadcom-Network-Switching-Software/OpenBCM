/*! \file bcm56990_a0_vlan.c
 *
 * BCM56990_A0 VLAN subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/vlan.h>
#include <bcm_int/ltsw/xgs/vlan.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_VLAN

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_vlan_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    if (!hdl) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_init = xgs_ltsw_vlan_init;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_detach = xgs_ltsw_vlan_detach;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_otpid_init = xgs_ltsw_vlan_otpid_init;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_otpid_detach = xgs_ltsw_vlan_otpid_detach;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_otpid_add = xgs_ltsw_vlan_otpid_add;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_create = xgs_ltsw_vlan_create;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_destroy = xgs_ltsw_vlan_destroy;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_port_add = xgs_ltsw_vlan_port_add;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_port_get = xgs_ltsw_vlan_port_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_port_remove = xgs_ltsw_vlan_port_remove;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_gport_add = xgs_ltsw_vlan_gport_add;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_gport_delete = xgs_ltsw_vlan_gport_delete;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_gport_delete_all = xgs_ltsw_vlan_gport_delete_all;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_gport_get = xgs_ltsw_vlan_gport_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_gport_get_all = xgs_ltsw_vlan_gport_get_all;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_control_vlan_set = xgs_ltsw_vlan_control_vlan_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_control_vlan_get = xgs_ltsw_vlan_control_vlan_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_port_action_set = xgs_ltsw_vlan_port_action_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_port_action_get = xgs_ltsw_vlan_port_action_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_port_action_reset = xgs_ltsw_vlan_port_action_reset;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->egr_vlan_port_action_set = xgs_ltsw_egr_vlan_port_action_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->egr_vlan_port_action_get = xgs_ltsw_egr_vlan_port_action_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->egr_vlan_port_action_reset = xgs_ltsw_egr_vlan_port_action_reset;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_mcast_flood_set = xgs_ltsw_vlan_mcast_flood_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_mcast_flood_get = xgs_ltsw_vlan_mcast_flood_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_block_set = xgs_ltsw_vlan_block_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_block_get = xgs_ltsw_vlan_block_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_control_get = xgs_ltsw_vlan_control_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_control_set = xgs_ltsw_vlan_control_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_control_port_set = xgs_ltsw_vlan_control_port_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_control_port_get = xgs_ltsw_vlan_control_port_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_cross_connect_add = xgs_ltsw_vlan_cross_connect_add;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_cross_connect_delete = xgs_ltsw_vlan_cross_connect_delete;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_cross_connect_delete_all = xgs_ltsw_vlan_cross_connect_delete_all;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_cross_connect_traverse = xgs_ltsw_vlan_cross_connect_traverse;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_stat_attach = xgs_ltsw_vlan_stat_attach;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_stat_detach = xgs_ltsw_vlan_stat_detach;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_ing_qos_set = xgs_ltsw_vlan_ing_qos_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_ing_qos_get = xgs_ltsw_vlan_ing_qos_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_egr_qos_set = xgs_ltsw_vlan_egr_qos_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_egr_qos_get = xgs_ltsw_vlan_egr_qos_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_internal_control_set = xgs_ltsw_vlan_internal_control_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_internal_control_get = xgs_ltsw_vlan_internal_control_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_stg_set = xgs_ltsw_vlan_stg_set;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_stg_get = xgs_ltsw_vlan_stg_get;
    ((mbcm_ltsw_vlan_drv_t *)hdl)->vlan_traverse = xgs_ltsw_vlan_traverse;

exit:
    SHR_FUNC_EXIT();
}

