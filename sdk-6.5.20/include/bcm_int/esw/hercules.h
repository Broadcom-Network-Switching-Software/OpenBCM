/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hercules.h
 * Purpose:     Function declarations for Hercules bcm functions
 */

#ifndef _BCM_INT_HERCULES_H_
#define _BCM_INT_HERCULES_H_

#include <bcm_int/esw/mbcm.h>

/****************************************************************
 *
 * Hercules functions
 */

extern int bcm_hercules_l2_init(int unit);
extern int bcm_hercules_l2_term(int unit);
extern int bcm_hercules_l2_addr_get(int unit, bcm_mac_t mac_addr,
			     bcm_vlan_t vid, bcm_l2_addr_t *l2addr);
extern int bcm_hercules_l2_addr_add(int unit, bcm_l2_addr_t *l2addr);
extern int bcm_hercules_l2_addr_delete(int, bcm_mac_t, bcm_vlan_t);
extern int bcm_hercules_l2_addr_delete_by_mac(int, bcm_mac_t, uint32);
extern int bcm_hercules_l2_addr_delete_by_mac_port(int, bcm_mac_t,
					    bcm_module_t, bcm_port_t,
					    uint32);
extern int bcm_hercules_l2_conflict_get(int, bcm_l2_addr_t*, bcm_l2_addr_t*,
				 int, int*);

extern int bcm_hercules_vlan_init(int, bcm_vlan_data_t *);
extern int bcm_hercules_vlan_reload(int, vbmp_t *, int *);
extern int bcm_hercules_vlan_create(int, bcm_vlan_t);
extern int bcm_hercules_vlan_destroy(int, bcm_vlan_t);
extern int bcm_hercules_vlan_destroy_all(int);
extern int bcm_hercules_vlan_port_add(int, bcm_vlan_t, bcm_pbmp_t,
                                      bcm_pbmp_t, bcm_pbmp_t);
extern int bcm_hercules_vlan_port_remove(int, bcm_vlan_t, bcm_pbmp_t,
                                         bcm_pbmp_t, bcm_pbmp_t);
extern int bcm_hercules_vlan_port_get(int, bcm_vlan_t, bcm_pbmp_t *,
                                      bcm_pbmp_t *, bcm_pbmp_t *);
extern int bcm_hercules_vlan_list(int, bcm_vlan_data_t **, int *);
extern int bcm_hercules_vlan_stg_get(int, bcm_vlan_t, int *);
extern int bcm_hercules_vlan_stg_set(int, bcm_vlan_t, int);

extern int bcm_hercules_port_cfg_init(int, bcm_port_t, bcm_vlan_data_t *);
extern int bcm_hercules_port_cfg_get(int, bcm_port_t, bcm_port_cfg_t *);
extern int bcm_hercules_port_cfg_set(int, bcm_port_t, bcm_port_cfg_t *);

extern int bcm_hercules_trunk_modify(int, bcm_trunk_t,
			   bcm_trunk_info_t *, int, bcm_trunk_member_t *,
                           trunk_private_t *, int, bcm_trunk_member_t *);
extern int bcm_hercules_trunk_get(int, bcm_trunk_t,
			   bcm_trunk_info_t *, int, bcm_trunk_member_t *,
                           int *, trunk_private_t *);
extern int bcm_hercules_trunk_destroy(int, bcm_trunk_t, trunk_private_t *);
extern int bcm_hercules_trunk_mcast_join(int, bcm_trunk_t,
				  bcm_vlan_t, bcm_mac_t,
				  trunk_private_t *);

extern int bcm_hercules_stg_stp_init(int, bcm_stg_t);
extern int bcm_hercules_stg_stp_get(int, bcm_stg_t, bcm_port_t, int *);
extern int bcm_hercules_stg_stp_set(int, bcm_stg_t, bcm_port_t, int);

extern int bcm_hercules_cosq_init(int unit);
extern int bcm_hercules_cosq_detach(int unit, int software_state_only);
extern int bcm_hercules_cosq_config_set(int unit, int numq);
extern int bcm_hercules_cosq_config_get(int unit, int *numq);
extern int bcm_hercules_cosq_mapping_set(int unit,
					 bcm_port_t port,
					 bcm_cos_t priority,
					 bcm_cos_queue_t cosq);
extern int bcm_hercules_cosq_mapping_get(int unit,
					 bcm_port_t port,
					 bcm_cos_t priority,
					 bcm_cos_queue_t *cosq);
extern int bcm_hercules_cosq_port_sched_set(int unit, bcm_pbmp_t, int mode,
				     const int weights[], int delay);
extern int bcm_hercules_cosq_port_sched_get(int unit, bcm_pbmp_t, int *mode,
				     int weights[], int *delay);
extern int bcm_hercules_cosq_sched_weight_max_get(int unit, int mode,
					   int *weight_max);
extern int bcm_hercules_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                                bcm_cos_queue_t cosq,
                                                uint32 kbits_sec_min,
                                                uint32 kbits_sec_max,
                                                uint32 kbits_sec_burst,
                                                uint32 flags);
extern int bcm_hercules_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                                bcm_cos_queue_t cosq,
                                                uint32 *kbits_sec_min,
                                                uint32 *kbits_sec_max,
                                                uint32 *kbits_sec_burst,
                                                uint32 *flags);
extern int bcm_hercules_cosq_discard_set(int unit, uint32 flags);
extern int bcm_hercules_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_hercules_cosq_discard_port_set(int unit, bcm_port_t port,
                                       bcm_cos_queue_t cosq,
                                       uint32 color,
                                       int drop_start,
                                       int drop_slope,
                                       int average_time);
extern int bcm_hercules_cosq_discard_port_get(int unit, bcm_port_t port,
                                       bcm_cos_queue_t cosq,
                                       uint32 color,
                                       int *drop_start,
                                       int *drop_slope,
                                       int *average_time);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_hercules_cosq_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_hercules_cosq_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

extern int bcm_hercules_mcast_addr_add(int, bcm_mcast_addr_t *);
extern int bcm_hercules_mcast_addr_remove(int, bcm_mac_t, bcm_vlan_t);
extern int bcm_hercules_mcast_port_get(int, bcm_mac_t, bcm_vlan_t, 
				bcm_mcast_addr_t *);
extern int bcm_hercules_mcast_init(int);
extern int _bcm_hercules_mcast_detach(int);
extern int bcm_hercules_mcast_addr_init(bcm_mcast_addr_t *, bcm_mac_t,
				 bcm_vlan_t);
extern int bcm_hercules_mcast_addr_add_w_l2mcindex(int, bcm_mcast_addr_t *);
extern int bcm_hercules_mcast_addr_remove_w_l2mcindex(int,
					       bcm_mcast_addr_t *);
extern int bcm_hercules_mcast_port_add(int, bcm_mcast_addr_t *);
extern int bcm_hercules_mcast_port_remove(int, bcm_mcast_addr_t *);

extern int bcm_hercules_meter_init(int unit);
extern int bcm_hercules_meter_create(int unit, int port, int *mid);
extern int bcm_hercules_meter_delete(int unit, int port, int mid);
extern int bcm_hercules_meter_delete_all(int unit);
extern int bcm_hercules_meter_get(int unit, int port, int mid, 
			   uint32 *kbits_sec, uint32 *kbits_burst);
extern int bcm_hercules_meter_set(int unit, int port, int mid, 
			   uint32 kbits_sec, uint32 kbits_burst);
extern int bcm_hercules_ffppacketcounter_set(int unit, int port, int mid, 
				      uint64 val);
extern int bcm_hercules_ffppacketcounter_get(int unit, int port, int mid, 
				      uint64 *val);
extern int bcm_hercules_ffpcounter_init(int unit);
extern int bcm_hercules_ffpcounter_create(int unit, int port, 
				   int *ffpcounterid);
extern int bcm_hercules_ffpcounter_delete(int unit, int port, 
				   int ffpcounterid);
extern int bcm_hercules_ffpcounter_delete_all(int unit);
extern int bcm_hercules_ffpcounter_set(int unit, int port, int ffpcounterid,
				uint64 val);
extern int bcm_hercules_ffpcounter_get(int unit, int port, int ffpcounterid,
				uint64 *val);
extern int bcm_hercules_port_rate_egress_set(int unit, int port, 
				      uint32 kbits_sec,
				      uint32 kbits_burst, uint32 mode);
extern int bcm_hercules_port_rate_egress_get(int unit, int port, 
				      uint32 *kbits_sec,
				      uint32 *kbits_burst, uint32 *mode);

#ifdef INCLUDE_L3
extern int bcm_hercules_l3_tables_init(int);
extern int bcm_hercules_l3_tables_cleanup(int);
extern int bcm_hercules_l3_enable(int, int);
extern int bcm_hercules_l3_intf_get(int, _bcm_l3_intf_cfg_t *);
extern int bcm_hercules_l3_intf_get_by_vid(int, _bcm_l3_intf_cfg_t *);
extern int bcm_hercules_l3_intf_create(int, _bcm_l3_intf_cfg_t *);
extern int bcm_hercules_l3_intf_id_create(int, _bcm_l3_intf_cfg_t *);
extern int bcm_hercules_l3_intf_lookup(int, _bcm_l3_intf_cfg_t *);
extern int bcm_hercules_l3_intf_del(int, _bcm_l3_intf_cfg_t *);
extern int bcm_hercules_l3_intf_del_all(int);
extern int bcm_hercules_l3_tables_entries(int);

extern int bcm_hercules_l3_get(int, _bcm_l3_cfg_t *);
extern int bcm_hercules_l3_add(int, _bcm_l3_cfg_t *);
extern int bcm_hercules_l3_del(int, _bcm_l3_cfg_t *);
extern int bcm_hercules_l3_del_prefix(int, _bcm_l3_cfg_t *);
extern int bcm_hercules_l3_del_intf(int, _bcm_l3_cfg_t *, int);
extern int bcm_hercules_l3_del_all(int);
extern int bcm_hercules_l3_replace(int, _bcm_l3_cfg_t *);
extern int bcm_hercules_l3_age(int unit, uint32 flags, bcm_l3_host_traverse_cb
                               age_out, void *);
extern int bcm_hercules_l3_traverse(int unit, uint32 flags,
                                    uint32 start, uint32 end,
                                    bcm_l3_host_traverse_cb cb,
                                    void *user_data);

extern int bcm_hercules_l3_ip6_get(int, _bcm_l3_cfg_t *);
extern int bcm_hercules_l3_ip6_add(int, _bcm_l3_cfg_t *);
extern int bcm_hercules_l3_ip6_del(int, _bcm_l3_cfg_t *);
extern int bcm_hercules_l3_ip6_del_prefix(int, _bcm_l3_cfg_t *);
extern int bcm_hercules_l3_ip6_replace(int, _bcm_l3_cfg_t *);
extern int bcm_hercules_l3_ip6_traverse(int unit, uint32 flags,
                                        uint32 start, uint32 end,
                                        bcm_l3_host_traverse_cb cb,
                                        void *user_data);

extern int bcm_hercules_defip_cfg_get(int, _bcm_defip_cfg_t *);
extern int bcm_hercules_defip_ecmp_get_all(int, _bcm_defip_cfg_t *,
                                    bcm_l3_route_t *, int, int *);
extern int bcm_hercules_defip_add(int, _bcm_defip_cfg_t *);
extern int bcm_hercules_defip_del(int, _bcm_defip_cfg_t *);
extern int bcm_hercules_defip_del_intf(int, _bcm_defip_cfg_t *, int);
extern int bcm_hercules_defip_del_all(int);
extern int bcm_hercules_defip_age(int unit, bcm_l3_route_traverse_cb age_out,
                                  void *);
extern int bcm_hercules_defip_traverse(int unit, uint32 start, uint32 end,
                      bcm_l3_route_traverse_cb trav_fn, void *user_data);

extern int bcm_hercules_ip6_defip_cfg_get(int, _bcm_defip_cfg_t *);
extern int bcm_hercules_ip6_defip_ecmp_get_all(int, _bcm_defip_cfg_t *,
                                    bcm_l3_route_t *, int, int *);
extern int bcm_hercules_ip6_defip_add(int, _bcm_defip_cfg_t *);
extern int bcm_hercules_ip6_defip_del(int, _bcm_defip_cfg_t *);
extern int bcm_hercules_ip6_defip_traverse(int unit, uint32 start, uint32 end,
                      bcm_l3_route_traverse_cb trav_fn, void *user_data);

extern int bcm_hercules_l3_conflict_get(int unit, bcm_l3_key_t *ipkey,
					bcm_l3_key_t *cf_array,
					int cf_max, int *cf_count);
extern int bcm_hercules_l3_info(int unit, bcm_l3_info_t *l3_info);

extern int bcm_hercules_ipmc_init(int unit);
extern int bcm_hercules_ipmc_detach(int unit);
extern int bcm_hercules_ipmc_get(int unit, int index, bcm_ipmc_addr_t *ipmc);
extern int bcm_hercules_ipmc_lookup(int unit, int *index, bcm_ipmc_addr_t *ipmc);
extern int bcm_hercules_ipmc_add(int unit, bcm_ipmc_addr_t *ipmc);
extern int bcm_hercules_ipmc_put(int unit, int index, bcm_ipmc_addr_t *ipmc);
extern int bcm_hercules_ipmc_delete(int unit, bcm_ipmc_addr_t *ipmc);
extern int bcm_hercules_ipmc_delete_all(int unit);
extern int bcm_hercules_ipmc_age(int unit, uint32 flags, bcm_ipmc_traverse_cb age_out,
                                 void *user_data);
extern int bcm_hercules_ipmc_traverse(int unit, uint32 flags, bcm_ipmc_traverse_cb cb, 
                                      void *user_data);
extern int bcm_hercules_ipmc_enable(int unit, int enable);
extern int bcm_hercules_ipmc_src_port_check(int unit, int enable);
extern int bcm_hercules_ipmc_src_ip_search(int unit, int enable);
extern int bcm_hercules_ipmc_egress_port_set(int unit, bcm_port_t port, 
					     const bcm_mac_t mac,  int untag, 
					     bcm_vlan_t vid, int ttl_threshold);
extern int bcm_hercules_ipmc_egress_port_get(int unit, bcm_port_t port, 
					     sal_mac_addr_t mac, int *untag, 
					     bcm_vlan_t *vid, 
					     int *ttl_threshold);
extern int bcm_hercules_ipmc_repl_init(int unit);
extern int bcm_hercules_ipmc_repl_detach(int unit);
extern int bcm_hercules_ipmc_repl_get(int unit, int index, bcm_port_t port, 
				      bcm_vlan_vector_t vlan_vec);
extern int bcm_hercules_ipmc_repl_add(int unit, int index, bcm_port_t port, 
				      bcm_vlan_t vlan);
extern int bcm_hercules_ipmc_repl_delete(int unit, int index, bcm_port_t port, 
					 bcm_vlan_t vlan);
extern int bcm_hercules_ipmc_repl_delete_all(int unit, int index, 
					     bcm_port_t port);
extern int bcm_hercules_ipmc_egress_intf_add(int unit, int index, bcm_port_t port, 
			              bcm_l3_intf_t *l3_intf);
extern int bcm_hercules_ipmc_egress_intf_delete(int unit, int index, bcm_port_t port, 
			                 bcm_l3_intf_t *l3_intf);
extern int bcm_hercules_lpm_defip_sw_tbl_empty(int unit, int *count);
extern int bcm_hercules_lpm_defip_hw_tbl_empty(int unit, int *count);
extern int bcm_hercules_lpm_check_table(int unit);
extern int bcm_hercules_lpm_delete(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_hercules_lpm_insert(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_hercules_lpm_lookup(int unit, ip_addr_t iproute, 
                          _bcm_defip_cfg_t* lpm_cfg);
#endif	/* INCLUDE_L3 */

extern int bcm_hercules_switch_control_port_get(int unit, bcm_port_t port,
				    bcm_switch_control_t type,
				    int *arg);
extern int bcm_hercules_switch_control_port_set(int unit, bcm_port_t port,
                                         bcm_switch_control_t type, int arg);

#endif	/* !_BCM_INT_HERCULES_H_ */
