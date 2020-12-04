/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane3.h
 * Purpose:     Function declarations for Hurricane3  bcm functions
 */

#ifndef _BCM_INT_HURRICANE3_H_
#define _BCM_INT_HURRICANE3_H_
#ifdef BCM_FIELD_SUPPORT 
#include <bcm_int/esw/field.h>
#endif /* BCM_FIELD_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/switch_match.h>

/* HR3 specific HW index mask for L3_IIFm.TRUST_DSCP_PTRf 
 *  - L3_IIFm.TRUST_DSCP_PTRf is 6 bit length field.
 *  - value 63 means "Do not trust DSCP field"
 *  - other value must be mask to 0-31 as the index to DSCP_TABLEm
 *    -> this is different with other chips
 */
#define _BCM_HR3_L3IIF_TRUST_DSCP_PTR_MAX     (63)
#define _BCM_HR3_L3IIF_TRUST_DSCP_PTR_MASK    (0x1f)

/* Switch Encap related definitions */
#define _BCM_SWITCH_ENCAP_TYPE_MASK        0xfffffff
#define _BCM_SWITCH_ENCAP_SHIFT            28

/* Switch Encap Types (4-bit) */
#define _BCM_SWITCH_ENCAP_TYPE_MIML             1
#define _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER    2

/* MiML encap id: type(4-bit), egr_header_encap_index(12-bit), nh_index(16-bit) */
#define _BCM_SWITCH_ENCAP_MIML_NHI_MASK                0xffff
#define _BCM_SWITCH_ENCAP_MIML_NHI_SHIFT               0
#define _BCM_SWITCH_ENCAP_MIML_EGR_HEADER_ENCAP_MASK   0xfff
#define _BCM_SWITCH_ENCAP_MIML_EGR_HEADER_ENCAP_SHIFT  16

#define _BCM_SWITCH_ENCAP_MIML_PKT_TYPE_MASK    0xf
#define _BCM_SWITCH_ENCAP_MIML_PKT_TYPE_SHIFT   28
#define _BCM_SWITCH_ENCAP_MIML_IID_MASK         0xfff
#define _BCM_SWITCH_ENCAP_MIML_IID_SHIFT        16
#define _BCM_SWITCH_ENCAP_MIML_LENGTH_MASK      0x3fff
#define _BCM_SWITCH_ENCAP_MIML_LENGTH_SHIFT     0

/* Custom Header encap id: type(4-bit), egr_header_encap_index(12-bit) */
#define _BCM_SWITCH_ENCAP_CUSTOM_HEADER_EGR_HEADER_ENCAP_MASK   0xfff
#define _BCM_SWITCH_ENCAP_CUSTOM_HEADER_EGR_HEADER_ENCAP_SHIFT  0

#ifdef BCM_FIELD_SUPPORT 
extern int _bcm_field_hr3_init(int unit, _field_control_t *fc);
#endif

extern int bcm_hr3_cosq_drop_status_enable_set(int unit, bcm_port_t port, int enable);

extern int bcm_hr3_port_init(int unit);
extern int bcm_hr3_port_egress_set(int unit, bcm_port_t port, int modid,
                                  bcm_pbmp_t pbmp);
extern int bcm_hr3_port_egress_get(int unit, bcm_port_t port, int modid,
                                  bcm_pbmp_t *pbmp);
extern int _bcm_hr3_mod_port_tpid_enable_write(int unit, bcm_module_t mod,
                    bcm_port_t port, int tpid_enable);
extern int _bcm_hr3_mod_port_tpid_enable_read(int unit, bcm_module_t mod,
                    bcm_port_t port, int *tpid_enable);
extern int _bcm_hr3_mod_port_inner_tpid_enable_write(int unit, bcm_module_t mod,
                    bcm_port_t port, int tpid_enable);
extern int _bcm_hr3_mod_port_inner_tpid_enable_read(int unit, bcm_module_t mod,
                    bcm_port_t port, int *tpid_enable);
extern int _bcm_hr3_mod_port_tpid_set(int unit, bcm_module_t mod,
                    bcm_port_t port, uint16 tpid);
extern int _bcm_hr3_mod_port_tpid_get(int unit, bcm_module_t mod,
                    bcm_port_t port, uint16 *tpid);
extern int _bcm_hr3_mod_port_tpid_add(int unit, bcm_module_t mod,
                    bcm_port_t port, uint16 tpid);
extern int _bcm_hr3_mod_port_tpid_delete(int unit, bcm_module_t mod,
                    bcm_port_t port, uint16 tpid);
extern int _bcm_hr3_mod_port_tpid_delete_all(int unit, bcm_module_t mod,
                    bcm_port_t port);

/* ################ VLAN related ################### */
extern int _bcm_hr3_vlan_translate_stat_attach(int unit, bcm_gport_t port,
                bcm_vlan_translate_key_t key_type, bcm_vlan_t outer_vlan,
                bcm_vlan_t inner_vlan, uint32 stat_counter_id);
                
extern int _bcm_hr3_vlan_translate_stat_detach(int unit, bcm_gport_t port,
                bcm_vlan_translate_key_t key_type, bcm_vlan_t outer_vlan,
                bcm_vlan_t inner_vlan);

extern int _bcm_hr3_vlan_translate_stat_counter_set(int unit, bcm_gport_t port,
                bcm_vlan_translate_key_t key_type, bcm_vlan_t outer_vlan,
                bcm_vlan_t inner_vlan, bcm_vlan_stat_t stat,
                uint32 num_entries, uint32 *counter_indexes,
                bcm_stat_value_t *counter_values);
                
extern int _bcm_hr3_vlan_translate_stat_counter_get(int unit, int sync_mode,
                bcm_gport_t port, bcm_vlan_translate_key_t key_type,
                bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                bcm_vlan_stat_t stat, uint32 num_entries,
                uint32 *counter_indexes, bcm_stat_value_t *counter_values);

extern int _bcm_hr3_vlan_translate_stat_enable_set(int unit, bcm_gport_t port,
                bcm_vlan_translate_key_t key_type,
                bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                int enable);

extern int _bcm_hr3_vlan_translate_stat_id_get(int unit, bcm_gport_t port,
                bcm_vlan_translate_key_t key_type, 
                bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                bcm_vlan_stat_t stat, uint32 *stat_counter_id);

extern int _bcm_hr3_vlan_translate_egress_stat_attach(int unit, int port_class,
                bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                uint32 stat_counter_id);
extern int _bcm_hr3_vlan_translate_egress_stat_counter_get(int unit, int sync_mode,
                int port_class, bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                bcm_vlan_stat_t stat, uint32 num_entries, uint32 *counter_indexes,
                bcm_stat_value_t *counter_values);
extern int _bcm_hr3_vlan_translate_egress_stat_counter_set(int unit, int port_class,
                bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                bcm_vlan_stat_t stat, uint32 num_entries,
                uint32 *counter_indexes, bcm_stat_value_t *counter_values);
extern int _bcm_hr3_vlan_translate_egress_stat_id_get(int unit, int port_class,
                bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                bcm_vlan_stat_t stat, uint32 *stat_counter_id);
extern int _bcm_hr3_vlan_translate_egress_stat_detach(int unit, int port_class,
                bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan);
extern int _bcm_hr3_vlan_translate_egress_stat_enable_set(int unit, int port_class, 
                bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, int enable);

/* ################ Switch Encap related ################### */
extern int _bcm_hr3_switch_encap_idx2id(int unit, 
                int type, int index_count, int *index_array,  bcm_if_t *encap_id);
extern int _bcm_hr3_switch_encap_id2idx(int unit, 
                bcm_if_t encap_id, int index_count, int *index_array);
extern int bcm_hr3_switch_encap_detach(int unit);
extern int bcm_hr3_switch_encap_init(int unit);
extern int bcm_hr3_switch_encap_create(int unit, 
                bcm_switch_encap_info_t *encap_info, bcm_if_t *encap_id);
extern int _bcm_hr3_switch_encap_create(int unit,
                bcm_switch_encap_info_t *encap_info,
                uint32 count, bcm_if_t *base_encap_id);
extern int bcm_hr3_switch_encap_destroy(int unit, bcm_if_t encap_id);
extern int bcm_hr3_switch_encap_destroy_all(int unit);
extern int bcm_hr3_switch_encap_set(int unit, bcm_if_t encap_id, 
                bcm_switch_encap_info_t *encap_info);
extern int bcm_hr3_switch_encap_get(int unit, bcm_if_t encap_id, 
                bcm_switch_encap_info_t *encap_info);
extern int bcm_hr3_switch_encap_traverse(int unit, 
                bcm_switch_encap_traverse_cb cb_fn, void *user_data);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_hr3_switch_encap_sync(int unit, uint8 **scache_ptr);
extern int _bcm_hr3_switch_encap_reinit(int unit, uint8 **scache_ptr);
extern int _bcm_hr3_switch_encap_wb_scache_size_get(int unit,
	          int *req_scache_size);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_SWITCH_MATCH_SUPPORT
/* ################ Switch Match related ################### */
extern const bcmi_switch_match_service_info_t bcmi_hr3_miml_match_info;
extern const bcmi_switch_match_service_info_t bcmi_hr3_custom_hdr_match_info;

extern int
bcmi_hr3_switch_match_dev_info_init(
    int unit,
    const bcmi_switch_match_dev_info_t **dev_info);
#endif /* BCM_SWITCH_MATCH_SUPPORT */

extern int bcmi_wh2_port_encap_set(int unit, bcm_port_t port, int mode);
extern int bcmi_wh2_port_encap_get(int unit, bcm_port_t port, int *mode);
#endif  /* BCM_HURRICANE3_SUPPORT */
#endif  /* !_BCM_INT_HURRICANE3_H_ */
