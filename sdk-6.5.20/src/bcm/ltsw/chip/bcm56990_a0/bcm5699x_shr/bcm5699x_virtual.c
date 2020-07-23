/*! \file bcm5699x_virtual.c
 *
 * BCM5699x Virtual management subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/mbcm/virtual.h>
#include <bcm_int/ltsw/xgs/virtual.h>
#include <bcm_int/ltsw/xgs/types.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/sbr.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/* Reserved first 4K VFI for identical mapping to VLAN. */
#define VFI_RESERVED   (BCM_VLAN_MAX + 1)


/******************************************************************************
 * Private functions
 */

static int
bcm5699x_ltsw_virtual_index_get(int unit, int *vfi_min, int *vfi_max,
                                int *vp_min, int *vp_max, int *l2_iif_min,
                                int *l2_iif_max)
{
    uint64_t min = 0;
    uint64_t max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, VFIs, VFI_IDs,
                                       &min, &max));
    /*
     * Reserved first 4K VFI for identical mapping to VLAN.
     */
    *vfi_min = VFI_RESERVED;
    *vfi_max = (int)max;

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, PORT_SVPs, PORT_SVP_IDs,
                                       &min, &max));
    /*
     * VP index is used for VP validation and 0 is treated as invalid VP.
     */
    *vp_min = min > 1? (int)min:1;
    *vp_max = (int)max;

    /* No L2_IIF in TH4G. */
    *l2_iif_min = 0;
    *l2_iif_max = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm5699x_ltsw_virtual_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (xgs_ltsw_virtual_init(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm5699x_ltsw_virtual_tnl_index_get(int unit, int *tnl_min, int *tnl_max)
{
    uint64_t min = 0;
    uint64_t max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_EIFs, L3_EIF_IDs,
                                       &min, &max));

    /*
     * Index 0 of L3_EIF is reserved.
     * Tunnel is L3_EIF based and shares same range with L3_EIF.
     */
    *tnl_min = min > 1? (int)min:1;
    *tnl_max = (int)max;

exit:
    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public functions
 */
int
bcm5699x_virtual_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    if (!hdl) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_index_get = bcm5699x_ltsw_virtual_index_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_vfi_init = xgs_ltsw_virtual_vfi_init;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_vfi_cleanup = xgs_ltsw_virtual_vfi_cleanup;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_learn_get = xgs_ltsw_virtual_port_learn_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_learn_set = xgs_ltsw_virtual_port_learn_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_vlan_action_set = xgs_ltsw_virtual_port_vlan_action_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_vlan_action_get = xgs_ltsw_virtual_port_vlan_action_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_vlan_action_reset = xgs_ltsw_virtual_port_vlan_action_reset;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_class_get = xgs_ltsw_virtual_port_class_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_class_set = xgs_ltsw_virtual_port_class_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_vlan_ctrl_get = xgs_ltsw_virtual_port_vlan_ctrl_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_vlan_ctrl_set = xgs_ltsw_virtual_port_vlan_ctrl_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_mirror_ctrl_get = xgs_ltsw_virtual_port_mirror_ctrl_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_mirror_ctrl_set = xgs_ltsw_virtual_port_mirror_ctrl_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_dscp_map_mode_get = xgs_ltsw_virtual_port_dscp_map_mode_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_dscp_map_mode_set = xgs_ltsw_virtual_port_dscp_map_mode_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_phb_vlan_map_get = xgs_ltsw_virtual_port_phb_vlan_map_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_phb_vlan_map_set = xgs_ltsw_virtual_port_phb_vlan_map_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_phb_dscp_map_get = xgs_ltsw_virtual_port_phb_dscp_map_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_phb_dscp_map_set = xgs_ltsw_virtual_port_phb_dscp_map_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_phb_vlan_remark_map_get = xgs_ltsw_virtual_port_phb_vlan_remark_map_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_phb_vlan_remark_map_set = xgs_ltsw_virtual_port_phb_vlan_remark_map_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_nw_group_num_get = xgs_ltsw_virtual_nw_group_num_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_init = bcm5699x_ltsw_virtual_init;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_control_get = xgs_ltsw_virtual_port_control_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_control_set = xgs_ltsw_virtual_port_control_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_stat_attach = xgs_ltsw_virtual_port_stat_attach;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_port_stat_detach_with_id = xgs_ltsw_virtual_port_stat_detach_with_id;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_network_group_split_config_set = xgs_ltsw_virtual_network_group_split_config_set;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_network_group_split_config_get = xgs_ltsw_virtual_network_group_split_config_get;
    ((mbcm_ltsw_virtual_drv_t *)hdl)->virtual_tnl_index_get = bcm5699x_ltsw_virtual_tnl_index_get;

exit:
    SHR_FUNC_EXIT();
}
