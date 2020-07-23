/*! \file bcm56996_a0_tm_bst.c
 *
 * TM BST non-imm functionality implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_msgq.h>
#include <shr/shr_debug.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/chip/bcm56996_a0_tm.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmbd/bcmbd_mmu_intr.h>
#include <bcmbd/chip/bcm56996_a0_mmui_intr.h>
#include "bcm56996_a0_tm_bst.h"

/*******************************************************************************
 * Local definitions
 */
static struct bst_sid_list_s {
    bcmdrd_sid_t    sid;
    bcmdrd_fid_t    fid;
} bst_event_sid_list[] = {
    /* BST tracking. State = Armed, Armed passive. */
    /* instance = global */
    {MMU_GLBCFG_BST_TRACKING_ENABLEr, BST_TRACK_EN_THDI0f}, /* 0 */
    {MMU_GLBCFG_BST_TRACKING_ENABLEr, BST_TRACK_EN_THDI1f}, /* 1 */
    {MMU_GLBCFG_BST_TRACKING_ENABLEr, BST_TRACK_EN_THDO0f}, /* 2 */
    {MMU_GLBCFG_BST_TRACKING_ENABLEr, BST_TRACK_EN_THDO1f}, /* 3 */
    {MMU_GLBCFG_BST_TRACKING_ENABLEr, BST_TRACK_EN_THDR0f}, /* 4 */
    {MMU_GLBCFG_BST_TRACKING_ENABLEr, BST_TRACK_EN_THDR1f}, /* 5 */
    {MMU_GLBCFG_BST_TRACKING_ENABLEr, BST_TRACK_EN_CFAP0f}, /* 6 */
    {MMU_GLBCFG_BST_TRACKING_ENABLEr, BST_TRACK_EN_CFAP1f}, /* 7 */

    /* Skip BST interrupt enable. State = Armed. Set via API.*/

    /* BST interrupt set. State = Triggered. */
    /* instance = ITM */
    {MMU_THDI_CPU_INT_SET_INSTr, BST_TRIGf},                /* 8 */
    {MMU_THDO_BST_CPU_INT_SETr, BSTf},                      /* 9 */
    {MMU_THDR_QE_CPU_INT_SET_INSTr, BSTf},                  /* 10 */
    {MMU_CFAP_INT2_SETr, BST_TRIGf},                        /* 11 */
};

/*
 * Index into the bst_event_sid_list table for all states.
 * Input - class, state and all register indices.
 *          Register index is valid only if !(-1)
 */
static int
bst_state_sid_map[BST_EVENT_CLASS_MAX][BST_EVENT_STATE_MAX][TH4G_ITMS_PER_DEV] =
{
    /* Off,    Armed,  Armed passive, Triggered. */
    {{-1, -1}, {0, 1}, {0, 1}, { 8,  8}}, /* Ingress */
    {{-1, -1}, {2, 3}, {2, 3}, { 9,  9}}, /* Egress */
    {{-1, -1}, {4, 5}, {4, 5}, {10, 10}}, /* RQE */
    {{-1, -1}, {6, 7}, {6, 7}, {11, 11}}  /* Device */
};

/*
 * Index into the bst_event_sid_list table for state OFF
 * Input - class, register list
 */
static int
bst_state_sid_map_off[BST_EVENT_CLASS_MAX][3] =
{
/* ITM0 ITM1 INT_SET */
    {0, 1, 8},  /* Ingress */
    {2, 3, 9},  /* Egress */
    {4, 5, 10}, /* RQE */
    {6, 7, 11}  /* Device */
};

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief BST event notification trigger.
 *
 * \param [in] unit Logical unit number.
 * \param [in] itm TM buffer pool.
 * \param [in] class BST event class for which notification triggered.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56996_a0_bst_event_notify_triggered(int unit,
                                       int buffer_pool,
                                       bcmtm_bst_event_class_t class)
{
    bcmtm_bst_event_cfg_t bst_event_cfg;
    bcmtm_bst_dev_info_t *bst_dev_info;

    SHR_FUNC_ENTER(unit);

    bcmtm_bst_dev_info_get(unit, &bst_dev_info);
    sal_memset(&bst_event_cfg, 0, sizeof(bcmtm_bst_event_cfg_t));
    bst_event_cfg.class = class;
    bst_event_cfg.buffer_pool = buffer_pool;
    bst_event_cfg.state = BST_EVENT_STATE_TRIGGERED;

    SHR_IF_ERR_EXIT
        (bcmtm_bst_event_state_update(unit, &bst_event_cfg));

    /* update the other instance to be armed passive. */
    buffer_pool = (!buffer_pool & 0x1);
    if (bst_dev_info->trigger_state[buffer_pool][class]) {
        bst_event_cfg.class = class;
        bst_event_cfg.buffer_pool = buffer_pool;
        bst_event_cfg.state = BST_EVENT_STATE_ARMED_PASSIVE;

        SHR_IF_ERR_EXIT
            (bcmtm_bst_event_state_update(unit, &bst_event_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Ingress source LT function.
 *
 * The function updates the IMM Ingress source table with the
 * details of the event.
 * It then sets the state of TM_BST_EVENT_STATE to TRIGGERED.
 *
 * \param [in] unit Unit number
 * \param [in] ltid Logical table ID.
 * \param [in] pipe Pipe Number.
 * \param [in] type Ingress BST event type.
 * \param [in] data Interrupt data to be parsed.
 * \param [out] in List populated based on data deom interrupt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Internal implementation error.
 */
static int
bcm56996_a0_tm_bst_event_ing_src(int unit,
                                 bcmltd_sid_t ltid,
                                 int pipe,
                                 bcmtm_bst_ing_stat_t type,
                                 uint32_t data,
                                 bcmltd_fields_t *in)
{
    bcmltd_fid_t fid;
    int local_mport;
    int lport, mport;
    /*
     * This is inline to the bcmtm_bst_ing_stat_t.
     * The order should not be changed.
     */
    bcmltd_fid_t lt_field[] = {
        TM_BST_EVENT_SOURCE_INGt_SERVICE_POOLf,
        TM_BST_EVENT_SOURCE_INGt_PORT_SERVICE_POOLf,
        TM_BST_EVENT_SOURCE_INGt_PRI_GRP_HEADROOMf,
        TM_BST_EVENT_SOURCE_INGt_PRI_GRPf,
        TM_BST_EVENT_SOURCE_INGt_HEADROOM_POOLf,
    };

    SHR_FUNC_ENTER(unit);

    fid = lt_field[type];
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, in, fid, 0, 1));
    switch (type) {
        case SERVICE_POOL:
            fid = TM_BST_EVENT_SOURCE_INGt_SERVICE_POOL_INDEXf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, data));
            break;
        case PORT_SERVICE_POOL:
            fid = TM_BST_EVENT_SOURCE_INGt_PORT_SERVICE_POOL_INDEXf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, (data & 0x3)));
            local_mport = (data >> 2);
            mport =  pipe * TH4G_MMU_PORTS_PER_PIPE + local_mport;
            SHR_IF_ERR_EXIT
                (bcmtm_mport_lport_get(unit, mport, &lport));
            fid = TM_BST_EVENT_SOURCE_INGt_PORT_SERVICE_POOL_PORTf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, lport));
            break;
        case PG_HEADROOM:
            fid = TM_BST_EVENT_SOURCE_INGt_PRI_GRP_HEADROOM_INDEXf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, (data & 0x7)));
            local_mport = (data >> 3);
            mport =  pipe * TH4G_MMU_PORTS_PER_PIPE + local_mport;
            SHR_IF_ERR_EXIT
                (bcmtm_mport_lport_get(unit, mport, &lport));
            fid = TM_BST_EVENT_SOURCE_INGt_PRI_GRP_HEADROOM_PORTf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, lport));
            break;
        case PG_SHARED:
            fid = TM_BST_EVENT_SOURCE_INGt_PRI_GRP_INDEXf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, (data & 0x7)));
            local_mport = (data >> 3);
            mport =  pipe * TH4G_MMU_PORTS_PER_PIPE + local_mport;
            SHR_IF_ERR_EXIT
                (bcmtm_mport_lport_get(unit, mport, &lport));
            fid = TM_BST_EVENT_SOURCE_INGt_PRI_GRP_PORTf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, lport));
            break;
        case HEADROOM_POOL:
            fid = TM_BST_EVENT_SOURCE_INGt_HEADROOM_POOL_INDEXf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, data));
            break;
        default:
            SHR_EXIT();
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handles events from HW related to Ingress
 *
 * The function extracts the ING related parameters and
 * notifies the ING source LT tables.
 *
 * \param [in] unit Unit number
 * \param [in] buffer_pool Buffer pool ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Internal implementation error.
 */
static int
bcm56996_a0_bst_event_ingress(int unit,
                              int buffer_pool)
{
    uint32_t pipe_bmp;
    bcmltd_sid_t ltid = TM_BST_EVENT_SOURCE_INGt;
    bcmltd_fid_t fid;
    bcmdrd_sid_t sid0 = MMU_THDI_BST_TRIGGER_STATUS_TYPEr;
    bcmdrd_sid_t sid1 = THDI_BST_TRIGGER_STATUS_32r;
    uint32_t ltmbuf = 0, ltmbuf1 = 0;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_fields_t in;
    uint32_t fval, idx, pipe = 0;
    bcmdrd_fid_t data[] = { SP_SHARED_TRIGGER_STATUSf,
                            PORTSP_SHARED_TRIGGER_STATUSf,
                            PG_HDRM_TRIGGER_STATUSf,
                            PG_SHARED_TRIGGER_STATUSf,
                            HEADROOM_POOL_TRIGGER_STATUSf};
    bcmdrd_fid_t status[] = { SP_SHARED_TRIGGERf,
                              PORTSP_SHARED_TRIGGERf,
                              PG_HDRM_TRIGGERf,
                              PG_SHARED_TRIGGERf,
                              HEADROOM_POOL_TRIGGERf};

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_BST_EVENT_SOURCE_INGt_FIELD_COUNT, &in));

    in.count = 0;
    pipe_bmp = (buffer_pool == 1)? 0x0ff0: 0xf00f;

    while (pipe_bmp) {
        if (pipe_bmp & 0x1) {
            SHR_IF_ERR_EXIT
                (bcmtm_bst_event_src_ing_reset(unit, &in));
            BCMTM_PT_DYN_INFO(pt_info, 0, pipe);

            /* Read from PT the THDI interrupt status register. */
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, sid0, ltid, &pt_info, &ltmbuf));

            /* Get the interrupt state details. */
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, sid1, ltid, &pt_info, &ltmbuf1));

            for (idx = 0; idx < MAX_BST_ING_STATUS; idx++) {

                /* Checking for the interrupt type triggered. */
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, sid0, status[idx], &ltmbuf, &fval));

                /* If interrupt is set then only check for the data. */
                if (fval) {
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_get(unit, sid1, data[idx], &ltmbuf1, &fval));
                    SHR_IF_ERR_EXIT
                        (bcm56996_a0_tm_bst_event_ing_src(unit, ltid, pipe,
                                                          idx, fval, &in));
                }
            }
            fid = TM_BST_EVENT_SOURCE_INGt_TM_PIPEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, 0, pipe));
            SHR_IF_ERR_EXIT
                (bcmimm_entry_update(unit, 0, ltid, &in));
        }
        pipe_bmp >>= 1;
        pipe++;
    }
    SHR_IF_ERR_EXIT
        (bcm56996_a0_bst_event_notify_triggered(unit, buffer_pool,
                                                BST_EVENT_CLASS_ING));
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Egress source LT function.
 *
 * The function updates the IMM Egress source table with the
 * details of the event.
 * It then sets the state of TM_BST_EVENT_STATE to TRIGGERED.
 *
 * \param [in] unit Unit number
 * \param [in] ltid Logical table ID.
 * \param [in] type BST egress interrupt type.
 * \param [in] fval Data from interrupt to be parsed.
 * \param [out] in List to be populated based on the data from interrupt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Internal implementation error.
 */
static int
bcm56996_a0_bst_event_egress_source(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmtm_bst_egr_stat_t type,
                                    uint32_t fval,
                                    bcmltd_fields_t *in)
{
    bcmltd_fid_t fid;
    uint32_t q_id;
    bcmtm_q_type_t q_type;
    int lport, mchip_port;

    SHR_FUNC_ENTER(unit);

    switch (type) {
        case QUEUE:
            SHR_IF_ERR_EXIT
                (bcm56996_a0_tm_mmu_q_from_chip_q_get(unit, fval, &lport,
                                                      &q_id, &q_type));
            if (q_type == MC_Q) {/* multicast queue */
                fid = TM_BST_EVENT_SOURCE_EGRt_MC_Qf;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, in, fid, 0, 1));
                fid = TM_BST_EVENT_SOURCE_EGRt_TM_MC_Q_IDf;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, in, fid, 0, q_id));
                fid = TM_BST_EVENT_SOURCE_EGRt_MC_Q_PORTf;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, in, fid, 0, lport));
            } else if (q_type == UC_Q) { /* unicast queue */
                fid = TM_BST_EVENT_SOURCE_EGRt_UC_Qf;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, in, fid, 0, 1));
                fid = TM_BST_EVENT_SOURCE_EGRt_TM_UC_Q_IDf;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, in, fid, 0, q_id));
                fid = TM_BST_EVENT_SOURCE_EGRt_UC_Q_PORTf;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, in, fid, 0, lport));
            }
            break;
        case UC_PORT_SERVICE_POOL:
            fid = TM_BST_EVENT_SOURCE_EGRt_PORT_SERVICE_POOL_UCf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, 1));
            fid = TM_BST_EVENT_SOURCE_EGRt_PORT_SERVICE_POOL_UC_INDEXf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, (fval & 0x3)));
            mchip_port = fval >> 2;
            SHR_IF_ERR_EXIT
                (bcmtm_mchip_port_lport_get(unit, mchip_port, &lport));
            fid = TM_BST_EVENT_SOURCE_EGRt_PORT_SERVICE_POOL_UC_PORTf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, lport));
            break;
        case UC_SERVICE_POOL:
            fid = TM_BST_EVENT_SOURCE_EGRt_SERVICE_POOL_UCf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, 1));
            fid = TM_BST_EVENT_SOURCE_EGRt_SERVICE_POOL_UC_INDEXf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, fval));
            break;
        case MC_PORT_SERVICE_POOL:
            fid = TM_BST_EVENT_SOURCE_EGRt_PORT_SERVICE_POOL_MCf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, 1));
            fid = TM_BST_EVENT_SOURCE_EGRt_PORT_SERVICE_POOL_MC_INDEXf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, (fval & 0x3)));
            mchip_port = fval >> 2;
            SHR_IF_ERR_EXIT
                (bcmtm_mchip_port_lport_get(unit, mchip_port, &lport));
            fid = TM_BST_EVENT_SOURCE_EGRt_PORT_SERVICE_POOL_MC_PORTf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, lport));
            break;
        case MC_SERVICE_POOL:
            fid = TM_BST_EVENT_SOURCE_EGRt_SERVICE_POOL_MCf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, 1));
            fid = TM_BST_EVENT_SOURCE_EGRt_SERVICE_POOL_MC_INDEXf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, in, fid, 0, fval));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handles events from HW related to Egress
 *
 * The function extracts the EGR related parameters and
 * notifies the EGR source LT tables.
 *
 * \param [in] unit Unit number
 * \param [in] itm TM ITM number
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Internal implementation error.
 */
static int
bcm56996_a0_bst_event_egress(int unit, int itm)
{
    uint32_t fval;
    uint32_t ltmbuf = 0, ltmbuf1 = 0;
    bcmdrd_sid_t sid, sid1;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = TM_BST_EVENT_SOURCE_EGRt;
    bcmltd_fields_t in;
    bcmtm_bst_egr_stat_t type;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_BST_EVENT_SOURCE_EGRt_FIELD_COUNT, &in));
    in.count = 0;

    SHR_IF_ERR_EXIT
        (bcmtm_bst_event_src_egr_reset(unit, &in));
    /* Read MMU_THDO_BST_STAT register for interrupts */
    BCMTM_PT_DYN_INFO(pt_info, 0, itm);
    sid = MMU_THDO_BST_STATr;
    sid1 = MMU_THDO_BST_STAT1r;

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid1, ltid, &pt_info, &ltmbuf1));

    /* Get the interrupts enabled */
    fid = TM_BST_EVENT_SOURCE_EGRt_BUFFER_POOLf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, itm));

    /* MMU_THDO_QUE_TOT_BST_TRIG */
    type = QUEUE;
    fid = MMU_THDO_QUE_TOT_BST_TRIGf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    if (fval) {
        fid = MMU_THDO_QUE_TOT_BST_IDf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcm56996_a0_bst_event_egress_source(unit, ltid, type, fval, &in));
    }

    /* MMU_THDO_PRT_SP_SHR_BST_TRIG */
    type = UC_PORT_SERVICE_POOL;
    fid = MMU_THDO_PRT_SP_SHR_BST_TRIGf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    if (fval) {
        fid = MMU_THDO_PRT_SP_SHR_BST_IDf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcm56996_a0_bst_event_egress_source(unit, ltid, type, fval, &in));
    }

    /* MMU_THDO_SP_SHR_BST_TRIG */
    type = UC_SERVICE_POOL;
    fid = MMU_THDO_SP_SHR_BST_TRIGf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    if (fval) {
        fid = MMU_THDO_SP_SHR_BST_IDf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcm56996_a0_bst_event_egress_source(unit, ltid, type, fval, &in));
    }

    /* MMU_THDO_MC_CQE_PRT_SP_BST_TRIG */
    type = MC_PORT_SERVICE_POOL;
    fid = MMU_THDO_MC_CQE_PRT_SP_BST_TRIGf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    if (fval) {
        fid = MMU_THDO_MC_CQE_PRT_SP_BST_IDf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid1, fid, &ltmbuf1, &fval));
        SHR_IF_ERR_EXIT
            (bcm56996_a0_bst_event_egress_source(unit, ltid, type, fval, &in));
    }

    /* MMU_THDO_MC_CQE_SP_BST_TRIG */
    type = MC_SERVICE_POOL;
    fid = MMU_THDO_MC_CQE_SP_BST_TRIGf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    if (fval) {
        fid = MMU_THDO_MC_CQE_SP_BST_IDf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid1, fid, &ltmbuf1, &fval));
        SHR_IF_ERR_EXIT
            (bcm56996_a0_bst_event_egress_source(unit, ltid, type, fval, &in));
    }

    /* Notify the BST Source LT about the event details. */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
    SHR_IF_ERR_EXIT
        (bcm56996_a0_bst_event_notify_triggered(unit, itm,
                                                BST_EVENT_CLASS_EGR));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handles events from HW related to RQE
 *
 * The function extracts the RQE related parameters and
 * updates the source LT tables. It then sets the state
 * of TM_BST_EVENT_STATE to TRIGGERED.
 *
 * \param [in] unit Unit number
 * \param [in] itm TM ITM number
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56996_a0_bst_event_repl_queue(int unit, int itm)
{
    uint32_t fval;
    uint32_t ltm_buf0 = {0};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = TM_BST_EVENT_SOURCE_REPL_Qt;
    bcmltd_fid_t fid;
    bcmdrd_sid_t sid;
    bcmltd_fields_t in_flds;
    size_t count = 0;

    SHR_FUNC_ENTER(unit);


    sal_memset(&in_flds, 0, sizeof(bcmltd_fields_t));
    count = TM_BST_EVENT_SOURCE_REPL_Qt_FIELD_COUNT;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, count, &in_flds));

    in_flds.count = 0;
    BCMTM_PT_DYN_INFO(pt_info, 0, itm);

    /* Read from PT the RQE interrupt status register. */
    sid = MMU_THDR_QE_BST_STATr;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltm_buf0));

    /* Check if PRIQ is triggered. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, PRIQ_BST_TRIGGEREDf, &ltm_buf0, &fval));

    fid = TM_BST_EVENT_SOURCE_REPL_Qt_PRI_Qf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in_flds, fid, 0, fval));

    /* Retrieve the RQE queue which triggered BST interrupt. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, PRIQ_BST_IDf, &ltm_buf0, &fval));

    fid = TM_BST_EVENT_SOURCE_REPL_Qt_REPL_Q_NUMf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in_flds, fid, 0, fval));


    /* Check if RQE SP is triggered. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, SP_BST_TRIGGEREDf, &ltm_buf0, &fval));

    fid = TM_BST_EVENT_SOURCE_REPL_Qt_SERVICE_POOLf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in_flds, fid, 0, fval));

    /* Notify the BST Source LT about the event details. */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in_flds));
    SHR_IF_ERR_EXIT
        (bcm56996_a0_bst_event_notify_triggered(unit, itm,
                                        BST_EVENT_CLASS_REPL_QUEUE));
exit:
    bcmtm_field_list_free(&in_flds);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handles events from HW related to CFAP
 *
 * \param [in] unit Unit number
 * \param [in] itm TM ITM number
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56996_a0_bst_event_cfap(int unit, int itm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56996_a0_bst_event_notify_triggered(unit, itm,
                                                BST_EVENT_CLASS_DEVICE));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable BST interrupt for a class.
 *
 * \param [in] unit Unit number.
 * \param [in] class BST class.
 * \param [in] enable Enable or disable interrupt.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56996_a0_tm_bst_intr_enable(int unit,
                               bcmtm_bst_event_class_t class,
                               bcmtm_intr_enable_t enable)
{
    int intr_num;
    SHR_FUNC_ENTER(unit);

    switch (class) {
        case BST_EVENT_CLASS_ING:
            intr_num = MMUI_INTR_THDI_BST;
            break;
        case BST_EVENT_CLASS_EGR:
            intr_num = MMUI_INTR_THDO_BST;
            break;
        case BST_EVENT_CLASS_REPL_QUEUE:
            intr_num = MMUI_INTR_THDR_BST;
            break;
        case BST_EVENT_CLASS_DEVICE:
            intr_num = MMUI_INTR_CFAP_BST;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_mmui_interrupt_enable(unit, intr_num,
                                bcm56996_a0_tm_bst_intr_notify, enable));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the BST interrupt.
 *
 * \param [in] unit Logical unit number.
 * \param [in] buffer_pool Buffer pool ID.
 * \param [in] BST interrupt class.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Error in unit ID.
 */
static int
bcm56996_a0_tm_bst_intr_clear(int unit,
                              int buffer_pool,
                              bcmtm_bst_event_class_t class)
{
    uint32_t intr_num;
    SHR_FUNC_ENTER(unit);
    switch (class) {
        case BST_EVENT_CLASS_ING:
            intr_num = MMUI_INTR_THDI_BST;
            break;
        case BST_EVENT_CLASS_EGR:
            intr_num = MMUI_INTR_THDO_BST;
            break;
        case BST_EVENT_CLASS_REPL_QUEUE:
            intr_num = MMUI_INTR_THDR_BST;
            break;
        case BST_EVENT_CLASS_DEVICE:
            intr_num = MMUI_INTR_CFAP_BST;
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    MMU_INTR_INST_SET(intr_num, buffer_pool);
    SHR_IF_ERR_EXIT
        (bcmbd_mmui_intr_clear(unit, intr_num));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calls the handler once interrupt is triggered based on the interrupt
 * received.
 *
 * \param [in] unit Logical unit number.
 * \param [in] trigger_info Trigger information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56996_a0_tm_bst_trigger(int unit,
                           bcmtm_bst_trigger_t *trigger_info)
{
    int buffer_pool;
    bcmtm_bst_dev_info_t *bst_dev_info;

    SHR_FUNC_ENTER(unit);

    bcmtm_bst_dev_info_get(unit, &bst_dev_info);

    buffer_pool = trigger_info->buffer_pool;

    if (bst_dev_info->trigger_state[buffer_pool][trigger_info->class]) {
        switch (trigger_info->class) {
            /* Ingress BST events */
            case BST_EVENT_CLASS_ING:
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_bst_event_ingress(unit, buffer_pool));
                break;
                /* Egress BST events */
            case BST_EVENT_CLASS_EGR:
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_bst_event_egress(unit, buffer_pool));
                break;
                /* Replication queue BST events */
            case BST_EVENT_CLASS_REPL_QUEUE:
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_bst_event_repl_queue(unit, buffer_pool));
                break;
                /* Device CFAP BST events */
            case BST_EVENT_CLASS_DEVICE:
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_bst_event_cfap(unit, buffer_pool));
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tm_bst_intr_clear(unit, buffer_pool, trigger_info->class));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tm_bst_intr_clear(unit, buffer_pool, trigger_info->class));
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tm_bst_intr_enable(unit, trigger_info->class, INTR_ENABLE));
    }
exit:
    SHR_FUNC_EXIT();
}

/*! \brief BST control physical table settings.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] bst_cfg BST user provided configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error codes.
 */
static int
bcm56996_a0_tm_bst_control_pt_set(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmtm_bst_control_cfg_t *bst_cfg)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf, fval;
    int itm;
    int snapshot_en_mask;

    SHR_FUNC_ENTER(unit);

    switch (bst_cfg->class) {
        case BST_EVENT_CLASS_ING:
            sid = MMU_THDI_BSTCONFIGr;
            snapshot_en_mask = 0xc;
            break;
        case BST_EVENT_CLASS_EGR:
            sid = MMU_THDO_BST_CONFIGr;
            snapshot_en_mask = 0x3;
            break;
        case BST_EVENT_CLASS_REPL_QUEUE:
            sid = MMU_THDR_QE_BST_CONFIGr;
            snapshot_en_mask = 0x30;
            break;
        case BST_EVENT_CLASS_DEVICE:
            sid = MMU_CFAP_BSTCONFIGr;
            snapshot_en_mask = 0xc0;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Tracking mode */
    fid = (bst_cfg->class == BST_EVENT_CLASS_REPL_QUEUE) ?
                    TRACK_MODEf : TRACKING_MODEf;
    fval = bst_cfg->mode;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    /* Clear on read for BST counters. */
    fid = (bst_cfg->class == BST_EVENT_CLASS_ING)? HW_CORf: HWM_CORf;
    fval = bst_cfg->clear_on_read;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    /* Snapshot enable (high water mark) and usage mode. */
    fid = (bst_cfg->class == BST_EVENT_CLASS_REPL_QUEUE) ?
                    HW_SNAPSHOT_ENf: SNAPSHOT_ENf;
    fval = bst_cfg->snapshot;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    /*! Writing for both the itms. */
    for (itm = 0; itm < TH4G_ITMS_PER_DEV; itm++) {
        /* skipping for EGR as the register if duplicate. */
        if ((bst_cfg->class == BST_EVENT_CLASS_EGR) && (itm == 1)) {
            continue;
        }
        if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, itm))) {
            BCMTM_PT_DYN_INFO(pt_info, 0, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
        }
    }

    ltmbuf = 0;
    /* MMU_GLBCFG_BST_SNAPSHOT_ACTION_ENr */
    sid = MMU_GLBCFG_BST_SNAPSHOT_ACTION_ENr;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    if (bst_cfg->snapshot) {
        ltmbuf |= snapshot_en_mask;
    } else {
        ltmbuf &= ~(snapshot_en_mask);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief BST Event notify enable.
 * This enables the event registers when the state changes from any stated to
 * ARMED or TRIGGERED state.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] buffer_pool Buffer pool ID.
 * \param [in] class BST class.
 * \param [in] state BST event state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error codes.
 */
static int
bcm56996_a0_tm_bst_event_state_enable(int unit,
                                      bcmltd_sid_t ltid,
                                      uint8_t buffer_pool,
                                      bcmtm_bst_event_class_t class,
                                      bcmtm_bst_event_state_t state)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_info = {0};
    int map_id;
    uint32_t ltmbuf[2] = {0}, fval = 1;

    SHR_FUNC_ENTER(unit);

    map_id = bst_state_sid_map[class][state][buffer_pool];

    sid = bst_event_sid_list[map_id].sid;
    fid = bst_event_sid_list[map_id].fid;

    /* triggered registers are per buffer pool. */
    if (state != BST_EVENT_STATE_TRIGGERED) {
        BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    } else {
        BCMTM_PT_DYN_INFO(pt_info, 0, buffer_pool);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief BST Event notify enable.
 * This enables the event registers when the state changes from any stated to
 * OFF state.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] buffer_pool Buffer pool ID.
 * \param [in] class BST class.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error codes.
 */
static int
bcm56996_a0_tm_bst_event_state_disable(int unit,
                                       bcmltd_sid_t ltid,
                                       uint8_t buffer_pool,
                                       bcmtm_bst_event_class_t class)
{
    int map_id;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t fval = 0, ltmbuf[2] = {0};

    SHR_FUNC_ENTER(unit);

    /* BST tracking disable */
    map_id = bst_state_sid_map_off[class][buffer_pool];
    sid = bst_event_sid_list[map_id].sid;
    fid = bst_event_sid_list[map_id].fid;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));

    /*
     * For INT_SET registers, buffer_pool access is per instance.
     */
    map_id = bst_state_sid_map_off[class][2];
    sid = bst_event_sid_list[map_id].sid;
    fid = bst_event_sid_list[map_id].fid;
    BCMTM_PT_DYN_INFO(pt_info, 0, buffer_pool);

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief BST event control configure.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] bst_event_cfg BST event control configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error codes.
 */
static int
bcm56996_a0_tm_bst_event_control_set(int unit,
                                     bcmltd_sid_t ltid,
                                     bcmtm_bst_event_cfg_t *bst_event_cfg)
{
    uint8_t buffer_pool;
    bcmtm_bst_event_class_t class = 0;
    bcmtm_bst_event_state_t state = 0;

    SHR_FUNC_ENTER(unit);

    buffer_pool = bst_event_cfg->buffer_pool;
    class = bst_event_cfg->class;
    state = bst_event_cfg->state;

    /* Disabling the BST events to OFF. */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_tm_bst_event_state_disable(unit, ltid, buffer_pool, class));
    /* Clear all the interrupts. */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_tm_bst_intr_clear(unit, buffer_pool, class));

    switch (state) {
        /*
         * Event state set to ARMED. The BST interrupts are enabled and
         * receives an interrupt when the threshold mark is reached.
         */
        case BST_EVENT_STATE_ARMED:
        /*
         * This is used by application to get the current snapshot using
         *  software trigger.
         */
        /* Fall through */
        case BST_EVENT_STATE_TRIGGERED:
            /*! Interrupt is enable in case of state is armed. */
            SHR_IF_ERR_EXIT
                (bcm56996_a0_tm_bst_intr_enable(unit, class, INTR_ENABLE));
        /* Fall through */
        case BST_EVENT_STATE_ARMED_PASSIVE:
            /*! Programming the event state */
            SHR_IF_ERR_EXIT
                (bcm56996_a0_tm_bst_event_state_enable(unit, ltid, buffer_pool,
                                                       class, state));
            break;
            /*! BST Event state switched to OFF. */
        case BST_EVENT_STATE_OFF:
            SHR_IF_ERR_EXIT
                (bcm56996_a0_tm_bst_intr_enable(unit, class, INTR_DISABLE));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
void
bcm56996_a0_tm_bst_intr_notify(int unit,
                               uint32_t intr_param)
{
    uint32_t intr_num, intr_inst;
    bcmtm_bst_dev_info_t *bd;
    bcmtm_bst_trigger_t trigger_info;

    bcmtm_bst_dev_info_get(unit, &bd);
    sal_memset(&trigger_info, 0, sizeof(bcmtm_bst_trigger_t));

    intr_num = MMU_INTR_NUM_GET(intr_param);
    intr_inst = MMU_INTR_INST_GET(intr_param);
    trigger_info.buffer_pool = intr_inst;

    /* disabling and clearing the interrupt as interrupt is received. */
    (void)(bcmbd_mmui_intr_disable(unit, intr_num));

    /* Interrupt message recieved. */
    switch (intr_num) {
        case MMUI_INTR_THDI_BST:
            trigger_info.class = BST_EVENT_CLASS_ING;
            break;
        case MMUI_INTR_THDO_BST:
            trigger_info.class = BST_EVENT_CLASS_EGR;
            break;
        case MMUI_INTR_THDR_BST:
            trigger_info.class = BST_EVENT_CLASS_REPL_QUEUE;
            break;
        case MMUI_INTR_CFAP_BST:
            trigger_info.class = BST_EVENT_CLASS_DEVICE;
            break;
        default:
            return;
    }
    trigger_info.type = BST_TRIG_NOTIF;
    /* posting the message to message queue. */
    if (bd && bd->bcmtm_bst_trigger_q) {
        (void) sal_msgq_post(bd->bcmtm_bst_trigger_q,
                             &trigger_info,
                             SAL_MSGQ_HIGH_PRIORITY, SAL_MSGQ_FOREVER);
    }
}

int
bcm56996_a0_tm_bst_drv_get(int unit, void *bst_drv)
{
    bcmtm_bst_drv_t bcm56996_a0_bst_drv = {
        .bst_event_control_set = bcm56996_a0_tm_bst_event_control_set,
        .bst_control_pt_set = bcm56996_a0_tm_bst_control_pt_set,
        .bst_trigger = bcm56996_a0_tm_bst_trigger,
    };
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(bst_drv, SHR_E_INTERNAL);
    *((bcmtm_bst_drv_t *)bst_drv) = bcm56996_a0_bst_drv;
exit:
    SHR_FUNC_EXIT();
}


