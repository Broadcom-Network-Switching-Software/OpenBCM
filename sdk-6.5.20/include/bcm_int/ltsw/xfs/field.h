/*! \file field.h
 *
 * Field headfiles to declare internal APIs for XFS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMINT_LTSW_XFS_FIELD_H
#define BCMINT_LTSW_XFS_FIELD_H

extern int
xfs_field_ing_match_id_enums_init(
        int unit,
        bcmint_field_stage_info_t *stage_info);

extern int
xfs_field_egr_match_id_enums_init(
        int unit,
        bcmint_field_stage_info_t *stage_info);

extern int
xfs_field_vlan_match_id_enums_init(
        int unit,
        bcmint_field_stage_info_t *stage_info);

extern int
xfs_ltsw_field_stage_group_sbr_prof_init(
        int unit,
        bcmint_field_stage_info_t *stage_info);

extern int
xfs_ltsw_field_ingress_profile_init(
        int unit);

extern int
xfs_ltsw_field_ingress_profile_deinit(
        int unit);

extern int
xfs_ltsw_field_action_value_set(
        int unit,
        bcm_field_entry_t entry_id,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *action_value);

extern int
xfs_ltsw_field_action_value_get(
        int unit,
        bcm_field_entry_t entry_id,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *action_value);

extern int
xfs_ltsw_field_action_profile_index_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_action_t action,
        uint16_t *act_prof_idx);

extern int
xfs_ltsw_field_action_profile_add(
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *act_prof_idx_val);

extern int
xfs_ltsw_field_action_profile_get(
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        uint64_t *act_prof_idx_val,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config);

extern int
xfs_ltsw_field_action_profile_del(
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        uint64_t *act_prof_idx_val);

extern int
xfs_ltsw_field_action_meter_cont_get(
        int unit,
        bcmi_ltsw_field_stage_t stage,
        int slice_idx,
        char **cont_name);

/*!
 * \brief Set FP group compression.
 *
 * \param [in] unit         Unit number.
 * \param [in] stage_info   Reference stage info structure.
 * \param [in] qset         Qualifier set.
 * \param [in] templ        SDKLT field template.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_field_compression_set(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_field_qset_t *qset,
        uint32_t group_flags,
        bcmlt_entry_handle_t templ);
/*!
 * \brief Delete FP compression.
 *
 * \param [in] unit         Unit number.
 * \param [in] stage_info   Reference stage info structure.
 * \param [in] qset         Reference to field qualifier set.
 * \param [in] template     SDKLT group/rule template.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_field_compression_del(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_qset_t *qset,
        bcmlt_entry_handle_t templ);

/*!
 * \brief Set the filter enable state in the Port Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_field_port_filter_enable_set(
        int unit,
        uint32 state,
        bcm_pbmp_t port_pbmp);

extern int
xfs_ltsw_field_qualify_macro_values_get(
        int unit,
        uint8 set_flag,
        bcm_qual_field_t *qual_info,
        bcm_qual_field_values_t *qual_info_values);

extern int
xfs_ltsw_ifp_clear(
        int unit);

extern int
xfs_ltsw_vfp_clear(
        int unit);

extern int
xfs_ltsw_efp_clear(
        int unit);

extern int
xfs_ltsw_em_clear(
        int unit);

extern int
xfs_ltsw_ft_clear(
        int unit);

extern int
xfs_ltsw_field_clear(
        int unit);

extern int
xfs_ltsw_field_lt_qual_name_update(
        int unit,
        bool group_map,
        bcmint_field_stage_info_t *stage_info,
        const bcm_field_qset_t *qset,
        const bcm_field_qual_map_info_t *lt_map_ref,
        char **lt_field_name);

extern int
xfs_ltsw_field_udf_qual_set(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_group_oper_t *group_oper,
        bcmi_field_ha_entry_oper_t *entry_hash,
        bcm_udf_id_t udf_id,
        int length,
        uint8 *data,
        uint8 *mask);

extern int
xfs_ltsw_field_udf_qual_get(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_group_oper_t *group_oper,
        bcmi_field_ha_entry_oper_t *entry_hash,
        bcm_udf_id_t udf_id,
        int max_len,
        uint8 *data,
        uint8 *mask,
        int *actual_len);

extern int
xfs_ltsw_field_udf_qset_set(
        int unit,
        int num_objects,
        int objects_list[],
        bcm_field_qset_t *qset);

extern int
xfs_ltsw_field_udf_obj_get(
        int unit,
        bcm_field_qset_t qset,
        int max,
        int *objects_list,
        int *actual);

extern int
xfs_ltsw_field_udf_qset_del(
        int unit,
        int num_objects,
        int objects_list[],
        bcm_field_qset_t *qset);

extern int
xfs_ltsw_field_group_mode_set(
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcmlt_entry_handle_t grp_template,
        bool *mode_auto,
        char **group_mode);

extern int
xfs_ltsw_field_group_pipe_get(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t pbmp,
        uint8_t *field_pipe);

extern int
xfs_ltsw_field_group_pbmp_get(
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t *pbmp);

extern int
xfs_ltsw_field_pbmp_index_get(
        int unit,
        bcm_field_group_t group,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t *in_pbmp,
        bcm_pbmp_t *in_pbmp_mask,
        uint8_t *out_pbmp_idx,
        uint8_t *out_pbmp_idx_mask);

extern int
xfs_ltsw_field_pbmp_get(
        int unit,
        bcm_field_group_t group,
        bcmint_field_stage_info_t *stage_info,
        uint8_t *out_pbmp_idx,
        uint8_t *out_pbmp_idx_mask,
        bcm_pbmp_t *in_pbmp,
        bcm_pbmp_t *in_pbmp_mask);

extern int
xfs_ltsw_field_internal_entry_enable_get(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bool *enable);

extern int
xfs_ltsw_field_qual_gport_validate(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        bcm_gport_t gport,
        bcm_port_t *port);

extern int
xfs_ltsw_field_qualify_copytocpureasonhigh_set(
        int unit,
        uint32_t *data,
        uint32_t *mask);

extern int
xfs_ltsw_field_qualify_copytocpureasonhigh_get(
        int unit,
        uint32_t *data,
        uint32_t *mask);

extern int
xfs_ltsw_field_qual_info_lt_name_get(
        int unit,
        const char *lt_field_name,
        char *num_offset_field_name,
        char *offset_field_name,
        char *width_field_name);

extern int
xfs_ltsw_field_policer_attach(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_policer_t policer_id);

extern int
xfs_ltsw_field_policer_detach(
        int unit,
        bcm_field_entry_t entry);

extern int
xfs_ltsw_field_qual_mask_lt_field_get(
        int unit,
        char *qual_field,
        char **qual_mask_field);

extern int
xfs_ltsw_field_qualify_color_set(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        uint8 color);

extern int
xfs_ltsw_field_qualify_color_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        uint8 *color);

extern int
xfs_ltsw_field_qual_value_set(
        int unit,
        bcm_field_entry_t entry_id,
        bcm_qual_field_t *qual_info);

extern int
xfs_ltsw_field_qual_value_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info);

extern int
xfs_ltsw_field_qual_info_set(
        int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info);

extern int
xfs_ltsw_field_entry_table_id_get(
        int unit,
        const char *table_name,
        uint32_t *table_id,
        uint32_t *entry_idx);

extern int
xfs_ltsw_field_aacl_class_id_get(
        int unit,
        bcm_field_qualify_t qid,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        int *class_id,
        bcm_field_qualify_t *pair_qual);

extern int
xfs_ltsw_field_action_stat_group_lt_field_validate(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_hintid_t hintid,
        const char *field_name,
        bool *is_valid);

extern int
xfs_ltsw_field_group_udf_qset_update_with_hints(
        int unit,
        bcm_udf_id_t udf_id,
        bcm_field_hint_t *hint,
        bcm_field_qset_t *udf_qset,
        bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap);

extern int
xfs_ltsw_field_action_lt_map_valid_info_get(
        int unit,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_entry_t entry_id,
        bcm_field_action_t action,
        uint16_t lt_map_flags,
        uint16_t map_count,
        bool *lt_field_map);

extern int
xfs_ltsw_field_action_mirror_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport);

extern int
xfs_ltsw_field_action_mirror_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport);

extern int
xfs_ltsw_field_action_mirror_index_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int entry,
    bcm_field_action_t action,
    int mirror_cont,
    bool *enable,
    int *mirror_index);

extern int
xfs_ltsw_field_qualify_port_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t *data_modid,
    bcm_module_t *mask_modid,
    bcm_port_t *data_port,
    bcm_port_t *mask_port);

extern int
xfs_ltsw_field_qualify_port_set(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t data_modid,
    bcm_module_t mask_modid,
    bcm_port_t data_port,
    bcm_port_t mask_port);

extern int
xfs_ltsw_field_qualify_dstl3egress_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_if_t *if_id);

extern int
xfs_ltsw_field_qualify_dstl3egress_set(
        int unit,
        bcm_field_entry_t entry,
        bcm_if_t if_id);

extern int
xfs_ltsw_field_group_mode_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcm_field_group_mode_t *mode);

extern int
xfs_ltsw_field_qualify_mhopcode_set(int unit,
                                bcm_field_entry_t entry,
                                uint8 data, uint8 mask);
extern int
xfs_ltsw_field_qualify_mhopcode_get(int unit,
                                    bcm_field_entry_t entry,
                                    uint8 *data, uint8 *mask);
#endif /* BCMINT_LTSW_XFS_FIELD_H */
