/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        trident2.h
 * Purpose:     Function declarations for Trident2 Internal functions.
 */

#ifndef _BCM_INT_TRIDENT2_H_
#define _BCM_INT_TRIDENT2_H_

#if defined(BCM_TRIDENT2_SUPPORT)

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/l3.h>

#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/flex_ctr.h>
#if defined(INCLUDE_L3)
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/port.h>
#include <bcm/extender.h>
#endif /* INCLUDE_L3 */
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT)
extern int bcm_td2_l2_init(int unit);

extern int bcm_td2_port_rate_ingress_set(int unit, bcm_port_t port,
                                         uint32 bandwidth, uint32 burst);
extern int bcm_td2_port_rate_ingress_get(int unit, bcm_port_t port,
                                         uint32 *bandwidth, uint32 *burst);
extern int bcm_td2_port_rate_pause_get(int unit, bcm_port_t port,
                                       uint32 *pause, uint32 *resume);
extern int bcm_td2_port_rate_egress_set(int unit, bcm_port_t port,
                                        uint32 bandwidth, uint32 burst,
                                        uint32 mode);
extern int bcm_td2_port_rate_egress_get(int unit, bcm_port_t port,
                                        uint32 *bandwidth, uint32 *burst,
                                        uint32 *mode);
extern int bcm_td2_port_drain_cells(int unit, int port);
extern int _bcm_td2_port_drain_cells(int unit, int port);
extern int _bcm_td2_nh_nat_id_update_by_l3_intf(int unit,
                                                bcm_if_t intf, int nat_id);
extern int _bcm_td2_nh_nat_id_update_by_l3_iif(int unit,
                                               bcm_if_t l3_iif, int nat_id);
extern int _bcm_td2_nh_nat_id_update_by_vlan(int unit,
                                             bcm_vlan_t vlan, int nat_id);
#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_td2_lookup_slice_clear(int unit, unsigned slice_num);
extern int _bcm_field_td2_stage_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_td2_init(int unit, _field_control_t *fc);
extern int _bcm_field_td2_qual_tcam_key_mask_set(int unit,
                                                 _field_entry_t *f_ent,
                                                 unsigned validf);
extern int _bcm_field_td2_egress_selcode_set(int unit,
                                             _field_group_t *fg,
                                             uint32 efp_slice_mode, 
                                             uint32 efp_key_mode,
                                             uint32 key_match_type, 
                                             int *parts_count);
extern int _bcm_field_td2_group_construct_quals_add(int unit,
                                                    _field_control_t *fc,
                                                    _field_stage_t   *stage_fc,
                                                    uint8 entry_type,
                                                    _field_group_t *fg,
                                                    int8 pri_key1,
                                                    int8 pri_key2
                                                   );
extern int _bcm_field_td2_qual_tcam_key_mask_get(int unit,
                                                _field_entry_t *f_ent,
                                                _field_tcam_t *tcam);
extern int _bcm_field_td2_qualify_PacketRes(int               unit,
                                            bcm_field_entry_t entry,
                                            uint32            *data,
                                            uint32            *mask
                                            );
extern int _bcm_field_td2_qualify_PacketRes_get(int               unit,
                                                bcm_field_entry_t entry,
                                                uint32            *data,
                                                uint32            *mask
                                                );
extern int
_bcm_field_td2_qualify_LoopbackType(int                      unit,
                                    bcm_field_LoopbackType_t loopback_type,
                                    uint32                   *tcam_data,
                                    uint32                   *tcam_mask
                                    );
extern int
_bcm_field_td2_qualify_LoopbackType_get(uint8                    tcam_data,
                                        uint8                    tcam_mask,
                                        bcm_field_LoopbackType_t *loopback_type
                                        );
extern int 
_bcm_field_td2_qualify_TunnelType(bcm_field_TunnelType_t tunnel_type,
                                  uint32                 *tcam_data,
                                  uint32                 *tcam_mask
                                  );
extern int 
_bcm_field_td2_qualify_TunnelType_get(uint8                  tcam_data,
                                      uint8                  tcam_mask,
                                      bcm_field_TunnelType_t *tunnel_type
                                      );
extern int _bcm_field_td2_qualify_class(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_qualify_t qual,
                                        uint32 *data,
                                        uint32 *mask
                                        );
extern int _bcm_field_td2_qualify_class_get(int unit,
                                            bcm_field_entry_t entry,
                                            bcm_field_qualify_t qual,
                                            uint32 *data,
                                            uint32 *mask
                                            );
extern int _bcm_field_td2_action_get(int             unit,
                                     soc_mem_t       mem,
                                     _field_entry_t  *f_ent,
                                     int             tcam_idx,
                                     _field_action_t *fa,
                                     uint32          *buf
                                     );

extern int _bcm_field_td2_action_conflict_check(int                unit,
                                                _field_entry_t     *f_ent,
                                                bcm_field_action_t action1,
                                                bcm_field_action_t action
                                                );

extern int 
_bcm_field_td2_hash_select_profile_ref_count_get(int unit,
                                                 soc_mem_t profile_mem, 
                                                 int index, 
                                                 int *ref_count);
extern int 
_bcm_field_td2_hash_select_profile_delete(int unit, 
                                         soc_mem_t profile_mem, 
                                         int index);
extern int 
_bcm_field_td2_hash_select_profile_alloc(int unit,
                                         _field_entry_t *f_ent,
                                         _field_action_t *fa);
extern int 
_bcm_field_td2_hash_select_profile_get(int unit, 
                                       soc_mem_t profile_mem,
                                       soc_profile_mem_t **hash_select_profile);
extern int
_bcm_field_td2_hash_select_profile_hw_free(int unit, 
                                           _field_entry_t *f_ent, 
                                           uint32 flags);
extern int
_bcm_field_td2_hash_select_profile_hw_alloc (int unit, _field_entry_t *f_ent);

extern int _bcm_field_td2_group_install(int unit, _field_group_t *fg);
extern int _bcm_field_td2_write_slice_map(int unit, _field_stage_t *stage_fc,
                                          _field_group_t *fg);

extern int _bcm_field_td2_slice_clear(int unit,
                                      _field_group_t *fg,
                                      _field_slice_t *fs);

extern int _bcm_field_td2_selcodes_install(int unit,
                                           _field_group_t *fg,
                                           uint8 slice_num,
                                           bcm_pbmp_t pbmp,
                                           int selcode_idx);

extern int _bcm_field_td2_entry_move(int unit,
                                     _field_entry_t *f_ent,
                                    int parts_count,
                                    int *tcam_idx_old,
                                    int *tcam_idx_new);

extern int _bcm_field_td2_action_support_check(int unit,
                                               _field_entry_t *f_ent,
                                               bcm_field_action_t action,
                                               int *result);

extern int _bcm_field_td2_action_params_check(int unit,
                                              _field_entry_t  *f_ent,
                                              _field_action_t *fa);

extern int _bcm_field_td2_stage_action_support_check(int unit,
                                                     unsigned stage,
                                                     bcm_field_action_t action,
                                                     int *result);
extern int
_bcm_field_td2_lookup_selcodes_install(int            unit,
                                       _field_group_t *fg,
                                       uint8          slice_num,
                                       int            selcode_idx
                                       );
extern int
_bcm_field_td2_lookup_mode_set(int            unit,
                               uint8          slice_num,
                               _field_group_t *fg,
                               uint8          flags
                               );
extern int
_bcm_field_td2_qualify_VlanTranslationHit(int               unit,
                                          bcm_field_entry_t entry,
                                          uint8             *data,
                                          uint8             *mask);
extern int
_bcm_field_td2_qualify_VlanTranslationHit_get(int               unit,
                                              bcm_field_entry_t entry,
                                              uint8             *data,
                                              uint8             *mask);

extern int 
_bcm_field_td2_qualify_ip_type(int unit, bcm_field_entry_t entry,
                               bcm_field_IpType_t type,
                               bcm_field_qualify_t qual);
extern int 
_bcm_field_td2_qualify_ip_type_get(int unit, bcm_field_entry_t entry,
                                   bcm_field_IpType_t *type,
                                   bcm_field_qualify_t qual);
extern int 
_bcm_field_td2_qualify_ip_type_encode_get(int unit, uint32 hw_data,
                                                     uint32 hw_mask,
                                                     bcm_field_IpType_t *type);

#endif /* BCM_FIELD_SUPPORT */

#define _BCM_TD2_L3_PORT_MASK        0x7f
#define _BCM_TD2_L3_MODID_MASK       0xff
#define _BCM_TD2_L3_MODID_SHIFT      0x7
#define _BCM_TD2_L3_DEFAULT_MTU_SIZE 0x3fff
#define _BCM_TD2_L3_MEM_BANKS_ALL     (-1)
#define _BCM_TD2_HOST_ENTRY_NOT_FOUND (-1)

#if defined(INCLUDE_L3)

#define BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(_u_, _intf_, _nhidx_)      \
             ((_nhidx_ == BCM_XGS3_L3_INVALID_INDEX)             && \
              (!BCM_XGS3_L3_EGRESS_IDX_VALID(_u_, _intf_))       && \
              (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(_u_, _intf_)) && \
              (!BCM_XGS3_DVP_EGRESS_IDX_VALID(_u_, _intf_))      && \
              (!BCM_XGS3_PROXY_EGRESS_IDX_VALID(_u_, _intf_))    && \
              (BCM_XGS3_L3_EGRESS_MODE_ISSET(_u_))               && \
              (soc_feature(unit, soc_feature_l3_extended_host_entry)))
            
#define BCM_TD2_L3_HOST_TABLE_MEM(_u_, _intf_, _v6_, _mem_, _nhidx_)                     \
            _mem_ = (_v6_) ? ((BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(_u_, _intf_, _nhidx_))?  \
                             L3_ENTRY_IPV6_MULTICASTm:L3_ENTRY_IPV6_UNICASTm) :          \
                           ((BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(_u_, _intf_, _nhidx_))?    \
                             L3_ENTRY_IPV4_MULTICASTm:L3_ENTRY_IPV4_UNICASTm)

#define BCM_TD2_L3_IS_EMBEDDED_NH_TABLE(_mem_, _v6_)   \
    (_mem_ == L3_ENTRY_IPV4_MULTICASTm && !_v6_) || \
    (_mem_ == L3_ENTRY_IPV4_MULTICASTm && _v6_)

#define  BCM_TD2_L3_HOST_TABLE_FLD(_u_, _mem_, _v6_, _fld_)                                   \
              _fld_ = (_bcm_l3_fields_t *) INT_TO_PTR((_v6_) ?                                \
                       ((_mem_ == L3_ENTRY_IPV6_UNICASTm) ?                                   \
                         BCM_XGS3_L3_MEM_FIELDS(_u_,v6): BCM_XGS3_L3_MEM_FIELDS(_u_, v6_4)) : \
                       ((_mem_ == L3_ENTRY_IPV4_UNICASTm) ?                                   \
                         BCM_XGS3_L3_MEM_FIELDS(_u_,v4): BCM_XGS3_L3_MEM_FIELDS(_u_, v4_2)))

#define BCM_TD2_L3_HOST_ENTRY_BUF(_v6_, _mem_, _buf_, _v4ent_, _v4extent_, _v6ent_, _v6extent_)  \
            _buf_ = ((_v6_) ?                                                        \
                  ((_mem_ == L3_ENTRY_IPV6_UNICASTm) ? (uint32 *) &_v6ent_ :         \
                   (uint32 *) &_v6extent_) :                                         \
                  ((_mem_ == L3_ENTRY_IPV4_UNICASTm) ? (uint32 *) &_v4ent_ :         \
                   (uint32 *) &_v4extent_)) 


#define BCM_TD3_L3_HOST_TABLE_MEM(_u_, _intf_, _v6_, _mem_, _nhidx_)                     \
            _mem_ = (_v6_) ? ((BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(_u_, _intf_, _nhidx_))?  \
                             BCM_XGS3_L3_MEM(unit, v6_4):BCM_XGS3_L3_MEM(unit, v6)) :          \
                           ((BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(_u_, _intf_, _nhidx_))?    \
                             BCM_XGS3_L3_MEM(unit, v4_2):BCM_XGS3_L3_MEM(unit, v4))

#define  BCM_TD3_L3_HOST_TABLE_FLD(_u_, _mem_, _v6_, _fld_)                                   \
              _fld_ = (_bcm_l3_fields_t *) INT_TO_PTR((_v6_) ?                                \
                       (((_mem_ == L3_ENTRY_ONLY_DOUBLEm) || (_mem_ == L3_ENTRY_DOUBLEm))?                                   \
                         BCM_XGS3_L3_MEM_FIELDS(_u_,v6): BCM_XGS3_L3_MEM_FIELDS(_u_, v6_4)) : \
                       (((_mem_ == L3_ENTRY_ONLY_SINGLEm) || (_mem_ == L3_ENTRY_SINGLEm))?                                   \
                         BCM_XGS3_L3_MEM_FIELDS(_u_,v4): BCM_XGS3_L3_MEM_FIELDS(_u_, v4_2)))

#define BCM_TD3_L3_HOST_ENTRY_BUF(_v6_, _mem_, _buf_, _v4ent_, _v4extent_, _v6ent_, _v6extent_)  \
            _buf_ = ((_v6_) ?                                                        \
                  (((_mem_ == L3_ENTRY_ONLY_DOUBLEm) || (_mem_ == L3_ENTRY_DOUBLEm))? (uint32 *) &_v6ent_ :         \
                   (uint32 *) &_v6extent_) :                                         \
                  (((_mem_ == L3_ENTRY_ONLY_SINGLEm) || (_mem_ == L3_ENTRY_SINGLEm))? (uint32 *) &_v4ent_ :         \
                   (uint32 *) &_v4extent_)) 

#define BCM_TH3_L3_HOST_TABLE_MEM(_u_, _intf_, _v6_, _ipmc, _mem_, _nhidx_)                     \
            _mem_ = (_v6_) ? ((ipmc)?  \
                             BCM_XGS3_L3_MEM(unit, v6_4):BCM_XGS3_L3_MEM(unit, v6)) :          \
                           ((ipmc)?    \
                             BCM_XGS3_L3_MEM(unit, v4_2):BCM_XGS3_L3_MEM(unit, v4))


#define  BCM_TH3_L3_HOST_TABLE_FLD(_u_, _mem_, _v6_, _fld_)                                   \
              _fld_ = (_bcm_l3_fields_t *) INT_TO_PTR((_v6_) ?                                \
                       (((_mem_ == L3_ENTRY_ONLY_DOUBLEm) || (_mem_ == L3_ENTRY_DOUBLEm))?                                   \
                         BCM_XGS3_L3_MEM_FIELDS(_u_,v6): BCM_XGS3_L3_MEM_FIELDS(_u_, v6_4)) : \
                       (((_mem_ == L3_ENTRY_ONLY_SINGLEm) || (_mem_ == L3_ENTRY_SINGLEm))?                                   \
                         BCM_XGS3_L3_MEM_FIELDS(_u_,v4): BCM_XGS3_L3_MEM_FIELDS(_u_, v4_2)))

#define BCM_TH3_L3_HOST_ENTRY_BUF(_v6_, _mem_, _buf_, _v4ent_, _v4extent_, _v6ent_, _v6extent_)  \
            _buf_ = ((_v6_) ?                                                        \
                  (((_mem_ == L3_ENTRY_ONLY_DOUBLEm) || (_mem_ == L3_ENTRY_DOUBLEm))? (uint32 *) &_v6ent_ :         \
                   (uint32 *) &_v6extent_) :                                         \
                  (((_mem_ == L3_ENTRY_ONLY_SINGLEm) || (_mem_ == L3_ENTRY_SINGLEm))? (uint32 *) &_v4ent_ :         \
                   (uint32 *) &_v4extent_))


/* Add support for Extended based on Flex Counter support type */
#define BCM_TD2_L3_ALPM_TABLE_MEM(_u_, _v6_, _mem_)             \
            _mem_ = (_v6_) ? L3_DEFIP_ALPM_IPV6_64m : L3_DEFIP_ALPM_IPV4m

/* Add suport for Extended */
#define BCM_TD2_L3_ALPM_ENTRY_BUF(_v6_, _mem_, _buf_, _v4ent_, _v6ent_)  \
            _buf_ = (_v6_) ? (uint32 *)&_v6ent_ : (uint32 *)&_v4ent_

/* L3 Internal Functions */
extern int _bcm_td2_l3_ent_init(int unit, soc_mem_t mem,
                                _bcm_l3_cfg_t *l3cfg, void *l3x_entry);
extern int _bcm_td2_l3_ipmc_ent_init(int unit, uint32 *buf_p,
                                     _bcm_l3_cfg_t *l3cfg);

/* Host table support */
extern int _bcm_td2_l3_add(int unit, _bcm_l3_cfg_t *l3cfg, int nh_idx);
extern int _bcm_td2_l3_get(int unit, _bcm_l3_cfg_t *l3cfg, int *nh_idx);
extern int _bcm_td2_l3_del(int unit, _bcm_l3_cfg_t *l3cfg);
extern int _bcm_td2_l3_del_match(int unit, int flags, void *pattern,
                       bcm_xgs3_ent_op_cb cmp_func,
                       bcm_l3_host_traverse_cb notify_cb, void *user_data);
extern int _bcm_td2_l3_traverse(int unit, int flags, uint32 start, uint32 end,
                                 bcm_l3_host_traverse_cb cb, void *user_data);

extern int _bcm_td2_l3_get_by_idx(int unit, void *dma_ptr, int idx,
                                   _bcm_l3_cfg_t *l3cfg, int *nh_idx);
extern int bcm_td2_l3_conflict_get(int unit, bcm_l3_key_t *ipkey, bcm_l3_key_t *cf_array,
                                   int cf_max, int *cf_count);
extern int bcm_td2_l3_replace(int unit, _bcm_l3_cfg_t *l3cfg);
extern int bcm_td2_l3_del_intf(int unit, _bcm_l3_cfg_t *l3cfg, int negate);
extern int
_bcm_td2_l3_ent_init(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg, void *l3x_entry);

#ifdef ALPM_ENABLE
/* ALPM */
extern int _bcm_td2_alpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                             int nh_ecmp_idx);
extern int _bcm_td2_alpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_td2_alpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int
                             *nh_ecmp_idx);
extern int _bcm_td2_alpm_find(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                              int *nh_ecmp_idx);
extern int _bcm_td2_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);

typedef int (*_bcm_td2_alpm_trace_dump_cb)(int u, void *user_data, char *log_data);

extern int _bcm_td2_alpm_trace_enabled(int u);
extern int _bcm_td2_alpm_trace_dump(int u, int showflags, _bcm_td2_alpm_trace_dump_cb dump_cb, void *user_data);
extern int _bcm_td2_alpm_trace_cnt(int u);
extern int _bcm_td2_alpm_trace_init(int u);
extern int _bcm_td2_alpm_trace_deinit(int u);
extern int _bcm_td2_alpm_trace_set(int u, int val);
extern int _bcm_td2_alpm_trace_clear(int u);
extern int _bcm_td2_alpm_trace_sanity(int u, int enable, int start, int end);
extern int _bcm_td2_alpm_trace_wrap_set(int u, int val);
extern int _bcm_td2_alpm_trace_log(int u, int op, _bcm_defip_cfg_t *cfg, int nh_idx, int rc);

#endif


/* L3 IPMC */
extern int _bcm_td2_l3_ipmc_get_by_idx(int unit, void *dma_ptr, int idx,
                                       _bcm_l3_cfg_t *l3cfg);
extern int _bcm_td2_l3_ipmc_get(int unit, _bcm_l3_cfg_t *l3cfg);
extern int _bcm_td2_l3_ipmc_add(int unit, _bcm_l3_cfg_t *l3cfg);
extern int _bcm_td2_l3_ipmc_del(int unit, _bcm_l3_cfg_t *l3cfg);
extern int bcm_td2_ipmc_pim_bidir_init(int unit);
extern int bcm_td2_ipmc_pim_bidir_detach(int unit);
extern int bcm_td2_ipmc_rp_create(int unit, uint32 flags, int *rp_id);
extern int bcm_td2_ipmc_rp_destroy(int unit, int rp_id);
extern int bcm_td2_ipmc_rp_set(int unit, int rp_id, int intf_count,
        bcm_if_t *intf_array);
extern int bcm_td2_ipmc_rp_get(int unit, int rp_id, int intf_max,
        bcm_if_t *intf_array, int *intf_count);
extern int bcm_td2_ipmc_rp_add(int unit, int rp_id, bcm_if_t intf_id);
extern int bcm_td2_ipmc_rp_delete(int unit, int rp_id, bcm_if_t intf_id);
extern int bcm_td2_ipmc_rp_delete_all(int unit, int rp_id);
extern int bcm_td2_ipmc_rp_ref_count_incr(int unit, int rp_id);
extern int bcm_td2_ipmc_rp_ref_count_decr(int unit, int rp_id);
extern int bcm_td2_ipmc_range_add(int unit, int *range_id,
        bcm_ipmc_range_t *range);
extern int bcm_td2_ipmc_range_delete(int unit, int range_id);
extern int bcm_td2_ipmc_range_delete_all(int unit);
extern int bcm_td2_ipmc_range_get(int unit, int range_id,
        bcm_ipmc_range_t *range);
extern int bcm_td2_ipmc_range_size_get(int unit, int *size);
/* TD2 v6 reserved multicast */
extern int  _bcm_td2_ipv6_reserved_multicast_addr_set(int unit, bcm_ip6_t ip6_addr,
                                                      bcm_ip6_t ip6_mask);
extern int  _bcm_td2_ipv6_reserved_multicast_addr_get(int unit, bcm_ip6_t *ip6_addr,
                                                      bcm_ip6_t *ip6_mask);
#if defined(INCLUDE_BFD)
extern int bcmi_td2_bfd_init(int unit);
#endif /* INCLUDE_BFD */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_td2_ipmc_pim_bidir_scache_size_get(int unit, uint32 *size);
extern int bcm_td2_ipmc_pim_bidir_sync(int unit, uint8 **scache_ptr);
extern int bcm_td2_ipmc_pim_bidir_scache_recover(int unit, uint8 **scache_ptr); 
extern int bcm_td2_ipmc_rp_ref_count_recover(int unit, int rp_id); 
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_td2_ipmc_pim_bidir_sw_dump(int unit);
extern void _bcm_td2_vxlan_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Flex stat - L3 host table */
extern bcm_error_t _bcm_td2_l3_host_stat_attach(
            int unit,
            bcm_l3_host_t *info,
            uint32 stat_counter_id);
extern bcm_error_t _bcm_td2_l3_host_stat_detach(
            int unit,
            bcm_l3_host_t *info);
extern bcm_error_t _bcm_td2_l3_host_stat_counter_get(
            int unit,
            int sync_mode,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values);
extern bcm_error_t _bcm_td2_l3_host_stat_counter_set(
            int unit,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values);
extern bcm_error_t _bcm_td2_l3_host_stat_id_get(
            int unit,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 *stat_counter_id);

/* Flex Stat - L3 Route defip */
extern bcm_error_t _bcm_td2_l3_route_stat_attach (
            int                 unit,
            bcm_l3_route_t      *info,
            uint32              stat_counter_id);
extern bcm_error_t    _bcm_td2_l3_route_stat_detach (
            int                 unit,
            bcm_l3_route_t      *info);

extern bcm_error_t _bcm_td2_l3_route_stat_counter_get(
            int                 unit, 
            int                 sync_mode, 
            bcm_l3_route_t      *info,
            bcm_l3_route_stat_t stat, 
            uint32              num_entries, 
            uint32              *counter_indexes, 
            bcm_stat_value_t    *counter_values);

extern bcm_error_t _bcm_td2_l3_route_stat_counter_set(
            int                 unit, 
            bcm_l3_route_t      *info,
            bcm_l3_route_stat_t stat, 
            uint32              num_entries, 
            uint32              *counter_indexes, 
            bcm_stat_value_t    *counter_values);
extern bcm_error_t _bcm_td2_l3_route_stat_multi_get(
            int                 unit, 
            bcm_l3_route_t      *info,
            int                 nstat, 
            bcm_l3_route_stat_t    *stat_arr,
            uint64              *value_arr);

extern bcm_error_t _bcm_td2_l3_route_stat_multi_get32(
            int                 unit, 
            bcm_l3_route_t      *info,
            int                 nstat, 
            bcm_l3_route_stat_t    *stat_arr,
            uint32              *value_arr);

extern bcm_error_t _bcm_td2_l3_route_stat_multi_set(
            int                 unit, 
            bcm_l3_route_t      *info,
            int                 nstat, 
            bcm_l3_route_stat_t    *stat_arr,
            uint64              *value_arr);

extern bcm_error_t _bcm_td2_l3_route_stat_multi_set32(
            int                 unit, 
            bcm_l3_route_t      *info,
            int                 nstat, 
            bcm_l3_route_stat_t  *stat_arr,
            uint32              *value_arr);
extern bcm_error_t _bcm_td2_l3_route_stat_id_get(
            int                 unit,
            bcm_l3_route_t      *info,
            bcm_l3_route_stat_t stat, 
            uint32              *stat_counter_id);
extern int _bcm_td2_ip_key_to_l3cfg(
            int unit, bcm_l3_key_t *ipkey, 
            _bcm_l3_cfg_t *l3cfg);
extern int
_bcm_td2_l3cfg_to_ipkey(
            int unit, bcm_l3_key_t *ipkey, _bcm_l3_cfg_t *l3cfg);

extern bcm_error_t _bcm_td2_ipmc_stat_attach(
            int unit,
            bcm_ipmc_addr_t *info,
            uint32 stat_counter_id);
extern bcm_error_t _bcm_td2_ipmc_stat_detach(
            int unit,
            bcm_ipmc_addr_t *info);
extern bcm_error_t _bcm_td2_ipmc_stat_counter_get(
            int unit,
            int sync_mode,
            bcm_ipmc_addr_t *info,
            bcm_ipmc_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values);
extern bcm_error_t _bcm_td2_ipmc_stat_counter_set(
            int unit,
            bcm_ipmc_addr_t *info,
            bcm_ipmc_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values);
extern bcm_error_t _bcm_td2_ipmc_stat_id_get(
            int unit,
            bcm_ipmc_addr_t *info,
            bcm_ipmc_stat_t stat,
            uint32 *stat_counter_id);
extern  bcm_error_t  _bcm_td2_ipmc_stat_multi_get(
                            int                 unit, 
                            bcm_ipmc_addr_t     *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t     *stat_arr,
                            uint64              *value_arr);
extern bcm_error_t  _bcm_td2_ipmc_stat_multi_get32(
                            int                 unit, 
                            bcm_ipmc_addr_t     *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t     *stat_arr,
                            uint32              *value_arr);
extern bcm_error_t  _bcm_td2_ipmc_stat_multi_set(
                            int                 unit, 
                            bcm_ipmc_addr_t     *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t     *stat_arr,
                            uint64              *value_arr);
extern  bcm_error_t  _bcm_td2_ipmc_stat_multi_set32(
                            int                 unit, 
                            bcm_ipmc_addr_t     *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t     *stat_arr,
                            uint32              *value_arr);

extern int _bcm_td2_l3_ip4_options_free_resources(int unit);
extern int _bcm_td2_l3_ip4_options_profile_init(int unit);
extern int _bcm_td2_l3_ip4_options_profile_create(int unit, uint32 flags, 
                                   bcm_l3_ip4_options_action_t default_action, 
                                   int *ip4_options_profile_id);
extern int _bcm_td2_l3_ip4_options_profile_destroy(int unit, 
                                                   int ip4_options_profile_id);
extern int _bcm_td2_l3_ip4_options_profile_action_set(int unit,
                         int ip4_options_profile_id, 
                         int ip4_option, 
                         bcm_l3_ip4_options_action_t action);
extern int _bcm_td2_l3_ip4_options_profile_action_get(int unit,
                         int ip4_options_profile_id, 
                         int ip4_option, 
                         bcm_l3_ip4_options_action_t *action);
extern int _bcm_td2_l3_ip4_options_profile_multi_get(int unit, int array_size, 
                                      int *ip_options_ids_array, 
                                      int *array_count);
extern int _bcm_td2_l3_ip4_options_profile_action_multi_get(int unit, 
                          uint32 flags,
                          int ip_option_profile_id, int array_size, 
                          bcm_l3_ip4_options_action_t *array, int *array_count);
extern int _bcm_td2_l3_ip4_options_profile_id2idx(int unit, int profile_id, 
                                                  int *hw_idx);
extern int _bcm_td2_l3_ip4_options_profile_idx2id(int unit, int hw_idx, 
                                                  int *profile_id);
extern int _bcm_td2_vp_urpf_mode_set(int unit, bcm_port_t port, int arg);
extern int _bcm_td2_vp_urpf_mode_get(int unit, bcm_port_t port, int *arg);
extern int _bcm_td2_l3_intf_iif_profile_update(int unit, bcm_vlan_t vid,
                                               bcm_vlan_control_vlan_t *control);
extern int _bcm_td2_l3_intf_iif_profile_get(int unit, bcm_vlan_t vid,
                                            bcm_vlan_control_vlan_t *control);
extern int _bcm_td2_l3_iif_ip4_options_profile_index_set(
               int unit, _bcm_l3_intf_cfg_t *intf_info);
extern int _bcm_td2_l3_iif_ip4_options_profile_index_get(
               int unit, _bcm_l3_intf_cfg_t *intf_info);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_td2_l3_ip4_options_profile_scache_len_get(int unit, 
                                                          int *wb_alloc_size);
extern int _bcm_td2_l3_ip4_options_recover(int unit, uint8 **scache_ptr,
                                           int offset);

extern int _bcm_td2_l3_ip4_options_sync(int unit, uint8 **scache_ptr, int offset);
#endif /* BCM_WARM_BOOT_SUPPORT */

/* VXLAN */
extern int bcm_td2_vxlan_udpDestPort_set(int unit, int udpDestPort);
extern int bcm_td2_vxlan_udpSourcePort_set(int unit, int hashEnable);
extern int bcm_td2_vxlan_udpDestPort_get(int unit, int *udpDestPort);
extern int bcm_td2_vxlan_udpSourcePort_get(int unit, int *hashEnable);
extern int bcm_td2_vxlan_egress_get(int unit, bcm_l3_egress_t *egr, int nh_index);
extern int bcm_td2_vxlan_next_hop_set(int unit, int nh_index, uint32 flags, int vp, int drop);
extern int bcm_td2_vxlan_egress_reset(int unit, int nh_index);

extern int _bcm_td2_vxlan_port_resolve(
           int unit, 
           bcm_gport_t l2gre_port_id, 
           bcm_if_t encap_id, 
           bcm_module_t *modid, 
           bcm_port_t *port,
           bcm_trunk_t *trunk_id, 
           int *id);

extern  int  bcm_td2_vxlan_lock(int unit);
extern void bcm_td2_vxlan_unlock(int unit);
extern int bcm_td2_vxlan_init(int unit);
extern int bcm_td2_vxlan_cleanup(int unit);
extern int bcm_td2_vxlan_vpn_create(int unit, bcm_vxlan_vpn_config_t *info);
extern int bcm_td2_vxlan_vpn_destroy(int unit, bcm_vpn_t l2vpn);
extern int bcm_td2_vxlan_vpn_destroy_all(int unit);
extern int bcm_td2_vxlan_vpn_traverse(int unit, bcm_vxlan_vpn_traverse_cb cb, void *user_data);
extern int bcm_td2_vxlan_port_traverse(int unit, bcm_vxlan_port_traverse_cb cb, void *user_data);
extern int bcm_td2_vxlan_vpn_get( int unit, bcm_vpn_t l2vpn, bcm_vxlan_vpn_config_t *info);
extern int bcm_td2_vxlan_port_add(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t  *vxlan_port);
extern int bcm_td2_vxlan_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t vxlan_port_id);
extern int bcm_td2_vxlan_port_delete_all(int unit, bcm_vpn_t vpn);
extern int bcm_td2_vxlan_port_get(int unit, bcm_vpn_t vpn, bcm_vxlan_port_t *vxlan_port);
extern int bcm_td2_vxlan_port_get_all( int unit, bcm_vpn_t l2vpn, int port_max, bcm_vxlan_port_t *port_array, int *port_count);
extern void bcm_td2_vxlan_port_match_count_adjust(int unit, int vp, int step);
extern int bcm_td2_vxlan_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info);
extern int bcm_td2_vxlan_tunnel_initiator_destroy(int unit, bcm_gport_t vxlan_tunnel_id);
extern int bcm_td2_vxlan_tunnel_initiator_destroy_all(int unit);
extern int bcm_td2_vxlan_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info);
extern int bcm_td2_vxlan_tunnel_initiator_traverse(int unit, bcm_tunnel_initiator_traverse_cb cb, void *user_data);
extern int _bcm_td2_vxlan_tunnel_initiator_cmp(int unit, void *buf, int index,
                                               int *cmp_result);
extern int _bcm_td2_vxlan_tunnel_initiator_hash_calc(int unit, void *buf,
                                                     uint16 *hash);
extern int bcm_td2_vxlan_tunnel_terminator_create(int unit, bcm_tunnel_terminator_t *info);
extern int bcm_td2_vxlan_tunnel_terminator_update(int unit, bcm_tunnel_terminator_t *info);
extern int bcm_td2_vxlan_tunnel_terminator_destroy(int unit, bcm_gport_t vxlan_tunnel_id);
extern int bcm_td2_vxlan_tunnel_terminator_destroy_all(int unit);
extern int bcm_td2_vxlan_tunnel_terminator_get( int unit, bcm_tunnel_terminator_t *info);
extern int bcm_td2_vxlan_tunnel_terminator_traverse(int unit, bcm_tunnel_terminator_traverse_cb cb, void *user_data);
extern int  _bcm_td2_vxlan_vpn_get(int unit, int vfi_index, bcm_vlan_t *vid);
extern int bcm_td2_vxlan_multicast_leaf_entry_check(int unit, int tunnel_idx, int enable_flag);
extern int _bcm_td2_vxlan_vpn_is_valid( int unit, bcm_vpn_t l2vpn);
extern int _bcm_td2_vxlan_vpn_is_eline( int unit, bcm_vpn_t l2vpn, uint8 *isEline);
extern int bcm_td2_vxlan_port_learn_set(int unit, bcm_gport_t vxlan_port_id, uint32 flags);
extern int bcm_td2_vxlan_port_learn_get(int unit, bcm_gport_t vxlan_port_id, uint32 *flags);
extern int bcm_td2_vxlan_trunk_member_add (int unit, bcm_trunk_t trunk_id, 
                                             int trunk_member_count, bcm_port_t *trunk_member_array);
extern int bcm_td2_vxlan_trunk_member_delete(int unit, bcm_trunk_t trunk_id, 
                                             int trunk_member_count, bcm_port_t *trunk_member_array); 
extern int _bcm_td2_vxlan_port_get(int unit, bcm_vpn_t vpn, int vp,
									bcm_vxlan_port_t  *vxlan_port);
extern int _bcm_td2_vxlan_match_add(int unit, bcm_vxlan_port_t *vxlan_port, int vp, bcm_vpn_t vpn);
extern int _bcm_td2_vxlan_match_delete(int unit,  int vp , bcm_vxlan_port_t vxlan_port, bcm_vpn_t vpn);
extern int bcm_td2_vxlan_port_source_vp_lag_set(int unit, bcm_gport_t gport, int vp_lag_vp);
extern int bcm_td2_vxlan_port_source_vp_lag_clear(int unit, bcm_gport_t gport, int vp_lag_vp);
extern int bcm_td2_vxlan_port_source_vp_lag_get(int unit, bcm_gport_t gport, int *vp_lag_vp);
extern int _bcm_td2_vxlan_svp_field_set(int unit, bcm_gport_t vp, soc_field_t field, int value);

/* L2GRE */
extern int bcm_td2_l2gre_match_add(int unit, bcm_l2gre_port_t *l2gre_port, int vp, bcm_vpn_t vpn);
extern int bcm_td2_l2gre_match_delete(int unit,  int vp, bcm_l2gre_port_t *l2gre_port);
extern int bcm_td2_l2gre_match_get(int unit, bcm_l2gre_port_t *l2gre_port, int vp);
extern int _bcm_td2_l2gre_eline_vp_configure (int unit, int vfi_index, int active_vp, 
                  source_vp_entry_t *svp, int tpid_enable, bcm_l2gre_port_t  *l2gre_port);
extern int _bcm_td2_l2gre_elan_vp_configure (int unit, int vfi_index, int active_vp, 
                  source_vp_entry_t *svp, int tpid_enable, bcm_l2gre_port_t  *l2gre_port);
extern int _bcm_td2_vxlan_port_get(int unit, bcm_vpn_t vpn, int vp,
                                  bcm_vxlan_port_t  *vxlan_port);
extern int _bcm_td2_vxlan_default_port_get(int unit, int vp, bcm_vxlan_port_t *vxlan_port);

/* VP LAG */
extern int bcm_td2_vp_lag_init(int unit);
extern void bcm_td2_vp_lag_deinit(int unit);
extern int bcm_td2_vp_lag_create(int unit, uint32 flags, bcm_trunk_t *tid,
                                 bcm_gport_t *vplag_port_id);
extern int bcm_td2_vp_lag_info_get(int unit, int *ngroups_vp_lag,
        int *nports_vp_lag);
extern int bcm_td2_vp_lag_set(int unit, bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info, int member_count,
        bcm_trunk_member_t *member_array);
extern int bcm_td2_vp_lag_destroy(int unit, bcm_trunk_t tid);
extern int bcm_td2_vp_lag_get(int unit, bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info, int member_max,
        bcm_trunk_member_t *member_array, int *member_count);
extern int _bcm_td2_vp_lag_match_multi_get(
               int unit, bcm_trunk_t tid, int size,
               bcm_port_match_info_t *match_array, int *count);
extern int bcm_td2_tid_to_vp_lag_vp(int unit, bcm_trunk_t tid, int *vp_lag_vp);
extern int bcm_td2_vp_lag_vp_to_tid(int unit, int vp_lag_vp, bcm_trunk_t *tid);
extern int bcm_td2_vp_lag_find(int unit, bcm_port_t port, bcm_trunk_t *tid);
extern int bcm_td2_vlan_vp_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_td2_vlan_vp_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_td2_vlan_vp_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp);
extern int bcm_td2_niv_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_td2_niv_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_td2_niv_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp);
extern int bcm_td2_extender_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_td2_extender_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int bcm_td2_extender_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp);
extern int bcm_td2_mim_port_source_vp_lag_set(int unit, bcm_gport_t gport,
                                              int vp_lag_vp);
extern int bcm_td2_mim_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
                                                int vp_lag_vp);
extern int bcm_td2_mim_port_source_vp_lag_get(int unit, bcm_gport_t gport,
                                              int *vp_lag_vp);
extern int bcm_td2_l2gre_port_source_vp_lag_set(int unit, bcm_gport_t gport,
                                                int vp_lag_vp);
extern int bcm_td2_l2gre_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
                                                  int vp_lag_vp);
extern int bcm_td2_l2gre_port_source_vp_lag_get(int unit, bcm_gport_t gport,
                                                int *vp_lag_vp);
extern int bcm_td2_vp_lag_status_get(int unit, int vp_lag_id, int *is_vp_lag,
        int *is_used, int *has_member);
extern int _bcm_td2_vp_lag_member_is_vfi_type(int unit, bcm_trunk_t tid, int *vfi_type);
extern int _bcm_esw_num_vp_trunk_groups(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_td2_vp_lag_reinit(int unit);
extern int _bcm_td2_vp_lag_egr_dis_vp_scache_size_get(int unit, int *size);
extern int _bcm_td2_vp_lag_egr_dis_vp_scache_recover(int unit, uint8 **scache_ptr);
extern int _bcm_td2_vp_lag_egr_dis_vp_sync(int unit, uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_td2_vp_lag_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

extern int bcm_td2_multicast_l3_vp_init(int unit);
extern int bcm_td2_multicast_l3_vp_detach(int unit);
extern int bcm_td2_multicast_l3_vp_encap_get(int unit, bcm_multicast_t group,
        bcm_gport_t port, bcm_if_t intf, bcm_if_t *encap_id);
extern int bcm_td2_multicast_l3_vp_next_hop_free(int unit, int intf);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_td2_multicast_l3_vp_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/* ECMP resilient hashing */
extern int bcm_td2_ecmp_rh_init(int unit);
extern void bcm_td2_ecmp_rh_deinit(int unit);
extern int bcm_td2_l3_egress_ecmp_rh_create(int unit,
        bcm_l3_egress_ecmp_t *ecmp, int intf_count, bcm_if_t *intf_array,
        int op, int count, bcm_if_t *intf);
extern int bcm_td2_l3_egress_ecmp_rh_destroy(int unit, bcm_if_t mpintf);
extern int bcm_td2_l3_egress_ecmp_rh_get(int unit, bcm_l3_egress_ecmp_t *ecmp);
extern int bcm_td2_l3_egress_ecmp_rh_ethertype_set(int unit, uint32 flags,
        int ethertype_count, int *ethertype_array);
extern int bcm_td2_l3_egress_ecmp_rh_ethertype_get(int unit, uint32 *flags,
        int ethertype_max, int *ethertype_array, int *ethertype_count);
extern int bcm_td2_l3_egress_ecmp_rh_shared_copy(int unit, int old_ecmp_group,
        int new_ecmp_group);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_td2_ecmp_rh_hw_recover(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_td2_ecmp_rh_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
/* Proxy */
extern int bcm_td2_proxy_egress_create(int unit, uint32 flags, bcm_proxy_egress_t *proxy_egr, 
                          bcm_if_t *proxy_intf_id);
extern int  bcm_td2_proxy_egress_destroy(int unit, bcm_if_t proxy_intf_id);
extern int  bcm_td2_proxy_egress_get(int unit, bcm_if_t proxy_intf_id, bcm_proxy_egress_t *proxy_egr);
extern int _bcm_td2_proxy_nh_add(int unit, int idx, void *buf, void *info);
extern int _bcm_td2_proxy_nh_get(int unit, int idx, bcm_proxy_egress_t *proxy_nh_entry);
extern int bcm_td2_proxy_egress_traverse(int unit, bcm_proxy_egress_traverse_cb  cb,
                                                        void *user_data);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_td2_vlan_vp_scache_size_get(int unit, int *size);
extern int _bcm_td2_vlan_vp_sync(int unit, uint8 **scache_ptr);
extern int _bcm_td2_vlan_vp_recover(int unit, uint8 **scache_ptr);
#endif
#endif /* INCLUDE_L3 */

extern int bcm_td2_cosq_init(int unit);

extern int bcm_td2_cosq_detach(int unit, int software_state_only);

extern int bcm_td2_cosq_config_set(int unit, int numq);

extern int bcm_td2_cosq_config_get(int unit, int *numq);

#if defined(BCM_TRIDENT2_SUPPORT)
extern int bcm_td2p_port_cosq_config_set(int unit, bcm_port_t  port, int enable);
#endif

extern int bcm_td2_cosq_mapping_set(int unit, bcm_port_t gport, 
                        bcm_cos_t priority, bcm_cos_queue_t cosq);

extern int bcm_td2_cosq_mapping_get(int unit, bcm_port_t gport,
                        bcm_cos_t priority, bcm_cos_queue_t *cosq);

extern int bcm_td2_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int *weights, int delay);

extern int bcm_td2_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int *weights, int *delay);

extern int bcm_td2_cosq_priority_mapping_get_all(int unit, bcm_gport_t gport,
                                     int index, bcm_cosq_priority_mapping_t type,
                                     int pri_max, int *pri_array,
                                     int *pri_count);

extern int bcm_td2_cosq_sched_weight_max_get(int unit, int mode, 
                                                int *weight_max);

extern int bcm_td2_cosq_discard_set(int unit, uint32 flags);

extern int bcm_td2_cosq_discard_get(int unit, uint32 *flags);

extern int
bcm_td2_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int
bcm_td2_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int
_bcm_td2_cosq_pfc_class_resolve(bcm_switch_control_t sctype, int *type,
                               int *class);

extern int
bcm_td2_cosq_port_pfc_op(int unit, bcm_port_t port,
                        bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                        bcm_gport_t *gport, int gport_count);

extern int
bcm_td2_cosq_port_pfc_get(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype,
                         bcm_gport_t *gport, int gport_count,
                         int *actual_gport_count);

extern int
bcm_td2_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                      bcm_gport_t *gport);

extern int
bcm_td2_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq);

extern int
bcm_td2_cosq_discard_port_get(int unit, bcm_port_t port, 
                 bcm_cos_queue_t cosq, uint32 color, int *drop_start, 
                        int *drop_slope, int *average_time);

extern int
bcm_td2_cosq_gport_delete(int unit, bcm_gport_t gport);

extern int
bcm_td2_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                      int *numq, uint32 *flags);

extern int
bcm_td2_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data);

extern int
bcm_td2_cosq_discard_port_set(int unit, bcm_port_t port, 
                            bcm_cos_queue_t cosq,
                            uint32 color, int drop_start, int drop_slope,
                            int average_time);

extern int
bcm_td2_cosq_gport_attach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq);

extern int
bcm_td2_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq);

extern int
bcm_td2_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                             bcm_cos_queue_t cosq, bcm_gport_t *out_gport);

extern int
bcm_td2_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 min_quantum, uint32 max_quantum,
                               uint32 burst_quantum, uint32 flags);

extern int
bcm_td2_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *min_quantum, uint32 *max_quantum,
                               uint32 *burst_quantum, uint32 *flags);

extern int
bcm_td2_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int pps);

extern int
bcm_td2_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int *pps);

extern int bcm_td2_cosq_port_burst_set(int unit, bcm_port_t port, 
                                       bcm_cos_queue_t cosq, int burst);

extern int
bcm_td2_cosq_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value);

extern int
bcm_td2_cosq_stat_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, int sync_mode, uint64 *value);

extern int
bcm_td2_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int *burst);

extern int
bcm_td2_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg);

extern int
bcm_td2_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int *arg);

extern int
bcm_td2_cosq_service_pool_set(int unit, bcm_service_pool_id_t id, 
                                      bcm_cosq_service_pool_t cosq_service_pool);

extern int
bcm_td2_cosq_service_pool_get(int unit, bcm_service_pool_id_t id, 
                                      bcm_cosq_service_pool_t *cosq_service_pool);

extern int
bcm_td2_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst_min, 
                                      uint32 kbits_burst_max);

extern int
bcm_td2_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 *kbits_burst_min,
                                       uint32 *kbits_burst_max);
extern int
bcm_td2_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags);

extern int
bcm_td2_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags);

extern int bcm_td2_cosq_congestion_queue_set(int unit, bcm_port_t port,
                                            bcm_cos_queue_t cosq, int index);
extern int bcm_td2_cosq_congestion_queue_get(int unit, bcm_port_t port,
                                            bcm_cos_queue_t cosq, int *index);
extern int bcm_td2_cosq_congestion_quantize_set(int unit, bcm_port_t port,
                                               bcm_cos_queue_t cosq,
                                               int weight_code, int setpoint);
extern int bcm_td2_cosq_congestion_quantize_get(int unit, bcm_port_t port,
                                               bcm_cos_queue_t cosq,
                                               int *weight_code,
                                               int *setpoint);
extern int bcm_td2_cosq_congestion_sample_int_set(int unit, bcm_port_t port,
                                                 bcm_cos_queue_t cosq,
                                                 int min, int max);
extern int bcm_td2_cosq_congestion_sample_int_get(int unit, bcm_port_t port,
                                                 bcm_cos_queue_t cosq,
                                                 int *min, int *max);
extern int bcm_td2_cosq_drop_status_enable_set(int unit, bcm_port_t port,
                                              int enable);
extern int _bcm_td2_get_def_qbase(int unit, bcm_port_t inport, int qtype, 
                            int *pbase, int *pnumq);

extern int bcm_td2_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);
extern int bcm_td2_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int *mode,
                                       int *weight);
extern int
_bcm_td2_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                          bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                          int *qnum);

extern int
bcm_td2_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t gport, bcm_cos_queue_t cosq);

extern int 
bcm_td2_cosq_gport_congestion_config_set(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config);
extern int
bcm_td2_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t *gport, bcm_cos_queue_t *cosq);
extern int 
bcm_td2_cosq_gport_congestion_config_get(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config);

extern int 
bcm_td2_cosq_bst_profile_set(int unit, 
                             bcm_gport_t port, 
                             bcm_cos_queue_t cosq, 
                             bcm_bst_stat_id_t bid,
                             bcm_cosq_bst_profile_t *profile);

extern int 
bcm_td2_cosq_bst_profile_get(int unit, 
                             bcm_gport_t port, 
                             bcm_cos_queue_t cosq, 
                             bcm_bst_stat_id_t bid,
                             bcm_cosq_bst_profile_t *profile);

extern int 
bcm_td2_cosq_bst_stat_get(int unit, 
                              bcm_gport_t port, 
                              bcm_cos_queue_t cosq, 
                              bcm_bst_stat_id_t bid,
                              uint32 options,
                              uint64 *pvalue);

extern int 
bcm_td2_cosq_bst_stat_multi_get(int unit,
                                bcm_gport_t port,
                                bcm_cos_queue_t cosq,
                                uint32 options,
                                int max_values,
                                bcm_bst_stat_id_t *id_list,
                                uint64 *pvalues);
extern int 
bcm_td2_cosq_bst_stat_clear(int unit, 
                            bcm_gport_t port, 
                            bcm_cos_queue_t cosq, 
                            bcm_bst_stat_id_t bid);

extern int 
bcm_td2_cosq_bst_stat_sync(int unit, bcm_bst_stat_id_t bid);

extern int bcm_td2_cosq_endpoint_create(
        int unit, 
        bcm_cosq_classifier_t *classifier, 
        int *classifier_id);
extern int bcm_td2_cosq_endpoint_destroy(
        int unit, 
        int classifier_id);
extern int bcm_td2_cosq_endpoint_get(
        int unit, 
        int classifier_id,
        bcm_cosq_classifier_t *classifier);
extern int bcm_td2_cosq_endpoint_map_set(
        int unit, 
        bcm_port_t port, 
        int classifier_id, 
        bcm_gport_t queue_group, 
        int array_count, 
        bcm_cos_t *priority_array, 
        bcm_cos_queue_t *cosq_array);
extern int bcm_td2_cosq_endpoint_map_get(
        int unit, 
        bcm_port_t port, 
        int classifier_id, 
        bcm_gport_t *queue_group, 
        int array_max, 
        bcm_cos_t *priority_array, 
        bcm_cos_queue_t *cosq_array, 
        int *array_count);
extern int bcm_td2_cosq_endpoint_map_clear(
        int unit, 
        bcm_gport_t port, 
        int classifier_id);

extern int
bcm_td2_cosq_field_classifier_get(
    int unit, int classifier_id, bcm_cosq_classifier_t *classifier);

extern int
bcm_td2_cosq_field_classifier_id_create(
    int unit, bcm_cosq_classifier_t *classifier, int *classifier_id);

extern int
bcm_td2_cosq_field_classifier_id_destroy(
    int unit, int classifier_id);

extern int
bcm_td2_cosq_field_classifier_map_set(
    int unit, int classifier_id, int count,
    bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);

extern int
bcm_td2_cosq_field_classifier_map_get(
    int unit, int classifier_id,
    int array_max, bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array, int *array_count);

extern int
bcm_td2_cosq_field_classifier_map_clear(
    int unit, int classifier_id);

extern int bcm_td2_cosq_sync(int unit);
extern int bcm_td2_cosq_reinit(int unit);

extern void bcm_td2_cosq_sw_dump(int unit);

extern int bcm_td2_cosq_cpu_cosq_enable_set(
        int unit, 
        bcm_cos_queue_t cosq, 
        int enable);

extern int bcm_td2_cosq_cpu_cosq_enable_get(
        int unit, 
        bcm_cos_queue_t cosq, 
        int *enable);

extern int
bcm_td2_cosq_service_classifier_id_create(int unit,
                                          bcm_cosq_classifier_t *classifier,
                                          int *classifier_id);
extern int
bcm_td2_cosq_service_classifier_id_destroy(int unit, int classifier_id);
extern int
bcm_td2_cosq_service_classifier_get(int unit, int classifier_id,
                                    bcm_cosq_classifier_t *classifier);
extern int
bcm_td2_cosq_service_map_set(int unit, bcm_port_t port, int classifier_id,
                             bcm_gport_t queue_group, int array_count,
                             bcm_cos_t *priority_array,
                             bcm_cos_queue_t *cosq_array);
extern int
bcm_td2_cosq_service_map_get(int unit, bcm_port_t port, int classifier_id,
                             bcm_gport_t *queue_group, int array_max,
                             bcm_cos_t *priority_array,
                             bcm_cos_queue_t *cosq_array, int *array_count);
extern int
bcm_td2_cosq_service_map_clear(int unit, bcm_gport_t port, int classifier_id);

typedef enum {
    _BCM_TD2_COSQ_INDEX_STYLE_BUCKET,
    _BCM_TD2_COSQ_INDEX_STYLE_QCN,
    _BCM_TD2_COSQ_INDEX_STYLE_WRED,
    _BCM_TD2_COSQ_INDEX_STYLE_WRED_PORT,
    _BCM_TD2_COSQ_INDEX_STYLE_WRED_POOL,
    _BCM_TD2_COSQ_INDEX_STYLE_SCHEDULER,
    _BCM_TD2_COSQ_INDEX_STYLE_PERQ_XMT,
    _BCM_TD2_COSQ_INDEX_STYLE_UCAST_DROP,
    _BCM_TD2_COSQ_INDEX_STYLE_COS,
    _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_TD2_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
    _BCM_TD2_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
    _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
    _BCM_TD2_COSQ_INDEX_STYLE_UC_EGR_POOL,
    _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP,
    _BCM_TD2_COSQ_INDEX_STYLE_COUNT
} _bcm_td2_cosq_index_style_t;

extern int
_bcm_td2_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq, 
                           _bcm_td2_cosq_index_style_t style,                 
                           bcm_port_t *local_port, int *index, int *count);
extern int
bcm_td2_cosq_pfc_class_mapping_set(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array);
extern int
bcm_td2_cosq_pfc_class_mapping_get(int unit,
    bcm_gport_t port,
    int array_max,
    bcm_cosq_pfc_class_mapping_t *mapping_array,
    int *array_count);
extern int
bcm_td2_cosq_default_llfc_profile_attach(int unit, bcm_gport_t port);

extern int
bcm_td2_cosq_llfc_profile_detach(int unit, bcm_gport_t port);

extern int
bcm_td2_port_priority_group_mapping_set(int unit, bcm_gport_t gport,
                                        int prio, int priority_group);
extern int
bcm_td2_port_priority_group_mapping_get(int unit, bcm_gport_t gport,
                                        int prio, int *priority_group);
extern int
bcm_td2_port_priority_group_config_set(int unit, bcm_gport_t gport,
    int priority_group, bcm_port_priority_group_config_t *prigrp_config);
extern int
bcm_td2_port_priority_group_config_get(int unit, bcm_gport_t gport,
    int priority_group, bcm_port_priority_group_config_t *prigrp_config);
/* Flexible hashing support routines */

#if defined (BCM_WARM_BOOT_SUPPORT)
extern int bcmi_td2_switch_wb_hash_entry_reinit(
    int unit,
    uint8 **scache_ptr);

#if defined (BCM_TOMAHAWK_SUPPORT)
extern int bcmi_th_switch_wb_hash_entry_reinit(
    int unit,
    uint8 **scache_ptr);
#endif

extern int bcmi_td2_switch_wb_hash_entry_sync(
    int unit,
    uint8 **scache_ptr);

extern int bcmi_td2_switch_wb_hash_entry_scache_size_get(
    int unit,
    int *req_scache_size);
#endif

extern int bcm_td2_switch_hash_entry_create( 
                                int unit, 
                                bcm_field_group_t group, 
                                bcm_hash_entry_t *entry_id);
extern int bcm_td2_switch_hash_entry_create_qset( 
                                int unit, 
                                bcm_field_qset_t qset, 
                                bcm_hash_entry_t *entry_id);
extern int bcm_td2_switch_hash_entry_set(
                                int unit,
                                bcm_hash_entry_t entry, 
                                bcm_switch_hash_entry_config_t *config);
extern int bcm_td2_switch_hash_entry_get(
                                int unit,
                                bcm_hash_entry_t entry, 
                                bcm_switch_hash_entry_config_t *config);
extern int bcm_td2_switch_hash_entry_traverse(
                                int unit,
                                int flags, 
                                bcm_switch_hash_entry_traverse_cb cb_fn,
                                void *user_data);
extern int bcm_td2_switch_hash_qualify_udf(
                                int unit,
                                bcm_hash_entry_t entry_id, 
                                bcm_udf_id_t udf_id,
                                int length, 
                                uint8 *data, uint8 *mask);
extern int bcm_td2_switch_hash_qualify_udf_get(
                                int unit, 
                                bcm_hash_entry_t entry_id, 
                                bcm_udf_id_t udf_id,
                                int max_length, 
                                uint8 *data, uint8 *mask,
                                int *actual_length);
extern int bcm_td2_switch_hash_qualify_data (
                                int unit, 
                                bcm_hash_entry_t entry_id,
                                int qual_id, 
                                uint32 data, 
                                uint32 mask);
extern int bcm_td2_switch_hash_entry_remove (
                                int unit, 
                                bcm_hash_entry_t entry_id);
extern int bcm_td2_switch_hash_entry_reinstall (
                                int unit, 
                                bcm_hash_entry_t entry_id, 
                                uint32 offset, 
                                uint32 mask);
extern int bcm_td2_switch_hash_entry_install (
                                int unit, 
                                bcm_hash_entry_t entry_id, 
                                uint32 offset, 
                                uint32 mask);
extern int bcm_td2_switch_hash_entry_destroy (
                                int unit, 
                                bcm_hash_entry_t entry_id);
extern int bcm_td2_switch_hash_entry_detach(int unit);
extern int bcm_td2_switch_hash_entry_init (int unit);
extern int bcm_td2_switch_match_detach(int unit);
extern int bcm_td2_switch_match_init(int unit);
extern int bcm_td2_switch_match_config_add(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_config_info_t *config_info,
    int *match_id);
extern int bcm_td2_switch_match_config_delete(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id);
extern int bcm_td2_switch_match_config_delete_all(
    int unit,
    bcm_switch_match_service_t match_service);
extern int bcm_td2_switch_match_config_get(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id,
    bcm_switch_match_config_info_t *config_info);
extern int bcm_td2_switch_match_config_set(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id,
    bcm_switch_match_config_info_t *config_info);
extern int bcm_td2_switch_match_config_traverse(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_config_traverse_cb cb_fn,
    void *user_data);
extern int bcm_td2_l2x_freeze_mode_set(int unit, int arg);
extern int bcm_td2_l2x_freeze_mode_get(int unit, int *arg);
extern int bcm_td2_switch_l2_freeze_mode_init(int unit);
extern int _bcm_td2_flags_to_stp_state(int unit, uint32 flags, int egress);
extern int _bcm_td2_stp_state_to_flags(int unit, int stp_state, int egress);
extern int bcm_td2_ing_vp_vlan_membership_add(int unit, 
                      int vp, bcm_vlan_t vlan, uint32 flags, 
                      _bcm_vp_vlan_mbrship_key_type_e key_type);
extern int bcm_td2_egr_vp_vlan_membership_add(int unit, 
                             int vp, bcm_vlan_t vlan, uint32 flags, 
                             _bcm_vp_vlan_mbrship_key_type_e key_type);
extern int bcm_td2_ing_vp_vlan_membership_delete(int unit,
                             int vp, bcm_vlan_t vlan, 
                             _bcm_vp_vlan_mbrship_key_type_e key_type);
extern int bcm_td2_egr_vp_vlan_membership_delete(int unit,
                             int vp, bcm_vlan_t vlan, 
                             _bcm_vp_vlan_mbrship_key_type_e key_type);
extern int bcm_td2_ing_vp_vlan_membership_get(int unit, 
              int vp, bcm_vlan_t vlan, uint32 *flags,
              _bcm_vp_vlan_mbrship_key_type_e key_type);
extern int bcm_td2_egr_vp_vlan_membership_get(int unit, 
              int vp, bcm_vlan_t vlan, uint32 *flags,
              _bcm_vp_vlan_mbrship_key_type_e key_type);
extern int bcm_td2_ing_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap, int arr_size, int *flags_arr);
extern int bcm_td2_egr_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap, int arr_size, int *flags_arr);
extern int bcm_td2_ing_vp_vlan_membership_delete_all(int unit, bcm_vlan_t vlan);
extern int bcm_td2_egr_vp_vlan_membership_delete_all(int unit, bcm_vlan_t vlan);
extern int bcm_td2_vp_vlan_stp_set(int unit, 
                int vp, bcm_vlan_t vlan, int stp_state);
extern int bcm_td2_vp_vlan_stp_get(int unit, 
                int vp, bcm_vlan_t vlan, int *stp_state);
extern int bcm_td2_vp_vlan_membership_add(int unit, int vp, bcm_vlan_t vlan, int flags);
extern int bcm_td2_vp_vlan_membership_delete(int unit, int vp, bcm_vlan_t vlan);
extern int bcm_td2_vp_vlan_membership_get(int unit, int vp, bcm_vlan_t vlan, int *flags);
extern int bcm_td2_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan,
        int array_max, bcm_gport_t *gport_array,
        int *flags_array, int *array_size);

/* Higig resilient hashing */
extern int bcm_td2_hg_rh_init(int unit);
extern void bcm_td2_hg_rh_deinit(int unit);
extern int bcm_td2_hg_rh_dynamic_size_encode(int unit, int dynamic_size,
        int *encoded_value);
extern int bcm_td2_hg_rh_set(int unit, int hgtid,
        _esw_trunk_add_info_t *add_info);
extern int bcm_td2_hg_rh_replace(int unit, int hgtid,
        _esw_trunk_add_info_t *add_info, uint16 num_existing,
        uint16 *existing_ports, uint32 *existing_flags);
extern int bcm_td2_hg_rh_add(int unit, int hgtid,
        _esw_trunk_add_info_t *add_info, bcm_trunk_member_t *new_member);
extern int bcm_td2_hg_rh_delete(int unit, int hgtid,
        _esw_trunk_add_info_t *add_info, bcm_trunk_member_t *leaving_member);
extern int bcm_td2_hg_rh_free_resource(int unit, int hgtid);
extern int bcm_td2_hg_rh_status_get(int unit, int *rh_status);
extern int bcm_td2_hg_rh_dynamic_size_set(int unit, int hgtid, int dynamic_size);
extern int bcm_td2_hg_rh_ethertype_set(int unit, uint32 flags,
        int ethertype_count, int *ethertype_array);
extern int bcm_td2_hg_rh_ethertype_get(int unit, uint32 *flags,
        int ethertype_max, int *ethertype_array, int *ethertype_count);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_td2_hg_rh_recover(int unit, int hgtid,
        trunk_private_t *trunk_info);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_td2_hg_rh_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* LAG resilient hashing */
extern int bcm_td2_lag_rh_init(int unit);
extern void bcm_td2_lag_rh_deinit(int unit);
extern int bcm_td2_lag_rh_dynamic_size_encode(int unit, int dynamic_size,
        int *encoded_value);
extern int bcm_td2_lag_rh_set(int unit, int tid,
        _esw_trunk_add_info_t *add_info, int num_rh_ports, int *mod_array,
        int *port_array);
extern int bcm_td2_lag_rh_add(int unit, int tid,
        _esw_trunk_add_info_t *add_info, int num_rh_ports, int *mod_array,
        int *port_array, bcm_trunk_member_t *new_member);
extern int bcm_td2_lag_rh_delete(int unit, int tid,
        _esw_trunk_add_info_t *add_info, int num_rh_ports, int *mod_array,
        int *port_array, bcm_trunk_member_t *leaving_member);
extern int bcm_td2_lag_rh_replace(int unit, int tid,
        _esw_trunk_add_info_t *add_info, int num_rh_ports, int *mod_array,
        int *port_array, uint16 num_existing, uint16 *existing_modports,
        uint32 *existing_flags);
extern int bcm_td2_lag_rh_free_resource(int unit, int tid);
extern int bcm_td2_lag_rh_dynamic_size_set(int unit, int tid, int dynamic_size);
extern int bcm_td2_lag_rh_ethertype_set(int unit, uint32 flags,
        int ethertype_count, int *ethertype_array);
extern int bcm_td2_lag_rh_ethertype_get(int unit, uint32 *flags,
        int ethertype_max, int *ethertype_array, int *ethertype_count);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_td2_lag_rh_recover(int unit, int tid,
        trunk_private_t *trunk_info);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_td2_lag_rh_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* FCOE */
extern int bcm_td2_fcoe_init(int unit);
extern int bcm_td2_fcoe_cleanup(int unit);
extern int bcm_td2_fcoe_zone_add(int unit, bcm_fcoe_zone_entry_t *zone);
extern int bcm_td2_fcoe_zone_delete(int unit,
                                    bcm_fcoe_zone_entry_t *zone);
extern int bcm_td2_fcoe_zone_delete_all (int unit);
extern int bcm_td2_fcoe_zone_delete_by_vsan (int unit,
                                             bcm_fcoe_zone_entry_t *zone);
extern int bcm_td2_fcoe_zone_delete_by_sid (int unit,
                                            bcm_fcoe_zone_entry_t *zone);
extern int bcm_td2_fcoe_zone_delete_by_did (int unit,
                                            bcm_fcoe_zone_entry_t *zone);
extern int bcm_td2_fcoe_zone_get (int unit, bcm_fcoe_zone_entry_t *zone);
extern int bcm_td2_fcoe_route_add(int unit, bcm_fcoe_route_t *route);
extern int bcm_td2_fcoe_route_delete(int unit, bcm_fcoe_route_t *route);
extern int bcm_td2_fcoe_route_delete_by_prefix(int unit,
                                               bcm_fcoe_route_t *route);
extern int bcm_td2_fcoe_route_delete_by_interface(int unit,
                                                  bcm_fcoe_route_t *route);
extern int bcm_td2_fcoe_route_delete_all(int unit);
extern int bcm_td2_fcoe_route_find(int unit, bcm_fcoe_route_t *route);
extern int bcm_td2_fcoe_vsan_create(int unit, uint32 options, 
                                    bcm_fcoe_vsan_t *vsan, 
                                    bcm_fcoe_vsan_id_t *vsan_id);
extern int bcm_td2_fcoe_vsan_destroy(int unit,  bcm_fcoe_vsan_id_t vsan_id);
extern int bcm_td2_fcoe_vsan_destroy_all(int unit);
extern int bcm_td2_fcoe_control_vsan_get(int unit,  
                                         uint32 vsan_id, 
                                         bcm_fcoe_vsan_control_t type, 
                                         int *arg);
extern int bcm_td2_fcoe_control_vsan_set(int unit, 
                                         uint32 vsan_id, 
                                         bcm_fcoe_vsan_control_t type, 
                                         int arg);
extern int bcm_td2_fcoe_vsan_get(int unit, uint32 vsan_id, 
                                 bcm_fcoe_vsan_t *vsan);
extern int bcm_td2_fcoe_vsan_set(int unit, uint32 vsan_id, 
                                 bcm_fcoe_vsan_t *vsan);
extern int bcm_td2_fcoe_vsan_stat_attach(int                 unit,
                                         bcm_fcoe_vsan_id_t  vsan,
                                         uint32              stat_counter_id);
extern int bcm_td2_fcoe_vsan_stat_detach(int unit, bcm_fcoe_vsan_id_t  vsan);
extern int bcm_td2_fcoe_vsan_stat_counter_get(
                                      int                  unit, 
                                      int                  sync_mode, 
                                      bcm_fcoe_vsan_id_t   vsan,
                                      bcm_fcoe_vsan_stat_t stat, 
                                      uint32               num_entries, 
                                      uint32               *counter_indexes, 
                                      bcm_stat_value_t     *counter_values);
extern int bcm_td2_fcoe_vsan_stat_counter_set(
                                      int                  unit, 
                                      bcm_fcoe_vsan_id_t   vsan,
                                      bcm_fcoe_vsan_stat_t stat, 
                                      uint32               num_entries, 
                                      uint32               *counter_indexes, 
                                      bcm_stat_value_t     *counter_values);
extern int bcm_td2_fcoe_vsan_stat_id_get(int                  unit,
                                         bcm_fcoe_vsan_id_t   vsan,
                                         bcm_fcoe_vsan_stat_t stat, 
                                         uint32               *stat_counter_id);
extern int bcm_td2_fcoe_route_stat_attach(int                 unit,
                                          bcm_fcoe_route_t    *route,
                                          uint32              stat_counter_id);
extern int bcm_td2_fcoe_route_stat_detach(int unit, bcm_fcoe_route_t *route);
extern int bcm_td2_fcoe_route_stat_counter_get(
                                       int                   unit, 
                                       int                   sync_mode, 
                                       bcm_fcoe_route_t      *route,
                                       bcm_fcoe_route_stat_t stat, 
                                       uint32                num_entries, 
                                       uint32                *counter_indexes, 
                                       bcm_stat_value_t      *counter_values);
extern int bcm_td2_fcoe_route_stat_counter_set (
                                        int                   unit, 
                                        bcm_fcoe_route_t      *route,
                                        bcm_fcoe_route_stat_t stat, 
                                        uint32                num_entries, 
                                        uint32                *counter_indexes, 
                                        bcm_stat_value_t      *counter_values);
extern int bcm_td2_fcoe_route_stat_id_get(
                                        int                   unit,
                                        bcm_fcoe_route_t      *route,
                                        bcm_fcoe_route_stat_t stat, 
                                        uint32                *stat_counter_id);
int bcm_td2_fcoe_intf_config_set(int unit, uint32 flags, bcm_if_t intf, 
                                 bcm_fcoe_intf_config_t *cfg);

int bcm_td2_fcoe_intf_config_get(int unit, uint32 flags, bcm_if_t intf, 
                                 bcm_fcoe_intf_config_t *cfg);
extern int bcm_td2_fcoe_zone_traverse(int unit,
                                      uint32 flags,
                                      bcm_fcoe_zone_traverse_cb trav_fn,
                                      void *user_data);
extern int bcm_td2_fcoe_vsan_traverse(int unit,
                                      uint32 flags,
                                      bcm_fcoe_vsan_traverse_cb trav_fn,
                                      void *user_data);
extern int bcm_td2_fcoe_route_traverse(int unit,
                                       uint32 flags,
                                       bcm_fcoe_route_traverse_cb trav_fn,
                                       void *user_data);
extern int bcm_td2_vp_lag_port_learn_set(int unit, 
                                         bcm_trunk_t tid, uint32 flags);
extern int bcm_td2_vp_lag_port_learn_get(int unit, 
                                         bcm_trunk_t tid, uint32 *flags);

/* QOS */

extern int bcm_td2_qos_init(int unit);
extern int bcm_td2_qos_detach(int unit);
extern int bcm_td2_qos_map_create(int unit, uint32 flags, int *map_id);
extern int bcm_td2_qos_map_destroy(int unit, int map_id);
extern int bcm_td2_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map,
                               int map_id);
extern int bcm_td2_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map,
                                  int map_id);
extern int bcm_td2_qos_port_map_set(int unit, bcm_gport_t port, int ing_map,
                                    int egr_map);
extern int bcm_td2_qos_map_multi_get(int unit, uint32 flags,
                                      int map_id, int array_size, 
                                      bcm_qos_map_t *array, int *array_count);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_td2_qos_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_td2_qos_reinit_scache_len_get(int unit, uint32 *scache_len);
extern int _bcm_td2_qos_reinit_from_hw_state(int unit, soc_mem_t mem, 
                                             soc_field_t field, uint8 map_type, 
                                             SHR_BITDCL *hw_idx_bmp, int hw_idx_bmp_len);
extern int _bcm_td2_qos_unsynchronized_reinit(int unit);
extern int _bcm_td2_qos_extended_reinit(int unit, uint8 **scache_ptr);
extern int _bcm_td2_qos_reinit_hw_profiles_update (int unit);
extern int _bcm_td2_qos_sync(int unit, uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */
extern int _bcm_td2_egr_port_set(int unit, bcm_port_t port,
                                 soc_field_t field, int value);
extern int _bcm_td2_egr_port_get(int unit, bcm_port_t port,
                                 soc_field_t field, int *value);

extern int _bcm_td2_qos_idx2id(int unit, int hw_idx, int type, int *map_id);
extern int bcm_td2_qos_port_map_type_get(int unit, bcm_gport_t port,
                                         uint32 flags, int *map_id);

extern int bcm_td2_fcoe_fc_header_type_set(int unit, uint32 flags, uint8 r_ctl, 
                                           bcm_fcoe_fc_header_type_t hdr_type);
extern int bcm_td2_fcoe_fc_header_type_get(int unit, uint32 flags, uint8 r_ctl, 
                                           bcm_fcoe_fc_header_type_t *hdr_type);

extern int bcm_td2_fcoe_vsan_translate_action_add(
                                    int unit, 
                                    bcm_fcoe_vsan_translate_key_config_t *key, 
                                    bcm_fcoe_vsan_action_set_t *action);
extern int bcm_td2_fcoe_vsan_translate_action_delete(
                                    int unit, 
                                    bcm_fcoe_vsan_translate_key_config_t *key);
extern uint32 _bcm_td2_fcoe_ing_vft_pri_action_encode(
                                    bcm_fcoe_vsan_action_t action);
extern uint32 _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                    bcm_fcoe_vsan_action_t action);
extern uint32 _bcm_td2_fcoe_egr_vft_tag_action_encode(
                                    bcm_fcoe_vsan_action_t action);
extern int bcm_td2_port_dscp_map_set(int unit, bcm_port_t port, int srccp,
                            int mapcp, int prio, int cng);

/* L2 */
extern int _bcm_td2_l2_hit_retrieve(int unit, l2x_entry_t *l2x_entry,
                                 int l2_hw_index);
extern int _bcm_td2_l2_hit_range_retrieve(int unit, soc_mem_t mem, int idx_min,
                                          int idx_max, uint32 *l2_tbl_chnk);

extern int
_bcm_td2_port_enqueue_set(int unit, bcm_port_t gport,
                                    int enable);

extern int
_bcm_td2_cosq_port_queue_state_check(int unit, bcm_port_t gport);
extern int
_bcm_td2_cosq_localport_resolve(int unit, bcm_gport_t gport,
                               bcm_port_t *local_port);
extern int
_bcm_td2_cosq_hw_idx_get(int unit, int max_count,
                         bcm_gport_t *port, int *hw_idx, int* count);


extern int
_bcm_td2_port_enqueue_get(int unit, bcm_port_t gport,
                                            int *enable);
/* tag/pri actions for ING_VLAN_TAG_ACTION_PROFILE table */
#define _BCM_TD2_ING_TAG_ACTION_NONE              0 /* do not modify */
#define _BCM_TD2_ING_TAG_CFI_ACTION_NONE          0 /* do not modify */
#define _BCM_TD2_ING_TAG_PRI_ACTION_NONE          0 /* do not modify */
#define _BCM_TD2_EGR_TAG_ACTION_NONE              0 /* do not modify */
#define _BCM_TD2_EGR_TAG_CFI_ACTION_NONE          0 /* do not modify */
#define _BCM_TD2_EGR_TAG_PRI_ACTION_NONE          0 /* do not modify */

/* actions for ING VFT PRI (ING_VLAN_TAG_ACTION_PROFILE) */
#define _BCM_TD2_ING_VFT_PRI_ACTION_NONE          0
#define _BCM_TD2_ING_VFT_PRI_ACTION_USE_INNER_PRI 1
#define _BCM_TD2_ING_VFT_PRI_ACTION_USE_OUTER_PRI 2
#define _BCM_TD2_ING_VFT_PRI_ACTION_REPLACE       3  /* use VLAN_XLATE.XLATE::
                                                        FCOE_VSAN_PRI */

/* actions for EGR VFT PRI (EGR_VLAN_TAG_ACTION_PROFILE) */
#define _BCM_TD2_EGR_VFT_PRI_ACTION_NONE              0
#define _BCM_TD2_EGR_VFT_PRI_ACTION_USE_INTERNAL_IPRI 1
#define _BCM_TD2_EGR_VFT_PRI_ACTION_USE_INTERNAL_OPRI 2
#define _BCM_TD2_EGR_VFT_PRI_ACTION_REPLACE           3  /* use EVXLT.XLATE::
                                                            FCOE_VSAN_PRI */

/* actions for EGR VFT Tag (EGR_VLAN_TAG_ACTION_PROFILE) */
#define _BCM_TD2_EGR_VFT_TAG_ACTION_NONE              0
#define _BCM_TD2_EGR_VFT_TAG_ACTION_USE_INTERNAL_IVID 1
#define _BCM_TD2_EGR_VFT_TAG_ACTION_USE_INTERNAL_OVID 2
#define _BCM_TD2_EGR_VFT_TAG_ACTION_DELETE            3
#define _BCM_TD2_EGR_VFT_TAG_ACTION_USE_INTERNAL_VSAN 4
#define _BCM_TD2_EGR_VFT_TAG_ACTION_REPLACE           5  /* use EVXLT.XLATE::
                                                            FCOE_VSAN_ID */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_td2_fcoe_sync(int unit);
extern int _bcm_td2_fcoe_reinit(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_td2_fcoe_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
extern int soc_td2x_ser_dlb_parity_set(int unit, int enable);

/*
 * 8 Priority Groups supported.
 */
#define TD2_PRIOROTY_GROUP_ID_MIN 0
#define TD2_PRIOROTY_GROUP_ID_MAX 7
/*
 * Inpur priority range.
 * PFC : 0-7
 * SAFC : 0-15
 */
#define TD2_PFC_INPUT_PRIORITY_MIN  0
#define TD2_PFC_INPUT_PRIORITY_MAX  7
#define TD2_SAFC_INPUT_PRIORITY_MIN TD2_PFC_INPUT_PRIORITY_MIN
#define TD2_SAFC_INPUT_PRIORITY_MAX 15

extern void _bcm_td2_qos_fcoe_get(int unit, int array_size,
                                  int *map_ids_array, int *flags_array,
                                  int *array_count);
extern int _bcm_td2_qos_multi_get(int unit, int array_size,
                                  int *map_ids_array, int *flags_array,
                                  int *array_count);
extern int bcm_td2_switch_ser_log_info_get(int unit,
                                  int id, bcm_switch_ser_log_info_t *info);

extern int bcm_td2_switch_ser_error_stat_get(int unit,
                                  bcm_switch_ser_error_stat_type_t stat_type,
                                  uint32 *value);

extern int bcm_td2_switch_ser_error_stat_clear(int unit,
                                  bcm_switch_ser_error_stat_type_t stat_type);
#endif /* BCM_TRIDENT2_SUPPORT  */

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
#if defined(INCLUDE_L3)

/* Port Extender */
extern int bcm_td2_extender_forward_add(int unit,
        bcm_extender_forward_t *extender_forward_entry);
extern int bcm_td2_extender_forward_delete(int unit,
        bcm_extender_forward_t *extender_forward_entry);
extern int bcm_td2_extender_forward_delete_all(int unit);
extern int bcm_td2_extender_forward_get(int unit,
        bcm_extender_forward_t *extender_forward_entry);
extern int bcm_td2_extender_forward_traverse(int unit,
        bcm_extender_forward_traverse_cb cb, void *user_data);
#endif /* INCLUDE_L3  */
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */

#endif /* !_BCM_INT_TRIDENT2_H_ */
