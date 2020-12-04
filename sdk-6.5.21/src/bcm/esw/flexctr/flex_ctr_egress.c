/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flex_ctr_egress.c
 * Purpose:     Manage flex counter ingress group creation and deletion
 */

#include <shared/bsl.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/firebolt6.h>

static soc_reg_t _egress_pkt_selector_key_reg[] = {
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_0r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_1r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_2r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_3r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_4r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_5r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_6r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_7r,
#if defined (BCM_APACHE_SUPPORT)
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_8r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_9r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_10r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_11r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_12r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_13r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_14r,
                 EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_15r
#endif
                 };
#define NUM_EGR_KEYS_APACHE 16
#define NUM_EGR_KEYS_DEFAULT 8

/* Getting the number egress keys for the device */
#define GET_NUM_EGR_KEYS(num, unit) \
        if (SOC_IS_APACHE(unit)) { \
            num = NUM_EGR_KEYS_APACHE ; \
        } else { \
            num = NUM_EGR_KEYS_DEFAULT ; \
        }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
bcmi_flex_ctr_subport_info_t egr_port_map_info[BCM_MAX_NUM_UNITS][4];
#endif

/*
 * Function:
 *      _bcm_esw_stat_flex_update_egress_selector_keys
 * Description:
 *      Update flex egress selector keys
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
_bcm_esw_stat_flex_update_egress_selector_keys(
    int                               unit,
    bcm_stat_flex_packet_attr_type_t  pkt_attr_type,
    soc_reg_t                         pkt_selector_key_reg,
    bcm_stat_flex_egr_pkt_attr_bits_t pkt_attr_bits)
{
    uint64 pkt_selector_key_reg_value, val64;
    uint8  current_bit_position=0;
    uint8         index=0;
    int           num_pkt_sel_key_reg;


    COMPILER_64_ZERO(pkt_selector_key_reg_value);
    COMPILER_64_ZERO(val64);
    
    /*APACHE supports 16 pkt selector keys */
    GET_NUM_EGR_KEYS(num_pkt_sel_key_reg, unit);
    for (index=0;index<num_pkt_sel_key_reg ;index++) {
         if (pkt_selector_key_reg == _egress_pkt_selector_key_reg[index]) {
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
       EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_?r:SELECTOR_KEY field */

    /* First Get complete value of EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_?r value*/
    SOC_IF_ERROR_RETURN(soc_reg_get(unit,
                                    pkt_selector_key_reg,
                                    REG_PORT_ANY,
                                    0,
                                    &pkt_selector_key_reg_value));

    /* Next set field value for
       EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_?r:SELECTOR_KEY field */
    soc_reg64_field_set(unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        USE_UDF_KEYf,
                        val64);
    soc_reg64_field_set(unit,
                        pkt_selector_key_reg,
                        &pkt_selector_key_reg_value,
                        USER_SPECIFIED_UDF_VALIDf,
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
                           /*IP_PKT bit position */
                           pkt_attr_bits.ip_pkt_pos, /* 0..0 */
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
                             pkt_attr_bits.drop_pos, /* 1..1 */
                             pkt_attr_bits.drop,
                             /* 1:DROP total bits */
                             pkt_attr_bits.drop_mask,
                             &current_bit_position));
    }
    if (pkt_attr_bits.dvp_type != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* DVP_TYPE bit position */
                           pkt_attr_bits.dvp_type_pos, /* 2..2 */
                           pkt_attr_bits.dvp_type,
                           /* 1:DVP_TYPE total bits  */
                           pkt_attr_bits.dvp_type_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.svp_type != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* SVP_TYPE bit position */
                           pkt_attr_bits.svp_type_pos, /* 3..3 */
                           /* 1:SVP_TYPE total bits */
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
                           /* PKT_RESOLUTION bit position */
                           pkt_attr_bits.pkt_resolution_pos, /* 4..4 */
                           /* 1:PKT_RESOLUTION total bits */
                           pkt_attr_bits.pkt_resolution,
                           pkt_attr_bits.pkt_resolution_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.tos_dscp != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* TOS bit position */
                           pkt_attr_bits.tos_dscp_pos, /* 5 */
                           /* 8:TOS total bits */
                           pkt_attr_bits.tos_dscp,
                           pkt_attr_bits.tos_dscp_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.tos_ecn != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* TOS bit position */
                           pkt_attr_bits.tos_ecn_pos, /* 5 */
                           /* 8:TOS total bits */
                           pkt_attr_bits.tos_ecn,
                           pkt_attr_bits.tos_ecn_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.egr_port != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* EGR_PORT bit position */
                           pkt_attr_bits.egr_port_pos, /* 13 */
                           /* 6:EGR_PORT total bits */
                           pkt_attr_bits.egr_port,
                           pkt_attr_bits.egr_port_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.inner_dot1p != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* INNER_DOT1P bit position */
                           pkt_attr_bits.inner_dot1p_pos, /* 19 */
                           /* 3:INNER_DOT1P total bits */
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
                           /* OUTER_DOT1P bit position */
                           pkt_attr_bits.outer_dot1p_pos, /* 22 */
                           /* 3:OUTER_DOT1P total bits */
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
                           /* VLAN_FORMAT bit position */
                           pkt_attr_bits.vlan_format_pos, /* 25 */
                           /* 2:VLAN_FORMAT total bits  */
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
                           /* INT_PRI bit position */
                           pkt_attr_bits.int_pri_pos, /* 27 */
                           /* 4:INT_PRI total bits */
                           pkt_attr_bits.int_pri,
                           pkt_attr_bits.int_pri_mask,
                           &current_bit_position));
    }
    if (pkt_attr_bits.cng != 0) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* CNG bit position */
                           pkt_attr_bits.cng_pos, /* 31 */
                           /* 2:CNG total bits */
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
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
       if (soc_feature(unit, soc_feature_flex_stat_egr_queue_congestion_marked)) {
           if (pkt_attr_bits.congestion_marked != 0) {
               BCM_IF_ERROR_RETURN(
                       _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* bit position */
                           pkt_attr_bits.congestion_marked_pos,
                           /* total bits */
                           pkt_attr_bits.congestion_marked,
                           pkt_attr_bits.congestion_marked_mask,
                           &current_bit_position));
           }
           if (pkt_attr_bits.uc_queueing!= 0) {
               BCM_IF_ERROR_RETURN(
                       _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* bit position */
                           pkt_attr_bits.uc_queueing_pos,
                           /* total bits */
                           pkt_attr_bits.uc_queueing,
                           pkt_attr_bits.uc_queueing_mask,
                           &current_bit_position));
           }
           if (pkt_attr_bits.mmu_cos != 0) {
               BCM_IF_ERROR_RETURN(
                       _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                           unit,
                           pkt_selector_key_reg,
                           &pkt_selector_key_reg_value,
                           /* bit position */
                           pkt_attr_bits.mmu_cos_pos,
                           /* total bits */
                           pkt_attr_bits.mmu_cos,
                           pkt_attr_bits.mmu_cos_mask,
                           &current_bit_position));
           }
       }
#endif
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
       /* Finally set value for EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_?r */
    SOC_IF_ERROR_RETURN(soc_reg_set(        unit,
                        pkt_selector_key_reg,
                        REG_PORT_ANY,
                        0,
                        pkt_selector_key_reg_value));
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_stat_flex_create_egress_uncompress_mode
 * Description:
 *      Creates New Flex Egress Uncompressed Mode
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
_bcm_esw_stat_flex_create_egress_uncompress_mode(
    int                                         unit,
    bcm_stat_flex_egr_uncmprsd_attr_selectors_t *uncmprsd_attr_selectors,
    bcm_stat_flex_mode_t                        *mode,
    uint32                                      *total_counters)
{
    bcm_stat_flex_mode_t                        mode_l=0;
    bcm_stat_flex_egr_pkt_attr_bits_t           pkt_attr_bits={0};
    uint32                                      total_egress_bits=0;
    uint32                                      index=0;
    bcm_stat_flex_egress_mode_t                 *flex_egress_mode=NULL;

    bcm_stat_flex_egr_uncmprsd_attr_selectors_t uncmprsd_attr_selectors_l;
    uint32                                      num_flex_egress_pools;
    int                                         num_keys;

    num_flex_egress_pools = SOC_INFO(unit).num_flex_egress_pools;

    flex_egress_mode = sal_alloc(sizeof(bcm_stat_flex_egress_mode_t),
                                        "flex_egress_mode");
    if (flex_egress_mode == NULL) {
        return BCM_E_MEMORY;
    }
    
    sal_memset(flex_egress_mode, 0, sizeof(bcm_stat_flex_egress_mode_t));

    for (index =0; index < BCM_STAT_FLEX_COUNTER_MAX_MODE ; index++) {
         if (_bcm_esw_stat_flex_get_egress_mode_info(
             unit,
             index,
             flex_egress_mode) == BCM_E_NONE) {
             if (flex_egress_mode->egr_attr.packet_attr_type !=
                 bcmStatFlexPacketAttrTypeUncompressed) {
                 continue;
             }
             uncmprsd_attr_selectors_l = flex_egress_mode->egr_attr.
                                         uncmprsd_attr_selectors;
             if (sal_memcmp(&uncmprsd_attr_selectors_l,uncmprsd_attr_selectors,
                            sizeof(uncmprsd_attr_selectors_l)) == 0) {
                 *total_counters = flex_egress_mode->total_counters;
                 *mode=index;
                 sal_free(flex_egress_mode);
                 return BCM_E_EXISTS;
             }
         }
    }
    sal_free(flex_egress_mode);

    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_available_mode(
                        unit,
                        bcmStatFlexDirectionEgress,
                        &mode_l));
    /* Step2: Packet Attribute selection */
#ifdef BCM_APACHE_SUPPORT
    if (soc_feature(unit,soc_feature_flex_ctr_mpls_3_label_count)) {
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_CNG_3_ATTR_BITS) {
            pkt_attr_bits.cng_3 = egr_pkt_attr_uncmprsd_bits_g[unit].cng_3;
            pkt_attr_bits.cng_3_mask = egr_pkt_attr_uncmprsd_bits_g[unit].cng_3_mask;
            pkt_attr_bits.cng_3_pos = egr_pkt_attr_uncmprsd_bits_g[unit].cng_3_pos;
            total_egress_bits += pkt_attr_bits.cng_3;
        }
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_PHB_3_ATTR_BITS) {
            pkt_attr_bits.phb_3 = egr_pkt_attr_uncmprsd_bits_g[unit].cng_3;
            pkt_attr_bits.phb_3_mask = egr_pkt_attr_uncmprsd_bits_g[unit].phb_3_mask;
            pkt_attr_bits.phb_3_pos = egr_pkt_attr_uncmprsd_bits_g[unit].phb_3_pos;
            total_egress_bits += pkt_attr_bits.phb_3;
        }
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_CNG_2_ATTR_BITS) {
            pkt_attr_bits.cng_2 = egr_pkt_attr_uncmprsd_bits_g[unit].cng_2;
            pkt_attr_bits.cng_2_mask = egr_pkt_attr_uncmprsd_bits_g[unit].cng_2_mask;
            pkt_attr_bits.cng_2_pos = egr_pkt_attr_uncmprsd_bits_g[unit].cng_2_pos;
            total_egress_bits += pkt_attr_bits.cng_2;
        }
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_PHB_2_ATTR_BITS) {
            pkt_attr_bits.phb_2 = egr_pkt_attr_uncmprsd_bits_g[unit].cng_2;
            pkt_attr_bits.phb_2_mask = egr_pkt_attr_uncmprsd_bits_g[unit].phb_2_mask;
            pkt_attr_bits.phb_2_pos = egr_pkt_attr_uncmprsd_bits_g[unit].phb_2_pos;
            total_egress_bits += pkt_attr_bits.phb_2;
        }
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_CNG_1_ATTR_BITS) {
            pkt_attr_bits.cng_1 = egr_pkt_attr_uncmprsd_bits_g[unit].cng_1;
            pkt_attr_bits.cng_1_mask = egr_pkt_attr_uncmprsd_bits_g[unit].cng_1_mask;
            pkt_attr_bits.cng_1_pos = egr_pkt_attr_uncmprsd_bits_g[unit].cng_1_pos;
            total_egress_bits += pkt_attr_bits.cng_1;
        }
        if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_PHB_1_ATTR_BITS) {
            pkt_attr_bits.phb_1 = egr_pkt_attr_uncmprsd_bits_g[unit].cng_1;
            pkt_attr_bits.phb_1_mask = egr_pkt_attr_uncmprsd_bits_g[unit].phb_1_mask;
            pkt_attr_bits.phb_1_pos = egr_pkt_attr_uncmprsd_bits_g[unit].phb_1_pos;
            total_egress_bits += pkt_attr_bits.phb_1;
        }
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
       if (soc_feature(unit, soc_feature_flex_stat_egr_queue_congestion_marked)) {
           if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                   BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_MMU_COS_ATTR_BITS) {
               pkt_attr_bits.mmu_cos = egr_pkt_attr_uncmprsd_bits_g[unit].mmu_cos;
               pkt_attr_bits.mmu_cos_mask = egr_pkt_attr_uncmprsd_bits_g[unit].mmu_cos_mask;
               pkt_attr_bits.mmu_cos_pos = egr_pkt_attr_uncmprsd_bits_g[unit].mmu_cos_pos;
               total_egress_bits += pkt_attr_bits.mmu_cos;
           }
           if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                   BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_UC_QUEUEING_ATTR_BITS) {
               pkt_attr_bits.uc_queueing = egr_pkt_attr_uncmprsd_bits_g[unit].uc_queueing;
               pkt_attr_bits.uc_queueing_mask = egr_pkt_attr_uncmprsd_bits_g[unit].uc_queueing_mask;
               pkt_attr_bits.uc_queueing_pos = egr_pkt_attr_uncmprsd_bits_g[unit].uc_queueing_pos;
               total_egress_bits += pkt_attr_bits.uc_queueing;
           }
           if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                   BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_CONGESTION_MARKED_ATTR_BITS) {
               pkt_attr_bits.congestion_marked = egr_pkt_attr_uncmprsd_bits_g[unit].congestion_marked;
               pkt_attr_bits.congestion_marked_mask = egr_pkt_attr_uncmprsd_bits_g[unit].congestion_marked_mask;
               pkt_attr_bits.congestion_marked_pos = egr_pkt_attr_uncmprsd_bits_g[unit].congestion_marked_pos;
               total_egress_bits += pkt_attr_bits.congestion_marked;
           }
       }
#endif
    if (soc_feature(unit, soc_feature_flex_stat_int_cn_support)) {
       if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
                BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_INT_CN_ATTR_BITS) {
            pkt_attr_bits.int_cn = egr_pkt_attr_uncmprsd_bits_g[unit].int_cn;
            pkt_attr_bits.int_cn_mask = egr_pkt_attr_uncmprsd_bits_g[unit].int_cn_mask;
            pkt_attr_bits.int_cn_pos = egr_pkt_attr_uncmprsd_bits_g[unit].int_cn_pos;
            total_egress_bits += pkt_attr_bits.int_cn;
        }
    }

   if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_CNG_ATTR_BITS) {
        pkt_attr_bits.cng = egr_pkt_attr_uncmprsd_bits_g[unit].cng;
        pkt_attr_bits.cng_mask = egr_pkt_attr_uncmprsd_bits_g[unit].cng_mask;
        pkt_attr_bits.cng_pos = egr_pkt_attr_uncmprsd_bits_g[unit].cng_pos;
        total_egress_bits +=pkt_attr_bits.cng;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_INT_PRI_ATTR_BITS) {
        pkt_attr_bits.int_pri = egr_pkt_attr_uncmprsd_bits_g[unit].int_pri ;
        pkt_attr_bits.int_pri_mask = egr_pkt_attr_uncmprsd_bits_g[unit].int_pri_mask;
        pkt_attr_bits.int_pri_pos = egr_pkt_attr_uncmprsd_bits_g[unit].int_pri_pos;
        total_egress_bits +=pkt_attr_bits.int_pri;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_VLAN_FORMAT_ATTR_BITS) {
        pkt_attr_bits.vlan_format = egr_pkt_attr_uncmprsd_bits_g[unit].vlan_format ;
        pkt_attr_bits.vlan_format_mask= egr_pkt_attr_uncmprsd_bits_g[unit].vlan_format_mask;
        pkt_attr_bits.vlan_format_pos= egr_pkt_attr_uncmprsd_bits_g[unit].vlan_format_pos;
        total_egress_bits +=pkt_attr_bits.vlan_format;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_OUTER_DOT1P_ATTR_BITS) {
        pkt_attr_bits.outer_dot1p = egr_pkt_attr_uncmprsd_bits_g[unit].outer_dot1p;
        pkt_attr_bits.outer_dot1p_mask= egr_pkt_attr_uncmprsd_bits_g[unit].outer_dot1p_mask;
        pkt_attr_bits.outer_dot1p_pos= egr_pkt_attr_uncmprsd_bits_g[unit].outer_dot1p_pos;
        total_egress_bits +=pkt_attr_bits.outer_dot1p;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_INNER_DOT1P_ATTR_BITS) {
        pkt_attr_bits.inner_dot1p = egr_pkt_attr_uncmprsd_bits_g[unit].inner_dot1p ;
        pkt_attr_bits.inner_dot1p_mask = egr_pkt_attr_uncmprsd_bits_g[unit].inner_dot1p_mask;
        pkt_attr_bits.inner_dot1p_pos = egr_pkt_attr_uncmprsd_bits_g[unit].inner_dot1p_pos;
        total_egress_bits +=pkt_attr_bits.inner_dot1p;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_EGRESS_PORT_ATTR_BITS) {
        pkt_attr_bits.egr_port = egr_pkt_attr_uncmprsd_bits_g[unit].egr_port ;
        pkt_attr_bits.egr_port_mask = egr_pkt_attr_uncmprsd_bits_g[unit].egr_port_mask;
        pkt_attr_bits.egr_port_pos = egr_pkt_attr_uncmprsd_bits_g[unit].egr_port_pos;
        total_egress_bits +=pkt_attr_bits.egr_port;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_TOS_DSCP_ATTR_BITS) {
        pkt_attr_bits.tos_dscp = egr_pkt_attr_uncmprsd_bits_g[unit].tos_dscp;
        pkt_attr_bits.tos_dscp_mask = egr_pkt_attr_uncmprsd_bits_g[unit].tos_dscp_mask;
        pkt_attr_bits.tos_dscp_pos = egr_pkt_attr_uncmprsd_bits_g[unit].tos_dscp_pos;
        total_egress_bits +=pkt_attr_bits.tos_dscp;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_TOS_ECN_ATTR_BITS) {
        pkt_attr_bits.tos_ecn = egr_pkt_attr_uncmprsd_bits_g[unit].tos_ecn;
        pkt_attr_bits.tos_ecn_mask = egr_pkt_attr_uncmprsd_bits_g[unit].tos_ecn_mask;
        pkt_attr_bits.tos_ecn_pos = egr_pkt_attr_uncmprsd_bits_g[unit].tos_ecn_pos;
        total_egress_bits +=pkt_attr_bits.tos_ecn;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_PKT_RESOLUTION_ATTR_BITS) {
        pkt_attr_bits.pkt_resolution = egr_pkt_attr_uncmprsd_bits_g[unit].pkt_resolution ;
        pkt_attr_bits.pkt_resolution_mask= egr_pkt_attr_uncmprsd_bits_g[unit].
                                           pkt_resolution_mask;
        pkt_attr_bits.pkt_resolution_pos= egr_pkt_attr_uncmprsd_bits_g[unit].
                                           pkt_resolution_pos;
        total_egress_bits +=pkt_attr_bits.pkt_resolution;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_SVP_TYPE_ATTR_BITS) {
        pkt_attr_bits.svp_type = egr_pkt_attr_uncmprsd_bits_g[unit].svp_type ;
        pkt_attr_bits.svp_type_mask = egr_pkt_attr_uncmprsd_bits_g[unit].svp_type_mask ;
        pkt_attr_bits.svp_type_pos = egr_pkt_attr_uncmprsd_bits_g[unit].svp_type_pos ;
        total_egress_bits +=pkt_attr_bits.svp_type;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_DVP_TYPE_ATTR_BITS) {
        pkt_attr_bits.dvp_type = egr_pkt_attr_uncmprsd_bits_g[unit].dvp_type ;
        pkt_attr_bits.dvp_type_mask = egr_pkt_attr_uncmprsd_bits_g[unit].dvp_type_mask;
        pkt_attr_bits.dvp_type_pos = egr_pkt_attr_uncmprsd_bits_g[unit].dvp_type_pos;
        total_egress_bits += pkt_attr_bits.dvp_type;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_DROP_ATTR_BITS) {
        pkt_attr_bits.drop = egr_pkt_attr_uncmprsd_bits_g[unit].drop;
        pkt_attr_bits.drop_mask = egr_pkt_attr_uncmprsd_bits_g[unit].drop_mask;
        pkt_attr_bits.drop_pos = egr_pkt_attr_uncmprsd_bits_g[unit].drop_pos;
        total_egress_bits +=pkt_attr_bits.drop;
    }
    if (uncmprsd_attr_selectors->uncmprsd_attr_bits_selector &
        BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_IP_PKT_ATTR_BITS) {
        pkt_attr_bits.ip_pkt = egr_pkt_attr_uncmprsd_bits_g[unit].ip_pkt ;
        pkt_attr_bits.ip_pkt_mask = egr_pkt_attr_uncmprsd_bits_g[unit].ip_pkt_mask ;
        pkt_attr_bits.ip_pkt_pos = egr_pkt_attr_uncmprsd_bits_g[unit].ip_pkt_pos ;
        total_egress_bits +=pkt_attr_bits.ip_pkt;
    }
    if (total_egress_bits > 8) {
        return BCM_E_PARAM;
    }

    /* Step3: Packet Attribute filling */
    /* APACHE supports 4 pkt keys per pool */
    num_keys = SOC_IS_APACHE(unit) ? 4 : 1 ;
    for (index = 0; index < num_keys ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_egress_selector_keys(unit,
                             bcmStatFlexPacketAttrTypeUncompressed,
                             _egress_pkt_selector_key_reg[mode_l + (index*4)],
                             pkt_attr_bits));
    }

    if ((num_flex_egress_pools / 4) > 1) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_egress_selector_keys(unit,
                            bcmStatFlexPacketAttrTypeUncompressed,
                            _egress_pkt_selector_key_reg[mode_l+4],
                            pkt_attr_bits));
    }
    /* Step4: Offset table filling */
    for (index=0;index < num_flex_egress_pools ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_offset_table(unit,
                             bcmStatFlexDirectionEgress,
                             EGR_FLEX_CTR_OFFSET_TABLE_0m+index,
                             mode_l,
                             256,
                             uncmprsd_attr_selectors->offset_table_map));
    }
    /* Step5: Final: reserver mode and return */
    *total_counters=uncmprsd_attr_selectors->total_counters;
    *mode=mode_l;
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_stat_flex_create_egress_compress_mode
 * Description:
 *      Creates New Flex Egress Compressed Mode
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
_bcm_esw_stat_flex_create_egress_compress_mode(
    int                                       unit,
    bcm_stat_flex_egr_cmprsd_attr_selectors_t *cmprsd_attr_selectors,
    bcm_stat_flex_mode_t                      *mode,
    uint32                                    *total_counters,
    bcm_stat_flex_attribute_t                 *flex_attribute)
{
    bcm_stat_flex_mode_t              mode_l=0;
    bcm_stat_flex_egr_pkt_attr_bits_t pkt_attr_bits={0};
    uint32                            total_egress_bits=0;
    uint32                            index=0;
    bcm_stat_flex_egress_mode_t       *flex_egress_mode=NULL;
    bcm_stat_flex_egr_pkt_attr_bits_t pkt_attr_bits_exist;

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
    uint32                            num_flex_egress_pools;


    /* MAP ENTRY Structure */
    uint32                           alloc_size=0;
    egr_flex_ctr_pkt_pri_map_entry_t *pkt_pri_map_dma=NULL;
    uint32                           pkt_pri_map_value=0;
    egr_flex_ctr_pkt_res_map_entry_t *pkt_res_map_dma=NULL;
    uint32                           pkt_res_map_value=0;
    egr_flex_ctr_port_map_entry_t    *port_map_dma=NULL;
    uint32                           port_map_value=0;
    egr_flex_ctr_pri_cng_map_entry_t *pri_cng_map_dma=NULL;
    uint32                           pri_cng_map_value=0;
    egr_flex_ctr_tos_map_entry_t     *tos_map_dma=NULL;
    uint32                           tos_map_value=0;

    int                              num_keys;
    int                              rv = BCM_E_NONE;
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    uint32 cos_uc_map_used = 0;
    egr_flex_ctr_cos_map_entry_t *cos_uc_map_dma = NULL;
    uint32 cos_uc_map_value = 0;
    uint32 cos_uc_cng_same = 0;
#endif

    num_flex_egress_pools = SOC_INFO(unit).num_flex_egress_pools;
    flex_egress_mode = sal_alloc(sizeof(bcm_stat_flex_egress_mode_t),
                                "flex_egress_mode");
    if (flex_egress_mode == NULL) {
        return BCM_E_MEMORY;
    }
    
    sal_memset(flex_egress_mode, 0, sizeof(bcm_stat_flex_egress_mode_t));

    if (soc_feature(unit, soc_feature_flex_stat_compression_share)) {
        rv = _bcm_esw_stat_flex_compress_mode_id_compare(unit,
                bcmStatFlexDirectionEgress,
                mode,
                flex_attribute);
        if (BCM_FAILURE(rv)) {
            sal_free(flex_egress_mode);
            return rv;
        }
    } else {
        for (index =0 ; index < BCM_STAT_FLEX_COUNTER_MAX_MODE ; index++) {
            sal_memset(flex_egress_mode,0,sizeof(bcm_stat_flex_egress_mode_t));
            if (_bcm_esw_stat_flex_get_egress_mode_info(
                        unit,
                        index,
                        flex_egress_mode) == BCM_E_NONE) {
                if (flex_egress_mode->egr_attr.packet_attr_type !=
                        bcmStatFlexPacketAttrTypeCompressed) {
                    continue;
                }
                pkt_attr_bits_exist = flex_egress_mode->egr_attr.
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
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_flex_stat_egr_queue_congestion_marked)) {
                    if ((pkt_attr_bits_exist.mmu_cos ==
                                cmprsd_attr_selectors->pkt_attr_bits.mmu_cos) &&
                        (pkt_attr_bits_exist.uc_queueing ==
                                cmprsd_attr_selectors->pkt_attr_bits.uc_queueing) &&
                        (pkt_attr_bits_exist.congestion_marked ==
                            cmprsd_attr_selectors->pkt_attr_bits.congestion_marked)) {
                        cos_uc_cng_same = 1;
                    }
                } else {
                    cos_uc_cng_same = 1;
                }
#endif

                if (
#ifdef BCM_APACHE_SUPPORT
                        mpls_label_3_exist &&
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
                        cos_uc_cng_same &&
#endif

                        (pkt_attr_bits_exist.cng ==
                         cmprsd_attr_selectors->pkt_attr_bits.cng) &&
                        (pkt_attr_bits_exist.int_pri ==
                         cmprsd_attr_selectors->pkt_attr_bits.int_pri) &&
                        (pkt_attr_bits_exist.vlan_format ==
                         cmprsd_attr_selectors->pkt_attr_bits.vlan_format) &&
                        (pkt_attr_bits_exist.outer_dot1p ==
                         cmprsd_attr_selectors->pkt_attr_bits.outer_dot1p) &&
                        (pkt_attr_bits_exist.inner_dot1p ==
                         cmprsd_attr_selectors->pkt_attr_bits.inner_dot1p) &&
                        ((pkt_attr_bits_exist.egr_port ==
                         cmprsd_attr_selectors->pkt_attr_bits.egr_port)
#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
                         && ((soc_feature(unit,
                             soc_feature_channelized_switching)) &&
                             (bcmi_flex_ctr_cmpsd_port_sanitize(unit,
                                index, 1, NULL, cmprsd_attr_selectors, 1)) == 1)
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
                        (pkt_attr_bits_exist.dvp_type ==
                         cmprsd_attr_selectors->pkt_attr_bits.dvp_type) &&
                        (pkt_attr_bits_exist.drop ==
                         cmprsd_attr_selectors->pkt_attr_bits.drop) &&
                        (pkt_attr_bits_exist.ip_pkt ==
                         cmprsd_attr_selectors->pkt_attr_bits.ip_pkt)) {
                             *total_counters = flex_egress_mode->total_counters;
                             *mode=index;
                             sal_free(flex_egress_mode);
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
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_flex_stat_egr_queue_congestion_marked)) {
                    if ((pkt_attr_bits_exist.mmu_cos!= 0) ||
                        (pkt_attr_bits_exist.uc_queueing)){
                        cos_uc_map_used = 1;
                    }
                }
#endif

                if ((pkt_attr_bits_exist.cng != 0) ||
                        (pkt_attr_bits_exist.int_pri)) {
                    pri_cnf_map_used=1;
                }
                if ((pkt_attr_bits_exist.vlan_format != 0) ||
                        (pkt_attr_bits_exist.outer_dot1p) ||
                        (pkt_attr_bits_exist.inner_dot1p)){
                    pkt_pri_map_used=1;
                }
                if (pkt_attr_bits_exist.egr_port != 0){
                    port_map_used=1;
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                    if (soc_feature(unit, soc_feature_channelized_switching)) {
                         /* subport related check to see of exclusive ports are there.*/
                        if ((bcmi_flex_ctr_cmpsd_port_sanitize(unit,
                                index, 1, NULL, cmprsd_attr_selectors, 0)) == 0) {
                            port_map_used=0;
                        }
                    }
#endif
                }
                if ((pkt_attr_bits_exist.tos_dscp != 0) ||
                        (pkt_attr_bits_exist.tos_ecn != 0)){
                    tos_map_used=1;
                }
                if ((pkt_attr_bits_exist.pkt_resolution!=0) ||
                        (pkt_attr_bits_exist.svp_type) ||
                        (pkt_attr_bits_exist.dvp_type) ||
                        (pkt_attr_bits_exist.drop)){
                    pkt_res_map_used=1;
                }
            }
        }
    }
    sal_free(flex_egress_mode);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_available_mode(
                        unit,
                        bcmStatFlexDirectionEgress,&mode_l));

    /* Step2: Packet Attribute selection */
#ifdef BCM_APACHE_SUPPORT
    if (soc_feature(unit,soc_feature_flex_ctr_mpls_3_label_count)) {
        if (cmprsd_attr_selectors->pkt_attr_bits.cng_3 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.cng_3 >
                    egr_pkt_attr_cmprsd_bits_g[unit].cng_3) {
                return BCM_E_PARAM;
            }
            if (cng_3_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_egress_bits += cmprsd_attr_selectors->pkt_attr_bits.cng_3;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.phb_3 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.phb_3 >
                    egr_pkt_attr_cmprsd_bits_g[unit].phb_3) {
                return BCM_E_PARAM;
            }
            if (phb_3_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_egress_bits += cmprsd_attr_selectors->pkt_attr_bits.phb_3;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.cng_2 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.cng_2 >
                    egr_pkt_attr_cmprsd_bits_g[unit].cng_2) {
                return BCM_E_PARAM;
            }
            if (cng_2_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_egress_bits += cmprsd_attr_selectors->pkt_attr_bits.cng_2;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.phb_2 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.phb_2 >
                    egr_pkt_attr_cmprsd_bits_g[unit].phb_2) {
                return BCM_E_PARAM;
            }
            if (phb_2_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_egress_bits += cmprsd_attr_selectors->pkt_attr_bits.phb_2;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.cng_1 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.cng_1 >
                    egr_pkt_attr_cmprsd_bits_g[unit].cng_1) {
                return BCM_E_PARAM;
            }
            if (cng_1_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_egress_bits += cmprsd_attr_selectors->pkt_attr_bits.cng_1;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.phb_1 != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.phb_1 >
                    egr_pkt_attr_cmprsd_bits_g[unit].phb_1) {
                return BCM_E_PARAM;
            }
            if (phb_1_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_egress_bits += cmprsd_attr_selectors->pkt_attr_bits.phb_1;
        }
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_stat_egr_queue_congestion_marked)) {
        if (cmprsd_attr_selectors->pkt_attr_bits.mmu_cos != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.mmu_cos >
                egr_pkt_attr_cmprsd_bits_g[unit].mmu_cos) {
                return BCM_E_PARAM;
            }
            if (cos_uc_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_egress_bits += cmprsd_attr_selectors->pkt_attr_bits.mmu_cos;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.uc_queueing != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.uc_queueing >
                egr_pkt_attr_cmprsd_bits_g[unit].uc_queueing) {
                return BCM_E_PARAM;
            }
            if (cos_uc_map_used == 1) {
                return BCM_E_PARAM;
            }
            total_egress_bits += cmprsd_attr_selectors->pkt_attr_bits.uc_queueing;
        }
        if (cmprsd_attr_selectors->pkt_attr_bits.congestion_marked != 0) {
            if (cmprsd_attr_selectors->pkt_attr_bits.congestion_marked>
                egr_pkt_attr_cmprsd_bits_g[unit].congestion_marked) {
                return BCM_E_PARAM;
            }
            total_egress_bits += cmprsd_attr_selectors->pkt_attr_bits.congestion_marked;
        }
    }
#endif

    if (cmprsd_attr_selectors->pkt_attr_bits.cng !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.cng >
            egr_pkt_attr_cmprsd_bits_g[unit].cng) {
            return BCM_E_PARAM;
        }
        if (pri_cnf_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.cng;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.int_pri !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.int_pri > 
            egr_pkt_attr_cmprsd_bits_g[unit].int_pri) {
            return BCM_E_PARAM;
        }
        if (pri_cnf_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.int_pri;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.vlan_format !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.vlan_format >
            egr_pkt_attr_cmprsd_bits_g[unit].vlan_format) {
            return BCM_E_PARAM;
        }
        if (pkt_pri_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.vlan_format;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.outer_dot1p !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.outer_dot1p  > 
            egr_pkt_attr_cmprsd_bits_g[unit].outer_dot1p) {
            return BCM_E_PARAM;
        }
        if (pkt_pri_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.outer_dot1p ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.inner_dot1p !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.inner_dot1p  >
            egr_pkt_attr_cmprsd_bits_g[unit].inner_dot1p) {
            return BCM_E_PARAM;
        }
        if (pkt_pri_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.inner_dot1p ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.egr_port !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.egr_port  > 
            egr_pkt_attr_cmprsd_bits_g[unit].egr_port) {
            return BCM_E_PARAM;
        }
        if (port_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.egr_port ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.tos_dscp !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.tos_dscp  > 
            egr_pkt_attr_cmprsd_bits_g[unit].tos_dscp) {
            return BCM_E_PARAM;
        }
        if (tos_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.tos_dscp ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.tos_ecn !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.tos_ecn  > 
            egr_pkt_attr_cmprsd_bits_g[unit].tos_ecn) {
            return BCM_E_PARAM;
        }
        if (tos_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.tos_ecn ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.pkt_resolution !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.pkt_resolution  > 
            egr_pkt_attr_cmprsd_bits_g[unit].pkt_resolution) {
            return BCM_E_PARAM;
        }
        if (pkt_res_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.
                              pkt_resolution;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.svp_type !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.svp_type  > 
            egr_pkt_attr_cmprsd_bits_g[unit].svp_type) {
            return BCM_E_PARAM;
        }
        if (pkt_res_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.svp_type ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.dvp_type !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.dvp_type  > 
            egr_pkt_attr_cmprsd_bits_g[unit].dvp_type) {
            return BCM_E_PARAM;
        }
        if (pkt_res_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.dvp_type ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.drop        !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.drop > 
            egr_pkt_attr_cmprsd_bits_g[unit].drop) {
            return BCM_E_PARAM;
        }
        if (pkt_res_map_used==1) {
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.drop ;
    }
    if (cmprsd_attr_selectors->pkt_attr_bits.ip_pkt        !=0) {
        if (cmprsd_attr_selectors->pkt_attr_bits.ip_pkt >
            egr_pkt_attr_cmprsd_bits_g[unit].ip_pkt) {  
            return BCM_E_PARAM;
        }
        total_egress_bits +=cmprsd_attr_selectors->pkt_attr_bits.ip_pkt ;
    }
    if (total_egress_bits > 8) {
        return BCM_E_PARAM;
    }
    pkt_attr_bits =cmprsd_attr_selectors->pkt_attr_bits;
    /* Step3: Fill up map array */
    if (soc_feature(unit, soc_feature_flex_stat_compression_share)) {
        /* Write global compression tables */
        rv = _bcm_stat_flex_compressed_attr_tbl_write(unit,
                bcmStatFlexDirectionEgress,NULL,
                cmprsd_attr_selectors);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        if ((pkt_attr_bits.cng != 0) ||
                (pkt_attr_bits.int_pri)) {
            alloc_size = soc_mem_index_count(unit,EGR_FLEX_CTR_PRI_CNG_MAPm) *
                sizeof(egr_flex_ctr_pri_cng_map_entry_t);

            pri_cng_map_dma=soc_cm_salloc( unit, alloc_size,"pri_cng_map");
            if (pri_cng_map_dma == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(pri_cng_map_dma, 0,alloc_size);
            if (soc_mem_read_range(
                        unit,
                        EGR_FLEX_CTR_PRI_CNG_MAPm,
                        MEM_BLOCK_ANY,
                        soc_mem_index_min(unit,EGR_FLEX_CTR_PRI_CNG_MAPm),
                        soc_mem_index_max(unit,EGR_FLEX_CTR_PRI_CNG_MAPm),
                        pri_cng_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,pri_cng_map_dma);
                return BCM_E_INTERNAL;
            }
            for (index=0;
                    index< soc_mem_index_count(unit,EGR_FLEX_CTR_PRI_CNG_MAPm);
                    index++) {
                pri_cng_map_value=cmprsd_attr_selectors->pri_cnf_attr_map[index];
                soc_mem_field_set(
                        unit,
                        EGR_FLEX_CTR_PRI_CNG_MAPm,
                        (uint32 *)&pri_cng_map_dma[index],
                        PRI_CNG_FNf,
                        &pri_cng_map_value);
            }
            if (soc_mem_write_range(unit,
                        EGR_FLEX_CTR_PRI_CNG_MAPm,
                        MEM_BLOCK_ALL,
                        soc_mem_index_min(unit,EGR_FLEX_CTR_PRI_CNG_MAPm),
                        soc_mem_index_max(unit,EGR_FLEX_CTR_PRI_CNG_MAPm),
                        pri_cng_map_dma) != BCM_E_NONE) {
                soc_cm_sfree(unit,pri_cng_map_dma);
                return BCM_E_INTERNAL;
            }
            soc_cm_sfree(unit,pri_cng_map_dma);
        }
        if ((pkt_attr_bits.vlan_format != 0) ||
                (pkt_attr_bits.outer_dot1p) ||
                (pkt_attr_bits.inner_dot1p)){
            alloc_size= soc_mem_index_count(unit,EGR_FLEX_CTR_PKT_PRI_MAPm) *
                sizeof(egr_flex_ctr_pkt_pri_map_entry_t);
            pkt_pri_map_dma = soc_cm_salloc( unit, alloc_size, "pkt_pri_map");
            if (pkt_pri_map_dma == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(pkt_pri_map_dma, 0, alloc_size);
            if (soc_mem_read_range(
                        unit,
                        EGR_FLEX_CTR_PKT_PRI_MAPm,
                        MEM_BLOCK_ANY,
                        soc_mem_index_min(unit,EGR_FLEX_CTR_PKT_PRI_MAPm),
                        soc_mem_index_max(unit,EGR_FLEX_CTR_PKT_PRI_MAPm),
                        pkt_pri_map_dma) != BCM_E_NONE) {
                soc_cm_sfree(unit,pkt_pri_map_dma);
                return BCM_E_INTERNAL;
            }
            for (index=0;
                    index< soc_mem_index_count(unit,EGR_FLEX_CTR_PKT_PRI_MAPm);
                    index++) {
                pkt_pri_map_value= cmprsd_attr_selectors->pkt_pri_attr_map[index];
                soc_mem_field_set(
                        unit,
                        EGR_FLEX_CTR_PKT_PRI_MAPm,
                        (uint32 *)&pkt_pri_map_dma[index],
                        PKT_PRI_FNf,
                        &pkt_pri_map_value);
            }
            if (soc_mem_write_range(unit,
                        EGR_FLEX_CTR_PKT_PRI_MAPm,
                        MEM_BLOCK_ALL,
                        soc_mem_index_min(unit,EGR_FLEX_CTR_PKT_PRI_MAPm),
                        soc_mem_index_max(unit,EGR_FLEX_CTR_PKT_PRI_MAPm),
                        pkt_pri_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,pkt_pri_map_dma);
                return BCM_E_INTERNAL;
            }
            soc_cm_sfree(unit,pkt_pri_map_dma);
        }
        if ((pkt_attr_bits.egr_port != 0) &&
            (!soc_feature(unit, soc_feature_channelized_switching))) {

            int local_max_index = 0;
            int final_port_index = 0;
            int index1 = 0;

            alloc_size= soc_mem_index_count(unit,EGR_FLEX_CTR_PORT_MAPm) *
                sizeof(egr_flex_ctr_port_map_entry_t);
            port_map_dma = soc_cm_salloc( unit, alloc_size, "port_map");
            if (port_map_dma == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(port_map_dma, 0, alloc_size);
            if (soc_mem_read_range(
                        unit,
                        EGR_FLEX_CTR_PORT_MAPm,
                        MEM_BLOCK_ANY,
                        soc_mem_index_min(unit,EGR_FLEX_CTR_PORT_MAPm),
                        soc_mem_index_max(unit,EGR_FLEX_CTR_PORT_MAPm),
                        port_map_dma) != BCM_E_NONE) {
                soc_cm_sfree(unit,port_map_dma);
                return BCM_E_INTERNAL;
            }
            local_max_index = soc_mem_index_count(unit, EGR_FLEX_CTR_PORT_MAPm);
            if (soc_feature(unit, soc_feature_flex_ctr_lsb_port_ctrl)) {
                /* extra space is given. Only first quarter is used. */
                local_max_index /= 4;
            }

            for(index=0;
                    index < local_max_index;
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
                        EGR_FLEX_CTR_PORT_MAPm,
                        (uint32 *)&port_map_dma[final_port_index],
                        PORT_FNf,
                        &port_map_value);
            }
            if (soc_mem_write_range(
                        unit,
                        EGR_FLEX_CTR_PORT_MAPm,
                        MEM_BLOCK_ALL,
                        soc_mem_index_min(unit,EGR_FLEX_CTR_PORT_MAPm),
                        soc_mem_index_max(unit,EGR_FLEX_CTR_PORT_MAPm),
                        port_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,port_map_dma);
                return BCM_E_INTERNAL;
            }
            soc_cm_sfree(unit,port_map_dma);
        }
#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)

        if ((pkt_attr_bits.egr_port != 0) &&
            soc_feature(unit, soc_feature_channelized_switching)) {
            int local_max_index = 0;

            local_max_index = soc_mem_index_count(unit, EGR_FLEX_CTR_PORT_MAPm);
            if (soc_feature(unit, soc_feature_flex_ctr_lsb_port_ctrl)) {
                /* extra space is given. Only first quarter is used. */
                local_max_index /= 4;
            }


            for (index=0;
                    index< local_max_index;
                    index++) {
                egr_port_map_info[unit][mode_l].portmap[index] =
                    cmprsd_attr_selectors->port_attr_map[index];
            }

            BCM_IF_ERROR_RETURN(
                bcmi_flex_ctr_update_portmap(unit, mode_l, 1, 1));
        }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

        if ((pkt_attr_bits.tos_dscp != 0) ||
                (pkt_attr_bits.tos_ecn != 0)) {
            alloc_size = soc_mem_index_count(unit,EGR_FLEX_CTR_TOS_MAPm) *
                sizeof(egr_flex_ctr_tos_map_entry_t);
            tos_map_dma = soc_cm_salloc( unit, alloc_size , "tos_map");
            if (tos_map_dma == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(tos_map_dma, 0, alloc_size);
            if (soc_mem_read_range(
                        unit,
                        EGR_FLEX_CTR_TOS_MAPm,
                        MEM_BLOCK_ANY,
                        soc_mem_index_min(unit,EGR_FLEX_CTR_TOS_MAPm),
                        soc_mem_index_max(unit,EGR_FLEX_CTR_TOS_MAPm),
                        tos_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,tos_map_dma);
                return BCM_E_INTERNAL;
            }
            for(index=0;
                    index< soc_mem_index_count(unit,EGR_FLEX_CTR_TOS_MAPm);
                    index++) {
                tos_map_value = cmprsd_attr_selectors->tos_attr_map[index];
                soc_mem_field_set(
                        unit,
                        EGR_FLEX_CTR_PORT_MAPm,
                        (uint32 *)&tos_map_dma[index],
                        TOS_FNf,
                        &tos_map_value);
            }
            if (soc_mem_write_range(unit,
                        EGR_FLEX_CTR_TOS_MAPm,
                        MEM_BLOCK_ALL,
                        soc_mem_index_min(unit,EGR_FLEX_CTR_TOS_MAPm),
                        soc_mem_index_max(unit,EGR_FLEX_CTR_TOS_MAPm),
                        tos_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,tos_map_dma);
                return BCM_E_INTERNAL;
            }
            soc_cm_sfree(unit,tos_map_dma);
        }
        if ((pkt_attr_bits.pkt_resolution != 0) ||
                (pkt_attr_bits.svp_type) ||
                (pkt_attr_bits.dvp_type) ||
                (pkt_attr_bits.drop)){
            alloc_size = soc_mem_index_count(unit,EGR_FLEX_CTR_PKT_RES_MAPm) *
                sizeof(egr_flex_ctr_pkt_res_map_entry_t);
            pkt_res_map_dma= soc_cm_salloc( unit, alloc_size, "pkt_res_map");
            if (pkt_res_map_dma == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(pkt_res_map_dma, 0,alloc_size);
            if (soc_mem_read_range(
                        unit,
                        EGR_FLEX_CTR_PKT_RES_MAPm,
                        MEM_BLOCK_ANY,
                        soc_mem_index_min(unit,EGR_FLEX_CTR_PKT_RES_MAPm),
                        soc_mem_index_max(unit,EGR_FLEX_CTR_PKT_RES_MAPm),
                        pkt_res_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,pkt_res_map_dma);
                return BCM_E_INTERNAL;
            }
            for (index=0;
                    index< soc_mem_index_count(unit,EGR_FLEX_CTR_PKT_RES_MAPm);
                    index++) {
                pkt_res_map_value= cmprsd_attr_selectors->pkt_res_attr_map[index];
                soc_mem_field_set(
                        unit,
                        EGR_FLEX_CTR_PKT_RES_MAPm,
                        (uint32 *)&pkt_res_map_dma[index],
                        PKT_RES_FNf,
                        &pkt_res_map_value);
            }
            if (soc_mem_write_range(unit,
                        EGR_FLEX_CTR_PKT_RES_MAPm,
                        MEM_BLOCK_ALL,
                        soc_mem_index_min(unit,EGR_FLEX_CTR_PKT_RES_MAPm),
                        soc_mem_index_max(unit,EGR_FLEX_CTR_PKT_RES_MAPm),
                        pkt_res_map_dma) != BCM_E_NONE){
                soc_cm_sfree(unit,pkt_res_map_dma);
                return BCM_E_INTERNAL;
            }
            soc_cm_sfree(unit,pkt_res_map_dma);
        }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_stat_egr_queue_congestion_marked)) {
            if ((pkt_attr_bits.mmu_cos != 0) ||
                    (pkt_attr_bits.uc_queueing != 0)){
                uint32 mem_cnt = soc_mem_index_count(unit, EGR_FLEX_CTR_COS_MAPm);
                uint32 idx_min = soc_mem_index_min(unit, EGR_FLEX_CTR_COS_MAPm);
                uint32 idx_max = soc_mem_index_max(unit, EGR_FLEX_CTR_COS_MAPm);

                alloc_size = mem_cnt * sizeof(egr_flex_ctr_cos_map_entry_t);
                cos_uc_map_dma= soc_cm_salloc( unit, alloc_size, "cos_uc_map");
                if (cos_uc_map_dma == NULL) {
                    return BCM_E_MEMORY;
                }
                sal_memset(cos_uc_map_dma, 0, alloc_size);
                if (soc_mem_read_range(
                            unit,
                            EGR_FLEX_CTR_COS_MAPm,
                            MEM_BLOCK_ANY,
                            idx_min,
                            idx_max,
                            cos_uc_map_dma) != BCM_E_NONE){
                    soc_cm_sfree(unit, cos_uc_map_dma);
                    return BCM_E_INTERNAL;
                }
                for (index = 0; index < mem_cnt; index++) {
                    cos_uc_map_value = cmprsd_attr_selectors->cos_uc_attr_map[index];
                    soc_mem_field_set(
                            unit,
                            EGR_FLEX_CTR_COS_MAPm,
                            (uint32 *)&cos_uc_map_dma[index],
                            COS_FNf,
                            &cos_uc_map_value);
                }
                if (soc_mem_write_range(unit,
                            EGR_FLEX_CTR_COS_MAPm,
                            MEM_BLOCK_ALL,
                            idx_min,
                            idx_max,
                            cos_uc_map_dma) != BCM_E_NONE){
                    soc_cm_sfree(unit, cos_uc_map_dma);
                    return BCM_E_INTERNAL;
                }
                soc_cm_sfree(unit, cos_uc_map_dma);
            }
        }
#endif
    }
    /* Step4: Packet Attribute filling */
    /* APACHE supports 4 pkt keys per pool */
    num_keys = SOC_IS_APACHE(unit) ? 4 : 1 ;
    for (index = 0; index < num_keys ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_egress_selector_keys(
                             unit,
                             bcmStatFlexPacketAttrTypeCompressed,
                             _egress_pkt_selector_key_reg[mode_l + (index*4)],
                             pkt_attr_bits));
    }

    if ((num_flex_egress_pools / 4) > 1) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_egress_selector_keys(
                            unit,
                            bcmStatFlexPacketAttrTypeCompressed,
                            _egress_pkt_selector_key_reg[mode_l+4],
                            pkt_attr_bits));

    }
    /* Step5: Offset table filling */
    for (index=0;index < num_flex_egress_pools ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_offset_table(
                             unit,
                             bcmStatFlexDirectionEgress,
                             EGR_FLEX_CTR_OFFSET_TABLE_0m+index,
                             mode_l,
                             256,
                             cmprsd_attr_selectors->offset_table_map));
    }
    /* Step6: Final: reserve mode and return */
    *total_counters=cmprsd_attr_selectors->total_counters;
    *mode=mode_l;
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_stat_flex_create_egress_udf_mode
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
_bcm_esw_stat_flex_create_egress_udf_mode(
    int                                    unit,
    bcm_stat_flex_udf_pkt_attr_selectors_t *udf_pkt_attr_selectors,
    bcm_stat_flex_mode_t                   *mode,
    uint32                                 *total_counters)
{
    bcm_stat_flex_mode_t                   mode_l=0;
    uint32                                 total_udf_bits=0;
    uint32                                 index=0;
    bcm_stat_flex_egress_mode_t            *flex_egress_mode=NULL;
    bcm_stat_flex_udf_pkt_attr_selectors_t udf_pkt_attr_selectors_l;
    bcm_stat_flex_udf_pkt_attr_bits_t      udf_pkt_attr_bits_l;
    uint32                                 num_flex_egress_pools;
    int                                    num_keys;

    num_flex_egress_pools = SOC_INFO(unit).num_flex_egress_pools;
    flex_egress_mode = sal_alloc(sizeof(bcm_stat_flex_egress_mode_t),
                                  "flex_egress_mode");
    if (flex_egress_mode == NULL) {
        return BCM_E_MEMORY;
    }
  
    sal_memset(flex_egress_mode, 0, sizeof(bcm_stat_flex_egress_mode_t));

    for (index =0 ; index < BCM_STAT_FLEX_COUNTER_MAX_MODE ; index++) {
         sal_memset(flex_egress_mode,0,sizeof(bcm_stat_flex_egress_mode_t));
         if (_bcm_esw_stat_flex_get_egress_mode_info(
             unit,
             index,
             flex_egress_mode) == BCM_E_NONE) {
             if (flex_egress_mode->egr_attr.packet_attr_type !=
                 bcmStatFlexPacketAttrTypeUdf) {
                 continue;
             }
             udf_pkt_attr_selectors_l = flex_egress_mode->egr_attr.
                                        udf_pkt_attr_selectors;
             udf_pkt_attr_bits_l = udf_pkt_attr_selectors_l.udf_pkt_attr_bits;
             if ((udf_pkt_attr_bits_l.udf0==
                  udf_pkt_attr_selectors->udf_pkt_attr_bits.udf0) &&
                 (udf_pkt_attr_bits_l.udf1==
                  udf_pkt_attr_selectors->udf_pkt_attr_bits.udf1)) {
                  *total_counters = flex_egress_mode->total_counters;
                  *mode=index;
                  sal_free(flex_egress_mode);
                  return BCM_E_EXISTS;
             }
         }
    }
    sal_free(flex_egress_mode);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_available_mode(
                        unit,
                        bcmStatFlexDirectionEgress,&mode_l));
    /* Step2: Packet Attribute filling */
    /* APACHE supports 4 pkt keys per pool */
    num_keys = SOC_IS_APACHE(unit) ? 4 : 1 ;
    for (index = 0; index < num_keys ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_udf_selector_keys(
                             unit,
                             bcmStatFlexDirectionEgress,
                             _egress_pkt_selector_key_reg[mode_l * (index*4)],
                             udf_pkt_attr_selectors,
                             &total_udf_bits));
    }

    if ((num_flex_egress_pools / 4) > 1) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_udf_selector_keys(
                            unit,
                            bcmStatFlexDirectionEgress,
                            _egress_pkt_selector_key_reg[mode_l+4],
                            udf_pkt_attr_selectors,
                            &total_udf_bits));
    }
    /* Step3: Offset table filling */
    for (index=0;index < num_flex_egress_pools ; index++) {
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_update_offset_table(
                             unit,
                             bcmStatFlexDirectionEgress,
                             ING_FLEX_CTR_OFFSET_TABLE_0m+index,
                             mode_l,
                             256,
                             NULL));
    }
    /* Step4: Final: reserve mode and return */
    *total_counters = (1 << total_udf_bits);
    *mode=mode_l;
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_stat_flex_create_egress_mode
 * Description:
 *      Creates New Flex Egress Mode(Either uncompressd or compressed or udf)
 *
 * Parameters:
 *      unit      - (IN) unit number
 *      ing_attr  - (IN) Flex Ingress attributes
 *      mode      - (OUT) Flex mode
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

bcm_error_t _bcm_esw_stat_flex_create_egress_mode (
            int unit,
            bcm_stat_flex_egr_attr_t *egr_attr,
            bcm_stat_flex_mode_t *mode,
            bcm_stat_flex_attribute_t  *flex_attribute)
{
    uint32        total_counters=0;
    switch(egr_attr->packet_attr_type) {
    case bcmStatFlexPacketAttrTypeUncompressed:
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Creating Egress uncompressed mode \n")));
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_create_egress_uncompress_mode(
                             unit,
                             &(egr_attr->uncmprsd_attr_selectors),
                             mode,
                             &total_counters));
         break;
    case bcmStatFlexPacketAttrTypeCompressed:
        if (SOC_IS_TOMAHAWK3(unit)) {
            return BCM_E_UNAVAIL;
        }
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Creating Egress compressed mode \n")));
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_create_egress_compress_mode(
                             unit,
                             &(egr_attr->cmprsd_attr_selectors),
                             mode,
                             &total_counters,flex_attribute));
         break;
    case bcmStatFlexPacketAttrTypeUdf:
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Creating Egress udf mode \n")));
         BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_create_egress_udf_mode(
                             unit,
                             &(egr_attr->udf_pkt_attr_selectors),
                             mode,
                             &total_counters));
         break;
    default:
         return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_egress_reserve_mode(
                        unit,
                        *mode,
                        total_counters,
                        egr_attr));
    /* *mode |= 0x80; */
    LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
              (BSL_META_U(unit,
                          "\n Done %d \n"),
               *mode));
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_stat_flex_delete_egress_mode
 * Description:
 *      Destroys New Flex Egress Mode(Either uncompressd or compressed or udf)
 *
 * Parameters:
 *      unit  - (IN) unit number
 *      mode  - (IN) Flex mode
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

bcm_error_t _bcm_esw_stat_flex_delete_egress_mode(
            int                  unit,
            bcm_stat_flex_mode_t mode)
{
    if (mode > (BCM_STAT_FLEX_COUNTER_MAX_MODE-1)) {
        /* Exceeding max allowed value : (BCM_STAT_FLEX_COUNTER_MAX_MODE-1) */
        return BCM_E_PARAM;
    }
    return _bcm_esw_stat_flex_unreserve_mode(
           unit,
           bcmStatFlexDirectionEgress,
           mode);
}
