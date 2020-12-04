/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        portctrl.h
 * Purpose:     SDK Port Control Glue Layer
 */

#ifndef   _BCM_INT_PORTCTRL_H_
#define   _BCM_INT_PORTCTRL_H_

#include <sal/types.h>
#include <soc/types.h>
#include <soc/esw/portctrl.h>
#include <bcm/types.h>
#include <soc/higig.h>




/* Functions */
extern int bcmi_esw_portctrl_probe(int unit, bcm_gport_t port, int init_flag,
                                   void *add_info, int *okay);
extern int bcmi_esw_portctrl_probe_pbmp(int unit,
                                        pbmp_t pbmp, pbmp_t *okay_pbmp);

extern int bcmi_esw_portctrl_ability_get(int unit, bcm_gport_t port,
                                         bcm_port_ability_t *port_ability,
                                         bcm_port_abil_t *ability_mask);
extern int bcmi_esw_portctrl_speed_ability_local_get(int unit,
                                             bcm_gport_t port,
                                             int max_num_ability,
                                             bcm_port_speed_ability_t *abilities,
                                             int *actual_num_ability);

extern int bcmi_esw_portctrl_ability_remote_get(int unit, bcm_gport_t port,
                                                bcm_port_ability_t *ability,
                                                bcm_port_abil_t *ability_mask);

extern int bcmi_esw_portctrl_autoneg_ability_remote_get(int unit, bcm_gport_t port,
                                                        int max_num_ability,
                                                        bcm_port_speed_ability_t *abilities,
                                                        int *actual_num_ability);
extern int bcmi_esw_portctrl_enable_get(int unit,
                                        bcm_gport_t port, int *enable);
extern int bcmi_esw_portctrl_enable_set(int unit,
                                        bcm_gport_t port, int enable);
extern int bcmi_esw_portctrl_mac_enable_set(int unit,
                                            bcm_gport_t port, int enable);
extern int bcmi_esw_portctrl_phy_enable_set(int unit,
                                            bcm_gport_t port, int enable);

extern int bcmi_esw_portctrl_egress_queue_drain(int unit, bcm_gport_t port);
extern int bcmi_esw_portctrl_update(int unit, bcm_gport_t port, int link);
extern int bcmi_esw_tx_roe_portctrl_update(int unit, bcm_gport_t port, int link);
extern int bcmi_esw_rx_roe_portctrl_update(int unit, bcm_gport_t port, int link);

extern int bcmi_esw_portctrl_fault_get(int unit,
                                       bcm_gport_t port, uint32 *flags);

extern int bcmi_esw_portctrl_link_fault_get(int unit, bcm_gport_t port,
                                            int *local_fault,
                                            int *remote_fault);

extern int bcmi_esw_portctrl_loopback_get(int unit,
                                          bcm_gport_t port, int *loopback);
extern int bcmi_esw_portctrl_loopback_set(int unit,
                                          bcm_gport_t port, int loopback);

extern int bcmi_esw_portctrl_speed_get(int unit, bcm_gport_t port, int *speed);
extern int bcmi_esw_portctrl_speed_max(int unit, bcm_gport_t port, int *speed);
extern int bcmi_esw_portctrl_speed_set(int unit, bcm_gport_t port, int speed);

extern int bcmi_esw_portctrl_resource_speed_config_validate(int unit,
                            bcm_port_resource_t *resource, bcm_pbmp_t *pbmp);

extern int bcmi_esw_portctrl_speed_multi_set(int unit, int nport,
                                             bcm_port_resource_t *resource);

extern int bcmi_esw_portctrl_stk_my_modid_set(int unit, bcm_gport_t port,
                                              int my_modid);

extern int bcmi_esw_portctrl_pause_get(int unit, bcm_gport_t port,
                                       int *pause_tx, int *pause_rx);
extern int bcmi_esw_portctrl_pause_set(int unit, bcm_gport_t port,
                                       int pause_tx, int pause_rx);

extern int bcmi_esw_portctrl_pause_addr_get(int unit, bcm_gport_t port,
                                            bcm_mac_t mac);
extern int bcmi_esw_portctrl_pause_addr_set(int unit, bcm_gport_t port,
                                            bcm_mac_t mac);

extern int bcmi_esw_portctrl_eee_enable_get(int unit,
                                            bcm_gport_t port, int *enable);
extern int bcmi_esw_portctrl_eee_enable_set(int unit,
                                            bcm_gport_t port, int enable);

extern int bcmi_esw_portctrl_medium_config_set(int unit, bcm_gport_t port,
                                               bcm_port_medium_t medium,
                                               bcm_phy_config_t *config);
extern int bcmi_esw_portctrl_medium_config_get(int unit, bcm_gport_t port,
                                               bcm_port_medium_t medium,
                                               bcm_phy_config_t *config);
extern int bcmi_esw_portctrl_medium_get(int unit, bcm_gport_t port,
                                        bcm_port_medium_t *medium);

extern int bcmi_esw_portctrl_master_get(int unit, bcm_gport_t port, int *ms);
extern int bcmi_esw_portctrl_master_set(int unit, bcm_gport_t port, int ms);

extern int bcmi_esw_portctrl_interface_get(int unit, bcm_gport_t port,
                                           bcm_port_if_t *intf);
extern int bcmi_esw_portctrl_interface_set(int unit, bcm_gport_t port,
                                           bcm_port_if_t intf);

extern int bcmi_esw_portctrl_mdix_get(int unit, bcm_gport_t port,
                                      bcm_port_mdix_t *mode);
extern int bcmi_esw_portctrl_mdix_set(int unit, bcm_gport_t port,
                                      bcm_port_mdix_t mode);

extern int bcmi_esw_portctrl_mdix_status_get(int unit, bcm_gport_t port,
                                             bcm_port_mdix_status_t *status);
extern int bcmi_esw_portctrl_phy_get(int unit, bcm_gport_t port, uint32 flags,
                                     uint32 phy_reg_addr, uint32 *phy_data);
extern int bcmi_esw_portctrl_phy_multi_get(int unit, bcm_gport_t port,
                                           uint32 flags, uint32 dev_addr,
                                           uint32 offset, int max_size,
                                           uint8 *data, int *actual_size);
extern int bcmi_esw_portctrl_phy_modify(int unit, bcm_gport_t port,
                                        uint32 flags, uint32 phy_reg_addr,
                                        uint32 phy_data, uint32 phy_mask);
extern int bcmi_esw_portctrl_phy_set(int unit, bcm_gport_t port, uint32 flags,
                                     uint32 phy_reg_addr, uint32 phy_data);

extern int bcmi_esw_portctrl_phy_drv_name_get(int unit, bcm_gport_t port,
                                              char *name, int len);
extern int bcmi_esw_portctrl_phy_reset(int unit, bcm_gport_t port);

extern int bcmi_esw_portctrl_ability_advert_get(int unit, bcm_gport_t port,
                                     bcm_port_ability_t *port_ability,
                                     bcm_port_abil_t *ability_mask);
extern int bcmi_esw_portctrl_ability_advert_set(int unit, bcm_gport_t port,
                                     bcm_port_ability_t *port_ability,
                                     bcm_port_abil_t ability_mask);

extern int bcmi_esw_portctrl_autoneg_get(int unit, bcm_gport_t port,
                                         int *autoneg);
extern int bcmi_esw_portctrl_autoneg_set(int unit, bcm_gport_t port,
                                         int autoneg);

extern int bcmi_esw_portctrl_autoneg_ability_advert_get(int unit, bcm_gport_t port,
                                                        int max_num_ability,
                                                        bcm_port_speed_ability_t *abilities,
                                                        int *actual_num_ability);
extern int bcmi_esw_portctrl_autoneg_ability_advert_set(int unit, bcm_gport_t port,
                                                        int num_ability,
                                                        bcm_port_speed_ability_t *abilities);

extern int bcmi_esw_portctrl_duplex_get(int unit, bcm_gport_t port,
                                        int *duplex);
extern int bcmi_esw_portctrl_duplex_set(int unit, bcm_gport_t port,
                                        int duplex);
extern int bcmi_esw_portctrl_clear_rx_lss_status_set(int unit, bcm_gport_t port,
                                                     int local_fault,
                                                     int remote_fault);
extern int bcmi_esw_portctrl_clear_rx_lss_status_get(int unit, bcm_gport_t port,
                                                     int *local_fault,
                                                     int *remote_fault);

extern int bcmi_esw_portctrl_frame_max_get(int unit,
                                           bcm_gport_t port, int *size);
extern int bcmi_esw_portctrl_frame_max_set(int unit,
                                           bcm_gport_t port, int size);

extern int bcmi_esw_portctrl_encap_get(int unit, bcm_gport_t port, int *mode);
extern int bcmi_esw_portctrl_encap_set(int unit, bcm_gport_t port,
                                       int mode, int force);
extern int bcmi_esw_portctrl_encap_set_execute(int unit, bcm_gport_t port,
                                               int mode, int force);

extern int bcmi_esw_portctrl_encap_xport_set(int unit, bcm_gport_t port,
                                             int mode);
extern int bcmi_esw_portctrl_encap_higig_lite_set(int unit, bcm_gport_t port);
extern int bcmi_esw_portctrl_higig_mode_set(int unit, bcm_gport_t port,
                                            int higig_mode);
extern int bcmi_esw_portctrl_higig2_mode_set(int unit, bcm_gport_t port,
                                             int higig2_mode);

extern int bcmi_esw_portctrl_link_get(int unit, bcm_gport_t port,
                                      int hw, int *up);

extern int bcmi_esw_portctrl_mode_setup(int unit, bcm_gport_t port, int enable);
extern int bcmi_esw_portctrl_detach(int unit, pbmp_t pbmp, pbmp_t *detached);
extern int bcmi_esw_portctrl_ifg_set(int unit, bcm_gport_t port, int speed,
                                     bcm_port_duplex_t duplex, int ifg);
extern int bcmi_esw_portctrl_ifg_get(int unit, bcm_gport_t port, int speed,
                                     bcm_port_duplex_t duplex, int *ifg);
extern int bcmi_esw_portctrl_mac_rx_control(int unit, bcm_port_t port, uint8 optype, int *enable);

extern int bcmi_esw_portctrl_fast_reboot_mac_up(int unit, int nport, bcm_port_t *port);
extern int bcmi_esw_portctrl_mac_up(int unit, int nport, bcm_port_t *port);
extern int bcmi_esw_portctrl_mac_rx_down(int unit, int nport, bcm_port_t *port);
extern int bcmi_esw_portctrl_mac_tx_down(int unit, int nport, bcm_port_t *port);

extern int bcmi_esw_portctrl_llfc_get(int unit, bcm_gport_t port, bcm_port_control_t type, int *value);
extern int bcmi_esw_portctrl_llfc_set(int unit, bcm_port_t port, bcm_port_control_t type, int value);
extern int bcmi_esw_portctrl_lag_failover_disable(int unit, bcm_gport_t port);
extern int bcmi_esw_portctrl_lag_failover_loopback_get(int unit, bcm_gport_t port, int* value);
extern int bcmi_esw_portctrl_lag_failover_status_toggle(int unit, bcm_gport_t port);
extern int bcmi_esw_portctrl_lag_remove_failover_lpbk_set(int unit, bcm_gport_t port, int value);
extern int bcmi_esw_portctrl_trunk_hwfailover_set(int unit, bcm_gport_t port, int hw_count);

extern int bcmi_esw_td2_portctrl_lanes_set(int unit, bcm_port_t port_base, int lanes);

extern int bcmi_esw_portctrl_hwfailover_enable_set(int unit, bcm_gport_t port,
                                                   int enable);
extern int bcmi_esw_portctrl_hwfailover_enable_get(int unit, bcm_gport_t port,
                                                   int *enable);
extern int bcmi_esw_portctrl_hwfailover_status_get(int unit, bcm_gport_t port,
                                                   int *status);

extern int bcmi_esw_portctrl_phy_control_set(int unit, bcm_gport_t port,
                                   bcm_port_phy_control_t type, uint32 value);
extern int bcmi_esw_portctrl_phy_control_get(int unit, bcm_gport_t port,
                                   bcm_port_phy_control_t type, uint32 *value);

extern int bcmi_esw_portctrl_control_validate(int unit, bcm_port_control_t type,
                                              int *valid);
extern int bcmi_esw_portctrl_control_set(int unit, bcm_gport_t port,
                                         bcm_port_control_t type, int value);
extern int bcmi_esw_portctrl_control_get(int unit, bcm_gport_t port,
                                         bcm_port_control_t type, int *value);

extern int bcmi_esw_portctrl_mode_get(int unit, bcm_gport_t port, int *mode);

extern int bcmi_esw_portctrl_priority_group_config_set(int unit, bcm_gport_t port,
                                           int priority_group,
                                           bcm_port_priority_group_config_t *prigrp_config);

extern int bcmi_esw_portctrl_e2e_tx_enable_get(int unit, bcm_gport_t port,
                                             int *enable);
extern int bcmi_esw_portctrl_e2e_tx_enable_set(int unit, bcm_gport_t port,
                                             int enable);
extern int bcmi_esw_portctrl_e2ecc_hdr_get(int unit, bcm_gport_t port,
                                          soc_higig_e2ecc_hdr_t *e2ecc_hdr);
extern int bcmi_esw_portctrl_e2ecc_hdr_set(int unit, bcm_gport_t port,
                                          soc_higig_e2ecc_hdr_t *e2ecc_hdr);
extern int bcmi_esw_portctrl_vlan_tpid_set(int unit, bcm_gport_t port, int tpid);

extern int bcmi_esw_portctrl_vlan_inner_tpid_set(int unit, bcm_gport_t port, int tpid);


extern int bcmi_esw_portctrl_cntmaxsize_get(int unit, bcm_gport_t port,
                                            int *val);
extern int bcmi_esw_portctrl_cntmaxsize_set(int unit, bcm_gport_t port,
                                            int val);
extern int _bcm_esw_port_vrf_set(int unit, bcm_port_t port, int val);
extern int _bcm_esw_port_vrf_get(int unit, bcm_port_t port, int *val);
extern int  bcmi_esw_portctrl_cable_diag(int unit, bcm_port_t port,
                    bcm_port_cable_diag_t *status);
extern int
bcmi_esw_portctrl_preemption_status_tx_get(int unit, bcm_port_t port,
                                      uint32* value);

extern int
bcmi_esw_portctrl_preemption_status_verify_get(int unit, bcm_port_t port,
                                      uint32* value);
extern int
bcmi_esw_portctrl_preemption_mac_config_set(int unit, bcm_port_t port,
                                   bcm_port_preempt_control_t type,
                                   uint32 arg);
extern int
bcmi_esw_portctrl_preemption_mac_config_get(int unit, bcm_port_t port,
                                   bcm_port_preempt_control_t type,
                                   uint32* arg);
extern int
bcmi_esw_portctrl_timestamp_adjust_set(int unit, bcm_gport_t port,
                          bcm_port_timestamp_adjust_t *ts_adjust);
extern int
bcmi_esw_portctrl_timestamp_adjust_get(int unit, bcm_gport_t port,
                        bcm_port_timestamp_adjust_t *ts_adjust);
extern int
bcmi_esw_portctrl_phy_tx_set(int unit,
                             bcm_port_t port,
                             bcm_port_phy_tx_t *tx);
extern int
bcmi_esw_portctrl_phy_tx_get(int unit,
                             bcm_port_t port,
                             bcm_port_phy_tx_t *tx);
extern int
bcmi_esw_portctrl_speed_config_get(int unit,
                                   bcm_gport_t gport,
                                   void *speed_config);
extern int
bcmi_esw_portctrl_rlm_config_set(int unit,
                                 bcm_port_t port,
                                 bcm_port_rlm_config_t *rlm_config,
                                 int enable);
extern int
bcmi_esw_portctrl_rlm_config_get(int unit,
                                 bcm_port_t port,
                                 bcm_port_rlm_config_t *rlm_config,
                                 int *enable);
extern int
bcmi_esw_portctrl_rlm_status_get(int unit,
                                 bcm_port_t port,
                                 bcm_port_rlm_status_t *rlm_status);
extern int
bcmi_esw_portctrl_autoneg_status_get(int unit,
                                     bcm_gport_t port,
                                     int *enabled,
                                     int *locked);

#if defined(INCLUDE_PHY_542XX)
extern  int bcmi_esw_portctrl_serdes_link_update(int unit,
                                                 bcm_port_t port,
                                                 int link);
#endif /* defined(INCLUDE_PHY_542XX) */

extern int
bcmi_esw_portctrl_post_ber_proj_get(int unit, bcm_gport_t port,
                                    bcm_port_ber_proj_params_t *ber_proj,
                                    int max_errcnt,
                                    bcm_port_ber_proj_analyzer_errcnt_t *errcnt_array,
                                    int *actual_errcnt);
extern int
bcmi_esw_portctrl_portmod_thread_stop(int unit);

extern int
bcmi_esw_portctrl_phy_fec_error_inject_set(int unit, bcm_port_t port,
                                           uint16 error_control_map,
                                           bcm_port_phy_fec_error_mask_t bit_error_mask);
extern int
bcmi_esw_portctrl_phy_fec_error_inject_get(int unit, bcm_port_t port,
                                           uint16 *error_control_map,
                                           bcm_port_phy_fec_error_mask_t *bit_error_mask);
#endif /* _BCM_INT_PORTCTRL_H_ */
