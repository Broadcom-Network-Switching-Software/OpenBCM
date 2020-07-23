/*! \file vlan.c
 *
 * BCM56880_A0 VLAN Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/vlan.h>
#include <bcm_int/ltsw/xfs/vlan.h>
#include <bcm_int/ltsw/chip/bcm56880_a0/variant_dispatch.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_VLAN

static mbcm_ltsw_vlan_drv_t bcm56880_a0_ltsw_vlan_drv = {
    /* vlan init. */
    .vlan_init = xfs_ltsw_vlan_init,

    /* vlan detach. */
    .vlan_detach = xfs_ltsw_vlan_detach,

    /* vlan outer tpid init. */
    .vlan_otpid_init = xfs_ltsw_vlan_otpid_init,

    /* vlan outer tpid detach. */
    .vlan_otpid_detach = xfs_ltsw_vlan_otpid_detach,

    /* vlan outer tpid add. */
    .vlan_otpid_add = xfs_ltsw_vlan_otpid_add,

    /* vlan inner tpid set. */
    .vlan_itpid_set = xfs_ltsw_vlan_itpid_set,

    /* vlan inner tpid get. */
    .vlan_itpid_get = xfs_ltsw_vlan_itpid_get,

    /* vlan port add. */
    .vlan_port_add = xfs_ltsw_vlan_port_add,

    /* vlan port get. */
    .vlan_port_get = xfs_ltsw_vlan_port_get,

    /* vlan port remove. */
    .vlan_port_remove = xfs_ltsw_vlan_port_remove,

    /*! Set the state of VLAN membership check on a given port. */
    .vlan_port_check_set = xfs_ltsw_vlan_port_check_set,

    /* vlan create. */
    .vlan_create = xfs_ltsw_vlan_create,

    /* vlan destroy. */
    .vlan_destroy = xfs_ltsw_vlan_destroy,

    /* vlan config set. */
    .vlan_control_vlan_set = xfs_ltsw_vlan_control_vlan_set,

    /* vlan config get. */
    .vlan_control_vlan_get = xfs_ltsw_vlan_control_vlan_get,

    /* port-specfic vlan options set. */
    .vlan_control_port_set = xfs_ltsw_vlan_control_port_set,

    /* port-specfic vlan options get. */
    .vlan_control_port_get = xfs_ltsw_vlan_control_port_get,

    /* ingress vlan xlate add. */
    .vlan_ing_xlate_add = xfs_ltsw_vlan_ing_xlate_add,

    /* ingress vlan xlate update. */
    .vlan_ing_xlate_update = xfs_ltsw_vlan_ing_xlate_update,

    /* ingress vlan xlate get. */
    .vlan_ing_xlate_get = xfs_ltsw_vlan_ing_xlate_get,

    /* ingress vlan xlate delete. */
    .vlan_ing_xlate_delete = xfs_ltsw_vlan_ing_xlate_delete,

    /* ingress vlan xlate traverse. */
    .vlan_ing_xlate_traverse = xfs_ltsw_vlan_ing_xlate_traverse,

    /* ingress vlan action xlate add. */
    .vlan_ing_xlate_action_add = xfs_ltsw_vlan_ing_xlate_action_add,

    /* ingress vlan action xlate get. */
    .vlan_ing_xlate_action_get = xfs_ltsw_vlan_ing_xlate_action_get,

    /* ingress vlan action xlate delete. */
    .vlan_ing_xlate_action_delete = xfs_ltsw_vlan_ing_xlate_action_delete,

    /* ingress vlan action xlate traverse. */
    .vlan_ing_xlate_action_traverse = xfs_ltsw_vlan_ing_xlate_action_traverse,

    /* egress vlan action xlate add. */
    .vlan_egr_xlate_action_add = xfs_ltsw_vlan_egr_xlate_action_add,

    /* egress vlan action xlate get. */
    .vlan_egr_xlate_action_get = xfs_ltsw_vlan_egr_xlate_action_get,

    /* egress vlan action xlate delete. */
    .vlan_egr_xlate_action_delete = xfs_ltsw_vlan_egr_xlate_action_delete,

    /* egress vlan action xlate traverse. */
    .vlan_egr_xlate_action_traverse = xfs_ltsw_vlan_egr_xlate_action_traverse,

    /* force vlan port type set. */
    .vlan_force_port_set = xfs_ltsw_vlan_force_port_set,

    /* force vlan port type get. */
    .vlan_force_port_get = xfs_ltsw_vlan_force_port_get,

    /* port vlan action set. */
    .vlan_port_action_set = xfs_ltsw_vlan_port_action_set,

    /* port vlan action get. */
    .vlan_port_action_get = xfs_ltsw_vlan_port_action_get,

    /* port vlan action reset. */
    .vlan_port_action_reset = xfs_ltsw_vlan_port_action_reset,

    /* attach flex counter to the given vlan. */
    .vlan_stat_attach = xfs_ltsw_vlan_stat_attach,

    /* detach flex counter to the given vlan. */
    .vlan_stat_detach = xfs_ltsw_vlan_stat_detach,

    /* Get counter entries of the given vlan. */
    .vlan_stat_get = xfs_ltsw_vlan_stat_get,

    /*! Attach flex-counter into a given ingress VLAN translation table. */
    .vlan_ing_xlate_flexctr_attach = xfs_ltsw_vlan_ing_xlate_flexctr_attach,

    /*! Detach flex-counter from a given ingress vlan translation table. */
    .vlan_ing_xlate_flexctr_detach = xfs_ltsw_vlan_ing_xlate_flexctr_detach,

    /*! Get flex-counter from a given ingress vlan translation table. */
    .vlan_ing_xlate_flexctr_get = xfs_ltsw_vlan_ing_xlate_flexctr_get,

    /*! Attach flex-counter into a given egress VLAN translation table. */
    .vlan_egr_xlate_flexctr_attach = xfs_ltsw_vlan_egr_xlate_flexctr_attach,

    /*! Detach flex-counter from a given egress vlan translation table. */
    .vlan_egr_xlate_flexctr_detach = xfs_ltsw_vlan_egr_xlate_flexctr_detach,

    /*! Get flex-counter from a given egress vlan translation table. */
    .vlan_egr_xlate_flexctr_get = xfs_ltsw_vlan_egr_xlate_flexctr_get,

    /*! Add a Gport to the specified vlan. */
    .vlan_gport_add = xfs_ltsw_vlan_gport_add,

    /*! Delete a Gport from the specified vlan. */
    .vlan_gport_delete = xfs_ltsw_vlan_gport_delete,

    /*! Delete all Gports from the specified vlan. */
    .vlan_gport_delete_all = xfs_ltsw_vlan_gport_delete_all,

    /*! Get a virtual or physical port from the specified VLAN. */
    .vlan_gport_get = xfs_ltsw_vlan_gport_get,

    /*! Get all virtual and physical ports from the specified VLAN. */
    .vlan_gport_get_all = xfs_ltsw_vlan_gport_get_all,

    /*! Get miscellaneous VLAN-specific chip options. */
    .vlan_control_get = xfs_ltsw_vlan_control_get,

    /*! Set miscellaneous VLAN-specific chip options. */
    .vlan_control_set = xfs_ltsw_vlan_control_set,

    /*! Update all VLAN information to L2 station. */
    .vlan_l2_station_update_all = xfs_ltsw_vlan_l2_station_update_all,

    /*! Update all VLAN information to L3 interface. */
    .vlan_l3_intf_update_all = xfs_ltsw_vlan_l3_intf_update_all,

    /*! Add VLAN range entry. */
    .vlan_translate_action_range_add = xfs_ltsw_vlan_translate_action_range_add,

    /*! Delete VLAN range entry. */
    .vlan_translate_action_range_delete = xfs_ltsw_vlan_translate_action_range_delete,

    /*! Delete all VLAN range entries. */
    .vlan_translate_action_range_delete_all = xfs_ltsw_vlan_translate_action_range_delete_all,

    /*! Traverses over VLAN range table and callback. */
    .vlan_translate_action_range_traverse = xfs_ltsw_vlan_translate_action_range_traverse,

    /*! Set per VLAN block configuration. */
    .vlan_block_set = xfs_ltsw_vlan_block_set,

    /*! Get per VLAN block configuration. */
    .vlan_block_get = xfs_ltsw_vlan_block_get,

    /*! Delete VLAN profile entry per index. */
    .vlan_profile_delete = xfs_ltsw_vlan_profile_delete,

    /*! Update VLAN range index when trunk member changes. */
    .vlan_range_update = xfs_ltsw_vlan_range_update,

    /*! Get port bitmap of reserved default VLAN flooding group. */
    .vlan_def_flood_port_get = xfs_ltsw_vlan_def_flood_port_get,

    /*! Update vlan xlate pipe sw info. */
    .vlan_xlate_pipe_update = xfs_ltsw_vlan_xlate_pipe_update,

    /*! Set STG into a specified VLAN entry. */
    .vlan_stg_set = xfs_ltsw_vlan_stg_set,

    /*! Get STG from a specified VLAN entry. */
    .vlan_stg_get = xfs_ltsw_vlan_stg_get,

    /*! Notify VLAN membership check information for L2 tunnel case.*/
    .vlan_check_info_notify = xfs_ltsw_vlan_check_info_notify,

    /*! Display VLAN software structure information. */
    .vlan_info_dump = xfs_ltsw_vlan_info_dump
};

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_ltsw_vlan_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv_set(unit, &bcm56880_a0_ltsw_vlan_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_ltsw_variant_drv_attach(unit, BCM56880_A0_LTSW_VARIANT_VLAN));
exit:
    SHR_FUNC_EXIT();
}
