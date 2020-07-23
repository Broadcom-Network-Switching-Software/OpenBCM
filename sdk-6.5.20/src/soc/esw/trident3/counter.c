/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * TD3 counter module routines.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/trident3.h>

#ifdef BCM_TRIDENT3_SUPPORT

int
soc_counter_trident3_get_child_dma_by_idx(int unit,
    soc_counter_non_dma_t *parent,
    int idx, soc_counter_non_dma_t **child)
{
    int rv = SOC_E_NONE;

    if ((parent->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        if (idx >= parent->extra_ctr_ct) {
            return SOC_E_PARAM;
        }
        *child = &parent->extra_ctrs[idx << 1];
    } else {
        *child = parent;
    }
    return rv;
}

int
soc_counter_trident3_get_child_dma(int unit, soc_reg_t id,
                                   soc_ctr_control_info_t ctrl_info,
                                   soc_counter_non_dma_t **child_dma)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_counter_non_dma_t *parent_dma = NULL;
    int c_offset = -1;

    if ((id < NUM_SOC_REG) || (id >= SOC_COUNTER_NON_DMA_END)) {
        return SOC_E_PARAM;
    }
    parent_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if ((!(parent_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) || 
        (ctrl_info.instance == -1)) {
        *child_dma = parent_dma;
        return SOC_E_NONE;
    }

    switch (ctrl_info.instance_type) {
        case SOC_CTR_INSTANCE_TYPE_POOL:
            c_offset = ctrl_info.instance;
            if ((c_offset < 0) || (c_offset >= parent_dma->extra_ctr_ct)) {
                return SOC_E_PARAM;
            }

            switch (id) {
                case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE: 
                case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
                case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
                case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:
                    if ((parent_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                        (parent_dma->extra_ctrs != NULL)) {
                        /* Refer extra_ctrs_init for PKT/BYTE control block placement.
                        */
                        *child_dma = &parent_dma->extra_ctrs[(c_offset << 1)];
                    }
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;
        default:
            return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

int
soc_counter_trident3_generic_get_info(int unit, soc_ctr_control_info_t
                                      ctrl_info, soc_reg_t id, int* base_index,
                                      int* num_entries) {
    soc_info_t *si = &SOC_INFO(unit);
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_counter_non_dma_t *non_dma;
    soc_port_t port;
    soc_port_t phy_port = 0, mmu_port = 0;
    int pipe, c_offset, pipe_offset = 0;

    if ((id < NUM_SOC_REG) || (id >= SOC_COUNTER_NON_DMA_END)) {
        return SOC_E_PARAM;
    }

    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID) &&
            !(non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* For Parent-Child model, return E_UNAVAIL if child non_dma is INVALID.
         */
        return SOC_E_UNAVAIL;
    }

    *base_index = 0;
    switch (ctrl_info.instance_type) {
        case SOC_CTR_INSTANCE_TYPE_PORT:
            port = ctrl_info.instance;
            if (si->port_l2p_mapping[port] == -1) {
                *base_index = 0;
                *num_entries = 0;
                return SOC_E_PARAM;
            }
            pipe = si->port_pipe[port];

            switch (id) {
            case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
            case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
                if (IS_CPU_PORT(unit, port)) {
                    *base_index = 0;
                } else {
                    *base_index = pipe * 1368 + 48 + (port % 66) * 20 + 10;
                }
                *num_entries = si->port_num_cosq[port];
                break;
            case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC:
            case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC:
                if (IS_CPU_PORT(unit, port)) {
                    *base_index = 0;
                } else {
                    *base_index = pipe * 1368 + 48 + (port % 66) * 20;
                }
                *num_entries = si->port_num_cosq[port];
                break;
            case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO:
            case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO:
            case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI:
            case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI:
            case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0:
            case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0:
            case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1:
            case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1:
            case SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS:
                if (IS_CPU_PORT(unit, port)) {
                    return SOC_E_PARAM;
                }
                return soc_counter_trident3_get_info(unit, port, id,
                                                     base_index,
                                                     num_entries);
                break;
                default:
                    return SOC_E_PARAM;
            }
            break;
        case SOC_CTR_INSTANCE_TYPE_XPE_PORT:
            port =  (ctrl_info.instance & 0xFFC000) >> 14;
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
            pipe = si->port_pipe[port];

            switch (id) {
                case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
                case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
                case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
                case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
                    if ((id == SOC_COUNTER_NON_DMA_COSQ_DROP_PKT) ||
                        (id == SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE)) {
                        *base_index = si->port_cosq_base[port];
                        *num_entries = si->port_num_cosq[port];
                    } else {
                        *base_index = si->port_uc_cosq_base[port];
                        *num_entries = si->port_num_uc_cosq[port];
                    }
                    pipe_offset =  (non_dma->num_entries / NUM_PIPE(unit)) * pipe;
                    *base_index += pipe_offset;
                    break;

                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
                case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
                case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
                case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
                case SOC_COUNTER_NON_DMA_QGROUP_MIN_CURRENT:
                    if (IS_CPU_PORT(unit, port)) {
                        *base_index = 64;
                    } else if (IS_LB_PORT(unit, port)) {
                        *base_index = pipe * 66 + 65;
                    } else {
                        *base_index = pipe * 66 +
                                      (mmu_port & SOC_TD3_MMU_PORT_STRIDE);
                    }
                    *num_entries = 1;
                    break;

                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
                case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
                    *base_index = mmu_port & SOC_TD3_MMU_PORT_STRIDE;
                    *num_entries = 1;
                    pipe_offset =  (non_dma->num_entries / NUM_PIPE(unit)) * pipe;
                    *base_index += pipe_offset;
                    break;
                case SOC_COUNTER_NON_DMA_PG_MIN_PEAK:
                case SOC_COUNTER_NON_DMA_PG_MIN_CURRENT:
                case SOC_COUNTER_NON_DMA_PG_SHARED_PEAK:
                case SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT:
                case SOC_COUNTER_NON_DMA_PG_HDRM_PEAK:
                case SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT:
                    *num_entries = non_dma->entries_per_port;
                    pipe_offset = (non_dma->num_entries / NUM_PIPE(unit)) * pipe;

                    if (IS_CPU_PORT(unit, port)) {
                        *base_index += mmu_port * _TD3_MMU_NUM_PG;
                    } else {
                        *base_index += pipe_offset +
                                        ((mmu_port & SOC_TD3_MMU_PORT_STRIDE) *
                                         _TD3_MMU_NUM_PG);
                    }
                    break;
                case SOC_COUNTER_NON_DMA_QUEUE_PEAK:
                case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
                case SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK:
                case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
                case SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT:
                    if ((id == SOC_COUNTER_NON_DMA_QUEUE_PEAK) ||
                            (id == SOC_COUNTER_NON_DMA_QUEUE_CURRENT)) {
                        *base_index = si->port_cosq_base[port];
                        *num_entries = si->port_num_cosq[port];
                    } else {
                        *base_index = si->port_uc_cosq_base[port];
                        *num_entries = si->port_num_uc_cosq[port];
                    }
                    pipe_offset =  (non_dma->num_entries / NUM_PIPE(unit)) * pipe;
                    *base_index += pipe_offset;
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;
        case SOC_CTR_INSTANCE_TYPE_XPE:
            switch (id) {
                case SOC_COUNTER_NON_DMA_DROP_RQ_PKT:
                case SOC_COUNTER_NON_DMA_DROP_RQ_BYTE:
                case SOC_COUNTER_NON_DMA_DROP_RQ_PKT_YELLOW:
                case SOC_COUNTER_NON_DMA_DROP_RQ_PKT_RED:
                case SOC_COUNTER_NON_DMA_POOL_PEAK:
                case SOC_COUNTER_NON_DMA_POOL_CURRENT:
                case SOC_COUNTER_NON_DMA_HDRM_POOL_PEAK:
                case SOC_COUNTER_NON_DMA_HDRM_POOL_CURRENT:
                case SOC_COUNTER_NON_DMA_EGRESS_POOL_PEAK:
                case SOC_COUNTER_NON_DMA_EGRESS_POOL_CURRENT:
                case SOC_COUNTER_NON_DMA_EGRESS_MCQE_POOL_PEAK:
                case SOC_COUNTER_NON_DMA_EGRESS_MCQE_POOL_CURRENT:
                    *base_index = 0;
                    *num_entries = non_dma->num_entries;
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;
        case SOC_CTR_INSTANCE_TYPE_PIPE:
            pipe = ctrl_info.instance;
            if ((pipe >=  NUM_PIPE(unit)) || (pipe < -1) ) {
                return SOC_E_PARAM;
            }
            switch (id) {
                case SOC_COUNTER_NON_DMA_EFP_PKT:
                case SOC_COUNTER_NON_DMA_EFP_BYTE:
                    *base_index = 0;
                    *num_entries = non_dma->num_entries / NUM_PIPE(unit);
                    if (pipe > 0) {
                        pipe_offset = pipe * (*num_entries);
                        *base_index += pipe_offset;
                    }
                    break;
                default:
                    return SOC_E_INTERNAL;
            }
            break;
        case SOC_CTR_INSTANCE_TYPE_POOL:
            c_offset = ctrl_info.instance;
            if ((c_offset < 0) || (c_offset >= non_dma->extra_ctr_ct)) {
                return SOC_E_PARAM;
            }

            switch (id) {
                case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE:
                case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
                case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
                case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:
                    *base_index = 0;

                    if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                            (non_dma->extra_ctrs != NULL)) {
                        /* Refer extra_ctrs_init for PKT/BYTE control block placement.
                         */
                        non_dma = &non_dma->extra_ctrs[(c_offset << 1)];
                    }
                    *num_entries = non_dma->num_entries / NUM_PIPE(unit);
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;
        case SOC_CTR_INSTANCE_TYPE_POOL_PIPE:
            c_offset = (ctrl_info.instance & 0x3F0) >> 4;
            pipe = ctrl_info.instance & 0xF;
            if ((c_offset < 0) || (c_offset >= non_dma->extra_ctr_ct) ||
                    (pipe >=  NUM_PIPE(unit)) || (pipe < 0)) {
                return SOC_E_PARAM;
            }
            switch (id) {
                case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE:
                case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
                case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
                case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:

                    if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                            (non_dma->extra_ctrs != NULL)) {
                        /* Refer extra_ctrs_init for PKT/BYTE control block placement.
                         */
                        non_dma = &non_dma->extra_ctrs[(c_offset << 1)];
                    }
                    *num_entries = non_dma->num_entries / NUM_PIPE(unit);
                    *base_index = (*num_entries)*pipe;
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;

        case SOC_CTR_INSTANCE_TYPE_SFLOW:
           c_offset = ctrl_info.instance;
           if ((c_offset < 0) || (c_offset >= non_dma->extra_ctr_ct)) {
               return SOC_E_PARAM;
           }

           *base_index = 0;
           if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                    (non_dma->extra_ctrs != NULL)) {
               non_dma = &non_dma->extra_ctrs[(c_offset << 1)];
           }
           *num_entries = non_dma->num_entries;
           break;

        default:
            return SOC_E_PARAM;
    }
    *base_index += non_dma->base_index;
    return SOC_E_NONE;
}


int
soc_counter_trident3_get_info(int unit, soc_port_t port, soc_reg_t id,
                              int *base_index, int *num_entries) {
    soc_info_t *si;
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    soc_port_t phy_port, mmu_port;
    int pipe, pipe_offset, obm_id = 0, lane = 0;
    uint8 c_offset = 0; /* child offset in Parent-child model */

    soc = SOC_CONTROL(unit);

    if ((id <  SOC_COUNTER_NON_DMA_START) || (id >= SOC_COUNTER_NON_DMA_END)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
                      "Invalid NON DMA Counter ID: %d.\n"), id));
        return SOC_E_PARAM;
    }
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID) &&
        !(non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* For Parent-Child model, return E_UNAVAIL if child non_dma is INVALID.
         */
        return SOC_E_UNAVAIL;
    }

    si = &SOC_INFO(unit);
    if (si->port_l2p_mapping[port] == -1) {
        *base_index = 0;
        *num_entries = 0;
        return SOC_E_NONE;
    }

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];
    pipe = si->port_pipe[port];

    switch (id) {
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
        if (IS_CPU_PORT(unit, port)) {
            *base_index = 0;
        } else {
            *base_index = pipe * 1368 + 48 + (port % 66) * 20 + 10;
        }
        *num_entries = si->port_num_cosq[port];
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC:
        if (IS_CPU_PORT(unit, port)) {
            *base_index = 0;
        } else {
            *base_index = pipe * 1368 + 48 + (port % 66) * 20;
        }
        *num_entries = si->port_num_cosq[port];
        break;
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT:
        if ((id == SOC_COUNTER_NON_DMA_COSQ_DROP_PKT) ||
            (id == SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE)) {
            *base_index = si->port_cosq_base[port];
            *num_entries = si->port_num_cosq[port];
        } else {
            *base_index = si->port_uc_cosq_base[port];
            *num_entries = si->port_num_uc_cosq[port];
        }
        pipe_offset =  (non_dma->num_entries / NUM_PIPE(unit)) * pipe;
        *base_index += pipe_offset;
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
        *base_index = mmu_port & 0x3f;
        pipe_offset =  (non_dma->num_entries / NUM_PIPE(unit)) * pipe;
        *base_index += pipe_offset;
        *num_entries = 1;
        break;
    case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE:
    case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
    case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
    case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:
        *base_index = 0;

        if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
            (non_dma->extra_ctrs != NULL)) {
            /* refer extra_ctrs_init for pkt/byte control block placement.
             */
            non_dma = &non_dma->extra_ctrs[(c_offset << 1)];
        }
        *num_entries = non_dma->num_entries / NUM_PIPE(unit);
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
        if (IS_CPU_PORT(unit, port)) {
            *base_index = 64;
        } else if (IS_LB_PORT(unit, port)) {
            *base_index = pipe * 128 + 65;
        } else {
            *base_index = pipe * 128 + (mmu_port & SOC_TD3_MMU_PORT_STRIDE);
        }
        *num_entries = 1;
        break;

    case SOC_COUNTER_NON_DMA_EFP_PKT:
    case SOC_COUNTER_NON_DMA_EFP_BYTE:
        *base_index = 0;
        *num_entries = non_dma->num_entries / NUM_PIPE(unit);
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1:
    case SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS:
        *base_index = 0;
        *num_entries = 1;
        /* find obm from the port number */
        SOC_IF_ERROR_RETURN
            (soc_trident3_port_obm_info_get(unit, port, &obm_id, &lane));
        pipe = si->port_pipe[port];
        soc_counter_trident3_get_child_dma_by_idx(unit, non_dma,
                                                  obm_id, &non_dma);
        *base_index += (pipe * 4) + lane;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    *base_index += non_dma->base_index;
    return SOC_E_NONE;
}

int
soc_counter_trident3_eviction_init(int unit)
{
    soc_control_t *soc;
    soc_counter_evict_t *evict;
    int pipe, mem_id, offset=0;
    int counter_id;
    static const struct {
        int pkt_counter_id;
        int byte_counter_id;
        soc_mem_t mem;
        int pool_id;
    } evict_info[SOC_TD3_NUM_EVICT_MEM] = {
        { /* MEMORY_ID 0 is reserved */
            -1,
            -1,
            INVALIDm,
            0
        },
        { /* MEMORY_ID 1 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_0m,
            0
        },
        { /* MEMORY_ID 2 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_1m,
            1
        },
        { /* MEMORY_ID 3 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_2m,
            2
        },
        { /* MEMORY_ID 4 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_3m,
            3
        },
        { /* MEMORY_ID 5 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_4m,
            4
        },
        { /* MEMORY_ID 6 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_5m,
            5
        },
        { /* MEMORY_ID 7 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_6m,
            6
        },
        { /* MEMORY_ID 8 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_7m,
            7
        },
        { /* MEMORY_ID 9 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_8m,
            8
        },
        { /* MEMORY_ID 10 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_9m,
            9
        },
        { /* MEMORY_ID 11 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_10m,
            10
        },
        { /* MEMORY_ID 12 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_11m,
            11
        },
        { /* MEMORY_ID 13 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_12m,
            12
        },
        { /* MEMORY_ID 14 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_13m,
            13
        },
        { /* MEMORY_ID 15 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_14m,
            14
        },
        { /* MEMORY_ID 16 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_15m,
            15
        },
        { /* MEMORY_ID 17 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_16m,
            16
        },
        { /* MEMORY_ID 18 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_17m,
            17
        },
        { /* MEMORY_ID 19 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_18m,
            18
        },
        { /* MEMORY_ID 20 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_19m,
            19
        },
        { /* MEMORY_ID 21 */
            SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
            EGR_FLEX_CTR_COUNTER_TABLE_0m,
            0
        },
        { /* MEMORY_ID 22 */
            SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
            EGR_FLEX_CTR_COUNTER_TABLE_1m,
            1
        },
        { /* MEMORY_ID 23 */
            SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
            EGR_FLEX_CTR_COUNTER_TABLE_2m,
            2
        },
        { /* MEMORY_ID 24 */
            SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
            EGR_FLEX_CTR_COUNTER_TABLE_3m,
            3
        },
        { /* MEMORY_ID 25 */
            SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
            SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
            EGR_PERQ_XMT_COUNTERSm,
            0
        },
        { /* MEMORY_ID 26 */
            SOC_COUNTER_NON_DMA_EFP_PKT,
            SOC_COUNTER_NON_DMA_EFP_BYTE,
            EFP_COUNTER_TABLEm,
            0
        }
    };

    soc = SOC_CONTROL(unit);

    if (soc->counter_evict == NULL) {
        /* memory_id 0 is reserved, allocate 1 extra entries */
        soc->counter_evict = sal_alloc(sizeof(soc_counter_evict_t) *
                                        SOC_TD3_NUM_EVICT_MEM,
                                       "Trident3 counter_evict");
        if (soc->counter_evict == NULL) {
            return SOC_E_MEMORY;
        }
    }

    counter_id = -1;
    soc->counter_evict_max_mem_id = 0;
    for (mem_id = 1; mem_id < sizeof(evict_info) / sizeof(evict_info[0]);
         mem_id++) {
        evict = &soc->counter_evict[mem_id];
        evict->pkt_counter_id = evict_info[mem_id].pkt_counter_id;
        evict->byte_counter_id = evict_info[mem_id].byte_counter_id;
        evict->pool_id = evict_info[mem_id].pool_id;
        soc->counter_evict_max_mem_id = mem_id;
        if (counter_id != evict->pkt_counter_id) {
            offset = 0;
            counter_id = evict->pkt_counter_id;
        }
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            evict->offset[pipe] = offset;
            evict->mem[pipe] =
                SOC_MEM_UNIQUE_ACC(unit, evict_info[mem_id].mem)[pipe];
            offset += soc_mem_index_count(unit, evict->mem[pipe]);
        }
    }

    return SOC_E_NONE;
}

int
soc_counter_trident3_dma_flags_update(int unit, soc_counter_non_dma_t *ctr_dma,
                                      uint32 flags, int val)
{
    int rv = SOC_E_NONE;
    int count = 0, subset_ct = 0;

    if ((ctr_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
        (ctr_dma->extra_ctrs != NULL)) {
        subset_ct = ctr_dma->extra_ctr_ct << 1;
        ctr_dma = ctr_dma->extra_ctrs;
    } else {
        subset_ct = 1;
    }

    do {
        switch (flags) {
            case _SOC_COUNTER_NON_DMA_CLEAR_ON_READ:
                /* Update only for CLR_ON_READ, until necessity calls for other
                 * FLAGS */
                if (ctr_dma->flags & _SOC_COUNTER_NON_DMA_CTR_EVICT) {
                    if (val) {
                         ctr_dma->flags |= _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
                    } else {
                         ctr_dma->flags &= ~_SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
                    }
                }
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_ALL:
                if (ctr_dma->flags & _SOC_COUNTER_NON_DMA_CTR_EVICT) {
                    ctr_dma->dma_rate_profile = (val) ?
                                       _SOC_COUNTER_DMA_RATE_PROFILE_ALL : 0;
                }
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_AUTO:
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_WFS:
                return SOC_E_UNAVAIL;
            default:
                return SOC_E_PARAM;
        }
        count++;
        ctr_dma++;
    } while ((count < subset_ct) && ctr_dma);

    return rv;
}

int
soc_counter_trident3_extra_ctrs_init(int unit, soc_reg_t id,
                                     soc_counter_non_dma_t *non_dma_parent,
                                     soc_counter_non_dma_t *non_dma_extra,
                                     uint32 extra_ctr_ct,
                                     int *total_entries)
{
    int i, pipe, count = 0;
    soc_mem_t *base_mem_array = NULL, base_mem = INVALIDm;
    int base_mem_ct = 0;
    int parent_base_index = 0;
    uint8 mmu_mem = FALSE;
    int xpe;

    soc_mem_t ing_flex_mems[] = {
        ING_FLEX_CTR_COUNTER_TABLE_0m,   /* MEMORYID 1 */
        ING_FLEX_CTR_COUNTER_TABLE_1m,   /* MEMORYID 2 */
        ING_FLEX_CTR_COUNTER_TABLE_2m,   /* MEMORYID 3 */
        ING_FLEX_CTR_COUNTER_TABLE_3m,   /* MEMORYID 4 */
        ING_FLEX_CTR_COUNTER_TABLE_4m,   /* MEMORYID 5 */
        ING_FLEX_CTR_COUNTER_TABLE_5m,   /* MEMORYID 6 */
        ING_FLEX_CTR_COUNTER_TABLE_6m,   /* MEMORYID 7 */
        ING_FLEX_CTR_COUNTER_TABLE_7m,   /* MEMORYID 8 */
        ING_FLEX_CTR_COUNTER_TABLE_8m,   /* MEMORYID 9 */
        ING_FLEX_CTR_COUNTER_TABLE_9m,   /* MEMORYID 10 */
        ING_FLEX_CTR_COUNTER_TABLE_10m,  /* MEMORYID 11 */
        ING_FLEX_CTR_COUNTER_TABLE_11m,  /* MEMORYID 12 */
        ING_FLEX_CTR_COUNTER_TABLE_12m,  /* MEMORYID 13 */
        ING_FLEX_CTR_COUNTER_TABLE_13m,  /* MEMORYID 14 */
        ING_FLEX_CTR_COUNTER_TABLE_14m,  /* MEMORYID 15 */
        ING_FLEX_CTR_COUNTER_TABLE_15m,  /* MEMORYID 16 */
        ING_FLEX_CTR_COUNTER_TABLE_16m,  /* MEMORYID 17 */
        ING_FLEX_CTR_COUNTER_TABLE_17m,  /* MEMORYID 18 */
        ING_FLEX_CTR_COUNTER_TABLE_18m,  /* MEMORYID 19 */
        ING_FLEX_CTR_COUNTER_TABLE_19m   /* MEMORYID 20 */
    };

    soc_mem_t egr_flex_mems[] = {
        EGR_FLEX_CTR_COUNTER_TABLE_0m,   /* MEMORYID 21 */
        EGR_FLEX_CTR_COUNTER_TABLE_1m,   /* MEMORYID 22 */
        EGR_FLEX_CTR_COUNTER_TABLE_2m,   /* MEMORYID 23 */
        EGR_FLEX_CTR_COUNTER_TABLE_3m   /* MEMORYID 24 */
    };

    /* DO SANITY CHECK FOR INPUT PARAMs */
    if (!(non_dma_parent->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* Only for Extra counters */
        return SOC_E_PARAM;
    }

    parent_base_index = non_dma_parent->base_index;
    *total_entries = 0;

    switch (id) {
        case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
        case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE:
            base_mem_array = ing_flex_mems;
            count = sizeof(ing_flex_mems) / sizeof(ing_flex_mems[0]);
            break;
        case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:
        case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
            base_mem_array = egr_flex_mems;
            count = sizeof(egr_flex_mems) / sizeof(egr_flex_mems[0]);
            break;
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
        case SOC_COUNTER_NON_DMA_QUEUE_PEAK:
        case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
        case SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK:
        case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT:
            mmu_mem = TRUE;
            count = NUM_XPE(unit);
            base_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma_parent->mem,
                                                   0, 0);
            if (base_mem != INVALIDm) {
                /* XPE 0/PIPE 0 combination is true for both IP/EP */
                base_mem_ct = soc_mem_index_max(unit, base_mem) + 1;
            }
            break;
        default:
            return SOC_E_INTERNAL;
    }

    if (extra_ctr_ct > count) {
        return SOC_E_INTERNAL;
    }

    for (i = 0; i < count; i++) {
        if (mmu_mem == FALSE) {
            non_dma_extra->mem = base_mem_array[i];
            base_mem_ct = soc_mem_index_count(unit, non_dma_extra->mem);

            /* Set Child attributes */
            non_dma_extra->base_index = parent_base_index + *total_entries;
            non_dma_extra->num_entries = NUM_PIPE(unit) * base_mem_ct;

            /* Inherit few info from the Parent */
            non_dma_extra->flags = (non_dma_parent->flags &
                                    ~_SOC_COUNTER_NON_DMA_SUBSET_PARENT) |
                                    _SOC_COUNTER_NON_DMA_SUBSET_CHILD;

            non_dma_extra->cname = sal_alloc(sal_strlen(non_dma_parent->cname) + 9,
                                             "Extra ctrs cname");

            if (non_dma_extra->cname == NULL) {
                return SOC_E_MEMORY;
            }
            sal_sprintf(non_dma_extra->cname, "*%s_PL%d",
                        non_dma_parent->cname, i);

            non_dma_extra->field = non_dma_parent->field;
            non_dma_extra->reg = non_dma_parent->reg;

            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                non_dma_extra->dma_mem[pipe] =
                    SOC_MEM_UNIQUE_ACC(unit, non_dma_extra->mem)[pipe];

                non_dma_extra->dma_index_max[pipe] =
                    soc_mem_index_max(unit, non_dma_extra->dma_mem[pipe]);
                non_dma_extra->dma_index_min[pipe] = 0;
                
                non_dma_extra->dma_buf[pipe] = non_dma_parent->dma_buf[pipe];
                *total_entries += (non_dma_extra->dma_index_max[pipe] -
                                    non_dma_extra->dma_index_min[pipe] + 1);
            }
            /* Allocation of Child Control blocks.
             * [0],..<even indices> - PKT_CTR non_dma ctrl block
             * [1],..<odd indices> - BYTE_CTR non_dma ctrl block
             * Hence in below code, jump by 2, so all similar control blocks are
             * initilized at the same time.
             */
            non_dma_extra += 2;
            /* coverity[check_after_deref] */
            if (!non_dma_extra) {
                /* ERR: Cannot be NULL, until 'i reaches 'count' */
                /* This condition may not hit but intentional for defensive check */
                /* coverity[dead_error_line] */
                return SOC_E_INTERNAL;
            }
        } else {
            int entries_per_xpe = 0;
            int entries_per_pipe = 0;
            xpe = i;
            /* Inherit few info from the Parent */
            non_dma_extra->flags = (non_dma_parent->flags &
                                    ~_SOC_COUNTER_NON_DMA_SUBSET_PARENT) |
                                    _SOC_COUNTER_NON_DMA_SUBSET_CHILD;

            non_dma_extra->cname = non_dma_parent->cname;
            non_dma_extra->field = non_dma_parent->field;
            non_dma_extra->reg = non_dma_parent->reg;
            non_dma_extra->entries_per_port = non_dma_parent->entries_per_port;
            non_dma_extra->id = non_dma_parent->id;
            non_dma_extra->num_valid_pipe = 0;

            /* Set Child attributes */
            non_dma_extra->base_index = parent_base_index + *total_entries;
            if (non_dma_parent->mem != MMU_CTR_COLOR_DROP_MEMm) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    non_dma_extra->dma_mem[pipe] =
                        SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma_parent->mem,
                                                    xpe, pipe);
                    non_dma_extra->dma_index_min[pipe] = 0;

                    if (non_dma_extra->dma_mem[pipe] != INVALIDm) {
                        non_dma_extra->dma_index_max[pipe] = base_mem_ct - 1;
                        non_dma_extra->dma_buf[pipe] = non_dma_parent->dma_buf[pipe];
                    } else {
                        non_dma_extra->dma_index_max[pipe] = -1;
                    }

                    entries_per_pipe = (non_dma_extra->dma_index_max[pipe] -
                                       non_dma_extra->dma_index_min[pipe] + 1);
                    if (entries_per_pipe > 0) {
                        non_dma_extra->num_valid_pipe ++;
                    }
                    entries_per_xpe += entries_per_pipe;
                    *total_entries += entries_per_pipe;
                }
                non_dma_extra->num_entries = entries_per_xpe;
                non_dma_extra->mem = non_dma_parent->mem;
            } else {
                for(pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    non_dma_extra->dma_mem[pipe] =
                        SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma_parent->mem,
                                                    xpe, pipe);
                    if (non_dma_extra->dma_mem[pipe] != INVALIDm) {
                        int mmu_total_ports;

#ifdef BCM_HELIX5_SUPPORT
                        if (SOC_IS_HELIX5X(unit)) {
                            mmu_total_ports = 72;
                        } else
#endif
                        {
                            mmu_total_ports = 66;
                        }
                        non_dma_extra->dma_buf[pipe] = non_dma_parent->dma_buf[pipe];
                        switch(non_dma_extra->id) {
                            case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
                                non_dma_extra->dma_index_min[pipe] = 0;
                                non_dma_extra->dma_index_max[pipe] = mmu_total_ports - 1;
                                break;
                            case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
                                non_dma_extra->dma_index_min[pipe] = mmu_total_ports;
                                non_dma_extra->dma_index_max[pipe]= (mmu_total_ports * 2) - 1;
                                break;
                            case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
                                non_dma_extra->dma_index_min[pipe] = mmu_total_ports * 2;
                                non_dma_extra->dma_index_max[pipe] = (mmu_total_ports * 3) - 1;
                                break;
                            case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
                                non_dma_extra->dma_index_min[pipe] = mmu_total_ports * 3;
                                non_dma_extra->dma_index_max[pipe] = (mmu_total_ports * 4) - 1;
                                break;
                            case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
                                non_dma_extra->dma_index_min[pipe] = mmu_total_ports * 4;
                                non_dma_extra->dma_index_max[pipe] = (mmu_total_ports * 5) - 1;
                                break;
                            default:
                                return SOC_E_PARAM;
                        }
                    }else {
                        non_dma_extra->dma_index_max[pipe] = -1;
                    }
                    entries_per_pipe = (non_dma_extra->dma_index_max[pipe] -
                                        non_dma_extra->dma_index_min[pipe] + 1);
                    entries_per_xpe += entries_per_pipe;
                    *total_entries += entries_per_pipe;
                }
                non_dma_extra->mem = non_dma_parent->mem;
                non_dma_extra->num_entries += entries_per_xpe;
            }

            /* Allocation of Child Control blocks.
             * [0],..<even indices> - PKT_CTR non_dma ctrl block
             * [1],..<odd indices> - BYTE_CTR non_dma ctrl block
             * Hence in below code, jump by 2, so all similar control blocks are
             * initilized at the same time.
             */
            non_dma_extra += 2;
            /* coverity[check_after_deref] */
            if (!non_dma_extra) {
                /* ERR: Cannot be NULL, until 'i reaches 'count' */
                /* coverity[dead_error_line] */
                return SOC_E_INTERNAL;
            }
        }
    }
    return SOC_E_NONE;
}

/* Function to initialize sFlow extra counters */
int
soc_counter_td3_extra_sflow_ctrs_init(int unit,
                               soc_counter_non_dma_t *non_dma_parent,
                               soc_counter_non_dma_t *non_dma_extra,
                               uint32 extra_ctr_ct,
                               int *total_entries)
{
    int i, parent_base_index = 0;
    int base_mem_ct = 0;
    int do_dma = 1;
    int pipe;

    soc_field_t sflow_counter_fields[] = {
        SAMPLE_POOLf,
        SAMPLE_POOL_SNAPSHOTf,
        SAMPLE_COUNTf
    };

    /* Do sanity check for input params */
    if (!(non_dma_parent->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* Only for Extra counters */
        return SOC_E_PARAM;
    }

    parent_base_index = non_dma_parent->base_index;
    *total_entries = 0;

    for (i = 0; i < extra_ctr_ct; i++) {
        /* Set Child attributes. Inherit few info from the Parent */
        non_dma_extra->mem = non_dma_parent->mem;
        non_dma_extra->base_index = parent_base_index + *total_entries;
        base_mem_ct = (soc_mem_index_count(unit, non_dma_extra->mem) *
                        NUM_PIPE(unit)) / _TD3_PIPES_PER_DEV;

        if (NUM_PIPE(unit) == 1) {
            base_mem_ct = soc_mem_index_count(unit, non_dma_extra->mem);
        }

        non_dma_extra->num_entries = base_mem_ct;

        non_dma_extra->flags = (non_dma_parent->flags &
                                ~_SOC_COUNTER_NON_DMA_SUBSET_PARENT) |
                                _SOC_COUNTER_NON_DMA_SUBSET_CHILD;

        /* DMA should be done only once to get sFlow data table */
        if (do_dma != 1) {
            non_dma_extra->flags = (non_dma_extra->flags &
                                    ~_SOC_COUNTER_NON_DMA_DO_DMA);
        }

        non_dma_extra->cname = sal_alloc(sal_strlen(non_dma_parent->cname) + 9,
                                         "Extra ctrs cname");

        if (non_dma_extra->cname == NULL) {
            return SOC_E_MEMORY;
        }

        sal_sprintf(non_dma_extra->cname, "*%s_CNTR%d",
                    non_dma_parent->cname, i);

        non_dma_extra->field = sflow_counter_fields[i];
        non_dma_extra->reg = non_dma_parent->reg;
        non_dma_extra->dma_mem[0] = non_dma_parent->mem;

        /* No per-pipe instance for sFlow counters memory, hence pipe 1/2/3
         * are assigned to INVALIDm
         */
        for (pipe = 1; pipe < NUM_PIPE(unit); pipe++) {
            non_dma_extra->dma_mem[pipe] = INVALIDm;
        }
        non_dma_extra->dma_index_max[0] = (soc_mem_index_max(unit,
                                                        non_dma_parent->mem) *
                                                        NUM_PIPE(unit)) /
                                                        _TD3_PIPES_PER_DEV;
        if (NUM_PIPE(unit) == 1) {
            non_dma_extra->dma_index_max[0] =
                soc_mem_index_max(unit, non_dma_parent->mem);
        }

        for (pipe = 1; pipe < NUM_PIPE(unit); pipe++) {
            non_dma_extra->dma_index_max[pipe] = -1;
        }
        non_dma_extra->dma_index_min[0] = 0;
        for (pipe = 1; pipe < NUM_PIPE(unit); pipe++) {
            non_dma_extra->dma_index_min[pipe] = -1;
        }
        non_dma_extra->dma_buf[0] = non_dma_parent->dma_buf[0];

        *total_entries += (non_dma_extra->dma_index_max[0] -
                           non_dma_extra->dma_index_min[0] + 1);

        /* Allocation of Child Control blocks.
         * [0],..<even indices> - PKT_CTR non_dma ctrl block
         * [1],..<odd indices> - BYTE_CTR non_dma ctrl block
         * Hence in below code, jump by 2, so all similar control blocks are
         * initilized at the same time.
         */
        non_dma_extra += 2;

        do_dma = 0;

        /* coverity[check_after_deref] */
        if (!non_dma_extra) {
            /* ERR: Cannot be NULL, until 'i reaches 'extra_ctr_ct' */
            /* This condition may not hit but intentional for defensive check */
            /* coverity[dead_error_line] */
            return SOC_E_INTERNAL;
        }
    }

    return SOC_E_NONE;
}

/* Function to initialize OBM extra counters */
int
soc_counter_trident3_extra_obm_ctrs_init(int unit,
                                         soc_counter_non_dma_t *non_dma_parent,
                                         soc_counter_non_dma_t *non_dma_extra,
                                         uint32 extra_ctr_ct,
                                         int *total_entries)
{
#define LOSSY_LO 0
#define LOSSY_HI 1
#define LOSSLESS0 2
#define LOSSLESS1 3
    char obm_type_str[][12] = {"LOSSY_LO", "LOSSY_HI",
                               "LOSSLESS0", "LOSSLESS1"}; /* For CNAME */
    int i, byte_flag = 0, obm_type = 0, fc = 0, parent_base_index = 0;
    uint32 count = 0;

    soc_reg_t obm_drop_pkt_regs[][4] = {
        {IDB_OBM0_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM0_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM0_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM0_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM1_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM1_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM1_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM1_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM2_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM2_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM2_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM2_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM3_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM3_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM3_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM3_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM4_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM4_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM4_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM4_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM5_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM5_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM5_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM5_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM6_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM6_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM6_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM6_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM7_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM7_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM7_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM7_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM8_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM8_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM8_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM8_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM9_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM9_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM9_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM9_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM10_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM10_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM10_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM10_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM11_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM11_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM11_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM11_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM12_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM12_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM12_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM12_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM13_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM13_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM13_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM13_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM14_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM14_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM14_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM14_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM15_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM15_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM15_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM15_LOSSLESS1_PKT_DROP_COUNTr
        },
#if defined(BCM_MAVERICK2_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
        {IDB_OBM16_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM16_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM16_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM16_LOSSLESS1_PKT_DROP_COUNTr
        },
#endif
#ifdef BCM_MAVERICK2_SUPPORT
        {IDB_OBM17_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM17_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM17_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM17_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM18_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM18_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM18_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM18_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM19_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM19_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM19_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM19_LOSSLESS1_PKT_DROP_COUNTr
        },
#endif
    };
    soc_reg_t obm_drop_byte_regs[][4] = {
        {IDB_OBM0_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM0_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM0_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM0_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM1_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM1_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM1_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM1_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM2_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM2_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM2_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM2_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM3_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM3_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM3_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM3_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM4_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM4_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM4_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM4_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM5_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM5_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM5_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM5_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM6_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM6_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM6_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM6_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM7_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM7_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM7_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM7_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM8_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM8_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM8_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM8_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM9_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM9_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM9_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM9_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM10_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM10_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM10_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM10_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM11_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM11_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM11_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM11_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM12_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM12_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM12_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM12_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM13_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM13_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM13_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM13_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM14_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM14_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM14_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM14_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM15_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM15_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM15_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM15_LOSSLESS1_BYTE_DROP_COUNTr
        },
#if defined(BCM_MAVERICK2_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
        {IDB_OBM16_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM16_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM16_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM16_LOSSLESS1_BYTE_DROP_COUNTr
        },
#endif
#ifdef BCM_MAVERICK2_SUPPORT
        {IDB_OBM17_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM17_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM17_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM17_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM18_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM18_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM18_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM18_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM19_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM19_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM19_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM19_LOSSLESS1_BYTE_DROP_COUNTr
        },
#endif
    };
    soc_reg_t obm_fc_event_regs[] = {
        IDB_OBM0_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM1_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM2_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM3_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM4_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM5_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM6_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM7_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM8_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM9_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM10_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM11_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM12_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM13_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM14_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM15_FLOW_CONTROL_EVENT_COUNTr,
#if defined(BCM_MAVERICK2_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
        IDB_OBM16_FLOW_CONTROL_EVENT_COUNTr,
#endif
#ifdef BCM_MAVERICK2_SUPPORT
        IDB_OBM17_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM18_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM19_FLOW_CONTROL_EVENT_COUNTr,
#endif
    };
    count = COUNTOF(obm_drop_byte_regs);
    if (count < extra_ctr_ct) {
        return SOC_E_PARAM;
    }

    /* Do sanity check for input params */
    if (!(non_dma_parent->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* Only for Extra counters */
        return SOC_E_PARAM;
    }
    switch (non_dma_parent->id) {
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1:
            byte_flag = 0;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1:
            byte_flag = 1;
            break;
        case SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS:
            fc = 1;
            break;
        default:
            return SOC_E_PARAM;
    }

    switch (non_dma_parent->id) {
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO:
            obm_type = LOSSY_LO;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI:
            obm_type = LOSSY_HI;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0:
            obm_type = LOSSLESS0;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1:
            obm_type = LOSSLESS1;
            break;
    }

    parent_base_index = non_dma_parent->base_index;
    *total_entries = 0;

    for (i = 0; i < extra_ctr_ct; i++) {
        *non_dma_extra = *non_dma_parent;
        if (fc) {
            non_dma_extra->reg = obm_fc_event_regs[i];
        } else if (byte_flag == 0) {
            non_dma_extra->reg = obm_drop_pkt_regs[i][obm_type];
        } else {
            non_dma_extra->reg = obm_drop_byte_regs[i][obm_type];
        }
        non_dma_extra->extra_ctrs = NULL;
        non_dma_extra->extra_ctr_ct = 0;
        non_dma_extra->base_index = parent_base_index + *total_entries;
        non_dma_extra->flags = (non_dma_parent->flags &
                                ~_SOC_COUNTER_NON_DMA_SUBSET_PARENT) |
                                _SOC_COUNTER_NON_DMA_SUBSET_CHILD;

        non_dma_extra->cname = sal_alloc(sal_strlen(non_dma_parent->cname) + 9,
                                         "Extra ctrs cname");

        if (non_dma_extra->cname == NULL) {
            return SOC_E_MEMORY;
        }

        if (fc) {
            sal_sprintf(non_dma_extra->cname, "*OBM%d_FC_EVENTS", i);
        } else {
            sal_sprintf(non_dma_extra->cname, "*OBM%d_%s_DRP_%s",
                        i, obm_type_str[obm_type], byte_flag ? "BYTE":"PKT");
        }

        non_dma_extra->num_entries = non_dma_extra->entries_per_port *
                                     NUM_PIPE(unit);
        *total_entries += non_dma_extra->num_entries;

        /* Allocation of Child Control blocks.
         * [0],..<even indices> - PKT_CTR non_dma ctrl block
         * [1],..<odd indices> - BYTE_CTR non_dma ctrl block
         * Hence in below code, jump by 2, so all similar control blocks are
         * initilized at the same time.
         */
        non_dma_extra += 2;
        /* coverity[check_after_deref] */
        if (!non_dma_extra) {
            /* ERR: Cannot be NULL, until 'i reaches 'extra_ctr_ct' */
            /* This condition may not hit but intentional for defensive check */
            /* coverity[dead_error_line] */
            return SOC_E_INTERNAL;
        }
    }
    return SOC_E_NONE;
}

/* Trident3 Counters are of 2 types:
 * 1. Traditional Non-StatDma
 * 2. Counter Eviction eligible counters.
 *
 * When Counter Eviction is enabled, HW performs Clear on Read on the eviction
 * enabled counters when a Read is performed.
 *
 * When Counter Eviction is disabled, all counters will work as in TD2.
 */
int
soc_counter_trident3_non_dma_init(int unit, int nports,
                                  int non_dma_start_index,
                                  int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1, *non_dma2, *temp;
    int rv = SOC_E_NONE;
    int num_entries, alloc_size, entry_words, pipe, num_extra_ctr_entries;
    int pool_count = 0;
    soc_mem_t child_mem = INVALIDm;
    uint32 *buf;
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);

    if (soc_ctr_ctrl == NULL) {
        return SOC_E_INIT;
    }

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    /* Registration of Chip specific functions to the Global Counter 
     * control structure.
     */
    soc_ctr_ctrl->get_child_dma = soc_counter_trident3_get_child_dma;
    soc_ctr_ctrl->dma_config_update = soc_counter_trident3_dma_flags_update;
    soc_ctr_ctrl->port_obm_info_get = soc_trident3_port_obm_info_get;

    /* ING_FLEX_CTR_COUNTER_TABLE_0 is the largest table to be DMA'ed */
    num_entries = soc_mem_index_count(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    entry_words = soc_mem_entry_words(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    alloc_size = NUM_PIPE(unit) * num_entries * sizeof(uint32) * entry_words;

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    /* EGR_PERQ counters - Control Block
     * Eviction enabled counter
     */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_ALLOC |
                      _SOC_COUNTER_NON_DMA_CTR_EVICT;

    non_dma0->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE_ALL;

    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* cpu port has max number of queues */
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "MC_PERQ_PKT";
    num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    entry_words = soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC(unit, non_dma0->mem)[pipe];
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }

    *non_dma_entries += non_dma0->num_entries;
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "MC_PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma2->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;
    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 10; /* 10 UC Qs Per port */
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_PKT";

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma2->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;
    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 10; /* 10 UC Qs Per port */
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_BYTE";
    /* End of EGR_PERQ counter control block */

    /* CTR_MC_DROP Counters - Control Block */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->mem = MMU_CTR_MC_DROP_MEMm;

    /* MMU memories with Unique access type have number of entries in base view
     * set to zero. So, get number of entries from first expanded view
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "PERQ_DROP_PKT";

    for(pipe = 0; pipe < NUM_PIPE(unit);pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_perq_drop_mc");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }

    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "PERQ_DROP_BYTE";
    non_dma1->extra_ctr_ct = NUM_XPE(unit);

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs) {
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                     SOC_COUNTER_NON_DMA_START));
        }
    }
    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    *non_dma_entries += non_dma1->num_entries;
    /* End of CTR_MC_DROP Control Block */

    /* DROP_PKT_UC Counters - Control Block */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 10; /* 10 UC queues per port */
    non_dma0->mem = MMU_CTR_UC_DROP_MEMm;

    /* MMU memories with Unique access type have number of entries in base view
     * set to zero. So, get number of entries from first expanded view
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "PERQ_DROP_PKT_UC";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_perq_drop_uc");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }

    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "PERQ_DROP_BYTE_UC";
    non_dma1->extra_ctr_ct = NUM_XPE(unit);

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs)
    {
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC -
                     SOC_COUNTER_NON_DMA_START));
        }
    }
    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;

    *non_dma_entries += non_dma1->num_entries;
    /* End of DROP_PKT_UC Counters Control Block */

    /* DROP_PKT_ING Counters - Control Block */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_CTR_ING_DROP_MEMm;

    /* MMU memories with Unique access type have number of entries in base view
     * set to zero. So, get number of entries from first expanded view
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "DROP_PKT_ING";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_ing_drop");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "DROP_BYTE_ING";
    non_dma1->extra_ctr_ct = NUM_XPE(unit);

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs) {
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING -
                     SOC_COUNTER_NON_DMA_START));
        }
    }
    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;

    *non_dma_entries += non_dma1->num_entries;
    /* End of DROP_PKT_ING Counters - Control Block */

    /* Color Drop Counter */
    /* Control block for wred counter */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_CTR_COLOR_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->cname = "WRED_PKT_RED";
    non_dma0->field = PKTCNTf;
    num_entries = 66; /* 1 per port, 66 per pipe */
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_min[pipe] = 0;
        non_dma0->dma_index_max[pipe] = 65;
    }
    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used. */

    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_wred_pkt_red");

    /*
     * Below represents how Extra cntrs are arranged for wred red pkt, wred
     * yellow pkt, wred green pkt, drop pkt red, drop pkt yellow counters.
     *
     *  n = NUM_XPE
     *
     *   [0]     [2]        [2n-2]
     *  ------  ------     ------
     * |      ||      |...|      |
     * | PKT  || PKT  |...| PKT  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------ ... ------
     * |      ||      |...|      |
     * | N/A  || N/A  |...| N/A  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------     ------
     *   [1]     [3]        [2n-1]
     */

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* Control block for wred yellow counter */ 
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_YEL";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma1->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma1->dma_index_min[pipe] = 66;
        non_dma1->dma_index_max[pipe] = 131;
    }
    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used. */
    non_dma1->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_wred_pkt_yel");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Control block for wred green counter */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_GRE";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma1->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma1->dma_index_min[pipe] = 132;
        non_dma1->dma_index_max[pipe] = 197;
    }
    non_dma1->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_wred_pkt_gre");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Control block for red drop counter */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "DROP_PKT_RED";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma1->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma1->dma_index_min[pipe] = 198;
        non_dma1->dma_index_max[pipe] = 263;
    }
    non_dma1->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_drop_pkt_red");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }

    *non_dma_entries += non_dma1->num_entries;

    /* Control block for yellow drop counter */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "DROP_PKT_YEL";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma1->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma1->dma_index_min[pipe] = 264;
        non_dma1->dma_index_max[pipe] = 329;
    }
    non_dma1->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_drop_pkt_yel");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* End Color Drop Counter */

    /* MC Queue Count Counters - Control Block */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_QUEUE_PEAK;

    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_PEAK |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->mem = MMU_THDM_DB_QUEUE_COUNTm;

    /* MMU memories with Unique access type have number of entries in base view
     * set to zero. So, get number of entries from first expanded view
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->field = SHARED_COUNTf;
    non_dma0->cname = "QUEUE_PEAK";
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_mc_q_ct");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_QUEUE_PEAK,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                                SOC_COUNTER_NON_DMA_QUEUE_PEAK -
                                SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }

    *non_dma_entries += non_dma0->num_entries;
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_QUEUE_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma2->cname = "QUEUE_CUR";
    /* End of MC Queue Count Counters - Control Block */

    /* UC Queue Count Counters - Control Block */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK;

    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_PEAK |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit); /* No UC queue for CPU/LB ports */
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 10;
    non_dma0->mem = MMU_THDU_COUNTER_QUEUEm;

    /* MMU memories with Unique access type have number of entries in base view
     * set to zero. So, get number of entries from first expanded view
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->field = SHARED_COUNTf;
    non_dma0->cname = "UC_QUEUE_PEAK";
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_uc_q_ct");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                                SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK -
                                SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }

    *non_dma_entries += non_dma0->num_entries;
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma2->cname = "UC_QUEUE_CUR";
    /* End of UC Queue Count Counters - Control Block */

    /* RQ Drop Counters - Control Block */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->id = SOC_COUNTER_NON_DMA_DROP_RQ_PKT;
    SOC_PBMP_CLEAR(non_dma0->pbmp); /* Port = 0 */
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 11;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = DROP_PKT_CNT_RQE_PKT_64r;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "RQ_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_DROP_RQ_BYTE;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_RQE_BYTE_64r;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "RQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_DROP_RQ_PKT_YELLOW;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_RQE_YEL_64r;
    non_dma1->field = PKTCNTf;
    non_dma1->cname = "RQ_DROP_PKT_YEL";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_DROP_RQ_PKT_RED;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_RQE_RED_64r;
    non_dma1->field = PKTCNTf;
    non_dma1->cname = "RQ_DROP_PKT_RED";
    *non_dma_entries += non_dma1->num_entries;
    /* End of RQ Drop Counters - Control Block */

    /* Pool Peak Counters - Control Block */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_POOL_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->id = SOC_COUNTER_NON_DMA_POOL_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp); /* Port = 0 */
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 4;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = THDI_POOL_SHARED_COUNT_SPr;
    non_dma0->field = TOTAL_BUFFER_COUNTf;
    non_dma0->cname = "POOL_PEAK";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_POOL_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_POOL_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "POOL_CUR";
    /* End of Pool Peak Counters -  Control Block */

    /* PG Counters - Control Block */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PG_MIN_PEAK;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->mem = THDI_PORT_PG_CNTRS_RT1m;

    /* MMU memories with Unique access type have number of entries in base view
     * set to zero. So, get number of entries from first expanded view
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PG_MIN_COUNTf;
    non_dma0->cname = "PG_MIN_PEAK";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }
    *non_dma_entries += non_dma0->num_entries;
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG_MIN_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_MIN_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PG_SHARED_PEAK;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_PEAK;
    non_dma1->cname = "PG_SHARED_PEAK";
    non_dma1->field = PG_SHARED_COUNTf;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_CURRENT |
                      _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->cname = "PG_SHARED_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PG_HDRM_PEAK;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->mem = THDI_PORT_PG_CNTRS_RT2m;

    /* MMU memories with Unique access type have number of entries in base view
     * set to zero. So, get number of entries from first expanded view
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PG_HDRM_COUNTf;
    non_dma0->cname = "PG_HDRM_PEAK";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_HDRM_CUR";
    /* End of PG Counters - Control Block */

    /* ING_FLEX counters - Control Block
     * Eviction enabled counters
     */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_ING_FLEX_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_ING_FLEX_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;

   /* Clear on Read Flag set if eviction is enabled */
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    pool_count = 20;
    non_dma0->num_entries =
        pool_count * NUM_PIPE(unit) *
                        soc_mem_index_count(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    non_dma0->mem = ING_FLEX_CTR_COUNTER_TABLE_0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "ING_FLEX_PKT";

    num_entries = soc_mem_index_count(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    entry_words = soc_mem_entry_words(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }
    non_dma0->extra_ctr_ct = pool_count;
    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_ing_flex");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init
                                           (unit,
                                            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_ING_FLEX_PKT -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_ING_FLEX_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_ING_FLEX_BYTE;
    
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;

    /* Clear on Read Flag set if eviction is enabled */
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "ING_FLEX_BYTE";
    non_dma1->extra_ctr_ct = pool_count; /* Taken care by PKT_CTR */

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs)
    {
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_ING_FLEX_BYTE -
                     SOC_COUNTER_NON_DMA_START));
        }
    }

    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    *non_dma_entries += non_dma1->num_entries;

    /* End of ING_FLEX counters - Control Block */

    /* EGR_FLEX Counters - Control Block */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_FLEX_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_EGR_FLEX_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;

    /* Clear on Read Flag set if eviction is enabled */
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    pool_count = 4;
    non_dma0->entries_per_port = 0;
    non_dma0->mem = EGR_FLEX_CTR_COUNTER_TABLE_0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "EGR_FLEX_PKT";

    num_entries = soc_mem_index_count(unit, EGR_FLEX_CTR_COUNTER_TABLE_0m);
    entry_words = soc_mem_entry_words(unit, EGR_FLEX_CTR_COUNTER_TABLE_0m);
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = pool_count;

    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_ing_flex");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_EGR_FLEX_PKT -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }

    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE;

    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    /* Clear on Read Flag set if eviction is enabled */
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "EGR_FLEX_BYTE";
    non_dma1->extra_ctr_ct = pool_count; /* Taken care by PKT_CTR */

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs)
    {
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE -
                     SOC_COUNTER_NON_DMA_START));
        }
    }
    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    *non_dma_entries += non_dma1->num_entries;

    /* End of EGR_FLEX Counters - Control Block */

    /* EFP Counters - Control Block */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EFP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_EFP_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, EFP_COUNTER_TABLEm);
    non_dma0->mem = EFP_COUNTER_TABLEm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "EFP_PKT";

    num_entries = soc_mem_index_count(unit, EFP_COUNTER_TABLEm);
    entry_words = soc_mem_entry_words(unit, EFP_COUNTER_TABLEm);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC(unit, non_dma0->mem)[pipe];
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }

    *non_dma_entries += non_dma0->num_entries;


    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EFP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_EFP_BYTE;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "EFP_BYTE";
    *non_dma_entries += non_dma1->num_entries;
    /* End of EFP Counters - Control Block */

    /* Control block for Ingress sFlow counters */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_SFLOW_ING_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_SFLOW_ING_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA
                      | _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = SFLOW_ING_DATA_SOURCEm;

    num_entries = (soc_mem_index_count(unit, SFLOW_ING_DATA_SOURCEm) *
                                        NUM_PIPE(unit)) / _TD3_PIPES_PER_DEV;

#ifdef BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        num_entries = soc_mem_index_count(unit, SFLOW_ING_DATA_SOURCEm);
    }
#endif

    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = SOC_SFLOW_CTR_TYPE_COUNT * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->cname = "SFLOW_ING_PKT";
    non_dma0->field = SAMPLE_POOLf;
    non_dma0->dma_buf[0] = &buf[0];

    non_dma0->extra_ctr_ct = SOC_SFLOW_CTR_TYPE_COUNT;

    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used.
     * This memory supports BYTE Count, but not implemented yet. So space
     * reserved.
     */
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_sflow_ing");

    /*
     * Below represents how Extra cntrs are arranged
     * for sFlow counters.
     *
     *       --------------------------
     * [0]  |      SAMPLE_POOL [0]     |
     *       --------------------------
     * [1]  |           N/A [0]        |
     *       --------------------------
     * [2]  | SAMPLE_POOL_SNAPSHOT [0] |
     *       --------------------------
     * [3]  |           N/A [0]        |
     *       --------------------------
     * [4]  |      SAMPLE_COUNT [0]    |
     *       --------------------------
     * [5]  |           N/A [0]        |
     *       --------------------------
 */

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_td3_extra_sflow_ctrs_init(unit,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                        SOC_COUNTER_NON_DMA_SFLOW_ING_PKT -
                        SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* Control block for Flex sFlow counters */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_SFLOW_FLEX_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_SFLOW_FLEX_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA
                      | _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = SFLOW_ING_FLEX_DATA_SOURCEm;

    num_entries = (soc_mem_index_count(unit, SFLOW_ING_FLEX_DATA_SOURCEm) *
                                        NUM_PIPE(unit)) / _TD3_PIPES_PER_DEV;

#ifdef BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        num_entries = soc_mem_index_count(unit, SFLOW_ING_FLEX_DATA_SOURCEm);
    }
#endif

    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = SOC_SFLOW_CTR_TYPE_COUNT * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->cname = "SFLOW_FLEX_PKT";
    non_dma0->field = SAMPLE_POOLf;
    non_dma0->dma_buf[0] = &buf[0];

    non_dma0->extra_ctr_ct = SOC_SFLOW_CTR_TYPE_COUNT;

    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used.
     * This memory supports BYTE Count, but not implemented yet. So space
     * reserved.
     */
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_sflow_flex");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_td3_extra_sflow_ctrs_init(unit,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                        SOC_COUNTER_NON_DMA_SFLOW_FLEX_PKT -
                        SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;




    /* Control block for IDB OBM counters */
    /* Lossy LO drop counters - Pkt Counters */
    non_dma0 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO -
                              SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO;
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->reg = IDB_OBM0_LOSSY_LO_PKT_DROP_COUNTr;
    non_dma0->cname = "OBM_LOSSY_LO_DRP_PKT";

    /* Common across all OBM structures */
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_OBM;
    /* Use Parent-Child model to cover all 16 OBMs */
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->entries_per_port = 4;
    non_dma0->num_entries = 4 * NUM_PIPE(unit);
    non_dma0->mem = INVALIDm;
    non_dma0->field = COUNTERf;
    non_dma0->dma_buf[0] = &buf[0];

    /*
     * Below represents how Extra cntrs are arranged
     * for OBM 0-15 Drop Counters.
     * n = NUM_OBMs (16)
     *
     *   [0]     [2]        [2n-2]
     *  ------  ------     ------
     * |      ||      |...|      |
     * | PKT  || PKT  |...| PKT  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------ ... ------
     * |      ||      |...|      |
     * | BYTE || BYTE |...| BYTE |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------     ------
     *   [1]     [3]        [2n-1]
     */
    non_dma0->extra_ctr_ct = _TD3_PBLKS_PER_PIPE; /* 16 OBMs per Pipe */

#ifdef BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        non_dma0->extra_ctr_ct = 20; /* 20 OBMs per Pipe */
    }
#endif

    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    temp = non_dma0->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossy_lo");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_obm_ctrs_init(unit,
                                                      non_dma0,
                                                      non_dma0->extra_ctrs,
                                                      non_dma0->extra_ctr_ct,
                                                      &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                        non_dma0->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* Lossy LO drop counters - Byte Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSY_LO_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSY_LO_DRP_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_trident3_extra_obm_ctrs_init(unit,
                                                      non_dma1,
                                                      non_dma1->extra_ctrs,
                                                      non_dma1->extra_ctr_ct,
                                                      &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Lossy HI drop counters - Packet Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSY_HI_PKT_DROP_COUNTr;
    non_dma1->cname = "OBM_LOSSY_HI_DRP_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossy_hi");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_obm_ctrs_init(unit,
                                                      non_dma1,
                                                      non_dma1->extra_ctrs,
                                                      non_dma1->extra_ctr_ct,
                                                      &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;


    /* Lossy HI drop counters - Byte Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSY_HI_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSY_HI_DRP_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_trident3_extra_obm_ctrs_init(unit,
                                                      non_dma1,
                                                      non_dma1->extra_ctrs,
                                                      non_dma1->extra_ctr_ct,
                                                      &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;


    /* Lossless0 drop counters - Packet Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS0_PKT_DROP_COUNTr;
    non_dma1->cname = "OBM_LOSSLESS0_DRP_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossless0");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_obm_ctrs_init(unit,
                                                      non_dma1,
                                                      non_dma1->extra_ctrs,
                                                      non_dma1->extra_ctr_ct,
                                                      &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Lossless0 drop counters - Byte Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS0_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSLESS0_DRP_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_trident3_extra_obm_ctrs_init(unit,
                                                      non_dma1,
                                                      non_dma1->extra_ctrs,
                                                      non_dma1->extra_ctr_ct,
                                                      &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Lossless1 drop counters - Packet counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS1_PKT_DROP_COUNTr;
    non_dma1->cname = "OBM_LOSSLESS1_DRP_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossless1");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_obm_ctrs_init(unit,
                                                      non_dma1,
                                                      non_dma1->extra_ctrs,
                                                      non_dma1->extra_ctr_ct,
                                                      &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Lossless1 drop counters - Byte Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS1_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSLESS1_DRP_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_trident3_extra_obm_ctrs_init(unit,
                                                      non_dma1,
                                                      non_dma1->extra_ctrs,
                                                      non_dma1->extra_ctr_ct,
                                                      &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

    /* FC event counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_FLOW_CONTROL_EVENT_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_FC_EVENTS";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_fc");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_obm_ctrs_init(unit,
                                                      non_dma1,
                                                      non_dma1->extra_ctrs,
                                                      non_dma1->extra_ctr_ct,
                                                      &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;
    /* End of Control block for IDB OBM counters */


   /* Ctrl block for Headroom Pool counter */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_HDRM_POOL_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->id = SOC_COUNTER_NON_DMA_HDRM_POOL_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 4 * NUM_XPE(unit);
    non_dma0->mem = INVALIDm;
    non_dma0->reg = THDI_HDRM_POOL_COUNT_HPr;
    non_dma0->field = TOTAL_BUFFER_COUNTf;
    non_dma0->cname = "HDRM_POOL_PEAK";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_HDRM_POOL_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_HDRM_POOL_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "HDRM_POOL_CUR";
    /* End Ctrl block for Headroom Pool counter */

   /* Ctrl block for Egress Pool counter */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGRESS_POOL_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->id = SOC_COUNTER_NON_DMA_EGRESS_POOL_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 4 * NUM_XPE(unit);
    non_dma0->mem = INVALIDm;
    non_dma0->reg = MMU_THDM_DB_POOL_SHARED_COUNTr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "EGRESS_POOL_PEAK";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGRESS_POOL_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_EGRESS_POOL_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "EGRESS_POOL_CUR";
    /* End Ctrl block for Egress Pool counter */

   /* Ctrl block for queue group counter */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QGROUP_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma0->id = SOC_COUNTER_NON_DMA_QGROUP_MIN_CURRENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_THDU_COUNTER_QGROUPm;
    non_dma0->reg = INVALIDm;
    non_dma0->field = MIN_COUNTf;
    non_dma0->cname = "QGROUP_MIN_CURRENT";
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC(unit, non_dma0->mem)[pipe];
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }

    *non_dma_entries += non_dma0->num_entries;
    /* End Ctrl block for queue group counter */

    /* Ctrl block for Egress MCQE Pool counter */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGRESS_MCQE_POOL_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->id = SOC_COUNTER_NON_DMA_EGRESS_MCQE_POOL_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 4 * NUM_XPE(unit);
    non_dma0->mem = INVALIDm;
    non_dma0->reg = MMU_THDM_MCQE_POOL_SHARED_COUNTr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "EGRESS_MCQE_POOL_PEAK";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGRESS_MCQE_POOL_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_EGRESS_MCQE_POOL_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "EGRESS_MCQE_POOL_CUR";
    /* End Ctrl block for Egress Pool MCQE counter */

    /* Eviction table init */
    SOC_IF_ERROR_RETURN(soc_counter_trident3_eviction_init(unit));

    /* Ctrl block for PERQ_WRED_DROP_PKT_UC */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 10; /* 10 UC queues per port */
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_WRED_DROP_MEMm);
    non_dma0->mem = MMU_CTR_WRED_DROP_MEMm;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "PERQ_WRED_DROP_PKT_UC";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_perq_wred_drop_uc");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_trident3_extra_ctrs_init(unit,
                SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT,
                non_dma0,
                non_dma0->extra_ctrs,
                non_dma0->extra_ctr_ct,
                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                            "ERR: Extra ctrs Init FAILED for id %d\n"),
                        SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT -
                        SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    return SOC_E_NONE;

}

#endif /* BCM_TRIDENT3_SUPPORT */
