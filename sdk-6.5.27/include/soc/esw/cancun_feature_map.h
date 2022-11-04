 /*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    cancun_feature_map.h
 * Purpose: Header file for CANCUN version to SOC feature mapping
 */

#ifndef _CANCUN_FEATURE_MAP_H_
#define _CANCUN_FEATURE_MAP_H_

#include <soc/esw/cancun.h>

/*
 * CANCUN version based soc_feature support map
 *
 * For all features that need to be enabled/disabled based on a
 * specific/range of cancun versions please update this structure
 *
 * There are options to check individual as well as range
 * versions. A feature can either be enabled/disabled based on these
 * rules. If there is a generic range and then followed by a specific
 * rule for a given soc_feature, please place the specific rule at
 * the bottm of the list.
 *
 * When not dealing with a range of versions, please leave the MAX version
 * as invalid and only use the MIN value for the cancun version in
 * question. Use 'CANCUN_VER_CHECK_IN_RANGE' as range check when doing a
 * range since the comparison is understood to be between the two versions.
 *
*/

static soc_feature_cancun_version_map_t soc_feature_cancun_version_map[] = {

    /* For TD3 and MV2, TWAMP_OWAMP is supported only
     * from 6.0.0 & 4.0.0 * and above respectively.
     * For HX5, this feature is supported only from
     * B370.4.0.0 onwards.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,0,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_twamp_owamp_support,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_twamp_owamp_support,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_twamp_owamp_support,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3/MV2, Large scale NAT is supported only
     * from 6.0.0 and 4.0.0 respectively.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION_DEF_6_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_large_scale_nat,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_large_scale_nat,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5/FB6, Large scale NAT is supported only
     * from 4.0.0 for both.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_large_scale_nat,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_large_scale_nat,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },

    /* For TD3, HX5, MV2, HR4 and FB6 'vfi_switched_l2_change_fields'
     * supported only from 6.0.0, 4.0.0,4.0.0, 4.0.0 and 4.0.0
     * versions respectively.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION_DEF_6_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vfi_switched_l2_change_fields,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vfi_switched_l2_change_fields,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vfi_switched_l2_change_fields,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vfi_switched_l2_change_fields,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vfi_switched_l2_change_fields,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },

    /* For TD3, HX5 and MV2, 'ioam_support'
     * supported only from B870.6.0.0,B770.4.0.0,B370.4.0.0
     * versions respectively.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION_DEF_6_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ioam_support,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ioam_support,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION_DEF_4_0_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ioam_support,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },

    /* For TD3, MV2 and HX5, 'ipv4_checksum_validate'
     * supported only from 6.1.1, 4.1.0 and 4.1.0
     * versions respectively.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION_DEF_6_1_1,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ip_checksum_validate,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION_DEF_4_1_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ip_checksum_validate,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION_DEF_4_1_0,
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ip_checksum_validate,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },

    /* For TD3, 'mim_decoupled_mode'
     * supported only from 6.1.2.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,1,2),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_mim_decoupled_mode,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
     /* For MV2, 'mim_decoupled_mode'
     * supported only from 4.1.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,1,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mim_decoupled_mode,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'mim_decoupled_mode'
     * supported only from 4.2.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,2,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mim_decoupled_mode,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HR4, 'mim_decoupled_mode'
     * supported only from 4.0.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,0,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mim_decoupled_mode,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For FB6, 'mim_decoupled_mode'
     * supported only from 4.0.0.
     */
    {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,0,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mim_decoupled_mode,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },

    /* For TD3, 'separate_key_for_ipmc_route'
     * supported only from 6.1.2.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,1,2),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_separate_key_for_ipmc_route,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For MV2, 'separate_key_for_ipmc_route'
     * supported only from 4.1.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,1,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_separate_key_for_ipmc_route,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'separate_key_for_ipmc_route'
     * supported only from 4.1.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,1,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_separate_key_for_ipmc_route,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_td3_lpm_ipmc_war'
     * supported only before 6.1.2.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,1,2),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_td3_lpm_ipmc_war,
        CANCUN_SOC_FEATURE_DISABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For MV2, 'soc_feature_td3_lpm_ipmc_war'
     * supported only before 4.1.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,1,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_td3_lpm_ipmc_war,
        CANCUN_SOC_FEATURE_DISABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_td3_lpm_ipmc_war'
     * supported only before 4.1.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,1,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_td3_lpm_ipmc_war,
        CANCUN_SOC_FEATURE_DISABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },

    /* For TD3, 'soc_feature_derive_phb_based_on_vfi'
     * supported only from 6.1.2.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,1,2),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_derive_phb_based_on_vfi,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For MV2, 'soc_feature_derive_phb_based_on_vfi'
     * supported only from 4.1.1.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,1,1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_derive_phb_based_on_vfi,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_derive_phb_based_on_vfi'
     * supported only from 4.1.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,1,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_derive_phb_based_on_vfi,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HR4, 'soc_feature_derive_phb_based_on_vfi'
     * supported only from 4.0.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,0,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_derive_phb_based_on_vfi,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For FB6, 'soc_feature_derive_phb_based_on_vfi'
     * supported only from 4.0.0.
     */
    {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,0,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_derive_phb_based_on_vfi,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_enable_flow_based_udf_extraction'
     * supported only from 6.1.3.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,1,3),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_enable_flow_based_udf_extraction,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_enable_flow_based_udf_extraction'
     * supported only from 4.1.2.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,1,2),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_enable_flow_based_udf_extraction,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_enable_flow_based_udf_extraction'
     * supported only from 4.1.1.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,1,1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_enable_flow_based_udf_extraction,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_enable_ifa_2'
     * supported only from 6.2.0.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,2,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_enable_ifa_2,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For TD3, 'soc_feature_ifa_adapt_table_select'
     * supported only from 6.2.2.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,2,2),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_ifa_adapt_table_select,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
        /* For MV2, 'soc_feature_enable_ifa_2'
     * supported only from 4.2.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,2,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_enable_ifa_2,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_ifa_adapt_table_select'
     * supported only from 4.2.1.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,2,1),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_ifa_adapt_table_select,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
        /* For HX5, 'soc_feature_enable_ifa_2'
     * supported only from 4.2.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,2,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_enable_ifa_2,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For HX5, 'soc_feature_ifa_adapt_table_select'
     * supported only from 4.2.1.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,2,1),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_ifa_adapt_table_select,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For TD3, 'soc_feature_egr_vlan_xlate2_enable'
     * supported only from 6.3.0.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,3,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_egr_vlan_xlate2_enable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_egr_vlan_xlate2_enable'
     * supported only from 4.3.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,3,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_egr_vlan_xlate2_enable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_pt2pt_access_vlan_check'
     * supported only from 6.3.2.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,3,2),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_pt2pt_access_vlan_check,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_pt2pt_access_vlan_check'
     * supported only from 4.3.1.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,3,1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_pt2pt_access_vlan_check,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_pt2pt_access_vlan_check'
     * supported only from 4.2.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,2,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_pt2pt_access_vlan_check,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_nsh_over_l2'
     * supported only from 6.3.2.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,3,2),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_nsh_over_l2,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_nsh_over_l2'
     * supported only from 4.3.1.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,3,1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_nsh_over_l2,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_allow_l3_ing_tag_actions'
     * supported only from 6.1.3.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,1,3),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_allow_l3_ing_tag_actions,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_allow_l3_ing_tag_actions'
     * supported only from 4.1.2.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,1,2),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_allow_l3_ing_tag_actions,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_allow_l3_ing_tag_actions'
     * supported only from 4.1.1.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,1,1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_allow_l3_ing_tag_actions,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_vxlan_tunnel_vlan_egress_translation'
     * supported only from 6.4.0.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,4,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_vxlan_tunnel_vlan_egress_translation,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_vxlan_tunnel_vlan_egress_translation'
     * supported only from 4.4.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,4,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_tunnel_vlan_egress_translation,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_vxlan_tunnel_vlan_egress_translation'
     * supported only from 4.2.1.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,2,1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_tunnel_vlan_egress_translation,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HR4, 'soc_feature_vxlan_tunnel_vlan_egress_translation'
     * supported only from 4.0.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,0,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_tunnel_vlan_egress_translation,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For FB6, 'soc_feature_vxlan_tunnel_vlan_egress_translation'
     * supported only from 4.0.0.
     */
    {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,0,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_tunnel_vlan_egress_translation,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_per_port_mpls_label_removal'
     * supported only from 6.1.3.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,1,3),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_per_port_mpls_label_removal,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_per_port_mpls_label_removal'
     * supported only from 4.1.2.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,1,2),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_per_port_mpls_label_removal,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_per_port_mpls_label_removal'
     * supported only from 4.1.1.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,1,1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_per_port_mpls_label_removal,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_mpls_frr_bottom_label'
     * supported only from 6.5.0.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,5,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_mpls_frr_bottom_label,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_mpls_frr_bottom_label'
     * supported only from 4.5.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,5,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mpls_frr_bottom_label,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_mpls_frr_bottom_label'
     * supported only from 4.3.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,3,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mpls_frr_bottom_label,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For FB6, 'soc_feature_mpls_frr_bottom_label'
     * supported only from 4.0.0.
     */
    {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,0,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mpls_frr_bottom_label,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_ipmc_riot_l3_iif_check'
     * supported only from 6.5.0.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6, 5, 0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ipmc_riot_l3_iif_check,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For MV2, 'soc_feature_ipmc_riot_l3_iif_check'
     * supported only from 4.1.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4, 5, 0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ipmc_riot_l3_iif_check,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_ipmc_riot_l3_iif_check'
     * supported only from 4.3.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4, 3, 0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ipmc_riot_l3_iif_check,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_snap_nonzero_oui'
     * supported only from 6.5.0.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6, 5, 0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_snap_nonzero_oui,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For MV2, 'soc_feature_snap_nonzero_oui'
     * supported only from 4.1.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4, 5, 0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_snap_nonzero_oui,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_snap_nonzero_oui'
     * supported only from 4.3.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4, 3, 0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_snap_nonzero_oui,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_ifa_truncated_pkt_hdr_len_update'
     * supported only from 6.6.1.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,6,1),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_ifa_truncated_pkt_hdr_len_update,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_ifa_truncated_pkt_hdr_len_update'
     * supported only from 4.6.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,6,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ifa_truncated_pkt_hdr_len_update,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_mpls_evpn'
     * supported only from 6.6.0.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,6,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_mpls_evpn,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For TD3, 'soc_feature_hx5_flex_gbp_support'
     * supported only from 6.7.0.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,7,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_hx5_flex_gbp_support,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_hx5_flex_gbp_support'
     * supported only from 4.6.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,6,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_hx5_flex_gbp_support,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3, 'soc_feature_pt2pt_tunnel_term_learn_disable'
     * supported only from 6.4.0.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,4,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_pt2pt_tunnel_term_learn_disable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* For MV2, 'soc_feature_pt2pt_tunnel_term_learn_disable'
     * supported only from 4.4.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,4,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_pt2pt_tunnel_term_learn_disable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_pt2pt_tunnel_term_learn_disable'
     * supported only from 4.2.1
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4, 2, 1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_pt2pt_tunnel_term_learn_disable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For FB6, 'soc_feature_pt2pt_tunnel_term_learn_disable'
     * supported only from 4.1.0.
     */
    {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,1,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_pt2pt_tunnel_term_learn_disable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For TD3X family,'tunnel_term_dscp_phb' is
     * supported only from following versions:
     * TD3: 6.7.1
     * MV2: 4.7.0
     * HX5: 4.5.0
     * FB6: 4.2.0
     * HR4: 4.2.0
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,7,1),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_tunnel_term_dscp_phb,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,7,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_tunnel_term_dscp_phb,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,5,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_tunnel_term_dscp_phb,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,2,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_tunnel_term_dscp_phb,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,2,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_tunnel_term_dscp_phb,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* 'soc_feature_vxlan_sip_dip_based_svp_assignment'
     * is supported on the mentioned devices in the captured
     * Cancun versions.
     * TD3: 6.6.0
     * MV2: 4.6.0
     * HX5: 4.4.0
     * FB6: 4.1.0
     * HR4: 4.1.0
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,6,1),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_vxlan_sip_dip_based_svp_assignment,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,6,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_vxlan_sip_dip_based_svp_assignment,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,4,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_vxlan_sip_dip_based_svp_assignment,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,1,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_vxlan_sip_dip_based_svp_assignment,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,1,0),
        SOC_CANCUN_VERSION(0xFF,0xFF,0xFF),
        soc_feature_vxlan_sip_dip_based_svp_assignment,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_IN_RANGE
    },
    /* 'soc_feature_l2_tunnel_pyld_sgpp_tpid_disable'
     * is supported on the mentioned devices in the captured
     * Cancun versions.
     * TD3: 6.6.0
     * MV2: 4.6.0
     * HX5: 4.4.0
     * FB6: 4.1.0
     * HR4: 4.1.0
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,8,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_l2_tunnel_pyld_sgpp_tpid_disable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,7,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_l2_tunnel_pyld_sgpp_tpid_disable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,5,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_l2_tunnel_pyld_sgpp_tpid_disable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,2,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_l2_tunnel_pyld_sgpp_tpid_disable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,2,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_l2_tunnel_pyld_sgpp_tpid_disable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
     /* For HX5, 'soc_feature_hgoe_line_card_mode'
      * is supported only on B370-HGOEL.1.0.0 version of CANCUN.
      */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(1,0,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_hgoe_line_card_mode,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
        CANCUN_FILE_BRANCH_ID_HGoE
    },
    /* Enable 'soc_feature_allow_l3_ing_tag_actions' for TD3
     * B870-HG3L.01.00.01 version of CANCUN.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(1,0,1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_allow_l3_ing_tag_actions,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
        CANCUN_FILE_BRANCH_ID_HG3L
    },
     /* For TD3, 'soc_feature_hgoe_line_card_mode'
      * is supported on B870-HGOE.6.8.0 version of CANCUN.
      */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,8,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_hgoe_line_card_mode,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
        CANCUN_FILE_BRANCH_ID_HGoE
    },
     /* For MV2, 'soc_feature_hgoe_line_card_mode'
      * is supported on B770-HGOE.4.7.0 version of CANCUN.
      */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,7,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_hgoe_line_card_mode,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
        CANCUN_FILE_BRANCH_ID_HGoE
    },
    /* For TD3, 'soc_feature_ipuc_riot_l3_iif_check'
     * supported only from 6.5.0.
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6, 5, 0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ipuc_riot_l3_iif_check,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For MV2, 'soc_feature_ipuc_riot_l3_iif_check'
     * supported only from 4.5.0.
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4, 5, 0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ipuc_riot_l3_iif_check,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HX5, 'soc_feature_ipuc_riot_l3_iif_check'
     * supported only from 4.3.0.
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4, 3, 0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_ipuc_riot_l3_iif_check,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
    },
    /* For HR4, 'soc_feature_hgoe_line_card_mode'
     * is supported on B275-HGOE.4.2.0 version of CANCUN.
     */
    {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,2,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_hgoe_line_card_mode,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
        CANCUN_FILE_BRANCH_ID_HGoE
     },
     /* For TD3, 'soc_feature_srv6_enable'
      * is supported on B870.6.8.0 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,8,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_srv6_enable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* For MV2, 'soc_feature_srv6_enable'
      * is supported on B770.4.9.0 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,9,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_srv6_enable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* For HX5, 'soc_feature_vxlan_gbp_flex'
      * is supported on B370.4.7.0 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,7,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_gbp_flex,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* For MV2, 'soc_feature_vxlan_gbp_flex'
      * is supported on B770.4.10.2 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,10,2),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_gbp_flex,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* For TD3, 'soc_feature_vxlan_gbp_flex'
      * is supported on B870.6.11.2 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,11,2),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_gbp_flex,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* For FB6, 'soc_feature_vxlan_gbp_flex'
      * is supported on B470.4.3.0 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,3,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_gbp_flex,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* For HR4, 'soc_feature_vxlan_gbp_flex'
      * is supported on B275.4.3.0 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,3,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_gbp_flex,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* 'soc_feature_mirror_erspan_gre_key_enable'
     * is supported on the mentioned devices in the captured
     * Cancun versions.
     * TD3: 6.11.1
     * MV2: 4.10.1
     * HX5: 4.7.0
     * HR4: 4.3.0
     * FB6: 4.3.0
     */
     {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,11,1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mirror_erspan_gre_key_enable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,10,1),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mirror_erspan_gre_key_enable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,7,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mirror_erspan_gre_key_enable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,3,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mirror_erspan_gre_key_enable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,3,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_mirror_erspan_gre_key_enable,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     /* 'soc_feature_l3mc_dnapt_snat'
     * is supported on the mentioned devices in the captured
     * Cancun versions.
     * TD3: 6.12.0
     * MV2: 4.11.0
     * HX5: 4.8.0
     */
     {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,12,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_l3mc_dnapt_snat,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,11,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_l3mc_dnapt_snat,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,8,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_l3mc_dnapt_snat,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     /* 'soc_feature_retain_ing_pkt_otpid'
     * is supported on the mentioned devices in the captured
     * Cancun versions.
     * TD3: 6.12.0
     * MV2: 4.11.0
     * HX5: 4.8.0
     * HR4: 4.4.0
     * FB6: 4.4.0
     */
     {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,12,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_retain_ing_pkt_otpid,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,11,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_retain_ing_pkt_otpid,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,8,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_retain_ing_pkt_otpid,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,4,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_retain_ing_pkt_otpid,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,4,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_retain_ing_pkt_otpid,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO
     },
     /* For TD3, 'soc_feature_vxlan_evpn_vlan_aware'
      * is supported on B870.6.12.0 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_VERSION(6,12,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_evpn_vlan_aware,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* For HR4, 'soc_feature_vxlan_evpn_vlan_aware'
      * is supported on B275.4.4.0 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_VERSION(4,4,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_evpn_vlan_aware,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* For MV2, 'soc_feature_vxlan_evpn_vlan_aware'
      * is supported on B770.4.11.0 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_VERSION(4,11,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_evpn_vlan_aware,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* For HX5, 'soc_feature_vxlan_evpn_vlan_aware'
      * is supported on B370.4.8.0 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_VERSION(4,8,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_evpn_vlan_aware,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
     /* For FB6, 'soc_feature_vxlan_evpn_vlan_aware'
      * is supported on B470.4.4.0 version of CANCUN.
      */
     {
        SOC_INFO_CHIP_TYPE_FIREBOLT6,
        SOC_CANCUN_VERSION(4,4,0),
        SOC_CANCUN_VERSION_DEF_INVALID,
        soc_feature_vxlan_evpn_vlan_aware,
        CANCUN_SOC_FEATURE_ENABLE,
        CANCUN_VER_CHECK_GREATER_THAN_OR_EQUAL_TO,
     },
};

static cancun_branch_version_map_t cancun_branch_version_map[] = {

    /* For HX5, B370-HGOEL.1.0.0 version of CANCUN
     * is based on B370.4.4.0 version of CANCUN and
     * should have the same soc_feature enables as
     * B370.4.4.0
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_HGoE,1,0,0),
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_DEF,4,4,0)
    },
    /* For TD3, B870-HG3L.1.0.1 version of CANCUN
     * is based on B870.6.5.0 version of CANCUN and
     * should have the same soc_feature enables as
     * B870.6.5.0
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_HG3L,1,0,1),
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_DEF,6,5,0)
    },
    /* For TD3, B870-HGOE.6.8.0 version of CANCUN
     * is based on B870.6.8.0 version of CANCUN and
     * should have the same soc_feature enables as
     * B870.6.8.0
     */
    {
        SOC_INFO_CHIP_TYPE_TRIDENT3,
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_HGoE,6,8,0),
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_DEF,6,8,0)
    },
    /* For MV2, B770-HGOE.4.7.0 version of CANCUN
     * is based on B770.4.7.0 version of CANCUN and
     * should have the same soc_feature enables as
     * B770.4.7.0
     */
    {
        SOC_INFO_CHIP_TYPE_MAVERICK2,
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_HGoE,4,7,0),
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_DEF,4,7,0)
    },
    /* For HX5, B370-HGOE.4.7.0 version of CANCUN
     * is based on B370.4.7.0 version of CANCUN and
     * should have the same soc_feature enables as
     * B770.4.7.0
     */
    {
        SOC_INFO_CHIP_TYPE_HELIX5,
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_HGoE,4,7,0),
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_DEF,4,7,0)
    },
    /* For HR4, B275-HGOE.4.2.0 version of CANCUN
     * is based on B275.4.2.0 version of CANCUN and
     * should have the same soc_feature enables as
     * B770.4.5.0
     */
    {
        SOC_INFO_CHIP_TYPE_HURRICANE4,
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_HGoE,4,2,0),
        SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_DEF,4,2,0)
    },
};

/* Additiona Mapping for branched cancun which deviates in features from
 * base cancun as mapped in cancun cancun_branch_version_map[].
 */
static soc_feature_cancun_version_map_t soc_feature_cancun_branch_exception_map[] = {

     {
         SOC_INFO_CHIP_TYPE_HELIX5,
         SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_HGoE,4,7,0),
         SOC_CANCUN_FULL_VERSION(CANCUN_FILE_BRANCH_ID_HGoE,4,7,0),
         soc_feature_mirror_erspan_gre_key_enable,
         CANCUN_SOC_FEATURE_DISABLE,
         CANCUN_VER_CHECK_IN_RANGE,
         CANCUN_FILE_BRANCH_ID_HGoE,
     },
};

#endif /* _CANCUN_FEATURE_MAP_H_ */
