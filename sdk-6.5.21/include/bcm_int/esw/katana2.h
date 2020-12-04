/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        katana2.h
 * Purpose:     Function declarations for Trident  bcm functions
 */

#ifndef _BCM_INT_KATANA2_H_
#define _BCM_INT_KATANA2_H_
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm/oob.h>
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/field.h>
#include <bcm/qos.h>
#include <bcm/failover.h>
#include <bcm_int/esw/cosq.h>
typedef  enum bcm_cosq_flush_type_e {
    bcmCosqFlushTypeQueue=0,
    bcmCosqFlushTypePort=1
}bcm_cosq_flush_type_t;

typedef enum {
    _BCM_KT2_COSQ_NODE_LEVEL_ROOT,
    _BCM_KT2_COSQ_NODE_LEVEL_S0,
    _BCM_KT2_COSQ_NODE_LEVEL_S1,
    _BCM_KT2_COSQ_NODE_LEVEL_L0,
    _BCM_KT2_COSQ_NODE_LEVEL_L1,
    _BCM_KT2_COSQ_NODE_LEVEL_L2,
    _BCM_KT2_COSQ_NODE_LEVEL_MAX
}_bcm_kt2_cosq_node_level_t;

typedef enum {
    _BCM_KT2_NODE_UNKNOWN = 0,
    _BCM_KT2_NODE_UCAST,
    _BCM_KT2_NODE_MCAST,
    _BCM_KT2_NODE_VOQ,
    _BCM_KT2_NODE_VLAN_UCAST,
    _BCM_KT2_NODE_VM_UCAST,
    _BCM_KT2_NODE_SERVICE_UCAST,
    _BCM_KT2_NODE_SCHEDULER,
    _BCM_KT2_NODE_SUBSCRIBER_GPORT_ID,
    _BCM_KT2_NODE_VOQ_ID,
    _BCM_KT2_NODE_SCHEDULER_WFQ
} _bcm_kt2_node_type_e;

typedef struct _bcm_kt2_cosq_node_s{
    struct _bcm_kt2_cosq_node_s *parent;
    struct _bcm_kt2_cosq_node_s *sibling;
    struct _bcm_kt2_cosq_node_s *child;
    bcm_gport_t gport;
    SHR_BITDCL *cosq_map;
    int in_use;
    int wrr_in_use;
    int wrr_mode;
    int base_index;
    int mc_base_index;
    int numq;
    int hw_index;
    _bcm_kt2_cosq_node_level_t level;
    _bcm_kt2_node_type_e type;
    int cosq_attached_to;
    int num_child;
    int first_child;
    int linkphy_enabled;
    int subport_enabled;
    bcm_port_t local_port;
    int wred_disabled;
    int burst_calculation_enable;
 
    /* DPVOQ specific information */
    bcm_module_t remote_modid;
    bcm_module_t fabric_modid;
    bcm_port_t   remote_port;
    bcm_pbmp_t   fabric_pbmp;
}_bcm_kt2_cosq_node_t;

extern int
bcm_kt2_cosq_subscriber_qid_set(int unit,
        bcm_gport_t *gport,
        int queue_id); 
extern int
_bcm_kt2_subscriber_sw_qid_set(int unit,
                               bcm_gport_t *gport,
                               int queue_id);
extern int
bcm_kt2_is_uc_queue(int unit, int queue_id, int *is_ucq);
extern int bcm_kt2_port_cfg_init(int, bcm_port_t, bcm_vlan_data_t *);
extern int
bcm_katana2_cosq_reconfigure_flexio(int uint, bcm_port_t port, int lanes);
extern int bcm_kt2_cosq_port_flush(int unit, 
                                   bcm_port_t port, 
                                   bcm_cosq_flush_type_t type);

extern int
bcm_kt2_sw_hw_queue(int unit, int *queue_array);
extern int
_bcm_kt2_intf_ref_cnt_increment(int unit, uint32 queue_id, int count);
extern int
_bcm_kt2_intf_ref_cnt_decrement(int unit, uint32 queue_id, int count);

#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_kt2_init(int unit, _field_control_t *fc);
extern int _bcm_field_kt2_qual_tcam_key_mask_get(int unit,
                                                 _field_entry_t *f_ent,
                                                 _field_tcam_t  *tcam
                                                 );
extern int _bcm_field_kt2_qual_tcam_key_mask_set(int unit,
                                                 _field_entry_t *f_ent,
                                                 unsigned       validf
                                                 );
extern int _bcm_field_kt2_qualify_class (int unit, 
                                         bcm_field_entry_t entry, 
                                         bcm_field_qualify_t qual, 
                                         uint32 *data, 
                                         uint32 *mask 
                                        );
extern int _bcm_field_kt2_qualify_class_get (int unit,
                                             bcm_field_entry_t   entry,
                                             bcm_field_qualify_t qual,
                                             uint32              *data,
                                             uint32              *mask
                                            );
extern int _bcm_field_kt2_qualify_PacketRes(int               unit,
                                            bcm_field_entry_t entry,
                                            uint32            *data,
                                            uint32            *mask
                                            );
extern int _bcm_field_kt2_qualify_PacketRes_get(int               unit,
                                                bcm_field_entry_t entry,
                                                uint32            *data,
                                                uint32            *mask
                                                );
extern int soc_katana2_port_lanes_set_post_operation(int unit, bcm_port_t port);
extern int _bcm_kt2_port_lanes_set_post_operation(int unit, bcm_port_t port);
extern int _bcm_kt2_port_lanes_set(int unit, bcm_port_t port, int value);
extern int _bcm_kt2_port_lanes_get(int unit, bcm_port_t port, int *value);
extern int _bcm_kt2_cosq_port_ext_buf_set(int unit, bcm_port_t port, int enable);
extern int _bcm_kt2_cosq_port_ext_buf_get(int unit, bcm_port_t port, int *enable);
extern int _bcm_kt2_port_hw_info_display(int unit, bcm_port_t port);
extern int _bcm_kt2_port_sw_info_display(int unit, bcm_port_t port);
#ifdef BCM_WARM_BOOT_SUPPORT /* BCM_WARM_BOOT_SUPPORT */
extern int _bcm_field_kt2_loopback_type_sel_recover(int unit,
                                                    int slice_idx,
                                                    char *fp_global_mask_tcam_buf,
                                                    _field_stage_t *stage_fc,
                                                    int intraslice,
                                                    int8 *loopback_type_sel);
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_FIELD_SUPPORT */

extern int bcm_kt2_subport_init(int unit);
extern int bcm_kt2_subport_cleanup(int unit);
extern int bcm_kt2_subport_group_create(int unit,
                                     bcm_subport_group_config_t *config,
                                     bcm_gport_t *group);
extern int bcm_kt2_subport_group_destroy(int unit, bcm_gport_t group);
extern int bcm_kt2_subport_group_get(int unit, bcm_gport_t group,
                                     bcm_subport_group_config_t *config);
extern int bcm_kt2_subport_group_traverse(int unit, bcm_gport_t group,
                                     bcm_subport_port_traverse_cb cb,
                                     void *user_data);
extern int bcm_kt2_subport_port_add(int unit, bcm_subport_config_t *config,
                                   bcm_gport_t *port);
extern int bcm_kt2_subport_port_delete(int unit, bcm_gport_t port);
extern int bcm_kt2_subport_port_get(int unit, bcm_gport_t port,
                                   bcm_subport_config_t *config);
extern int bcm_kt2_subport_port_traverse(int unit,
                                      bcm_subport_port_traverse_cb cb,
                                        void *user_data);
extern int bcm_kt2_subport_group_linkphy_config_get(int unit, 
                          bcm_gport_t group, 
                          bcm_subport_group_linkphy_config_t *linkphy_config);
extern int bcm_kt2_subport_group_linkphy_config_set(int unit,
                          bcm_gport_t group,
                          bcm_subport_group_linkphy_config_t *linkphy_config);
extern int bcm_kt2_subport_port_stat_set(int unit, 
                          bcm_gport_t port, 
                          int stream_id, 
                          bcm_subport_stat_t stat_type, 
                          uint64 val);
extern int bcm_kt2_subport_port_stat_get(
                          int unit, 
                          bcm_gport_t port, 
                          int stream_id, 
                          bcm_subport_stat_t stat_type, 
                          uint64 *val);
extern int bcm_kt2_subport_linkphy_rx_error_register (
        int unit,
        bcm_subport_linkphy_rx_errors_t errors,
        bcm_subport_linkphy_rx_error_reg_info_t *reg_info,
        void *user_data);
extern int bcm_kt2_subport_linkphy_rx_error_unregister (
        int unit,
        bcm_subport_linkphy_rx_errors_t errors);
extern int bcmi_kt2_subport_gport_modport_get(int unit,
            bcm_gport_t subport_gport, bcm_module_t *module, bcm_port_t *port);
extern int bcm_kt2_subport_port_stat_show(int unit, uint32 flag, int sid);
extern int bcm_kt2_subport_group_resolve(int unit,
                              bcm_gport_t subport_group_gport,
                              bcm_module_t *modid, bcm_port_t *port,
                              bcm_trunk_t *trunk_id, int *id);
extern int bcm_kt2_subport_port_resolve(int unit,
                              bcm_gport_t subport_port_gport,
                              bcm_module_t *modid, bcm_port_t *port,
                              bcm_trunk_t *trunk_id, int *id);
extern int bcm_kt2_subport_pp_port_subport_info_get(int unit,
                              bcm_port_t pp_port,
                              _bcm_kt2_subport_info_t *subport_info);
extern int bcm_kt2_subport_pp_port_s1_index_get(int unit,
                                                bcm_port_t pp_port,
                                                int *s1_index);
extern int bcm_kt2_subport_counter_init(int unit);
extern int bcm_kt2_subport_counter_cleanup(int unit);

extern int bcm_kt2_subport_egr_subtag_dot1p_map_add(int unit,
                                        bcm_qos_map_t *map);
extern int bcm_kt2_subport_egr_subtag_dot1p_map_delete(int unit,
                                        bcm_qos_map_t *map);
extern int bcm_kt2_subport_subtag_port_tpid_set(int unit,
                                 bcm_gport_t gport, uint16 tpid);
extern int bcm_kt2_subport_subtag_port_tpid_get(int unit,
                                 bcm_gport_t gport, uint16 *tpid);
extern int bcm_kt2_subport_subtag_port_tpid_delete(int unit,
                                 bcm_gport_t gport, uint16 tpid);
extern int bcm_kt2_port_control_subtag_status_set(int unit,
                                 bcm_port_t port, int value);
extern int bcm_kt2_port_control_subtag_status_get(int unit,
                                 bcm_port_t port, int *value);
extern int bcm_kt2_port_control_linkphy_status_set(int unit,
                                 bcm_port_t port, int value);
extern int bcm_kt2_port_control_linkphy_status_get(int unit,
                                 bcm_port_t port, int *value);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_kt2_subport_wb_scache_alloc(int unit, uint16 version,
                                           int *alloc_sz);
extern int bcm_kt2_subport_wb_sync(int unit, uint16 version,
                                   uint8 **scache_ptr);
extern int bcm_kt2_subport_wb_recover(int unit, uint16 version,
                                      uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int bcm_kt2_oam_init(int unit);

extern int bcm_kt2_oam_detach(int unit);

extern int bcm_kt2_oam_group_create(int unit, bcm_oam_group_info_t *group_info);

extern int bcm_kt2_oam_group_get(int unit, bcm_oam_group_t group,
                                 bcm_oam_group_info_t *group_info);

extern int bcm_kt2_oam_group_destroy(int unit, bcm_oam_group_t group);

extern int bcm_kt2_oam_group_destroy_all(int unit);

extern int bcm_kt2_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb,
                                      void *user_data);

extern int bcm_kt2_oam_endpoint_create(int unit,
                                       bcm_oam_endpoint_info_t *endpoint_info);

extern int bcm_kt2_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint,
                                    bcm_oam_endpoint_info_t *endpoint_info);

extern int bcm_kt2_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint);

extern int bcm_kt2_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group);

extern int bcm_kt2_oam_endpoint_traverse(int unit, bcm_oam_group_t group,
                                         bcm_oam_endpoint_traverse_cb cb,
                                         void *user_data);

extern int bcm_kt2_oam_event_register(int unit, 
                                      bcm_oam_event_types_t event_types,
                                      bcm_oam_event_cb cb, void *user_data);

extern int bcm_kt2_oam_event_unregister(int unit, 
                                      bcm_oam_event_types_t event_types,
                                      bcm_oam_event_cb cb);

extern int bcm_kt2_oam_endpoint_action_set(int unit, bcm_oam_endpoint_t id, 
                                           bcm_oam_endpoint_action_t *action); 

extern int
_bcm_kt2_oam_olp_fp_hw_index_get(int unit,
                                 bcm_field_olp_header_type_t olp_hdr_type,
                                 int *hwindex);

extern int
_bcm_kt2_oam_olp_hw_index_olp_type_get(int unit,
                                   int hwindex,
                                   bcm_field_olp_header_type_t *olp_hdr_type);


extern int _bcm_kt2_port_control_oam_loopkup_with_dvp_set(int unit, 
                                                          bcm_port_t port, 
                                                          int val);
extern int _bcm_kt2_port_control_oam_loopkup_with_dvp_get(int unit, 
                                                          bcm_port_t port, 
                                                          int *val);
extern int bcm_kt2_stg_stp_init(int unit, bcm_stg_t stg);
extern int bcm_kt2_stg_stp_set(int unit, bcm_stg_t stg, 
                         bcm_port_t port, int stp_state);
extern int bcm_kt2_stg_stp_get(int unit, bcm_stg_t stg, 
                         bcm_port_t port, int *stp_state);
extern int bcm_kt2_oam_control_get(int unit, 
                                   bcm_oam_control_type_t type,
                                   uint64  *arg); 
extern int bcm_kt2_oam_control_set(int unit, 
                                   bcm_oam_control_type_t type,
                                   uint64 arg); 
extern int bcm_kt2_oam_upmep_cosq_set(int unit, bcm_oam_upmep_pdu_type_t upmep_pdu_type,
                                      bcm_cos_queue_t cosq);
extern int bcm_kt2_oam_upmep_cosq_get(int unit, bcm_oam_upmep_pdu_type_t upmep_pdu_type,
                                      bcm_cos_queue_t *cosq);

extern int bcm_kt2_oam_trunk_ports_add(int unit, bcm_gport_t trunk_gport, 
                                int max_ports, bcm_gport_t *port_arr);
extern int bcm_kt2_oam_trunk_ports_delete(int unit, bcm_gport_t trunk_gport, 
                                int max_ports, bcm_gport_t *port_arr);
extern int bcm_kt2_dump_port_lls(int unit, bcm_port_t port);

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_kt2_ipmc_subscriber_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#ifdef BCM_WARM_BOOT_SUPPORT /* BCM_WARM_BOOT_SUPPORT */
extern int _bcm_kt2_oam_sync(int unit);
extern int _bcm_kt2_ipmc_repl_reload(int unit);
extern int _bcm_kt2_extq_repl_reload(int unit);
#endif /* !BCM_WARM_BOOT_SUPPORT */
#define KT2_MAX_NUM_PROFILES 4
#define KT2_MIN_RQE_PROFILE_INDEX 0
#define KT2_MAX_RQE_PROFILE_INDEX 3
extern int _bcm_kt2_init_check(int unit, int ipmc_id);
extern int bcm_kt2_ipmc_repl_init(int unit);
extern int
_bcm_kt2_ipmc_egress_intf_set(int unit, int repl_group, bcm_port_t port,
                             int ipmc_ptr, int if_count, bcm_if_t *if_array,
                             int *new_ipmc_ptr, int *last_ipmc_flag,
                             int check_port);
extern int
_bcm_kt2_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                              int ipmc_ptr, int id, int  *new_ipmc_ptr,
                              int *last_flag);
extern int
_bcm_kt2_ipmc_egress_intf_get(int unit, int repl_group,
                             int ipmc_ptr, bcm_if_t *if_array,
                             int *if_count);
extern int
_bcm_kt2_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                                int ipmc_ptr, int id, int  *new_ipmc_ptr,
                                int *last_flag);

extern int
_bcm_kt2_ipmc_egress_intf_delete_all(int unit, int ipmc_id, int ipmc_ptr); 
extern int
_bcm_kt2_ipmc_subscriber_egress_queue_qos_map(int unit, int extq_ptr,
                                             int base_queue_id,
                                             int *profile,
                                             int set);
extern int
_bcm_kt2_ipmc_subscriber_egress_intf_qos_map_set(int unit,
                                        int ipmc_id,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int qos_map_id);

extern int
_bcm_kt2_ipmc_subscriber_egress_intf_qos_map_get(int unit,
                                        int ipmc_id,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int *qos_map_id);




#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP /* BCM_WARM_BOOT_SUPPORT_SW_DUMP*/
extern void _bcm_kt2_oam_sw_dump(int unit);
#endif /* !BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#if defined(INCLUDE_BHH)
extern int bcm_kt2_oam_loopback_add(int unit, bcm_oam_loopback_t *loopback_p);
extern int bcm_kt2_oam_loopback_get(int unit, bcm_oam_loopback_t *loopback_p);
extern int bcm_kt2_oam_loopback_delete(int unit, bcm_oam_loopback_t *loopback_p);
extern int bcm_kt2_oam_loss_add( int unit, bcm_oam_loss_t *loss_ptr);
extern int bcm_kt2_oam_loss_delete( int unit, bcm_oam_loss_t *loss_ptr);
extern int bcm_kt2_oam_loss_get( int unit, bcm_oam_loss_t *loss_ptr);
extern int bcm_kt2_oam_delay_add( int unit, bcm_oam_delay_t *delay_ptr);
extern int bcm_kt2_oam_delay_delete( int unit, bcm_oam_delay_t *delay_ptr);
extern int bcm_kt2_oam_delay_get( int unit, bcm_oam_delay_t *delay_ptr);
extern int bcm_kt2_oam_csf_add(int unit, bcm_oam_csf_t *csf_ptr);
extern int bcm_kt2_oam_csf_delete(int unit, bcm_oam_csf_t *csf_ptr);
extern int bcm_kt2_oam_csf_get(int unit, bcm_oam_csf_t *csf_ptr);
extern int bcm_kt2_oam_endpoint_faults_multi_get( int unit, uint32 flags,
                          bcm_oam_protocol_type_t endpoint_protocol,
                          uint32 max_endpoints, bcm_oam_endpoint_fault_t *faults,
                          uint32 *endpoint_count);
extern int
bcm_kt2_oam_pm_profile_create(int unit, bcm_oam_pm_profile_info_t *profile_info);
extern int
bcm_kt2_oam_pm_profile_delete(int unit, bcm_oam_pm_profile_t profile_id);
extern int
bcm_kt2_oam_pm_profile_delete_all(int unit);
extern int 
bcm_kt2_oam_pm_profile_detach(int unit, bcm_oam_endpoint_t endpoint_id,
                                  bcm_oam_pm_profile_t profile_id);
extern int
bcm_kt2_oam_pm_profile_get(int unit, bcm_oam_pm_profile_info_t *profile_info);
extern int
bcm_kt2_oam_pm_profile_traverse(int unit,
                    bcm_oam_pm_profile_traverse_cb cb, void *user_data);
extern int bcm_kt2_oam_pm_stats_get(int unit,
                            bcm_oam_endpoint_t endpoint_id, bcm_oam_pm_stats_t *stats_ptr);
extern int
bcm_kt2_oam_pm_profile_attach_get(
        int unit, bcm_oam_endpoint_t endpoint_id, bcm_oam_pm_profile_t *profile_id);
extern int
bcm_kt2_oam_pm_profile_attach(int unit, bcm_oam_endpoint_t endpoint_id,
        bcm_oam_pm_profile_t profile_id);
extern int bcm_kt2_oam_mpls_tp_channel_type_tx_set(int unit,
                                                   bcm_oam_mpls_tp_channel_type_t channel_type,
                                                   int value);
extern int bcm_kt2_oam_mpls_tp_channel_type_tx_get(int unit,
                                                   bcm_oam_mpls_tp_channel_type_t channel_type,
                                                   int *value);
#endif /* INCLUDE_BHH */

typedef enum {
    _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
    _BCM_KT2_COSQ_INDEX_STYLE_WRED,
    _BCM_KT2_COSQ_INDEX_STYLE_SCHEDULER,
    _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_KT2_COSQ_INDEX_STYLE_PERQ_XMT,
    _BCM_KT2_COSQ_INDEX_STYLE_QUEUE_REPLICATION
}_bcm_kt2_cosq_index_style_t;

extern int bcm_kt2_cosq_init(int unit);
extern int bcm_kt2_cosq_detach(int unit, int software_state_only);
extern int bcm_kt2_cosq_config_set(int unit, int numq);
extern int bcm_kt2_cosq_config_get(int unit, int *numq);
extern int bcm_kt2_cosq_mapping_set(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_kt2_cosq_mapping_get(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t *cosq);
extern int bcm_kt2_cosq_port_mapping_multi_set(int unit, bcm_port_t port,int count,
                                   bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);
extern int bcm_kt2_cosq_port_mapping_multi_get(int unit, bcm_port_t port,int count,
                                   bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);
extern int bcm_kt2_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                      int mode, const int *weights, int delay);
extern int bcm_kt2_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                      int *mode, int *weights, int *delay);
extern int bcm_kt2_cosq_sched_weight_max_get(int unit, int mode,
                                            int *weight_max);
extern int bcm_kt2_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 min_quantum,
                                          uint32 max_quantum,
                                          uint32 burst_quantum,
                                          uint32 flags);
extern int bcm_kt2_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 *min_quantum,
                                          uint32 *max_quantum,
                                          uint32 *burst_quantum,
                                          uint32 *flags);
extern int bcm_kt2_cosq_discard_set(int unit, uint32 flags);
extern int bcm_kt2_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_kt2_cosq_discard_port_set(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int drop_start, int drop_slope,
                                        int average_time);
extern int bcm_kt2_cosq_discard_port_get(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int *drop_start, int *drop_slope,
                                        int *average_time);
extern int bcm_kt2_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                                 uint32 flags, bcm_gport_t *gport);
extern int bcm_kt2_cosq_gport_delete(int unit, bcm_gport_t gport);
extern int bcm_kt2_cosq_gport_get(int unit, bcm_gport_t gport,
                                 bcm_gport_t *port, int *numq, uint32 *flags);
extern int bcm_kt2_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                                      void *user_data);
extern int bcm_kt2_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);
extern int bcm_kt2_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       int *mode, int *weight);
extern int bcm_kt2_cosq_gport_attach(int unit, bcm_gport_t sched_gport,
                                    bcm_gport_t input_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_kt2_cosq_gport_validate(int unit, bcm_gport_t sched_gport,
                                    bcm_gport_t input_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_kt2_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                                    bcm_gport_t input_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_kt2_cosq_gport_reattach(int unit, bcm_gport_t sched_gport,
                                    bcm_gport_t input_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_kt2_cosq_gport_reattach(int unit, bcm_gport_t sched_gport,
                                    bcm_gport_t input_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_kt2_cosq_gport_child_node_bandwidth_set(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max,
                                           uint32 flags);
extern int bcm_kt2_cosq_gport_child_node_bandwidth_get(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max,
                                           uint32 *flags);
extern int bcm_kt2_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max, uint32 flags);
extern int bcm_kt2_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max,
                                           uint32 *burst_min,
                                           uint32 *burst_max,
                                           uint32 *flags);
extern int bcm_kt2_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 kbits_burst_min,
                                                 uint32 kbits_burst_max);
extern int bcm_kt2_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 *kbits_burst_min,
                                                 uint32 *kbits_burst_max);
extern int bcm_kt2_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                                        bcm_gport_t *input_gport,
                                        bcm_cos_queue_t *cosq);
extern int bcm_kt2_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_kt2_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_kt2_cosq_stat_set(int unit, bcm_gport_t port, 
                                bcm_cos_queue_t cosq,
                                bcm_cosq_stat_t stat, uint64 value);
extern int bcm_kt2_cosq_stat_get(int unit, bcm_gport_t port, 
                                bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, 
                                int sync_mode, uint64 *value);
extern int bcm_kt2_cosq_gport_stat_set(int unit, bcm_gport_t port, 
                                bcm_cos_queue_t cosq,
                                bcm_cosq_gport_stats_t stat, uint64 value);
extern int bcm_kt2_cosq_gport_stat_get(int unit, bcm_gport_t port, 
                                bcm_cos_queue_t cosq,
                                bcm_cosq_gport_stats_t stat, uint64 *value);
extern int bcm_kt2_cosq_port_get(int unit, int queue_id, bcm_port_t *port);
extern int _bcm_kt2_cosq_index_resolve(int unit, bcm_port_t port,
                           bcm_cos_queue_t cosq,
                           _bcm_kt2_cosq_index_style_t style,
                           bcm_port_t *local_port, int *index, int *count);
#ifndef BCM_KATANA_SUPPORT
extern int bcm_kt_port_rate_egress_set(int unit, bcm_port_t port,
                           uint32 kbits_sec, uint32 kbits_burst, uint32 mode);
extern int bcm_kt_port_rate_egress_get(int unit, bcm_port_t port,
                           uint32 *kbits_sec, uint32 *kbits_burst, uint32 *mode);
extern int bcm_kt_port_pps_rate_egress_set(int unit, bcm_port_t port,
                                           uint32 pps, uint32 burst);
#endif
extern int bcm_kt2_cosq_drop_status_enable_set(int unit, bcm_port_t port,
                                              int enable);
extern int bcm_kt2_cosq_control_set(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_control_t type, int arg);

extern int bcm_kt2_cosq_control_get(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_control_t type, int *arg);
extern int _bcm_kt2_cosq_port_resolve(int unit, bcm_gport_t gport,
                                     bcm_module_t *modid, bcm_port_t *port,
                                     bcm_trunk_t *trunk_id, int *id,
                                     int *qnum);
extern int
_bcm_kt2_cosq_localport_resolve(int unit, bcm_gport_t gport,
                               bcm_port_t *local_port);
extern int
bcm_kt2_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int pps);
extern int
bcm_kt2_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          int *pps);

extern int
bcm_kt2_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int burst);

extern int
bcm_kt2_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                            int *burst);
extern int
bcm_kt2_cosq_pfc_class_mapping_set(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array);
extern int
bcm_kt2_cosq_pfc_class_mapping_get(int unit,
     bcm_gport_t port,
     int array_max,
     int *array_count,
     bcm_cosq_pfc_class_mapping_t *mapping_array);
extern int
bcm_kt2_cosq_safc_class_mapping_set(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_safc_class_mapping_t *mapping_array);
extern int
bcm_kt2_cosq_safc_class_mapping_get(int unit,
     bcm_gport_t port,
     int array_max,
     int *array_count,
     bcm_cosq_safc_class_mapping_t *mapping_array);
extern int
bcm_kt2_cosq_port_pfc_op(int unit, bcm_port_t port,
                        bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                        bcm_gport_t *gport, int gport_count);
extern int
bcm_kt2_cosq_port_pfc_get(int unit, bcm_port_t port,
                          bcm_switch_control_t sctype,
                          bcm_gport_t *gport, int gport_count,
                          int *actual_gport_count);



/* Routines to program IFP_COS_MAP table. */
extern int
bcm_kt2_cosq_field_classifier_id_create(int unit,
                                        bcm_cosq_classifier_t *classifier,
                                        int *classifier_id);
extern int 
bcm_kt2_cosq_field_classifier_map_set(int unit,
                                      int classifier_id,
                                      int array_count,
                                      bcm_cos_t *priority_array,
                                      bcm_cos_queue_t *cosq_array);
extern int
bcm_kt2_cosq_field_classifier_map_get(int unit,
                                      int classifier_id,
                                      int array_max,
                                      bcm_cos_t *priority_array,
                                      bcm_cos_queue_t *cosq_array,
                                      int *array_count);
extern int
bcm_kt2_cosq_field_classifier_map_clear(int unit, int classifier_id);

extern int
bcm_kt2_cosq_field_classifier_id_destroy(int unit, int classifier_id);

extern int
bcm_kt2_cosq_field_classifier_get(int unit, int classifier_id,
                                  bcm_cosq_classifier_t *classifier);

/* Routines to program SERVICE_COS_MAP table. */
extern int 
bcm_kt2_cosq_service_classifier_id_create(int unit,
        bcm_cosq_classifier_t *classifier,
        int *classifier_id);

extern int 
bcm_kt2_cosq_service_classifier_id_destroy(int unit,
        int classifier_id);

extern int 
bcm_kt2_cosq_service_classifier_get(int unit,
        int classifier_id,
        bcm_cosq_classifier_t *classifier);

extern int 
bcm_kt2_cosq_service_map_set(int unit, bcm_port_t port,
        int classifier_id,
        bcm_gport_t queue_group,
        int array_count,
        bcm_cos_t *priority_array,
        bcm_cos_queue_t *cosq_array);

extern int 
bcm_kt2_cosq_service_map_get(int unit, bcm_port_t port,
        int classifier_id,
        bcm_gport_t *queue_group,
        int array_max,
        bcm_cos_t *priority_array,
        bcm_cos_queue_t *cosq_array,
        int *array_count);

extern int 
bcm_kt2_cosq_service_map_clear(int unit,
        bcm_gport_t port,
        int classifier_id);

extern void soc_katana2_pbmp_all_resync(int unit);
extern void soc_katana2_pbmp_remove(int unit, soc_port_t port);


#if defined(INCLUDE_L3)
extern int bcm_kt2_failover_prot_nhi_cleanup(int unit,
                                                         int nh_index);
extern int bcm_kt2_failover_status_set(int unit,
                                            bcm_failover_element_t *failover,
                                            int value);
extern int bcm_kt2_failover_status_get(int unit,
                                            bcm_failover_element_t *failover,
                                            int  *value);
/*LPM */
extern int _bcm_kt2_l3_lpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx);
extern int _bcm_kt2_l3_lpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg, int nh_ecmp_idx);
extern int _bcm_kt2_l3_lpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_kt2_l3_lpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
extern int _bcm_kt2_l3_defip_init(int unit);
extern int _bcm_kt2_l3_defip_deinit(int unit);

#endif /* INCLUDE_L3 */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_kt2_cosq_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_kt2_cosq_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

extern int
bcm_kt2_modid_set_all(int unit, int *my_modid_list, int *my_modid_valid_list,
                      int *base_port_ptr_list);

extern int
bcm_kt2_modid_set(int unit, int mod_index, int modid,
                  int modid_valid, int base_port_ptr);

extern int 
bcm_kt2_modid_get_all(int unit, int *my_modid_list, int *my_modid_valid_list,
        int *base_port_ptr_list);

extern int
bcm_kt2_modid_get(int unit, int mod_index, int *my_modid_list,
                  int *my_modid_valid_list, int *base_port_ptr_list);

extern int
_bcm_kt2_pp_port_to_modport_get(int unit, bcm_port_t pp_port, int *modid,
                              bcm_port_t *port);
extern int
_bcm_kt2_modport_to_pp_port_get(int unit, int modid, bcm_port_t port,
                            bcm_port_t *pp_port);
extern int
_bcm_kt2_mod_is_coe_mod_check(int unit, int modid);

extern int
 _bcm_kt2_port_mac_set(int unit, bcm_port_t port, uint64 port_mac);
extern int
_bcm_kt2_port_mac_get(int unit, bcm_port_t port, uint64 *port_mac);
extern int
_bcm_kt2_port_gport_validate(int unit, bcm_port_t port_in, bcm_port_t *port_out);
extern int
_bcm_kt2_source_trunk_map_set(int unit, bcm_port_t port, 
                                         soc_field_t field, uint32 value);
extern int
_bcm_kt2_source_trunk_map_get(int unit, bcm_port_t port, 
                                         soc_field_t field, uint32 *value);
extern int
_bcm_kt2_trunk_table_write(int unit, bcm_port_t port, int tid);
extern int
_bcm_kt2_trunk_table_read(int unit, bcm_port_t port, int *tid);
extern void
_bcm_kt2_subport_pbmp_update(int unit, bcm_pbmp_t *pbmp);

extern int
_bcm_kt2_modport_is_local_coe_subport(int unit, int mod, bcm_port_t port,
                                      int *is_local_subport);

extern int
_bcm_kt2_flexio_pbmp_update(int unit, bcm_pbmp_t *pbmp);

extern int
bcm_kt2_port_dscp_map_set(int unit, bcm_port_t port, int srccp,
                            int mapcp, int prio, int cng);

extern int 
bcm_kt2_cosq_gport_congestion_config_set(int unit, bcm_gport_t gport, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config);
extern int 
bcm_kt2_cosq_gport_congestion_config_get(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config);
int
bcm_kt2_cosq_congestion_mapping_set(int unit,int fabric_modid ,
        bcm_cosq_congestion_mapping_info_t *config);

int
bcm_kt2_cosq_congestion_mapping_get(int unit,int fabric_modid ,
        bcm_cosq_congestion_mapping_info_t *config);

extern int
bcm_kt2_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t gport, bcm_cos_queue_t cosq);

extern int
bcm_kt2_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t *gport, bcm_cos_queue_t *cosq);

extern int
bcm_kt2_cosq_gport_egress_mapping_set(int unit, bcm_port_t gport, bcm_cos_t priority,
        bcm_cos_t cosq, uint32 flags);

extern int
bcm_kt2_cosq_gport_egress_mapping_get(int unit, bcm_port_t gport, bcm_cos_t *priority,
                        bcm_cos_queue_t *cosq, uint32 flags);

extern int
_soc_katana2_mmu_set_invalid_parent(int unit);

extern int
bcmi_katana2_port_fn_drv_init(int unit);


#define KT2_MAX_MODIDS_PER_TRANSLATION_TABLE 4
#define KT2_MAX_PP_PORT_GPP_TRANSLATION_TABLES 4 
#define KT2_MAX_EGR_PP_PORT_GPP_TRANSLATION_TABLES 2 
#define KT2_MAX_PHYSICAL_PORTS 40
#define KT2_HG_NUM_PHYSICAL_QUEUES  24
#define KT2_CPU_NUM_PHYSICAL_QUEUES  48
#define KT2_DEFAULT_NUM_PHYSICAL_QUEUES  8

#define _BCMI_KT2_MAX_COE_MODULES 3
#define _BCMI_KT2_PP_PORT_MAX    (128)

#define _BCMI_KATANA2_MODULE_1  (0)
#define _BCMI_KATANA2_MODULE_2  (1)
#define _BCMI_KATANA2_MODULE_3  (2)
#define _BCMI_KATANA2_MODULE_4  (3)

/* OOB FC Tx side global configuration API */
extern int bcm_kt2_oob_fc_tx_config_set(
    int unit,
    bcm_oob_fc_tx_config_t *config);

/* OOB FC Tx side global configuration get API */
extern int bcm_kt2_oob_fc_tx_config_get(
    int unit,
    bcm_oob_fc_tx_config_t *config);

extern int
_bcm_kt2_oob_fc_tx_port_control_set(int unit, bcm_port_t port,
                                   int status, int dir);
extern int
_bcm_kt2_oob_fc_tx_port_control_get(int unit, bcm_port_t port,
                                   int *status, int dir);

/* OOB FC Tx side global info get API */
extern int bcm_kt2_oob_fc_tx_info_get(
    int unit,
    bcm_oob_fc_tx_info_t *info);

/*
 * OOB FC Rx Interface side Traffic Class to Priority Mapping Multi Set
 * API
 */
extern int bcm_kt2_oob_fc_rx_port_tc_mapping_multi_set(
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
extern int bcm_kt2_oob_fc_rx_port_tc_mapping_multi_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    int array_max,
    uint32 *tc,
    uint32 *pri_bmp,
    int *array_count);

/* OOB FC Rx Interface side Traffic Class to Priority Mapping set API */
extern int bcm_kt2_oob_fc_rx_port_tc_mapping_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 tc,
    uint32 pri_bmp);

/* OOB FC Rx Interface side Traffic Class to Priority Mapping get API */
extern int bcm_kt2_oob_fc_rx_port_tc_mapping_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 tc,
    uint32 *pri_bmp);

/* OOB FC Rx Interface side Configuration set API */
extern int bcm_kt2_oob_fc_rx_config_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_oob_fc_rx_config_t *config,
    int array_count,
    bcm_gport_t *gport_array);

/* OOB FC Rx Interface side Configuration get API */
extern int bcm_kt2_oob_fc_rx_config_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_oob_fc_rx_config_t *config,
    int array_max,
    bcm_gport_t *gport_array,
    int *array_count);

/* OOB FC Rx Interface side Channel offset value for a port */
extern int bcm_kt2_oob_fc_rx_port_offset_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 *offset);

extern int
soc_katana2_mmu_port_enable_set(int unit, bcm_port_t gport,
                                int enable);
extern int
_bcm_kt2_port_enqueue_set(int unit, bcm_port_t gport,
                          int enable);
extern int 
bcm_kt2_port_drain_cells(int unit, int port);

extern int
bcm_kt2_cosq_gport_e2ecc_transmit_set(int unit, bcm_gport_t gport, 
                                      bcm_cos_queue_t cosq, 
                                      bcm_cosq_control_t type, 
                                      int enable);
extern int
bcm_kt2_cosq_gport_e2ecc_transmit_get(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      bcm_cosq_control_t type,
                                      int *enable);
extern int 
bcm_kt2_cosq_stat_config_set(int unit, int stat_count, 
                    bcm_cosq_stat_t  *stat_array);

extern int 
bcm_kt2_cosq_stat_config_get(int unit, int stat_count, 
                    bcm_cosq_stat_t  *stat_array);

/*
 * 8 Priority Groups supported.
 */
#define KT2_PRIOROTY_GROUP_ID_MIN 0
#define KT2_PRIOROTY_GROUP_ID_MAX 7
/*
 * Inpur priority range.
 * PFC : 0-7
 * SAFC : 0-15
 */
#define KT2_PFC_INPUT_PRIORITY_MIN 0
#define KT2_PFC_INPUT_PRIORITY_MAX 7
#define KT2_SAFC_INPUT_PRIORITY_MIN KT2_PFC_INPUT_PRIORITY_MIN
#define KT2_SAFC_INPUT_PRIORITY_MAX 15

extern int
bcm_kt2_port_priority_group_mapping_set(int unit,
                                        bcm_gport_t gport,
                                        int prio,
                                        int priority_group);
extern int
bcm_kt2_port_priority_group_mapping_get(int unit,
                                        bcm_gport_t gport,
                                        int prio,
                                        int *priority_group);
extern int bcm_kt2_switch_control_set(int unit, bcm_switch_control_t type, int arg);
extern int bcm_kt2_switch_control_get(int unit, bcm_switch_control_t type, int *arg);
extern int bcm_kt2_cosq_is_attached(int unit,
                                    bcm_port_t port,
                                    int exclude_default,
                                    int *present);
#endif /* BCM_KATANA2_SUPPORT */
#endif  /* !_BCM_INT_KATANA2_H_ */
