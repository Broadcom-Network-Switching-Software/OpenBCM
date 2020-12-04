/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/esw/preemption_cnt.h>

#if defined(BCM_GREYHOUND2_SUPPORT) && defined(BCM_PREEMPTION_SUPPORT)
/* for ingress table ING_RX_COUNTERm :
 * entry 0~127 is for non-preemptable, entry 128~255 is for preemptable packet
 * so the number of entries is half of table
 */
#define IGR_COUNTER_ENTRY_NUM (soc_mem_index_count(unit, ING_RX_COUNTERm) >> 1)
#define EGR_COUNTER_ENTRY_NUM (soc_mem_index_count(unit, EGR_MFRAME_COUNTERm))

/* sw database for ingress counter mode (packet or byte mode)
 * (need not sw database for egress, because always packet mode)
 */
STATIC uint32 *igr_counter_mode_non_preempt[SOC_MAX_NUM_DEVICES];
STATIC uint32 *igr_counter_mode_preempt[SOC_MAX_NUM_DEVICES];

STATIC int
bcmi_gh2_preemption_counter_set(int unit, bcm_port_t local_port,
                                bcm_port_stat_t stat, uint64 val)
{
    int mode = 1;

    if(NULL == igr_counter_mode_non_preempt[unit] ||
       NULL == igr_counter_mode_preempt[unit]) {
       return BCM_E_INIT;
    }

    /* check local_port valid */
    switch (stat) {
        case bcmPortStatIngressPackets:
        case bcmPortStatIngressBytes:
        case bcmPortStatIngressPreemptPackets:
        case bcmPortStatIngressPreemptBytes:
            if (local_port < 0 || local_port >= IGR_COUNTER_ENTRY_NUM) {
                return BCM_E_PARAM;
            }
            break;
        case bcmPortStatEgressPreemptPackets:
        case bcmPortStatEgressPackets:
        case bcmPortStatEgressBytes:
        case bcmPortStatEgressPreemptBytes:
            if (local_port < 0 || local_port >= EGR_COUNTER_ENTRY_NUM) {
                return BCM_E_PARAM;
            }
            break;
    }

    /* set counter */
    switch (stat) {
        case bcmPortStatIngressPackets:
        case bcmPortStatIngressBytes:
            BCM_IF_ERROR_RETURN(
                soc_counter_set(
                    unit, local_port,
                    SOC_COUNTER_NON_DMA_ING_RX_COUNTER_NON_PREEMPTABLE_CNT,
                    0, val));
            break;
        case bcmPortStatIngressPreemptPackets:
        case bcmPortStatIngressPreemptBytes:
            BCM_IF_ERROR_RETURN(
                soc_counter_set(
                    unit, local_port,
                    SOC_COUNTER_NON_DMA_ING_RX_COUNTER_PREEMPTABLE_CNT,
                    0, val));
            break;
        case bcmPortStatEgressPreemptPackets:
            BCM_IF_ERROR_RETURN(
                soc_counter_set(
                    unit, local_port,
                    SOC_COUNTER_NON_DMA_EGR_TX_COUNTER_PREEMPTABLE_CNT,
                    0, val));
            break;
        case bcmPortStatEgressPackets:
        case bcmPortStatEgressBytes:
        case bcmPortStatEgressPreemptBytes:
            /* egress: only support preemptable packet mode in GH2 */
            return BCM_E_UNAVAIL;
    }

    /* update counter mode */
    switch (stat) {
        case bcmPortStatIngressPackets:
            mode = 0;
            /* fall through */
        case bcmPortStatIngressBytes:
            BCM_IF_ERROR_RETURN(
                soc_mem_field32_modify(
                    unit, ING_RX_COUNTERm,
                    local_port,
                    COUNT_BYTEf, mode));
            BCM_IF_ERROR_RETURN(
                soc_mem_field32_modify(
                    unit, ING_RX_COUNTERm,
                    IGR_COUNTER_ENTRY_NUM + local_port,
                    COUNT_BYTEf, mode));
            igr_counter_mode_non_preempt[unit][local_port] = mode;
            igr_counter_mode_preempt[unit][local_port] = mode;
            break;
        case bcmPortStatIngressPreemptPackets:
            mode = 0;
            /* fall through */
        case bcmPortStatIngressPreemptBytes:
            BCM_IF_ERROR_RETURN(
                soc_mem_field32_modify(
                    unit, ING_RX_COUNTERm,
                    IGR_COUNTER_ENTRY_NUM + local_port,
                    COUNT_BYTEf, mode));
            igr_counter_mode_preempt[unit][local_port] = mode;
            break;
        default:
            /* need not update coutner mode for egress in GH2
             *(because always packet mode)
             */
            break;
    }
    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_preemption_counter_get(int unit, int sync_mode, bcm_port_t local_port,
                                bcm_port_stat_t stat, uint64 *val)
{
    uint64 local_val;

    if (NULL == val) {
        return BCM_E_PARAM;
    }
    if(NULL == igr_counter_mode_non_preempt[unit] ||
       NULL == igr_counter_mode_preempt[unit]) {
       return BCM_E_INIT;
    }

    /* check local_port valid */
    switch (stat) {
        case bcmPortStatIngressPackets:
        case bcmPortStatIngressBytes:
        case bcmPortStatIngressPreemptPackets:
        case bcmPortStatIngressPreemptBytes:
            if (local_port < 0 || local_port >= IGR_COUNTER_ENTRY_NUM) {
                return BCM_E_PARAM;
            }
            break;
        case bcmPortStatEgressPreemptPackets:
        case bcmPortStatEgressPackets:
        case bcmPortStatEgressBytes:
        case bcmPortStatEgressPreemptBytes:
            if (local_port < 0 || local_port >= EGR_COUNTER_ENTRY_NUM) {
                return BCM_E_PARAM;
            }
            break;
    }

    /* check counter mode first */
    switch (stat) {
        case bcmPortStatIngressPackets:
            if (0 != igr_counter_mode_non_preempt[unit][local_port]) {
                return BCM_E_PARAM;
            }
            if (0 != igr_counter_mode_preempt[unit][local_port]) {
                return BCM_E_PARAM;
            }
            break;
        case bcmPortStatIngressBytes:
            if (1 != igr_counter_mode_non_preempt[unit][local_port]) {
                return BCM_E_PARAM;
            }
            if (1 != igr_counter_mode_preempt[unit][local_port]) {
                return BCM_E_PARAM;
            }
            break;
        case bcmPortStatIngressPreemptPackets:
            if (0 != igr_counter_mode_preempt[unit][local_port]) {
                return BCM_E_PARAM;
            }
            break;
        case bcmPortStatIngressPreemptBytes:
            if (1 != igr_counter_mode_preempt[unit][local_port]) {
                return BCM_E_PARAM;
            }
            break;
        case bcmPortStatEgressPreemptPackets:
            break;
        case bcmPortStatEgressPackets:
        case bcmPortStatEgressBytes:
        case bcmPortStatEgressPreemptBytes:
            /* egress: only support preemptable packet mode in GH2 */
            return BCM_E_UNAVAIL;
    }

    /* get counter */
    COMPILER_64_ZERO(*val);
    switch (stat) {
        case bcmPortStatIngressPackets:
        case bcmPortStatIngressBytes:
            if (1 == sync_mode) {
                BCM_IF_ERROR_RETURN(
                    soc_counter_sync_get(
                        unit, local_port,
                        SOC_COUNTER_NON_DMA_ING_RX_COUNTER_NON_PREEMPTABLE_CNT,
                        0, &local_val));
            } else {
                BCM_IF_ERROR_RETURN(
                    soc_counter_get(
                        unit, local_port,
                        SOC_COUNTER_NON_DMA_ING_RX_COUNTER_NON_PREEMPTABLE_CNT,
                        0, &local_val));
            }
            COMPILER_64_ADD_64(*val, local_val);
            /* fall through */
        case bcmPortStatIngressPreemptPackets:
        case bcmPortStatIngressPreemptBytes:
            if (1 == sync_mode) {
                BCM_IF_ERROR_RETURN(
                    soc_counter_sync_get(
                        unit, local_port,
                        SOC_COUNTER_NON_DMA_ING_RX_COUNTER_PREEMPTABLE_CNT,
                        0, &local_val));
            } else {
                BCM_IF_ERROR_RETURN(
                    soc_counter_get(
                        unit, local_port,
                        SOC_COUNTER_NON_DMA_ING_RX_COUNTER_PREEMPTABLE_CNT,
                        0, &local_val));
            }
            COMPILER_64_ADD_64(*val, local_val);
            break;
        case bcmPortStatEgressPreemptPackets:
            if (1 == sync_mode) {
                BCM_IF_ERROR_RETURN(
                    soc_counter_sync_get(
                        unit, local_port,
                        SOC_COUNTER_NON_DMA_EGR_TX_COUNTER_PREEMPTABLE_CNT,
                        0, &local_val));
            } else {
                BCM_IF_ERROR_RETURN(
                    soc_counter_get(
                        unit, local_port,
                        SOC_COUNTER_NON_DMA_EGR_TX_COUNTER_PREEMPTABLE_CNT,
                        0, &local_val));
            }
            COMPILER_64_ADD_64(*val, local_val);
            break;
        default:
            return BCM_E_INTERNAL; /* should not enter here */
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_preemption_counter_clean(int unit)
{
    if(NULL != igr_counter_mode_non_preempt[unit]) {
        sal_free(igr_counter_mode_non_preempt[unit]);
        igr_counter_mode_non_preempt[unit] = NULL;
    }
    if(NULL != igr_counter_mode_preempt[unit]) {
        sal_free(igr_counter_mode_preempt[unit]);
        igr_counter_mode_preempt[unit] = NULL;
    }
    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_preemption_counter_init(int unit)
{
    int i;
    uint32 **counter_mode[2];

    /* prepare sw database for ingress counter mode
     * entry 0~127 for non-preempt, 128~255 for preempt
     */
    counter_mode[0] = &(igr_counter_mode_non_preempt[unit]);
    counter_mode[1] = &(igr_counter_mode_preempt[unit]);

    if(NULL != *(counter_mode[0]) || NULL != *(counter_mode[1])) {
        bcmi_gh2_preemption_counter_clean(unit);
    }

    for (i = 0; i < 2; i++) {
        uint8 *dma_buffer = NULL;
        int alloc_size, j, entry_byte, rv;

        /* alloc sw database */
        alloc_size = IGR_COUNTER_ENTRY_NUM * sizeof(uint32);
        *(counter_mode[i]) = sal_alloc(alloc_size, "igr_counter_mode");
        if (NULL == *(counter_mode[i])) {
            bcmi_gh2_preemption_counter_clean(unit);
            return BCM_E_MEMORY;
        }

        /* sync sw database with hw at initial time */
        entry_byte = WORDS2BYTES(soc_mem_entry_words(unit, ING_RX_COUNTERm));
        alloc_size = IGR_COUNTER_ENTRY_NUM * entry_byte;
        dma_buffer = soc_cm_salloc(unit, alloc_size, "dma_buffer");
        if (NULL == dma_buffer) {
            bcmi_gh2_preemption_counter_clean(unit);
            return BCM_E_MEMORY;
        }
        rv = soc_mem_read_range(unit, ING_RX_COUNTERm, MEM_BLOCK_ANY,
                                (IGR_COUNTER_ENTRY_NUM * i),
                                (IGR_COUNTER_ENTRY_NUM * (i+1)) -1,
                                dma_buffer);
        if (SOC_FAILURE(rv)) {
            bcmi_gh2_preemption_counter_clean(unit);
            soc_cm_sfree(unit, dma_buffer);
            return BCM_E_MEMORY;
        }
        for (j = 0; j < IGR_COUNTER_ENTRY_NUM; j++) {
            uint32 mode;

            mode = soc_mem_field32_get(unit, ING_RX_COUNTERm,
                                       &dma_buffer[j * entry_byte],
                                       COUNT_BYTEf);
            (*(counter_mode[i]))[j] = mode;
        }
        soc_cm_sfree(unit, dma_buffer);
    }

    return BCM_E_NONE;
}

int
bcmi_gh2_preemption_counter_dev_func_init(
    bcmi_preemption_counte_func_t* dev_func)
{
    dev_func->preemption_counter_init = bcmi_gh2_preemption_counter_init;
    dev_func->preemption_counter_clean = bcmi_gh2_preemption_counter_clean;
    dev_func->preemption_counter_get = bcmi_gh2_preemption_counter_get;
    dev_func->preemption_counter_set = bcmi_gh2_preemption_counter_set;
    return BCM_E_NONE;
}
#endif /* BCM_GREYHOUND2_SUPPORT && defined(BCM_PREEMPTION_SUPPORT) */
