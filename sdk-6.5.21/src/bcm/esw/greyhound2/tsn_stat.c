/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:
 * Manage chip specific functionality for TSN Statistics implementation on GH2.
 */

#include <sal/core/libc.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/tsn.h>
#include <bcm_int/esw/tsn_stat.h>
#include <bcm_int/esw/tsn.h>
#include <bcm_int/esw/tsn_taf.h>
#include <soc/profile_mem.h>
#include <shared/bsl.h>
#include <shared/shr_res_bitmap.h>

#if defined(BCM_GREYHOUND2_SUPPORT) && defined(BCM_TSN_SUPPORT)
/* TSN stat portcnt table contents for GH2 */
STATIC const tsn_stat_portcnt_table_info_t portcnt_table[] =
{
    {bcmTsnStatIngressSrTaggedPackets,
        SR_PORT_COUNT_RX_TAGGEDm, COUNTf},
    {bcmTsnStatIngressNonLinkLocalPackets,
        SR_PORT_COUNT_RXm, COUNTf},
    {bcmTsnStatIngressSrWrongLanPackets,
        SR_PORT_COUNT_RX_WRONG_LANm, COUNTf},
    {bcmTsnStatIngressSrRxErrorPackets,
        SR_PORT_COUNT_RX_ERRORm, COUNTf},
    {bcmTsnStatIngressSrTagErrorPackets,
        SR_PORT_COUNT_RX_TAG_ERRORm, COUNTf},
    {bcmTsnStatIngressSrDuplicatePackets,
        SR_PORT_COUNT_RX_DUPLICATESm, COUNTf},
    {bcmTsnStatIngressSrOutOfOrderSrPackets,
        SR_PORT_COUNT_RX_OUTOFORDERm, COUNTf},
    {bcmTsnStatIngressSrOwnRxPackets,
        SR_PORT_COUNT_OWN_RXm, COUNTf},
    {bcmTsnStatIngressSrUnexpectedPackets,
        SR_PORT_COUNT_UNEXPECTED_FRAMEm, COUNTf},
    {bcmTsnStatIngressMtuErrorPackets,
        SR_PORT_MTU_ERRORm, COUNTf},
    {bcmTsnStatIngressStuErrorPackets,
        SR_PORT_STU_ERRORm, COUNTf},
    {bcmTsnStatIngressSrAcceptedSrPackets,
        SR_PORT_COUNT_RX_PASSEDm, COUNTf},
    {bcmTsnStatIngressSrSaSrcFilteredPackets,
        SR_PORT_PROXY_MAC_ERRORm, COUNTf},
    {bcmTsnStatEgressSrTaggedPackets,
        SR_PORT_COUNT_TX_TAGGEDm, COUNTf},
    {bcmTsnStatEgressNonLinkLocalPackets,
        SR_PORT_COUNT_TXm, COUNTf},
    {bcmTsnStatEgressMtuErrorPackets,
        SR_PORT_TX_MTU_ERRORm, COUNTf},
    {bcmTsnStatEgressStuErrorPackets,
        SR_PORT_TX_STU_ERRORm, COUNTf}
};

#define GH2_TSN_STAT_PORTCNT_TABLE_SIZE  (COUNTOF(portcnt_table))

/* TSN stat flowcnt table contents for GH2 */
STATIC const soc_mem_t flowcnt_group_mem[bcmTsnStatGroupTypeCount] =
                           {SR_ING_COUNTER_PROFILEm,
                            SR_EGR_COUNTER_PROFILEm};
STATIC const uint32 flowcnt_group_num[bcmTsnStatGroupTypeCount] = {8, 8};
STATIC const soc_mem_t flowcnt_flow_mem[bcmTsnStatGroupTypeCount] =
                           {SR_FLOW_ING_COUNTER_CONTROLm,
                            SR_FLOW_EGR_COUNTER_CONTROLm};
STATIC const uint32 flowcnt_flow_num[bcmTsnStatGroupTypeCount] = {8192, 8192};
#ifdef BCM_FIRELIGHT_SUPPORT
STATIC const uint32 flowcnt_flow_num_fl[bcmTsnStatGroupTypeCount] = {512, 512};
#endif /* BCM_FIRELIGHT_SUPPORT */

STATIC const soc_field_t flowcnt_flow_profile_field[bcmTsnStatGroupTypeCount] =
                             {SR_ING_COUNTER_PROFILEf,
                              SR_EGR_COUNTER_PROFILEf};
STATIC const soc_mem_t flowcnt_pool_mem[bcmTsnStatGroupTypeCount]
                                       [bcmTsnStatCount] =
                           {{SR_FLOW_COUNT_POOL0m, SR_FLOW_COUNT_POOL1m,
                             SR_FLOW_COUNT_POOL2m, SR_FLOW_COUNT_POOL3m,
                             SR_FLOW_COUNT_POOL4m, SR_FLOW_COUNT_POOL5m,
                             SR_FLOW_COUNT_POOL6m, SR_FLOW_COUNT_POOL7m},
                            {EGR_SR_FLOW_COUNT_POOL0m,
                             EGR_SR_FLOW_COUNT_POOL1m}};

STATIC const uint32 flowcnt_pool_num[bcmTsnStatGroupTypeCount] = {8, 2};
STATIC uint32 flowcnt_enable_value_mapping_cnt[SOC_MAX_NUM_DEVICES]
                                              [bcmTsnStatGroupTypeCount];
STATIC const bcm_tsn_stat_t flowcnt_enable_value_mapping
                                [bcmTsnStatGroupTypeCount][bcmTsnStatCount] =
                                    {{bcmTsnStatIngressSrSaSrcFilteredPackets,
                                      bcmTsnStatIngressSrAcceptedSrPackets,
                                      bcmTsnStatCount /* bit 2: reserved  */ ,
                                      bcmTsnStatIngressStuErrorPackets,
                                      bcmTsnStatIngressMtuErrorPackets,
                                      bcmTsnStatIngressSrUnexpectedPackets,
                                      bcmTsnStatIngressSrOwnRxPackets,
                                      bcmTsnStatIngressSrOutOfOrderSrPackets,
                                      bcmTsnStatCount/* bit 8: reserved */,
                                      bcmTsnStatIngressSrDuplicatePackets,
                                      bcmTsnStatIngressSrTagErrorPackets,
                                      bcmTsnStatIngressSrRxErrorPackets,
                                      bcmTsnStatIngressSrWrongLanPackets,
                                      bcmTsnStatIngressNonLinkLocalPackets,
                                      bcmTsnStatIngressSrTaggedPackets,
                                      bcmTsnStatIngressTafMtuPass,
                                      bcmTsnStatIngressTafGatePass,
                                      bcmTsnStatIngressTafGateCloseDrop,
                                      bcmTsnStatIngressTafGateNoByteDrop,
                                      bcmTsnStatIngressTafMeterDrop},
                                     {bcmTsnStatEgressStuErrorPackets,
                                      bcmTsnStatEgressMtuErrorPackets,
                                      bcmTsnStatEgressNonLinkLocalPackets,
                                      bcmTsnStatEgressSrTaggedPackets}};
/*
 * sw database for flowcnt : counter
 */
STATIC uint64 *flowcnt_counter_sw[SOC_MAX_NUM_DEVICES]
                                 [bcmTsnStatGroupTypeCount][bcmTsnStatCount];
STATIC uint64 *flowcnt_counter_hw[SOC_MAX_NUM_DEVICES]
                                 [bcmTsnStatGroupTypeCount][bcmTsnStatCount];

STATIC uint8 *flowcnt_counter_dma_buffer[SOC_MAX_NUM_DEVICES]
                                        [bcmTsnStatGroupTypeCount];
STATIC uint64 flowcnt_counter_max[SOC_MAX_NUM_DEVICES]
                                 [bcmTsnStatGroupTypeCount][bcmTsnStatCount];
STATIC uint64 flowcnt_counter_mask[SOC_MAX_NUM_DEVICES]
                                  [bcmTsnStatGroupTypeCount][bcmTsnStatCount];

/*
 * sw database for flowcnt : group
 */
STATIC soc_profile_mem_t *flowcnt_group_mem_profile[SOC_MAX_NUM_DEVICES]
                                                   [bcmTsnStatGroupTypeCount];
/* for each group, mapping between statType and pool_id */
STATIC int32 **flowcnt_group_stat2pool[SOC_MAX_NUM_DEVICES]
                                      [bcmTsnStatGroupTypeCount];

/* the relationship between "statType" and "flow_id bitmap"
 *  ex. Given a specific statType,
 *      1. we can traverse and find which groupId contains this statType
 *      2. Then we can get flow_id bitmap attached to this groupId
 *        (1. is impacted when the group's enable_value changes)
 *        (2. is impacted when flow attach/detach the group)
 *
 *  If we want to keep the cache between "statType" and "flow_id bitmap"
 *  we need to update the cache value when event 1 or 2 occurs.
 *  However before the user wants to change group's enable_value,
 *  he/she needs to detach all flows that attached to this group first.
 *  According to this assumption, we only need to care about event 2.
 */
STATIC SHR_BITDCL *flowcnt_stat_fbmp_cache[SOC_MAX_NUM_DEVICES]
                                          [bcmTsnStatGroupTypeCount]
                                          [bcmTsnStatCount];

/*
 * sw database for flowcnt : pool
 */
STATIC int32 flowcnt_pool_bitmap_alloc_min[SOC_MAX_NUM_DEVICES]
                                          [bcmTsnStatGroupTypeCount];
STATIC int32 flowcnt_pool_bitmap_alloc_max[SOC_MAX_NUM_DEVICES]
                                          [bcmTsnStatGroupTypeCount];
STATIC shr_res_bitmap_handle_t flowcnt_pool_bitmap[SOC_MAX_NUM_DEVICES]
                                                  [bcmTsnStatGroupTypeCount];
STATIC uint8 *flowcnt_mem_entry_buf[SOC_MAX_NUM_DEVICES];


typedef int (*bcmi_gh2_tsn_stat_flowcnt_group_traverse_cb)(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id,
    void *user_data);

typedef struct bcmi_gh2_tsn_stat_flowcnt_group_traverse_userdata_s {
    bcm_tsn_stat_group_traverse_cb cb;
    void *cb_user_data;
} bcmi_gh2_tsn_stat_flowcnt_group_traverse_userdata_t;

/* helper macro */
#define BCMI_TSN_FLOWCNT_MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define BCMI_TSN_FLOWCNT_STAT_ID_EGRESS_BIT (20)
#define ERROR_RETURN_WITH_EXTRA_CLEAN(_op_, _extra_clean_) \
        do {                                               \
            int _rv_ = (_op_);                             \
            if (BCM_FAILURE(_rv_)) {                       \
                (_extra_clean_);                           \
                return (_rv_);                             \
            }                                              \
        } while (0)

#define INIT_ERROR_RETURN_WITH_CLEAN(_op_)              \
        do {                                            \
            int _rv_ = (_op_);                          \
            if (BCM_FAILURE(_rv_)) {                    \
                bcmi_gh2_tsn_stat_flowcnt_cleanup(unit);\
                return (_rv_);                          \
            }                                           \
        } while (0)

#define INIT_ERROR_RETURN_WITH_EXTRA_CLEAN(_op_, _extra_clean_)  \
        do {                                                     \
            int _rv_ = (_op_);                                   \
            if (BCM_FAILURE(_rv_)) {                             \
                (_extra_clean_);                                 \
                bcmi_gh2_tsn_stat_flowcnt_cleanup(unit);         \
                return (_rv_);                                   \
            }                                                    \
        } while (0)

#define WRITE_PTR(__ptr__, __type__, __data__)    \
        do {                                      \
          *((__type__*)(__ptr__)) = (__data__);   \
          (__ptr__) += sizeof(__type__);          \
        } while (0)

#define READ_PTR(__ptr__, __type__, __dest__)     \
        do {                                      \
          (__dest__) = *((__type__*)(__ptr__));   \
          (__ptr__) += sizeof(__type__);          \
        } while (0)


/**************** TSN Stat Threshold ****************/

/*
 * TSN Stat Threshold Select Structure on GH2
 */
typedef struct bcmi_gh2_tsn_stat_threshold_select_s {
    bcm_tsn_stat_t stat_type;      /* TSN Statistics type */
    soc_reg_t reg;                 /* Counter Threshold register */
    soc_field_t threshold_field;   /* Counter threshold field */
    soc_field_t interrupt_field;   /* Interrupt field */
    soc_field_t copy_to_cpu_field; /* Copy to cpu field */
} bcmi_gh2_tsn_stat_threshold_select_t;

/*
 * TSN Stat Threshold Select info structure for Source Port on GH2
 */
STATIC const bcmi_gh2_tsn_stat_threshold_select_t
    gh2_port_count_threshold_info[] =
{
    {bcmTsnStatIngressSrTaggedPackets,
        SR_PORT_COUNT_THRESHOLD_RX_TAGGEDr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressNonLinkLocalPackets,
        SR_PORT_COUNT_THRESHOLD_RXr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrWrongLanPackets,
        SR_PORT_COUNT_THRESHOLD_RX_WRONG_LANr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrRxErrorPackets,
        SR_PORT_COUNT_THRESHOLD_RX_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrTagErrorPackets,
        SR_PORT_COUNT_THRESHOLD_RX_TAG_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatIngressSrDuplicatePackets,
        SR_PORT_COUNT_THRESHOLD_RX_DUPLICATESr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrOutOfOrderSrPackets,
        SR_PORT_COUNT_THRESHOLD_RX_OUTOFORDERr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrOwnRxPackets,
        SR_PORT_COUNT_THRESHOLD_OWN_RXr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrUnexpectedPackets,
        SR_PORT_COUNT_THRESHOLD_UNEXPECTED_FRAMEr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressMtuErrorPackets,
        SR_PORT_THRESHOLD_MTU_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatIngressStuErrorPackets,
        SR_PORT_THRESHOLD_STU_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatIngressSrAcceptedSrPackets,
        SR_PORT_COUNT_THRESHOLD_RX_PASSEDr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrSaSrcFilteredPackets,
        SR_PORT_COUNT_THRESHOLD_PROXY_MAC_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatEgressSrTaggedPackets,
        SR_PORT_COUNT_THRESHOLD_TX_TAGGEDr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatEgressNonLinkLocalPackets,
        SR_PORT_COUNT_THRESHOLD_TXr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatEgressMtuErrorPackets,
        SR_PORT_THRESHOLD_TX_MTU_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatEgressStuErrorPackets,
        SR_PORT_THRESHOLD_TX_STU_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf}
};

/*
 * TSN Stat Threshold Select info structure for Source SR Flow on GH2
 */
STATIC const bcmi_gh2_tsn_stat_threshold_select_t
    gh2_flow_count_threshold_info[] =
{
    {bcmTsnStatIngressSrTaggedPackets,
        SR_FLOW_COUNT_THRESHOLD_RX_TAGGEDr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressNonLinkLocalPackets,
        SR_FLOW_COUNT_THRESHOLD_RXr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrWrongLanPackets,
        SR_FLOW_COUNT_THRESHOLD_RX_WRONG_LANr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrRxErrorPackets,
        SR_FLOW_COUNT_THRESHOLD_RX_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrTagErrorPackets,
        SR_FLOW_COUNT_THRESHOLD_RX_TAG_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatIngressSrDuplicatePackets,
        SR_FLOW_COUNT_THRESHOLD_RX_DUPLICATESr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrOutOfOrderSrPackets,
        SR_FLOW_COUNT_THRESHOLD_RX_OUTOFORDERr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrOwnRxPackets,
        SR_FLOW_COUNT_THRESHOLD_OWN_RXr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrUnexpectedPackets,
        SR_FLOW_COUNT_THRESHOLD_UNEXPECTED_FRAMEr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressMtuErrorPackets,
        SR_FLOW_THRESHOLD_MTU_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatIngressStuErrorPackets,
        SR_FLOW_THRESHOLD_STU_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatIngressSrAcceptedSrPackets,
        SR_FLOW_COUNT_THRESHOLD_RX_PASSEDr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatIngressSrSaSrcFilteredPackets,
        SR_FLOW_THRESHOLD_PROXY_MAC_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, COPY_TO_CPUf},
    {bcmTsnStatEgressSrTaggedPackets,
        SR_FLOW_COUNT_THRESHOLD_TX_TAGGEDr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatEgressNonLinkLocalPackets,
        SR_FLOW_COUNT_THRESHOLD_TXr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatEgressMtuErrorPackets,
        SR_FLOW_THRESHOLD_TX_MTU_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf},
    {bcmTsnStatEgressStuErrorPackets,
        SR_FLOW_THRESHOLD_TX_STU_ERRORr, COUNTER_THRESHOLDf,
        INTERRUPT_ENABLEf, INVALIDf}
};

/* TSN Stat threshold default value for initialization */
#define GH2_TSN_STAT_THRESHOLD_DEFAULT_VALUE  0xffffffff

/* TSN Stat threshold valid check for SR */
#define GH2_TSN_STAT_THRESHOLD_SR_IS_VALID(unit, type)      \
    (soc_feature(unit, soc_feature_tsn_sr) &&               \
    ((type >= bcmTsnStatIngressSrTaggedPackets &&           \
    type <= bcmTsnStatIngressSrUnexpectedPackets) ||        \
    (type >= bcmTsnStatIngressSrAcceptedSrPackets &&        \
    type <= bcmTsnStatEgressNonLinkLocalPackets)))

/* TSN Stat threshold valid check for MTU/STU */
#define GH2_TSN_STAT_THRESHOLD_MTU_STU_IS_VALID(unit, type) \
    (soc_feature(unit, soc_feature_tsn_mtu_stu) &&          \
    ((type >= bcmTsnStatIngressMtuErrorPackets &&           \
    type <= bcmTsnStatIngressStuErrorPackets) ||            \
    (type >= bcmTsnStatEgressMtuErrorPackets &&             \
    type <= bcmTsnStatEgressStuErrorPackets)))

/* TSN Stat threshold valid check */
#define GH2_TSN_STAT_THRESHOLD_IS_VALID(unit, type)         \
    (GH2_TSN_STAT_THRESHOLD_SR_IS_VALID(unit, type) ||      \
    GH2_TSN_STAT_THRESHOLD_MTU_STU_IS_VALID(unit, type))

/************** End of TSN Stat Threshold **************/

/* check parameter valid or not */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_valid_check(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    const uint32 *group_id,
    const uint32 *group_enable_values)
{
    soc_mem_t mem;

    if (group_type < 0 || group_type >= bcmTsnStatGroupTypeCount) {
        return BCM_E_PARAM;
    }

    mem = flowcnt_group_mem[group_type];
    /* check group_id valid */
    if (NULL != group_id) {
        if (!soc_mem_index_valid(unit, mem, *group_id)) {
            return BCM_E_PARAM;
        }
    }

    /* check group_enable_values valid */
    if (NULL != group_enable_values) {
        if (0 == *group_enable_values) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(soc_mem_field32_fit(unit, mem,
                                                SR_COUNTER_ENABLESf,
                                                *group_enable_values));
    }
    return BCM_E_NONE;
}

/* update sw cache : mapping statType to poolId in for some specific group */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_update_stat2pool(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id,
    uint32 group_enable_values)
{
    int i, pool_index, enable_values_count_max;
    const bcm_tsn_stat_t *enable_values_mapping;

    assert(group_type >= 0 && group_type < bcmTsnStatGroupTypeCount);
    assert(group_id < flowcnt_group_num[group_type]);

    /* init flowcnt_group_stat2pool as all invalid first */
    for (i = 0; i < bcmTsnStatCount; i++) {
        flowcnt_group_stat2pool[unit][group_type][group_id][i] = -1;
    }

    enable_values_mapping = flowcnt_enable_value_mapping[group_type];
    enable_values_count_max = flowcnt_enable_value_mapping_cnt[unit]
                                                              [group_type];

    /* starting from MSB, the first bit that is set to 1 corresponds
     * to pool 0, the next bit set to 1 corresponds to pool 1, etc
     */
    pool_index = 0;
    for (i = enable_values_count_max - 1; i >= 0; i--) {
        if (group_enable_values & (1 << i)) {
            if (bcmTsnStatCount == enable_values_mapping[i]) {
                /* should not happen */
                return BCM_E_INTERNAL;
            }
            flowcnt_group_stat2pool
                [unit][group_type][group_id]
                [enable_values_mapping[i]] = pool_index;
            pool_index++;
        }
    }

    return BCM_E_NONE;
}

/* get enable value for a given (group_type+group_id) */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_enable_value_get(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id,
    uint32 *ret_enable_values)
{
    soc_mem_t mem;
    soc_profile_mem_t *profile_mem;
    void *profile_mem_ptr;

    if (NULL == ret_enable_values) {
        return BCM_E_PARAM;
    }
    if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == group_id) {
        /* should not query reserved group's enable value */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    &group_id, NULL));

    profile_mem = flowcnt_group_mem_profile[unit][group_type];
    mem = flowcnt_group_mem[group_type];

    profile_mem_ptr = SOC_PROFILE_MEM_ENTRY(unit, profile_mem,
                                            void *, group_id);
    *ret_enable_values = soc_mem_field32_get(unit, mem, profile_mem_ptr,
                                             SR_COUNTER_ENABLESf);

    return BCM_E_NONE;
}

/* set enable value for a given (group_type+group_id) */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_enable_value_set(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id,
    uint32 group_enable_values)
{
    soc_mem_t mem;
    soc_profile_mem_t *profile_mem;
    void *entry_buf = flowcnt_mem_entry_buf[unit];
    void *entries[1];
    int rv = BCM_E_NONE;

    if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == group_id) {
        /* should not set reserved group's enable value */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type, &group_id,
                                                    &group_enable_values));
    profile_mem = flowcnt_group_mem_profile[unit][group_type];
    mem = flowcnt_group_mem[group_type];

    sal_memset(entry_buf, 0, WORDS2BYTES(soc_mem_entry_words(unit, mem)));
    soc_mem_field32_set(unit, mem, entry_buf,
                        SR_COUNTER_ENABLESf, group_enable_values);

    entries[0] = entry_buf;
    rv = soc_profile_mem_set(unit, profile_mem, entries, group_id);
    BCM_IF_ERROR_RETURN(rv);

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_update_stat2pool(
            unit, group_type, group_id, group_enable_values));

    return BCM_E_NONE;
}

/* find enable value in one specific group
 * return BCM_E_NONE : not exited
 * return BCM_E_EXISTS : has already exited
 */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_enable_value_exist_check(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 find_enable_values)
{
    soc_mem_t mem;
    soc_profile_mem_t *profile_mem;
    int idx;

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    NULL, NULL));
    profile_mem = flowcnt_group_mem_profile[unit][group_type];
    mem = flowcnt_group_mem[group_type];

    for (idx = 0; idx < flowcnt_group_num[group_type]; idx++) {
        void *profile_mem_ptr;
        uint32 enable_values;
        int ref_count;

        if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == idx) {
            continue;
        }
        BCM_IF_ERROR_RETURN(
            soc_profile_mem_ref_count_get(unit, profile_mem, idx, &ref_count));
        if (0 == ref_count) {
            /* just need to find the enable value "in use",
             * so skip the entry with ref_cnt = 0
             */
            continue;
        }
        profile_mem_ptr = SOC_PROFILE_MEM_ENTRY(unit, profile_mem, void *, idx);
        enable_values = soc_mem_field32_get(unit, mem, profile_mem_ptr,
                                            SR_COUNTER_ENABLESf);
        if (enable_values == find_enable_values) {
            return BCM_E_EXISTS;
        }
    }
    return BCM_E_NONE;
}

/* add a enable value into the given group, and return the new added group_id */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_enable_value_insert(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 insert_enable_values,
    uint32 *ret_group_id)
{
    soc_mem_t mem;
    soc_profile_mem_t *profile_mem;
    void *entry_buf = flowcnt_mem_entry_buf[unit];
    int group_id;

    if (NULL == ret_group_id) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type, NULL,
                                                    &insert_enable_values));
    profile_mem = flowcnt_group_mem_profile[unit][group_type];
    mem = flowcnt_group_mem[group_type];
    /* check the new value not duplicate  */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_enable_value_exist_check(
            unit, group_type, insert_enable_values));

    /* insert */
    sal_memset(entry_buf, 0, WORDS2BYTES(soc_mem_entry_words(unit, mem)));
    soc_mem_field32_set(unit, mem, entry_buf,
                        SR_COUNTER_ENABLESf, insert_enable_values);

    BCM_IF_ERROR_RETURN(
        soc_profile_mem_single_table_add(unit, profile_mem, entry_buf,
                                         1, &group_id));
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_update_stat2pool(
            unit, group_type, group_id, insert_enable_values));

    *ret_group_id = (uint32)group_id;

    return BCM_E_NONE;
}

/* delete a enable value from the given group */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_enable_value_delete(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id)
{
    soc_profile_mem_t *profile_mem;
    if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == group_id) {
        /* should not delete reserved group's enable value */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    &group_id, NULL));
    profile_mem = flowcnt_group_mem_profile[unit][group_type];

    BCM_IF_ERROR_RETURN(
        soc_profile_mem_delete(unit, profile_mem, group_id));

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_update_stat2pool(
            unit, group_type, group_id, 0));

    return BCM_E_NONE;
}

/* encode multiple stat types to a enable_values */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_enable_value_encode(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    bcm_tsn_stat_t *stat_arr,
    uint32 stat_arr_cnt,
    uint32 *ret_enable_values)
{
    int i, j, enable_values_count_limit, enable_values_count_max;
    const bcm_tsn_stat_t *enable_values_mapping;

    if (NULL == ret_enable_values || NULL == stat_arr) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    NULL, NULL));

    enable_values_count_limit = flowcnt_pool_num[group_type];
    enable_values_count_max = flowcnt_enable_value_mapping_cnt[unit]
                                                              [group_type];
    enable_values_mapping = flowcnt_enable_value_mapping[group_type];

    if (stat_arr_cnt <= 0 || stat_arr_cnt > enable_values_count_limit) {
        return BCM_E_PARAM;
    }

    *ret_enable_values = 0;
    for (i = 0; i < stat_arr_cnt; i++) {
        int found_out = 0;

        if (bcmTsnStatCount == stat_arr[i]) {
            return BCM_E_PARAM;
        }
        for (j = 0; j < enable_values_count_max; j++) {
            if (stat_arr[i] == enable_values_mapping[j]) {
                *ret_enable_values |= (1 << j);
                found_out = 1;
                break;
            }
        }
        if (0 == found_out) {
            return BCM_E_PARAM;
        }
    }
    if (0 == *ret_enable_values) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* decode enable_values to multiple stat types */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_enable_value_decode(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 enable_values,
    bcm_tsn_stat_t *stat_arr,
    uint32 stat_arr_len,
    int *ret_stat_arr_cnt)
{
    int write_ptr, i, enable_values_count_max, actual_count;
    const bcm_tsn_stat_t *enable_values_mapping;

    if (0 == enable_values || NULL == ret_stat_arr_cnt) {
        return BCM_E_PARAM;
    }

    if (0 == stat_arr_len) {
        /* user just want to get the "needed count"
         * for returned ret_stat_arr_cnt
         * so need not check stat_arr here
         */
    } else {
        /* user want we write the result into stat_arr
         * and get "written count" for returned ret_stat_arr_cnt
         */
        if (NULL == stat_arr) {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    NULL, NULL));

    enable_values_mapping = flowcnt_enable_value_mapping[group_type];
    enable_values_count_max = flowcnt_enable_value_mapping_cnt[unit]
                                                              [group_type];

    write_ptr = 0;
    actual_count = 0;
    for (i = 0; i < enable_values_count_max; i++) {
        if (enable_values & (1 << i)) {
            if (write_ptr < stat_arr_len) {
                stat_arr[write_ptr] = enable_values_mapping[i];
                write_ptr++;
            }
            actual_count++;
        }
    }
    if (0 == stat_arr_len) {
        *ret_stat_arr_cnt = actual_count;
    } else {
        *ret_stat_arr_cnt = write_ptr;
    }
    return BCM_E_NONE;
}

/* set the reference count for the given group_id in one group */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_refcnt_set(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id,
    int refcnt_diff)
{
    soc_profile_mem_t *profile_mem;
    if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == group_id) {
        /* should not modify reserved group's ref count */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    &group_id, NULL));
    profile_mem = flowcnt_group_mem_profile[unit][group_type];
    SOC_PROFILE_MEM_REFERENCE(unit, profile_mem, group_id, refcnt_diff);
    return BCM_E_NONE;
}

/* get the reference count for the given group_id in one group */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_refcnt_get(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id,
    int *ret_refcnt)
{
    soc_profile_mem_t *profile_mem;
    if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == group_id) {
        /* should not modify reserved group's ref count */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    &group_id, NULL));
    profile_mem = flowcnt_group_mem_profile[unit][group_type];
    return soc_profile_mem_ref_count_get(unit, profile_mem, group_id,
                                         ret_refcnt);
}

/* encode "group_type, group_id" to "stat_id" */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_id_encode(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id,
    bcm_tsn_stat_group_t *ret_stat_id)
{
    if (NULL == ret_stat_id) {
        return BCM_E_PARAM;
    }
    if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == group_id) {
        /* should not encode reserved group's to stat_id */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    &group_id, NULL));

    *ret_stat_id = group_id;
    if (bcmTsnStatGroupTypeEgress == group_type) {
        *ret_stat_id |= (1 << BCMI_TSN_FLOWCNT_STAT_ID_EGRESS_BIT);
    }

    return BCM_E_NONE;
}

/* decode "stat_id" to "group_type, group_id" */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_id_decode(
    int unit,
    bcm_tsn_stat_group_t stat_id,
    bcm_tsn_stat_group_type_t *ret_group_type,
    uint32 *ret_group_id)
{
    if (NULL == ret_group_id || NULL == ret_group_type) {
        return BCM_E_PARAM;
    }
    if (stat_id < 0) {
        return BCM_E_PARAM;
    }

    if (stat_id & (1 << BCMI_TSN_FLOWCNT_STAT_ID_EGRESS_BIT)) {
        *ret_group_type = bcmTsnStatGroupTypeEgress;
    } else {
        *ret_group_type = bcmTsnStatGroupTypeIngress;
    }

    *ret_group_id = stat_id & ((1 << BCMI_TSN_FLOWCNT_STAT_ID_EGRESS_BIT) - 1);

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, *ret_group_type,
                                                    ret_group_id, NULL));
    if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == *ret_group_id) {
        /* should not decode reserved stat_id */
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*  for each valid "group_type, group_id", invoke cb with userdata */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_internal_traverse(
    int unit,
    bcmi_gh2_tsn_stat_flowcnt_group_traverse_cb cb,
    void *user_data)
{
    bcm_tsn_stat_group_type_t group_type;

    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        soc_mem_t mem;
        int group_id;

        BCM_IF_ERROR_RETURN(
            bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                        NULL, NULL));
        mem = flowcnt_group_mem[group_type];

        for (group_id = 0; group_id < flowcnt_group_num[group_type];
             group_id++) {
            uint32 enable_values;
            void *entry_buf = flowcnt_mem_entry_buf[unit];

            if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == group_id) {
                continue;
            }

            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ANY, group_id, entry_buf));
            enable_values = soc_mem_field32_get(unit, mem, entry_buf,
                                                SR_COUNTER_ENABLESf);
            if (0 == enable_values) {
                continue;
            }
            BCM_IF_ERROR_RETURN(cb(unit, group_type, group_id, user_data));
        }
    }
    return BCM_E_NONE;
}

/* helper function for bcmi_gh2_tsn_stat_flowcnt_group_traverse() */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_traverse_cb_user(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id,
    void *user_data)
{
    bcmi_gh2_tsn_stat_flowcnt_group_traverse_userdata_t *ori_user_data;
    bcm_tsn_stat_group_t stat_id;

    if (NULL == user_data) {
        return BCM_E_PARAM;
    }

    ori_user_data =
        (bcmi_gh2_tsn_stat_flowcnt_group_traverse_userdata_t *)user_data;
    if (NULL == ori_user_data->cb) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_id_encode(
            unit, group_type, group_id, &stat_id));
    return ori_user_data->cb(unit, stat_id, ori_user_data->cb_user_data);
}

#if defined(BCM_WARM_BOOT_SUPPORT)
/* recover mem profile when warm boot */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_traverse_cb_recover(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id,
    void *user_data)
{
    soc_profile_mem_t *profile_mem =
        flowcnt_group_mem_profile[unit][group_type];
    void *buf = SOC_PROFILE_MEM_ENTRY(unit, profile_mem, void *, group_id);
    uint32 enable_values;

    BCM_IF_ERROR_RETURN(
        soc_profile_mem_sw_state_set(
            unit, profile_mem, &buf, 1, group_id));

    enable_values = soc_mem_field32_get(unit, flowcnt_group_mem[group_type],
                                        buf, SR_COUNTER_ENABLESf);
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_update_stat2pool(
            unit, group_type, group_id, enable_values));

    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_tsn_stat_flowcnt_get_scache_size(
    int unit, int *size)
{
    bcm_tsn_stat_group_type_t group_type;

    if (NULL == size) {
        return BCM_E_PARAM;
    }
    *size = 0;

    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        uint32 pool_index;

        for (pool_index = 0; pool_index < flowcnt_pool_num[group_type];
             pool_index++) {

            soc_mem_t mem = flowcnt_pool_mem[group_type][pool_index];
            *size += sizeof(uint32) * 2 * soc_mem_index_count(unit, mem);
        }
    }
    return BCM_E_NONE;
}

/* sync data to scache */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_sync(
    int unit, uint8 *scache_ptr)
{
    bcm_tsn_stat_group_type_t group_type;

    if (NULL == scache_ptr) {
        return BCM_E_PARAM;
    }

    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        uint32 pool_index;

        for (pool_index = 0; pool_index < flowcnt_pool_num[group_type];
             pool_index++) {

            uint32 entry_index;
            soc_mem_t mem = flowcnt_pool_mem[group_type][pool_index];

            for (entry_index = 0;
                 entry_index < soc_mem_index_count(unit, mem);
                 entry_index++) {

                WRITE_PTR(scache_ptr, uint32,
                          COMPILER_64_LO(flowcnt_counter_sw[unit][group_type]
                                                           [pool_index]
                                                           [entry_index]));
                WRITE_PTR(scache_ptr, uint32,
                          COMPILER_64_HI(flowcnt_counter_sw[unit][group_type]
                                                           [pool_index]
                                                           [entry_index]));
            }
        }
    }
    return BCM_E_NONE;
}

/* layer-1 warmboot : reload data from hw */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_reload_l1(int unit)
{
    /* if there is no scache, just copy hw counter to sw counter */
    bcm_tsn_stat_group_type_t group_type;

    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        uint32 pool_index;

        for (pool_index = 0; pool_index < flowcnt_pool_num[group_type];
             pool_index++) {

            uint32 entry_index;
            soc_mem_t mem = flowcnt_pool_mem[group_type][pool_index];

            for (entry_index = 0;
                 entry_index < soc_mem_index_count(unit, mem);
                 entry_index++) {

                COMPILER_64_COPY(flowcnt_counter_sw[unit][group_type]
                                                  [pool_index][entry_index],
                                 flowcnt_counter_hw[unit][group_type]
                                                  [pool_index][entry_index]);
            }
        }
    }
    return BCM_E_NONE;
}

/* layer-2 warmboot : reload data from scache */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_reload_l2(
    int unit, uint8 *scache_ptr)
{
    bcm_tsn_stat_group_type_t group_type;

    if (NULL == scache_ptr) {
        return BCM_E_PARAM;
    }

    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        uint32 pool_index;

        for (pool_index = 0; pool_index < flowcnt_pool_num[group_type];
             pool_index++) {

            uint32 entry_index;
            soc_mem_t mem = flowcnt_pool_mem[group_type][pool_index];

            for (entry_index = 0;
                 entry_index < soc_mem_index_count(unit, mem);
                 entry_index++) {

                uint32 sec_lo, sec_hi;

                READ_PTR(scache_ptr, uint32, sec_lo);
                READ_PTR(scache_ptr, uint32, sec_hi);
                COMPILER_64_SET(flowcnt_counter_sw[unit][group_type]
                                                  [pool_index][entry_index],
                                sec_hi, sec_lo);
            }
        }
    }
    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/* traverse TSN stat groups */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_traverse(
    int unit,
    bcm_tsn_stat_group_traverse_cb cb,
    void *user_data)
{
    bcmi_gh2_tsn_stat_flowcnt_group_traverse_userdata_t ori_user_data;
    ori_user_data.cb = cb;
    ori_user_data.cb_user_data = user_data;

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_internal_traverse(
            unit, bcmi_gh2_tsn_stat_flowcnt_group_traverse_cb_user,
            &ori_user_data));
    return BCM_E_NONE;
}

/* alloc a index of counter pool
 * if is_index_assign = 0 : parameter "index" is for output a available index
 * if is_index_assign = 1 : parameter "index" is for input, warmboot usage
 */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_pool_counter_alloc(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    int *index,
    int is_index_assign)
{
    shr_res_bitmap_handle_t pool_bitmap;
    int new_index;

    if (index == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    NULL, NULL));

    pool_bitmap = flowcnt_pool_bitmap[unit][group_type];
    if (0 == is_index_assign) {
        /* allocate a new available index */
        BCM_IF_ERROR_RETURN(
            shr_res_bitmap_alloc(pool_bitmap, 0, 1, &new_index));
        *index = new_index;
    } else {
        /* for warm boot, alloc a specified index */
        BCM_IF_ERROR_RETURN(
            shr_res_bitmap_alloc(pool_bitmap,
                                 SHR_RES_BITMAP_ALLOC_WITH_ID, 1,
                                 index));
    }

    /* update min/max */
    if (-1 == flowcnt_pool_bitmap_alloc_min[unit][group_type]) {
        flowcnt_pool_bitmap_alloc_min[unit][group_type] = *index;
        flowcnt_pool_bitmap_alloc_max[unit][group_type] = *index;
    } else if (*index < flowcnt_pool_bitmap_alloc_min[unit][group_type]) {
        flowcnt_pool_bitmap_alloc_min[unit][group_type] = *index;
    } else if (*index > flowcnt_pool_bitmap_alloc_max[unit][group_type]) {
        flowcnt_pool_bitmap_alloc_max[unit][group_type] = *index;
    }

    return BCM_E_NONE;
}

/* free the index in counter pool */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_pool_counter_free(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    int index)
{
    shr_res_bitmap_handle_t pool_bitmap;

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    NULL, NULL));

    pool_bitmap = flowcnt_pool_bitmap[unit][group_type];
    BCM_IF_ERROR_RETURN(shr_res_bitmap_free(pool_bitmap, 1, index));

    /* update min/max */
    if (index == flowcnt_pool_bitmap_alloc_min[unit][group_type] &&
        index == flowcnt_pool_bitmap_alloc_max[unit][group_type]) {
        /* if there is only one index has been allocated
         * (and it is the index that user want to free)
         * let the min/max value back to initial value
         */
        flowcnt_pool_bitmap_alloc_min[unit][group_type] = -1;
        flowcnt_pool_bitmap_alloc_max[unit][group_type] = -1;
    } else if (index == flowcnt_pool_bitmap_alloc_min[unit][group_type]) {
        int new_min;
        int found = 0;

        /* ex. the min of allocated index is 3
         *     the max of allocated index is 10
         *     when the user want to free index 3 (which is the min)
         *     we need to find the new min from bitmap
         */
        for (new_min = index + 1;
             new_min <= flowcnt_pool_bitmap_alloc_max[unit][group_type];
             new_min++) {
            int rv;

            rv = shr_res_bitmap_check(
                     flowcnt_pool_bitmap[unit][group_type],
                     1, new_min);
            if (BCM_E_EXISTS == rv) {
                flowcnt_pool_bitmap_alloc_min[unit][group_type] = new_min;
                found = 1;
                break;
            }
        }
        if (0 == found) {
            return BCM_E_INTERNAL; /* should not happen */
        }
    } else if (index == flowcnt_pool_bitmap_alloc_max[unit][group_type]) {
        int new_max;
        int found = 0;

        /* ex. the min of allocated index is 3
         *     the max of allocated index is 10
         *     when the user want to free index 10 (which is the max)
         *     we need to find the new max from bitmap
         */
        for (new_max = index - 1;
             new_max >= flowcnt_pool_bitmap_alloc_min[unit][group_type];
             new_max--) {
            int rv;

            rv = shr_res_bitmap_check(
                     flowcnt_pool_bitmap[unit][group_type],
                     1, new_max);
            if (BCM_E_EXISTS == rv) {
                flowcnt_pool_bitmap_alloc_max[unit][group_type] = new_max;
                found = 1;
                break;
            }
        }
        if (0 == found) {
            return BCM_E_INTERNAL; /* should not happen */
        }
    }
    return BCM_E_NONE;
}

/* update sw cache : hw flow_id(s) bitmap attached to some specific statType
 *    is_attached - (IN) 1: hw_flow_id attach on this group_id
 *                       0: hw_flow_id deattach from this group_id
 */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_group_update_attached_flow(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id,
    uint32 hw_flow_id,
    uint32 is_attached)
{
    uint32 enable_values;
    bcm_tsn_stat_t stat_array[bcmTsnStatCount];
    int stat_count, i;

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_enable_value_get(
            unit, group_type, group_id, &enable_values));

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_enable_value_decode(
            unit, group_type, enable_values,
            stat_array, bcmTsnStatCount, &stat_count));

    assert(stat_count < bcmTsnStatCount);
    for (i = 0; i < stat_count; i++) {
        if (is_attached) {
            SHR_BITSET(
                flowcnt_stat_fbmp_cache[unit][group_type][stat_array[i]],
                hw_flow_id);
        } else {
            SHR_BITCLR(
                flowcnt_stat_fbmp_cache[unit][group_type][stat_array[i]],
                hw_flow_id);
        }
    }
    return BCM_E_NONE;
}

/* get the group id that attached on the given flow_id */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_flow_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_stat_group_type_t group_type,
    uint32 *ret_group_id)
{
    soc_mem_t mem;
    uint32 hw_flow_id;
    void *buff;

    if (NULL == ret_group_id) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(
            unit, group_type, NULL, NULL));
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_sr_hw_flow_id_get(
            unit, flow_id, &hw_flow_id));

    mem = flowcnt_flow_mem[group_type];
    buff = flowcnt_mem_entry_buf[unit];

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_flow_id, buff));

    *ret_group_id = soc_mem_field32_get(unit, mem, buff,
                                        flowcnt_flow_profile_field[group_type]);
    return BCM_E_NONE;
}

/* attach the specified group_id to given flow_id */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_flow_set(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_stat_group_type_t group_type,
    uint32 group_id)
{
    soc_mem_t mem;
    uint32 hw_flow_id;
    uint32 pre_group_id, pre_counter_idx;
    void *buff;

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(
            unit, group_type, &group_id, NULL));
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_sr_hw_flow_id_get(
            unit, flow_id, &hw_flow_id));

    mem = flowcnt_flow_mem[group_type];
    buff = flowcnt_mem_entry_buf[unit];

    /* get the previous attached group_id and counter_idx */
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_flow_id, buff));

    pre_group_id = soc_mem_field32_get(unit, mem, buff,
                                       flowcnt_flow_profile_field[group_type]);
    pre_counter_idx = soc_mem_field32_get(unit, mem, buff,
                                          SR_COUNTER_INDEXf);

    /* switch to new group id */
    if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED != group_id) {
        int alloc_new_counter_idx;

        if (group_id == pre_group_id) {
            /* return success directly if group_id does not change */
            return BCM_E_NONE;
        }

        /* alloc new pool index */
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_tsn_stat_flowcnt_pool_counter_alloc(
                unit, group_type, &alloc_new_counter_idx, 0));

        /* increase ref count for this group id */
        ERROR_RETURN_WITH_EXTRA_CLEAN(
            bcmi_gh2_tsn_stat_flowcnt_group_refcnt_set(
                unit, group_type, group_id, 1),
            bcmi_gh2_tsn_stat_flowcnt_pool_counter_free(
                unit, group_type, alloc_new_counter_idx));

        soc_mem_field32_set(unit, mem, buff, SR_COUNTER_INDEXf,
                            alloc_new_counter_idx);
        soc_mem_field32_set(unit, mem, buff,
                            flowcnt_flow_profile_field[group_type],
                            group_id);

        ERROR_RETURN_WITH_EXTRA_CLEAN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_flow_id, buff),
            bcmi_gh2_tsn_stat_flowcnt_pool_counter_free(
                unit, group_type, alloc_new_counter_idx));

        ERROR_RETURN_WITH_EXTRA_CLEAN(
            bcmi_gh2_tsn_stat_flowcnt_group_update_attached_flow(
                unit, group_type, group_id, hw_flow_id, 1),
            bcmi_gh2_tsn_stat_flowcnt_pool_counter_free(
                unit, group_type, alloc_new_counter_idx));
    } else {
        /* detach */
        soc_mem_field32_set(unit, mem, buff,
                            flowcnt_flow_profile_field[group_type],
                            BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_flow_id, buff));
    }

    /* de-refer previous group and pool index  */
    if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED != pre_group_id) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_tsn_stat_flowcnt_pool_counter_free(
                unit, group_type, pre_counter_idx));

        BCM_IF_ERROR_RETURN(
            bcmi_gh2_tsn_stat_flowcnt_group_refcnt_set(
                unit, group_type, pre_group_id, -1));

        BCM_IF_ERROR_RETURN(
            bcmi_gh2_tsn_stat_flowcnt_group_update_attached_flow(
                unit, group_type, pre_group_id, hw_flow_id, 0));
    }
    return BCM_E_NONE;
}

/* given a stat type, return all hw flow_id(s) been attached on it */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_flow_get_fbmp(
    int unit,
    bcm_tsn_stat_t stat,
    SHR_BITDCL  *fbmp,
    uint32 fbmp_size)
{
    bcm_tsn_stat_group_type_t group_type;

    if (stat < 0 || stat >= bcmTsnStatCount || NULL == fbmp) {
        return BCM_E_PARAM;
    }

    SHR_BITCLR_RANGE(fbmp, 0, fbmp_size * SHR_BITWID);

    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {

#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            if (fbmp_size < _SHR_BITDCLSIZE(flowcnt_flow_num_fl[group_type])) {
                return BCM_E_PARAM;
            }

            SHR_BITOR_RANGE(fbmp,
                            flowcnt_stat_fbmp_cache[unit][group_type][stat],
                            0, flowcnt_flow_num_fl[group_type], fbmp);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            if (fbmp_size < _SHR_BITDCLSIZE(flowcnt_flow_num[group_type])) {
                return BCM_E_PARAM;
            }

            SHR_BITOR_RANGE(fbmp,
                            flowcnt_stat_fbmp_cache[unit][group_type][stat],
                            0, flowcnt_flow_num[group_type], fbmp);
        }
    }
    return BCM_E_NONE;
}

/*
 * sync flow counter from chip to sw database (flowcnt_counter_hw/sw)
 *    1. read chip actual value from chip, and write it to flowcnt_counter_hw
 *    2. according to the difference with previous value in flowcnt_counter_hw,
 *       increase this difference to flowcnt_counter_sw
 *
 *    ex. flowcnt_counter_hw = 100, flowcnt_counter_sw = 300
 *         if chip actual counter is 102
 *          ==> flowcnt_counter_hw = 102, flowcnt_counter_sw = 302
 *
 * pool_index_min/max : the range of counter pool
 * index_index_min/max : the range of counter index
 */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_counter_sync(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    uint32 pool_index_min,
    uint32 pool_index_max,
    uint32 entry_index_min,
    uint32 entry_index_max)
{
    uint32 pool_index;

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_stat_flowcnt_group_valid_check(unit, group_type,
                                                    NULL, NULL));

    if (pool_index_min >= flowcnt_pool_num[group_type] ||
        pool_index_max >= flowcnt_pool_num[group_type]) {
        return BCM_E_PARAM;
    }

    for (pool_index = pool_index_min; pool_index <= pool_index_max;
         pool_index++) {
        soc_mem_t mem = flowcnt_pool_mem[group_type][pool_index];
        soc_memacc_t memacc_count;
        uint32 entry_byte;
        uint8 *buffer_ptr;
        uint32 entry_index;

        if (!soc_mem_index_valid(unit, mem, entry_index_min) ||
            !soc_mem_index_valid(unit, mem, entry_index_max)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(soc_memacc_init(unit, mem, COUNTf, &memacc_count));
        entry_byte = WORDS2BYTES(soc_mem_entry_words(unit, mem));
        buffer_ptr = &(flowcnt_counter_dma_buffer[unit][group_type]
                       [entry_index_min * entry_byte]);
        BCM_IF_ERROR_RETURN(
            soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                               entry_index_min, entry_index_max,
                               buffer_ptr));

        /* update sw counter for each entry */
        for (entry_index = entry_index_min; entry_index <= entry_index_max;
             entry_index++) {

            uint64 last_count, diff_count, new_count;
            uint64 *array_ptr_sw =
                flowcnt_counter_sw[unit][group_type][pool_index];
            uint64 *array_ptr_hw =
                flowcnt_counter_hw[unit][group_type][pool_index];

            if (NULL == array_ptr_sw || NULL == array_ptr_hw) {
                return BCM_E_INIT;
            }

            /* update sw counter from buffer */
            buffer_ptr = &(flowcnt_counter_dma_buffer[unit][group_type]
                           [entry_index * entry_byte]);
            COMPILER_64_COPY(last_count, array_ptr_hw[entry_index]);
            soc_memacc_field64_get(&memacc_count, buffer_ptr, &new_count);
            if (COMPILER_64_GT(last_count, new_count)) {
                /* diff_count = new_count + count_max - last_count */
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        "Roll over happened\n")));
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        ".Read Packet64 Count: %x:%x\n"),
                             COMPILER_64_HI(new_count),
                             COMPILER_64_LO(new_count)));
                COMPILER_64_COPY(diff_count, new_count);
                COMPILER_64_ADD_64(diff_count, flowcnt_counter_max[unit]
                                                                  [group_type]
                                                                  [pool_index]);
                COMPILER_64_SUB_64(diff_count, last_count);
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        ".Diffed 64-Packet Count: %x:%x\n"),
                             COMPILER_64_HI(diff_count),
                             COMPILER_64_LO(diff_count)));
            } else {
                /* diff_count = new_count - last_count */
                COMPILER_64_COPY(diff_count, new_count);
                COMPILER_64_SUB_64(diff_count, last_count);
            }

            /* update flowcnt_counter_hw */
            COMPILER_64_COPY(array_ptr_hw[entry_index], new_count);

            /* Add difference (if it is) */
            if (!COMPILER_64_IS_ZERO(diff_count)) {
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        "port:%d==>"
                                        "Old 64-Packet Count Value%x:%x\t"),
                             entry_index,
                             COMPILER_64_HI(
                                 array_ptr_sw[entry_index]),
                             COMPILER_64_LO(
                                 array_ptr_sw[entry_index])));
                COMPILER_64_ADD_64(array_ptr_sw[entry_index],
                                   diff_count);
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                       "New 64-Packet Value : %x:%x\n"),
                             COMPILER_64_HI(
                                 array_ptr_sw[entry_index]),
                             COMPILER_64_LO(
                                 array_ptr_sw[entry_index])));
            }
        }
    }
    return BCM_E_NONE;
}

/* helper function for flow counter update */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_counter_update(
    int unit,
    soc_mem_t mem,
    int mem_idx,
    uint64 new_count)
{
    bcm_tsn_stat_group_type_t group_type;
    bcm_tsn_stat_t stat_type;

    /* search the memory and set counter */
    for (stat_type = 0; stat_type < bcmTsnStatCount; stat_type++) {
        uint64 *array_ptr_sw, *array_ptr_hw;

        for (group_type = 0; group_type < bcmTsnStatGroupTypeCount;
             group_type++) {
            soc_mem_t flowcnt_mem = flowcnt_pool_mem[group_type]
                                                    [stat_type];
            if (mem != flowcnt_mem) {
                continue;
            }
            array_ptr_sw = flowcnt_counter_sw[unit][group_type]
                                             [stat_type];
            array_ptr_hw = flowcnt_counter_hw[unit][group_type]
                                             [stat_type];
            COMPILER_64_COPY(
                array_ptr_sw[mem_idx],
                new_count);
            COMPILER_64_COPY(
                array_ptr_hw[mem_idx],
                new_count);
            return BCM_E_NONE;
        }
    }
    return BCM_E_NOT_FOUND;
}

/* helper function for flow counter access */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_counter_access(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcmi_tsn_stat_counter_action_t action,
    uint32 arr_cnt,
    bcm_tsn_stat_t *stat_type_arr,
    uint64 *stat_sw_value_arr,
    uint64 *stat_hw_value_arr)
{
    int arr_idx;
    bcm_tsn_stat_group_type_t group_type;
    uint32 group_id[bcmTsnStatGroupTypeCount];
    uint32 counter_idx[bcmTsnStatGroupTypeCount];
    uint32 hw_flow_id;

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_sr_hw_flow_id_get(
            unit, flow_id, &hw_flow_id));

    if (NULL == stat_type_arr) {
        return BCM_E_PARAM;
    }
    for (arr_idx = 0; arr_idx < arr_cnt; arr_idx++) {
        if (stat_type_arr[arr_idx] >= bcmTsnStatCount) {
            return BCM_E_PARAM;
        }
    }

    /* get group_id and counter_idx */
    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        soc_mem_t mem = flowcnt_flow_mem[group_type];
        void *buff = flowcnt_mem_entry_buf[unit];

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_flow_id, buff));

        group_id[group_type] = soc_mem_field32_get(
                                   unit, mem, buff,
                                   flowcnt_flow_profile_field[group_type]);
        counter_idx[group_type] = soc_mem_field32_get(unit, mem, buff,
                                                      SR_COUNTER_INDEXf);
    }

    /* for each stat type that user specify, set/get counter */
    for (arr_idx = 0; arr_idx < arr_cnt; arr_idx++) {
        int found = 0;
        int select_pool_index;
        int select_counter_index;
        bcm_tsn_stat_group_type_t select_group_type = bcmTsnStatGroupTypeCount;
        uint64 *array_ptr_sw, *array_ptr_hw;

        for (group_type = 0; group_type < bcmTsnStatGroupTypeCount;
             group_type++) {

            if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == group_id[group_type]) {
                continue;
            }
            select_pool_index =
                flowcnt_group_stat2pool[unit][group_type]
                                       [group_id[group_type]]
                                       [stat_type_arr[arr_idx]];
            if (-1 != select_pool_index) {
                select_group_type = group_type;
                found = 1;
                break;
            }
        }

        if (0 == found) {
            /* the user has not install this stat type before */
            BCM_IF_ERROR_RETURN(BCM_E_NOT_FOUND);
        }

        assert(select_group_type >= 0 &&
               select_group_type < bcmTsnStatGroupTypeCount);
        assert(select_pool_index >= 0 &&
               select_pool_index < flowcnt_pool_num[select_group_type]);
        array_ptr_sw = flowcnt_counter_sw[unit][select_group_type]
                                         [select_pool_index];
        array_ptr_hw = flowcnt_counter_hw[unit][select_group_type]
                                         [select_pool_index];
        select_counter_index = counter_idx[select_group_type];
        /* get flow counter */
        switch (action) {
            case bcmiTsnStatCounterActionReadSync:
                BCM_IF_ERROR_RETURN(
                    bcmi_gh2_tsn_stat_flowcnt_counter_sync(
                        unit, select_group_type,
                        select_pool_index, select_pool_index,
                        select_counter_index,
                        select_counter_index));
                /* fall through */
           case bcmiTsnStatCounterActionRead:
                if (NULL != stat_sw_value_arr) {
                    COMPILER_64_COPY(
                        stat_sw_value_arr[arr_idx],
                        array_ptr_sw[select_counter_index]);
                }
                if (NULL != stat_hw_value_arr) {
                    COMPILER_64_COPY(
                        stat_hw_value_arr[arr_idx],
                        array_ptr_hw[select_counter_index]);
                }
                break;
           case bcmiTsnStatCounterActionWrite:
                if (NULL != stat_sw_value_arr) {
                    COMPILER_64_COPY(
                        array_ptr_sw[select_counter_index],
                        stat_sw_value_arr[arr_idx]);
                }
                if (NULL != stat_hw_value_arr) {
                    uint8 *buffer_ptr;
                    uint64 new_count;
                    soc_mem_t mem = flowcnt_pool_mem[select_group_type]
                                                    [select_pool_index];

                    COMPILER_64_COPY(new_count, stat_hw_value_arr[arr_idx]);
                    COMPILER_64_AND(new_count,
                                    flowcnt_counter_mask[unit]
                                                        [select_group_type]
                                                        [select_pool_index]);

                    /* write masked value into chip actual data and
                     * update flowcnt_counter_hw
                     */
                    COMPILER_64_COPY(
                        array_ptr_hw[select_counter_index],
                        new_count);
                    buffer_ptr = &(flowcnt_counter_dma_buffer
                                   [unit][select_group_type][0]);
                    BCM_IF_ERROR_RETURN(
                        soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     select_counter_index, buffer_ptr));
                    soc_mem_field64_set(
                        unit, mem, buffer_ptr, COUNTf, new_count);
                    BCM_IF_ERROR_RETURN(
                        soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      select_counter_index, buffer_ptr));
                }
                break;
           default:
                return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/* callback for flow counter, sync all stat types
 * (just need to update the counter index has been in use)
 */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_counter_sync_all(int unit)
{
    bcm_tsn_stat_group_type_t group_type;

    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        if (-1 == flowcnt_pool_bitmap_alloc_min[unit][group_type]) {
            continue;
        }
        bcmi_gh2_tsn_stat_flowcnt_counter_sync(
            unit, group_type,
            0, flowcnt_pool_num[group_type] - 1,
            flowcnt_pool_bitmap_alloc_min[unit][group_type],
            flowcnt_pool_bitmap_alloc_max[unit][group_type]);
    }

    return BCM_E_NONE;
}

/* callback for flow counter, sync one specific stat type
 * (just need to update the counter index has been in use)
 */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_counter_sync_stat(int unit, bcm_tsn_stat_t stat)
{
    bcm_tsn_stat_group_type_t group_type;
    int pool_idx;

    if (stat < 0 || stat >= bcmTsnStatCount) {
        return BCM_E_PARAM;
    }

    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        SHR_BITDCL select_pool[1];
        int pool_num = flowcnt_pool_num[group_type];
        int group_id;

        if (-1 == flowcnt_pool_bitmap_alloc_min[unit][group_type]) {
            continue;
        }
        assert(pool_num <= SHR_BITWID);
        SHR_BITCLR_RANGE(&(select_pool[0]), 0, pool_num);
        for (group_id = 0; group_id < flowcnt_group_num[group_type];
             group_id++) {
            pool_idx = flowcnt_group_stat2pool[unit][group_type]
                                              [group_id][stat];
            if (-1 == pool_idx) {
                continue;
            }
            assert(pool_idx >= 0 && pool_idx < pool_num);
            SHR_BITSET(&(select_pool[0]), pool_idx);
        }

        SHR_BIT_ITER(&(select_pool[0]), pool_num, pool_idx) {
            bcmi_gh2_tsn_stat_flowcnt_counter_sync(
                unit, group_type, pool_idx, pool_idx,
                flowcnt_pool_bitmap_alloc_min[unit][group_type],
                flowcnt_pool_bitmap_alloc_max[unit][group_type]);
        }
    }

    return BCM_E_NONE;
}

/* clean the resource for tsn flow counter */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_cleanup(int unit)
{
    bcm_tsn_stat_group_type_t group_type;

    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        int i;

        if (NULL != flowcnt_group_mem_profile[unit][group_type]) {
            soc_profile_mem_destroy(
                unit, flowcnt_group_mem_profile[unit][group_type]);
            sal_free(flowcnt_group_mem_profile[unit][group_type]);
            flowcnt_group_mem_profile[unit][group_type] = NULL;
        }
        if (NULL != flowcnt_group_stat2pool[unit][group_type]) {
            for (i = 0; i < flowcnt_group_num[group_type]; i++) {
                if (NULL != flowcnt_group_stat2pool[unit][group_type][i]) {
                    sal_free(flowcnt_group_stat2pool[unit][group_type][i]);
                    flowcnt_group_stat2pool[unit][group_type][i] = NULL;
                }
            }
            sal_free(flowcnt_group_stat2pool[unit][group_type]);
            flowcnt_group_stat2pool[unit][group_type] = NULL;
        }
        if (NULL != flowcnt_pool_bitmap[unit][group_type]) {
            shr_res_bitmap_destroy(flowcnt_pool_bitmap[unit][group_type]);
            flowcnt_pool_bitmap[unit][group_type] = NULL;
        }
        if (NULL != flowcnt_counter_dma_buffer[unit][group_type]) {
            soc_cm_sfree(unit, flowcnt_counter_dma_buffer[unit][group_type]);
            flowcnt_counter_dma_buffer[unit][group_type] = NULL;
        }
        for (i = 0;
             i < flowcnt_pool_num[group_type];
             i++) {

            if (NULL != flowcnt_counter_sw[unit][group_type][i]) {
                sal_free(flowcnt_counter_sw[unit][group_type][i]);
                flowcnt_counter_sw[unit][group_type][i] = NULL;
            }
            if (NULL != flowcnt_counter_hw[unit][group_type][i]) {
                sal_free(flowcnt_counter_hw[unit][group_type][i]);
                flowcnt_counter_hw[unit][group_type][i] = NULL;
            }
        }
        for (i = 0; i < bcmTsnStatCount; i++) {
            if (NULL != flowcnt_stat_fbmp_cache[unit][group_type][i]) {
                sal_free(flowcnt_stat_fbmp_cache[unit][group_type][i]);
                flowcnt_stat_fbmp_cache[unit][group_type][i] = NULL;
            }
        }
        flowcnt_pool_bitmap_alloc_min[unit][group_type] = -1;
        flowcnt_pool_bitmap_alloc_max[unit][group_type] = -1;
    }

    if (NULL != flowcnt_mem_entry_buf[unit]) {
        sal_free(flowcnt_mem_entry_buf[unit]);
        flowcnt_mem_entry_buf[unit] = NULL;
    }
    return BCM_E_NONE;
}

/* init the resource for tsn flow counter */
STATIC int
bcmi_gh2_tsn_stat_flowcnt_init(int unit)
{
    bcm_tsn_stat_group_type_t group_type;
    uint32 max_entry_size = 0;
    void *entry_buf;

    /* parameter setup */
    if (soc_feature(unit, soc_feature_tsn_taf)) {
        flowcnt_enable_value_mapping_cnt[unit][bcmTsnStatGroupTypeIngress] = 20;
        flowcnt_enable_value_mapping_cnt[unit][bcmTsnStatGroupTypeEgress]  = 4;
    } else {
        flowcnt_enable_value_mapping_cnt[unit][bcmTsnStatGroupTypeIngress] = 15;
        flowcnt_enable_value_mapping_cnt[unit][bcmTsnStatGroupTypeEgress]  = 4;
    }

    /* handle flow mem */
    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        soc_mem_t mem = flowcnt_flow_mem[group_type];

        max_entry_size =
            BCMI_TSN_FLOWCNT_MAX(max_entry_size,
                                 WORDS2BYTES(soc_mem_entry_words(unit, mem)));

        /* verify flowcnt_flow_num */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            if (flowcnt_flow_num_fl[group_type] !=
                soc_mem_index_count(unit, mem)) {
                INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_INTERNAL);
            }
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            if (flowcnt_flow_num[group_type] !=
                soc_mem_index_count(unit, mem)) {
                INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_INTERNAL);
            }
        }
    }

    /* handle group mem */
    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        soc_mem_t group_mem = flowcnt_group_mem[group_type];
        soc_profile_mem_t *profile_mem;
        int mem_word, group_id;

         /* verify flowcnt_group_num */
        if (flowcnt_group_num[group_type] !=
            soc_mem_index_count(unit, group_mem)) {
            INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_INTERNAL);
        }

        /* because we use bit 20 for egress bit in encoding stat id
         * , so the number of entries cannot excess 1 << 20
         */
        if (flowcnt_group_num[group_type] >=
            (1 << BCMI_TSN_FLOWCNT_STAT_ID_EGRESS_BIT)) {
            INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_INTERNAL);
        }

        max_entry_size =
            BCMI_TSN_FLOWCNT_MAX(max_entry_size,
                                 WORDS2BYTES(soc_mem_entry_words(unit,
                                                                 group_mem)));

        /* profile mem */
        mem_word = soc_mem_entry_words(unit, group_mem);
        flowcnt_group_mem_profile[unit][group_type] =
            sal_alloc(sizeof(soc_profile_mem_t), "TSN Group Profile Mem");
        profile_mem = flowcnt_group_mem_profile[unit][group_type];
        if (NULL == profile_mem) {
            INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
        }
        soc_profile_mem_t_init(profile_mem);
        INIT_ERROR_RETURN_WITH_CLEAN(
            soc_profile_mem_create(unit, &group_mem, &mem_word,
                                   1, profile_mem));

        /* for each gorup_id
         *   alloc flowcnt_group_stat2pool
         */
        flowcnt_group_stat2pool[unit][group_type] =
            sal_alloc(flowcnt_group_num[group_type] * sizeof(int32 *),
                      "flowcnt_group_stat2pool");
        if (NULL == flowcnt_group_stat2pool[unit][group_type]) {
            INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
        }

        for (group_id = 0; group_id < flowcnt_group_num[group_type];
             group_id++) {

            int stat_id;

            flowcnt_group_stat2pool[unit][group_type][group_id] =
                sal_alloc(bcmTsnStatCount * sizeof(int32),
                          "flowcnt_group_stat2pool");
            if (NULL == flowcnt_group_stat2pool[unit][group_type][group_id]) {
                INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
            }
            for (stat_id = 0; stat_id < bcmTsnStatCount; stat_id++) {
                flowcnt_group_stat2pool[unit][group_type]
                                       [group_id][stat_id] = -1;
            }
        }
    }

    /* handle pool mem */
    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        int index, pool_entry_count;
        soc_mem_t mem;


        for (index = 0;
             index < flowcnt_pool_num[group_type];
             index++) {

            mem = flowcnt_pool_mem[group_type][index];
            max_entry_size =
                BCMI_TSN_FLOWCNT_MAX(max_entry_size,
                                     WORDS2BYTES(
                                         soc_mem_entry_words(unit, mem)));
        }

        /* allocate bitmap */
        mem = flowcnt_pool_mem[group_type][0];
        pool_entry_count = soc_mem_index_count(unit, mem);
        INIT_ERROR_RETURN_WITH_CLEAN(
            shr_res_bitmap_create(&flowcnt_pool_bitmap[unit][group_type],
                                  0, pool_entry_count));
        flowcnt_pool_bitmap_alloc_min[unit][group_type] = -1;
        flowcnt_pool_bitmap_alloc_max[unit][group_type] = -1;

        /* allocate counter sw database */
        for (index = 0;
             index < flowcnt_pool_num[group_type]; index++) {

            flowcnt_counter_sw[unit][group_type][index] =
                sal_alloc(pool_entry_count * sizeof(uint64),
                          "tsn flow sw counter");
            if (NULL == flowcnt_counter_sw[unit][group_type][index]) {
                INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
            }
            sal_memset(flowcnt_counter_sw[unit][group_type][index],
                       0, pool_entry_count * sizeof(uint64));

            flowcnt_counter_hw[unit][group_type][index] =
                sal_alloc(pool_entry_count * sizeof(uint64),
                          "tsn flow hw counter");
            if (NULL == flowcnt_counter_hw[unit][group_type][index]) {
                INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
            }
            sal_memset(flowcnt_counter_hw[unit][group_type][index],
                       0, pool_entry_count * sizeof(uint64));

            /* setup max/mask for counter */
            COMPILER_64_SET(flowcnt_counter_max[unit][group_type][index], 0, 1);
            COMPILER_64_SHL(flowcnt_counter_max[unit][group_type][index],
                            soc_mem_field_length(
                                unit, flowcnt_pool_mem[group_type][index],
                                COUNTf));
            COMPILER_64_COPY(flowcnt_counter_mask[unit][group_type][index],
                             flowcnt_counter_max[unit][group_type][index]);
            COMPILER_64_SUB_32(flowcnt_counter_mask[unit][group_type][index],
                               1);
        }

        /* allocate dma buffer */
        flowcnt_counter_dma_buffer[unit][group_type] =
            soc_cm_salloc(unit, pool_entry_count *
                          WORDS2BYTES(soc_mem_entry_words(unit, mem)),
                          "tsn flowctr counter buffer");
        if (NULL == flowcnt_counter_dma_buffer[unit][group_type]) {
             INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
        }

        /* setup cache for statType and fbmp */
        for (index = 0; index < bcmTsnStatCount; index++) {
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                flowcnt_stat_fbmp_cache[unit][group_type][index] =
                        sal_alloc(SHR_BITALLOCSIZE(flowcnt_flow_num_fl[group_type]),
                                  "flowcnt_stat_fbmp_cache");
                if (NULL == flowcnt_stat_fbmp_cache[unit][group_type][index]) {
                    INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
                }
                SHR_BITCLR_RANGE(
                    flowcnt_stat_fbmp_cache[unit][group_type][index],
                    0, flowcnt_flow_num_fl[group_type]);
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                flowcnt_stat_fbmp_cache[unit][group_type][index] =
                        sal_alloc(SHR_BITALLOCSIZE(flowcnt_flow_num[group_type]),
                                  "flowcnt_stat_fbmp_cache");
                if (NULL == flowcnt_stat_fbmp_cache[unit][group_type][index]) {
                    INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
                }
                SHR_BITCLR_RANGE(
                    flowcnt_stat_fbmp_cache[unit][group_type][index],
                    0, flowcnt_flow_num[group_type]);
            }
        }
    }

    /* allocate mem entry buf */
    flowcnt_mem_entry_buf[unit] = sal_alloc(max_entry_size, "tsn mem buf");
    entry_buf = flowcnt_mem_entry_buf[unit];
    if (NULL == entry_buf) {
        INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
    }

    /* reserve profile entry 0 for invalid usage */
    for (group_type = 0; group_type < bcmTsnStatGroupTypeCount; group_type++) {
        soc_mem_t mem = flowcnt_group_mem[group_type];
        int group_id;

        soc_mem_field32_set(unit, mem, entry_buf, SR_COUNTER_ENABLESf, 0);
        INIT_ERROR_RETURN_WITH_CLEAN(
            soc_profile_mem_single_table_add(
                unit, flowcnt_group_mem_profile[unit][group_type],
                entry_buf, 1, &group_id));
        if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED != group_id) {
            INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_INTERNAL);
        }
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        /* warmboot : recover group profile,
         * traverse each valid "group_type, group_id", recover its refcnt
         * back to 1
         */
        INIT_ERROR_RETURN_WITH_CLEAN(
            bcmi_gh2_tsn_stat_flowcnt_group_internal_traverse(
                unit, bcmi_gh2_tsn_stat_flowcnt_group_traverse_cb_recover,
                NULL));

        /* warmboot : recover reference count and pool alloc bitmap,
         * traverse each flow, if it attached to some group
         * ==> group's refcnt++ , setup alloc bitmap
         */
        for (group_type = 0; group_type < bcmTsnStatGroupTypeCount;
             group_type++) {
            int entry_byte, alloc_size, index;
            soc_mem_t mem = flowcnt_flow_mem[group_type];
            int entry_num = flowcnt_flow_num[group_type];
            uint8 *dma_entry_buf;

#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                entry_num = flowcnt_flow_num_fl[group_type];
            }
#endif /* BCM_FIRELIGHT_SUPPORT */
            entry_byte = WORDS2BYTES(soc_mem_entry_words(unit, mem));
            alloc_size = entry_num * entry_byte;
            dma_entry_buf = soc_cm_salloc(unit, alloc_size,
                                          "tsn counter dmabuf");
            if (NULL == dma_entry_buf) {
                INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
            }

            INIT_ERROR_RETURN_WITH_EXTRA_CLEAN(
                soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                   0, entry_num -1, dma_entry_buf),
                soc_cm_sfree(unit, dma_entry_buf));

            for (index = 0; index < entry_num; index++) {
                uint32 group_id;
                int counter_idx;

                group_id =
                    soc_mem_field32_get(unit, mem,
                                        &dma_entry_buf[index * entry_byte],
                                        flowcnt_flow_profile_field[group_type]);
                counter_idx =
                    soc_mem_field32_get(unit, mem,
                                        &dma_entry_buf[index * entry_byte],
                                        SR_COUNTER_INDEXf);

                if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED != group_id) {
                    SOC_PROFILE_MEM_REFERENCE(
                        unit, flowcnt_group_mem_profile[unit][group_type],
                        group_id, 1);

                    INIT_ERROR_RETURN_WITH_EXTRA_CLEAN(
                        bcmi_gh2_tsn_stat_flowcnt_pool_counter_alloc(
                            unit, group_type, &counter_idx, 1),
                        soc_cm_sfree(unit, dma_entry_buf));

                    INIT_ERROR_RETURN_WITH_EXTRA_CLEAN(
                        bcmi_gh2_tsn_stat_flowcnt_group_update_attached_flow(
                            unit, group_type, group_id, index, 1),
                        soc_cm_sfree(unit, dma_entry_buf));
                }
            }
            soc_cm_sfree(unit, dma_entry_buf);
        }

        /* warmboot : recover counter
         *    recover flowcnt_counter_hw by hw data
         *    (flowcnt_counter_sw will be recoved in reload)
         */
        for (group_type = 0; group_type < bcmTsnStatGroupTypeCount;
             group_type++) {

            uint32 pool_index;
            for (pool_index = 0; pool_index < flowcnt_pool_num[group_type];
                 pool_index++) {

                soc_mem_t mem = flowcnt_pool_mem[group_type][pool_index];
                soc_memacc_t memacc_count;
                int entry_byte, entry_index;
                uint8 *buffer_ptr;

                INIT_ERROR_RETURN_WITH_CLEAN(
                    soc_memacc_init(unit, mem, COUNTf, &memacc_count));

                entry_byte = WORDS2BYTES(soc_mem_entry_words(unit, mem));
                buffer_ptr = flowcnt_counter_dma_buffer[unit][group_type];

                INIT_ERROR_RETURN_WITH_CLEAN(
                    soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                       0, soc_mem_index_count(unit, mem) - 1,
                                       buffer_ptr));

                for (entry_index = 0;
                     entry_index < soc_mem_index_count(unit, mem);
                     entry_index++) {
                    uint64 new_count;
                    uint64 *array_ptr_hw =
                        flowcnt_counter_hw[unit][group_type][pool_index];

                    buffer_ptr =
                        &(flowcnt_counter_dma_buffer[unit][group_type]
                                                    [entry_index * entry_byte]);
                    soc_memacc_field64_get(&memacc_count, buffer_ptr,
                                           &new_count);
                    COMPILER_64_COPY(array_ptr_hw[entry_index], new_count);
                }
            }
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_tsn_stat_threshold_select_set
 * Purpose:
 *    Configure threshold for a specific stat type based on source
 * Parameters:
 *    unit         - (IN) Unit number
 *    select_info  - (IN) TSN stat threshold selected source
 *    count        - (IN) count of TSN stat threshold selected source
 *    stat         - (IN) TSN statistic type (see tsn.h)
 *    config       - (IN) TSN stat threshold configuration structure
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_stat_threshold_select_set(
    int unit,
    const bcmi_gh2_tsn_stat_threshold_select_t *select_info,
    int count,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_config_t *config)
{
    const bcmi_gh2_tsn_stat_threshold_select_t *info;
    int interrupt_enable = 0, copy_to_cpu = 0;
    int idx;
    uint32 threshold_value;
    uint32 field_value = 0;
    uint64 reg_val64;
    int is_changed = 0;

    /* Input parameter check */
    if ((NULL == config) || (NULL == select_info)) {
        return BCM_E_PARAM;
    }

    threshold_value = config->threshold;
    if (config->flags) {
        if (!(config->flags & BCM_TSN_STAT_THRESHOLD_EVENT_NOTIFICATION) &&
            !(config->flags & BCM_TSN_STAT_THRESHOLD_COPY_TO_CPU)) {
            return BCM_E_UNAVAIL;
        }

        /*
         * For MTU/STU, should write value (N) to HW register.
         * For other counters, should write value (N-1) to HW register.
         */
        if (!GH2_TSN_STAT_THRESHOLD_MTU_STU_IS_VALID(unit, stat)) {
            if (threshold_value > 0) {
                threshold_value -= 1;
            } else {
                /* Can't write value (0-1=-1) to HW register */
                return BCM_E_CONFIG;
            }
        }
    } else {
        /*
         * If flags = 0 (threshold disabled),
         * ignore the counter value and write maximum to the register
         */
        threshold_value = GH2_TSN_STAT_THRESHOLD_DEFAULT_VALUE;
    }

    /* Event notification */
    if (config->flags & BCM_TSN_STAT_THRESHOLD_EVENT_NOTIFICATION) {
        interrupt_enable = 0x1;
    }

    /* Copy the packet to CPU */
    if (config->flags & BCM_TSN_STAT_THRESHOLD_COPY_TO_CPU) {
        copy_to_cpu = 0x1;
    }

    /* Select the TSN stat type */
    for (idx = 0; idx < count; idx++) {
        info = select_info + idx;
        if (info->stat_type != stat) {
            continue;
        }

        COMPILER_64_ZERO(reg_val64);
        /* Read the specified threshold register */
        BCM_IF_ERROR_RETURN(
            soc_reg64_get(unit, info->reg, REG_PORT_ANY, 0, &reg_val64));

        /* COUNTER_THRESHOLDf */
        field_value = soc_reg64_field32_get(unit, info->reg,
                                            reg_val64, info->threshold_field);
        if (threshold_value != field_value) {
            is_changed = 1;
            soc_reg64_field32_set(unit, info->reg, &reg_val64,
                                  info->threshold_field, threshold_value);
        }

        /* INTERRUPT_ENABLEf */
        field_value = soc_reg64_field32_get(unit, info->reg,
                                            reg_val64, info->interrupt_field);
        if (interrupt_enable != field_value) {
            is_changed = 1;
            soc_reg64_field32_set(unit, info->reg, &reg_val64,
                                  info->interrupt_field, interrupt_enable);
        }

        /* COPY_TO_CPUf */
        if (info->copy_to_cpu_field != INVALIDf) {
            field_value = soc_reg64_field32_get(unit, info->reg,
                                                reg_val64,
                                                info->copy_to_cpu_field);
            if (copy_to_cpu != field_value) {
                is_changed = 1;
                soc_reg64_field32_set(unit, info->reg, &reg_val64,
                                      info->copy_to_cpu_field, copy_to_cpu);
            }
        } else {
            if (copy_to_cpu) {
                return BCM_E_PARAM;
            }
        }

        if (is_changed) {
            /* Write the specified threshold register if any changes */
            BCM_IF_ERROR_RETURN(
                soc_reg64_set(unit, info->reg, REG_PORT_ANY, 0, reg_val64));
        }

        break;
    }

    if (idx == count) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_tsn_stat_threshold_select_get
 * Purpose:
 *    Get the threshold configuration for a specific stat type based on source
 * Parameters:
 *    unit         - (IN) Unit number
 *    select_info  - (IN) TSN stat threshold selected source
 *    count        - (IN) count of TSN stat threshold selected source
 *    stat         - (IN) TSN statistics type (see tsn.h)
 *    config       - (OUT) TSN stat threshold configuration structure
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_stat_threshold_select_get(
    int unit,
    const bcmi_gh2_tsn_stat_threshold_select_t *select_info,
    int count,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_config_t *config)
{
    const bcmi_gh2_tsn_stat_threshold_select_t *info;
    int interrupt_enable = 0, copy_to_cpu = 0;
    int idx;
    uint64 reg_val64;

    /* Input parameter check */
    if ((NULL == config) || (NULL == select_info)) {
        return BCM_E_PARAM;
    }

    /* Clear buffer to zero */
    bcm_tsn_stat_threshold_config_t_init(config);

    /* Select the TSN stat type */
    for (idx = 0; idx < count; idx++) {
        info = select_info + idx;
        if (info->stat_type != stat) {
            continue;
        }

        COMPILER_64_ZERO(reg_val64);
        /* Read the specified threshold register */
        BCM_IF_ERROR_RETURN(
            soc_reg64_get(unit, info->reg, REG_PORT_ANY, 0, &reg_val64));

        /* COUNTER_THRESHOLDf */
        config->threshold = soc_reg64_field32_get(unit, info->reg,
                                                  reg_val64,
                                                  info->threshold_field);
        /* INTERRUPT_ENABLEf */
        interrupt_enable = soc_reg64_field32_get(unit, info->reg,
                                                 reg_val64,
                                                 info->interrupt_field);
        /* Event notification */
        if (interrupt_enable) {
            config->flags |= BCM_TSN_STAT_THRESHOLD_EVENT_NOTIFICATION;
        }

        /* COPY_TO_CPUf */
        if (info->copy_to_cpu_field != INVALIDf) {
            copy_to_cpu = soc_reg64_field32_get(unit, info->reg,
                                                reg_val64,
                                                info->copy_to_cpu_field);
            /* Copy the packet to CPU */
            if (copy_to_cpu) {
                config->flags |= BCM_TSN_STAT_THRESHOLD_COPY_TO_CPU;
            }
        }

        break;
    }

    if (idx == count) {
        return BCM_E_PARAM;
    }

    /*
     * For MTU/STU, read value N from HW register should return (N).
     * For other counters, read value N from HW register should return (N+1).
     */
    if (!GH2_TSN_STAT_THRESHOLD_MTU_STU_IS_VALID(unit, stat)) {
        if (config->threshold < GH2_TSN_STAT_THRESHOLD_DEFAULT_VALUE) {
            config->threshold += 1;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_tsn_stat_threshold_set
 * Purpose:
 *    Configure threshold for a specific stat type on
 *    a specific source
 * Parameters:
 *    unit   - (IN) Unit number
 *    source - (IN) Stat source for TSN stat threshold check
 *    stat   - (IN) TSN statistics type (see tsn.h)
 *    config - (IN) TSN stat threshold configuration structure
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_stat_threshold_set(
    int unit,
    bcm_tsn_stat_threshold_source_t source,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_config_t *config)
{
    int rv = BCM_E_UNAVAIL;
    int count = 0;
    const bcmi_gh2_tsn_stat_threshold_select_t *select_info;

    /* Input parameter check */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_tsn)) {
        /* Valid check for stat type */
        if (!GH2_TSN_STAT_THRESHOLD_IS_VALID(unit, stat)) {
            return BCM_E_PARAM;
        }

        /* Select the source type: Port or SR Flow */
        switch (source) {
            case bcmTsnStatThresholdSourcePort:
                /*
                 * Set the threshold for a specific TSN stat
                 * on source port
                 */
                select_info = gh2_port_count_threshold_info;
                count = COUNTOF(gh2_port_count_threshold_info);
                break;
            case bcmTsnStatThresholdSourceSRFlow:
                /*
                 * Set the threshold for a specific TSN stat
                 * on source SR flow
                 */
                select_info = gh2_flow_count_threshold_info;
                count = COUNTOF(gh2_flow_count_threshold_info);
                break;
            default:
                return BCM_E_PARAM;
        }

        rv = bcmi_gh2_tsn_stat_threshold_select_set(unit, select_info,
                                                    count, stat, config);
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_tsn_stat_threshold_get
 * Purpose:
 *    Get the threshold configuration for a specific stat type on
 *    a specific source
 * Parameters:
 *    unit   - (IN) Unit number
 *    source - (IN) Stat source for TSN stat threshold check
 *    stat   - (IN) TSN statistics type (see tsn.h)
 *    config - (OUT) TSN stat threshold configuration structure
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_stat_threshold_get(
    int unit,
    bcm_tsn_stat_threshold_source_t source,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_config_t *config)
{
    int rv = BCM_E_UNAVAIL;
    int count = 0;
    const bcmi_gh2_tsn_stat_threshold_select_t *select_info;

    /* Input parameter check */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_tsn)) {
        /* Valid check for stat type */
        if (!GH2_TSN_STAT_THRESHOLD_IS_VALID(unit, stat)) {
            return BCM_E_PARAM;
        }

        /* Select the source type: Port or SR Flow */
        switch (source) {
            case bcmTsnStatThresholdSourcePort:
                /*
                 * Get the threshold config for a specific TSN stat
                 * on source port
                 */
                select_info = gh2_port_count_threshold_info;
                count = COUNTOF(gh2_port_count_threshold_info);
                break;
            case bcmTsnStatThresholdSourceSRFlow:
                /*
                 * Get the threshold config for a specific TSN stat
                 * on source SR flow
                 */
                select_info = gh2_flow_count_threshold_info;
                count = COUNTOF(gh2_flow_count_threshold_info);
                break;
            default:
                return BCM_E_PARAM;
        }

        rv = bcmi_gh2_tsn_stat_threshold_select_get(unit, select_info,
                                                    count, stat, config);
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_tsn_stat_threshold_init
 * Purpose:
 *    Helper function to initialize all TSN counter thresholds
 * Parameters:
 *    unit   - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_stat_threshold_init(int unit)
{
    int rv = BCM_E_UNAVAIL;
    const bcmi_gh2_tsn_stat_threshold_select_t *info, *select_info;
    bcm_tsn_stat_threshold_config_t config;
    int idx = 0, count = 0;

    bcm_tsn_stat_threshold_config_t_init(&config);
    config.threshold = GH2_TSN_STAT_THRESHOLD_DEFAULT_VALUE;

    if (soc_feature(unit, soc_feature_tsn)) {
        /* TSN Stat threshold for source Port */
        select_info = gh2_port_count_threshold_info;
        count = COUNTOF(gh2_port_count_threshold_info);
        for (idx = 0; idx < count; idx++) {
            info = select_info + idx;
            rv = bcmi_gh2_tsn_stat_threshold_select_set(unit, select_info,
                                                        count,
                                                        info->stat_type,
                                                        &config);
        }

        /* TSN Stat threshold for source SR Flow */
        select_info = gh2_flow_count_threshold_info;
        count = COUNTOF(gh2_flow_count_threshold_info);
        for (idx = 0; idx < count; idx++) {
            info = select_info + idx;
            rv = bcmi_gh2_tsn_stat_threshold_select_set(unit, select_info,
                                                        count,
                                                        info->stat_type,
                                                        &config);
        }
    }

    return rv;
}

/* Per-chip TSN stat portcnt device info */
STATIC const tsn_stat_portcnt_dev_info_t gh2_tsn_stat_portcnt_dev_info = {
    portcnt_table,
    GH2_TSN_STAT_PORTCNT_TABLE_SIZE
};

/* Per-chip TSN stat flowcnt device info */
STATIC const
tsn_stat_flowcnt_dev_info_t gh2_tsn_stat_flowcnt_dev_info = {
    /* per-chip control drivers */
    bcmi_gh2_tsn_stat_flowcnt_init,
#if defined(BCM_WARM_BOOT_SUPPORT)
    bcmi_gh2_tsn_stat_flowcnt_get_scache_size,
    bcmi_gh2_tsn_stat_flowcnt_sync,
    bcmi_gh2_tsn_stat_flowcnt_reload_l1,
    bcmi_gh2_tsn_stat_flowcnt_reload_l2,
#endif /* BCM_WARM_BOOT_SUPPORT */
    bcmi_gh2_tsn_stat_flowcnt_cleanup,
    bcmi_gh2_tsn_stat_flowcnt_counter_sync_all,
    bcmi_gh2_tsn_stat_flowcnt_counter_sync_stat,
    bcmi_gh2_tsn_stat_flowcnt_group_id_encode,
    bcmi_gh2_tsn_stat_flowcnt_group_id_decode,
    bcmi_gh2_tsn_stat_flowcnt_group_refcnt_get,
    bcmi_gh2_tsn_stat_flowcnt_group_enable_value_set,
    bcmi_gh2_tsn_stat_flowcnt_group_enable_value_get,
    bcmi_gh2_tsn_stat_flowcnt_group_enable_value_exist_check,
    bcmi_gh2_tsn_stat_flowcnt_group_enable_value_insert,
    bcmi_gh2_tsn_stat_flowcnt_group_enable_value_delete,
    bcmi_gh2_tsn_stat_flowcnt_group_enable_value_encode,
    bcmi_gh2_tsn_stat_flowcnt_group_enable_value_decode,
    bcmi_gh2_tsn_stat_flowcnt_group_traverse,
    bcmi_gh2_tsn_stat_flowcnt_counter_access,
    bcmi_gh2_tsn_stat_flowcnt_counter_update,
    bcmi_gh2_tsn_stat_flowcnt_flow_set,
    bcmi_gh2_tsn_stat_flowcnt_flow_get,
    bcmi_gh2_tsn_stat_flowcnt_flow_get_fbmp
};

/* Per-chip TSN stat threshold control drivers */
STATIC const tsn_stat_threshold_ctrl_info_t gh2_tsn_stat_threshold_ctrl_info =
{
    bcmi_gh2_tsn_stat_threshold_init,
    bcmi_gh2_tsn_stat_threshold_set,
    bcmi_gh2_tsn_stat_threshold_get
};

/* Per-chip TSN stat device info */
STATIC const bcmi_esw_tsn_stat_dev_info_t gh2_tsn_stat_devinfo = {
    &gh2_tsn_stat_portcnt_dev_info,
    &gh2_tsn_stat_flowcnt_dev_info,
    &gh2_tsn_stat_threshold_ctrl_info
};

/*
 * Function:
 *    bcmi_gh2_tsn_stat_info_init
 * Description:
 *    Setup the chip specific info for TSN stat.
 * Parameters:
 *    unit - (IN) Unit number
 *    devinfo - (OUT) device info structure.
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_tsn_stat_info_init(
    int unit,
    const bcmi_esw_tsn_stat_dev_info_t **devinfo)
{
    int rv = BCM_E_NONE;

    if (devinfo == NULL) {
        return BCM_E_PARAM;
    }

    /* Return the chip info structure for TSN stat */
    *devinfo = &gh2_tsn_stat_devinfo;

    return rv;
}

#endif  /* BCM_GREYHOUND2_SUPPORT && BCM_TSN_SUPPORT */

