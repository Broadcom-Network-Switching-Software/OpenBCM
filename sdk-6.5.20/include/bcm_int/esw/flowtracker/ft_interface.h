/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_field.h
 * Purpose:     Flowtracker module specific routines in flowtracker
 *              stage in FP.
 */

#ifndef _BCM_INT_FT_API_H
#define _BCM_INT_FT_API_H

#include <soc/defs.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)
#include <bcm/flowtracker.h>

/* API from Field module to Flowtracker module */
extern int
bcmi_esw_ft_group_hw_install(int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_esw_ft_group_hw_uninstall(int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_esw_ft_group_extraction_mode_set(int unit,
        int key_or_data,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_key_data_mode_t mode);

extern int
bcmi_esw_ft_group_extraction_hw_info_set(int unit,
        bcm_flowtracker_group_t id,
        int key_or_data,
        int num_elements,
        bcmi_ft_hw_extractor_info_t *elements);

extern int
bcmi_esw_ft_group_extraction_hw_info_get(int unit,
        int key_or_data,
        bcm_flowtracker_group_t id,
        int max_elements,
        bcmi_ft_hw_extractor_info_t *elements,
        int *num_elements);

extern int
bcmi_esw_ft_group_extraction_alu_info_set(int unit,
        bcm_flowtracker_group_t id,
        int key_or_data,
        int num_alu_info,
        bcmi_ft_group_alu_info_t *alu_info);

extern int
bcmi_esw_ft_group_extraction_alu_info_get(int unit,
        bcm_flowtracker_group_t id,
        int key_or_data,
        int max_alu_info,
        bcmi_ft_group_alu_info_t *alu_info,
        int *num_alu_info);

extern int
bcmi_esw_ft_group_param_retrieve(int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_params_t param,
        int *arg);

extern int
bcmi_esw_ft_group_param_update(int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_params_t param,
        bcmi_ft_group_params_operation_t opr,
        int arg);

extern int
bcmi_esw_ft_group_entry_add_check(int unit,
        bcm_flowtracker_group_type_t group_type,
        bcm_flowtracker_group_t id);

extern int
bcmi_esw_ft_check_action_num_operands_get(int unit,
        bcm_flowtracker_check_action_t action,
        int *num_operands);

extern int
bcmi_esw_ft_group_tracking_param_mask_status_get(int unit,
        bcm_flowtracker_tracking_param_info_t *tp_info,
        uint16 qual_size,
        int *mask_present,
        int *partial_mask);

extern int
bcmi_esw_ft_type_support_check(int unit,
        bcmi_ft_type_t ft_type,
        int *support);

extern int
bcmi_esw_ft_group_state_transition_type_get(int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_state_transition_type_t *st_type);

extern int
bcmi_esw_ft_group_checks_bitmap_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *check_bmp_data,
        uint32 *check_bmp_mask);

extern int
bcmi_esw_ft_group_checks_list_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 check_bmp_data,
        uint32 check_bmp_mask,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *actual_num_checks);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
extern int
bcmi_esw_ftv3_user_entry_entry_add_check(int unit,
        bcm_flowtracker_group_type_t group_type,
        bcm_flowtracker_group_t id,
        int flow_index);

extern int
bcmi_esw_ftv3_aggregate_class_entry_add_check(int unit,
        uint32 aggregate_class);

extern int
bcmi_esw_ftv3_aggregate_class_ref_update(int unit,
        uint32 aggregate_class,
        int count);

extern int
bcmi_esw_ftv3_user_entry_ref_update(int unit,
        bcm_flowtracker_group_type_t group_type,
        int flow_index,
        int count);

#endif /* BCM_FLWOTRACKER_V3_SUPPORT */
#endif /* BCM_FLOWTRACKER_SUPPORT */
#endif /* _BCM_INT_FT_API_H */
