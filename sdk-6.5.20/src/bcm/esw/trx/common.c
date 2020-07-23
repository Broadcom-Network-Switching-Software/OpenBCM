/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    common.c
 * Purpose: Manages resources shared across multiple modules
 */
#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/mcm/allenum.h>
#include <soc/profile_mem.h>
#include <sal/compiler.h>
#include <soc/scache.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/switch.h>
#ifdef BCM_TRX_SUPPORT
#include <bcm_int/esw/triumph.h>
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
#include <bcm_int/esw/triumph3.h>
#endif
#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/katana2.h>
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
#include <bcm_int/esw/trident2plus.h>
#endif
#ifdef BCM_SABER2_SUPPORT
#include <bcm_int/esw/saber2.h>
#endif
#ifdef BCM_TRIDENT3_SUPPORT
#include <bcm_int/esw/trident3.h>
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif


#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
#define MY_STATION_SHADOW_MASK_MAX_ARRAY_SIZE 1024
typedef struct my_station_shadow_mask_s {
    uint32   buf[(MY_STATION_SHADOW_MASK_MAX_ARRAY_SIZE+31)/32];
} my_station_shadow_mask_t;
#endif

typedef struct _bcm_common_bookkeeping_s {
    int initialized;            /* Flag to check initialized status */
    int mac_da_profile_created; /* Flag to check MAC_DA profile creation */
    int lport_profile_created;  /* Flag to check LPORT profile creation */
    int ing_pri_cng_map_created; /* Flag to check ING_PRI_CNG_MAP creation */
    int ing_vft_pri_map_created; /* Flag to check ING_VFT_PRI_MAP creation */
    int ing_l2_vlan_etag_map_created; /* Flag to check ING_ETAG_PCP_MAPPING creation */    
    int egr_vft_pri_map_created; /* Flag to check EGR_VFT_PRI_MAP creation */
    int egr_vsan_intpri_map_created; /* Flag to check EGR_VSAN_INTPRI_MAP created */
    int egr_l2_vlan_etag_map_created; /* Flag to check EGR_ETAG_PCP_MAPPING creation */
    int egr_vft_fields_created; /* Flag to check EGR_VFT_FIELDS_PROFILE created */
    int egr_mpls_combo_map_created; /* Flag to check combo map creation */
    int dscp_table_created; /* Flag to check DSCP_TABLE creation */
    int egr_dscp_table_created; /* Flag to check EGR_DSCP_TABLE creation */
    int ing_outer_dot1p_created; /* Flag to check ING_OUTER_DOT1P_MAPPING_TABLE 
                                    creation */
#ifdef BCM_TRIUMPH3_SUPPORT
    int ing_l3_nh_attribute_created;  /* Flag to check ING_L3_NEXT_HOP_ATTRIBUTE_1
                                         creation */
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    int l3_iif_profile_created;  /* Flag to check L3_IIF_PROFILE creation */
    int l3_ip4_options_profile_created;  /* Flag to check L3_IP4_OPTIONS_PROFILE creation */
    int fc_map_profile_created;          /* Flag to check FC_MAP_PROFILE */
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    int vfi_profile_created;     /* Flags to check vfi profile creation */
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    int prot_pkt_ctrl_created;
#ifdef BCM_KATANA_SUPPORT
    int ing_queue_offset_map_created; /* Flag to check ING_QUEUE_OFFSET_MAP_
                                         TABLE creation */
#endif
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    int vlan_protocol_data_profile_created; /* Flag to check if VLAN_PROTOCOL_DATA profile creation */
#endif
#ifdef BCM_HURRICANE3_SUPPORT
    int egr_header_encap_data_created; /* Flag to check if EGR_HEADER_ENCAP_DATA 
                                                 profile creation */
    int custom_header_match_created;  /* Flag to check if CUSTOM_HEADER_MATCH profile creation */
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    int ing_tunnel_term_map_created; /* Flag to check if ING_TUNNEL_TERM_MAP profile creation */
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
	defined(BCM_TRIDENT3_SUPPORT)
    int ing_exp_to_ip_ecn_map_created; /* Flag to check if ING_EXP_TO_IP_ECN_MAP profile creation */
    int egr_ip_ecn_to_exp_map_created; /* Flag to check if EGR_IP_ECN_TO_EXP_MAP profile creation */
    int egr_int_cn_to_exp_map_created; /* Flag to check if EGR_INT_CN_TO_EXP_MAP profile creation */
#endif /* BCM_TOMAHAWK2_SUPPORT or BCM_TOMAHAWK3_SUPPORT */

    soc_profile_mem_t *mac_da_profile; /* cache of EGR_MAC_DA_PROFILE */
    soc_profile_mem_t *lport_profile;  /* cache of LPORT_TABLE (profile) */
    soc_profile_mem_t *ing_pri_cng_map;  /* cache of ING_PRI_CNG_MAP (profile) */
    soc_profile_mem_t *egr_mpls_combo_map;  /* cache of combined tables */
    soc_profile_mem_t *dscp_table;  /* cache of DSCP_TABLE (profile) */
    soc_profile_mem_t *egr_dscp_table;  /* cache of EGR_DSCP_TABLE (profile) */
    soc_profile_mem_t *ing_outer_dot1p;  /* cache of ING_OUTER_DOT1P_MAPPING_TABLE 
                                            (profile) */
#ifdef BCM_TRIUMPH3_SUPPORT
    soc_profile_mem_t *ing_l3_nh_attribute;  /* cache of ING_L3_NEXT_HOP_ATTRIBUTE_1
                                                (profile) */ 
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    soc_profile_mem_t *l3_iif_profile;  /* Cache of L3 IIF profile */
    soc_profile_mem_t *l3_ip4_options_profile;  /* Cache of L3 IP4 options profile */
    soc_profile_mem_t *fc_map_profile;
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    soc_profile_mem_t *vfi_profile;  /* Cache of VFI profile */
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    soc_profile_mem_t *offset_map_table; /* cache of ING_QUEUE_OFFSET_MAP_TABLE
                                            (profile) */
#endif /* BCM_KATANA_SUPPORT */
/*TD2+ support is added here for xgs5/port.c usage*/
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    /* cache of VLAN_PROTOCOL_DATA profile */
    soc_profile_mem_t *vlan_protocol_data_profile;
#endif

    soc_profile_mem_t *ing_vft_pri_map;
    soc_profile_mem_t *ing_l2_vlan_etag_map;
    soc_profile_mem_t *egr_vft_pri_map;
    soc_profile_mem_t *egr_vsan_intpri_map;
    soc_profile_mem_t *egr_l2_vlan_etag_map;
    soc_profile_mem_t *egr_vft_fields;
    soc_profile_reg_t *prot_pkt_ctrl;
    uint16      *mpls_station_hash; /* Hash values for MPLS STATION entries */
    my_station_tcam_entry_t *my_station_shadow;
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    my_station_shadow_mask_t *my_station_shadow_mask;
#endif
    void *my_station_l3_mask;
    void *my_station_tunnel_mask;
    my_station_tcam_2_entry_t *my_station2_shadow;
#if defined(BCM_TRIDENT3_SUPPORT)
        my_station_shadow_mask_t *my_station2_shadow_mask;
#endif
    my_station_tcam_2_entry_t my_station2_l3_mask;
    my_station_tcam_2_entry_t my_station2_tunnel_mask;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    soc_profile_mem_t *ing_vlan_vfi_mbrship_profile;
    soc_profile_mem_t *egr_vlan_vfi_mbrship_profile;
    soc_profile_mem_t *misc_port_profile;  /* Misc port profile for IFP related per port attrs */
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    soc_profile_mem_t *egr_vlan_vfi_untag_profile;
#endif
    soc_profile_mem_t *lport_ind_profile;  /* Independent profile for ING LPORT */
    soc_profile_mem_t *rtag7_ind_profile;  /* Independent profile for RTAG7 */
    soc_profile_mem_t *egr_lport_profile; /* Egr LPORT profiling */
    soc_profile_mem_t *egr_qos_profile; /* Egr QOS profiling */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    int ing_vlan_vfi_mbrship_profile_created;
    int egr_vlan_vfi_mbrship_profile_created;
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    int egr_vlan_vfi_untag_profile_created;
#endif
    int misc_port_profile_created;  /* Flag to check MISC PORT profile creation */
    int lport_ind_profile_created;  /* Flag to check LPORT_ 1 profile creation */
    int rtag7_ind_profile_created;  /* Flag to check RTAG7_ 1 profile creation */
    int egr_lport_profile_created;  /* Flag to check Egr LPORT profile creation */
    int egr_qos_profile_created;  /* Flag to check Egr QOS profile creation */
#ifdef BCM_SABER2_SUPPORT
    int service_pri_map_created; /* Flag to check ING/EGR_SERVICE_PRI_MAP_0/1/2 
                                         TABLEs creation */
#endif
#ifdef BCM_SABER2_SUPPORT
    soc_profile_mem_t *service_pri_map_table; /* cache of ING/EGR_SERVICE_PRI_MAP_0/1/2 
                                            (profile) */
#endif /* BCM_SABER2_SUPPORT */
#ifdef BCM_HURRICANE3_SUPPORT
    soc_profile_mem_t *egr_header_encap_data;  /* cache of EGR_HEADER_ENCAP_DATA */
    soc_profile_mem_t *custom_header_match;     /* cache of CUSTOM_HEADER_MATCH */
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    soc_profile_mem_t *ing_tunnel_term_map; /* cache of ING_TUNNEL_TERM_MAP profile */
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
    defined(BCM_TRIDENT3_SUPPORT)
    soc_profile_mem_t *ing_exp_to_ip_ecn_map; /* cache of ING_EXP_TO_IP_ECN_MAP profile */
    soc_profile_mem_t *egr_ip_ecn_to_exp_map; /* cache of EGR_IP_ECN_TO_EXP_MAP profile */
    soc_profile_mem_t *egr_int_cn_to_exp_map; /*cache of EGR_INT_CN_TO_EXP_MAP profile */
#endif /* BCM_TOMAHAWK2_SUPPORT or BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_FLOWTRACKER_SUPPORT
    soc_profile_mem_t *age_out_profile;
    soc_profile_mem_t *flow_limit_profile;
    soc_profile_mem_t *collector_copy_profile;
    soc_profile_mem_t *pdd_profile;
    soc_profile_mem_t *meter_profile;
    soc_profile_mem_t *ts_profile;
    soc_profile_mem_t *alu_mask_profile;
    soc_profile_mem_t *agg_alu_mask_profile;
#endif /*BCM_FLOWTRACKER_SUPPORT*/
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    soc_profile_mem_t *subport_tunnel_pbmp;
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

} _bcm_common_bookkeeping_t;

STATIC _bcm_common_bookkeeping_t  _bcm_common_bk_info[BCM_MAX_NUM_UNITS] = {{ 0 }};
STATIC sal_mutex_t _common_mutex[BCM_MAX_NUM_UNITS] = {NULL};
 
#define COMMON_INFO(_unit_)   (&_bcm_common_bk_info[_unit_])
#define MAC_DA_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].mac_da_profile)
#define LPORT_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].lport_profile)
#define ING_PRI_CNG_MAP(_unit_) (_bcm_common_bk_info[_unit_].ing_pri_cng_map)
#define EGR_MPLS_COMBO_MAP(_unit_) (_bcm_common_bk_info[_unit_].egr_mpls_combo_map)
#define DSCP_TABLE(_unit_) (_bcm_common_bk_info[_unit_].dscp_table)
#define EGR_DSCP_TABLE(_unit_) (_bcm_common_bk_info[_unit_].egr_dscp_table)
#define ING_OUTER_DOT1P(_unit_) (_bcm_common_bk_info[_unit_].ing_outer_dot1p)
#define PROT_PKT_CTRL(_unit_) (_bcm_common_bk_info[_unit_].prot_pkt_ctrl)
#define ING_VFT_PRI_MAP(_unit_) (_bcm_common_bk_info[_unit_].ing_vft_pri_map)
#define ING_L2_VLAN_ETAG_MAP(_unit_) (_bcm_common_bk_info[_unit_].ing_l2_vlan_etag_map)
#define EGR_VFT_PRI_MAP(_unit_) (_bcm_common_bk_info[_unit_].egr_vft_pri_map)
#define EGR_VSAN_INTPRI_MAP(_unit_) (_bcm_common_bk_info[_unit_].egr_vsan_intpri_map)
#define EGR_VFT_FIELDS(_unit_) (_bcm_common_bk_info[_unit_].egr_vft_fields)
#define EGR_L2_VLAN_ETAG_MAP(_unit_) (_bcm_common_bk_info[_unit_].egr_l2_vlan_etag_map)
#define MPLS_STATION_HASH(_unit_, _index_) \
        (_bcm_common_bk_info[_unit_].mpls_station_hash[_index_])

#ifdef BCM_TRIDENT2PLUS_SUPPORT
#define ING_VLAN_VFI_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].ing_vlan_vfi_mbrship_profile)
#define EGR_VLAN_VFI_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].egr_vlan_vfi_mbrship_profile)
#define MISC_PORT_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].misc_port_profile)
#endif
#ifdef BCM_TRIDENT3_SUPPORT
#define EGR_VLAN_VFI_UNTAG_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].egr_vlan_vfi_untag_profile)
#endif
#define LPORT_IND_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].lport_ind_profile)
#define RTAG7_IND_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].rtag7_ind_profile)
#define EGR_LPORT_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].egr_lport_profile)
#define EGR_QOS_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].egr_qos_profile)

#ifdef BCM_TRIUMPH3_SUPPORT
#define ING_L3_NH_ATTRIB_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].ing_l3_nh_attribute)
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
#define L3_IIF_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].l3_iif_profile)
#define L3_IP4_OPTIONS_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].l3_ip4_options_profile)
#define FC_MAP_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].fc_map_profile)
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
#define VFI_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].vfi_profile)
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
#define OFFSET_MAP_TABLE(_unit_) (_bcm_common_bk_info[_unit_].offset_map_table)
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
#define SERVICE_PRI_MAP_TABLE(_unit_) (_bcm_common_bk_info[_unit_].service_pri_map_table)
#endif /* BCM_SABER2_SUPPORT */


#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
#define VLAN_PROTOCOL_DATA_TABLE(_unit_) \
        (_bcm_common_bk_info[_unit_].vlan_protocol_data_profile)
#endif /* BCM_KATANA2_SUPPORT */

#ifdef BCM_HURRICANE3_SUPPORT
#define EGR_HEADER_ENCAP_DATA(_unit_) \
        (_bcm_common_bk_info[_unit_].egr_header_encap_data)
#define CUSTOM_HEADER_MATCH(_unit_) \
        (_bcm_common_bk_info[_unit_].custom_header_match)
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
#define ING_TUNNEL_TERM_MAP(_unit_) \
            (_bcm_common_bk_info[_unit_].ing_tunnel_term_map)
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
    defined(BCM_TRIDENT3_SUPPORT)
#define ING_EXP_TO_IP_ECN_MAP(_unit_) \
                (_bcm_common_bk_info[_unit_].ing_exp_to_ip_ecn_map)
#define EGR_IP_ECN_TO_EXP_MAP(_unit_) \
                    (_bcm_common_bk_info[_unit_].egr_ip_ecn_to_exp_map)
#define EGR_INT_CN_TO_EXP_MAP(_unit_) \
                    (_bcm_common_bk_info[_unit_].egr_int_cn_to_exp_map)
#endif /* BCM_TOMAHAWK2_SUPPORT or BCM_TOMAHAWK3_SUPPORT */

#ifdef BCM_FLOWTRACKER_SUPPORT

#define FT_AGE_OUT(_u) \
            (_bcm_common_bk_info[_u].age_out_profile)

#define FT_FLOW_LIMIT(_u) \
            (_bcm_common_bk_info[_u].flow_limit_profile)

#define FT_COLLECTOR_COPY(_u) \
            (_bcm_common_bk_info[_u].collector_copy_profile)

#define FT_PDD_PROFILE(_u) \
            (_bcm_common_bk_info[_u].pdd_profile)

#define FT_ALU_MASK_PROFILE(_u) \
            (_bcm_common_bk_info[_u].alu_mask_profile)

#define FT_AGG_ALU_MASK_PROFILE(_u) \
            (_bcm_common_bk_info[_u].agg_alu_mask_profile)

#define FT_METER_PROFILE(_u) \
            (_bcm_common_bk_info[_u].meter_profile)

#define FT_TS_PROFILE(_u) \
            (_bcm_common_bk_info[_u].ts_profile)

#endif /* BCM_FLOWTRACKER_SUPPORT */

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
#define SUBPORT_TUNNEL_PBMP(_u) \
            (_bcm_common_bk_info[_u].subport_tunnel_pbmp)
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

/* 
 * Common resource lock
 */
#define COMMON_LOCK(unit) \
        sal_mutex_take(_common_mutex[unit], sal_mutex_FOREVER); 

#define COMMON_UNLOCK(unit) \
        sal_mutex_give(_common_mutex[unit]); 

#if defined(BCM_TRIDENT3_SUPPORT)
#define VFP_LO_FOR_TUNNEL_TERM_FLOW_STRENGTH 0x28
#define VFP_LO_FOR_NON_TUNNEL_TERM_FLOW_STRENGTH 0x28
#define VFP_HI_FOR_TUNNEL_TERM_FLOW_STRENGTH 0x3c
#define VFP_HI_FOR_NON_TUNNEL_TERM_FLOW_STRENGTH 0x3c
#define SGPP_FOR_TUNNEL_TERM_FLOW_STRENGTH 0x32
#define SGPP_FOR_NON_TUNNEL_TERM_FLOW_STRENGTH 0x32
#define VFI_FOR_TUNNEL_TERM_FLOW_STRENGTH 0x1e
#define VFI_FOR_NON_TUNNEL_TERM_FLOW_STRENGTH 0x1e
#define L3_IIF_FOR_TUNNEL_TERM_FLOW_STRENGTH 0xa
#define L3_IIF_FOR_NON_TUNNEL_TERM_FLOW_STRENGTH 0xa
#endif

/* Forward declaration */
STATIC int
_bcm_trx_mpls_station_hash_calc(int unit, bcm_mac_t mac,
                                bcm_vlan_t vlan, uint16 *hash);
#ifdef BCM_TRIUMPH2_SUPPORT
STATIC int
_bcm_esw_flex_stat_hw_set(int unit, _bcm_flex_stat_type_t type,
                          int fs_idx, _bcm_flex_stat_t stat, uint64 val);
STATIC int
_bcm_esw_flex_stat_sw_clear(int unit, _bcm_flex_stat_type_t type,
                            int fs_idx);
#endif

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
static soc_memacc_t *_bcm_trx_mysta_memacc_list[BCM_MAX_NUM_UNITS];
static soc_memacc_t *_bcm_trx_mysta2_memacc_list[BCM_MAX_NUM_UNITS];
STATIC int _bcm_trx_mysta_memacc_init(int unit);
#endif 


/*
 * Function:
 *      _bcm_common_free_resource
 * Purpose:
 *      Free all allocated tables and memory
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_common_free_resource(int unit, _bcm_common_bookkeeping_t *info)
{
    if (!info) {
        return;
    }
    if (_common_mutex[unit]) {
        sal_mutex_destroy(_common_mutex[unit]);
        _common_mutex[unit] = NULL;
    } 
    if (info->mpls_station_hash) {
        sal_free(info->mpls_station_hash);
        info->mpls_station_hash = NULL;
    }
    if (info->my_station_shadow) {
        sal_free(info->my_station_shadow);
        info->my_station_shadow = NULL;
    }
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (info->my_station_shadow_mask) {
        sal_free(info->my_station_shadow_mask);
        info->my_station_shadow_mask = NULL;
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (info->my_station2_shadow_mask) {
        sal_free(info->my_station2_shadow_mask);
        info->my_station2_shadow_mask = NULL;
    }
#endif

    if (info->my_station_l3_mask) {
        sal_free(info->my_station_l3_mask);
        info->my_station_l3_mask = NULL;
    }
    if (info->my_station_tunnel_mask) {
        sal_free(info->my_station_tunnel_mask);
        info->my_station_tunnel_mask = NULL;
    }

#ifdef BCM_RIOT_SUPPORT
    if (info->my_station2_shadow) {
        sal_free(info->my_station2_shadow);
        info->my_station2_shadow = NULL;
    }
#endif /* BCM_RIOT_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (_bcm_trx_mysta_memacc_list[unit]) {
        sal_free(_bcm_trx_mysta_memacc_list[unit]);
        _bcm_trx_mysta_memacc_list[unit] = NULL;
    }
#endif
#ifdef BCM_RIOT_SUPPORT
    if (_bcm_trx_mysta2_memacc_list[unit]) {
        sal_free(_bcm_trx_mysta2_memacc_list[unit]);
        _bcm_trx_mysta2_memacc_list[unit] = NULL;
    }
#endif /* BCM_RIOT_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_KATANA2(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        if (info->vlan_protocol_data_profile) {
            if(info->vlan_protocol_data_profile_created) {
                (void) soc_profile_mem_destroy(unit, info->vlan_protocol_data_profile);
            }
            sal_free(info->vlan_protocol_data_profile);
            info->vlan_protocol_data_profile = NULL;
        }
    }
#endif
    if (info->mac_da_profile) {
        if (info->mac_da_profile_created) {
            (void) soc_profile_mem_destroy(unit, info->mac_da_profile);
        }
        sal_free(info->mac_da_profile);
        info->mac_da_profile = NULL;
    }
    if (info->lport_profile) {
        if (info->lport_profile_created) {
            (void) soc_profile_mem_destroy(unit, info->lport_profile);
        }
        sal_free(info->lport_profile);
        info->lport_profile = NULL;
    }

    if (info->lport_ind_profile) {
        if (info->lport_ind_profile_created) {
            (void) soc_profile_mem_destroy(unit, info->lport_ind_profile);
        }
        sal_free(info->lport_ind_profile);
        info->lport_ind_profile = NULL;
    }

    if (info->rtag7_ind_profile) {
        if (info->rtag7_ind_profile_created) {
            (void) soc_profile_mem_destroy(unit, info->rtag7_ind_profile);
        }
        sal_free(info->rtag7_ind_profile);
        info->rtag7_ind_profile = NULL;
    }

    if (info->egr_lport_profile) {
        if (info->egr_lport_profile_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_lport_profile);
        }
        sal_free(info->egr_lport_profile);
        info->egr_lport_profile = NULL;
    }

    if (info->egr_qos_profile) {
        if (info->egr_qos_profile_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_qos_profile);
        }
        sal_free(info->egr_qos_profile);
        info->egr_qos_profile = NULL;
    }

    if (info->ing_pri_cng_map) {
        if (info->ing_pri_cng_map_created) {
            (void) soc_profile_mem_destroy(unit, info->ing_pri_cng_map);
        }
        sal_free(info->ing_pri_cng_map);
        info->ing_pri_cng_map = NULL;
    }
    if (info->ing_vft_pri_map) {
        if (info->ing_vft_pri_map_created) {
            (void) soc_profile_mem_destroy(unit, info->ing_vft_pri_map);
        }
        sal_free(info->ing_vft_pri_map);
        info->ing_vft_pri_map = NULL;
    }
    if (info->ing_l2_vlan_etag_map) {
        if (info->ing_l2_vlan_etag_map_created) {
            (void) soc_profile_mem_destroy(unit, info->ing_l2_vlan_etag_map);
        }
        sal_free(info->ing_l2_vlan_etag_map);
        info->ing_l2_vlan_etag_map = NULL;
    }    
    if (info->egr_vft_pri_map) {
        if (info->egr_vft_pri_map_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_vft_pri_map);
        }
        sal_free(info->egr_vft_pri_map);
        info->egr_vft_pri_map = NULL;
    }
    if (info->egr_vsan_intpri_map) {
        if (info->egr_vsan_intpri_map_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_vsan_intpri_map);
        }
        sal_free(info->egr_vsan_intpri_map);
        info->egr_vsan_intpri_map = NULL;
    }
    if (info->egr_l2_vlan_etag_map) {
        if (info->egr_l2_vlan_etag_map_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_l2_vlan_etag_map);
        }
        sal_free(info->egr_l2_vlan_etag_map);
        info->egr_l2_vlan_etag_map = NULL;
    }
    if (info->egr_vft_fields) {
        if (info->egr_vft_fields_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_vft_fields);
        }
        sal_free(info->egr_vft_fields);
        info->egr_vft_fields = NULL;
    }
    if (info->egr_mpls_combo_map) {
        if (info->egr_mpls_combo_map_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_mpls_combo_map);
        }
        sal_free(info->egr_mpls_combo_map);
        info->egr_mpls_combo_map = NULL;
    }
    if (info->dscp_table) {
        if (info->dscp_table_created) {
            (void) soc_profile_mem_destroy(unit, info->dscp_table);
        }
        sal_free(info->dscp_table);
        info->dscp_table = NULL;
    }
    if (info->egr_dscp_table) {
        if (info->egr_dscp_table_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_dscp_table);
        }
        sal_free(info->egr_dscp_table);
        info->egr_dscp_table = NULL;
    }
    if (info->ing_outer_dot1p) {
        if (info->ing_outer_dot1p_created) {
            (void) soc_profile_mem_destroy(unit, info->ing_outer_dot1p);
        }
        sal_free(info->ing_outer_dot1p);
        info->ing_outer_dot1p = NULL;
    }
    if (info->prot_pkt_ctrl) {
        if (info->prot_pkt_ctrl_created) {
            (void) soc_profile_reg_destroy(unit, info->prot_pkt_ctrl);
        }
        sal_free(info->prot_pkt_ctrl);
        info->prot_pkt_ctrl = NULL;
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        if (info->ing_l3_nh_attribute) {
            if (info->ing_l3_nh_attribute_created) {
                (void) soc_profile_mem_destroy(unit,
                        info->ing_l3_nh_attribute);
            }
            sal_free(info->ing_l3_nh_attribute);
            info->ing_l3_nh_attribute = NULL;
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        if (info->l3_iif_profile) {
            if (info->l3_iif_profile_created) {
                (void) soc_profile_mem_destroy(unit,
                        info->l3_iif_profile);
            }
            sal_free(info->l3_iif_profile);
            info->l3_iif_profile = NULL;
        }
        if (info->l3_ip4_options_profile) {
            if (info->l3_ip4_options_profile_created) {
                (void) soc_profile_mem_destroy(unit,
                        info->l3_ip4_options_profile);
            }
            sal_free(info->l3_ip4_options_profile);
            info->l3_ip4_options_profile = NULL;
        }
        if (info->fc_map_profile) {
            if (info->fc_map_profile_created) {
                (void) soc_profile_mem_destroy(unit,
                        info->fc_map_profile);
            }
            sal_free(info->fc_map_profile);
            info->fc_map_profile = NULL;
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (info->offset_map_table) {
        if (info->ing_queue_offset_map_created) {
            (void) soc_profile_mem_destroy(unit, info->offset_map_table);
        }
        sal_free(info->offset_map_table);
        info->offset_map_table = NULL;
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (info->vfi_profile) {
        if (info->vfi_profile_created) {
            (void)soc_profile_mem_destroy(unit, info->vfi_profile);
        }
        sal_free(info->vfi_profile);
        info->vfi_profile = NULL;
    }

    if (info->ing_vlan_vfi_mbrship_profile) {
        if (info->ing_vlan_vfi_mbrship_profile_created) {
            (void) soc_profile_mem_destroy(unit,
                    info->ing_vlan_vfi_mbrship_profile);
        }
        sal_free(info->ing_vlan_vfi_mbrship_profile);
        info->ing_vlan_vfi_mbrship_profile = NULL;
    }

    if (info->egr_vlan_vfi_mbrship_profile) {
        if (info->egr_vlan_vfi_mbrship_profile_created) {
            (void) soc_profile_mem_destroy(unit,
                    info->egr_vlan_vfi_mbrship_profile);
        }
        sal_free(info->egr_vlan_vfi_mbrship_profile);
        info->egr_vlan_vfi_mbrship_profile = NULL;
    }

    if (info->misc_port_profile) {
        if (info->misc_port_profile_created) {
            (void) soc_profile_mem_destroy(unit, info->misc_port_profile);
        }
        sal_free(info->misc_port_profile);
        info->misc_port_profile = NULL;
    }
#endif

#ifdef BCM_TRIDENT3_SUPPORT
    if (info->egr_vlan_vfi_untag_profile) {
        if (info->egr_vlan_vfi_untag_profile_created) {
            (void) soc_profile_mem_destroy(unit,
                    info->egr_vlan_vfi_untag_profile);
        }
        sal_free(info->egr_vlan_vfi_untag_profile);
        info->egr_vlan_vfi_untag_profile = NULL;
    }
#endif

#ifdef BCM_SABER2_SUPPORT
    if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
        if (info->service_pri_map_table) {
            if (info->service_pri_map_created) {
                (void) soc_profile_mem_destroy(unit,
                                     info->service_pri_map_table);
            }
            sal_free(info->service_pri_map_table);
            info->service_pri_map_table = NULL;
        }
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_HURRICANE3_SUPPORT
    if (soc_feature(unit, soc_feature_miml) || 
        soc_feature(unit, soc_feature_custom_header)) {
        if (info->egr_header_encap_data) {
            if (info->egr_header_encap_data_created) {
                (void) soc_profile_mem_destroy(unit, info->egr_header_encap_data);
            }
            sal_free(info->egr_header_encap_data);
            info->egr_header_encap_data = NULL;
        }

        if (info->custom_header_match) {
            if (info->custom_header_match) {
                (void) soc_profile_mem_destroy(unit, info->custom_header_match);
            }
            sal_free(info->custom_header_match);
            info->custom_header_match = NULL;
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (soc_feature(unit, soc_feature_ecn_wred)) {
            if (info->ing_tunnel_term_map) {
                if (info->ing_tunnel_term_map_created) {
                    (void) soc_profile_mem_destroy(unit, info->ing_tunnel_term_map);
                }
                sal_free(info->ing_tunnel_term_map);
                info->ing_tunnel_term_map = NULL;
            }
        }
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
    defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_mpls_ecn)) {
            if (info->ing_exp_to_ip_ecn_map) {
                if (info->ing_exp_to_ip_ecn_map_created) {
                    (void) soc_profile_mem_destroy(unit, info->ing_exp_to_ip_ecn_map);
                }
                sal_free(info->ing_exp_to_ip_ecn_map);
                info->ing_exp_to_ip_ecn_map = NULL;
            }
            if (info->egr_ip_ecn_to_exp_map) {
                if (info->egr_ip_ecn_to_exp_map_created) {
                    (void) soc_profile_mem_destroy(unit, info->egr_ip_ecn_to_exp_map);
                }
                sal_free(info->egr_ip_ecn_to_exp_map);
                info->egr_ip_ecn_to_exp_map = NULL;
            }
            if (info->egr_int_cn_to_exp_map) {
                if (info->egr_int_cn_to_exp_map_created) {
                    (void) soc_profile_mem_destroy(unit, info->egr_int_cn_to_exp_map);
                }
                sal_free(info->egr_int_cn_to_exp_map);
                info->egr_int_cn_to_exp_map = NULL;
            }            
        }
#endif /* BCM_TOMAHAWK2_SUPPORT or BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        if (soc_feature(unit, soc_feature_gport_service_counters)) {
            /* Initialize flex stats */
            (void) _bcm_esw_flex_stat_detach(unit, _bcmFlexStatTypeGport);
            (void) _bcm_esw_flex_stat_detach(unit, _bcmFlexStatTypeService);
            (void) _bcm_esw_flex_stat_detach(unit, _bcmFlexStatTypeEgressGport);
            (void) _bcm_esw_flex_stat_detach(unit, _bcmFlexStatTypeEgressService);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */


#ifdef BCM_FLOWTRACKER_SUPPORT
    if(soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        if (FT_AGE_OUT(unit)) {
            (void) soc_profile_mem_destroy(unit, FT_AGE_OUT(unit));
            sal_free(FT_AGE_OUT(unit));
            FT_AGE_OUT(unit) = NULL;
        }

        if (FT_FLOW_LIMIT(unit)) {
            (void) soc_profile_mem_destroy(unit, FT_FLOW_LIMIT(unit));
            sal_free(FT_FLOW_LIMIT(unit));
            FT_FLOW_LIMIT(unit) = NULL;
        }

        if (FT_COLLECTOR_COPY(unit)) {
            (void) soc_profile_mem_destroy(unit, FT_COLLECTOR_COPY(unit));
            sal_free(FT_COLLECTOR_COPY(unit));
            FT_COLLECTOR_COPY(unit) = NULL;
        }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
            if (FT_ALU_MASK_PROFILE(unit)) {
                (void) soc_profile_mem_destroy(unit, FT_ALU_MASK_PROFILE(unit));
                sal_free(FT_ALU_MASK_PROFILE(unit));
                FT_ALU_MASK_PROFILE(unit) = NULL;
            }
        }
#endif

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
            if (FT_AGG_ALU_MASK_PROFILE(unit)) {
                (void) soc_profile_mem_destroy(unit, FT_AGG_ALU_MASK_PROFILE(unit));
                sal_free(FT_AGG_ALU_MASK_PROFILE(unit));
                FT_AGG_ALU_MASK_PROFILE(unit) = NULL;
            }
        }
#endif

        if (FT_PDD_PROFILE(unit)) {
            (void) soc_profile_mem_destroy(unit, FT_PDD_PROFILE(unit));
            sal_free(FT_PDD_PROFILE(unit));
            FT_PDD_PROFILE(unit) = NULL;
        }

        if (FT_METER_PROFILE(unit)) {
            (void) soc_profile_mem_destroy(unit, FT_METER_PROFILE(unit));
            sal_free(FT_METER_PROFILE(unit));
           FT_METER_PROFILE(unit) = NULL;
        }

        if (FT_TS_PROFILE(unit)) {
            (void) soc_profile_mem_destroy(unit, FT_TS_PROFILE(unit));
            sal_free(FT_TS_PROFILE(unit));
           FT_TS_PROFILE(unit) = NULL;
        }
    }
#endif /* BCM_FLOWTRACKER_SUPPORT */

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        if (SUBPORT_TUNNEL_PBMP(unit)) {
            (void) soc_profile_mem_destroy(unit, SUBPORT_TUNNEL_PBMP(unit));
            sal_free(SUBPORT_TUNNEL_PBMP(unit));
            SUBPORT_TUNNEL_PBMP(unit) = NULL;
        }
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_2

#if defined(BCM_TRIUMPH2_SUPPORT)
STATIC int
_bcm_common_wb_scache_size_get_tr2_flex_stat(int unit,
                                                        int *req_scache_size);
STATIC int
_bcm_common_wb_tr2_flex_stat_sync(int unit, uint8 **scache_ptr);
STATIC int
_bcm_common_wb_tr2_flex_stat_reinit(int unit, uint8 **scache_ptr);
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
STATIC int
_bcm_common_wb_scache_size_get_my_station_shadow_mask(int unit,
                                                      int *req_scache_size);
STATIC int
_bcm_common_wb_my_station_shadow_mask_sync(int unit, uint8 **scache_ptr);
STATIC int
_bcm_common_wb_my_station_shadow_mask_reinit(int unit, int is_station2, uint8 **scache_ptr);
#endif

/* Returns required scache size for common module */
STATIC int
_bcm_common_wb_scache_size_get(int unit, int *req_scache_size)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        BCM_IF_ERROR_RETURN(
            _bcm_common_wb_scache_size_get_tr2_flex_stat(unit, req_scache_size));
    }
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    BCM_IF_ERROR_RETURN(
        _bcm_common_wb_scache_size_get_my_station_shadow_mask(unit, req_scache_size));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        int tmp_size=0;

        tmp_size = (*req_scache_size);
        (*req_scache_size) = 2*tmp_size;
    }
#endif
#endif
    return BCM_E_NONE;
}

/* Allocates required scache size for the common module recovery */
STATIC int
_bcm_common_wb_alloc(int unit)
{
    int     rv;
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;
    int     req_scache_size = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COMMON, 0);

    rv = _bcm_common_wb_scache_size_get(unit, &req_scache_size);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
            req_scache_size, &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_common_wb_sync
 * Purpose:
 *      Syncs common warmboot state to scache
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_common_scache_sync(int unit)
{
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COMMON, 0);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL));
#if defined(BCM_TRIUMPH2_SUPPORT)    
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        BCM_IF_ERROR_RETURN(_bcm_common_wb_tr2_flex_stat_sync(unit, &scache_ptr));
    }
#endif /* BCM_TRIUMPH2_SUPPORT */  
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    BCM_IF_ERROR_RETURN(_bcm_common_wb_my_station_shadow_mask_sync(unit, &scache_ptr));
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_common_wb_reinit
 * Purpose:
 *      Recovers common warmboot state from scache
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_common_wb_reinit(int unit)
{
    int     rv = BCM_E_INTERNAL;
    uint8   *scache_ptr;
    uint16  recovered_ver = 0;
    soc_scache_handle_t scache_handle;
    int additional_scache_size = 0;
    int alloc_size = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COMMON, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);

    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    } else if (rv == BCM_E_NOT_FOUND) {
        return _bcm_common_wb_alloc(unit);
    } else if (BCM_E_NONE == rv) {
        if (recovered_ver >= BCM_WB_VERSION_1_0) {
#if defined(BCM_TRIUMPH2_SUPPORT)
            if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
                BCM_IF_ERROR_RETURN(_bcm_common_wb_tr2_flex_stat_reinit(unit, &scache_ptr));
            }
#endif /* BCM_TRIUMPH2_SUPPORT */            
        }

        if (recovered_ver >= BCM_WB_VERSION_1_1) {
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
            BCM_IF_ERROR_RETURN(
                _bcm_common_wb_my_station_shadow_mask_reinit(
                    unit, FALSE, &scache_ptr));
#endif
        } else {
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
            BCM_IF_ERROR_RETURN(
                _bcm_common_wb_scache_size_get_my_station_shadow_mask(
                    unit, &alloc_size));
#endif
            additional_scache_size += alloc_size;
        }

        if (recovered_ver >= BCM_WB_VERSION_1_2) {
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN(
                     _bcm_common_wb_my_station_shadow_mask_reinit(
                         unit, TRUE, &scache_ptr));
            }
#endif
        } else {
            alloc_size = 0;
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_common_wb_scache_size_get_my_station_shadow_mask(
                        unit, &alloc_size));
            }
#endif
            additional_scache_size += alloc_size;
        }

        if (additional_scache_size > 0) {
            rv = soc_scache_realloc(unit,scache_handle,additional_scache_size);
            if(BCM_FAILURE(rv)) {
               return rv;
            }
        }
    }

    return rv;
}

STATIC int
_bcm_common_reinit(int unit)
{
    int i, num_station;
    bcm_mac_t hw_mac;
    bcm_vlan_t hw_vlan;
    mpls_station_tcam_entry_t mpls_station_entry;
    uint16 hash;

    if (SOC_MEM_IS_VALID(unit, MPLS_STATION_TCAMm)) {
        /* Compute hash for each entry */
        num_station = soc_mem_index_count(unit, MPLS_STATION_TCAMm);
        for (i = 0; i < num_station; i++) {
            BCM_IF_ERROR_RETURN
                (READ_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY, i, &mpls_station_entry));
            soc_mem_mac_addr_get(unit, MPLS_STATION_TCAMm,
                                 &mpls_station_entry, MAC_ADDRf, hw_mac);
            hw_vlan = soc_mem_field32_get(unit, MPLS_STATION_TCAMm,
                                          &mpls_station_entry, VLAN_IDf);
            /* Get the hash value for this key */
            BCM_IF_ERROR_RETURN
                (_bcm_trx_mpls_station_hash_calc(unit, hw_mac, hw_vlan, &hash));
            MPLS_STATION_HASH(unit, i) = hash;
        }
    } else {
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, MY_STATION_TCAMm)) {
            _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
            my_station_tcam_entry_t   entry;
            bcm_mac_t                 mac_addr; 
            bcm_vlan_t                vid;
            bcm_l2_addr_t             l2addr;
            int                       rv;
            sal_memset(&l2addr, 0, sizeof(bcm_l2_addr_t));

            /*
             * The code is used to recover COMMON_INFO(unit)->my_station_shadow
             * before BCM_WB_VERSION_1_1 where L2 recovery for the bookkeeping
             * is added.
             */
            /* Compute hash for each entry */
            num_station = soc_mem_index_count(unit, MY_STATION_TCAMm);
            for (i = 0; i < num_station; i++) {
                sal_memcpy(&entry, soc_mem_entry_null(unit, MY_STATION_TCAMm),
                           sizeof(my_station_tcam_entry_t));
                BCM_IF_ERROR_RETURN
                    (READ_MY_STATION_TCAMm(unit, MEM_BLOCK_ANY, i, &entry));
                if (!soc_mem_field32_get(unit,
                                          MY_STATION_TCAMm, &entry, VALIDf)) {
                    continue;
                }
                
                soc_mem_mac_addr_get(unit, MY_STATION_TCAMm,
                                     &entry, MAC_ADDRf, mac_addr);
                vid = soc_mem_field32_get(unit, MY_STATION_TCAMm,
                                          &entry, VLAN_IDf);
#ifdef BCM_TRIUMPH3_SUPPORT
                if (soc_feature(unit, soc_feature_ism_memory)) {
                    rv = _bcm_tr3_l2_addr_get(unit, mac_addr, vid, TRUE, &l2addr);
                    if (BCM_FAILURE(rv)) {
                        continue;
                    }
                } else
#endif
                {
                    rv = _bcm_tr_l2_addr_get(unit, mac_addr, vid, TRUE, &l2addr);
                    if (BCM_FAILURE(rv)) {
                        continue;
                    }
                }
                
                sal_memcpy(&info->my_station_shadow[i], &entry,
                           sizeof(my_station_tcam_entry_t));
            }

#ifdef BCM_RIOT_SUPPORT
            if (soc_feature(unit, soc_feature_riot) &&
                                              !(SOC_IS_TRIDENT3X(unit))) {
                num_station = soc_mem_index_count(unit, MY_STATION_TCAM_2m);
                for (i = 0; i < num_station; i++) {
                    BCM_IF_ERROR_RETURN
                        (READ_MY_STATION_TCAM_2m(unit, MEM_BLOCK_ANY, i,
                                               &info->my_station2_shadow[i]));
                }
            }
#endif /* BCM_RIOT_SUPPORT */

            /* Initialize memory accelation structures */
            BCM_IF_ERROR_RETURN(_bcm_trx_mysta_memacc_init(unit));
        }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */
    }

    /* Restore the CPU TDM slot cache entry in soc_control */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
        int i, alloc_size;
        uint32 port_num;
        arb_tdm_table_entry_t *arb_tdm, *arb_tdm_table;
        iarb_tdm_table_entry_t *iarb_tdm, *iarb_tdm_table;
        alloc_size = sizeof(arb_tdm_table_entry_t) * 
                     soc_mem_index_count(unit, ARB_TDM_TABLEm);
        arb_tdm_table = soc_cm_salloc(unit, alloc_size, "ARB TDM reload");
        if (arb_tdm_table == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(arb_tdm_table, 0, alloc_size);
        alloc_size = sizeof(iarb_tdm_table_entry_t) * 
                     soc_mem_index_count(unit, IARB_TDM_TABLEm);
        iarb_tdm_table = soc_cm_salloc(unit, alloc_size, "IARB TDM reload");
        if (iarb_tdm_table == NULL) {
            soc_cm_sfree(unit, arb_tdm_table);
            return BCM_E_MEMORY;
        }
        sal_memset(iarb_tdm_table, 0, alloc_size);
        if (soc_mem_read_range(unit, ARB_TDM_TABLEm, MEM_BLOCK_ANY,
                               soc_mem_index_min(unit, ARB_TDM_TABLEm),
                               soc_mem_index_max(unit, ARB_TDM_TABLEm),
                               arb_tdm_table) < 0) {
            soc_cm_sfree(unit, arb_tdm_table);
            soc_cm_sfree(unit, iarb_tdm_table);
            return SOC_E_INTERNAL;
        }
        if (soc_mem_read_range(unit, IARB_TDM_TABLEm, MEM_BLOCK_ANY,
                               soc_mem_index_min(unit, IARB_TDM_TABLEm),
                               soc_mem_index_max(unit, IARB_TDM_TABLEm),
                               iarb_tdm_table) < 0) {
            soc_cm_sfree(unit, arb_tdm_table);
            soc_cm_sfree(unit, iarb_tdm_table);
            return SOC_E_INTERNAL;
        }
        for (i = 0; i < soc_mem_index_count(unit, ARB_TDM_TABLEm); i++) {
            arb_tdm = soc_mem_table_idx_to_pointer(unit, ARB_TDM_TABLEm,
                                                   arb_tdm_table_entry_t *,
                                                   arb_tdm_table, i);
            iarb_tdm = soc_mem_table_idx_to_pointer(unit, IARB_TDM_TABLEm,
                                                    iarb_tdm_table_entry_t *,
                                                    iarb_tdm_table, i);             
            if (soc_ARB_TDM_TABLEm_field32_get(unit, arb_tdm, WRAP_ENf)) {
                break;
            }
            port_num = soc_ARB_TDM_TABLEm_field32_get(unit, arb_tdm, PORT_NUMf);
            if (IS_CPU_PORT(unit, port_num)) {
                sal_memcpy(&(SOC_CONTROL(unit)->iarb_tdm), iarb_tdm, 
                           sizeof(iarb_tdm_table_entry_t));
                SOC_CONTROL(unit)->iarb_tdm_idx = i;
            }
        }
        if (arb_tdm_table != NULL) {
            soc_cm_sfree(unit, arb_tdm_table);
        }
        if (iarb_tdm_table != NULL) {
            soc_cm_sfree(unit, iarb_tdm_table);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */  

    _bcm_common_wb_reinit(unit);

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, MY_STATION_TCAMm)) {
        _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
        my_station_tcam_entry_t   entry;

        /* Compute hash for each entry */
        num_station = soc_mem_index_count(unit, MY_STATION_TCAMm);
        for (i = 0; i < num_station; i++) {
            sal_memcpy(&entry, soc_mem_entry_null(unit, MY_STATION_TCAMm),
                       sizeof(my_station_tcam_entry_t));
            BCM_IF_ERROR_RETURN
                (READ_MY_STATION_TCAMm(unit, MEM_BLOCK_ANY, i, &entry));
            if (!(info->my_station_shadow_mask->buf[i/32] & 1<<(i%32))) {
                continue;
            }
            sal_memcpy(&info->my_station_shadow[i], &entry,
                       sizeof(my_station_tcam_entry_t));
        }
    }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
        my_station_tcam_2_entry_t   entry;

        /* Compute hash for each entry */
        num_station = soc_mem_index_count(unit, MY_STATION_TCAM_2m);
        for (i = 0; i < num_station; i++) {  
            sal_memcpy(&entry, soc_mem_entry_null(unit, MY_STATION_TCAM_2m),
                       sizeof(my_station_tcam_2_entry_t));
            BCM_IF_ERROR_RETURN
                (READ_MY_STATION_TCAM_2m(unit, MEM_BLOCK_ANY, i, &entry));
            if (!soc_mem_field32_get(unit,MY_STATION_TCAM_2m, &entry, VALIDf)) {
                    continue;
            }
            if (!(info->my_station2_shadow_mask->buf[i/32] & 1<<(i%32))) {
                continue;
            }
            sal_memcpy(&info->my_station2_shadow[i], &entry,
                       sizeof(my_station_tcam_2_entry_t));
        }
    }
#endif

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_common_cleanup
 * Purpose:
 *      Internal function for initializing common resource management
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */
int _bcm_common_cleanup(int unit)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    _bcm_common_free_resource(unit, info);
    info->mac_da_profile_created = FALSE;
    info->lport_profile_created = FALSE;
    info->ing_pri_cng_map_created = FALSE;
    info->ing_vft_pri_map_created = FALSE;
    info->ing_outer_dot1p_created = FALSE;
    info->ing_l2_vlan_etag_map_created = FALSE;    
    info->egr_l2_vlan_etag_map_created = FALSE;
    info->egr_vsan_intpri_map_created = FALSE;
    info->egr_vft_pri_map_created = FALSE;
    info->egr_vft_fields_created = FALSE;
    info->egr_mpls_combo_map_created = FALSE;
    info->dscp_table_created = FALSE;
    info->egr_dscp_table_created = FALSE;
    info->prot_pkt_ctrl_created = FALSE;
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        info->ing_l3_nh_attribute_created = FALSE;
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        info->l3_iif_profile_created = FALSE;
        info->l3_ip4_options_profile_created = FALSE;
        info->fc_map_profile_created = FALSE;
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (soc_feature(unit, soc_feature_vfi_profile)) {
        info->vfi_profile_created = FALSE;
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    info->ing_queue_offset_map_created = FALSE;
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    info->vlan_protocol_data_profile_created = FALSE;
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_SABER2_SUPPORT
    if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
        info->service_pri_map_created = FALSE;
    }
#endif /* BCM_SABER2_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    info->ing_vlan_vfi_mbrship_profile_created = FALSE;
    info->egr_vlan_vfi_mbrship_profile_created = FALSE;
    info->misc_port_profile_created = FALSE;
#endif

#ifdef BCM_TRIDENT3_SUPPORT
    info->egr_vlan_vfi_untag_profile_created = FALSE;
#endif

    info->lport_ind_profile_created = FALSE;
    info->rtag7_ind_profile_created = FALSE;
    info->egr_lport_profile_created = FALSE;
    info->egr_qos_profile_created = FALSE;
#ifdef BCM_HURRICANE3_SUPPORT
    info->egr_header_encap_data_created = FALSE;
    info->custom_header_match_created = FALSE;
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    info->ing_tunnel_term_map_created = FALSE;
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
    defined(BCM_TRIDENT3_SUPPORT)
    info->ing_exp_to_ip_ecn_map_created = FALSE;
    info->egr_ip_ecn_to_exp_map_created = FALSE;
    info->egr_int_cn_to_exp_map_created = FALSE;
#endif /* BCM_TOMAHAWK2_SUPPORT or BCM_TOMAHAWK3_SUPPORT */

    info->initialized = FALSE;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_common_init
 * Purpose:
 *      Internal function for initializing common resource management
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */
int _bcm_common_init(int unit)
{
    int num_station = 0, rv = BCM_E_NONE;
    soc_mem_t mem;
    soc_mem_t mems[10];
    int mem_words[10];
    int index_min[10];
    int index_max[10];
    int mems_cnt;
    int entry_words, alloc_size;
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    fp_i2e_classid_select_entry_t fp_ent;
    fp_hg_classid_select_entry_t hg_ent;
    void *entries[2];
    uint32 base_id;
#endif
#ifdef BCM_RIOT_SUPPORT
    soc_mem_t mem2;
#endif
#ifdef BCM_SABER2_SUPPORT
    int mem_index;
    soc_mem_t
    _bcm_sb2_svc_pri_map_profile_mem_index_to_mem[_BCM_SB2_OAM_PRI_MAP_TABLE_MAX] =
    {
            ING_SERVICE_PRI_MAP_0m,
            ING_SERVICE_PRI_MAP_1m,
            ING_SERVICE_PRI_MAP_2m,

            EGR_SERVICE_PRI_MAP_0m,
            EGR_SERVICE_PRI_MAP_1m,
            EGR_SERVICE_PRI_MAP_2m
    };
#endif

    if (info->initialized) {
        BCM_IF_ERROR_RETURN(_bcm_common_cleanup(unit));
    }

    /* Create mutex */
    if (NULL == _common_mutex[unit]) {
        _common_mutex[unit] = sal_mutex_create("common mutex");
        if (_common_mutex[unit] == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }
    }

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_src_modid_base_index)) {
        rv = bcmi_esw_src_modid_base_index_bk_init(unit);
        if (BCM_FAILURE(rv)) {      
            _bcm_common_free_resource(unit, info);
            return rv;
        }
    }

#endif /* BCM_TRIDENT_SUPPORT || BCM_HURRICANE3_SUPPORT */

    if (SAL_BOOT_SIMULATION &&
        soc_property_get(unit, "skip_common_init", 0)) {
        info->initialized = TRUE;
        return BCM_E_NONE;
    }

    /* MPLS station hash */
    if (SOC_MEM_IS_VALID(unit, MPLS_STATION_TCAMm)) {
        num_station = soc_mem_index_count(unit, MPLS_STATION_TCAMm);
        if (NULL == info->mpls_station_hash) {
            info->mpls_station_hash = sal_alloc(sizeof(uint16) *
                                        num_station, "mpls station hash");
            if (info->mpls_station_hash == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            sal_memset(info->mpls_station_hash, 0, 
                sizeof(uint16) * num_station);
        }
    } else if (SOC_MEM_IS_VALID(unit, MY_STATION_TCAMm)) {
        num_station = soc_mem_index_count(unit, MY_STATION_TCAMm);
        if (SOC_MEM_IS_VALID(unit, MY_STATION_TCAMm)) {
            alloc_size = sizeof(my_station_tcam_entry_t) * num_station;
            if (NULL == info->my_station_shadow) {
                info->my_station_shadow =
                    sal_alloc(alloc_size, "my station shadow");
                if (info->my_station_shadow == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                sal_memset(info->my_station_shadow, 0, alloc_size);
            }
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
            alloc_size = sizeof(my_station_shadow_mask_t);
            if (NULL == info->my_station_shadow_mask) {
                info->my_station_shadow_mask =
                    sal_alloc(alloc_size, "my station shadow mask");
                if (info->my_station_shadow_mask == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                sal_memset(info->my_station_shadow_mask, 0, alloc_size);
            }
#endif
            /* L3 flags */
            if (SOC_IS_TRIDENT3X(unit)) {
                mem = MY_STATION_PROFILE_1m;
            } else {
                mem = MY_STATION_TCAMm;
            }

            if (NULL == info->my_station_l3_mask) {
                info->my_station_l3_mask =
                    sal_alloc(soc_mem_entry_words(unit, mem) * 4, "l3_mask");
                if (info->my_station_l3_mask == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                sal_memset(info->my_station_l3_mask, 0,
                           soc_mem_entry_words(unit, mem) * 4);
            }

            soc_mem_field32_set(unit, mem, info->my_station_l3_mask,
                                IPV4_TERMINATION_ALLOWEDf, 1);
            soc_mem_field32_set(unit, mem, info->my_station_l3_mask,
                                IPV6_TERMINATION_ALLOWEDf, 1);
            soc_mem_field32_set(unit, mem, info->my_station_l3_mask,
                                ARP_RARP_TERMINATION_ALLOWEDf, 1);
            /* OAM flags */
            if (SOC_IS_KATANA(unit) || SOC_IS_TRIDENT3X(unit)) {
                if (soc_mem_field_valid(unit,mem, OAM_TERMINATION_ALLOWEDf)) {
                    soc_mem_field32_set(unit, mem, info->my_station_l3_mask,
                                        OAM_TERMINATION_ALLOWEDf, 1);
                }
            }

            if (NULL == info->my_station_tunnel_mask) {
                info->my_station_tunnel_mask =
                    sal_alloc(soc_mem_entry_words(unit, mem) * 4, "l3_mask");
                if (info->my_station_tunnel_mask == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                sal_memset(info->my_station_tunnel_mask, 0,
                           soc_mem_entry_words(unit, mem) * 4);
            }

            /* Tunnel flags */
            if (soc_mem_field_valid(unit, mem, MIM_TERMINATION_ALLOWEDf)) {
                soc_mem_field32_set(unit, mem, info->my_station_tunnel_mask,
                                    MIM_TERMINATION_ALLOWEDf, 1);
            }
            if (soc_mem_field_valid(unit, mem, MPLS_TERMINATION_ALLOWEDf)) {
                soc_mem_field32_set(unit, mem, info->my_station_tunnel_mask,
                                    MPLS_TERMINATION_ALLOWEDf, 1);
            }
            if (soc_mem_field_valid(unit, mem, TRILL_TERMINATION_ALLOWEDf)) {
                soc_mem_field32_set(unit, mem, info->my_station_tunnel_mask,
                                    TRILL_TERMINATION_ALLOWEDf, 1);
            }
        }

#ifdef BCM_RIOT_SUPPORT
        if (soc_feature(unit, soc_feature_riot) || SOC_IS_TRIDENT3X(unit)) {
            if (SOC_IS_TRIDENT3X(unit)) {
                mem2 = MY_STATION_PROFILE_2m;
            } else {
                mem2 = MY_STATION_TCAM_2m;
            }


            num_station = soc_mem_index_count(unit, MY_STATION_TCAM_2m);
            alloc_size = sizeof(my_station_tcam_2_entry_t) * num_station;
            if (NULL == info->my_station2_shadow) {
                info->my_station2_shadow =
                    sal_alloc(alloc_size, "my_station2 shadow");
                if (info->my_station2_shadow == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                sal_memset(info->my_station2_shadow, 0, alloc_size);
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            alloc_size = sizeof(my_station_shadow_mask_t);
            if (NULL == info->my_station2_shadow_mask) {
                info->my_station2_shadow_mask =
                    sal_alloc(alloc_size, "my station2 shadow mask");
                if (info->my_station2_shadow_mask == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                sal_memset(info->my_station2_shadow_mask, 0, alloc_size);
            }
#endif
            /* L3 flags */
            soc_mem_field32_set(unit, mem2,
                                &info->my_station2_l3_mask,
                                IPV4_TERMINATION_ALLOWEDf, 1);
            soc_mem_field32_set(unit, mem2,
                                &info->my_station2_l3_mask,
                                IPV6_TERMINATION_ALLOWEDf, 1);
            soc_mem_field32_set(unit, mem2,
                                &info->my_station2_l3_mask,
                                ARP_RARP_TERMINATION_ALLOWEDf, 1);
            /* Tunnel flags */
            soc_mem_field32_set(unit, mem2,
                                &info->my_station2_tunnel_mask,
                                MIM_TERMINATION_ALLOWEDf, 1);
            soc_mem_field32_set(unit, mem2,
                                &info->my_station2_tunnel_mask,
                                MPLS_TERMINATION_ALLOWEDf, 1);
            if (soc_mem_field_valid(unit, mem2, 
                                    TRILL_TERMINATION_ALLOWEDf)) {
                soc_mem_field32_set(unit, mem2,
                                &info->my_station2_tunnel_mask,
                                TRILL_TERMINATION_ALLOWEDf, 1);
            }
        }
#endif /* BCM_RIOT_SUPPORT */


#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        /* Initialize memory accelation structures */
        rv = _bcm_trx_mysta_memacc_init(unit);    
        if (BCM_FAILURE(rv)) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }
#endif
    }

    /* Create profile table cache (or re-init if it already exists) */
    mem = EGR_MAC_DA_PROFILEm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->mac_da_profile) {
            info->mac_da_profile = sal_alloc(sizeof(soc_profile_mem_t),
                    "MAC DA Profile Mem");
            if (info->mac_da_profile == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->mac_da_profile);

            entry_words = sizeof(egr_mac_da_profile_entry_t) / sizeof(uint32);
            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                    info->mac_da_profile);
            info->mac_da_profile_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }

    if(soc_feature(unit, soc_feature_separate_ing_lport_rtag7_profile)) {

        /* Create profile table cache (or re-init if it already exists) */
        /*
         * The LPORT profile table is composed of these memory tables:
         *     LPORT_TAB
         */
        mem = LPORT_TABm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->lport_ind_profile ) {
                info->lport_ind_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                                "LPORT_PROFILE_TABLE Mem");
                if (info->lport_ind_profile == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->lport_ind_profile);

                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(lport_tab_entry_t) / sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, NULL,
                                                  mems_cnt, info->lport_ind_profile);

                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }

                info->lport_ind_profile_created = TRUE;

                if (SOC_WARM_BOOT(unit)) {
                    /*
                     * Set at least one reference count for default entry.
                     *
                     * Other modules that make a reference to the LPORT table are
                     * responsible for updating the LPORT reference count.
                     */
                    rv = _bcm_lport_ind_profile_mem_reference
                        (unit,
                         _bcm_trx_lport_tab_default_entry_index_get(unit), 1); 
                } else {
                    /* coverity[stack_use_callee] */
                    /* coverity[stack_use_overflow] */
                    rv = _bcm_trx_lport_tab_default_entry_add(unit,
                                                              info->lport_ind_profile);
                }
                if (BCM_FAILURE(rv)) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }

        mem = RTAG7_PORT_BASED_HASHm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->rtag7_ind_profile) {
                info->rtag7_ind_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                                "RTAG7_PROFILE_TABLE Mem");
                if (info->rtag7_ind_profile == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->rtag7_ind_profile);

                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(rtag7_port_based_hash_entry_t) / sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                /* Reserving the last set of entries for Front-Panel ports */
                /* Not sure why TD2PLUS reserves the RTAG7_PORT_BASED_HASH indices,
                   TD3 don't seem to be necessary any index should work for FP ports*/
                if (!SOC_IS_TRIDENT3X(unit)) {
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem) -
                                          soc_mem_index_max(unit, PORT_TABm);
                }
                mems_cnt++;

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, NULL,
                                                  mems_cnt, info->rtag7_ind_profile);

                if (BCM_FAILURE(rv)) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }

                info->rtag7_ind_profile_created = TRUE;

                if (SOC_WARM_BOOT(unit)) {
                    /*
                     * Set at least one reference count for default entry.
 */
                    rv = _bcm_rtag7_ind_profile_mem_reference
                        (unit,
                         _bcm_trx_rtag7_tab_default_entry_index_get(unit), 1); 
                } else {
                    /* coverity[stack_use_callee] */
                    /* coverity[stack_use_overflow] */
                    rv = _bcm_trx_rtag7_tab_default_entry_add(unit,
                                                              info->rtag7_ind_profile);
                }
            }
        }
    } else {

        /* Create profile table cache (or re-init if it already exists) */
        /*
         * The LPORT profile table is composed of these memory tables:
         *     LPORT_TAB
         *     RTAG7_PORT_BASED_HASH (when available)
         *
         * The first set of entries in the RTAG7 table is used by the
         * LPORT profile table (when RTAG7 is larger than LPORT).
         */
        mem = LPORT_TABm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->lport_profile && 
                soc_feature(unit, soc_feature_lport_tab_profile)) {
                info->lport_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                                "LPORT_PROFILE_TABLE Mem");
                if (info->lport_profile == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->lport_profile);

                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(lport_tab_entry_t) / sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                /* Tomahawk 3 split this memory into per pipe, so need to
                 * access differently if the memory is unique */
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    /* Create one logical table for all of the unique per pipe
                     * memories */
                    index_max[mems_cnt] =
                        ((soc_mem_index_max(unit, mem) + 1) * _TH3_PIPES_PER_DEV) - 1;
                } else
#endif
                {
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                }
                mems_cnt++;

                mem = RTAG7_PORT_BASED_HASHm;
                /* For TD2Plus, RTA7_PORT_PROFILE_INDEX is derived
                 * from L/PORT_PROFILE_TABLE__RTAG7_PORT_PROFILE_INDEX
                 * so Allocate only available entries (i.e) LPORT 
                 * entries > RTAG7_PORT_PROFILE_TABLE entries. 
                 */
                if (SOC_MEM_IS_VALID(unit, mem) && 
                    (!SOC_IS_TRIDENT2PLUS(unit)) &&
                    (!SOC_IS_TRIDENT3X(unit)) &&
                    (!SOC_IS_TD2P_TT2P(unit))) {
                    int lport_index = 0;

                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] = sizeof(rtag7_port_based_hash_entry_t) /
                        sizeof(uint32);
                    index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem);

                    /* Make sure RTAG7 table is at least as large as LPORT */
                    if ((index_max[mems_cnt] - index_min[mems_cnt]) <
                        (index_max[lport_index] - index_min[lport_index])) { 
                        _bcm_common_free_resource(unit, info);
                        return BCM_E_INTERNAL;
                    }
                    /* Use size of LPORT table */
                    index_max[mems_cnt] = index_min[mems_cnt] +
                        index_max[lport_index] - index_min[lport_index];
                    
                    mems_cnt++;
                }

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, NULL,
                                                  mems_cnt, info->lport_profile);

                info->lport_profile_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }

                if (SOC_WARM_BOOT(unit)) {
                    /*
                     * Set at least one reference count for default entry.
                     *
                     * Other modules that make a reference to the LPORT table are
                     * responsible for updating the LPORT reference count.
                     */
                    rv = _bcm_lport_profile_mem_reference
                        (unit,
                         _bcm_trx_lport_tab_default_entry_index_get(unit), 1);
                } else {
                    /* coverity[stack_use_callee] */
                    /* coverity[stack_use_overflow] */
                    rv = _bcm_trx_lport_tab_default_entry_add(unit,
                                                              info->lport_profile);
                }
                if (BCM_FAILURE(rv)) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }
    }

    if (soc_feature(unit, soc_feature_egr_lport_tab_profile)) {
        int amend = soc_mem_index_max(unit, EGR_PORTm);
        /* Create profile table cache (or re-init if it already exists) */
        /*
         * The LPORT profile table is composed of these memory tables:
         *     EGR_LPORT_PROFILE
         *     EGR_VLAN_CONTROL_1
         *     EGR_VLAN_CONTROL_2
         *     EGR_VLAN_CONTROL_3
         *     EGR_IPMC_CFG2
         *     EGR_MTU
         *     EGR_PORT_1
         *     EGR_COUNTER_CONTROL
         *     EGR_SHAPING_CONTROL
         *     
         * The first set of entries in the RTAG7 table is used by the
         * LPORT profile table (when RTAG7 is larger than LPORT).
         */
        mem = EGR_LPORT_PROFILEm;
        if (soc_feature(unit, soc_feature_egr_all_profile)) {
            amend = 0;
        }
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->egr_lport_profile) {

                if (!SOC_WARM_BOOT(unit) &&
                    soc_feature(unit, soc_feature_egr_all_profile)) {
                    soc_port_t port;
                    PBMP_ALL_ITER(unit, port) {
                        rv = soc_mem_field32_modify(unit, EGR_PORTm, port,
                                EGR_LPORT_PROFILE_IDXf, 0);
                        if (SOC_FAILURE(rv)) {
                            _bcm_common_free_resource(unit, info);
                            return rv;
                        }
                    }
                }

                info->egr_lport_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                                "EGR_LPORT_PROFILE_TABLE Mem");
                if (info->egr_lport_profile == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->egr_lport_profile);

                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(egr_lport_profile_entry_t) / sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);

                mems_cnt++;

                mem = EGR_VLAN_CONTROL_1m;
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] = sizeof(egr_vlan_control_1_entry_t) /
                        sizeof(uint32);

                    /* The first */
                    index_min[mems_cnt] = soc_mem_index_min(unit, mem) + amend;
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem);

                    mems_cnt++;
                }

                mem = EGR_VLAN_CONTROL_2m;
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] = sizeof(egr_vlan_control_2_entry_t) /
                        sizeof(uint32);
                    index_min[mems_cnt] = soc_mem_index_min(unit, mem) + amend;
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem);

                    mems_cnt++;
                }

                mem = EGR_VLAN_CONTROL_3m;
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] = sizeof(egr_vlan_control_3_entry_t) /
                        sizeof(uint32);
                    index_min[mems_cnt] = soc_mem_index_min(unit, mem) + amend;
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem);

                    mems_cnt++;
                }

                mem = EGR_IPMC_CFG2m;
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] = sizeof(egr_ipmc_cfg2_entry_t) /
                        sizeof(uint32);
                    index_min[mems_cnt] = soc_mem_index_min(unit, mem) + amend;
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem);

                    mems_cnt++;
                }

                mem = EGR_MTUm;
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] = sizeof(egr_mtu_entry_t) /
                        sizeof(uint32);
                    index_min[mems_cnt] = soc_mem_index_min(unit, mem) + amend;
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem);

                    mems_cnt++;
                }

                mem = EGR_PORT_1m;
                if (SOC_MEM_IS_VALID(unit, mem) &&
                    (SOC_SWITCH_BYPASS_MODE(unit) != SOC_SWITCH_BYPASS_MODE_L3_AND_FP)) {
                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] = sizeof(egr_port_1_entry_t) /
                        sizeof(uint32);
                    index_min[mems_cnt] = soc_mem_index_min(unit, mem) + amend;
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem);

                    mems_cnt++;
                }

                if (soc_feature(unit, soc_feature_egr_all_profile) == 0) {
                    mem = EGR_COUNTER_CONTROLm;
                    if (SOC_MEM_IS_VALID(unit, mem)) {
                        mems[mems_cnt] = mem;
                        mem_words[mems_cnt] = sizeof(egr_counter_control_entry_t) /
                            sizeof(uint32);
                        index_min[mems_cnt] = soc_mem_index_min(unit, mem) + amend;
                        index_max[mems_cnt] = soc_mem_index_max(unit, mem);

                        mems_cnt++;
                    }

                    mem = EGR_SHAPING_CONTROLm;
                    if (SOC_MEM_IS_VALID(unit, mem)) {
                        mems[mems_cnt] = mem;
                        mem_words[mems_cnt] = sizeof(egr_shaping_control_entry_t) /
                            sizeof(uint32);
                        index_min[mems_cnt] = soc_mem_index_min(unit, mem) + amend;
                        index_max[mems_cnt] = soc_mem_index_max(unit, mem);

                        mems_cnt++;
                    }
                }

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, NULL,
                                                  mems_cnt, info->egr_lport_profile);

                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }

                info->egr_lport_profile_created = TRUE;

                /* coverity[stack_use_callee] */
                /* coverity[stack_use_overflow] */
                rv = _bcm_trx_egr_lport_tab_default_entry_add(unit,
                                                          info->egr_lport_profile);

                if (BCM_FAILURE(rv)) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }
    }
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if(soc_feature(unit, soc_feature_misc_i2e_hgclass_combo_profile)) {

        /* Create profile table cache (or re-init if it already exists) */
        /*
         * The MISC PORT profile table is composed of these memory tables:
         *     FP_I2E_CLASSID_SELECT
         *     FP_HG_CLASSID_SELECT
 */
        mem = FP_I2E_CLASSID_SELECTm;

        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->misc_port_profile) {
                info->misc_port_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                                "MISC_PORT_PROFILE_TABLE Mem");
                if (info->misc_port_profile == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->misc_port_profile);

                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(fp_i2e_classid_select_entry_t) / sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                mem = FP_HG_CLASSID_SELECTm;
                if (SOC_MEM_IS_VALID(unit, mem)) {

                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] = sizeof(fp_hg_classid_select_entry_t) /
                        sizeof(uint32);
                    index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                    mems_cnt++;
                }

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, NULL,
                                                  mems_cnt, info->misc_port_profile);

                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }

                /* Create a default entry */
                sal_memset(&fp_ent, 0, sizeof(fp_i2e_classid_select_entry_t));
                sal_memset(&hg_ent, 0, sizeof(fp_hg_classid_select_entry_t));
                entries[0] = &fp_ent;
                entries[1] = &hg_ent;
                BCM_IF_ERROR_RETURN
                    (_bcm_misc_port_profile_entry_add(unit, entries, 1, &base_id));

                info->misc_port_profile_created = TRUE;
            }
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    /* Create profile table cache (or re-init if it already exists) */
    mem = (SOC_IS_TRIDENT3X(unit))? PHB_MAPPING_TBL_1m : ING_PRI_CNG_MAPm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->ing_pri_cng_map) {
            info->ing_pri_cng_map = sal_alloc(sizeof(soc_profile_mem_t),
                                              "ING_PRI_CNG_MAP Profile Mem");
            if (info->ing_pri_cng_map == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->ing_pri_cng_map);

            mems_cnt = 0;
            mems[mems_cnt] = mem;
            if(SOC_IS_TRIDENT3X(unit)) {
                mem_words[mems_cnt] = sizeof(phb_mapping_tbl_1_entry_t) /
                                      sizeof(uint32);
            } else {
                mem_words[mems_cnt] = sizeof(ing_pri_cng_map_entry_t) /
                                      sizeof(uint32);
            }
            index_min[mems_cnt] = soc_mem_index_min(unit, mem);
            index_max[mems_cnt] = soc_mem_index_max(unit, mem);
            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
                SOC_IS_HURRICANEX(unit) || SOC_IS_TRIUMPH3(unit) ||
                SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
                /* 63 profiles * 16 enitres for tagged packets
                 * entry 1008 to 1070 are 63 profiles for untagged packets */
                index_max[mems_cnt] = 1007;
            } else if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
                /* First 53 chunks are hard-wired to physical ports */
                /* Only the nine remaining are flexible profiles */    
                index_min[mems_cnt] = 848;
                index_max[mems_cnt] = 1007;
            }
            mems_cnt++;

            mem = ING_UNTAGGED_PHBm;
            if (SOC_MEM_IS_VALID(unit, mem)) {
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(ing_untagged_phb_entry_t) /
                    sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;
            }
            rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                              index_min, index_max, NULL,
                                              mems_cnt, info->ing_pri_cng_map);
            info->ing_pri_cng_map_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }

    if (soc_feature(unit, soc_feature_fcoe)) {
        mem = ING_VFT_PRI_MAPm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->ing_vft_pri_map) {
                info->ing_vft_pri_map = sal_alloc(sizeof(soc_profile_mem_t),
                                              "ING_VFT_PRI_MAP Profile Mem");
                if (info->ing_vft_pri_map == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->ing_vft_pri_map);

                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(ing_vft_pri_map_entry_t) /
                    sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                        index_min, index_max, NULL, mems_cnt,
                        info->ing_vft_pri_map);
                info->ing_vft_pri_map_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }
    }

    if (soc_mem_index_max(unit, ING_ETAG_PCP_MAPPINGm) > 0) {

        mem = ING_ETAG_PCP_MAPPINGm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->ing_l2_vlan_etag_map) {
                info->ing_l2_vlan_etag_map = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "ING_ETAG_PCP_MAPPING Profile Mem");
                if (info->ing_l2_vlan_etag_map == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->ing_l2_vlan_etag_map);
    
                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(ing_etag_pcp_mapping_entry_t) /
                    sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, NULL,
                                                  mems_cnt, info->ing_l2_vlan_etag_map);
                info->ing_l2_vlan_etag_map_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }    
    }

    if (soc_feature(unit, soc_feature_fcoe)) {
        mem = EGR_VFT_PRI_MAPm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->egr_vft_pri_map) {
                info->egr_vft_pri_map = sal_alloc(sizeof(soc_profile_mem_t),
                                              "EGR_VFT_PRI_MAP Profile Mem");
                if (info->egr_vft_pri_map == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->egr_vft_pri_map);

                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(egr_vft_pri_map_entry_t) /
                                        sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                        index_min, index_max, NULL, mems_cnt,
                        info->egr_vft_pri_map);
                info->egr_vft_pri_map_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }

        mem = EGR_VSAN_INTPRI_MAPm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->egr_vsan_intpri_map) {
                info->egr_vsan_intpri_map = sal_alloc(sizeof(soc_profile_mem_t),
                                            "EGR_VSAN_INTPRI_MAP Profile Mem");
                if (info->egr_vsan_intpri_map == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->egr_vsan_intpri_map);
                
                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(egr_vsan_intpri_map_entry_t) /
                                            sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                        index_min, index_max, NULL, mems_cnt,
                        info->egr_vsan_intpri_map);
                info->egr_vsan_intpri_map_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }
    }

    if (SOC_IS_TRIDENT3X(unit)) {
        mem = EGR_ZONE_3_DOT1P_MAPPING_TABLE_2m;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->egr_l2_vlan_etag_map) {
                info->egr_l2_vlan_etag_map = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "EGR_ETAG_PCP_MAPPING Profile Mem");
                if (info->egr_l2_vlan_etag_map == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->egr_l2_vlan_etag_map);

                mems_cnt = 0;
                /* outer etag */
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(egr_zone_3_dot1p_mapping_table_2_entry_t) /
                                      sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                /* inner etag */
                mem = EGR_ZONE_1_DOT1P_MAPPING_TABLE_2m;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(egr_zone_1_dot1p_mapping_table_2_entry_t) /
                                      sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, NULL,
                                                  mems_cnt, info->egr_l2_vlan_etag_map);
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
                info->egr_l2_vlan_etag_map_created = TRUE;
            }
        }
    } else {
        mem = EGR_ETAG_PCP_MAPPINGm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->egr_l2_vlan_etag_map) {
                info->egr_l2_vlan_etag_map = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "EGR_ETAG_PCP_MAPPING Profile Mem");
                if (info->egr_l2_vlan_etag_map == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->egr_l2_vlan_etag_map);

                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(egr_etag_pcp_mapping_entry_t) /
                    sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, NULL,
                                                  mems_cnt, info->egr_l2_vlan_etag_map);
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
                info->egr_l2_vlan_etag_map_created = TRUE;
            }
        }
    }

    if (soc_feature(unit, soc_feature_fcoe)) {
        mem = EGR_VFT_FIELDS_PROFILEm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->egr_vft_fields) {
                info->egr_vft_fields = sal_alloc(sizeof(soc_profile_mem_t),
                                            "EGR_VFT_FIELDS_PROFILE Mem");
                if (info->egr_vft_fields == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->egr_vft_fields);

                mems_cnt = 0;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = sizeof(egr_vft_fields_profile_entry_t) /
                                        sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                        index_min, index_max, NULL, mems_cnt,
                        info->egr_vft_fields);
                info->egr_vft_fields_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }
    }

    mem = EGR_MPLS_PRI_MAPPINGm;
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        /* TRIDENT3 use Flex QoS architecture - memory binding and mappings are 
           completely different from old design
         */
        /* Init per SOC control structure */
        bcm_td3_egr_mpls_combo_map_info_init(unit);

        /* Create memory binding */
        rv = bcm_td3_egr_mpls_combo_map_create(unit, 
                                          &info->egr_mpls_combo_map_created,
                                          &info->egr_mpls_combo_map);
       if (rv < 0) {
           _bcm_common_free_resource(unit, info);
           return rv;
       }

       /* Now fill in the defaults MPLS and L2 mapping */
       rv = bcm_td3_egr_mpls_combo_map_default(unit);
       if (rv < 0) {
           _bcm_common_free_resource(unit, info);
           return rv;
       }
    } else
#endif /* End of BCM_TRIDENT3_SUPPORT */

    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->egr_mpls_combo_map) {
            info->egr_mpls_combo_map = sal_alloc(sizeof(soc_profile_mem_t),
                                                 "MPLS Combo Profile Mem");
            if (info->egr_mpls_combo_map == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->egr_mpls_combo_map);

            /* Prevent entries from moving around in the tables */
            info->egr_mpls_combo_map->flags = SOC_PROFILE_MEM_F_NO_SHARE;

            mems_cnt = 0;
            mems[mems_cnt] = mem;
            mem_words[mems_cnt] = sizeof(egr_mpls_pri_mapping_entry_t) /
                sizeof(uint32);
            mems_cnt++;
            if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_1m)) {
                mems[mems_cnt] = EGR_MPLS_EXP_MAPPING_1m;
                mem_words[mems_cnt] = sizeof(egr_mpls_exp_mapping_1_entry_t) /
                    sizeof(uint32);
                mems_cnt++;
            }
            if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_2m)) {
                mems[mems_cnt] = EGR_MPLS_EXP_MAPPING_2m;
                mem_words[mems_cnt] = sizeof(egr_mpls_exp_mapping_2_entry_t) /
                    sizeof(uint32);
                mems_cnt++;
            }
            rv = soc_profile_mem_create(unit, mems, mem_words, mems_cnt,
                                        info->egr_mpls_combo_map);
            info->egr_mpls_combo_map_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
            /* Create a set of default 1-to-1 internal priority to packet priority
             * mapping profiles for reference by egress objects such as egr_vlan_xlate
             * Must create first to get the 0 indexed profile. 
             */
            if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                egr_mpls_pri_mapping_entry_t egr_mpls_pri_map[64];
                egr_mpls_exp_mapping_1_entry_t egr_mpls_exp_map1[64];
                egr_mpls_exp_mapping_2_entry_t egr_mpls_exp_map2[64];
                uint32 index;
                int int_pri;
                int cng;
                void *entries[3];

                mem = EGR_MPLS_PRI_MAPPINGm;
                mems_cnt = 0;
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    sal_memset(egr_mpls_pri_map, 0, sizeof(egr_mpls_pri_map));
                    for (int_pri = 0; int_pri < 16; int_pri++) {
                        for (cng = 0; cng < 4; cng++) {
                            soc_mem_field32_set(unit, mem, 
                                (uint32 *)&egr_mpls_pri_map[(int_pri << 2) | cng],
                                NEW_PRIf, int_pri > 7? 7: int_pri);
                            soc_mem_field32_set(unit, mem, 
                                (uint32 *)&egr_mpls_pri_map[(int_pri << 2) | cng],
                                NEW_CFIf, cng > 1? 1:cng);
                        }
                    }
                    entries[mems_cnt++] = egr_mpls_pri_map;
                }

                mem = EGR_MPLS_EXP_MAPPING_1m;
                sal_memset(egr_mpls_exp_map1, 0, sizeof(egr_mpls_exp_map1));
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    if (!SOC_IS_TRIDENT3X(unit)) {
                        for (int_pri = 0; int_pri < 16; int_pri++) {
                            for (cng = 0; cng < 4; cng++) {
                                soc_mem_field32_set(unit, mem,
                                    (uint32 *)&egr_mpls_exp_map1[(int_pri << 2) | cng],
                                    PRIf, int_pri > 7? 7: int_pri);
                                soc_mem_field32_set(unit, mem,
                                    (uint32 *)&egr_mpls_exp_map1[(int_pri << 2) | cng],
                                    CFIf, cng > 1?1:cng);
                            }
                        }
                    }
                    entries[mems_cnt++] = egr_mpls_exp_map1;
                }

                mem = EGR_MPLS_EXP_MAPPING_2m;
                sal_memset(egr_mpls_exp_map2, 0, sizeof(egr_mpls_exp_map2));
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    if (!SOC_IS_TRIDENT3X(unit)) {
                        for (int_pri = 0; int_pri < 16; int_pri++) {
                            for (cng = 0; cng < 4; cng++) {
                                soc_mem_field32_set(unit, mem,
                                    (uint32 *)&egr_mpls_exp_map2[(int_pri << 2) | cng],
                                    PRIf, int_pri > 7? 7: int_pri);
                                soc_mem_field32_set(unit, mem,
                                    (uint32 *)&egr_mpls_exp_map2[(int_pri << 2) | cng],
                                    CFIf, cng > 1?1:cng);
                            }
                        }
                    }
                    entries[mems_cnt++] = egr_mpls_exp_map2;
                }

                BCM_IF_ERROR_RETURN
                    (_bcm_egr_mpls_combo_map_entry_add(unit, entries, 64,
                                                       &index));
            }
        }
    }

    mem = SOC_IS_TRIDENT3X(unit) ? PHB_MAPPING_TBL_2m : DSCP_TABLEm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->dscp_table) {
            info->dscp_table = sal_alloc(sizeof(soc_profile_mem_t),
                                         "DSCP_TABLE Profile Mem");
            if (info->dscp_table == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->dscp_table);
            if(SOC_IS_TRIDENT3X(unit)) {
                entry_words = sizeof(phb_mapping_tbl_2_entry_t) / sizeof(uint32);
            } else {
                entry_words = sizeof(dscp_table_entry_t) / sizeof(uint32);
            }
            index_min[0] = soc_mem_index_min(unit, mem);
            index_max[0] = soc_mem_index_max(unit, mem);
            mems[0] = mem;
            mem_words[0] = entry_words;
            if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
                /* First 53 chunks are hard-wired to physical ports */
                /* Only the nine remaining are flexible profiles */
                index_min[0] = 3392;
            }
            if (SOC_IS_KATANA(unit) || SOC_IS_TRIUMPH3(unit) ||
                SOC_IS_TRIDENT(unit)) {
                index_min[0] = SOC_PORT_MAX(unit,all) * 64;
            }
            if (SOC_IS_FIREBOLT6(unit)) {
                index_min[0] = 128 * 64;
            }

            rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                              index_min, index_max, NULL, 1,
                                              info->dscp_table);
            info->dscp_table_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }

    if(soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
        /* Create profile table cache (or re-init if it already exists) */
        /*
         * The MISC PORT profile table is composed of these memory tables:
         *     EGR_PRI_CNG_MAP
         *     EGR_DSCP
         * 
         *     !! ToDo: Implementation pending, need to tie this in with the existing 'EGR_DSCP'
         *           profiling, the indices are managed externally to the 'profiling module'. 
 */
        mem = SOC_IS_TRIDENT3X(unit) ? EGR_ZONE_1_DOT1P_MAPPING_TABLE_4m:EGR_PRI_CNG_MAPm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->egr_qos_profile) {
                egr_qos_l2_map_entry_t egr_pri_cng_map[64];
                egr_qos_l3_map_entry_t egr_dscp_table[64];
                soc_field_t mem_field;
                uint32 index;
                int int_pri;
                int cng;
                int dscp;
                void *entries[3];
                int i;
                egr_pri_cng_map_entry_t egr_pri_cng_map_mask;
                egr_dscp_table_entry_t egr_dscp_table_mask;
                egr_zone_1_dot1p_mapping_table_4_entry_t egr_zone_1_dot1p_mapping_table_4_mask;
                egr_zone_4_qos_mapping_table_entry_t egr_zone_4_qos_mapping_table_mask;
                egr_zone_2_qos_mapping_table_entry_t egr_zone_2_qos_mapping_table_mask;

                info->egr_qos_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                                "MISC_PORT_PROFILE_TABLE Mem");
                if (info->egr_qos_profile == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->egr_qos_profile);

                mems_cnt = 0;
                if (SOC_IS_TRIDENT3X(unit)) {
                    mem = EGR_ZONE_1_DOT1P_MAPPING_TABLE_4m;
                    entries[mems_cnt] = &egr_zone_1_dot1p_mapping_table_4_mask;
                    sal_memset(&egr_zone_1_dot1p_mapping_table_4_mask, 0xff,
                               sizeof(egr_zone_1_dot1p_mapping_table_4_mask));
                } else {
                    mem = EGR_PRI_CNG_MAPm;
                    entries[mems_cnt] = &egr_pri_cng_map_mask;
                    sal_memset(&egr_pri_cng_map_mask, 0xff,
                               sizeof(egr_pri_cng_map_mask));
                }

                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = soc_mem_entry_words(unit,mem);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                if (SOC_IS_TRIDENT3X(unit)) {
                    mem = EGR_ZONE_4_QOS_MAPPING_TABLEm;
                    entries[mems_cnt] = &egr_zone_4_qos_mapping_table_mask;
                    sal_memset(&egr_zone_4_qos_mapping_table_mask, 0xff,
                               sizeof(egr_zone_4_qos_mapping_table_mask));
                } else {
                    mem = EGR_DSCP_TABLEm;
                    entries[mems_cnt] = &egr_dscp_table_mask;
                    sal_memset(&egr_dscp_table_mask, 0xff,
                               sizeof(egr_dscp_table_mask));
                }

                if (SOC_MEM_IS_VALID(unit, mem)) {
                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] = soc_mem_entry_words(unit,mem);;
                    index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                    mems_cnt++;
                }

                if (SOC_IS_TRIDENT3X(unit)) {
                    mem = EGR_ZONE_2_QOS_MAPPING_TABLEm;
                    entries[mems_cnt] = &egr_zone_2_qos_mapping_table_mask;
                    sal_memset(&egr_zone_2_qos_mapping_table_mask, 0xff,
                               sizeof(egr_zone_2_qos_mapping_table_mask));
                    if (SOC_MEM_IS_VALID(unit, mem)) {
                        mems[mems_cnt] = mem;
                        mem_words[mems_cnt] = soc_mem_entry_words(unit,mem);;
                        index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                        index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                        mems_cnt++;
                    }
                }

                for (i = 0; i < mems_cnt; i++) {
                    if (SOC_MEM_FIELD_VALID(unit, mems[i], ECCPf)) {
                        soc_mem_field32_set(unit, mems[i], entries[i],
                                            ECCPf, 0);
                    } else if (SOC_MEM_FIELD_VALID(unit, mems[i],
                               EVEN_PARITYf)) {
                        soc_mem_field32_set(unit, mems[i], entries[i],
                                            EVEN_PARITYf, 0);
                    }
                }
                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, entries,
                                                  mems_cnt, info->egr_qos_profile);

                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }

                
                info->egr_qos_profile_created = TRUE;

                /* Create a set of default 1-to-1 internal priority to packet priority
                 * mapping profiles for reference by egress objects
                 * Must create first to get the 0 indexed profile. 
                 */
                if (SOC_IS_TRIDENT3X(unit)) {
                    mem = EGR_ZONE_1_DOT1P_MAPPING_TABLE_4m;
                } else {
                    mem = EGR_PRI_CNG_MAPm;
                }
                mems_cnt = 0;
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    sal_memset(egr_pri_cng_map, 0, sizeof(egr_pri_cng_map));
                    for (int_pri = 0; int_pri < 16; int_pri++) {
                        for (cng = 0; cng < 4; cng++) {
                            soc_mem_field32_set(unit, mem, 
                                (uint32 *)((char *)egr_pri_cng_map + 
                                                   soc_mem_entry_words(unit, mem) * 
                                                   sizeof(uint32) * (int_pri << 2 | cng)),
                                PRIf, int_pri > 7? 7: int_pri);
                            soc_mem_field32_set(unit, mem, 
                                (uint32 *)((char *)egr_pri_cng_map +
                                                   soc_mem_entry_words(unit, mem) *           
                                                   sizeof(uint32) * (int_pri << 2 | cng)),
                                CFIf, cng > 1? 1:cng);
                        }
                    }
                    entries[mems_cnt++] = egr_pri_cng_map;
                }

                if (SOC_IS_TRIDENT3X(unit)) {
                    mem = EGR_ZONE_4_QOS_MAPPING_TABLEm;
                    mem_field = QOSf; 
                } else {
                    mem = EGR_DSCP_TABLEm;
                    mem_field = DSCPf;
                }
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    sal_memset(egr_dscp_table, 0, sizeof(egr_dscp_table));
                    dscp = 0;
                    for (int_pri = 0; int_pri < 16; int_pri++) {
                        for (cng = 0; cng < 4; cng++) {
                            soc_mem_field32_set(unit, mem,
                                (uint32 *)((char *)egr_dscp_table +
                                           soc_mem_entry_words(unit, mem) *
                                           sizeof(uint32) * (int_pri << 2 | cng)),
                                mem_field, (dscp++));
                        }
                    }
                    entries[mems_cnt++] = egr_dscp_table;

                    if (SOC_IS_TRIDENT3X(unit)) {
                        if (SOC_MEM_IS_VALID(unit,
                                      EGR_ZONE_2_QOS_MAPPING_TABLEm)) {
                            /* same setting for EGR_ZONE_2_QOS_MAPPING_TABLEm*/
                            entries[mems_cnt++] = egr_dscp_table;
                        }
                    }
                }

                BCM_IF_ERROR_RETURN 
                    (_bcm_egr_qos_profile_entry_add(unit, entries, 64,
                                                       &index));
            }
        }
    } else {
        mems_cnt = 0;
        if (SOC_IS_TRIDENT3X(unit)) {
            mem = EGR_ZONE_4_QOS_MAPPING_TABLEm;
        } else {
            mem = EGR_DSCP_TABLEm;
        }

        if (SOC_MEM_IS_VALID(unit, mem)) {
            mems[mems_cnt] = mem;
            mem_words[mems_cnt] = soc_mem_entry_words(unit,mem); 
            mems_cnt++;

            /* One dscp profile for EGR_ZONE_2/4_QOS_MAPPING_TABLEs 
             * Zone2 for outer L3 header and Zone4 for inner L3 
             */
            if (SOC_IS_TRIDENT3X(unit)) {
                mem = EGR_ZONE_2_QOS_MAPPING_TABLEm;
                if (SOC_MEM_IS_VALID(unit, mem)) {
                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] = soc_mem_entry_words(unit,mem); 
                    mems_cnt++;
                }
            }

            if (NULL == info->egr_dscp_table) {
                info->egr_dscp_table = sal_alloc(sizeof(soc_profile_mem_t),
                                                 "EGR_DSCP_TABLE Profile Mem");
                if (info->egr_dscp_table == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->egr_dscp_table);

                rv = soc_profile_mem_create(unit, mems, mem_words, mems_cnt,
                                            info->egr_dscp_table);
                info->egr_dscp_table_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }
    }

    if (SOC_REG_INFO(unit, PROTOCOL_PKT_CONTROLr).regtype != soc_portreg &&
        SOC_REG_INFO(unit, IGMP_MLD_PKT_CONTROLr).regtype != soc_portreg) {
        if (info->prot_pkt_ctrl == NULL) {
            soc_reg_t reg[2];
            info->prot_pkt_ctrl = 
                sal_alloc(sizeof(soc_profile_reg_t),
                          "PROTOCOL_PKT_CONTROL Profile Reg");
            if (info->prot_pkt_ctrl == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_reg_t_init(info->prot_pkt_ctrl);

            reg[0] = PROTOCOL_PKT_CONTROLr;
            reg[1] = IGMP_MLD_PKT_CONTROLr;
            rv = soc_profile_reg_create(unit, reg, 2,
                                        info->prot_pkt_ctrl);
            info->prot_pkt_ctrl_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }
#ifdef BCM_TRIDENT_SUPPORT
    mem = ING_OUTER_DOT1P_MAPPING_TABLEm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        uint32 idx, max;
        ing_outer_dot1p_mapping_table_entry_t dot1p[16];
        void *entries[1];
        int base = 0;

        if (NULL == info->ing_outer_dot1p) {
            info->ing_outer_dot1p = sal_alloc(sizeof(soc_profile_mem_t),
                               "ING_OUTER_DOT1P_MAPPING_TABLE Profile Mem");
            if (info->ing_outer_dot1p == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->ing_outer_dot1p);

            entry_words = sizeof(ing_outer_dot1p_mapping_table_entry_t) / 
                          sizeof(uint32);
            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                        info->ing_outer_dot1p);
            info->ing_outer_dot1p_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
        /* Add default profile entry - identity mapping */
        idx = soc_mem_index_min(unit, mem);
        max = 16;
        sal_memset(&dot1p, 0, sizeof(dot1p));
        while (BCM_SUCCESS(rv) && idx < max) {
            soc_mem_field32_set(unit, mem, &dot1p[idx], NEW_CFIf, idx & 1);
            soc_mem_field32_set(unit, mem, &dot1p[idx], NEW_DOT1Pf, 
                                (idx >> 1) & 7);
            idx++;
        }
        entries[0] = &dot1p;
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, ING_OUTER_DOT1P(unit), entries, 16, 
                                 (uint32 *)&base));
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_VALKYRIE2(unit) ||
        SOC_IS_HURRICANEX(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_APOLLO(unit) || SOC_IS_TD_TT(unit) || SOC_IS_KATANAX(unit) ||
        SOC_IS_ENDURO(unit) || SOC_IS_GREYHOUND(unit) || 
        SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            rv = _bcm_td3_port_phb_map_tab1_default_entry_add
                     (unit, info->ing_pri_cng_map);
        } else
#endif
        {
            rv = _bcm_tr2_ing_pri_cng_map_default_entry_add
                     (unit, info->ing_pri_cng_map);
        }
        if (BCM_FAILURE(rv)) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    mem = ING_L3_NEXT_HOP_ATTRIBUTE_1m;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->ing_l3_nh_attribute) {
            info->ing_l3_nh_attribute = sal_alloc(sizeof(soc_profile_mem_t),
                                     "ING_L3_NEXT_HOP_ATTRIBUTE_1 Profile Mem");
            if (info->ing_l3_nh_attribute == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->ing_l3_nh_attribute);

            entry_words = sizeof(ing_l3_next_hop_attribute_1_entry_t) /
                                 sizeof(uint32);

            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                        info->ing_l3_nh_attribute);
            info->ing_l3_nh_attribute_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    mem = L3_IIF_PROFILEm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->l3_iif_profile) {
            void *entries[1];
            iif_profile_entry_t l3_iif_profile;

            info->l3_iif_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                     "L3_IIF_PROFILE Mem");
            if (info->l3_iif_profile == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->l3_iif_profile);

            entry_words = soc_mem_entry_words(unit, mem);

            index_min[0] = soc_mem_index_min(unit, mem);
            index_max[0] = soc_mem_index_max(unit, mem);
            mems[0] = mem;
            mem_words[0] = entry_words;

            entries[0] = &l3_iif_profile;
            sal_memset(&l3_iif_profile, 0xff, sizeof(iif_profile_entry_t));

            if (soc_mem_field_valid(unit, L3_IIF_PROFILEm, ECCPf)) {
                soc_mem_field32_set(unit, L3_IIF_PROFILEm, &l3_iif_profile,
                                    ECCPf, 0);
            } else {
                if (soc_mem_field_valid(unit, L3_IIF_PROFILEm, EVEN_PARITYf)) {
                    soc_mem_field32_set(unit, L3_IIF_PROFILEm, &l3_iif_profile,
                                        EVEN_PARITYf, 0);
                    }
            }

            rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                              index_min, index_max, entries, 1,
                                              info->l3_iif_profile);
            info->l3_iif_profile_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }
    mem = IP_OPTION_CONTROL_PROFILE_TABLEm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->l3_ip4_options_profile) {
            info->l3_ip4_options_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                         "IP_OPTION_CONTROL_PROFILE_TABLEm Profile Mem");
            if (info->l3_ip4_options_profile == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->l3_ip4_options_profile);

            entry_words = 1;
            index_min[0] = soc_mem_index_min(unit, mem);
            index_max[0] = soc_mem_index_max(unit, mem);
            mems[0] = mem;
            mem_words[0] = entry_words;
            rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                              index_min, index_max, NULL, 1,
                                              info->l3_ip4_options_profile);
            info->l3_ip4_options_profile_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }

    if (soc_feature(unit, soc_feature_fcoe)) {
        mem = FC_MAP_PROFILEm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->fc_map_profile) {
                info->fc_map_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                            "FC MAP PROFILE Mem");
                if (info->fc_map_profile == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->fc_map_profile);

                entry_words = 1;
                index_min[0] = soc_mem_index_min(unit, mem);
                index_max[0] = soc_mem_index_max(unit, mem);
                mems[0] = mem;
                mem_words[0] = entry_words;
                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                        index_min, index_max, NULL, 1, info->fc_map_profile);
                info->fc_map_profile_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }
    }

#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    mem = VFI_PROFILEm;
    if (soc_feature(unit, soc_feature_vfi_profile)) {
        uint32 index;
        void *entries[3];
        vfi_profile_entry_t vfi_profile_entry;
        vfi_profile_2_entry_t vfi_profile_2_entry;

        if (NULL == info->vfi_profile) {
            info->vfi_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                          "VFI_PROFILE Mem");
            if (info->vfi_profile == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            /*
             * Mark the profile as created so _bcm_common_free_resource()
             * can free the allocated memory on error.
             */
            info->vfi_profile_created = TRUE;

            soc_profile_mem_t_init(info->vfi_profile);

            mems_cnt = 0;
            mems[mems_cnt] = mem;
            mem_words[mems_cnt] = soc_mem_entry_words(unit, mem);
            index_min[mems_cnt] = soc_mem_index_min(unit, mem);
            index_max[mems_cnt] = soc_mem_index_max(unit, mem);
            mems_cnt++;

            if (soc_feature(unit, soc_feature_vfi_combo_profile)) {
                mem = VFI_PROFILE_2m;
                mems[mems_cnt] = mem;
                mem_words[mems_cnt] = soc_mem_entry_words(unit, mem);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;
            }

            rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                              index_min, index_max, NULL,
                                              mems_cnt,
                                        info->vfi_profile);
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }

        entries[0] = &vfi_profile_entry;
        if (soc_feature(unit, soc_feature_vfi_combo_profile)) {
            entries[1] = &vfi_profile_2_entry;
        }
        sal_memset(&vfi_profile_entry, 0, sizeof(vfi_profile_entry_t));
        sal_memset(&vfi_profile_2_entry, 0, sizeof(vfi_profile_2_entry_t));
        soc_mem_field32_set(unit, VFI_PROFILEm, &vfi_profile_entry, EN_IFILTERf, 1);

        /* Create/Reserve profile 0 for defaults */
        rv = soc_profile_mem_add(unit, info->vfi_profile, entries, 1, &index);
        SOC_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */


   if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        /* Initialize flex stats */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_flex_stat_init(unit, _bcmFlexStatTypeGport));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_flex_stat_init(unit, _bcmFlexStatTypeService));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_flex_stat_init(unit, _bcmFlexStatTypeEgressGport));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_flex_stat_init(unit, _bcmFlexStatTypeEgressService));
    }
   }
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    mem = ING_QUEUE_OFFSET_MAPPING_TABLEm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->offset_map_table) {
            info->offset_map_table = sal_alloc(sizeof(soc_profile_mem_t),
                                         "OFFSET_MAP_TABLE Profile Mem");
            if (info->offset_map_table == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->offset_map_table);

            entry_words = 1;
            index_min[0] = soc_mem_index_min(unit, mem);
            index_max[0] = soc_mem_index_max(unit, mem);
            mems[0] = mem;
            mem_words[0] = entry_words;
            rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                              index_min, index_max, NULL, 1,
                                              info->offset_map_table);
            info->ing_queue_offset_map_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }
#endif /* BCM_KATANA_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if ((SOC_IS_KATANA2(unit) || SOC_IS_TOMAHAWKX(unit)
         || SOC_IS_TRIDENT3X(unit)) &&
        (soc_mem_index_max(unit, VLAN_PROTOCOL_DATAm) > 0)) {
        mem = VLAN_PROTOCOL_DATAm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->vlan_protocol_data_profile) {
                info->vlan_protocol_data_profile =
                    sal_alloc(sizeof(soc_profile_mem_t),
                    "VLAN_PROTOCOL_DATA Profile Mem");
                if (info->vlan_protocol_data_profile == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->vlan_protocol_data_profile);

                entry_words = sizeof(vlan_protocol_data_entry_t) / sizeof(uint32);
                index_min[0] = soc_mem_index_min(unit, mem);
                index_max[0] = soc_mem_index_max(unit, mem);
                mems[0] = mem;
                mem_words[0] = entry_words;
                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, NULL, 1,
                                                  info->vlan_protocol_data_profile);
                info->vlan_protocol_data_profile_created= TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    mem = ING_VLAN_VFI_MEMBERSHIPm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->ing_vlan_vfi_mbrship_profile) {
            info->ing_vlan_vfi_mbrship_profile = 
                sal_alloc(sizeof(soc_profile_mem_t),
                "ING_VLAN_VFI_MEMBERSHIP Profile Mem");

            if (info->ing_vlan_vfi_mbrship_profile == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->ing_vlan_vfi_mbrship_profile);

            mems_cnt = 0;
            mems[mems_cnt] = mem;
            mem_words[mems_cnt] = sizeof(ing_vlan_vfi_membership_entry_t) /
                sizeof(uint32);
            index_min[mems_cnt] = soc_mem_index_min(unit, mem);
            index_max[mems_cnt] = soc_mem_index_max(unit, mem);
            mems_cnt++;

            rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                              index_min, index_max, NULL,
                                              mems_cnt, info->ing_vlan_vfi_mbrship_profile);

            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }

            /*
             * Set one reference count for default entry.
             */
            if (SOC_WARM_BOOT(unit)) {
                rv = _bcm_vlan_vfi_membership_profile_mem_reference
                    (unit,
                     soc_mem_index_min(unit, ING_VLAN_VFI_MEMBERSHIPm), 1, 0); 
                rv = _bcm_vlan_vfi_membership_profile_mem_reference(unit, 1, 1, 0);
            } else {
                /* coverity[stack_use_callee] */
                /* coverity[stack_use_overflow] */
                ing_vlan_vfi_membership_entry_t ing_vlan_vfi_entry;
                void *entries[1];
                uint32 prof_index = 0;
                bcm_pbmp_t pbmp;

                BCM_PBMP_CLEAR(pbmp);
                BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
                BCM_PBMP_REMOVE(pbmp, PBMP_LB(unit));
                BCM_PBMP_REMOVE(pbmp, PBMP_RDB_ALL(unit));
                BCM_PBMP_REMOVE(pbmp, PBMP_MACSEC_ALL(unit));

                sal_memcpy(&ing_vlan_vfi_entry,
                           soc_mem_entry_null(unit, ING_VLAN_VFI_MEMBERSHIPm),
                           (soc_mem_entry_words(unit, ING_VLAN_VFI_MEMBERSHIPm)
                            * sizeof(uint32)));

                entries[0] = &ing_vlan_vfi_entry;
                BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit,
                    info->ing_vlan_vfi_mbrship_profile, entries, 1, &prof_index));

                /* VFIs should have all physical ports in VFI membership tables, by default.
                   When membership checks are disabled on virtual ports, it defaults to
                   checks at the physical ports. Since the membership checks are enabled
                   by default in SDK, we'll have packet drops without programming these. */
                soc_mem_pbmp_field_set(unit, mem,
                    &ing_vlan_vfi_entry, ING_PORT_BITMAPf, &pbmp);
                /* Reserve profile 1 as default VFI membership profile */
                BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit,
                    info->ing_vlan_vfi_mbrship_profile, entries, 1, &prof_index));
            }

            info->ing_vlan_vfi_mbrship_profile_created = TRUE;
        }
    }

    mem = EGR_VLAN_VFI_MEMBERSHIPm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->egr_vlan_vfi_mbrship_profile) {
            info->egr_vlan_vfi_mbrship_profile = 
                sal_alloc(sizeof(soc_profile_mem_t),
                          "EGR_VLAN_VFI_MEMBERSHIP Profile Mem");
            if (info->egr_vlan_vfi_mbrship_profile == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->egr_vlan_vfi_mbrship_profile);

            mems_cnt = 0;
            mems[mems_cnt] = mem;
            mem_words[mems_cnt] = sizeof(egr_vlan_vfi_membership_entry_t) /
                sizeof(uint32);
            index_min[mems_cnt] = soc_mem_index_min(unit, mem);
            index_max[mems_cnt] = soc_mem_index_max(unit, mem);
            mems_cnt++;

            rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                              index_min, index_max, NULL,
                                              mems_cnt, info->egr_vlan_vfi_mbrship_profile);

            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }

            /*
             * Set one reference count for default entry.
             */
            if (SOC_WARM_BOOT(unit)) {
                rv = _bcm_vlan_vfi_membership_profile_mem_reference
                    (unit, soc_mem_index_min(unit, EGR_VLAN_VFI_MEMBERSHIPm), 
                     1, 1); 
                rv = _bcm_vlan_vfi_membership_profile_mem_reference(unit, 1, 1, 1);
            } else {
                /* coverity[stack_use_callee] */
                /* coverity[stack_use_overflow] */
                egr_vlan_vfi_membership_entry_t egr_vlan_vfi_entry;
                void *entries[1];
                uint32 prof_index = 0;
                bcm_pbmp_t pbmp;

                BCM_PBMP_CLEAR(pbmp);
                BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
                BCM_PBMP_REMOVE(pbmp, PBMP_LB(unit));
                BCM_PBMP_REMOVE(pbmp, PBMP_RDB_ALL(unit));
                BCM_PBMP_REMOVE(pbmp, PBMP_MACSEC_ALL(unit));

                sal_memcpy(&egr_vlan_vfi_entry,
                           soc_mem_entry_null(unit, EGR_VLAN_VFI_MEMBERSHIPm),
                           (soc_mem_entry_words(unit, EGR_VLAN_VFI_MEMBERSHIPm)
                            * sizeof(uint32)));

                entries[0] = &egr_vlan_vfi_entry;
                BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit,
                    info->egr_vlan_vfi_mbrship_profile, entries, 1, &prof_index));

                /* VFIs should have all physical ports in VFI membership tables, by default.
                   When membership checks are disabled on virtual ports, it defaults to
                   checks at the physical ports. Since the membership checks are enabled
                   by default in SDK, we'll have packet drops without programming these. */
                soc_mem_pbmp_field_set(unit, mem, &egr_vlan_vfi_entry, PORT_BITMAPf, &pbmp);
                /* Reserve profile 1 as default VFI membership profile */
                BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit,
                    info->egr_vlan_vfi_mbrship_profile, entries, 1, &prof_index));
            }

            info->egr_vlan_vfi_mbrship_profile_created = TRUE;
        }
    }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    mem = EGR_VLAN_VFI_UNTAGm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->egr_vlan_vfi_untag_profile) {
            info->egr_vlan_vfi_untag_profile = 
                sal_alloc(sizeof(soc_profile_mem_t),
                          "EGR_VLAN_VFI_UNTAG Profile Mem");
            if (info->egr_vlan_vfi_untag_profile == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->egr_vlan_vfi_untag_profile);

            mems_cnt = 0;
            mems[mems_cnt] = mem;
            mem_words[mems_cnt] = sizeof(egr_vlan_vfi_untag_entry_t) /
                sizeof(uint32);
            index_min[mems_cnt] = soc_mem_index_min(unit, mem);
            index_max[mems_cnt] = soc_mem_index_max(unit, mem);
            mems_cnt++;

            rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                              index_min, index_max, NULL,
                                              mems_cnt, info->egr_vlan_vfi_untag_profile);

            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }

            /*
             * Set one reference count for default entry.
             */
            if (SOC_WARM_BOOT(unit)) {
                rv = _bcm_vlan_vfi_untag_profile_mem_reference
                    (unit, soc_mem_index_min(unit, mem), 1); 
            } else {
                /* coverity[stack_use_callee] */
                /* coverity[stack_use_overflow] */
                egr_vlan_vfi_untag_entry_t egr_vlan_vfi_entry;
                void *entries[1];
                uint32 prof_index = 0;

                sal_memcpy(&egr_vlan_vfi_entry,
                           soc_mem_entry_null(unit, mem),
                           (soc_mem_entry_words(unit, mem)
                            * sizeof(uint32)));

                entries[0] = &egr_vlan_vfi_entry;
                /* Reserve profile 1 as default VFI membership profile */
                BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit,
                    info->egr_vlan_vfi_untag_profile, entries, 1, &prof_index));
            }

            info->egr_vlan_vfi_untag_profile_created = TRUE;
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
    if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
        if (NULL == info->service_pri_map_table) {
            info->service_pri_map_table = sal_alloc(sizeof(soc_profile_mem_t),
                    "SERVICE_PRI_MAP TABLE Profile Mem");
            if (info->service_pri_map_table == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->service_pri_map_table);

            for(mem_index = 0; mem_index < _BCM_SB2_OAM_PRI_MAP_TABLE_MAX; mem_index++)
            {
                mem_words[mem_index] = (sizeof(ing_service_pri_map_0_entry_t) / sizeof(uint32));
                mems[mem_index] = _bcm_sb2_svc_pri_map_profile_mem_index_to_mem[mem_index];
            }
            rv = soc_profile_mem_create(unit, mems, mem_words,
                    _BCM_SB2_OAM_PRI_MAP_TABLE_MAX,
                    info->service_pri_map_table);
            info->service_pri_map_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }
#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_HURRICANE3_SUPPORT
    /* Create profile table cache (or re-init if it already exists) */
    mem = EGR_HEADER_ENCAP_DATAm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->egr_header_encap_data) {
            info->egr_header_encap_data = sal_alloc(sizeof(soc_profile_mem_t),
                    "EGR_HEADER_ENCAP_DATA Profile Mem");
            if (info->egr_header_encap_data == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->egr_header_encap_data);

            entry_words = sizeof(egr_header_encap_data_entry_t) / sizeof(uint32);
            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                    info->egr_header_encap_data);
            info->egr_header_encap_data_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }

    mem = CUSTOM_HEADER_MATCHm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (NULL == info->custom_header_match) {
            info->custom_header_match = sal_alloc(sizeof(soc_profile_mem_t),
                    "CUSTOM_HEADER_MATCH Profile Mem");
            if (info->custom_header_match == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(info->custom_header_match);

            entry_words = sizeof(custom_header_match_entry_t) / sizeof(uint32);
            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                    info->custom_header_match);
            info->custom_header_match_created = TRUE;
            if (rv < 0) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_ecn_wred)) {
        mem = ING_TUNNEL_ECN_DECAPm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->ing_tunnel_term_map) {
                info->ing_tunnel_term_map = sal_alloc(sizeof(soc_profile_mem_t),
                                                      "ING_TUNNEL_TERM_MAP Profile Mem");
                if (info->ing_tunnel_term_map == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->ing_tunnel_term_map);
                mems_cnt = 0;
                mems[mems_cnt] = mem;
                info->ing_tunnel_term_map->flags = SOC_PROFILE_MEM_F_NO_SHARE;
                mem_words[mems_cnt] =
                    sizeof(ing_tunnel_ecn_decap_entry_t) / sizeof(uint32);
                index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                mems_cnt++;

                if (SOC_MEM_IS_VALID(unit, ING_TUNNEL_ECN_DECAP_2m)) {
                    mem = ING_TUNNEL_ECN_DECAP_2m;
                    mems[mems_cnt] = mem;
                    mem_words[mems_cnt] =
                        sizeof(ing_tunnel_ecn_decap_2_entry_t) /
                        sizeof(uint32);
                    index_min[mems_cnt] = soc_mem_index_min(unit, mem);
                    index_max[mems_cnt] = soc_mem_index_max(unit, mem);
                    mems_cnt++;
                }
                rv = soc_profile_mem_index_create(unit, mems, mem_words,
                                                  index_min, index_max, NULL,
                                                  mems_cnt,
                                            info->ing_tunnel_term_map);
                info->ing_tunnel_term_map_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
    defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if(soc_feature(unit, soc_feature_td3_style_mpls)) {
            mem = ING_EXP_TO_ECN_MAPPING_1m;
        } else
#endif
        {
            mem = ING_EXP_TO_IP_ECN_MAPPINGm;
        }
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->ing_exp_to_ip_ecn_map) {
                info->ing_exp_to_ip_ecn_map = sal_alloc(sizeof(soc_profile_mem_t),
                                                        "ING_EXP_TO_IP_ECN_MAP Profile Mem");
                if (info->ing_exp_to_ip_ecn_map == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->ing_exp_to_ip_ecn_map);
                info->ing_exp_to_ip_ecn_map->flags = SOC_PROFILE_MEM_F_NO_SHARE;
                entry_words = 
                    sizeof(ing_exp_to_ip_ecn_mapping_entry_t) / sizeof(uint32);
                rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                            info->ing_exp_to_ip_ecn_map);
                info->ing_exp_to_ip_ecn_map_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }

#if defined(BCM_TRIDENT3_SUPPORT)
    if(soc_feature(unit, soc_feature_td3_style_mpls)) {
        mem = EGR_PKT_ECN_TO_EXP_MAPPING_1m;
    } else
#endif
    {
        mem = EGR_IP_ECN_TO_EXP_MAPPING_TABLEm;
    }
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->egr_ip_ecn_to_exp_map) {
                info->egr_ip_ecn_to_exp_map = sal_alloc(sizeof(soc_profile_mem_t),
                                                        "EGR_IP_ECN_TO_EXP_MAP Profile Mem");
                if (info->egr_ip_ecn_to_exp_map == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->egr_ip_ecn_to_exp_map);
                info->egr_ip_ecn_to_exp_map->flags = SOC_PROFILE_MEM_F_NO_SHARE;

#if defined(BCM_TRIDENT3_SUPPORT)
                if(soc_feature(unit, soc_feature_td3_style_mpls)) {
                    entry_words =
                        sizeof(egr_pkt_ecn_to_exp_mapping_1_entry_t) / sizeof(uint32);
                } else
#endif
                {
                    entry_words =
                        sizeof(egr_ip_ecn_to_exp_mapping_table_entry_t) / sizeof(uint32);
                }
                rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                            info->egr_ip_ecn_to_exp_map);
                info->egr_ip_ecn_to_exp_map_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }  
        mem = EGR_INT_CN_TO_EXP_MAPPING_TABLEm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (NULL == info->egr_int_cn_to_exp_map) {
                info->egr_int_cn_to_exp_map = sal_alloc(sizeof(soc_profile_mem_t),
                                                        "EGR_INT_CN_TO_EXP_MAP Profile Mem");
                if (info->egr_int_cn_to_exp_map == NULL) {
                    _bcm_common_free_resource(unit, info);
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(info->egr_int_cn_to_exp_map);
                info->egr_int_cn_to_exp_map->flags = SOC_PROFILE_MEM_F_NO_SHARE;
                entry_words = 
                    sizeof(egr_int_cn_to_exp_mapping_table_entry_t) / sizeof(uint32);
                rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                            info->egr_int_cn_to_exp_map);
                info->egr_int_cn_to_exp_map_created = TRUE;
                if (rv < 0) {
                    _bcm_common_free_resource(unit, info);
                    return rv;
                }
            }
        }                
    }
#endif /* BCM_TOMAHAWK2_SUPPORT or BCM_TOMAHAWK3_SUPPORT */

#ifdef BCM_FLOWTRACKER_SUPPORT
    if(soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {

        /* Memory for the Age out profile. */
        mem = BSC_KG_AGE_OUT_PROFILEm;

        /* Set the Age out profile entry. */
        FT_AGE_OUT(unit) = sal_alloc(sizeof(soc_profile_mem_t), "Age Out Profile");

        if (FT_AGE_OUT(unit) == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }

        soc_profile_mem_t_init(FT_AGE_OUT(unit));
        entry_words = sizeof(bsc_kg_age_out_profile_entry_t) / sizeof(uint32);

        rv = soc_profile_mem_create(unit, &mem, &entry_words, 1, FT_AGE_OUT(unit));

        /* Clear the memory allocated for profile above. */
        if (BCM_FAILURE(rv)) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }


        /* Memory for the Flow limit profile. */
        mem = BSC_KG_FLOW_EXCEED_PROFILEm;

        /* Set the Age out profile entry. */
        FT_FLOW_LIMIT(unit) = sal_alloc(sizeof(soc_profile_mem_t), "Flow Limit Profile");

        if (FT_FLOW_LIMIT(unit) == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }

        soc_profile_mem_t_init(FT_FLOW_LIMIT(unit));
        entry_words = sizeof(bsc_kg_flow_exceed_profile_entry_t) / sizeof(uint32);

        rv = soc_profile_mem_create(unit, &mem, &entry_words, 1, FT_FLOW_LIMIT(unit));

        /* Clear the memory allocated for profile above. */
        if (BCM_FAILURE(rv)) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }

        mem = BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILEm;

        /* Set the Collector copy profile entry. */
        FT_COLLECTOR_COPY(unit) = sal_alloc(sizeof(soc_profile_mem_t), "collector copy Profile");

        if (FT_COLLECTOR_COPY(unit) == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }

        soc_profile_mem_t_init(FT_COLLECTOR_COPY(unit));
        entry_words = sizeof(bsc_dg_group_copy_to_collector_profile_entry_t) / sizeof(uint32);

        rv = soc_profile_mem_create(unit, &mem, &entry_words, 1, FT_COLLECTOR_COPY(unit));

        /* Clear the memory allocated for profile above. */
        if (BCM_FAILURE(rv)) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

            mem = BSC_DT_ALU_MASK_PROFILEm;

            /* Set the ALU Mask profile entry. */
            FT_ALU_MASK_PROFILE(unit) = 
               sal_alloc(sizeof(soc_profile_mem_t), "alu mask profile");

            if (FT_ALU_MASK_PROFILE(unit) == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }

            soc_profile_mem_t_init(FT_ALU_MASK_PROFILE(unit));
            entry_words = 
               sizeof(bsc_dt_alu_mask_profile_entry_t) / sizeof(uint32);

            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1, 
                                                     FT_ALU_MASK_PROFILE(unit));

            if (BCM_FAILURE(rv)) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
#endif

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {

            mem = BSC_DT_AGG_ALU_MASK_PROFILEm;

            /* Set the ALU Mask profile entry. */
            FT_AGG_ALU_MASK_PROFILE(unit) =
               sal_alloc(sizeof(soc_profile_mem_t), "alu mask profile");

            if (FT_AGG_ALU_MASK_PROFILE(unit) == NULL) {
                _bcm_common_free_resource(unit, info);
                return BCM_E_MEMORY;
            }

            soc_profile_mem_t_init(FT_AGG_ALU_MASK_PROFILE(unit));
            entry_words =
               sizeof(bsc_dt_alu_mask_profile_entry_t) / sizeof(uint32);

            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                               FT_AGG_ALU_MASK_PROFILE(unit));

            if (BCM_FAILURE(rv)) {
                _bcm_common_free_resource(unit, info);
                return rv;
            }
        }
#endif

        /* Set the PDD profile entry. */
        FT_PDD_PROFILE(unit) = 
                            sal_alloc(sizeof(soc_profile_mem_t), "pdd Profile");

        if (FT_PDD_PROFILE(unit) == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }

        soc_profile_mem_t_init(FT_PDD_PROFILE(unit));

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

            mems[0] = BSD_POLICY_ACTION_PROFILE_0m;
            mems[1] = BSD_POLICY_ACTION_PROFILE_1m;
            mems[2] = BSD_POLICY_ACTION_PROFILE_2m;

            mem_words[0] = 
                   sizeof(bsd_policy_action_profile_0_entry_t) / sizeof(uint32);
            mem_words[1] =
                   sizeof(bsd_policy_action_profile_1_entry_t) / sizeof(uint32);
            mem_words[2] =
                   sizeof(bsd_policy_action_profile_2_entry_t) / sizeof(uint32);

            rv = soc_profile_mem_create(unit, mems, mem_words, 3, 
                                                          FT_PDD_PROFILE(unit));
        } else
#endif
        {
            mem = BSD_POLICY_ACTION_PROFILEm;
            entry_words = 
                     sizeof(bsd_policy_action_profile_entry_t) / sizeof(uint32);

            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                                          FT_PDD_PROFILE(unit));
        }

        /* Clear the memory allocated for profile above. */
        if (BCM_FAILURE(rv)) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }
        mem = BSC_DG_GROUP_METER_PROFILEm;

        /* Set the Meter profile entry. */
        FT_METER_PROFILE(unit) = sal_alloc(sizeof(soc_profile_mem_t), "FT meter Profile");

        if (FT_METER_PROFILE(unit) == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }

        soc_profile_mem_t_init(FT_METER_PROFILE(unit));
        entry_words = sizeof(bsc_dg_group_meter_profile_entry_t) / sizeof(uint32);

        rv = soc_profile_mem_create(unit, &mem, &entry_words, 1, FT_METER_PROFILE(unit));

        /* Clear the memory allocated for profile above. */
        if (BCM_FAILURE(rv)) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }

        /* Set the Timestamp profile entry. */
        mem = BSC_DG_GROUP_TIMESTAMP_PROFILEm;
        FT_TS_PROFILE(unit) = sal_alloc(sizeof(soc_profile_mem_t), "FT timestamp Profile");

        if (FT_TS_PROFILE(unit) == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }

        soc_profile_mem_t_init(FT_TS_PROFILE(unit));
        entry_words = sizeof(bsc_dg_group_timestamp_profile_entry_t) / sizeof(uint32);

        rv = soc_profile_mem_create(unit, &mem, &entry_words, 1, FT_TS_PROFILE(unit));

        /* Clear the memory allocated for profile above. */
        if (BCM_FAILURE(rv)) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }
    }
#endif /* BCM_FLOWTRACKER_SUPPORT */

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        /* Set the Timestamp profile entry. */
        mem = ALLOWED_SUBPORT_PROFILEm;
        SUBPORT_TUNNEL_PBMP(unit) = sal_alloc(sizeof(soc_profile_mem_t), "FT timestamp Profile");

        if (SUBPORT_TUNNEL_PBMP(unit) == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }

        soc_profile_mem_t_init(SUBPORT_TUNNEL_PBMP(unit));
        entry_words = sizeof(allowed_subport_profile_entry_t) / sizeof(uint32);

        rv = soc_profile_mem_create(unit, &mem, &entry_words, 1, SUBPORT_TUNNEL_PBMP(unit));

        if (BCM_FAILURE(rv)) {
            /* Clear the memory allocated for profile above. */
            _bcm_common_free_resource(unit, info);

            return rv;
        }
        /* Set the reference on index 0 to make it a default entry.
        (void)bcmi_subport_tunnel_pbmp_profile_refcount_set(unit, 0);*/

    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */


#if defined(BCM_TRIDENT3_SUPPORT)
    mem = PKT_FLOW_VRF_STRENGTH_PROFILE_1_Am;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        pkt_flow_vrf_strength_profile_1_a_entry_t vrf_strength_profile_a_ent;
        sal_memset(&vrf_strength_profile_a_ent, 0,
                   sizeof(pkt_flow_vrf_strength_profile_1_a_entry_t));
        soc_mem_field32_set(
            unit, mem, &vrf_strength_profile_a_ent, VFP_LO_FOR_TUNNEL_TERM_FLOWf,
            VFP_LO_FOR_TUNNEL_TERM_FLOW_STRENGTH);
        soc_mem_field32_set(
            unit, mem, &vrf_strength_profile_a_ent,
            VFP_LO_FOR_NON_TUNNEL_TERM_FLOWf,
            VFP_LO_FOR_NON_TUNNEL_TERM_FLOW_STRENGTH);
        soc_mem_field32_set(
            unit, mem, &vrf_strength_profile_a_ent, VFP_HI_FOR_TUNNEL_TERM_FLOWf,
            VFP_HI_FOR_TUNNEL_TERM_FLOW_STRENGTH);
        soc_mem_field32_set(
            unit, mem, &vrf_strength_profile_a_ent,
            VFP_HI_FOR_NON_TUNNEL_TERM_FLOWf,
            VFP_HI_FOR_NON_TUNNEL_TERM_FLOW_STRENGTH);
        soc_mem_field32_set(
            unit, mem, &vrf_strength_profile_a_ent, SGPP_FOR_TUNNEL_TERM_FLOWf,
            SGPP_FOR_TUNNEL_TERM_FLOW_STRENGTH);
        soc_mem_field32_set(
            unit, mem, &vrf_strength_profile_a_ent,
            SGPP_FOR_NON_TUNNEL_TERM_FLOWf,
            SGPP_FOR_NON_TUNNEL_TERM_FLOW_STRENGTH);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 1,
                            &vrf_strength_profile_a_ent));
    }
    mem = PKT_FLOW_VRF_STRENGTH_PROFILE_1_Bm;
    if (SOC_MEM_IS_VALID(unit, mem)) {
        pkt_flow_vrf_strength_profile_1_b_entry_t vrf_strength_profile_b_ent;
        sal_memset(&vrf_strength_profile_b_ent, 0,
                   sizeof(pkt_flow_vrf_strength_profile_1_b_entry_t));
        soc_mem_field32_set(
            unit, mem, &vrf_strength_profile_b_ent, VFI_FOR_TUNNEL_TERM_FLOWf,
            VFI_FOR_TUNNEL_TERM_FLOW_STRENGTH);
        soc_mem_field32_set(
            unit, mem, &vrf_strength_profile_b_ent,
            VFI_FOR_NON_TUNNEL_TERM_FLOWf,
            VFI_FOR_NON_TUNNEL_TERM_FLOW_STRENGTH);
        soc_mem_field32_set(
            unit, mem, &vrf_strength_profile_b_ent, L3_IIF_FOR_TUNNEL_TERM_FLOWf,
            L3_IIF_FOR_TUNNEL_TERM_FLOW_STRENGTH);
        soc_mem_field32_set(
            unit, mem, &vrf_strength_profile_b_ent,
            L3_IIF_FOR_NON_TUNNEL_TERM_FLOWf,
            L3_IIF_FOR_NON_TUNNEL_TERM_FLOW_STRENGTH);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 1,
                            &vrf_strength_profile_b_ent));
    }
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_common_reinit(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }
    } else {
        rv = _bcm_common_wb_alloc(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

#endif /* BCM_WARM_BOOT_SUPPORT */
    
    info->initialized = TRUE;
    return rv;
}

/*
 * Function:
 *      _bcm_mac_da_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the MAC_DA_PROFILE table
 *      Adds an entry to the global shared SW copy of the MAC_DA_PROFILE table
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_mac_da_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, MAC_DA_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_mac_da_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the MAC_DA_PROFILE table
 *      Deletes an entry from the global shared SW copy of the MAC_DA_PROFILE table
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_mac_da_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, MAC_DA_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_mem_reference_unique
 * Purpose:
 *      Update LPORT_PROFILE_TABLE entry reference count and entries_per_set
 *      for memories split across pipes as unique memory
 *      information for given index.
 * Parameters:
 *      unit            - (IN) Device number
 *      index           - (IN) Base index for entry to update
 *      entries_per_set - (IN) Number of entries in the set
 *      pipe            - (IN) Pipe to affect
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_profile_mem_reference_unique(int unit, int index, int entries_per_set, int pipe)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference_unique(unit, LPORT_PROFILE(unit),
                                   index, entries_per_set, pipe);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_mem_reference
 * Purpose:
 *      Update LPORT_PROFILE_TABLE entry reference count and entries_per_set 
 *      information for given index.
 * Parameters:
 *      unit            - (IN) Device number
 *      index           - (IN) Base index for entry to update
 *      entries_per_set - (IN) Number of entries in the set
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_profile_mem_reference(int unit, int index, int entries_per_set)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, LPORT_PROFILE(unit),
                                   index, entries_per_set);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_entry_add_unique
 * Purpose:
 *      Internal function for adding an entry to the LPORT_PROFILE_TABLE.
 *      Adds an entry to the global shared SW copy of the LPORT_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 *
 *      This is for memories who have been split across pipes as UNIQUE
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      pipe    -  (IN) Pipe to affect
 *      index   -  (OUT) Base index for the entries allocated in HW
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_profile_entry_add_unique(int unit, void **entries,
                              int entries_per_set, int pipe, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add_unique(unit, LPORT_PROFILE(unit), entries,
                             entries_per_set, pipe, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the LPORT_PROFILE_TABLE.
 *      Adds an entry to the global shared SW copy of the LPORT_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entries allocated in HW
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, LPORT_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_entry_delete_unique
 * Purpose:
 *      Internal function for deleting an entry from the LPORT_PROFILE_TABLE.
 *      Deletes an entry from the global shared SW copy of the
 *      LPORT_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 *      pipe    -  (IN) Pipe to affect
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_profile_entry_delete_unique(int unit, int index, int pipe)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete_unique(unit, LPORT_PROFILE(unit), index, pipe);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the LPORT_PROFILE_TABLE.
 *      Deletes an entry from the global shared SW copy of the
 *      LPORT_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, LPORT_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_entry_get
 * Purpose:
 *      Internal function for getting an entry from the LPORT_PROFILE_TABLE.
 *      Gets an entry from the global shared SW copy of the
 *      LPORT_PROFILE_TABLE.
 *      Shared by WLAN and proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    - (IN) Device number
 *      index   - (IN) Base index to the entries in HW for table
 *      count   - (IN) Array of number of entries to retrieve
 *      entries - (OUT) Array of pointer to table entries to get
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_profile_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, LPORT_PROFILE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_fields32_modify
 * Purpose:
 *      Modify the specified fields for all valid (non-zero reference count)
 *      entries in the LPORT Profile Table.
 *
 *      The LPORT Profile Table consists of:
 *          LPORT_TAB (must exist)
 *          RTAG7_PORT_BASED_HASH (IF available, extension of LPORT_TAB)
 *
 *      This is useful when a field(s) needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit        - (IN) Device number
 *      table_id    - (IN) Index of table to modify, LPORT or RTAG7
 *      field_count - (IN) Number of fields to modify
 *      fields      - (IN) Array of fields to modify
 *      values      - (IN) Array of new field values
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_profile_fields32_modify(int unit, int table_id, int field_count,
                                   soc_field_t *fields, uint32 *values)
{
    int rv;
    int i;
    soc_mem_t mem;
    soc_profile_mem_t *profile = NULL;

    if (table_id == LPORT_PROFILE_LPORT_TAB) {
        mem = LPORT_TABm;
    } else if (table_id == LPORT_PROFILE_RTAG7_TAB) {
        mem = RTAG7_PORT_BASED_HASHm;
    } else {
        return BCM_E_INTERNAL;
    }

    /* Check all fields are valid */
    for (i = 0; i < field_count; i++) {
        if (!SOC_MEM_FIELD_VALID(unit, mem, fields[i])) {
            return BCM_E_UNAVAIL;
        }
    }

    COMMON_LOCK(unit);

    if(soc_feature(unit, soc_feature_separate_ing_lport_rtag7_profile)) {
        profile = LPORT_IND_PROFILE(unit);
    } else {
        profile =  LPORT_PROFILE(unit);
    }

    /* For memories who are split per pipe, they need to be handled
     * differently.  The function needs to iterate per pipe memory
     * rather than a single memory */
    if (SOC_MEM_UNIQUE_ACC(unit, LPORT_TABm)) {
        rv = soc_profile_mem_fields32_modify_unique(unit, profile, table_id,
                                         field_count, fields, values);
    } else {
    rv = soc_profile_mem_fields32_modify(unit, profile, table_id,
                                         field_count, fields, values);
    }

    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_field32_modify
 * Purpose:
 *      Modify the specified field for all valid (non-zero reference count)
 *      entries in the LPORT Profile Table.
 *
 *      This is useful when a field needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit     - (IN) Device number
 *      table_id - (IN) Index of table to modify, LPORT or RTAG7
 *      field    - (IN) Fields to modify
 *      value    - (IN) New field value
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_profile_field32_modify(int unit, int table_id,
                                  soc_field_t field, uint32 value)
{
    return _bcm_lport_profile_fields32_modify(unit, table_id,
                                              1, &field, &value);
}

/*
 * Function:
 *      _bcm_lport_ind_profile_mem_reference
 * Purpose:
 *      Update LPORT_1_PROFILE_TABLE entry reference count and entries_per_set 
 *      information for given index.
 * Parameters:
 *      unit            - (IN) Device number
 *      index           - (IN) Base index for entry to update
 *      entries_per_set - (IN) Number of entries in the set
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_ind_profile_mem_reference(int unit, int index, int entries_per_set)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, LPORT_IND_PROFILE(unit),
                                   index, entries_per_set);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_ind_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the LPORT_PROFILE_TABLE.
 *      Adds an entry to the global shared SW copy of the LPORT_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entries allocated in HW
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_ind_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, LPORT_IND_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_ind_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the LPORT_PROFILE_TABLE.
 *      Deletes an entry from the global shared SW copy of the
 *      LPORT_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_ind_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, LPORT_IND_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_ind_profile_entry_get
 * Purpose:
 *      Internal function for getting an entry from the LPORT_PROFILE_TABLE.
 *      Gets an entry from the global shared SW copy of the
 *      LPORT_PROFILE_TABLE.
 *      Shared by WLAN and proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    - (IN) Device number
 *      index   - (IN) Base index to the entries in HW for table
 *      count   - (IN) Array of number of entries to retrieve
 *      entries - (OUT) Array of pointer to table entries to get
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_ind_profile_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, LPORT_IND_PROFILE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_ind_profile_fields32_modify
 * Purpose:
 *      Modify the specified fields for all valid (non-zero reference count)
 *      entries in the LPORT Profile Table.
 *
 *      The LPORT Profile Table consists of:
 *          LPORT_TAB (must exist)
 *          RTAG7_PORT_BASED_HASH (IF available, extension of LPORT_TAB)
 *
 *      This is useful when a field(s) needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit        - (IN) Device number
 *      table_id    - (IN) Index of table to modify, LPORT or RTAG7
 *      field_count - (IN) Number of fields to modify
 *      fields      - (IN) Array of fields to modify
 *      values      - (IN) Array of new field values
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_ind_profile_fields32_modify(int unit, int table_id, int field_count,
                                   soc_field_t *fields, uint32 *values)
{
    int rv;
    int i;
    soc_mem_t mem;

    if (table_id == LPORT_PROFILE_LPORT_TAB) {
        mem = LPORT_TABm;
    } else {
        return BCM_E_INTERNAL;
    }

    /* Check all fields are valid */
    for (i = 0; i < field_count; i++) {
        if (!SOC_MEM_FIELD_VALID(unit, mem, fields[i])) {
            return BCM_E_UNAVAIL;
        }
    }

    COMMON_LOCK(unit);
    rv = soc_profile_mem_fields32_modify(unit, LPORT_IND_PROFILE(unit), table_id,
                                         field_count, fields, values);
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_lport_ind_profile_field32_modify
 * Purpose:
 *      Modify the specified field for all valid (non-zero reference count)
 *      entries in the LPORT Profile Table.
 *
 *      This is useful when a field needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit     - (IN) Device number
 *      table_id - (IN) Index of table to modify, LPORT or RTAG7
 *      field    - (IN) Fields to modify
 *      value    - (IN) New field value
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_lport_ind_profile_field32_modify(int unit, int table_id,
                                  soc_field_t field, uint32 value)
{
    return _bcm_lport_ind_profile_fields32_modify(unit, table_id,
                                              1, &field, &value);
}

/*
 * Function:
 *      _bcm_rtag7_ind_profile_mem_reference
 * Purpose:
 *      Update RTAG7_1_PROFILE_TABLE entry reference count and entries_per_set 
 *      information for given index.
 * Parameters:
 *      unit            - (IN) Device number
 *      index           - (IN) Base index for entry to update
 *      entries_per_set - (IN) Number of entries in the set
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_rtag7_ind_profile_mem_reference(int unit, int index, int entries_per_set)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, RTAG7_IND_PROFILE(unit),
                                   index, entries_per_set);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_rtag7_ind_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the LPORT_PROFILE_TABLE.
 *      Adds an entry to the global shared SW copy of the LPORT_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entries allocated in HW
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_rtag7_ind_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, RTAG7_IND_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_rtag7_ind_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the LPORT_PROFILE_TABLE.
 *      Deletes an entry from the global shared SW copy of the
 *      LPORT_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_rtag7_ind_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, RTAG7_IND_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_rtag7_ind_profile_entry_get
 * Purpose:
 *      Internal function for getting an entry from the LPORT_PROFILE_TABLE.
 *      Gets an entry from the global shared SW copy of the
 *      LPORT_PROFILE_TABLE.
 *      Shared by WLAN and proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    - (IN) Device number
 *      index   - (IN) Base index to the entries in HW for table
 *      count   - (IN) Array of number of entries to retrieve
 *      entries - (OUT) Array of pointer to table entries to get
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_rtag7_ind_profile_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, RTAG7_IND_PROFILE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_rtag7_ind_profile_fields32_modify
 * Purpose:
 *      Modify the specified fields for all valid (non-zero reference count)
 *      entries in the LPORT Profile Table.
 *
 *      The LPORT Profile Table consists of:
 *          LPORT_TAB (must exist)
 *          RTAG7_PORT_BASED_HASH (IF available, extension of LPORT_TAB)
 *
 *      This is useful when a field(s) needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit        - (IN) Device number
 *      table_id    - (IN) Index of table to modify, LPORT or RTAG7
 *      field_count - (IN) Number of fields to modify
 *      fields      - (IN) Array of fields to modify
 *      values      - (IN) Array of new field values
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_rtag7_ind_profile_fields32_modify(int unit, int table_id, int field_count,
                                   soc_field_t *fields, uint32 *values)
{
    int rv;
    int i;
    soc_mem_t mem;

    if (table_id == LPORT_PROFILE_RTAG7_TAB) {
        mem = RTAG7_PORT_BASED_HASHm;
    } else {
        return BCM_E_INTERNAL;
    }

    /* Check all fields are valid */
    for (i = 0; i < field_count; i++) {
        if (!SOC_MEM_FIELD_VALID(unit, mem, fields[i])) {
            return BCM_E_UNAVAIL;
        }
    }

    COMMON_LOCK(unit);
    rv = soc_profile_mem_fields32_modify(unit, RTAG7_IND_PROFILE(unit), table_id,
                                         field_count, fields, values);
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_rtag7_ind_profile_field32_modify
 * Purpose:
 *      Modify the specified field for all valid (non-zero reference count)
 *      entries in the LPORT Profile Table.
 *
 *      This is useful when a field needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit     - (IN) Device number
 *      table_id - (IN) Index of table to modify, LPORT or RTAG7
 *      field    - (IN) Fields to modify
 *      value    - (IN) New field value
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_rtag7_ind_profile_field32_modify(int unit, int table_id,
                                  soc_field_t field, uint32 value)
{
    return _bcm_rtag7_ind_profile_fields32_modify(unit, table_id,
                                              1, &field, &value);
}

int
_bcm_egr_lport_profile_mem_index_get(int unit, soc_mem_t mem, int *index)
{
    int rv = BCM_E_NOT_FOUND;

    COMMON_LOCK(unit);
    rv = soc_profile_mem_index_get(unit, EGR_LPORT_PROFILE(unit), mem, index);
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_egr_lport_profile_mem_reference
 * Purpose:
 *      Update LPORT_1_PROFILE_TABLE entry reference count and entries_per_set 
 *      information for given index.
 * Parameters:
 *      unit            - (IN) Device number
 *      index           - (IN) Base index for entry to update
 *      entries_per_set - (IN) Number of entries in the set
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_lport_profile_mem_reference(int unit, int index, int entries_per_set)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, EGR_LPORT_PROFILE(unit),
                                   index, entries_per_set);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_lport_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the EGR_LPORT_PROFILE_TABLE.
 *      Adds an entry to the global shared SW copy of the EGR_LPORT_PROFILE_TABLE.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entries allocated in HW
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_lport_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_LPORT_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_lport_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the EGR_LPORT_PROFILE_TABLE.
 *      Deletes an entry from the global shared SW copy of the
 *      EGR_LPORT_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_lport_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_LPORT_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_lport_profile_entry_get
 * Purpose:
 *      Internal function for getting an entry from the EGR_LPORT_PROFILE_TABLE.
 *      Gets an entry from the global shared SW copy of the
 *      EGR_LPORT_PROFILE_TABLE.
 *      Shared by WLAN and proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    - (IN) Device number
 *      index   - (IN) Base index to the entries in HW for table
 *      count   - (IN) Array of number of entries to retrieve
 *      entries - (OUT) Array of pointer to table entries to get
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_lport_profile_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, EGR_LPORT_PROFILE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_lport_profile_fields32_modify
 * Purpose:
 *      Modify the specified fields for all valid (non-zero reference count)
 *      entries in the EGR_LPORT Profile Table.
 *
 *      The EGR_LPORT Profile Table consists of:
 *      EGR_LPORT_PROFILE
 *      EGR_VLAN_CONTROL_1
 *      EGR_VLAN_CONTROL_2
 *      EGR_VLAN_CONTROL_3
 *      EGR_IPMC_CFG2
 *      EGR_MTU
 *      EGR_PORT_1
 *      EGR_COUNTER_CONTROL (non-TD3)
 *      EGR_SHAPING_CONTROL (non-TD3)
 *
 *      This is useful when a field(s) needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit        - (IN) Device number
 *      mem         - (IN) Table to modify
 *      field_count - (IN) Number of fields to modify
 *      fields      - (IN) Array of fields to modify
 *      values      - (IN) Array of new field values
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_lport_profile_fields32_modify(int unit, soc_mem_t mem, int field_count,
                                       soc_field_t *fields, uint32 *values)
{
    int rv;
    int i, table_id;

    /* Check all fields are valid */
    for (i = 0; i < field_count; i++) {
        if (!SOC_MEM_FIELD_VALID(unit, mem, fields[i])) {
            return BCM_E_UNAVAIL;
        }
    }

    COMMON_LOCK(unit);
    rv = soc_profile_mem_index_get(unit, EGR_LPORT_PROFILE(unit), mem,
                                   &table_id);
    if (SOC_SUCCESS(rv)) {
        rv = soc_profile_mem_fields32_modify(unit, EGR_LPORT_PROFILE(unit),
                    table_id, field_count, fields, values);
    }
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_egr_lport_profile_field32_modify
 * Purpose:
 *      Modify the specified field for all valid (non-zero reference count)
 *      entries in the EGR_LPORT Profile Table.
 *
 *      This is useful when a field needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit     - (IN) Device number
 *      mem      - (IN) Table to modify
 *      field    - (IN) Fields to modify
 *      value    - (IN) New field value
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_lport_profile_field32_modify(int unit, soc_mem_t mem,
                                  soc_field_t field, uint32 value)
{
    return _bcm_egr_lport_profile_fields32_modify(unit, mem, 1, &field, &value);
}

#ifdef BCM_TRIDENT2PLUS_SUPPORT
/*
 * Function:
 *      _bcm_misc_port_profile_mem_reference
 * Purpose:
 *      Update MISC_PORT_PROFILE_TABLE entry reference count and entries_per_set
 *      information for given index.
 * Parameters:
 *      unit            - (IN) Device number
 *      index           - (IN) Base index for entry to update
 *      entries_per_set - (IN) Number of entries in the set
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_misc_port_profile_mem_reference(int unit, int index, int entries_per_set)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, MISC_PORT_PROFILE(unit),
                                   index, entries_per_set);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_misc_port_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the MISC_PORT_PROFILE_TABLE.
 *      Adds an entry to the global shared SW copy of the MISC_PORT_PROFILE_TABLE.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entries allocated in HW
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_misc_port_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, MISC_PORT_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_misc_port_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the MISC_PORT_PROFILE_TABLE.
 *      Deletes an entry from the global shared SW copy of the
 *      MISC_PORT_PROFILE_TABLE.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_misc_port_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, MISC_PORT_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_misc_port_profile_entry_get
 * Purpose:
 *      Internal function for getting an entry from the MISC_PORT_PROFILE_TABLE.
 *      Gets an entry from the global shared SW copy of the
 *      MISC_PORT_PROFILE_TABLE.
 * Parameters:
 *      unit    - (IN) Device number
 *      index   - (IN) Base index to the entries in HW for table
 *      count   - (IN) Array of number of entries to retrieve
 *      entries - (OUT) Array of pointer to table entries to get
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_misc_port_profile_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, MISC_PORT_PROFILE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_misc_port_profile_fields32_modify
 * Purpose:
 *      Modify the specified fields for all valid (non-zero reference count)
 *      entries in the MISC_PORT_PROFILE_TABLE.
 *
 *      The MISC_PORT_PROFILE_TABLE consists of:
 *          FP_I2E_CLASSID_SELECT
 *          FP_HG_CLASSID_SELECT
 *
 *      This is useful when a field(s) needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit        - (IN) Device number
 *      table_id    - (IN) Index of table to modify, LPORT or RTAG7
 *      field_count - (IN) Number of fields to modify
 *      fields      - (IN) Array of fields to modify
 *      values      - (IN) Array of new field values
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_misc_port_profile_fields32_modify(int unit, int table_id, int field_count,
                                   soc_field_t *fields, uint32 *values)
{
    int rv;
    int i;
    soc_mem_t mem;

    if (table_id == 0) {
        mem = FP_I2E_CLASSID_SELECTm;
    } else if (table_id == 1) {
        mem = FP_HG_CLASSID_SELECTm;
    } else {
        return BCM_E_INTERNAL;
    }

    /* Check all fields are valid */
    for (i = 0; i < field_count; i++) {
        if (!SOC_MEM_FIELD_VALID(unit, mem, fields[i])) {
            return BCM_E_UNAVAIL;
        }
    }

    COMMON_LOCK(unit);
    rv = soc_profile_mem_fields32_modify(unit, MISC_PORT_PROFILE(unit), table_id,
                                         field_count, fields, values);
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_misc_port_profile_field32_modify
 * Purpose:
 *      Modify the specified field for all valid (non-zero reference count)
 *      entries in the MISC_PORT_PROFILE_TABLE.
 *
 *      This is useful when a field needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit     - (IN) Device number
 *      table_id - (IN) Index of table to modify, LPORT or RTAG7
 *      field    - (IN) Fields to modify
 *      value    - (IN) New field value
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_misc_port_profile_field32_modify(int unit, int table_id,
                                  soc_field_t field, uint32 value)
{
    return _bcm_misc_port_profile_fields32_modify(unit, table_id,
                                              1, &field, &value);
}
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

/*
 * Function:
 *      _bcm_egr_qos_profile_mem_reference
 * Purpose:
 *      Update EGR_EGR_QOS_PROFILE_TABLE entry reference count and entries_per_set 
 *      information for given index.
 * Parameters:
 *      unit            - (IN) Device number
 *      index           - (IN) Base index for entry to update
 *      entries_per_set - (IN) Number of entries in the set
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_qos_profile_mem_reference(int unit, int index, int entries_per_set)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, EGR_QOS_PROFILE(unit),
                                   index, entries_per_set);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_qos_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the EGR_QOS_PROFILE_TABLE.
 *      Adds an entry to the global shared SW copy of the EGR_QOS_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entries allocated in HW
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_qos_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_QOS_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_qos_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the EGR_QOS_PROFILE_TABLE.
 *      Deletes an entry from the global shared SW copy of the
 *      EGR_QOS_PROFILE_TABLE.
 *      Shared by WLAN, proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_qos_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_QOS_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_qos_profile_entry_get
 * Purpose:
 *      Internal function for getting an entry from the EGR_QOS_PROFILE_TABLE.
 *      Gets an entry from the global shared SW copy of the
 *      EGR_QOS_PROFILE_TABLE.
 *      Shared by WLAN and proxy, and SOURCE_TRUNK_MAP table.
 * Parameters:
 *      unit    - (IN) Device number
 *      index   - (IN) Base index to the entries in HW for table
 *      count   - (IN) Array of number of entries to retrieve
 *      entries - (OUT) Array of pointer to table entries to get
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_qos_profile_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, EGR_QOS_PROFILE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_qos_profile_fields32_modify
 * Purpose:
 *      Modify the specified fields for all valid (non-zero reference count)
 *      entries in the LPORT Profile Table.
 *
 *      The LPORT Profile Table consists of:
 *          LPORT_TAB (must exist)
 *          RTAG7_PORT_BASED_HASH (IF available, extension of LPORT_TAB)
 *
 *      This is useful when a field(s) needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit        - (IN) Device number
 *      table_id    - (IN) Index of table to modify, LPORT or RTAG7
 *      field_count - (IN) Number of fields to modify
 *      fields      - (IN) Array of fields to modify
 *      values      - (IN) Array of new field values
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_qos_profile_fields32_modify(int unit, int table_id, int field_count,
                                   soc_field_t *fields, uint32 *values)
{
    int rv;
    int i;
    soc_mem_t mem;

    if (table_id == EGR_PRI_CNG_MAPm) {
        mem = EGR_PRI_CNG_MAPm;
    } else if (table_id == EGR_DSCP_TABLEm) {
        mem = EGR_DSCP_TABLEm;
    } else if (table_id == EGR_ZONE_2_QOS_MAPPING_TABLEm) {
        mem = EGR_ZONE_2_QOS_MAPPING_TABLEm;
    } else {
        return BCM_E_INTERNAL;
    }

    /* Check all fields are valid */
    for (i = 0; i < field_count; i++) {
        if (!SOC_MEM_FIELD_VALID(unit, mem, fields[i])) {
            return BCM_E_UNAVAIL;
        }
    }

    COMMON_LOCK(unit);
    rv = soc_profile_mem_fields32_modify(unit, EGR_QOS_PROFILE(unit), table_id,
                                         field_count, fields, values);
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_egr_qos_profile_field32_modify
 * Purpose:
 *      Modify the specified field for all valid (non-zero reference count)
 *      entries in the LPORT Profile Table.
 *
 *      This is useful when a field needs to be set globally
 *      in the profile table.
 * Parameters:
 *      unit     - (IN) Device number
 *      table_id - (IN) Index of table to modify, LPORT or RTAG7
 *      field    - (IN) Fields to modify
 *      value    - (IN) New field value
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_qos_profile_field32_modify(int unit, int table_id,
                                  soc_field_t field, uint32 value)
{
    return _bcm_egr_qos_profile_fields32_modify(unit, table_id,
                                              1, &field, &value);
}


/*
 * Function:
 *      _bcm_ing_vft_pri_map_entry_add
 * Purpose:
 *      Adds an entry to the global shared SW copy of the ING_VFT_PRI_MAP table.
 * Parameters:
 *      unit            - (IN)  Device number.
 *      entries         - (IN)  Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index           - (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_ing_vft_pri_map_entry_add(int unit, void **entries,
                               int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, ING_VFT_PRI_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_l2_vlan_etag_map_entry_add
 * Purpose:
 *      Adds an entry to the global shared SW copy of the ING_ETAG_PCP_MAPPING table.
 * Parameters:
 *      unit            - (IN)  Device number.
 *      entries         - (IN)  Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index           - (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_ing_l2_vlan_etag_map_entry_add(int unit, void **entries,
                               int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, ING_L2_VLAN_ETAG_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_l2_vlan_etag_map_entry_add
 * Purpose:
 *      Adds an entry to the global shared SW copy of the EGR_ETAG_PCP_MAPPING table.
 * Parameters:
 *      unit            - (IN)  Device number.
 *      entries         - (IN)  Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index           - (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_l2_vlan_etag_map_entry_add(int unit, void **entries,
                               int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_L2_VLAN_ETAG_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_vft_pri_map_entry_add
 * Purpose:
 *      Adds an entry to the EGR_VFT_PRI_MAP table (both the hardware and 
 *      the global shared SW cache).
 * Parameters:
 *      unit            - (IN)  Device number.
 *      entries         - (IN)  Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index           - (OUT) Base index for the entries allocated
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_vft_pri_map_entry_add(int unit, void **entries,
                               int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_VFT_PRI_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_vsan_intpri_map_entry_add
 * Purpose:
 *      Adds an entry to the EGR_VSAN_INTPRI_MAP table (both the hardware and 
 *      the global shared SW cache).
 * Parameters:
 *      unit            - (IN)  Device number.
 *      entries         - (IN)  Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index           - (OUT) Base index for the entries allocated
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_vsan_intpri_map_entry_add(int unit, void **entries,
                                   int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_VSAN_INTPRI_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_vft_fields_entry_add
 * Purpose:
 *      Adds an entry to the EGR_VFT_FIELDS_PROFILE table (both the hardware and 
 *      the global shared SW cache).
 * Parameters:
 *      unit            - (IN)  Device number.
 *      entries         - (IN)  Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index           - (OUT) Base index for the entries allocated
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_egr_vft_fields_entry_add(int unit, void **entries, int entries_per_set,
                              uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_VFT_FIELDS(unit), entries, 
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function: 
 *     _bcm_ing_vft_pri_map_entry_del
 * Purpose:
 *      Deletes an entry from the ING_VFT_PRI_MAP table (both the hardware and
 *      the global shared SW cache).
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entries to be deleted
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_ing_vft_pri_map_entry_del(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, ING_VFT_PRI_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function: 
 *     _bcm_ing_l2_vlan_etag_map_entry_del
 * Purpose:
 *      Deletes an entry from the ING_ETAG_PCP_MAPPING table (both the hardware and
 *      the global shared SW cache).
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entries to be deleted
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_ing_l2_vlan_etag_map_entry_del(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, ING_L2_VLAN_ETAG_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function: 
 *     _bcm_egr_vft_pri_map_entry_del
 * Purpose:
 *      Deletes an entry from the EGR_VFT_PRI_MAP table (both the hardware and
 *      the global shared SW cache).
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entries to be deleted
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_egr_vft_pri_map_entry_del(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_VFT_PRI_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function: 
 *     _bcm_egr_vft_fields_entry_del
 * Purpose:
 *      Deletes an entry from the EGR_VFT_FIELDS_PROFILE table (both the 
 *      hardware and the global shared SW cache).
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entries to be deleted
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_egr_vft_fields_entry_del(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_VFT_FIELDS(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function: 
 *     _bcm_egr_vsan_intpri_map_entry_del
 * Purpose:
 *      Deletes an entry from the EGR_VSAN_INTPRI_MAP table (both the hardware
 *      and the global shared SW cache).
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entries to be deleted
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_egr_vsan_intpri_map_entry_del(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_VSAN_INTPRI_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function: 
 *     _bcm_egr_l2_vlan_etag_map_entry_del
 * Purpose:
 *      Deletes an entry from the EGR_ETAG_PCP_MAPPING table (both the hardware and
 *      the global shared SW cache).
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entries to be deleted
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_egr_l2_vlan_etag_map_entry_del(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_L2_VLAN_ETAG_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}


/*
 * Function: 
 *     _bcm_ing_vft_pri_map_entry_get
 * Purpose:
 *     Get a set of entries from the ING_VFT_PRI_MAP table.
 * Parameters:
 *     unit    - (IN)  Device number.
 *     index   - (IN)  Base index for the entries to be retrieved
 *     count   - (IN)  Number of entries in the set
 *     entries - (OUT) Array of pointer to table entries set
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_ing_vft_pri_map_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, ING_VFT_PRI_MAP(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_vft_pri_map_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in ING_VFT_PRI table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_vft_pri_map_entry_reference(int unit, int index,
                                     int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, ING_VFT_PRI_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}


/*
 * Function: 
 *     _bcm_ing_l2_vlan_etag_map_entry_get
 * Purpose:
 *     Get a set of entries from the ING_ETAG_PCP_MAPPING table.
 * Parameters:
 *     unit    - (IN)  Device number.
 *     index   - (IN)  Base index for the entries to be retrieved
 *     count   - (IN)  Number of entries in the set
 *     entries - (OUT) Array of pointer to table entries set
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_ing_l2_vlan_etag_map_entry_get(int unit, int index, 
                                              int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, ING_L2_VLAN_ETAG_MAP(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_l2_vlan_etag_map_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in ING_L2_VLAN_ETAG table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_l2_vlan_etag_map_entry_reference(int unit, int index,
                                     int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, ING_L2_VLAN_ETAG_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function: 
 *     _bcm_egr_vft_pri_map_entry_get
 * Purpose:
 *     Get a set of entries from the EGR_VFT_PRI_MAP table.
 * Parameters:
 *     unit    - (IN)  Device number.
 *     index   - (IN)  Base index for the entries to be retrieved
 *     count   - (IN)  Number of entries in the set
 *     entries - (OUT) Array of pointer to table entries set
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_egr_vft_pri_map_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, EGR_VFT_PRI_MAP(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_vft_pri_map_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in ING_VFT_PRI table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_vft_pri_map_entry_reference(int unit, int index,
                                     int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, EGR_VFT_PRI_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function: 
 *     _bcm_egr_vft_fields_entry_get
 * Purpose:
 *     Get a set of entries from the EGR_VFT_FIELDS_PROFILE table.
 * Parameters:
 *     unit    - (IN)  Device number.
 *     index   - (IN)  Base index for the entries to be retrieved
 *     count   - (IN)  Number of entries in the set
 *     entries - (OUT) Array of pointer to table entries set
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_egr_vft_fields_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, EGR_VFT_FIELDS(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function: 
 *     _bcm_egr_vsan_intpri_map_entry_get
 * Purpose:
 *     Get a set of entries from the EGR_VSAN_INTPRI_MAP table.
 * Parameters:
 *     unit    - (IN)  Device number.
 *     index   - (IN)  Base index for the entries to be retrieved
 *     count   - (IN)  Number of entries in the set
 *     entries - (OUT) Array of pointer to table entries set
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_egr_vsan_intpri_map_entry_get(int unit, int index, int count, 
                                   void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, EGR_VSAN_INTPRI_MAP(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_vft_fields_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in EGR_VFT_FIELDS_PROFILE table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_vft_fields_entry_reference(int unit, int index,
                                    int entries_per_set_override)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, EGR_VFT_FIELDS(unit), index,
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_vsan_intpri_map_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in EGR_VSAN_INT_PRI table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_vsan_intpri_map_entry_reference(int unit, int index,
                                     int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, EGR_VSAN_INTPRI_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function: 
 *     _bcm_egr_l2_vlan_etag_map_entry_get
 * Purpose:
 *     Get a set of entries from the EGR_ETAG_PCP_MAPPING table.
 * Parameters:
 *     unit    - (IN)  Device number.
 *     index   - (IN)  Base index for the entries to be retrieved
 *     count   - (IN)  Number of entries in the set
 *     entries - (OUT) Array of pointer to table entries set
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_egr_l2_vlan_etag_map_entry_get(int unit, int index, 
                                              int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, EGR_L2_VLAN_ETAG_MAP(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_l2_vlan_etag_map_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in EGR_L2_VLAN_ETAG table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_l2_vlan_etag_map_entry_reference(int unit, int index,
                                     int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, EGR_L2_VLAN_ETAG_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_pri_cng_map_entry_add
 * Purpose:
 *      Internal function for adding an entry to the ING_PRI_CNG_MAP table
 *      Adds an entry to the global shared SW copy of the ING_PRI_CNG_MAP table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_pri_cng_map_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, ING_PRI_CNG_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_pri_cng_map_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the ING_PRI_CNG_MAP table
 *      Deletes an entry from the global shared SW copy of the ING_PRI_CNG_MAP table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_pri_cng_map_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, ING_PRI_CNG_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_ing_pri_cng_map_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, ING_PRI_CNG_MAP(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_pri_cng_map_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in ING_PRI_CNG_MAP table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_pri_cng_map_entry_reference(int unit, int index,
                                     int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, ING_PRI_CNG_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_mpls_combo_map_entry_add
 * Purpose:
 *      Internal function for adding an entry to the combined
 *      EGR_MPLS_PRI_MAPPING and EGR_MPLS_EXP_MAPPING_1 table.
 *      Adds an entry to the global shared SW copy of the combo table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_mpls_combo_map_entry_add(int unit, void **entries, 
                                int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_MPLS_COMBO_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_mpls_combo_map_entry_update
 * Purpose:
 *      Internal function for updating an entry to the combined
 *      EGR_MPLS_PRI_MAPPING and EGR_MPLS_EXP_MAPPING_1 table.
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      index   -  (OUT) Base index to update
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_mpls_combo_map_entry_update(int unit, void **entries, 
                                     uint32 index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_set(unit, EGR_MPLS_COMBO_MAP(unit), entries,
                             index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_mpls_combo_map_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the combined
 *      EGR_MPLS_PRI_MAPPING and EGR_MPLS_EXP_MAPPING_1 table.
 *      Deletes an entry from the global shared SW copy of the combo table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_mpls_combo_map_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_MPLS_COMBO_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_mpls_combo_map_entry_reference
 * Purpose:
 *      Internal function for indicating an entry in the combined
 *      EGR_MPLS_PRI_MAPPING and EGR_MPLS_EXP_MAPPING_1 table
 *      is being used. Updates global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_mpls_combo_map_entry_reference(int unit, int index, 
                                        int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, EGR_MPLS_COMBO_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_mpls_combo_map_entry_write_mode_get
 * Purpose:
 *      Internal function to get write mode of
 *      EGR_MPLS_PRI_MAPPING and EGR_MPLS_EXP_MAPPING_1 table.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index to get
 *      mode    -  (OUT) table write mode
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_mpls_combo_map_entry_write_mode_get(int unit,
        uint32 index, int *mode)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_write_mode_get(unit, EGR_MPLS_COMBO_MAP(unit),
            index, mode);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_dscp_table_entry_add
 * Purpose:
 *      Internal function for adding an entry to the DSCP_TABLE table
 *      Adds an entry to the global shared SW copy of the DSCP_TABLE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_dscp_table_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, DSCP_TABLE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_dscp_table_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the DSCP_TABLE table
 *      Deletes an entry from the global shared SW copy of the DSCP_TABLE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_dscp_table_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, DSCP_TABLE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_dscp_table_entry_reference
 * Purpose:
 *      Internal function for indicating an entry in the DSCP_TABLE table
 *      is being used. Updates the global shared SW copy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_dscp_table_entry_reference(int unit, int index, 
                                int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, DSCP_TABLE(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_dscp_table_entry_ref_count_get(int unit, uint32 index,
                                                int* ref_count)
{
    int rv;
    rv = soc_profile_mem_ref_count_get(unit,
                                       DSCP_TABLE(unit), index, ref_count);
    return rv;
}

int
_bcm_dscp_table_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, DSCP_TABLE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_dscp_table_entry_range(int unit, int *index_min, int *index_max)
{
    if (DSCP_TABLE(unit) == NULL) {
        return BCM_E_INIT;
    }

    if (index_min) {
        *index_min = DSCP_TABLE(unit)->tables[0].index_min;
    }

    if (index_max) {
        *index_max = DSCP_TABLE(unit)->tables[0].index_max;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_egr_dscp_table_entry_add
 * Purpose:
 *      Internal function for adding an entry to the EGR_DSCP_TABLE table
 *      Adds an entry to the global shared SW copy of the EGR_DSCP_TABLE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_dscp_table_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index, uint32 flags)
{
    int rv;
    int old_flags;
    COMMON_LOCK(unit);
    old_flags = EGR_DSCP_TABLE(unit)->flags;
    EGR_DSCP_TABLE(unit)->flags = flags;
    rv = soc_profile_mem_add(unit, EGR_DSCP_TABLE(unit), entries,
                             entries_per_set, index);
    EGR_DSCP_TABLE(unit)->flags = old_flags;
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_dscp_table_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the EGR_DSCP_TABLE table
 *      Deletes an entry from the global shared SW copy of the EGR_DSCP_TABLE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_dscp_table_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_DSCP_TABLE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_dscp_table_entry_reference
 * Purpose:
 *      Internal function for indicating an entry from the EGR_DSCP_TABLE table
 *      is being used. Updates the global shared SW copy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_dscp_table_entry_reference(int unit, int index, 
                                    int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, EGR_DSCP_TABLE(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_egr_dscp_table_entry_ref_count_get(int unit, uint32 index,
                                                      int *ref_count)
{
    int rv;
    rv = soc_profile_mem_ref_count_get(unit,
                                       EGR_DSCP_TABLE(unit), index, ref_count);
    return rv;
}


/*
 * Function:
 *      _bcm_egr_dscp_table_entry_set
 * Purpose:
 *      Internal function for setting an entry to the EGR_DSCP_TABLE table
 *      Sets an entry to the global shared SW copy of the EGR_DSCP_TABLE table
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_dscp_table_entry_set(int unit, void **entries, uint32 index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_set(unit, EGR_DSCP_TABLE(unit), entries, index);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_egr_dscp_table_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, EGR_DSCP_TABLE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}


int
_bcm_prot_pkt_ctrl_add(int unit, uint32 prot_pkt_ctrl,
                       uint32 igmp_mld_pkt_ctrl, uint32 *index)
{
    int rv;
    uint64 rval64[2], *rval64s[2];

    COMPILER_64_SET(rval64[0], 0, prot_pkt_ctrl);
    COMPILER_64_SET(rval64[1], 0, igmp_mld_pkt_ctrl);
    rval64s[0] = &rval64[0];
    rval64s[1] = &rval64[1];

    COMMON_LOCK(unit);
    rv = soc_profile_reg_add(unit, PROT_PKT_CTRL(unit), rval64s, 1, index);
    COMMON_UNLOCK(unit);

    return rv;
}

int
_bcm_prot_pkt_ctrl_delete(int unit, uint32 index)
{
    int rv;

    COMMON_LOCK(unit);
    rv = soc_profile_reg_delete(unit, PROT_PKT_CTRL(unit), index);
    COMMON_UNLOCK(unit);

    return rv;
}

int
_bcm_prot_pkt_ctrl_get(int unit, uint32 index, uint32 *prot_pkt_ctrl,
                       uint32 *igmp_mld_pkt_ctrl)
{
    int rv;
    uint64 rval64[2], *rval64s[2];

    rval64s[0] = &rval64[0];
    rval64s[1] = &rval64[1];

    COMMON_LOCK(unit);
    rv = soc_profile_reg_get(unit, PROT_PKT_CTRL(unit), index, 1, rval64s);
    COMMON_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        *prot_pkt_ctrl = COMPILER_64_LO(rval64[0]);
        *igmp_mld_pkt_ctrl = COMPILER_64_LO(rval64[1]);
    }

    return rv;
}

int
_bcm_prot_pkt_ctrl_reference(int unit, uint32 index)
{
    int rv;

    COMMON_LOCK(unit);
    rv = soc_profile_reg_reference(unit, PROT_PKT_CTRL(unit), index, 1);
    COMMON_UNLOCK(unit);

    return rv;
}

int
_bcm_prot_pkt_ctrl_ref_count_get(int unit, uint32 index,
                                                int* ref_count)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_reg_ref_count_get(unit,
                                       PROT_PKT_CTRL(unit), index, ref_count);
    COMMON_UNLOCK(unit);

    return rv;
}
#ifdef BCM_TOMAHAWK_SUPPORT
/*
 * Function:
 *      _bcm_ing_tunnel_term_map_entry_add
 * Purpose:
 *      Internal function for adding an entry to the ING_TUNNEL_ECN_DECAP table
 *      Adds an entry to the global shared SW copy of the ING_TUNNEL_ECN_DECAP table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_tunnel_term_map_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, ING_TUNNEL_TERM_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_tunnel_term_map_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the ING_TUNNEL_ECN_DECAP table
 *      Deletes an entry from the global shared SW copy of the ING_TUNNEL_ECN_DECAP table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_tunnel_term_map_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, ING_TUNNEL_TERM_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_ing_tunnel_term_map_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, ING_TUNNEL_TERM_MAP(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_tunnel_term_map_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in ING_TUNNEL_ECN_DECAP table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_tunnel_term_map_entry_reference(int unit, int index,
                                     int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, ING_TUNNEL_TERM_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
    defined(BCM_TRIDENT3_SUPPORT) 
/*
 * Function:
 *      _bcm_ing_exp_to_ip_ecn_map_entry_add
 * Purpose:
 *      Internal function for adding an entry to the ING_EXP_TO_IP_ECN_MAPPING table
 *      Adds an entry to the global shared SW copy of the ING_EXP_TO_IP_ECN_MAPPING table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_exp_to_ip_ecn_map_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, ING_EXP_TO_IP_ECN_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_exp_to_ip_ecn_map_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the ING_EXP_TO_IP_ECN_MAPPING table
 *      Deletes an entry from the global shared SW copy of the ING_EXP_TO_IP_ECN_MAPPING table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_exp_to_ip_ecn_map_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, ING_EXP_TO_IP_ECN_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_ing_exp_to_ip_ecn_map_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, ING_EXP_TO_IP_ECN_MAP(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_exp_to_ip_ecn_map_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in ING_EXP_TO_IP_ECN_MAPPING table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_exp_to_ip_ecn_map_entry_reference(int unit, int index,
                                     int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, ING_EXP_TO_IP_ECN_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_ip_ecn_to_exp_map_entry_add
 * Purpose:
 *      Internal function for adding an entry to the EGR_IP_ECN_TO_EXP_MAPPING table
 *      Adds an entry to the global shared SW copy of the EGR_IP_ECN_TO_EXP_MAPPING table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_ip_ecn_to_exp_map_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_IP_ECN_TO_EXP_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_ip_ecn_to_exp_map_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the EGR_IP_ECN_TO_EXP_MAPPING table
 *      Deletes an entry from the global shared SW copy of the EGR_IP_ECN_TO_EXP_MAPPING table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_ip_ecn_to_exp_map_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_IP_ECN_TO_EXP_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_egr_ip_ecn_to_exp_map_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, EGR_IP_ECN_TO_EXP_MAP(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_ip_ecn_to_exp_map_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in EGR_IP_ECN_TO_EXP_MAPPING table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_ip_ecn_to_exp_map_entry_reference(int unit, int index,
                                     int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, EGR_IP_ECN_TO_EXP_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_int_cn_to_exp_map_entry_add
 * Purpose:
 *      Internal function for adding an entry to the EGR_INT_CN_TO_EXP_MAPPING table
 *      Adds an entry to the global shared SW copy of the EGR_INT_CN_TO_EXP_MAPPING table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_int_cn_to_exp_map_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_INT_CN_TO_EXP_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_int_cn_to_exp_map_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the EGR_INT_CN_TO_EXP_MAPPING table
 *      Deletes an entry from the global shared SW copy of the EGR_INT_CN_TO_EXP_MAPPING table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_int_cn_to_exp_map_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_INT_CN_TO_EXP_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_egr_int_cn_to_exp_map_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, EGR_INT_CN_TO_EXP_MAP(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_int_cn_to_exp_map_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in EGR_INT_CN_TO_EXP_MAPPING table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_int_cn_to_exp_map_entry_reference(int unit, int index,
                                     int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, EGR_INT_CN_TO_EXP_MAP(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}
#endif /* BCM_TOMAHAWK2_SUPPORT or BCM_TOMAHAWK3_SUPPORT */


#ifdef BCM_TRIUMPH3_SUPPORT
/*
 * Function:
 *      _bcm_ing_l3_nh_attrib_entry_add
 * Purpose:
 *      Internal function for adding an entry to the ING_L3_NEXT_HOP_ATTRIBUTE_1
 *      table. Adds an entry to the global shared SW copy of the
 *      ING_L3_NEXT_HOP_ATTRIBUTE_1 table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_l3_nh_attrib_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, ING_L3_NH_ATTRIB_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_l3_nh_attrib_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the
 *      ING_L3_NEXT_HOP_ATTRIBUTE_1 table Deletes an entry from
 *      the global shared SW copy of the IN_L3_NEXT_HOP_ATTRIBUTE_1 table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_l3_nh_attrib_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, ING_L3_NH_ATTRIB_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_l3_nh_attrib_entry_reference
 * Purpose:
 *      Internal function for indicating an entry in the
 *      IN_L3_NEXT_HOP_ATTRIBUTE_1 table is being used.
 *      Updates the global shared SW copy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_l3_nh_attrib_entry_reference(int unit, int index, 
                                int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, ING_L3_NH_ATTRIB_PROFILE(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}
#endif /* BCM_TRIUMPH3_SUPPORT */


#ifdef BCM_TRIDENT2_SUPPORT
/*
 * Function:
 *      _bcm_l3_iif_profile_sw_state_set
 * Purpose:
 *      Internal function for setting sw state for L3_IIF_PROFILE table
 *      It is used in the warmboot recover. entry will not be written 
 *      in hardware. Only software cache will be se and refcount will 
 *      be maintained.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_l3_iif_profile_sw_state_set(int unit, void **entries,             
                              int entries_per_set, uint32 index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_sw_state_set(unit, L3_IIF_PROFILE(unit),
                             entries, entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}   
    
/*
 * Function:
 *      _bcm_l3_iif_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the L3_IIF_PROFILE table
 *      Adds an entry to the global shared SW copy of the L3_IIF_PROFILE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_l3_iif_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, L3_IIF_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_l3_iif_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the L3_IIF_PROFILE table
 *      Deletes an entry from the global shared SW copy of the L3_IIF_PROFILE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_l3_iif_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, L3_IIF_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_l3_iif_profile_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, L3_IIF_PROFILE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_l3_iif_profile_entry_reference
 * Purpose:
 *      Internal function for indicating an entry in the
 *      L3_IIF_PROFILE table is being used.
 *      Updates the global shared SW copy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_l3_iif_profile_entry_reference(int unit, int index, 
                                int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, L3_IIF_PROFILE(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_l3_iif_profile_entry_reference_get(int unit, int index, 
                                int *ref_count) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_ref_count_get(unit,
                              L3_IIF_PROFILE(unit),
                              index,
                              ref_count);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_l3_iif_profile_entry_update
 * Purpose:
 *      Internal function for updating an entry to the combined
 *      L3_IIF_PROFILE and L3_IIF_PROFILE table.
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      index   -  (IN) Base index to update
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_l3_iif_profile_entry_update(int unit, void **entries, 
                                     uint32 *index)
{
    int rv;
    int ref_count;
    void *entries_get[1];
    iif_profile_entry_t l3_iif_profile;

    /* Do not overwrite the default enrty */
    if (*index == 0) {
        return BCM_E_INTERNAL;
    }

    entries_get[0] = &l3_iif_profile;
    sal_memset(&l3_iif_profile, 0, sizeof(iif_profile_entry_t));
    BCM_IF_ERROR_RETURN
        (_bcm_l3_iif_profile_entry_get(unit, *index, 1, entries_get));

    if (!sal_memcmp(entries[0], &l3_iif_profile, sizeof(iif_profile_entry_t))) {
        /* Profile entry needs no update */
        return BCM_E_NONE;
    }

    COMMON_LOCK(unit);
   
    rv = soc_profile_mem_ref_count_get(unit, L3_IIF_PROFILE(unit), *index, &ref_count);
    if (SOC_FAILURE(rv)) {
        COMMON_UNLOCK(unit);
        return rv;
    }

    if (0 == ref_count) {
       /* Profile entry does not exist */ 
       /* coverity[assigned_value] */
       rv = BCM_E_NOT_FOUND;
    }
    /*
     * Check in profile table if the new entry matches with any other
     * entry. This is important otherwise we may have duplicate entries.
     * profile_add routine will take care of match and will add new entry if 
     * no match is found.
     *
     * The caller has the responsibility to delete the old entry after
     * the new entry is written in the hardware else remove new entry
     * as old entry is still used in hardware and return error to user.
     * 
     * if the following condition is true 
     * (entry not found + no space left in table + entry not shared by anyone)
     * then just overide the old entry.
     */
    rv = _bcm_l3_iif_profile_entry_add(unit, entries, 1, index);
    if (rv == BCM_E_RESOURCE) {

        if (1 == ref_count) { 
            /* Profile entry exist and not shared */
            rv = soc_profile_mem_set(unit, L3_IIF_PROFILE(unit), entries,
                                     *index);
        }
    }

    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_l3_ip4_options_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the L3_IP_OPTIONS_PROFILE table
 *      Adds an entry to the global shared SW copy of the L3_IP_OPTIONS_PROFILE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_l3_ip4_options_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, L3_IP4_OPTIONS_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_l3_ip4_options_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the L3_IP_OPTIONS_PROFILE table
 *      Deletes an entry from the global shared SW copy of the L3_IP_OPTIONS_PROFILE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_l3_ip4_options_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, L3_IP4_OPTIONS_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_l3_ip4_options_profile_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, L3_IP4_OPTIONS_PROFILE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_l3_ip4_options_profile_entry_reference
 * Purpose:
 *      Internal function for indicating an entry in the
 *      L3_IP_OPTIONS_PROFILE table is being used.
 *      Updates the global shared SW copy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_l3_ip4_options_profile_entry_reference(int unit, int index, 
                                int entries_per_set_override) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, L3_IP4_OPTIONS_PROFILE(unit), index, 
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_l3_ip4_options_profile_entry_update
 * Purpose:
 *      Internal function for updating an entry to the combined
 *      L3_IP_OPTIONS_PROFILE and L3_IP_OPTIONS_PROFILE table.
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      index   -  (OUT) Base index to update
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_l3_ip4_options_profile_entry_update(int unit, void **entries, 
                                     uint32 index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_set(unit, L3_IP4_OPTIONS_PROFILE(unit), entries,
                             index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_fc_map_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the FC_MAP_PROFILE table
 *      Adds an entry to the global shared SW copy of the FC_MAP_PROFILE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_fc_map_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, FC_MAP_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_fc_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the FC_MAP_PROFILE table
 *      Deletes an entry from the global shared SW copy of the FC_MAP_PROFILE
 *      table shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_fc_map_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, FC_MAP_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_fc_map_profile_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, FC_MAP_PROFILE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_fc_map_profile_entry_reference
 * Purpose:
 *      Internal function for indicating that an entry in FC_MAP_PROFILE table
 *      is being used. Updates the global shared SW copy.
 * Parameters:
 *      unit    -  (IN) Device number
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
 int
_bcm_fc_map_profile_entry_reference(int unit, int index,
                                    int entries_per_set_override)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, FC_MAP_PROFILE(unit), index,
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIDENT2PLUS_SUPPORT


/*
 * Function:
 *      _bcm_vfi_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the vfi_profile table
 *      Adds an entry to the global shared SW copy of the vfi_profile table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vfi_profile_entry_add(int unit, void **entries,
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, VFI_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_vfi_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the vfi_profile table
 *      Deletes an entry from the global shared SW copy of the vfi_profile table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vfi_profile_entry_delete(int unit, int index)
{
    int rv;

    /* index 0 should never be deleted */
    if (index == 0) {
        return BCM_E_NONE;
    }

    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, VFI_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_vfi_profile_entry_get
 * Purpose:
 *      Internal function for retrieving entry from the vfi_profile table
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be fetched
 *      count   -  (IN) Array size
 *      entries -  (OUT) Output array of entries
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vfi_profile_entry_get(int unit, int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, VFI_PROFILE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_vfi_profile_entry_reference
 * Purpose:
 *      Internal function for indicating an entry in the
 *      vfi_profile table is being used.
 *      Updates the global shared SW copy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vfi_profile_entry_reference(int unit, int index,
                                int entries_per_set_override)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, VFI_PROFILE(unit), index,
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}

#endif /* BCM_TRIDENT2PLUS_SUPPORT */


#ifdef BCM_KATANA_SUPPORT
/*
 * Function:
 *      _bcm_offset_map_table_entry_add
 * Purpose:
 *      Internal function for adding an entry to the 
 *      ING_QUEUE_OFFSET_MAPPING_TABLE table.
 *      Adds an entry to the global shared SW copy of the 
 *      ING_QUEUE_OFFSET_MAPPING_TABLE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */


int
_bcm_offset_map_table_entry_add(int unit, void **entries,
                                int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, OFFSET_MAP_TABLE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}
/*
 * Function:
 *      _bcm_offset_map_table_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the 
 *      ING_QUEUE_OFFSET_MAPPING_TABLE table
 *      Deletes an entry from the global shared SW copy of the 
 *      ING_QUEUE_OFFSET_MAPPING_TABLE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_offset_map_table_entry_delete(int unit, int index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, OFFSET_MAP_TABLE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_offset_map_table_entry_get
 * Purpose:
 *      Internal function for getting an entry from the 
 *      ING_QUEUE_OFFSET_MAPPING_TABLE table
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index  - (IN) Base index for the entry to be deleted
 *      count  - (IN) number of entries  
 *      entries - (OUT) enrtries
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_offset_map_table_entry_get(int unit, int index, int count, 
                                      void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, OFFSET_MAP_TABLE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_offset_map_table_entry_reference
 * Purpose:
 *      Internal function for indicating an entry in the 
 *      ING_QUEUE_OFFSET_MAPPING_TABLE table
 *      is being used. Updates the global shared SW copy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_offset_map_table_entry_reference(int unit, int index,
                                int entries_per_set_override)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, OFFSET_MAP_TABLE(unit), index,
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
/*
 * Function:
 *      _bcm_service_pri_map_table_entry_add
 * Purpose:
 *      Internal function for adding an entry to the 
 *      ING/EGR_SERVICE_PRI_MAP_0/1/2 tables.
 *      Adds an entry to the global shared SW copy of the 
 *      ING/EGR_SERVICE_PRI_MAP_0/1/2 tables
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */


int
_bcm_service_pri_map_table_entry_add(int unit, void **entries,
                                int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, SERVICE_PRI_MAP_TABLE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}
/*
 * Function:
 *      _bcm_service_pri_map_table_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the 
 *      ING/EGR_SERVICE_PRI_MAP_0/1/2 tables
 *      Deletes an entry from the global shared SW copy of the 
 *      ING/EGR_SERVICE_PRI_MAP_0/1/2 tables
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_service_pri_map_table_entry_delete(int unit, int index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, SERVICE_PRI_MAP_TABLE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_service_pri_map_table_entry_get
 * Purpose:
 *      Internal function for getting an entry from the 
 *      ING/EGR_SERVICE_PRI_MAP_0/1/2 tables
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index  - (IN) Base index for the entry to be deleted
 *      count  - (IN) number of entries  
 *      entries - (OUT) enrtries
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_service_pri_map_table_entry_get(int unit, int index, int count, 
                                      void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, SERVICE_PRI_MAP_TABLE(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_service_pri_map_table_entry_reference
 * Purpose:
 *      Internal function for indicating an entry in the 
 *      ING_QUEUE_OFFSET_MAPPING_TABLE table
 *      is being used. Updates the global shared SW copy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_service_pri_map_table_entry_reference(int unit, int index,
                                int entries_per_set_override)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, SERVICE_PRI_MAP_TABLE(unit), index,
                                   entries_per_set_override);
    COMMON_UNLOCK(unit);
    return rv;
}
#endif /* BCM_SABER2_SUPPORT */

/*TD2+ support is added here for xgs5/port.c usage*/
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
/*
 * Function:
 *      _bcm_port_vlan_protocol_data_entry_add
 * Purpose:
 *      Internal function for adding an entry to the 
 *      VLAN_PROTOCOL_DATA table.
 *      Adds an entry to the global shared SW copy of the 
 *      VLAN_PROTOCOL_DATA table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_port_vlan_protocol_data_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv = BCM_E_NONE;

    if (SOC_MEM_IS_VALID(unit, VLAN_PROTOCOL_DATAm)) {
        COMMON_LOCK(unit);
        rv = soc_profile_mem_add(unit, VLAN_PROTOCOL_DATA_TABLE(unit), entries,
                             entries_per_set, index);
        COMMON_UNLOCK(unit);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_port_vlan_protocol_data_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the 
 *      VLAN_PROTOCOL_DATA table
 *      Deletes an entry from the global shared SW copy of the 
 *      VLAN_PROTOCOL_DATA table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be deleted
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_port_vlan_protocol_data_entry_delete(int unit, int index)
{
    int rv = BCM_E_NONE;
    if (SOC_MEM_IS_VALID(unit, VLAN_PROTOCOL_DATAm)) {
        COMMON_LOCK(unit);
        rv = soc_profile_mem_delete(unit, VLAN_PROTOCOL_DATA_TABLE(unit), index);
        COMMON_UNLOCK(unit);
    }
    return rv;
}
#endif
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
/*
 * Function:
 *      _bcm_port_vlan_protocol_data_entry_get
 * Purpose:
 *      Internal function for getting an entry from the 
 *      VLAN_PROTOCOL_DATA table
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index  - (IN) Base index for the entry to be deleted
 *      count  - (IN) number of entries  
 *      entries - (OUT) enrtries
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_port_vlan_protocol_data_entry_get(int unit,
    int index, int count, void **entries)
{
    int rv = BCM_E_NONE;
    if (SOC_MEM_IS_VALID(unit, VLAN_PROTOCOL_DATAm)) {
        COMMON_LOCK(unit);
        rv = soc_profile_mem_get(unit, VLAN_PROTOCOL_DATA_TABLE(unit), index, count,
                             entries);
        COMMON_UNLOCK(unit);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_vlan_port_protocol_data_entry_reference
 * Purpose:
 *      Internal function for indicating an entry in the 
 *      VLAN_PROTOCOL_DATA table
 *      is being used. Updates the global shared SW copy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entry to be updated
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_port_vlan_protocol_data_entry_reference(int unit, int index,
                                int entries_per_set_override) 
{
    int rv = BCM_E_NONE;


    if (SOC_MEM_IS_VALID(unit, VLAN_PROTOCOL_DATAm)) {
        COMMON_LOCK(unit);
        rv = soc_profile_mem_reference(unit, VLAN_PROTOCOL_DATA_TABLE(unit), index, 
                                   entries_per_set_override);
        COMMON_UNLOCK(unit);
    }
    return rv;
}

#endif /*BCM_KATANA2_SUPPORT */

#ifdef BCM_HURRICANE3_SUPPORT
/*
 * Function:
 *      _bcm_egr_header_encap_data_entry_add
 * Purpose:
 *      Internal function for adding an entry to the EGR_HEADER_ENCAP_DATA table
 *      Adds an entry to the global shared SW copy of the EGR_HEADER_ENCAP_DATA table
 *      Shared by MIML and Custom Packet Header
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_header_encap_data_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_HEADER_ENCAP_DATA(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_header_encap_data_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the EGR_HEADER_ENCAP_DATA table
 *      Deletes an entry from the global shared SW copy of the EGR_HEADER_ENCAP_DATA table
 *      Shared by MIML and Custom Packet Header
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_header_encap_data_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_HEADER_ENCAP_DATA(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_header_encap_data_entry_update
 * Purpose:
 *      Internal function for updating an entry to theEGR_HEADER_ENCAP_DATA table.
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      index   -  (OUT) Base index to update
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_header_encap_data_entry_update(int unit, void **entries, 
                              uint32 index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_set(unit, EGR_HEADER_ENCAP_DATA(unit), entries, index);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_egr_header_encap_data_entry_ref_count_get(int unit, 
                              uint32 index, int *ref_count)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_ref_count_get(unit, 
        EGR_HEADER_ENCAP_DATA(unit), index, ref_count);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_egr_header_encap_data_entry_search(int unit, void **entries,
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_search(unit, EGR_HEADER_ENCAP_DATA(unit), entries,
                                entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_custom_header_match_entry_add
 * Purpose:
 *      Internal function for adding an entry to the CUSTOM_HEADER_MATCH table
 *      Adds an entry to the global shared SW copy of the CUSTOM_HEADER_MATCH table
 *      Shared by MIML and Custom Packet Header
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_custom_header_match_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, CUSTOM_HEADER_MATCH(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_custom_header_match_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the CUSTOM_HEADER_MATCH table
 *      Deletes an entry from the global shared SW copy of the CUSTOM_HEADER_MATCH table
 *      Shared by MIML and Custom Packet Header
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_custom_header_match_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, CUSTOM_HEADER_MATCH(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_custom_header_match_entry_update
 * Purpose:
 *      Internal function for updating an entry to theCUSTOM_HEADER_MATCH table.
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      index   -  (OUT) Base index to update
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_custom_header_match_entry_update(int unit, void **entries, 
                              uint32 index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_set(unit, CUSTOM_HEADER_MATCH(unit), entries, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_custom_header_match_entry_get
 * Purpose:
 *      Internal function for getting an entry to theCUSTOM_HEADER_MATCH table.
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index  - (IN) Base index for the entry to be deleted
 *      count  - (IN) number of entries  
 *      entries - (OUT) enrtries
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_custom_header_match_entry_get(int unit,
    int index, int count, void **entries)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_get(unit, CUSTOM_HEADER_MATCH(unit), index, count,
                             entries);
    COMMON_UNLOCK(unit);
    return rv;
}

int
_bcm_custom_header_match_entry_ref_count_get(int unit, 
                              uint32 index, int *ref_count)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_ref_count_get(unit, 
        CUSTOM_HEADER_MATCH(unit), index, ref_count);
    COMMON_UNLOCK(unit);
    return rv;
}

#endif /* BCM_HURRICANE3_SUPPORT */

void
_bcm_common_profile_mem_ref_cnt_update(int unit, soc_mem_t mem,
                                       int profile_ptr, int count)
{
    int entries_per_set, index, i;
    soc_profile_mem_t *profile;

    switch (mem) {
    case EGR_MAC_DA_PROFILEm:
        profile = MAC_DA_PROFILE(unit);
        entries_per_set = 1;
        break;
    case ING_PRI_CNG_MAPm:
        profile = ING_PRI_CNG_MAP(unit);
        entries_per_set = 16;
        break;
    case LPORT_TABm:
        profile = LPORT_PROFILE(unit);
        entries_per_set = 1;
        break;
    case EGR_MPLS_PRI_MAPPINGm:
    case EGR_MPLS_EXP_MAPPING_1m:
        profile = EGR_MPLS_COMBO_MAP(unit);
        entries_per_set = 64;
        break;
    case DSCP_TABLEm:
        profile = DSCP_TABLE(unit);
        entries_per_set = 64;
        break;
    case PHB_MAPPING_TBL_2m:
        profile = DSCP_TABLE(unit);
        entries_per_set = 64;
        break;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    case EGR_PRI_CNG_MAPm:
        if(soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
            profile = EGR_QOS_PROFILE(unit);
            entries_per_set = 64;
        } else {
            return;
        }
        break;
#endif
    case EGR_DSCP_TABLEm:
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if(soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
            profile = EGR_QOS_PROFILE(unit);
            entries_per_set = 64;
        } else
#endif
        {
            profile = EGR_DSCP_TABLE(unit);
            entries_per_set = 64;
        }
        break;
    case EGR_ZONE_2_QOS_MAPPING_TABLEm:
        profile = EGR_DSCP_TABLE(unit);
        entries_per_set = 64;
        break;
#ifdef BCM_TRIUMPH3_SUPPORT
    case ING_L3_NEXT_HOP_ATTRIBUTE_1m:
        profile = NULL;
        entries_per_set = 0;
        if (SOC_IS_TRIUMPH3(unit)) {
            profile = ING_L3_NH_ATTRIB_PROFILE(unit);
            entries_per_set = 1;
        }
        break;
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    case L3_IIF_PROFILEm:
        profile = NULL;
        entries_per_set = 0;
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            profile = L3_IIF_PROFILE(unit);
            entries_per_set = 1;
        }
        break;
    case IP_OPTION_CONTROL_PROFILE_TABLEm:
        profile = NULL;
        entries_per_set = 1;
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
            profile = L3_IP4_OPTIONS_PROFILE(unit);
            entries_per_set = 256;
        }
        break;
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    case ING_QUEUE_OFFSET_MAPPING_TABLEm:
        profile = OFFSET_MAP_TABLE(unit);
        entries_per_set = 16;
        break;
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    case VLAN_PROTOCOL_DATAm:
        profile = VLAN_PROTOCOL_DATA_TABLE(unit);
        entries_per_set = 16;
        break;
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_HURRICANE3_SUPPORT
    case EGR_HEADER_ENCAP_DATAm:
        profile = EGR_HEADER_ENCAP_DATA(unit);
        entries_per_set = 1;
        break;
    case CUSTOM_HEADER_MATCHm:
        profile = CUSTOM_HEADER_MATCH(unit);
        entries_per_set = 1;
        break;
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    case ING_TUNNEL_ECN_DECAPm:
        profile = ING_TUNNEL_TERM_MAP(unit);
        entries_per_set = 16;
        break;
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
    defined(BCM_TRIDENT3_SUPPORT)
    case EGR_INT_CN_TO_EXP_MAPPING_TABLEm:
        profile = EGR_INT_CN_TO_EXP_MAP(unit);
        entries_per_set = 32;
        break;
    case EGR_PKT_ECN_TO_EXP_MAPPING_1m:
        profile = EGR_IP_ECN_TO_EXP_MAP(unit);
        entries_per_set = 32;
        break;
    case EGR_IP_ECN_TO_EXP_MAPPING_TABLEm:
        profile = EGR_IP_ECN_TO_EXP_MAP(unit);
        entries_per_set = 32;
        break;
    case ING_EXP_TO_IP_ECN_MAPPINGm:
        profile = ING_EXP_TO_IP_ECN_MAP(unit);
        entries_per_set = 32;
        break;            
#endif /* BCM_TOMAHAWK2_SUPPORT||BCM_TOMAHAWK3_SUPPORT||BCM_TRIDENT3_SUPPORT */
    
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    case ING_EXP_TO_ECN_MAPPING_1m:
        profile = ING_EXP_TO_IP_ECN_MAP(unit);
        entries_per_set = 32;
        break;
#endif /* BCM_TOMAHAWK2_SUPPORT or BCM_TRIDENT3_SUPPORT */

    default:
        return;
    }

    index = profile_ptr * entries_per_set;
    for (i = 0; i < entries_per_set; i++) {
        SOC_PROFILE_MEM_REFERENCE(unit, profile, index + i, count);
        SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, profile, index + i, 
                                        entries_per_set);
    }
    return;
}

/* Structure used for hash calculation */
typedef struct _bcm_trx_mpls_station_key_s {
    bcm_mac_t  mac;
    bcm_vlan_t vlan;
} _bcm_trx_mpls_station_key_t;

/*
 * Function:
 *      _bcm_trx_mpls_station_hash_calc
 * Purpose:
 *      Calculate MPLS_STATION_TCAM entry hash.
 * Parameters:
 *      unit -  (IN)  Device number.
 *      mac  -  (IN)  MAC address
 *      vlan -  (IN)  VLAN ID
 *      hash -  (OUT) Hash valu.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_trx_mpls_station_hash_calc(int unit, bcm_mac_t mac,
                                bcm_vlan_t vlan, uint16 *hash)
{
    _bcm_trx_mpls_station_key_t key;

    if (hash == NULL) {
        return (BCM_E_PARAM);
    }
    sal_memcpy(key.mac, mac, sizeof(bcm_mac_t));
    key.vlan = vlan;
    *hash = _shr_crc16(0, (uint8 *)&key, sizeof(_bcm_trx_mpls_station_key_t));

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_l2_tunnel_add
 * Purpose:
 *    Add a (MAC, VLAN) for tunnel/MPLS processing.
 *    Frames destined to (MAC, VLAN) is subjected to MPLS processing.
 *    Shared by MPLS and MIM
 * Parameters:
 *      unit - Unit number
 *      mac  - MAC address
 *      vlan - VLAN ID
 */
int
bcm_trx_metro_l2_tunnel_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    mpls_station_tcam_entry_t station_entry;
    uint16 hash;
    bcm_mac_t hw_mac;
    bcm_vlan_t hw_vlan;
    int rv, i, num_station, free_index = -1;

    /* Get the hash value for this key */
    BCM_IF_ERROR_RETURN(_bcm_trx_mpls_station_hash_calc(unit, mac, vlan, &hash));

    /* see if the entry already exists or find free entry */
    num_station = soc_mem_index_count(unit, MPLS_STATION_TCAMm);
    COMMON_LOCK(unit);
    for (i = 0; i < num_station; i++) {
        if (hash == MPLS_STATION_HASH(unit, i)) {
            /* Read HW entry to see if it's really a match */
            rv = READ_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY,
                                         i, &station_entry);
            if (rv < 0) {
                COMMON_UNLOCK(unit);
                return rv;
            }
            soc_mem_mac_addr_get(unit, MPLS_STATION_TCAMm,
                                 &station_entry, MAC_ADDRf, hw_mac);
            hw_vlan = soc_mem_field32_get(unit, MPLS_STATION_TCAMm,
                                          &station_entry, VLAN_IDf);
            if ((vlan == hw_vlan) &&
                !sal_memcmp(mac, hw_mac, sizeof(bcm_mac_t))) {
                break;
            }
        }
        if ((free_index == -1) && (MPLS_STATION_HASH(unit, i) == 0)) {
            /* Read HW entry to see if it's unused */
            rv = READ_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY,
                                         i, &station_entry);
            if (rv < 0) {
                COMMON_UNLOCK(unit);
                return rv;
            }
            if (!soc_mem_field32_get(unit, MPLS_STATION_TCAMm,
                                     &station_entry, VALIDf)) {
                free_index = i;
            }
        }
    }
    if (i < num_station) {
        /* Entry already exists */
        COMMON_UNLOCK(unit);
        return BCM_E_EXISTS;
    } else if (free_index == -1) {
        /* Table is full */
        COMMON_UNLOCK(unit);
        return BCM_E_FULL;
    }
    /* Commit entry to HW */
    sal_memset(&station_entry, 0, sizeof(mpls_station_tcam_entry_t));
    soc_mem_field32_set(unit, MPLS_STATION_TCAMm, &station_entry, VALIDf, 1);
    soc_mem_mac_addr_set(unit, MPLS_STATION_TCAMm, &station_entry,
                         MAC_ADDRf, mac);
    soc_mem_field32_set(unit, MPLS_STATION_TCAMm, &station_entry,
                        VLAN_IDf, vlan);
    /* set masks to all 1's */
    for (i = 0; i < 6; i++) {
        hw_mac[i] = 0xff;
    }
    hw_vlan = 0xfff;
    soc_mem_mac_addr_set(unit, MPLS_STATION_TCAMm, &station_entry,
                         MAC_ADDR_MASKf, hw_mac);
    soc_mem_field32_set(unit, MPLS_STATION_TCAMm, &station_entry,
                        VLAN_ID_MASKf, hw_vlan);
    rv = WRITE_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY,
                                  free_index, &station_entry);
    if (rv < 0) {
        COMMON_UNLOCK(unit);
        return rv;
    }
    MPLS_STATION_HASH(unit, free_index) = hash;
    COMMON_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_l2_tunnel_delete
 * Purpose:
 *    Delete a (MAC, VLAN) for tunnel/MPLS processing.
 *    Shared by MPLS and MIM
 * Parameters:
 *      unit - Unit number
 *      mac  - MAC address
 *      vlan - VLAN ID
 */
int
bcm_trx_metro_l2_tunnel_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    mpls_station_tcam_entry_t station_entry;
    uint16 hash;
    bcm_mac_t hw_mac;
    bcm_vlan_t hw_vlan;
    int rv, i, num_station;

    /* Get the hash value for this key */
    BCM_IF_ERROR_RETURN(_bcm_trx_mpls_station_hash_calc(unit, mac, vlan, &hash));

    /* find the entry to be deleted */
    num_station = soc_mem_index_count(unit, MPLS_STATION_TCAMm);
    COMMON_LOCK(unit);
    for (i = 0; i < num_station; i++) {
        if (hash == MPLS_STATION_HASH(unit, i)) {
            /* Read HW entry to see if it's really a match */
            rv = READ_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY,
                                         i, &station_entry);
            if (rv < 0) {
                COMMON_UNLOCK(unit);
                return rv;
            }
            soc_mem_mac_addr_get(unit, MPLS_STATION_TCAMm,
                                 &station_entry, MAC_ADDRf, hw_mac);
            hw_vlan = soc_mem_field32_get(unit, MPLS_STATION_TCAMm,
                                          &station_entry, VLAN_IDf);
            if ((vlan == hw_vlan) &&
                !sal_memcmp(mac, hw_mac, sizeof(bcm_mac_t))) {
                break;
            }
        }
    }
    if (i == num_station) {
        /* Entry not found */
        COMMON_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }
    /* Clear the entry in HW */
    sal_memset(&station_entry, 0, sizeof(mpls_station_tcam_entry_t));
    rv = WRITE_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY, i, &station_entry);
    if (rv < 0) {
        COMMON_UNLOCK(unit);
        return rv;
    }
    COMMON_UNLOCK(unit);
    MPLS_STATION_HASH(unit, i) = 0;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_l2_tunnel_delete_all
 * Purpose:
 *     Delete all (MAC, VLAN) for tunnel/MPLS processing.
 *    Shared by MPLS and MIM
 * Parameters:
 *      unit - Unit number
 */
int
bcm_trx_metro_l2_tunnel_delete_all(int unit)
{
    int rv, num_station;

    num_station = soc_mem_index_count(unit, MPLS_STATION_TCAMm);
    COMMON_LOCK(unit);
    rv = soc_mem_clear(unit, MPLS_STATION_TCAMm, COPYNO_ALL, 0);
    if (rv < 0) {
        COMMON_UNLOCK(unit);
        return rv;
    }
    sal_memset(COMMON_INFO(unit)->mpls_station_hash, 0,
               sizeof(uint16) * num_station);
    COMMON_UNLOCK(unit);
    return BCM_E_NONE;
}

#if defined(BCM_TRIUMPH2_SUPPORT)
/*
 * Flexible stats:
 * 
 * Ingress/egress packet/byte counters which may be assigned from a pool
 * to various packet categories.  There are two pools available:
 * Service interface types:  VLAN, VRF, VFI
 * Virtual interface types:  Some GPORT categories
 * The source table responsible for the packet classification holds an
 * index to the counter pool.  Otherwise, the implementation of the two
 * types is equivalent, so it is abstracted here.
 * This is implemented here because so many modules are touched by
 * this feature.
 */



static _bcm_flex_stat_info_t *fs_info[BCM_MAX_NUM_UNITS][_bcmFlexStatTypeAllHwNum];
static int _tr2_flex_stat_api_ver[BCM_MAX_NUM_UNITS] = {0};
static sal_mutex_t _flex_stat_mutex[BCM_MAX_NUM_UNITS] = {NULL};
static uint64 *_bcm_flex_stat_mem[BCM_MAX_NUM_UNITS][_bcmFlexStatTypeHwNum]
                                 [_BCM_FLEX_STAT_DIRS][_BCM_FLEX_STAT_GRANS];
static ing_service_counter_table_entry_t *_bcm_flex_stat_buff
                                 [BCM_MAX_NUM_UNITS][_bcmFlexStatTypeHwNum]
                                 [_BCM_FLEX_STAT_DIRS][_BCM_FLEX_STAT_BUFFS];
#ifdef BCM_TRIDENT_SUPPORT
static ing_service_counter_table_entry_t *_bcm_flex_stat_buff_y
                                 [BCM_MAX_NUM_UNITS][_bcmFlexStatTypeHwNum]
                                 [_BCM_FLEX_STAT_DIRS][_BCM_FLEX_STAT_BUFFS];
#endif /* BCM_TRIDENT_SUPPORT */
static uint32 _bcm_flex_stat_mem_types[] = {
    ING_SERVICE_COUNTER_TABLEm,
    EGR_SERVICE_COUNTER_TABLEm,
    ING_VINTF_COUNTER_TABLEm,
    EGR_VINTF_COUNTER_TABLEm
};
static uint8 _bcm_flex_stat_buff_toggle[BCM_MAX_NUM_UNITS] = {0};

static soc_memacc_t *_bcm_flex_stat_memacc[BCM_MAX_NUM_UNITS];

/* The gport, fp, MPLS label and both vxlt types are currently
 * implemented in the same HW. Here we collapse them into the same
 * info array for tracking.
 * We need the different types to distinguish the different handles.
 */
#define FS_TYPE_TO_HW_TYPE(unit, type) \
        (((type == _bcmFlexStatTypeService) || \
          (type == _bcmFlexStatTypeVrf))? _bcmFlexStatTypeService : \
          (type == _bcmFlexStatTypeEgressService)? \
          _bcmFlexStatTypeEgressService: \
         (type == _bcmFlexStatTypeEgressGport || \
          type == _bcmFlexStatTypeEgrVxlt)? \
           _bcmFlexStatTypeEgressGport: _bcmFlexStatTypeGport)

#define FS_TYPE_TO_SVC_VINTF_HW_TYPE(unit, type) \
        (((type == _bcmFlexStatTypeService) || \
          (type == _bcmFlexStatTypeVrf) || \
          (type == _bcmFlexStatTypeEgressService))? \
         _bcmFlexStatTypeService : \
         _bcmFlexStatTypeGport)

#define FS_INFO(unit, hw_type) \
            fs_info[unit][hw_type]

#define FS_MEM_SIZE(unit, hw_type)     FS_INFO(unit, hw_type)->stat_mem_size
#define FS_USED_BITMAP(unit, hw_type)  FS_INFO(unit, hw_type)->stats
#define FS_HANDLE_LIST_SIZE(unit, hw_type)  \
                            FS_INFO(unit, hw_type)->handle_list_size
#define FS_HANDLE_LIST(unit, hw_type)  FS_INFO(unit, hw_type)->handle_list
#define FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, list_index) \
            (&(FS_INFO(unit, hw_type)->handle_list[list_index]))
#define FS_HANDLE_LIST_ELEMENT_HANDLE(unit, hw_type, list_index) \
            ((FS_INFO(unit, hw_type)->handle_list[list_index].handle))
#define FS_HANDLE_LIST_ELEMENT_TYPE(unit, hw_type, list_index) \
            ((FS_INFO(unit, hw_type)->handle_list[list_index].type))
#define FS_HANDLE_LIST_ELEMENT_INDEX(unit, hw_type, list_index) \
            ((FS_INFO(unit, hw_type)->handle_list[list_index].index))
#define FS_HANDLE_LIST_ELEMENT_SIZE \
                                  sizeof(_bcm_flex_stat_handle_to_index_t)
#define FS_REF_CNT_ELEMENT(_u, _htype, _inx) \
            (FS_INFO((_u), (_htype))->ref_cnt[(_inx)])

/* Below, the macros work with the FS type */
/* type takes ingress hardware types */
#define FS_ING_MEM(type)         \
        ((FS_TYPE_TO_HW_TYPE(unit, type) == _bcmFlexStatTypeService) ? \
                ING_SERVICE_COUNTER_TABLEm : ING_VINTF_COUNTER_TABLEm)
/* type takes egress hardware types */
#define FS_EGR_MEM(type)         \
        (((FS_TYPE_TO_HW_TYPE(unit, type) == \
                        _bcmFlexStatTypeEgressService) || \
         (FS_TYPE_TO_HW_TYPE(unit, type) == _bcmFlexStatTypeService)) ? \
                EGR_SERVICE_COUNTER_TABLEm : EGR_VINTF_COUNTER_TABLEm)

/* 
 * Common resource lock
 */
#define FS_LOCK(unit) \
        sal_mutex_take(_flex_stat_mutex[unit], sal_mutex_FOREVER);

#define FS_UNLOCK(unit) \
        sal_mutex_give(_flex_stat_mutex[unit]); 

#define FS_INIT(unit, type) \
        if (FS_INFO(unit, FS_TYPE_TO_HW_TYPE(unit, type)) == NULL) { \
            FS_UNLOCK(unit); \
            return BCM_E_INIT; \
        }

#define TR2_FLEX_STAT_API_LEGACY    1
#define TR2_FLEX_STAT_API_NEW       2

#ifdef BCM_WARM_BOOT_SUPPORT

STATIC int
_bcm_common_wb_scache_size_get_tr2_flex_stat(int unit, int *req_scache_size)
{
    int alloc_size = 0;

    alloc_size += sizeof (_tr2_flex_stat_api_ver);

    *req_scache_size += alloc_size;

    return BCM_E_NONE;

}

/* Syncs _tr2_flex_stat_api_ver into scache */
STATIC int
_bcm_common_wb_tr2_flex_stat_sync(int unit, uint8 **scache_ptr)
{
    int i;
    uint32 *u32_scache_p;

    u32_scache_p = (uint32 *)(*scache_ptr);

    for (i = 0; i < BCM_MAX_NUM_UNITS; i++)
    {
        *u32_scache_p = _tr2_flex_stat_api_ver[i];
        u32_scache_p++;
    }

    *scache_ptr = (uint8 *)u32_scache_p;

    return BCM_E_NONE;
}

/* Recovers _tr2_flex_stat_api_ver from scache  */
STATIC int
_bcm_common_wb_tr2_flex_stat_reinit(int unit, uint8 **scache_ptr)
{
    int i;

    uint32 *u32_scache_p;

    u32_scache_p = (uint32 *)(*scache_ptr);

    for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
        _tr2_flex_stat_api_ver[i] = *u32_scache_p;
        u32_scache_p++;

    }
    *scache_ptr = (uint8 *)u32_scache_p;

    return BCM_E_NONE;
}

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
STATIC int
_bcm_common_wb_scache_size_get_my_station_shadow_mask(int unit, int *req_scache_size)
{
    int alloc_size = 0;

    alloc_size += sizeof (my_station_shadow_mask_t);

    *req_scache_size += alloc_size;

    return BCM_E_NONE;

}

STATIC int
_bcm_common_wb_my_station_shadow_mask_sync(int unit, uint8 **scache_ptr)
{
    int i;
    uint32 *u32_scache_p;
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);

    if (info->my_station_shadow_mask == NULL) {
        return BCM_E_NONE;
    }

    u32_scache_p = (uint32 *)(*scache_ptr);

    for (i = 0; i < (MY_STATION_SHADOW_MASK_MAX_ARRAY_SIZE+31)/32; i++)
    {
        *u32_scache_p = info->my_station_shadow_mask->buf[i];
        u32_scache_p++;
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        for (i = 0; i < (MY_STATION_SHADOW_MASK_MAX_ARRAY_SIZE+31)/32; i++)
        {
            *u32_scache_p = info->my_station2_shadow_mask->buf[i];
            u32_scache_p++;
        }
    }
#endif
    *scache_ptr = (uint8 *)u32_scache_p;

    return BCM_E_NONE;
}

STATIC int
_bcm_common_wb_my_station_shadow_mask_reinit(int unit, int is_station2, uint8 **scache_ptr)
{
    int i;

    uint32 *u32_scache_p;
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);

    if (info->my_station_shadow_mask == NULL) {
        return BCM_E_NONE;
    }

    u32_scache_p = (uint32 *)(*scache_ptr);

    for (i = 0; i < (MY_STATION_SHADOW_MASK_MAX_ARRAY_SIZE+31)/32; i++) {
        if (!is_station2) {
        info->my_station_shadow_mask->buf[i] = *u32_scache_p;
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        else if (SOC_IS_TRIDENT3X(unit)){
            info->my_station2_shadow_mask->buf[i] = *u32_scache_p;
        }
#endif
        u32_scache_p++;

    }
    *scache_ptr = (uint8 *)u32_scache_p;

    return BCM_E_NONE;
}
#endif
#endif /* BCM_WARM_BOOT_SUPPORT */


/* Allocate Flex counter table SW caches and fetch buffers */
STATIC int
_bcm_esw_flex_stat_mem_init(int unit)
{
    int i /* types */, j /* dir */, k /* grans/buffs */;
    int alloc_size, rv = BCM_E_NONE; 
    soc_mem_t mem;
    for (i = 0; i < _bcmFlexStatTypeHwNum; i++) {
        for (j = 0; j < _BCM_FLEX_STAT_DIRS; j++) {
            mem = _bcm_flex_stat_mem_types[(i * _BCM_FLEX_STAT_DIRS) + j];
            for (k = 0; k < _BCM_FLEX_STAT_GRANS; k++) {
                alloc_size = SOC_MEM_SIZE(unit, mem) * sizeof(uint64);
                _bcm_flex_stat_mem[unit][i][j][k] = 
                    sal_alloc(alloc_size, "Flexible counters cache");
                if (NULL == _bcm_flex_stat_mem[unit][i][j][k]) {
                    LOG_CLI((BSL_META_U(unit,
                                        "bcm_esw_flex_stat_mem_init: Mem alloc failed - "
                                        "unit: %d, type: %d, dir: %d, gran: %d\n"),
                             unit, i, j, k));
                    return BCM_E_MEMORY;
                }
                sal_memset(_bcm_flex_stat_mem[unit][i][j][k], 0, alloc_size);
            }
        }
    }
    for (i = 0; i < _bcmFlexStatTypeHwNum; i++) {
        for (j = 0; j < _BCM_FLEX_STAT_DIRS; j++) {
            mem = _bcm_flex_stat_mem_types[(i * _BCM_FLEX_STAT_DIRS) + j];
            alloc_size = SOC_MEM_TABLE_BYTES(unit, mem);
            for (k = 0; k < _BCM_FLEX_STAT_BUFFS; k++) {
                _bcm_flex_stat_buff[unit][i][j][k] = \
                    soc_cm_salloc(unit, alloc_size, "Flexible counters buff");
                if (NULL == _bcm_flex_stat_buff[unit][i][j][k]) {
                    LOG_CLI((BSL_META_U(unit,
                                        "bcm_esw_flex_stat_mem_init: Mem alloc failed - "
                                        "unit: %d, type: %d, dir: %d, buff: %d\n"),
                             unit, i, j, k));
                    LOG_CLI((BSL_META_U(unit,
                                        "Mem alloc failed.\n")));
                    return BCM_E_MEMORY;
                }
                sal_memset(_bcm_flex_stat_buff[unit][i][j][k], 0, alloc_size);
#ifdef BCM_TRIDENT_SUPPORT
                if (SOC_IS_TD_TT(unit)) {
                    _bcm_flex_stat_buff_y[unit][i][j][k] = \
                        soc_cm_salloc(unit, alloc_size, 
                                      "Flexible counters buff_y");
                    if (NULL == _bcm_flex_stat_buff_y[unit][i][j][k]) {
                            return BCM_E_MEMORY;
                    }
                    sal_memset(_bcm_flex_stat_buff_y[unit][i][j][k], 0, 
                               alloc_size);
                }
#endif /* BCM_TRIDENT_SUPPORT */
            }
        } 
    }
    return rv;
}

/* Free Flex counter table SW caches and fetch buffers */
STATIC void
_bcm_esw_flex_stat_mem_free(int unit)
{
    int i /* types */, j /* dir */, k /* grans/buffs */;
    for (i = 0; i < _bcmFlexStatTypeHwNum; i++) {
        for (j = 0; j < _BCM_FLEX_STAT_DIRS; j++) {
            for (k = 0; k < _BCM_FLEX_STAT_GRANS; k++) {
                sal_free(_bcm_flex_stat_mem[unit][i][j][k]);
            }
        }
    }
    for (i = 0; i < _bcmFlexStatTypeHwNum; i++) {
        for (j = 0; j < _BCM_FLEX_STAT_DIRS; j++) {
            for (k = 0; k < _BCM_FLEX_STAT_BUFFS; k++) {
                soc_cm_sfree(unit, _bcm_flex_stat_buff[unit][i][j][k]);
#ifdef BCM_TRIDENT_SUPPORT
                if (SOC_IS_TD_TT(unit)) {
                    soc_cm_sfree(unit, _bcm_flex_stat_buff_y[unit][i][j][k]);
                }
#endif /* BCM_TRIDENT_SUPPORT */
            }
        }
    }
}

typedef struct _bcm_fs_memacc_map_s {
    soc_mem_t mem;
    soc_field_t fld;
    int type_index;
} _bcm_fs_memacc_map_t;

static _bcm_fs_memacc_map_t _bcm_tr2_fs_memacc_map[] = {
    {ING_SERVICE_COUNTER_TABLEm, PACKET_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_ING_PKT},
    {ING_SERVICE_COUNTER_TABLEm, BYTE_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_ING_BYTE},
    {EGR_SERVICE_COUNTER_TABLEm, PACKET_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_EGR_PKT},
    {EGR_SERVICE_COUNTER_TABLEm, BYTE_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_EGR_BYTE},
    {ING_VINTF_COUNTER_TABLEm, PACKET_COUNTERf,
     _BCM_FS_MEMACC_VINTF_ING_PKT},
    {ING_VINTF_COUNTER_TABLEm, BYTE_COUNTERf,
     _BCM_FS_MEMACC_VINTF_ING_BYTE},
    {EGR_VINTF_COUNTER_TABLEm, PACKET_COUNTERf,
     _BCM_FS_MEMACC_VINTF_EGR_PKT},
    {EGR_VINTF_COUNTER_TABLEm, BYTE_COUNTERf,
     _BCM_FS_MEMACC_VINTF_EGR_BYTE},
};

#ifdef BCM_TRIDENT_SUPPORT
static _bcm_fs_memacc_map_t _bcm_td_fs_memacc_map[] = {
    {ING_SERVICE_COUNTER_TABLE_Xm, PACKET_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_ING_PKT},
    {ING_SERVICE_COUNTER_TABLE_Xm, BYTE_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_ING_BYTE},
    {EGR_SERVICE_COUNTER_TABLE_Xm, PACKET_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_EGR_PKT},
    {EGR_SERVICE_COUNTER_TABLE_Xm, BYTE_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_EGR_BYTE},
    {ING_VINTF_COUNTER_TABLE_Xm, PACKET_COUNTERf,
     _BCM_FS_MEMACC_VINTF_ING_PKT},
    {ING_VINTF_COUNTER_TABLE_Xm, BYTE_COUNTERf,
     _BCM_FS_MEMACC_VINTF_ING_BYTE},
    {EGR_VINTF_COUNTER_TABLE_Xm, PACKET_COUNTERf,
     _BCM_FS_MEMACC_VINTF_EGR_PKT},
    {EGR_VINTF_COUNTER_TABLE_Xm, BYTE_COUNTERf,
     _BCM_FS_MEMACC_VINTF_EGR_BYTE},

    {ING_SERVICE_COUNTER_TABLE_Ym, PACKET_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_ING_PKT + _BCM_FS_MEMACC_NUM},
    {ING_SERVICE_COUNTER_TABLE_Ym, BYTE_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_ING_BYTE + _BCM_FS_MEMACC_NUM},
    {EGR_SERVICE_COUNTER_TABLE_Ym, PACKET_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_EGR_PKT + _BCM_FS_MEMACC_NUM},
    {EGR_SERVICE_COUNTER_TABLE_Ym, BYTE_COUNTERf,
     _BCM_FS_MEMACC_SERVICE_EGR_BYTE + _BCM_FS_MEMACC_NUM},
    {ING_VINTF_COUNTER_TABLE_Ym, PACKET_COUNTERf,
     _BCM_FS_MEMACC_VINTF_ING_PKT + _BCM_FS_MEMACC_NUM},
    {ING_VINTF_COUNTER_TABLE_Ym, BYTE_COUNTERf,
     _BCM_FS_MEMACC_VINTF_ING_BYTE + _BCM_FS_MEMACC_NUM},
    {EGR_VINTF_COUNTER_TABLE_Ym, PACKET_COUNTERf,
     _BCM_FS_MEMACC_VINTF_EGR_PKT + _BCM_FS_MEMACC_NUM},
    {EGR_VINTF_COUNTER_TABLE_Ym, BYTE_COUNTERf,
     _BCM_FS_MEMACC_VINTF_EGR_BYTE + _BCM_FS_MEMACC_NUM},
};
#endif /*  BCM_TRIDENT_SUPPORT */

STATIC int
_bcm_esw_flex_stat_memacc_init(int unit)
{
    int alloc_size, rv = BCM_E_NONE;
    _bcm_fs_memacc_map_t *memacc_map = _bcm_tr2_fs_memacc_map;
    int mam_ix, memacc_map_size = _BCM_FS_MEMACC_NUM;

    alloc_size = (_BCM_FS_MEMACC_NUM * sizeof(soc_memacc_t));

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        /* X & Y pipes */
        alloc_size *= 2;
        memacc_map = _bcm_td_fs_memacc_map;
        memacc_map_size = 2 * _BCM_FS_MEMACC_NUM;
    }
#endif /* BCM_TRIDENT_SUPPORT */

    _bcm_flex_stat_memacc[unit] =
        sal_alloc(alloc_size, "Flexible counters memacc data");
    if (NULL == _bcm_flex_stat_memacc[unit]) {
        rv = BCM_E_MEMORY;
    }

    for (mam_ix = 0; mam_ix < memacc_map_size; mam_ix++) {
        if (BCM_SUCCESS(rv)) {
            rv = soc_memacc_init(unit, memacc_map[mam_ix].mem,
                                 memacc_map[mam_ix].fld,
                                 &(_bcm_flex_stat_memacc[unit]
                                   [memacc_map[mam_ix].type_index]));
        }
        if (BCM_SUCCESS(rv) &&
            (0 == SOC_MEMACC_FIELD_LENGTH(&(_bcm_flex_stat_memacc[unit]
                                         [memacc_map[mam_ix].type_index])))) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_esw_flex_stat_memacc_init: " 
                                    "Invalid memacc field info.\n")));
            rv = BCM_E_INTERNAL;
        }
    }
    return rv;
}

/* Callback to be invoked from counter thread */
void
_bcm_esw_flex_stat_collection(int unit,
                              _bcm_flex_stat_type_t type,
                              int fs_idx, _bcm_flex_stat_t stat)
{
    int i = 0 /* types */, j = 0 /* dir */, k = 0 /* index */, max = 0;
    soc_mem_t mem;
    ing_service_counter_table_entry_t *curr_ptr = NULL;
    ing_service_counter_table_entry_t *prev_ptr = NULL;
    ing_service_counter_table_entry_t *curr_eptr = NULL;
    ing_service_counter_table_entry_t *prev_eptr = NULL;
    ing_service_counter_table_entry_t flex_ctr_entry;
    uint64 *prev_pktptr, *prev_bytptr;
    int rv = BCM_E_NONE;
    soc_memacc_t *memacc_pkt = NULL;
    soc_memacc_t *memacc_byte = NULL;
    int field_length_byte = 0;
    int field_length_packet = 0;
    int max_stat_types, max_dirs;
    int min_dirs = 0;
    int min_stat_types, min_index;
    uint64 curr, prev, diff, size; 
    SHR_BITDCL *stat_bmp;
#ifdef BCM_TRIDENT_SUPPORT
    ing_service_counter_table_entry_t *curr_ptr_y = NULL; 
    ing_service_counter_table_entry_t *prev_ptr_y = NULL;
    ing_service_counter_table_entry_t *curr_eptr_y = NULL;
    ing_service_counter_table_entry_t *prev_eptr_y = NULL;
    ing_service_counter_table_entry_t flex_ctr_entry_y;
    soc_mem_t mem_x = 0, mem_y = 0;
    soc_memacc_t *memacc_pkt_x = NULL;
    soc_memacc_t *memacc_byte_x = NULL;
    soc_memacc_t *memacc_pkt_y = NULL;
    soc_memacc_t *memacc_byte_y = NULL;
    int field_length_byte_x = 0;
    int field_length_packet_x = 0;
    int field_length_byte_y = 0;
    int field_length_packet_y = 0;
    uint64 diff_y;
#endif /* BCM_TRIDENT_SUPPORT */

    /*
     * If fs_idx!= -1 is The request is to sync a specific statistics 
     * sw count with hw count.
     */
    if (fs_idx != -1) {
        /* set type, dir, index for a specific stat to retrieved */
        min_stat_types  = FS_TYPE_TO_SVC_VINTF_HW_TYPE(unit, type);
        max_stat_types = min_stat_types + 1;

        if (stat > _bcmFlexStatIngressBytes) {
            /* egress */
            min_dirs = 1;
        }

        min_index = fs_idx;
        max_dirs = min_dirs + 1;
    } else {
        /* retrieve all the flex stats */    
        min_stat_types = 0;
        max_stat_types = _bcmFlexStatTypeHwNum;
        min_dirs = 0;
        max_dirs = _BCM_FLEX_STAT_DIRS;
        min_index = 0;
    }

    /* For each table fetch stats from h/w */
    for (i = min_stat_types; i < max_stat_types; i++) {
        for (j = min_dirs; j < max_dirs; j++) {
            /* Take and release lock to permit other threads'
             * activity in between table scans. */
            FS_LOCK(unit);
            if (_tr2_flex_stat_api_ver[unit] == TR2_FLEX_STAT_API_NEW) {
                stat_bmp = FS_USED_BITMAP(unit,i+(_bcmFlexStatTypeHwNum * j));
            } else {
                stat_bmp = FS_USED_BITMAP(unit, i);
            }
            mem = _bcm_flex_stat_mem_types[(i * _BCM_FLEX_STAT_DIRS) + j];
            curr_ptr = _bcm_flex_stat_buff[unit][i][j]
                                          [_bcm_flex_stat_buff_toggle[unit]];
            if (NULL == curr_ptr) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "bcm_esw_flex_stat_callback: " 
                                        "Bad mem ptr - unit: %d, type: %d, dir: %d\n"),
                             unit, i, j));
                /* we are in a callback; no point continuing */
                FS_UNLOCK(unit);
                return;
            }
#ifdef BCM_TRIDENT_SUPPORT
            if (SOC_IS_TD_TT(unit)) {
                curr_ptr_y = _bcm_flex_stat_buff_y[unit][i][j]
                                 [_bcm_flex_stat_buff_toggle[unit]];
                if (NULL == curr_ptr_y) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "bcm_esw_flex_stat_callback: " 
                                            "Bad mem ptr - unit: %d, type: %d, dir: %d\n"),
                                 unit, i, j));
                    /* we are in a callback; no point continuing */
                    FS_UNLOCK(unit);
                    return;
                }
                switch (mem) {
                case ING_SERVICE_COUNTER_TABLEm:
                    mem_x = ING_SERVICE_COUNTER_TABLE_Xm;
                    memacc_pkt_x = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_ING_PKT]);
                    memacc_byte_x = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_ING_BYTE]);
                    mem_y = ING_SERVICE_COUNTER_TABLE_Ym;
                    memacc_pkt_y = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_ING_PKT +
                         _BCM_FS_MEMACC_NUM]);
                    memacc_byte_y = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_ING_BYTE +
                         _BCM_FS_MEMACC_NUM]);
                    break;
                case EGR_SERVICE_COUNTER_TABLEm:
                    mem_x = EGR_SERVICE_COUNTER_TABLE_Xm; 
                    memacc_pkt_x = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_EGR_PKT]);
                    memacc_byte_x = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_EGR_BYTE]);
                    mem_y = EGR_SERVICE_COUNTER_TABLE_Ym; 
                    memacc_pkt_y = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_EGR_PKT +
                         _BCM_FS_MEMACC_NUM]);
                    memacc_byte_y = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_EGR_BYTE +
                         _BCM_FS_MEMACC_NUM]);
                    break;
                case ING_VINTF_COUNTER_TABLEm:
                    mem_x = ING_VINTF_COUNTER_TABLE_Xm; 
                    memacc_pkt_x = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_ING_PKT]);
                    memacc_byte_x = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_ING_BYTE]);
                    mem_y = ING_VINTF_COUNTER_TABLE_Ym; 
                    memacc_pkt_y = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_ING_PKT +
                         _BCM_FS_MEMACC_NUM]);
                    memacc_byte_y = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_ING_BYTE +
                         _BCM_FS_MEMACC_NUM]);
                    break;
                case EGR_VINTF_COUNTER_TABLEm:
                    mem_x = EGR_VINTF_COUNTER_TABLE_Xm; 
                    memacc_pkt_x = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_EGR_PKT]);
                    memacc_byte_x = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_EGR_BYTE]);
                    mem_y = EGR_VINTF_COUNTER_TABLE_Ym; 
                    memacc_pkt_y = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_EGR_PKT +
                         _BCM_FS_MEMACC_NUM]);
                    memacc_byte_y = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_EGR_BYTE +
                         _BCM_FS_MEMACC_NUM]);
                    break;
                default:
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "bcm_esw_flex_stat_callback: " 
                                            "Bad mem type (%d) - unit: %d, "
                                            "type: %d, dir: %d\n"), mem, unit, i, j));
                    /* we are in a callback; no point continuing */
                    FS_UNLOCK(unit);
                    return;
                }

                /*
                 * If fs_idx!= -1 is The request is to sync a specific statistic 
                 * sw count with hw count. fs_idx is the index of the flex stat 
                 * to be read. issue a mem_read only for the index and process 
                 * the count.
                 */
                if (fs_idx != -1) {
                    rv = soc_mem_read(unit, mem_x, MEM_BLOCK_ANY,
                                      fs_idx, &flex_ctr_entry);
                    if (rv < 0) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                   (BSL_META_U(unit,
                                               "bcm_esw_flex_stat_callback: " 
                                               "Mem read failed.\n")));
                        /* we are in a callback; no point continuing */
                        FS_UNLOCK(unit);
                        return;
                    } 

                    /* Dual pipe, read from X pipe and Y pipe then add */
                    rv = soc_mem_read(unit, mem_y, MEM_BLOCK_ANY,
                                      fs_idx, &flex_ctr_entry_y);

                    if (rv < 0) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                   (BSL_META_U(unit,
                                               "bcm_esw_flex_stat_callback: " 
                                               "Mem read failed.\n")));
                        /* we are in a callback; no point continuing */
                        FS_UNLOCK(unit);
                        return;
                    } 
                    /* read and process specific index */
                    max = k + 1;
                } else {

                    rv = soc_mem_read_range(unit, mem_x, MEM_BLOCK_ANY, 
                                            soc_mem_index_min(unit, mem_x), 
                                            soc_mem_index_max(unit, mem_x), 
                                            curr_ptr);
                    if (rv < 0) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                   (BSL_META_U(unit,
                                               "bcm_esw_flex_stat_callback: " 
                                               "Mem read failed.\n")));
                        /* we are in a callback; no point continuing */
                        FS_UNLOCK(unit);
                        return;
                    } 
                    rv = soc_mem_read_range(unit, mem_y, MEM_BLOCK_ANY, 
                                            soc_mem_index_min(unit, mem_y), 
                                            soc_mem_index_max(unit, mem_y), 
                                            curr_ptr_y);
                    if (rv < 0) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                   (BSL_META_U(unit,
                                               "bcm_esw_flex_stat_callback: " 
                                               "Mem read failed.\n")));
                        /* we are in a callback; no point continuing */
                        FS_UNLOCK(unit);
                        return;
                    } 
                    max = soc_mem_index_max(unit, mem_x);
                }

                field_length_packet_x =
                    SOC_MEMACC_FIELD_LENGTH(memacc_pkt_x);
                field_length_byte_x =
                    SOC_MEMACC_FIELD_LENGTH(memacc_byte_x);
                field_length_packet_y =
                    SOC_MEMACC_FIELD_LENGTH(memacc_pkt_y);
                field_length_byte_y =
                    SOC_MEMACC_FIELD_LENGTH(memacc_byte_y);
            } else 
#endif /* BCM_TRIDENT_SUPPORT */
            {
                switch (mem) {
                case ING_SERVICE_COUNTER_TABLEm:
                    memacc_pkt = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_ING_PKT]);
                    memacc_byte = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_ING_BYTE]);
                    break;
                case EGR_SERVICE_COUNTER_TABLEm:
                    memacc_pkt = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_EGR_PKT]);
                    memacc_byte = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_SERVICE_EGR_BYTE]);
                    break;
                case ING_VINTF_COUNTER_TABLEm:
                    memacc_pkt = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_ING_PKT]);
                    memacc_byte = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_ING_BYTE]);
                    break;
                case EGR_VINTF_COUNTER_TABLEm:
                    memacc_pkt = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_EGR_PKT]);
                    memacc_byte = &(_bcm_flex_stat_memacc[unit]
                        [_BCM_FS_MEMACC_VINTF_EGR_BYTE]);
                    break;
                default:
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "bcm_esw_flex_stat_callback: " 
                                            "Bad mem type (%d) - unit: %d, "
                                            "type: %d, dir: %d\n"), mem, unit, i, j));
                    /* we are in a callback; no point continuing */
                    FS_UNLOCK(unit);
                    return;
                }

                /*
                 * If fs_idx!= -1 is The request is to sync a specific statistic 
                 * sw count with hw count. fs_idx is the index of the flex stat 
                 * to be read. issue a mem_read only for the index and process 
                 * the count.
                 */
                if (fs_idx != -1) {
                    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                      fs_idx, &flex_ctr_entry);
                    if (rv < 0) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                   (BSL_META_U(unit,
                                               "bcm_esw_flex_stat_callback: " 
                                               "Mem read failed.\n")));
                        /* we are in a callback; no point continuing */
                        FS_UNLOCK(unit);
                        return;
                    } 

                    /* fetching only 1 index */
                    max = k + 1;
                } else {
                    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, 
                                            soc_mem_index_min(unit, mem), 
                                            soc_mem_index_max(unit, mem), 
                                            curr_ptr);
                    if (rv < 0) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                   (BSL_META_U(unit,
                                               "bcm_esw_flex_stat_callback: " 
                                               "Mem read failed.\n")));
                        /* we are in a callback; no point continuing */
                        FS_UNLOCK(unit);
                        return;
                    } 

                    max = soc_mem_index_max(unit, mem);
                }
                field_length_packet = SOC_MEMACC_FIELD_LENGTH(memacc_pkt);
                field_length_byte = SOC_MEMACC_FIELD_LENGTH(memacc_byte);
            }

            /* Iterate through and check/get diffs */
            prev_ptr = _bcm_flex_stat_buff[unit][i][j]
                                          [!_bcm_flex_stat_buff_toggle[unit]];
#ifdef BCM_TRIDENT_SUPPORT
            if (SOC_IS_TD_TT(unit)) {
                prev_ptr_y = _bcm_flex_stat_buff_y[unit][i][j]
                                 [!_bcm_flex_stat_buff_toggle[unit]];
            }
#endif /* BCM_TRIDENT_SUPPORT */
            prev_pktptr = _bcm_flex_stat_mem[unit][i][j][0];
            prev_bytptr = _bcm_flex_stat_mem[unit][i][j][1];
            for (k = min_index ; k < max; k++) {
                if (!SHR_BITGET(stat_bmp, k)) {
                    /* Stat not enabled, don't bother to collect */
                    continue;
                }

                /* DMA memory entry arithmetic */ 
                if (fs_idx == -1) { 
                    curr_eptr = soc_mem_table_idx_to_pointer(unit,
                                            ING_SERVICE_COUNTER_TABLEm,
                                            ing_service_counter_table_entry_t *,
                                                             curr_ptr, k);
                } else {
                    /* stat retrieval for a specific index */
                    curr_eptr = &flex_ctr_entry;
                }

                prev_eptr = soc_mem_table_idx_to_pointer(unit,
                                        ING_SERVICE_COUNTER_TABLEm,
                                        ing_service_counter_table_entry_t *,
                                                         prev_ptr, k);
#ifdef BCM_TRIDENT_SUPPORT
                if (SOC_IS_TD_TT(unit)) {
                    if (fs_idx == -1) { 
                        curr_eptr_y = soc_mem_table_idx_to_pointer(unit,
                                            ING_SERVICE_COUNTER_TABLEm,
                                            ing_service_counter_table_entry_t *,
                                            curr_ptr_y, k);
                    } else {
                        /* stat retrieval for a specific index */
                        curr_eptr_y = &flex_ctr_entry_y;
                    }
                    prev_eptr_y = soc_mem_table_idx_to_pointer(unit,
                                        ING_SERVICE_COUNTER_TABLEm,
                                        ing_service_counter_table_entry_t *,
                                            prev_ptr_y, k);
                }
#endif /* BCM_TRIDENT_SUPPORT */

                /* first packets */
#ifdef BCM_TRIDENT_SUPPORT
                if (SOC_IS_TD_TT(unit)) {
                    soc_memacc_field64_get(memacc_pkt_x,
                                           (uint32 *)curr_eptr, &curr);
                    soc_memacc_field64_get(memacc_pkt_x,
                                           (uint32 *)prev_eptr, &prev);
                    /* Take care of rollovers */
                    if (COMPILER_64_LT(curr, prev)) {
                       COMPILER_64_SET(size, 0, 1);
                       COMPILER_64_SHL(size, field_length_packet_x);
                       COMPILER_64_ADD_64(curr, size); 
                    }
                    diff = curr;
                    COMPILER_64_SUB_64(diff, prev);

                    soc_memacc_field64_get(memacc_pkt_y,
                                           (uint32 *)curr_eptr_y, &curr);
                    soc_memacc_field64_get(memacc_pkt_y,
                                           (uint32 *)prev_eptr_y, &prev);
                    /* Take care of rollovers */
                    if (COMPILER_64_LT(curr, prev)) {
                       COMPILER_64_SET(size, 0, 1);
                       COMPILER_64_SHL(size, field_length_packet_y);
                       COMPILER_64_ADD_64(curr, size); 
                    }
                    diff_y = curr;         
                    COMPILER_64_SUB_64(diff_y, prev);
                    COMPILER_64_ADD_64(diff, diff_y);
                } else
#endif /* BCM_TRIDENT_SUPPORT */
                {
                    soc_memacc_field64_get(memacc_pkt,
                                           (uint32 *)curr_eptr, &curr);
                    soc_memacc_field64_get(memacc_pkt,
                                           (uint32 *)prev_eptr, &prev);
                    /* Take care of rollovers */
                    if (COMPILER_64_LT(curr, prev)) {
                       COMPILER_64_SET(size, 0, 1);
                       COMPILER_64_SHL(size, field_length_packet);
                       COMPILER_64_ADD_64(curr, size); 
                    }
                    diff = curr;                    
                    COMPILER_64_SUB_64(diff, prev);
                }
                if (!COMPILER_64_IS_ZERO(diff)) {
                    /* Update soft copy */
                    COMPILER_64_ADD_64(prev_pktptr[k], diff);
                }

                /* then bytes */
#ifdef BCM_TRIDENT_SUPPORT
                if (SOC_IS_TD_TT(unit)) {
                    soc_memacc_field64_get(memacc_byte_x,
                                           (uint32 *)curr_eptr,
                                           &curr);
                    soc_memacc_field64_get(memacc_byte_x,
                                           (uint32 *)prev_eptr,
                                           &prev);
                    /* Take care of rollovers */
                    if (COMPILER_64_LT(curr, prev)) {
                       COMPILER_64_SET(size, 0, 1);
                       COMPILER_64_SHL(size, field_length_byte_x);
                       COMPILER_64_ADD_64(curr, size); 
                    }
                    diff = curr;
                    COMPILER_64_SUB_64(diff, prev);

                    soc_memacc_field64_get(memacc_byte_y,
                                           (uint32 *)curr_eptr_y,
                                           &curr);
                    soc_memacc_field64_get(memacc_byte_y,
                                           (uint32 *)prev_eptr_y,
                                           &prev);
                    /* Take care of rollovers */
                    if (COMPILER_64_LT(curr, prev)) {
                       COMPILER_64_SET(size, 0, 1);
                       COMPILER_64_SHL(size, field_length_byte_y);
                       COMPILER_64_ADD_64(curr, size); 
                    }
                    diff_y = curr;
                    COMPILER_64_SUB_64(diff_y, prev);
                    COMPILER_64_ADD_64(diff, diff_y);
                } else
#endif /* BCM_TRIDENT_SUPPORT */
                {
                    soc_memacc_field64_get(memacc_byte,
                                           (uint32 *)curr_eptr,
                                           &curr);
                    soc_memacc_field64_get(memacc_byte,
                                           (uint32 *)prev_eptr,
                                           &prev);
                    /* Take care of rollovers */
                    if (COMPILER_64_LT(curr, prev)) {
                       COMPILER_64_SET(size, 0, 1);
                       COMPILER_64_SHL(size, field_length_byte);
                       COMPILER_64_ADD_64(curr, size); 
                    }
                    diff = curr;                    
                    COMPILER_64_SUB_64(diff, prev);
                }
                if (!COMPILER_64_IS_ZERO(diff)) {
                    /* Update soft copy */
                    COMPILER_64_ADD_64(prev_bytptr[k], diff);
                }
            }
            FS_UNLOCK(unit);
        }
    }

    /* Flip toggle with lock */
    FS_LOCK(unit);
    _bcm_flex_stat_buff_toggle[unit] = !_bcm_flex_stat_buff_toggle[unit];
    FS_UNLOCK(unit);
}

/* Callback to be invoked from counter thread */
void
_bcm_esw_flex_stat_callback(int unit)
{

    /* Flip toggle with lock */
    FS_LOCK(unit);
    _bcm_esw_flex_stat_collection(unit, 0, -1, 0);
    FS_UNLOCK(unit);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_detach
 * Purpose:
 *      De-allocate memory used by flexible counters.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_detach(int unit, _bcm_flex_stat_type_t fs_type)
{
    _bcm_flex_stat_info_t  *stat_info;
    _bcm_flex_stat_type_t type_idx,
        hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);
    int mutex = TRUE;

    if (fs_type >= _bcmFlexStatTypeAllHwNum) {
        /* We shouldn't be deallocating this */
        return BCM_E_INTERNAL;
    }

    if (NULL != _flex_stat_mutex[unit]) {
        FS_LOCK(unit);
    }

    stat_info = FS_INFO(unit, hw_type);

    if (stat_info != NULL) {
        if (stat_info->stats != NULL) {
            sal_free(stat_info->stats);
        }
        if (stat_info->handle_list != NULL) {
            sal_free(stat_info->handle_list);
        }
        if (stat_info->ref_cnt != NULL) {
            sal_free(stat_info->ref_cnt);
        }
        sal_free(stat_info);
        FS_INFO(unit, hw_type) = NULL;
    }

    if (NULL != _flex_stat_mutex[unit]) {
        FS_UNLOCK(unit);
        /* Check if all counter types are detached */
        for (type_idx = _bcmFlexStatTypeService;
             type_idx < _bcmFlexStatTypeAllHwNum; type_idx ++) {
            if (FS_INFO(unit, type_idx) != NULL) {
                mutex = FALSE;
                break;
            }
        }
        /* If so, unregister, free the memory and destroy the mutex also */
        if (mutex) {
            soc_counter_extra_unregister(unit, _bcm_esw_flex_stat_callback);
            _bcm_esw_flex_stat_mem_free(unit);
            if (NULL != _bcm_flex_stat_memacc[unit]) {
                sal_free(_bcm_flex_stat_memacc[unit]);
                _bcm_flex_stat_memacc[unit] = NULL;
            }
            sal_mutex_destroy(_flex_stat_mutex[unit]);
            _flex_stat_mutex[unit] = NULL;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_init
 * Purpose:
 *      Allocate memory used by flexible counters.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_init(int unit, _bcm_flex_stat_type_t fs_type)
{
    _bcm_flex_stat_info_t  *stat_info = NULL;
    int alloc_size, rv = BCM_E_NONE;
    soc_mem_t mem;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    if (fs_type >= _bcmFlexStatTypeAllHwNum) {
        /* We shouldn't be allocating this */
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_flex_stat_detach(unit, fs_type));

    /* Without the lock and memory, we can't do anything */
    if (_flex_stat_mutex[unit] == NULL) {
        _flex_stat_mutex[unit] = sal_mutex_create("common mutex");
        if (_flex_stat_mutex[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        /* Allocate cache and buffer memory */
        rv = _bcm_esw_flex_stat_mem_init(unit);
        if (BCM_SUCCESS(rv)) {
            /* Predetermined memory access accelerators */
            rv = _bcm_esw_flex_stat_memacc_init(unit);
        }
        if (BCM_SUCCESS(rv)) {
            /* Register for counter callback */
            rv = soc_counter_extra_register(unit, _bcm_esw_flex_stat_callback);
        }
    }

    FS_LOCK(unit);

    if (BCM_SUCCESS(rv)) {
        alloc_size = sizeof(_bcm_flex_stat_info_t);
        stat_info = sal_alloc(alloc_size, "Flexible counters info");
        if (NULL == stat_info) {
            rv = BCM_E_MEMORY;
        } else {
            sal_memset(stat_info, 0, alloc_size);
            mem = FS_ING_MEM(fs_type);
            stat_info->stat_mem_size = soc_mem_index_count(unit, mem);
        }
    }

    if (BCM_SUCCESS(rv)) {
        alloc_size = SHR_BITALLOCSIZE(stat_info->stat_mem_size);
        stat_info->stats =
            sal_alloc(alloc_size, "Flexible counters usage bitmap");
        if (NULL == stat_info->stats) {
            rv = BCM_E_MEMORY;
        } else {
            sal_memset(stat_info->stats, 0, alloc_size);
            SHR_BITSET(stat_info->stats, 0); /* Index 0 is invalid */
        }
    }

    if (BCM_SUCCESS(rv)) {
        alloc_size = (FS_HANDLE_LIST_ELEMENT_SIZE *
                      stat_info->stat_mem_size);
        stat_info->handle_list =
            sal_alloc(alloc_size, "Flexible counters handle list");
        if (NULL == stat_info->handle_list) {
            rv = BCM_E_MEMORY;
        } else {
            sal_memset(stat_info->handle_list, 0, alloc_size);
            stat_info->handle_list_size = 0;
        }
    }

    if (BCM_SUCCESS(rv)) {
        alloc_size = (sizeof(uint16) *
                      stat_info->stat_mem_size);
        stat_info->ref_cnt =
            sal_alloc(alloc_size, "Flexible counters reference table");
        if (NULL == stat_info->ref_cnt) {
            rv = BCM_E_MEMORY;
        } else {
            sal_memset(stat_info->ref_cnt, 0, alloc_size);
        }
    }

    FS_INFO(unit, hw_type) = stat_info;

    if (!SOC_WARM_BOOT(unit)) {
        if (_tr2_flex_stat_api_ver[unit]) {
            _tr2_flex_stat_api_ver[unit] = 0;
        }
    }
    FS_UNLOCK(unit);
    if (BCM_FAILURE(rv)) {
        /* Clean up */
        (void)_bcm_esw_flex_stat_detach(unit, fs_type);
        /* We already have an error in rv */
    }
    return rv;
}

/*
 * Function:
 *      _bcm_flex_stat_handle_to_index_t_compare
 * Purpose:
 *      Compare two _bcm_flex_stat_handle_to_index_t elements for 
 *      binary table sort.
 * Parameters:
 *      a - (IN) pointer to first _bcm_flex_stat_handle_to_index_t
 *      b - (IN) pointer to second _bcm_flex_stat_handle_to_index_t
 * Returns:
 *      a<=>b
 */
STATIC int
_bcm_flex_stat_handle_to_index_t_compare(void *a, void *b)
{
    _bcm_flex_stat_handle_to_index_t *first, *second;
    int word;

    first = (_bcm_flex_stat_handle_to_index_t *)a;
    second = (_bcm_flex_stat_handle_to_index_t *)b;

    if (first->type < second->type) {
        return (-1);
    } else if (first->type > second->type) {
        return (1);
    }

    /* Types equal, check handle */
    for (word = (_BCM_FLEX_STAT_HANDLE_WORDS - 1); word >= 0; word--) {
        if (_BCM_FLEX_STAT_HANDLE_WORD_GET(first->handle, word) <
            _BCM_FLEX_STAT_HANDLE_WORD_GET(second->handle, word)) {
            return (-1);
        } else if (_BCM_FLEX_STAT_HANDLE_WORD_GET(first->handle, word) >
                   _BCM_FLEX_STAT_HANDLE_WORD_GET(second->handle, word)) {
            return (1);
        }
    }

    return (0);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_index
 * Purpose:
 *      Retrieve the flexible stat index for the given handle and stat type.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 * Returns:
 *      Flexible stat index
 */
STATIC int
_bcm_esw_flex_stat_index(int unit, _bcm_flex_stat_type_t fs_type,
                         _bcm_flex_stat_handle_t handle)
{
    int list_index;
    _bcm_flex_stat_handle_to_index_t target;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    _BCM_FLEX_STAT_HANDLE_ASSIGN(target.handle, handle);
    target.type = fs_type;
    list_index = _shr_bsearch(FS_HANDLE_LIST(unit, hw_type),
                              FS_HANDLE_LIST_SIZE(unit, hw_type),
                              FS_HANDLE_LIST_ELEMENT_SIZE, &target,
                              &_bcm_flex_stat_handle_to_index_t_compare);

    if ((list_index >= 0) &&
        (list_index < FS_HANDLE_LIST_SIZE(unit, hw_type))) {
        return FS_HANDLE_LIST_ELEMENT_INDEX(unit, hw_type, list_index);
    }
    return 0;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_handle_list_insert
 * Purpose:
 *      Add the flexible stat index for the given handle and stat type
 *      to the handle list.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      index - flexible stat index
 * Returns:
 *      Flexible stat index, or 0 on resource failure
 */
STATIC int
_bcm_esw_flex_stat_handle_list_insert(int unit, _bcm_flex_stat_type_t fs_type,
                                _bcm_flex_stat_handle_t handle, uint32 index)
{
    int list_index, idx, tmp;
    _bcm_flex_stat_handle_to_index_t target;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    if (FS_HANDLE_LIST_SIZE(unit, hw_type) >= FS_MEM_SIZE(unit, hw_type)) {
        
        return 0;
    }

    _BCM_FLEX_STAT_HANDLE_ASSIGN(target.handle, handle);
    target.type = fs_type;
    list_index = _shr_bsearch(FS_HANDLE_LIST(unit, hw_type),
                              FS_HANDLE_LIST_SIZE(unit, hw_type),
                              FS_HANDLE_LIST_ELEMENT_SIZE, &target,
                              &_bcm_flex_stat_handle_to_index_t_compare);


    if (list_index >= 0) {
        /* FIXME: It already exists!  This should have been caught
         * earlier.  Again, we have a big problem in the index management.
         */
        return 0;
    }

    idx = (((-1) * list_index) - 1);
    tmp = FS_HANDLE_LIST_SIZE(unit, hw_type) - 1;
    while (tmp >= idx) {
        sal_memcpy(FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp + 1),
                   FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp),
                   FS_HANDLE_LIST_ELEMENT_SIZE);
        tmp--;
    }
    _BCM_FLEX_STAT_HANDLE_ASSIGN
        (FS_HANDLE_LIST_ELEMENT_HANDLE(unit, hw_type, idx), handle);
    FS_HANDLE_LIST_ELEMENT_TYPE(unit, hw_type, idx) = fs_type;
    FS_HANDLE_LIST_ELEMENT_INDEX(unit, hw_type, idx) = index;
    FS_HANDLE_LIST_SIZE(unit, hw_type)++;
    return index;
}

STATIC int
_bcm_esw_flex_stat_index_set(int unit, _bcm_flex_stat_type_t fs_type,
                               uint32 index)
{
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    SHR_BITSET(FS_USED_BITMAP(unit, hw_type), index);
    return index;
}

STATIC uint32
_bcm_esw_flex_stat_index_get(int unit, _bcm_flex_stat_type_t fs_type,
                               uint32 index)
{
    uint32 ret;

    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    ret = SHR_BITGET(FS_USED_BITMAP(unit, hw_type), index);
    return ret;
}

STATIC int
_bcm_esw_flex_stat_ref_cnt_dec(int unit, _bcm_flex_stat_type_t fs_type,
                               uint32 fs_inx)
{
    int ret = BCM_E_NONE;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    if (FS_REF_CNT_ELEMENT(unit,hw_type,fs_inx)) {
        FS_REF_CNT_ELEMENT(unit,hw_type,fs_inx) -= 1;
    } else {
        ret = BCM_E_PARAM;
    }

    return ret;
}

STATIC uint16
_bcm_esw_flex_stat_ref_cnt_get(int unit, _bcm_flex_stat_type_t fs_type,
                               uint32 fs_inx)
{
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    return FS_REF_CNT_ELEMENT(unit,hw_type,fs_inx);
}

STATIC int
_bcm_esw_flex_stat_ref_cnt_inc(int unit, _bcm_flex_stat_type_t fs_type,
                               uint32 fs_inx)
{
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);
    int rv = BCM_E_NONE;

    if (!FS_REF_CNT_ELEMENT(unit,hw_type,fs_inx)) {
        /* if first time, clear both hardware/software count */
        rv = _bcm_esw_flex_stat_sw_clear(unit, fs_type, fs_inx);
    }
    FS_REF_CNT_ELEMENT(unit,hw_type,fs_inx) += 1;
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_index_insert
 * Purpose:
 *      Add the flexible stat index for the given handle and stat type
 *      to the handle list.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      index - flexible stat index
 * Returns:
 *      Flexible stat index, or 0 on resource failure
 */
STATIC int
_bcm_esw_flex_stat_index_insert(int unit, _bcm_flex_stat_type_t fs_type,
                                _bcm_flex_stat_handle_t handle, uint32 index)
{
    int rv;

    rv = _bcm_esw_flex_stat_handle_list_insert(unit,fs_type,handle,index);
    if (!rv) {
        return 0;
    }
    return _bcm_esw_flex_stat_index_set(unit,fs_type,index);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_handle_list_delete
 * Purpose:
 *      Remove the flexible stat index for the given handle and stat type
 *      from the handle list.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      index - flexible stat index
 * Returns:
 *      Flexible stat index, or 0 on resource failure
 */
STATIC void
_bcm_esw_flex_stat_handle_list_delete(int unit, _bcm_flex_stat_type_t fs_type,
                                _bcm_flex_stat_handle_t handle)
{
    int list_index, max, tmp;
    _bcm_flex_stat_handle_to_index_t target;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    _BCM_FLEX_STAT_HANDLE_ASSIGN(target.handle, handle);
    target.type = fs_type;
    list_index = _shr_bsearch(FS_HANDLE_LIST(unit, hw_type),
                              FS_HANDLE_LIST_SIZE(unit, hw_type),
                              FS_HANDLE_LIST_ELEMENT_SIZE, &target,
                              &_bcm_flex_stat_handle_to_index_t_compare);

    if (list_index < 0) {
        
        return;
    }

    tmp = list_index;
    max = FS_HANDLE_LIST_SIZE(unit, hw_type) - 1;
    while (tmp < max) {
        sal_memcpy(FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp),
                   FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp + 1),
                   FS_HANDLE_LIST_ELEMENT_SIZE);
        tmp++;
    }
    FS_HANDLE_LIST_SIZE(unit, hw_type)--;
    sal_memset(FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type,
                                      FS_HANDLE_LIST_SIZE(unit, hw_type)),
               0, FS_HANDLE_LIST_ELEMENT_SIZE);
    return;
}

STATIC void
_bcm_esw_flex_stat_index_clear(int unit, _bcm_flex_stat_type_t fs_type,
                                uint32 index)
{
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);
    SHR_BITCLR(FS_USED_BITMAP(unit, hw_type), index);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_index_delete
 * Purpose:
 *      Remove the flexible stat index for the given handle and stat type
 *      from the handle list.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      index - flexible stat index
 * Returns:
 *      Flexible stat index, or 0 on resource failure
 */
STATIC void
_bcm_esw_flex_stat_index_delete(int unit, _bcm_flex_stat_type_t fs_type,
                                _bcm_flex_stat_handle_t handle, uint32 index)
{
     _bcm_esw_flex_stat_handle_list_delete(unit,fs_type,handle);
     _bcm_esw_flex_stat_index_clear(unit,fs_type,index);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_next_free_index
 * Purpose:
 *      Retrieve the next available flexible stat index for a stat type.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 * Returns:
 *      Flexible stat index
 */
STATIC int
_bcm_esw_flex_stat_next_free_index(int unit, _bcm_flex_stat_type_t fs_type)
{
    int                 ix, mem_size;
    uint32              bit;
    SHR_BITDCL          not_ptrs, *stat_bmp;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    mem_size = FS_MEM_SIZE(unit, hw_type);
    stat_bmp = FS_USED_BITMAP(unit, hw_type);
    for (ix = 0; ix < _SHR_BITDCLSIZE(mem_size); ix++) {
        not_ptrs = ~stat_bmp[ix];
        if (not_ptrs) {
            for (bit = 0; bit < SHR_BITWID; bit++) {
                if (not_ptrs & (1 << bit)) {
                    return (ix * SHR_BITWID) + bit;
                }
            }
        }
    }

    return 0;
}

STATIC int
_bcm_esw_flex_stat_hw_index_set(int unit, _bcm_flex_stat_type_t fs_type,
            int fs_inx, uint64 val)
{
    int rv;
    _bcm_flex_stat_type_t htype;

    htype = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    if ((htype == _bcmFlexStatTypeService) ||
        (htype == _bcmFlexStatTypeGport)) {
        rv = _bcm_esw_flex_stat_hw_set(unit, fs_type, fs_inx,
                      _bcmFlexStatIngressPackets, val);
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_esw_flex_stat_hw_set(unit, fs_type, fs_inx,
                      _bcmFlexStatIngressBytes, val);
        }
    } else  {
        rv = _bcm_esw_flex_stat_hw_set(unit, fs_type, fs_inx,
                      _bcmFlexStatEgressPackets, val);
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_esw_flex_stat_hw_set(unit, fs_type, fs_inx,
                      _bcmFlexStatEgressBytes, val);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_free_index_assign
 * Purpose:
 *      get the next available flexible stat index for a stat type.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 * Returns:
 *      Flexible stat index
 */
int
_bcm_esw_flex_stat_free_index_assign(int unit, _bcm_flex_stat_type_t fs_type)
{
    int fs_inx;
    int rv;
    uint64 val64;

    COMPILER_64_SET(val64,0,1);
    FS_LOCK(unit);
    fs_inx = _bcm_esw_flex_stat_next_free_index(unit,fs_type);
    fs_inx = _bcm_esw_flex_stat_index_set(unit,fs_type,fs_inx);
    FS_UNLOCK(unit);

    /* set a initial value 1 to the hardware counter to indicate
     * the counter has been reserved, but not used yet, for warmboot
     * purpose.
     */
    rv = _bcm_esw_flex_stat_hw_index_set(unit, fs_type, fs_inx, val64);

    if (BCM_SUCCESS(rv)) {
        return fs_inx;  
    } else {
        return 0;
    }
}

/*
 * Function:
 *      _bcm_esw_flex_stat_free_index_assign
 * Purpose:
 *      get the next available flexible stat index for a stat type.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 * Returns:
 *      Flexible stat index
 */
int
_bcm_esw_flex_stat_count_index_remove(int unit, _bcm_flex_stat_type_t fs_type,
                 int fs_inx)
{
    int rv;
    uint16 ref_cnt;
    uint64 val64;

    COMPILER_64_SET(val64,0,0);
    rv = BCM_E_NONE;
    FS_LOCK(unit);
    ref_cnt = _bcm_esw_flex_stat_ref_cnt_get(unit,fs_type,fs_inx);
    if (!ref_cnt) {
        _bcm_esw_flex_stat_index_clear(unit,fs_type,fs_inx);
    }  else {
         rv = BCM_E_PARAM;
    }
    FS_UNLOCK(unit);

    /* clear the hardware counter */
    if (rv == BCM_E_NONE) {
        rv = _bcm_esw_flex_stat_hw_index_set(unit, fs_type, fs_inx, val64);
    }
    return rv;

}

/*
 * Function:
 *      _bcm_esw_flex_stat_alloc
 * Purpose:
 *      Attempt to allocate a flexible stat index for a stat type.
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 * Returns:
 *      Success: Flexible stat index
 *      Failure: 0 (invalid stat index)
 */
STATIC int
_bcm_esw_flex_stat_alloc(int unit, _bcm_flex_stat_type_t type,
                         _bcm_flex_stat_handle_t handle)
{
    int index;

    /* Already checked for existence */
    index = _bcm_esw_flex_stat_next_free_index(unit, type);
    if (index) {
        /* Returns 0 on error */
        index = _bcm_esw_flex_stat_index_insert(unit, type, handle, index);
    }

    return index;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_release_handles
 * Purpose:
 *      Deallocate all handles of the given stat type.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 * Returns:
 *      Flexible stat index
 */
void
_bcm_esw_flex_stat_release_handles(int unit, _bcm_flex_stat_type_t fs_type)
{
    int list_index, idx, tmp, max;
    _bcm_flex_stat_handle_to_index_t target;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    /* Find start of this type */
    _BCM_FLEX_STAT_HANDLE_CLEAR(target.handle); 
    target.type = fs_type;
    
    FS_LOCK(unit);

    max = FS_HANDLE_LIST_SIZE(unit, hw_type);
    if (max == 0) {
        FS_UNLOCK(unit);
        return;
    }
    list_index = _shr_bsearch(FS_HANDLE_LIST(unit, hw_type), max,
                              FS_HANDLE_LIST_ELEMENT_SIZE, &target,
                              &_bcm_flex_stat_handle_to_index_t_compare);

    /* Starting point */
    if (list_index < 0) {
        idx = (((-1) * list_index) - 1);
    } else {
        idx = list_index;
    }

    tmp = idx;
    while (tmp < max) {
        if (FS_HANDLE_LIST_ELEMENT_TYPE(unit, hw_type, tmp) != fs_type) {
            if (tmp != idx) {
                /* Copy up entry */
                sal_memcpy(FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, idx),
                           FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp),
                           FS_HANDLE_LIST_ELEMENT_SIZE);
                idx++;
            }
        }
        if (tmp != idx) {
            /* Blank out current position */
            sal_memset(FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp),
                       0, FS_HANDLE_LIST_ELEMENT_SIZE);
        }
        tmp++;
    }
    if (idx < tmp) {
        FS_HANDLE_LIST_SIZE(unit, hw_type) = idx + 1;
    }
    FS_UNLOCK(unit);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_reinit_add
 * Purpose:
 *      Record the flexible stat information during reinitialization.
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      fs_index - the flexible stat index
 *      handle - the encoded handle for the type of packets to count.
 * Returns:
 *      BCM_X_XXX
 */
void
_bcm_esw_flex_stat_ext_reinit_add(int unit, _bcm_flex_stat_type_t type,
                                  int fs_index,
                                  _bcm_flex_stat_handle_t handle)
{
    FS_LOCK(unit);
    (void)_bcm_esw_flex_stat_index_insert(unit, type, handle, fs_index);
    FS_UNLOCK(unit);
    /* Ignoring insert return code */
}

/*
 * Function:
 *      _bcm_esw_vlan_flex_stat_hw_index_set
 * Purpose:
 *      Update the flexible stat pointers of HW tables for VLAN/VFI types.
 * Parameters:
 *      unit - SOC unit number
 *      handle - the encoded handle for the type of packets to count.
 *      fs_idx - the flexible stat index
 *      cookie - info to be used from original calling routine
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_vlan_flex_stat_hw_index_set(int unit, _bcm_flex_stat_handle_t fsh,
                                     int fs_idx, void *cookie)
{
    bcm_vlan_t handle = _BCM_FLEX_STAT_HANDLE_WORD_GET(fsh, 0);
    uint32 flags;
#if defined(INCLUDE_L3)
    int vfi;
#endif /* INCLUDE_L3 */

    flags = cookie==NULL? _BCM_FLEX_STAT_HW_BOTH:
            PTR_TO_INT(cookie);
#if defined(INCLUDE_L3)

    /* Is it a VPN? */
    if (_BCM_MPLS_VPN_IS_SET(handle)) {
        _BCM_MPLS_VPN_GET(vfi, _BCM_MPLS_VPN_TYPE_VPLS, handle);
        return _bcm_vfi_flex_stat_index_set(unit, vfi,
                                            _bcmVfiTypeMpls, fs_idx,flags);
    } else if (_BCM_MIM_VPN_IS_SET(handle)) {
        _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, handle);
        return _bcm_vfi_flex_stat_index_set(unit, vfi,
                                            _bcmVfiTypeMim, fs_idx,flags);
    } else 
#endif /* INCLUDE_L3 */
    {
        return _bcm_esw_vlan_flex_stat_index_set(unit, handle, fs_idx,flags);
    }    
}

/*
 * Function:
 *      _bcm_esw_port_flex_stat_hw_index_set
 * Purpose:
 *      Update the flexible stat pointers of HW tables for GPORT types.
 * Parameters:
 *      unit - SOC unit number
 *      handle - the encoded handle for the type of packets to count.
 *      fs_idx - the flexible stat index
 *      cookie - info to be used from original calling routine
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_port_flex_stat_hw_index_set(int unit, _bcm_flex_stat_handle_t fsh,
                                     int fs_idx, void *cookie)
{
    uint32 flags;
    uint32 handle = _BCM_FLEX_STAT_HANDLE_WORD_GET(fsh, 0);

    COMPILER_REFERENCE(cookie);

    flags = cookie==NULL? _BCM_FLEX_STAT_HW_BOTH:
            PTR_TO_INT(cookie);

#if defined(INCLUDE_L3)
    if (BCM_GPORT_IS_MPLS_PORT(handle)) {
#if defined(BCM_MPLS_SUPPORT)
        return _bcm_esw_mpls_flex_stat_index_set(unit, handle, fs_idx,flags);
#else
        return BCM_E_UNAVAIL;
#endif /* BCM_MPLS_SUPPORT */
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(handle) ||
               BCM_GPORT_IS_SUBPORT_PORT(handle)) {
        return _bcm_esw_subport_flex_stat_index_set(unit, handle, fs_idx,flags);
    } else if (BCM_GPORT_IS_MIM_PORT(handle)) {
        return _bcm_esw_mim_flex_stat_index_set(unit, handle, fs_idx,flags);
    } else if (BCM_GPORT_IS_WLAN_PORT(handle)) {
        return _bcm_esw_wlan_flex_stat_index_set(unit, handle, fs_idx,flags);
    } else 
#endif /* INCLUDE_L3 */
    {
        /* handle GPORT must be a local physical port */
        return _bcm_esw_port_flex_stat_index_set(unit, handle, fs_idx,flags);
    }
}

/*
 * Function:
 *      _bcm_esw_flex_stat_handle_free
 * Purpose:
 *      Deallocate the flexible stat for a handle that has been deleted
 *      by its controlling module.
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 * Returns:
 *      Success: Flexible stat index
 *      Failure: 0 (invalid stat index)
 * Notes:
 *      No HW update is required because the controlling module clears
 *      the entry for the associated handle. We note that this FS index
 *      is again usable, and the FS HW counters will be reset when this
 *      index is next used.
 */
void
_bcm_esw_flex_stat_ext_handle_free(int unit, _bcm_flex_stat_type_t type,
                                   _bcm_flex_stat_handle_t handle)
{
    int fs_idx;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, type);

    FS_LOCK(unit);
    if (FS_INFO(unit, hw_type) != NULL) {
        fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
        if (fs_idx > 0) {
            _bcm_esw_flex_stat_index_delete(unit, type, handle, fs_idx);
        }
    }
    FS_UNLOCK(unit);
}

/* Write to Flexible Counter HW */
STATIC int
_bcm_esw_flex_stat_hw_set(int unit, _bcm_flex_stat_type_t type,
                          int fs_idx, _bcm_flex_stat_t stat, uint64 val)
{
    ing_service_counter_table_entry_t flex_ctr_entry;
    soc_mem_t mem;
    soc_field_t stat_field = BYTE_COUNTERf;
    int rv;

    switch (stat) {
    case _bcmFlexStatIngressPackets:
        stat_field = PACKET_COUNTERf;
        /* Fall thru */
    case _bcmFlexStatIngressBytes:
        mem = FS_ING_MEM(type);
        break;
    case _bcmFlexStatEgressPackets:
        stat_field = PACKET_COUNTERf;
        /* Fall thru */
    case _bcmFlexStatEgressBytes:
        mem = FS_EGR_MEM(type);
        break;
    default:
        return BCM_E_PARAM;
    }

    MEM_LOCK(unit, mem);
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        soc_mem_t mem_x, mem_y;
        uint64 zero64;

        switch (mem) {
        case ING_SERVICE_COUNTER_TABLEm:
            mem_x = ING_SERVICE_COUNTER_TABLE_Xm;
            mem_y = ING_SERVICE_COUNTER_TABLE_Ym;
            break;
        case EGR_SERVICE_COUNTER_TABLEm:
            mem_x = EGR_SERVICE_COUNTER_TABLE_Xm; 
            mem_y = EGR_SERVICE_COUNTER_TABLE_Ym; 
            break;
        case ING_VINTF_COUNTER_TABLEm:
            mem_x = ING_VINTF_COUNTER_TABLE_Xm; 
            mem_y = ING_VINTF_COUNTER_TABLE_Ym; 
            break;
        case EGR_VINTF_COUNTER_TABLEm:
            mem_x = EGR_VINTF_COUNTER_TABLE_Xm; 
            mem_y = EGR_VINTF_COUNTER_TABLE_Ym; 
            break;
        /* Defensive Default */
        /* coverity[dead_error_begin] */
        default:
            MEM_UNLOCK(unit, mem);
            return BCM_E_INTERNAL;
        }
        rv = soc_mem_read(unit, mem_x, MEM_BLOCK_ANY,
                          fs_idx, &flex_ctr_entry);
        if (BCM_SUCCESS(rv)) {
            soc_mem_field64_set(unit, mem_x, (uint32 *)&flex_ctr_entry,
                                stat_field, val);
            rv = soc_mem_write(unit, mem_x, MEM_BLOCK_ANY, fs_idx,
                               &flex_ctr_entry);
        }
        /* Dual pipe, write non-zero value to X pipe, 0 to Y pipe */
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_read(unit, mem_y, MEM_BLOCK_ANY,
                              fs_idx, &flex_ctr_entry);
        }
        if (BCM_SUCCESS(rv)) {
            COMPILER_64_ZERO(zero64);
            soc_mem_field64_set(unit, mem_y, (uint32 *)&flex_ctr_entry,
                                stat_field, zero64);
            rv = soc_mem_write(unit, mem_y, MEM_BLOCK_ANY, fs_idx,
                               &flex_ctr_entry);
        }
    } else
#endif /* BCM_TRIDENT_SUPPORT */
    {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, fs_idx, &flex_ctr_entry);
        if (BCM_SUCCESS(rv)) {
            soc_mem_field64_set(unit, mem, (uint32 *)&flex_ctr_entry,
                                stat_field, val);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, fs_idx,
                               &flex_ctr_entry);
        }
    }
    MEM_UNLOCK(unit, mem);

    return rv;
}

/* Write to Flexible Counter SW cache and HW. 
   Value is written to SW as is but is trimmed for HW */
STATIC int
_bcm_esw_flex_stat_sw_set(int unit, _bcm_flex_stat_type_t type,
                          int fs_idx, _bcm_flex_stat_t stat, uint64 val)
{
    uint8 htype, dir = 0 /* ingress */, gran = 0 /* pkts */;
    soc_mem_t mem;
    soc_field_t stat_field = PACKET_COUNTERf;
    ing_service_counter_table_entry_t *bptr, *eptr;
    uint64 *cptr;
    uint32 value[2], mask[2];

    htype = FS_TYPE_TO_SVC_VINTF_HW_TYPE(unit, type);

    if (stat > _bcmFlexStatIngressBytes) {
        /* egress */
        dir = 1;
    }
    if ((stat == _bcmFlexStatIngressBytes) || 
        (stat == _bcmFlexStatEgressBytes)) {
        /* bytes */ 
        gran = 1;
        mask[0] = 0xffffffff;
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            mask[1] = 0xf; /* 36 bits */
            stat_field = BYTE_COUNTERf;
        } else 
#endif /* BCM_TRIDENT_SUPPORT */
        {
            mask[1] = 0x7; /* 35 bits */
            stat_field = BYTE_COUNTERf;
        }
    } else {
        /* packets */
        mask[0] = 0x1fffffff; /* 29 bits */
        mask[1] = 0; 
    }
    cptr = _bcm_flex_stat_mem[unit][htype][dir][gran];
    cptr[fs_idx] = val; 
    value[0] = COMPILER_64_LO(val) & mask[0];
    value[1] = COMPILER_64_HI(val) & mask[1];
    COMPILER_64_SET(val, value[1], value[0]);
    /* Set in the 'other' HW buffer */
    bptr = _bcm_flex_stat_buff[unit][htype][dir]
                              [!_bcm_flex_stat_buff_toggle[unit]];
    mem = _bcm_flex_stat_mem_types[(htype * _BCM_FLEX_STAT_DIRS) + dir];
    eptr = soc_mem_table_idx_to_pointer(unit, mem,
                                        ing_service_counter_table_entry_t *,
                                        bptr, fs_idx);
    soc_mem_field_set(unit, mem, (uint32 *)eptr, stat_field, value);
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        /* Clear Y-Pipe 'other' HW buffer. */
        bptr = _bcm_flex_stat_buff_y[unit][htype][dir]
            [!_bcm_flex_stat_buff_toggle[unit]];
        if (NULL == bptr) {
            /* Lock released in calling routine. */
            return (BCM_E_INTERNAL);
        }
        eptr = soc_mem_table_idx_to_pointer(unit, mem,
                                        ing_service_counter_table_entry_t *,
                                            bptr, fs_idx);
        /* Clear Y-pipe buffer value. */
        value[0] = value[1] = 0;
        soc_mem_field_set(unit, mem, (uint32 *)eptr, stat_field, value);
    }
#endif /* BCM_TRIDENT_SUPPORT */

    return _bcm_esw_flex_stat_hw_set(unit, type, fs_idx, stat, val);
}

/* 
 * Read from Flexible Counters SW cache 
 * if sync_mode is set, sync the sw accumulated count
 * with hw count value first, else return sw count.  
 */
STATIC int
_bcm_esw_flex_stat_sw_get(int unit, int sync_mode, _bcm_flex_stat_type_t type,
                          int fs_idx, _bcm_flex_stat_t stat, uint64 *val)
{
    int rv = BCM_E_NONE;
    uint8 htype, dir = 0 /* ingress */, gran = 0 /* pkts */;
    uint64 *cptr;
    htype = FS_TYPE_TO_SVC_VINTF_HW_TYPE(unit, type);
    if (stat > _bcmFlexStatIngressBytes) {
        /* egress */
        dir = 1;
    }
    if ((stat == _bcmFlexStatIngressBytes) || 
        (stat == _bcmFlexStatEgressBytes)) {
        /* bytes */ 
        gran = 1;
    } 

    /* if sync_mode is set, sync the sw count with hw count */
    if (sync_mode == 1) {
        FS_LOCK(unit);
        _bcm_esw_flex_stat_collection(unit, type, fs_idx, stat);
        FS_UNLOCK(unit);
    }

    cptr = _bcm_flex_stat_mem[unit][htype][dir][gran];
    *val = cptr[fs_idx];
    return rv;
}

/* Read from Flexible Counters HW */
int
_bcm_esw_flex_stat_hw_get(int unit, _bcm_flex_stat_type_t type,
                          int fs_idx, _bcm_flex_stat_t stat, uint64 *val)
{
    ing_service_counter_table_entry_t flex_ctr_entry;
    soc_mem_t mem;
    soc_field_t stat_field = BYTE_COUNTERf;
    int rv;

    switch (stat) {
    case _bcmFlexStatIngressPackets:
        stat_field = PACKET_COUNTERf;
        /* Fall thru */
    case _bcmFlexStatIngressBytes:
        mem = FS_ING_MEM(type);
        break;
    case _bcmFlexStatEgressPackets:
        stat_field = PACKET_COUNTERf;
        /* Fall thru */
    case _bcmFlexStatEgressBytes:
        mem = FS_EGR_MEM(type);
        break;
    default:
        return BCM_E_PARAM;
    }

    MEM_LOCK(unit, mem);
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        soc_mem_t mem_x, mem_y;
        uint64 val_y;

        switch (mem) {
        case ING_SERVICE_COUNTER_TABLEm:
            mem_x = ING_SERVICE_COUNTER_TABLE_Xm;
            mem_y = ING_SERVICE_COUNTER_TABLE_Ym;
            break;
        case EGR_SERVICE_COUNTER_TABLEm:
            mem_x = EGR_SERVICE_COUNTER_TABLE_Xm; 
            mem_y = EGR_SERVICE_COUNTER_TABLE_Ym; 
            break;
        case ING_VINTF_COUNTER_TABLEm:
            mem_x = ING_VINTF_COUNTER_TABLE_Xm; 
            mem_y = ING_VINTF_COUNTER_TABLE_Ym; 
            break;
        case EGR_VINTF_COUNTER_TABLEm:
            mem_x = EGR_VINTF_COUNTER_TABLE_Xm; 
            mem_y = EGR_VINTF_COUNTER_TABLE_Ym; 
            break;
        /* Defensive Default */
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_INTERNAL;
        }
        rv = soc_mem_read(unit, mem_x, MEM_BLOCK_ANY,
                          fs_idx, &flex_ctr_entry);
        if (BCM_SUCCESS(rv)) {
            soc_mem_field64_get(unit, mem_x, (uint32 *)&flex_ctr_entry,
                                stat_field, val);
        }
        /* Dual pipe, read from X pipe and Y pipe then add */
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_read(unit, mem_y, MEM_BLOCK_ANY,
                              fs_idx, &flex_ctr_entry);
        }
        if (BCM_SUCCESS(rv)) {
            soc_mem_field64_get(unit, mem_y, (uint32 *)&flex_ctr_entry,
                                stat_field, &val_y);
            COMPILER_64_ADD_64(*val, val_y);
        }
    } else
#endif /* BCM_TRIDENT_SUPPORT */
    {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, fs_idx, &flex_ctr_entry);

        if (BCM_SUCCESS(rv)) {
            soc_mem_field64_get(unit, mem, (uint32 *)&flex_ctr_entry,
                                stat_field, val);
        }
    }
    MEM_UNLOCK(unit, mem);

    return rv;
}

/* Clear the Flexible Counter HW */
STATIC int
_bcm_esw_flex_stat_hw_clear(int unit, _bcm_flex_stat_type_t type,
                            int fs_idx)
{
    ing_service_counter_table_entry_t flex_ctr_entry;
    soc_mem_t mem;
    int rv = BCM_E_NONE;

    /* This will work on dual-pipes because the base memory will write
     * the same value to both pipe memories. */
    sal_memset(&flex_ctr_entry, 0, sizeof(flex_ctr_entry));

    mem = FS_ING_MEM(type);
    MEM_LOCK(unit, mem);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, fs_idx, &flex_ctr_entry);
    MEM_UNLOCK(unit, mem);

    if (SOC_MEM_IS_VALID(unit, ING_VINTF_COUNTER_TABLE_Ym) && 
        (mem == ING_VINTF_COUNTER_TABLEm) && BCM_SUCCESS(rv)) {
        ing_vintf_counter_table_y_entry_t flex_ctr_vintf_entry;
        sal_memset(&flex_ctr_vintf_entry, 0, sizeof(flex_ctr_vintf_entry));

        MEM_LOCK(unit, mem);
        rv = soc_mem_write(unit, ING_VINTF_COUNTER_TABLE_Ym,
            MEM_BLOCK_ANY, fs_idx, &flex_ctr_vintf_entry);
        MEM_UNLOCK(unit, mem);
    } else if (SOC_MEM_IS_VALID(unit, ING_SERVICE_COUNTER_TABLE_Ym) && 
        (mem == ING_SERVICE_COUNTER_TABLEm) && BCM_SUCCESS(rv)) {
        ing_service_counter_table_y_entry_t flex_ctr_service_entry;
        sal_memset(&flex_ctr_service_entry, 0, sizeof(flex_ctr_service_entry));

        MEM_LOCK(unit, mem);
        rv = soc_mem_write(unit, ING_SERVICE_COUNTER_TABLE_Ym,
            MEM_BLOCK_ANY, fs_idx, &flex_ctr_service_entry);
        MEM_UNLOCK(unit, mem);
    }

    if (BCM_SUCCESS(rv)) {
        mem = FS_EGR_MEM(type);
        MEM_LOCK(unit, mem);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, fs_idx,
                           &flex_ctr_entry);
        MEM_UNLOCK(unit, mem);
    }

    if (SOC_MEM_IS_VALID(unit, EGR_VINTF_COUNTER_TABLE_Ym) &&
        (mem == EGR_VINTF_COUNTER_TABLEm) && BCM_SUCCESS(rv)) {
        egr_vintf_counter_table_y_entry_t flex_ctr_egr_vintf_entry;
        sal_memset(&flex_ctr_egr_vintf_entry, 0, sizeof(flex_ctr_egr_vintf_entry));

        MEM_LOCK(unit, mem);
        rv = soc_mem_write(unit, EGR_VINTF_COUNTER_TABLE_Ym,
            MEM_BLOCK_ANY, fs_idx, &flex_ctr_egr_vintf_entry);
        MEM_UNLOCK(unit, mem);
    } else if (SOC_MEM_IS_VALID(unit, EGR_SERVICE_COUNTER_TABLE_Ym) &&
        (mem == EGR_SERVICE_COUNTER_TABLEm) && BCM_SUCCESS(rv)) {
        egr_service_counter_table_y_entry_t flex_ctr_egr_service_entry;
        sal_memset(&flex_ctr_egr_service_entry, 0, sizeof(flex_ctr_egr_service_entry));

        MEM_LOCK(unit, mem);
        rv = soc_mem_write(unit, EGR_SERVICE_COUNTER_TABLE_Ym,
            MEM_BLOCK_ANY, fs_idx, &flex_ctr_egr_service_entry);
        MEM_UNLOCK(unit, mem);
    }

    return rv;
}

/* Clear the Flexible Counter SW and HW */
STATIC int
_bcm_esw_flex_stat_sw_clear(int unit, _bcm_flex_stat_type_t type,
                            int fs_idx)
{
    uint64 *cptr;
    ing_service_counter_table_entry_t *bptr, *eptr;
    uint8 htype = FS_TYPE_TO_SVC_VINTF_HW_TYPE(unit, type);
    /* Clear for both directions and both granularities */
    cptr = _bcm_flex_stat_mem[unit][htype][0][0];
    COMPILER_64_ZERO(cptr[fs_idx]);
    cptr = _bcm_flex_stat_mem[unit][htype][0][1];
    COMPILER_64_ZERO(cptr[fs_idx]);
    cptr = _bcm_flex_stat_mem[unit][htype][1][0];
    COMPILER_64_ZERO(cptr[fs_idx]);
    cptr = _bcm_flex_stat_mem[unit][htype][1][1];
    COMPILER_64_ZERO(cptr[fs_idx]);
    /* Clear for both directions only in the 'other' HW buffer */
    bptr = _bcm_flex_stat_buff[unit][htype][0]
                              [!_bcm_flex_stat_buff_toggle[unit]];
    eptr = soc_mem_table_idx_to_pointer(unit, ING_SERVICE_COUNTER_TABLEm,
                                        ing_service_counter_table_entry_t *,
                                        bptr, fs_idx);
    sal_memset(eptr, 0, sizeof(ing_service_counter_table_entry_t));
    bptr = _bcm_flex_stat_buff[unit][htype][1]
                                    [!_bcm_flex_stat_buff_toggle[unit]];
    eptr = soc_mem_table_idx_to_pointer(unit, ING_SERVICE_COUNTER_TABLEm,
                                        ing_service_counter_table_entry_t *,
                                        bptr, fs_idx);
    sal_memset(eptr, 0, sizeof(ing_service_counter_table_entry_t));
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        bptr = _bcm_flex_stat_buff_y[unit][htype][0]
                                    [!_bcm_flex_stat_buff_toggle[unit]];
        eptr = soc_mem_table_idx_to_pointer(unit, ING_SERVICE_COUNTER_TABLEm,
                                        ing_service_counter_table_entry_t *,
                                            bptr, fs_idx);
        sal_memset(eptr, 0, sizeof(ing_service_counter_table_entry_t));
        bptr = _bcm_flex_stat_buff_y[unit][htype][1]
                                    [!_bcm_flex_stat_buff_toggle[unit]];
        eptr = soc_mem_table_idx_to_pointer(unit, ING_SERVICE_COUNTER_TABLEm,
                                        ing_service_counter_table_entry_t *,
                                            bptr, fs_idx);
        sal_memset(eptr, 0, sizeof(ing_service_counter_table_entry_t));
    }
#endif /* BCM_TRIDENT_SUPPORT */
    return _bcm_esw_flex_stat_hw_clear(unit, type, fs_idx);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_enable_set
 * Purpose:
 *      Attach/detach a flexible statistic resource to a given packet
 *      lookup table hit.
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      fs_set_f - callback function to write index into HW table
 *      cookie - cookie for callback function
 *      handle - the encoded handle for the type of packets to count.
 *      enable - TRUE: configure flexible stat collection for the handle.
 *               FALSE: stop flexible stat collection for the handle.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_ext_enable_set(int unit, _bcm_flex_stat_type_t type,
                                  _bcm_flex_stat_src_index_set_f fs_set_f,
                                  void *cookie,
                                  _bcm_flex_stat_handle_t handle, int enable,
                                  int cnt_inx)
                       
{
    int fs_idx, rv = BCM_E_NONE;

    FS_LOCK(unit);
    FS_INIT(unit, type);

    /* non-zero cnt_inx,  used by the new flex count APIs
     * indicates a counter index is allocated outside of this routine. 
     * It doesn't have one-to-one mapping with a handle. Several handles
     * may be associated with a same counter index. For delete operation, 
     * cnt_inx should be ignore. A entry is removed from the handle list 
     * based on the handle.
     *
     * zero cnt_inx
     * Implies the legacy implementation. The counter index is automatically 
     * allocated with the handle entry, and has one-to-one mapping with a
     * handle.
     */
    if (cnt_inx) {
        /* legacy and new APIs cannot be mixed. Validate it first */
        if (_tr2_flex_stat_api_ver[unit]) {
            /* must be new APIs */
            if (_tr2_flex_stat_api_ver[unit] != TR2_FLEX_STAT_API_NEW) {
                LOG_CLI((BSL_META_U(unit,
                                    "ERROR: _bcm_esw_flex_stat_ext_enable_set: "
                                    "cannot mix the new API with legacy API\n")));
                FS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
        } else {  /* first time */
            _tr2_flex_stat_api_ver[unit] = TR2_FLEX_STAT_API_NEW;
        }

        /* Check if stat group is created, if not return error */
        if (!_bcm_esw_flex_stat_index_get(unit, type, cnt_inx)) {
            /* During reinit, stat module reset/reinit is done before other
             * modules reinit, hence check needs to be restricted to insert case.
             * For delete case fs_idx will be 0 and will return BCM_E_NOT_FOUND */
            if (enable) {
                FS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
        }

        fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
        if (enable) {
            if (fs_idx > 0) {
                rv = BCM_E_EXISTS;
            } else {
                _bcm_esw_flex_stat_handle_list_insert(unit,type,handle,
                                                      cnt_inx);
                rv = (*fs_set_f)(unit, handle, cnt_inx, cookie);
                if (BCM_FAILURE(rv)) {
                    _bcm_esw_flex_stat_handle_list_delete(unit, type, handle);
                } else {
                    rv = _bcm_esw_flex_stat_ref_cnt_inc(unit,type,cnt_inx);
                }
            }
        } else {  /* enable FALSE */
            if (fs_idx) { 
                _bcm_esw_flex_stat_handle_list_delete(unit,type,handle);
                rv = (*fs_set_f)(unit, handle, 0, cookie);
                if (BCM_SUCCESS(rv)) {
                    rv = _bcm_esw_flex_stat_ref_cnt_dec(unit,type,fs_idx);
                } 
            } else {
                rv = BCM_E_NOT_FOUND;
            }
        }
        FS_UNLOCK(unit);
        return rv;
    }

    /* legacy API is used */
    if (_tr2_flex_stat_api_ver[unit]) {    /* must be legacy APIs */
        if (_tr2_flex_stat_api_ver[unit] != TR2_FLEX_STAT_API_LEGACY) {
            LOG_CLI((BSL_META_U(unit,
                                "ERROR _bcm_esw_flex_stat_ext_enable_set: cannot mix "
                                "the new API with legacy API\n")));
            FS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
    } else {  /* first time */
        _tr2_flex_stat_api_ver[unit] = TR2_FLEX_STAT_API_LEGACY;
    }

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        if (enable) {
            rv = BCM_E_EXISTS;
        } else {
            _bcm_esw_flex_stat_index_delete(unit, type, handle, fs_idx);
            rv = (*fs_set_f)(unit, handle, 0, cookie);
        }
    } else {
        if (!enable) {
            rv = BCM_E_NOT_FOUND;
        } else {
            fs_idx = _bcm_esw_flex_stat_alloc(unit, type, handle);
            if (fs_idx) {
                rv = (*fs_set_f)(unit, handle, fs_idx, cookie);
                if (BCM_FAILURE(rv)) {
                    _bcm_esw_flex_stat_index_delete(unit, type,
                                                    handle, fs_idx);
                }
            } else {
                rv = BCM_E_RESOURCE;
            }
            /* Start with cleared stats for this index */
            if (BCM_SUCCESS(rv)) {
                rv = _bcm_esw_flex_stat_sw_clear(unit, type, fs_idx);
            }
        }
    }
    FS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_get
 * Purpose:
 *      Retrieve a flexible stat for a handle 
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit      - SOC unit number
 *      sync_mode - hwcount is to be synced to sw count
 *      type      - flexible stat type
 *      handle    - the encoded handle for the type of packets to count.
 *      stat      - (IN) Type of the flexible statistic to retrieve.
 *      val       - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_ext_get(int unit, int sync_mode, 
                           _bcm_flex_stat_type_t type,
                           _bcm_flex_stat_handle_t handle,
                           _bcm_flex_stat_t stat, uint64 *val)
{
    int fs_idx, rv = BCM_E_NONE;

    if (NULL == val) {
        return (BCM_E_PARAM);
    }

    if ((stat < _bcmFlexStatIngressPackets) ||
        (stat >= _bcmFlexStatNum)) {
        return (BCM_E_PARAM);
    }

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        rv = _bcm_esw_flex_stat_sw_get(unit, sync_mode, type, 
                                       fs_idx, stat, val);
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_get32
 * Purpose:
 *      Retrieve a flexible stat for a handle in 32-bit form 
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      stat - (IN) Type of the flexible statistic to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_ext_get32(int unit, int sync_mode, 
                             _bcm_flex_stat_type_t type,
                             _bcm_flex_stat_handle_t handle,
                             _bcm_flex_stat_t stat, uint32 *val)
{
    uint64 val64;            /* 64 bit counter value.    */
    int rv;

    if (NULL == val) {
        return (BCM_E_PARAM);
    }

    rv = _bcm_esw_flex_stat_ext_get(unit, sync_mode, type, 
                                    handle, stat, &val64);

    if (BCM_SUCCESS(rv)) {
        *val = COMPILER_64_LO(val64);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_set
 * Purpose:
 *      Assign a flexible stat for a handle 
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      stat - (IN) Type of the flexible statistic to retrieve.
 *      val - (IN) Counter value.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_ext_set(int unit, _bcm_flex_stat_type_t type,
                           _bcm_flex_stat_handle_t handle,
                           _bcm_flex_stat_t stat, uint64 val)
{
    int fs_idx, rv = BCM_E_NONE;

    if ((stat < _bcmFlexStatIngressPackets) ||
        (stat >= _bcmFlexStatNum)) {
        return (BCM_E_PARAM);
    }

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        rv = _bcm_esw_flex_stat_sw_set(unit, type, fs_idx, stat, val);
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_set32
 * Purpose:
 *      Assign a flexible stat for a handle in 32-bit form 
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      stat - (IN) Type of the flexible statistic to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_ext_set32(int unit, _bcm_flex_stat_type_t type,
                             _bcm_flex_stat_handle_t handle,
                             _bcm_flex_stat_t stat, uint32 val)
{
    uint64 val64;            /* 64 bit counter value.    */

    COMPILER_64_SET(val64, 0, val);
    return _bcm_esw_flex_stat_ext_set(unit, type, handle, stat, val64);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple flexible statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flex_stat_ext_multi_get(int unit, _bcm_flex_stat_type_t type,
                                 _bcm_flex_stat_handle_t handle, int nstat,
                                 _bcm_flex_stat_t *stat_arr,
                                 uint64 *value_arr)
{
    int                 rv = BCM_E_NONE; /* Operation return status.    */
    int                 idx;             /* Statistics iteration index. */
    int                 fs_idx;          /* Flexible stat mem index.    */

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        /* Read individual statistics. */
        for (idx = 0; idx < nstat; idx++) {
            if ((stat_arr[idx] < _bcmFlexStatIngressPackets) ||
                (stat_arr[idx] >= _bcmFlexStatNum)) {
                rv = BCM_E_PARAM;
                break;
            }
            rv = _bcm_esw_flex_stat_sw_get(unit, 0, type, fs_idx,
                                           stat_arr[idx], value_arr + idx);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_multi_get32
 * Purpose:
 *      Get 32-bit counter value for multiple flexible statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flex_stat_ext_multi_get32(int unit, _bcm_flex_stat_type_t type,
                                   _bcm_flex_stat_handle_t handle, int nstat,
                                   _bcm_flex_stat_t *stat_arr,
                                   uint32 *value_arr)
{
    uint64              value;           /* 64 bit counter value.       */
    int                 rv = BCM_E_NONE; /* Operation return status.    */
    int                 idx;             /* Statistics iteration index. */
    int                 fs_idx;          /* Flexible stat mem index.    */

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        /* Read individual statistics. */
        for (idx = 0; idx < nstat; idx++) {
            if ((stat_arr[idx] < _bcmFlexStatIngressPackets) ||
                (stat_arr[idx] >= _bcmFlexStatNum)) {
                rv = BCM_E_PARAM;
                break;
            }
            rv = _bcm_esw_flex_stat_sw_get(unit, 0, type, fs_idx,
                                           stat_arr[idx], &value);
            if (BCM_FAILURE(rv)) {
                break;
            }
            value_arr[idx] = COMPILER_64_LO(value);
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_multi_set
 * Purpose:
 *      Assign 64-bit counter value for multiple flexible statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (IN) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flex_stat_ext_multi_set(int unit, _bcm_flex_stat_type_t type,
                                 _bcm_flex_stat_handle_t handle, int nstat,
                                 _bcm_flex_stat_t *stat_arr, 
                                 uint64 *value_arr)
{
    int                 rv = BCM_E_NONE; /* Operation return status.    */
    int                 idx;             /* Statistics iteration index. */
    int                 fs_idx;          /* Flexible stat mem index.    */

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        /* Read individual statistics. */
        for (idx = 0; idx < nstat; idx++) {
            if ((stat_arr[idx] < _bcmFlexStatIngressPackets) ||
                (stat_arr[idx] >= _bcmFlexStatNum)) {
                rv = BCM_E_PARAM;
                break;
            }
            rv = _bcm_esw_flex_stat_sw_set(unit, type, fs_idx,
                                           stat_arr[idx], value_arr[idx]);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_multi_set32
 * Purpose:
 *      Assign 32-bit counter value for multiple flexible statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (IN) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flex_stat_ext_multi_set32(int unit, _bcm_flex_stat_type_t type,
                                   _bcm_flex_stat_handle_t handle, int nstat,
                                   _bcm_flex_stat_t *stat_arr, 
                                   uint32 *value_arr)
{
    uint64              value;           /* 64 bit counter value.       */
    int                 rv = BCM_E_NONE; /* Operation return status.    */
    int                 idx;             /* Statistics iteration index. */
    int                 fs_idx;          /* Flexible stat mem index.    */

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        /* Read individual statistics. */
        for (idx = 0; idx < nstat; idx++) {
            if ((stat_arr[idx] < _bcmFlexStatIngressPackets) ||
                (stat_arr[idx] >= _bcmFlexStatNum)) {
                rv = BCM_E_PARAM;
                break;
            }
            COMPILER_64_SET(value, 0, value_arr[idx]);
            rv = _bcm_esw_flex_stat_sw_set(unit, type, fs_idx,
                                           stat_arr[idx], value);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);

    return rv;
}

/* Functions for legacy uint32 handles to newer extended handles */
void
_bcm_esw_flex_stat_reinit_add(int unit, _bcm_flex_stat_type_t type,
                              int fs_index, uint32 handle)
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    _bcm_esw_flex_stat_ext_reinit_add(unit, type, fs_index, fsh);
}

void
_bcm_esw_flex_stat_handle_free(int unit, _bcm_flex_stat_type_t type,
                               uint32 handle)
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    _bcm_esw_flex_stat_ext_handle_free(unit, type, fsh);
}

int
_bcm_esw_flex_stat_enable_set(int unit, _bcm_flex_stat_type_t type,
                              _bcm_flex_stat_src_index_set_f fs_set_f,
                              void *cookie, uint32 handle, int enable,
           int cnt_inx /* count index, 0 for legacy API, auto allocated */
           )
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    return _bcm_esw_flex_stat_ext_enable_set(unit, type, fs_set_f,
                                             cookie, fsh, enable,cnt_inx);
}

int
_bcm_esw_flex_stat_get(int unit, int sync_mode, _bcm_flex_stat_type_t type,
                       uint32 handle, _bcm_flex_stat_t stat, uint64 *val)
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    return _bcm_esw_flex_stat_ext_get(unit, sync_mode, type, fsh, stat, val);
}

int
_bcm_esw_flex_stat_get32(int unit, int sync_mode, _bcm_flex_stat_type_t type,
                         uint32 handle, _bcm_flex_stat_t stat, uint32 *val)
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    return _bcm_esw_flex_stat_ext_get32(unit, sync_mode, type, fsh, stat, val);
}

int
_bcm_esw_flex_stat_set(int unit, _bcm_flex_stat_type_t type,
                       uint32 handle, _bcm_flex_stat_t stat, uint64 val)
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    return _bcm_esw_flex_stat_ext_set(unit, type, fsh, stat, val);
}

int
_bcm_esw_flex_stat_set32(int unit, _bcm_flex_stat_type_t type,
                         uint32 handle, _bcm_flex_stat_t stat, uint32 val)
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    return _bcm_esw_flex_stat_ext_set32(unit, type, fsh, stat, val);
}

int 
_bcm_esw_flex_stat_multi_get(int unit, _bcm_flex_stat_type_t type,
                             uint32 handle, int nstat,
                             _bcm_flex_stat_t *stat_arr,
                             uint64 *value_arr)
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    return _bcm_esw_flex_stat_ext_multi_get(unit, type, fsh,
                                            nstat, stat_arr, value_arr);
}

int 
_bcm_esw_flex_stat_multi_get32(int unit, _bcm_flex_stat_type_t type,
                               uint32 handle, int nstat,
                               _bcm_flex_stat_t *stat_arr,
                               uint32 *value_arr)
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    return _bcm_esw_flex_stat_ext_multi_get32(unit, type, fsh,
                                              nstat, stat_arr, value_arr);
}

int 
_bcm_esw_flex_stat_multi_set(int unit, _bcm_flex_stat_type_t type,
                             uint32 handle, int nstat,
                             _bcm_flex_stat_t *stat_arr, 
                             uint64 *value_arr)
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    return _bcm_esw_flex_stat_ext_multi_set(unit, type, fsh,
                                            nstat, stat_arr, value_arr);
}

int 
_bcm_esw_flex_stat_multi_set32(int unit, _bcm_flex_stat_type_t type,
                               uint32 handle, int nstat,
                               _bcm_flex_stat_t *stat_arr, 
                               uint32 *value_arr)
{
    _bcm_flex_stat_handle_t fsh;
    _BCM_FLEX_STAT_HANDLE_CLEAR(fsh);
    _BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, 0, handle);

    return _bcm_esw_flex_stat_ext_multi_set32(unit, type, fsh,
                                              nstat, stat_arr, value_arr);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)

typedef enum _bcm_trx_mysta_memacc_type_e {
    _BCM_TRX_MYSTA_MEMACC_MAC_ADDRf,
    _BCM_TRX_MYSTA_MEMACC_MAC_ADDR_MASKf,
    _BCM_TRX_MYSTA_MEMACC_VLAN_IDf,
    _BCM_TRX_MYSTA_MEMACC_VLAN_ID_MASKf,
    _BCM_TRX_MYSTA_MEMACC_ING_PORT_NUMf,
    _BCM_TRX_MYSTA_MEMACC_ING_PORT_NUM_MASKf,
    _BCM_TRX_MYSTA_MEMACC_SOURCE_FIELDf,
    _BCM_TRX_MYSTA_MEMACC_SOURCE_FIELD_MASKf, 
    _BCM_TRX_MYSTA_MEMACC_SOURCE_TYPEf,
    _BCM_TRX_MYSTA_MEMACC_SOURCE_TYPE_MASKf,
    _BCM_TRX_MYSTA_MEMACC_VALIDf,
    _BCM_TRX_MYSTA_MEMACC_COPY_TO_CPUf,
    _BCM_TRX_MYSTA_MEMACC_DISCARDf,
    _BCM_TRX_MYSTA_MEMACC_IPV4_TERMINATION_ALLOWEDf,
    _BCM_TRX_MYSTA_MEMACC_NUM
} _bcm_trx_mysta_memacc_type_t;

#define _BCM_TRX_MYSTA_MEMACC_LIST(u, mem) \
        (mem == MY_STATION_TCAM_2m) ? &(_bcm_trx_mysta2_memacc_list[u]) : \
                                      &(_bcm_trx_mysta_memacc_list[u])

#define _BCM_TRX_MYSTA_MEMACC(u, memacc_list, memacc_type) \
        (&((*memacc_list)[memacc_type]))

#define _BCM_TRX_MYSTA_FIELD_MEMACC(u, memacc_list, fld) \
        (&((*memacc_list)[fld]))

#define _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(u, memacc_list, ent, fld) \
        soc_memacc_field32_get(_BCM_TRX_MYSTA_FIELD_MEMACC(u, memacc_list, \
                            _BCM_TRX_MYSTA_MEMACC_##fld), ent)
#define _BCM_TRX_MYSTA_FIELD_MEMACC_GET(u, memacc_list, ent, fld, fldbuf) \
        soc_memacc_field_get(_BCM_TRX_MYSTA_FIELD_MEMACC(u, memacc_list, \
                            _BCM_TRX_MYSTA_MEMACC_##fld), ent, fldbuf)
#define _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_GET(u, memacc_list, ent, fld, mac) \
        soc_memacc_mac_addr_get(_BCM_TRX_MYSTA_FIELD_MEMACC(u, memacc_list, \
                            _BCM_TRX_MYSTA_MEMACC_##fld), ent, mac)                                           
#define _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(u, memacc_list, ent, fld, val) \
        soc_memacc_field32_set(_BCM_TRX_MYSTA_FIELD_MEMACC(u, memacc_list, \
                            _BCM_TRX_MYSTA_MEMACC_##fld), ent, val)
#define _BCM_TRX_MYSTA_FIELD_MEMACC_SET(u, memacc_list, ent, fld, fldbuf) \
        soc_memacc_field_set(_BCM_TRX_MYSTA_FIELD_MEMACC(u, memacc_list, \
                            _BCM_TRX_MYSTA_MEMACC_##fld), ent, fldbuf)
#define _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(u, memacc_list, ent, fld, mac) \
        soc_memacc_mac_addr_set(_BCM_TRX_MYSTA_FIELD_MEMACC(u, memacc_list, \
                            _BCM_TRX_MYSTA_MEMACC_##fld), ent, mac) 

static soc_field_t 
_bcm_trx_mysta_field_memacc_map[_BCM_TRX_MYSTA_MEMACC_NUM] = {
    MAC_ADDRf,
    MAC_ADDR_MASKf,
    VLAN_IDf,
    VLAN_ID_MASKf,
    ING_PORT_NUMf,
    ING_PORT_NUM_MASKf,
    SOURCE_FIELDf,
    SOURCE_FIELD_MASKf, 
    SOURCE_TYPEf,
    SOURCE_TYPE_MASKf,
    VALIDf,
    COPY_TO_CPUf,
    DISCARDf,
    IPV4_TERMINATION_ALLOWEDf
};

STATIC int
_bcm_trx_mysta_memacc_init(int unit)
{
    int alloc_size, rv = BCM_E_NONE;
    soc_field_t *memacc_map;
    soc_memacc_t **memacc_list = NULL;
    int mam_ix, memacc_map_size = _BCM_TRX_MYSTA_MEMACC_NUM;
    int i;
    soc_mem_t mem, mem_list[] = {
                      MY_STATION_TCAMm
#ifdef BCM_RIOT_SUPPORT
                      , MY_STATION_TCAM_2m
#endif /* BCM_RIOT_SUPPORT */
                      };

    alloc_size = (memacc_map_size * sizeof(soc_memacc_t));

    for (i = 0; i < COUNTOF(mem_list); i++) {
        mem = mem_list[i];

#ifdef BCM_RIOT_SUPPORT
        if ((!soc_feature(unit, soc_feature_riot)) &&
            !(SOC_IS_TRIDENT3X(unit)) &&
            (mem == MY_STATION_TCAM_2m)) {
            continue;
        }
#endif /* BCM_RIOT_SUPPORT */

        memacc_map = _bcm_trx_mysta_field_memacc_map;
        memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);

        *memacc_list = sal_alloc(alloc_size, "L2 tables memacc data");
        if (NULL == *memacc_list) {
            rv = BCM_E_MEMORY;
            break;
        }

        for (mam_ix = 0; mam_ix < memacc_map_size; mam_ix++) {
            if (INVALIDf == memacc_map[mam_ix]) {
                SOC_MEMACC_INVALID_SET(&((*memacc_list)[mam_ix]));
                continue;
            }
            if (!soc_mem_field_valid(unit, mem,
                                     memacc_map[mam_ix])) {
                continue;
            }
            rv = soc_memacc_init(unit, mem,
                                 memacc_map[mam_ix],
                                 &((*memacc_list)[mam_ix]));
            if (BCM_FAILURE(rv)) {
                break;
            }
        }

        if (BCM_FAILURE(rv)) {
            break;
        }
    } while (0);

    if (BCM_FAILURE(rv)) {
        sal_free(*memacc_list);
        *memacc_list = NULL;
    }

    return rv;
}

/*
 * Function:
 *     _bcm_td_l2cache_from_my_station
 * Purpose:
 *     Convert a MY_STATION_TCAM entry to an L2cache API data structure
 * Parameters:
 *     unit              Unit number
 *     l2caddr           L2 cache API data structure
 *     entry             MY_STATION_TCAM entry
 */
STATIC void
_bcm_td_l2cache_from_my_station(int unit, bcm_l2_cache_addr_t *l2caddr,
                                my_station_tcam_entry_t *entry)
{
    soc_mem_t mem = MY_STATION_TCAMm;
    soc_memacc_t **memacc_list = NULL;

    sal_memset(l2caddr, 0, sizeof(*l2caddr));

    if (soc_feature(unit, soc_feature_riot)) {
        mem = MY_STATION_TCAM_2m;
    }

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);
    
    /* Valid bit is ignored here; entry is assumed valid */

    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_GET(unit, memacc_list, entry, MAC_ADDRf, l2caddr->mac);
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_GET(unit, memacc_list, entry, MAC_ADDR_MASKf, l2caddr->mac_mask);
    l2caddr->vlan = 
        _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, VLAN_IDf);
    l2caddr->vlan_mask = 
        _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, VLAN_ID_MASKf);
    if (soc_feature(unit, soc_feature_gh2_my_station)) {
        l2caddr->src_port =
            _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list,
                    entry, SOURCE_FIELDf);
        l2caddr->src_port_mask =
            _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list,
                    entry, SOURCE_FIELD_MASKf);
    } else {
        l2caddr->src_port =
            _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list,
                    entry, ING_PORT_NUMf);
        l2caddr->src_port_mask =
            _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list,
                    entry, ING_PORT_NUM_MASKf);
    }

    l2caddr->flags |= BCM_L2_CACHE_L3;
    if (_BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, DISCARDf)) {
        l2caddr->flags |= BCM_L2_CACHE_DISCARD;
    }
    if (_BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, COPY_TO_CPUf)) {
        l2caddr->flags |= BCM_L2_CACHE_CPU;
    }
}

/*
 * Function:
 *      _bcm_td_l2cache_to_my_station
 * Purpose:
 *     Convert an L2 API data structure to a MY_STATION_TCAM entry
 * Parameters:
 *     unit              Unit number
 *     entry             MY_STATION_TCAM entry
 *     entry_prof        MY_STATION_PROFILE_1 entry
 *     l2caddr           L2 cache API data structure
 */
STATIC void
_bcm_td_l2cache_to_my_station(int unit, my_station_tcam_entry_t *entry,
                              my_station_profile_1_entry_t *entry_prof,
                              bcm_l2_cache_addr_t *l2caddr)
{
    int i, entry_words;
    uint32 *l3_mask;
    soc_mem_t mem = MY_STATION_TCAMm;
    soc_memacc_t **memacc_list = NULL;

    l3_mask = COMMON_INFO(unit)->my_station_l3_mask;

    if (soc_feature(unit, soc_feature_riot)) {
        mem = MY_STATION_TCAM_2m;
        l3_mask = COMMON_INFO(unit)->my_station2_l3_mask.entry_data;
    }

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);

    entry_words = soc_mem_entry_words(unit, mem);

    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, VALIDf,
        (1 << soc_mem_field_length(unit, mem, VALIDf)) - 1);
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, VLAN_IDf,
                        l2caddr->vlan);
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, VLAN_ID_MASKf,
                        l2caddr->vlan_mask);
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, entry, MAC_ADDRf,
                         l2caddr->mac);
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, entry, MAC_ADDR_MASKf,
                         l2caddr->mac_mask);
    if (soc_feature(unit, soc_feature_gh2_my_station)) {
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry,
                            SOURCE_FIELDf, l2caddr->src_port);
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry,
                            SOURCE_FIELD_MASKf, l2caddr->src_port_mask);
    } else {
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry,
                            ING_PORT_NUMf, l2caddr->src_port);
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry,
                            ING_PORT_NUM_MASKf, l2caddr->src_port_mask);
    }

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        for (i = 0; i < soc_mem_entry_words(unit, MY_STATION_PROFILE_1m); i++) {
            entry_prof->entry_data[i] |= l3_mask[i];
        }
    } else {
        for (i = 0; i < entry_words; i++) {
            entry->entry_data[i] |= l3_mask[i];
        }
    }

    if (l2caddr->flags & BCM_L2_CACHE_DISCARD) {
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, DISCARDf, 1);
    }
    if (l2caddr->flags & BCM_L2_CACHE_CPU) {
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, COPY_TO_CPUf, 1);
    }
}

/*
 * Function:
 *     _bcm_td_l2_from_my_station
 * Purpose:
 *     Convert a MY_STATION_TCAM entry to an L2 API data structure
 * Parameters:
 *     unit              Unit number
 *     l2addr            L2 API data structure
 *     entry             MY_STATION_TCAM entry
 */
STATIC void
_bcm_td_l2_from_my_station(int unit, bcm_l2_addr_t *l2addr,
                           my_station_tcam_entry_t *entry,
                           my_station_profile_1_entry_t *entry_prof)
{
    soc_mem_t mem = MY_STATION_TCAMm;
    soc_memacc_t **memacc_list = NULL;

    if (soc_feature(unit, soc_feature_riot)) {
        mem = MY_STATION_TCAM_2m;
    }

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);
    sal_memset(l2addr, 0, sizeof(*l2addr));

    /* Valid bit is ignored here; entry is assumed valid */

    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_GET(unit, memacc_list, entry, MAC_ADDRf,
                         l2addr->mac);
    l2addr->vid = _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, VLAN_IDf);

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        if (soc_mem_field32_get(unit, MY_STATION_PROFILE_1m, entry_prof,
                                IPV4_TERMINATION_ALLOWEDf)) {
            l2addr->flags |= BCM_L2_L3LOOKUP;
        }
    } else {
        if (_BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry,
                                IPV4_TERMINATION_ALLOWEDf)) {
            l2addr->flags |= BCM_L2_L3LOOKUP;
        }
    }
    if (_BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, DISCARDf)) {
        l2addr->flags |= BCM_L2_DISCARD_DST;
    }
    if (_BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, COPY_TO_CPUf)) {
        l2addr->flags |= BCM_L2_COPY_TO_CPU;
    }
    l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
}

/*
 * Function:
 *     _bcm_td_l2_to_my_station
 * Purpose:
 *     Convert an L2 API data structure to a MY_STATION_TCAM entry
 * Parameters:
 *     unit              Unit number
 *     entry             MY_STATION_TCAM entry
 *     l2addr            L2 API data structure
 *     incl_key_mask     Update key and mask fields as well
 */
STATIC void
_bcm_td_l2_to_my_station(int unit, int tcam_no, my_station_tcam_entry_t *entry,
                         my_station_profile_1_entry_t *entry_prof,
                         bcm_l2_addr_t *l2addr, int incl_key_mask)
{
    bcm_mac_t mac_mask;
    uint32 fval;
    int i, entry_words;
    uint32 *l3_mask;
    soc_mem_t mem = MY_STATION_TCAMm;
    soc_memacc_t **memacc_list = NULL;
    soc_mem_t proft = MY_STATION_PROFILE_1m;

    l3_mask = COMMON_INFO(unit)->my_station_l3_mask;

    if (tcam_no == 2) {
        mem = MY_STATION_TCAM_2m;
        l3_mask = COMMON_INFO(unit)->my_station2_l3_mask.entry_data;
        proft = MY_STATION_PROFILE_2m;
    }

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);
    entry_words = soc_mem_entry_words(unit, mem);

    if (incl_key_mask) {
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, VALIDf,
            (1 << soc_mem_field_length(unit, mem, VALIDf)) - 1);
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, VLAN_IDf,
                            l2addr->vid);
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, VLAN_ID_MASKf,
                            0xfff);
        _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, entry, MAC_ADDRf,
                             l2addr->mac);
        sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
        _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, entry, MAC_ADDR_MASKf,
                             mac_mask);
    }

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        entry_words = soc_mem_entry_words(unit, proft);
        for (i = 0; i < entry_words; i++) {
            entry_prof->entry_data[i] |= l3_mask[i];
        }
    } else {
        for (i = 0; i < entry_words; i++) {
            entry->entry_data[i] |= l3_mask[i];
        }
    }

    fval = l2addr->flags & BCM_L2_DISCARD_DST ? 1 : 0;
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, DISCARDf, fval);

    fval = l2addr->flags & BCM_L2_COPY_TO_CPU ? 1 : 0;
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, COPY_TO_CPUf, fval);
}

/*
 * Function:
 *     _bcm_td_my_station_lookup
 * Purpose:
 *     Perform following exact matched in MY_STATION_TCAM table:
 *     - target entry lookup:
 *       match the entry specified by vlan/mac and optional port
 *     - (optional) alternate entry lookup: (when alt_index != NULL)
 *       match the possible entry having the same vlan/mac as the target entry
 * Parameters:
 *     unit              Unit number
 *     mac               MAC address key
 *     vlan              VLAN id key
 *     port              Source port number key (-1 means to match any)
 *     index_to_be_skip  The index to be omit during lookup
 *     entry_index       (OUT) target entry index (if BCM_E_NONE)
 *                             available entry index or -1 (if BCM_E_NOT_FOUND)
 *     alt_index         (OUT) alternate entry index or -1 (if BCM_E_NOT_FOUND)
 *                       if alt_index is NULL, that means lookup only
 * Returns:
 *      BCM_E_NONE - target entry found
 *      BCM_E_NOT_FOUND - target entry not found
 * Notes:
 *     if (target entry is found) {
 *         entry_index: index for the matched target entry
 *         alt_index: N/A
 *     } else if (alternate entry is found) {
 *         entry_index: index for the free entry to be used (-1 if table full)
 *         alt_index: index for the matched alternate entry
 *         *** caller needs to compare both indices, move the entry if needed
 *     } else {
 *         entry_index: index for the free entry to be used (-1 if table full)
 *         alt_index: -1
 *     }
 */
STATIC int
_bcm_td_my_station_lookup(int unit, soc_mem_t mem, bcm_mac_t mac, bcm_vlan_t vlan,
                          bcm_port_t port, int index_to_skip,
                          int *entry_index, int *alt_index)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    my_station_tcam_entry_t *entry, entry0, entry1, entry2, mask0, mask1;
    int index0, index1, free_index, i, entry_words;
    int start, end, step;
    bcm_mac_t mac_mask;
    uint32 port_mask;
    soc_memacc_t **memacc_list = NULL;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;

    if (mem == MY_STATION_TCAM_2m) {
        my_station_shadow = (my_station_tcam_entry_t *)info->my_station2_shadow;
    }

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);

    LOG_INFO(BSL_LS_BCM_L2,
             (BSL_META_U(unit,
                         "_bcm_td_my_station_lookup: unit=%d "
                         "mac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d port=%d "
                         "index_to_skip=%d\n"),
              unit, vlan, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
              port, index_to_skip));

    sal_memset(&mac_mask, 0xff, sizeof(mac_mask));

    if (soc_mem_field_valid(unit, mem, SOURCE_FIELDf)) {
        port_mask =
            (1 << soc_mem_field_length(unit, mem, SOURCE_FIELDf)) - 1;
    } else {
        port_mask =
            (1 << soc_mem_field_length(unit, mem, ING_PORT_NUMf)) - 1;
    }
    entry_words = soc_mem_entry_words(unit, mem);

    /*
     * entry0, mask0 are for target entry
     * entry1, mask1 are for alternate entry
     * if (port == -1)
     *     entry0 is: vlan/mac/00/fff/ffffffffffff/ff
     *     entry1 is: vlan/mac/xx/fff/ffffffffffff/ff
     *     start from last entry (free index needs to have larger index than
     *     any alternate entry)
     * else
     *     entry0 is: vlan/mac/port/fff/ffffffffffff/ff
     *     entry1 is: vlan/mac/00/fff/ffffffffffff/ff
     *     start from first entry (free index needs to have smaller index than
     *     the alternate entry)
     */
    sal_memset(&entry0, 0, sizeof(entry0));
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &entry0, VALIDf,
        (1 << soc_mem_field_length(unit, MY_STATION_TCAMm, VALIDf)) - 1);
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &entry0, VLAN_IDf, vlan);
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, &entry0, MAC_ADDRf, mac);
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &entry0, VLAN_ID_MASKf, 0xfff);
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, &entry0, MAC_ADDR_MASKf,
                         mac_mask);
    mask0 = entry0;
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0, VLAN_IDf, 0xfff);
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, &mask0, MAC_ADDRf, mac_mask);

    if (soc_mem_field_valid(unit, mem, SOURCE_FIELDf)) {
        if (SOC_IS_KATANAX(unit)) {
            _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0, SOURCE_TYPEf, 1);
            _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0, 
                            SOURCE_TYPE_MASKf, 1);             
        }
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0, SOURCE_FIELDf,
                            port_mask);
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0, SOURCE_FIELD_MASKf,
                            port_mask);
    } else {
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0, ING_PORT_NUMf,
                            port_mask);
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0, ING_PORT_NUM_MASKf,
                            port_mask);
    }
    if (alt_index != NULL) {
        entry1 = entry0;
        mask1 = mask0;
    }

    if (port == -1) {
        start = soc_mem_index_max(unit, mem);
        end = -1;
        step = -1;
    } else {
        if (soc_mem_field_valid(unit, mem, SOURCE_FIELDf)) {
            if (SOC_IS_KATANAX(unit)) {
                if ((1 << SOC_TRUNK_BIT_POS(unit)) & port){
                    /* trunk */
                    if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {  
                        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &entry0, 
                                        SOURCE_TYPEf, 1);
                        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &entry0, 
                                        SOURCE_TYPE_MASKf, 1);  
                        port &= ~(1 << (SOC_TRUNK_BIT_POS(unit)));
                    }    
                }            
            }
            _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &entry0, SOURCE_FIELDf, port);
            _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &entry0,
                            SOURCE_FIELD_MASKf, port_mask);
            
        } else {
            _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &entry0, ING_PORT_NUMf, port);
            _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &entry0,
                            ING_PORT_NUM_MASKf, port_mask);
        }
        start = 0;
        end = soc_mem_index_count(unit, mem);
        step = 1;
    }

    if (alt_index != NULL) {
        if (port == -1) {
            if (soc_mem_field_valid(unit, mem, SOURCE_FIELDf)) {
                if (SOC_IS_KATANAX(unit)) 
                {
                    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask1, SOURCE_TYPEf,
                                        0);              
                }
                _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask1, SOURCE_FIELDf,
                                    0);
            } else {      
                _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask1, ING_PORT_NUMf,
                                    0);
            }
        }

        /* entry2 is for checking VALID bit */
        sal_memset(&entry2, 0, sizeof(entry2));
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &entry2, VALIDf,
            (1 << soc_mem_field_length(unit, MY_STATION_TCAMm, VALIDf)) - 1);
    }

    index1 = -1;
    free_index = -1;
    for (index0 = start; index0 != end; index0 += step) {
        /* Skip the entry that we want to move (we know it matches) */
        if (index0 == index_to_skip) {
            continue;
        }

        entry = &my_station_shadow[index0];

        /* Try matching target entry */
        for (i = 0; i < entry_words; i++) {
            if ((entry->entry_data[i] ^ entry0.entry_data[i]) &
                mask0.entry_data[i]) {
                break;
            }
        }
        if (i == entry_words) {
            /* Target entry is found, no more action needed */
            *entry_index = index0;
            LOG_INFO(BSL_LS_BCM_L2,
                     (BSL_META_U(unit,
                                 "_bcm_td_my_station_lookup: found entry_index=%d\n"),
                      *entry_index));
            return BCM_E_NONE;
        }

        if (alt_index == NULL) { /* lookup only */
            continue;
        }

        /* Keep track index of the first free entry */
        if (free_index == -1) {
            for (i = 0; i < entry_words; i++) {
                if (entry->entry_data[i] & entry2.entry_data[i]) {
                    break;
                }
            }
            if (i == entry_words) {
                if (index1 != -1) {
                    /* Both free entry and alternate entry are found */
                    *entry_index = index0;
                    *alt_index = index1;
                    LOG_INFO(BSL_LS_BCM_L2,
                             (BSL_META_U(unit,
                                         "_bcm_td_my_station_lookup: not found "
                                         "entry_index=%d alt_index=%d\n"),
                              *entry_index, *alt_index));
                    return BCM_E_NOT_FOUND;
                } else {
                    free_index = index0;
                    continue;
                }
            }
        }

        /* Try matching alternate entry. If alternate entry is found, it
         * implies target entry is not in the table */
        for (i = 0; i < entry_words; i++) {
            if ((entry->entry_data[i] ^ entry1.entry_data[i]) &
                mask1.entry_data[i]) {
                break;
            }
        }
        if (i == entry_words) {
            if (free_index != -1) {
                /* both free entry and alternate entry are found */
                *entry_index = free_index;
                *alt_index = index0;
                LOG_INFO(BSL_LS_BCM_L2,
                         (BSL_META_U(unit,
                                     "_bcm_td_my_station_lookup: not found "
                                     "entry_index=%d alt_index=%d\n"),
                          *entry_index, *alt_index));
                return BCM_E_NOT_FOUND;
            } else {
                index1 = index0;
                continue;
            }
        }
    }

    *entry_index = free_index;
    if (alt_index != NULL) {
        *alt_index = index1;
    }
    LOG_INFO(BSL_LS_BCM_L2,
             (BSL_META_U(unit,
                         "_bcm_td_my_station_lookup: not found "
                         "entry_index=%d alt_index=%d\n"),
              *entry_index, alt_index != NULL ? *alt_index : -100));

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_td_l2_myStation_add
 * Purpose:
 *     Add MY_STATION_TCAM entry by L2 API
 * Parameters:
 *     unit              Unit number
 *     l2addr            L2 API data structure
 */
STATIC int
bcm_td_l2_myStation_insert(int unit, int tcam_no, bcm_l2_addr_t *l2addr)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    int rv, rv1, index, alt_index;
    my_station_tcam_entry_t *entry;
    my_station_profile_1_entry_t entry_prof;
    uint32 old_pid = 0, new_pid;
#if defined(BCM_TRIDENT_SUPPORT)
    l2u_entry_t l2u_entry;
#endif
    soc_mem_t mem = MY_STATION_TCAMm;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;
    int proft = 0;

    if (tcam_no == 2) {
        mem = MY_STATION_TCAM_2m;
        my_station_shadow = (my_station_tcam_entry_t *)info->my_station2_shadow;
        proft = 1;
    }

    VLAN_CHK_ID(unit, l2addr->vid);

    soc_mem_lock(unit, mem);

    rv = _bcm_td_my_station_lookup(unit, mem, l2addr->mac, l2addr->vid, -1, -1,
                                   &index, &alt_index);
    if (BCM_SUCCESS(rv)) {
#if defined(BCM_TRIDENT_SUPPORT)
        if (soc_feature(unit, soc_feature_l2_cache_use_my_station)) {
            /* Check if the entry is not added by l2cache API */
            if (BCM_SUCCESS(soc_l2u_get(unit, &l2u_entry, index))) {
                if ((SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, L3f) &&
                     soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry, L3f)) ||
                    (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, RESERVED_0f) &&
                     soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,
                                     RESERVED_0f))) {
                    soc_mem_unlock(unit, mem);
                    return BCM_E_EXISTS;
                }
            }
        }
#endif
        /* Entry exist, update the entry */
        entry = &my_station_shadow[index];
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 dest_type;
            old_pid = soc_mem_field32_dest_get(unit, mem, entry,
                                    DESTINATIONf, &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_MYSTA) {
                rv1 = _bcm_l2_mysta_profile_entry_get(unit, &entry_prof, proft,
                                                      old_pid);
            } else {
                rv1 = BCM_E_FAIL;
            }
            if (BCM_FAILURE(rv1)) {
                soc_mem_unlock(unit, mem);
                return rv1;
            }
        }
        _bcm_td_l2_to_my_station(unit, tcam_no, entry, &entry_prof, l2addr, FALSE);
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            if (proft == 1) {
                bcm_l2_station_t station;
                sal_memset(&station, 0, sizeof(bcm_l2_station_t));
                station.flags = BCM_L2_STATION_MPLS | BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6 |
                            BCM_L2_STATION_ARP_RARP;
                _bcm_l2_mysta_station_to_entry(unit, &station, MY_STATION_PROFILE_2m, &entry_prof);
            }

            rv1 = _bcm_l2_mysta_profile_entry_add(unit, &entry_prof, proft, &new_pid);
            LOG_INFO(BSL_LS_BCM_L2,
                     (BSL_META_U(unit,
                                 "_bcm_td_my_station_insert: added proft %d new %d\n"),
                      proft, new_pid));
            if (BCM_SUCCESS(rv1)) {
                soc_mem_field32_dest_set(unit, mem, entry, DESTINATIONf,
                                         SOC_MEM_FIF_DEST_MYSTA, new_pid);
            LOG_INFO(BSL_LS_BCM_L2,
                     (BSL_META_U(unit,
                                 "_bcm_td_my_station_insert: found proft %d old %d new %d\n"),
                      proft, old_pid, new_pid));
                if (old_pid != new_pid)
                     rv1 = _bcm_l2_mysta_profile_entry_delete(unit, proft, old_pid);
            }
            if (BCM_FAILURE(rv1)) {
                soc_mem_unlock(unit, mem);
                return rv1;
            }
        }
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm)) {
            info->my_station_shadow_mask->buf[index/32] |= 1<<(index%32);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        else if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAM_2m)){
            info->my_station2_shadow_mask->buf[index/32] |= 1<<(index%32);
        }
#endif
#endif
    } else if (rv == BCM_E_NOT_FOUND && index == -1) {
        /* No free entry available for insertion */
        rv = BCM_E_FULL;
    }
    if (rv != BCM_E_NOT_FOUND) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    if (alt_index > index) {
        /* Free entry is available for new entry insertion and alternate entry
         * exists. However need to swap these 2 entries to maintain proper
         * lookup precedence */
        my_station_shadow[index] = my_station_shadow[alt_index];
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                           &my_station_shadow[index]);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (mem == MY_STATION_TCAMm) {
            info->my_station_shadow_mask->buf[index/32] |= 1<<(index%32);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        else if (mem == MY_STATION_TCAM_2m){
            info->my_station2_shadow_mask->buf[index/32] |= 1<<(index%32);
        }
#endif
#endif
        index = alt_index;
    }

    /* Add the new entry */
    entry = &my_station_shadow[index];
    sal_memset(entry, 0, sizeof(*entry));
    sal_memset(&entry_prof, 0, sizeof(entry_prof));
    _bcm_td_l2_to_my_station(unit, tcam_no, entry, &entry_prof, l2addr, TRUE);
    if (soc_feature(unit, soc_feature_mysta_profile)) {
        if (proft == 1) {
            bcm_l2_station_t station;
            sal_memset(&station, 0, sizeof(bcm_l2_station_t));
            station.flags = BCM_L2_STATION_MPLS | BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6 |
                            BCM_L2_STATION_ARP_RARP;
            _bcm_l2_mysta_station_to_entry(unit, &station, MY_STATION_PROFILE_2m, &entry_prof);
        }

        rv1 = _bcm_l2_mysta_profile_entry_add(unit, &entry_prof, proft, &new_pid);
        if (BCM_SUCCESS(rv1)) {
            soc_mem_field32_dest_set(unit, mem, entry, DESTINATIONf,
                                     SOC_MEM_FIF_DEST_MYSTA, new_pid);
        } else {
            soc_mem_unlock(unit, mem);
            return rv1;
        }
    }
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm)) {
        info->my_station_shadow_mask->buf[index/32] |= 1<<(index%32);
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    else if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAM_2m)){
        info->my_station2_shadow_mask->buf[index/32] |= 1<<(index%32);
    }
#endif
#endif

    soc_mem_unlock(unit, mem);
    return rv;
}


int
bcm_td_l2_myStation_add(int unit, bcm_l2_addr_t *l2addr)
{
    int rv;

    if (soc_feature(unit, soc_feature_riot) ||
                 (SOC_IS_TRIDENT3X(unit)  && (l2addr->flags & BCM_L2_L3LOOKUP))) {
        rv = bcm_td_l2_myStation_insert(unit, 2, l2addr);
        if (SOC_IS_TRIDENT3X(unit)) {
            /* In TD3 MY_STATION TCAMS have  two times capacity and
               can handle this */
            rv = bcm_td_l2_myStation_insert(unit, 1, l2addr);
        }
    } else {
        rv = bcm_td_l2_myStation_insert(unit, 1, l2addr);
    }
    return rv;
}


/*
 * Function:
 *     bcm_td_l2_myStation_delete
 * Purpose:
 *     Delete MY_STATION_TCAM entry added by L2 API
 * Parameters:
 *     unit              Unit number
 *     mac               MAC address
 *     vlan              VLAN ID
 */
int
bcm_td_l2_myStation_delete_entry(int unit, int tcam_no, bcm_mac_t mac, bcm_vlan_t vlan,
                           int *l2_index)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    int rv = BCM_E_NONE, index, i, entry_words, old_pid = 0;
    uint32 *entry;
#if defined(BCM_TRIDENT_SUPPORT)
    l2u_entry_t l2u_entry;
#endif
    uint32 *l3_mask, *tunnel_mask;
    soc_mem_t mem = MY_STATION_TCAMm;
    soc_memacc_t **memacc_list = NULL;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;
    int proft = 0;
    my_station_profile_1_entry_t entry_prof;
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    int delete_flag = 0;
#endif

    VLAN_CHK_ID(unit, vlan);

    l3_mask = info->my_station_l3_mask;
    tunnel_mask = info->my_station_tunnel_mask;
    entry_words = soc_mem_entry_words(unit, mem);

    if (tcam_no == 2) {
        mem = MY_STATION_TCAM_2m;
        my_station_shadow = (my_station_tcam_entry_t *)info->my_station2_shadow;
        l3_mask = info->my_station2_l3_mask.entry_data;
        tunnel_mask = info->my_station2_tunnel_mask.entry_data;
        proft = 1;
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            soc_mem_field32_set(unit, MY_STATION_PROFILE_2m, tunnel_mask,
                                 MPLS_TERMINATION_ALLOWEDf, 0);
        }

    }

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);

    soc_mem_lock(unit, mem);

    rv = _bcm_td_my_station_lookup(unit, mem, mac, vlan, -1, -1, &index, NULL);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    entry = (uint32 *)&my_station_shadow[index];

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        uint32 dest_type;
        old_pid = soc_mem_field32_dest_get(unit, mem, entry,
                                           DESTINATIONf, &dest_type);
        if (dest_type != SOC_MEM_FIF_DEST_MYSTA) {
            soc_mem_unlock(unit, mem);
            return BCM_E_NOT_FOUND;
        }

        rv = _bcm_l2_mysta_profile_entry_get(unit, &entry_prof, proft, old_pid);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
        entry_words = soc_mem_entry_words(unit, MY_STATION_PROFILE_1m);
        entry = entry_prof.entry_data;
    }

    /* Check if the entry has L3 related flag enabled */
    for (i = 0; i < entry_words; i++) {
        if (entry[i] & l3_mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        soc_mem_unlock(unit, mem);
        return BCM_E_NOT_FOUND;
    }

#if defined(BCM_TRIDENT_SUPPORT)

    if (soc_feature(unit, soc_feature_l2_cache_use_my_station)) {
        /* Check if the entry is not added by l2cache API */
        if (BCM_SUCCESS(soc_l2u_get(unit, &l2u_entry, index))) {
            if ((SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, L3f) &&
                 soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry, L3f)) ||
                (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, RESERVED_0f) &&
                 soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,
                                 RESERVED_0f))) {
                soc_mem_unlock(unit, mem);
                return BCM_E_NOT_FOUND;
            }
        }
    }
#endif
    /* Check to see if we need to delete or modify the entry */
    for (i = 0; i < entry_words; i++) {
        if (entry[i] & tunnel_mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            rv = _bcm_l2_mysta_profile_entry_delete(unit, proft, old_pid);
        }
        /* Delete the entry if no tunnel related flag is enabled */
        entry = (uint32 *)&my_station_shadow[index];
        sal_memset(entry, 0, sizeof(my_station_tcam_entry_t));
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        delete_flag = 1;
#endif
    } else {
        /* Modify the entry if any tunnel related flag is enabled */
        for (i = 0; i < entry_words; i++) {
            entry[i] &= ~l3_mask[i];
        }
        entry = (uint32 *)&my_station_shadow[index];
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 new_pid;
            rv = _bcm_l2_mysta_profile_entry_add(unit, &entry_prof, proft, &new_pid);
            if (BCM_SUCCESS(rv)) {
                soc_mem_field32_dest_set(unit, mem, entry, DESTINATIONf,
                                         SOC_MEM_FIF_DEST_MYSTA, new_pid);
                rv = _bcm_l2_mysta_profile_entry_delete(unit, proft, old_pid);
            }
        }
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, DISCARDf, 0);
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, COPY_TO_CPUf,
                            0);
    }

    if (BCM_SUCCESS(rv)) {
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm) && delete_flag) {
            info->my_station_shadow_mask->buf[index/32] &= ~(1<<(index%32));
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        else if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAM_2m) && delete_flag){
            info->my_station2_shadow_mask->buf[index/32] &= ~(1<<(index%32));
        }
#endif
#endif
    }

    soc_mem_unlock(unit, mem);
    return rv;
}

int
bcm_td_l2_myStation_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan,
                           int *l2_index)
{
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_riot)  ||
                  (SOC_IS_TRIDENT3X(unit))) {
        rv = bcm_td_l2_myStation_delete_entry(unit, 2, mac, vlan, l2_index);
        if (SOC_IS_TRIDENT3X(unit)) {
            rv = bcm_td_l2_myStation_delete_entry(unit, 1, mac, vlan, l2_index);
        }
    } else {
        rv =  bcm_td_l2_myStation_delete_entry(unit, 1, mac, vlan, l2_index);
    }
    return rv;
}

/*
 * Function:
 *     bcm_td_l2_myStation_get
 * Purpose:
 *     Get MY_STATION_TCAM entry added by L2 API
 * Parameters:
 *     unit              Unit number
 *     mac               MAC address
 *     vlan              VLAN ID
 */
int
bcm_td_l2_myStation_get(int unit, bcm_mac_t mac, bcm_vlan_t vlan,
                        bcm_l2_addr_t *l2addr)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    int index, i, entry_words;
    uint32 old_pid = 0;
    uint32 *entry;
#if defined(BCM_TRIDENT_SUPPORT)
    l2u_entry_t l2u_entry;
#endif
    uint32 *l3_mask;
    soc_mem_t mem = MY_STATION_TCAMm;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;
    my_station_profile_1_entry_t entry_prof;
    int proft = 0;
    int rv = BCM_E_NONE;

    VLAN_CHK_ID(unit, vlan);

    l3_mask = info->my_station_l3_mask;

    if (soc_feature(unit, soc_feature_riot) ||
            (SOC_IS_TRIDENT3X(unit))) {

        mem = MY_STATION_TCAM_2m;
        my_station_shadow = (my_station_tcam_entry_t *)info->my_station2_shadow;
        l3_mask = info->my_station2_l3_mask.entry_data;
        proft = 1;
    }

    entry_words = soc_mem_entry_words(unit, mem);

    rv = _bcm_td_my_station_lookup(unit, mem, mac, vlan, -1, -1, &index, NULL);

    if ((rv == BCM_E_NOT_FOUND) && (SOC_IS_TRIDENT3X(unit))) {
        mem = MY_STATION_TCAMm;
        BCM_IF_ERROR_RETURN(
            _bcm_td_my_station_lookup(unit, mem, mac, vlan, -1, -1, &index, NULL));
        my_station_shadow = (my_station_tcam_entry_t *)info->my_station_shadow;
        l3_mask = info->my_station_l3_mask;
        proft = 0;
    } else if (rv != BCM_E_NONE) {
        return rv;
    }

    entry = (uint32 *)&my_station_shadow[index];

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        uint32 dest_type;
        old_pid = soc_mem_field32_dest_get(unit, mem, entry,
                                           DESTINATIONf, &dest_type);
        if (dest_type != SOC_MEM_FIF_DEST_MYSTA) {
            return BCM_E_NOT_FOUND;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_l2_mysta_profile_entry_get(unit, &entry_prof, proft, old_pid));
        entry_words = soc_mem_entry_words(unit, MY_STATION_PROFILE_1m);
        entry = entry_prof.entry_data;
    }

    /* Check if the entry has L3 related flag enabled */
    for (i = 0; i < entry_words; i++) {
        if (entry[i] & l3_mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        return BCM_E_NOT_FOUND;
    }

#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_l2_cache_use_my_station)) {
        /* Check if the entry is not added by l2cache API */
        if (BCM_SUCCESS(soc_l2u_get(unit, &l2u_entry, index))) {
            if ((SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, L3f) &&
                 soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry, L3f)) ||
                (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, RESERVED_0f) &&
                 soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,
                                 RESERVED_0f))) {
                return BCM_E_NOT_FOUND;
            }
        }
    }
#endif

    _bcm_td_l2_from_my_station(unit, l2addr, &my_station_shadow[index],
                               &entry_prof);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_l2cache_myStation_set
 * Purpose:
 *     Add MY_STATION_TCAM entry from L2 cache API
 * Parameters:
 *     unit              Unit number
 *     index             Index
 *     l2caddr           L2 cache API data structure
 */
int
bcm_td_l2cache_myStation_set(int unit, int index, bcm_l2_cache_addr_t *l2caddr)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    soc_mem_t mem = MY_STATION_TCAMm;
    int rv, entry_index, alt_index;
    my_station_tcam_entry_t *entry;
    my_station_profile_1_entry_t entry_prof;
    uint32 new_pid = 0;
#if defined(BCM_TRIDENT_SUPPORT)
    l2u_entry_t l2u_entry;
#endif
    bcm_vlan_t vlan;
    bcm_mac_t mac;
    bcm_port_t port;
    soc_memacc_t **memacc_list = NULL;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;
    int proft = 0;

    if (soc_feature(unit, soc_feature_riot) ||
        (SOC_IS_TRIDENT3X(unit)  && (l2caddr->flags & BCM_L2_CACHE_L3))) {
        mem = MY_STATION_TCAM_2m;
        my_station_shadow = (my_station_tcam_entry_t *)info->my_station2_shadow;
        proft = 1;
    }

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);


    VLAN_CHK_ID(unit, l2caddr->vlan);

    soc_mem_lock(unit, mem);

    entry_index = -1;
    entry = &my_station_shadow[index];
    if (!_BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, VALIDf)) {
        /* Entry is not in used */
        entry_index = index;
    } else {
#if defined(BCM_TRIDENT_SUPPORT)
        if (soc_feature(unit, soc_feature_l2_cache_use_my_station)) {
            /* Check if the existing entry is added by l2cache API */
            rv = soc_l2u_get(unit, &l2u_entry, index);
            if (BCM_SUCCESS(rv) &&
                ((SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, L3f) &&
                  soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry, L3f)) ||
                 (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, RESERVED_0f) &&
                  soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,
                                  RESERVED_0f)))) {
                entry_index = index;
            }
        }
#endif
    }
    if (entry_index != -1) {
        /* The entry is not used by API other than l2cache */
        sal_memset(entry, 0, sizeof(*entry));
        sal_memset(&entry_prof, 0, sizeof(entry_prof));
        _bcm_td_l2cache_to_my_station(unit, entry, &entry_prof, l2caddr);

        if (soc_feature(unit, soc_feature_mysta_profile)) {
            rv = _bcm_l2_mysta_profile_entry_add(unit, &entry_prof, proft, &new_pid);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
            soc_mem_field32_dest_set(unit, mem, entry,
                DESTINATIONf, SOC_MEM_FIF_DEST_MYSTA, new_pid);
        }
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                           entry);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm)) {
            info->my_station_shadow_mask->buf[index/32] |= 1<<(index%32);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        else if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAM_2m)){
            info->my_station2_shadow_mask->buf[index/32] |= 1<<(index%32);
        }
#endif
#endif
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Need to move the current entry */
    vlan = _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, VLAN_IDf);
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_GET(unit, memacc_list, entry, MAC_ADDRf, mac);
    if (soc_feature(unit, soc_feature_gh2_my_station)) {
        if (_BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry,
                                SOURCE_FIELD_MASKf) == 0) {
            port = -1;
        } else {
            port = _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry,
                                       SOURCE_FIELDf);
        }

    } else {
        if (_BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry,
                                ING_PORT_NUM_MASKf) == 0) {
            port = -1;
        } else {
            port = _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry,
                                       ING_PORT_NUMf);
        }
    }
    rv = _bcm_td_my_station_lookup(unit, mem, mac, vlan, port, index, &entry_index,
                                   &alt_index);
    if (rv == BCM_E_NOT_FOUND && entry_index == -1) {
        /* No free entry available for insertion */
        rv = BCM_E_FULL;
    }
    if (rv != BCM_E_NOT_FOUND) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    if (alt_index != -1 &&
        ((port == -1 && alt_index > entry_index) ||
         (port != -1 && alt_index < entry_index))) {
        /* Free entry is available for moving the target entry and alternate
         * entry exists. However need to swap these 2 entries to maintain
         * proper lookup precedence */
        my_station_shadow[entry_index] =
            my_station_shadow[alt_index];
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, entry_index,
                           &my_station_shadow[entry_index]);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm)) {
            info->my_station_shadow_mask->buf[entry_index/32] |= 1<<(entry_index%32);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        else if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAM_2m)){
            info->my_station2_shadow_mask->buf[entry_index/32] |= 1<<(entry_index%32);
        }
#endif
#endif
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
        entry_index = alt_index;
    }

    /* Move the entry at specified index to the available free location */
    my_station_shadow[entry_index] = *entry;
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, entry_index,
                       entry);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm)) {
        info->my_station_shadow_mask->buf[entry_index/32] |= 1<<(entry_index%32);
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    else if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAM_2m)){
        info->my_station2_shadow_mask->buf[entry_index/32] |= 1<<(entry_index%32);
    }
#endif
#endif
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Add the new entry */
    sal_memset(entry, 0, sizeof(*entry));
    sal_memset(&entry_prof, 0, sizeof(entry_prof));
    _bcm_td_l2cache_to_my_station(unit, entry, &entry_prof, l2caddr);
    if (soc_feature(unit, soc_feature_mysta_profile)) {
        rv = _bcm_l2_mysta_profile_entry_add(unit, &entry_prof, proft, &new_pid);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
        soc_mem_field32_dest_set(unit, mem, entry, DESTINATIONf,
                                 SOC_MEM_FIF_DEST_MYSTA, new_pid);
    }
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                       entry);

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm)) {
        info->my_station_shadow_mask->buf[index/32] |= 1<<(index%32);
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    else if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAM_2m)){
        /* coverity[negative_shift] */
        info->my_station2_shadow_mask->buf[index/32] |= 1<<(index%32);
    }
#endif
#endif
    soc_mem_unlock(unit, mem);
    return rv;
}

/*
 * Function:
 *     bcm_td_l2cache_myStation_get
 * Purpose:
 *     Get MY_STATION_TCAM entry added by L2 cache API
 * Parameters:
 *     unit              Unit number
 *     index             Index
 *     l2caddr           L2 cache API data structure
 */
static int
bcm_td_l2cache_myStation_get_entry(int unit, soc_mem_t mem, int index, bcm_l2_cache_addr_t *l2caddr)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    int i, entry_words;
    uint32 old_pid = 0;
    uint32 *entry;
#if defined(BCM_TRIDENT_SUPPORT)
    l2u_entry_t l2u_entry;
#endif
    uint32 *l3_mask;
    soc_memacc_t **memacc_list = NULL;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;
    my_station_profile_1_entry_t entry_prof;
    int proft = 0;

    l3_mask = info->my_station_l3_mask;

    if (mem == MY_STATION_TCAM_2m) {
        my_station_shadow = (my_station_tcam_entry_t *)info->my_station2_shadow;
        l3_mask = info->my_station2_l3_mask.entry_data;
        proft = 1;
    }

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);
    entry_words = soc_mem_entry_words(unit, mem);
    entry = (uint32 *)&my_station_shadow[index];

    /* Check if the entry is valid */
    if (!_BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, VALIDf)) {
        return BCM_E_NOT_FOUND;
    }

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        uint32 dest_type;
        old_pid = soc_mem_field32_dest_get(unit, mem, entry,
                                           DESTINATIONf, &dest_type);
        if (dest_type != SOC_MEM_FIF_DEST_MYSTA) {
            return BCM_E_NOT_FOUND;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_l2_mysta_profile_entry_get(unit, &entry_prof, proft, old_pid));
        entry_words = soc_mem_entry_words(unit, MY_STATION_PROFILE_1m);
        entry = entry_prof.entry_data;
    }

    /* Check if the entry has L3 related flag enabled */
    for (i = 0; i < entry_words; i++) {
        if (entry[i] & l3_mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        return BCM_E_NOT_FOUND;
    }

#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_l2_cache_use_my_station)) {
        /* Check if the entry is added by l2cache API */
        SOC_IF_ERROR_RETURN(soc_l2u_get(unit, &l2u_entry, index));
        if (!((SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, L3f) &&
               soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry, L3f)) ||
              (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, RESERVED_0f) &&
               soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,    
                               RESERVED_0f)))) {
            return BCM_E_NOT_FOUND;
        }
    }
#endif

    _bcm_td_l2cache_from_my_station(unit, l2caddr, &my_station_shadow[index]);

    return BCM_E_NONE;
}

int
bcm_td_l2cache_myStation_get(int unit, int index, bcm_l2_cache_addr_t *l2caddr)
{
    int         rv = BCM_E_NONE;
    soc_mem_t mem = MY_STATION_TCAMm;

    if (soc_feature(unit, soc_feature_riot) ||
                 (SOC_IS_TRIDENT3X(unit)  && (l2caddr->flags & BCM_L2_CACHE_L3))) {
        mem = MY_STATION_TCAM_2m;
    }

    rv = bcm_td_l2cache_myStation_get_entry(unit, mem, index, l2caddr);

    if ((rv == BCM_E_NOT_FOUND) && (SOC_IS_TRIDENT3X(unit))) {
        mem = MY_STATION_TCAMm;
        BCM_IF_ERROR_RETURN(
            bcm_td_l2cache_myStation_get_entry(unit, mem, index, l2caddr));
    }

    return rv;

}

/*
 * Function:
 *     bcm_td_l2cache_myStation_delete
 * Purpose:
 *     Delete MY_STATION_TCAM entry added by L2 cache API
 * Parameters:
 *     unit              Unit number
 *     index             Index
 */
static int
bcm_td_l2cache_myStation_delete_entry(int unit, soc_mem_t mem, int index)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    int rv = BCM_E_NONE, i, entry_words, old_pid = 0;
    uint32 *entry;
#if defined(BCM_TRIDENT_SUPPORT)
    l2u_entry_t l2u_entry;
#endif
    uint32 *l3_mask, *tunnel_mask;
    soc_memacc_t **memacc_list = NULL;
    int proft = 0;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;
    my_station_profile_1_entry_t entry_prof;
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    int delete_flag = 0;
#endif

    l3_mask = info->my_station_l3_mask;
    tunnel_mask = info->my_station_tunnel_mask;

    if (mem == MY_STATION_TCAM_2m) {
        my_station_shadow = (my_station_tcam_entry_t *)info->my_station2_shadow;
        l3_mask = info->my_station2_l3_mask.entry_data;
        tunnel_mask = info->my_station2_tunnel_mask.entry_data;
        proft = 1;
    }

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);
    entry_words = soc_mem_entry_words(unit, mem);

    soc_mem_lock(unit, mem);

    entry = (uint32 *)&my_station_shadow[index];
    /* Check if the entry is valid */
    if (!_BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, VALIDf)) {
        soc_mem_unlock(unit, mem);
        return BCM_E_NOT_FOUND;
    }

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        uint32 dest_type;
        old_pid = soc_mem_field32_dest_get(unit, mem, entry,
                                           DESTINATIONf, &dest_type);
        if (dest_type != SOC_MEM_FIF_DEST_MYSTA) {
            soc_mem_unlock(unit, mem);
            return BCM_E_NOT_FOUND;
        }

        rv = _bcm_l2_mysta_profile_entry_get(unit, &entry_prof, proft, old_pid);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
        entry_words = soc_mem_entry_words(unit, MY_STATION_PROFILE_1m);
        entry = entry_prof.entry_data;
    }

    /* Check if the entry has L3 related flag enabled */
    for (i = 0; i < entry_words; i++) {
        if (entry[i] & l3_mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        soc_mem_unlock(unit, mem);
        return BCM_E_NOT_FOUND;
    }

#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_l2_cache_use_my_station)) {
        if (!soc_feature(unit, soc_feature_my_station_tcam_check)) {
            /* Check if the entry is added by l2cache API */
            SOC_IF_ERROR_RETURN(soc_l2u_get(unit, &l2u_entry, index));
            if (!((SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, L3f) &&
                soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry, L3f)) ||
                (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, RESERVED_0f) &&
                soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,
                               RESERVED_0f)))) {
                soc_mem_unlock(unit, mem);
                return BCM_E_NOT_FOUND;
            }
        }
    }
#endif
    /* Check to see if we need to delete or modify the entry */
    for (i = 0; i < entry_words; i++) {
        if (entry[i] & tunnel_mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            rv = _bcm_l2_mysta_profile_entry_delete(unit, proft, old_pid);
        }
        /* Delete the entry if no tunnel related flag is enabled */
        entry = (uint32 *)&my_station_shadow[index];
        sal_memset(entry, 0, sizeof(my_station_tcam_entry_t));
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        delete_flag = 1;
#endif
    } else {
        /* Modify the entry if any tunnel related flag is enabled */
        for (i = 0; i < entry_words; i++) {
            entry[i] &= ~l3_mask[i];
        }
        entry = (uint32 *)&my_station_shadow[index];
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 new_pid;
            rv = _bcm_l2_mysta_profile_entry_add(unit, &entry_prof, proft, &new_pid);
            if (BCM_SUCCESS(rv)) {
                soc_mem_field32_dest_set(unit, mem, entry, DESTINATIONf,
                                         SOC_MEM_FIF_DEST_MYSTA, new_pid);
                rv = _bcm_l2_mysta_profile_entry_delete(unit, 0, old_pid);
            }
        }
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, DISCARDf, 0);
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, COPY_TO_CPUf, 0);
    }

    if (BCM_SUCCESS(rv)) {
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm) && delete_flag) {
            info->my_station_shadow_mask->buf[index/32] &= ~(1<<(index%32));
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        else if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAM_2m) && delete_flag){
            info->my_station2_shadow_mask->buf[index/32] &= ~(1<<(index%32));
        }
#endif
#endif
    }

    soc_mem_unlock(unit, mem);
    return rv;
}

int
bcm_td_l2cache_myStation_delete(int unit, int index)
{
    soc_mem_t mem = MY_STATION_TCAMm;
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_riot) ||
                 (SOC_IS_TRIDENT3X(unit))) {
        mem = MY_STATION_TCAM_2m;
    }
    rv = bcm_td_l2cache_myStation_delete_entry(unit, mem, index);

    if (SOC_IS_TRIDENT3X(unit) &&
            (rv == BCM_E_NOT_FOUND)) {
             mem = MY_STATION_TCAMm;
             rv = bcm_td_l2cache_myStation_delete_entry(unit, mem, index);
    }
    return rv;

}

/*
 * Function:
 *     bcm_td_l2cache_myStation_lookup
 * Purpose:
 *     Search MY_STATION_TCAM for entry added by L2 cache API
 * Parameters:
 *     unit              Unit number
 *     l2caddr           L2 cache API data structure
 *     result_index      Index
 */
int
bcm_td_l2cache_myStation_lookup(int unit, bcm_l2_cache_addr_t *l2caddr,
                             int *result_index)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    int index, i, num_entries, prof_num_entries = 0;
    int entry_words, entry_prof_words = 0;
    my_station_tcam_entry_t *entry, entry0, mask0;
    my_station_profile_1_entry_t entry_prof, entry0_prof, mask0_prof;
    uint32 *l3_mask;
    bcm_mac_t mac_mask;
    uint32 port_mask;
    soc_mem_t mem = MY_STATION_TCAMm;
    soc_memacc_t **memacc_list = NULL;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;

    l3_mask = info->my_station_l3_mask;

    if (soc_feature(unit, soc_feature_riot) ||
                 (SOC_IS_TRIDENT3X(unit)  && (l2caddr->flags & BCM_L2_CACHE_L3))) {
        mem = MY_STATION_TCAM_2m;
        my_station_shadow = (my_station_tcam_entry_t *)info->my_station2_shadow;
        l3_mask = info->my_station2_l3_mask.entry_data;
    }

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);


    sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
    if (soc_feature(unit, soc_feature_gh2_my_station)) {
        port_mask =
            (1 << soc_mem_field_length(unit, mem, SOURCE_FIELDf)) - 1;
    } else {
        port_mask =
            (1 << soc_mem_field_length(unit, mem, ING_PORT_NUMf)) - 1;
    }

    num_entries = soc_mem_index_count(unit, mem);
    entry_words = soc_mem_entry_words(unit, mem);

    sal_memset(&entry0, 0, sizeof(entry0));
    sal_memset(&entry0_prof, 0, sizeof(entry0_prof));
    _bcm_td_l2cache_to_my_station(unit, &entry0, &entry0_prof, l2caddr);

    sal_memset(&mask0, 0, sizeof(mask0));
    sal_memset(&mask0_prof, 0, sizeof(mask0_prof));
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0, VALIDf,
        (1 << soc_mem_field_length(unit, mem, VALIDf)) - 1);
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0, VLAN_IDf,
                        l2caddr->vlan_mask);
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, &mask0, MAC_ADDRf,
                         l2caddr->mac_mask);
    if (soc_feature(unit, soc_feature_gh2_my_station)) {
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0,
                            SOURCE_FIELDf, l2caddr->src_port_mask);
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0,
                            SOURCE_FIELD_MASKf, port_mask);
    } else {
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0,
                            ING_PORT_NUMf, l2caddr->src_port_mask);
        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0,
                            ING_PORT_NUM_MASKf, port_mask);
    }
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &mask0, VLAN_ID_MASKf, 0xfff);
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, &mask0, MAC_ADDR_MASKf,
                         mac_mask);

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        entry_prof_words = soc_mem_entry_words(unit, MY_STATION_PROFILE_1m);
        prof_num_entries = soc_mem_index_count(unit, MY_STATION_PROFILE_1m);
        for (i = 0; i < entry_prof_words; i++) {
            mask0_prof.entry_data[i] |= l3_mask[i];
        }
    } else {
        for (i = 0; i < entry_words; i++) {
            mask0.entry_data[i] |= l3_mask[i];
        }
    }

    /* Check profile table first */
    if (soc_feature(unit, soc_feature_mysta_profile)) {
        int rv;
        uint32 prof_id = 0xffffffff;
        for (index = 0; index < prof_num_entries; index ++) {
            rv = _bcm_l2_mysta_profile_entry_get(unit, &entry_prof, 0, index);
            if (rv == BCM_E_NOT_FOUND) {
                continue;
            } else if (BCM_FAILURE(rv)) {
                break;
            }
            for (i = 0; i < entry_prof_words; i++) {
                if ((entry_prof.entry_data[i] ^ entry0_prof.entry_data[i]) &
                    mask0_prof.entry_data[i]) {
                    break;
                }
            }

            if (i == entry_prof_words) {
                prof_id = index;
                break;
            }
        }
        if (prof_id == 0xffffffff) {
            soc_mem_field32_set(unit, mem, &entry0, DESTINATIONf, 0);
        } else {
            soc_mem_field32_dest_set(unit, mem, &entry0, DESTINATIONf,
                                     SOC_MEM_FIF_DEST_MYSTA, prof_id);
        }
    }

    for (index = 0; index < num_entries; index++) {
        entry = &my_station_shadow[index];
        /* Try matching the entry */
        for (i = 0; i < entry_words; i++) {
            if ((entry->entry_data[i] ^ entry0.entry_data[i]) &
                mask0.entry_data[i]) {
                break;
            }
        }
        if (i == entry_words) {
            *result_index = index;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
/*
 * Function:
 *      bcm_td_metro_myStation_add
 * Purpose:
 *     Add a MY_STATION_TCAM entry from L2 tunnel API
 *     Shared by MPLS, MIM and TRILL
 * Parameters:
 *     unit              Unit number
 *     mac               MAC address
 *     vlan              VLAN ID
 *     port              Source port number
 *     int               Tunnel (1) or L3 (0) termination
 */
int
bcm_td_metro_myStation_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan,
                           bcm_port_t port, int tunnel_term)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    int rv, rv1, index, alt_index, i, entry_words;
    uint32 *entry;
    uint32 *term_mask;
    bcm_mac_t mac_mask;
    uint32 port_mask;
    soc_mem_t mem = MY_STATION_TCAMm;
    soc_memacc_t **memacc_list = NULL;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;
    my_station_profile_1_entry_t entry_prof;
    uint32 old_pid = 0, new_pid;

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);

    if (tunnel_term) {
        term_mask = info->my_station_tunnel_mask;
    } else {
        term_mask = info->my_station_l3_mask;
    }

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        entry_words = soc_mem_entry_words(unit, MY_STATION_PROFILE_1m);
    } else {
        entry_words = soc_mem_entry_words(unit, mem);
    }

    soc_mem_lock(unit, mem);

    rv = _bcm_td_my_station_lookup(unit, mem, mac, vlan, port, -1, &index,
                                   &alt_index);
    if (BCM_SUCCESS(rv)) {
        /* Entry exist, update the entry */
        entry = (uint32 *)&my_station_shadow[index];
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 dest_type;
            old_pid = soc_mem_field32_dest_get(unit, mem, entry,
                                    DESTINATIONf, &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_MYSTA) {
                rv1 = _bcm_l2_mysta_profile_entry_get(unit, &entry_prof, 0, old_pid);
            } else {
                rv1 = BCM_E_FAIL;
            }
            if (BCM_FAILURE(rv1)) {
                soc_mem_unlock(unit, mem);
                return rv1;
            }
            entry = entry_prof.entry_data;
        }

        for (i = 0; i < entry_words; i++) {
            entry[i] |= term_mask[i];
        }

        entry = (uint32 *)&my_station_shadow[index];
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            rv1 = _bcm_l2_mysta_profile_entry_add(unit, &entry_prof, 0,
                                                  &new_pid);
            if (BCM_SUCCESS(rv1)) {
                soc_mem_field32_dest_set(unit, mem, entry, DESTINATIONf,
                    SOC_MEM_FIF_DEST_MYSTA, new_pid);
                rv1 = _bcm_l2_mysta_profile_entry_delete(unit, 0, old_pid);
            }
            if (BCM_FAILURE(rv1)) {
                soc_mem_unlock(unit, mem);
                return rv1;
            }
        }

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (BCM_SUCCESS(unit) && (mem == MY_STATION_TCAMm)) {
            info->my_station_shadow_mask->buf[index/32] |= 1<<(index%32);
        }
#endif
    } else if (rv == BCM_E_NOT_FOUND && index == -1) {
        /* No free entry available for insertion */
        rv = BCM_E_FULL;
    }
    if (rv != BCM_E_NOT_FOUND) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    if (alt_index != -1 &&
        ((port == -1 && alt_index > index) ||
         (port != -1 && alt_index < index))) {
        /* Free entry is available for new entry insertion and alternate entry
         * exists. However need to swap these 2 entries to maintain proper
         * lookup precedence */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, alt_index,
                          &my_station_shadow[index]);
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                               &my_station_shadow[index]);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
            if (BCM_SUCCESS(unit) && (mem == MY_STATION_TCAMm)) {
                info->my_station_shadow_mask->buf[index/32] |= 1<<(index%32);
            }
#endif
        }
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
        index = alt_index;
    }

    /* Add the new entry */
    entry = (uint32 *)&my_station_shadow[index];
    sal_memset(entry, 0, sizeof(my_station_tcam_entry_t));
    sal_memset(&entry_prof, 0, sizeof(entry_prof));
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, VALIDf,
                                     (1 << soc_mem_field_length(unit, mem, VALIDf)) - 1);
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, VLAN_IDf, vlan);
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, VLAN_ID_MASKf, 0xfff);
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, entry, MAC_ADDRf, mac);
    sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
    _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_SET(unit, memacc_list, entry, MAC_ADDR_MASKf,
                         mac_mask);
    if (port != -1) {
        if (soc_mem_field_valid(unit, mem, SOURCE_FIELDf)) {
            if (SOC_IS_KATANAX(unit)) {
                SOC_IF_ERROR_RETURN(soc_mem_field32_fit(unit, mem,
                            SOURCE_FIELD_MASKf,
                            1 << (SOC_TRUNK_BIT_POS(unit) - 1 )));
                if ((1 << SOC_TRUNK_BIT_POS(unit)) & port){
                    /* trunk */
                    if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
                        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, 
                                            SOURCE_TYPEf, 1);
                        _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, 
                                            SOURCE_TYPE_MASKf, 1);
                        port &= ~(1 << (SOC_TRUNK_BIT_POS(unit)));
                    }    
                }            
            }  
            _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, SOURCE_FIELDf, port);
            port_mask = (1 << soc_mem_field_length(unit, mem, SOURCE_FIELDf)) - 1;
            _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, SOURCE_FIELD_MASKf, port_mask);
        } else {
            _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, ING_PORT_NUMf, port);
            port_mask = (1 << soc_mem_field_length(unit, mem, ING_PORT_NUMf)) - 1;
            _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, entry, ING_PORT_NUM_MASKf, port_mask);
        }
    }

    for (i = 0; i < entry_words; i++) {
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            entry_prof.entry_data[i] |= term_mask[i];
        } else {
            entry[i] |= term_mask[i];
        }
    }

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        rv1 = _bcm_l2_mysta_profile_entry_add(unit, &entry_prof, 0, &new_pid);
        if (BCM_SUCCESS(rv1)) {
            soc_mem_field32_dest_set(unit, mem, entry, DESTINATIONf,
                                     SOC_MEM_FIF_DEST_MYSTA, new_pid);
        } else {
            soc_mem_unlock(unit, mem);
            return rv1;
        }
    }

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm)) {
        info->my_station_shadow_mask->buf[index/32] |= 1<<(index%32);
    }
#endif

    soc_mem_unlock(unit, mem);

    if (BCM_SUCCESS(rv) &&
        soc_feature(unit, soc_feature_mysta_profile)) {
        bcm_l2_addr_t l2addr;
        sal_memset(&l2addr, 0, sizeof(bcm_l2_addr_t));
        l2addr.vid = vlan;
        sal_memcpy(&l2addr.mac, mac, sizeof(bcm_mac_t));
        rv = bcm_td_l2_myStation_insert(unit, 2, &l2addr);
    }

    return rv;
}

/*
 * Function:
 *    bcm_td_metro_myStation_delete
 * Purpose:
 *     Delete a MY_STATION_TCAM entry added by L2 tunnel API
 *     Shared by MPLS, MIM and TRILL
 * Parameters:
 *     unit              Unit number
 *     mac               MAC address
 *     vlan              VLAN ID
 *     port              Source port number
 *     int               Tunnel (1) or L3 (0) termination
 */
int
bcm_td_metro_myStation_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan,
                              bcm_port_t port, int tunnel_term)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    int rv, index, i, entry_words;
    uint32 old_pid = 0;
    uint32 *entry;
    uint32 *mask, *mask_remove;
    soc_mem_t mem = MY_STATION_TCAMm;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;
    my_station_profile_1_entry_t entry_prof;
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    int delete_flag = 0;
#endif

    if (tunnel_term) {
        mask = info->my_station_l3_mask;
        mask_remove = info->my_station_tunnel_mask;
    } else {
        mask = info->my_station_tunnel_mask;
        mask_remove = info->my_station_l3_mask;
    }
    entry_words = soc_mem_entry_words(unit, mem);

    soc_mem_lock(unit, mem);

    rv = _bcm_td_my_station_lookup(unit, mem, mac, vlan, port, -1, &index, NULL);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    entry = (uint32 *)&my_station_shadow[index];

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        uint32 dest_type;
        old_pid = soc_mem_field32_dest_get(unit, mem, entry,
                                           DESTINATIONf, &dest_type);
        if (dest_type != SOC_MEM_FIF_DEST_MYSTA) {
            soc_mem_unlock(unit, mem);
            return BCM_E_NOT_FOUND;
        }

        rv = _bcm_l2_mysta_profile_entry_get(unit, &entry_prof, 0, old_pid);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
        entry_words = soc_mem_entry_words(unit, MY_STATION_PROFILE_1m);
        entry = entry_prof.entry_data;
    }

    /* Check if the entry has tunnel related flag enabled */
    for (i = 0; i < entry_words; i++) {
        if (entry[i] & mask_remove[i]) {
            break;
        }
    }
    if (i == entry_words) {
        soc_mem_unlock(unit, mem);
        return BCM_E_NOT_FOUND;
    }

    /* Check to see if we need to delete or modify the entry */
    for (i = 0; i < entry_words; i++) {
        if (entry[i] & mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            rv = _bcm_l2_mysta_profile_entry_delete(unit, 0, old_pid);
        }
        entry = (uint32 *)&my_station_shadow[index];
        /* Delete the entry if no L3 related flag is enabled */
        sal_memset(entry, 0, sizeof(my_station_tcam_entry_t));
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        delete_flag = 1;
#endif
    } else {
        /* Modify the entry if any L3 related flag is enabled */
        for (i = 0; i < entry_words; i++) {
            entry[i] &= ~mask_remove[i];
        }
        entry = (uint32 *)&my_station_shadow[index];
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 new_pid;
            rv = _bcm_l2_mysta_profile_entry_add(unit, &entry_prof, 0, &new_pid);
            if (BCM_SUCCESS(rv)) {
                soc_mem_field32_dest_set(unit, mem, entry, DESTINATIONf,
                                         SOC_MEM_FIF_DEST_MYSTA, new_pid);
                rv = _bcm_l2_mysta_profile_entry_delete(unit, 0, old_pid);
            }
        }
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm) && delete_flag) {
            info->my_station_shadow_mask->buf[index/32] &= ~(1<<(index%32));
        }
#endif
    }

    soc_mem_unlock(unit, mem);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (BCM_SUCCESS(rv) &&
        soc_feature(unit, soc_feature_mysta_profile)) {
        rv = bcm_td_l2_myStation_delete_entry(unit, 2, mac, vlan, NULL);
        if (rv == BCM_E_NOT_FOUND) {
            rv = BCM_E_NONE;
        }
    }
#endif

    return rv;
}

/*
 * Function:
 *     bcm_td_metro_myStation_delete_all
 * Purpose:
 *     Delete all MY_STATION_TCAM entries added by L2 tunnel API
 *     Shared by MPLS, MIM and TRILL
 * Parameters:
 *     unit              Unit number
 */
int
bcm_td_metro_myStation_delete_all(int unit)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    int rv, index, i, num_entries, entry_words;
    uint32 old_pid = 0;
    uint32 *entry;
    my_station_tcam_entry_t valid_mask;
    uint32 *l3_mask, *tunnel_mask;
    soc_mem_t mem = MY_STATION_TCAMm;
    soc_memacc_t **memacc_list = NULL;
    my_station_tcam_entry_t *my_station_shadow = info->my_station_shadow;
    my_station_profile_1_entry_t entry_prof;
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    int delete_flag = 0;
#endif

    memacc_list = _BCM_TRX_MYSTA_MEMACC_LIST(unit, mem);

    sal_memset(&valid_mask, 0, sizeof(valid_mask));
    _BCM_TRX_MYSTA_FIELD32_MEMACC_SET(unit, memacc_list, &valid_mask, VALIDf,
        (1 << soc_mem_field_length(unit, MY_STATION_TCAMm, VALIDf)) - 1);

    l3_mask = info->my_station_l3_mask;
    tunnel_mask = info->my_station_tunnel_mask;
    num_entries = soc_mem_index_count(unit, mem);
    entry_words = soc_mem_entry_words(unit, mem);

    soc_mem_lock(unit, mem);

    rv = BCM_E_NONE;
    for (index = 0; index < num_entries; index++) {
        bcm_mac_t mac;
        bcm_vlan_t vlan;
        entry = (uint32 *)&my_station_shadow[index];

        /* Check if the entry is valid */
        for (i = 0; i < entry_words; i++) {
            if (entry[i] & valid_mask.entry_data[i]) {
                break;
            }
        }
        if (i == entry_words) {
            continue;
        }

        /* Valid entries */
        _BCM_TRX_MYSTA_MAC_ADDR_MEMACC_GET(unit, memacc_list, entry, MAC_ADDRf, mac);
        vlan = _BCM_TRX_MYSTA_FIELD32_MEMACC_GET(unit, memacc_list, entry, VLAN_IDf);
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 dest_type;
            old_pid = soc_mem_field32_dest_get(unit, mem, entry,
                                DESTINATIONf, &dest_type);
            if (dest_type != SOC_MEM_FIF_DEST_MYSTA) {
                soc_mem_unlock(unit, mem);
                return BCM_E_NOT_FOUND;
            }

            rv = _bcm_l2_mysta_profile_entry_get(unit, &entry_prof, 0, old_pid);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
            entry = entry_prof.entry_data;
        }

        if (soc_feature(unit, soc_feature_mysta_profile)) {
            entry_words = soc_mem_entry_words(unit, MY_STATION_PROFILE_1m);
        } else {
            entry_words = soc_mem_entry_words(unit, mem);
        }
        /* Check if the entry has tunnel related flag enabled */
        for (i = 0; i < entry_words; i++) {
            if (entry[i] & tunnel_mask[i]) {
                break;
            }
        }
        if (i == entry_words) {
            continue;
        }

        /* Check to see if we need to delete or modify the entry */
        for (i = 0; i < entry_words; i++) {
            if (entry[i] & l3_mask[i]) {
                break;
            }
        }
        if (i == entry_words) {
            if (soc_feature(unit, soc_feature_mysta_profile)) {
                rv = _bcm_l2_mysta_profile_entry_delete(unit, 0, old_pid);
            }
            entry = (uint32 *)&my_station_shadow[index];
            /* Delete the entry if no L3 related flag is enabled */
            sal_memset(entry, 0, sizeof(my_station_tcam_entry_t));
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
            delete_flag = 1;
#endif
        } else {
            /* Modify the entry if any L3 related flag is enabled */
            for (i = 0; i < entry_words; i++) {
                entry[i] &= ~tunnel_mask[i];
            }
            entry = (uint32 *)&my_station_shadow[index];
            if (soc_feature(unit, soc_feature_mysta_profile)) {
                uint32 new_pid;
                rv = _bcm_l2_mysta_profile_entry_add(unit, &entry_prof, 0, &new_pid);
                if (BCM_SUCCESS(rv)) {
                    soc_mem_field32_dest_set(unit, mem, entry, DESTINATIONf,
                                             SOC_MEM_FIF_DEST_MYSTA, new_pid);
                    rv = _bcm_l2_mysta_profile_entry_delete(unit, 0, old_pid);
                }
            }
        }
        if (!SOC_HW_RESET(unit)) {
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                               entry);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
            if (BCM_SUCCESS(rv) && (mem == MY_STATION_TCAMm) && delete_flag) {
                info->my_station_shadow_mask->buf[index/32] &= ~(1<<(index%32));
            }
#endif
        }

        if (BCM_SUCCESS(rv) &&
            soc_feature(unit, soc_feature_mysta_profile)) {
            rv = bcm_td_l2_myStation_delete_entry(unit, 2, mac, vlan, NULL);
            if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
            }
            if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                return rv;
                }
            }
            }

        soc_mem_unlock(unit, mem);

    return rv;
}
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
/*
 * Function:
 *      _bcm_vlan_vfi_membership_profile_entry_op
 * Purpose:
 *      Internal function for adding/deleting an entry to the
 *      ING/EGR_VLAN_VFI_MEMBERSHIP
 *      tables. Adds an entry to the global shared SW copy of the
 *      ING/EGE_VLAN_VFI_MEMBERSHIP tables.
 *      Shared by VLAN, VFI, SOURCE_VP and STM
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      add     -   (IN)  True if operation is add
 *      egress  -   (IN)  True if direction is egress 
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */

int _bcm_vlan_vfi_membership_profile_entry_op(int unit,
                                              void **entries, 
                                              int entries_per_set,
                                              int add,
                                              int egress,
                                              uint32 *index)
{
    int rv;
    soc_profile_mem_t *profile = NULL;

    LOG_INFO(BSL_LS_BCM_COMMON,
        (BSL_META_U(unit,"!: Add: %d Egress: %d index: %d\n"),
                    add, egress, *index));

    COMMON_LOCK(unit);

    if(!egress) {
        profile = ING_VLAN_VFI_PROFILE(unit);
    } else {
        profile = EGR_VLAN_VFI_PROFILE(unit);
    }

    if(add) {
        rv = soc_profile_mem_add(unit, profile, entries,
                                 entries_per_set, index);
    } else {
        rv = soc_profile_mem_delete(unit, profile, *index);
    }

    COMMON_UNLOCK(unit);
    return rv;

}

/*
 * Function:
 *      _bcm_vlan_vfi_membership_profile_mem_reference
 * Purpose:
 *      Update ING/EGR_VLAN_VFI_PROFILE entry reference count
 *      and entries_per_set information for given index.
 * Parameters:
 *      unit            - (IN) Device number
 *      index           - (IN) Base index for entry to update
 *      entries_per_set - (IN) Number of entries in the set
 *      egress          - (IN) True if egress profile
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_vlan_vfi_membership_profile_mem_reference(int unit,
                                                    int index,
                                                    int entries_per_set,
                                                    int egress)
{
    int rv;
    soc_profile_mem_t *profile = NULL;

    if(!egress) {
        profile = ING_VLAN_VFI_PROFILE(unit);
    } else {
        profile = EGR_VLAN_VFI_PROFILE(unit);
    }

    COMMON_LOCK(unit);

    rv = soc_profile_mem_reference(unit, profile,
                                   index, entries_per_set);

    COMMON_UNLOCK(unit);
    return rv;

}

/*
 * Function:
 *      _bcm_vlan_vfi_mbrship_profile_ref_count
 * Purpose:
 *      Internal function for getting the ref count of a
 *      ING/EGR_VLAN_VFI_MEMBERSHIP table. 
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entires allocated in HW
 *      egress  -  (IN) True if egress
 *      ref_count  -  (OUT) Ref count of this index
 *      BCM_X_XXX
 */
int
_bcm_vlan_vfi_mbrship_profile_ref_count(int unit, int index,
                                        int egress, int *ref_count)
{
    int rv;

    soc_profile_mem_t *profile = NULL;

    if(!egress) {
        profile = ING_VLAN_VFI_PROFILE(unit);
    } else {
        profile = EGR_VLAN_VFI_PROFILE(unit);
    }

    rv = soc_profile_mem_ref_count_get(unit, profile, index, ref_count);
    return rv;
}
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      _bcm_vlan_vfi_untag_profile_entry_op
 * Purpose:
 *      Internal function for adding/deleting an entry to the
 *      ING/EGR_VLAN_VFI_MEMBERSHIP
 *      tables. Adds an entry to the global shared SW copy of the
 *      ING/EGE_VLAN_VFI_MEMBERSHIP tables.
 *      Shared by VLAN, VFI, SOURCE_VP and STM
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      add     -   (IN)  True if operation is add
 *      egress  -   (IN)  True if direction is egress 
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */

int _bcm_vlan_vfi_untag_profile_entry_op(int unit, void **entries, 
                                         int entries_per_set,
                                         int add,
                                         uint32 *index)
{
    int rv;
    soc_profile_mem_t *profile = NULL;

    LOG_INFO(BSL_LS_BCM_COMMON,
        (BSL_META_U(unit,"!: Add: %d index: %d\n"), add, *index));

    COMMON_LOCK(unit);

    profile = EGR_VLAN_VFI_UNTAG_PROFILE(unit);

    if (add) {
        rv = soc_profile_mem_add(unit, profile, entries,
                                 entries_per_set, index);
    } else {
        rv = soc_profile_mem_delete(unit, profile, *index);
    }

    COMMON_UNLOCK(unit);
    return rv;

}
/*
 * Function:
 *      _bcm_vlan_vfi_untag_profile_mem_reference
 * Purpose:
 *      Update ING/EGR_VLAN_VFI_PROFILE entry reference count
 *      and entries_per_set information for given index.
 * Parameters:
 *      unit            - (IN) Device number
 *      index           - (IN) Base index for entry to update
 *      entries_per_set - (IN) Number of entries in the set
 *      egress          - (IN) True if egress profile
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_vlan_vfi_untag_profile_mem_reference(int unit, int index,
                                              int entries_per_set)
{
    int rv;
    soc_profile_mem_t *profile = NULL;

    profile = EGR_VLAN_VFI_UNTAG_PROFILE(unit);

    COMMON_LOCK(unit);

    rv = soc_profile_mem_reference(unit, profile,
                                   index, entries_per_set);

    COMMON_UNLOCK(unit);
    return rv;

}

/*
 * Function:
 *      _bcm_vlan_vfi_mbrship_profile_ref_count
 * Purpose:
 *      Internal function for getting the ref count of a
 *      ING/EGR_VLAN_VFI_MEMBERSHIP table. 
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entires allocated in HW
 *      egress  -  (IN) True if egress
 *      ref_count  -  (OUT) Ref count of this index
 *      BCM_X_XXX
 */
int
_bcm_vlan_vfi_untag_profile_ref_count(int unit, int index, int *ref_count)
{
    int rv;

    soc_profile_mem_t *profile = NULL;
    profile = EGR_VLAN_VFI_UNTAG_PROFILE(unit);

    rv = soc_profile_mem_ref_count_get(unit, profile, index, ref_count);
    return rv;
}
#endif /* BCM_TRIDENT3_SUPPORT */


#ifdef BCM_FLOWTRACKER_SUPPORT
/*
 * Function:
 *   bcmi_ft_group_age_profile_entry_add
 * Purpose:
 *   Internal function for setting the profile
 *   entry for age out of an entry.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   entries -  (IN) entries to be added
 *   entries_per_set  - (IN) numbe rof entries
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_age_profile_entry_add(int unit, void **entries,
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, FT_AGE_OUT(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_age_profile_entry_delete
 * Purpose:
 *   Internal function for setting the profile
 *   entry for age out of an entry.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_age_profile_entry_delete(int unit, int index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, FT_AGE_OUT(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_age_profile_refcount_set
 * Purpose:
 *   Increase ref count on profile index.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_age_profile_refcount_set(int unit, uint32 index)
{
    int rv;

    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, FT_AGE_OUT(unit), index, 1);
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_flow_limit_profile_entry_add
 * Purpose:
 *   Internal function for setting the profile
 *   entry for flow limit entry.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   entries -  (IN) entries to be added
 *   entries_per_set  - (IN) numbe rof entries
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_flow_limit_profile_entry_add(int unit, void **entries,
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, FT_FLOW_LIMIT(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_flow_limit_profile_entry_delete
 * Purpose:
 *   Internal function for removing the profile
 *   entry for flow limit entry.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_flow_limit_profile_entry_delete(int unit, int index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, FT_FLOW_LIMIT(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_flow_limit_profile_refcount_set
 * Purpose:
 *   Increase ref count on profile index.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_flow_limit_profile_refcount_set(int unit, uint32 index)
{
    int rv;

    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, FT_FLOW_LIMIT(unit), index, 1);
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_collector_copy_profile_entry_add
 * Purpose:
 *   Internal function for setting the profile
 *   entry for collector copies.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   entries -  (IN) entries to be added
 *   entries_per_set  - (IN) numbe rof entries
 *   index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_collector_copy_profile_entry_add(int unit, void **entries,
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, FT_COLLECTOR_COPY(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_collector_copy_profile_entry_delete
 * Purpose:
 *   Internal function for removing the profile
 *   entry for collector copies.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_collector_copy_profile_entry_delete(int unit, int index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, FT_COLLECTOR_COPY(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_collector_copy_profile_refcount_set
 * Purpose:
 *   Increase ref count on profile index.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_collector_copy_profile_refcount_set(int unit, uint32 index)
{
    int rv;

    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, FT_COLLECTOR_COPY(unit), index, 1);
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_alu_mask_profile_entry_add
 * Purpose:
 *   Internal function for setting the profile
 *   entry for alu mask profile.
 * Parameters:
 *   unit             - (IN)  Device number.
 *   profile_idx      - (IN)  Profile index
 *   entries          - (IN)  entries to be added
 *   entries_per_set  - (IN)  numbe rof entries
 *   index            - (OUT) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_alu_mask_profile_entry_add(int unit, int profile_idx,
                void **entries, int entries_per_set, uint32 *index)
{
    int rv;

    COMMON_LOCK(unit);
    if (profile_idx == 0) {
        rv = soc_profile_mem_add(unit, FT_ALU_MASK_PROFILE(unit),
                entries, entries_per_set, index);
    } else {
        rv = soc_profile_mem_add(unit, FT_AGG_ALU_MASK_PROFILE(unit),
                entries, entries_per_set, index);
    }
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_alu_mask_profile_entry_delete
 * Purpose:
 *   Internal function for removing the profile
 *   entry for alu mask profile.
 * Parameters:
 *   unit        -  (IN) Device number.
 *   profile_idx -  (IN) Profile Index
 *   index       -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_alu_mask_profile_entry_delete(int unit, int profile_idx,
                                            int index)
{
    int rv;

    COMMON_LOCK(unit);
    if (profile_idx == 0) {
        rv = soc_profile_mem_delete(unit, FT_ALU_MASK_PROFILE(unit), index);
    } else {
        rv = soc_profile_mem_delete(unit, FT_AGG_ALU_MASK_PROFILE(unit), index);
    }
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_alu_mask_profile_refcount_set
 * Purpose:
 *   Increase ref count on alu mask profile index.
 * Parameters:
 *   unit        -  (IN) Device number.
 *   profile_idx -  (IN) Profile Index
 *   index       -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_alu_mask_profile_refcount_set(int unit, int profile_idx,
                                            uint32 index)
{
    int rv;

    COMMON_LOCK(unit);
    if (profile_idx == 0) {
        rv = soc_profile_mem_reference(unit, FT_ALU_MASK_PROFILE(unit),
                index, 1);
    } else {
        rv = soc_profile_mem_reference(unit, FT_AGG_ALU_MASK_PROFILE(unit),
                index, 1);
    }
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_alu_mask_profile_ref_count_get
 * Purpose:
 *   Internal function for getting the ref count
 *   for alu mask profile entries.
 * Parameters:
 *   unit        -  (IN)  Device number.
 *   profile_idx -  (IN)  Profile index
 *   index       -  (IN)  Base index for the entry
 *   ref_cnt     -  (OUT) Ref count of the entry
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_alu_mask_profile_refcount_get(int unit, int profile_idx,
                                            int index, int *ref_cnt)
{
    int rv;

    COMMON_LOCK(unit);
    if (profile_idx == 0) {
        rv = soc_profile_mem_ref_count_get(unit, FT_ALU_MASK_PROFILE(unit),
                index, ref_cnt);
    } else {
        rv = soc_profile_mem_ref_count_get(unit, FT_AGG_ALU_MASK_PROFILE(unit),
                index, ref_cnt);
    }
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_pdd_profile_entry_add
 * Purpose:
 *   Internal function for setting the profile
 *   entry for policy action profile.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   entries -  (IN) entries to be added
 *   entries_per_set  - (IN) numbe rof entries
 *   index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */

int
bcmi_ft_group_pdd_profile_entry_add(int unit, void **entries,
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, FT_PDD_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_pdd_profile_entry_delete
 * Purpose:
 *   Internal function for removing the profile
 *   entry for policy action profile.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_pdd_profile_entry_delete(int unit, int index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, FT_PDD_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_pdd_profile_refcount_set
 * Purpose:
 *   Increase ref count on profile index.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_pdd_profile_refcount_set(int unit, uint32 index)
{
    int rv;

    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, FT_PDD_PROFILE(unit), index, 1);
    COMMON_UNLOCK(unit);

    return rv;
}


/*
 * Function:
 *   bcmi_ft_group_pdd_profile_ref_count_get
 * Purpose:
 *   Internal function for getting the ref count
 *   for pdd profile entries.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entry
 *   ref_cnt -  (OUT) ref count of the entry
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_pdd_profile_refcount_get(int unit,
                            int index, int *ref_cnt)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_ref_count_get(unit, FT_PDD_PROFILE(unit), index,
                                       ref_cnt);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_meter_profile_entry_add
 * Purpose:
 *   Internal function for setting the profile
 *   entry for meter profile.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   entries -  (IN) entries to be added
 *   entries_per_set  - (IN) numbe rof entries
 *   index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */

int
bcmi_ft_group_meter_profile_entry_add(int unit, void **entries,
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, FT_METER_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_meter_profile_entry_delete
 * Purpose:
 *   Internal function for removing the profile
 *   entry for meter profiles.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_meter_profile_entry_delete(int unit, int index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, FT_METER_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_meter_profile_refcount_set
 * Purpose:
 *   Increase ref count on profile index.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_meter_profile_refcount_set(int unit, uint32 index)
{
    int rv;

    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, FT_METER_PROFILE(unit), index, 1);
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_timestamp_profile_entry_add
 * Purpose:
 *   Internal function for setting the profile
 *   entry for timestamps.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   entries -  (IN) entries to be added
 *   entries_per_set  - (IN) numbe rof entries
 *   index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */

int
bcmi_ft_group_timestamp_profile_entry_add(int unit, void **entries,
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, FT_TS_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_timestamp_profile_entry_delete
 * Purpose:
 *   Internal function for removing the profile
 *   entry for timestamps.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_timestamp_profile_entry_delete(int unit, int index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, FT_TS_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_timestamp_profile_refcount_set
 * Purpose:
 *   Increase ref count on profile index.
 * Parameters:
 *   unit    -  (IN) Device number.
 *   index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *   BCM_X_XXX
 */
int
bcmi_ft_group_timestamp_profile_refcount_set(int unit, uint32 index)
{
    int rv;

    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, FT_TS_PROFILE(unit), index, 1);
    COMMON_UNLOCK(unit);

    return rv;
}
#endif /* BCM_FLOWTRACKER_SUPPORT */

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)

/*
 * Function:
 *      bcmi_subport_tunnel_pbmp_profile_entry_add
 * Purpose:
 *      Internal function for setting the profile
 *      entry for subport tunnel pbmp.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) entries to be added
 *      entries_per_set  - (IN) numbe rof entries
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */

int
bcmi_subport_tunnel_pbmp_profile_entry_add(int unit, void **entries,
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, SUBPORT_TUNNEL_PBMP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcmi_subport_tunnel_pbmp_profile_entry_delete
 * Purpose:
 *      Internal function for removing the profile
 *      entry for subport tunnel pbmps.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
bcmi_subport_tunnel_pbmp_profile_entry_delete(int unit, int index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, SUBPORT_TUNNEL_PBMP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}


/*
 * Function:
 *      bcmi_subport_tunnel_pbmp_profile_refcount_set
 * Purpose:
 *      Increase ref count on profile index.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
bcmi_subport_tunnel_pbmp_profile_refcount_set(int unit, int index)
{
    int rv;

    COMMON_LOCK(unit);
    rv = soc_profile_mem_reference(unit, SUBPORT_TUNNEL_PBMP(unit), index, 1);
    COMMON_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_subport_tunnel_pbmp_profile_refcount_get
 * Purpose:
 *      Get ref count on profile index.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      index   -  (IN) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
bcmi_subport_tunnel_pbmp_profile_refcount_get(int unit, int index, int *ref)
{
    int rv;

    COMMON_LOCK(unit);
    rv = soc_profile_mem_ref_count_get(unit,
            SUBPORT_TUNNEL_PBMP(unit), index, ref);
    COMMON_UNLOCK(unit);

    return rv;
}


#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_common_sw_dump
 * Purpose:
 *     Displays common data structure information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 * Note:
 */
void
_bcm_common_sw_dump(int unit)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    soc_mem_t        mem;
    int              i, j, rv;
    int              num_entries;
    int              ref_count;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information Common - Unit %d\n"), unit));

    if (!info->initialized) {
        LOG_CLI((BSL_META_U(unit,
                            "  Common data structures uninitialized!\n")));
    }

    /* MPLS station hash */
    if (NULL != info->mpls_station_hash) {
        if (SOC_MEM_IS_VALID(unit, MY_STATION_TCAMm)) {
            num_entries = soc_mem_index_count(unit, MY_STATION_TCAMm);
        } else {
            num_entries = soc_mem_index_count(unit, MPLS_STATION_TCAMm);
        }

        LOG_CLI((BSL_META_U(unit,
                            "\n  MPLS station index hashes (%d)\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index     Hash\n")));
        ref_count = 0;
        for (i = 0; i < num_entries; i++) {
            if (0 != MPLS_STATION_HASH(unit, i)){
                LOG_CLI((BSL_META_U(unit,
                                    "  %3d      0x%04x\n"),
                         i, MPLS_STATION_HASH(unit, i)));
                ref_count++;
            }
        }
        if (0 == ref_count) {
            LOG_CLI((BSL_META_U(unit,
                                "    All indexes hash to zero.\n")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    /* MAC_DA_PROFILE table */
    if (NULL != info->mac_da_profile) {
        egr_mac_da_profile_entry_t *mac_da_entry_p;
        bcm_mac_t        l2mac;

        mem = EGR_MAC_DA_PROFILEm;
        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                            "  MAC DA Profile Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount - MAC address\n")));

        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->mac_da_profile,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"),
                         rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            mac_da_entry_p =
                SOC_PROFILE_MEM_ENTRY(unit, info->mac_da_profile,
                                      egr_mac_da_profile_entry_t *, i);

            soc_mem_mac_addr_get(unit, mem, mac_da_entry_p,
                                 MAC_ADDRESSf, l2mac);
            LOG_CLI((BSL_META_U(unit,
                                "  %5d %8d    0x%02x%02x%02x%02x%02x%02x\n"),
                     i, ref_count,
                     l2mac[0], l2mac[1], l2mac[2],
                     l2mac[3], l2mac[4], l2mac[5]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    /* LPORT_PROFILE table */
    if (NULL != info->lport_profile) {
        lport_tab_entry_t *lport_entry_p;

        mem = LPORT_TABm;
        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                            "  LPORT_PROFILE_TABLE Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount - LPORT_TAB entry\n")));

        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->lport_profile,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"),
                         rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            lport_entry_p =
                SOC_PROFILE_MEM_ENTRY(unit, info->lport_profile,
                                      lport_tab_entry_t *, i);

            LOG_CLI((BSL_META_U(unit, "  %5d %8d\n"), i, ref_count));
            
            /* Coverity checks 8k stack size, the diag shell thread
             * uses 128k by default
             */
            /* coverity[stack_use_callee] */  
            /* coverity[stack_use_overflow : FALSE] */
            soc_mem_entry_dump(unit, mem, lport_entry_p, BSL_LSS_CLI);
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    /* ING_PRI_CNG_MAP table */
    if (NULL != info->ing_pri_cng_map) {
        ing_pri_cng_map_entry_t *ing_pri_cng_map_entry_p;
        int pri, cng;
        int entries_per_set;
        soc_field_t pri_t;
        soc_profile_mem_t *profile;
        soc_profile_mem_table_t *table;

        if (SOC_IS_TRIDENT3X(unit)) {
            mem = PHB_MAPPING_TBL_1m;
            pri_t = INT_PRIf;
        } else {
            mem = ING_PRI_CNG_MAPm;
            pri_t = PRIf;
        }

        /*
         * Get num_entries from profile table->index_max since
         * some entries are reserved (see the details in _bcm_common_init).
         */
        profile = ING_PRI_CNG_MAP(unit);
        table = &profile->tables[0];
        num_entries = table->index_max;

        LOG_CLI((BSL_META_U(unit,
                            "  ING_PRI_CNG_MAP Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount EntriesPerSet - PRI CNG\n")));

        for (i = 0; i < num_entries; i += _BCM_TR2_PRI_CNG_MAP_SIZE) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->ing_pri_cng_map,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"),
                         rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            for (j = 0; j < _BCM_TR2_PRI_CNG_MAP_SIZE; j++) {
                entries_per_set =
                    info->ing_pri_cng_map->tables[0].entries[i + j].entries_per_set;
                ing_pri_cng_map_entry_p =
                    SOC_PROFILE_MEM_ENTRY(unit, info->ing_pri_cng_map,
                                          ing_pri_cng_map_entry_t *,
                                          i + j);
                pri = soc_mem_field32_get(unit, mem,
                                          ing_pri_cng_map_entry_p,
                                          pri_t);
                cng = soc_mem_field32_get(unit, mem,
                                          ing_pri_cng_map_entry_p,
                                          CNGf);

                LOG_CLI((BSL_META_U(unit,
                                    "  %5d %8d %13d     %2d  %2d\n"),
                         i + j, ref_count, entries_per_set,
                         pri, cng));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

#define _BCM_QOS_PROFILE_ENTRIES_PER_SET 64
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        if (NULL != info->egr_mpls_combo_map) {
            bcm_td3_egr_mpls_combo_dump(unit, info->egr_mpls_combo_map);
        }
    } else 
#endif
    {
        /* EGR_MPLS_PRI_MAPPING & EGR_MPLS_EXP_MAPPING_1 tables */
        if (NULL != info->egr_mpls_combo_map) {
            egr_mpls_pri_mapping_entry_t
                egr_mpls_pri_map_entry[_BCM_QOS_PROFILE_ENTRIES_PER_SET];
            egr_mpls_exp_mapping_1_entry_t
                egr_mpls_exp_map_entry[_BCM_QOS_PROFILE_ENTRIES_PER_SET];
            egr_mpls_exp_mapping_2_entry_t
                egr_mpls_exp_map_2_entry[_BCM_QOS_PROFILE_ENTRIES_PER_SET];
            int pri, new_cfi, cfi, exp;
            int entries_per_set;
            void *entries[3];

            mem = EGR_MPLS_PRI_MAPPINGm;
            num_entries = soc_mem_index_count(unit, mem);
            LOG_CLI((BSL_META_U(unit,
                                "  EGR_MPLS_PRI_MAPPING & EGR_MPLS_EXP_MAP Combo Mem\n")));
            LOG_CLI((BSL_META_U(unit,
                                "    Number of entries: %d\n"), num_entries));
            LOG_CLI((BSL_META_U(unit,
                                "    Index RefCount EntriesPerSet - PRI CFI MPLS_EXP\n")));

            for (i = 0; i < num_entries;
                 i += _BCM_QOS_PROFILE_ENTRIES_PER_SET) {
                rv = soc_profile_mem_ref_count_get(unit,
                                                   info->egr_mpls_combo_map,
                                                   i, &ref_count);
                if (SOC_FAILURE(rv)) {
                    LOG_CLI((BSL_META_U(unit,
                                        " *** Error retrieving profile reference: %d ***\n"),
                             rv));
                    break;
                }

                if (ref_count <= 0) {
                    continue;
                }

                entries[0] = &egr_mpls_pri_map_entry;
                entries[1] = &egr_mpls_exp_map_entry;
                entries[2] = &egr_mpls_exp_map_2_entry;

                rv = soc_profile_mem_get(unit, info->egr_mpls_combo_map,
                                         i, _BCM_QOS_PROFILE_ENTRIES_PER_SET, entries);
                if (SOC_FAILURE(rv)) {
                    LOG_CLI((BSL_META_U(unit,
                                        " *** Error retrieving profile data: %d ***\n"),
                             rv));
                    break;
                }

                for (j = 0; j < _BCM_QOS_PROFILE_ENTRIES_PER_SET; j++) {
                    entries_per_set =
                        info->egr_mpls_combo_map->tables[0].entries[i + j].entries_per_set;

                    pri = soc_mem_field32_get(unit, mem,
                                              &(egr_mpls_pri_map_entry[j]),
                                              NEW_PRIf);
                    new_cfi = soc_mem_field32_get(unit, mem,
                                                  &(egr_mpls_pri_map_entry[j]),
                                                  NEW_CFIf);
                    cfi = -1;
                    exp = -1;
                    if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_1m)) {
                        cfi = soc_mem_field32_get(unit, EGR_MPLS_EXP_MAPPING_1m,
                                                  &(egr_mpls_exp_map_entry[j]),
                                                  CFIf);
                        exp = soc_mem_field32_get(unit, EGR_MPLS_EXP_MAPPING_1m,
                                                  &(egr_mpls_exp_map_entry[j]),
                                                  MPLS_EXPf);
                        LOG_CLI((BSL_META_U(unit,
                                            "  %5d %8d %13d     %2d  %2d    %2d"),
                                 i + j, ref_count, entries_per_set,
                                 pri, cfi, exp));
                        if (new_cfi != cfi) {
                            LOG_CLI((BSL_META_U(unit,
                                                "   CFI inconsistent!")));
                        }
                    } else {
                        LOG_CLI((BSL_META_U(unit,
                                            "  %5d %8d %13d     %2d  %2d    %2d"),
                                 i + j, ref_count, entries_per_set,
                                 pri, new_cfi, exp));
                    }

                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                }
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
    }

    /* DSCP_TABLE table */
    if (NULL != info->dscp_table) {
        uint32 *dscp_table_entry_p; /* Not using entry specific types */
        int cng, pri, dscp;
        int entries_per_set;
        soc_field_t pri_t;
        
        if (SOC_IS_TRIDENT3X(unit)) {
            mem = PHB_MAPPING_TBL_2m;
            pri_t = INT_PRIf;
        } else {
            mem = DSCP_TABLEm;
            pri_t = PRIf;
        }

        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                            "  DSCP_TABLE Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount EntriesPerSet - CNG PRI DSCP\n")));

        for (i = 0; i < num_entries;
             i += _BCM_QOS_PROFILE_ENTRIES_PER_SET) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->dscp_table,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"),
                         rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            for (j = 0; j < _BCM_QOS_PROFILE_ENTRIES_PER_SET; j++) {
                entries_per_set =
                    info->dscp_table->tables[0].entries[i + j].entries_per_set;
                dscp_table_entry_p =
                    SOC_PROFILE_MEM_ENTRY(unit, info->dscp_table,
                                          uint32 *, i + j);
                cng = soc_mem_field32_get(unit, mem,
                                          dscp_table_entry_p,
                                          CNGf);
                pri = soc_mem_field32_get(unit, mem,
                                          dscp_table_entry_p,
                                          pri_t);
                dscp = soc_mem_field32_get(unit, mem,
                                           dscp_table_entry_p,
                                           DSCPf);

                LOG_CLI((BSL_META_U(unit,
                                    "  %5d %8d %13d     %2d  %2d  %2d\n"),
                         i + j, ref_count, entries_per_set,
                         cng, pri, dscp));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

#ifdef BCM_TRIDENT_SUPPORT
#define _BCM_QOS_ING_OUTER_DOT1P_MAPPING_ENTRIES_PER_SET 16
    if (NULL != info->ing_outer_dot1p) {
        int n_cfi, n_dot1p;
        int entries_per_set;
        ing_outer_dot1p_mapping_table_entry_t *dot1p_entry_p;
        mem = ING_OUTER_DOT1P_MAPPING_TABLEm;

        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                    "  ING_OUTER_DOT1P_MAPPING_TABLE Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                    "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                    "    Index RefCount EntriesPerSet - NewCFI NewDOT1P\n")));

        for (i = 0; i < num_entries;
                i += _BCM_QOS_ING_OUTER_DOT1P_MAPPING_ENTRIES_PER_SET) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->ing_outer_dot1p,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                    " *** Error retrieving profile reference: %d ***\n"), rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            for (j = 0;
                    j < _BCM_QOS_ING_OUTER_DOT1P_MAPPING_ENTRIES_PER_SET; j++) {
                entries_per_set =
                  info->ing_outer_dot1p->tables[0].entries[i+j].entries_per_set;
                dot1p_entry_p =
                    SOC_PROFILE_MEM_ENTRY(unit, info->ing_outer_dot1p,
                        ing_outer_dot1p_mapping_table_entry_t *, i + j);
                n_cfi = soc_mem_field32_get(unit, mem, dot1p_entry_p, NEW_CFIf);
                n_dot1p =
                    soc_mem_field32_get(unit, mem, dot1p_entry_p, NEW_DOT1Pf);

                LOG_CLI((BSL_META_U(unit,
                        "  %5d %8d %13d      %2d       %2d\n"),
                        i + j, ref_count, entries_per_set, n_cfi, n_dot1p));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }
#undef _BCM_QOS_ING_OUTER_DOT1P_MAPPING_ENTRIES_PER_SET
#endif /* BCM_TRIDENT_SUPPORT */

    if(soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
        /* EGR_PRI_CNG_MAP & EGR_DSCP_TABLE tables */
        if (NULL != info->egr_qos_profile) {
            egr_pri_cng_map_entry_t
                egr_pri_cng_map_entry[_BCM_QOS_PROFILE_ENTRIES_PER_SET];
            egr_dscp_table_entry_t
                egr_dscp_table_entry[_BCM_QOS_PROFILE_ENTRIES_PER_SET];
            int pri, cfi, dscp;
            int entries_per_set;
            void *entries[2];

            if (SOC_IS_TRIDENT3X(unit)) {
                mem = EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m;
            } else {
                mem = EGR_PRI_CNG_MAPm;
            }
            num_entries = soc_mem_index_count(unit, mem);
            LOG_CLI((BSL_META_U(unit,
                                "  EGR_PRI_CNG_MAP & EGR_DSCP_TABLE Combo Mem\n")));
            LOG_CLI((BSL_META_U(unit,
                                "    Number of entries: %d\n"), num_entries));
            LOG_CLI((BSL_META_U(unit,
                                "    Index RefCount EntriesPerSet - PRI CFI DSCP\n")));

            for (i = 0; i < num_entries;
                 i += _BCM_QOS_PROFILE_ENTRIES_PER_SET) {
                rv = soc_profile_mem_ref_count_get(unit,
                                                   info->egr_qos_profile,
                                                   i, &ref_count);
                if (SOC_FAILURE(rv)) {
                    LOG_CLI((BSL_META_U(unit,
                                        " *** Error retrieving profile reference: %d ***\n"),
                             rv));
                    break;
                }

                if (ref_count <= 0) {
                    continue;
                }

                entries[0] = &egr_pri_cng_map_entry;
                entries[1] = &egr_dscp_table_entry;

                rv = soc_profile_mem_get(unit, info->egr_qos_profile,
                                         i, _BCM_QOS_PROFILE_ENTRIES_PER_SET, entries);
                if (SOC_FAILURE(rv)) {
                    LOG_CLI((BSL_META_U(unit,
                                        " *** Error retrieving profile data: %d ***\n"),
                             rv));
                    break;
                }

                for (j = 0; j < _BCM_QOS_PROFILE_ENTRIES_PER_SET; j++) {
                    entries_per_set =
                        info->egr_qos_profile->tables[0].entries[i + j].entries_per_set;

                    pri = soc_mem_field32_get(unit, mem,
                                              &(egr_pri_cng_map_entry[j]),
                                              PRIf);
                    cfi = soc_mem_field32_get(unit, mem,
                                                  &(egr_pri_cng_map_entry[j]),
                                                  CFIf);
                    dscp = -1;
                    if (SOC_MEM_IS_VALID(unit, EGR_DSCP_TABLEm)) {
                        dscp = soc_mem_field32_get(unit, EGR_DSCP_TABLEm,
                                                  &(egr_dscp_table_entry[j]),
                                                  DSCPf);
                        LOG_CLI((BSL_META_U(unit,
                                            "  %5d %8d %13d     %2d  %2d    %2d"),
                                 i + j, ref_count, entries_per_set,
                                 pri, cfi, dscp));
                    } else {
                        LOG_CLI((BSL_META_U(unit,
                                            "  %5d %8d %13d     %2d  %2d    %2d"),
                                 i + j, ref_count, entries_per_set,
                                 pri, cfi, dscp));
                    }

                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                }
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
    }else {
        /* EGR_DSCP_TABLE table */
        if (NULL != info->egr_dscp_table) {
            egr_dscp_table_entry_t *egr_dscp_table_entry_p;
            int entries_per_set;
            int dscp;

            if (SOC_IS_TRIDENT3X(unit)) {
                mem = EGR_ZONE_2_QOS_MAPPING_TABLEm;
            } else {
                mem = EGR_DSCP_TABLEm;
            }
            num_entries = soc_mem_index_count(unit, mem);
            LOG_CLI((BSL_META_U(unit,
                                "  EGR_DSCP_TABLE Mem\n")));
            LOG_CLI((BSL_META_U(unit,
                                "    Number of entries: %d\n"), num_entries));
            LOG_CLI((BSL_META_U(unit,
                                "    Index RefCount EntriesPerSet - DSCP\n")));

            for (i = 0; i < num_entries;
                 i += _BCM_QOS_PROFILE_ENTRIES_PER_SET) {
                rv = soc_profile_mem_ref_count_get(unit,
                                                   info->egr_dscp_table,
                                                   i, &ref_count);
                if (SOC_FAILURE(rv)) {
                    LOG_CLI((BSL_META_U(unit,
                                        " *** Error retrieving profile reference: %d ***\n"),
                             rv));
                    break;
                }

                if (ref_count <= 0) {
                    continue;
                }



                for (j = 0; j < _BCM_QOS_PROFILE_ENTRIES_PER_SET; j++) {
                    entries_per_set =
                        info->egr_dscp_table->tables[0].entries[i + j].entries_per_set;
                    egr_dscp_table_entry_p = (egr_dscp_table_entry_t *)
                        SOC_PROFILE_MEM_ENTRY(unit, info->egr_dscp_table,
                                          uint32 *,
                                              i + j);
                    dscp = soc_mem_field32_get(unit, mem,
                                               egr_dscp_table_entry_p,
                                               DSCPf);

                    LOG_CLI((BSL_META_U(unit,
                                        "  %5d %8d %13d     %2d\n"),
                             i + j, ref_count, entries_per_set,
                             dscp));
                }
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
    }

    if (NULL != info->prot_pkt_ctrl) {
        uint32 prot_pkt_ctrl;
        uint32 igmp_mld_pkt_ctrl;

        num_entries = SOC_REG_NUMELS(unit, PROTOCOL_PKT_CONTROLr);
        LOG_CLI((BSL_META_U(unit,
                            "  PROTOCOL_PKT_CONTROL & IGMP_MLD_PKT_CONTROL\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount -  Protocol    IGMP_MLD\n")));

        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_reg_ref_count_get(unit,
                                               info->prot_pkt_ctrl,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"),
                         rv));
                break;
            }
            if (ref_count <= 0) {
                continue;
            }

            prot_pkt_ctrl = 0;
            igmp_mld_pkt_ctrl = 0;
            rv = _bcm_prot_pkt_ctrl_get(unit, i, &prot_pkt_ctrl,
                                        &igmp_mld_pkt_ctrl);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile register data: %d ***\n"),
                         rv));
                break;
            }

            LOG_CLI((BSL_META_U(unit,
                                "  %5d %8d      0x%08x  0x%08x\n"),
                     i, ref_count, prot_pkt_ctrl, igmp_mld_pkt_ctrl));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        _bcm_flex_stat_type_t fs_type, hw_type;
        uint32 fs_index;
        _bcm_flex_stat_handle_t fs_handle;
        int list_index, list_size;
        const char *fs_type_string[_bcmFlexStatTypeNum] = {
            "Service",
            "Gport",
            "Field",
            "VRF",
            "Vxlt",
            "EgrVxlt"
        };

        LOG_CLI((BSL_META_U(unit,
                            "  Flexible statistics\n")));
        for (hw_type = _bcmFlexStatTypeService;
             hw_type < _bcmFlexStatTypeAllHwNum; hw_type++) {
            
            list_size = FS_HANDLE_LIST_SIZE(unit, hw_type);

            LOG_CLI((BSL_META_U(unit,
                                "    %s type, Mem size %5d, List size %5d\n"),
                     fs_type_string[hw_type],
                     FS_MEM_SIZE(unit, hw_type),
                     list_size));
            if (0 == list_size) {
                continue;
            }
            LOG_CLI((BSL_META_U(unit,
                                "       Handle                  Index      Type\n")));
            for (list_index = 0; list_index < list_size; list_index++) {
                _BCM_FLEX_STAT_HANDLE_ASSIGN(fs_handle,
                          FS_HANDLE_LIST_ELEMENT_HANDLE(unit, hw_type,
                                                        list_index));
                fs_index = FS_HANDLE_LIST_ELEMENT_INDEX(unit, hw_type,
                                                        list_index);
                fs_type = FS_HANDLE_LIST_ELEMENT_TYPE(unit, hw_type,
                                                      list_index);

                if (fs_type >= _bcmFlexStatTypeNum) {
                    continue;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "       0x%08x 0x%08x %5d        %s\n"),
                         _BCM_FLEX_STAT_HANDLE_WORD_GET(fs_handle, 1),
                         _BCM_FLEX_STAT_HANDLE_WORD_GET(fs_handle, 0),
                         fs_index, fs_type_string[fs_type]));
            }
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_HURRICANE3_SUPPORT
    /* EGR_HEADER_ENCAP_DATA table */
    if (NULL != info->egr_header_encap_data) {
        egr_header_encap_data_entry_t *egr_header_encap_data_entry_p;
        int  pri_select;
        uint32  map_id;
        uint32  fld_value;

        mem = EGR_HEADER_ENCAP_DATAm;
        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                            "  EGR_HEADER_ENCAP_DATA Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount - VALUE32     PRI_SEL MAP_ID\n")));

        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->egr_header_encap_data,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"), rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            egr_header_encap_data_entry_p =
                SOC_PROFILE_MEM_ENTRY(unit, info->egr_header_encap_data,
                                      egr_header_encap_data_entry_t *, i);

            pri_select = soc_mem_field32_get(unit, mem,
                                       egr_header_encap_data_entry_p,
                                       CUSTOM_PRI_SELECTf);
            map_id = soc_mem_field32_get(unit, mem,
                                       egr_header_encap_data_entry_p,
                                       CUSTOM_PRI_MAPPING_PTRf);
            fld_value = soc_mem_field32_get(unit, mem,
                                       egr_header_encap_data_entry_p,
                                       CUSTOM_PACKET_HEADERf);
            LOG_CLI((BSL_META_U(unit,
                                "  %5d %8d     0x%08x  %5d %5d\n"), 
                                i, ref_count, fld_value, pri_select, map_id));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    /* CUSTOM_HEADER_MATCHm table */
    if (NULL != info->custom_header_match) {
        custom_header_match_entry_t *custom_header_match_entry_p;
        int f_valid;
        uint32 f_custom_header;

        mem = CUSTOM_HEADER_MATCHm;
        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                            "  CUSTOM_HEADER_MATCH Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "  Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "  %10s\t%10s\t%10s\t%-20s\n"),
                            "Index", "RefCount", "VALID", "CUSTOM_HEADER"));

        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->custom_header_match,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"), rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            custom_header_match_entry_p =
                SOC_PROFILE_MEM_ENTRY(unit, info->custom_header_match,
                                      custom_header_match_entry_t *, i);

            f_valid = soc_mem_field32_get(unit, mem,
                                       custom_header_match_entry_p,
                                       VALIDf);
            f_custom_header = soc_mem_field32_get(unit, mem,
                                       custom_header_match_entry_p,
                                       CUSTOM_HEADERf);

            LOG_CLI((BSL_META_U(unit,
                                "  %10d\t%10d\t%10d\t0x%-20x\n"), 
                                i, ref_count, f_valid, f_custom_header));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (NULL != info->ing_tunnel_term_map) {
        ing_tunnel_ecn_decap_entry_t *ing_tunnel_ecn_decap_map_p;
        int  inner_ecn;
        int  drop;
        mem = ING_TUNNEL_ECN_DECAPm;
        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                            "  ING_TUNNEL_TERM_MAP Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount      INNER_ECN DROP\n")));

        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->ing_tunnel_term_map,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"), rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            ing_tunnel_ecn_decap_map_p =
                SOC_PROFILE_MEM_ENTRY(unit, info->ing_tunnel_term_map,
                                      ing_tunnel_ecn_decap_entry_t *, i);

            inner_ecn = soc_mem_field32_get(unit, mem,
                                            ing_tunnel_ecn_decap_map_p,
                                            INNER_ECNf);
            drop = soc_mem_field32_get(unit, mem,
                                       ing_tunnel_ecn_decap_map_p,
                                       DROPf);
            LOG_CLI((BSL_META_U(unit,
                                "  %5d %8d       %5d %5d\n"), 
                                i, ref_count, inner_ecn, drop));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
    defined(BCM_TRIDENT3_SUPPORT)
    if (NULL != info->ing_exp_to_ip_ecn_map) {
        ing_exp_to_ip_ecn_mapping_entry_t *ing_exp_to_ip_ecn_map_p;
        int  ecn;
        int  drop;

#if defined(BCM_TRIDENT3_SUPPORT)
        if(soc_feature(unit, soc_feature_td3_style_mpls)) {
            mem = ING_EXP_TO_ECN_MAPPING_1m;
        } else
#endif
        {
            mem = ING_EXP_TO_IP_ECN_MAPPINGm;
        }
        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                            "  ING_EXP_TO_IP_ECN_MAP Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount      ECN DROP\n")));

        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->ing_exp_to_ip_ecn_map,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"), rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            ing_exp_to_ip_ecn_map_p =
                SOC_PROFILE_MEM_ENTRY(unit, info->ing_exp_to_ip_ecn_map,
                                      ing_exp_to_ip_ecn_mapping_entry_t *, i);

            ecn = soc_mem_field32_get(unit, mem,
                                      ing_exp_to_ip_ecn_map_p,
                                      ECNf);
            drop = soc_mem_field32_get(unit, mem,
                                       ing_exp_to_ip_ecn_map_p,
                                       DROPf);
            LOG_CLI((BSL_META_U(unit,
                                "  %5d %8d       %5d %5d \n"), 
                                i, ref_count, ecn, drop));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }
    if (NULL != info->egr_ip_ecn_to_exp_map) {
        egr_ip_ecn_to_exp_mapping_table_entry_t *egr_ip_ecn_to_exp_map_p;
        int  exp;
        int  drop;
        int  mark_enable;
        int  exp_res;
        int  drop_res;
        int  mark_enable_res;

#if defined(BCM_TRIDENT3_SUPPORT)
    if(soc_feature(unit, soc_feature_td3_style_mpls)) {
        mem = EGR_PKT_ECN_TO_EXP_MAPPING_1m;
    } else
#endif
    {
        mem = EGR_IP_ECN_TO_EXP_MAPPING_TABLEm;
    }
        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                            "  EGR_IP_ECN_TO_EXP_MAP Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    RESPONSIVE \n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount      ECN DROP MARK ECN_RES DROP_RES MARK_RES\n")));

        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->egr_ip_ecn_to_exp_map,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"), rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            egr_ip_ecn_to_exp_map_p =
                SOC_PROFILE_MEM_ENTRY(unit, info->egr_ip_ecn_to_exp_map,
                                      egr_ip_ecn_to_exp_mapping_table_entry_t *, 
                                      i);

            exp = soc_mem_field32_get(unit, mem,
                                      egr_ip_ecn_to_exp_map_p,
                                      EXPf);
            drop = soc_mem_field32_get(unit, mem,
                                       egr_ip_ecn_to_exp_map_p,
                                       DROPf);
            mark_enable = soc_mem_field32_get(unit, mem,
                                              egr_ip_ecn_to_exp_map_p,
                                              CHANGE_PACKET_EXPf); 
            exp_res = soc_mem_field32_get(unit, mem,
                                          egr_ip_ecn_to_exp_map_p,
                                          RESPONSIVE_EXPf);
            drop_res = soc_mem_field32_get(unit, mem,
                                           egr_ip_ecn_to_exp_map_p,
                                           RESPONSIVE_DROPf);
            mark_enable_res = soc_mem_field32_get(unit, mem,
                                                  egr_ip_ecn_to_exp_map_p,
                                                  RESPONSIVE_CHANGE_PACKET_EXPf);                 
            LOG_CLI((BSL_META_U(unit,
                                "  %5d %8d       %5d %5d %5d %5d %5d %5d \n"), 
                                i, ref_count, exp, drop, mark_enable, 
                                exp_res, drop_res, mark_enable_res));
        }       
        LOG_CLI((BSL_META_U(unit, "\n")));
    } 
    if (NULL != info->egr_int_cn_to_exp_map) {
        egr_int_cn_to_exp_mapping_table_entry_t *egr_int_cn_to_exp_map_p;
        int  exp;
        int  drop;
        int  mark_enable;
        mem = EGR_INT_CN_TO_EXP_MAPPING_TABLEm;
        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                            "  EGR_INT_CN_TO_EXP_MAP Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount      ECN DROP MARK \n")));

        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->egr_int_cn_to_exp_map,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"), rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            egr_int_cn_to_exp_map_p =
                SOC_PROFILE_MEM_ENTRY(unit, info->egr_int_cn_to_exp_map,
                                      egr_int_cn_to_exp_mapping_table_entry_t *, 
                                      i);

            exp = soc_mem_field32_get(unit, mem,
                                      egr_int_cn_to_exp_map_p,
                                      EXPf);
            drop = soc_mem_field32_get(unit, mem,
                                       egr_int_cn_to_exp_map_p,
                                       DROPf);
            mark_enable = soc_mem_field32_get(unit, mem,
                                              egr_int_cn_to_exp_map_p,
                                              CHANGE_PACKET_EXPf);             
            LOG_CLI((BSL_META_U(unit,
                                "  %5d %8d       %5d %5d %5d \n"), 
                                i, ref_count, exp, drop, mark_enable));
        }       
        LOG_CLI((BSL_META_U(unit, "\n")));
    }        
#endif /* BCM_TOMAHAWK2_SUPPORT or BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (NULL != info->l3_iif_profile) {
        mem = L3_IIF_PROFILEm;
        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit,
                            "  L3 IIF PROFILE Mem\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount      \n")));

        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_mem_ref_count_get(unit,
                                               info->l3_iif_profile,
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"), rv));
                break;
            }

            if (ref_count <= 0) {
                continue;
            }
            
            LOG_CLI((BSL_META_U(unit,
                                "  %5d %8d       \n"), 
                                i, ref_count));
        }       
        LOG_CLI((BSL_META_U(unit, "\n")));
    }
#endif        
    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
