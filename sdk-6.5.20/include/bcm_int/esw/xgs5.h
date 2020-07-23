/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgs5.h
 * Purpose:     Definitions for XGS5 systems.
 */

#ifndef   _BCM_INT_XGS5_H_
#define   _BCM_INT_XGS5_H_
#include <shared/shr_bprof.h>

#include <bcm/qos.h>
#include <bcm/switch.h>
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/port.h>
#if defined(INCLUDE_FLOWTRACKER)
#include <soc/shared/ft_msg.h>
#include <bcm_int/esw/flowtracker.h>
#include <bcm/cosq.h>
#include <soc/shared/qcm.h>
#endif
#include <soc/esw/port.h>

#include <bcm/ecn.h>
#ifdef BCM_MPLS_SUPPORT
#include <bcm/mpls.h>
#endif

#if defined(INCLUDE_IFA)
#include <bcm/ifa.h>
#endif

#if defined(INCLUDE_BFD)

#include <soc/mcm/allenum.h>
#include <soc/tnl_term.h>
#include <bcm/tunnel.h>
#include <bcm/bfd.h>
#include <bcm_int/esw/bfd.h>

/*
 * Local RX DMA channel
 *
 * This is the channel number local to the uC (0..3).
 * Each uController application needs to use a different RX DMA channel.
 */
#define BCM_XGS5_BFD_RX_CHANNEL    1


/*
 * Device Specific HW Definitions
 */

/* Device programming routines */
typedef struct bcm_xgs5_bfd_hw_calls_s {
    int (*l3_tnl_term_entry_init)(int unit, 
                                  bcm_tunnel_terminator_t *tnl_info,
                                  soc_tunnel_term_t *entry);
    int (*mpls_lock)(int unit);
    void (*mpls_unlock)(int unit);
} bcm_xgs5_bfd_hw_calls_t;

/* L2 Table */
typedef struct bcm_xgs5_bfd_l2_table_s {
    soc_mem_t    mem;
    soc_field_t  key_type;
    uint32       bfd_key_type;
    soc_field_t  valid;
    soc_field_t  static_bit;
    soc_field_t  session_id_type;
    soc_field_t  your_discr;
    soc_field_t  label;
    soc_field_t  session_index;
    soc_field_t  cpu_queue_class;
    soc_field_t  remote;
    soc_field_t  dst_module;
    soc_field_t  dst_port;
    soc_field_t  int_pri;
} bcm_xgs5_bfd_l2_table_t;

/* L3 IPv4 Unicast Table */
typedef struct bcm_xgs5_bfd_l3_ipv4_table_s {
    soc_mem_t    mem;
    soc_field_t  vrf_id;
    soc_field_t  ip_addr;
    soc_field_t  key_type;
    soc_field_t  local_address;
    soc_field_t  bfd_enable;
} bcm_xgs5_bfd_l3_ipv4_table_t;

/* L3 IPv6 Unicast Table */
typedef struct bcm_xgs5_bfd_l3_ipv6_table_s {
    soc_mem_t    mem;
    soc_field_t  ip_addr_lwr_64;
    soc_field_t  ip_addr_upr_64;
    soc_field_t  key_type_0;
    soc_field_t  key_type_1;
    soc_field_t  vrf_id;
    soc_field_t  local_address;
    soc_field_t  bfd_enable;
    soc_field_t  base_valid_0;
    soc_field_t  base_valid_1;
    soc_field_t  key_type;
    soc_field_t  data_type;
} bcm_xgs5_bfd_l3_ipv6_table_t;

/* L3 Tunnel Table */
typedef struct bcm_xgs5_bfd_l3_tunnel_table_s {
    soc_mem_t    mem;
    soc_field_t  bfd_enable;
} bcm_xgs5_bfd_l3_tunnel_table_t;

/* L3 Tunnel Table IPV6*/
typedef struct bcm_xgs5_bfd_l3_tunnel_ipv6_table_s {
    soc_mem_t    mem;
    soc_field_t  bfd_enable;
} bcm_xgs5_bfd_l3_tunnel_ipv6_table_t;

/* MPLS Table */
typedef struct bcm_xgs5_bfd_mpls_table_s {
    soc_mem_t    mem;
    soc_field_t  valid;
    soc_field_t  key_type;
    uint32       key_type_value;
    soc_field_t  mpls_label;
    soc_field_t  session_id_type;
    soc_field_t  bfd_enable;
    soc_field_t  cw_check_ctrl;
    soc_field_t  pw_cc_type;
    soc_field_t  mpls_action_if_bos;
    soc_field_t  l3_iif;
    soc_field_t  decap_use_ttl;
    soc_field_t  mod_id;
    soc_field_t  port_num;
} bcm_xgs5_bfd_mpls_table_t;

/* HW Definitions */
typedef struct bcm_xgs5_bfd_hw_defs_t {
    bcm_xgs5_bfd_hw_calls_t         *hw_call;    /* Chip programming */
    bcm_xgs5_bfd_l2_table_t         *l2;         /* L2 Memory Table */
    bcm_xgs5_bfd_l3_ipv4_table_t    *l3_ipv4;    /* L3 IPv4 UC Table */
    bcm_xgs5_bfd_l3_ipv6_table_t    *l3_ipv6;    /* L3 IPv6 UC Table */
    bcm_xgs5_bfd_l3_tunnel_table_t  *l3_tunnel;  /* L3 Tunnel Table */
    bcm_xgs5_bfd_l3_tunnel_ipv6_table_t *l3_tunnel_ipv6; /*L3 Tunnel IPV6 table */
    bcm_xgs5_bfd_mpls_table_t       *mpls;       /* MPLS Table */
    uint8                           bfd_feature_enable; /*Save BFD Feature Flag */ 
} bcm_xgs5_bfd_hw_defs_t;


/* Functions */
extern
int bcmi_xgs5_bfd_init(int unit,
                       bcm_esw_bfd_drv_t *drv,
                       bcm_xgs5_bfd_hw_defs_t *hw_defs);
extern
int bcmi_xgs5_bfd_detach(int unit);
extern
int bcmi_xgs5_bfd_endpoint_create(int unit,
                                  bcm_bfd_endpoint_info_t *endpoint_info);
extern
int bcmi_xgs5_bfd_endpoint_get(int unit, bcm_bfd_endpoint_t endpoint, 
                               bcm_bfd_endpoint_info_t *endpoint_info);
extern
int bcmi_xgs5_bfd_tx_start(int unit);
extern
int bcmi_xgs5_bfd_tx_stop(int unit);
extern
int bcmi_xgs5_bfd_endpoint_destroy(int unit,
                                   bcm_bfd_endpoint_t endpoint);
extern
int bcmi_xgs5_bfd_endpoint_destroy_all(int unit);
extern
int bcmi_xgs5_bfd_endpoint_poll(int unit, bcm_bfd_endpoint_t endpoint);
extern
int bcmi_xgs5_bfd_event_register(int unit,
                                 bcm_bfd_event_types_t event_types, 
                                 bcm_bfd_event_cb cb, void *user_data);
extern
int bcmi_xgs5_bfd_event_unregister(int unit,
                                   bcm_bfd_event_types_t event_types, 
                                   bcm_bfd_event_cb cb);
extern
int bcmi_xgs5_bfd_endpoint_stat_get(int unit,
                                    bcm_bfd_endpoint_t endpoint, 
                                    bcm_bfd_endpoint_stat_t *ctr_info,
                                    uint32 options);
extern
int bcmi_xgs5_bfd_auth_sha1_set(int unit, int index,
                                bcm_bfd_auth_sha1_t *sha1);
extern
int bcmi_xgs5_bfd_auth_sha1_get(int unit, int index,
                                bcm_bfd_auth_sha1_t *sha1);
extern
int bcmi_xgs5_bfd_auth_simple_password_set(int unit, int index, 
                                           bcm_bfd_auth_simple_password_t *sp);
extern
int bcmi_xgs5_bfd_auth_simple_password_get(int unit, int index, 
                                           bcm_bfd_auth_simple_password_t *sp);
extern
int bcmi_xgs5_bfd_status_multi_get(int unit, int max_endpoints,
                                   bcm_bfd_status_t *status_arr,
                                   int *count);
extern
int bcmi_xgs5_bfd_discard_stat_set(int unit,
                                   bcm_bfd_discard_stat_t *discarded_info);
extern
int bcmi_xgs5_bfd_discard_stat_get(int unit,
                                   bcm_bfd_discard_stat_t *discarded_info);

#ifdef BCM_WARM_BOOT_SUPPORT
extern
int bcmi_xgs5_bfd_sync(int unit);

extern
int _bcmi_bfd_wb_downgrade_config_set(int unit, uint32 warmboot_ver);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern
void bcmi_xgs5_bfd_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern
void bcmi_xgs5_bfd_dump_trace(int unit);
#endif /* INCLUDE_BFD */

/* ING Port Table */
typedef struct bcmi_xgs5_subport_coe_ing_port_table_s {
    soc_mem_t    mem;
    soc_field_t  port_type;
} bcmi_xgs5_subport_coe_ing_port_table_t;

/* EGR Port Table */
typedef struct bcmi_xgs5_subport_coe_egr_port_table_s {
    soc_mem_t    mem;
    soc_field_t  port_type;
} bcmi_xgs5_subport_coe_egr_port_table_t;

/* Subport Tag SGPP Memory Table */
typedef struct bcmi_xgs5_subport_coe_subport_tag_sgpp_table_s {
    soc_mem_t    mem;
    soc_field_t  valid;
    soc_field_t  subport_tag;
    soc_field_t  subport_tag_mask;
    soc_field_t  subport_tag_namespace;
    soc_field_t  subport_tag_namespace_mask;
    soc_field_t  src_modid;
    soc_field_t  src_port;
    soc_field_t  phb_enable;
    soc_field_t  int_pri;
    soc_field_t  cng;
} bcmi_xgs5_subport_coe_subport_tag_sgpp_table_t;

/* Modport map subport Memory Table */
typedef struct bcmi_xgs5_subport_coe_modport_map_subport_table_s {
    soc_mem_t    mem;
    soc_field_t  dest;
    soc_field_t  is_trunk;
    soc_field_t  enable;
#ifdef BCM_FIREBOLT6_SUPPORT
    soc_field_t  dst_subport;
#endif
} bcmi_xgs5_subport_coe_modport_map_subport_table_t;

/* Egress subport tag dot1p Table */
typedef struct bcmi_xgs5_subport_coe_egr_subport_tag_dot1p_table_s {
    soc_mem_t    mem;
    soc_field_t  subport_tag_priority;
    soc_field_t  subport_tag_cfi;
    soc_field_t  subport_tag_color;
} bcmi_xgs5_subport_coe_egr_subport_tag_dot1p_table_t;

typedef struct bcmi_xgs5_subport_coe_hw_defs_s {
     /* Ingress Port Memory Table */
    bcmi_xgs5_subport_coe_ing_port_table_t              *igr_port;
    /* Egress Port Memory Table */     
    bcmi_xgs5_subport_coe_egr_port_table_t              *egr_port;
    /* Subport Tag SGPP Memory Table */
    bcmi_xgs5_subport_coe_subport_tag_sgpp_table_t      *subport_tag_sgpp;
    /* Modport map subport Memory Table */
    bcmi_xgs5_subport_coe_modport_map_subport_table_t   *modport_map_subport; 
  /* Egress subport tag dot1p Table */
    bcmi_xgs5_subport_coe_egr_subport_tag_dot1p_table_t *egr_subport_tag_dot1p;
} bcmi_xgs5_subport_coe_hw_defs_t;


/* Functions */
extern int bcmi_xgs5_subport_init(int unit,
                           bcm_esw_subport_drv_t *drv,
                           bcmi_xgs5_subport_coe_hw_defs_t *hw_defs);
extern int _bcmi_xgs5_subport_reinit(int unit);
extern int bcmi_xgs5_subport_coe_cleanup(int unit);
extern int bcmi_xgs5_subport_coe_group_create(int unit,
                                     bcm_subport_group_config_t *config,
                                     bcm_gport_t *group);
extern int bcmi_xgs5_subport_coe_group_destroy(int unit, bcm_gport_t group);
extern int bcmi_xgs5_subport_coe_group_get(int unit, bcm_gport_t group,
                                     bcm_subport_group_config_t *config);
extern int bcmi_xgs5_subport_coe_group_traverse(int unit, bcm_gport_t group,
                                     bcm_subport_port_traverse_cb cb,
                                     void *user_data);
extern int bcmi_xgs5_subport_coe_port_add(int unit, bcm_subport_config_t 
                                          *config, bcm_gport_t *port);
extern int bcmi_xgs5_subport_coe_port_delete(int unit, bcm_gport_t port);
extern int bcmi_xgs5_subport_coe_port_get(int unit, bcm_gport_t port,
                                   bcm_subport_config_t *config);
extern int bcmi_xgs5_subport_coe_port_traverse(int unit,
                                      bcm_subport_port_traverse_cb cb,
                                        void *user_data);

extern int bcmi_xgs5_subport_egr_subtag_dot1p_map_add(int unit,
                                                      bcm_qos_map_t *map);
extern int bcmi_xgs5_subport_egr_subtag_dot1p_map_delete(int unit,
                                                         bcm_qos_map_t *map);

extern int bcmi_xgs5_subport_subtag_port_tpid_set(int unit, bcm_gport_t gport,
                                                  uint16 tpid);

extern int bcmi_xgs5_subport_subtag_port_tpid_delete(int unit, 
                                                     bcm_gport_t gport,
                                                     uint16 tpid);

extern int bcmi_xgs5_subport_subtag_port_tpid_get(int unit, bcm_gport_t gport,
                                                  uint16 *tpid);

extern int bcmi_xgs5_port_control_subtag_status_set(int unit,
                                                    bcm_port_t port, 
                                                    int value);

extern int bcmi_xgs5_port_control_subtag_status_get(int unit, 
                                                    bcm_port_t port, 
                                                    int *value);

extern int _bcmi_xgs5_subport_group_resolve(int unit,
                                           bcm_gport_t subport_group_gport,
                                           bcm_module_t *modid, 
                                           bcm_port_t *port,
                                           bcm_trunk_t *trunk_id, 
                                           int *id);

extern int _bcm_xgs5_subport_port_resolve(int unit, 
                                          bcm_gport_t subport_port_gport,
                                          bcm_module_t *modid,
                                          bcm_port_t *port,
                                          bcm_trunk_t *trunk_id,
                                          int *id);

extern int bcmi_xgs5_subport_gport_modport_get(int unit,
            bcm_gport_t subport_gport, bcm_module_t *module, bcm_port_t *port);

extern int _bcmi_coe_subport_physical_port_get(int unit, bcm_gport_t subport,
                                               int *local_port);

extern int _bcmi_coe_subport_mod_port_physical_port_get(int unit, bcm_module_t modid, 
                                                        bcm_port_t portid,
                                                        int *local_port);

extern int _bcm_xgs5_subport_coe_mod_local(int unit, int modid);

extern int _bcm_xgs5_subport_gport_validate(int unit, bcm_port_t port_in,
                                            bcm_port_t *port_out);

extern int _bcm_xgs5_subport_coe_mod_port_local(int unit, int modid, 
                                                bcm_port_t port);

extern int _bcm_xgs5_subport_coe_gport_local(int unit, bcm_gport_t gport);

extern int _bcmi_coe_multicast_subport_encap_get(int unit, 
                                                 bcm_gport_t subport_port,
                                                 bcm_if_t *encap_id);

extern int _bcmi_coe_subport_tcam_idx_get(int unit, bcm_gport_t subport,
                                          int *hw_idx);

extern int bcmi_xgs5_subport_coe_ether_type_size_set(int unit, 
                                              bcm_switch_control_t type, 
                                              int arg);

extern int bcmi_xgs5_subport_coe_ether_type_size_get(int unit, 
                                              bcm_switch_control_t type, 
                                              int *arg);

extern int bcmi_xgs5_subport_subtag_port_override_set(int unit, int port, int arg);

extern int bcmi_xgs5_subport_subtag_port_override_get(int unit, int port, int *arg);

extern int bcmi_xgs5_subport_port_learn_set(int unit, bcm_gport_t subport_port_id, 
                                  uint32 flags);

extern int bcmi_xgs5_subport_port_learn_get(int unit, bcm_gport_t subport_port_id,
                                  uint32 *flags);


#ifdef INCLUDE_L3
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))

#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_INVALID    0x0
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_POP        0x1
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_NHI    0x2
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI   0x3
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP  0x4
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_ECMP   0x5

#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_INVALID        0x0
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L2_SVP         0x1
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_IIF         0x2
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI       0x3
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI         0x4
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP        0x5
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP      0x6
#define _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_ROE_NHI        0x7

/* ROE Tunnel Type */
#define _BCM_MPLS_XGS5_MPLS_ROE_CUSTOM_TUNNEL            0x1
#define _BCM_MPLS_XGS5_MPLS_ROE_TUNNEL                   0x2

/* TD3:MPLS_ACTION
 * Common format used for both MPLS_ACTION_IF_NOT_BOS
 * and MPLS_ACTION_IF_BOS.*/
#define _BCM_MPLS_TD3_MPLS_ACTION_INVALID          0x0
#define _BCM_MPLS_TD3_MPLS_ACTION_L2_SVP           0x1
#define _BCM_MPLS_TD3_MPLS_ACTION_L3_IIF           0x2
#define _BCM_MPLS_TD3_MPLS_ACTION_SWAP             0x3
#define _BCM_MPLS_TD3_MPLS_ACTION_RESERVED_2       0x4
#define _BCM_MPLS_TD3_MPLS_ACTION_PHP              0x5
#define _BCM_MPLS_TD3_MPLS_ACTION_POP_SVP_ENTROPY  0x6 /*Applicable only for BOS=0 labels*/
#define _BCM_MPLS_TD3_MPLS_ACTION_POP              0x7 /*Applicable only for BOS=0 labels*/
#define _BCM_MPLS_TD3_MPLS_ACTION_RESERVED_1       0x8



/* MPLS functions */
extern int bcmi_xgs5_mpls_tunnel_switch_get(int unit,
        bcm_mpls_tunnel_switch_t *info);
extern int bcmi_xgs5_mpls_tunnel_switch_delete_all(int unit);
extern int bcmi_xgs5_mpls_tunnel_switch_delete(int unit,
        bcm_mpls_tunnel_switch_t *info);
extern int bcmi_xgs5_mpls_tunnel_switch_add(int unit,
        bcm_mpls_tunnel_switch_t *info);
extern int bcmi_xgs5_mpls_tunnel_switch_traverse(int unit,
        bcm_mpls_tunnel_switch_traverse_cb cb,
        void *user_data);

/*****************************************************
 **********                                 **********
 ******                                         ******
 ****      Segment Routing Changes - START        ****
 */
extern int bcmi_xgs5_mpls_tunnel_initiator_set(int unit,
        bcm_if_t intf, int num_labels,
        bcm_mpls_egress_label_t *label_array);
extern int bcmi_xgs5_mpls_tunnel_initiator_get(int unit,
        bcm_if_t intf, int label_max,
        bcm_mpls_egress_label_t *label_array, int *label_count);
extern int bcmi_xgs5_mpls_tunnel_initiator_clear(int unit, int intf_id);
extern int bcmi_xgs5_mpls_tunnel_initiator_clear_all(int unit);
extern int bcmi_xgs5_mpls_tunnel_initiator_reinit(int unit);
extern int bcmi_egr_ip_tunnel_mpls_sw_init(int unit);
extern int bcmi_egr_ip_tunnel_mpls_sw_cleanup(int unit);
/*****************************************************
 **********                                 **********
 ******                                         ******
 ****      Segment Routing Changes - END          ****
 */
extern int
bcmi_xgs5_mpls_failover_nw_port_match_get(int unit, bcm_mpls_port_t *mpls_port,
                                    int vp, mpls_entry_entry_t *return_ment);

/* End of MPLS functions */

#endif /* (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)) */
#if (defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT))
extern int bcmi_xgs5_mpls_ecn_map_create(int unit, uint32 flags,
                                         int *map_id);
extern int bcmi_xgs5_mpls_ecn_map_destroy(int unit, int map_id);
extern int bcmi_xgs5_mpls_ecn_map_set(int unit, int map_id,
                                      bcm_ecn_map_t *map);
extern int bcmi_xgs5_mpls_ecn_map_get(int unit, int map_id,
                                      bcm_ecn_map_t *map);
extern int
bcmi_xgs5_mpls_ecn_port_map_get(int unit, bcm_gport_t port,
                                bcm_ecn_port_map_t* ecn_map);
extern int
bcmi_xgs5_mpls_ecn_port_map_set(int unit, bcm_gport_t port,
                                bcm_ecn_port_map_t* ecn_map);

#endif /* BCM_TOMAHAWK2_SUPPORT */

extern int bcmi_xgs5_ecn_map_create(int unit, uint32 flags,
                                    int *map_id);
extern int bcmi_xgs5_ecn_map_destroy(int unit, int map_id);
extern int bcmi_xgs5_ecn_map_set(int unit, int map_id,
                                 bcm_ecn_map_t *map);
extern int bcmi_xgs5_ecn_map_get(int unit, int map_id,
                                 bcm_ecn_map_t *map);
extern int
bcmi_xgs5_ecn_port_map_get(int unit, bcm_gport_t port,
                           bcm_ecn_port_map_t* ecn_map);
extern int
bcmi_xgs5_ecn_port_map_set(int unit, bcm_gport_t port,
                           bcm_ecn_port_map_t* ecn_map);

extern int bcmi_xgs5_ecn_init(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcmi_xgs5_ecn_sw_dump(int unit);
#endif
#endif /* INCLUDE_L3 */


/* EP Redirection definitions */

/* Device specific hardware defines */
/* EGR Port Table */
typedef struct bcmi_xgs5_port_redirection_egr_port_table_s {
    soc_mem_t    mem;
    soc_field_t  dest_type;
    soc_field_t  dest_value;
    soc_field_t  drop_original;
    soc_field_t  pkt_priority;
    soc_field_t  pkt_change_priority;
    soc_field_t  pkt_color;
    soc_field_t  pkt_change_color;
    soc_field_t  strength;
    soc_field_t  buffer_priority;
    soc_field_t  action;
    soc_field_t  redir_pkt_source;
    soc_field_t  redir_pkt_truncate;
} bcmi_xgs5_port_redirection_egr_port_table_t;

/* Port hardware defs */
typedef struct bcmi_xgs5_port_hw_defs_s {
    bcmi_xgs5_port_redirection_egr_port_table_t  *egr_port;
} bcmi_xgs5_port_hw_defs_t;

/*
 * Port Functions
 */

/*
 * FlexPort Operations Changes
 *
 * Flags to be used to determine the type of operations required
 * when the FlexPort API multi_set() is called.
 *
 * OP_NONE  - No changes.
 * OP_REMAP - Change in port mapping.  It requires FlexPort sequence.
 * OP_LANES - Change in lanes.  It requires FlexPort sequence.
 * OP_SPEED - Change in speed.  This is covered in a FlexPort sequence.
 *            If 'remap' or 'lanes' are not changing, then
 *            calling the 'speed' set function is enough (no need
 *            to call the FlexPort sequence).
 * OP_ENCAP - Change in encap mode.  It is NOT covered in a FlexPort
 *            sequence, so an explicit call must be made to change
 *            the encap mode.
 * OP_ADD   - Add a new logical port. It requires FlexPort sequence.
 * OP_DEL   - Delete a logical port. It requires FlexPort sequence.
 * OP_ACTIVE - Active a logical port. It requires FlexPort sequence.
 * OP_ENABLE - Enable or disable a logical port. It does NOT require
 *             FlexPort sequence.
 * OP_CALENDAR - Only TDM calendar change. Currently only between 25G port use 25G TDM
 *             calendar and 25G port use 50G TDM.
 * OP_ENCAP_FLEX - When change in encap mode, requires  FlexPort sequence.
 */
#define BCMI_XGS5_PORT_RESOURCE_OP_NONE     0
#define BCMI_XGS5_PORT_RESOURCE_OP_REMAP   (1 << 0)
#define BCMI_XGS5_PORT_RESOURCE_OP_LANES   (1 << 1)
#define BCMI_XGS5_PORT_RESOURCE_OP_SPEED   (1 << 2)
#define BCMI_XGS5_PORT_RESOURCE_OP_ENCAP   (1 << 3)
#define BCMI_XGS5_PORT_RESOURCE_OP_ADD     (1 << 4)
#define BCMI_XGS5_PORT_RESOURCE_OP_DEL     (1 << 5)
#define BCMI_XGS5_PORT_RESOURCE_OP_ACTIVE  (1 << 6)
#define BCMI_XGS5_PORT_RESOURCE_OP_ENABLE  (1 << 7)
#define BCMI_XGS5_PORT_RESOURCE_OP_CALENDAR (1 << 8)
#define BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX (1 << 9)
#define BCMI_XGS5_PORT_RESOURCE_OP_CHL      (1 << 10)

/*
 * Sequence of attaching a port to BCM layer.
 * Follow the same sequence as in _bcm_modules_init()
 */
typedef enum bcmi_xgs5_port_attach_exec_seq_e{
    /* MMU config init */
    PORT_ATTACH_EXEC__MMU_INIT,

    /* PORT module (bcm_esw_port_init) */
    PORT_ATTACH_EXEC__DSCP_PROFILE,
    PORT_ATTACH_EXEC__SOFTWARE,
    PORT_ATTACH_EXEC__EGR_VLAN_ACTION,
    PORT_ATTACH_EXEC__PORT_CFG_INIT,
    PORT_ATTACH_EXEC__PORT_PROBE,
    PORT_ATTACH_EXEC__FRAME_LENGTH_CHECK,
    PORT_ATTACH_EXEC__RCPU_MTU,
    PORT_ATTACH_EXEC__OUTER_TPID,
    PORT_ATTACH_EXEC__EGR_BLOCK_PROFILE,
    PORT_ATTACH_EXEC__VLAN_PROTOCOL,
    PORT_ATTACH_EXEC__EEE,
    PORT_ATTACH_EXEC__HIGIG,
    PORT_ATTACH_EXEC__PORT_INFO_CFG,       /* Set PORT_INFO with status from HW*/
    PORT_ATTACH_EXEC__EGR_LINK_DELAY,

    /* Other modules related to port */
    PORT_ATTACH_EXEC__STG,
    PORT_ATTACH_EXEC__VLAN,
    PORT_ATTACH_EXEC__TRUNK,
    PORT_ATTACH_EXEC__COSQ,
    PORT_ATTACH_EXEC__LINKSCAN,
    PORT_ATTACH_EXEC__STATISTIC,
    PORT_ATTACH_EXEC__STACK,
    PORT_ATTACH_EXEC__RATE,
    PORT_ATTACH_EXEC__FIELD,
    PORT_ATTACH_EXEC__MIRROR,
    PORT_ATTACH_EXEC__L3,
    PORT_ATTACH_EXEC__IPMC,
    PORT_ATTACH_EXEC__IPMC_REPL,
    PORT_ATTACH_EXEC__MPLS,
    PORT_ATTACH_EXEC__MIM,

    PORT_ATTACH_EXEC__MAX
} bcmi_xgs5_port_attach_exec_seq_t;

#define BCMI_XGS5_PORT_ATTACH_PHASES_MSG    {\
    "MMU init",                 /* PORT_ATTACH_EXEC__MMU_INIT */ \
    "DSCP profile",             /* PORT_ATTACH_EXEC__DSCP_PROFILE */ \
    "Software init",            /* PORT_ATTACH_EXEC__SOFTWARE */ \
    "Egress Vlan action",       /* PORT_ATTACH_EXEC__EGR_VLAN_ACTION */ \
    "Port config init",         /* PORT_ATTACH_EXEC__PORT_CFG_INIT */ \
    "Port probe",               /* PORT_ATTACH_EXEC__PORT_PROBE */ \
    "Fram length check",        /* PORT_ATTACH_EXEC__FRAME_LENGTH_CHECK */ \
    "RCPU MTU",                 /* PORT_ATTACH_EXEC__RCPU_MTU */ \
    "Outer TPID",               /* PORT_ATTACH_EXEC__OUTER_TPID */ \
    "Egress block profile",     /* PORT_ATTACH_EXEC__EGR_BLOCK_PROFILE */ \
    "Vlan protocol",            /* PORT_ATTACH_EXEC__VLAN_PROTOCOL */ \
    "EEE config",               /* PORT_ATTACH_EXEC__EEE */ \
    "HIGIG",                    /* PORT_ATTACH_EXEC__HIGIG */ \
    "Port info config",         /* PORT_ATTACH_EXEC__PORT_INFO_CFG */ \
    "Egress link delay",        /* PORT_ATTACH_EXEC__EGR_LINK_DELAY */ \
    "STG",                      /* PORT_ATTACH_EXEC__STG */ \
    "Vlan",                     /* PORT_ATTACH_EXEC__VLAN */ \
    "Trunk",                    /* PORT_ATTACH_EXEC__TRUNK */ \
    "COSQ",                     /* PORT_ATTACH_EXEC__COSQ  */ \
    "Linkscan",                 /* PORT_ATTACH_EXEC__LINKSCAN */ \
    "Statistic",                /* PORT_ATTACH_EXEC__STATISTIC */ \
    "Stack",                    /* PORT_ATTACH_EXEC__STACK */ \
    "Rate",                     /* PORT_ATTACH_EXEC__RATE */ \
    "Field",                    /* PORT_ATTACH_EXEC__FIELD */ \
    "Mirror",                   /* PORT_ATTACH_EXEC__MIRROR */ \
    "L3",                       /* PORT_ATTACH_EXEC__L3 */ \
    "IPMC",                     /* PORT_ATTACH_EXEC__IPMC */ \
    "IPMC replication",         /* PORT_ATTACH_EXEC__IPMC_REPL */ \
    "MPLS",                     /* PORT_ATTACH_EXEC__MPLS */ \
    "MIM",                      /* PORT_ATTACH_EXEC__MIM */ \
    "Unkown phase"              /* PORT_ATTACH_EXEC__MAX */ \
    }

/*
 * Sequence of detaching a port from BCM layer.
 * Follow the same sequence as in _bcm_esw_modules_deinit()
 */
typedef enum bcmi_xgs5_port_detach_exec_seq_e{

    /* Other modules related to port */
    PORT_DETACH_EXEC__IPMC_REPL,
    PORT_DETACH_EXEC__IPMC,
    PORT_DETACH_EXEC__MPLS,
    PORT_DETACH_EXEC__MIM,
    PORT_DETACH_EXEC__L3,
    PORT_DETACH_EXEC__MIRROR,
    PORT_DETACH_EXEC__FIELD,
    PORT_DETACH_EXEC__RATE,
    PORT_DETACH_EXEC__STACK,
    PORT_DETACH_EXEC__COSQ,
    PORT_DETACH_EXEC__TRUNK,
    PORT_DETACH_EXEC__VLAN,
    PORT_DETACH_EXEC__STG,

    /* PORT module (_bcm_esw_port_deinit) */
    PORT_DETACH_EXEC__ASF,
    PORT_DETACH_EXEC__EGR_LINK_DELAY,
    PORT_DETACH_EXEC__HIGIG,
    PORT_DETACH_EXEC__EEE,
    PORT_DETACH_EXEC__VLAN_PROTOCOL,
    PORT_DETACH_EXEC__EGR_BLOCK_PROFILE,
    PORT_DETACH_EXEC__PHY,
    PORT_DETACH_EXEC__DSCP_PROFILE,
    PORT_DETACH_EXEC__PORT_CFG_INIT,
    PORT_DETACH_EXEC__EGR_VLAN_ACTION,
    PORT_DETACH_EXEC__SOFTWARE,

    PORT_DETACH_EXEC__MAX
} bcmi_xgs5_port_detach_exec_seq_t;

#define BCMI_XGS5_PORT_DETACH_PHASES_MSG    {\
    "IPMC replication",         /* PORT_DETACH_EXEC__IPMC_REPL */ \
    "IPMC",                     /* PORT_DETACH_EXEC__IPMC */ \
    "MPLS",                     /* PORT_DETACH_EXEC__MPLS */ \
    "MIM",                      /* PORT_DETACH_EXEC__MIM */ \
    "L3",                       /* PORT_DETACH_EXEC__L3 */ \
    "Mirror",                   /* PORT_DETACH_EXEC__MIRROR */ \
    "Field",                    /* PORT_DETACH_EXEC__FIELD */ \
    "Rate",                     /* PORT_DETACH_EXEC__RATE */ \
    "Stack",                    /* PORT_DETACH_EXEC__STACK */ \
    "COSQ",                     /* PORT_DETACH_EXEC__COSQ */ \
    "Trunk",                    /* PORT_DETACH_EXEC__TRUNK */ \
    "Vlan",                     /* PORT_DETACH_EXEC__VLAN */ \
    "STG",                      /* PORT_DETACH_EXEC__STG */ \
    "ASF",                      /* PORT_DETACH_EXEC__ASF */ \
    "Egress link delay",        /* PORT_DETACH_EXEC__EGR_LINK_DELAY */ \
    "HIGIG",                    /* PORT_DETACH_EXEC__HIGIG */ \
    "EEE config",               /* PORT_DETACH_EXEC__EEE */ \
    "Vlan protocol",            /* PORT_DETACH_EXEC__VLAN_PROTOCOL */ \
    "Egress block profile",     /* PORT_DETACH_EXEC__EGR_BLOCK_PROFILE */ \
    "PHY Detach",               /* PORT_DETACH_EXEC__PHY */ \
    "DSCP profile",             /* PORT_DETACH_EXEC__DSCP_PROFILE */ \
    "Port config init",         /* PORT_DETACH_EXEC__PORT_CFG_INIT */ \
    "Egress Vlan action",       /* PORT_DETACH_EXEC__EGR_VLAN_ACTION */ \
    "Software",                 /* PORT_DETACH_EXEC__SOFTWARE */ \
    "Unkown phase"              /* PORT_DETACH_EXEC__MAX */ \
    }

#define BCMI_XGS5_PORTS_PER_PBLK        4 /* Num of ports per port block */
#define BCMI_XGS5_PORTS_PER_PM8X50_PBLK 8 /* Num of ports per PM8x50 block */

/*
 * Flexport restriction
 */
#define BCMI_XGS5_FLEXPORT_RESTRICTION_MIX_LR_OVS_DEV       (1 << 0)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_MIX_LR_OVS_PM        (1 << 1)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_PIPE_BANDWIDTH       (1 << 2)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_SPEED_CLASS          (1 << 3)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_PM_OVS_MIX_SISTER    (1 << 4)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_PM_SINGLE_PLL        (1 << 5)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_PM_MIX_ETH_HIGIG     (1 << 6)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_PORT_RATIO           (1 << 7)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_LR_OVS               (1 << 8)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_PM_HG_MIX_SPEED      (1 << 9)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_PM_LR_MIX_SISTER     (1 << 10)
#define BCMI_XGS5_FLEXPORT_RESTRICTION_PM_DOUBLE_PLL        (1 << 11)

/*
 * Port Ratio
 */
#define BCMI_XGS5_PORT_RATIO_SINGLE            (1 << 0)
#define BCMI_XGS5_PORT_RATIO_DUAL_1_1          (1 << 1)
#define BCMI_XGS5_PORT_RATIO_DUAL_2_1          (1 << 2)
#define BCMI_XGS5_PORT_RATIO_DUAL_1_2          (1 << 3)
#define BCMI_XGS5_PORT_RATIO_TRI_023_2_1_1     (1 << 4)
#define BCMI_XGS5_PORT_RATIO_TRI_023_4_1_1     (1 << 5)
#define BCMI_XGS5_PORT_RATIO_TRI_012_1_1_2     (1 << 6)
#define BCMI_XGS5_PORT_RATIO_TRI_012_1_1_4     (1 << 7)
#define BCMI_XGS5_PORT_RATIO_QUAD              (1 << 8)

/* Lanes support */
#define BCMI_XGS5_PORT_LANES_1      (1 << 0)
#define BCMI_XGS5_PORT_LANES_2      (1 << 1)
#define BCMI_XGS5_PORT_LANES_4      (1 << 2)
#define BCMI_XGS5_PORT_LANES_10     (1 << 3)
#define BCMI_XGS5_PORT_LANES_12     (1 << 4)
#define BCMI_XGS5_PORT_LANES_8      (1 << 5)


/*
 * Physical Port Information
 */
typedef struct bcmi_xgs5_phy_port_s {
    uint32               lanes_valid;  /* Lanes capabilities */
    int                  pipe;
    int                  flex;         /* Flex enable by SOC property
                                          'port_flex_enable' */
    int                  force_lb;
    int                  max_lane_speed;
    int                  force_hg;     /* Port forced to hg mode only, per TSC port */
} bcmi_xgs5_phy_port_t;

typedef struct bcmi_xgs5_speed_class_s {
    uint32  speed_class_num; /* Max speed class num */
    uint32  no_mix_speed_mask; /* Not supported combination of speed classes */
} bcmi_xgs5_speed_class_t;

/*
 * Physical Device information
 */
typedef struct bcmi_xgs5_dev_info_s {
    int     num_pipe;  /* Number of pipes in device */
    int     num_tsc;   /* Number of TSCs in device, not including management TSC */
    int     phy_ports_max;   /* Max physical ports in device */
    int     ports_pipe_max[SOC_MAX_NUM_PIPES];  /* Max logical ports per pipe */
    int     mmu_lossless;    /* Default MMU lossless */
    int     asf_prof_default;/* Default ASF MEM profile */

    /* Logical port boundary for each pipe */
    int     pipe_bound; /* Indicate if have logic port boundary for pipe */
    int     pipe_log_port_base[SOC_MAX_NUM_PIPES];
    int     pipe_phy_port_base[SOC_MAX_NUM_PIPES];

    /* Ethernet speed and related Higig speed share same speed class in TDM */
    int     speed_unify; /* Indicate that schedulers handle Ethernet ports
                          * and the related HiGig port speed identically */

    int     tdm_speed_min; /* Min speed involved in DV TDM code (Mbps) */

    /* The Max packet size that is used in statistic counter update*/
    int     cntmaxsize_xl; /* default value for XLPORT */
    int     cntmaxsize_cl; /* default value for CLPORT */

    /* Restrictions */
    uint32  restriction_mask;
    uint32  encap_mask;
    uint32  hg_encap_mask; /* Supported Higig encap mask */
    uint32  port_ratio_mask;
    uint8   aux_port_flexible; /* 1: Auxiliary ports support FlexPort opertions
                                * 0: Auxiliary ports does NOT support FlexPort
                                */
    pbmp_t  aux_pbm; /* Port bitmap of auxiliary ports */
    uint32  pipe_lr_bw; /* Max line rate bandwidth (Mbps) */
    int     tdm_pipe_lr_bw_limit_checker_speed_min; /* Min speed used in
                                                     * line rate bandwidth limit
                                                     * check calculations for
                                                     * TDM (Mbps)
                                                     */
    uint32  speed_valid[SOC_PORT_RESOURCE_LANES_MAX+1];
    bcmi_xgs5_speed_class_t speed_class;
    bcmi_xgs5_phy_port_t *phy_port_info;

    /* Bitmap of modules initialized in _bcm_modules_init */
    SHR_BITDCL init_cond[_SHR_BITDCLSIZE(SHR_BPROF_STATS_MAX)];

    /* Enable to do entire FlexPort sequence for encap change
     * operation i.e BCMI_XGS5_PORT_RESOURCE_OP_ENCAP
     */
    uint8 encap_change_flex;
} bcmi_xgs5_dev_info_t;

/* Device programming routines */
typedef struct bcmi_xgs5_port_func_s {
    int (*reconfigure_ports)(int unit, 
                             soc_port_schedule_state_t *flexport_info); /*DV provide*/
    int (*soc_resource_init)(int unit, int nport,
                             bcm_port_resource_t *resource,
                             soc_port_resource_t *soc_resource);
    int (*port_resource_validate)(int unit,
                             soc_port_schedule_state_t *port_schedule_state);
    int (*pre_flexport_tdm)(int unit,
                             soc_port_schedule_state_t *port_schedule_state);
    int (*post_flexport_tdm)(int unit,
                             soc_port_schedule_state_t *port_schedule_state);
    int (*port_macro_update)(int unit,
                             soc_port_schedule_state_t *port_schedule_state);
    int (*port_enable)(int unit,
                             soc_port_schedule_state_t *port_schedule_state);
    int (*port_disable)(int unit,
                             soc_port_schedule_state_t *port_schedule_state);
    int (*no_tdm_speed_update)(int unit, bcm_port_t port, int speed);
    int (*speed_ability_get)(int unit, bcm_port_t port, bcm_port_abil_t *mask);
    int (*speed_mix_validate)(int unit, uint32 speed_mask);
    int (*port_cosmap_update)(int unit, pbmp_t pbm, int enable);

    int (*port_attach_exec[PORT_ATTACH_EXEC__MAX])(int unit, bcm_port_t port);
    int (*port_detach_exec[PORT_DETACH_EXEC__MAX])(int unit, bcm_port_t port);

} bcmi_xgs5_port_func_t;

/* HW Definitions */
typedef struct bcmi_xgs5_port_drv_s {
    bcmi_xgs5_port_func_t   *port_calls;/* Chip programming */
    bcmi_xgs5_dev_info_t    *dev_info[SOC_MAX_NUM_DEVICES];/* Device info */
} bcmi_xgs5_port_drv_t;

#ifdef BCM_XGS5_SWITCH_PORT_SUPPORT
extern int
bcmi_xgs5_port_fn_drv_init(int unit, bcm_esw_port_drv_t *drv,
                           bcmi_xgs5_port_drv_t *bcmi_drv,
                           bcmi_xgs5_port_hw_defs_t *hw_defs);

extern int
bcmi_xgs5_port_addressable_local_get(int unit,
                                     bcm_port_t port, bcm_port_t *local_port);
extern int
bcmi_xgs5_port_resource_set(int unit,
                            bcm_gport_t port, bcm_port_resource_t *resource);
extern int
bcmi_xgs5_port_resource_get(int unit, 
                            bcm_gport_t port, bcm_port_resource_t *resource);
extern int
bcmi_xgs5_port_resource_multi_set(int unit, 
                                  int nport, bcm_port_resource_t *resource);
extern int
bcmi_xgs5_port_resource_traverse(int unit, 
                                 bcm_port_resource_traverse_cb trav_fn, 
                                 void *user_data);
extern int
bcmi_xgs5_port_lanes_set(int unit, bcm_port_t port, int lanes);

extern int
bcmi_xgs5_port_lanes_get(int unit, bcm_port_t port, int *lanes);

extern int
bcmi_xgs5_port_speed_set(int unit, bcm_port_t port, int speed);

extern int
bcmi_xgs5_port_resource_status_update(int unit,
                                      int nport,
                                      bcm_port_resource_t *resource);

extern int
bcmi_xgs5_port_resource_speed_multi_set(int unit,
                                         int nport,
                                         bcm_port_resource_t *resource);

/*
 * Port attach
 */
extern int
bcmi_xgs5_port_attach_dscp(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_dscp(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_software(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_egr_vlan_action(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_cfg_init(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_port_probe(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_frame_length_check(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_rcpu_mtu(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_outer_tpid(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_egr_block_profile_init(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_vlan_protocol(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_eee(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_higig(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_port_info_cfg(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_egr_link_delay(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_stg(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_vlan(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_trunk(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_linkscan(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_stat(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_stack(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_rate(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_field(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_mirror(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_l3(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_ipmc(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_mpls(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_attach_mim(int unit, bcm_port_t port);

/*
 * Port Detach
 */
extern int
bcmi_xgs5_port_detach_dscp(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_software(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_egr_vlan_action(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_phy(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_vlan_protocol(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_eee(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_higig(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_egr_link_delay(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_stg(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_vlan(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_trunk(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_stat(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_stack(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_rate(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_field(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_mirror(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_l3(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_ipmc(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_mpls(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_detach_mim(int unit, bcm_port_t port);

extern int
bcmi_xgs5_port_enable_set(int unit, bcm_port_t port, int enable);

extern int
bcmi_xgs5_flexport_based_speed_ability_get(int unit, bcm_port_t port,
                                           bcm_port_abil_t *mask);

extern int
bcmi_xgs5_port_encap_speed_check(int unit, bcm_port_t port, int encap,
                                 int speed);

extern void
bcmi_esw_port_redirect_config_t_init(
    bcm_port_redirect_config_t *redirect_config);

extern int
bcmi_xgs5_port_redirect_config_set(int unit, bcm_gport_t port,
                              bcm_port_redirect_config_t *redirect_config);

extern int
bcmi_xgs5_port_redirect_config_get(int unit, bcm_gport_t port,
                                   bcm_port_redirect_config_t *redirect_config);
extern int
bcmi_xgs5_port_force_lb_set(int unit);

extern int
bcmi_xgs5_port_force_lb_check(int unit, int port, int loopback);

/* Debug functions for Flexport */
extern int
bcmi_xgs5_num_tsc(int unit, int *num);

extern int
bcmi_xgs5_phy_to_pipe(int unit, int phy_port, int *pipe);

extern int
bcmi_xgs5_port_range(int unit, int pipe, int *port_start, int *port_end);

extern int
bcmi_xgs5_speed_to_lanes(int unit, int lane_max, int *speed, int *lanes);

#endif /* BCM_XGS5_SWITCH_PORT_SUPPORT */

#if defined (BCM_EP_REDIRECT_VERSION_2)
extern int
bcmi_xgs5_rx_CopyToCpu_config_add(int unit, uint32 options,
                                bcm_rx_CopyToCpu_config_t *copyToCpu_config);

extern int
bcmi_xgs5_rx_CopyToCpu_config_get(int unit, int index,
                                bcm_rx_CopyToCpu_config_t *copyToCpu_config);

extern int
bcmi_xgs5_rx_CopyToCpu_config_delete(int unit, int index);

extern int
bcmi_xgs5_rx_CopyToCpu_config_get_all(int unit, int entries_max,
                                 bcm_rx_CopyToCpu_config_t *copyToCpu_config,
                                 int *entries_count);
extern int
bcmi_xgs5_rx_CopyToCpu_config_delete_all(int unit);

extern int
bcmi_rx_copyToCpu_drop_reason_field_get(int unit,
                                            bcm_pkt_drop_reason_t drop_reason,
                                            soc_field_t *drop_reason_field);
#endif
typedef enum _bcmi_mpls_entry_format_fields_e {
    bcmiMplsLabel,
    bcmiMplsPushAction,
    bcmiMplsExpSelect,
    bcmiMplsExpMappingPtr,
    bcmiNewCfi,
    bcmiNewPri,
    bcmiMplsExp,
    bcmiLabelSelect,
    bcmiMplsTtl,
    bcmiSpecialLabelPushType,
    bcmiExpMappingTableSelect,
    bcmiPktEcnToExpMappingPtr,
    bcmiPktEcnToExpPriority,
    bcmiEgrFlexCtrActionSet,
    bcmiFlexCtrBaseCounterIdx,
    bcmiFlexCtrPoolNumber,
    bcmiRsvdFlexCtrPoolNumber,
    bcmiFlexCtrOffsetMode,
    bcmiEcnMappingEnable,
    bcmiEcnMappingTableSelect,
    bcmiMplsEntryFormatReserved

}_bcmi_mpls_entry_format_fields_t;

extern int
bcmi_egr_ip_tnl_mpls_entry_format_set(int unit, int field_type, void *tnl_buf,
        int lbl_offset, uint32 *retbuf,
        int set_value);

extern int
bcmi_egr_ip_tnl_mpls_entry_format_get(int unit, void *tnl_buf, int lbl_offset,
        uint32 *inbuf, int field_type,
        uint32 *retval);

extern void
bcm_td3_ip_tnl_mpls_label_info_get(int unit, void *tnl_buf, int type,
        int offset, int *val);

extern int
bcmi_egress_tnl_label_entry_get(int unit, void *tnl_buf,
        int offset, uint32 *data_buf);

extern int
bcmi_egress_tnl_label_entry_set(int unit, void *tnl_buf,
        int offset, uint32 *data_buf);

#if defined(INCLUDE_FLOWTRACKER)
/* Flowtracker functions */
extern int bcm_xgs5_ft_eapp_init(int unit);
extern int bcm_xgs5_ft_eapp_detach(int unit);

extern int
_bcm_xgs5_ft_template_list_template_add(int unit,
                           bcm_flowtracker_export_template_t template_id,
                           _bcm_int_ft_export_template_info_t *template_int_info);
extern int
_bcm_xgs5_ft_template_list_template_delete (int unit,
                              bcm_flowtracker_export_template_t template_id);
extern int
_bcm_xgs5_ft_template_list_template_modify(
                              int unit,
                              bcm_flowtracker_export_template_t template_id,
                              _bcm_int_ft_export_template_info_t *template_info);

extern int
_bcm_xgs5_ft_template_list_template_get (int unit,
                              bcm_flowtracker_export_template_t template_id,
                              _bcm_int_ft_export_template_info_t *template_info);
extern int
_bcm_xgs5_ft_template_list_destroy (int unit);

extern int
_bcm_xgs5_ft_collector_list_collector_add(int unit,
                           bcm_flowtracker_collector_t collector_id,
                           bcm_flowtracker_collector_info_t *collector_info);
extern int
_bcm_xgs5_ft_collector_list_collector_delete (int unit,
                              bcm_flowtracker_collector_t collector_id);
extern int
_bcm_xgs5_ft_collector_list_collector_modify(int unit,
                           bcm_flowtracker_collector_t collector_id,
                           bcm_flowtracker_collector_info_t *collector_info);
extern int
_bcm_xgs5_ft_collector_list_collector_get (int unit,
                              bcm_flowtracker_collector_t collector_id,
                              _bcm_int_ft_collector_info_t *collector_info);
extern int
_bcm_xgs5_ft_collector_list_destroy (int unit);

extern int
_bcm_xgs5_ft_v2_collector_list_collector_add(int unit,
                                             bcm_collector_t collector_id,
                                             int coll_int_id);
extern int
_bcm_xgs5_ft_v2_collector_list_collector_delete(int unit,
                                                bcm_collector_t collector_id);
extern int
_bcm_xgs5_ft_v2_collector_list_collector_get(int unit,
                                             bcm_collector_t collector_id,
                                             int *coll_int_id);
extern void
_bcm_xgs5_ft_v2_collector_list_collector_in_use(int unit,
                                                bcm_collector_t collector_id,
                                                int *in_use);
extern int
_bcm_xgs5_ft_v2_collector_list_export_profile_add(int unit,
                                                  int export_profile_id,
                                                  int export_profile_int_id);
extern int
_bcm_xgs5_ft_v2_collector_list_export_profile_delete(int unit,
                                                     int export_profile_id);

extern int
_bcm_xgs5_ft_v2_collector_list_export_profile_get(int unit,
                                                  int export_profile,
                                                  int *export_profile_int_id);
extern void
_bcm_xgs5_ft_v2_collector_list_export_profile_in_use(int unit,
                                                     int export_profile_id,
                                                     int *in_use);

extern int
_bcm_xgs5_ft_flow_group_list_flow_group_add(int unit,
                                            bcm_flowtracker_group_t flow_group_id);
extern int
_bcm_xgs5_ft_flow_group_list_flow_group_delete (int unit,
                              bcm_flowtracker_group_t flow_group_id);
extern int
_bcm_xgs5_ft_flow_group_list_flow_group_modify(int unit,
                           bcm_flowtracker_group_t flow_group_id,
                           _bcm_int_ft_flow_group_info_t *flow_group_int_info);
extern int
_bcm_xgs5_ft_flow_group_list_flow_group_get (int unit,
                              bcm_flowtracker_group_t flow_group_id,
                              _bcm_int_ft_flow_group_info_t *flow_group_int_info);
extern int
_bcm_xgs5_ft_flow_group_list_destroy (int unit);
extern int
_bcm_xgs5_ft_flow_group_list_collector_in_use_check(int unit,
                    bcm_flowtracker_collector_t collector_id);
extern int
_bcm_xgs5_ft_flow_group_list_template_in_use_check(int unit,
                    bcm_flowtracker_export_template_t template_id);
extern int
_bcm_xgs5_ft_flow_group_list_elph_profile_in_use_check(
                                   int unit,
                                   bcm_flowtracker_export_template_t profile_id);
extern int
_bcm_xgs5_ft_flow_group_list_get_all_ids(
                                 int unit,
                                 bcm_flowtracker_group_t *flow_group_id_list,
                                 int *num_flow_groups);

extern int _bcm_xgs5_ft_eapp_send_elph_profile_delete_msg (int unit,
                                                           int profile_id);

extern int _bcm_xgs5_ft_eapp_send_flow_group_create_msg (int unit,
                    bcm_flowtracker_group_t flow_group_id);
extern int _bcm_xgs5_ft_eapp_send_flow_group_delete_msg (int unit,
                    bcm_flowtracker_group_t flow_group_id);
extern int _bcm_xgs5_ft_eapp_send_flow_group_update_msg (int unit,
                    bcm_flowtracker_group_t flow_group_id,
                    uint32 update_flags, uint32 update_value);
extern int _bcm_xgs5_ft_eapp_send_flow_group_get_msg (int unit,
                    bcm_flowtracker_group_t flow_group_id,
                    uint32 *flow_count);
extern int _bcm_xgs5_ft_eapp_send_template_create_msg (int unit,
                    bcm_flowtracker_export_template_t template_id);
extern int _bcm_xgs5_ft_eapp_send_template_delete_msg (int unit,
                    bcm_flowtracker_export_template_t template_id);
extern int _bcm_xgs5_ft_eapp_send_collector_create_msg (int unit,
                                                        int collector_id,
                                                        int export_profile_id);
extern int _bcm_xgs5_ft_eapp_send_collector_delete_msg (int unit,
                                                        uint16 collector_id);
extern int _bcm_xgs5_ft_eapp_send_em_key_format_msg (int unit,
            shr_ft_msg_ctrl_em_key_format_t *em_key_format);

extern int
_bcm_xgs5_ft_eapp_send_em_group_create_msg(int unit,
                                           shr_ft_msg_ctrl_em_group_create_t *msg);

extern int
_bcm_xgs5_ft_eapp_send_em_group_delete_msg(int unit,
                                           int em_group_id);

extern int
_bcm_xgs5_ft_eapp_send_flow_group_flow_data_get_msg(
                                         int unit,
                                         bcm_flowtracker_group_t flow_group_id,
                                         bcm_flowtracker_flow_key_t *flow_key,
                                         bcm_flowtracker_flow_data_t *flow_data);
extern int
_bcm_xgs5_ft_eapp_send_ser_event_msg(int unit, int pipe, soc_mem_t mem, int index);

extern int
_bcm_xgs5_ft_eapp_send_template_xmit_start(int unit,
                                  bcm_flowtracker_export_template_t template_id,
                                  bcm_flowtracker_collector_t collector_id);

extern int
_bcm_xgs5_ft_eapp_send_group_actions_set_msg(int unit,
                                             shr_ft_msg_ctrl_group_actions_set_t *msg);

extern int
_bcm_xgs5_ft_elph_profile_list_add(int unit,
                                   bcm_flowtracker_elephant_profile_t id,
                                   bcm_flowtracker_elephant_profile_info_t *profile);

extern int
_bcm_xgs5_ft_eapp_send_elph_profile_create_msg(int unit, int profile_id);

extern int
_bcm_xgs5_ft_eapp_send_export_interval_update_msg (int unit,
                                                 uint32 export_interval);

extern int
_bcm_xgs5_ft_elph_profile_list_get(int unit,
                                   bcm_flowtracker_elephant_profile_t id,
                                   _bcm_int_ft_elph_profile_info_t *profile);

extern int
_bcm_xgs5_ft_elph_profile_list_delete(int unit,
                                      bcm_flowtracker_elephant_profile_t id);

extern void _bcm_xgs5_flowtracker_sw_dump(int unit);
extern void _bcm_xgs5_flowtracker_dump_stats_learn(int unit);
extern void _bcm_xgs5_flowtracker_dump_stats_export(int unit);
extern void _bcm_xgs5_flowtracker_dump_stats_age(int unit);
extern void _bcm_xgs5_flowtracker_dump_stats_elephant(int unit);
extern void _bcm_xgs5_qcm_sw_dump(int unit);
extern void _bcm_xgs5_qcm_stats_enable(int unit, uint8 enable);
extern void _bcm_xgs5_qcm_dump_stats_learn(int unit);
extern void _bcm_xgs5_qcm_dump_stats_export(int unit);
extern void _bcm_xgs5_qcm_dump_stats_age(int unit);
extern void _bcm_xgs5_qcm_dump_stats_scan(int unit);

extern int _bcm_xgs5_ft_qcm_eapp_cfg_init_set(int unit,
        _bcm_esw_ft_qcm_init_cfg_t cfg_init);

extern int _bcm_xgs5_ft_qcm_eapp_host_buf_set(int unit,
        uint32 host_mem_size, uint32 share_mem_add);
extern int _bcm_xgs5_ft_qcm_eapp_host_buf_get(int unit,
        uint32 *host_mem_size, uint32 **host_mem_add);

extern int _bcm_xgs5_ft_qcm_eapp_config_set(int unit,
        int num_gports, _bcm_esw_ft_qcm_port_cfg_t *cfg_info);
extern int _bcm_xgs5_ft_qcm_eapp_config_get(int unit,
        int max_gports, bcm_gport_t *gport_list, int *num_gports);

extern int _bcm_xgs5_ft_qcm_eapp_flow_view_cfg_set(int unit, uint32 options,
        bcm_cosq_burst_monitor_flow_view_info_t *flow_view_data);
extern int _bcm_xgs5_ft_qcm_eapp_flow_view_cfg_get(int unit,
        bcm_cosq_burst_monitor_flow_view_info_t *flow_view_data);

extern int _bcm_xgs5_ft_qcm_eapp_flow_view_data_clear(int unit);
extern int _bcm_xgs5_ft_qcm_eapp_flow_view_data_get(int unit,
        uint32 usr_mem_size, uint8 *usr_mem_add);

extern int _bcm_xgs5_ft_qcm_eapp_get_current_time(int unit, uint64 *time_usecs);
extern int _bcm_xgs5_ft_qcm_eapp_view_summary_get(int unit, uint32 mem_size,
        uint8 *mem_addr, int max_num_views, int *num_views);
extern int _bcm_xgs5_ft_qcm_eapp_flow_view_stats_get(int unit, uint32 mem_size,
        uint8 *mem_addr, int max_num_flows, int view_id, uint32 flags,
        int *num_flows);

extern int _bcm_xgs5_ft_qcm_eapp_send_em_group_create_msg(int unit,
        shr_ft_qcm_msg_ctrl_em_key_format_t *msg);

extern int _bcm_xgs5_ft_qcm_eapp_send_flow_group_flow_data_get_msg(
                                         int unit,
                                         bcm_flowtracker_group_t flow_group_id,
                                         bcm_flowtracker_flow_key_t *flow_key);
#endif /* INCLUDE_FLOWTRACKER */

#if defined(INCLUDE_IFA)
/* IFA module APIs */
extern int bcm_xgs5_ifa_eapp_init(int unit);
extern int bcm_xgs5_ifa_eapp_detach(int unit);
extern int bcm_xgs5_ifa_collector_set(int unit,
        bcm_ifa_collector_info_t *ifa_collector_info);
extern int bcm_xgs5_ifa_collector_get(int unit,
        bcm_ifa_collector_info_t *ifa_collector_info);
extern int bcm_xgs5_ifa_collector_modify(int unit,
        bcm_ifa_collector_info_t *ifa_collector_info);
extern int bcm_xgs5_ifa_eapp_collector_set_msg(int unit,
        bcm_ifa_collector_info_t *ifa_collector_info);
extern int bcm_xgs5_ifa_config_set(int unit,
        bcm_ifa_config_info_t *ifa_config_info);
extern int bcm_xgs5_ifa_config_get(int unit,
        bcm_ifa_config_info_t *ifa_config_info);
extern int bcm_xgs5_ifa_config_modify(int unit,
        bcm_ifa_config_info_t *ifa_config_info);
extern int bcm_xgs5_ifa_eapp_config_info_msg(int unit,
        bcm_ifa_config_info_t *ifa_config_info);
extern int bcm_xgs5_ifa_eapp_collector_info_get_msg(int unit,
        bcm_ifa_collector_info_t *ifa_collector_info);
extern int bcm_xgs5_ifa_eapp_config_info_get_msg(int unit,
        bcm_ifa_config_info_t *config_info);
extern int bcm_xgs5_ifa_eapp_stat_info_get_msg(int unit,
        bcm_ifa_stat_info_t *stat_data);
extern int bcm_xgs5_ifa_eapp_stat_info_set_msg(int unit,
        bcm_ifa_stat_info_t *stat_data);

/* IFAv2 APIs. */
extern int _bcm_xgs5_ifa2_init(int unit, uint8 *success);
extern int _bcm_xgs5_ifa2_detach(int unit);
extern int _bcm_xgs5_ifa2_config_info_set(int unit,
                                          bcm_ifa_config_info_t *cfg_info);
extern int _bcm_xgs5_ifa2_config_info_get(int unit,
                                          bcm_ifa_config_info_t *cfg_info);
extern int _bcm_xgs5_ifa2_stat_info_set(int unit, bcm_ifa_stat_info_t *stats);
extern int _bcm_xgs5_ifa2_stat_info_get(int unit, bcm_ifa_stat_info_t *stats);
extern int _bcm_xgs5_ifa2_collector_attach(int unit,
                                           bcm_collector_t collector_id,
                                           int export_profile_id,
                                           bcm_ifa_export_template_t template_id);
extern int _bcm_xgs5_ifa2_collector_attach_get(int unit,
                                               bcm_collector_t *collector_id,
                                               int *export_profile_id,
                                               bcm_ifa_export_template_t *template_id);
extern int _bcm_xgs5_ifa2_collector_detach(int unit,
                                           bcm_collector_t collector_id,
                                           int export_profile_id,
                                           bcm_ifa_export_template_t template_id);
extern int _bcm_xgs5_ifa2_export_template_create(int unit,
                                                 uint32 options,
                                                 bcm_ifa_export_template_t *id,
                                                 uint16 set_id,
                                                 int num_export_elements,
                                                 bcm_ifa_export_element_info_t *list_of_export_elements);

extern int _bcm_xgs5_ifa2_export_template_get(int unit,
                                              bcm_ifa_export_template_t id,
                                              uint16 *set_id,
                                              int max_size,
                                              bcm_ifa_export_element_info_t *list_of_export_elements,
                                              int *list_size);

extern int _bcm_xgs5_ifa2_export_template_destroy(int unit,
                                                  bcm_ifa_export_template_t id);

extern int _bcm_xgs5_ifa2_template_transmit_config_set(int unit,
                                                       bcm_ifa_export_template_t template_id,
                                                       bcm_collector_t collector_id,
                                                       bcm_ifa_template_transmit_config_t *config);

extern int _bcm_xgs5_ifa2_template_transmit_config_get(int unit,
                                                       bcm_ifa_export_template_t template_id,
                                                       bcm_collector_t collector_id,
                                                       bcm_ifa_template_transmit_config_t *config);

extern int _bcm_xgs5_ifa2_sync(int unit);
extern int _bcm_xgs5_ifa2_appl_callback(int unit, int uC,
                                        soc_cmic_uc_shutdown_stage_t stage,
                                        void *user_data);
#endif /* INCLUDE_IFA */
#endif /* _BCM_INT_XGS5_H_ */
