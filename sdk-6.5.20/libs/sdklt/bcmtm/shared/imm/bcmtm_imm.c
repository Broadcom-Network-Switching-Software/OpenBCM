/*! \file bcmtm_imm.c
 *
 * TM imm callback function register.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>
#include <bcmtm/wred/bcmtm_wred_internal.h>
#include <bcmtm/ct/bcmtm_ct_internal.h>
#include <bcmtm/obm/bcmtm_obm_internal.h>
#include <bcmtm/oobfc/bcmtm_oobfc_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/pfc/bcmtm_pfc_deadlock_internal.h>
#include <bcmtm/ebst/bcmtm_ebst_internal.h>
#include <bcmtm/bcmtm_device_rxq_internal.h>
#include <bcmtm/mod/bcmtm_mirror_on_drop_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Public functions
 */
int
bcmtm_imm_cb_reg(int unit)
{
    bcmtm_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    drv->imm_update = bcmtm_imm_update;

    if (drv->scheduler_drv_get != NULL) {
        /* Scheduler shaper imm callback register */
        SHR_IF_ERR_EXIT
            (bcmtm_imm_scheduler_shaper_cb_reg(unit));
    }

    if (drv->ct_drv_get != NULL) {
        /* Cut-Through imm callback */
        SHR_IF_ERR_EXIT
            (bcmtm_ct_port_imm_register(unit));
    }

    if (drv->wred_drv_get != NULL) {
        /*! WRED imm callback */
        SHR_IF_ERR_EXIT
            (bcmtm_imm_wred_cb_reg(unit));
    }

    if (drv->obm_drv_get != NULL) {
        /* OBM imm callback */
        SHR_IF_ERR_EXIT
            (bcmtm_imm_obm_cb_reg(unit));
    }

    if (drv->bst_drv_get != NULL) {
        SHR_IF_ERR_EXIT
            (bcmtm_imm_bst_control_register(unit));

        SHR_IF_ERR_EXIT(bcmtm_imm_bst_event_register(unit));
    }

    if (drv->mc_drv_get != NULL) {
        /* Multicast */
        SHR_IF_ERR_EXIT
            (bcmtm_imm_mc_group_register(unit));
        SHR_IF_ERR_EXIT
            (bcmtm_imm_mc_port_list_register(unit));
    }


    if (drv->ebst_drv_get != NULL) {
        /* EBST */
        SHR_IF_ERR_EXIT
            (bcmtm_imm_ebst_control_register(unit));
        SHR_IF_ERR_EXIT
            (bcmtm_imm_ebst_port_register(unit));
        SHR_IF_ERR_EXIT
            (bcmtm_imm_ebst_portsp_register(unit));
        SHR_IF_ERR_EXIT
            (bcmtm_imm_ebst_q_register(unit));
        SHR_IF_ERR_EXIT
            (bcmtm_imm_ebst_sp_register(unit));
    }

    if (drv->pfc_drv_get != NULL) {
        /* PFC imm stage callback. */
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_imm_register(unit));
    }

    if (drv->oobfc_drv_get != NULL) {
        /* OOBFC */
        SHR_IF_ERR_EXIT
            (bcmtm_imm_oobfc_cb_reg(unit));
    }

    if (drv->pfc_ddr_drv_get != NULL) {
        /* PFC deadlock imm stage callback. */
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_deadlock_imm_register(unit));
    }

    if (drv->mod_drv_get != NULL) {
        /* Mirror on drop imm stage callback. */
        SHR_IF_ERR_EXIT
            (bcmtm_mod_imm_register(unit));
    }

    if (drv->rxq_drv_get) {
        /*! Device RX Queue imm stage callback. */
        SHR_IF_ERR_EXIT
            (bcmtm_device_pkt_rxq_imm_register(unit));
    }
    /*! Egress service pool */
    SHR_IF_ERR_EXIT
        (bcmtm_egr_sp_imm_register(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_imm_init(int unit, bool warm)
{
    bcmtm_drv_t *drv;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    if (drv->scheduler_drv_get != NULL) {
        /* Scheduler profile internal structure initialization. */
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_profile_imm_init(unit, warm));
    }
    if (drv->mc_drv_get != NULL) {
        /* Multicast port aggergate list initialization. */
        SHR_IF_ERR_EXIT
            (bcmtm_imm_mc_port_agg_list_init(unit, warm));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_imm_update(int unit, int lport, uint8_t action)
{
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE];
    bcmtm_drv_t *drv;

    SHR_FUNC_ENTER(unit);
    if (lport == BCMTM_INVALID_LPORT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));

    sal_memset(profile, 0,
               sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE);
    if (drv->scheduler_drv_get != NULL) {
        /* scheduler shaper updates */
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_port_update(unit, lport, PORT_UPDATE,
                                         profile, action));
    }

    if (drv->wred_drv_get != NULL) {
        /* wred updates */
        SHR_IF_ERR_EXIT
            (bcmtm_wred_ucq_internal_update(unit, lport, action));

        SHR_IF_ERR_EXIT
            (bcmtm_wred_portsp_internal_update(unit, lport, action));
    }

    if (drv->ct_drv_get != NULL) {
        /* cut-through updates. */
        SHR_IF_ERR_EXIT
            (bcmtm_ct_port_internal_update(unit, lport, action));
    }

    if (drv->obm_drv_get != NULL) {
        /* obm updates */
        SHR_IF_ERR_EXIT
            (bcmtm_obm_port_pkt_pri_tc_internal_update(unit, lport, action));

        SHR_IF_ERR_EXIT
            (bcmtm_obm_port_fc_internal_update(unit, lport, action));

        SHR_IF_ERR_EXIT
            (bcmtm_obm_port_pkt_parse_internal_update(unit, lport, action));
    }

    if (drv->pfc_drv_get != NULL) {
        /* PFC updates. */
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_port_internal_update(unit, lport, action));
    }

    if (drv->ebst_drv_get != NULL) {
        /*! EBST updates */
        SHR_IF_ERR_EXIT
            (bcmtm_ebst_port_internal_update(unit, lport, action));

        SHR_IF_ERR_EXIT
            (bcmtm_ebst_port_q_internal_update(unit, lport, action));

    }

    if (drv->mod_drv_get != NULL) {
        /* MoD updates. */
        SHR_IF_ERR_EXIT
            (bcmtm_mod_port_internal_update(unit, lport, action));
    }

    if (drv->oobfc_drv_get != NULL) {
        /* OOBFC updates */
        SHR_IF_ERR_EXIT
            (bcmtm_oobfc_port_internal_update(unit, lport, action));
    }

exit:
    SHR_FUNC_EXIT();
}
