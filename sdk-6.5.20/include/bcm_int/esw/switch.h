/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains internal definitions for switch module to the BCM library.
 */

#ifndef _BCM_INT_SWITCH_H
#define _BCM_INT_SWITCH_H

#include <bcm/switch.h>

typedef int (*xlate_arg_f)(int unit, int arg, int set);

typedef struct {
    bcm_switch_control_t        type;
    uint32                      chip;
    soc_reg_t                   reg;
    soc_field_t                 field;
    xlate_arg_f                 xlate_arg;
    soc_feature_t               feature;
} bcm_switch_binding_t;

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
typedef struct {
   uint32     flag;
   uint32     hw_map;
} _hash_bmap_t;

typedef struct {
    bcm_switch_control_t        type;
    _hash_bmap_t                *map;
    int                         size;
    soc_reg_t                   reg;
    soc_field_t                 field;
} _hash_fieldselect_t;

enum ENHANCE_XGS3_HASH {
     /* WARNING: values given match hardware register; do not modify */
     ENHANCE_XGS3_HASH_KOOPMAN_CRC32HI = 15,
     ENHANCE_XGS3_HASH_KOOPMAN_CRC32LO = 14,
     ENHANCE_XGS3_HASH_ETH_CRC32HI = 13,
     ENHANCE_XGS3_HASH_ETH_CRC32LO = 12,
     ENHANCE_XGS3_HASH_CRC32HI    = 11,
     ENHANCE_XGS3_HASH_CRC32LO    = 10,
     ENHANCE_XGS3_HASH_CRC16CCITT = 9,
     ENHANCE_XGS3_HASH_XOR16      = 8,
     ENHANCE_XGS3_HASH_CRC16XOR8  = 7,
     ENHANCE_XGS3_HASH_CRC16XOR4  = 6,
     ENHANCE_XGS3_HASH_CRC16XOR2  = 5,
     ENHANCE_XGS3_HASH_CRC16XOR1  = 4,
     ENHANCE_XGS3_HASH_CRC16      = 3
};

typedef struct {
    int         idx;
    int         hash_concat;
    int         regmem;
    soc_field_t sub_f;
    soc_field_t offset_f;
    soc_field_t concat_f;
} hash_offset_info_t;
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

/* Following struct contains synce L1 clock recovery port to unicore, warpcore
 * lane map and mux value
 */
typedef struct {
    bcm_port_t  port;                   /* phyical port number */
    uint32      unicore_or_warpcore;    /* unicore -> 0, wrapcore -> 1, 2*/
    uint32      lane_num;
    uint32      recovered_clock_select; /* configured mux value */
} _bcm_l1_port_mx_lane_map_t;

/* Following struct contains AGM monitor state and attribute */
#define _AGM_POOL_ID_CNT        (2)
#define _AGM_POOL_ID_INVALID    (-1)
#define _AGM_ID_INVALID         (0)
#define _AGM_GROUP_ID_INVALID   (-1)
#define _AGM_TIME_ID_INVALID    (-1)
#define _AGM_TIME_ID_DFLT       (0)

typedef struct agm_monitor_s {
    int             agm_pool_id;
    int             in_use;
    int             running;
    int             group_id;
    int             counter_id;
    bcm_switch_agm_info_t attr;
} agm_monitor_t;

extern int
_bcm_th_switch_agm_info(int unit, bcm_switch_agm_id_t agm_id,
                       agm_monitor_t *agm_mnt);

extern int
_bcm_th_switch_agm_update_counter_id(int unit, bcm_switch_agm_id_t agm_id,
                                    int counter_id);

extern int
bcm_th_switch_agm_id_validate(int unit, bcm_switch_agm_id_t agm_id);

extern int
bcm_th_switch_agm_init(int unit);

extern int
bcm_th_switch_agm_deinit(int unit);

extern int
bcm_th_switch_agm_wb_scache_size_get(int unit, int *req_scache_size);

extern int
bcm_th_switch_agm_wb_sync(int unit, uint8 **scache_ptr);

extern int
bcm_th_switch_agm_wb_reinit(int unit, uint8 **scache_ptr);

extern int
bcm_th_switch_agm_info(int unit, bcm_switch_agm_id_t agm_id,
                       agm_monitor_t *agm_mnt);
extern int
bcm_th_switch_agm_fwd_grp_update(int unit, bcm_switch_agm_id_t agm_id,
                                 int group_id);
extern int
bcm_th_switch_agm_id_get_by_group(int unit, int group_id,
                                  bcm_switch_agm_id_t *agm_id);

extern int bcm_th_switch_agm_dump(int unit);

extern int
bcm_th_switch_agm_create(int unit,
                         uint32 options,
                         bcm_switch_agm_info_t *agm_info);
extern int
bcm_th_switch_agm_enable_set(int unit,
                             bcm_switch_agm_id_t agm_id,
                             int enable);
extern int
bcm_th_switch_agm_stat_get(int unit,
                           bcm_switch_agm_id_t agm_id,
                           int nstat,
                           bcm_switch_agm_stat_t *stat_arr);

extern int
bcm_th_switch_agm_stat_clear(int unit, bcm_switch_agm_id_t agm_id);

extern int
bcm_th_switch_agm_destroy(int unit, bcm_switch_agm_id_t agm_id);

extern int
bcm_th_switch_agm_trunk_attach_get(int unit, bcm_switch_agm_id_t agm_id,
                                   bcm_trunk_t *trunk_id);

extern int
bcm_th_switch_agm_l3_ecmp_attach_get(int unit, bcm_switch_agm_id_t agm_id,
                                     bcm_if_t *l3_ecmp_id);

extern int
bcm_th_switch_agm_get(int unit, bcm_switch_agm_info_t *agm_info);

extern int
bcm_th_switch_agm_enable_get(int unit, bcm_switch_agm_id_t agm_id, int *enable);

extern int
bcm_th_switch_agm_traverse(int unit, int flags,
                           bcm_switch_agm_traverse_cb trav_fn,
                           void *user_data);

extern int
bcm_th_agm_stat_get_table_info(int unit, bcm_switch_agm_id_t agm_id,
                                uint32 *num_of_tables, void *table_info);

extern int
bcm_th_agm_stat_attach(int unit, bcm_switch_agm_id_t agm_id,
                    int agm_pid, uint32 counter_id);

extern int
bcm_th_agm_stat_detach(int unit,
    bcm_switch_agm_id_t agm_id, int agm_pid, uint32 ctr_id);

extern int
bcm_th_agm_stat_id_clear(int unit, uint32 stat_counter_id);

extern int
bcm_th_agm_stat_id_get(int unit, bcm_switch_agm_id_t agm_id,
                    int agm_pid, int total_cntr_idxs, uint32 *stat_counter_id);

#if defined(BCM_TOMAHAWK3_SUPPORT)
#if defined(INCLUDE_L3)
extern int
bcm_th3_switch_agm_l3_ecmp_stat_create(int unit,
                                bcm_switch_agm_id_t agm_id,
                                bcm_if_t l3_ecmp_id);

extern int
bcm_th3_switch_agm_l3_ecmp_stat_destroy(int unit, bcm_switch_agm_id_t agm_id);
#endif /* INCLUDE_L3 */

extern int
bcm_th3_switch_agm_stat_get(int unit,
                           bcm_switch_agm_id_t agm_id,
                           int nstat,
                           bcm_switch_agm_stat_t *stat_arr);

extern int
bcm_th3_switch_agm_stat_clear(int unit, bcm_switch_agm_id_t agm_id);
#endif /* BCM_TOMAHAWK3_SUPPORT */

extern int
bcm_th_xgs3_meter_adjust_set(int unit, bcm_port_t port, int arg);


extern int
bcm_th_xgs3_meter_adjust_get(int unit, bcm_port_t port, int *arg);

/* Number of KT2 physical ports = 40 */
extern _bcm_l1_port_mx_lane_map_t _kt2_l1_port_mx_lane_map[40 + 1];

#define MMRP_ETHERTYPE_DEFAULT      0x88f6
#define SRP_ETHERTYPE_DEFAULT       0x1
#define TS_ETHERTYPE_DEFAULT        0x88f7
#define MMRP_MAC_OUI_DEFAULT        0x0180c2
#define SRP_MAC_OUI_DEFAULT         0x0
#define TS_MAC_OUI_DEFAULT          0x0180c2
#define MMRP_MAC_NONOUI_DEFAULT     0x000020
#define SRP_MAC_NONOUI_DEFAULT      0x0
#define TS_MAC_NONOUI_DEFAULT       0x00000e
#define TS_MAC_MSGBITMAP_DEFAULT    0x000d
#define EP_COPY_TOCPU_DEFAULT       0x1
#define TCP_FLAGS_MASK_DEFAULT      0xff


extern int _bcm_esw_switch_init(int unit);
extern int _bcm_esw_switch_detach(int unit);

extern int _bcm_switch_module_type_get(int unit, bcm_module_t mod,
                                       uint32 *mod_type);

extern int _bcm_switch_pkt_info_ecmp_hash_get(int unit,
                                              bcm_switch_pkt_info_t *pkt_info,
                                              bcm_gport_t *dst_gport,
                                              bcm_if_t *dst_intf);
extern int _bcm_td2_switch_pkt_info_hash_get(int unit,
                                             bcm_switch_pkt_info_t *pkt_info,
                                             bcm_gport_t *dst_gport,
                                             bcm_if_t *dst_intf);
extern int _bcm_tr3_switch_pkt_info_hash_get(int unit,
                                             bcm_switch_pkt_info_t *pkt_info,
                                             bcm_gport_t *dst_gport,
                                             bcm_if_t *dst_intf);
extern int _bcm_kt2_switch_pkt_info_hash_get(int unit,
                                             bcm_switch_pkt_info_t *pkt_info,
                                             bcm_gport_t *dst_gport,
                                             bcm_if_t *dst_intf);
extern int _bcm_gh2_switch_pkt_info_hash_get(int unit,
                                             bcm_switch_pkt_info_t *pkt_info,
                                             bcm_gport_t *dst_gport,
                                             bcm_if_t* dst_intf);
#ifdef BCM_TOMAHAWK_SUPPORT
extern int _bcm_th_switch_pkt_info_hash_get(int unit,
                                             bcm_switch_pkt_info_t *pkt_info,
                                             bcm_gport_t *dst_gport,
                                             bcm_if_t *dst_intf);
#endif

#ifdef BCM_SABER2_SUPPORT
extern int _bcm_switch_port_to_phyaddr(int unit, int port);
#endif
extern int _bcm_tr3_repl_head_entry_info_get(int unit, int *free);

extern int _bcm_en_switch_pkt_info_hash_get(int unit,
                                            bcm_switch_pkt_info_t *pkt_info,
                                            bcm_gport_t *dst_gport,
                                            bcm_if_t *dst_intf);

#define _BCM_SWITCH_PKT_INFO_FLAG_TEST(_pkt_info, _flag_suffix) \
        (0 != (((_pkt_info)->flags) & BCM_SWITCH_PKT_INFO_##_flag_suffix))

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
extern int _bcm_switch_olp_port_mac_get(int unit, bcm_port_t port, bcm_mac_t *mac);
extern int _bcm_switch_olp_dglp_get(int unit, int dglp, bcm_mac_t *mac,int *Index);
#endif
/* Switch OLP bookkeeping structure */
typedef struct _bcm_switch_olp_bookeeping_s {
    int             api_ver;
} _bcm_switch_olp_bookeeping_t;

extern _bcm_switch_olp_bookeeping_t _bcm_switch_olp_bk_info[BCM_MAX_NUM_UNITS];
extern int _bcm_esw_switch_olp_init(int unit);

#define _BCM_SWITCH_OLP_INFO(_unit_)   (&_bcm_switch_olp_bk_info[_unit_])
#define _BCM_SWITCH_OLP_APIV(_unit_) \
    (_bcm_switch_olp_bk_info[_unit_].api_ver)
#define _BCM_SWITCH_OLP_APIV_SET(_unit_, v) \
    (_bcm_switch_olp_bk_info[_unit_].api_ver = (v))

/*
 * Following flags are used with _bcm_switch_olp_bookeeping_t.api_ver
 * _BCM_SWITCH_OLP_OPEN_API represents that either of bcm_l2_station_xxx OR
 *                          bcm_switch_olp_l2_addr_xxx can be used at this moment
 *                          for OLP configuration.
 * _BCM_SWITCH_OLP_L2_STATION_API represents that only bcm_l2_station_xxx can be
 *                          used for olp configuration
 * _BCM_SWITCH_OLP_TRUE_API represents that only  bcm_switch_olp_l2_addr_xxx can be
 *                          used for olp configuration
 */
#define _BCM_SWITCH_OLP_OPEN_API                 0
#define _BCM_SWITCH_OLP_L2_STATION_API           1
#define _BCM_SWITCH_OLP_TRUE_API                 2

/* Followings defines would be used for OBM classification */

#define BCM_OBM_CLASSIFIER_NUM_ENTRY_DSCP       64
#define BCM_OBM_CLASSIFIER_NUM_ENTRY_MPLS       8
#define BCM_OBM_CLASSIFIER_NUM_ENTRY_ETAG       8
#define BCM_OBM_CLASSIFIER_NUM_ENTRY_VLAN       8
#define BCM_OBM_CLASSIFIER_NUM_ENTRY_HIGIG2     16
#define BCM_OBM_CLASSIFIER_NUM_ENTRY_FIELD      5

#define BCM_OBM_PRIORITY_MAX                    4
#define BCM_OBM_DSCP_CHUNK_SIZE                 16

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_scache_ptr_get(int unit, soc_scache_handle_t handle,
                                   int create, uint32 size,
                                   uint8 **scache_ptr, uint16 default_ver,
                                   uint16 *recovered_ver);
extern int _bcm_mem_scache_sync(int unit);
extern int _bcm_switch_control_scache_sync(int unit);
extern int _bcm_field_wb_downgrade_config_set(int unit, uint32 warmboot_ver);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_GREYHOUND_SUPPORT
extern int
_bcm_gh_switch_control_port_binding_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg, int *found);
extern int
_bcm_gh_switch_control_port_binding_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg, int *found);
#endif

#ifdef BCM_TRIDENT3_SUPPORT
extern int
_bcm_td3_switch_control_port_binding_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg, int *found);
extern int
_bcm_td3_switch_control_port_binding_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg, int *found);

extern int
_bcm_td3_meter_adjust_set(int unit, bcm_port_t port, int arg);

extern int
_bcm_td3_shaper_adjust_set(int unit, bcm_port_t port, int arg);

extern int
_bcm_td3_meter_adjust_get(int unit, bcm_port_t port, int *arg);

extern int
_bcm_td3_shaper_adjust_get(int unit, bcm_port_t port, int *arg);

extern int
_bcm_td3_dosattack_pkt_size_set(int unit, bcm_switch_control_t type, int arg);

extern int
_bcm_td3_dosattack_pkt_size_get(int unit, bcm_switch_control_t type, int *arg);

extern int
_bcm_td3_gbp_ethertype_set(int unit, int arg);

extern int
_bcm_td3_ing_gbp_ethertype_get(int unit, int *arg);

extern int
_bcm_td3_vxlan_udp_source_port_sel_set(int unit, int mode);

extern int
_bcm_td3_vxlan_udp_source_port_sel_get(int unit, int *mode);

extern int
_bcm_td3_gpe_udp_source_port_sel_set(int unit, int mode);

extern int
_bcm_td3_gpe_udp_source_port_sel_get(int unit, int *mode);

extern int
_bcm_td3_geneve_udp_source_port_sel_set(int unit, int mode);

extern int
_bcm_td3_geneve_udp_source_port_sel_get(int unit, int *mode);

extern int
_bcm_td3_custom_fields_set(int unit, bcm_switch_control_t type, int arg);

extern int
_bcm_td3_custom_fields_get(int unit, bcm_switch_control_t type, int *arg);

extern int
_bcm_td3_large_scale_nat_mc_get(int unit, bcm_switch_control_t type, int *arg);

extern int
_bcm_td3_large_scale_nat_mc_set(int unit, bcm_switch_control_t type, int arg);

extern int
_bcm_td3_large_scale_nat_uc_get(int unit, bcm_switch_control_t type, int *arg);

extern int
_bcm_td3_large_scale_nat_uc_set(int unit, bcm_switch_control_t type, int arg);

extern int
_bcm_td3_ioam_max_payload_length_set(int unit, int payload_length);

extern int
_bcm_td3_ioam_max_payload_length_get(int unit, int *payload_length);

extern int
_bcm_td3_ioam_namespaceid_set(int unit, int namespace_id);

extern int
_bcm_td3_ioam_namespaceid_get(int unit, int *namespace_id);

extern int
_bcm_td3_ioam_protocol_set(int unit, int ioam_proto);

extern int
_bcm_td3_ioam_protocol_get(int unit, int *ioam_proto);

extern int
_bcm_td3_v6linklocal_disposition_set(int unit, int drop_control);

extern int
_bcm_td3_v6linklocal_disposition_get(int unit, int *drop_control);

#endif

#if defined(BCM_HURRICANE4_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
extern int
_bcm_td3_ioam_e2e_protocol_set(int unit, int ioam_proto);

extern int
_bcm_td3_ioam_e2e_protocol_get(int unit, int *ioam_proto);

extern int
_bcm_td3_gbp_type_set(int unit, int gbp_type);

extern int
_bcm_td3_gbp_type_get(int unit, int *gbp_type);

extern int
_bcm_td3_ioam_e2e_gbp_type_set(int unit, int gbp_type);

extern int
_bcm_td3_ioam_e2e_gbp_type_get(int unit, int *gbp_type);


#endif /* BCM_HURRICANE4_SUPPORT & BCM_FIREBOLT6_SUPPORT */

#ifdef BCM_HELIX5_SUPPORT
extern int
_bcm_hx5_switch_dosattack_events_init(int unit);

extern int
_bcm_mcastgbp_default_classid_set(int unit, int arg);

extern int
_bcm_mcastgbp_default_classid_get(int unit, int *arg);
#endif

#ifdef BCM_APACHE_SUPPORT
extern int
_bcm_apache_switch_control_port_binding_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg, int *found);
extern int
_bcm_apache_switch_control_port_binding_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg, int *found);
#endif

#ifdef BCM_TOMAHAWK2_SUPPORT
extern int
_bcm_th2_switch_control_port_binding_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg, int *found);
extern int
_bcm_th2_switch_control_port_binding_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg, int *found);
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
extern int
_bcm_th3_switch_control_port_binding_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg, int *found);
extern int
_bcm_th3_switch_control_port_binding_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg, int *found);
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)

#define OUT
#define IN_OUT

/* tflow mode checker */
typedef struct _bcm_switch_tfow_mode_s {
    int enabled;
} _bcm_switch_tflow_mode_t;

extern
_bcm_switch_tflow_mode_t _bcm_switch_tflow_mode_info[BCM_MAX_NUM_UNITS];

extern int
bcm_th_switch_tflow_wb_scache_size_get(int unit, int *req_scache_size);

extern int
bcm_th_switch_tflow_wb_sync(int unit, uint8 **scache_ptr);

extern int
bcm_th_switch_tflow_wb_reinit(int unit, uint8 **scache_ptr);

extern int
bcm_esw_switch_latency_deinit(int unit);

extern int
bcmi_esw_switch_latency_show(int unit);

extern int
bcmi_esw_switch_latency_diag(int unit);

extern int
bcmi_esw_switch_latency_mem_avail(
    int         unit,
    soc_mem_t   mem);

extern int
bcmi_esw_switch_latency_reg_avail(
    int         unit,
    soc_reg_t   reg);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
bcm_esw_scache_ver_get(
    int               unit,
    int               module,
    OUT uint16* const version);

extern int
bcmi_esw_switch_latency_wb_memsz_get(
    int                 unit,
    OUT uint32* const   mem_sz,
    uint16              scache_ver);

extern int
bcmi_esw_switch_latency_wb_sync(
    int             unit,
    IN_OUT uint8*   wb_data);

extern int
bcmi_esw_switch_latency_wb_recover(
    int            unit,
    uint8* const   wb_data,
    uint16         scache_ver);
#endif

#endif

extern int
bcmi_esw_int_switch_control_set(int unit, bcm_switch_control_t type, int arg);
extern int
bcmi_esw_int_switch_control_get(int unit, bcm_switch_control_t type, int *arg);

#define BCM_FB2_BMC_IFG_MAX     0x3f
#define NUM_MMU_AGE_CYCLES 7
/* IGMP/MLD values */
#define BCM_SWITCH_FORWARD_VALUE        0
#define BCM_SWITCH_DROP_VALUE           1
#define BCM_SWITCH_FLOOD_VALUE          2
#define BCM_SWITCH_RESERVED_VALUE       3
#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_SWITCH_WB_FLEXHASH_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_SWITCH_WB_FLEXHASH_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_SWITCH_WB_FLEXHASH_DEFAULT_VERSION            BCM_SWITCH_WB_FLEXHASH_VERSION_1_1
#define BCM_SWITCH_WB_SCACHE_PART_FLEXHASH                1

#define BCM_SWITCH_WB_OLP_VERSION_1_0                     SOC_SCACHE_VERSION(1,0)
#define BCM_SWITCH_WB_OLP_DEFAULT_VERSION                 BCM_SWITCH_WB_OLP_VERSION_1_0
#define BCM_SWITCH_WB_SCACHE_PART_OLP                     2

#define BCM_SWITCH_WB_ENCAP_VERSION_1_0                     SOC_SCACHE_VERSION(1,0)
#define BCM_SWITCH_WB_ENCAP_DEFAULT_VERSION                 BCM_SWITCH_WB_ENCAP_VERSION_1_0
#define BCM_SWITCH_WB_SCACHE_PART_ENCAP                     3

#define BCM_SWITCH_WB_AGM_VERSION_1_0                     SOC_SCACHE_VERSION(1,0)
#define BCM_SWITCH_WB_AGM_DEFAULT_VERSION                 BCM_SWITCH_WB_AGM_VERSION_1_0
#define BCM_SWITCH_WB_SCACHE_PART_AGM                     4

#define BCM_SWITCH_WB_L2_VERSION_1_0                      SOC_SCACHE_VERSION(1,0)
#define BCM_SWITCH_WB_L2_DEFAULT_VERSION                  BCM_SWITCH_WB_L2_VERSION_1_0
#define BCM_SWITCH_WB_SCACHE_PART_L2                      5

#define BCM_SWITCH_WB_TFLOW_VERSION_1_0                   SOC_SCACHE_VERSION(1,0)
#define BCM_SWITCH_WB_TFLOW_DEFAULT_VERSION               BCM_SWITCH_WB_TFLOW_VERSION_1_0
#define BCM_SWITCH_WB_SCACHE_PART_TFLOW                   8

#define BCM_SWITCH_WB_MISC_VERSION_1_0                    SOC_SCACHE_VERSION(1,0)
#define BCM_SWITCH_WB_MISC_VERSION_1_1                    SOC_SCACHE_VERSION(1,1)
#define BCM_SWITCH_WB_MISC_VERSION_1_2                    SOC_SCACHE_VERSION(1,2)
#define BCM_SWITCH_WB_MISC_VERSION_1_3                    SOC_SCACHE_VERSION(1,3)
#define BCM_SWITCH_WB_MISC_DEFAULT_VERSION                BCM_SWITCH_WB_MISC_VERSION_1_3
#define BCM_SWITCH_WB_SCACHE_PART_MISC                    9

#define BCM_SWITCH_WB_APPL_SIGN_VERSION_1_0               SOC_SCACHE_VERSION(1,0)
#define BCM_SWITCH_WB_APPL_SIGN_DEFAULT_VERSION           BCM_SWITCH_WB_APPL_SIGN_VERSION_1_0
#define BCM_SWITCH_WB_SCAHCE_PART_APPL_SIGN               10

#define _BCM_SYNC_MODULE_BACKUP(unit, mod, ptr, min, max, bksize, rtn, rv) { \
            rv = rtn(unit, mod, ptr, min, max, bksize);       \
            if(!BCM_SUCCESS(rv)) {                            \
                LOG_CLI((BSL_META_U(unit,                     \
                    "Warmboot Sync backup failed for  %s Module\n"), \
                    shr_bprof_stats_name(mod)));              \
                    sal_free(*ptr);                           \
                    return rv;                                \
            }                                                \
        }

#define _BCM_SYNC_MODULE_DIRTY(unit, mod, ptr, min, max, rtn, rv) { \
            rv = rtn(unit, mod, ptr, min, max);       \
            if(!BCM_SUCCESS(rv)) {                            \
                LOG_CLI((BSL_META_U(unit,                     \
                    "Warmboot Sync dirty set failed for  %s Module\n"), \
                    shr_bprof_stats_name(mod)));              \
                    sal_free(*ptr);                           \
                    return rv;                                \
            }                                                \
        }


#define _BCM_SYNC_MODULE(unit, mod, rtn, ptr, rv) {           \
            rv = rtn(unit);                                   \
            if(!_BCM_SYNC_SUCCESS(rv)) {                      \
                LOG_CLI((BSL_META_U(unit,                     \
                    "Warmboot Sync failed for  %s Module\n"), \
                    shr_bprof_stats_name(mod)));              \
                    sal_free(*ptr);                           \
                    return rv;                                \
            }                                                \
        }


#define _BCM_SYNC_SUCCESS(rv) \
        (BCM_SUCCESS(rv) || (BCM_E_INIT == rv) || (BCM_E_UNAVAIL == rv))

extern int
_bcm_esw_switch_scache_sync(int unit);



#endif

#define CLEARMEM(_unit, _mem) \
        if (SOC_MEM_IS_VALID(_unit, _mem) && SOC_MEM_SIZE(_unit, _mem) > 0) {\
            SOC_IF_ERROR_RETURN(soc_mem_clear(_unit, _mem, MEM_BLOCK_ALL, 0));\
        }

#define CLEAR_MEM_FIELD_ALL(_unit, _mem, _field) \
        if (SOC_MEM_IS_VALID(_unit, _mem) && SOC_MEM_SIZE(_unit, _mem) > 0) {\
            SOC_IF_ERROR_RETURN(soc_mem_field_clear_all(_unit, \
                                                        _mem, \
                                                        _field, \
                                                        MEM_BLOCK_ALL, \
                                                        0));\
        }



#endif	/* !_BCM_INT_SWITCH_H */
