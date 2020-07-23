/*! \file bcm56996_a0_tm_scheduler.c
 *
 * Scheduler related api and function for bcm56996_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56996_a0_tm.h>
#include "bcm56996_a0_tm_scheduler_shaper.h"
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/*******************************************************************************
 * Local definitions
 */
static bcm56996_a0_tm_cos_l0_map_t
bcm56996_a0_tm_cos_l0_map[BCMDRD_CONFIG_MAX_UNITS][TH4G_NUM_SCHED_PROFILE];
#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP
/*******************************************************************************
 * Private functions
 */
static int
bcm56996_a0_tm_cos_l0_map_set(int unit,
                              int profile_id,
                              bcmtm_scheduler_profile_t *profile,
                              int *opcode)
{
    int idx;
    int max_num_ucq, max_num_mcq,num_ucq, num_mcq, total_node_q, total_q = 0;
    int total_mcq = 0, total_ucq = 0;
    bcm56996_a0_tm_cos_l0_map_t *l0_map =
            &(bcm56996_a0_tm_cos_l0_map[unit][profile_id]);

    SHR_FUNC_ENTER(unit);

    /*
     * Number of unicast queue and number of multicast queue are
     * as per MMU_Q_MODE.
     */
    max_num_mcq = bcm56996_a0_tm_num_mc_q_non_cpu_port_get(unit);
    max_num_ucq = bcm56996_a0_tm_num_uc_q_non_cpu_port_get(unit);
    for (idx = 0; idx < TH4G_TM_NUM_Q; idx++) {
        num_ucq = profile[idx].num_ucq;
        num_mcq = profile[idx].num_mcq;
        total_node_q = num_ucq + num_mcq;
        total_ucq += num_ucq;
        total_mcq += num_mcq;
        total_q += (total_node_q);
        if (total_node_q) {
            l0_map->map[idx] = total_q - 1;
        }
    }
    if (total_mcq > max_num_mcq) {
        *opcode = MC_Q_LIMIT_EXCEEDS;
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (total_ucq > max_num_ucq) {
        *opcode = UC_Q_LIMIT_EXCEEDS;
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (total_q != TH4G_TM_NUM_Q) {
        *opcode = SCHED_PROFILE_INCOMPLETE;
        SHR_VERBOSE_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_tm_mmuq_l1_map_get(int unit,
                               uint8_t profile_id,
                               uint32_t sched_level,
                               uint32_t sched_node,
                               int *l1_node)
{
    int mmu_q_id;
    int mcq_offset;
    uint32_t fval, ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    bcmdrd_sid_t sid;
    bcmdrd_sid_t fid;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_fid_t sched_q_fid[]= {SCH_Q_NUM_0f, SCH_Q_NUM_1f, SCH_Q_NUM_2f,
                                 SCH_Q_NUM_3f, SCH_Q_NUM_4f, SCH_Q_NUM_5f,
                                 SCH_Q_NUM_6f, SCH_Q_NUM_7f, SCH_Q_NUM_8f,
                                 SCH_Q_NUM_9f, SCH_Q_NUM_10f, SCH_Q_NUM_11f};

    SHR_FUNC_ENTER(unit);
    mmu_q_id = sched_node;
    if (sched_level == L1_SCHED_MULTICAST_QUEUE) {
        mcq_offset = bcm56996_a0_tm_num_uc_q_non_cpu_port_get(unit);
        mmu_q_id += mcq_offset;
    }
    sid = MMU_EBPCC_MMUQ_SCHQ_PROFILEr;
    fid = sched_q_fid[mmu_q_id];

    BCMTM_PT_DYN_INFO(pt_dyn_info, profile_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, -1, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &fval));
    *l1_node = fval;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Node scheduler profile.
 *
 * Validates the node profile. Each node can have maximum of two queues. The
 * possible combinations are two unicast queues, one unicast one multicast,
 * one unicast queue and one multicast queue.
 *
 * \param [in] unit Unit number.
 * \param [in] node_id Scheduler node ID.
 * \param [in] profile Profile
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in profile.
 */
static int
bcm56996_a0_tm_scheduler_node_profile_validate(int unit,
                                               int node_id,
                                               bcmtm_scheduler_profile_t *profile)
{
    int num_ucq, num_mcq, total_q;

    SHR_FUNC_ENTER(unit);

    num_ucq = profile->num_ucq;
    num_mcq = profile->num_mcq;
    total_q = num_mcq + num_ucq;
    if ((num_ucq > 2) || (num_mcq > 1) || (total_q > 2)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Scheduler profile validate.
 *
 * Validates the number of unicast queues and multicast queues are as per
 * MMU_Q_MODE. Verifies that L0.0 can have only one L1 node. Other L0.n nodes
 * can have L1.n and L1.(n-1) nodes. One L0 node can have maximum of two queues
 * one unicast + one multicast, two unicast, one unicast or one multicast queue.
 * It also verifies no two consecutive nodes have two queues attached to it.
 *
 * \param [in] unit  Unit number.
 * \param [in/out] profile Logical port ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcm56996_a0_tm_scheduler_profile_validate(int unit,
                                          uint8_t profile_id,
                                          int node_id,
                                          bcmtm_scheduler_profile_t *profile)
{
    uint16_t total_ucq = 0, total_mcq = 0;
    int idx, l0_idx, opcode = SCHED_PROFILE_INCOMPLETE;
    int num_ucq, num_mcq, total_q;
    uint8_t l1_map[TH4G_TM_NUM_Q]= {0};
    bcm56996_a0_tm_cos_l0_map_t *l0_map;

    SHR_FUNC_ENTER(unit);

    if (SHR_E_NONE !=
            bcm56996_a0_tm_cos_l0_map_set(unit, profile_id, profile, &opcode)) {
        goto opstatus_update;
    }

    l0_map = &(bcm56996_a0_tm_cos_l0_map[unit][profile_id]);
    for (idx = 0; idx < TH4G_TM_NUM_Q; idx++) {
        num_ucq = profile[idx].num_ucq;
        num_mcq = profile[idx].num_mcq;
        total_q = num_mcq + num_ucq;
        total_ucq += num_ucq;
        total_mcq += num_mcq;

        /* Verifying L0->L1 connection.
         * L0.0 can have only one L1 node (L1.0)
         * Other L0.n can have L1.n and L1.(n-1) node.
         */
        l0_idx = l0_map->map[idx];
        if (l0_idx == -1) {
            break;
        }
        if (total_q == 1) {
            if ((l1_map[l0_idx] == 0) || ((l0_idx > 0) && (l1_map[l0_idx-1]) == 0)) {
                if ((l0_idx > 0) && (l1_map[l0_idx - 1]) == 0) {
                    l1_map[l0_idx - 1] = 1;
                } else {
                    l1_map[l0_idx] = 1;
                }
            } else {
                opcode = SCHED_PROFILE_INVALID;
                goto opstatus_update;
            }
        }
        if (total_q == 2) {
            if ((l1_map[l0_idx] == 0) && (l1_map[l0_idx-1] == 0)) {
                l1_map[l0_idx] = 1;
                l1_map[l0_idx - 1] = 1;
            } else {
                opcode = SCHED_PROFILE_INVALID;
                goto opstatus_update;
            }
        }
    }
    opcode = VALID;
opstatus_update:
    for (idx = 0; idx < TH4G_TM_NUM_Q; idx++) {
        if ((profile[idx].opcode != ENTRY_INVALID) || (idx == node_id)) {
            profile[idx].opcode = opcode;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Scheduler node physical table configure.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] sched_node_id Scheduler node ID.
 * \param [in] sched_level Scheduler level.
 * \param [in] ltid Logical table ID.
 * \param [in] scheduler_cfg Scheduler configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcm56996_a0_tm_scheduler_node_set(int unit,
        bcmtm_lport_t lport,
        uint32_t sched_node_id,
        uint32_t sched_level,
        bcmltd_sid_t ltid,
        bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval = 0;
    int mport, pipe, index, node_id;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint8_t profile_id;

    SHR_FUNC_ENTER(unit);

    sid = MMU_QSCH_PORT_CONFIGr;
    SHR_IF_ERR_EXIT
        (bcmtm_lport_mport_get(unit, lport, &mport));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, ltmbuf));

    profile_id = scheduler_cfg->profile_id;
    if (sched_level == L0_SCHED_COS) {/* L0 Level */
        fid = SP_L0_CHILDf;
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tm_sched_node_l0_map_get(unit, profile_id,
                                                  sched_node_id, &node_id));
    } else {/* L1 level */
        fid = SP_L0_PARENTf;
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tm_mmuq_l1_map_get(unit, profile_id, sched_level,
                                           sched_node_id, &node_id));
    }

    /* Incomplete scheduler profile. */
    if (node_id == -1) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &fval));
    if (scheduler_cfg->sched_mode == 0) {
        /* Strict Priority */
        SHR_BITSET(&fval, node_id);
    } else {
        /* Round Robin */
        SHR_BITCLR(&fval, node_id);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));

    sal_memset(ltmbuf, 0, sizeof(uint32_t) * BCMTM_MAX_ENTRY_WSIZE);

    SHR_IF_ERR_EXIT(bcmtm_lport_mmu_local_port_get(unit, lport, &mport));
    SHR_IF_ERR_EXIT(bcmtm_lport_pipe_get(unit, lport, &pipe));

    /* Scheduler Weight */
    if (sched_level == L0_SCHED_COS) {
        /* L0 level */
        sid = MMU_QSCH_L0_WEIGHT_MEMm;
    } else {
        /* L1 level */
        sid = MMU_QSCH_L1_WEIGHT_MEMm;
    }
    fid = WEIGHTf;
    fval = scheduler_cfg->weight;

    if (bcmtm_lport_is_mgmt(unit, lport)) {
        index = TH4G_TM_MGMT_Q_OFFSET + node_id;
    } else if (bcmtm_lport_is_lb(unit, lport)) {
        index = TH4G_TM_LB_Q_OFFSET + node_id;
    } else {
        index = mport * TH4G_TM_NUM_Q + node_id;
    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L1 to L0 mapping.
 *
 * Creates the bit map mapping L1 to L0 based on the profile created for
 * the scheduling port.
 *
 * \param [in] unit Unit.
 * \param [in] profile Scheduler profile.
 * \param [out] bmp L1 to L0 bitmap.
 */
static void
bcm56996_a0_tm_l1_to_l0_mapping(int unit,
                                uint32_t profile_id,
                                bcmtm_scheduler_profile_t *profile,
                                uint32_t *bmap)
{
    int idx, l0_idx, num_ucq, num_mcq, total_q;
    int l1_map[TH4G_TM_NUM_Q];
    bcm56996_a0_tm_cos_l0_map_t *l0_map;

    l0_map = &(bcm56996_a0_tm_cos_l0_map[unit][profile_id]);

    sal_memset(l1_map, -1, sizeof(int) * TH4G_TM_NUM_Q);

    for (idx = 0; idx < TH4G_TM_NUM_Q; idx++) {
        num_ucq = profile[idx].num_ucq;
        num_mcq = profile[idx].num_mcq;
        total_q = num_ucq + num_mcq;

        l0_idx = l0_map->map[idx];
        if (l0_idx == -1) {
            break;
        }
        if (total_q == 1) {
            if ((l0_idx > 0) && (l1_map[l0_idx - 1]) == -1) {
                l1_map[l0_idx - 1] = l0_idx;
            } else {
                l1_map[l0_idx] = l0_idx;
            }
        }
        if (total_q == 2) {
            l1_map[l0_idx] = l0_idx;
            l1_map[l0_idx - 1] = l0_idx;
        }
    }

    for (idx = 0; idx < TH4G_TM_NUM_Q; idx++) {
        if (l1_map[idx] != idx) {
            SHR_BITSET(bmap, idx);
        }
    }
}

/*!
 * \brief Scheduler port table configure.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] ltid Logical table ID.
 * \param [in] scheduler_cfg Scheduler configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcm56996_a0_tm_scheduler_port_set(int unit,
        bcmltd_sid_t ltid,
        bcmtm_lport_t lport,
        bcmtm_scheduler_profile_t *profile,
        bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[2] = {0};
    uint32_t fval = 0, bmap = 0, profile_id;
    bcmtm_pt_info_t pt_dyn_info = {0};
    int mport, idx;
    bcmdrd_sid_t profile_sid_list[] = {MMU_EBPCC_MMUQ_SCHQ_CFGr,
                                       MMU_MTRO_MMUQ_SCHQ_CFGr,
                                       MMU_QSCH_MMUQ_TO_SCHQ_MAPr,
                                       MMU_PORT_MMUQ_SCHQ_CFGr};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mport_get(unit, lport, &mport));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);

    /* Profile ID */
    fid = Q_PROFILE_SELf;
    profile_id = scheduler_cfg->profile_id;
    for (idx = 0; idx < COUNTOF(profile_sid_list); idx++) {
        sid = profile_sid_list[idx];
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &profile_id));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));
    }

    /* MMU_QSCH_PORT_CONFIGr */
    sid = MMU_QSCH_PORT_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));

    /* WRR mapping */
    fid = ENABLE_WRRf;
    fval = scheduler_cfg->wrr_enable;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* L1 to L0 mapping.*/
    fid = L1_TO_L0_MAPPINGf;
    if (profile) {
        bcm56996_a0_tm_l1_to_l0_mapping(unit, profile_id, profile, &bmap);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &bmap));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));

    /* MMU_MTRO_MMUQ_SCHQ_CFGr */
    sid = MMU_MTRO_MMUQ_SCHQ_CFGr;
    fid = L1_TO_L0_MAPf;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &bmap));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_tm_scheduler_profile_reset(int unit,
                                       bcmltd_sid_t ltid,
                                       uint8_t profile_id)
{
    bcmdrd_sid_t sid_list[] = { MMU_PPSCH_SCHQ_MMUQ_PROFILEr,
                                MMU_MTRO_MMUQ_SCHQ_PROFILEr,
                                MMU_EBPCC_MMUQ_SCHQ_PROFILEr };
    bcmdrd_sid_t sid;
    uint32_t idx;
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, profile_id, 0);

    for (idx = 0; idx < (sizeof(sid_list)/sizeof(bcmdrd_sid_t)); idx++) {
        sid = sid_list[idx];
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Scheduler profile PT configure.
 *
 * \param [in] unit  Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id Profile ID.
 * \param [in] profile Scheduler profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcm56996_a0_tm_scheduler_profile_set(int unit,
                                     bcmltd_sid_t ltid,
                                     uint8_t profile_id,
                                     bcmtm_scheduler_profile_t *profile)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    int ucq_offset = 0, mcq_offset, num_ucq, num_mcq, sched_node = 0, idx;
    uint32_t mmu_q_map[TH4G_TM_NUM_Q] = {0};
    uint32_t fval, ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};

    bcmdrd_fid_t sched_q_fid[] = { SCH_Q_NUM_0f, SCH_Q_NUM_1f, SCH_Q_NUM_2f,
                                   SCH_Q_NUM_3f, SCH_Q_NUM_4f, SCH_Q_NUM_5f,
                                   SCH_Q_NUM_6f, SCH_Q_NUM_7f, SCH_Q_NUM_8f,
                                   SCH_Q_NUM_9f, SCH_Q_NUM_10f, SCH_Q_NUM_11f};
    bcmdrd_fid_t mmu_q_fid[] = { MMU_Q_NUM_0f, MMU_Q_NUM_1f, MMU_Q_NUM_2f,
                                 MMU_Q_NUM_3f, MMU_Q_NUM_4f, MMU_Q_NUM_5f,
                                 MMU_Q_NUM_6f, MMU_Q_NUM_7f, MMU_Q_NUM_8f,
                                 MMU_Q_NUM_9f, MMU_Q_NUM_10f, MMU_Q_NUM_11f};

    SHR_FUNC_ENTER(unit);

    sid = MMU_PPSCH_SCHQ_MMUQ_PROFILEr;
    mcq_offset = bcm56996_a0_tm_num_uc_q_non_cpu_port_get(unit);

    if (!profile) {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tm_scheduler_profile_reset(unit, ltid, profile_id));
        SHR_EXIT();
    }
    for (idx = 0; idx < TH4G_TM_NUM_Q; idx++) {
        num_ucq = profile[idx].num_ucq;
        num_mcq = profile[idx].num_mcq;
        while (num_ucq) {
            fval = ucq_offset;
            fid = mmu_q_fid[sched_node];
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
            num_ucq--;
            mmu_q_map[ucq_offset] = sched_node;
            ucq_offset++;
            sched_node++;
        }
        while (num_mcq) {
            fval = mcq_offset;
            fid = mmu_q_fid[sched_node];
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
            num_mcq--;
            mmu_q_map[mcq_offset] = sched_node;
            mcq_offset++;
            sched_node++;
        }
    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, profile_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));


    sal_memset(ltmbuf, 0, sizeof(uint32_t) * BCMTM_MAX_ENTRY_WSIZE);
    sid = MMU_MTRO_MMUQ_SCHQ_PROFILEr;
    for (idx = 0; idx < TH4G_TM_NUM_Q; idx++) {
        fid = sched_q_fid[idx];
        fval = mmu_q_map[idx];
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));

    sid = MMU_EBPCC_MMUQ_SCHQ_PROFILEr;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Scheduler cpu node physical table configure.
 *
 * \param [in] unit  Unit number.
 * \param [in] sched_node_id Scheduler node ID.
 * \param [in] sched_level Scheduler level.
 * \param [in] ltid Logical table ID.
 * \param [in] scheduler_cfg Scheduler configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcm56996_a0_tm_scheduler_cpu_node_set(int unit,
        uint32_t sched_node_id,
        uint32_t sched_level,
        bcmltd_sid_t ltid,
        bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf_arr[2] = {0}, ltmbuf = 0;
    uint32_t fval_arr[2] = {0}, fval = 0;
    int index;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);

    if ((sched_level == L0_SCHED_COS) && (sched_node_id >= TH4G_NUM_L0_NODE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sid = MMU_QSCH_CPU_PORT_CONFIGr;

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid,(void *)&pt_dyn_info,
                               ltmbuf_arr));
    if (sched_level == L0_SCHED_COS) {
        /* L0 Level */
        fid = SP_L0f;
    } else {
        /* L1 level */
        fid = SP_L1f;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf_arr, fval_arr));
    /* Strict Priority */
    if (scheduler_cfg->sched_mode == 0)
        SHR_BITSET(fval_arr, sched_node_id);
    /* Round Robin */
    else
        SHR_BITCLR(fval_arr, sched_node_id);

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf_arr, fval_arr));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info,
                                ltmbuf_arr));

    /* Parent node */
    if (sched_level == L1_SCHED_MULTICAST_QUEUE) { /* L1 node level */
        fid = SELECTf;
        BCMTM_PT_DYN_INFO(pt_dyn_info, sched_node_id, 0);

        sid = MMU_QSCH_CPU_L1_TO_L0_MAPPINGr;
        fval = scheduler_cfg->parent_node;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info,
                                    &ltmbuf));

        sid = MMU_MTRO_CPU_L1_TO_L0_MAPPINGr;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info,
                                    &ltmbuf));

    }
    /* Scheduler Weight */
    /* L0 level */
    ltmbuf = 0;
    if (sched_level == L0_SCHED_COS) {
        sid = MMU_QSCH_L0_WEIGHT_MEMm;
    } else {
        sid = MMU_QSCH_L1_WEIGHT_MEMm;
    }
    fid = WEIGHTf;
    fval = scheduler_cfg->weight;
    index = TH4G_TM_CPU_Q_OFFSET + sched_node_id;
    BCMTM_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
void
bcm56996_a0_tm_sched_node_mmu_q_get(int unit,
                                    int sched_node,
                                    bcmtm_scheduler_profile_t *profile,
                                    int *mmu_q_num)
{
    int idx;
    int num_ucq, num_mcq;
    int ucq_id = 0, mcq_id = 0, mcq_offset;

    mmu_q_num[0] = -1;
    mmu_q_num[1] = -1;

    mcq_offset = bcm56996_a0_tm_num_uc_q_non_cpu_port_get(unit);
    for (idx = 0; idx < sched_node; idx++) {
        num_ucq = profile[idx].num_ucq;
        num_mcq = profile[idx].num_mcq;
        ucq_id += num_ucq;
        mcq_id += num_mcq;
    }

    num_ucq = profile[sched_node].num_ucq;
    num_mcq = profile[sched_node].num_mcq;
    idx = 0;
    while (num_ucq) {
        mmu_q_num[idx] = ucq_id;
        num_ucq--;
        ucq_id++;
        idx++;
    }
    if (num_mcq) {
        mmu_q_num[idx] = mcq_offset + mcq_id;
    }
}

int
bcm56996_a0_tm_mmu_q_sched_node_id_get(int unit,
                                       int mmu_q_id,
                                       bcmtm_scheduler_profile_t *profile)
{
    int num_ucq, num_mcq, idx;
    int ucq_id = 0, mcq_id = 0;
    int mcq_offset = 0;

    mcq_offset = bcm56996_a0_tm_num_uc_q_non_cpu_port_get(unit);

    for (idx = 0; idx < TH4G_TM_NUM_Q; idx++){
        num_ucq = profile[idx].num_ucq;
        num_mcq = profile[idx].num_mcq;
        while (num_ucq) {
            if (ucq_id == mmu_q_id) {
                return idx;
            }
            ucq_id++;
            num_ucq--;
        }
        if (num_mcq) {
            if ((mcq_id + mcq_offset) == mmu_q_id) {
                return idx;
            }
            mcq_id++;
        }
    }
    return -1;
}

int
bcm56996_a0_tm_sched_node_l0_map_get(int unit,
                                     uint8_t profile_id,
                                     uint32_t sched_node,
                                     int *l0_node)
{
    bcm56996_a0_tm_cos_l0_map_t *l0_map;
    SHR_FUNC_ENTER(unit);

    l0_map = &(bcm56996_a0_tm_cos_l0_map[unit][profile_id]);
    *l0_node = l0_map->map[sched_node];
    if (*l0_node == -1) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_scheduler_profile_init(int unit,
                                      uint8_t profile_id,
                                      bcmtm_scheduler_profile_t *profile)
{
    int idx, opcode;
    bcm56996_a0_tm_cos_l0_map_t *l0_map;
    SHR_FUNC_ENTER(unit);

    if (!profile) {
        for (idx = 0; idx < TH4G_NUM_SCHED_PROFILE; idx++) {
            l0_map = &(bcm56996_a0_tm_cos_l0_map[unit][idx]);
            sal_memset(l0_map->map, -1, sizeof(bcm56996_a0_tm_cos_l0_map_t));
        }
    } else {
        l0_map = &(bcm56996_a0_tm_cos_l0_map[unit][profile_id]);
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tm_cos_l0_map_set(unit, profile_id, profile,
                                           &opcode));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_scheduler_drv_get(int unit, void *sched_drv)
{
    bcmtm_scheduler_drv_t bcm56996_a0_sched_drv = {
        .node_set = bcm56996_a0_tm_scheduler_node_set,
        .port_set = bcm56996_a0_tm_scheduler_port_set,
        .node_profile_validate = bcm56996_a0_tm_scheduler_node_profile_validate,
        .profile_validate = bcm56996_a0_tm_scheduler_profile_validate,
        .cpu_node_set = bcm56996_a0_tm_scheduler_cpu_node_set,
        .profile_set = bcm56996_a0_tm_scheduler_profile_set,
        .profile_init = bcm56996_a0_tm_scheduler_profile_init
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(sched_drv, SHR_E_INTERNAL);
    *((bcmtm_scheduler_drv_t *)sched_drv) = bcm56996_a0_sched_drv;
exit:
    SHR_FUNC_EXIT();
}
