/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    lpm.c
 * Purpose: Trident3 L3 LPM specific functions
 */

#include <shared/bsl.h>

#include <bcm/error.h>
#include <soc/defs.h>
#include <soc/drv.h>

#if defined(INCLUDE_L3) && defined(BCM_TRIDENT3_SUPPORT) 

#include <bcm/l3.h>
#include <soc/lpm.h>
#include <soc/format.h>
#include <soc/esw/flow_db.h>
#include <bcm_int/esw/lpmv6.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/flow.h>

/*
 * Function:    
 *     _bcm_defip_pair128_flex_ip6_addr_set
 * Purpose:  
 *     Set IP6 address field in memory from ip6 addr type. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) view id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (IN) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_bcm_defip_pair128_flex_ip6_addr_set(int unit, soc_mem_t view_id, uint32 *entry, 
                                const ip6_addr_t ip6)
{
    uint32              ip6_field[4];

    if (SOC_MEM_FIELD_VALID(unit, view_id, IPV6__DIPf)) {
        soc_mem_ip6_addr_set(unit, view_id, entry,IPV6__DIPf, ip6, 0);
    } else if (SOC_MEM_FIELD_VALID(unit, view_id, IPV6__SIPf)) {
        soc_mem_ip6_addr_set(unit, view_id, entry,IPV6__SIPf, ip6, 0);
    } else {
        ip6_field[3] = ((ip6[12] << 24)| (ip6[13] << 16) |
                    (ip6[14] << 8) | (ip6[15] << 0));
        soc_mem_field32_set(unit, view_id, entry, IP_ADDR0_LWRf, ip6_field[3]);
        ip6_field[2] = ((ip6[8] << 24) | (ip6[9] << 16) |
                    (ip6[10] << 8) | (ip6[11] << 0));
        soc_mem_field32_set(unit, view_id, entry, IP_ADDR1_LWRf, ip6_field[2]);
        ip6_field[1] = ((ip6[4] << 24) | (ip6[5] << 16) |
                    (ip6[6] << 8)  | (ip6[7] << 0));
        soc_mem_field32_set(unit, view_id, entry, IP_ADDR0_UPRf, ip6_field[1]);
        ip6_field[0] = ((ip6[0] << 24) | (ip6[1] << 16) |
                    (ip6[2] << 8)  | (ip6[3] << 0));
        soc_mem_field32_set(unit, view_id, entry, IP_ADDR1_UPRf, ip6_field[0]);
    }
}

/*
 * Function:    
 *     _bcm_defip_pair128_flex_ip6_mask_set
 * Purpose:  
 *     Set IP6 mask field in memory from ip6 addr type. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) View id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (IN) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_bcm_defip_pair128_flex_ip6_mask_set(int unit, soc_mem_t view_id, uint32 *entry, 
                                const ip6_addr_t ip6)
{
    uint32              ip6_field[4];

    if (SOC_MEM_FIELD_VALID(unit, view_id, IPV6__MASKf)) {
        soc_mem_ip6_addr_set(unit, view_id, entry,IPV6__MASKf, ip6, 0);
    } else {
        ip6_field[3] = ((ip6[12] << 24)| (ip6[13] << 16) |
                        (ip6[14] << 8) | (ip6[15] << 0));
        soc_mem_field32_set(unit, view_id, entry, IP_ADDR_MASK0_LWRf, ip6_field[3]);
        ip6_field[2] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        soc_mem_field32_set(unit, view_id, entry, IP_ADDR_MASK1_LWRf, ip6_field[2]);
        ip6_field[1] = ((ip6[4] << 24) | (ip6[5] << 16) |
                        (ip6[6] << 8)  | (ip6[7] << 0));
        soc_mem_field32_set(unit, view_id, entry, IP_ADDR_MASK0_UPRf, ip6_field[1]);
        ip6_field[0] = ((ip6[0] << 24) | (ip6[1] << 16) |
                        (ip6[2] << 8)  | (ip6[3] << 0));
        soc_mem_field32_set(unit, view_id, entry, IP_ADDR_MASK1_UPRf, ip6_field[0]);
    }
}
/*
 * Function:
 *      _bcm_defip_pair128_flex_entry_clear
 * Purpose:
 *      Clear/Reset a single flex route entry. 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      view_id   - (IN)View ID number.
 *      hw_index  - (IN)Entry index in the tcam. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_defip_pair128_flex_entry_clear(int unit, soc_mem_t view_id, int hw_index)  
{
    uint32 hw_entry[SOC_MAX_MEM_FIELD_WORDS];
    soc_field_t key_field[4], mask_field[4];
    int field_count, i;
    uint64 val64;
    key_field[0] = KEY0_UPRf;
    key_field[1] = KEY1_UPRf;
    key_field[2] = KEY0_LWRf;
    key_field[3] = KEY1_LWRf;
    mask_field[0] = MASK0_UPRf;
    mask_field[1] = MASK1_UPRf;
    mask_field[2] = MASK0_LWRf;
    mask_field[3] = MASK1_LWRf;
    field_count = 4;
    sal_memset(hw_entry, 0, WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS));
    COMPILER_64_ZERO(val64);
    for (i = 0; i < field_count; i++) {
        if (SOC_MEM_FIELD_VALID(unit, view_id, key_field[i])) { 
            soc_mem_field64_set(unit, view_id, hw_entry, key_field[i], val64);
        }
        if (SOC_MEM_FIELD_VALID(unit, view_id, mask_field[i])) { 
            soc_mem_field64_set(unit, view_id, hw_entry, mask_field[i], val64);
        }
    }

    /* Reset hw entry. */
    BCM_IF_ERROR_RETURN
        (BCM_XGS3_MEM_WRITE(unit, view_id, hw_index,
                            &soc_mem_entry_null(unit, view_id)));
    BCM_IF_ERROR_RETURN
        (BCM_XGS3_MEM_WRITE(unit, view_id, hw_index, hw_entry));

    /* Reset sw entry. */
    BCM_DEFIP_PAIR128_ENTRY_SET(unit, hw_index, 0, 0);

    /* Reset urpf source lookup entry. */
    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF (Ex: KT2).
     */
    if (SOC_URPF_STATUS_GET(unit) &&
        !soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_WRITE(unit, view_id, 
                            (hw_index + BCM_DEFIP_PAIR128_URPF_OFFSET(unit)),
                            &soc_mem_entry_null(unit, view_id)));
        BCM_IF_ERROR_RETURN
            (BCM_XGS3_MEM_WRITE(unit, view_id, 
                 (hw_index+ BCM_DEFIP_PAIR128_URPF_OFFSET(unit)), hw_entry));
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_l3_defip_pair128_flex_add
 * Purpose:
 *      Add an flex entry to internal L3_DEFIP_PAIR_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)defip information. 
 *      view_id     - (IN)View ID number.
 *      hw_entry    - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_l3_defip_pair128_flex_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                              int nh_ecmp_idx, uint32 *hw_entry) 
{
    bcm_ip6_t mask;                             /* Subnet mask.              */
    int hw_index = 0;                           /* Entry offset in the TCAM. */ 
    int rv = BCM_E_NONE;                        /* Operation return status.  */

    soc_mem_t view_id;
    int     i;
    uint32  field_array[128];
    uint32  field_count;
    uint32  opaque_array[16];
    uint32  opaque_count;
    bcm_flow_logical_field_t* user_fields;

    uint32 action_set = 0;

    uint8 process_destination = 0;
    uint8 process_class_id = 0;
    uint8 process_qos = 0;
    uint8 process_pim_sm = 0;
    uint8 process_fwd_attributes = 0;
    uint8 process_fwd_3_attributes = 0;
    soc_mem_t phy_mem;

#ifdef BCM_HELIX5_SUPPORT
    uint32  entry_data[SOC_MAX_MEM_FIELD_WORDS];
    uint32  entry_hit;
#endif

    /* Input parameters check. */
    if (lpm_cfg == NULL) {
        return (BCM_E_PARAM);
    }

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);

    /* Allocate a new index for inserted entry. */
    if (BCM_XGS3_L3_INVALID_INDEX == lpm_cfg->defip_index) {
        rv = _bcm_defip_pair128_idx_alloc(unit, lpm_cfg, &hw_index, nh_ecmp_idx);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        hw_index = lpm_cfg->defip_index;
    }

    BCM_IF_ERROR_RETURN(
        soc_flow_db_ffo_to_mem_view_id_get(unit,
                          lpm_cfg->defip_flow_handle,
                          lpm_cfg->defip_flow_option_handle,
                          SOC_FLOW_DB_FUNC_L3_ROUTE_ID,
                          (uint32 *)&view_id));

    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, hw_entry);

    BCM_IF_ERROR_RETURN(rv);

    /* Get logical KEY field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                            16,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = lpm_cfg->defip_logical_fields;

    for (i=0; i<opaque_count; i++) {
        if (user_fields[i].id == opaque_array[i]) {
            soc_mem_field32_set(unit, view_id, hw_entry, user_fields[i].id,
                                user_fields[i].value);
        }
    }

    /* Get logical PDD field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                            16,
                            opaque_array,
                            &opaque_count);

    BCM_IF_ERROR_RETURN(rv);

    user_fields = lpm_cfg->defip_logical_fields;

    for (i=0; i<opaque_count; i++) {
        if (user_fields[i].id == opaque_array[i]) {
            soc_mem_field32_set(unit, view_id, hw_entry, user_fields[i].id,
                                user_fields[i].value);
        }
    }

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            field_array,
                            &field_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<field_count; i++) {
        switch (field_array[i]) {
            case DESTINATION_ACTION_SETf:
                process_destination = 1;
                break;
            case CLASS_ID_ACTION_SETf:
                process_class_id = 1;
                break;
            case QOS_ACTION_SETf:
                process_qos = 1;
                break;
            case FWD_ATTRIBUTES_ACTION_SETf:
                process_fwd_attributes = 1;
                break;
            case FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf:
                process_fwd_3_attributes = 1;
                break;
            case PIM_SM_ACTION_SETf:
                process_pim_sm = 1;
                break;
            default:
                return BCM_E_INTERNAL;
        }       
    }

    /* Set hit bit. */
    if (lpm_cfg->defip_flags & BCM_L3_HIT) {
        if (SOC_MEM_FIELD_VALID(unit, view_id, HITf)) {
            soc_mem_field32_set(unit, view_id, hw_entry, HITf, 1);
        }
        if (SOC_MEM_FIELD_VALID(unit, view_id, HIT0f)) {
            soc_mem_field32_set(unit, view_id, hw_entry, HIT0f, 1);
        }
        if (SOC_MEM_FIELD_VALID(unit, view_id, HIT1f)) {
            soc_mem_field32_set(unit, view_id, hw_entry, HIT1f, 1);
        }
    }

    /* Set priority override bit. */
    action_set = 0;

    if (lpm_cfg->defip_flags & BCM_L3_RPE) {
        if (!process_qos) {
            return BCM_E_UNAVAIL;
        }
        soc_format_field32_set(unit, QOS_ACTION_SETfmt,
                               &action_set, RPEf, 1);
    }
    if (process_qos) {
        /* Write priority field. */
        soc_format_field32_set(unit, QOS_ACTION_SETfmt,
                           &action_set, PRIf, lpm_cfg->defip_prio);

        soc_mem_field32_set(unit, view_id, hw_entry,
                        QOS_ACTION_SETf, action_set);
    }

    /* Set classification group id. */
    if (process_class_id) {
        /* CLASS_IDf */
        soc_mem_field32_set(unit, view_id, hw_entry, CLASS_ID_ACTION_SETf,
                            lpm_cfg->defip_lookup_class);
    }

    /* Fill next hop information. */
    if (!process_destination) {
        return BCM_E_INTERNAL;
    }

    action_set = 0;
    if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, DEST_TYPE1f, BCMI_TD3_DEST_TYPE1_ECMP);
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, ECMP_GROUPf, nh_ecmp_idx);
    } else {
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, DEST_TYPE0f, BCMI_TD3_DEST_TYPE0_NH);
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, NEXT_HOP_INDEXf, nh_ecmp_idx);
    }
    soc_mem_field32_set(unit, view_id, hw_entry,
                        DESTINATION_ACTION_SETf, action_set);

    /* Set destination discard flag. */
    action_set = 0;
    if (lpm_cfg->defip_flags & BCM_L3_DST_DISCARD) {
        if (!process_fwd_attributes) {
            return BCM_E_UNAVAIL;
        }
        soc_format_field32_set(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, DST_DISCARDf, 1);
        soc_mem_field32_set(unit, view_id, hw_entry,
                             FWD_ATTRIBUTES_ACTION_SETf, action_set);
    }

    if (soc_feature(unit, soc_feature_ipmc_defip) &&
            (lpm_cfg->defip_flags & BCM_L3_IPMC)) {

        /* L3MC_INDEXf */
        action_set = 0;
        if (!process_pim_sm) {
            return BCM_E_UNAVAIL;
        }

        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, DEST_TYPE1f, BCMI_TD3_DEST_TYPE1_IPMC);
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, IPMC_GROUPf, lpm_cfg->defip_mc_group);
        soc_mem_field32_set(unit, view_id, hw_entry,
                          DESTINATION_ACTION_SETf, action_set);

        /*
         * RP ID has be encoded to 14 bit value, Now it will fit in EXPECTED_L3_IIFf
         */
        action_set = 0;
        if (lpm_cfg->defip_l3a_rp != BCM_IPMC_RP_ID_INVALID) {
            soc_format_field32_set(unit, PIM_SM_ACTION_SETfmt,
                           &action_set, EXPECTED_L3_IIFf,
                           _BCM_DEFIP_IPMC_RP_SET(lpm_cfg->defip_l3a_rp));

        } else if ((lpm_cfg->defip_ipmc_flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK) &&
                (lpm_cfg->defip_expected_intf != 0))  {

            soc_format_field32_set(unit, PIM_SM_ACTION_SETfmt,
                           &action_set, EXPECTED_L3_IIFf,
                           lpm_cfg->defip_expected_intf);

            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_DROP) {
                soc_format_field32_set(unit, PIM_SM_ACTION_SETfmt,
                           &action_set, IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf, 1);
            }
            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_TOCPU) {
                soc_format_field32_set(unit, PIM_SM_ACTION_SETfmt,
                           &action_set, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf, 1);
            }
        }
        soc_mem_field32_set(unit, view_id, hw_entry,
                          PIM_SM_ACTION_SETf, action_set);
    }

    if ((!SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_MASK0_LWRf) ||
        !SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_MASK1_LWRf) ||
        !SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_MASK0_UPRf) ||
        !SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_MASK1_UPRf)) &&
        (!SOC_MEM_FIELD_VALID(unit, view_id, IPV6__MASKf))) {
        return BCM_E_INTERNAL;
    }

    /* Set destination/source ip address. */
    _bcm_defip_pair128_flex_ip6_addr_set(unit, view_id, hw_entry,
                                 lpm_cfg->defip_ip6_addr);

    /* Set ip mask. */
    _bcm_defip_pair128_flex_ip6_mask_set(unit, view_id, hw_entry, mask);

    /* Set vrf id & vrf mask. */
    action_set = 0;
    if (BCM_L3_VRF_OVERRIDE != lpm_cfg->defip_vrf) {
        if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_0f) ||
            SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_1f)) {
            if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_0f)) {
                soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_0f, lpm_cfg->defip_vrf);
                soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK0f,
                                (1 << soc_mem_field_length(unit, view_id,
                                                     VRF_ID_MASK0f)) - 1);
            }
            if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_1f)) {
                soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_1f, lpm_cfg->defip_vrf);
                soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK1f,
                                (1 << soc_mem_field_length(unit, view_id,
                                                     VRF_ID_MASK1f)) - 1);
            }
        } else {
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_0_LWRf, lpm_cfg->defip_vrf);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_1_LWRf, lpm_cfg->defip_vrf);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_0_UPRf, lpm_cfg->defip_vrf);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_1_UPRf, lpm_cfg->defip_vrf);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK0_LWRf,
                                (1 << soc_mem_field_length(unit, view_id,
                                                     VRF_ID_MASK0_LWRf)) - 1);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK1_LWRf,
                                (1 << soc_mem_field_length(unit, view_id,
                                                     VRF_ID_MASK1_LWRf)) - 1);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK0_UPRf,
                                (1 << soc_mem_field_length(unit, view_id,
                                                         VRF_ID_MASK0_UPRf)) - 1);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK1_LWRf,
                                (1 << soc_mem_field_length(unit, view_id,
                                                         VRF_ID_MASK1_UPRf)) - 1);
        }
        if (process_fwd_3_attributes) {
            soc_format_field32_set(unit, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, GLOBAL_ROUTEf, 0);
            soc_mem_field32_set(unit, view_id, hw_entry,
                             FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf, action_set);
        }
    } else {
        if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_0f) ||
            SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_1f)) {
            if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_0f)) {
                soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_0f, 0);
                soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK0f, 0);
            }
            if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_1f)) {
                soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_1f, 0);
                soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK1f, 0);
            }
        } else {
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_0_LWRf, 0);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_1_LWRf, 0);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_0_UPRf, 0);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_1_UPRf, 0);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK0_LWRf, 0);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK1_LWRf, 0);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK0_UPRf, 0);
            soc_mem_field32_set(unit, view_id, hw_entry, VRF_ID_MASK1_UPRf, 0);
        }
    }

    /* Configure entry for SIP lookup if URPF is enabled. This => SIP is default route.*/
    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF (Ex: KT2).
     */
    action_set = 0;
    if (soc_feature(unit, soc_feature_l3_defip_advanced_lookup) &&
        process_fwd_3_attributes) {
        soc_format_field32_set(unit, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETfmt,
                           &action_set, DEFAULT_ROUTEf,
                           (SOC_URPF_STATUS_GET(unit) ? 1 : 0));
        soc_mem_field32_set(unit, view_id, hw_entry,
                         FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf, action_set);
    }

    SOC_IF_ERROR_RETURN(soc_mem_view_phy_mem_get(unit, view_id, &phy_mem));

    /* Write buffer to hw. */
#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(unit,soc_feature_update_defip_pair_data_only) &&
        (phy_mem == L3_DEFIP_PAIR_128m) &&
        (lpm_cfg->defip_flags & BCM_L3_REPLACE) &&
        (BCM_XGS3_L3_INVALID_INDEX != lpm_cfg->defip_index)) {
        /* Zero buffers. */
        sal_memset(entry_data, 0, WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS));
        soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, hw_entry, DATA_RANGEf, entry_data);
        rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128_DATA_ONLYm,
           hw_index, entry_data);
        if (BCM_SUCCESS(rv)) {
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, hw_entry, HITf, &entry_hit);
            rv =BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128_HIT_ONLYm,
                    hw_index, &entry_hit);
        }
    } else
#endif
    {
        rv = BCM_XGS3_MEM_WRITE(unit, phy_mem, hw_index, hw_entry);
    }
    if (BCM_FAILURE(rv)) {
        BCM_DEFIP_PAIR128_ENTRY_SET(unit, hw_index, 0, 0);
        return (rv);
    }

    /* Write source lookup portion of the entry. */
    /* If soc_feature_l3_defip_advanced_lookup is TRUE then 
     * both DIP and SIP lookup is done using 1 L3_DEFIP entry.
     * L3_DEFIP table is not divided into 2 to support URPF (Ex: KT2).
     */
    action_set = 0;
    if (SOC_URPF_STATUS_GET(unit) &&
        !soc_feature(unit, soc_feature_l3_defip_advanced_lookup)) {
        if (!process_fwd_attributes) {
            return BCM_E_PARAM;
        }
        soc_format_field32_set(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, SRC_DISCARDf, 0);
        soc_mem_field32_set(unit, view_id, hw_entry,
                            FWD_ATTRIBUTES_ACTION_SETf, action_set);
#if defined(BCM_HELIX5_SUPPORT)
        if (soc_feature(unit, soc_feature_update_defip_pair_data_only) &&
                (phy_mem == L3_DEFIP_PAIR_128m) &&
                (lpm_cfg->defip_flags & BCM_L3_REPLACE) &&
                (BCM_XGS3_L3_INVALID_INDEX != lpm_cfg->defip_index)) {
            /* Zero buffers. */
            sal_memset(entry_data, 0, WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS));
            soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, hw_entry, DATA_RANGEf, entry_data);
            rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128_DATA_ONLYm,
                (hw_index + BCM_DEFIP_PAIR128_URPF_OFFSET(unit)), entry_data);
            if (BCM_SUCCESS(rv)) {
                soc_mem_field_get(unit, L3_DEFIP_PAIR_128m, hw_entry, HITf, &entry_hit);
                rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128_HIT_ONLYm,
                        (hw_index + BCM_DEFIP_PAIR128_URPF_OFFSET(unit)), &entry_hit);
            }
        } else
#endif
        {
            rv = BCM_XGS3_MEM_WRITE(unit, L3_DEFIP_PAIR_128m,
                    (hw_index + BCM_DEFIP_PAIR128_URPF_OFFSET(unit)),
                    hw_entry);
        }
        if (BCM_FAILURE(rv)) {
            _bcm_defip_pair128_flex_entry_clear(unit, view_id, hw_index);
            return (rv);
        }
    }

    /* If new route added increment total number of routes.  */
    /* Lack of index indicates a new route. */
    if (BCM_XGS3_L3_INVALID_INDEX == lpm_cfg->defip_index) {
        BCM_XGS3_L3_DEFIP_CNT_INC(unit, TRUE);
        BCM_DEFIP_PAIR128_USED_COUNT(unit)++;
        soc_lpm_stat_128b_count_update(unit, 1);
    }

    return rv;

}
/*
 * Function:
 *     _bcm_td3_flex_ip6_defip_lwr_set
 * Purpose:
 *    Set an IP6 address field in L3_DEFIPm at index n
 * Parameters:
 *    unit    - (IN) SOC unit number;
 *    lpm_key - (OUT) Buffer to fill.
 *    lpm_cfg - (IN) Route information.
 *    view_id - (IN) Memory view id 
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_td3_flex_ip6_defip_lwr_set(int unit, void *lpm_key, _bcm_defip_cfg_t *lpm_cfg, soc_mem_t view_id)
{
    uint8 *ip6;                 /* Ip6 address.       */
    uint8 mask[BCM_IP6_ADDRLEN];        /* Subnet mask.       */
    uint32 ip6_word;            /* Temp storage.      */
    int idx;

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

   /* Apply subnet mask */
    idx = lpm_cfg->defip_sub_len / 8;   /* Unchanged byte count.    */
    if (idx < BCM_IP6_ADDRLEN) {
        ip6[idx] &= mask[idx];      /* Apply mask on next byte. */
    }
    for (idx++; idx < BCM_IP6_ADDRLEN; idx++) {
        ip6[idx] = 0;           /* Reset rest of bytes.     */
    }

    ip6_word = ((ip6[8] << 24) | (ip6[9] << 16) | (ip6[10] << 8) | (ip6[11]));
    soc_mem_field32_set(unit, view_id, lpm_key, IP_ADDR1f, ip6_word);

    ip6_word = ((ip6[12] << 24) | (ip6[13] << 16) | (ip6[14] << 8) | (ip6[15]));
    soc_mem_field32_set(unit, view_id, lpm_key, IP_ADDR0f, ip6_word);

    ip6_word = ((mask[8] << 24) | (mask[9] << 16) | (mask[10] << 8) |
               (mask[11]));
    soc_mem_field32_set(unit, view_id, lpm_key, IP_ADDR_MASK1f, ip6_word);

    ip6_word = ((mask[12] << 24) | (mask[13] << 16) | (mask[14] << 8) |
               (mask[15]));
    soc_mem_field32_set(unit, view_id, lpm_key, IP_ADDR_MASK0f, ip6_word);
}
/*
 * Function:
 *     _bcm_td3_flex_ip6_defip_set
 * Purpose:
 *    Set an IP6 address field in L3_DEFIPm
 * Parameters:
 *    unit    - (IN) SOC unit number;
 *    lpm_key - (OUT) Buffer to fill.
 *    lpm_cfg - (IN) Route information.
 *    view_id - (IN) Memory view id 
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_td3_flex_ip6_defip_set(int unit, void *lpm_key, _bcm_defip_cfg_t *lpm_cfg, soc_mem_t view_id)
{
    uint8 *ip6;                 /* Ip6 address.       */
    uint8 mask[BCM_IP6_ADDRLEN];        /* Subnet mask.       */
    uint32 ip6_word;            /* Temp storage.      */
    int idx;                    /* Iteration index .  */

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply subnet mask */
    idx = lpm_cfg->defip_sub_len / 8;   /* Unchanged byte count.    */
    ip6[idx] &= mask[idx];      /* Apply mask on next byte. */
    for (idx++; idx < BCM_IP6_ADDRLEN; idx++) {
        ip6[idx] = 0;           /* Reset rest of bytes.     */
    }


    ip6_word = ((ip6[0] << 24) | (ip6[1] << 16) | (ip6[2] << 8) | (ip6[3]));
    soc_mem_field32_set(unit, view_id, lpm_key, IP_ADDR1f, ip6_word);

    ip6_word = ((ip6[4] << 24) | (ip6[5] << 16) | (ip6[6] << 8) | (ip6[7]));
    soc_mem_field32_set(unit, view_id, lpm_key, IP_ADDR0f, ip6_word);

    ip6_word = ((mask[0] << 24) | (mask[1] << 16) | (mask[2] << 8) | (mask[3]));
    soc_mem_field32_set(unit, view_id, lpm_key, IP_ADDR_MASK1f, ip6_word);

    ip6_word = ((mask[4] << 24) | (mask[5] << 16) | (mask[6] << 8) | (mask[7]));
    soc_mem_field32_set(unit, view_id, lpm_key, IP_ADDR_MASK0f, ip6_word);
}
/*
 * Function:
 *      _bcm_td3_lpm_flex_ent_init
 * Purpose:
 *      Service routine used to initialize lkup key for flex lpm entry.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_cfg   - (IN)Prefix info.
 *      lpm_entry - (OUT)Hw buffer to fill.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_lpm_flex_ent_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
                     uint32  *lpm_entry)
{
    bcm_ip_t ip4_mask;
    int vrf_id;
    int vrf_mask;
    int ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6;
    int mode = 1;
    soc_mem_t view_id;
    uint32 action_set;

    /* Extract entry  vrf id  & vrf mask. */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_internal_lpm_vrf_calc(unit, lpm_cfg, &vrf_id, &vrf_mask));

    BCM_IF_ERROR_RETURN(
        soc_flow_db_ffo_to_mem_view_id_get(unit,
                          lpm_cfg->defip_flow_handle,
                          lpm_cfg->defip_flow_option_handle,
                          SOC_FLOW_DB_FUNC_L3_ROUTE_ID,
                          (uint32 *)&view_id));

    if (!(SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_MASK0f) ||
        SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_MASK1f))) {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_entry_init(unit, view_id, lpm_entry));

    if (SOC_MEM_FIELD_VALID(unit, view_id, KEY_MODE0_UPRf)) {
        mode = soc_mem_field32_get(unit, view_id, lpm_entry, KEY_MODE0_UPRf);
    } else if (SOC_MEM_FIELD_VALID(unit, view_id, KEY_MODE0f)) {
        mode = soc_mem_field32_get(unit, view_id, lpm_entry, KEY_MODE0f);
    }
    /* Set prefix ip address & mask. */
    if (ipv6) {
         if (mode == 3) {
            _bcm_td3_flex_ip6_defip_lwr_set(unit, lpm_entry, lpm_cfg, view_id);
         } else {
            _bcm_td3_flex_ip6_defip_set(unit, lpm_entry, lpm_cfg, view_id);
         }
    } else {
        ip4_mask = BCM_IP4_MASKLEN_TO_ADDR(lpm_cfg->defip_sub_len);
        /* Apply subnet mask. */
        lpm_cfg->defip_ip_addr &= ip4_mask;

        /* Set address to the buffer. */
        soc_mem_field32_set(unit, view_id, lpm_entry, IP_ADDR0f,
                                  lpm_cfg->defip_ip_addr);
        soc_mem_field32_set(unit, view_id, lpm_entry, IP_ADDR_MASK0f, ip4_mask);
    }

    /* Set Virtual Router id if supported. */
    if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_0f)) {
        soc_mem_field32_set(unit, view_id, lpm_entry, VRF_ID_0f, vrf_id);
        soc_mem_field32_set(unit, view_id, lpm_entry, VRF_ID_MASK0f, vrf_mask);
    }

    if (ipv6) {
        /* Set Virtual Router id if supported. */
        if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_1f)) {
            soc_mem_field32_set(unit, view_id, lpm_entry, VRF_ID_1f, vrf_id);
            soc_mem_field32_set(unit, view_id, lpm_entry, VRF_ID_MASK1f, vrf_mask);
        }
    }

    action_set = 0;
    if (SOC_MEM_FIELD_VALID(unit, view_id, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf)) {
        if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
            soc_format_field32_set(unit, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, GLOBAL_ROUTEf, 1);
        }
        if (BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) {
            soc_format_field32_set(unit, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, GLOBAL_ROUTEf, 1);
        }
        soc_mem_field32_set(unit, view_id, lpm_entry,
                     FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf, action_set);
    }
    return (BCM_E_NONE);
}
int
_bcm_td3_lpm_prepare_flex_defip_entry(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                int nh_ecmp_idx, uint32  *lpm_entry)
{
    int     i,j;
    int     rv;
    soc_mem_t view_id;
    uint32  field_array[128];
    uint32  field_count;
    uint32  opaque_array[16];
    uint32  opaque_count;
    bcm_flow_logical_field_t* user_fields;

    uint32 action_set = 0;

    uint8 process_destination = 0;
    uint8 process_class_id = 0;
    uint8 process_qos = 0;
    uint8 process_pim_sm = 0;
    uint8 process_fwd_attributes = 0;
    uint8 process_fwd_3_attributes = 0;
    uint8 process_vrf = 0;
    uint8 process_vrf_mask = 0;

    if (NULL == lpm_cfg || NULL == lpm_entry) {
        return BCM_E_PARAM;
    }

    /* This routine handles only IP4 entries in L3_DEFIP */
    if (lpm_cfg->defip_flags & BCM_L3_IP6) {
        return BCM_E_PARAM;
    }

    /* Set hit bit. */
    if (lpm_cfg->defip_flags & BCM_L3_HIT) {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, HIT0f, 1);
    }

    BCM_IF_ERROR_RETURN(
        soc_flow_db_ffo_to_mem_view_id_get(unit,
                          lpm_cfg->defip_flow_handle,
                          lpm_cfg->defip_flow_option_handle,
                          SOC_FLOW_DB_FUNC_L3_ROUTE_ID,
                          (uint32 *)&view_id));

    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, (uint32 *) lpm_entry);

    BCM_IF_ERROR_RETURN(rv);

    /* Get logical KEY field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                            16,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = lpm_cfg->defip_logical_fields;

    for (j = 0; j < lpm_cfg->defip_num_of_fields; j++) {
        for (i=0; i<opaque_count; i++) {
            if (user_fields[j].id == opaque_array[i]) {
                soc_mem_field32_set(unit, view_id, lpm_entry, user_fields[j].id,
                                user_fields[j].value);
                break;
            }
        }
    }

    /* Get logical PDD field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                            16,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = lpm_cfg->defip_logical_fields;

    for (j = 0; j < lpm_cfg->defip_num_of_fields; j++) {
        for (i=0; i<opaque_count; i++) {
            if (user_fields[j].id == opaque_array[i]) {
                soc_mem_field32_set(unit, view_id, lpm_entry, user_fields[j].id,
                                user_fields[j].value);
                break;
            }
        }
    }

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            field_array,
                            &field_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<field_count; i++) {
        switch (field_array[i]) {
            case DESTINATION_ACTION_SETf:
                process_destination = 1;
                break;
            case CLASS_ID_ACTION_SETf:
                process_class_id = 1;
                break;
            case QOS_ACTION_SETf:
                process_qos = 1;
                break;
            case FWD_ATTRIBUTES_ACTION_SETf:
                process_fwd_attributes = 1;
                break;
            case FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf:
                process_fwd_3_attributes = 1;
                break;
            case PIM_SM_ACTION_SETf:
                process_pim_sm = 1;
                break;
            case VRF_ID_0f:
                process_vrf = 1;
                break;
            case VRF_ID_MASK0f:
                process_vrf_mask = 1;
                break;
            default:
                break;
        }       
    }

    action_set = 0;
    if (process_vrf) { 
        soc_mem_field32_set(unit, view_id, lpm_entry,
                         VRF_ID_0f, lpm_cfg->defip_vrf);
    }

    if (process_vrf_mask) { 
        soc_mem_field32_set(unit, view_id, lpm_entry,
                         VRF_ID_MASK0f, 0xfff);
    }

    /* Configure entry for SIP lookup if URPF is enabled.
     * For devices like KT2 both DIP and SIP lookup is done
     * using only 1 L3_DEFIP entry, unlike other devices where
     * L3_DEFIP table is divided in 2 parts to support URPF.
     * RPExf and DEFAULTROUTExf are not overlayed in KT2 like previous devices.
     */
    action_set = 0;
    if (soc_feature(unit, soc_feature_l3_defip_advanced_lookup) &&
        process_fwd_3_attributes) {
        /* DEFAULTROUTE0f */
        soc_format_field32_set(unit, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETfmt,
                           &action_set, DEFAULT_ROUTEf,
                           (SOC_URPF_STATUS_GET(unit) ? 1 : 0));
        soc_mem_field32_set(unit, view_id, lpm_entry,
                         FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf, action_set);
 
    }

    /* Set priority override bit. */
    action_set = 0;

    if (lpm_cfg->defip_flags & BCM_L3_RPE) {
        if (!process_qos) {
            return BCM_E_UNAVAIL;
        }
        soc_format_field32_set(unit, QOS_ACTION_SETfmt,
                               &action_set, RPEf, 1);
    }

    /* Write priority field. */
    soc_format_field32_set(unit, QOS_ACTION_SETfmt,
                           &action_set, PRIf, lpm_cfg->defip_prio);

    if (process_qos) {
        soc_mem_field32_set(unit, view_id, lpm_entry,
                        QOS_ACTION_SETf, action_set);
    }

    /* Fill next hop information. */
    if (!process_destination) {
        return BCM_E_INTERNAL;
    }

    action_set = 0;

    if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, DEST_TYPE1f, BCMI_TD3_DEST_TYPE1_ECMP);
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, ECMP_GROUPf, nh_ecmp_idx);
 
    } else if (nh_ecmp_idx != BCM_XGS3_L3_INVALID_INDEX) {
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, DEST_TYPE0f, BCMI_TD3_DEST_TYPE0_NH);
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, NEXT_HOP_INDEXf, nh_ecmp_idx);
    }
    soc_mem_field32_set(unit, view_id, lpm_entry,
                        DESTINATION_ACTION_SETf, action_set);

    /* Set destination discard flag. */
    action_set = 0;
    if (lpm_cfg->defip_flags & BCM_L3_DST_DISCARD) {
        if (!process_fwd_attributes) {
            return BCM_E_UNAVAIL;
        }
        soc_format_field32_set(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, DST_DISCARDf, 1);
        soc_mem_field32_set(unit, view_id, lpm_entry,
                             FWD_ATTRIBUTES_ACTION_SETf, action_set);
    }

    /* Set classification group id. */
    if (process_class_id) {
        /* CLASS_ID0f */
        soc_mem_field32_set(unit, view_id, lpm_entry, CLASS_ID_ACTION_SETf,
                            lpm_cfg->defip_lookup_class);
    }

    /* Set Global route flag. */
    if (process_fwd_3_attributes) {
        if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
            /* GLOBAL_ROUTE0f */
            soc_format_field32_set(unit, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, GLOBAL_ROUTEf, 1);
     
        }
        if (BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) {
            soc_format_field32_set(unit, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, GLOBAL_ROUTEf, 1);
        }
        soc_mem_field32_set(unit, view_id, lpm_entry,
                         FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf, action_set);
    }

    if (soc_feature(unit, soc_feature_ipmc_defip) &&  
            (lpm_cfg->defip_flags & BCM_L3_IPMC)) {

        /* L3MC_INDEXf */
        action_set = 0;
        if (!process_pim_sm) {
            return BCM_E_UNAVAIL;
        }

        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, DEST_TYPE1f, BCMI_TD3_DEST_TYPE1_IPMC);
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                           &action_set, IPMC_GROUPf, lpm_cfg->defip_mc_group);
        soc_mem_field32_set(unit, view_id, lpm_entry,
                          DESTINATION_ACTION_SETf, action_set);

        /*
         * RP ID needs uplift by _BCM_L3_DEFIP_RP_ID_BASE value, Now it wil fit in EXPECTED_L3_IIFf
         */
        action_set = 0;
        if (lpm_cfg->defip_l3a_rp != BCM_IPMC_RP_ID_INVALID) {
            soc_format_field32_set(unit, PIM_SM_ACTION_SETfmt,
                           &action_set, EXPECTED_L3_IIFf,
                           _BCM_DEFIP_IPMC_RP_SET(lpm_cfg->defip_l3a_rp));

        } else if ((lpm_cfg->defip_ipmc_flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK) && 
                (lpm_cfg->defip_expected_intf != 0))  {

            soc_format_field32_set(unit, PIM_SM_ACTION_SETfmt,
                           &action_set, EXPECTED_L3_IIFf,
                           lpm_cfg->defip_expected_intf);

            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_DROP) {
                soc_format_field32_set(unit, PIM_SM_ACTION_SETfmt,
                           &action_set, IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf, 1);
            }
            if (lpm_cfg->defip_ipmc_flags & BCM_IPMC_RPF_FAIL_TOCPU) {
                soc_format_field32_set(unit, PIM_SM_ACTION_SETfmt,
                           &action_set, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf, 1);
            }
        }
        soc_mem_field32_set(unit, view_id, lpm_entry,
                          PIM_SM_ACTION_SETf, action_set);
    }

    /*    
     * Note ipv4 entries are expected to reside in part 0 of the entry.
     *      ipv6 entries both parts should be filled.
     *      Hence if entry is ipv6 copy part 0 to part 1
     */
    if (lpm_cfg->defip_flags & BCM_L3_IP6) {
        soc_fb_lpm_ip4entry0_to_1(unit, lpm_entry, lpm_entry, TRUE);
    }

    /*
     * Initialize hw buffer from lpm configuration.
     * NOTE: DON'T MOVE _bcm_fb_defip_ent_init CALL UP,
     * We want to copy flags & nh info, avoid  ipv6 mask & address corruption.
     */
    BCM_IF_ERROR_RETURN(_bcm_td3_lpm_flex_ent_init(unit, lpm_cfg, lpm_entry));

    return BCM_E_NONE;
}
/*
 * Function:    
 *     _bcm_defip_pair128_ip6_addr_get
 * Purpose:  
 *     Read IP6 address field from memory field to ip6_addr_t buffer. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (OUT) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_bcm_defip_pair128_flex_ip6_addr_get(int unit, soc_mem_t view_id, const void *entry, ip6_addr_t ip6)
{
    uint32              ip6_field[4];
    if (SOC_MEM_FIELD_VALID(unit, view_id, IPV6__DIPf)) {
        soc_mem_ip6_addr_get(unit, view_id, entry,IPV6__DIPf, ip6, 0);
    } else if (SOC_MEM_FIELD_VALID(unit, view_id, IPV6__SIPf)) {
        soc_mem_ip6_addr_get(unit, view_id, entry,IPV6__SIPf, ip6, 0);
    } else {
        ip6_field[3] = soc_mem_field32_get(unit, view_id, entry, IP_ADDR0_LWRf);
        ip6[12] = (uint8) (ip6_field[3] >> 24);
        ip6[13] = (uint8) (ip6_field[3] >> 16 & 0xff);
        ip6[14] = (uint8) (ip6_field[3] >> 8 & 0xff);
        ip6[15] = (uint8) (ip6_field[3] & 0xff);
        ip6_field[2] = soc_mem_field32_get(unit, view_id, entry, IP_ADDR1_LWRf);
        ip6[8] = (uint8) (ip6_field[2] >> 24);
        ip6[9] = (uint8) (ip6_field[2] >> 16 & 0xff);
        ip6[10] = (uint8) (ip6_field[2] >> 8 & 0xff);
        ip6[11] = (uint8) (ip6_field[2] & 0xff);
        ip6_field[1] = soc_mem_field32_get(unit, view_id, entry, IP_ADDR0_UPRf);
        ip6[4] = (uint8) (ip6_field[1] >> 24);
        ip6[5] = (uint8) (ip6_field[1] >> 16 & 0xff);
        ip6[6] =(uint8) (ip6_field[1] >> 8 & 0xff);
        ip6[7] =(uint8) (ip6_field[1] & 0xff);
        ip6_field[0] = soc_mem_field32_get(unit, view_id, entry, IP_ADDR1_UPRf);
        ip6[0] =(uint8) (ip6_field[0] >> 24);
        ip6[1] =(uint8) (ip6_field[0] >> 16 & 0xff);
        ip6[2] =(uint8) (ip6_field[0] >> 8 & 0xff);
        ip6[3] =(uint8) (ip6_field[0] & 0xff);
    } 
}
/*
 * Function:    
 *     _bcm_defip_pair128_flex_ip6_mask_get
 * Purpose:  
 *     Read IP6 mask field from memory field to ip6_addr_t buffer. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (OUT) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_bcm_defip_pair128_flex_ip6_mask_get(int unit, soc_mem_t view_id, const void *entry, 
                                ip6_addr_t ip6)
{
    uint32              ip6_field[4];

    if (SOC_MEM_FIELD_VALID(unit, view_id, IPV6__MASKf)) {
        soc_mem_ip6_addr_get(unit, view_id, entry,IPV6__MASKf, ip6, 0);
    } else {
        ip6_field[3] = soc_mem_field32_get(unit, view_id, entry, IP_ADDR_MASK0_LWRf);
        ip6[12] = (uint8) (ip6_field[3] >> 24);
        ip6[13] = (uint8) (ip6_field[3] >> 16 & 0xff);
        ip6[14] = (uint8) (ip6_field[3] >> 8 & 0xff);
        ip6[15] = (uint8) (ip6_field[3] & 0xff);
        ip6_field[2] = soc_mem_field32_get(unit, view_id, entry, IP_ADDR_MASK1_LWRf);
        ip6[8] = (uint8) (ip6_field[2] >> 24);
        ip6[9] = (uint8) (ip6_field[2] >> 16 & 0xff);
        ip6[10] = (uint8) (ip6_field[2] >> 8 & 0xff);
        ip6[11] = (uint8) (ip6_field[2] & 0xff);
        ip6_field[1] = soc_mem_field32_get(unit, view_id, entry, IP_ADDR_MASK0_UPRf);
        ip6[4] = (uint8) (ip6_field[1] >> 24);
        ip6[5] = (uint8) (ip6_field[1] >> 16 & 0xff);
        ip6[6] =(uint8) (ip6_field[1] >> 8 & 0xff);
        ip6[7] =(uint8) (ip6_field[1] & 0xff);
        ip6_field[0] = soc_mem_field32_get(unit, view_id, entry, IP_ADDR_MASK1_UPRf);
        ip6[0] =(uint8) (ip6_field[0] >> 24);
        ip6[1] =(uint8) (ip6_field[0] >> 16 & 0xff);
        ip6[2] =(uint8) (ip6_field[0] >> 8 & 0xff);
        ip6[3] =(uint8) (ip6_field[0] & 0xff);
    }
}
/*
 * Function:
 *      _bcm_defip_pair128_get_key
 * Purpose:
 *      Parse route entry key from L3_DEFIP_PAIR_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      entry       - (IN)Hw entry buffer. 
 *      lpm_cfg     - (IN/OUT)Buffer to fill defip information. 
 * Returns:
 *      BCM_E_XXX
 */
int  
_bcm_defip_pair128_flex_get_key(int unit, uint32 *hw_entry,
                           _bcm_defip_cfg_t *lpm_cfg, uint32 key_type)
{
    bcm_ip6_t mask;                      /* Subnet mask.        */
    soc_mem_t view_id;

    BCM_IF_ERROR_RETURN (
        soc_flow_db_mem_to_view_id_get(unit,
                    L3_DEFIP_PAIR_128m,
                    key_type,
                    SOC_FLOW_DB_DATA_TYPE_INVALID,
                    0,
                    NULL,
                    (uint32 *)&view_id));

    if ((!SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_MASK0_LWRf) ||
        !SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_MASK1_LWRf) ||
        !SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_MASK0_UPRf) ||
        !SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_MASK1_UPRf)) &&
        (!SOC_MEM_FIELD_VALID(unit, view_id, IPV6__MASKf))) {
        return BCM_E_INTERNAL;
    }

    /* Extract ip6 address. */
    _bcm_defip_pair128_flex_ip6_addr_get(unit, view_id, hw_entry, lpm_cfg->defip_ip6_addr);

    /* Extract subnet mask. */
    _bcm_defip_pair128_flex_ip6_mask_get(unit, view_id, hw_entry, mask);

    lpm_cfg->defip_sub_len = bcm_ip6_mask_length(mask);

    /* Extract vrf id & vrf mask. */
    if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_MASK0_LWRf)) {
        if (!soc_mem_field32_get(unit, view_id, hw_entry, VRF_ID_MASK0_LWRf)) {
            lpm_cfg->defip_vrf = BCM_L3_VRF_OVERRIDE;
        } else {
            lpm_cfg->defip_vrf = soc_mem_field32_get(unit, view_id, hw_entry, VRF_ID_0_LWRf);
        }
    } else if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_MASK0f)) {
        if (!soc_mem_field32_get(unit, view_id, hw_entry, VRF_ID_MASK0f)) {
            lpm_cfg->defip_vrf = BCM_L3_VRF_OVERRIDE;
        } else {
            lpm_cfg->defip_vrf = soc_mem_field32_get(unit, view_id, hw_entry, VRF_ID_0f);
        }
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_td3_lpm_flex_ent_parse
 * Purpose:
 *      Parse flex route entry to sw structure.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      hw_entry    - (IN)Hw entry buffer. 
 *      lpm_cfg     - (IN/OUT)Buffer to fill defip information. 
 *      nh_ecmp_idx - (OUT)Next hop ecmp table index. 
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_td3_lpm_flex_ent_parse(int unit, uint32 *lpm_entry, uint32 key_type, soc_mem_t mem, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    uint32 action_set;
    uint32 dest_action_set;
    uint32 dest_type1;
    soc_mem_t view_id;
    uint32 data_ids[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS];
    uint32 key_ids[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS];
    uint32 key_cnt = 0;
    uint32 data_cnt = 0;
    uint32 i = 0;

    BCM_IF_ERROR_RETURN(
        soc_flow_db_mem_to_view_id_get(unit,
                     mem,
                     key_type,
                     SOC_FLOW_DB_DATA_TYPE_INVALID,
                     0,
                     NULL,
                     (uint32 *)&view_id));

    if (mem == L3_DEFIP_PAIR_128m ||
       (SOC_MEM_FIELD_VALID(unit, view_id, IPV6f)) ||
       (SOC_MEM_FIELD_VALID(unit, view_id, IPV6__SIPf)) ||
       (SOC_MEM_FIELD_VALID(unit, view_id, IPV6__DIPf))) {
        lpm_cfg->defip_flags |= BCM_L3_IP6;
    }

    dest_action_set = soc_mem_field32_get(unit, view_id, lpm_entry,
                      DESTINATION_ACTION_SETf);

    dest_type1 =  soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                      &dest_action_set, DEST_TYPE1f);

    if (soc_feature(unit, soc_feature_ipmc_defip) &&
        dest_type1 == BCMI_TD3_DEST_TYPE1_IPMC) {

        int val;
        lpm_cfg->defip_flags |= BCM_L3_IPMC;

        action_set = soc_mem_field32_get(unit, view_id, lpm_entry,
                          PIM_SM_ACTION_SETf);

        if (soc_format_field32_get(unit, PIM_SM_ACTION_SETfmt, &action_set, IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf)) {
            lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_DROP;
        }
        if (soc_format_field32_get(unit, PIM_SM_ACTION_SETfmt, &action_set, IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf)) {
            lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RPF_FAIL_TOCPU;
        }
        val = soc_format_field32_get(unit, PIM_SM_ACTION_SETfmt,
                       &action_set, EXPECTED_L3_IIFf);
        /*
         * RP ID and ExpectedIIF are overlaid.
         */
        if (soc_feature(unit, soc_feature_l3defip_rp_l3iif_resolve)) {
            int rpa_id_mask;
            if (SOC_MEM_FIELD_VALID(unit, mem, RPA_ID0f)) {
                rpa_id_mask = (1 << soc_mem_field_length(unit, mem, RPA_ID0f)) - 1;
            } else {
                rpa_id_mask = (1 << soc_mem_field_length(unit, mem, RPA_IDf)) - 1;
            }
            /*
             * 1) If value is more than RPA_ID field then its l3_iif.
             *    N/A for TD3/MV2/HX5 to support warmboot.
             * 2) When RPF FAIL DROP/TOCPU flags are set then its l3iif valid
             */
            if ((lpm_cfg->defip_ipmc_flags &
                 (BCM_IPMC_RPF_FAIL_DROP | BCM_IPMC_RPF_FAIL_TOCPU)) ||
                (!(SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit) ||
                   SOC_IS_HELIX5(unit)) && (val > rpa_id_mask))) {
                lpm_cfg->defip_expected_intf = val;
                if (0 != lpm_cfg->defip_expected_intf) {
                    lpm_cfg->defip_ipmc_flags |= BCM_IPMC_POST_LOOKUP_RPF_CHECK;
                }
                lpm_cfg->defip_l3a_rp = BCM_IPMC_RP_ID_INVALID;
            } else {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_RP_ID_EXPECTED_INTF_RESOLVE;
                lpm_cfg->defip_l3a_rp = val & rpa_id_mask;
                lpm_cfg->defip_expected_intf = val;
            }
        } else if (_BCM_DEFIP_IPMC_RP_IS_SET(val)) {
            lpm_cfg->defip_l3a_rp = _BCM_DEFIP_IPMC_RP_GET(val);
        } else {
            lpm_cfg->defip_expected_intf = val;

            if (0 != lpm_cfg->defip_expected_intf) {
                lpm_cfg->defip_ipmc_flags |= BCM_IPMC_POST_LOOKUP_RPF_CHECK;
            }
            lpm_cfg->defip_l3a_rp = BCM_IPMC_RP_ID_INVALID;
        }
        lpm_cfg->defip_mc_group = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                          &dest_action_set, L3MC_INDEXf);
    }

    if (dest_type1 == BCMI_TD3_DEST_TYPE1_ECMP) {
        lpm_cfg->defip_ecmp = TRUE;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;
    } else {
        lpm_cfg->defip_ecmp = 0;
        lpm_cfg->defip_ecmp_count = 0;
    }
    if (nh_ecmp_idx != NULL) {
        *nh_ecmp_idx = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                       &dest_action_set, NEXT_HOP_INDEXf);
    }

    if (SOC_MEM_FIELD_VALID(unit, view_id, QOS_ACTION_SETf)) {
        action_set = soc_mem_field32_get(unit, view_id, lpm_entry,
                        QOS_ACTION_SETf);

        /* Get entry priority. */
        lpm_cfg->defip_prio = soc_format_field32_get(unit, QOS_ACTION_SETfmt,
                           &action_set, PRIf);

        /* Get priority override bit. */
        if (soc_format_field32_get(unit, QOS_ACTION_SETfmt, &action_set, RPEf)) {
            lpm_cfg->defip_flags |= BCM_L3_RPE;
        }
    }

    /* Get classification group id. */
    if (SOC_MEM_FIELD_VALID(unit, view_id, CLASS_ID_ACTION_SETf)) {
        lpm_cfg->defip_lookup_class = 
            soc_mem_field32_get(unit, view_id, lpm_entry, CLASS_ID_ACTION_SETf);
    }

    /* Get hit bit. */
    if (SOC_MEM_FIELD_VALID(unit, view_id, HITf)) {
        if (soc_mem_field32_get(unit, view_id, lpm_entry, HITf)) {
            lpm_cfg->defip_flags |= BCM_L3_HIT;
        }
    }

    if (SOC_MEM_FIELD_VALID(unit, view_id, HIT0f)) {
        if (soc_mem_field32_get(unit, view_id, lpm_entry, HIT0f)) {
            lpm_cfg->defip_flags |= BCM_L3_HIT;
        }
    }

    /* Get destination discard field. */
    if (SOC_MEM_FIELD_VALID(unit, view_id, FWD_ATTRIBUTES_ACTION_SETf)) {
        action_set = soc_mem_field32_get(unit, view_id, lpm_entry,
                             FWD_ATTRIBUTES_ACTION_SETf);

        if (soc_format_field32_get(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                                   &action_set, DST_DISCARDf)) {
            lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
        }
    }

    /* Get the logical fields */
    lpm_cfg->defip_num_of_fields = 0;
    BCM_IF_ERROR_RETURN(
           soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                            BCM_FLOW_MAX_NOF_LOGICAL_FIELDS,
                            key_ids,
                            &key_cnt));

    if (key_cnt) {
        for (i = 0;
             (i <  key_cnt) ; i++) {
            lpm_cfg->defip_logical_fields[i].id =  key_ids[i];
            lpm_cfg->defip_logical_fields[i].value =
                         soc_mem_field32_get(unit, view_id, lpm_entry,
                         key_ids[i]);

        }
        lpm_cfg->defip_num_of_fields = key_cnt;
    }
    BCM_IF_ERROR_RETURN(
           soc_flow_db_mem_view_field_list_get(unit,
                      view_id,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      BCM_FLOW_MAX_NOF_LOGICAL_FIELDS, data_ids, &data_cnt));
    if (data_cnt) {
        /* Entry traverse */
        for ( ;
             ((i < data_cnt) &&
              ( i < SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA));
              i++) {
            lpm_cfg->defip_logical_fields[i].id = data_ids[i];
            lpm_cfg->defip_logical_fields[i].value =
                         soc_mem_field32_get(unit, view_id, lpm_entry,
                         data_ids[i]);
        }
        lpm_cfg->defip_num_of_fields += data_cnt;
    }

    SOC_IF_ERROR_RETURN(_bcm_flow_mem_view_to_flow_info_get(unit, view_id,
               SOC_FLOW_DB_FUNC_L3_ROUTE_ID,
               &lpm_cfg->defip_flow_handle, &lpm_cfg->defip_flow_option_handle, 1));

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_td3_flex_ip6_defip_get
 * Purpose:
 *    Set an IP6 address field in L3_DEFIPm
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_td3_flex_ip6_defip_get(int unit, const void *lpm_key, soc_mem_t view_id,
                          _bcm_defip_cfg_t *lpm_cfg)
{
    uint8 *ip6;                 /* Ip6 address.       */
    uint8 mask[BCM_IP6_ADDRLEN];        /* Subnet mask.       */
    uint32 ip6_word;            /* Temp storage.      */

    sal_memset(mask, 0, sizeof (bcm_ip6_t));

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;
    sal_memset(ip6, 0, sizeof (bcm_ip6_t));

    ip6_word = soc_mem_field32_get(unit, view_id, lpm_key, IP_ADDR1f);
    ip6[0] = (uint8) ((ip6_word >> 24) & 0xff);
    ip6[1] = (uint8) ((ip6_word >> 16) & 0xff);
    ip6[2] = (uint8) ((ip6_word >> 8) & 0xff);
    ip6[3] = (uint8) (ip6_word & 0xff);

    ip6_word = soc_mem_field32_get(unit, view_id, lpm_key, IP_ADDR0f);
    ip6[4] = (uint8) ((ip6_word >> 24) & 0xff);
    ip6[5] = (uint8) ((ip6_word >> 16) & 0xff);
    ip6[6] = (uint8) ((ip6_word >> 8) & 0xff);
    ip6[7] = (uint8) (ip6_word & 0xff);

    ip6_word = soc_mem_field32_get(unit, view_id, lpm_key, IP_ADDR_MASK1f);
    mask[0] = (uint8) ((ip6_word >> 24) & 0xff);
    mask[1] = (uint8) ((ip6_word >> 16) & 0xff);
    mask[2] = (uint8) ((ip6_word >> 8) & 0xff);
    mask[3] = (uint8) (ip6_word & 0xff);

    ip6_word = soc_mem_field32_get(unit, view_id, lpm_key, IP_ADDR_MASK0f);
    mask[4] = (uint8) ((ip6_word >> 24) & 0xff);
    mask[5] = (uint8) ((ip6_word >> 16) & 0xff);
    mask[6] = (uint8) ((ip6_word >> 8) & 0xff);
    mask[7] = (uint8) (ip6_word & 0xff);

    lpm_cfg->defip_sub_len = bcm_ip6_mask_length(mask);
}
/*
 * Function:
 *      _bcm_td3_lpm_flex_vrf_get
 * Purpose:
 *      Service routine used to translate hw specific vrf id to API format.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_entry - (IN)Route info buffer from hw.
 *      vrf_id    - (OUT)Virtual router id.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td3_lpm_flex_vrf_get(int unit, void *lpm_entry, int *vrf, soc_mem_t view_id)
{
    int vrf_id;

    /* Get Virtual Router id if supported. */
    if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_MASK0f)){
        vrf_id = soc_mem_field32_get(unit, view_id, lpm_entry, VRF_ID_0f);

        /* Special vrf's handling. */
        if (soc_mem_field32_get(unit, view_id, lpm_entry, VRF_ID_MASK0f)) {
            *vrf = vrf_id;    
        } else if (SOC_VRF_MAX(unit) == vrf_id) {
            *vrf = BCM_L3_VRF_GLOBAL;
        } else {
            *vrf = BCM_L3_VRF_OVERRIDE;    
            if (SOC_MEM_FIELD_VALID(unit, view_id, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf)) {
                uint32 action_set = soc_mem_field32_get(unit, view_id, lpm_entry,
                                                FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf);
                if (soc_format_field32_get(unit, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETfmt,
                                           &action_set, GLOBAL_ROUTEf)) {
                    *vrf = BCM_L3_VRF_GLOBAL; 
                }
            }
#if 0 
            if (SOC_MEM_FIELD_VALID(unit, L3_DEFIPm, GLOBAL_HIGH0f)) {
                if (soc_mem_field32_get(unit, view_id, lpm_entry, GLOBAL_HIGH0f)){
                    *vrf = BCM_L3_VRF_OVERRIDE;
                }
            }
#endif
        }
    } else {
        /* No vrf support on this device. */
        *vrf = BCM_L3_VRF_DEFAULT;
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_fb_lpm_flex_ent_get_key
 * Purpose:
 *      Get flex entry key from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (OUT)Buffer to fill defip information.
 *      lpm_entry   - (IN) Buffer read from hw.
 * Returns:
 *      void
 */
int
_bcm_td3_lpm_flex_ent_get_key(int unit, _bcm_defip_cfg_t *lpm_cfg,
                        uint32  *lpm_entry, uint32 key_type)
{
    bcm_ip_t v4_mask;
    int ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6;
    soc_mem_t view_id;

    BCM_IF_ERROR_RETURN (
        soc_flow_db_mem_to_view_id_get(unit,
                    L3_DEFIPm,
                    key_type,
                    SOC_FLOW_DB_DATA_TYPE_INVALID,
                    0,
                    NULL,
                    (uint32 *)&view_id));

    /* Set prefix ip address & mask. */
    if (ipv6) {
        _bcm_td3_flex_ip6_defip_get(unit, lpm_entry, view_id, lpm_cfg);
    } else {
        /* Get ipv4 address. */
        lpm_cfg->defip_ip_addr =
            soc_mem_field32_get(unit, view_id, lpm_entry, IP_ADDR0f);

        /* Get subnet mask. */
        v4_mask = soc_mem_field32_get(unit, view_id, lpm_entry, IP_ADDR_MASK0f);

        /* Fill mask length. */
        lpm_cfg->defip_sub_len = bcm_ip_mask_length(v4_mask);
    }

    /* Get Virtual Router id */
    _bcm_td3_lpm_flex_vrf_get(unit, lpm_entry, &lpm_cfg->defip_vrf, view_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb_mem_ip6_defip_set
 * Purpose:
 *    Set an IP6 address field in L3_DEFIPm
 * Parameters:
 *    unit    - (IN) SOC unit number;
 *    lpm_key - (OUT) Buffer to fill.
 *    lpm_cfg - (IN) Route information.
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
void
_bcm_td3_mem_ip6mc_defip_set(int unit, void *lpm_key, _bcm_defip_cfg_t *lpm_cfg)
{
    uint8 *ip6;                 /* Ip6 address.       */
    uint8 mask[BCM_IP6_ADDRLEN];        /* Subnet mask.       */
    uint32 ip6_word[2];            /* Temp storage.      */
    int idx;                    /* Iteration index .  */

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;
    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply subnet mask */
    idx = lpm_cfg->defip_sub_len / 8;   /* Unchanged byte count.    */
    ip6[idx] &= mask[idx];      /* Apply mask on next byte. */
    for (idx++; idx < BCM_IP6_ADDRLEN; idx++) {
        ip6[idx] = 0;           /* Reset rest of bytes.     */
    }
    if (SOC_IS_FIREBOLT6(unit)) {
        /* set upper 18 bits addr */
        ip6_word[0] = ((ip6[0] << 10) | (ip6[1] << 2) | (ip6[2] >> 6));
        soc_L3_DEFIPm_field_set(unit, lpm_key, IPMC_V6_ADDR_127_110f, (void *)&ip6_word[0]);

        /* set lower 46 bits addr */
        ip6_word[1] = (((ip6[2] & 0x3f) << 8) | (ip6[3]));
        ip6_word[0] = ((ip6[4] << 24) | (ip6[5] << 16) | (ip6[6] << 8) | (ip6[7]));
        soc_L3_DEFIPm_field_set(unit, lpm_key, IPMC_V6_ADDR_109_64f, (void *)&ip6_word);

        /* set upper 18 bits mask */
        ip6_word[0] = ((mask[0] << 10) | (mask[1] << 2) | (mask[2] >> 6));
        soc_L3_DEFIPm_field_set(unit, lpm_key, IPMC_V6_ADDR_127_110_MASKf,
                                (void *)&ip6_word);

        /* set lower 46 bits mask */
        ip6_word[1] = (((mask[2] & 0x3f) << 8) | (mask[3]));
        ip6_word[0] = ((mask[4] << 24) | (mask[5] << 16) | (mask[6] << 8) | (mask[7]));
        soc_L3_DEFIPm_field_set(unit, lpm_key, IPMC_V6_ADDR_109_64_MASKf,
                                (void *)&ip6_word);
    } else {
        /* set upper 20 bits addr */
        ip6_word[0] = ((ip6[0] << 12) | (ip6[1] << 4) | (ip6[2] >> 4));
        soc_L3_DEFIPm_field_set(unit, lpm_key, IPMC_V6_ADDR_127_108f, (void *)&ip6_word[0]);

        /* set lower 44 bits addr */
        ip6_word[1] = (((ip6[2] & 0xf) << 8) | (ip6[3]));
        ip6_word[0] = ((ip6[4] << 24) | (ip6[5] << 16) | (ip6[6] << 8) | (ip6[7]));
        soc_L3_DEFIPm_field_set(unit, lpm_key, IPMC_V6_ADDR_107_64f, (void *)&ip6_word);

        /* set upper 20 bits mask */
        ip6_word[0] = ((mask[0] << 12) | (mask[1] << 4) | (mask[2] >> 4));
        soc_L3_DEFIPm_field_set(unit, lpm_key, IPMC_V6_ADDR_127_108_MASKf,
                                (void *)&ip6_word);

        /* set lower 44 bits mask */
        ip6_word[1] = (((mask[2] & 0xf) << 8) | (mask[3]));
        ip6_word[0] = ((mask[4] << 24) | (mask[5] << 16) | (mask[6] << 8) | (mask[7]));
        soc_L3_DEFIPm_field_set(unit, lpm_key, IPMC_V6_ADDR_107_64_MASKf,
                                (void *)&ip6_word);
    }
}
/*
 * Function:
 *     _bcm_td3_mem_ip6mc_defip_get
 * Purpose:
 *    Set an IP6 address field in L3_DEFIPm
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
void
_bcm_td3_mem_ip6mc_defip_get(int unit, const void *lpm_key,
                          _bcm_defip_cfg_t *lpm_cfg)
{
    uint8 *ip6;                 /* Ip6 address.       */
    uint8 mask[BCM_IP6_ADDRLEN];        /* Subnet mask.       */
    uint32 ip6_word[2];            /* Temp storage.      */

    sal_memset(mask, 0, sizeof (bcm_ip6_t));

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;
    sal_memset(ip6, 0, sizeof (bcm_ip6_t));
    if (SOC_IS_FIREBOLT6(unit)) {
        /* get upper 18 bits addr */
        soc_L3_DEFIPm_field_get(unit, lpm_key, IPMC_V6_ADDR_127_110f, ip6_word);
        ip6[0] = (uint8) ((ip6_word[0] >> 10) & 0xff);
        ip6[1] = (uint8) ((ip6_word[0] >> 2) & 0xff);
        ip6[2] = (uint8) ((ip6_word[0] & 0x3) << 6);

        /* get lower 46 bits addr */
        soc_L3_DEFIPm_field_get(unit, lpm_key, IPMC_V6_ADDR_109_64f, ip6_word);
        ip6[2] |= (uint8) ((ip6_word[1] >> 8) & 0x3f);
        ip6[3] = (uint8) (ip6_word[1] & 0xff);
        ip6[4] = (uint8) ((ip6_word[0] >> 24) & 0xff);
        ip6[5] = (uint8) ((ip6_word[0] >> 16) & 0xff);
        ip6[6] = (uint8) ((ip6_word[0] >> 8) & 0xff);
        ip6[7] = (uint8) (ip6_word[0] & 0xff);

        /* get upper 18 bits mask */
        soc_L3_DEFIPm_field_get(unit, lpm_key, IPMC_V6_ADDR_127_110_MASKf, ip6_word);
        mask[0] = (uint8) ((ip6_word[0] >> 10) & 0xff);
        mask[1] = (uint8) ((ip6_word[0] >> 2) & 0xff);
        mask[2] = (uint8) ((ip6_word[0] & 0x3) << 6);

        /* get lower 46 bits mask */
        soc_L3_DEFIPm_field_get(unit, lpm_key, IPMC_V6_ADDR_109_64_MASKf, ip6_word);
        mask[2] |= (uint8) ((ip6_word[1] >> 8) & 0x3f);
        mask[3] = (uint8) (ip6_word[1] & 0xff);
        mask[4] = (uint8) ((ip6_word[0] >> 24) & 0xff);
        mask[5] = (uint8) ((ip6_word[0] >> 16) & 0xff);
        mask[6] = (uint8) ((ip6_word[0] >> 8) & 0xff);
        mask[7] = (uint8) (ip6_word[0] & 0xff);
    } else {
        /* get upper 20 bits addr */
        soc_L3_DEFIPm_field_get(unit, lpm_key, IPMC_V6_ADDR_127_108f, ip6_word);
        ip6[0] = (uint8) ((ip6_word[0] >> 12) & 0xff);
        ip6[1] = (uint8) ((ip6_word[0] >> 4) & 0xff);
        ip6[2] = (uint8) ((ip6_word[0] & 0xf) << 4);

        /* get lower 44 bits addr */
        soc_L3_DEFIPm_field_get(unit, lpm_key, IPMC_V6_ADDR_107_64f, ip6_word);
        ip6[2] |= (uint8) ((ip6_word[1] >> 8) & 0xf);
        ip6[3] = (uint8) (ip6_word[1] & 0xff);
        ip6[4] = (uint8) ((ip6_word[0] >> 24) & 0xff);
        ip6[5] = (uint8) ((ip6_word[0] >> 16) & 0xff);
        ip6[6] = (uint8) ((ip6_word[0] >> 8) & 0xff);
        ip6[7] = (uint8) (ip6_word[0] & 0xff);

        /* get upper 20 bits mask */
        soc_L3_DEFIPm_field_get(unit, lpm_key, IPMC_V6_ADDR_127_108_MASKf, ip6_word);
        mask[0] = (uint8) ((ip6_word[0] >> 12) & 0xff);
        mask[1] = (uint8) ((ip6_word[0] >> 4) & 0xff);
        mask[2] = (uint8) ((ip6_word[0] & 0xf) << 4);

        /* get lower 44 bits mask */
        soc_L3_DEFIPm_field_get(unit, lpm_key, IPMC_V6_ADDR_107_64_MASKf, ip6_word);
        mask[2] |= (uint8) ((ip6_word[1] >> 8) & 0xf);
        mask[3] = (uint8) (ip6_word[1] & 0xff);
        mask[4] = (uint8) ((ip6_word[0] >> 24) & 0xff);
        mask[5] = (uint8) ((ip6_word[0] >> 16) & 0xff);
        mask[6] = (uint8) ((ip6_word[0] >> 8) & 0xff);
        mask[7] = (uint8) (ip6_word[0] & 0xff);
    }
    lpm_cfg->defip_sub_len = bcm_ip6_mask_length(mask);
}
/*
 * Function:
 *      _bcm_td3_lpm_ipv6mc_ent_init
 * Purpose:
 *      Service routine used to initialize lkup key for IPMC V6 lpm entry when feature is enabled.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_cfg   - (IN)Prefix info.
 *      lpm_entry - (OUT)Hw buffer to fill.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_lpm_ipv6mc_ent_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
                     uint32  *lpm_entry)
{
    int vrf_id;
    int vrf_mask;
    int mode = 1;
    int pfx_len = lpm_cfg->defip_sub_len;

    /* Extract entry  vrf id  & vrf mask. */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_internal_lpm_vrf_calc(unit, lpm_cfg, &vrf_id, &vrf_mask));

    /* Set mode  */
    if (pfx_len > 64 ||
        lpm_cfg->defip_flags_high & BCM_XGS3_L3_ENTRY_IN_DEFIP_PAIR) {
        mode = 3;
    } else {
        mode = 1;
    }
    _bcm_td3_mem_ip6mc_defip_set(unit, lpm_entry, lpm_cfg);

    soc_L3_DEFIPm_field32_set(unit, lpm_entry, IPMC_V6_VRF_IDf, vrf_id);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, IPMC_V6_VRF_ID_MASKf, vrf_mask);

    soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID0f, 1);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID1f, 1);

    /* IPMC v6 */
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, DATA_TYPE0f, 2);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, KEY_TYPE0f, BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, KEY_TYPE_MASK0f, 0xf);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, KEY_TYPE1f, BCMI_LPM_SEPARATE_IPV6MC_KEY_TYPE);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, KEY_TYPE_MASK1f, 0xf);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, KEY_MODE0f, mode);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, KEY_MODE1f, mode);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, KEY_MODE_MASK1f, 0x3);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, KEY_MODE_MASK0f, 0x3);

    if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, GLOBAL_ROUTE0f, 0x1);
    }
    return BCM_E_NONE;
}
#endif
