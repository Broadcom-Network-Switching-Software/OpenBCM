/*! \file virtual.h
 *
 * Virtual resource management header file to declare internal APIs for XGS
 * devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef XGS_LTSW_VIRTUAL_H
#define XGS_LTSW_VIRTUAL_H

#include <sal/sal_types.h>

extern int
xgs_ltsw_virtual_index_get(int unit, int *vfi_min, int *vfi_max, int *vp_min,
                           int *vp_max, int *l2_iif_min, int * l2_iif_max);

extern int
xgs_ltsw_virtual_vfi_init(int unit, int vfi);

extern int
xgs_ltsw_virtual_init(int unit);

extern int
xgs_ltsw_virtual_vfi_cleanup(int unit, int vfi);

extern int
xgs_ltsw_virtual_port_learn_set(int unit, bcm_port_t port, uint32_t flags);

extern int
xgs_ltsw_virtual_port_learn_get(int unit, bcm_port_t port, uint32_t *flags);

extern int
xgs_ltsw_virtual_port_vlan_action_set(int unit, bcm_port_t port,
                                      bcm_vlan_action_set_t *action);

extern int
xgs_ltsw_virtual_port_vlan_action_get(int unit, bcm_port_t port,
                                      bcm_vlan_action_set_t *action);
extern int
xgs_ltsw_virtual_port_vlan_action_reset(int unit, bcm_port_t port);

extern int
xgs_ltsw_virtual_port_class_get(int unit, bcm_port_t port,
                                bcm_port_class_t pclass,
                                uint32_t *pclass_id);

extern int
xgs_ltsw_virtual_port_class_set(int unit, bcm_port_t port,
                                bcm_port_class_t pclass,
                                uint32_t pclass_id);

extern int
xgs_ltsw_virtual_port_vlan_ctrl_get(int unit, bcm_port_t port,
                                    bcm_vlan_control_port_t type,
                                    int *arg);
extern int
xgs_ltsw_virtual_port_vlan_ctrl_set(int unit, bcm_port_t port,
                                    bcm_vlan_control_port_t type,
                                    int arg);

extern int
xgs_ltsw_virtual_port_mirror_ctrl_get(int unit, bcm_port_t port,
                                      int *enable, int *mirror_idx);

extern int
xgs_ltsw_virtual_port_mirror_ctrl_set(int unit, bcm_port_t port,
                                      int enable, int *mirror_idx);

extern int
xgs_ltsw_virtual_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode);

extern int
xgs_ltsw_virtual_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode);

extern int
xgs_ltsw_virtual_port_phb_vlan_map_get(int unit, bcm_port_t port, int *map_ptr);

extern int
xgs_ltsw_virtual_port_phb_vlan_map_set(int unit, bcm_port_t port, int map_ptr);

extern int
xgs_ltsw_virtual_port_phb_dscp_map_get(int unit, bcm_port_t port, int *map_ptr);

extern int
xgs_ltsw_virtual_port_phb_dscp_map_set(int unit, bcm_port_t port, int map_ptr);

extern int
xgs_ltsw_virtual_port_phb_vlan_remark_map_get(int unit, bcm_port_t port,
                                               int *map_ptr);

extern int
xgs_ltsw_virtual_port_phb_vlan_remark_map_set(int unit, bcm_port_t port,
                                               int map_ptr);
extern int
xgs_ltsw_virtual_nw_group_num_get(int unit, int *num);

extern int
xgs_ltsw_virtual_port_control_get(int unit, bcm_port_t port,
                                  bcm_port_control_t type, int *value);

extern int
xgs_ltsw_virtual_port_control_set(int unit, bcm_port_t port,
                                  bcm_port_control_t type, int value);

extern int
xgs_ltsw_virtual_port_stat_attach(int unit, bcm_port_t port,
                                  uint32_t stat_counter_id);

extern int
xgs_ltsw_virtual_port_stat_detach_with_id(int unit, bcm_port_t port,
                                          uint32_t stat_counter_id);

extern int
xgs_ltsw_virtual_network_group_split_config_set(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config);

extern int
xgs_ltsw_virtual_network_group_split_config_get(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config);

#endif /* XGS_LTSW_VIRTUAL_H */
