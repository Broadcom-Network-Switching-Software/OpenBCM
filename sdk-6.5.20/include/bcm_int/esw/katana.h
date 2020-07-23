/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        katana.h
 * Purpose:     Function declarations for Trident  bcm functions
 */

#ifndef _BCM_INT_KATANA_H_
#define _BCM_INT_KATANA_H_
#if defined(BCM_KATANA_SUPPORT)
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/field.h>
#include <soc/tnl_term.h>
#include <bcm/qos.h>
#include <bcm/trill.h>
#include <bcm/field.h>
#include <bcm_int/esw/bfd.h>
#if defined(INCLUDE_PTP)
#include <bcm_int/esw/ptp.h>
#endif

typedef enum {
    _BCM_KT_COSQ_NODE_LEVEL_ROOT,
    _BCM_KT_COSQ_NODE_LEVEL_L0,
    _BCM_KT_COSQ_NODE_LEVEL_L1,
    _BCM_KT_COSQ_NODE_LEVEL_L2,
    _BCM_KT_COSQ_NODE_LEVEL_MAX
}_bcm_kt_cosq_node_level_t;

typedef enum {
    _BCM_KT_NODE_UNKNOWN = 0,
    _BCM_KT_NODE_SUBSCRIBER_GPORT_ID
} _bcm_kt_node_type_e;

typedef struct _bcm_kt_cosq_node_s{
    struct _bcm_kt_cosq_node_s *parent;
    struct _bcm_kt_cosq_node_s *sibling;
    struct _bcm_kt_cosq_node_s *child;
    bcm_gport_t gport;
    SHR_BITDCL *cosq_map;
    int in_use;
    int wrr_in_use;
    int base_index;
    int numq;
    int hw_index;
    _bcm_kt_cosq_node_level_t level;
    int cosq_attached_to;
    int num_child;
    int first_child;
    _bcm_kt_node_type_e type;

}_bcm_kt_cosq_node_t;

extern int bcm_kt_port_downsizer_chk_reinit(int unit);
extern int bcm_kt_cosq_port_pps_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int pps);
extern int bcm_kt_cosq_port_pps_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *pps);
extern int bcm_kt_cosq_port_burst_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int burst);
extern int bcm_kt_cosq_port_burst_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int *burst);
extern int
bcm_kt_cosq_subscriber_qid_set(int unit,
         bcm_gport_t *gport,
         int queue_id);

extern int 
bcm_kt_sw_hw_queue(int unit, int *queue_array);
extern int
_bcm_kt_intf_ref_cnt_increment(int unit, uint32 queue_id, int count);
extern int
_bcm_kt_intf_ref_cnt_decrement(int unit, uint32 queue_id, int count);
#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_katana_init(int unit, _field_control_t *fc);
#endif /* BCM_FIELD_SUPPORT */

#ifdef INCLUDE_L3
extern int bcm_kt_mpls_tunnel_switch_traverse(int unit,
                                   bcm_mpls_tunnel_switch_traverse_cb cb,
                                   void *user_data);
extern int bcm_kt_mpls_tunnel_switch_get(int unit, bcm_mpls_tunnel_switch_t *info);
extern int bcm_kt_mpls_tunnel_switch_delete_all(int unit);
extern  int bcm_kt_mpls_tunnel_switch_delete(int unit, bcm_mpls_tunnel_switch_t *info);
extern int bcm_kt_mpls_tunnel_switch_add(int unit, bcm_mpls_tunnel_switch_t *info);

extern bcm_error_t bcm_kt_mpls_port_stat_attach(
                   int         unit,
                   bcm_vpn_t   vpn,
                   bcm_gport_t port,
                   uint32      stat_counter_id);
extern bcm_error_t bcm_kt_mpls_port_stat_detach(
                   int         unit, 
                   bcm_vpn_t   vpn,
                   bcm_gport_t port);
extern bcm_error_t bcm_kt_mpls_port_stat_counter_get(
                   int                  unit,
                   int                  sync_mode,
                   bcm_vpn_t            vpn,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values);
extern bcm_error_t bcm_kt_mpls_port_stat_counter_set(
                   int                  unit,
                   bcm_vpn_t            vpn,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values);
extern bcm_error_t bcm_kt_mpls_port_stat_id_get(
                   int                  unit,
                   bcm_vpn_t            vpn,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               *stat_counter_id);
extern bcm_error_t bcm_kt_mpls_label_stat_attach(
                   int              unit,
                   bcm_mpls_label_t label,
                   bcm_gport_t      port,
                   uint32           stat_counter_id);
extern bcm_error_t bcm_kt_mpls_label_stat_detach(
                   int              unit,
                   bcm_mpls_label_t label,
                   bcm_gport_t      port);
extern bcm_error_t bcm_kt_mpls_label_stat_counter_get(
                   int                  unit,
                   int                  sync_mode,
                   bcm_mpls_label_t     label,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values);
extern bcm_error_t bcm_kt_mpls_label_stat_counter_set(
                   int                  unit,
                   bcm_mpls_label_t     label,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values);
extern bcm_error_t bcm_kt_mpls_label_stat_id_get(
                   int                  unit,
                   bcm_mpls_label_t     label,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               *stat_counter_id);
extern int bcm_kt_mpls_label_stat_enable_set(
           int              unit,
           bcm_mpls_label_t label,
           bcm_gport_t      port,
           int              enable);
extern int bcm_kt_mpls_label_stat_get(
           int              unit, 
           int              sync_mode,
           bcm_mpls_label_t label, 
           bcm_gport_t      port,
           bcm_mpls_stat_t  stat,
           uint64           *val);
extern int bcm_kt_mpls_label_stat_get32(
           int              unit,
           int              sync_mode,
           bcm_mpls_label_t label, 
           bcm_gport_t      port, 
           bcm_mpls_stat_t  stat,
           uint32           *val);
extern int bcm_kt_mpls_label_stat_clear(
           int              unit,
           bcm_mpls_label_t label, 
           bcm_gport_t      port, 
           bcm_mpls_stat_t  stat);
extern int _bcm_kt_egress_object_p2mp_set(
          int             unit,
          bcm_multicast_t mc_group,
          int             flag,
          uint32          value);

#endif /* INCLUDE_L3 */

typedef enum {
    _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
    _BCM_KT_COSQ_INDEX_STYLE_WRED,
    _BCM_KT_COSQ_INDEX_STYLE_SCHEDULER,
    _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_KT_COSQ_INDEX_STYLE_PERQ_XMT,
    _BCM_KT_COSQ_INDEX_STYLE_QUEUE_REPLICATION
}_bcm_kt_cosq_index_style_t;
extern int
bcm_kt_is_uc_queue(int unit, int queue_id, int *is_ucq);
extern int bcm_kt_cosq_init(int unit);
extern int bcm_kt_cosq_detach(int unit, int software_state_only);
extern int bcm_kt_cosq_config_set(int unit, int numq);
extern int bcm_kt_cosq_config_get(int unit, int *numq);
extern int bcm_kt_cosq_mapping_set(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_kt_cosq_mapping_get(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t *cosq);
extern int bcm_kt_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                      int mode, const int *weights, int delay);
extern int bcm_kt_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                      int *mode, int *weights, int *delay);
extern int bcm_kt_cosq_sched_weight_max_get(int unit, int mode,
                                            int *weight_max);
extern int bcm_kt_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 min_quantum,
                                          uint32 max_quantum,
                                          uint32 burst_quantum,
                                          uint32 flags);
extern int bcm_kt_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 *min_quantum,
                                          uint32 *max_quantum,
                                          uint32 *burst_quantum,
                                          uint32 *flags);
extern int bcm_kt_cosq_discard_set(int unit, uint32 flags);
extern int bcm_kt_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_kt_cosq_discard_port_set(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int drop_start, int drop_slope,
                                        int average_time);
extern int bcm_kt_cosq_discard_port_get(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int *drop_start, int *drop_slope,
                                        int *average_time);
extern int bcm_kt_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                                 uint32 flags, bcm_gport_t *gport);
extern int bcm_kt_cosq_gport_delete(int unit, bcm_gport_t gport);
extern int bcm_kt_cosq_gport_get(int unit, bcm_gport_t gport,
                                 bcm_gport_t *port, int *numq, uint32 *flags);
extern int bcm_kt_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                                      void *user_data);
extern int bcm_kt_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);
extern int bcm_kt_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       int *mode, int *weight);
extern int bcm_kt_cosq_gport_attach(int unit, bcm_gport_t sched_gport,
                                    bcm_gport_t input_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_kt_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                                    bcm_gport_t input_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_kt_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max, uint32 flags);
extern int bcm_kt_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max,
                                           uint32 *flags);
extern int bcm_kt_cosq_gport_child_node_bandwidth_set(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max,
                                           uint32 flags);
extern int bcm_kt_cosq_gport_child_node_bandwidth_get(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max,
                                           uint32 *flags);
extern int bcm_kt_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 kbits_burst_min,
                                                 uint32 kbits_burst_max);
extern int bcm_kt_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 *kbits_burst_min,
                                                 uint32 *kbits_burst_max);
extern int bcm_kt_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                                        bcm_gport_t *input_gport,
                                        bcm_cos_queue_t *cosq);
extern int bcm_kt_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_kt_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_kt_cosq_stat_set(int unit, bcm_gport_t port, 
                                bcm_cos_queue_t cosq,
                                bcm_cosq_stat_t stat, uint64 value);
extern int bcm_kt_cosq_stat_get(int unit, bcm_gport_t port, 
                                bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, 
                                int sync_mode, uint64 *value);
extern int bcm_kt_cosq_gport_stat_set(int unit, bcm_gport_t port, 
                                bcm_cos_queue_t cosq,
                                bcm_cosq_gport_stats_t stat, uint64 value);
extern int bcm_kt_cosq_gport_stat_get(int unit, int sync_mode, 
                                      bcm_gport_t port, 
                                      bcm_cos_queue_t cosq, 
                                      bcm_cosq_gport_stats_t stat, 
                                      uint64 *value);
extern int bcm_kt_cosq_port_get(int unit, int queue_id, bcm_port_t *port);
extern int _bcm_kt_cosq_index_resolve(int unit, bcm_port_t port,
                           bcm_cos_queue_t cosq,
                           _bcm_kt_cosq_index_style_t style,
                           bcm_port_t *local_port, int *index, int *count);
extern int bcm_kt_port_rate_egress_set(int unit, bcm_port_t port,
                           uint32 kbits_sec, uint32 kbits_burst, uint32 mode);
extern int bcm_kt_port_rate_egress_get(int unit, bcm_port_t port,
                           uint32 *kbits_sec, uint32 *kbits_burst, uint32 *mode);
extern int bcm_kt_port_pps_rate_egress_set(int unit, bcm_port_t port,
                                           uint32 pps, uint32 burst);
extern int bcm_kt_cosq_drop_status_enable_set(int unit, bcm_port_t port,
                                              int enable);
extern int bcm_kt_cosq_control_set(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_control_t type, int arg);

extern int bcm_kt_cosq_control_get(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_control_t type, int *arg);
extern int _bcm_kt_cosq_port_resolve(int unit, bcm_gport_t gport,
                                     bcm_module_t *modid, bcm_port_t *port,
                                     bcm_trunk_t *trunk_id, int *id,
                                     int *qnum);
extern int bcm_kt_dump_port_lls(int unit, bcm_port_t port);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_kt_cosq_sync(int unit);
extern int _bcm_kt_ipmc_hw_queue_info_scache_size_get(int unit, uint32 *size);
extern int _bcm_kt_ipmc_hw_queue_info_sync(int unit, uint8 **scache_ptr);
extern int _bcm_kt_ipmc_hw_queue_info_recover(int unit, uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_kt_cosq_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#if defined (INCLUDE_L3)

#if defined(INCLUDE_BFD)
extern int bcmi_kt_bfd_init(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcmi_kt_bfd_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcmi_kt_bfd_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#endif /* INCLUDE_BFD */

#define BCM_KT_DEFIP_PAIR128_HASH_SZ            (0x6)
#define BCM_KT_DEFIP_PAIR128(_unit_)             l3_kt_defip_pair128[(_unit_)]
#define BCM_KT_DEFIP_PAIR128_ARR(_unit_)         BCM_KT_DEFIP_PAIR128((_unit_))->entry_array
#define BCM_KT_DEFIP_PAIR128_IDX_MAX(_unit_)     BCM_KT_DEFIP_PAIR128((_unit_))->idx_max
#define BCM_KT_DEFIP_PAIR128_USED_COUNT(_unit_)  BCM_KT_DEFIP_PAIR128((_unit_))->used_count
#define BCM_KT_DEFIP_PAIR128_TOTAL(_unit_)       BCM_KT_DEFIP_PAIR128((_unit_))->total_count
#define BCM_KT_DEFIP_PAIR128_URPF_OFFSET(_unit_) BCM_KT_DEFIP_PAIR128((_unit_))->urpf_offset
#define BCM_KT_DEFIP_PAIR128_ENTRY_SET(_unit_, _idx_, _plen_, _hash_)          \
            BCM_KT_DEFIP_PAIR128_ARR((_unit_))[(_idx_)].prefix_len = (_plen_); \
            BCM_KT_DEFIP_PAIR128_ARR((_unit_))[(_idx_)].entry_hash = (_hash_)


typedef struct _bcm_l3_defip_pair128_entry_s {
    uint8  prefix_len; /* Route entry  prefix length.*/
    uint8  entry_hash; /* Route entry key hash.      */
} _bcm_l3_defip_pair128_entry_t;

typedef struct _bcm_l3_defip_pair128_table_s {
    _bcm_l3_defip_pair128_entry_t *entry_array; /* Cam entries array.      */
    uint16 idx_max;                   /* Last cam entry index.             */
    uint16 used_count;                /* Used cam entry count.             */
    uint16 total_count;               /* Total number of available entries.*/
    uint16 urpf_offset;               /* Src lookup start offset.          */
} _bcm_l3_defip_pair128_table_t;


extern int _bcm_kt_defip_pair128_init(int unit);
extern int _bcm_kt_defip_pair128_deinit(int unit);
extern int _bcm_kt_defip_pair128_get(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                  int *nh_ecmp_idx); 
extern int _bcm_kt_defip_pair128_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                  int nh_ecmp_idx);
extern int _bcm_kt_defip_pair128_delete(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_kt_defip_pair128_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);

extern int bcm_kt_ipmc_repl_detach(int unit);
extern int _bcm_kt_init_check(int unit, int ipmc_id);
extern int _bcm_kt_ipmc_subscriber_egress_intf_set(int unit, int mc_index,
                                       bcm_port_t port, int if_count,
                                       bcm_if_t *if_array,
                                       bcm_gport_t queue,
                                       int l3,
                                       int *queue_count,
                                       int *q_count_increased);
extern int _bcm_kt_ipmc_subscriber_egress_intf_get(int unit, int mc_index,
                                       int port_count,
                                       bcm_gport_t *port_array,
                                       bcm_if_t *encap_id_array,
                                       bcm_gport_t *subscriber_queue_array,
                                       int *if_count);
extern int _bcm_kt_ipmc_subscriber_egress_intf_delete(int unit, int ipmc_id,
                                       bcm_port_t port,
                                       int nh_index,
                                       bcm_gport_t subscriber_queue,
                                       int *queue_count,
                                       int *q_count_decreased);
extern int _bcm_kt_ipmc_mmu_mc_remap_ptr(int unit, int ipmc_id,
                                   int *remap_id, int set);
extern int _bcm_kt_ipmc_set_remap_group(int unit, int mc_index,
                                       bcm_port_t port_out,
                                       int count);
extern int _bcm_kt_get_subscriber_queue_entries(int unit, int extq_ptr, 
                                     SHR_BITDCL *q_vec, 
                                     uint32  *q_repl_ptr, 
                                     int *q_num);
extern void _bcm_kt_ipmc_port_ext_queue_count_increment(int unit,
                                     int ipmc_id,
                                     bcm_port_t port);
extern void _bcm_kt_ipmc_port_ext_queue_count_decrement(int unit,
                                     int ipmc_id,
                                     bcm_port_t port);

extern int _bcm_kt_defip_pair128_used_count_get(int unit);
extern int _bcm_kt_l3_info_dump(int unit);
extern int bcmi_kt_extended_queue_config_destroy(int unit, bcm_if_t intf);
extern int _bcm_kt_ipmc_subscriber_egress_intf_delete_all(int unit, int ipmc_id, 
                                                 int port_count,
                                                 bcm_port_t *port_array,
                                                 int *encap_array,
                                                 bcm_if_t *q_array,
                                                 int *q_count_decreased);

extern int _bcm_kt_hw_to_sw_queue(int unit, int *queue_array, bcm_port_t *port);
extern void _bcm_kt_sw_to_hw_queue_clear(int unit);
#endif

#if defined(INCLUDE_PTP)
extern int bcm_kt_ptp_init(int unit);
extern int bcm_kt_ptp_detach(int unit);
extern int bcm_kt_ptp_stack_create(int unit,
        bcm_ptp_stack_info_t *ptp_info);
extern int bcm_kt_ptp_firmware_load(int unit,
    bcm_ptp_stack_id_t ptp_id);
extern int bcm_kt_ptp_time_format_set( int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_time_type_t type);
extern int bcm_kt_ptp_cb_register(int unit, 
    bcm_ptp_cb_types_t cb_types, 
    bcm_ptp_cb cb, 
    void *user_data);
extern int bcm_kt_ptp_cb_unregister(int unit, 
    bcm_ptp_cb_types_t cb_types, 
    bcm_ptp_cb cb);
extern int bcm_kt_ptp_clock_create(int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    bcm_ptp_clock_info_t *clock_info);

#endif

extern int _bcm_field_kt_qualify_PacketRes(int               unit,
                                            bcm_field_entry_t entry,
                                            uint32            *data,
                                            uint32            *mask
                                            );
extern int _bcm_field_kt_qualify_PacketRes_get(int               unit,
                                                bcm_field_entry_t entry,
                                                uint32            *data,
                                                uint32            *mask
                                                );
/*
 * Function:
 *     _bcm_field_fabricQueue_action_node_increment_ref_Count
 * Purpose:
 *     Increment the referecne count of the fabric node if it is existing
 * Parameters:
 *     unit           - (IN) BCM device number
 *     queue_index    - (IN) cosq queue index
 *     profile_indexx - (IN) QoS map profile index
 * Returns:
 *     BCM_E_XXX
 */
extern int _bcm_field_fabricQueue_action_node_increment_ref_Count (int unit,
                          uint32 queue_index, uint32 profile_index);
/*
 * Function:
 *     _bcm_field_fabricQueue_action_node_add
 * Purpose:
 *     add the fabric node to the linked list
 * Parameters:
 *     unit           - (IN) BCM device number
 *     queue_index    - (IN) cosq queue index
 *     profile_indexx - (IN) QoS map profile index
 * Returns:
 *     BCM_E_XXX
 */
extern int _bcm_field_fabricQueue_action_node_add (int unit,
                            uint32 queue_index, uint32 profile_index);
/*
 * Function:
 *     _bcm_field_fabricQueue_action_node_delete
 * Purpose:
 *     delete the node from linkedlist if reference count is
 *     1. else decrement the reference count.
 * Parameters:
 *     unit           - (IN) BCM device number
 *     queue_index    - (IN) cosq queue index
 *     profile_indexx - (IN) QoS map profile index
 *     referenceCount - (OUT) reference count of the node
 * Returns:
 *     BCM_E_XXX
 */
extern int _bcm_field_fabricQueue_action_node_delete (int unit,
                          uint32 queue_index, uint32 profile_index,
                          int *referenceCount);
extern int _bcm_kt_extended_queue_config(int unit, uint32 options,
                                         uint32 flags, int queue_id,
                                         int profile_index, bcm_if_t *intf);
extern int _bcm_kt_extended_queue_qos_profile_get(int unit,
                                                  bcm_if_t intf,
                                                  int *profile_index);
#endif /* BCM_KATANA_SUPPORT */
#endif  /* !_BCM_INT_KATANA_H_ */

