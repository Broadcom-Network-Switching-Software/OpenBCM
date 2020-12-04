/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flex_ctr_ingress.c
 * Purpose:     Manage flex counter ingress group creation and deletion
 */

#include <shared/bsl.h>

#include <bcm_int/esw/flex_ctr.h>


static soc_reg_t _ingress_pkt_selector_key_reg[] = {
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_0r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_1r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_2r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_3r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_4r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_5r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_6r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_7r,
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || defined (BCM_APACHE_SUPPORT)
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_8r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_9r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_10r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_11r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_12r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_13r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_14r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_15r,
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined (BCM_APACHE_SUPPORT)
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_16r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_17r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_18r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_19r,
#endif
#if defined (BCM_APACHE_SUPPORT)
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_20r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_21r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_22r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_23r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_24r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_25r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_26r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_27r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_28r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_29r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_30r,
                 ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_31r
#endif
                 };

#define NUM_ING_KEYS_APACHE     8
#define NUM_ING_KEYS_TOMAHAWK   4
#define NUM_ING_KEYS_DEFAULT    2
#define NUM_ING_KEYS_METROLITE  1
#define NUM_ING_KEYS_TRIUMPH3   4

/*  Getting the number ingress keys for the device */
#define GET_NUM_UNCOMP_ING_KEYS(num, unit) \
        if (SOC_IS_APACHE(unit)) { \
            num = NUM_ING_KEYS_APACHE ; \
        } else if (SOC_IS_METROLITE(unit)) { \
            num = NUM_ING_KEYS_METROLITE ; \
        } else if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) { \
            num = NUM_ING_KEYS_TOMAHAWK ; \
        } else if (SOC_IS_TRIUMPH3(unit) && !SOC_IS_HELIX4(unit)) { \
            num = NUM_ING_KEYS_TRIUMPH3; \
        } else { \
            num = NUM_ING_KEYS_DEFAULT ; \
        }

#define GET_NUM_COMP_ING_KEYS(num, unit) \
        if (SOC_IS_APACHE(unit)) { \
            num = NUM_ING_KEYS_APACHE ; \
        } else if (SOC_IS_METROLITE(unit)) { \
            num = NUM_ING_KEYS_METROLITE ; \
        } else if (SOC_IS_FIREBOLT6(unit)) { \
            num = NUM_ING_KEYS_TOMAHAWK; \
        } else if (SOC_IS_TRIUMPH3(unit) && !SOC_IS_HELIX4(unit)) { \
            num = NUM_ING_KEYS_TRIUMPH3; \
        } else { \
            num = NUM_ING_KEYS_DEFAULT ; \
        }

#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)
/* Ingress Port map information */
bcmi_flex_ctr_subport_info_t ing_port_map_info[BCM_MAX_NUM_UNITS][4];
#endif

/*
 * Function:
 *      _bcm_esw_stat_flex_update_ingress_selector_keys
 * Description:
 *      Update flex ingress selector keys
 * Parameters:
 *      unit                  - (IN) unit number
 *      pkt_attr_type         - (IN) Flex Packet Attribute Type
 *      pkt_selector_key_reg  - (IN) Flex Packet Attribute selector key register
 *      pkt_attr_bits         - (IN) Flex Packet Attribute Bits
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
static bcm_error_t 
_bcm_esw_stat_flex_update_ingress_selector_keys(
    int                               unit,
    bcm_stat_flex_packet_attr_type_t  pkt_attr_type,
    soc_reg_t                         pkt_selector_key_reg,
    bcm_stat_flex_ing_pkt_attr_bits_t pkt_attr_bits)
{
    uint64        pkt_selector_key_reg_value, val64;
    uint8         current_bit_position = 0;
    uint8         index = 0;
    int           num_pkt_sel_key_reg = COUNTOF(_ingress_pkt_selector_key_reg);

    COMPILER_64_ZERO(pkt_selector_key_reg_value);
    COMPILER_64_ZERO(val64);
     
    for (index = 0; index < num_pkt_sel_key_reg; index++) {
         if (pkt_selector_key_reg == _ingress_pkt_selector_key_reg[index]) {
            break;
         }
    }
    if (index == num_pkt_sel_key_reg) {
           return BCM_E_PARAM;
    }
    /* bcmStatFlexPacketAttrTypeUdf not supported here */
    if (!((pkt_attr_type==bcmStatFlexPacketAttrTypeUncompressed) ||
          (pkt_attr_type==bcmStatFlexPacketAttrTypeCompressed))) {
           return BCM_E_PARAM;
    }
    /* Valid register value go ahead for setting
       ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_?r:SELECTOR_KEY field */

    /* First Get complete value of ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_?r value*/
    SOC_IF_ERROR_RETURN(soc_reg_get(unit,
                                    pkt_selector_key_reg,
                                    REG_PORT_ANY,
                                    0,
                                    &pkt_selector_key_reg_value));

    /* Next set field value for
    ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_?r:SELECTOR_KEY field*/
    soc_reg64_field_set(unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        USER_SPECIFIED_UDF_VALIDf,
                        val64);
    soc_reg64_field_set(unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        USE_UDF_KEYf,
                        val64);
    if (soc_reg_field_valid(unit, pkt_selector_key_reg, USE_COMPRESSED_PKT_KEYf)) {
        if (pkt_attr_type==bcmStatFlexPacketAttrTypeCompressed) {
            COMPILER_64_SET(val64, 0, 1);
            soc_reg64_field_set(unit,
                                pkt_selector_key_reg,
                                &pkt_selector_key_reg_value,
                                USE_COMPRESSED_PKT_KEYf,
                                val64);
        }else {
            soc_reg64_field_set(unit,
                                pkt_selector_key_reg,
                                &pkt_selector_key_reg_value,
                                USE_COMPRESSED_PKT_KEYf,
                                val64);
        }
    }
    if (pkt_attr_bits.ip_pkt != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                            unit,
                            pkt_selector_key_reg,
                            &pkt_selector_key_reg_value,
                            /* IP_PKT bit position */
                            pkt_attr_bits.ip_pkt_pos,
                            /* 1:IP_PKT total bits */
                            pkt_attr_bits.ip_pkt,
                            pkt_attr_bits.ip_pkt_mask,
                            &current_bit_position));
    }
    if (pkt_attr_bits.drop != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* DROP bit position */
                           pkt_attr_bits.drop_pos,
                           /*1:DROP total bits */
                           pkt_attr_bits.drop,
                           pkt_attr_bits.drop_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.svp_type != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*SVP_TYPE bit position*/
                           pkt_attr_bits.svp_type_pos,
                           /*1:SVP_TYPE total bits */
                           pkt_attr_bits.svp_type,
                           pkt_attr_bits.svp_type_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.pkt_resolution != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*PKT_RESOLUTION bit position*/
                           pkt_attr_bits.pkt_resolution_pos,
                           /*6:SVP_TYPE total bits */
                           pkt_attr_bits.pkt_resolution,
                           pkt_attr_bits.pkt_resolution_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.tos_ecn != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*TOS bit position*/
                           pkt_attr_bits.tos_ecn_pos,
                           /*8:TOS total bits */
                           pkt_attr_bits.tos_ecn,
                           pkt_attr_bits.tos_ecn_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.tos_dscp != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*TOS bit position*/
                           pkt_attr_bits.tos_dscp_pos,
                           /*8:TOS total bits */
                           pkt_attr_bits.tos_dscp,
                           pkt_attr_bits.tos_dscp_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.ing_port != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*ING_PORT bit position*/
                           pkt_attr_bits.ing_port_pos,
                           /*6:ING_PORT total bits */
                           pkt_attr_bits.ing_port,
                           pkt_attr_bits.ing_port_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.inner_dot1p != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*INNER_DOT1P bit position*/
                           pkt_attr_bits.inner_dot1p_pos,
                           /*3:INNER_DOT1P total bits */
                           pkt_attr_bits.inner_dot1p,
                           pkt_attr_bits.inner_dot1p_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.outer_dot1p != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*OUTER_DOT1P bit position*/
                           pkt_attr_bits.outer_dot1p_pos,
                           /*3:OUTER_DOT1P total bits */
                           pkt_attr_bits.outer_dot1p,
                           pkt_attr_bits.outer_dot1p_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.vlan_format != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*VLAN_FORMAT bit position*/
                           pkt_attr_bits.vlan_format_pos,
                           /*2:VLAN_FORMAT total bits */
                           pkt_attr_bits.vlan_format,
                           pkt_attr_bits.vlan_format_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.int_pri != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*INT_PRI bit position*/
                           pkt_attr_bits.int_pri_pos,
                           /*4:INT_PRI total bits */
                           pkt_attr_bits.int_pri,
                           pkt_attr_bits.int_pri_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.ifp_cng != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*IFP_CNG bit position*/
                           pkt_attr_bits.ifp_cng_pos,
                           /*2:IFP_CNG total bits */
                           pkt_attr_bits.ifp_cng,
                           pkt_attr_bits.ifp_cng_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.cng != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /*CNG bit position*/
                           pkt_attr_bits.cng_pos,
                           /*2:CNG total bits */
                           pkt_attr_bits.cng,
                           pkt_attr_bits.cng_mask,
                           &current_bit_position));
    }
#ifdef BCM_APACHE_SUPPORT
    if (soc_feature(unit,soc_feature_flex_ctr_mpls_3_label_count)) {
        if (pkt_attr_bits.phb_1 != 0) {
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                        unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        /* bit position */
                        pkt_attr_bits.phb_1_pos, /*  */
                        /* total bits */
                        pkt_attr_bits.phb_1,
                        pkt_attr_bits.phb_1_mask,
                        &current_bit_position));
        }
        if (pkt_attr_bits.cng_1 != 0) {
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                        unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        /* bit position */
                        pkt_attr_bits.cng_1_pos, /*  */
                        /* total bits */
                        pkt_attr_bits.cng_1,
                        pkt_attr_bits.cng_1_mask,
                        &current_bit_position));
        }
        if (pkt_attr_bits.phb_2 != 0) {
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                        unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        /* bit position */
                        pkt_attr_bits.phb_2_pos, /*  */
                        /* total bits */
                        pkt_attr_bits.phb_2,
                        pkt_attr_bits.phb_2_mask,
                        &current_bit_position));
        }
        if (pkt_attr_bits.cng_2 != 0) {
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                        unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        /* bit position */
                        pkt_attr_bits.cng_2_pos, /*  */
                        /* total bits */
                        pkt_attr_bits.cng_2,
                        pkt_attr_bits.cng_2_mask,
                        &current_bit_position));
        }
        if (pkt_attr_bits.phb_3 != 0) {
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                        unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        /* bit position */
                        pkt_attr_bits.phb_3_pos, /*  */
                        /* total bits */
                        pkt_attr_bits.phb_3,
                        pkt_attr_bits.phb_3_mask,
                        &current_bit_position));
        }
        if (pkt_attr_bits.cng_3 != 0) {
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                        unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        /* bit position */
                        pkt_attr_bits.cng_3_pos, /*  */
                        /* total bits */
                        pkt_attr_bits.cng_3,
                        pkt_attr_bits.cng_3_mask,
                        &current_bit_position));
        }
    }
#endif
    if (soc_feature(unit, soc_feature_flex_stat_ing_tcp_flags_support)) {
        if (pkt_attr_bits.tcp_flags != 0) {
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                        unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        /* bit position */
                        pkt_attr_bits.tcp_flags_pos, /*  */
                        /* total bits */
                        pkt_attr_bits.tcp_flags,
                        pkt_attr_bits.tcp_flags_mask,
                        &current_bit_position));
        }
    }
    if (soc_feature(unit, soc_feature_flex_stat_int_cn_support)) {
        if (pkt_attr_bits.int_cn != 0) {
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                        unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        /* bit position */
                        pkt_attr_bits.int_cn_pos, /*  */
                        /* total bits */
                        pkt_attr_bits.int_cn,
                        pkt_attr_bits.int_cn_mask,
                        &current_bit_position));
        }
    }
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        if (pkt_attr_bits.fc_type != 0) {
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                        unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        /* bit position */
                        pkt_attr_bits.fc_type_pos, /*  */
                        /* total bits */
                        pkt_attr_bits.fc_type,
                        pkt_attr_bits.fc_type_mask,
                        &current_bit_position));
        }
    }
    /* Finally set value for ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_?r */
    SOC_IF_ERROR_RETURN(soc_reg_set(unit,
                        pkt_selector_key_reg,
                        REG_PORT_ANY,
                        0,
                        pkt_selector_key_reg_value));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_stat_flex_create_ingress_uncompress_mode
 * Description:
 *      Creates New Flex Ingress Uncompressed Mode
 *
 * Parameters:
 *      unit                     - (IN) unit number
 *      uncmprsd_attr_selectors  - (IN) Flex attributes
 *      mode                     - (OUT) Flex mode
 *      total_counters           - (OUT) Total Counters associated with new
 *                                       flex uncompressed mode
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
static bcm_error_t 
_bcm_esw_stat_flex_create_ingress_uncompress_mode(
    int                                         unit,
    bcm_stat_flex_ing_uncmprsd_attr_selectors_t *uncmprsd_attr_selectors,
    bcm_stat_flex_mode_t                        *mode,
    uint32                                      *total_counters)
{
    bcm_stat_flex_mode_t                        mode_l=0;
    bcm_stat_flex_ing_pkt_attr_bits_t           pkt_attr_bits={0};
    uint32                                      total_ingress_bits=0;
    uint32                                      index=0;
    bcm_stat_flex_ingress_mode_t                *flex_ingress_mode=NULL;
    bcm_stat_flex_ing_uncmprsd_attr_selectors_t uncmprsd_attr_selectors_l;
    uint32                                      num_flex_ingress_pools;
    int                                         num_pkt_sel_key;

    num_flex_ingress_pools = SOC_INFO(unit).num_flex_ingress_pools;

    flex_ingress_mode = sal_alloc(sizeof(bcm_stat_flex_ingress_mode_t),
                                        "flex_ingress_mode");
    if (flex_ingress_mode == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(flex_ingress_mode, 0, sizeof(bcm_stat_flex_ingress_mode_t));

    for (index =0; index < BCM_STAT_FLEX_COUNTER_MAX_MODE; index++) {
         if (_bcm_esw_stat_flex_get_ingress_mode_info(
             unit,
             index,
             flex_ingress_mode) == BCM_E_NONE) {
             if (flex_ingress_mode->ing_attr.packet_attr_type !=
                 bcmStatFlexPacketAttrTypeUncompressed) {
                 continue;
             }
             uncmprsd_attr_selectors_l = flex_ingress_mode->ing_attr.
                                         uncmprsd_attr_selectors;
             if ((flex_ingress_mode->total_counters ==
                  uncmprsd_attr_selectors->total_counters) &&
                  (sal_memcmp(&uncmprsd_attr_selectors_l,uncmprsd_attr_selectors,
                         sizeof(uncmprsd_attr_selectors_l)) == 0)) {
                 *total_counters = flex_ingress_mode->total_counters;
                 *mode=index;
                 sal_free(flex_ingress_mode);
                 return BCM_E_EXISTS;
             }
         }
    }
    sal_free(flex_ingress_mode);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_available_mode(
                        unit,
                        bcmStatFlexDirectionIngress,
                        &mode_l));
    /* Step2: Packet Attribute selection */
#ifdef BCM_APACHE_SUPPORT
    if (soc_feature(unit,soc_feature_flex_ctr_mpls_3_label_count)) {
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_CNG_3_ATTR_BITS) {
            pkt_attr_bits.cng_3 = ing_pkt_attr_uncmprsd_bits_g[unit].cng_3;
            pkt_attr_bits.cng_3_mask = ing_pkt_attr_uncmprsd_bits_g[unit].cng_3_mask;
            pkt_attr_bits.cng_3_pos = ing_pkt_attr_uncmprsd_bits_g[unit].cng_3_pos;
            total_ingress_bits += pkt_attr_bits.cng_3;
        }
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_PHB_3_ATTR_BITS) {
            pkt_attr_bits.phb_3 = ing_pkt_attr_uncmprsd_bits_g[unit].phb_3;
            pkt_attr_bits.phb_3_mask = ing_pkt_attr_uncmprsd_bits_g[unit].phb_3_mask;
            pkt_attr_bits.phb_3_pos = ing_pkt_attr_uncmprsd_bits_g[unit].phb_3_pos;
            total_ingress_bits += pkt_attr_bits.phb_3;
        }
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_CNG_2_ATTR_BITS) {
            pkt_attr_bits.cng_2 = ing_pkt_attr_uncmprsd_bits_g[unit].cng_2;
            pkt_attr_bits.cng_2_mask = ing_pkt_attr_uncmprsd_bits_g[unit].cng_2_mask;
            pkt_attr_bits.cng_2_pos = ing_pkt_attr_uncmprsd_bits_g[unit].cng_2_pos;
            total_ingress_bits += pkt_attr_bits.cng_2;
        }
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_PHB_2_ATTR_BITS) {
            pkt_attr_bits.phb_2 = ing_pkt_attr_uncmprsd_bits_g[unit].phb_2;
            pkt_attr_bits.phb_2_mask = ing_pkt_attr_uncmprsd_bits_g[unit].phb_2_mask;
            pkt_attr_bits.phb_2_pos = ing_pkt_attr_uncmprsd_bits_g[unit].phb_2_pos;
            total_ingress_bits += pkt_attr_bits.phb_2;
        }
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_CNG_1_ATTR_BITS) {
            pkt_attr_bits.cng_1 = ing_pkt_attr_uncmprsd_bits_g[unit].cng_1;
            pkt_attr_bits.cng_1_mask = ing_pkt_attr_uncmprsd_bits_g[unit].cng_1_mask;
            pkt_attr_bits.cng_1_pos = ing_pkt_attr_uncmprsd_bits_g[unit].cng_1_pos;
            total_ingress_bits += pkt_attr_bits.cng_1;
        }
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_PHB_1_ATTR_BITS) {
            pkt_attr_bits.phb_1 = ing_pkt_attr_uncmprsd_bits_g[unit].phb_1;
            pkt_attr_bits.phb_1_mask = ing_pkt_attr_uncmprsd_bits_g[unit].phb_1_mask;
            pkt_attr_bits.phb_1_pos = ing_pkt_attr_uncmprsd_bits_g[unit].phb_1_pos;
            total_ingress_bits += pkt_attr_bits.phb_1;
        }
    }
#endif
    if (soc_feature(unit, soc_feature_flex_stat_ing_tcp_flags_support)) {
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_TCP_FLAG_ATTR_BITS) {
            pkt_attr_bits.tcp_flags = ing_pkt_attr_uncmprsd_bits_g[unit].tcp_flags;
            pkt_attr_bits.tcp_flags_mask = ing_pkt_attr_uncmprsd_bits_g[unit].tcp_flags_mask;
            pkt_attr_bits.tcp_flags_pos = ing_pkt_attr_uncmprsd_bits_g[unit].tcp_flags_pos;
            total_ingress_bits += pkt_attr_bits.tcp_flags;
        }
    }
    if (soc_feature(unit, soc_feature_flex_stat_int_cn_support)) {
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_INT_CN_ATTR_BITS) {
            pkt_attr_bits.int_cn = ing_pkt_attr_uncmprsd_bits_g[unit].int_cn;
            pkt_attr_bits.int_cn_mask = ing_pkt_attr_uncmprsd_bits_g[unit].int_cn_mask;
            pkt_attr_bits.int_cn_pos = ing_pkt_attr_uncmprsd_bits_g[unit].int_cn_pos;
            total_ingress_bits += pkt_attr_bits.int_cn;
        }
    }
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_FC_TYPE_ATTR_BITS) {
            pkt_attr_bits.fc_type = ing_pkt_attr_uncmprsd_bits_g[unit].fc_type;
            pkt_attr_bits.fc_type_mask = ing_pkt_attr_uncmprsd_bits_g[unit].fc_type_mask;
            pkt_attr_bits.fc_type_pos = ing_pkt_attr_uncmprsd_bits_g[unit].fc_type_pos;
            total_ingress_bits += pkt_attr_bits.fc_type;
        }
    }

if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_CNG_ATTR_BITS) {
        pkt_attr_bits.cng = ing_pkt_attr_uncmprsd_bits_g[unit].cng;
        pkt_attr_bits.cng_mask = ing_pkt_attr_uncmprsd_bits_g[unit].cng_mask;
        pkt_attr_bits.cng_pos = ing_pkt_attr_uncmprsd_bits_g[unit].cng_pos;
        total_ingress_bits +=pkt_attr_bits.cng;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_IFP_CNG_ATTR_BITS) {
        pkt_attr_bits.ifp_cng = ing_pkt_attr_uncmprsd_bits_g[unit].ifp_cng ;
        pkt_attr_bits.ifp_cng_mask = ing_pkt_attr_uncmprsd_bits_g[unit].ifp_cng_mask;
        pkt_attr_bits.ifp_cng_pos = ing_pkt_attr_uncmprsd_bits_g[unit].ifp_cng_pos;
        total_ingress_bits +=pkt_attr_bits.ifp_cng;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_INT_PRI_ATTR_BITS) {
        pkt_attr_bits.int_pri = ing_pkt_attr_uncmprsd_bits_g[unit].int_pri ;
        pkt_attr_bits.int_pri_mask = ing_pkt_attr_uncmprsd_bits_g[unit].int_pri_mask ;
        pkt_attr_bits.int_pri_pos = ing_pkt_attr_uncmprsd_bits_g[unit].int_pri_pos ;
        total_ingress_bits +=pkt_attr_bits.int_pri;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_VLAN_FORMAT_ATTR_BITS) {
        pkt_attr_bits.vlan_format = ing_pkt_attr_uncmprsd_bits_g[unit].vlan_format ;
        pkt_attr_bits.vlan_format_mask = ing_pkt_attr_uncmprsd_bits_g[unit].vlan_format_mask ;
        pkt_attr_bits.vlan_format_pos = ing_pkt_attr_uncmprsd_bits_g[unit].vlan_format_pos ;
        total_ingress_bits +=pkt_attr_bits.vlan_format;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_OUTER_DOT1P_ATTR_BITS) {
        pkt_attr_bits.outer_dot1p = ing_pkt_attr_uncmprsd_bits_g[unit].outer_dot1p;
        pkt_attr_bits.outer_dot1p_mask = ing_pkt_attr_uncmprsd_bits_g[unit].outer_dot1p_mask;
        pkt_attr_bits.outer_dot1p_pos = ing_pkt_attr_uncmprsd_bits_g[unit].outer_dot1p_pos;
        total_ingress_bits +=pkt_attr_bits.outer_dot1p;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_INNER_DOT1P_ATTR_BITS) {
        pkt_attr_bits.inner_dot1p = ing_pkt_attr_uncmprsd_bits_g[unit].inner_dot1p ;
        pkt_attr_bits.inner_dot1p_mask = ing_pkt_attr_uncmprsd_bits_g[unit].inner_dot1p_mask ;
        pkt_attr_bits.inner_dot1p_pos = ing_pkt_attr_uncmprsd_bits_g[unit].inner_dot1p_pos ;
        total_ingress_bits += pkt_attr_bits.inner_dot1p;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_INGRESS_PORT_ATTR_BITS) {
        pkt_attr_bits.ing_port = ing_pkt_attr_uncmprsd_bits_g[unit].ing_port; 
        pkt_attr_bits.ing_port_mask = ing_pkt_attr_uncmprsd_bits_g[unit].ing_port_mask ;
        pkt_attr_bits.ing_port_pos = ing_pkt_attr_uncmprsd_bits_g[unit].ing_port_pos ;
        total_ingress_bits += pkt_attr_bits.ing_port;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_TOS_DSCP_ATTR_BITS) {
        pkt_attr_bits.tos_dscp = ing_pkt_attr_uncmprsd_bits_g[unit].tos_dscp ;
        pkt_attr_bits.tos_dscp_mask = ing_pkt_attr_uncmprsd_bits_g[unit].tos_dscp_mask ;
        pkt_attr_bits.tos_dscp_pos = ing_pkt_attr_uncmprsd_bits_g[unit].tos_dscp_pos ;
        total_ingress_bits += pkt_attr_bits.tos_dscp;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_TOS_ECN_ATTR_BITS) {
        pkt_attr_bits.tos_ecn = ing_pkt_attr_uncmprsd_bits_g[unit].tos_ecn ;
        pkt_attr_bits.tos_ecn_mask = ing_pkt_attr_uncmprsd_bits_g[unit].tos_ecn_mask ;
        pkt_attr_bits.tos_ecn_pos = ing_pkt_attr_uncmprsd_bits_g[unit].tos_ecn_pos ;
        total_ingress_bits += pkt_attr_bits.tos_ecn;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_PKT_RESOLUTION_ATTR_BITS) {
        pkt_attr_bits.pkt_resolution = ing_pkt_attr_uncmprsd_bits_g[unit].pkt_resolution ;
        pkt_attr_bits.pkt_resolution_mask= ing_pkt_attr_uncmprsd_bits_g[unit].
                                           pkt_resolution_mask;
        pkt_attr_bits.pkt_resolution_pos= ing_pkt_attr_uncmprsd_bits_g[unit].
                                           pkt_resolution_pos;
        total_ingress_bits +=pkt_attr_bits.pkt_resolution ;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_SVP_TYPE_ATTR_BITS) {
        pkt_attr_bits.svp_type = ing_pkt_attr_uncmprsd_bits_g[unit].svp_type ;
        pkt_attr_bits.svp_type_mask = ing_pkt_attr_uncmprsd_bits_g[unit].svp_type_mask ;
        pkt_attr_bits.svp_type_pos = ing_pkt_attr_uncmprsd_bits_g[unit].svp_type_pos ;
        total_ingress_bits +=pkt_attr_bits.svp_type;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_DROP_ATTR_BITS) {
        pkt_attr_bits.drop = ing_pkt_attr_uncmprsd_bits_g[unit].drop ;
        pkt_attr_bits.drop_mask = ing_pkt_attr_uncmprsd_bits_g[unit].drop_mask ;
        pkt_attr_bits.drop_pos = ing_pkt_attr_uncmprsd_bits_g[unit].drop_pos ;
        total_ingress_bits += pkt_attr_bits.drop;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_IP_PKT_ATTR_BITS) {
        pkt_attr_bits.ip_pkt = ing_pkt_attr_uncmprsd_bits_g[unit].ip_pkt ;
        pkt_attr_bits.ip_pkt_mask = ing_pkt_attr_uncmprsd_bits_g[unit].ip_pkt_mask;
        pkt_attr_bits.ip_pkt_pos = ing_pkt_attr_uncmprsd_bits_g[unit].ip_pkt_pos;
        total_ingress_bits +=  pkt_attr_bits.ip_pkt;
    }
    if (total_ingress_bits > BCM_STAT_FLEX_MAX_PKT_ATTR_SEL_KEY_SIZE) {
        return BCM_E_PARAM;
    }
    /* Step3: Packet Attribute filling */

    /* APACHE supports 4 pkt keys per pool */
    GET_NUM_UNCOMP_ING_KEYS(num_pkt_sel_key, unit);
    for (index = 0; index < num_pkt_sel_key ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_ingress_selector_keys(unit,
                              bcmStatFlexPacketAttrTypeUncompressed,
                              _ingress_pkt_selector_key_reg[mode_l + (index*4)],
                              pkt_attr_bits));
    }
    /* Step4: Offset table filling */
    for (index = 0; index < num_flex_ingress_pools ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_offset_table(unit,
                              bcmStatFlexDirectionIngress,
                              ING_FLEX_CTR_OFFSET_TABLE_0m+index,
                              mode_l,
                              BCM_STAT_FLEX_MAX_COUNTER,
                              uncmprsd_attr_selectors->offset_table_map));
    }
    /* Step5: Final: reserve mode and return */
    *total_counters = uncmprsd_attr_selectors->total_counters;
    *mode=mode_l;
    return BCM_E_NONE;
}

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
/*
 * Function:
 *      bcmi_flex_ctr_cmpsd_port_sanitize
 * Description:
 *      Sanitize incoming ports for portmap.
 *      This method is also used for matching.
 *      if no subports are present in existing
 *      mode and new attributes then return "1"
 *      to imply that it is okay to move ahead for matching.
 *      non-subport matching is already taken care so
 *      this becomes a "dont-care".
 *
 * Parameters:
 *      unit                     - (IN) unit number
 *      mode                     - (IN) mode
 *      dir                      - (IN) dir
 *      icmprsd_attr_selectors  - (IN) ingress Flex attributes
 *      ecmprsd_attr_selectors  - (IN) egress Flex attributes
 *
 * Return Value:
 *      BCM_E_NONE - New Mode can be added.
 *      BCM_E_PARAM - Not valid programming.
 *      1           - No subport present in old or new mode. no sanitization required.
 * Notes:
 */
int
bcmi_flex_ctr_cmpsd_port_sanitize(int unit, int mode, int dir,
        bcm_stat_flex_ing_cmprsd_attr_selectors_t *icmprsd_attr_selectors,
        bcm_stat_flex_egr_cmprsd_attr_selectors_t *ecmprsd_attr_selectors,
        int match)
{

    int local_max_index;
    uint32 highest_mode_val = 0;
    int index;
    uint32 port_map_value=0, cmprsd_port_map_value=0;
    soc_mem_t mem;
    int local_highest_idx = 0;
    uint8 local_port_map[512];
    int valid_mode;
    int subport_present = 0;


    if (dir && (ecmprsd_attr_selectors == NULL)) {
        return BCM_E_NONE;
    } else if ((dir == 0) && ((icmprsd_attr_selectors == NULL))) {
        return BCM_E_NONE;
    }


    mem = ((dir) ? EGR_FLEX_CTR_PORT_MAPm : ING_FLEX_CTR_PORT_MAPm);
    local_max_index = soc_mem_index_count(unit, mem);

    if (soc_feature(unit, soc_feature_flex_ctr_lsb_port_ctrl)) {
         /* last 2 bits for port ctrl id are not used. */
         local_max_index /= 4;
    }

    if (dir) {
        valid_mode = egr_port_map_info[unit][mode].port_map_valid;
        sal_memcpy(local_port_map, egr_port_map_info[unit][mode].portmap, 512);
    } else {
        valid_mode = ing_port_map_info[unit][mode].port_map_valid;
        sal_memcpy(local_port_map, ing_port_map_info[unit][mode].portmap, 512);
    }

    if (!valid_mode) {
        /* If it is not valid mode, then for matching it should pass. */
        return match;
    }

    highest_mode_val = local_port_map[0];
    for (index=0; index< local_max_index-1; index++) {
        highest_mode_val = ((highest_mode_val > local_port_map[index+1]) ?
                                highest_mode_val : local_port_map[index+1]);
    }

    /* 72 physical ports. if map is not highest then return error. */
    for (index=0; index<72; index++) {

       /* This is invalid index for first mode. We can use it. */
        port_map_value = local_port_map[index];
        if (highest_mode_val != port_map_value) {
            return BCM_E_PARAM;
        }
     }
     local_highest_idx = 0;
     /* Get the highest index for the incoming list. */
     for (index=0; index< local_max_index; index++) {
         if (dir) {
             local_highest_idx = ((local_highest_idx >
                                 ecmprsd_attr_selectors->port_attr_map[index]) ?
                                     local_highest_idx :
                                     ecmprsd_attr_selectors->port_attr_map[index]);

         } else {
            local_highest_idx = ((local_highest_idx >
                                 icmprsd_attr_selectors->port_attr_map[index]) ?
                                     local_highest_idx :
                                     icmprsd_attr_selectors->port_attr_map[index]);
         }
     }

    /* Make sure that new incoming subport list are on invalid old indexes. */
    for (index=72; index<local_max_index; index++) {
        if (dir) {
            cmprsd_port_map_value = ecmprsd_attr_selectors->port_attr_map[index];
        } else {
            cmprsd_port_map_value = icmprsd_attr_selectors->port_attr_map[index];
        }

        if (cmprsd_port_map_value != local_highest_idx) {
            subport_present = 1;
        }
        if (local_port_map[index] != highest_mode_val) {
            /* if any subport is enabled in this mode then proceed for next checks. */
            subport_present=1;
        }

    }
    if (match && (!subport_present)) {
        /* As no subport is present, return PASS to say it matches. */
       return 1;
    }

    /* Make sure that new incoming subport list are on invalid old indexes. */
    for (index=0; index<local_max_index; index++) {
        if (dir) {
            cmprsd_port_map_value = ecmprsd_attr_selectors->port_attr_map[index];
        } else {
            cmprsd_port_map_value = icmprsd_attr_selectors->port_attr_map[index];
        }

        if (cmprsd_port_map_value != local_highest_idx) {
            /* If any physical port is added in map then return error. */
            if (index < 72) {
                return BCM_E_PARAM;
            }

            /* This is a valid index for this portmap. */
            port_map_value = local_port_map[index];
            /* This is invalid index for first mode. We can use it. */
            if (highest_mode_val != port_map_value)  {
                return BCM_E_PARAM;
            }
        }

    }
    /* Return Success to tell that no overlapping entry found. */
    return BCM_E_NONE;
}

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
void
bcmi_flex_ctr_portmap_info_init(int unit)
{

    int i =0;

    for (; i<BCM_STAT_FLEX_COUNTER_MAX_MODE; i++) {
        egr_port_map_info[unit][i].port_map_valid = 0;
        egr_port_map_info[unit][i].pool = -1;
        sal_memset(egr_port_map_info[unit][i].portmap, 0, 512);
        ing_port_map_info[unit][i].port_map_valid = 0;
        ing_port_map_info[unit][i].pool = -1;
        sal_memset(ing_port_map_info[unit][i].portmap, 0, 512);
    }
}
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

/*
 * Function:
 *      bcmi_flex_ctr_update_portmap
 * Description:
 *      update portmap based on multiple modes. This is done specially
 *      for subports as the total number of subports are more than 256.
 *      The assumption is normal logial ports are not part of compressed
 *      port map for any mode.
 *      The subports are exclusive for all the modes.
 *      No more than 2 modeis has portmap valid.
 * Parameters:
 *      unit                     - (IN) unit number
 *      mode                     - (IN) mode
 *      v                        - (IN) valid
 *      dir                      - (IN) dir
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_flex_ctr_update_portmap(int unit, int mode, int v, int dir)
{

    int local_max_index;
    uint32 highest_mode_val[4] = {0};
    int valid = 0, i, j,index;
    uint32 port_map_value=0, final_port_index=0;
    int alloc_size = 0;
    ing_flex_ctr_port_map_entry_t     *port_map_dma=NULL;
    soc_mem_t mem;
    int valid_mode[4];
    uint8 local_port_map[BCM_STAT_FLEX_COUNTER_MAX_MODE][512];
    int overall_high_val = 0;
    uint32 w_en = 1; /* enable/disable writing. */


    local_max_index = soc_mem_index_count(unit, ING_FLEX_CTR_PORT_MAPm);

    if ((v < 0) || (v > 1)) {
        return BCM_E_INTERNAL;
    }

    mem = ((dir) ? EGR_FLEX_CTR_PORT_MAPm : ING_FLEX_CTR_PORT_MAPm);

    if (soc_feature(unit, soc_feature_flex_ctr_lsb_port_ctrl)) {
         /* last 2 bits for port ctrl id are not used. */
         local_max_index /= 4;
    }
    if (dir) {
        egr_port_map_info[unit][mode].port_map_valid = v;
        egr_port_map_info[unit][mode].pool = -1;
        for (i = 0; i<4; i++) {
            valid_mode[i] = egr_port_map_info[unit][i].port_map_valid;
            sal_memcpy(local_port_map[i], egr_port_map_info[unit][i].portmap, 512);
        }
    } else {
        ing_port_map_info[unit][mode].port_map_valid = v;
        ing_port_map_info[unit][mode].pool = -1;
        for (i = 0; i<4; i++) {
            valid_mode[i] = ing_port_map_info[unit][i].port_map_valid;
            sal_memcpy(local_port_map[i], ing_port_map_info[unit][i].portmap, 512);
        }
    }

   for (i = 0; i<4; i++) {
        if (valid_mode[i]) {
            valid++;
        }
        if (valid > 2) {
            goto portmap_error;
        }
        for (index=0; index< local_max_index; index++) {
            highest_mode_val[i] = ((highest_mode_val[i] > local_port_map[i][index]) ?
                                    highest_mode_val[i] : local_port_map[i][index]);

        }
       overall_high_val = (overall_high_val > highest_mode_val[i]) ?
                         overall_high_val : highest_mode_val[i];
    }

    alloc_size = soc_mem_index_count(unit, mem) *
                sizeof(ing_flex_ctr_port_map_entry_t);
    port_map_dma = soc_cm_salloc(
                    unit,alloc_size, "port_map");
    if (port_map_dma == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(port_map_dma, 0,alloc_size);

    valid = -1;

    for (i = 0; i<4; i++) {
        if (!valid_mode[i]) {
            continue;
        }
        for (index=0; index<local_max_index; index++) {
            final_port_index = index * 4;

            /* Get the old Portmap value. */
            port_map_value = local_port_map[i][index];
            /*
             * During the second mode write, make sure invalid indexes are not written
             * if they are not highest invalid indexes.
             * Check with overall highest value to enable writing.
             */
            w_en = 1;
            if ((valid != -1) && (highest_mode_val[i] == port_map_value)) {
                /* The highest for this mode should not be less than overlal highest */
                if ((highest_mode_val[i]) >= overall_high_val) {
                    for (j=0; j<4; j++) {
                        if (!valid_mode[j]) {
                            continue;
                         }

                        if ((j != i) &&
                             (highest_mode_val[j] != local_port_map[j][index])) {
                            w_en = 0;
                        }
                    }
                } else {
                    w_en = 0;
                }
            }
            if (!w_en) {
                continue;
            }

            /* This is invalid index for first mode. We can use it. */
            soc_mem_field_set(
                        unit,
                        mem,
                        (uint32 *)&port_map_dma[final_port_index],
                        PORT_FNf,
                        &port_map_value);


        }
        valid = i;
    }
    if (soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                        soc_mem_index_min(unit, mem),
                        soc_mem_index_max(unit, mem),
                        port_map_dma) != BCM_E_NONE) {
        soc_cm_sfree(unit,port_map_dma);
        goto portmap_error;
   }

   soc_cm_sfree(unit,port_map_dma);
   return BCM_E_NONE;
portmap_error:
    if (dir) {
        egr_port_map_info[unit][mode].port_map_valid = 0;
    } else {
        ing_port_map_info[unit][mode].port_map_valid = 0;
    }

    soc_cm_sfree(unit,port_map_dma);
    return BCM_E_INTERNAL;

}
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

/*
 * Function:
 *      _bcm_esw_stat_flex_create_ingress_compress_mode
 * Description:
 *      Creates New Flex Ingress Compressed Mode
 * Parameters:
 *      unit                   - (IN) unit number
 *      cmprsd_attr_selectors  - (IN) Flex Compressed Attribute Selector
 *      mode                   - (OUT) Flex mode
 *      total_counters         - (OUT) Total Counters associated with new
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
static bcm_error_t 
_bcm_esw_stat_flex_create_ingress_compress_mode(
    int                                       unit,
    bcm_stat_flex_ing_cmprsd_attr_selectors_t *cmprsd_attr_selectors,
    bcm_stat_flex_mode_t                      *mode,
    uint32                                    *total_counters,
    bcm_stat_flex_attribute_t  *flex_attribute)
{
    bcm_stat_flex_mode_t              mode_l=0;
    bcm_stat_flex_ing_pkt_attr_bits_t pkt_attr_bits={0};
    uint32                            total_ingress_bits=0;
    uint32                            index=0;
    bcm_stat_flex_ingress_mode_t      *flex_ingress_mode=NULL;
    bcm_stat_flex_ing_pkt_attr_bits_t pkt_attr_bits_exist={0};
    uint32                            num_flex_ingress_pools;

    /*soc_mem_info_t                  *mem_info; */
#ifdef BCM_APACHE_SUPPORT
    uint32                            mpls_label_3_exist = 0;
    uint32                            cng_3_map_used = 0;
    uint32                            phb_3_map_used = 0;
    uint32                            cng_2_map_used = 0;
    uint32                            phb_2_map_used = 0;
    uint32                            cng_1_map_used = 0;
    uint32                            phb_1_map_used = 0;
#endif
    uint32                            pri_cnf_map_used=0;
    uint32                            pkt_pri_map_used=0;
    uint32                            port_map_used=0;
    uint32                            tos_map_used=0;
    uint32                            pkt_res_map_used=0;

    uint32                            alloc_size=0;
    ing_flex_ctr_pkt_pri_map_entry_t  *pkt_pri_map_dma=NULL;
    uint32                            pkt_pri_map_value=0;
    ing_flex_ctr_pkt_res_map_entry_t  *pkt_res_map_dma=NULL;
    uint32                            pkt_res_map_value=0;
    ing_flex_ctr_port_map_entry_t     *port_map_dma=NULL;
    uint32                            port_map_value=0;
    ing_flex_ctr_pri_cng_map_entry_t  *pri_cng_map_dma=NULL;
    uint32                            pri_cng_map_value=0;
    ing_flex_ctr_tos_map_entry_t      *tos_map_dma=NULL;
    uint32                            tos_map_value=0;
    int                               num_pkt_sel_key;
    int                               rv = BCM_E_NONE;

    num_flex_ingress_pools = SOC_INFO(unit).num_flex_ingress_pools;
    flex_ingress_mode = sal_alloc(sizeof(bcm_stat_flex_ingress_mode_t),
                                  "flex_ingress_mode");
    if (flex_ingress_mode == NULL) {
        return BCM_E_MEMORY;
    }
    
    sal_memset(flex_ingress_mode, 0, sizeof(bcm_stat_flex_ingress_mode_t));
    /* Step1: Compare with existing mode_ids. */
    if (soc_feature(unit, soc_feature_flex_stat_compression_share)) {
        rv = _bcm_esw_stat_flex_compress_mode_id_compare(unit,
                bcmStatFlexDirectionIngress,
                mode,
                flex_attribute);
        if (BCM_FAILURE(rv)) {
            sal_free(flex_ingress_mode);
            return rv;
        }
    } else {
        for (index =0 ; index < BCM_STAT_FLEX_COUNTER_MAX_MODE ; index++) {
            sal_memset(flex_ingress_mode,0,sizeof(bcm_stat_flex_ingress_mode_t));
            if (_bcm_esw_stat_flex_get_ingress_mode_info(
                        unit,
                        index,
                        flex_ingress_mode) == BCM_E_NONE) {
                if (flex_ingress_mode->ing_attr.packet_attr_type !=
                        bcmStatFlexPacketAttrTypeCompressed) {
                    continue;
                }
                pkt_attr_bits_exist = flex_ingress_mode->ing_attr.
                    cmprsd_attr_selectors.pkt_attr_bits;
#ifdef BCM_APACHE_SUPPORT
                if (soc_feature(unit,soc_feature_flex_ctr_mpls_3_label_count)) {
                    if ((pkt_attr_bits_exist.cng_3 ==
                                cmprsd_attr_selectors->pkt_attr_bits.cng_3) &&
                            (pkt_attr_bits_exist.phb_3 ==
                             cmprsd_attr_selectors->pkt_attr_bits.phb_3) &&
                            (pkt_attr_bits_exist.cng_2 ==
                             cmprsd_attr_selectors->pkt_attr_bits.cng_2) &&
                            (pkt_attr_bits_exist.phb_2 ==
                             cmprsd_attr_selectors->pkt_attr_bits.phb_2) &&
                            (pkt_attr_bits_exist.cng_1 ==
                             cmprsd_attr_selectors->pkt_attr_bits.cng_1) &&
                            (pkt_attr_bits_exist.phb_1 ==
                             cmprsd_attr_selectors->pkt_attr_bits.phb_1)) {
                        mpls_label_3_exist = 1;
                    }
                } else {
                    mpls_label_3_exist = 1;
                }
#endif
                if (
#ifdef BCM_APACHE_SUPPORT
                        mpls_label_3_exist &&
#endif
                        (pkt_attr_bits_exist.cng ==
                         cmprsd_attr_selectors->pkt_attr_bits.cng) &&
                        (pkt_attr_bits_exist.ifp_cng ==
                         cmprsd_attr_selectors->pkt_attr_bits.ifp_cng) &&
                        (pkt_attr_bits_exist.int_pri ==
                         cmprsd_attr_selectors->pkt_attr_bits.int_pri) &&
                        (pkt_attr_bits_exist.vlan_format ==
                         cmprsd_attr_selectors->pkt_attr_bits.vlan_format) &&
                        (pkt_attr_bits_exist.outer_dot1p ==
                         cmprsd_attr_selectors->pkt_attr_bits.outer_dot1p) &&
                        (pkt_attr_bits_exist.inner_dot1p ==
                         cmprsd_attr_selectors->pkt_attr_bits.inner_dot1p) &&
                        ((pkt_attr_bits_exist.ing_port ==
                         cmprsd_attr_selectors->pkt_attr_bits.ing_port)
#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
                         && ((soc_feature(unit,
                             soc_feature_channelized_switching)) &&
                             (bcmi_flex_ctr_cmpsd_port_sanitize(unit,
                                index, 0, cmprsd_attr_selectors, NULL, 1)) >= 1)
#endif
                        ) &&
                        (pkt_attr_bits_exist.tos_dscp ==
                         cmprsd_attr_selectors->pkt_attr_bits.tos_dscp) &&
                        (pkt_attr_bits_exist.tos_ecn ==
                         cmprsd_attr_selectors->pkt_attr_bits.tos_ecn) &&
                        (pkt_attr_bits_exist.pkt_resolution ==
                         cmprsd_attr_selectors->pkt_attr_bits.pkt_resolution) &&
                        (pkt_attr_bits_exist.svp_type ==
                         cmprsd_attr_selectors->pkt_attr_bits.svp_type) &&
                        (pkt_attr_bits_exist.drop ==
                         cmprsd_attr_selectors->pkt_attr_bits.drop) &&
                        (pkt_attr_bits_exist.ip_pkt ==
                         cmprsd_attr_selectors->pkt_attr_bits.ip_pkt)) {
                             *total_counters = flex_ingress_mode->total_counters;
                             *mode=index;
                             sal_free(flex_ingress_mode);
                             return BCM_E_EXISTS;
                         }
#ifdef BCM_APACHE_SUPPORT
                if (soc_feature(unit,soc_feature_flex_ctr_mpls_3_label_count)) {
                    if (pkt_attr_bits_exist.cng_3 != 0) {
                        cng_3_map_used = 1;
                    }
                    if (pkt_attr_bits_exist.phb_3 != 0) {
                        phb_3_map_used = 1;
                    }
                    if (pkt_attr_bits_exist.cng_2 != 0) {
                        cng_2_map_used = 1;
                    }
                    if (pkt_attr_bits_exist.phb_2 != 0) {
                        phb_2_map_used = 1;
                    }
                    if (pkt_attr_bits_exist.cng_1 != 0) {
                        cng_1_map_used = 1;
                    }
                    if (pkt_attr_bits_exist.phb_1 != 0) {
                        phb_1_map_used = 1;
                    }
                }
#endif
                if ((pkt_attr_bits_exist.cng) ||
                        (pkt_attr_bits_exist.ifp_cng)  ||
                        (pkt_attr_bits_exist.int_pri)) {
                    pri_cnf_map_used=1;
                }
                if ((pkt_attr_bits_exist.vlan_format) ||
                        (pkt_attr_bits_exist.outer_dot1p) ||
                        (pkt_attr_bits_exist.inner_dot1p)) {
                    pkt_pri_map_used=1;
                }
                if (pkt_attr_bits_exist.ing_port) {
                    port_map_used=1;
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                    if (soc_feature(unit, soc_feature_channelized_switching)) {
                         /* subport related check to see of exclusive ports are there.*/
                        if ((bcmi_flex_ctr_cmpsd_port_sanitize(unit,
                                index, 0, cmprsd_attr_selectors, NULL, 0)) == 0) {
                            port_map_used=0;
                        }
                    }
#endif
                }
                if ((pkt_attr_bits_exist.tos_dscp) ||
                        (pkt_attr_bits_exist.tos_ecn)) {
                    tos_map_used=1;
                }
                if ((pkt_attr_bits_exist.pkt_resolution) ||
                        (pkt_attr_bits_exist.svp_type) ||
                        (pkt_attr_bits_exist.drop)) {
                    pkt_res_map_used=1;
                }
            }
        }
    }
    sal_free(flex_ingress_mode);

    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_available_mode(
                        unit,
                        bcmStatFlexDirectionIngress,
                        &mode_l));
    if (cmprsd_attr_selectors->pkt_attr_bits.cng !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.cng > 
            ing_pkt_attr_cmprsd_bits_g[unit].cng) {
            return BCM_E_PARAM;
        }
        if (pri_cnf_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.cng;
    }
#ifdef BCM_APACHE_SUPPORT
    if (soc_feature(unit,soc_feature_flex_ctr_mpls_3_label_count)) {
        if (cmprsd_attr_selectors->pkt_attr_bits.cng_3 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.cng_3 >
                    ing_pkt_attr_cmprsd_bits_g[unit].cng_3) {
                return BCM_E_PARAM;
            }
            if (cng_3_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_ingress_bits += cmprsd_attr_selectors->pkt_attr_bits.cng_3;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.phb_3 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.phb_3 >
                    ing_pkt_attr_cmprsd_bits_g[unit].phb_3) {
                return BCM_E_PARAM;
            }
            if (phb_3_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_ingress_bits += cmprsd_attr_selectors->pkt_attr_bits.phb_3;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.cng_2 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.cng_2 >
                    ing_pkt_attr_cmprsd_bits_g[unit].cng_2) {
                return BCM_E_PARAM;
            }
            if (cng_2_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_ingress_bits += cmprsd_attr_selectors->pkt_attr_bits.cng_2;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.phb_2 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.phb_2 >
                    ing_pkt_attr_cmprsd_bits_g[unit].phb_2) {
                return BCM_E_PARAM;
            }
            if (phb_2_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_ingress_bits += cmprsd_attr_selectors->pkt_attr_bits.phb_2;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.cng_1 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.cng_1 >
                    ing_pkt_attr_cmprsd_bits_g[unit].cng_1) {
                return BCM_E_PARAM;
            }
            if (cng_1_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_ingress_bits += cmprsd_attr_selectors->pkt_attr_bits.cng_1;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.phb_1 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.phb_1 >
                    ing_pkt_attr_cmprsd_bits_g[unit].phb_1) {
                return BCM_E_PARAM;
            }
            if (phb_1_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_ingress_bits += cmprsd_attr_selectors->pkt_attr_bits.phb_1;
        }
    }
#endif
    if (cmprsd_attr_selectors->pkt_attr_bits.ifp_cng !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.ifp_cng >
            ing_pkt_attr_cmprsd_bits_g[unit].ifp_cng) {
            return BCM_E_PARAM;
        }
        if (pri_cnf_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.ifp_cng;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.int_pri !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.int_pri > 
            ing_pkt_attr_cmprsd_bits_g[unit].int_pri) {
            return BCM_E_PARAM;
        }
        if (pri_cnf_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.int_pri;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.vlan_format !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.vlan_format >
            ing_pkt_attr_cmprsd_bits_g[unit].vlan_format) {
            return BCM_E_PARAM;
        }
        if (pkt_pri_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.vlan_format;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.outer_dot1p !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.outer_dot1p  > 
            ing_pkt_attr_cmprsd_bits_g[unit].outer_dot1p) {
            return BCM_E_PARAM;
        }
        if (pkt_pri_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.outer_dot1p ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.inner_dot1p !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.inner_dot1p  >
            ing_pkt_attr_cmprsd_bits_g[unit].inner_dot1p) {
            return BCM_E_PARAM;
        }
        if (pkt_pri_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.inner_dot1p ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.ing_port !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.ing_port  > 
            ing_pkt_attr_cmprsd_bits_g[unit].ing_port) {
            return BCM_E_PARAM;
        }
        if (port_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.ing_port ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.tos_dscp !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.tos_dscp  > 
            ing_pkt_attr_cmprsd_bits_g[unit].tos_dscp) {
            return BCM_E_PARAM;
        }
        if (tos_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.tos_dscp ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.tos_ecn !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.tos_ecn  > 
            ing_pkt_attr_cmprsd_bits_g[unit].tos_ecn) {
            return BCM_E_PARAM;
        }
        if (tos_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.tos_ecn ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.pkt_resolution !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.pkt_resolution  > 
            ing_pkt_attr_cmprsd_bits_g[unit].pkt_resolution) {
            return BCM_E_PARAM;
        }
        if (pkt_res_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.
                               pkt_resolution ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.svp_type !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.svp_type  > 
            ing_pkt_attr_cmprsd_bits_g[unit].svp_type) {
            return BCM_E_PARAM;
        }
        if (pkt_res_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.svp_type ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.drop        !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.drop > 
            ing_pkt_attr_cmprsd_bits_g[unit].drop) {
            return BCM_E_PARAM;
        }
        if (pkt_res_map_used==1) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.drop ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.ip_pkt        !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.ip_pkt > 
            ing_pkt_attr_cmprsd_bits_g[unit].ip_pkt) {
            return BCM_E_PARAM;
        }
        total_ingress_bits +=cmprsd_attr_selectors->pkt_attr_bits.ip_pkt ;
    }
    if (total_ingress_bits > 8) {
        return BCM_E_PARAM;
    }
    pkt_attr_bits = cmprsd_attr_selectors->pkt_attr_bits;

    if (soc_feature(unit, soc_feature_flex_stat_compression_share)) {
        /* Write global compression tables */
        rv = _bcm_stat_flex_compressed_attr_tbl_write(unit,
                bcmStatFlexDirectionIngress,
                cmprsd_attr_selectors,NULL);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        /* Step3: Fill up map array */
        if ((pkt_attr_bits.cng != 0) ||
                (pkt_attr_bits.ifp_cng)  ||
                (pkt_attr_bits.int_pri)) {
            alloc_size = soc_mem_index_count(unit,ING_FLEX_CTR_PRI_CNG_MAPm) *
                sizeof(ing_flex_ctr_pri_cng_map_entry_t);

            pri_cng_map_dma=soc_cm_salloc( unit,alloc_size, "pri_cng_map");
            if (pri_cng_map_dma == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(pri_cng_map_dma, 0,alloc_size);
            if (soc_mem_read_range(
                        unit,
                        ING_FLEX_CTR_PRI_CNG_MAPm,
                        MEM_BLOCK_ANY,
                        soc_mem_index_min(unit,ING_FLEX_CTR_PRI_CNG_MAPm),
                        soc_mem_index_max(unit,ING_FLEX_CTR_PRI_CNG_MAPm),
                        pri_cng_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,pri_cng_map_dma);
                return BCM_E_INTERNAL;
            }
            for (index=0;
                    index< soc_mem_index_count(unit,ING_FLEX_CTR_PRI_CNG_MAPm);
                    index++) {
                pri_cng_map_value=cmprsd_attr_selectors->pri_cnf_attr_map[index];
                soc_mem_field_set(
                        unit,
                        ING_FLEX_CTR_PRI_CNG_MAPm,
                        (uint32 *)&pri_cng_map_dma[index],
                        PRI_CNG_FNf,
                        &pri_cng_map_value);
            }
            if (soc_mem_write_range(
                        unit,
                        ING_FLEX_CTR_PRI_CNG_MAPm,
                        MEM_BLOCK_ALL,
                        soc_mem_index_min(unit,ING_FLEX_CTR_PRI_CNG_MAPm),
                        soc_mem_index_max(unit,ING_FLEX_CTR_PRI_CNG_MAPm),
                        pri_cng_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,pri_cng_map_dma);
                return BCM_E_INTERNAL;
            }
            soc_cm_sfree(unit,pri_cng_map_dma);
        }
        if ((pkt_attr_bits.vlan_format != 0) ||
                (pkt_attr_bits.outer_dot1p) ||
                (pkt_attr_bits.inner_dot1p)) {
            /* Sanity Check */
            alloc_size = soc_mem_index_count(unit, ING_FLEX_CTR_PKT_PRI_MAPm) *
                sizeof(ing_flex_ctr_pkt_pri_map_entry_t);
            pkt_pri_map_dma = soc_cm_salloc( unit,alloc_size, "pkt_pri_map");
            if (pkt_pri_map_dma == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(pkt_pri_map_dma, 0,alloc_size);
            if (soc_mem_read_range(
                        unit,
                        ING_FLEX_CTR_PKT_PRI_MAPm,
                        MEM_BLOCK_ANY,
                        soc_mem_index_min(unit,ING_FLEX_CTR_PKT_PRI_MAPm),
                        soc_mem_index_max(unit,ING_FLEX_CTR_PKT_PRI_MAPm),
                        pkt_pri_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,pkt_pri_map_dma);
                return BCM_E_INTERNAL;
            }
            for (index=0;
                    index< soc_mem_index_count(unit, ING_FLEX_CTR_PKT_PRI_MAPm);
                    index++) {
                pkt_pri_map_value= cmprsd_attr_selectors->pkt_pri_attr_map[index];
                soc_mem_field_set(
                        unit,
                        ING_FLEX_CTR_PKT_PRI_MAPm,
                        (uint32 *)&pkt_pri_map_dma[index],
                        PKT_PRI_FNf,
                        &pkt_pri_map_value);
            }
            if (soc_mem_write_range(
                        unit,
                        ING_FLEX_CTR_PKT_PRI_MAPm,
                        MEM_BLOCK_ALL,
                        soc_mem_index_min(unit,ING_FLEX_CTR_PKT_PRI_MAPm),
                        soc_mem_index_max(unit,ING_FLEX_CTR_PKT_PRI_MAPm),
                        pkt_pri_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,pkt_pri_map_dma);
                return BCM_E_INTERNAL;
            }
            soc_cm_sfree(unit,pkt_pri_map_dma);
        }
        if ((pkt_attr_bits.ing_port != 0) &&
            (!(soc_feature(unit, soc_feature_channelized_switching)))) {

            int local_max_index = 0;
            int final_port_index = 0;
            int index1 = 0;

            alloc_size = soc_mem_index_count(unit, ING_FLEX_CTR_PORT_MAPm) *
                sizeof(ing_flex_ctr_port_map_entry_t);
            port_map_dma = soc_cm_salloc(
                    unit,alloc_size, "port_map");
            if (port_map_dma == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(port_map_dma, 0,alloc_size);
            if (soc_mem_read_range(
                        unit,
                        ING_FLEX_CTR_PORT_MAPm,
                        MEM_BLOCK_ANY,
                        soc_mem_index_min(unit,ING_FLEX_CTR_PORT_MAPm),
                        soc_mem_index_max(unit,ING_FLEX_CTR_PORT_MAPm),
                        port_map_dma) != BCM_E_NONE) {
                soc_cm_sfree(unit,port_map_dma);
                return BCM_E_INTERNAL;
            }
            local_max_index = soc_mem_index_count(unit, ING_FLEX_CTR_PORT_MAPm);
            if (soc_feature(unit, soc_feature_flex_ctr_lsb_port_ctrl)) {
                /* extra space is given. Only first quarter is used. */
                local_max_index /= 4;
            }

            for (index=0;
                    index< local_max_index;
                    index++) {

                final_port_index = index;
                index1 = 0;
                if (soc_feature(unit, soc_feature_flex_ctr_lsb_port_ctrl)) {
                    /* extra space is given. Only first quarter is used. */
                    final_port_index = index * 4;
                }

                port_map_value = cmprsd_attr_selectors->port_attr_map[index];

#ifdef BCM_HURRICANE4_SUPPORT
                    if (SOC_IS_HURRICANE4(unit)) {
#if defined(INCLUDE_XFLOW_MACSEC)
                        bcm_xflow_macsec_port_info_t port_info;
                        bcm_xflow_macsec_port_info_t_init(&port_info);
                        if (soc_feature(unit, soc_feature_xflow_macsec)) {
                            bcm_common_xflow_macsec_port_info_get(unit, index, &port_info);
                        }
                        if (port_info.enable) {
                            index1 = 3;
                        }
#endif /* INCLUDE_XFLOW_MACSEC */
                    }
#endif /* BCM_HURRICANE4_SUPPORT*/
                final_port_index += index1;

                soc_mem_field_set(
                        unit,
                        ING_FLEX_CTR_PORT_MAPm,
                        (uint32 *)&port_map_dma[final_port_index],
                        PORT_FNf,
                        &port_map_value);
            }

            if (soc_mem_write_range(
                        unit,
                        ING_FLEX_CTR_PORT_MAPm,
                        MEM_BLOCK_ALL,
                        soc_mem_index_min(unit,ING_FLEX_CTR_PORT_MAPm),
                        soc_mem_index_max(unit,ING_FLEX_CTR_PORT_MAPm),
                        port_map_dma) != BCM_E_NONE) {
                soc_cm_sfree(unit,port_map_dma);
                return BCM_E_INTERNAL;
            }
            soc_cm_sfree(unit,port_map_dma); 
        }
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        if ((pkt_attr_bits.ing_port != 0) &&
            soc_feature(unit, soc_feature_channelized_switching)) {
            int local_max_index = 0;

            local_max_index = soc_mem_index_count(unit, ING_FLEX_CTR_PORT_MAPm);
            if (soc_feature(unit, soc_feature_flex_ctr_lsb_port_ctrl)) {
                /* Last 2 bits are port ctrl id, which are unused. */
                local_max_index /= 4;
            }


            for (index=0;
                    index< local_max_index;
                    index++) {
                ing_port_map_info[unit][mode_l].portmap[index] =
                    cmprsd_attr_selectors->port_attr_map[index];
            }

            BCM_IF_ERROR_RETURN(
                bcmi_flex_ctr_update_portmap(unit, mode_l, 1, 0));
        }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

        if ((pkt_attr_bits.tos_dscp != 0) ||
                (pkt_attr_bits.tos_ecn != 0)) {
            alloc_size = soc_mem_index_count(unit,ING_FLEX_CTR_TOS_MAPm) *
                sizeof(ing_flex_ctr_tos_map_entry_t);
            tos_map_dma = soc_cm_salloc( unit,alloc_size, "tos_map");
            if (tos_map_dma == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(tos_map_dma, 0,alloc_size);
            if (soc_mem_read_range(
                        unit,
                        ING_FLEX_CTR_TOS_MAPm,
                        MEM_BLOCK_ANY,
                        soc_mem_index_min(unit,ING_FLEX_CTR_TOS_MAPm),
                        soc_mem_index_max(unit,ING_FLEX_CTR_TOS_MAPm),
                        tos_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,tos_map_dma);
                return BCM_E_INTERNAL;
            }
            for (index=0;
                    index< soc_mem_index_count(unit,ING_FLEX_CTR_TOS_MAPm);
                    index++) {
                tos_map_value = cmprsd_attr_selectors->tos_attr_map[index];
                soc_mem_field_set(
                        unit,
                        ING_FLEX_CTR_TOS_MAPm,
                        (uint32 *)&tos_map_dma[index],
                        TOS_FNf,
                        &tos_map_value);
            }
            if (soc_mem_write_range(
                        unit,
                        ING_FLEX_CTR_TOS_MAPm,
                        MEM_BLOCK_ALL,
                        soc_mem_index_min(unit,ING_FLEX_CTR_TOS_MAPm),
                        soc_mem_index_max(unit,ING_FLEX_CTR_TOS_MAPm),
                        tos_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,tos_map_dma);
                return BCM_E_INTERNAL;
            }
            soc_cm_sfree(unit,tos_map_dma);
        }
        if ((pkt_attr_bits.pkt_resolution != 0) ||
                (pkt_attr_bits.svp_type) ||
                (pkt_attr_bits.drop)){
            alloc_size = soc_mem_index_count(unit, ING_FLEX_CTR_PKT_RES_MAPm) *
                sizeof(ing_flex_ctr_pkt_res_map_entry_t);
            pkt_res_map_dma= soc_cm_salloc( unit, alloc_size, "pkt_res_map");
            if (pkt_res_map_dma == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(pkt_res_map_dma, 0,alloc_size);
            if (soc_mem_read_range(
                        unit,
                        ING_FLEX_CTR_PKT_RES_MAPm,
                        MEM_BLOCK_ANY,
                        soc_mem_index_min(unit,ING_FLEX_CTR_PKT_RES_MAPm),
                        soc_mem_index_max(unit,ING_FLEX_CTR_PKT_RES_MAPm),
                        pkt_res_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,pkt_res_map_dma);
                return BCM_E_INTERNAL;
            }
            for (index=0;
                    index< soc_mem_index_count(unit, ING_FLEX_CTR_PKT_RES_MAPm);
                    index++) {
                pkt_res_map_value = cmprsd_attr_selectors->pkt_res_attr_map[index];
                soc_mem_field_set(
                        unit,
                        ING_FLEX_CTR_PKT_RES_MAPm,
                        (uint32 *)&pkt_res_map_dma[index],
                        PKT_RES_FNf,
                        &pkt_res_map_value);
            }
            if (soc_mem_write_range(unit,
                        ING_FLEX_CTR_PKT_RES_MAPm,
                        MEM_BLOCK_ALL,
                        soc_mem_index_min(unit,ING_FLEX_CTR_PKT_RES_MAPm),
                        soc_mem_index_max(unit,ING_FLEX_CTR_PKT_RES_MAPm),
                        pkt_res_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,pkt_res_map_dma);
                return BCM_E_INTERNAL;
            }
            soc_cm_sfree(unit,pkt_res_map_dma); 
        }
    }
    /* Step4: Packet Attribute filling */
    /* APACHE supports 4 pkt keys per pool */
    GET_NUM_COMP_ING_KEYS(num_pkt_sel_key, unit);
    for (index = 0; index < num_pkt_sel_key ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_ingress_selector_keys(
                             unit,
                             bcmStatFlexPacketAttrTypeCompressed,
                             _ingress_pkt_selector_key_reg[mode_l+(index*4)],
                             pkt_attr_bits));
    }
    /* Step5: Offset table filling */
    for (index=0;index < num_flex_ingress_pools ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_offset_table(
                             unit,
                             bcmStatFlexDirectionIngress,
                             ING_FLEX_CTR_OFFSET_TABLE_0m+index,
                             mode_l,
                             256,
                             cmprsd_attr_selectors->offset_table_map));
    }
    /* Step6: Final: reserve mode and return */
    *total_counters = cmprsd_attr_selectors->total_counters;
    *mode=mode_l;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_stat_flex_create_ingress_udf_mode
 * Description:
 *      Creates New Flex Ingress UDF(User Defined Field) Mode
 * Parameters:
 *      unit                    - (IN) unit number
 *      udf_pkt_attr_selectors  - (IN) Flex attributes
 *      mode                    - (OUT) Flex mode
 *      total_counters          - (OUT) Total Counters associated with new
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *       Not being used currently
 */
static bcm_error_t 
_bcm_esw_stat_flex_create_ingress_udf_mode(
    int                                    unit,
    bcm_stat_flex_udf_pkt_attr_selectors_t *udf_pkt_attr_selectors,
    bcm_stat_flex_offset_table_entry_t     *offset_table,
    bcm_stat_flex_mode_t                   *mode,
    uint32                                 *total_counters)
{
    bcm_stat_flex_mode_t              mode_l=0;
    uint32                            total_udf_bits=0;
    uint32                            index=0;
    bcm_stat_flex_ingress_mode_t      *flex_ingress_mode=NULL;
    bcm_stat_flex_udf_pkt_attr_bits_t udf_pkt_attr_bits_l;
    uint32                            num_flex_ingress_pools;
    int                               num_pkt_sel_key;

    num_flex_ingress_pools=SOC_INFO(unit).num_flex_ingress_pools;
    flex_ingress_mode = sal_alloc(sizeof(bcm_stat_flex_ingress_mode_t),
                                  "flex_ingress_mode");
    if (flex_ingress_mode == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(flex_ingress_mode, 0, sizeof(bcm_stat_flex_ingress_mode_t));    

    for (index =0 ; index < BCM_STAT_FLEX_COUNTER_MAX_MODE ; index++) {
         sal_memset(flex_ingress_mode,0,sizeof(bcm_stat_flex_ingress_mode_t));
         if (_bcm_esw_stat_flex_get_ingress_mode_info(
             unit,
             index,
             flex_ingress_mode) == BCM_E_NONE) {
             if (flex_ingress_mode->ing_attr.packet_attr_type != 
                 bcmStatFlexPacketAttrTypeUdf) {
                 continue;
             }
             udf_pkt_attr_bits_l = flex_ingress_mode->ing_attr.
                                   udf_pkt_attr_selectors.udf_pkt_attr_bits;
             if ((udf_pkt_attr_bits_l.udf0==
                  udf_pkt_attr_selectors->udf_pkt_attr_bits.udf0) &&
                 (udf_pkt_attr_bits_l.udf1==
                  udf_pkt_attr_selectors->udf_pkt_attr_bits.udf1)) {
                  *total_counters = flex_ingress_mode->total_counters;
                  *mode=index;
                  sal_free(flex_ingress_mode);
                  return BCM_E_EXISTS;
             }
         }
    }
    sal_free(flex_ingress_mode);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_available_mode(
                        unit,
                        bcmStatFlexDirectionIngress,
                        &mode_l));
    /* Step2: Packet Attribute filling */
    /* APACHE supports 4 pkt keys per pool */
    GET_NUM_COMP_ING_KEYS(num_pkt_sel_key, unit);
    for (index = 0; index < num_pkt_sel_key; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_udf_selector_keys(
                             unit,
                             bcmStatFlexDirectionIngress,
                             _ingress_pkt_selector_key_reg[mode_l+(index*4)],
                             udf_pkt_attr_selectors,
                             &total_udf_bits));
    }
    /* Step3: Offset table filling */
    for (index=0;index < num_flex_ingress_pools ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_offset_table(
                             unit,
                             bcmStatFlexDirectionIngress,
                             ING_FLEX_CTR_OFFSET_TABLE_0m+index,
                             mode_l,
                             256,
                             offset_table));
    }
    /* Step4: Final: reserve mode and return */
    *total_counters = udf_pkt_attr_selectors->total_counters;
    *mode=mode_l;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_stat_flex_create_ingress_mode
 * Description:
 *      Creates New Flex Ingress Mode(Either uncompressd or compressed or udf)
 *
 * Parameters:
 *      unit      - (IN) unit number
 *      ing_attr  - (IN) Flex Ingress attributes
 *      mode      - (OUT) Flex mode
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t _bcm_esw_stat_flex_create_ingress_mode (
            int                      unit,
            bcm_stat_flex_ing_attr_t *ing_attr,
            bcm_stat_flex_mode_t     *mode,
            bcm_stat_flex_attribute_t  *flex_attribute)
{
    uint32        total_counters=0;
    switch(ing_attr->packet_attr_type) {
    case bcmStatFlexPacketAttrTypeUncompressed:
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Creating Ingress uncompressed mode \n")));
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_create_ingress_uncompress_mode(
                             unit,
                             &(ing_attr->uncmprsd_attr_selectors),
                             mode,&total_counters));
         break;
    case bcmStatFlexPacketAttrTypeCompressed:
        if (SOC_IS_TOMAHAWK3(unit)) {
            LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                      (BSL_META_U(unit,
                                  "bcmStatFlexPacketAttrTypeCompressed IsNotAvailable"
                                   "for %s\n"), SOC_UNIT_NAME(unit)));
            return BCM_E_UNAVAIL;
        }
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Creating Ingress compressed mode \n")));
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_create_ingress_compress_mode(
                             unit,
                             &(ing_attr->cmprsd_attr_selectors),
                             mode,&total_counters,flex_attribute));
         break;
    case bcmStatFlexPacketAttrTypeUdf:
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Creating Ingress udf mode \n")));
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_create_ingress_udf_mode(
                             unit,
                             &(ing_attr->udf_pkt_attr_selectors),
                             (ing_attr->uncmprsd_attr_selectors).offset_table_map,
                             mode,&total_counters));
         break;
    default:
         return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_ingress_reserve_mode( 
                        unit,
                        *mode,
                        total_counters,
                        ing_attr));
    LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
              (BSL_META_U(unit,
                          "\n Done %d \n"),
               *mode));
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_stat_flex_delete_ingress_mode
 * Description:
 *      Destroys New Flex Ingress Mode(Either uncompressd or compressed or udf)
 *
 * Parameters:
 *      unit  - (IN) unit number
 *      mode  - (IN) Flex mode
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t  _bcm_esw_stat_flex_delete_ingress_mode(
             int                  unit,
             bcm_stat_flex_mode_t mode)
{
#if defined (BCM_TOMAHAWK_SUPPORT)
    bcm_stat_flex_custom_ingress_mode_t cmode;
    if ((mode >= BCM_CUSTOM_INGRESS_MODE_START)  &&
        (mode < BCM_STAT_FLEX_CUSTOM_INGRESS_COUNTER_MAX_MODE)) {
        _bcm_esw_stat_flex_get_custom_ingress_mode_info(unit, mode, &cmode);
        mode =  cmode.offset_mode;
    }
#endif
   if (mode > (BCM_STAT_FLEX_COUNTER_MAX_MODE-1)) {
       /* Exceeding max allowed value : (BCM_STAT_FLEX_COUNTER_MAX_MODE-1) */
       return BCM_E_PARAM;
   }
   return _bcm_esw_stat_flex_unreserve_mode(
          unit,
          bcmStatFlexDirectionIngress,
          mode);
}

/*
 * Function:                                          
 *      _bcm_esw_stat_flex_delete_ingress_flags       
 * Description:                                       
 *      Deletes New Flex Ingress Mode Flags(Either uncompressd or compressed or
udf)
 *                                                    
 * Parameters:                                        
 *      unit   - (IN) unit number                     
 *      mode   - (IN) Flex mode                       
 *      flags  - (IN) Flex mode flags                 
 * Return Value:                                      
 *      BCM_E_XXX                                     
 * Notes:                                             
 */                                                   
bcm_error_t  _bcm_esw_stat_flex_delete_ingress_flags( 
             int                  unit,               
             bcm_stat_flex_mode_t mode,               
             uint32               flags)              
{                                                     
   if (mode > (BCM_STAT_FLEX_COUNTER_MAX_MODE-1)) {   
       /* Exceeding max allowed value :               
        (BCM_STAT_FLEX_COUNTER_MAX_MODE-1) */         
       return BCM_E_PARAM;                            
   }                                                  
   return _bcm_esw_stat_flex_unreserve_flags(         
          unit,                                       
          mode,                                       
          flags);                                     
}

