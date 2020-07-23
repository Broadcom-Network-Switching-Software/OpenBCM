/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:
 *     This file contains switch match definitions internal to the BCM library.
 */

#ifndef _BCM_INT_SWITCH_MATCH_H
#define _BCM_INT_SWITCH_MATCH_H
#include <bcm/switch.h>

#if defined(BCM_SWITCH_MATCH_SUPPORT)
/*
 * Switch match's platform specific information per service
 * This structure maintains all chip specific information
 */
typedef struct bcmi_switch_match_service_info_s {
    char *table_name;
    int (* switch_match_table_size_get)(
        int unit,
        uint32 *table_size);
    int (* switch_match_hw_id_get)(
        int unit,
        uint32 sw_idx,
        uint32 *hw_id);
    int (* switch_match_sw_idx_get)(
        int unit,
        uint32 hw_id,
        uint32 *sw_idx);
    int (* switch_match_wb_hw_existed)(
        int unit,
        uint32 sw_idx);
    int (* switch_match_config_add)(
        int unit,
        bcm_switch_match_config_info_t *config_info,
        uint32 *sw_idx);
    int (* switch_match_config_delete)(
        int unit,
        uint32 sw_idx);
    int (* switch_match_config_get)(
        int unit,
        uint32 sw_idx,
        bcm_switch_match_config_info_t *config_info);
    int (* switch_match_config_set)(
        int unit,
        uint32 sw_idx,
        bcm_switch_match_config_info_t *config_info);
    int (* switch_match_control_get)(
        int unit,
        bcm_switch_match_control_type_t control_type,
        bcm_gport_t gport,
        bcm_switch_match_control_info_t *control_info);
    int (* switch_match_control_set)(
        int unit,
        bcm_switch_match_control_type_t control_type,
        bcm_gport_t gport,
        bcm_switch_match_control_info_t *control_info);
} bcmi_switch_match_service_info_t;

/* Switch match platform-specific information */
typedef struct bcmi_switch_match_dev_info_s {
    const bcmi_switch_match_service_info_t
        *service_info[bcmSwitchMatchService__Count];
} bcmi_switch_match_dev_info_t;

/* Switch match initial and detach */
extern int bcmi_switch_match_init(int unit);

extern int bcmi_switch_match_detach(int unit);

/* Switch match configuration operations */
extern int bcmi_switch_match_config_add(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_config_info_t *config_info,
    int *match_id);

extern int bcmi_switch_match_config_delete(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id);

extern int bcmi_switch_match_config_delete_all(
    int unit,
    bcm_switch_match_service_t match_service);

extern int bcmi_switch_match_config_get(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id,
    bcm_switch_match_config_info_t *config_info);

extern int bcmi_switch_match_config_set(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id,
    bcm_switch_match_config_info_t *config_info);

extern int bcmi_switch_match_config_traverse(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_config_traverse_cb cb_fn,
    void *user_data);

/* Switch match control operations */
extern int bcmi_switch_match_control_get(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t *control_info);

extern int bcmi_switch_match_control_set(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t *control_info);

extern int bcmi_switch_match_control_traverse(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_traverse_cb cb_fn,
    void *user_data);

/*
 * Switch Match conversion between software Match ID and hardware ID
 */
extern int
bcmi_switch_match_hw_id_get(
    int unit,
    int match_id,
    bcm_switch_match_service_t *match_service,
    uint32 *hw_id);

extern int
bcmi_switch_match_match_id_get(
    int unit,
    bcm_switch_match_service_t match_service,
    uint32 hw_id,
    int *match_id);

#endif /* BCM_SWITCH_MATCH_SUPPORT */
#endif /* !_BCM_INT_SWITCH_MATCH_H */
