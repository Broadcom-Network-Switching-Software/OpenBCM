/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * MMU/Cosq soc routines
 */

#include <sal/core/libc.h>

#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>
#include <soc/counter.h>
#include <shared/bsl.h>
#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#endif
#include <soc/mmu_config.h>
#if defined(BCM_TOMAHAWK_SUPPORT)

#define TH_MMU_PER_PIPE_PORT_OFFSET 64
#define TH_MMU_GP_UCQ_NUM           320 /* Generl ports UC Queue Num in per Pipe Queue Number Space */
#define TH_MMU_MGMT_UCQ_NUM         10  /* Management port UC Queues Num in Pipe Queue Number */
#define TH_MMU_GP_MCQ_NUM           320 /* Generl ports MC Queue Num in per Pipe Queue Number Space */
#define TH_MMU_LB_MCQ_NUM           10  /* Loopback port MC Queue Num in per Pipe Queue Number Space */
#define TH_MMU_LB_MCQ_OFFSET       ((TH_MMU_GP_UCQ_NUM) + (TH_MMU_MGMT_UCQ_NUM) + \
                                     (TH_MMU_GP_MCQ_NUM))
#define TH_MMU_CPU_MCQ_OFFSET      (TH_MMU_LB_MCQ_OFFSET + TH_MMU_LB_MCQ_NUM)
#define TH_MMU_MGMT_UCQ_OFFSET     (TH_MMU_GP_UCQ_NUM)
#define TH_MMU_MGMT_MCQ_OFFSET     (TH_MMU_LB_MCQ_OFFSET + TH_MMU_LB_MCQ_NUM)

int
soc_th_sched_weight_set(int unit, int port, int level,
                        int index, int weight, int mc)
{
    soc_info_t *si;
    soc_mem_t mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int phy_port, mmu_port, pipe;

    si = &SOC_INFO(unit);

    pipe = si->port_pipe[port];
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if (level == SOC_TH_NODE_LVL_L0) {
        mem = Q_SCHED_L0_WEIGHT_MEMm;
        index = ((mmu_port % TH_MMU_PER_PIPE_PORT_OFFSET)  * 10) + index
                + (mc ? SOC_TH_NUM_UC_QUEUES_PER_PIPE : 0);
    } else if (level == SOC_TH_NODE_LVL_L1) {
        mem = Q_SCHED_L1_WEIGHT_MEMm;

        if (IS_CPU_PORT(unit, port)) {
            index = TH_MMU_CPU_MCQ_OFFSET + index;
        } else if (IS_LB_PORT(unit, port)) {
            index = TH_MMU_LB_MCQ_OFFSET + index;
        } else if (SOC_PBMP_MEMBER(si->management_pbm, port)) {
            index = (mc ? TH_MMU_MGMT_MCQ_OFFSET : TH_MMU_MGMT_UCQ_OFFSET) + index;
        } else {
            index = ((mmu_port % TH_MMU_PER_PIPE_PORT_OFFSET)  * 10) + index
                    + (mc ? SOC_TH_NUM_UC_QUEUES_PER_PIPE : 0);
        }
    } else {
        return SOC_E_PARAM;
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    soc_mem_field32_set(unit, mem, &entry, WEIGHTf, weight);
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));

    return SOC_E_NONE;
}

int
soc_th_sched_weight_get(int unit, int port, int level,
                        int index, int *weight, int mc)
{
    soc_info_t *si;
    soc_mem_t mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int phy_port, mmu_port, pipe;

    si = &SOC_INFO(unit);

    pipe = si->port_pipe[port];
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if (level == SOC_TH_NODE_LVL_L0) {
        mem = Q_SCHED_L0_WEIGHT_MEMm;
        index = ((mmu_port % TH_MMU_PER_PIPE_PORT_OFFSET)  * 10) + index
                + (mc ? SOC_TH_NUM_UC_QUEUES_PER_PIPE : 0);
    } else if (level == SOC_TH_NODE_LVL_L1) {
        mem = Q_SCHED_L1_WEIGHT_MEMm;

        if (IS_CPU_PORT(unit, port)) {
            index = TH_MMU_CPU_MCQ_OFFSET + index;
        } else if (IS_LB_PORT(unit, port)) {
            index = TH_MMU_LB_MCQ_OFFSET + index;
        } else if (SOC_PBMP_MEMBER(si->management_pbm, port)) {
            index = (mc ? TH_MMU_MGMT_MCQ_OFFSET : TH_MMU_MGMT_UCQ_OFFSET) + index;
        } else {
            index = ((mmu_port % TH_MMU_PER_PIPE_PORT_OFFSET)  * 10) + index
                    + (mc ? SOC_TH_NUM_UC_QUEUES_PER_PIPE : 0);
        }
    } else {
        return SOC_E_PARAM;
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    *weight = soc_mem_field32_get(unit, mem, &entry, WEIGHTf);

    return SOC_E_NONE;
}

int
soc_th_cosq_sched_mode_set(int unit, soc_port_t port, int level, int index,
                           soc_th_sched_mode_e mode, int weight, int mc)
{
    soc_reg_t reg, reg2 = INVALIDr, reg3 = INVALIDr;
    uint32 fval = 0, rval, wrr_mask, sp_mask;
    soc_info_t *si = &SOC_INFO(unit);
    int child_idx = index;

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d MODE=%d WT=%d\n"),
			 port, (level == 0) ? "r" : "", level - 1,
             index, mode, weight));

    /* selection between SP and WxRR is based on weight property. */
    SOC_IF_ERROR_RETURN(soc_th_sched_weight_set(unit, port,
                                                level, index, weight, mc));

    if (level == SOC_TH_NODE_LVL_L0) {
        if (IS_CPU_PORT(unit, port)) {
            reg = Q_SCHED_CPU_PORT_CONFIGr;
            reg2 = Q_SCHED_PORT_CONFIGr;
        } else {
            reg = Q_SCHED_PORT_CONFIGr;
            reg3 = Q_SCHED_L0_NODE_CONFIGr;
        }
        index = 0;
    } else if (level == SOC_TH_NODE_LVL_L1) {
        if (IS_CPU_PORT(unit, port)) {
            int parent_index = 0;
            reg = Q_SCHED_CPU_L0_NODE_CONFIGr;
            reg2 = Q_SCHED_L0_NODE_CONFIGr;
            SOC_IF_ERROR_RETURN
                (soc_th_cosq_cpu_parent_get(unit, index, SOC_TH_NODE_LVL_L1,
                                            &parent_index));
            index = parent_index;
        } else {
            reg = Q_SCHED_L0_NODE_CONFIGr;
            index %= si->port_num_cosq[port];
            if (mc) {
                reg3 = Q_SCHED_L1_MC_QUEUE_CONFIGr;
            } else {
                reg3 = Q_SCHED_L1_UC_QUEUE_CONFIGr;
            }
        }
    } else {
        return SOC_E_PARAM;
    }

    if (mode == SOC_TH_SCHED_MODE_WRR) {
        fval = 1;
    } else if (mode == SOC_TH_SCHED_MODE_WERR) {
        fval = 0;
    }

    if (mode != SOC_TH_SCHED_MODE_STRICT) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
        wrr_mask = soc_reg_field_get(unit, reg, rval, ENABLE_WRRf);
        wrr_mask &= ~(1 << index);
        wrr_mask |= (fval << index);
        soc_reg_field_set(unit, reg, &rval, ENABLE_WRRf, wrr_mask);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));

        /* WAR: set port 32 besides CPU reg,
           as CPU WRED takes some info from port 32. */
        if (reg2 != INVALIDr) {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg2, port, 0, &rval));
            wrr_mask = soc_reg_field_get(unit, reg2, rval, ENABLE_WRRf);
            wrr_mask &= ~(1 << index);
            wrr_mask |= (fval << index);
            soc_reg_field_set(unit, reg2, &rval, ENABLE_WRRf, wrr_mask);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg2, port, 0, rval));
        }
    }

    if (reg3 != INVALIDr) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg3, port, 0, &rval));
        sp_mask = soc_reg_field_get(unit, reg3, rval, ENABLE_SP_IN_MINf);
        if (mode == SOC_TH_SCHED_MODE_STRICT) {
            sp_mask |= (1 << child_idx);
        } else {
            sp_mask &= ~(1 << child_idx);
        }
        soc_reg_field_set(unit, reg3, &rval, ENABLE_SP_IN_MINf, sp_mask);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg3, port, 0, rval));
    }

    return SOC_E_NONE;
}

int
soc_th_cosq_sched_mode_get(int unit, soc_port_t port, int level, int index,
                           soc_th_sched_mode_e *mode, int *weight, int mc)
{
    soc_reg_t reg;
    uint32 rval, wrr_mask;
    soc_info_t *si = &SOC_INFO(unit);

    /* selection between SP and WxRR is based on weight property. */
    SOC_IF_ERROR_RETURN(soc_th_sched_weight_get(unit, port,
                                                level, index, weight, mc));

    if (level == SOC_TH_NODE_LVL_L0) {
        if (IS_CPU_PORT(unit, port)) {
            reg = Q_SCHED_CPU_PORT_CONFIGr;
        } else {
            reg = Q_SCHED_PORT_CONFIGr;
        }
        index = 0;
    } else if (level == SOC_TH_NODE_LVL_L1) {
        if (IS_CPU_PORT(unit, port)) {
            int parent_index = 0;
            reg = Q_SCHED_CPU_L0_NODE_CONFIGr;
            SOC_IF_ERROR_RETURN
                (soc_th_cosq_cpu_parent_get(unit, index, SOC_TH_NODE_LVL_L1,
                                            &parent_index));
            index = parent_index;
        } else {
            reg = Q_SCHED_L0_NODE_CONFIGr;
            index %= si->port_num_cosq[port];
        }
    } else {
        return SOC_E_PARAM;
    }

    if (*weight == 0) {
        *mode = SOC_TH_SCHED_MODE_STRICT;
    } else {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
        wrr_mask = soc_reg_field_get(unit, reg, rval, ENABLE_WRRf);
        if (wrr_mask & (1 << index)) {
            *mode = SOC_TH_SCHED_MODE_WRR;
        } else {
            *mode = SOC_TH_SCHED_MODE_WERR;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function to Set Parent(L0) for a given L1 queue of the CPU port
 * Note: Only child @ L1 level is supported.
 * Because only L1 level queues can be attached to different parent (L0.0-L0.9)
 * of the CPU port.
 */
int
soc_th_cosq_cpu_parent_set(int unit, int child_index, int child_level,
                           int parent_index)
{
    soc_reg_t reg = Q_SCHED_CPU_L1_MC_QUEUE_L0_MAPPINGr;
    uint32 rval = 0;

    if (child_level != SOC_TH_NODE_LVL_L1) {
        /* Only child at L1 are allowed to move */
        return SOC_E_PARAM;
    }
    /* Set the L1 queue's parent to the given L0 node */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, 0, child_index, &rval));
    soc_reg_field_set(unit, reg, &rval, SELECTf, parent_index);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, 0, child_index, rval));

    return SOC_E_NONE;
}

/*
 * Function to Get Parent(L0) for a given L1 queue of the CPU port
 * Note: Only child @ L1 level is supported.
 * Because only L1 level queues can be attached to different parent (L0.0-L0.9)
 * of the CPU port.
 */
int
soc_th_cosq_cpu_parent_get(int unit, int child_index, int child_level,
                           int *parent_index)
{
    soc_reg_t reg = Q_SCHED_CPU_L1_MC_QUEUE_L0_MAPPINGr;
    uint32 rval = 0;

    if (child_level != SOC_TH_NODE_LVL_L1) {
        /* Only child at L1 are allowed to move */
        return SOC_E_PARAM;
    }
    /* Set the L1 queue's parent to the given L0 node */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, 0, child_index, &rval));
    *parent_index = soc_reg_field_get(unit, reg, rval, SELECTf);

    return SOC_E_NONE;
}

int
soc_th_mmu_get_shared_size(int unit, int *thdi_shared,
                               int *thdo_db_shared, int *thdo_qe_shared)
{
    int xpe, xpe_map;
    int granularity;
    uint32 rval = 0;

    xpe_map = SOC_INFO(unit).ipipe_xpe_map[0]|SOC_INFO(unit).ipipe_xpe_map[1];

    for (xpe = 0; xpe < NUM_XPE(unit); xpe ++) {
        /* unavailable XPEs */
        if (!(xpe_map & (1 << xpe))) {
            thdi_shared[xpe] = thdo_db_shared[xpe] = thdo_qe_shared[xpe] = -1;
            continue;
        }

        /* available XPEs */
        /* Read shared size from Service pool 0 */
        /* THDI */
        rval = 0;
        granularity = 1;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_xpe_reg32_get(unit, THDI_BUFFER_CELL_LIMIT_SPr,
                xpe, -1, 0, &rval));
        thdi_shared[xpe] = soc_reg_field_get(unit, THDI_BUFFER_CELL_LIMIT_SPr,
                                        rval, LIMITf) * granularity;
        /* THDO - DB */
        rval = 0;
        granularity = 1;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_xpe_reg32_get(unit, MMU_THDM_DB_POOL_SHARED_LIMITr,
                xpe, -1, 0, &rval));
        thdo_db_shared[xpe] = soc_reg_field_get(unit, MMU_THDM_DB_POOL_SHARED_LIMITr,
                                        rval, SHARED_LIMITf) * granularity;
        
        /* THDO - QE */
        rval = 0;
        granularity = 4;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_xpe_reg32_get(unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr,
                xpe, -1, 0, &rval));
        thdo_qe_shared[xpe] = soc_reg_field_get(unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr,
                                        rval, SHARED_LIMITf) * granularity;
        
        LOG_VERBOSE(BSL_LS_SOC_MMU,
                    (BSL_META_U(unit,
                                "XPE[%d] Shared cell allocation: ING: %d, "
                                "EGR: DB %d, QE %d (unit of cells)\n"), xpe, 
                                *thdi_shared, *thdo_db_shared, *thdo_qe_shared));
    }
    
    return SOC_E_NONE;
}

int soc_th_mmu_config_res_limits_update(int unit, int res, int *ing_shd,
                                        int *egr_db_shd, int *egr_qe_shd,
                                        int post_update)
{
    if (SOC_IS_TOMAHAWK(unit)) {
        return soc_th_mmu_config_shared_buf_recalc(unit, res, ing_shd,
                                                   egr_db_shd, egr_qe_shd,
                                                   post_update);
    }
#if defined(BCM_TOMAHAWK2_SUPPORT)
    else if (SOC_IS_TOMAHAWK2(unit)) {
        return soc_th2_mmu_config_shared_buf_set_hw(unit, res, ing_shd,
                                                   egr_db_shd, egr_qe_shd,
                                                   post_update);
    }
#endif
    return SOC_E_INTERNAL;
}

/* 
 * Function to calculate reserved memory in Egress buffer
 */
int
soc_th_mmu_get_egr_rsvd (int unit, int pipe, int *egr_rsvd)
{
    int port, phy_port, mmu_port, new_pipe, local_mmu_port;
    int startq, to_cos, cos, count;
    int qg_valid, use_qg_min;
    soc_mem_t mem1 = INVALIDm, base_mem1 = INVALIDm;
    soc_mem_t mem2 = INVALIDm, base_mem2 = INVALIDm;
    soc_mem_t mem3 = INVALIDm, base_mem3 = INVALIDm;
    soc_mem_t mem4 = INVALIDm, base_mem4 = INVALIDm;
    mmu_thdo_q_to_qgrp_map_entry_t entry1;
    mmu_thdu_config_qgroup_entry_t entry2;
    mmu_thdu_config_queue_entry_t entry3;
    mmu_thdm_db_queue_config_entry_t entry4;
    soc_info_t *si= &SOC_INFO(unit);
    soc_field_t qg_min_field = USE_QGROUP_MINf;
    *egr_rsvd = 0;
    if (pipe >= NUM_PIPE(unit)) {
        return SOC_E_PARAM;
    }
    base_mem1 = MMU_THDU_Q_TO_QGRP_MAPm;
    mem1 = SOC_MEM_UNIQUE_ACC(unit, base_mem1)[pipe];
    base_mem2 = MMU_THDU_CONFIG_QGROUPm;
    mem2 = SOC_MEM_UNIQUE_ACC(unit, base_mem2)[pipe];
    base_mem3 = MMU_THDU_CONFIG_QUEUEm;
    mem3 = SOC_MEM_UNIQUE_ACC(unit, base_mem3)[pipe];
    base_mem4 = MMU_THDM_DB_QUEUE_CONFIGm;
    mem4 = SOC_MEM_UNIQUE_ACC(unit, base_mem4)[pipe];
    /*Loop for UC queue/qgroup Mins, excluding CPU and LB ports*/
    PBMP_ALL_ITER(unit, port) {
        if (!(IS_CPU_PORT(unit, port)) && !(IS_LB_PORT(unit, port))) {
            new_pipe = si->port_pipe[port];
            if (pipe == new_pipe) {
                /*Calculate qmin+qgroup_min for each port in the pipe*/
                phy_port = si->port_l2p_mapping[port];
                mmu_port = si->port_p2m_mapping[phy_port];
                local_mmu_port = mmu_port & (SOC_TH_MMU_PORT_STRIDE - 1);
                startq = si->port_uc_cosq_base[port];
                count = si->port_num_cosq[port];
                to_cos = startq + count;
                /*Loop for qgroup min; add if atleast one queue uses
                 *qgroup min and break
                 */
                for (cos = startq; cos < to_cos; cos++) {
                    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem1,
                                MEM_BLOCK_ALL, cos, &entry1));
                    qg_valid = soc_mem_field32_get(unit, mem1, &entry1,
                            QGROUP_VALIDf);
                    use_qg_min = soc_mem_field32_get(unit, mem1, &entry1,
                            qg_min_field);
                    if ((qg_valid == 1) && (use_qg_min ==1)) {
                        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem2, MEM_BLOCK_ALL,
                                    local_mmu_port, &entry2));
                        *egr_rsvd += soc_mem_field32_get(unit, mem2, &entry2,
                                Q_MIN_LIMIT_CELLf);
                        break;
                    }
                }
                /*loop for qmin; check all queues and add queue mins*/
                for (cos = startq; cos < to_cos; cos++) {
                    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem1, MEM_BLOCK_ALL,
                                cos, &entry1));
                    qg_valid = soc_mem_field32_get(unit, mem1, &entry1,
                            QGROUP_VALIDf);
                    use_qg_min = soc_mem_field32_get(unit, mem1, &entry1,
                            qg_min_field);
                    if ((qg_valid == 0) || (use_qg_min ==0)) {
                        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem3,
                                    MEM_BLOCK_ALL,cos, &entry3));
                        *egr_rsvd += soc_mem_field32_get(unit, mem3,
                                &entry3, Q_MIN_LIMIT_CELLf);
                    }
                }
            }
        }
    }
    /*Loop for MC queue Mins, including CPU and LB ports*/
    PBMP_ALL_ITER(unit, port) {
        new_pipe = si->port_pipe[port];
        if (pipe == new_pipe) {
            /*Calculate qmin+qgroup_min for each port in the pipe*/
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
            local_mmu_port = mmu_port & (SOC_TH_MMU_PORT_STRIDE - 1);
            startq = si->port_cosq_base[port];
            count = si->port_num_cosq[port];
            to_cos = startq + count;
            /*loop for MC qmin; check all queues and add queue mins*/
            for (cos = startq; cos < to_cos; cos++) {
                SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem4, MEM_BLOCK_ALL,
                                cos, &entry4));
                *egr_rsvd += soc_mem_field32_get(unit, mem4, &entry4,
                            Q_MIN_LIMITf);
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function to calculate the new value of egress shared limit
 */
int
soc_th_recalc_new_egress_shared_limit (int unit, int pipe,
        int *new_shared_limit)
{
    soc_info_t *si;
    int  lossless, asf_profile;
    _soc_mmu_rsvd_buffer_t rsvd_buffer;
    int asf_rsvd1 = 0, asf_rsvd2 = 0;
    int egr_rsvd1 = 0, egr_rsvd2 = 0 ;
    int xpe, pipe2 = 0, count, temp_val;
    uint32 pipe_map;
    si = &SOC_INFO(unit);
    *new_shared_limit = 0;
   /*Getting the second egress pipe which is part of same xpe as this pipe*/
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        pipe_map = si->xpe_epipe_map[xpe];
        if (pipe_map & (1 << pipe)) {
            count = 0;
            while(pipe_map) {
                temp_val = (pipe_map >> 1);
                if ((temp_val != 0) && (count != pipe)) {
                    pipe2=count;
                    break;
                }
                count++;
            }
        }
    }

    if (SOC_IS_TOMAHAWKPLUS(unit)) {
        lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 
                                    SOC_TH_MMU_LOSSLESS_DEFAULT_DISABLE);
    } else {
        lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 
                                    SOC_TH_MMU_LOSSLESS_DEFAULT_ENABLE);
    }

    asf_profile = soc_property_get(unit, spn_ASF_MEM_PROFILE,
                                   _SOC_TH_ASF_MEM_PROFILE_SIMILAR);
    soc_th_mmu_additional_buffer_reserve(unit, pipe, si->flex_eligible,
                                                lossless, asf_profile,
                                                 &rsvd_buffer);
    asf_rsvd1 = rsvd_buffer.asf_rsvd_cells;
    soc_th_mmu_additional_buffer_reserve(unit, pipe2, si->flex_eligible,
                                                lossless, asf_profile,
                                                 &rsvd_buffer);
    asf_rsvd2 = rsvd_buffer.asf_rsvd_cells;
    soc_th_mmu_get_egr_rsvd (unit, pipe, &egr_rsvd1);
    soc_th_mmu_get_egr_rsvd (unit, pipe2, &egr_rsvd2);
    *new_shared_limit = _TH_MMU_TOTAL_CELLS_PER_XPE - asf_rsvd1 -
        egr_rsvd1 -egr_rsvd2 - asf_rsvd2 - (_TH_MMU_NUM_RQE_QUEUES * 8);
    LOG_INFO(BSL_LS_SOC_MMU,
    (BSL_META_U(unit,
    "MMU buffer recal:  asf_rsvd1: %d egr_rsvd1: %d  new_shared_limit: %d\n"),
               asf_rsvd1, egr_rsvd1,  *new_shared_limit));
    return SOC_E_NONE;
}

int soc_th_cosq_threshold_dynamic_change_check(
    int unit,
    int new_threshold,
    soc_reg_t ctr_reg,
    soc_ctr_control_info_t ctrl_info,
    int ar_idx,
    uint8 sync_mode,
    uint8 *allowed)
{
    uint32 flags = sync_mode ? SOC_COUNTER_SYNC_ENABLE : 0;
    uint64 val64s[_TH_XPES_PER_DEV];
    soc_timeout_t timeout;
    int index;
    int check_pass = TRUE;

    /* Not check for threshold <= 0 case */
    if (new_threshold <= 0) {
        *allowed = TRUE;
        return SOC_E_NONE;
    }

    *allowed = FALSE;
    soc_timeout_init(&timeout, SOC_MMU_THRESHOLD_DYNAMIC_CHECK_TIMEOUT, 1);
    do {
        check_pass = TRUE;

        /* Each XPE maps to a pair of IPIPE or EPIPE based on memory type. */
        /* Retrieve the use count on all the valid XPEs. */
        sal_memset(&val64s, 0, sizeof(val64s));
        SOC_IF_ERROR_RETURN
            (soc_counter_get_per_buffer(unit, ctr_reg, ctrl_info,
                                                  flags, ar_idx, val64s));

        for (index = 0; index < _TH_XPES_PER_DEV; index++) {
            /* Use count of any XPE greater or equal than new threshold. */
            if (COMPILER_64_HI(val64s[index])
                || COMPILER_64_LO(val64s[index]) >= new_threshold) {
                check_pass = FALSE;
                break;
            }
        }

        if (check_pass == TRUE) {
            *allowed = TRUE;
            return SOC_E_NONE;
        }

        if (soc_timeout_check(&timeout)) {
            LOG_ERROR(BSL_LS_SOC_COSQ,
                      (BSL_META_U(unit,
                                  "ERROR: Timeout during threshold dynamic check!\n")));
            return SOC_E_BUSY;
        }
    } while (TRUE);

}

#endif /* BCM_TOMAHAWK_SUPPORT */
