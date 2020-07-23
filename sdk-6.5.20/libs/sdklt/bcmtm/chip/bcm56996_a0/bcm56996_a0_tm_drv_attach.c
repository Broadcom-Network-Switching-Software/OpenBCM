/*! \file bcm56996_a0_tm_drv_attach.c
 *
 * Traffic manager chip driver
 *
 * The library functions definitions for bcm56996_a0 device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/chip/bcm56996_a0_acc.h>
#include <bcmpc/bcmpc_tm.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/pfc/bcmtm_pfc_deadlock_internal.h>
#include <bcmtm/bcmtm_sysm.h>
#include <bcmtm/chip/bcm56996_a0_tm.h>
#include "bcm56996_a0_tm_ct.h"
#include "bcm56996_a0_tm_multicast_internal.h"
#include "bcm56996_a0_tm_bst.h"
#include "bcm56996_a0_tm_pfc.h"
#include "bcm56996_a0_tm_scheduler_shaper.h"
#include "bcm56996_a0_tm_wred.h"
#include "bcm56996_a0_tm_obm.h"
#include "bcm56996_a0_tm_ebst.h"
#include "bcm56996_a0_tm_mirror_on_drop.h"
#include "bcm56996_a0_tm_device_rxq.h"


#define BSL_LOG_MODULE  BSL_LS_BCMTM_INIT


/*!
 * \brief BCMTM device stop for TH4G.
 *
 * \param [in] unit Logical unit number.
 * \param [in] warm Warm boot.
 *
 * \return SHR_E_NONE No error.
 * \return !SHR_E_NONE Error code.
 */
static int
bcm56996_a0_bcmtm_dev_stop(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_bst_cleanup(unit));
exit:
    SHR_FUNC_EXIT();
}

static bcmpc_tm_handler_t *
bcm56996_a0_bcmtm_pc_hdl_get(int unit)
{
    static bcmpc_tm_handler_t hdl;

    hdl.tm_configure = bcmtm_pc_tm_configure;
    hdl.tm_validate = bcm56996_a0_tm_validate;
    hdl.port_op_exec = bcmtm_port_op_exec;

    return &hdl;
}

/*!
 * \brief BCMTM PFC deadlock handler get for TH4G.
 *
 * \param [in] unit Logical unit number.
 *
 * \return Return the handler get function for PFC deadlock configuration.
 */
static bcmtm_pfc_ddr_drv_t *
bcm56996_a0_bcmtm_pfc_ddr_drv_get(int unit)
{
    static bcmtm_pfc_ddr_drv_t drv = {
        .pt_set = bcm56996_a0_pfc_deadlock_config_hw_set,
        .hw_status_get = bcm56996_a0_pfc_deadlock_hw_status_get,
        .hw_recovery_begin = bcm56996_a0_pfc_deadlock_recovery_begin,
        .hw_recovery_end = bcm56996_a0_pfc_deadlock_recovery_end,
    };

    return &drv;
}

/*!
 * \brief BCMTM PFC handler get for TH4G.
 *
 * \param [in] unit Logical unit number.
 *
 * \return Return the handler function for PFC configuration.
 */
static bcmtm_pfc_drv_t *
bcm56996_a0_bcmtm_pfc_drv_get(int unit)
{
    static bcmtm_pfc_drv_t drv = {
        .egr_port_set = bcm56996_a0_tm_pfc_egr_port_set,
        .rx_pri_cos_map_set = bcm56996_a0_tm_pfc_rx_pri_cos_map_set,
    };

    return &drv;
}

static bcmtm_drv_t bcm56996_a0_tm_drv = {
    .dev_init = bcm56996_a0_tm_chip_init,
    .dev_stop = bcm56996_a0_bcmtm_dev_stop,
    .pc_hdl_get = bcm56996_a0_bcmtm_pc_hdl_get,
    .pt_sid_rename = NULL,
    .imm_init = bcm56996_a0_tm_imm_init,
    .chip_q_get = bcm56996_a0_tm_chip_q_get,
    .mchip_port_lport_get = bcm56996_a0_tm_mchip_port_lport_get,
    .port_ing_down = bcm56996_a0_tm_port_idb_down,
    .port_ing_up   = bcm56996_a0_tm_port_idb_up,
    .port_mmu_down = bcm56996_a0_tm_port_mmu_down,
    .port_mmu_up   = NULL,
    .port_egr_down = bcm56996_a0_tm_port_edb_down,
    .port_egr_up   = bcm56996_a0_tm_port_edb_up,
    .port_egr_fwd_enable = bcm56996_a0_ip_port_forwarding_bitmap_enable,
    .port_egr_fwd_disable = bcm56996_a0_ip_port_forwarding_bitmap_disable,
    .num_uc_q_cfg_get = bcm56996_a0_tm_num_uc_q_non_cpu_port_get,
    .num_mc_q_cfg_get = bcm56996_a0_tm_num_mc_q_non_cpu_port_get,
    .pfc_ddr_drv_get =  bcm56996_a0_bcmtm_pfc_ddr_drv_get,
    .mod_drv_get = bcm56996_a0_tm_mod_drv_get,
    .pfc_drv_get = bcm56996_a0_bcmtm_pfc_drv_get,
    .scheduler_drv_get = bcm56996_a0_tm_scheduler_drv_get,
    .shaper_drv_get = bcm56996_a0_tm_shaper_drv_get,
    .wred_drv_get = bcm56996_a0_tm_wred_drv_get,
    .obm_drv_get = bcm56996_a0_tm_obm_drv_get,
    .ct_drv_get = bcm56996_a0_tm_ct_drv_get,
    .mc_drv_get = bcm56996_a0_tm_mc_drv_get,
    .bst_drv_get = bcm56996_a0_tm_bst_drv_get,
    .ebst_drv_get = bcm56996_a0_tm_ebst_drv_get,
    .itm_valid_get = bcm56996_a0_tm_check_valid_itm,
    .port_egr_credit_reset = bcm56996_a0_tm_mmu_port_clear_edb_credit_counter,
    .tm_port_add = bcm56996_a0_tm_port_add,
    .tm_port_delete = bcm56996_a0_tm_port_delete,
    .portmap_update = bcm56996_a0_tm_drv_info_portmap_fill,
    .rxq_drv_get = bcm56996_a0_tm_rxq_drv_get,
    .check_ep_mmu_credit = bcm56996_a0_tm_check_ep_mmu_credit,
    .check_port_ep_credit = bcm56996_a0_tm_check_port_ep_credit,
};

/*******************************************************************************
* Public functions
 */
int
bcmtm_bcm56996_a0_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);
    bcmtm_drv_set(unit, &bcm56996_a0_tm_drv);
    SHR_FUNC_EXIT();
}
