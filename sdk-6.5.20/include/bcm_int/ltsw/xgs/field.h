/*! \file field.h
 *
 * Field headfiles to declare internal APIs for XGS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMINT_LTSW_XGS_FIELD_H
#define BCMINT_LTSW_XGS_FIELD_H

/*!
 * \brief Clear field related LT tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_ifp_clear(int unit);

extern int
xgs_ltsw_vfp_clear(int unit);

extern int
xgs_ltsw_efp_clear(int unit);

extern int
xgs_ltsw_field_clear(int unit);

/*!
 * \brief Set the filter enable state in the Port Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_field_port_filter_enable_set(int unit, uint32 state,
                                      bcm_pbmp_t port_pbmp);

extern int
xgs_ltsw_field_udf_qset_set(
    int unit,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset);

extern int
xgs_ltsw_field_group_udf_qset_update_with_hints(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_field_hint_t *hint,
    bcm_field_qset_t *udf_qset,
    bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap);

extern int
xgs_ltsw_field_udf_qual_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_hash,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask);

extern int
xgs_ltsw_field_udf_qual_get(
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
xgs_ltsw_field_udf_obj_get(
    int unit,
    bcm_field_qset_t qset,
    int max,
    int *objects_list,
    int *actual);

extern int
xgs_ltsw_field_udf_qset_del(
    int unit,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset);

extern int
xgs_ltsw_field_action_profile_index_get(int unit,
        bcm_field_entry_t entry,
        bcm_field_action_t action,
        uint16_t *act_prof_idx);

extern int
xgs_ltsw_field_action_profile_add(int unit,
            bcmi_ltsw_field_stage_t stage_id,
                   bcm_field_action_t action,
           bcm_field_action_params_t *params,
           bcm_field_action_match_config_t *match_config,
                  uint64_t *act_prof_idx_val);

extern int
xgs_ltsw_field_action_profile_get(int unit,
               bcmi_ltsw_field_stage_t stage_id,
                      bcm_field_action_t action,
                      uint64_t *act_prof_idx_val,
               bcm_field_action_params_t *params,
               bcm_field_action_match_config_t *match_config);

extern int
xgs_ltsw_field_action_profile_del(int unit,
                      bcmi_ltsw_field_stage_t stage_id,
                             bcm_field_action_t action,
                             uint64_t *act_prof_idx_val);

extern int
xgs_ltsw_field_ingress_profile_init(int unit);

extern int
xgs_ltsw_field_ingress_profile_deinit(int unit);

extern int
xgs_ltsw_field_qual_info_set(int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info);

extern int
xgs_ltsw_field_qual_value_get(int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info);

extern int
xgs_ltsw_field_qual_value_set(int unit,
                    bcm_field_entry_t entry_id,
                    bcm_qual_field_t *qual_info);

extern int
xgs_ltsw_field_action_value_set(int unit,
                        bcm_field_entry_t entry_id,
                        bcmi_ltsw_field_stage_t stage,
                        bcm_field_action_t action,
                        bcm_field_action_params_t *params,
                        bcm_field_action_match_config_t *match_config,
                        uint64_t *action_value);

extern int
xgs_ltsw_field_action_value_get(int unit,
                    bcm_field_entry_t entry_id,
                    bcmi_ltsw_field_stage_t stage,
                    bcm_field_action_t action,
                    bcm_field_action_params_t *params,
                    bcm_field_action_match_config_t *match_config,
                    uint64_t *action_value);

extern int
xgs_ltsw_field_action_lt_map_valid_info_get(
        int unit,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_entry_t entry_id,
        bcm_field_action_t action,
        uint16_t lt_map_flags,
        uint16_t map_count,
        bool *lt_field_map);

extern int
xgs_ltsw_field_action_mirror_cont_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry_id,
    bcm_field_action_t action,
    int mirror_cont,
    char **cont_name);

extern int
xgs_ltsw_field_action_mirror_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport);

extern int
xgs_ltsw_field_action_mirror_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport);

extern int
xgs_ltsw_field_action_mirror_index_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int entry,
    bcm_field_action_t action,
    int mirror_cont,
    bool *enable,
    int *mirror_index);

extern int
xgs_ltsw_field_action_default_value_get(
        int unit, bcm_field_entry_t entry_id,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_action_t action,
        bool del_action,
        bcmlt_field_def_t *fld_defs_out);

extern int
xgs_ltsw_field_qualify_port_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t *data_modid,
    bcm_module_t *mask_modid,
    bcm_port_t *data_port,
    bcm_port_t *mask_port);

extern int
xgs_ltsw_field_qualify_port_set(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t data_modid,
    bcm_module_t mask_modid,
    bcm_port_t data_port,
    bcm_port_t mask_port);

extern int
xgs_ltsw_field_qualify_dstl3egress_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_if_t *if_id);

extern int
xgs_ltsw_field_qualify_dstl3egress_set(
        int unit,
        bcm_field_entry_t entry,
        bcm_if_t if_id);
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
xgs_ltsw_field_compression_set(
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
xgs_ltsw_field_compression_del(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_qset_t *qset,
        bcmlt_entry_handle_t templ);

#endif /* BCMINT_LTSW_XGS_FIELD_H */
