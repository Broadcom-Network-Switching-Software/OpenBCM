/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FB6 counter module routines.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/firebolt6.h>

#ifdef BCM_FIREBOLT6_SUPPORT

STATIC soc_mem_t _fb6_ch_ctr_mems[] = {
    /* Align with definitions for SOC_COUNTER_NON_DMA_CES_xx */
    INVALIDm, /* SOC_COUNTER_NON_DMA_CES_START */
    CIMRP4m,
    CIMRP6m,
    CING_ECN_COUNTERm,
    CING_NIV_RX_FRAMES_ERROR_DROPm,
    CING_NIV_RX_FRAMES_FORWARDING_DROPm,
    CING_NIV_RX_FRAMES_VLAN_TAGGEDm,
    CRDBGC0m,
    CRDBGC1m,
    CRDBGC10m,
    CRDBGC11m,
    CRDBGC12m,
    CRDBGC13m,
    CRDBGC14m,
    CRDBGC15m,
    CRDBGC2m,
    CRDBGC3m,
    CRDBGC4m,
    CRDBGC5m,
    CRDBGC6m,
    CRDBGC7m,
    CRDBGC8m,
    CRDBGC9m,
    CRIPC4m,
    CRIPC6m,
    CRIPD4m,
    CRIPD6m,
    CRIPHE4m,
    CRIPHE6m,
    CRPARITYDm,
    CRPORTDm,
    CRTUNm,
    CRUCm,
    CEGR_ECN_COUNTERm,
    CTDBGC0m,
    CTDBGC1m,
    CTDBGC10m,
    CTDBGC11m,
    CTDBGC12m,
    CTDBGC13m,
    CTDBGC14m,
    CTDBGC15m,
    CTDBGC2m,
    CTDBGC3m,
    CTDBGC4m,
    CTDBGC5m,
    CTDBGC6m,
    CTDBGC7m,
    CTDBGC8m,
    CTDBGC9m,
    CTPCEm,
    INVALIDm, /* SOC_COUNTER_NON_DMA_CES_END */
};

int
soc_counter_firebolt6_generic_get_info(int unit, soc_ctr_control_info_t
                                       ctrl_info, soc_reg_t id, int* base_index,
                                       int* num_entries)
{
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
    pipe = 0;
    switch (ctrl_info.instance_type) {
        case SOC_CTR_INSTANCE_TYPE_PORT:
            port = ctrl_info.instance;
            return soc_counter_firebolt6_get_info(unit, port, id,
                                                  base_index, num_entries);
            break;
        case SOC_CTR_INSTANCE_TYPE_XPE_PORT:
            port =  (ctrl_info.instance & 0xFFC000) >> 14;
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
            pipe = 0;

            switch (id) {
                case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
                case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
                case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
                case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
                case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
                case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
                case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
                case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
                    return soc_counter_firebolt6_get_info(unit, port, id,
                                                       base_index, num_entries);
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
                        *base_index += mmu_port;
                    } else {
                        *base_index += pipe_offset +
                                        (mmu_port & SOC_FB6_MMU_PORT_STRIDE);
                    }
                    *base_index *= _FB6_MMU_NUM_PG;
                    break;
                case SOC_COUNTER_NON_DMA_QUEUE_PEAK:
                case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
                case SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK:
                case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
                    if ((id == SOC_COUNTER_NON_DMA_QUEUE_PEAK) ||
                            (id == SOC_COUNTER_NON_DMA_QUEUE_CURRENT)) {
                        *base_index = si->port_cosq_base[port];
                        *num_entries = si->port_num_cosq[port];
                    } else {
                        *base_index = si->port_uc_cosq_base[port];
                        *num_entries = si->port_num_uc_cosq[port];
                    }
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
        case SOC_CTR_INSTANCE_TYPE_FT_GROUP:
           *num_entries = non_dma->num_entries;
            break;
        default:
            return SOC_E_PARAM;
    }
    *base_index += non_dma->base_index;
    return SOC_E_NONE;
}

int
soc_counter_firebolt6_get_info(int unit, soc_port_t port, soc_reg_t id,
                               int *base_index, int *num_entries)
{
    soc_info_t *si;
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    soc_port_t phy_port, mmu_port;
    int pipe, pipe_offset, obm_id = 0, lane = 0;
    uint8 c_offset = 0; /* child offset in Parent-child model */

    soc = SOC_CONTROL(unit);

    if (id >= NUM_SOC_REG) {
        if (id >= SOC_COUNTER_NON_DMA_END) {
            return SOC_E_PARAM;
        }
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
        return SOC_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];
    pipe = si->port_pipe[port];

    switch (id) {
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
        *base_index  = si->port_cosq_base[port] + SOC_FB6_NUM_UC_QUEUES_PER_PIPE;
        *num_entries = si->port_num_cosq[port];
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC:
        *base_index = si->port_uc_cosq_base[port];
        *num_entries = si->port_num_uc_cosq[port];
        break;
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
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
        *base_index = mmu_port;
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
        if (mmu_port < 72) {
            *base_index = mmu_port;
            *num_entries = 1;
        } else {
            return SOC_E_INTERNAL;
        }
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
        if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
            IS_FAE_PORT(unit, port)) {
            return SOC_E_PARAM;
        }
        *base_index = 0;
        *num_entries = 1;
        /* find obm from the port number */
        SOC_IF_ERROR_RETURN
            (soc_fb6_port_obm_info_get(unit, port, &obm_id, &lane));
        if (obm_id >= _FB6_OBMS_PER_PIPE) {
            return SOC_E_PARAM;
        }
        soc_counter_trident3_get_child_dma_by_idx(unit, non_dma,
                                                  obm_id, &non_dma);
        *base_index += lane;
        break;
    default:
        if (id > SOC_COUNTER_NON_DMA_CES_START && id < SOC_COUNTER_NON_DMA_CES_END) {
            uint32 rval;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, MTRO_PORT_L0_MAPPINGr,
                                              port, 0,  &rval));
            *base_index = soc_reg_field_get(unit, MTRO_PORT_L0_MAPPINGr, rval, START_L0_NUMf);
            *num_entries = si->port_num_subport[port];
        } else {
            return SOC_E_INTERNAL;
        }
    }

    *base_index += non_dma->base_index;
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
soc_counter_firebolt6_non_dma_init(int unit, int nports,
                                   int non_dma_start_index,
                                   int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1, *non_dma2, *temp;
    int rv = SOC_E_NONE;
    int num_entries, alloc_size, entry_words, pipe, num_extra_ctr_entries;
    int pool_count = 0, i;
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
    soc_ctr_ctrl->port_obm_info_get = soc_fb6_port_obm_info_get;

    /* EFP_COUNTER_TABLEm is the largest table to be DMA'ed */
    num_entries = soc_mem_index_count(unit, EFP_COUNTER_TABLEm);
    entry_words = soc_mem_entry_words(unit, EFP_COUNTER_TABLEm);
    alloc_size = NUM_PIPE(unit) * num_entries * sizeof(uint32) * entry_words;

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);
    pipe = 0;

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
    non_dma0->entries_per_port = FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX *
                                 SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "MC_PERQ_PKT";
    num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    entry_words = soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm);

    non_dma0->dma_mem[pipe] = SOC_MEM_UNIQUE_ACC(unit, non_dma0->mem)[pipe];
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

    *non_dma_entries += non_dma0->num_entries;
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma1->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;
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
    non_dma0->entries_per_port = FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX *
                                 SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
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

    non_dma0->dma_mem[pipe] =  SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

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
    non_dma0->entries_per_port = FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX *
                                 SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
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

    non_dma0->dma_mem[pipe] = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

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

    non_dma0->dma_mem[pipe] = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

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
    num_entries = 72; /* 1 per port, 72 per pipe */
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);

    non_dma0->dma_mem[pipe] = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_min[pipe] = 0;
    non_dma0->dma_index_max[pipe] = 71;
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

    non_dma1->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma1->dma_index_min[pipe] = 72;
    non_dma1->dma_index_max[pipe] = 143;
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

    non_dma1->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma1->dma_index_min[pipe] = 144;
    non_dma1->dma_index_max[pipe] = 215;
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

    non_dma1->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma1->dma_index_min[pipe] = 216;
    non_dma1->dma_index_max[pipe] = 287;
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

    non_dma1->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma1->dma_index_min[pipe] = 288;
    non_dma1->dma_index_max[pipe] = 359;
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
    non_dma0->entries_per_port = FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX *
                                 SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
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
    non_dma0->dma_mem[pipe] = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

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
    non_dma0->entries_per_port = FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX *
                                 SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
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
    non_dma0->dma_mem[pipe] = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

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

    non_dma0->dma_mem[pipe] = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
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

    non_dma0->dma_mem[pipe] = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, pipe);
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
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
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
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
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

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
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];

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

    non_dma0->dma_mem[pipe] = SOC_MEM_UNIQUE_ACC(unit, non_dma0->mem)[pipe];
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

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

    num_entries = soc_mem_index_count(unit, SFLOW_ING_DATA_SOURCEm);
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

    num_entries = soc_mem_index_count(unit, SFLOW_ING_FLEX_DATA_SOURCEm);
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
    /* Use Parent-Child model to cover all 10 OBMs */
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->entries_per_port = 4;
    non_dma0->num_entries = 4 * NUM_PIPE(unit);
    non_dma0->mem = INVALIDm;
    non_dma0->field = COUNTERf;
    non_dma0->dma_buf[0] = &buf[0];

    /*
     * Below represents how Extra cntrs are arranged
     * for OBM 0-16 Drop Counters.
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
    non_dma0->extra_ctr_ct = _FB6_OBMS_PER_PIPE; /* 10 OBMs per Pipe */
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

    /* Flowtracker Group Counters - Control Block */
    /* GROUP_FLOW_EXCEED_COUNT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_NUM_EXCEEDED_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_NUM_EXCEEDED_CNT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    non_dma0->mem = BSC_KG_FLOW_EXCEED_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = FLOW_COUNTf;
    non_dma0->cname = "GROUP_FLOW_EXCEED_COUNT";

    num_entries = soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    entry_words = soc_mem_entry_words(unit, BSC_KG_GROUP_TABLEm);

    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->dma_mem[pipe] =  BSC_KG_FLOW_EXCEED_COUNTERm;
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

    *non_dma_entries += non_dma0->num_entries;

    /* GROUP_FLOW_LEARNED_COUNT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_LEARNT_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_LEARNT_CNT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    non_dma0->mem = BSC_KG_FLOW_LEARNED_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = FLOW_COUNTf;
    non_dma0->cname = "GROUP_FLOW_LEARNED_COUNT";

    num_entries = soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    entry_words = soc_mem_entry_words(unit, BSC_KG_GROUP_TABLEm);

    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->dma_mem[pipe] = BSC_KG_FLOW_LEARNED_COUNTERm;
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

    *non_dma_entries += non_dma0->num_entries;


    /* GROUP_AGE_OUT_COUNT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_AGE_OUT_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_AGE_OUT_CNT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    non_dma0->mem = BSC_KG_FLOW_AGE_OUT_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = FLOW_COUNTf;
    non_dma0->cname = "GROUP_AGE_OUT_COUNT";

    num_entries = soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    entry_words = soc_mem_entry_words(unit, BSC_KG_GROUP_TABLEm);

    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->dma_mem[pipe] = BSC_KG_FLOW_AGE_OUT_COUNTERm;
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

    *non_dma_entries += non_dma0->num_entries;

    /* GROUP_FLOW_MISSED_COUNT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_MISSED_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_MISSED_CNT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    non_dma0->mem = BSC_KG_FLOW_MISSED_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = FLOW_COUNTf;
    non_dma0->cname = "GROUP_FLOW_MISSED_COUNT";

    num_entries = soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    entry_words = soc_mem_entry_words(unit, BSC_KG_GROUP_TABLEm);

    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->dma_mem[pipe] = BSC_KG_FLOW_MISSED_COUNTERm;
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

    *non_dma_entries += non_dma0->num_entries;

    /* GROUP_METER_EXCEED_COUNT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_METER_EXCEEDED_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_METER_EXCEEDED_CNT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, BSC_DG_GROUP_TABLEm);
    non_dma0->mem = BSC_DG_FLOW_METER_EXCEED_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = FLOW_COUNTf;
    non_dma0->cname = "GROUP_METER_EXCEED_COUNT";

    num_entries = soc_mem_index_count(unit, BSC_DG_GROUP_TABLEm);
    entry_words = soc_mem_entry_words(unit, BSC_DG_GROUP_TABLEm);

    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->dma_mem[pipe] = BSC_DG_FLOW_METER_EXCEED_COUNTERm;
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

    *non_dma_entries += non_dma0->num_entries;

    /* GROUP_FLOW_START_COUNT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_START_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_START_CNT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    non_dma0->mem = BSC_DG_FLOW_OPAQUE_COUNTER_0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = FLOW_COUNTf;
    non_dma0->cname = "GROUP_FLOW_START_COUNT";

    num_entries = soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    entry_words = soc_mem_entry_words(unit, BSC_KG_GROUP_TABLEm);

    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->dma_mem[pipe] =  BSC_DG_FLOW_OPAQUE_COUNTER_0m;
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

    *non_dma_entries += non_dma0->num_entries;

    /* GROUP_FLOW_END_COUNT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_END_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_END_CNT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    non_dma0->mem = BSC_DG_FLOW_OPAQUE_COUNTER_1m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = FLOW_COUNTf;
    non_dma0->cname = "GROUP_FLOW_END_COUNT";

    num_entries = soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);
    entry_words = soc_mem_entry_words(unit, BSC_KG_GROUP_TABLEm);

    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;
    non_dma0->dma_mem[pipe] =  BSC_DG_FLOW_OPAQUE_COUNTER_1m;
    non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);

    *non_dma_entries += non_dma0->num_entries;

    for (i = 1; i + SOC_COUNTER_NON_DMA_CES_START < SOC_COUNTER_NON_DMA_CES_END; i++) {
        non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_CES_START + i -
                                         SOC_COUNTER_NON_DMA_START];
        non_dma0->id = SOC_COUNTER_NON_DMA_CES_START + i;

        non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                          _SOC_COUNTER_NON_DMA_DO_DMA;

        SOC_PBMP_CLEAR(non_dma0->pbmp);
        num_entries = soc_mem_index_count(unit, _fb6_ch_ctr_mems[i]);
        entry_words = soc_mem_entry_words(unit, _fb6_ch_ctr_mems[i]);

        non_dma0->base_index = non_dma_start_index + *non_dma_entries;
        non_dma0->num_entries = num_entries;
        non_dma0->entries_per_port =  FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX;
        non_dma0->mem = _fb6_ch_ctr_mems[i];
        non_dma0->reg = INVALIDr;
        non_dma0->field = COUNTf;
#if !defined(SOC_NO_NAMES)
        non_dma0->cname = soc_mem_name[_fb6_ch_ctr_mems[i]];
#endif

        non_dma0->pbmp = SOC_INFO(unit).subtag_allowed_pbm;


        non_dma0->dma_mem[pipe] = _fb6_ch_ctr_mems[i];
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words];
        non_dma0->dma_index_max[pipe] = soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);


        *non_dma_entries += non_dma0->num_entries;
    }

    /* Eviction table init */
    SOC_IF_ERROR_RETURN(soc_counter_trident3_eviction_init(unit));

    return SOC_E_NONE;
}

#endif /* BCM_FIREBOLT6_SUPPORT */
