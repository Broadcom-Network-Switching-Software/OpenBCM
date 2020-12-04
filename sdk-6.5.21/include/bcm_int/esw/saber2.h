/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        saber2.h
 * Purpose:     Function declarations for Saber2 bcm functions
 */

#ifndef _BCM_INT_SABER2_H_
#define _BCM_INT_SABER2_H_
#if defined(BCM_SABER2_SUPPORT)
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/field.h>
#include <bcm/qos.h>
#include <bcm/failover.h>
#include <bcm/sat.h>
#include <soc/saber2.h>
#if defined(INCLUDE_L3)
extern int
bcm_sb2_failover_init(int unit);
extern int
bcm_sb2_failover_cleanup(int unit);
extern int bcm_sb2_failover_prot_nhi_cleanup(int unit,
                                                         int nh_index);
extern int bcm_sb2_failover_status_set(int unit,
                                            bcm_failover_element_t *failover,
                                            int value);
extern int bcm_sb2_failover_status_get(int unit,
                                            bcm_failover_element_t *failover,
                                            int  *value);
extern int
bcm_sb2_failover_prot_nhi_set(int unit, uint32 flags, int nh_index, uint32 prot_nh_index, 
                                           bcm_multicast_t  mc_group, bcm_failover_t failover_id);

#endif
/* OAM ING/EGR_SERVICE_PRI_MAP_0/1/2 memory related routines */
#define _BCM_SB2_OAM_PRI_MAP_COUNTER_OFFSET_MAX 7
#define _BCM_SB2_OAM_PRI_MAP_COUNTER_OFFSET_MIN 0

#define _BCM_SB2_INT_PRI_MAX                15
#define _BCM_SB2_INT_PRI_MIN                0

#define _BCM_SB2_PKT_PRI_MAX                7
#define _BCM_SB2_PKT_PRI_MIN                0

#define _BCM_SB2_MPLS_EXP_MAX               7
#define _BCM_SB2_MPLS_EXP_MIN               0

#define _BCM_SB2_OAM_PRI_MAP_TABLE_MAX      6

/* Define OAMP port number */
#define _BCM_SB2_SAT_OAMP_PHY_PORT_NUMBER SOC_SB2_SAT_OAMP_PHY_PORT_NUMBER

#define SB2_MAX_NUM_PROFILES                4
#define SB2_MIN_RQE_PROFILE_INDEX           0
#define SB2_MAX_RQE_PROFILE_INDEX           3

extern int _bcm_sb2_port_lanes_set_post_operation(int unit, bcm_port_t port);
extern int _bcm_sb2_port_lanes_set(int unit, bcm_port_t port, int value);
extern int _bcm_sb2_port_lanes_get(int unit, bcm_port_t port, int *value);
extern int bcm_sb2_flexio_pbmp_update(int unit, bcm_pbmp_t *pbmp);

#if defined(BCM_WARM_BOOT_SUPPORT)
extern int _bcm_sb2_wb_v1_flexio_pbmp_update(int unit, bcm_pbmp_t *pbmp);
#endif

extern int _bcm_sb2_port_oamp_enable(int unit, bcm_port_t port, int enable);

extern int bcm_sb2_oam_init(int unit);

extern int bcm_sb2_oam_detach(int unit);

extern int bcm_sb2_oam_group_create(int unit, bcm_oam_group_info_t *group_info);

extern int bcm_sb2_oam_group_get(int unit, bcm_oam_group_t group,
                                 bcm_oam_group_info_t *group_info);

extern int bcm_sb2_oam_group_destroy(int unit, bcm_oam_group_t group);

extern int bcm_sb2_oam_group_destroy_all(int unit);

extern int bcm_sb2_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb,
                                      void *user_data);

extern int bcm_sb2_oam_endpoint_create(int unit,
                                       bcm_oam_endpoint_info_t *endpoint_info);

extern int bcm_sb2_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint,
                                    bcm_oam_endpoint_info_t *endpoint_info);

extern int bcm_sb2_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint);

extern int bcm_sb2_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group);

extern int bcm_sb2_oam_endpoint_traverse(int unit, bcm_oam_group_t group,
                                         bcm_oam_endpoint_traverse_cb cb,
                                         void *user_data);

extern int bcm_sb2_oam_event_register(int unit, 
                                      bcm_oam_event_types_t event_types,
                                      bcm_oam_event_cb cb, void *user_data);

extern int bcm_sb2_oam_event_unregister(int unit, 
                                      bcm_oam_event_types_t event_types,
                                      bcm_oam_event_cb cb);

extern int bcm_sb2_oam_endpoint_action_set(int unit, bcm_oam_endpoint_t id, 
                                           bcm_oam_endpoint_action_t *action); 

extern int _bcm_sb2_port_control_oam_loopkup_with_dvp_set(int unit, 
                                                          bcm_port_t port, 
                                                          int val);
extern int _bcm_sb2_port_control_oam_loopkup_with_dvp_get(int unit, 
                                                          bcm_port_t port, 
                                                          int *val);
extern int bcm_sb2_oam_lookup_enable_set( int unit, bcm_oam_lookup_type_t type,
        bcm_oam_condition_t condition, int enable);
extern int bcm_sb2_oam_lookup_enable_multi_set(int unit, bcm_oam_lookup_types_t type,
        bcm_oam_conditions_t condition, int enable);
extern int bcm_sb2_oam_lookup_enable_get(int unit, bcm_oam_lookup_type_t type,
        bcm_oam_conditions_t *condition);
extern int bcm_sb2_oam_trunk_ports_add(int unit, bcm_gport_t trunk_gport, 
                                int max_ports, bcm_gport_t *port_arr);
extern int bcm_sb2_oam_trunk_ports_delete(int unit, bcm_gport_t trunk_gport, 
                                int max_ports, bcm_gport_t *port_arr);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP /* BCM_WARM_BOOT_SUPPORT_SW_DUMP*/
extern void _bcm_sb2_oam_sw_dump(int unit);
#endif /* !BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#ifdef BCM_WARM_BOOT_SUPPORT /* BCM_WARM_BOOT_SUPPORT */
extern int _bcm_sb2_oam_sync(int unit);
extern int _bcm_sb2_ipmc_repl_reload(int unit);
#endif/* BCM_WARM_BOOT_SUPPORT */
extern int bcm_sb2_oam_loopback_add(int unit, bcm_oam_loopback_t *loopback_p);
extern int bcm_sb2_oam_loopback_get(int unit, bcm_oam_loopback_t *loopback_p);
extern int bcm_sb2_oam_loopback_delete(int unit, bcm_oam_loopback_t *loopback_p);
extern int bcm_sb2_oam_loss_add( int unit, bcm_oam_loss_t *loss_ptr);
extern int bcm_sb2_oam_loss_delete( int unit, bcm_oam_loss_t *loss_ptr);
extern int bcm_sb2_oam_loss_get( int unit, bcm_oam_loss_t *loss_ptr);
extern int bcm_sb2_oam_delay_add( int unit, bcm_oam_delay_t *delay_ptr);
extern int bcm_sb2_oam_delay_delete( int unit, bcm_oam_delay_t *delay_ptr);
extern int bcm_sb2_oam_delay_get( int unit, bcm_oam_delay_t *delay_ptr);
extern int bcm_sb2_oam_csf_add(int unit, bcm_oam_csf_t *csf_ptr);
extern int bcm_sb2_oam_csf_delete(int unit, bcm_oam_csf_t *csf_ptr);
extern int bcm_sb2_oam_csf_get(int unit, bcm_oam_csf_t *csf_ptr);
extern int bcm_sb2_stg_stp_init(int unit, bcm_stg_t stg);
extern int bcm_sb2_stg_stp_set(int unit, bcm_stg_t stg, 
                         bcm_port_t port, int stp_state);
extern int bcm_sb2_stg_stp_get(int unit, bcm_stg_t stg, 
                         bcm_port_t port, int *stp_state);
extern int bcm_sb2_oam_control_get(int unit, 
                                   bcm_oam_control_type_t type,
                                   uint64  *arg); 
extern int bcm_sb2_oam_control_set(int unit, 
                                   bcm_oam_control_type_t type,
                                   uint64 arg); 
extern int bcm_sb2_oam_upmep_cosq_set(int unit, bcm_oam_upmep_pdu_type_t upmep_pdu_type,
                                      bcm_cos_queue_t cosq);
extern int bcm_sb2_oam_upmep_cosq_get(int unit, bcm_oam_upmep_pdu_type_t upmep_pdu_type,
                                      bcm_cos_queue_t *cosq);
extern int
bcm_sb2_oam_pm_profile_create(int unit, bcm_oam_pm_profile_info_t *profile_info);
extern int
bcm_sb2_oam_pm_profile_delete(int unit, bcm_oam_pm_profile_t profile_id);
extern int
bcm_sb2_oam_pm_profile_delete_all(int unit);
extern int 
bcm_sb2_oam_pm_profile_detach(int unit, bcm_oam_endpoint_t endpoint_id,
                                  bcm_oam_pm_profile_t profile_id);
extern int
bcm_sb2_oam_pm_profile_get(int unit, bcm_oam_pm_profile_info_t *profile_info);
extern int
bcm_sb2_oam_pm_profile_traverse(int unit,
                    bcm_oam_pm_profile_traverse_cb cb, void *user_data);
extern int bcm_sb2_oam_pm_stats_get(int unit,
                            bcm_oam_endpoint_t endpoint_id, bcm_oam_pm_stats_t *stats_ptr);
extern int bcm_sb2_oam_pm_event_register(int unit, bcm_oam_event_types_t event_types,
                                    bcm_oam_pm_event_cb cb, void *user_data);
extern int bcm_sb2_oam_pm_event_unregister(int unit, bcm_oam_event_types_t event_types,
                                     bcm_oam_pm_event_cb cb);
extern int
bcm_sb2_oam_pm_raw_data_read_done(int unit, bcm_oam_event_types_t event_types,
                                  uint32 read_index);
extern int
bcm_sb2_oam_pm_profile_attach_get(
        int unit, bcm_oam_endpoint_t endpoint_id, bcm_oam_pm_profile_t *profile_id);
extern int
bcm_sb2_oam_pm_profile_attach(int unit, bcm_oam_endpoint_t endpoint_id,
        bcm_oam_pm_profile_t profile_id);

extern int
_bcm_sb2_oam_olp_fp_hw_index_get(int unit,
                                 bcm_field_olp_header_type_t olp_hdr_type,
                                 int *hwindex);

extern int
_bcm_sb2_oam_olp_hw_index_olp_type_get(int unit,
                                       int hwindex,
                                       bcm_field_olp_header_type_t *olp_hdr_type);


extern int bcm_sb2_sat_init(int unit);
extern int bcm_sb2_sat_detach(int unit);
extern int bcm_sb2_sat_endpoint_create(int unit,
                                       bcm_sat_endpoint_info_t *endpoint_info);

extern int bcm_sb2_sat_endpoint_get(int unit, 
                                    bcm_sat_endpoint_t endpoint,
                                    uint32 flags,
                                    bcm_sat_endpoint_info_t *endpoint_info);

extern int bcm_sb2_sat_endpoint_destroy(int unit, 
                                        bcm_sat_endpoint_t endpoint,
                                        uint32 flags);

extern int bcm_sb2_sat_endpoint_destroy_all(int unit, uint32  flags);

extern int bcm_sb2_sat_endpoint_traverse(int unit, uint32 flags,
                                         bcm_sat_endpoint_traverse_cb cb,
                                         void *user_data);

extern int bcm_sb2_sat_oamp_enable_set(int unit, int enable); 
extern int bcm_sb2_sat_oamp_enable_get(int unit, int *enable); 
extern int bcm_sb2_sat_ts_format_set(int unit, int ts_format); 
extern int bcm_sb2_sat_ctf_report_process(int unit,
                                          bcm_sat_event_cb cb, void *cookie);
extern int bcm_sb2_sat_init_check(int unit);

extern int _bcm_sb2_port_sw_info_display(int unit, bcm_port_t port);
extern int _bcm_sb2_port_hw_info_display(int unit, bcm_port_t port);

extern  int
 _bcm_sb2_ipmc_subscriber_egress_intf_add(int unit, int ipmc_id,
                                         bcm_port_t port,
                                         int id,
                                         bcm_gport_t subscriber_queue,
                                         int is_l3,
                                         int *queue_count,
                                         int *q_count_increased
                                         );
 extern int
 _bcm_sb2_ipmc_subscriber_egress_intf_delete_all(int unit, int ipmc_id,
                                         bcm_port_t *port_array,
                                         int *encap_array,
                                         bcm_if_t *q_array,
                                         int *q_count_increased); 

 extern int
 _bcm_sb2_ipmc_subscriber_egress_intf_delete(int unit, int ipmc_id,
                                             bcm_port_t port,
                                             int id,
                                             bcm_gport_t subscriber_queue,
                                             int *queue_count,
                                             int *q_count_increased
                                          );
extern  int
_bcm_sb2_ipmc_subscriber_egress_intf_set(int unit, int ipmc_id,
                                         bcm_port_t port, int if_count,
                                         int *id,
                                         bcm_gport_t subscriber_queue,
                                         int is_l3,
                                         int *queue_count,
                                         int *q_count_increased);
int
 _bcm_sb2_ipmc_subscriber_egress_intf_get(int unit, int ipmc_id, int if_max,
                                          bcm_gport_t *port_array,
                                         bcm_if_t *if_array,
                                         bcm_gport_t *subs_array,
                                         int *count);

extern int
_bcm_sb2_ipmc_subscriber_egress_intf_qos_map_set(int unit,
                                        int ipmc_id,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int qos_map_id);
extern int
_bcm_sb2_ipmc_subscriber_egress_intf_qos_map_get(int unit,
                                        int ipmc_id,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int *qos_map_id);

extern  void 
_bcm_sb2_ipmc_port_ext_queue_count_increment(int unit,
                                       int ipmc_id, bcm_port_t port);

extern  void _bcm_sb2_ipmc_port_ext_queue_count_decrement(int unit,
                                       int ipmc_id, bcm_port_t port);

extern  int
 _bcm_sb2_ipmc_set_remap_group(int unit, int ipmc_id,
                             bcm_port_t port, int count);

extern int
bcm_sb2_failover_destroy(int unit, int failover_id);
extern 
soc_error_t soc_saber2_port_detach(int unit,uint8 block_port);

extern int                                                                      
bcm_sb2_oam_opcode_group_set(int unit, bcm_oam_protocol_type_t  proto,          
                            bcm_oam_opcodes_t opcodes, uint8 opcode_group);

extern int
bcm_sb2_oam_opcode_group_get(int unit, bcm_oam_protocol_type_t  proto,          
                            bcm_oam_opcode_t opcode, uint8 *opcode_group);

/* Attach an egress MPLS gport to a MPLS LSP endpoint */
extern int bcm_sb2_oam_endpoint_gport_egress_attach(
    int unit,
    bcm_oam_endpoint_t endpoint,
    bcm_gport_t gport);

/* Detach an egress MPLS gport from a MPLS LSP endpoint */
extern int bcm_sb2_oam_endpoint_gport_egress_detach(
    int unit,
    bcm_oam_endpoint_t endpoint,
    bcm_gport_t gport);

/* Get the list of MPLS gports attached to a MPLS LSP endpoint */
extern int bcm_sb2_oam_endpoint_gport_egress_attach_get(
    int unit,
    bcm_oam_endpoint_t endpoint,
    int size,
    bcm_gport_t *gport,
    int *num_gports);

/* Attach Egress object pointed by egress_intf to a MPLS LSP endpoint */
extern int bcm_sb2_oam_endpoint_egress_intf_egress_attach(
        int unit,
        bcm_oam_endpoint_t ep_id,
        bcm_if_t egress_intf);

/* Detach Egress object pointed by egress_intf from MPLS LSP endpoint */
extern int bcm_sb2_oam_endpoint_egress_intf_egress_detach(
        int unit,
        bcm_oam_endpoint_t ep_id,
        bcm_if_t egress_intf);

/* Get the list of Egress interfaces attached to MPLS LSP endpoint */
extern int bcm_sb2_oam_endpoint_egress_intf_egress_attach_get(int unit,
        bcm_oam_endpoint_t ep_id,
        int size,
        bcm_if_t *egress_intf,
        int *count);

/* Get the faults bitmap for the valid endpoints maximum upto max_endpoints in SB2 */ 
extern int bcm_sb2_oam_endpoint_faults_multi_get(
        int unit,
        uint32 flags,
        bcm_oam_protocol_type_t endpoint_protocol,
        uint32 max_endpoints,
        bcm_oam_endpoint_fault_t *faults,
        uint32 *endpoint_count);

extern 
soc_error_t soc_saber2_port_attach(
        int unit, uint8 block, uint8 block_port );
#ifdef BCM_WARM_BOOT_SUPPORT
extern int
bcm_sb2_failover_sync(int unit);
extern int bcm_sb2_wb_v2_flexio_update;
#endif
#if defined(INCLUDE_BHH)
extern int bcm_sb2_oam_mpls_tp_channel_type_tx_set(int unit,
                                                   bcm_oam_mpls_tp_channel_type_t channel_type,
                                                   int value);
extern int bcm_sb2_oam_mpls_tp_channel_type_tx_get(int unit,
                                                   bcm_oam_mpls_tp_channel_type_t channel_type,
                                                   int *value);
#endif /* INCLUDE_BHH */
#endif /* BCM_SABER2_SUPPORT */
#endif  /* !_BCM_INT_SABER2_H_ */

