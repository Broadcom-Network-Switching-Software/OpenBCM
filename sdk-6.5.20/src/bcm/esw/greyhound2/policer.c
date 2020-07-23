/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>

#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/mem.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/policer.h>
#include <bcm/field.h>
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/greyhound2.h>
#include <bcm_int/esw/policer.h>
#if defined(BCM_GLOBAL_METER_V2_SUPPORT)

#define GH2_GLOBAL_METER_TCAM_SIZE (SOC_INFO(unit).global_meter_tcam_size)

#define UNKNOWN_PKT                 (0x00)
#define CONTROL_PKT                 (0x01)
#define OAM_PKT                     (0x02)
#define BFD_PKT                     (0x03)
#define BPDU_PKT                    (0x04)
#define ICNM_PKT                    (0x05)
#define PKT_IS_1588                 (0x06)
#define KNOWN_L2UC_PKT              (0x07)

#define UNKNOWN_L2UC_PKT            (0x08)
#define KNOWN_L2MC_PKT              (0x09)
#define UNKNOWN_L2MC_PKT            (0x0a)
#define L2BC_PKT                    (0x0b)
#define KNOWN_L3UC_PKT              (0x0c)
#define UNKNOWN_L3UC_PKT            (0x0d)
#define KNOWN_IPMC_PKT              (0x0e)
#define UNKNOWN_IPMC_PKT            (0x0f)

#define KNOWN_MPLS_L2_PKT           (0x10)
#define UNKNOWN_MPLS_PKT            (0x11)
#define KNOWN_MPLS_L3_PKT           (0x12)
#define KNOWN_MPLS_PKT              (0x13)
#define KNOWN_MPLS_MULTICAST_PKT    (0x14)
#define KNOWN_MIM_PKT               (0x15)
#define UNKNOWN_MIM_PKT             (0x16)
#define KNOWN_TRILL_PKT             (0x17)

#define UNKNOWN_TRILL_PKT           (0x18)
#define KNOWN_NIV_PKT               (0x19)
#define UNKNOWN_NIV_PKT             (0x1a)
#define KNOWN_L2GRE_PKT             (0x1b)
#define KNOWN_VDL2_PKT              (0x1c)
#define KNOWN_FCOE_PKT              (0x1d)
#define UNKNOWN_FCOE_PKT            (0x1e)
#define SAT_DN_SAMP_RX              (0x1f)
#define FP_RESOLUTION_MAX           (0x20)  /* Max value */

/* For those unsupported packets will be mapped to 0 (GH2_UNKNOWN_PKT_VAL) */
#define GH2_UNKNOWN_PKT_VAL                 (0)

#define GH2_CONTROL_PKT_VAL                 (1)
#define GH2_OAM_PKT_VAL                     (18)
#define GH2_BFD_PKT_VAL                     (GH2_UNKNOWN_PKT_VAL)
#define GH2_BPDU_PKT_VAL                    (2)
#define GH2_ICNM_PKT_VAL                    (GH2_UNKNOWN_PKT_VAL)
#define GH2_PKT_IS_1588_VAL                 (19)
#define GH2_KNOWN_L2UC_PKT_VAL              (4)

#define GH2_UNKNOWN_L2UC_PKT_VAL            (5)
#define GH2_KNOWN_L2MC_PKT_VAL              (8)
#define GH2_UNKNOWN_L2MC_PKT_VAL            (9)
#define GH2_L2BC_PKT_VAL                    (3)
#define GH2_KNOWN_L3UC_PKT_VAL              (10)
#define GH2_UNKNOWN_L3UC_PKT_VAL            (11)
#define GH2_KNOWN_IPMC_PKT_VAL              (7)
#define GH2_UNKNOWN_IPMC_PKT_VAL            (6)

#define GH2_KNOWN_MPLS_L2_PKT_VAL           (14)
#define GH2_UNKNOWN_MPLS_PKT_VAL            (15)
#define GH2_KNOWN_MPLS_L3_PKT_VAL           (13)
#define GH2_KNOWN_MPLS_PKT_VAL              (12)
#define GH2_KNOWN_MPLS_MULTICAST_PKT_VAL    (GH2_UNKNOWN_PKT_VAL)
#define GH2_KNOWN_MIM_PKT_VAL               (16)
#define GH2_UNKNOWN_MIM_PKT_VAL             (17)
#define GH2_KNOWN_TRILL_PKT_VAL             (GH2_UNKNOWN_PKT_VAL)

#define GH2_UNKNOWN_TRILL_PKT_VAL           (GH2_UNKNOWN_PKT_VAL)
#define GH2_KNOWN_NIV_PKT_VAL               (20)
#define GH2_UNKNOWN_NIV_PKT_VAL             (21)
#define GH2_KNOWN_L2GRE_PKT_VAL             (GH2_UNKNOWN_PKT_VAL)
#define GH2_KNOWN_VDL2_PKT_VAL              (GH2_UNKNOWN_PKT_VAL)
#define GH2_KNOWN_FCOE_PKT_VAL              (GH2_UNKNOWN_PKT_VAL)
#define GH2_UNKNOWN_FCOE_PKT_VAL            (GH2_UNKNOWN_PKT_VAL)
#define GH2_SAT_DN_SAMP_RX_VAL              (GH2_UNKNOWN_PKT_VAL)

/*
 * Refer to FP_RESOLUTION in reg bcm53570_a0
 */
STATIC const uint8 bcmi_gh2_global_meter_pkt_res[FP_RESOLUTION_MAX] =
{
    GH2_UNKNOWN_PKT_VAL,
    GH2_CONTROL_PKT_VAL,
    GH2_OAM_PKT_VAL,
    GH2_BFD_PKT_VAL,
    GH2_BPDU_PKT_VAL,
    GH2_ICNM_PKT_VAL,
    GH2_PKT_IS_1588_VAL,
    GH2_KNOWN_L2UC_PKT_VAL,

    GH2_UNKNOWN_L2UC_PKT_VAL,
    GH2_KNOWN_L2MC_PKT_VAL,
    GH2_UNKNOWN_L2MC_PKT_VAL,
    GH2_L2BC_PKT_VAL,
    GH2_KNOWN_L3UC_PKT_VAL,
    GH2_UNKNOWN_L3UC_PKT_VAL,
    GH2_KNOWN_IPMC_PKT_VAL,
    GH2_UNKNOWN_IPMC_PKT_VAL,

    GH2_KNOWN_MPLS_L2_PKT_VAL,
    GH2_UNKNOWN_MPLS_PKT_VAL,
    GH2_KNOWN_MPLS_L3_PKT_VAL,
    GH2_KNOWN_MPLS_PKT_VAL,
    GH2_KNOWN_MPLS_MULTICAST_PKT_VAL,
    GH2_KNOWN_MIM_PKT_VAL,
    GH2_UNKNOWN_MIM_PKT_VAL,
    GH2_KNOWN_TRILL_PKT_VAL,

    GH2_UNKNOWN_TRILL_PKT_VAL,
    GH2_KNOWN_NIV_PKT_VAL,
    GH2_UNKNOWN_NIV_PKT_VAL,
    GH2_KNOWN_L2GRE_PKT_VAL,
    GH2_KNOWN_VDL2_PKT_VAL,
    GH2_KNOWN_FCOE_PKT_VAL,
    GH2_UNKNOWN_FCOE_PKT_VAL,
    GH2_SAT_DN_SAMP_RX_VAL
};

/* Parameter NULL error handling */
#define GH2_POL_CHECK_NULL_PARAMETER(_u_, _parameter_)           \
    do {                                                         \
        if (NULL == (_parameter_)) {                             \
            LOG_ERROR(BSL_LS_BCM_POLICER,                        \
                      (BSL_META_U((_u_),                         \
                                  "Error: NULL parameter\n")));  \
            return BCM_E_PARAM;                                  \
        }                                                        \
    } while (0)

/* Function Enter/Leave Tracing */
#define GH2_POL_FUNCTION_TRACE(_u_, _str_)                                 \
    do {                                                                   \
        LOG_VERBOSE(BSL_LS_BCM_POLICER,                                    \
                  (BSL_META_U((_u_),                                       \
                               "%s: " _str_ "\n"), FUNCTION_NAME()));      \
    } while (0)

/* Function DEBUG Tracing */
#define GH2_POL_FUNCTION_DEBUG(_u_, fmt, _args_)                           \
    do {                                                                   \
        LOG_DEBUG(BSL_LS_BCM_POLICER,                                      \
                  (BSL_META_U((_u_),                                       \
                              "%s: " fmt "\n"), FUNCTION_NAME(), _args_)); \
    } while (0)

/* Function ERROR Tracing */
#define GH2_POL_FUNCTION_ERROR(_u_, fmt, _args_)                           \
    do {                                                                   \
        LOG_ERROR(BSL_LS_BCM_POLICER,                                      \
                  (BSL_META_U((_u_),                                       \
                              fmt "\n"), _args_));                         \
    } while (0)

/* Check if bcm_policer_action_t is supported or not */
STATIC int
bcmi_gh2_action_supported(
    int unit,
    bcm_policer_action_t action)
{
    bcm_error_t rv = BCM_E_UNAVAIL;

    switch (action) {
        case bcmPolicerActionGpDrop:
        case bcmPolicerActionYpDrop:
        case bcmPolicerActionRpDrop:
        case bcmPolicerActionGpEcnNew:
        case bcmPolicerActionYpEcnNew:
        case bcmPolicerActionRpEcnNew:
        case bcmPolicerActionGpDscpNew:
        case bcmPolicerActionYpDscpNew:
        case bcmPolicerActionRpDscpNew:
        case bcmPolicerActionGpPrioIntNew:
        case bcmPolicerActionYpPrioIntNew:
        case bcmPolicerActionRpPrioIntNew:
        case bcmPolicerActionGpCngNew:
        case bcmPolicerActionYpCngNew:
        case bcmPolicerActionRpCngNew:
        case bcmPolicerActionGpVlanPrioNew:
        case bcmPolicerActionYpVlanPrioNew:
        case bcmPolicerActionRpVlanPrioNew:
        case bcmPolicerActionGpIntCongestionNotificationNew:
        case bcmPolicerActionYpIntCongestionNotificationNew:
        case bcmPolicerActionRpIntCongestionNotificationNew:
        {
            /* Supported */
            rv = BCM_E_NONE;
            break;
        }
        default:
        {
            /* Not Supported */
            rv = BCM_E_UNAVAIL;
            break;
        }
    }

    return rv;
}

/* Check if bcm_policer_group_mode_type_t is supported or not */
STATIC int
bcmi_gh2_group_mode_type_supported(
    int unit,
    bcm_policer_group_mode_type_t group_mode_type)
{
    /* GH2 doesn't support Cascade metering */
    if (group_mode_type != bcmPolicerGroupModeTypeNormal) {
        GH2_POL_FUNCTION_ERROR(unit,
            "Greyhound2 only support following bcm_policer_group_mode_type_t :"
            "bcmPolicerGroupModeTypeNormal(%d)",
            bcmPolicerGroupModeTypeNormal);
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/* Check if bcm_policer_group_mode_t is supported or not */
STATIC int
bcmi_gh2_group_mode_supported(
    int unit,
    bcm_policer_group_mode_t group_mode)
{
    bcm_error_t rv = BCM_E_UNAVAIL;

    switch (group_mode) {
        case bcmPolicerGroupModeSingle:
        case bcmPolicerGroupModeTrafficType:
        case bcmPolicerGroupModeDlfAll:
        case bcmPolicerGroupModeTyped:
        case bcmPolicerGroupModeTypedAll:
        case bcmPolicerGroupModeSingleWithControl:
        case bcmPolicerGroupModeTrafficTypeWithControl:
        case bcmPolicerGroupModeDlfAllWithControl:
        case bcmPolicerGroupModeTypedWithControl:
        case bcmPolicerGroupModeTypedAllWithControl:
        case bcmPolicerGroupModeDot1P:
        case bcmPolicerGroupModeInnerDot1P:
        case bcmPolicerGroupModeIntPri:
        case bcmPolicerGroupModeShortIntPri:
        case bcmPolicerGroupModeDlfIntPri:
        case bcmPolicerGroupModeTypedIntPri:
        case bcmPolicerGroupModeDlfIntPriWithControl:
        case bcmPolicerGroupModeTypedIntPriWithControl:
        {
            /* Supported */
            rv = BCM_E_NONE;
            break;
        }
        case bcmPolicerGroupModePreemptable:
        {
            if (soc_feature(unit, soc_feature_preemption)) {
                /* Supported */
                rv = BCM_E_NONE;
            } else {
                /* Not Supported */
                rv = BCM_E_UNAVAIL;
            }
            break;
        }
        default:
        {
            /* Not Supported */
            rv = BCM_E_UNAVAIL;
            break;
        }
    }

    return rv;
}

/* Check if bcm_policer_group_mode_attr_t is supported or not */
STATIC int
bcmi_gh2_group_mode_attr_supported(
    int unit,
    bcm_policer_group_mode_attr_t group_mode_attr)
{
    bcm_error_t rv = BCM_E_UNAVAIL;

    switch (group_mode_attr) {
        case bcmPolicerGroupModeAttrFieldIngressColor:
        case bcmPolicerGroupModeAttrIntPri:
        case bcmPolicerGroupModeAttrVlan:
        case bcmPolicerGroupModeAttrOuterPri:
        case bcmPolicerGroupModeAttrInnerPri:
        case bcmPolicerGroupModeAttrPort:
        case bcmPolicerGroupModeAttrTosDscp:
        case bcmPolicerGroupModeAttrPktType:
        case bcmPolicerGroupModeAttrEtherType:
        case bcmPolicerGroupModeAttrOuterVlanCfi:
        case bcmPolicerGroupModeAttrInnerVlanCfi:
        {
            /* Supported */
            rv = BCM_E_NONE;
            break;
        }
        case bcmPolicerGroupModeAttrPreemptable:
        {
            if (soc_feature(unit, soc_feature_preemption)) {
                /* Supported */
                rv = BCM_E_NONE;
            } else {
                /* Not Supported */
                rv = BCM_E_UNAVAIL;
            }
            break;
        }
        default:
        {
            /* Not Supported */
            rv = BCM_E_UNAVAIL;
            break;
        }
    }
    return rv;
}

/* Clear all meter tables */
STATIC int
bcmi_gh2_global_meter_mem_clear(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int i;
    static const soc_mem_t clear_mem_list[] = {
        SVM_METER_TABLEm,
        SVM_POLICY_TABLEm,
        SVC_METER_OFFSET_TCAMm,
        SVC_METER_OFFSET_POLICY_TABLEm
    };

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    for (i = 0;
         i < sizeof(clear_mem_list) / sizeof(soc_mem_t);
         i++) {
        soc_mem_lock(unit, clear_mem_list[i]);
        rv = soc_mem_clear(unit, clear_mem_list[i], MEM_BLOCK_ALL, TRUE);
        soc_mem_unlock(unit, clear_mem_list[i]);
        BCM_IF_ERROR_RETURN(rv);
    }

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/* Clear a range (base_index ~ base_index + numbers) of meter table */
STATIC int
bcmi_gh2_global_meter_mem_range_clear(
    int unit,
    int base_index,
    uint32 numbers)
{
    bcm_error_t rv = BCM_E_NONE;
    soc_mem_t policy_mem = SVM_POLICY_TABLEm;
    soc_mem_t meter_mem = SVM_METER_TABLEm;
    svm_policy_table_entry_t *policy_entry_buf;
    svm_meter_table_entry_t *svm_meter_buf;
    int entry_mem_size;    /* Size of table entry. */
    int entry_index_max = 0;
    int meter_base_index;
    uint32 meter_numbers;


    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Clear base_index ~ base_index + numbers of SVM_POLICY_TABLEm */
    entry_mem_size = sizeof(svm_policy_table_entry_t);
    entry_index_max = base_index + numbers - 1;
    policy_entry_buf =
        soc_cm_salloc(unit, entry_mem_size * numbers,
                      "svm policy table entry");
    if (policy_entry_buf == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(policy_entry_buf, 0, entry_mem_size * numbers);

    soc_mem_lock(unit, policy_mem);
    rv = soc_mem_write_range(
        unit, policy_mem, MEM_BLOCK_ALL, base_index,
        entry_index_max, policy_entry_buf);
    soc_mem_unlock(unit, policy_mem);

    if (policy_entry_buf != NULL) {
        soc_cm_sfree(unit, policy_entry_buf);
    }
    BCM_IF_ERROR_RETURN(rv);

    /* Clear base_index*2 ~ base_index*2 + numbers*2 of SVM_METER_TABLEm */
    meter_base_index = base_index * 2;
    meter_numbers = numbers * 2;

    entry_mem_size = sizeof(svm_meter_table_entry_t);
    entry_index_max = meter_base_index + meter_numbers - 1;
    svm_meter_buf =
        soc_cm_salloc(unit, entry_mem_size * meter_numbers,
                      "svm meter table entry");
    if (svm_meter_buf == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(svm_meter_buf, 0, entry_mem_size * meter_numbers);

    soc_mem_lock(unit, meter_mem);
    rv = soc_mem_write_range(
        unit, meter_mem, MEM_BLOCK_ALL, meter_base_index,
        entry_index_max, svm_meter_buf);
    soc_mem_unlock(unit, meter_mem);

    if (svm_meter_buf != NULL) {
        soc_cm_sfree(unit, svm_meter_buf);
    }

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_gh2_global_meter_ifg_set
 * Purpose:
 *   Enable/Disable Inter Frame Gap(IFG)
 * Parameters:
 *   unit       - (IN) unit number
 *   ifg_enable - (IN) Enable/disable
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_ifg_set(
    int unit,
    uint8 ifg_enable)
{
    uint32 rval = 0, val;

    GH2_POL_FUNCTION_TRACE(unit, "IN");
    GH2_POL_FUNCTION_DEBUG(unit, "ifg_set %d", ifg_enable);

    if (ifg_enable) {
        val = 1;
    } else {
        val = 0;
    }

    BCM_IF_ERROR_RETURN(READ_SVM_TSN_CONTROLr(unit, &rval));
    soc_reg_field_set(unit,
                      SVM_TSN_CONTROLr,
                      &rval,
                      SVM_PACKET_IFG_ENf,
                      val);
    BCM_IF_ERROR_RETURN(WRITE_SVM_TSN_CONTROLr(unit, rval));
    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_global_meter_ifg_get
 * Purpose:
 *   Get Enable/Disable Inter Frame Gap(IFG)
 * Parameters:
 *   unit       - (IN) unit number
 *   ifg_enable - (OUT) Enable/disable
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_ifg_get(
    int unit,
    uint8 *ifg_enable)
{
    uint32 rval = 0;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, ifg_enable);

    BCM_IF_ERROR_RETURN(READ_SVM_TSN_CONTROLr(unit, &rval));
    *ifg_enable = (uint8)soc_reg_field_get(unit,
                                           SVM_TSN_CONTROLr,
                                           rval,
                                           SVM_PACKET_IFG_ENf);

    GH2_POL_FUNCTION_DEBUG(unit, "ifg_set %d", *ifg_enable);
    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_global_meter_enable
 * Purpose:
 *   Enable/Disable SVM Control block
 * Parameters:
 *   unit    - (IN) unit number
 *   enabled - (IN) Enable/disable
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_enable(
    int unit,
    uint8 enable)
{
    uint32 rval = 0, val;

    GH2_POL_FUNCTION_TRACE(unit, "IN");
    GH2_POL_FUNCTION_DEBUG(unit, "enable %d", enable);

    if (enable) {
        val = 0;
    } else {
        val = 1;
    }
    BCM_IF_ERROR_RETURN(READ_SVM_TSN_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, SVM_TSN_CONTROLr, &rval, SVM_BLOCK_DISABLEf, val);
    BCM_IF_ERROR_RETURN(WRITE_SVM_TSN_CONTROLr(unit, rval));

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_global_meter_enable_get
 * Purpose:
 *   Get Enable/Disable of SVM Control block
 * Parameters:
 *   unit    - (IN) unit number
 *   enabled - (OUT) Enable/disable
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_enable_get(
    int unit,
    uint8 *enabled)
{
    uint32 rval = 0, val;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, enabled);

    BCM_IF_ERROR_RETURN(READ_SVM_TSN_CONTROLr(unit, &rval));
    val = soc_reg_field_get(unit, SVM_TSN_CONTROLr, rval, SVM_BLOCK_DISABLEf);

    if (val == 1) {
        *enabled = FALSE;
    } else {
        *enabled = TRUE;
    }

    GH2_POL_FUNCTION_DEBUG(unit, "enabled %d", *enabled);
    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_global_meter_seop_enable
 * Purpose:
 *   Enable/Disable SOP/EOP Metering
 * Note: SEOP Metering
 * In GH2, the SOP cell size is at maximum. 144 bytes. Which means if packet
 * arrives with size lesser than or equal to 144 bytes, both SOP & EOP happen
 * on same instance in IPIPE, and an SEOP signal is generated. For these packet
 * sizes, length of packet (L) is known at SOP itself, which is also EOP.
 * Parameters:
 *   unit    - (IN) unit number
 *   enabled - (IN) Enable/disable
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_seop_enable(
    int unit,
    uint8 enable)
{
    uint32 rval = 0;

    GH2_POL_FUNCTION_TRACE(unit, "IN");
    GH2_POL_FUNCTION_DEBUG(unit, "enable %d", enable);

    BCM_IF_ERROR_RETURN(READ_SVM_TSN_CONTROLr(unit, &rval));
    soc_reg_field_set(unit,
                      SVM_TSN_CONTROLr,
                      &rval,
                      SVM_SEOP_METERING_ENf,
                      enable);
    BCM_IF_ERROR_RETURN(WRITE_SVM_TSN_CONTROLr(unit, rval));

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_global_meter_seop_enable_get
 * Purpose:
 *   Get Enable/Disable of SOP/EOP Metering
 * Note: SEOP Metering
 * In GH2, the SOP cell size is at maximum. 144 bytes. Which means if packet
 * arrives with size lesser than or equal to 144 bytes, both SOP & EOP happen
 * on same instance in IPIPE, and an SEOP signal is generated. For these packet
 * sizes, length of packet (L) is known at SOP itself, which is also EOP.
 * Parameters:
 *   unit    - (IN) unit number
 *   enabled - (OUT) Enable/disable
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_seop_enable_get(
    int unit,
    uint8 *enabled)
{
    uint32 rval = 0;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, enabled);

    BCM_IF_ERROR_RETURN(READ_SVM_TSN_CONTROLr(unit, &rval));
    *enabled = (uint8)soc_reg_field_get(unit,
                                        SVM_TSN_CONTROLr,
                                        rval,
                                        SVM_SEOP_METERING_ENf);

    GH2_POL_FUNCTION_DEBUG(unit, "enabled %d", *enabled);
    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_global_meter_source_order_set
 * Purpose:
 *   Configure the priority of VFP/L2/VXLT/VLAN/Port in the case of
 *   conflict source happened.
 * Parameters:
 *   unit               - (IN) unit number
 *   source_order       - (IN) Service Metering Source Table Priority Config
 *   (The lower index of meter source in config array has higher priority)
 *   source_order_count - (IN) Valid index count in source_order
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_source_order_set(
    int unit,
    bcm_policer_global_meter_source_t *source_order,
    uint32 source_order_count)
{
    int priority, index;
    bcm_policer_global_meter_source_t source;
    soc_field_t source_field = INVALIDf;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, source_order);

    if (source_order_count > 7) {
        return BCM_E_PARAM;
    }

    for (index = 0; index < source_order_count; index++) {
        source = source_order[index];
        if (source > 7) {
            LOG_ERROR(BSL_LS_BCM_POLICER,
                      (BSL_META_U(unit,
                                  "Invalid source(%d).\n"),
                                  source));
            return BCM_E_PARAM;
        }
        /* highest value will have the highest priority */
        priority = source_order_count - index - 1;
        if (source == bcmPolicerGlobalMeterSourceFieldStageLookup) {
            source_field = VFP_TABLEf;
        } else if (source == bcmPolicerGlobalMeterSourceDstMac) {
            source_field = L2_TABLE_DA_LOOKUPf;
        } else if (source == bcmPolicerGlobalMeterSourceSrcMac) {
            source_field = L2_TABLE_SA_LOOKUPf;
        } else if (source == bcmPolicerGlobalMeterSourceVlanTranslateFirst) {
            source_field = VXLT_TABLE_KEY1f;
        } else if (source == bcmPolicerGlobalMeterSourceVlanTranslateSecond) {
            source_field = VXLT_TABLE_KEY2f;
        } else if (source == bcmPolicerGlobalMeterSourceVlan) {
            source_field = VLAN_TABLEf;
        } else if (source == bcmPolicerGlobalMeterSourcePort) {
            source_field = PORT_TABLEf;
            /* need to write LPORT_TABLEf and PORT_TABLEf */
            BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                METER_SOURCE_PRIORITYr,
                REG_PORT_ANY,
                LPORT_TABLEf,
                priority));
        }
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
            METER_SOURCE_PRIORITYr,
            REG_PORT_ANY,
            source_field,
            priority));
    }

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_global_meter_source_order_get
 * Purpose:
 *   Get the priority of VFP/L2/VXLT/VLAN/Port in the case of
 *   conflict source happened.
 * Parameters:
 *   unit               - (IN) unit number
 *   source_order_count - (IN) Valid index count in source_order
 *   source_order       - (OUT) Service Metering Source Table Priority Config
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_source_order_get(
    int unit,
    uint32 source_order_count,
    bcm_policer_global_meter_source_t *source_order)
{
    uint32 rval = 0;
    soc_field_t source_field = INVALIDf;
    int priority, index;
    bcm_policer_global_meter_source_t source;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, source_order);

    if (source_order_count > 7) {
        return BCM_E_PARAM;
    }

    for (source = 0; source < source_order_count; source++) {
        if (source == bcmPolicerGlobalMeterSourceFieldStageLookup) {
            source_field = VFP_TABLEf;
        } else if (source == bcmPolicerGlobalMeterSourceDstMac) {
            source_field = L2_TABLE_DA_LOOKUPf;
        } else if (source == bcmPolicerGlobalMeterSourceSrcMac) {
            source_field = L2_TABLE_SA_LOOKUPf;
        } else if (source == bcmPolicerGlobalMeterSourceVlanTranslateFirst) {
            source_field = VXLT_TABLE_KEY1f;
        } else if (source == bcmPolicerGlobalMeterSourceVlanTranslateSecond) {
            source_field = VXLT_TABLE_KEY2f;
        } else if (source == bcmPolicerGlobalMeterSourceVlan) {
            source_field = VLAN_TABLEf;
        } else if (source == bcmPolicerGlobalMeterSourcePort) {
            source_field = PORT_TABLEf;
        }

        BCM_IF_ERROR_RETURN(READ_METER_SOURCE_PRIORITYr(unit, &rval));
        priority = soc_reg_field_get(unit,
                                     METER_SOURCE_PRIORITYr,
                                     rval,
                                     source_field);
        /* highest value will have the highest priority */
        index = source_order_count - priority - 1;
        source_order[index] = source;
    }

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/* TCAM (SVC_METER_OFFSET_TCAM) fields mapping of bcm_policer_group_mode_t */
const soc_field_t gh2_policer_group_mode_field_list[] =
{
    INVALIDf,
    INVALIDf,
    /* bcmPolicerGroupModeAttrUdf = 1 not used */
    INVALIDf,
    INVALIDf,
    /* bcmPolicerGroupModeAttrFieldIngressColor */
    CNGf,
    CNG_MASKf,
    /* bcmPolicerGroupModeAttrIntPri */
    INT_PRIf,
    INT_PRI_MASKf,
    /* bcmPolicerGroupModeAttrVlan */
    VLAN_FORMATf,
    VLAN_FORMAT_MASKf,
    /* bcmPolicerGroupModeAttrOuterPri */
    OUTER_DOT1Pf,
    OUTER_DOT1P_MASKf,
    /* bcmPolicerGroupModeAttrInnerPri */
    INNER_DOT1Pf,
    INNER_DOT1P_MASKf,
    /* bcmPolicerGroupModeAttrPort */
    INGRESS_PORTf,
    INGRESS_PORT_MASKf,
    /* bcmPolicerGroupModeAttrTosDscp */
    TOSf,
    TOS_MASKf,
    /* bcmPolicerGroupModeAttrTosEcn */
    INVALIDf,
    INVALIDf,
    /* bcmPolicerGroupModeAttrPktType */
    PACKET_RESOLUTIONf,
    PACKET_RESOLUTION_MASKf,
    /* bcmPolicerGroupModeAttrIngNetworkGroup */
    INVALIDf,
    INVALIDf,
    /* bcmPolicerGroupModeAttrDrop */
    INVALIDf,
    INVALIDf,
    /* bcmPolicerGroupModeAttrPacketTypeIp */
    INVALIDf,
    INVALIDf,
    /* bcmPolicerGroupModeAttrPreemptable */
    PREEMPTABLE_CELLf,
    PREEMPTABLE_CELL_MASKf,
    /* bcmPolicerGroupModeAttrEtherType */
    ETHER_TYPEf,
    ETHER_TYPE_MASKf,
    /* bcmPolicerGroupModeAttrOuterVlanCfi */
    OUTER_DOT1Pf,
    OUTER_DOT1P_MASKf,
    /* bcmPolicerGroupModeAttrInnerVlanCfi */
    INNER_DOT1Pf,
    INNER_DOT1P_MASKf
};

/* validate bcm_policer_group_mode_attr_t & value */
STATIC void
bcmi_gh2_group_mode_attr_value_remapping(
    bcm_policer_group_mode_attr_t group_mode_attr,
    uint32 value,
    uint32 value_mask,
    uint32 *hw_value,
    uint32 *hw_value_mask)
{
    uint32 val, val_mask;

    /* Parameter NULL error handling */
    if (hw_value == NULL || hw_value_mask == NULL) {
        return;
    }
    val = value;
    val_mask = value_mask;
    switch (group_mode_attr) {
        case bcmPolicerGroupModeAttrFieldIngressColor:
            /* Re-mapping packet color to CNG value */
            if (val == bcmColorYellow) {
                val = 0x3;
            } else if (val == bcmColorRed) {
                val = 0x1;
            }
            break;
        case bcmPolicerGroupModeAttrVlan:
            if (val == bcmPolicerGroupModeAttrVlanUnTagged) {
                val = 0x0;
            } else if (val == bcmPolicerGroupModeAttrVlanInnerTag) {
                val = 0x1;
            } else if (val == bcmPolicerGroupModeAttrVlanOuterTag) {
                val = 0x2;
            } else if (val == bcmPolicerGroupModeAttrVlanStackedTag) {
                val = 0x3;
            }
            break;
        case bcmPolicerGroupModeAttrOuterPri:
        case bcmPolicerGroupModeAttrInnerPri:
            /* 3bit PRI + 1 bit CFI */
            val = (val << 1);
            /* Skip CFI bit */
            val_mask = val_mask & (~0x1);
            break;
        case bcmPolicerGroupModeAttrOuterVlanCfi:
        case bcmPolicerGroupModeAttrInnerVlanCfi:
            /* 3bit PRI + 1 bit CFI */
            val = val & 0x1;
            /* Skip PRI bit */
            val_mask = val_mask & 0x1;
            break;
        case bcmPolicerGroupModeAttrPktType:
            switch(val) {
                case bcmPolicerGroupModeAttrPktTypeUnknown:
                    val =
                        bcmi_gh2_global_meter_pkt_res[UNKNOWN_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeControl:
                    val =
                        bcmi_gh2_global_meter_pkt_res[CONTROL_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeOAM:
                    val =
                        bcmi_gh2_global_meter_pkt_res[OAM_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeBFD:
                    val =
                        bcmi_gh2_global_meter_pkt_res[BFD_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeBPDU:
                    val =
                        bcmi_gh2_global_meter_pkt_res[BPDU_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeICNM:
                    val =
                        bcmi_gh2_global_meter_pkt_res[ICNM_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktType1588:
                    val =
                        bcmi_gh2_global_meter_pkt_res[PKT_IS_1588];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownL2UC:
                    val =
                        bcmi_gh2_global_meter_pkt_res[KNOWN_L2UC_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeUnknownL2UC:
                    val =
                        bcmi_gh2_global_meter_pkt_res[UNKNOWN_L2UC_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeL2BC:
                    val =
                        bcmi_gh2_global_meter_pkt_res[L2BC_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownL2MC:
                    val =
                        bcmi_gh2_global_meter_pkt_res[KNOWN_L2MC_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeUnknownL2MC:
                    val =
                        bcmi_gh2_global_meter_pkt_res[UNKNOWN_L2MC_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownL3UC:
                    val =
                        bcmi_gh2_global_meter_pkt_res[KNOWN_L3UC_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeUnknownL3UC:
                    val =
                        bcmi_gh2_global_meter_pkt_res[UNKNOWN_L3UC_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownIPMC:
                    val =
                        bcmi_gh2_global_meter_pkt_res[KNOWN_IPMC_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeUnknownIPMC:
                    val =
                        bcmi_gh2_global_meter_pkt_res[UNKNOWN_IPMC_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownMplsL2:
                    val =
                        bcmi_gh2_global_meter_pkt_res[KNOWN_MPLS_L2_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownMplsL3:
                    val =
                        bcmi_gh2_global_meter_pkt_res[KNOWN_MPLS_L3_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownMpls:
                    val =
                        bcmi_gh2_global_meter_pkt_res[KNOWN_MPLS_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeUnknownMpls:
                    val =
                        bcmi_gh2_global_meter_pkt_res[UNKNOWN_MPLS_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownMplsMulticast:
                    val =
                    bcmi_gh2_global_meter_pkt_res[KNOWN_MPLS_MULTICAST_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownMim:
                    val =
                        bcmi_gh2_global_meter_pkt_res[KNOWN_MIM_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeUnknownMim:
                    val =
                        bcmi_gh2_global_meter_pkt_res[UNKNOWN_MIM_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownTrill:
                    val =
                        bcmi_gh2_global_meter_pkt_res[KNOWN_TRILL_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeUnknownTrill:
                    val =
                        bcmi_gh2_global_meter_pkt_res[UNKNOWN_TRILL_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeKnownNiv:
                    val =
                        bcmi_gh2_global_meter_pkt_res[KNOWN_NIV_PKT];
                    break;
                case bcmPolicerGroupModeAttrPktTypeUnknownNiv:
                    val =
                        bcmi_gh2_global_meter_pkt_res[UNKNOWN_NIV_PKT];
                    break;
                default:
                    break;
                }
            val_mask = 0x3F;
            break;
        default:
            break;
    }
    *hw_value = val;
    *hw_value_mask = val_mask;
}

/* validate bcm_policer_group_mode_attr_t & value */
STATIC int
bcmi_gh2_group_mode_attr_validate(
    int unit,
    bcm_policer_group_mode_attr_t group_mode_attr,
    uint32 value)
{
    soc_mem_t meter_mem = SVC_METER_OFFSET_TCAMm;
    soc_field_t field = INVALIDf;
    uint32 hw_value = 0, hw_value_mask = 0;
    uint32 value_mask = 0;

    /* Check if group_mode_attr supported */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_group_mode_attr_supported(
            unit, group_mode_attr));

    /* Skip all cases checking */
    if (value == BCM_POLICER_GROUP_MODE_ATTR_ALL_VALUES) {
        return BCM_E_UNAVAIL;
    } else if (group_mode_attr == bcmPolicerGroupModeAttrVlan &&
               value == bcmPolicerGroupModeAttrVlanAll) {
        return BCM_E_UNAVAIL;
    } else if (group_mode_attr == bcmPolicerGroupModeAttrPktType &&
               value == bcmPolicerGroupModeAttrPktTypeAll) {
        return BCM_E_UNAVAIL;
    }

    /* group_mode_attr field for checking value */
    field = gh2_policer_group_mode_field_list[group_mode_attr * 2];

    if (field == INVALIDf) {
        LOG_ERROR(BSL_LS_BCM_POLICER,
                  (BSL_META_U(unit,
                              "Invalid field.\n"
                              "GH2 doesn't support group_mode_attr(%d)\n"),
                              group_mode_attr));
        return BCM_E_PARAM;
    }

    /* Check if the value fit to HW field */
    bcmi_gh2_group_mode_attr_value_remapping(
        group_mode_attr, value, value_mask, &hw_value, &hw_value_mask);

    if (group_mode_attr == bcmPolicerGroupModeAttrPktType &&
        value != bcmPolicerGroupModeAttrPktTypeUnknown &&
        hw_value == GH2_UNKNOWN_PKT_VAL) {
        /*
         * GH2 doesn't support this pkt resolution,
         * Refer to FP_RESOLUTION in reg bcm53570_a0
         */
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
        soc_mem_field32_fit(
            unit, meter_mem, field, hw_value));
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_global_meter_tcam_keys_set
 * Purpose:
 *   Set the Packet Attribute to TCAM hardware
 * Parameters:
 *   unit                 - (IN) Unit number
 *   pkt_attr_bits_v      - (IN) Packet TCAM attributes
 * Returns:
 *   BCM_E_xxx
 */
STATIC bcm_error_t
bcmi_gh2_global_meter_tcam_keys_set(
    int unit,
    pkt_tcam_attr_bits_t *pkt_attr_bits_v)
{
    uint32 hw_idx;
    int attr;
    uint32 value, value_mask;
    uint32 hw_value = 0, hw_value_mask = 0;
    uint32 fldbuf[SOC_MAX_MEM_WORDS] = { 0 };
    uint8 pkt_attr_selector_enable = 0;
    soc_mem_t meter_mem = SVC_METER_OFFSET_TCAMm;
    soc_mem_t policy_mem = SVC_METER_OFFSET_POLICY_TABLEm;
    soc_field_t field = INVALIDf, mask_field = INVALIDf;
    svc_meter_offset_tcam_entry_t tcam;
    svc_meter_offset_policy_table_entry_t policy;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, pkt_attr_bits_v);

    hw_idx = pkt_attr_bits_v->tcam_idx;

    /* Validate hw_idx */
    if (hw_idx >= GH2_GLOBAL_METER_TCAM_SIZE) {
        LOG_ERROR(BSL_LS_BCM_POLICER,
                  (BSL_META_U(unit,
                              "Invalid hw_idx(%d).\n"),
                              hw_idx));
        return BCM_E_PARAM;
    }

    sal_memset(&tcam, 0,
               sizeof(svc_meter_offset_tcam_entry_t));
    sal_memset(&policy, 0,
               sizeof(svc_meter_offset_policy_table_entry_t));

    /* Write SVC_METER_OFFSET_TCAM start */
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, meter_mem, MEM_BLOCK_ANY, hw_idx, &tcam));

    pkt_attr_selector_enable =
        pkt_attr_bits_v->attr_bits_selector == 0 ? 0 : 1 ;

    /*
     * Write SVC_METER_INDEXf and SVC_METER_OFFSET_MODEf
     * looping all elements in gh2_policer_group_mode_field_list
     */
    for (attr = bcmPolicerGroupModeAttrFieldIngressColor;
         attr <= bcmPolicerGroupModeAttrInnerVlanCfi;
         attr++) {

        /* TCAM value field */
        field = gh2_policer_group_mode_field_list[attr * 2];
        if (field == INVALIDf) {
            continue;
        }
        /* TCAM value mask field */
        mask_field = gh2_policer_group_mode_field_list[attr * 2 + 1];

        if (pkt_attr_selector_enable &&
            pkt_attr_bits_v->value_mask[attr] != 0) {
            value = pkt_attr_bits_v->value[attr];
            value_mask = pkt_attr_bits_v->value_mask[attr];
            GH2_POL_FUNCTION_DEBUG(unit, "TCAM attr %d", attr);
            GH2_POL_FUNCTION_DEBUG(unit, "TCAM value 0x%x", value);
            GH2_POL_FUNCTION_DEBUG(unit, "TCAM value_mask 0x%x", value_mask);
            bcmi_gh2_group_mode_attr_value_remapping(
                attr, value, value_mask, &hw_value, &hw_value_mask);
            GH2_POL_FUNCTION_DEBUG(unit, "TCAM hw_value 0x%x", hw_value);
            GH2_POL_FUNCTION_DEBUG(unit, "TCAM hw_value_mask 0x%x", hw_value_mask);
        } else {
            hw_value = 0;
            hw_value_mask = 0;
        }

        /*
         * PRI & CFI Share the same field, we need to avoid overwrite issue
         * OUTER_DOT1Pf & OUTER_DOT1P_MASKf
         * INNER_DOT1Pf & INNER_DOT1P_MASKf
         */
        if (attr == bcmPolicerGroupModeAttrOuterPri ||
            attr == bcmPolicerGroupModeAttrInnerPri) {
            uint32 fldval = 0;
            soc_mem_field_get(
                unit, meter_mem, (uint32 *)&tcam, field, &fldval);
            /* OR original 1-bit CFI value & mask */
            hw_value |= (fldval & 0x1);
            soc_mem_field_get(
                unit, meter_mem, (uint32 *)&tcam, mask_field, &fldval);
            hw_value_mask |= (fldval & 0x1);
        }

        if (attr == bcmPolicerGroupModeAttrOuterVlanCfi ||
            attr == bcmPolicerGroupModeAttrInnerVlanCfi) {
            uint32 fldval = 0;
            soc_mem_field_get(
                unit, meter_mem, (uint32 *)&tcam, field, &fldval);
            /* OR original 3-bit PRI value & mask */
            hw_value |= (fldval & (0x7 << 1));
            soc_mem_field_get(
                unit, meter_mem, (uint32 *)&tcam, mask_field, &fldval);
            hw_value_mask |= (fldval & (0x7 << 1));
        }

        /* Write value */
        if (SOC_MEM_FIELD_VALID(unit, meter_mem, field)) {
            BCM_IF_ERROR_RETURN(
                soc_mem_field32_fit(
                    unit, meter_mem, field, hw_value));
            soc_mem_field_set(
                unit, meter_mem, (uint32 *)&tcam, field, &hw_value);
        }
        /* Write value_mask */
        if (SOC_MEM_FIELD_VALID(unit, meter_mem, mask_field)) {
            sal_memset(fldbuf, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            sal_memcpy(fldbuf, &hw_value_mask, sizeof(uint32));
            soc_mem_field_width_fit_set(
                unit, meter_mem, (uint32 *)&tcam, mask_field, fldbuf);
        }
    }

    /* UDF0 */
    if (pkt_attr_bits_v->udf.udf0 != 0) {
        if (pkt_attr_selector_enable &&
            pkt_attr_bits_v->udf_mask.udf0 != 0) {
            hw_value = (uint32)pkt_attr_bits_v->udf.udf0;
            hw_value_mask = (uint32)pkt_attr_bits_v->udf_mask.udf0;
        } else {
            hw_value = 0;
            hw_value_mask = 0;
        }
        GH2_POL_FUNCTION_DEBUG(unit, "UDF0 hw_value 0x%x", hw_value);
        GH2_POL_FUNCTION_DEBUG(unit, "UDF0 hw_value_mask 0x%x", hw_value_mask);
        BCM_IF_ERROR_RETURN(
            soc_mem_field32_fit(
                unit, meter_mem, UDF0f, hw_value));
        BCM_IF_ERROR_RETURN(
            soc_mem_field32_fit(
                unit, meter_mem, UDF0_MASKf, hw_value_mask));

        soc_mem_field32_set(
            unit, meter_mem, &tcam, UDF0f, hw_value);
        soc_mem_field32_set(
            unit, meter_mem, &tcam, UDF0_MASKf, hw_value_mask);
        soc_mem_field32_set(
            unit, meter_mem, &tcam, UDF_VALID0f, 1);
        soc_mem_field32_set(
            unit, meter_mem, &tcam, UDF_VALID0_MASKf, 1);
    }

    /* UDF1 */
    if (pkt_attr_bits_v->udf.udf1 != 0) {
        if (pkt_attr_selector_enable &&
            pkt_attr_bits_v->udf_mask.udf1 != 0) {
            hw_value = (uint32)pkt_attr_bits_v->udf.udf1;
            hw_value_mask = (uint32)pkt_attr_bits_v->udf_mask.udf1;
        } else {
            hw_value = 0;
            hw_value_mask = 0;
        }
        GH2_POL_FUNCTION_DEBUG(unit, "UDF1 hw_value 0x%x", hw_value);
        GH2_POL_FUNCTION_DEBUG(unit, "UDF1 hw_value_mask 0x%x", hw_value_mask);
        BCM_IF_ERROR_RETURN(
            soc_mem_field32_fit(
                unit, meter_mem, UDF1f, hw_value));
        BCM_IF_ERROR_RETURN(
            soc_mem_field32_fit(
                unit, meter_mem, UDF1_MASKf, hw_value_mask));

        soc_mem_field32_set(
            unit, meter_mem, &tcam, UDF1f, hw_value);
        soc_mem_field32_set(
            unit, meter_mem, &tcam, UDF1_MASKf, hw_value_mask);
        soc_mem_field32_set(
            unit, meter_mem, &tcam, UDF_VALID1f, 1);
        soc_mem_field32_set(
            unit, meter_mem, &tcam, UDF_VALID1_MASKf, 1);
    }

    /* OFFSET_MODE */
    if (pkt_attr_selector_enable) {
        hw_value = (uint32)pkt_attr_bits_v->offset_mode;
        hw_value_mask = 0x3;
    } else {
        hw_value = 0;
        hw_value_mask = 0;
    }
    GH2_POL_FUNCTION_DEBUG(
        unit, "OFFSET_MODE hw_value 0x%x", hw_value);
    GH2_POL_FUNCTION_DEBUG(
        unit, "OFFSET_MODE hw_value_mask 0x%x", hw_value_mask);
    BCM_IF_ERROR_RETURN(
        soc_mem_field32_fit(
            unit, meter_mem, OFFSET_MODEf, hw_value));

    soc_SVC_METER_OFFSET_TCAMm_field32_set(unit,
        &tcam, OFFSET_MODEf, hw_value);
    soc_SVC_METER_OFFSET_TCAMm_field32_set(unit,
        &tcam, OFFSET_MODE_MASKf, hw_value_mask);

    /* ENABLE / DISABLE pkt_attr_selector */
    soc_SVC_METER_OFFSET_TCAMm_field32_set(unit,
        &tcam, VALIDf, (uint32)pkt_attr_selector_enable);

    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, meter_mem, MEM_BLOCK_ALL, hw_idx, &tcam));
    /* Write SVC_METER_OFFSET_TCAM end */

    /* Write SVC_METER_OFFSET_POLICY_TABLE start */
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, hw_idx, &policy));

    soc_mem_field32_set(
        unit, policy_mem, &policy, FINAL_OFFSET_ENABLEf,
        pkt_attr_selector_enable);

    soc_mem_field32_set(
        unit, policy_mem, &policy, FINAL_OFFSETf,
        pkt_attr_bits_v->final_offset);

    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, hw_idx, &policy));
    /* Write SVC_METER_OFFSET_POLICY_TABLE end */

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_global_meter_tcam_keys_get
 * Purpose:
 *   Get the Packet Attribute from TCAM hardware
 * Parameters:
 *   unit                 - (IN) Unit number
 *   pkt_tcam_attr_bits_t - (OUT) Packet TCAM attributes
 * Returns:
 *   BCM_E_xxx
 */
STATIC bcm_error_t
bcmi_gh2_global_meter_tcam_keys_get(
    int unit,
    pkt_tcam_attr_bits_t *pkt_attr_bits_v)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    GH2_POL_FUNCTION_TRACE(unit, "IN");
    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_gh2_global_meter_tcam_keys_delete
 * Purpose:
 *   Delete the Packet Attribute on TCAM hardware
 * Parameters:
 *   unit   - (IN) Unit number
 *  sw_idx  - (IN) SW memory index.
 * Returns:
 *   BCM_E_xxx
 */
STATIC bcm_error_t
bcmi_gh2_global_meter_tcam_keys_delete(
    int unit,
    uint32 sw_idx)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 hw_idx;
    pkt_tcam_attr_bits_t pkt_attr_bits_v;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    hw_idx = sw_idx;

    /* Validate hw_idx */
    if (hw_idx >= GH2_GLOBAL_METER_TCAM_SIZE) {
        LOG_ERROR(BSL_LS_BCM_POLICER,
                  (BSL_META_U(unit,
                              "Invalid hw_idx(%d).\n"),
                              hw_idx));
        return BCM_E_PARAM;
    }

    /* attr_bits_selector set to 0 will reset all value to 0 */
    (void)pkt_tcam_attr_bits_t_init(&pkt_attr_bits_v);
    pkt_attr_bits_v.tcam_idx = hw_idx;

    rv = bcmi_gh2_global_meter_tcam_keys_set(unit, &pkt_attr_bits_v);
    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/* Write meter_index & meter_offset_mode to source table */
STATIC bcm_error_t
bcmi_gh2_global_meter_policer_to_table_set(
    int unit,
    soc_mem_t mem,
    uint32 mem_index,
    void *mem_data,
    int meter_index,
    int meter_offset_mode,
    int skip_read)
{
    uint32 hw_idx = mem_index;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, mem_data);

    if (!SOC_MEM_IS_VALID(unit, mem) ||
        !SOC_MEM_FIELD_VALID(unit, mem, SVC_METER_INDEXf) ||
        !SOC_MEM_FIELD_VALID(unit, mem, SVC_METER_OFFSET_MODEf)) {
        return BCM_E_INTERNAL;
    }

    /* Validate hw_idx */
    if (hw_idx > (soc_mem_index_max(unit, mem)))
    {
        GH2_POL_FUNCTION_ERROR(unit, "Invalid hw_idx (%d) \n", hw_idx);
        return BCM_E_PARAM;
    }

    if (FALSE == skip_read) {
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, mem_data));
    }

    /* Write SVC_METER_INDEXf and SVC_METER_OFFSET_MODEf */
    soc_mem_field32_set(unit, mem, mem_data,
                        SVC_METER_INDEXf, meter_index);

    soc_mem_field32_set(unit, mem, mem_data,
                        SVC_METER_OFFSET_MODEf, meter_offset_mode);

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/* Read meter_index & meter_offset_mode from source table */
STATIC bcm_error_t
bcmi_gh2_global_meter_policer_to_table_get(
    int unit,
    soc_mem_t mem,
    uint32 mem_index,
    void *mem_data,
    int *meter_index,
    int *meter_offset_mode,
    int skip_read)
{
    uint32 hw_idx = mem_index;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, mem_data);
    GH2_POL_CHECK_NULL_PARAMETER(unit, meter_index);
    GH2_POL_CHECK_NULL_PARAMETER(unit, meter_offset_mode);

    if (!SOC_MEM_IS_VALID(unit, mem) ||
        !SOC_MEM_FIELD_VALID(unit, mem, SVC_METER_INDEXf) ||
        !SOC_MEM_FIELD_VALID(unit, mem, SVC_METER_OFFSET_MODEf)) {
        return BCM_E_INTERNAL;
    }

    /* Validate hw_idx */
    if (hw_idx > (soc_mem_index_max(unit, mem)))
    {
        GH2_POL_FUNCTION_ERROR(unit, "Invalid hw_idx (%d) \n", hw_idx);
        return BCM_E_PARAM;
    }

    if (FALSE == skip_read) {
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, mem_data));
    }

    /* Write SVC_METER_INDEXf and SVC_METER_OFFSET_MODEf */
    *meter_index =
        soc_mem_field32_get(unit, mem, mem_data,
                            SVC_METER_INDEXf);
    *meter_offset_mode =
        soc_mem_field32_get(unit, mem, mem_data,
                            SVC_METER_OFFSET_MODEf);
    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_gh2_global_meter_meter_table_set
 * Purpose:
 *  Update (Install) the policer parameters
 * Parameters:
 *  unit          - (IN) BCM device number.
 *  sw_idx        - (IN) SW memory index.
 *  meter_mode    - (IN) policer meter mode.
 *  meter_flags   - (IN) policer meter flag.
 *  bucket_size   - (IN) Encoded bucket size.
 *  bucket_count  - (IN) Encoded bucket count.
 *  refresh_rate  - (IN) Tokens refresh rate.
 *  granularity   - (IN) Tokens granularity.
 * Returns:
 *  BCM_E_XXX
 */
STATIC bcm_error_t
bcmi_gh2_global_meter_meter_table_set(
    int unit,
    uint32 sw_idx,
    bcm_policer_mode_t meter_mode,
    uint32 meter_flags,
    uint32 average_pkt_size,
    uint32 meter_gran_mode,
    uint32 bucket_size,
    uint32 bucket_count,
    uint32 refresh_rate,
    uint32 granularity)
{
    int rv = BCM_E_NONE;
    uint32 hw_idx = sw_idx;
    svm_meter_table_entry_t entry;
    uint32 refresh_mode = 0, tsn_modified = 0, pkt_bytes = 0;
    soc_mem_t mem = SVM_METER_TABLEm;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Validate hw_idx */
    if (hw_idx > (soc_mem_index_max(unit, mem)))
    {
        GH2_POL_FUNCTION_ERROR(unit, "Invalid hw_idx (%d) \n", hw_idx);
        return BCM_E_PARAM;
    }

    /* Refresh mode is only set to 1 for Single Rate. Other modes get 0 */
    if (meter_mode == bcmPolicerModeSrTcmTsn) {
        refresh_mode = 1;
        tsn_modified = 1;
    } else if ((meter_mode == bcmPolicerModeSrTcm) ||
               (meter_mode == bcmPolicerModeSrTcmModified)) {
        refresh_mode = 1;
        tsn_modified = 0;
    } else {
        refresh_mode = 0;
        tsn_modified = 0;
    }

    /* PKT/BYTE - by default BYTE counter */
    if (meter_flags & BCM_POLICER_MODE_PACKETS) {
        pkt_bytes = 1;
    }
    soc_mem_lock(unit, mem);

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, &entry);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    soc_SVM_METER_TABLEm_field_set(unit, &entry,
                                   BUCKETCOUNTf,
                                   &bucket_count);
    soc_SVM_METER_TABLEm_field_set(unit, &entry,
                                   REFRESHCOUNTf,
                                   &refresh_rate);
    soc_SVM_METER_TABLEm_field_set(unit, &entry,
                                   BUCKETSIZEf,
                                   &bucket_size);
    soc_SVM_METER_TABLEm_field_set(unit, &entry,
                                   REFRESH_MODEf,
                                   &refresh_mode);
    soc_SVM_METER_TABLEm_field_set(unit, &entry,
                                   METER_MODE_TSN_MODIFIERf,
                                   &tsn_modified);
    soc_SVM_METER_TABLEm_field_set(unit, &entry,
                                   PKTS_BYTESf,
                                   &pkt_bytes);
    soc_SVM_METER_TABLEm_field_set(unit, &entry,
                                   METER_GRANf,
                                   &granularity);
    soc_SVM_METER_TABLEm_field_set(unit, &entry,
                                   THRESHOLD_CHECKf,
                                   &average_pkt_size);
    soc_SVM_METER_TABLEm_field_set(unit, &entry,
                                   METER_GRAN_MODEf,
                                   &meter_gran_mode);

    /* write to hw */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_idx, &entry);
    soc_mem_unlock(unit, mem);

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *  bcmi_gh2_global_meter_meter_table_get
 * Purpose:
 *  Get the policer parameters
 * Parameters:
 *  unit          - (IN) BCM device number.
 *  sw_idx        - (IN) SW memory index.
 *  bucket_size   - (OUT) policer meter flag.
 *  bucket_count  - (OUT) Encoded bucket count.
 *  refresh_rate  - (OUT) Tokens refresh rate.
 *  granularity   - (OUT) Tokens granularity.
 *  pkt_bytes     - (OUT) packet mode or byte mode.
 *  tsn_modified  - (OUT) TSN mode or not.
 * Returns:
 *  BCM_E_XXX
 */
STATIC bcm_error_t
bcmi_gh2_global_meter_meter_table_get(
    int unit,
    uint32 sw_idx,
    uint32 *bucket_size,
    uint32 *bucket_count,
    uint32 *refresh_rate,
    uint32 *granularity,
    uint32 *pkt_bytes,
    uint32 *tsn_modified,
    uint32 *meter_gran_mode,
    uint32 *average_pkt_size)
{
    int rv = BCM_E_NONE;
    uint32 hw_idx = sw_idx;
    svm_meter_table_entry_t entry;
    soc_mem_t meter_mem = SVM_METER_TABLEm;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, bucket_size);
    GH2_POL_CHECK_NULL_PARAMETER(unit, bucket_count);
    GH2_POL_CHECK_NULL_PARAMETER(unit, refresh_rate);
    GH2_POL_CHECK_NULL_PARAMETER(unit, granularity);
    GH2_POL_CHECK_NULL_PARAMETER(unit, pkt_bytes);
    GH2_POL_CHECK_NULL_PARAMETER(unit, tsn_modified);
    GH2_POL_CHECK_NULL_PARAMETER(unit, meter_gran_mode);
    GH2_POL_CHECK_NULL_PARAMETER(unit, average_pkt_size);

    /* Validate hw_idx */
    if (hw_idx > (soc_mem_index_max(unit, meter_mem)))
    {
        GH2_POL_FUNCTION_ERROR(unit, "Invalid hw_idx (%d) \n", hw_idx);
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, meter_mem, MEM_BLOCK_ANY, hw_idx, &entry));

    soc_SVM_METER_TABLEm_field_get(unit, &entry,
                                   BUCKETCOUNTf,
                                   bucket_count);
    soc_SVM_METER_TABLEm_field_get(unit, &entry,
                                   REFRESHCOUNTf,
                                   refresh_rate);
    soc_SVM_METER_TABLEm_field_get(unit, &entry,
                                   BUCKETSIZEf,
                                   bucket_size);
    soc_SVM_METER_TABLEm_field_get(unit, &entry,
                                   METER_MODE_TSN_MODIFIERf,
                                   tsn_modified);
    soc_SVM_METER_TABLEm_field_get(unit, &entry,
                                   METER_GRANf,
                                   granularity);
    soc_SVM_METER_TABLEm_field_get(unit, &entry,
                                   PKTS_BYTESf,
                                   pkt_bytes);
    soc_SVM_METER_TABLEm_field_get(unit, &entry,
                                   THRESHOLD_CHECKf,
                                   average_pkt_size);
    soc_SVM_METER_TABLEm_field_get(unit, &entry,
                                   METER_GRAN_MODEf,
                                   meter_gran_mode);

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *  bcmi_gh2_global_meter_meter_table_bitsize_get
 * Purpose:
 *  Get the policer parameters
 * Parameters:
 *  unit          - (IN) BCM device number.
 *  sw_idx        - (IN) SW memory index.
 *  refresh_bitsize  - (OUT) length of REFRESHCOUNTf
 *  bucket_max_bitsize   - (OUT) length of BUCKETSIZEf
 *  bucket_cnt_bitsize  - (OUT) length of BUCKETCOUNTf
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_meter_table_bitsize_get(
    int unit,
    uint32 sw_idx,
    uint32 *refresh_bitsize,
    uint32 *bucket_max_bitsize,
    uint32 *bucket_cnt_bitsize)
{
    int rv = BCM_E_NONE;
    uint32 hw_idx = sw_idx;
    svm_meter_table_entry_t entry;
    soc_mem_t meter_mem = SVM_METER_TABLEm;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, refresh_bitsize);
    GH2_POL_CHECK_NULL_PARAMETER(unit, bucket_max_bitsize);
    GH2_POL_CHECK_NULL_PARAMETER(unit, bucket_cnt_bitsize);

    /* Validate hw_idx */
    if (hw_idx > (soc_mem_index_max(unit, meter_mem)))
    {
        GH2_POL_FUNCTION_ERROR(unit, "Invalid hw_idx(%d)\n", hw_idx);
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, meter_mem, MEM_BLOCK_ANY, hw_idx, &entry));

    *refresh_bitsize =
        soc_mem_field_length(unit, meter_mem, REFRESHCOUNTf);
    *bucket_max_bitsize =
        soc_mem_field_length(unit, meter_mem, BUCKETSIZEf);
    *bucket_cnt_bitsize =
        soc_mem_field_length(unit, meter_mem, BUCKETCOUNTf);

    GH2_POL_FUNCTION_DEBUG(unit, "sw_idx %d", sw_idx);
    GH2_POL_FUNCTION_DEBUG(unit, "refresh_bitsize %d", *refresh_bitsize);
    GH2_POL_FUNCTION_DEBUG(unit, "bucket_max_bitsize %d", *bucket_max_bitsize);
    GH2_POL_FUNCTION_DEBUG(unit, "bucket_cnt_bitsize %d", *bucket_cnt_bitsize);
    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/* Set the policy meter mode and flags to global meter */
STATIC int
bcmi_gh2_global_meter_policy_mode_set(
    int unit,
    uint32 sw_idx,
    bcm_policer_mode_t meter_mode,
    uint32 meter_flags)
{
    int rv = BCM_E_NONE;
    uint32 hw_idx = sw_idx;
    svm_policy_table_entry_t policy_entry;
    soc_mem_t policy_mem = SVM_POLICY_TABLEm;
    uint32 modifier_mode = 0, pair_mode = 0;

    GH2_POL_FUNCTION_TRACE(unit, "IN");
    GH2_POL_FUNCTION_DEBUG(unit, "meter_mode %d", meter_mode);
    GH2_POL_FUNCTION_DEBUG(unit, "meter_flags 0x%x", meter_flags);

    /* Validate hw_idx */
    if (hw_idx > soc_mem_index_max(unit, policy_mem)) {
        GH2_POL_FUNCTION_ERROR(unit, "Invalid hw_idx (%d) \n", hw_idx);
        return BCM_E_PARAM;
    }

    sal_memset(&policy_entry, 0,
               sizeof(svm_policy_table_entry_t));

    /* Refresh mode is only set to 1 for Single Rate. Other modes get 0 */
    switch (meter_mode) {
        case bcmPolicerModeGreen:
            pair_mode = BCM_FIELD_METER_MODE_DEFAULT; /* 0 */
            break;
        case bcmPolicerModeCommitted:
            pair_mode = BCM_FIELD_METER_MODE_FLOW; /* 1 */
            break;
        case bcmPolicerModeTrTcm:
            pair_mode =
                (meter_flags & BCM_POLICER_COLOR_BLIND) ?
                    BCM_FIELD_METER_MODE_trTCM_COLOR_BLIND: /* 2 */
                    BCM_FIELD_METER_MODE_trTCM_COLOR_AWARE; /* 3 */
            break;
        case bcmPolicerModeTrTcmDs:
            pair_mode =
                (meter_flags & BCM_POLICER_COLOR_BLIND) ? 4 : 5;
            break;
        case bcmPolicerModeSrTcmModified:
            modifier_mode = 1;
            /* fall through */
        case bcmPolicerModeSrTcm:
            pair_mode =
                (meter_flags & BCM_POLICER_COLOR_BLIND) ?
                    BCM_FIELD_METER_MODE_srTCM_COLOR_BLIND: /* 6 */
                    BCM_FIELD_METER_MODE_srTCM_COLOR_AWARE; /* 7 */
            break;
        case bcmPolicerModeSrTcmTsn:
            pair_mode =
                (meter_flags & BCM_POLICER_COLOR_BLIND) ?
                    BCM_FIELD_METER_MODE_srTCM_COLOR_BLIND: /* 6 */
                    BCM_FIELD_METER_MODE_srTCM_COLOR_AWARE; /* 7 */
            modifier_mode = 0;
            break;
        default:
            pair_mode = 0;
            break;
    }

    /* SVM_POLICY_TABLEm */
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, hw_idx, &policy_entry));
    GH2_POL_FUNCTION_DEBUG(unit, "pair_mode %d", pair_mode);
    GH2_POL_FUNCTION_DEBUG(unit, "modifier_mode %d", modifier_mode);

    soc_SVM_POLICY_TABLEm_field_set(unit, &policy_entry,
                                    METER_PAIR_MODEf,
                                    &pair_mode);
    soc_SVM_POLICY_TABLEm_field_set(unit, &policy_entry,
                                    METER_PAIR_MODE_MODIFIERf,
                                    &modifier_mode);
    /* write to hw */
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, hw_idx, &policy_entry));

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/* Get the policy meter mode and flags from global meter */
STATIC int
bcmi_gh2_global_meter_policy_mode_get(
    int unit,
    uint32 sw_idx,
    bcm_policer_mode_t *meter_mode,
    uint32 *meter_flags)
{
    int rv = BCM_E_NONE;
    uint32 meter_hw_idx = sw_idx * 2;
    uint32 policy_hw_idx = sw_idx;
    svm_meter_table_entry_t meter_entry;
    svm_policy_table_entry_t policy_entry;
    soc_mem_t meter_mem = SVM_METER_TABLEm;
    soc_mem_t policy_mem = SVM_POLICY_TABLEm;
    uint32 modifier_mode, pair_mode, tsn_modifier;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, meter_mode);
    GH2_POL_CHECK_NULL_PARAMETER(unit, meter_flags);

    /* Validate meter_mem hw_idx */
    if (meter_hw_idx > soc_mem_index_max(unit, meter_mem)) {
        GH2_POL_FUNCTION_ERROR(
            unit, "Invalid meter_hw_idx (%d) \n", meter_hw_idx);
        return BCM_E_PARAM;
    }

    /* Validate policy_mem hw_idx */
    if (policy_hw_idx > soc_mem_index_max(unit, policy_mem)) {
        GH2_POL_FUNCTION_ERROR(
            unit, "Invalid policy_hw_idx (%d) \n", policy_hw_idx);
        return BCM_E_PARAM;
    }

    sal_memset(&meter_entry, 0,
               sizeof(svm_meter_table_entry_t));
    sal_memset(&policy_entry, 0,
               sizeof(svm_policy_table_entry_t));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(
            unit, meter_mem, MEM_BLOCK_ANY, meter_hw_idx, &meter_entry));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(
            unit, policy_mem, MEM_BLOCK_ANY, policy_hw_idx, &policy_entry));

    pair_mode =
        soc_mem_field32_get(unit, policy_mem,
                            &policy_entry, METER_PAIR_MODEf);
    modifier_mode =
        soc_mem_field32_get(unit, policy_mem,
                            &policy_entry, METER_PAIR_MODE_MODIFIERf);

    GH2_POL_FUNCTION_DEBUG(unit, "pair_mode %d", pair_mode);
    GH2_POL_FUNCTION_DEBUG(unit, "modifier_mode %d", modifier_mode);

    switch (pair_mode) {
        case BCM_FIELD_METER_MODE_DEFAULT: /* 0 */
            *meter_mode = bcmPolicerModeGreen;
            break;
        case BCM_FIELD_METER_MODE_FLOW: /* 1 */
            *meter_mode = bcmPolicerModeCommitted;
            break;
        case BCM_FIELD_METER_MODE_trTCM_COLOR_BLIND: /* 2 */
            *meter_flags |= BCM_POLICER_COLOR_BLIND;
            /* Fall thru */
        case BCM_FIELD_METER_MODE_trTCM_COLOR_AWARE: /* 3 */
            *meter_mode = bcmPolicerModeTrTcm;
            break;
        case BCM_FIELD_METER_MODE_srTCM_COLOR_BLIND: /* 6 */
            *meter_flags |= BCM_POLICER_COLOR_BLIND;
            /* Fall thru */
        case BCM_FIELD_METER_MODE_srTCM_COLOR_AWARE: /* 7 */
            *meter_mode = modifier_mode
                          ? bcmPolicerModeSrTcmModified
                          : bcmPolicerModeSrTcm;
            tsn_modifier =
                soc_mem_field32_get(unit, meter_mem, &meter_entry,
                                    METER_MODE_TSN_MODIFIERf);
            if (tsn_modifier == 1) {
                *meter_mode = bcmPolicerModeSrTcmTsn;
            }
            break;
        case 4:
            *meter_flags |= BCM_POLICER_COLOR_BLIND;
            /* Fall thru */
        case 5:
            *meter_mode = bcmPolicerModeTrTcmDs;
            break;
        default:
            ;
    }
    GH2_POL_FUNCTION_DEBUG(unit, "meter_mode %d", *meter_mode);
    GH2_POL_FUNCTION_DEBUG(unit, "meter_flags 0x%x", *meter_flags);
    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/* refer to # FORMAT SVM_ACTIONS */

/* NEW_PKT_PRI (other chip called NEW_DOT1P) */
#define GH2_POLICER_ACTION_PKT_PRI_POS                   (0)
/* NEW_INT_CN */
#define GH2_POLICER_ACTION_INT_CN_BIT_POS                (3)
/* NEW_DSCP_TOS */
#define GH2_POLICER_ACTION_DSCP_BIT_POS                  (5)
/* DROP_PRECEDENCE (other chip called CHANGE_CNG) */
#define GH2_POLICER_ACTION_DROP_PRECEDENCE_BIT_POS       (11)
/* DROP */
#define GH2_POLICER_ACTION_DROP_BIT_POS                  (13)
/* COS_INT_PRI */
#define GH2_POLICER_ACTION_INT_PRI_BIT_POS               (15)
/* CHANGE_PKT_PRI (other chip called CHANGE_DOT1P) */
#define GH2_POLICER_ACTION_CHANGE_PKT_PRI_BIT_POS        (23)
/* CHANGE_INT_CN */
#define GH2_POLICER_ACTION_CHANGE_INT_CN_BIT_POS         (26)
/* CHANGE_ECN */
#define GH2_POLICER_ACTION_CHANGE_ECN_BIT_POS            (27)
/* CHANGE_DSCP_TOS */
#define GH2_POLICER_ACTION_CHANGE_DSCP_BIT_POS           (29)
/* CHANGE_COS_OR_INT_PRI, 2*32 bit value, thus need to minus by 32 */
#define GH2_POLICER_ACTION_CHANGE_COS_OR_INT_PRI_BIT_POS (32 - 32)

/*
 * Function:
 *  bcmi_gh2_global_meter_policy_action_param_configure
 * Purpose:
 *  Update (Install) the policer action parameters
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  g_action - (IN) Green action
 *  y_action - (IN) Yellow action
 *  r_action - (IN) Red action
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_policy_action_param_configure(
    int unit,
    bcm_policer_action_t action,
    uint32 param0,
    uint64 *g_action,
    uint64 *y_action,
    uint64 *r_action)
{
    uint32 green_action[2] = {0}, yellow_action[2] = {0}, red_action[2] = {0};

    GH2_POL_CHECK_NULL_PARAMETER(unit, green_action);
    GH2_POL_CHECK_NULL_PARAMETER(unit, yellow_action);
    GH2_POL_CHECK_NULL_PARAMETER(unit, red_action);

    COMPILER_64_TO_32_LO(green_action[0], *g_action);
    COMPILER_64_TO_32_HI(green_action[1], *g_action);
    COMPILER_64_TO_32_LO(yellow_action[0], *y_action);
    COMPILER_64_TO_32_HI(yellow_action[1], *y_action);
    COMPILER_64_TO_32_LO(red_action[0], *r_action);
    COMPILER_64_TO_32_HI(red_action[1], *r_action);

    switch (action) {
        case bcmPolicerActionGpDrop:
            if (param0 != 0) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for Drop\n")));
                return BCM_E_PARAM;
            }
            SHR_BITSET(green_action,
                GH2_POLICER_ACTION_DROP_BIT_POS);
            break;
        case bcmPolicerActionYpDrop:
            if (param0 != 0) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for Drop\n")));
                return BCM_E_PARAM;
            }
            SHR_BITSET(yellow_action,
                GH2_POLICER_ACTION_DROP_BIT_POS);
           break;
        case bcmPolicerActionRpDrop:
            if (param0 != 0) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for Drop\n")));
                return BCM_E_PARAM;
            }
            SHR_BITSET(red_action,
                GH2_POLICER_ACTION_DROP_BIT_POS);
            break;
        case bcmPolicerActionGpEcnNew:
            if (param0 > 0x3) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for ECN\n")));
                return BCM_E_PARAM;
            }
            /* GH2 merge CHANGE_ECN and NEW_ECN into CHANGE_ECN */
            SHR_BITCLR_RANGE(green_action,
                GH2_POLICER_ACTION_CHANGE_ECN_BIT_POS, 2);
            green_action[0] = green_action[0] |
                   (param0 << GH2_POLICER_ACTION_CHANGE_ECN_BIT_POS);
            break;
        case bcmPolicerActionYpEcnNew:
            if (param0 > 0x3) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for ECN\n")));
                return BCM_E_PARAM;
            }
            /* GH2 merge CHANGE_ECN and NEW_ECN into CHANGE_ECN */
            SHR_BITCLR_RANGE(yellow_action,
                GH2_POLICER_ACTION_CHANGE_ECN_BIT_POS, 2);
            yellow_action[0] = yellow_action[0] |
                   (param0 << GH2_POLICER_ACTION_CHANGE_ECN_BIT_POS);
            break;
        case bcmPolicerActionRpEcnNew:
            if (param0 > 0x3) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for ECN\n")));
                return BCM_E_PARAM;
            }
            /* GH2 merge CHANGE_ECN and NEW_ECN into CHANGE_ECN */
            SHR_BITCLR_RANGE(red_action,
                GH2_POLICER_ACTION_CHANGE_ECN_BIT_POS, 2);
            red_action[0] = red_action[0] |
               (param0 << GH2_POLICER_ACTION_CHANGE_ECN_BIT_POS);
            break;
        case bcmPolicerActionGpDscpNew:
            if (param0 > 0x3f) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for DSCP\n")));
                return BCM_E_PARAM;
            }
            SHR_BITCLR_RANGE(green_action,
                GH2_POLICER_ACTION_DSCP_BIT_POS, 6);
            green_action[0] = green_action[0] |
                (param0 << GH2_POLICER_ACTION_DSCP_BIT_POS);

            /* CHANGE_DSCP_TOS = 3 for CHANGE_DSCP */
            SHR_BITCLR_RANGE(green_action,
                GH2_POLICER_ACTION_CHANGE_DSCP_BIT_POS, 3);
            green_action[0] = green_action[0] |
                (0x3 << GH2_POLICER_ACTION_CHANGE_DSCP_BIT_POS);

            break;
        case bcmPolicerActionYpDscpNew:
            if (param0 > 0x3f) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for DSCP\n")));
                return BCM_E_PARAM;
            }
            SHR_BITCLR_RANGE(yellow_action,
                GH2_POLICER_ACTION_DSCP_BIT_POS, 6);
            yellow_action[0] = yellow_action[0] |
                (param0 << GH2_POLICER_ACTION_DSCP_BIT_POS);

            /* CHANGE_DSCP_TOS = 3 for CHANGE_DSCP */
            SHR_BITCLR_RANGE(yellow_action,
                GH2_POLICER_ACTION_CHANGE_DSCP_BIT_POS, 3);
            yellow_action[0] = yellow_action[0] |
                (0x3 << GH2_POLICER_ACTION_CHANGE_DSCP_BIT_POS);
            break;
        case bcmPolicerActionRpDscpNew:
            if (param0 > 0x3f) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for DSCP\n")));
                return BCM_E_PARAM;
            }
            SHR_BITCLR_RANGE(red_action,
                GH2_POLICER_ACTION_DSCP_BIT_POS, 6);
            red_action[0] = red_action[0] |
                (param0 << GH2_POLICER_ACTION_DSCP_BIT_POS);

            /* CHANGE_DSCP_TOS = 3 for CHANGE_DSCP */
            SHR_BITCLR_RANGE(red_action,
                GH2_POLICER_ACTION_CHANGE_DSCP_BIT_POS, 3);
            red_action[0] = red_action[0] |
                (0x3 << GH2_POLICER_ACTION_CHANGE_DSCP_BIT_POS);
            break;
        case bcmPolicerActionGpPrioIntNew:
            if (param0 > 0xf) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for int pri\n")));
                return BCM_E_PARAM;
            }
            /* COS_INT_PRI */
            SHR_BITCLR_RANGE(green_action,
                GH2_POLICER_ACTION_INT_PRI_BIT_POS, 4);
            green_action[0] = green_action[0] |
                (param0 <<
                 GH2_POLICER_ACTION_INT_PRI_BIT_POS);
            /* CHANGE_COS_OR_INT_PRI = 5 for CHANGE_INT_PRIORITY_NEW */
            green_action[1] = green_action[1] |
                (0x5 << GH2_POLICER_ACTION_CHANGE_COS_OR_INT_PRI_BIT_POS);
            break;
        case bcmPolicerActionYpPrioIntNew:
            if (param0 > 0xf) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for int pri\n")));
                return BCM_E_PARAM;
            }
            /* COS_INT_PRI */
            SHR_BITCLR_RANGE(yellow_action,
                GH2_POLICER_ACTION_INT_PRI_BIT_POS, 4);
            yellow_action[0] = yellow_action[0] |
                (param0 <<
                 GH2_POLICER_ACTION_INT_PRI_BIT_POS);
            /* CHANGE_COS_OR_INT_PRI = 5 for CHANGE_INT_PRIORITY_NEW */
            yellow_action[1] = yellow_action[1] |
                (0x5 << GH2_POLICER_ACTION_CHANGE_COS_OR_INT_PRI_BIT_POS);
            break;
        case bcmPolicerActionRpPrioIntNew:
            if (param0 > 0xf) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for int pri\n")));
                return BCM_E_PARAM;
            }
            /* COS_INT_PRI */
            SHR_BITCLR_RANGE(red_action,
                GH2_POLICER_ACTION_INT_PRI_BIT_POS, 4);
            red_action[0] = red_action[0] |
                (param0 <<
                 GH2_POLICER_ACTION_INT_PRI_BIT_POS);
            /* CHANGE_COS_OR_INT_PRI = 5 for CHANGE_INT_PRIORITY_NEW */
            red_action[1] = red_action[1] |
                (0x5 << GH2_POLICER_ACTION_CHANGE_COS_OR_INT_PRI_BIT_POS);
            break;
        case bcmPolicerActionGpCngNew:
            if (param0 > 0x3) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for CNG\n")));
                return BCM_E_PARAM;
            }
            /* DROP_PRECEDENCE (other chip called CHANGE_CNG) */
            SHR_BITCLR_RANGE(green_action,
                GH2_POLICER_ACTION_DROP_PRECEDENCE_BIT_POS, 2);
            green_action[0] = green_action[0] |
                (param0 <<
                 GH2_POLICER_ACTION_DROP_PRECEDENCE_BIT_POS);
            break;
        case bcmPolicerActionYpCngNew:
            if (param0 > 0x3) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for CNG\n")));
                return BCM_E_PARAM;
            }
            /* DROP_PRECEDENCE (other chip called CHANGE_CNG) */
            SHR_BITCLR_RANGE(yellow_action,
                GH2_POLICER_ACTION_DROP_PRECEDENCE_BIT_POS, 2);
            yellow_action[0] = yellow_action[0] |
                (param0 <<
                 GH2_POLICER_ACTION_DROP_PRECEDENCE_BIT_POS);
            break;
        case bcmPolicerActionRpCngNew:
            if (param0 > 0x3) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for CNG\n")));
                return BCM_E_PARAM;
            }
            /* DROP_PRECEDENCE (other chip called CHANGE_CNG) */
            SHR_BITCLR_RANGE(red_action,
                GH2_POLICER_ACTION_DROP_PRECEDENCE_BIT_POS, 2);
            red_action[0] = red_action[0] |
                (param0 <<
                 GH2_POLICER_ACTION_DROP_PRECEDENCE_BIT_POS);
            break;
        case bcmPolicerActionGpVlanPrioNew:
            if (param0 > 0x7) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for vlan pri\n")));
                return BCM_E_PARAM;
            }
            /* NEW_PKT_PRI */
            SHR_BITCLR_RANGE(green_action,
                GH2_POLICER_ACTION_PKT_PRI_POS, 3);
            green_action[0] = green_action[0] |
                (param0 <<
                 GH2_POLICER_ACTION_PKT_PRI_POS);

            /* CHANGE_PKT_PRI = 5 for NEW_PKT_PRI */
            SHR_BITCLR_RANGE(green_action,
                GH2_POLICER_ACTION_CHANGE_PKT_PRI_BIT_POS, 3);
            green_action[0] = green_action[0] |
                (0x5 << GH2_POLICER_ACTION_CHANGE_PKT_PRI_BIT_POS);
            break;
        case bcmPolicerActionYpVlanPrioNew:
            if (param0 > 0x7) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for vlan pri\n")));
                return BCM_E_PARAM;
            }
            /* NEW_PKT_PRI */
            SHR_BITCLR_RANGE(yellow_action,
                GH2_POLICER_ACTION_PKT_PRI_POS, 3);
            yellow_action[0] = yellow_action[0] |
                (param0 <<
                 GH2_POLICER_ACTION_PKT_PRI_POS);

            /* CHANGE_PKT_PRI = 5 for NEW_PKT_PRI */
            SHR_BITCLR_RANGE(yellow_action,
                GH2_POLICER_ACTION_CHANGE_PKT_PRI_BIT_POS, 3);
            yellow_action[0] = yellow_action[0] |
                (0x5 << GH2_POLICER_ACTION_CHANGE_PKT_PRI_BIT_POS);
            break;
        case bcmPolicerActionRpVlanPrioNew:
            if (param0 > 0x7) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for vlan pri\n")));
                return BCM_E_PARAM;
            }
            /* NEW_PKT_PRI */
            SHR_BITCLR_RANGE(red_action,
                GH2_POLICER_ACTION_PKT_PRI_POS, 3);
            red_action[0] = red_action[0] |
                (param0 <<
                 GH2_POLICER_ACTION_PKT_PRI_POS);

            /* CHANGE_PKT_PRI = 5 for NEW_PKT_PRI */
            SHR_BITCLR_RANGE(red_action,
                GH2_POLICER_ACTION_CHANGE_PKT_PRI_BIT_POS, 3);
            red_action[0] = red_action[0] |
                (0x5 << GH2_POLICER_ACTION_CHANGE_PKT_PRI_BIT_POS);
            break;
        case bcmPolicerActionGpIntCongestionNotificationNew:
            if (param0 > 0x3) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for "
                                     "Congestion Notification\n")));
                return BCM_E_PARAM;
            }
            SHR_BITCLR_RANGE(green_action,
                GH2_POLICER_ACTION_INT_CN_BIT_POS, 2);
            green_action[0] = green_action[0] |
                (param0 << GH2_POLICER_ACTION_INT_CN_BIT_POS);

            /* CHANGE_INT_CN = 1 for NEW_INT_CN */
            green_action[0] = green_action[0] |
                (0x1 << GH2_POLICER_ACTION_CHANGE_INT_CN_BIT_POS);
            break;
        case bcmPolicerActionYpIntCongestionNotificationNew:
            if (param0 > 0x3) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for "
                                     "Congestion Notification\n")));
                return BCM_E_PARAM;
            }
            SHR_BITCLR_RANGE(yellow_action,
                GH2_POLICER_ACTION_INT_CN_BIT_POS, 2);
            yellow_action[0] = yellow_action[0] |
                (param0 << GH2_POLICER_ACTION_INT_CN_BIT_POS);

            /* CHANGE_INT_CN = 1 for NEW_INT_CN */
            yellow_action[0] = yellow_action[0] |
                (0x1 << GH2_POLICER_ACTION_CHANGE_INT_CN_BIT_POS);
            break;
        case bcmPolicerActionRpIntCongestionNotificationNew:
            if (param0 > 0x3) {
                LOG_ERROR(BSL_LS_BCM_POLICER,
                         (BSL_META_U(unit,
                                     "Invalid value for "
                                     "Congestion Notification\n")));
                return BCM_E_PARAM;
            }
            SHR_BITCLR_RANGE(red_action,
                GH2_POLICER_ACTION_INT_CN_BIT_POS, 2);
            red_action[0] = red_action[0] |
                (param0 << GH2_POLICER_ACTION_INT_CN_BIT_POS);

            /* CHANGE_INT_CN = 1 for NEW_INT_CN */
            red_action[0] = red_action[0] |
                (0x1 << GH2_POLICER_ACTION_CHANGE_INT_CN_BIT_POS);
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_POLICER,
                      (BSL_META_U(unit,
                                  "Unsupported Action specified\n")));
            return BCM_E_PARAM;
    }

    COMPILER_64_SET(*g_action, green_action[1], green_action[0]);
    COMPILER_64_SET(*y_action, yellow_action[1], yellow_action[0]);
    COMPILER_64_SET(*r_action, red_action[1], red_action[0]);
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_gh2_global_meter_policy_action_set
 * Purpose:
 *  Update (Install) the policer action parameters
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  g_action - (IN) Green action
 *  y_action - (IN) Yellow action
 *  r_action - (IN) Red action
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_policy_action_set(
    int unit,
    uint32 sw_idx,
    uint64 g_action,
    uint64 y_action,
    uint64 r_action)
{
    int rv = BCM_E_NONE;
    uint32 hw_idx = sw_idx;
    soc_mem_t policy_mem = SVM_POLICY_TABLEm;
    svm_policy_table_entry_t policy_entry;

    GH2_POL_FUNCTION_TRACE(unit, "IN");
    GH2_POL_FUNCTION_DEBUG(unit, "sw_idx %d", sw_idx);
    GH2_POL_FUNCTION_DEBUG(unit, "Green H 0x%x", COMPILER_64_HI(g_action));
    GH2_POL_FUNCTION_DEBUG(unit, "Green L 0x%x", COMPILER_64_LO(g_action));
    GH2_POL_FUNCTION_DEBUG(unit, "Yellow H 0x%x", COMPILER_64_HI(y_action));
    GH2_POL_FUNCTION_DEBUG(unit, "Yellow L 0x%x", COMPILER_64_LO(y_action));
    GH2_POL_FUNCTION_DEBUG(unit, "Red H 0x%x", COMPILER_64_HI(r_action));
    GH2_POL_FUNCTION_DEBUG(unit, "Red L 0x%x", COMPILER_64_LO(r_action));

    /* Validate hw_idx */
    if (hw_idx > soc_mem_index_max(unit, policy_mem)) {
        LOG_ERROR(BSL_LS_BCM_POLICER,
                  (BSL_META_U(unit,
                              "Invalid hw_idx \n")));
        return BCM_E_PARAM;
    }

    sal_memset(&policy_entry, 0,
               sizeof(svm_policy_table_entry_t));

    BCM_IF_ERROR_RETURN(
        READ_SVM_POLICY_TABLEm(unit, MEM_BLOCK_ANY, hw_idx, &policy_entry));

    soc_mem_field64_set(unit, policy_mem, &policy_entry, G_ACTIONSf, g_action);
    soc_mem_field64_set(unit, policy_mem, &policy_entry, Y_ACTIONSf, y_action);
    soc_mem_field64_set(unit, policy_mem, &policy_entry, R_ACTIONSf, r_action);

    BCM_IF_ERROR_RETURN(
        WRITE_SVM_POLICY_TABLEm(unit, MEM_BLOCK_ALL, hw_idx, &policy_entry));

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *  bcmi_gh2_global_meter_policy_action_get
 * Purpose:
 *  Get the policer action parameters
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  g_action - (OUT) Green action
 *  y_action - (OUT) Yellow action
 *  r_action - (OUT) Red action
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
bcmi_gh2_global_meter_policy_action_get(
    int unit,
    uint32 sw_idx,
    uint64 *g_action,
    uint64 *y_action,
    uint64 *r_action)
{
    int rv = BCM_E_NONE;
    uint32 hw_idx = sw_idx;
    soc_mem_t policy_mem = SVM_POLICY_TABLEm;
    svm_policy_table_entry_t policy_entry;

    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, g_action);
    GH2_POL_CHECK_NULL_PARAMETER(unit, y_action);
    GH2_POL_CHECK_NULL_PARAMETER(unit, r_action);

    /* Validate hw_idx */
    if (hw_idx > soc_mem_index_max(unit, policy_mem)) {
        GH2_POL_FUNCTION_ERROR(unit, "Invalid hw_idx (%d) \n", hw_idx);
        return BCM_E_PARAM;
    }

    sal_memset(&policy_entry, 0,
               sizeof(svm_policy_table_entry_t));

    BCM_IF_ERROR_RETURN(
        READ_SVM_POLICY_TABLEm(unit, MEM_BLOCK_ANY, hw_idx, &policy_entry));
    soc_mem_field64_get(
        unit, policy_mem, &policy_entry, G_ACTIONSf, g_action);
    soc_mem_field64_get(
        unit, policy_mem, &policy_entry, Y_ACTIONSf, y_action);
    soc_mem_field64_get(
        unit, policy_mem, &policy_entry, R_ACTIONSf, r_action);

    GH2_POL_FUNCTION_DEBUG(unit, "sw_idx %d", sw_idx);
    GH2_POL_FUNCTION_DEBUG(unit, "Green H 0x%x", COMPILER_64_HI(*g_action));
    GH2_POL_FUNCTION_DEBUG(unit, "Green L 0x%x", COMPILER_64_LO(*g_action));
    GH2_POL_FUNCTION_DEBUG(unit, "Yellow H 0x%x", COMPILER_64_HI(*y_action));
    GH2_POL_FUNCTION_DEBUG(unit, "Yellow L 0x%x", COMPILER_64_LO(*y_action));
    GH2_POL_FUNCTION_DEBUG(unit, "Red H 0x%x", COMPILER_64_HI(*r_action));
    GH2_POL_FUNCTION_DEBUG(unit, "Red L 0x%x", COMPILER_64_LO(*r_action));
    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

STATIC const bcmi_global_meter_dev_info_t bcmi_gh2_global_meter_dev_info = {
    "gh2_global_meter_dev_info",                    /* table_name */
    bcmi_gh2_action_supported,                      /* action_supported */
    bcmi_gh2_group_mode_supported,                  /* group_mode_supported */
    bcmi_gh2_group_mode_type_supported,        /* group_mode_type_supported */
    bcmi_gh2_group_mode_attr_supported,        /* group_mode_attr_supported */
    bcmi_gh2_group_mode_attr_validate,          /* group_mode_attr_validate */
    bcmi_gh2_global_meter_mem_clear,                /* mem_clear */
    bcmi_gh2_global_meter_mem_range_clear,          /* mem_range_clear */
    bcmi_gh2_global_meter_ifg_set,                  /* ifg_set */
    bcmi_gh2_global_meter_ifg_get,                  /* ifg_get */
    bcmi_gh2_global_meter_source_order_set,         /* source_order_set */
    bcmi_gh2_global_meter_source_order_get,         /* source_order_get */
    bcmi_gh2_global_meter_enable,                   /* svm_enable */
    bcmi_gh2_global_meter_enable_get,               /* svm_enable_get */
    bcmi_gh2_global_meter_seop_enable,              /* seop_enable */
    bcmi_gh2_global_meter_seop_enable_get,          /* seop_enable_get */
    bcmi_gh2_global_meter_tcam_keys_set,            /* tcam_keys_set */
    bcmi_gh2_global_meter_tcam_keys_get,            /* tcam_keys_get */
    bcmi_gh2_global_meter_tcam_keys_delete,         /* tcam_keys_delete */
    bcmi_gh2_global_meter_policer_to_table_set,     /* policer_to_table_set */
    bcmi_gh2_global_meter_policer_to_table_get,     /* policer_to_table_get */
    bcmi_gh2_global_meter_meter_table_bitsize_get,/* meter_table_bitsize_get */
    bcmi_gh2_global_meter_meter_table_set,          /* meter_table_set */
    bcmi_gh2_global_meter_meter_table_get,          /* meter_table_get */
    bcmi_gh2_global_meter_policy_mode_set,          /* policy_mode_set */
    bcmi_gh2_global_meter_policy_mode_get,          /* policy_mode_get */
    bcmi_gh2_global_meter_policy_action_param_configure,
                                        /* policy_action_param_configure */
    bcmi_gh2_global_meter_policy_action_set,        /* policy_action_set */
    bcmi_gh2_global_meter_policy_action_get         /* policy_action_get */
};

/*
 * Function:
 *  bcmi_gh2_global_meter_dev_info_init
 * Description:
 *  Set-up the chip specific info.
 * Parameters:
 *  unit    - (IN) Unit number.
 *  dev_info - (OUT) device info structure.
 * Returns:
 *  BCM_E_XXX
 */
int
bcmi_gh2_global_meter_dev_info_init(
    int unit,
    const bcmi_global_meter_dev_info_t **dev_info)
{
    GH2_POL_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    GH2_POL_CHECK_NULL_PARAMETER(unit, dev_info);

    /* Return the chip info structure */
    *dev_info = &bcmi_gh2_global_meter_dev_info;

    GH2_POL_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}
#endif /* BCM_GLOBAL_METER_V2_SUPPORT */
#endif /* BCM_GREYHOUND2_SUPPORT */



