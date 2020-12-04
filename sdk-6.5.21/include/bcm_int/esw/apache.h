/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        apache.h
 * Purpose:     Function declarations for apache Internal functions.
 */

#ifndef _BCM_INT_APACHE_H_
#define _BCM_INT_APACHE_H_

#if defined(BCM_APACHE_SUPPORT)

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/vxlan.h>

#define BCM_AP_COS_PORT_TYPE_HSP 0x1 
#define BCM_AP_COS_PORT_TYPE_LLS 0x2
#define BCM_AP_COS_PORT_TYPE_OVERSUB 0x4


/*
 * 8 Priority Groups supported.
 */
#define AP_PRIOROTY_GROUP_ID_MIN 0
#define AP_PRIOROTY_GROUP_ID_MAX 7
/*
 * Inpur priority range.
 * PFC : 0-7
 * SAFC : 0-15
 */
#define AP_PFC_INPUT_PRIORITY_MIN  0
#define AP_PFC_INPUT_PRIORITY_MAX  7
#define AP_SAFC_INPUT_PRIORITY_MIN AP_PFC_INPUT_PRIORITY_MIN
#define AP_SAFC_INPUT_PRIORITY_MAX 15

extern int bcm_ap_port_rate_ingress_set(int unit, bcm_port_t port,
                                         uint32 bandwidth, uint32 burst);
extern int bcm_ap_port_rate_ingress_get(int unit, bcm_port_t port,
                                         uint32 *bandwidth, uint32 *burst);
extern int bcm_ap_port_rate_pause_get(int unit, bcm_port_t port,
                                       uint32 *pause, uint32 *resume);
extern int bcm_ap_port_rate_egress_set(int unit, bcm_port_t port,
                                        uint32 bandwidth, uint32 burst,
                                        uint32 mode);
extern int bcm_ap_port_rate_egress_get(int unit, bcm_port_t port,
                                        uint32 *bandwidth, uint32 *burst,
                                        uint32 *mode);

extern int bcm_apache_flexport_pbmp_update(int unit, bcm_pbmp_t *pbmp);

extern int bcm_apache_port_drain_cells(int unit, int port);
extern int bcm_ap_port_init(int unit);

extern int _bcm_ap_port_lls_config_set(int unit, bcm_port_t port);
extern int _bcm_ap_port_hsp_config_set(int unit, bcm_port_t port);
extern int bcm_ap_cosq_flex_port_update(int unit, bcm_port_t port , 
                                                      int enable); 
extern int
bcm_ap_port_priority_group_mapping_set(int unit, bcm_gport_t gport,
                                        int prio, int priority_group);
extern int
bcm_ap_port_priority_group_mapping_get(int unit, bcm_gport_t gport,
                                        int prio, int *priority_group);
#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_ap_stage_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_ap_init(int unit, _field_control_t *fc);
extern int _bcm_field_ap_qual_tcam_key_mask_set(int unit,
                                                 _field_entry_t *f_ent,
                                                 unsigned validf);
extern int _bcm_field_ap_egress_selcode_set(int unit,
                                             _field_group_t *fg,
                                             uint32 efp_slice_mode, 
                                             uint32 efp_key_mode,
                                             uint32 key_match_type, 
                                             int *parts_count);
extern int _bcm_field_ap_group_construct_quals_add(int unit,
                                                    _field_control_t *fc,
                                                    _field_stage_t   *stage_fc,
                                                    bcm_field_qset_t qset,
                                                    uint8 entry_type,
                                                    _field_group_t *fg
                                                   );
extern int _bcm_field_ap_qual_tcam_key_mask_get(int unit,
                                                _field_entry_t *f_ent,
                                                _field_tcam_t *tcam);
extern int _bcm_field_ap_qualify_PacketRes(int               unit,
                                            bcm_field_entry_t entry,
                                            uint32            *data,
                                            uint32            *mask
                                            );
extern int _bcm_field_ap_qualify_PacketRes_get(int               unit,
                                                bcm_field_entry_t entry,
                                                uint32            *data,
                                                uint32            *mask
                                                );
extern int
_bcm_field_ap_qualify_LoopbackType(bcm_field_LoopbackType_t loopback_type,
                                    uint32                   *tcam_data,
                                    uint32                   *tcam_mask
                                    );
extern int
_bcm_field_ap_qualify_LoopbackType_get(uint8                    tcam_data,
                                        uint8                    tcam_mask,
                                        bcm_field_LoopbackType_t *loopback_type
                                        );
extern int 
_bcm_field_ap_qualify_TunnelType(bcm_field_TunnelType_t tunnel_type,
                                  uint32                 *tcam_data,
                                  uint32                 *tcam_mask
                                  );
extern int 
_bcm_field_ap_qualify_TunnelType_get(uint8                  tcam_data,
                                      uint8                  tcam_mask,
                                      bcm_field_TunnelType_t *tunnel_type
                                      );
extern int _bcm_field_ap_qualify_class(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_qualify_t qual,
                                        uint32 *data,
                                        uint32 *mask
                                        );
extern int _bcm_field_ap_qualify_class_get(int unit,
                                            bcm_field_entry_t entry,
                                            bcm_field_qualify_t qual,
                                            uint32 *data,
                                            uint32 *mask
                                            );
extern int _bcm_field_ap_action_get(int             unit,
                                     soc_mem_t       mem,
                                     _field_entry_t  *f_ent,
                                     int             tcam_idx,
                                     _field_action_t *fa,
                                     uint32          *buf
                                     );
extern int 
_bcm_field_ap_hash_select_profile_ref_count_get(int unit,
                                                 soc_mem_t profile_mem, 
                                                 int index, 
                                                 int *ref_count);
extern int 
_bcm_field_ap_hash_select_profile_delete(int unit, 
                                         soc_mem_t profile_mem, 
                                         int index);
extern int 
_bcm_field_ap_hash_select_profile_alloc(int unit,
                                         _field_entry_t *f_ent,
                                         _field_action_t *fa);
extern int 
_bcm_field_ap_hash_select_profile_get(int unit, 
                                       soc_mem_t profile_mem,
                                       soc_profile_mem_t **hash_select_profile);
extern int
_bcm_field_ap_hash_select_profile_hw_free(int unit, 
                                           _field_entry_t *f_ent, 
                                           uint32 flags);
extern int
_bcm_field_ap_hash_select_profile_hw_alloc (int unit, _field_entry_t *f_ent);

extern int _bcm_field_ap_group_install(int unit, _field_group_t *fg);
extern int _bcm_field_ap_write_slice_map(int unit, _field_stage_t *stage_fc);

extern int _bcm_field_ap_slice_clear(int unit,
                                      _field_group_t *fg,
                                      _field_slice_t *fs);

extern int _bcm_field_ap_selcodes_install(int unit,
                                           _field_group_t *fg,
                                           uint8 slice_num,
                                           bcm_pbmp_t pbmp,
                                           int selcode_idx);

extern int _bcm_field_ap_entry_move(int unit,
                                     _field_entry_t *f_ent,
                                    int parts_count,
                                    int *tcam_idx_old,
                                    int *tcam_idx_new);

extern int _bcm_field_ap_action_support_check(int unit,
                                               _field_entry_t *f_ent,
                                               bcm_field_action_t action,
                                               int *result);

extern int _bcm_field_ap_action_params_check(int unit,
                                              _field_entry_t  *f_ent,
                                              _field_action_t *fa);

extern int _bcm_field_ap_stage_action_support_check(int unit,
                                                     unsigned stage,
                                                     bcm_field_action_t action,
                                                     int *result);
extern int
_bcm_field_ap_lookup_selcodes_install(int            unit,
                                       _field_group_t *fg,
                                       uint8          slice_num,
                                       int            selcode_idx
                                       );
extern int
_bcm_field_ap_lookup_mode_set(int            unit,
                               uint8          slice_num,
                               _field_group_t *fg,
                               uint8          flags
                               );
extern int
_bcm_field_ap_qualify_VlanTranslationHit(int               unit,
                                          bcm_field_entry_t entry,
                                          uint8             *data,
                                          uint8             *mask);
extern int
_bcm_field_ap_qualify_VlanTranslationHit_get(int               unit,
                                              bcm_field_entry_t entry,
                                              uint8             *data,
                                              uint8             *mask);

extern int 
_bcm_field_ap_qualify_ip_type(int unit, _field_entry_t *f_ent, 
                                          bcm_field_IpType_t type);
extern int 
_bcm_field_ap_qualify_ip_type_get(int unit, _field_entry_t *f_ent, 
                                              bcm_field_IpType_t *type);
extern int 
_bcm_field_ap_qualify_ip_type_encode_get(int unit, uint32 hw_data,
                                                     uint32 hw_mask,
                                                     bcm_field_IpType_t *type);

#endif /* BCM_FIELD_SUPPORT */

#define _BCM_AP_L3_PORT_MASK        0x7f
#define _BCM_AP_L3_MODID_MASK       0xff
#define _BCM_AP_L3_MODID_SHIFT      0x7
#define _BCM_AP_L3_DEFAULT_MTU_SIZE 0x3fff

#if defined(INCLUDE_L3)

#define BCM_AP_L3_USE_EMBEDDED_NEXT_HOP(_u_, _intf_, _nhidx_)      \
             ((_nhidx_ == BCM_XGS3_L3_INVALID_INDEX)             && \
              (!BCM_XGS3_L3_EGRESS_IDX_VALID(_u_, _intf_))       && \
              (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(_u_, _intf_)) && \
              (!BCM_XGS3_DVP_EGRESS_IDX_VALID(_u_, _intf_))      && \
              (BCM_XGS3_L3_EGRESS_MODE_ISSET(_u_))               && \
              (soc_feature(unit, soc_feature_l3_extended_host_entry)))
            
#define BCM_AP_L3_HOST_TABLE_MEM(_u_, _intf_, _v6_, _mem_, _nhidx_)                     \
            _mem_ = (_v6_) ? ((BCM_AP_L3_USE_EMBEDDED_NEXT_HOP(_u_, _intf_, _nhidx_))?  \
                             L3_ENTRY_IPV6_MULTICASTm:L3_ENTRY_IPV6_UNICASTm) :          \
                           ((BCM_AP_L3_USE_EMBEDDED_NEXT_HOP(_u_, _intf_, _nhidx_))?    \
                             L3_ENTRY_IPV4_MULTICASTm:L3_ENTRY_IPV4_UNICASTm)

#define BCM_AP_L3_IS_EMBEDDED_NH_TABLE(_mem_, _v6_)   \
    (_mem_ == L3_ENTRY_IPV4_MULTICASTm && !_v6_) || \
    (_mem_ == L3_ENTRY_IPV4_MULTICASTm && _v6_)

#define  BCM_AP_L3_HOST_TABLE_FLD(_u_, _mem_, _v6_, _fld_)                                   \
              _fld_ = (_bcm_l3_fields_t *) INT_TO_PTR((_v6_) ?                                \
                       ((_mem_ == L3_ENTRY_IPV6_UNICASTm) ?                                   \
                         BCM_XGS3_L3_MEM_FIELDS(_u_,v6): BCM_XGS3_L3_MEM_FIELDS(_u_, v6_4)) : \
                       ((_mem_ == L3_ENTRY_IPV4_UNICASTm) ?                                   \
                         BCM_XGS3_L3_MEM_FIELDS(_u_,v4): BCM_XGS3_L3_MEM_FIELDS(_u_, v4_2)))

#define BCM_AP_L3_HOST_ENTRY_BUF(_v6_, _mem_, _buf_, _v4ent_, _v4extent_, _v6ent_, _v6extent_)  \
            _buf_ = ((_v6_) ?                                                        \
                  ((_mem_ == L3_ENTRY_IPV6_UNICASTm) ? (uint32 *) &_v6ent_ :         \
                   (uint32 *) &_v6extent_) :                                         \
                  ((_mem_ == L3_ENTRY_IPV4_UNICASTm) ? (uint32 *) &_v4ent_ :         \
                   (uint32 *) &_v4extent_)) 


/* L3 Internal Functions */
/* Host table support */
extern int _bcm_ap_l3_add(int unit, _bcm_l3_cfg_t *l3cfg, int nh_idx);
extern int _bcm_ap_l3_get(int unit, _bcm_l3_cfg_t *l3cfg, int *nh_idx);
extern int _bcm_ap_l3_del(int unit, _bcm_l3_cfg_t *l3cfg);
extern int _bcm_ap_l3_del_match(int unit, int flags, void *pattern,
                       bcm_xgs3_ent_op_cb cmp_func,
                       bcm_l3_host_traverse_cb notify_cb, void *user_data);
extern int _bcm_ap_l3_traverse(int unit, int flags, uint32 start, uint32 end,
                                 bcm_l3_host_traverse_cb cb, void *user_data);

extern int _bcm_ap_l3_get_by_idx(int unit, void *dma_ptr, int idx,
                                   _bcm_l3_cfg_t *l3cfg, int *nh_idx);
extern int bcm_ap_l3_conflict_get(int unit, bcm_l3_key_t *ipkey, bcm_l3_key_t *cf_array,
                                   int cf_max, int *cf_count);
extern int bcm_ap_l3_replace(int unit, _bcm_l3_cfg_t *l3cfg);
extern int bcm_ap_l3_del_intf(int unit, _bcm_l3_cfg_t *l3cfg, int negate);

#ifdef ALPM_ENABLE
/* ALPM */
extern int _bcm_ap_alpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                             int nh_ecmp_idx);
extern int _bcm_ap_alpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_ap_alpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int
                             *nh_ecmp_idx);
extern int _bcm_ap_alpm_find(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                              int *nh_ecmp_idx);
extern int _bcm_ap_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
#endif


/* L3 IPMC */
extern int _bcm_ap_l3_ipmc_get_by_idx(int unit, void *dma_ptr, int idx,
                                       _bcm_l3_cfg_t *l3cfg);
extern int _bcm_ap_l3_ipmc_get(int unit, _bcm_l3_cfg_t *l3cfg);
extern int _bcm_ap_l3_ipmc_add(int unit, _bcm_l3_cfg_t *l3cfg);
extern int _bcm_ap_l3_ipmc_del(int unit, _bcm_l3_cfg_t *l3cfg);
extern int bcm_ap_ipmc_pim_bidir_init(int unit);
extern int bcm_ap_ipmc_pim_bidir_detach(int unit);
extern int bcm_ap_ipmc_rp_create(int unit, uint32 flags, int *rp_id);
extern int bcm_ap_ipmc_rp_destroy(int unit, int rp_id);
extern int bcm_ap_ipmc_rp_set(int unit, int rp_id, int intf_count,
        bcm_if_t *intf_array);
extern int bcm_ap_ipmc_rp_get(int unit, int rp_id, int intf_max,
        bcm_if_t *intf_array, int *intf_count);
extern int bcm_ap_ipmc_rp_add(int unit, int rp_id, bcm_if_t intf_id);
extern int bcm_ap_ipmc_rp_delete(int unit, int rp_id, bcm_if_t intf_id);
extern int bcm_ap_ipmc_rp_delete_all(int unit, int rp_id);
extern int bcm_ap_ipmc_rp_ref_count_incr(int unit, int rp_id);
extern int bcm_ap_ipmc_rp_ref_count_decr(int unit, int rp_id);
extern int bcm_ap_ipmc_range_add(int unit, int *range_id,
        bcm_ipmc_range_t *range);
extern int bcm_ap_ipmc_range_delete(int unit, int range_id);
extern int bcm_ap_ipmc_range_delete_all(int unit);
extern int bcm_ap_ipmc_range_get(int unit, int range_id,
        bcm_ipmc_range_t *range);
extern int bcm_ap_ipmc_range_size_get(int unit, int *size);
/* AP v6 reserved multicast */
extern int  _bcm_ap_ipv6_reserved_multicast_addr_set(int unit, bcm_ip6_t ip6_addr,
                                                      bcm_ip6_t ip6_mask);
extern int  _bcm_ap_ipv6_reserved_multicast_addr_get(int unit, bcm_ip6_t *ip6_addr,
                                                      bcm_ip6_t *ip6_mask);
#if defined(INCLUDE_BFD)
extern int bcmi_ap_bfd_init(int unit);
#endif /* INCLUDE_BFD */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_ap_ipmc_pim_bidir_scache_size_get(int unit, uint32 *size);
extern int bcm_ap_ipmc_pim_bidir_sync(int unit, uint8 **scache_ptr);
extern int bcm_ap_ipmc_pim_bidir_scache_recover(int unit, uint8 **scache_ptr); 
extern int bcm_ap_ipmc_rp_ref_count_recover(int unit, int rp_id); 
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_ap_ipmc_pim_bidir_sw_dump(int unit);
extern void _bcm_ap_vxlan_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/* Flex stat - L3 host table */
extern bcm_error_t _bcm_ap_l3_host_stat_attach(
            int unit,
            bcm_l3_host_t *info,
            uint32 stat_counter_id);
extern bcm_error_t _bcm_ap_l3_host_stat_detach(
            int unit,
            bcm_l3_host_t *info);
extern bcm_error_t _bcm_ap_l3_host_stat_counter_get(
            int unit,
            int sync_mode,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values);
extern bcm_error_t _bcm_ap_l3_host_stat_counter_set(
            int unit,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values);
extern bcm_error_t _bcm_ap_l3_host_stat_id_get(
            int unit,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 *stat_counter_id);

/* Flex Stat - L3 Route defip */
extern bcm_error_t _bcm_ap_l3_route_stat_attach (
            int                 unit,
            bcm_l3_route_t      *info,
            uint32              stat_counter_id);
extern bcm_error_t    _bcm_ap_l3_route_stat_detach (
            int                 unit,
            bcm_l3_route_t      *info);

extern bcm_error_t _bcm_ap_l3_route_stat_counter_get(
            int                 unit, 
            int                 sync_mode, 
            bcm_l3_route_t      *info,
            bcm_l3_route_stat_t stat, 
            uint32              num_entries, 
            uint32              *counter_indexes, 
            bcm_stat_value_t    *counter_values);

extern bcm_error_t _bcm_ap_l3_route_stat_counter_set(
            int                 unit, 
            bcm_l3_route_t      *info,
            bcm_l3_route_stat_t stat, 
            uint32              num_entries, 
            uint32              *counter_indexes, 
            bcm_stat_value_t    *counter_values);
extern bcm_error_t _bcm_ap_l3_route_stat_multi_get(
            int                 unit, 
            bcm_l3_route_t      *info,
            int                 nstat, 
            bcm_l3_route_stat_t    *stat_arr,
            uint64              *value_arr);

extern bcm_error_t _bcm_ap_l3_route_stat_multi_get32(
            int                 unit, 
            bcm_l3_route_t      *info,
            int                 nstat, 
            bcm_l3_route_stat_t    *stat_arr,
            uint32              *value_arr);

extern bcm_error_t _bcm_ap_l3_route_stat_multi_set(
            int                 unit, 
            bcm_l3_route_t      *info,
            int                 nstat, 
            bcm_l3_route_stat_t    *stat_arr,
            uint64              *value_arr);

extern bcm_error_t _bcm_ap_l3_route_stat_multi_set32(
            int                 unit, 
            bcm_l3_route_t      *info,
            int                 nstat, 
            bcm_l3_route_stat_t  *stat_arr,
            uint32              *value_arr);
extern bcm_error_t _bcm_ap_l3_route_stat_id_get(
            int                 unit,
            bcm_l3_route_t      *info,
            bcm_l3_route_stat_t stat, 
            uint32              *stat_counter_id);
extern bcm_error_t _bcm_ap_ipmc_stat_attach(
            int unit,
            bcm_ipmc_addr_t *info,
            uint32 stat_counter_id);
extern bcm_error_t _bcm_ap_ipmc_stat_detach(
            int unit,
            bcm_ipmc_addr_t *info);
extern bcm_error_t _bcm_ap_ipmc_stat_counter_get(
            int unit,
            int sync_mode,
            bcm_ipmc_addr_t *info,
            bcm_ipmc_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values);
extern bcm_error_t _bcm_ap_ipmc_stat_counter_set(
            int unit,
            bcm_ipmc_addr_t *info,
            bcm_ipmc_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values);
extern bcm_error_t _bcm_ap_ipmc_stat_id_get(
            int unit,
            bcm_ipmc_addr_t *info,
            bcm_ipmc_stat_t stat,
            uint32 *stat_counter_id);
extern  bcm_error_t  _bcm_ap_ipmc_stat_multi_get(
                            int                 unit, 
                            bcm_ipmc_addr_t     *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t     *stat_arr,
                            uint64              *value_arr);
extern bcm_error_t  _bcm_ap_ipmc_stat_multi_get32(
                            int                 unit, 
                            bcm_ipmc_addr_t     *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t     *stat_arr,
                            uint32              *value_arr);
extern bcm_error_t  _bcm_ap_ipmc_stat_multi_set(
                            int                 unit, 
                            bcm_ipmc_addr_t     *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t     *stat_arr,
                            uint64              *value_arr);
extern  bcm_error_t  _bcm_ap_ipmc_stat_multi_set32(
                            int                 unit, 
                            bcm_ipmc_addr_t     *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t     *stat_arr,
                            uint32              *value_arr);

extern int _bcm_ap_l3_ip4_options_free_resources(int unit);
extern int _bcm_ap_l3_ip4_options_profile_init(int unit);
extern int _bcm_ap_l3_ip4_options_profile_create(int unit, uint32 flags, 
                                   bcm_l3_ip4_options_action_t default_action, 
                                   int *ip4_options_profile_id);
extern int _bcm_ap_l3_ip4_options_profile_destroy(int unit, 
                                                   int ip4_options_profile_id);
extern int _bcm_ap_l3_ip4_options_profile_action_set(int unit,
                         int ip4_options_profile_id, 
                         int ip4_option, 
                         bcm_l3_ip4_options_action_t action);
extern int _bcm_ap_l3_ip4_options_profile_action_get(int unit,
                         int ip4_options_profile_id, 
                         int ip4_option, 
                         bcm_l3_ip4_options_action_t *action);
extern int _bcm_ap_l3_ip4_options_profile_multi_get(int unit, int array_size, 
                                      int *ip_options_ids_array, 
                                      int *array_count);
extern int _bcm_ap_l3_ip4_options_profile_action_multi_get(int unit, 
                          uint32 flags,
                          int ip_option_profile_id, int array_size, 
                          bcm_l3_ip4_options_action_t *array, int *array_count);
extern int _bcm_ap_l3_ip4_options_profile_id2idx(int unit, int profile_id, 
                                                  int *hw_idx);
extern int _bcm_ap_l3_ip4_options_profile_idx2id(int unit, int hw_idx, 
                                                  int *profile_id);
extern int _bcm_ap_vp_urpf_mode_set(int unit, bcm_port_t port, int arg);
extern int _bcm_ap_vp_urpf_mode_get(int unit, bcm_port_t port, int *arg);
extern int _bcm_ap_l3_intf_iif_profile_update(int unit, bcm_vlan_t vid,
                                               bcm_vlan_control_vlan_t *control);
extern int _bcm_ap_l3_intf_iif_profile_get(int unit, bcm_vlan_t vid,
                                            bcm_vlan_control_vlan_t *control);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_ap_l3_ip4_options_profile_scache_len_get(int unit, 
                                                          int *wb_alloc_size);
extern int _bcm_ap_l3_ip4_options_recover(int unit, uint8 **scache_ptr);

extern int _bcm_ap_l3_ip4_options_sync(int unit, uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */

/* VXLAN */
extern int bcm_ap_vxlan_udpDestPort_set(int unit, int udpDestPort);
extern int bcm_ap_vxlan_udpSourcePort_set(int unit, int hashEnable);
extern int bcm_ap_vxlan_udpDestPort_get(int unit, int *udpDestPort);
extern int bcm_ap_vxlan_udpSourcePort_get(int unit, int *hashEnable);
extern int bcm_ap_vxlan_egress_get(int unit, bcm_l3_egress_t *egr, int nh_index);
extern int bcm_ap_vxlan_egress_set(int unit, int nh_index, uint32 flags);
extern int bcm_ap_vxlan_egress_reset(int unit, int nh_index);

extern int _bcm_ap_vxlan_port_resolve(
           int unit, 
           bcm_gport_t l2gre_port_id, 
           bcm_if_t encap_id, 
           bcm_module_t *modid, 
           bcm_port_t *port,
           bcm_trunk_t *trunk_id, 
           int *id);

extern  int  bcm_ap_vxlan_lock(int unit);
extern void bcm_ap_vxlan_unlock(int unit);
extern int bcm_ap_vxlan_init(int unit);
extern int bcm_ap_vxlan_cleanup(int unit);
extern int bcm_ap_vxlan_vpn_create(int unit, bcm_vxlan_vpn_config_t *info);
extern int bcm_ap_vxlan_vpn_destroy(int unit, bcm_vpn_t l2vpn);
extern int bcm_ap_vxlan_vpn_destroy_all(int unit);
extern int bcm_ap_vxlan_vpn_traverse(int unit, bcm_vxlan_vpn_traverse_cb cb, void *user_data);
extern int bcm_ap_vxlan_vpn_get( int unit, bcm_vpn_t l2vpn, bcm_vxlan_vpn_config_t *info);
extern int bcm_ap_vxlan_port_add(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t  *vxlan_port);
extern int bcm_ap_vxlan_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t vxlan_port_id);
extern int bcm_ap_vxlan_port_delete_all(int unit, bcm_vpn_t vpn);
extern int bcm_ap_vxlan_port_get(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t *vxlan_port);
extern int bcm_ap_vxlan_port_get_all( int unit, bcm_vpn_t l2vpn, int port_max, bcm_vxlan_port_t *port_array, int *port_count);
extern int bcm_ap_vxlan_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info);
extern int bcm_ap_vxlan_tunnel_initiator_destroy(int unit, bcm_gport_t vxlan_tunnel_id);
extern int bcm_ap_vxlan_tunnel_initiator_destroy_all(int unit);
extern int bcm_ap_vxlan_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info);
extern int bcm_ap_vxlan_tunnel_initiator_traverse(int unit, bcm_tunnel_initiator_traverse_cb cb, void *user_data);
extern int bcm_ap_vxlan_tunnel_terminator_create(int unit, bcm_tunnel_terminator_t *info);
extern int bcm_ap_vxlan_tunnel_terminator_update(int unit, bcm_tunnel_terminator_t *info);
extern int bcm_ap_vxlan_tunnel_terminator_destroy(int unit, bcm_gport_t vxlan_tunnel_id);
extern int bcm_ap_vxlan_tunnel_terminator_destroy_all(int unit);
extern int bcm_ap_vxlan_tunnel_terminator_get( int unit, bcm_tunnel_terminator_t *info);
extern int bcm_ap_vxlan_tunnel_terminator_traverse(int unit, bcm_tunnel_terminator_traverse_cb cb, void *user_data);
extern int  _bcm_ap_vxlan_vpn_get(int unit, int vfi_index, bcm_vlan_t *vid);
extern int bcm_ap_vxlan_multicast_leaf_entry_check(int unit, bcm_ip_t mc_ip_addr, bcm_ip_t  src_ip_addr, int enable_flag);
extern int _bcm_ap_vxlan_vpn_is_eline( int unit, bcm_vpn_t l2vpn, uint8 *isEline);
extern int bcm_ap_vxlan_port_learn_set(int unit, bcm_gport_t vxlan_port_id, uint32 flags);
extern int bcm_ap_vxlan_port_learn_get(int unit, bcm_gport_t vxlan_port_id, uint32 *flags);
extern int bcm_ap_vxlan_trunk_member_add (int unit, bcm_trunk_t trunk_id, 
                                             int trunk_member_count, bcm_port_t *trunk_member_array);
extern int bcm_ap_vxlan_trunk_member_delete(int unit, bcm_trunk_t trunk_id, 
                                             int trunk_member_count, bcm_port_t *trunk_member_array); 
extern int _bcm_ap_vxlan_port_get(int unit, bcm_vpn_t vpn, int vp,
									bcm_vxlan_port_t  *vxlan_port);

/* L2GRE */
extern int bcm_ap_l2gre_match_add(int unit, bcm_l2gre_port_t *l2gre_port, int vp, bcm_vpn_t vpn);
extern int bcm_ap_l2gre_match_delete(int unit,  int vp);
extern int _bcm_ap_l2gre_eline_vp_configure (int unit, int vfi_index, int active_vp, 
                  source_vp_entry_t *svp, int tpid_enable, bcm_l2gre_port_t  *l2gre_port);
extern int _bcm_ap_l2gre_elan_vp_configure (int unit, int vfi_index, int active_vp, 
                  source_vp_entry_t *svp, int tpid_enable, bcm_l2gre_port_t  *l2gre_port);
extern int _bcm_ap_vxlan_port_get(int unit, bcm_vpn_t vpn, int vp,
                                  bcm_vxlan_port_t  *vxlan_port);

/* Port Extender */
extern int bcm_ap_extender_forward_add(int unit,
        bcm_extender_forward_t *extender_forward_entry);
extern int bcm_ap_extender_forward_delete(int unit,
        bcm_extender_forward_t *extender_forward_entry);
extern int bcm_ap_extender_forward_delete_all(int unit);
extern int bcm_ap_extender_forward_get(int unit,
        bcm_extender_forward_t *extender_forward_entry);
extern int bcm_ap_extender_forward_traverse(int unit,
        bcm_extender_forward_traverse_cb cb, void *user_data);

/* VP LAG */
extern int bcm_ap_vp_lag_init(int unit);
extern void bcm_ap_vp_lag_deinit(int unit);
extern int bcm_ap_vp_lag_create(int unit, uint32 flags, bcm_trunk_t *tid);
extern int bcm_ap_vp_lag_info_get(int unit, int *ngroups_vp_lag,
        int *nports_vp_lag);
extern int bcm_ap_vp_lag_set(int unit, bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info, int member_count,
        bcm_trunk_member_t *member_array);
extern int bcm_ap_vp_lag_destroy(int unit, bcm_trunk_t tid);
extern int bcm_ap_vp_lag_get(int unit, bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info, int member_max,
        bcm_trunk_member_t *member_array, int *member_count);
extern int bcm_ap_tid_to_vp_lag_vp(int unit, bcm_trunk_t tid, int *vp_lag_vp);
extern int bcm_ap_vp_lag_vp_to_tid(int unit, int vp_lag_vp, bcm_trunk_t *tid);
extern int bcm_ap_vp_lag_find(int unit, bcm_port_t port, bcm_trunk_t *tid);
extern int bcm_ap_vlan_vp_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_ap_vlan_vp_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_ap_vlan_vp_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp);
extern int bcm_ap_niv_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_ap_niv_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_ap_niv_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp);
extern int bcm_ap_extender_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_ap_extender_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_ap_extender_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp);
extern int bcm_ap_vp_lag_status_get(int unit, int vp_lag_id, int *is_vp_lag,
        int *is_used, int *has_member);
extern int _bcm_esw_num_vp_trunk_groups(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_ap_vp_lag_reinit(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_ap_vp_lag_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern int bcm_ap_multicast_l3_vp_init(int unit);
extern int bcm_ap_multicast_l3_vp_detach(int unit);
extern int bcm_ap_multicast_l3_vp_encap_get(int unit, bcm_multicast_t group,
        bcm_gport_t port, bcm_if_t intf, bcm_if_t *encap_id);
extern int bcm_ap_multicast_l3_vp_next_hop_free(int unit, int intf);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_ap_multicast_l3_vp_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/* ECMP resilient hashing */
extern int bcm_ap_ecmp_rh_init(int unit);
extern void bcm_ap_ecmp_rh_deinit(int unit);
extern int bcm_ap_l3_egress_ecmp_rh_create(int unit,
        bcm_l3_egress_ecmp_t *ecmp, int intf_count, bcm_if_t *intf_array,
        int op, int count, bcm_if_t *intf);
extern int bcm_ap_l3_egress_ecmp_rh_destroy(int unit, bcm_if_t mpintf);
extern int bcm_ap_l3_egress_ecmp_rh_get(int unit, bcm_l3_egress_ecmp_t *ecmp);
extern int bcm_ap_l3_egress_ecmp_rh_ethertype_set(int unit, uint32 flags,
        int ethertype_count, int *ethertype_array);
extern int bcm_ap_l3_egress_ecmp_rh_ethertype_get(int unit, uint32 *flags,
        int ethertype_max, int *ethertype_array, int *ethertype_count);
extern int bcm_ap_l3_egress_ecmp_rh_shared_copy(int unit, int old_ecmp_group,
        int new_ecmp_group);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_ap_ecmp_rh_hw_recover(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_ap_ecmp_rh_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/* Proxy */
extern int bcm_ap_proxy_egress_create(int unit, uint32 flags, bcm_proxy_egress_t *proxy_egr, 
                          bcm_if_t *proxy_intf_id);
extern int  bcm_ap_proxy_egress_destroy(int unit, bcm_if_t proxy_intf_id);
extern int  bcm_ap_proxy_egress_get(int unit, bcm_if_t proxy_intf_id, bcm_proxy_egress_t *proxy_egr);
extern int _bcm_ap_proxy_nh_add(int unit, int idx, void *buf, int max_paths);
extern int _bcm_ap_proxy_nh_get(int unit, int idx, bcm_proxy_egress_t *proxy_nh_entry);
extern int bcm_ap_proxy_egress_traverse(int unit, bcm_proxy_egress_traverse_cb  cb,
                                                        void *user_data);

#endif /* INCLUDE_L3 */

extern int bcm_ap_cosq_init(int unit);

extern int bcm_ap_cosq_detach(int unit, int software_state_only);

extern int bcm_ap_cosq_config_set(int unit, int numq);

extern int bcm_ap_cosq_config_get(int unit, int *numq);

extern int bcm_ap_cosq_mapping_set(int unit, bcm_port_t gport, 
                        bcm_cos_t priority, bcm_cos_queue_t cosq);

extern int bcm_ap_cosq_mapping_get(int unit, bcm_port_t gport,
                        bcm_cos_t priority, bcm_cos_queue_t *cosq);

extern int bcm_ap_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int *weights, int delay);

extern int bcm_ap_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int *weights, int *delay);

extern int bcm_ap_cosq_priority_mapping_get_all(int unit, bcm_gport_t gport,
                                     int index, bcm_cosq_priority_mapping_t type,
                                     int pri_max, int *pri_array,
                                     int *pri_count);

extern int bcm_ap_cosq_sched_weight_max_get(int unit, int mode, 
                                                int *weight_max);

extern int bcm_ap_cosq_discard_set(int unit, uint32 flags);

extern int bcm_ap_cosq_discard_get(int unit, uint32 *flags);

extern int
bcm_ap_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int
bcm_ap_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int
_bcm_ap_cosq_pfc_class_resolve(bcm_switch_control_t sctype, int *type,
                               int *class);

extern int
bcm_ap_cosq_port_pfc_op(int unit, bcm_port_t port,
                        bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                        bcm_gport_t *gport, int gport_count);

extern int
bcm_ap_cosq_port_pfc_get(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype,
                         bcm_gport_t *gport, int gport_count,
                         int *actual_gport_count);

extern int
bcm_ap_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                      bcm_gport_t *gport);

extern int
bcm_ap_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq);

extern int
bcm_ap_cosq_discard_port_get(int unit, bcm_port_t port, 
                 bcm_cos_queue_t cosq, uint32 color, int *drop_start, 
                        int *drop_slope, int *average_time);

extern int
bcm_ap_cosq_gport_delete(int unit, bcm_gport_t gport);

extern int
bcm_ap_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                      int *numq, uint32 *flags);

extern int
bcm_ap_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data);

extern int
bcm_ap_cosq_discard_port_set(int unit, bcm_port_t port, 
                            bcm_cos_queue_t cosq,
                            uint32 color, int drop_start, int drop_slope,
                            int average_time);

extern int
bcm_ap_cosq_gport_attach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq);

extern int
bcm_ap_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq);

extern int
bcm_ap_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                             bcm_cos_queue_t cosq, bcm_gport_t *out_gport);

extern int
bcm_ap_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 min_quantum, uint32 max_quantum,
                               uint32 burst_quantum, uint32 flags);

extern int
bcm_ap_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *min_quantum, uint32 *max_quantum,
                               uint32 *burst_quantum, uint32 *flags);

extern int
bcm_ap_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int pps);

extern int
bcm_ap_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int *pps);

extern int bcm_ap_cosq_port_burst_set(int unit, bcm_port_t port, 
                                       bcm_cos_queue_t cosq, int burst);

extern int
bcm_ap_cosq_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value);

extern int
bcm_ap_cosq_stat_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, int sync_mode, uint64 *value);

extern int
bcm_ap_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int *burst);

extern int
bcm_ap_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg);

extern int
bcm_ap_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int *arg);

extern int
bcm_ap_cosq_service_pool_set(int unit, bcm_service_pool_id_t id, 
                                      bcm_cosq_service_pool_t cosq_service_pool);

extern int
bcm_ap_cosq_service_pool_get(int unit, bcm_service_pool_id_t id, 
                                      bcm_cosq_service_pool_t *cosq_service_pool);

extern int
bcm_ap_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst_min, 
                                      uint32 kbits_burst_max);

extern int
bcm_ap_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 *kbits_burst_min,
                                       uint32 *kbits_burst_max);
extern int
bcm_ap_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags);

extern int
bcm_ap_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags);

extern int bcm_ap_cosq_congestion_queue_set(int unit, bcm_port_t port,
                                            bcm_cos_queue_t cosq, int index);
extern int bcm_ap_cosq_congestion_queue_get(int unit, bcm_port_t port,
                                            bcm_cos_queue_t cosq, int *index);
extern int bcm_ap_cosq_congestion_quantize_set(int unit, bcm_port_t port,
                                               bcm_cos_queue_t cosq,
                                               int weight_code, int setpoint);
extern int bcm_ap_cosq_congestion_quantize_get(int unit, bcm_port_t port,
                                               bcm_cos_queue_t cosq,
                                               int *weight_code,
                                               int *setpoint);
extern int bcm_ap_cosq_congestion_sample_int_set(int unit, bcm_port_t port,
                                                 bcm_cos_queue_t cosq,
                                                 int min, int max);
extern int bcm_ap_cosq_congestion_sample_int_get(int unit, bcm_port_t port,
                                                 bcm_cos_queue_t cosq,
                                                 int *min, int *max);
extern int bcm_ap_cosq_drop_status_enable_set(int unit, bcm_port_t port,
                                              int enable);
extern int _bcm_ap_get_def_qbase(int unit, bcm_port_t inport, int qtype, 
                            int *pbase, int *pnumq);

extern int bcm_ap_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);
extern int bcm_ap_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int *mode,
                                       int *weight);
extern int
_bcm_ap_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                          bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                          int *qnum);

extern int
bcm_ap_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t gport, bcm_cos_queue_t cosq);

extern int 
bcm_ap_cosq_gport_congestion_config_set(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config);
extern int
bcm_ap_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t *gport, bcm_cos_queue_t *cosq);
extern int 
bcm_ap_cosq_gport_congestion_config_get(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config);

extern int 
bcm_ap_cosq_bst_profile_set(int unit, 
                             bcm_gport_t port, 
                             bcm_cos_queue_t cosq, 
                             bcm_bst_stat_id_t bid,
                             bcm_cosq_bst_profile_t *profile);

extern int 
bcm_ap_cosq_bst_profile_get(int unit, 
                             bcm_gport_t port, 
                             bcm_cos_queue_t cosq, 
                             bcm_bst_stat_id_t bid,
                             bcm_cosq_bst_profile_t *profile);

extern int 
bcm_ap_cosq_bst_stat_get(int unit, 
                              bcm_gport_t port, 
                              bcm_cos_queue_t cosq, 
                              bcm_bst_stat_id_t bid,
                              uint32 options,
                              uint64 *pvalue);

extern int 
bcm_ap_cosq_bst_stat_multi_get(int unit,
                                bcm_gport_t port,
                                bcm_cos_queue_t cosq,
                                uint32 options,
                                int max_values,
                                bcm_bst_stat_id_t *id_list,
                                uint64 *pvalues);
extern int 
bcm_ap_cosq_bst_stat_clear(int unit, 
                            bcm_gport_t port, 
                            bcm_cos_queue_t cosq, 
                            bcm_bst_stat_id_t bid);

extern int 
bcm_ap_cosq_bst_stat_sync(int unit, bcm_bst_stat_id_t bid);

extern int bcm_ap_cosq_endpoint_create(
        int unit, 
        bcm_cosq_classifier_t *classifier, 
        int *classifier_id);
extern int bcm_ap_cosq_endpoint_destroy(
        int unit, 
        int classifier_id);
extern int bcm_ap_cosq_endpoint_get(
        int unit, 
        int classifier_id,
        bcm_cosq_classifier_t *classifier);
extern int bcm_ap_cosq_endpoint_map_set(
        int unit, 
        bcm_port_t port, 
        int classifier_id, 
        bcm_gport_t queue_group, 
        int array_count, 
        bcm_cos_t *priority_array, 
        bcm_cos_queue_t *cosq_array);
extern int bcm_ap_cosq_endpoint_map_get(
        int unit, 
        bcm_port_t port, 
        int classifier_id, 
        bcm_gport_t *queue_group, 
        int array_max, 
        bcm_cos_t *priority_array, 
        bcm_cos_queue_t *cosq_array, 
        int *array_count);
extern int bcm_ap_cosq_endpoint_map_clear(
        int unit, 
        bcm_gport_t port, 
        int classifier_id);

extern int
bcm_ap_cosq_field_classifier_get(
    int unit, int classifier_id, bcm_cosq_classifier_t *classifier);

extern int
bcm_ap_cosq_field_classifier_id_create(
    int unit, bcm_cosq_classifier_t *classifier, int *classifier_id);

extern int
bcm_ap_cosq_field_classifier_id_destroy(
    int unit, int classifier_id);

extern int
bcm_ap_cosq_field_classifier_map_set(
    int unit, int classifier_id, int count,
    bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);

extern int
bcm_ap_cosq_field_classifier_map_get(
    int unit, int classifier_id,
    int array_max, bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array, int *array_count);

extern int
bcm_ap_cosq_field_classifier_map_clear(
    int unit, int classifier_id);

extern int bcm_ap_cosq_sync(int unit);
extern int bcm_ap_cosq_reinit(int unit);

extern void bcm_ap_cosq_sw_dump(int unit);

extern int bcm_ap_cosq_cpu_cosq_enable_set(
        int unit, 
        bcm_cos_queue_t cosq, 
        int enable);

extern int bcm_ap_cosq_cpu_cosq_enable_get(
        int unit, 
        bcm_cos_queue_t cosq, 
        int *enable);

typedef enum {
    _BCM_AP_COSQ_INDEX_STYLE_BUCKET,
    _BCM_AP_COSQ_INDEX_STYLE_QCN,
    _BCM_AP_COSQ_INDEX_STYLE_WRED,
    _BCM_AP_COSQ_INDEX_STYLE_WRED_PORT,
    _BCM_AP_COSQ_INDEX_STYLE_WRED_POOL,
    _BCM_AP_COSQ_INDEX_STYLE_SCHEDULER,
    _BCM_AP_COSQ_INDEX_STYLE_PERQ_XMT,
    _BCM_AP_COSQ_INDEX_STYLE_UCAST_DROP,
    _BCM_AP_COSQ_INDEX_STYLE_COS,
    _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_AP_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
    _BCM_AP_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
    _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
    _BCM_AP_COSQ_INDEX_STYLE_UC_EGR_POOL,
    _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP,
    _BCM_AP_COSQ_INDEX_STYLE_COUNT
} _bcm_ap_cosq_index_style_t;

extern int
_bcm_ap_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq, 
                           _bcm_ap_cosq_index_style_t style,                 
                           bcm_port_t *local_port, int *index, int *count);

/* Flexible hashing support routines */
extern int bcm_ap_switch_hash_entry_create( 
                                int unit, 
                                bcm_field_group_t group, 
                                bcm_hash_entry_t *entry_id);
extern int bcm_ap_switch_hash_entry_create_qset( 
                                int unit, 
                                bcm_field_qset_t qset, 
                                bcm_hash_entry_t *entry_id);
extern int bcm_ap_switch_hash_qualify_data (
                                int unit, 
                                bcm_hash_entry_t entry_id,
                                int qual_id, 
                                uint32 data, 
                                uint32 mask);
extern int bcm_ap_switch_hash_entry_remove (
                                int unit, 
                                bcm_hash_entry_t entry_id);
extern int bcm_ap_switch_hash_entry_reinstall (
                                int unit, 
                                bcm_hash_entry_t entry_id, 
                                uint32 offset, 
                                uint32 mask);
extern int bcm_ap_switch_hash_entry_install (
                                int unit, 
                                bcm_hash_entry_t entry_id, 
                                uint32 offset, 
                                uint32 mask);
extern int bcm_ap_switch_hash_entry_destroy (
                                int unit, 
                                bcm_hash_entry_t entry_id);
extern int bcm_ap_switch_hash_entry_detach(int unit);
extern int bcm_ap_switch_hash_entry_init (int unit);

extern int bcm_ap_ing_vp_vlan_membership_add(int unit, 
                      int vp, bcm_vlan_t vlan, uint32 flags);
extern int bcm_ap_egr_vp_vlan_membership_add(int unit, 
                             int vp, bcm_vlan_t vlan, uint32 flags);
extern int bcm_ap_ing_vp_vlan_membership_delete(int unit,
                             int vp, bcm_vlan_t vlan);
extern int bcm_ap_egr_vp_vlan_membership_delete(int unit,
                             int vp, bcm_vlan_t vlan);
extern int bcm_ap_ing_vp_vlan_membership_get(int unit, 
              int vp, bcm_vlan_t vlan, uint32 *flags);
extern int bcm_ap_egr_vp_vlan_membership_get(int unit, 
              int vp, bcm_vlan_t vlan, uint32 *flags);
extern int bcm_ap_ing_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap, int arr_size, int *flags_arr);
extern int bcm_ap_egr_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap, int arr_size, int *flags_arr);
extern int bcm_ap_ing_vp_vlan_membership_delete_all(int unit, bcm_vlan_t vlan);
extern int bcm_ap_egr_vp_vlan_membership_delete_all(int unit, bcm_vlan_t vlan);

/* Higig resilient hashing */
extern int bcm_ap_hg_rh_init(int unit);
extern void bcm_ap_hg_rh_deinit(int unit);
extern int bcm_ap_hg_rh_dynamic_size_encode(int dynamic_size,
        int *encoded_value);
extern int bcm_ap_hg_rh_set(int unit, int hgtid,
        _esw_trunk_add_info_t *add_info);
extern int bcm_ap_hg_rh_add(int unit, int hgtid,
        _esw_trunk_add_info_t *add_info, bcm_trunk_member_t *new_member);
extern int bcm_ap_hg_rh_delete(int unit, int hgtid,
        _esw_trunk_add_info_t *add_info, bcm_trunk_member_t *leaving_member);
extern int bcm_ap_hg_rh_free_resource(int unit, int hgtid);
extern int bcm_ap_hg_rh_status_get(int unit, int *rh_status);
extern int bcm_ap_hg_rh_dynamic_size_set(int unit, int hgtid, int dynamic_size);
extern int bcm_ap_hg_rh_ethertype_set(int unit, uint32 flags,
        int ethertype_count, int *ethertype_array);
extern int bcm_ap_hg_rh_ethertype_get(int unit, uint32 *flags,
        int ethertype_max, int *ethertype_array, int *ethertype_count);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_ap_hg_rh_recover(int unit, int hgtid,
        trunk_private_t *trunk_info);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_ap_hg_rh_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern int bcm_ap_trunk_init(int unit);

/* LAG resilient hashing */
extern int bcm_ap_lag_rh_init(int unit);
extern void bcm_ap_lag_rh_deinit(int unit);
extern int bcm_ap_lag_rh_dynamic_size_encode(int dynamic_size,
        int *encoded_value);
extern int bcm_ap_lag_rh_set(int unit, int tid,
        _esw_trunk_add_info_t *add_info, int num_rh_ports, int *mod_array,
        int *port_array);
extern int bcm_ap_lag_rh_add(int unit, int tid,
        _esw_trunk_add_info_t *add_info, int num_rh_ports, int *mod_array,
        int *port_array, bcm_trunk_member_t *new_member);
extern int bcm_ap_lag_rh_delete(int unit, int tid,
        _esw_trunk_add_info_t *add_info, int num_rh_ports, int *mod_array,
        int *port_array, bcm_trunk_member_t *leaving_member);
extern int bcm_ap_lag_rh_free_resource(int unit, int tid);
extern int bcm_ap_lag_rh_dynamic_size_set(int unit, int tid, int dynamic_size);
extern int bcm_ap_lag_rh_ethertype_set(int unit, uint32 flags,
        int ethertype_count, int *ethertype_array);
extern int bcm_ap_lag_rh_ethertype_get(int unit, uint32 *flags,
        int ethertype_max, int *ethertype_array, int *ethertype_count);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_ap_lag_rh_recover(int unit, int tid,
        trunk_private_t *trunk_info);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_ap_lag_rh_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/* FCOE */
extern int bcm_ap_fcoe_init(int unit);
extern int bcm_ap_fcoe_cleanup(int unit);
extern int bcm_ap_fcoe_zone_add(int unit, bcm_fcoe_zone_entry_t *zone);
extern int bcm_ap_fcoe_zone_delete(int unit,
                                    bcm_fcoe_zone_entry_t *zone);
extern int bcm_ap_fcoe_zone_delete_all (int unit);
extern int bcm_ap_fcoe_zone_delete_by_vsan (int unit,
                                             bcm_fcoe_zone_entry_t *zone);
extern int bcm_ap_fcoe_zone_delete_by_sid (int unit,
                                            bcm_fcoe_zone_entry_t *zone);
extern int bcm_ap_fcoe_zone_delete_by_did (int unit,
                                            bcm_fcoe_zone_entry_t *zone);
extern int bcm_ap_fcoe_zone_get (int unit, bcm_fcoe_zone_entry_t *zone);
extern int bcm_ap_fcoe_route_add(int unit, bcm_fcoe_route_t *route);
extern int bcm_ap_fcoe_route_delete(int unit, bcm_fcoe_route_t *route);
extern int bcm_ap_fcoe_route_delete_by_prefix(int unit,
                                               bcm_fcoe_route_t *route);
extern int bcm_ap_fcoe_route_delete_by_interface(int unit,
                                                  bcm_fcoe_route_t *route);
extern int bcm_ap_fcoe_route_delete_all(int unit);
extern int bcm_ap_fcoe_route_find(int unit, bcm_fcoe_route_t *route);
extern int bcm_ap_fcoe_vsan_create(int unit, uint32 options, 
                                    bcm_fcoe_vsan_t *vsan, 
                                    bcm_fcoe_vsan_id_t *vsan_id);
extern int bcm_ap_fcoe_vsan_destroy(int unit,  bcm_fcoe_vsan_id_t vsan_id);
extern int bcm_ap_fcoe_vsan_destroy_all(int unit);
extern int bcm_ap_fcoe_control_vsan_get(int unit,  
                                         uint32 vsan_id, 
                                         bcm_fcoe_vsan_control_t type, 
                                         int *arg);
extern int bcm_ap_fcoe_control_vsan_set(int unit, 
                                         uint32 vsan_id, 
                                         bcm_fcoe_vsan_control_t type, 
                                         int arg);
extern int bcm_ap_fcoe_vsan_get(int unit, uint32 vsan_id, 
                                 bcm_fcoe_vsan_t *vsan);
extern int bcm_ap_fcoe_vsan_set(int unit, uint32 vsan_id, 
                                 bcm_fcoe_vsan_t *vsan);
extern int bcm_ap_fcoe_vsan_stat_attach(int                 unit,
                                         bcm_fcoe_vsan_id_t  vsan,
                                         uint32              stat_counter_id);
extern int bcm_ap_fcoe_vsan_stat_detach(int unit, bcm_fcoe_vsan_id_t  vsan);
extern int bcm_ap_fcoe_vsan_stat_counter_get(
                                      int                  unit, 
                                      int                  sync_mode, 
                                      bcm_fcoe_vsan_id_t   vsan,
                                      bcm_fcoe_vsan_stat_t stat, 
                                      uint32               num_entries, 
                                      uint32               *counter_indexes, 
                                      bcm_stat_value_t     *counter_values);
extern int bcm_ap_fcoe_vsan_stat_counter_set(
                                      int                  unit, 
                                      bcm_fcoe_vsan_id_t   vsan,
                                      bcm_fcoe_vsan_stat_t stat, 
                                      uint32               num_entries, 
                                      uint32               *counter_indexes, 
                                      bcm_stat_value_t     *counter_values);
extern int bcm_ap_fcoe_vsan_stat_id_get(int                  unit,
                                         bcm_fcoe_vsan_id_t   vsan,
                                         bcm_fcoe_vsan_stat_t stat, 
                                         uint32               *stat_counter_id);
extern int bcm_ap_fcoe_route_stat_attach(int                 unit,
                                          bcm_fcoe_route_t    *route,
                                          uint32              stat_counter_id);
extern int bcm_ap_fcoe_route_stat_detach(int unit, bcm_fcoe_route_t *route);
extern int bcm_ap_fcoe_route_stat_counter_get(
                                       int                   unit, 
                                       int                   sync_mode, 
                                       bcm_fcoe_route_t      *route,
                                       bcm_fcoe_route_stat_t stat, 
                                       uint32                num_entries, 
                                       uint32                *counter_indexes, 
                                       bcm_stat_value_t      *counter_values);
extern int bcm_ap_fcoe_route_stat_counter_set (
                                        int                   unit, 
                                        bcm_fcoe_route_t      *route,
                                        bcm_fcoe_route_stat_t stat, 
                                        uint32                num_entries, 
                                        uint32                *counter_indexes, 
                                        bcm_stat_value_t      *counter_values);
extern int bcm_ap_fcoe_route_stat_id_get(
                                        int                   unit,
                                        bcm_fcoe_route_t      *route,
                                        bcm_fcoe_route_stat_t stat, 
                                        uint32                *stat_counter_id);
int bcm_ap_fcoe_intf_config_set(int unit, uint32 flags, bcm_if_t intf, 
                                 bcm_fcoe_intf_config_t *cfg);

int bcm_ap_fcoe_intf_config_get(int unit, uint32 flags, bcm_if_t intf, 
                                 bcm_fcoe_intf_config_t *cfg);
extern int bcm_ap_fcoe_zone_traverse(int unit,
                                      uint32 flags,
                                      bcm_fcoe_zone_traverse_cb trav_fn,
                                      void *user_data);
extern int bcm_ap_fcoe_vsan_traverse(int unit,
                                      uint32 flags,
                                      bcm_fcoe_vsan_traverse_cb trav_fn,
                                      void *user_data);
extern int bcm_ap_fcoe_route_traverse(int unit,
                                       uint32 flags,
                                       bcm_fcoe_route_traverse_cb trav_fn,
                                       void *user_data);

/* QOS */
#define _BCM_QOS_MAP_TYPE_ING_VFT_PRI_MAP    6
#define _BCM_QOS_MAP_TYPE_EGR_VFT_PRI_MAP    7
#define _BCM_QOS_MAP_TYPE_EGR_VSAN_PRI_MAP   8
#define _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP   9
#define _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP   10
#define _BCM_QOS_VFT_MAP_SHIFT               11

#define QOS_MAP_IS_FCOE(map_id)                                                \
  (  ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_ING_VFT_PRI_MAP) \
  || ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_VFT_PRI_MAP) \
  || ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_VSAN_PRI_MAP)\
  )

#define QOS_MAP_IS_L2_VLAN_ETAG(map_id)                                        \
  (  ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP) \
  || ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP) \
  )

#define QOS_FLAGS_ARE_FCOE(flags) \
  ( (flags & BCM_QOS_MAP_VFT) || (flags & BCM_QOS_MAP_VSAN) )

extern int bcm_ap_qos_init(int unit);
extern int bcm_ap_qos_detach(int unit);
extern int bcm_ap_qos_map_create(int unit, uint32 flags, int *map_id);
extern int bcm_ap_qos_map_destroy(int unit, int map_id);
extern int bcm_ap_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map,
                               int map_id);
extern int bcm_ap_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map,
                                  int map_id);
extern int bcm_ap_qos_port_map_set(int unit, bcm_gport_t port, int ing_map,
                                    int egr_map);
extern int bcm_ap_qos_map_multi_get(int unit, uint32 flags,
                                      int map_id, int array_size, 
                                      bcm_qos_map_t *array, int *array_count);
extern int _bcm_ap_egr_port_set(int unit, bcm_port_t port, 
                                 soc_field_t field, int value);
extern int bcm_ap_fcoe_fc_header_type_set(int unit, uint32 flags, uint8 r_ctl, 
                                           bcm_fcoe_fc_header_type_t hdr_type);
extern int bcm_ap_fcoe_fc_header_type_get(int unit, uint32 flags, uint8 r_ctl, 
                                           bcm_fcoe_fc_header_type_t *hdr_type);

extern int bcm_ap_fcoe_vsan_translate_action_add(
                                    int unit, 
                                    bcm_fcoe_vsan_translate_key_config_t *key, 
                                    bcm_fcoe_vsan_action_set_t *action);
extern int bcm_ap_fcoe_vsan_translate_action_delete(
                                    int unit, 
                                    bcm_fcoe_vsan_translate_key_config_t *key);
extern uint32 _bcm_ap_fcoe_ing_vft_pri_action_encode(
                                    bcm_fcoe_vsan_action_t action);
extern uint32 _bcm_ap_fcoe_egr_vft_pri_action_encode(
                                    bcm_fcoe_vsan_action_t action);
extern uint32 _bcm_ap_fcoe_egr_vft_tag_action_encode(
                                    bcm_fcoe_vsan_action_t action);
extern int bcm_ap_port_dscp_map_set(int unit, bcm_port_t port, int srccp,
                            int mapcp, int prio, int cng);

/* L2 */
extern int _bcm_ap_l2_hit_retrieve(int unit, l2x_entry_t *l2x_entry,
                                 int l2_hw_index);

extern int
_bcm_ap_port_enqueue_set(int unit, bcm_port_t gport,
                                    int enable);

extern int
_bcm_ap_cosq_port_queue_state_check(int unit, bcm_port_t gport);

extern int
_bcm_ap_port_enqueue_get(int unit, bcm_port_t gport,
                                            int *enable);
extern int
_bcm_ap_cosq_safc_class_mapping_set(int unit,
                    bcm_gport_t port,
                    int array_count,
                    bcm_cosq_safc_class_mapping_t *mapping_array);
extern  int
_bcm_ap_cosq_safc_class_mapping_get(int unit, bcm_gport_t port,
                                     int array_max,
                                     bcm_cosq_safc_class_mapping_t *mapping_array,
                                     int *array_count);

extern int
_bcm_ap_cosq_pfc_class_mapping_set(int unit,
                    bcm_gport_t port,
                    int array_count,
                    bcm_cosq_pfc_class_mapping_t *mapping_array);
extern int 
_bcm_ap_cosq_pfc_class_mapping_get(int unit,
      bcm_gport_t port,
      int array_max,
      int *array_count,
      bcm_cosq_pfc_class_mapping_t *mapping_array);

extern int
bcm_ap_cosq_default_llfc_profile_attach(int unit, bcm_gport_t port);

extern int
bcm_ap_cosq_llfc_profile_detach(int unit, bcm_gport_t port);

/* Subport functions */
extern int bcm_apache_subport_coe_init(int unit);

/* Service Queuing functions */
extern int
bcm_ap_cosq_service_classifier_id_create(int unit,
    bcm_cosq_classifier_t *classifier, int *classifier_id);

extern int
bcm_ap_cosq_service_classifier_id_destroy(int unit,
    int classifier_id);

extern int
bcm_ap_cosq_service_classifier_get(int unit,
    int classifier_id, bcm_cosq_classifier_t *classifier);

extern int
bcm_ap_cosq_service_map_set(int unit,
    bcm_port_t port, int classifier_id, bcm_gport_t queue_group,
    int array_count, bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);

extern int
bcm_ap_cosq_service_map_get(int unit,
    bcm_port_t port, int classifier_id, bcm_gport_t *queue_group,
    int array_max, bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array,
    int *array_count);

extern int
bcm_ap_cosq_service_map_clear( int unit,
    bcm_gport_t port, int classifier_id);
extern int bcm_ap_cosq_subport_flow_control_set(int unit, bcm_gport_t subport,
                                                   bcm_gport_t sched_port);
extern int bcm_ap_cosq_subport_flow_control_get(int unit, bcm_gport_t subport,
                                                  bcm_gport_t * sched_port);
extern int
_bcm_ap_cosq_localport_resolve(int unit, bcm_gport_t gport,
                               bcm_port_t *local_port);
extern int
_bcm_ap_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq, 
                           _bcm_ap_cosq_index_style_t style,      
                           bcm_port_t *local_port, int *index, int *count);

/* OAM functions */
extern int
_bcm_apache_oam_drop_control_fp_encode(bcm_field_oam_drop_mep_type_t mep_type,
                                       uint8 *data,
                                       uint8 *mask);

/* OOB FC Tx side global configuration API */
extern int bcm_ap_oob_fc_tx_config_set(
    int unit,
    bcm_oob_fc_tx_config_t *config);

/* OOB FC Tx side global configuration get API */
extern int bcm_ap_oob_fc_tx_config_get(
    int unit,
    bcm_oob_fc_tx_config_t *config);

extern int
_bcm_ap_oob_fc_tx_port_control_set(int unit, bcm_port_t port,
                                   int status, int dir);
extern int
_bcm_ap_oob_fc_tx_port_control_get(int unit, bcm_port_t port,
                                   int *status, int dir);

/* OOB FC Tx side global info get API */
extern int bcm_ap_oob_fc_tx_info_get(
    int unit,
    bcm_oob_fc_tx_info_t *info);

/*
 * OOB FC Rx Interface side Traffic Class to Priority Mapping Multi Set
 * API
 */
extern int bcm_ap_oob_fc_rx_port_tc_mapping_multi_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    int array_count,
    uint32 *tc,
    uint32 *pri_bmp);

/*
 * OOB FC Rx Interface side Traffic Class to Priority Mapping Multi Get
 * API
 */
extern int bcm_ap_oob_fc_rx_port_tc_mapping_multi_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    int array_max,
    uint32 *tc,
    uint32 *pri_bmp,
    int *array_count);

/* OOB FC Rx Interface side Traffic Class to Priority Mapping set API */
extern int bcm_ap_oob_fc_rx_port_tc_mapping_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 tc,
    uint32 pri_bmp);

/* OOB FC Rx Interface side Traffic Class to Priority Mapping get API */
extern int bcm_ap_oob_fc_rx_port_tc_mapping_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 tc,
    uint32 *pri_bmp);

/* OOB FC Rx Interface side Configuration set API */
extern int bcm_ap_oob_fc_rx_config_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_oob_fc_rx_config_t *config,
    int array_count,
    bcm_gport_t *gport_array);

/* OOB FC Rx Interface side Configuration get API */
extern int bcm_ap_oob_fc_rx_config_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_oob_fc_rx_config_t *config,
    int array_max,
    bcm_gport_t *gport_array,
    int *array_count);

/* OOB FC Rx Interface side Channel offset value for a port */
extern int bcm_ap_oob_fc_rx_port_offset_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 *offset);

extern int
bcmi_apache_port_fn_drv_init(int unit);

extern int
_bcm_apache_port_mac_get(int unit, bcm_port_t port, int oui, uint32* mac);

extern int
_bcm_apache_port_mac_set(int unit, bcm_port_t port, int oui, uint32 mac);

/* tag/pri actions for ING_VLAN_TAG_ACTION_PROFILE table */
#define _BCM_AP_ING_TAG_ACTION_NONE              0 /* do not modify */
#define _BCM_AP_ING_TAG_CFI_ACTION_NONE          0 /* do not modify */
#define _BCM_AP_ING_TAG_PRI_ACTION_NONE          0 /* do not modify */
#define _BCM_AP_EGR_TAG_ACTION_NONE              0 /* do not modify */
#define _BCM_AP_EGR_TAG_CFI_ACTION_NONE          0 /* do not modify */
#define _BCM_AP_EGR_TAG_PRI_ACTION_NONE          0 /* do not modify */

/* actions for ING VFT PRI (ING_VLAN_TAG_ACTION_PROFILE) */
#define _BCM_AP_ING_VFT_PRI_ACTION_NONE          0
#define _BCM_AP_ING_VFT_PRI_ACTION_USE_INNER_PRI 1
#define _BCM_AP_ING_VFT_PRI_ACTION_USE_OUTER_PRI 2
#define _BCM_AP_ING_VFT_PRI_ACTION_REPLACE       3  /* use VLAN_XLATE.XLATE::
                                                        FCOE_VSAN_PRI */

/* actions for EGR VFT PRI (EGR_VLAN_TAG_ACTION_PROFILE) */
#define _BCM_AP_EGR_VFT_PRI_ACTION_NONE              0
#define _BCM_AP_EGR_VFT_PRI_ACTION_USE_INTERNAL_IPRI 1
#define _BCM_AP_EGR_VFT_PRI_ACTION_USE_INTERNAL_OPRI 2
#define _BCM_AP_EGR_VFT_PRI_ACTION_REPLACE           3  /* use EVXLT.XLATE::
                                                            FCOE_VSAN_PRI */

/* actions for EGR VFT Tag (EGR_VLAN_TAG_ACTION_PROFILE) */
#define _BCM_AP_EGR_VFT_TAG_ACTION_NONE              0
#define _BCM_AP_EGR_VFT_TAG_ACTION_USE_INTERNAL_IVID 1
#define _BCM_AP_EGR_VFT_TAG_ACTION_USE_INTERNAL_OVID 2
#define _BCM_AP_EGR_VFT_TAG_ACTION_DELETE            3
#define _BCM_AP_EGR_VFT_TAG_ACTION_USE_INTERNAL_VSAN 4
#define _BCM_AP_EGR_VFT_TAG_ACTION_REPLACE           5  /* use EVXLT.XLATE::
                                                            FCOE_VSAN_ID */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_ap_fcoe_sync(int unit);
extern int _bcm_ap_fcoe_reinit(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int
bcm_apache_dump_port_lls_sw( int unit, int port);

extern int
_soc_apache_s1_scheduler_index(int unit, int port, int level, int hw_index);

/*
 * Port Functions
 */
extern int
bcmi_apache_port_fn_drv_init(int unit);
extern int
bcmi_apache_port_lanes_get(int unit, bcm_port_t port, int *lanes);
extern int
bcmi_apache_port_lanes_set(int unit, bcm_port_t port, int lanes);


/*
 * OBM Classifier functions.
 */

extern int
bcm_apache_switch_obm_classifier_mapping_multi_set(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_count,
    bcm_switch_obm_classifier_t *switch_obm_classifier);

extern int
bcm_apache_switch_obm_classifier_mapping_multi_get(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_max,
    bcm_switch_obm_classifier_t *switch_obm_classifier,
    int *array_count);

extern int
bcm_apache_obm_classifier_port_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value);

extern int
bcm_apache_obm_classifier_port_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *value);

#endif /* BCM_APACHE_SUPPORT  */

#endif /* !_BCM_INT_APACHE_H_ */
