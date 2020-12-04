/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        greyhound.h
 * Purpose:     Function declarations for Greyhound bcm functions
 */

#ifndef _BCM_INT_GREYHOUND_H_
#define _BCM_INT_GREYHOUND_H_
#ifdef BCM_FIELD_SUPPORT 
#include <bcm_int/esw/field.h>
#endif /* BCM_FIELD_SUPPORT */
#if defined(INCLUDE_L3)
#include <bcm/extender.h>
#include <bcm/niv.h>
#endif
#include <bcm/qos.h>

#if defined(BCM_GREYHOUND_SUPPORT)
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/cosq.h>
#include <soc/greyhound.h>

#ifdef BCM_FIELD_SUPPORT 
extern int _bcm_field_gh_init(int unit, _field_control_t *fc);
extern int _bcm_field_gh_write_slice_map(int unit, _field_stage_t *stage_fc,
                                                   _field_group_t *fg);
#endif
extern int bcm_gh_port_niv_type_set(int unit, bcm_port_t port, int value);
extern int bcm_gh_port_niv_type_get(int unit, bcm_port_t port, int *value);
extern int bcm_gh_port_extender_type_set(int unit, bcm_port_t port, int value);
extern int bcm_gh_port_extender_type_get(int unit, bcm_port_t port, int *value);
extern int
bcm_gh_port_priority_group_mapping_set(int unit, bcm_gport_t gport, 
               int prio, int priority_group);
extern int
bcm_gh_port_priority_group_mapping_get(int unit, bcm_gport_t gport, 
               int prio, int *priority_group);
extern int
bcm_gh_port_priority_group_config_set(int unit, bcm_gport_t gport, int priority_group, 
               bcm_port_priority_group_config_t *prigrp_config);
extern int
bcm_gh_port_priority_group_config_get(int unit, bcm_gport_t gport, int priority_group, 
               bcm_port_priority_group_config_t *prigrp_config);
extern int bcm_gh_qos_init(int unit);
extern int bcm_gh_qos_detach(int unit);
extern int bcm_gh_qos_map_create(int unit, uint32 flags, int *map_id);
extern int bcm_gh_qos_map_destroy(int unit, int map_id);
extern int bcm_gh_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map,
                               int map_id);
extern int bcm_gh_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map,
                                  int map_id);
extern int bcm_gh_qos_port_map_set(int unit, bcm_gport_t port, int ing_map,
                                    int egr_map);
extern int bcm_gh_qos_map_multi_get(int unit, uint32 flags,
                                      int map_id, int array_size, 
                                      bcm_qos_map_t *array, int *array_count);

#if defined(INCLUDE_L3)

/* NIV */
#define GH_L2_HASH_KEY_TYPE_VIF 5 /* Key type of L2X table for NIV */

extern int bcm_gh_niv_forward_add(int unit, bcm_niv_forward_t *iv_fwd_entry);
extern int bcm_gh_niv_forward_delete(int unit, bcm_niv_forward_t *iv_fwd_entry);
extern int bcm_gh_niv_forward_delete_all(int unit);
extern int bcm_gh_niv_forward_get(int unit, bcm_niv_forward_t *iv_fwd_entry);
extern int bcm_gh_niv_forward_traverse(int unit,
                                       bcm_niv_forward_traverse_cb cb,
                                       void *user_data);

extern int bcm_gh_niv_ethertype_set(int unit, int ethertype);
extern int bcm_gh_niv_ethertype_get(int unit, int *ethertype);

/* Port Extender */
#define GH_L2_HASH_KEY_TYPE_PE_VID 6 /* Key type of L2X table for Port Extender */

extern int bcm_gh_extender_forward_add(int unit, bcm_extender_forward_t *forward_entry);
extern int bcm_gh_extender_forward_delete(int unit, bcm_extender_forward_t *forward_entry);
extern int bcm_gh_extender_forward_delete_all(int unit);
extern int bcm_gh_extender_forward_get(int unit, bcm_extender_forward_t *forward_entry);
extern int bcm_gh_extender_forward_traverse(int unit,
                                            bcm_extender_forward_traverse_cb cb,
                                            void *user_data);

extern int bcm_gh_port_etag_pcp_de_source_set(int unit, bcm_port_t port, int value);
extern int bcm_gh_port_etag_pcp_de_source_get(int unit, bcm_port_t port, int *value);


extern int bcm_gh_etag_ethertype_set(int unit, int ethertype);
extern int bcm_gh_etag_ethertype_get(int unit, int *ethertype);
#endif /* INCLUDE_L3 */


/* WRED */
extern int
bcm_gh_cosq_discard_set(int unit, uint32 flags);

extern int
bcm_gh_cosq_discard_get(int unit, uint32 *flags);

extern int
bcm_gh_cosq_discard_port_set(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int drop_start,
                             int drop_slope,
                             int average_time);
extern int
bcm_gh_cosq_discard_port_get(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int *drop_start,
                             int *drop_slope,
                             int *average_time);

extern int
bcm_gh_cosq_gport_discard_set(int unit, bcm_gport_t port, 
                                bcm_cos_queue_t cosq,
                                bcm_cosq_gport_discard_t *discard);

extern int
bcm_gh_cosq_gport_discard_get(int unit, bcm_gport_t port, 
                                bcm_cos_queue_t cosq,
                                bcm_cosq_gport_discard_t *discard);

extern int
bcm_gh_cosq_port_pfc_op(int unit, bcm_port_t port,
                       bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                       bcm_cos_queue_t cosq);
extern int
bcm_gh_cosq_port_pfc_get(int unit, bcm_port_t port,
                        bcm_switch_control_t sctype, 
                        bcm_cos_queue_t *cosq);
extern int 
bcm_gh_port_init(int unit);

extern int
bcm_gh_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg);
extern int
bcm_gh_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
							bcm_cosq_control_t type, int *arg);

/*
 * 8 Priority Groups supported.
 */
#define GH_PRIOROTY_GROUP_ID_MIN 0
#define GH_PRIOROTY_GROUP_ID_MAX 7
/*
 * Inpur priority range.
 * PFC : 0-7
 * SAFC : 0-15
 */
#define GH_PFC_INPUT_PRIORITY_MIN 0
#define GH_PFC_INPUT_PRIORITY_MAX 7
#define GH_SAFC_INPUT_PRIORITY_MIN GH_PFC_INPUT_PRIORITY_MIN
#define GH_SAFC_INPUT_PRIORITY_MAX 15

#endif  /* BCM_GREYHOUND_SUPPORT */
#endif  /* !_BCM_INT_GREYHOUND_H_ */
