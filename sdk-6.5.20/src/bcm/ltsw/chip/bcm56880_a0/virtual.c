/*! \file virtual.c
 *
 * BCM56880_A0 virtual management Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/virtual.h>
#include <bcm_int/ltsw/xfs/virtual.h>

#include <shr/shr_debug.h>


/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON


/******************************************************************************
 * Private functions
 */

static mbcm_ltsw_virtual_drv_t bcm56880_a0_ltsw_virtual_drv = {
    .virtual_index_get      = xfs_ltsw_virtual_index_get,
    .virtual_vfi_init       = xfs_ltsw_virtual_vfi_init,
    .virtual_vfi_cleanup    = xfs_ltsw_virtual_vfi_cleanup,
    .virtual_port_learn_get       = xfs_ltsw_virtual_port_learn_get,
    .virtual_port_learn_set       = xfs_ltsw_virtual_port_learn_set,
    .virtual_port_vlan_action_set = xfs_ltsw_virtual_port_vlan_action_set,
    .virtual_port_vlan_action_get = xfs_ltsw_virtual_port_vlan_action_get,
    .virtual_port_vlan_action_reset = xfs_ltsw_virtual_port_vlan_action_reset,
    .virtual_port_class_get       = xfs_ltsw_virtual_port_class_get,
    .virtual_port_class_set       = xfs_ltsw_virtual_port_class_set,
    .virtual_port_vlan_ctrl_get   = xfs_ltsw_virtual_port_vlan_ctrl_get,
    .virtual_port_vlan_ctrl_set   = xfs_ltsw_virtual_port_vlan_ctrl_set,
    .virtual_port_mirror_ctrl_get   = xfs_ltsw_virtual_port_mirror_ctrl_get,
    .virtual_port_mirror_ctrl_set   = xfs_ltsw_virtual_port_mirror_ctrl_set,
    .virtual_port_dscp_map_mode_get   = xfs_ltsw_virtual_port_dscp_map_mode_get,
    .virtual_port_dscp_map_mode_set   = xfs_ltsw_virtual_port_dscp_map_mode_set,
    .virtual_port_phb_vlan_map_get   = xfs_ltsw_virtual_port_phb_vlan_map_get,
    .virtual_port_phb_vlan_map_set   = xfs_ltsw_virtual_port_phb_vlan_map_set,
    .virtual_port_phb_dscp_map_get   = xfs_ltsw_virtual_port_phb_dscp_map_get,
    .virtual_port_phb_dscp_map_set   = xfs_ltsw_virtual_port_phb_dscp_map_set,
    .virtual_port_phb_vlan_remark_map_get = xfs_ltsw_virtual_port_phb_vlan_remark_map_get,
    .virtual_port_phb_vlan_remark_map_set = xfs_ltsw_virtual_port_phb_vlan_remark_map_set,
    .virtual_nw_group_num_get = xfs_ltsw_virtual_nw_group_num_get,
    .virtual_init = xfs_ltsw_virtual_init,
    .virtual_tnl_index_get = xfs_ltsw_virtual_tnl_index_get,
    };

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_ltsw_virtual_drv_attach(int unit)
{
    return mbcm_ltsw_virtual_drv_set(unit, &bcm56880_a0_ltsw_virtual_drv);
}
