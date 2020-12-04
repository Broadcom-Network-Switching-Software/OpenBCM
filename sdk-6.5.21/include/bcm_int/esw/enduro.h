/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        enduro.h
 * Purpose:     Function declarations for Enduro  bcm functions
 */

#ifndef _BCM_INT_ENDURO_H_
#define _BCM_INT_ENDURO_H_
#include <bcm_int/esw/l2.h>
#ifdef BCM_FIELD_SUPPORT 
#include <bcm_int/esw/field.h>
#endif /* BCM_FIELD_SUPPORT */
#if defined(BCM_ENDURO_SUPPORT)
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>


#if defined(BCM_KATANA_SUPPORT) && defined(INCLUDE_BHH)
extern int bcm_en_oam_loopback_delete(int unit, bcm_oam_loopback_t *loopback);
extern int bcm_en_oam_loopback_add(int unit, bcm_oam_loopback_t *loopback);
extern int bcm_en_oam_loopback_get(int unit, bcm_oam_loopback_t *loopback);
extern int bcm_en_oam_csf_add(int unit, bcm_oam_csf_t *csf_ptr);
extern int bcm_en_oam_csf_delete(int unit, bcm_oam_csf_t *csf_ptr);
extern int bcm_en_oam_csf_get(int unit, bcm_oam_csf_t *csf_ptr);
extern int bcm_en_oam_endpoint_faults_multi_get(int unit, uint32 flags,
                          bcm_oam_protocol_type_t endpoint_protocol,
                          uint32 max_endpoints, bcm_oam_endpoint_fault_t *faults,
                          uint32 *endpoint_count);
extern int bcm_en_oam_trunk_ports_add(int unit, bcm_gport_t trunk_gport,
                                      int max_ports, bcm_gport_t *port_arr);
extern int bcm_en_oam_trunk_ports_delete(int unit, bcm_gport_t trunk_gport,
                                         int max_ports, bcm_gport_t *port_arr);
extern int bcm_en_oam_trunk_ports_get(int unit, bcm_gport_t trunk_gport,
                                      int max_ports, bcm_gport_t *port_arr,
                                      int *port_count);
extern int bcm_en_oam_mpls_tp_channel_type_tx_get(int unit,
                                                  bcm_oam_mpls_tp_channel_type_t channel_type,
                                                  int *value);
extern int bcm_en_oam_mpls_tp_channel_type_tx_set(int unit,
                                                  bcm_oam_mpls_tp_channel_type_t channel_type,
                                                  int value);
#endif
extern int bcm_en_oam_init(int unit);
extern int bcm_en_oam_detach(int unit);
extern int bcm_en_oam_lock(int unit);
extern int bcm_en_oam_unlock(int unit);
extern int bcm_en_oam_group_create(int unit,
    bcm_oam_group_info_t *group_info);
extern int bcm_en_oam_group_get(int unit, bcm_oam_group_t group, 
    bcm_oam_group_info_t *group_info);
extern int bcm_en_oam_group_destroy(int unit, bcm_oam_group_t group);
extern int bcm_en_oam_group_destroy_all(int unit);
extern int bcm_en_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb, 
    void *user_data);
extern int bcm_en_oam_endpoint_create(int unit, 
    bcm_oam_endpoint_info_t *endpoint_info);
extern int bcm_en_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint, 
    bcm_oam_endpoint_info_t *endpoint_info);
extern int bcm_en_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint);
extern int bcm_en_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group);
extern int bcm_en_oam_endpoint_traverse(int unit, bcm_oam_group_t group, 
    bcm_oam_endpoint_traverse_cb cb, void *user_data);
extern int bcm_en_oam_event_register(int unit, 
    bcm_oam_event_types_t event_types, bcm_oam_event_cb cb, 
    void *user_data);
extern int bcm_en_oam_event_unregister(int unit, 
    bcm_oam_event_types_t event_types, bcm_oam_event_cb cb);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_en_oam_sync(int unit);
#endif
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_en_oam_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
extern int _bcm_en_port_lanes_set(int unit, bcm_port_t port, int value);
extern int _bcm_en_port_lanes_get(int unit, bcm_port_t port, int *value);
#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_en_init(int unit, _field_control_t *fc);
#endif /* BCM_FIELD_SUPPORT */

extern int bcm_enduro_vlan_virtual_init(int unit);
extern int bcm_enduro_vlan_virtual_detach(int unit);
extern int bcm_enduro_vlan_vp_create(int unit, bcm_vlan_port_t *vlan_vp);
extern int bcm_enduro_vlan_vp_destroy(int unit, bcm_gport_t gport);
extern int bcm_enduro_vlan_vp_find(int unit, bcm_vlan_port_t *vlan_vp);
extern int bcm_enduro_vlan_vp_add(int unit, bcm_vlan_t vlan, bcm_gport_t gport, 
                                   int is_untagged);
extern int bcm_enduro_vlan_vp_delete(int unit, bcm_vlan_t vlan,
                                      bcm_gport_t gport); 
extern int bcm_enduro_vlan_vp_delete_all(int unit, bcm_vlan_t vlan);
extern int bcm_enduro_vlan_vp_get(int unit, bcm_vlan_t vlan, bcm_gport_t gport,
                                   int *is_untagged); 
extern int bcm_enduro_vlan_vp_get_all(int unit, bcm_vlan_t vlan, int array_max,
        bcm_gport_t *gport_array, int *is_untagged_array, int *array_size);
extern int _bcm_enduro_vlan_port_resolve(int unit, bcm_gport_t vlan_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_enduro_vlan_virtual_reinit(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_enduro_vlan_vp_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif  /* BCM_ENDURO_SUPPORT */
#endif  /* !_BCM_INT_ENDURO_H_ */
