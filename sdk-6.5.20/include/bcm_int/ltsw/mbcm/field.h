/*! \file field.h
 *
 * BCM field module APIs and structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_MBCM_FIELD_H
#define BCMINT_LTSW_MBCM_FIELD_H

#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/flexctr.h>

/*!
 * \brief Initialize Field control information.
 *
 * \param [in] unit Unit number.
 * \param [out] control_info Field control database.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*field_control_info_init_f)(
        int unit,
        bcmint_field_control_info_t *control_info);

/*!
 * \brief Initialize Field stage information.
 *
 * \param [in] unit Unit number.
 * \param [out] stage_info Field control database.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*field_stage_info_init_f)(
        int unit,
        bcmint_field_stage_info_t *stage_info);

typedef int (*field_stage_db_init_f)(
        int unit,
        bcmint_field_stage_info_t *stage_info);

typedef int (*field_ingress_profile_init_f)(
        int unit);

typedef int (*field_ingress_profile_deinit_f)(
        int unit);

typedef int (*field_auto_enum_to_lt_enum_value_get_f)(
        int unit,
        bcm_field_qualify_t qual,
        uint32 qual_enum,
        bcm_qual_field_t *qual_info);

typedef int (*field_auto_lt_enum_value_to_enum_get_f)(
        int unit,
        bcm_field_qualify_t qual,
        bcm_qual_field_t *qual_info,
        uint32 *qual_enum);

typedef int (*field_manual_enum_to_lt_enum_value_get_f)(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        uint32 qual_enum,
        bcm_qual_field_t *qual_info);

typedef int (*field_manual_lt_enum_value_to_enum_get_f)(
        int unit,
        bcm_field_entry_t entry, 
        bcm_field_qualify_t qual,
        bcm_qual_field_t *qual_info,
        uint32 *qual_enum);

/*!
 * \brief set action value
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*field_action_value_set_f)(
        int unit, bcm_field_entry_t entry_id,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *action_value);

/*!
 * \brief get action value
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*field_action_value_get_f)(
        int unit, bcm_field_entry_t entry_id,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *action_value);

typedef int (*field_src_class_enum_to_lt_symbol_get_f)(
        int unit,
        bcm_field_src_class_mode_t mode,
        char **key_fid);

typedef int (*field_src_class_lt_symbol_to_enum_get_f)(
        int unit,
        char *key_fid,
        bcm_field_src_class_mode_t *mode);

typedef int (*field_src_class_hw_fields_encode_set_f)(
        int unit,
        bcm_field_src_class_mode_t mode,
        bcm_field_src_class_t src_data,
        bcm_field_src_class_t src_mask,
        uint32 *hw_data,
        uint32 *hw_mask);

typedef int (*field_src_class_hw_fields_decode_get_f)(
        int unit,
        bcm_field_src_class_mode_t mode,
        uint32 hw_data,
        uint32 hw_mask,
        bcm_field_src_class_t *src_data,
        bcm_field_src_class_t *src_mask);

typedef int (*field_action_profile_index_get_f) (
        int unit, bcm_field_entry_t entry,
        bcm_field_action_t action,
        uint16_t *act_prof_idx);

typedef int (*field_action_profile_add_f) (
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *act_prof_idx_val);

typedef int (*field_action_profile_get_f) (
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        uint64_t *act_prof_idx_val,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config);

typedef int (*field_action_profile_del_f) (
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        uint64_t *act_prof_idx_val);

typedef int (*field_action_meter_cont_get_f)(
        int unit,
        bcmi_ltsw_field_stage_t stage,
        int slice_idx,
        char **cont_name);

typedef int (*field_compression_set_f)(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_field_qset_t *qset,
        uint32_t group_flags,
        bcmlt_entry_handle_t grp_template);

typedef int (*field_compression_del_f)(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_qset_t *qset,
        bcmlt_entry_handle_t grp_template);

typedef int (*field_port_filter_enable_set_f)(
        int unit,
        uint32 state,
        bcm_pbmp_t port_pbmp);

typedef int (*field_qualify_macro_values_get_f)(
        int unit,
        uint8 set_flag,
        bcm_qual_field_t *qual_info,
        bcm_qual_field_values_t *qual_info_values);

typedef int (*field_ifp_clear_f)(
        int unit);

typedef int (*field_vfp_clear_f)(
        int unit);

typedef int (*field_efp_clear_f)(
        int unit);

typedef int (*field_em_clear_f)(
        int unit);

typedef int (*field_ft_clear_f)(
        int unit);

typedef int (*field_clear_f)(
        int unit);

typedef int (*field_lt_qual_name_update_f)(
        int unit,
        bool group_map,
        bcmint_field_stage_info_t *stage_info,
        const bcm_field_qset_t *qset,
        const bcm_field_qual_map_info_t *lt_map_ref,
        char **lt_field_name);

typedef int (*field_udf_qual_set_f)(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_group_oper_t *group_oper,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_udf_id_t udf_id,
        int length,
        uint8 *data,
        uint8 *mask);

typedef int (*field_udf_qual_get_f)(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_group_oper_t *group_oper,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_udf_id_t udf_id,
        int max_len,
        uint8 *data,
        uint8 *mask,
        int *actual_len);

typedef int (*field_udf_qset_set_f)(
        int unit,
        int num_objects,
        int objects_list[],
        bcm_field_qset_t *qset);

typedef int (*field_udf_obj_get_f) (
        int unit,
        bcm_field_qset_t qset,
        int max,
        int *objects_list,
        int *actual);

typedef int (*field_group_mode_set_f) (
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcmlt_entry_handle_t grp_template,
        bool *mode_auto,
        char **group_mode);

typedef int (*field_egr_group_qset_validate_f) (
        int unit,
        bcm_field_group_t group_id,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_qset_t new_qset,
        bcm_field_group_mode_type_t *mode_type,
        bcm_field_group_packet_type_t *packet_type);

typedef int (*field_group_pipe_get_f) (
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t pbmp,
        uint8_t *field_pipe);

typedef int (*field_group_pbmp_get_f) (
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t *pbmp);

typedef int (*field_pbmp_index_get_f) (
        int unit,
        bcm_field_group_t group,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t *in_pbmp,
        bcm_pbmp_t *in_pbmp_mask,
        uint8 *out_pbmp_idx,
        uint8 *out_pbmp_idx_mask);

typedef int (*field_pbmp_get_f) (
        int unit,
        bcm_field_group_t group,
        bcmint_field_stage_info_t *stage_info,
        uint8 *out_pbmp_idx,
        uint8 *out_pbmp_idx_mask,
        bcm_pbmp_t *in_pbmp,
        bcm_pbmp_t *in_pbmp_mask);

typedef int (*field_destination_init_f) (
        int unit);

typedef int (*field_destination_deinit_f) (
        int unit);

typedef int (*field_destination_mirror_index_get_f) (
        int unit,
        bcm_field_destination_match_t  *match,
        int *mirror_index);

typedef int (*field_destination_entry_add_f) (
        int unit,
        uint32   options,
        bcm_field_destination_match_t *match,
        bcm_field_destination_action_t  *action);

typedef int (*field_destination_entry_get_f) (
        int unit,
        bcm_field_destination_match_t *match,
        bcm_field_destination_action_t  *action);

typedef int (*field_destination_entry_traverse_f) (
        int unit,
        bcm_field_destination_entry_traverse_cb callback,
        void *user_data);

typedef int (*field_destination_entry_delete_f) (
        int unit,
        bcm_field_destination_match_t *match);

typedef int (*field_destination_flexctr_attach_f) (
        int unit,
        bcm_field_destination_match_t *match,
        bcmi_ltsw_flexctr_counter_info_t *info);

typedef int (*field_destination_flexctr_detach_f) (
        int unit,
        bcm_field_destination_match_t *match);

typedef int (*field_destination_flexctr_info_get_f) (
        int unit,
        bcm_field_destination_match_t *match,
        bcmi_ltsw_flexctr_counter_info_t *info);

typedef int (*field_destination_flexctr_object_set_f) (
        int unit,
        bcm_field_destination_match_t *match,
        uint32_t value);

typedef int (*field_destination_flexctr_object_get_f) (
        int unit,
        bcm_field_destination_match_t *match,
        uint32_t *value);

typedef int (*field_internal_entry_enable_get_f) (
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bool *enable);

typedef int (*field_qual_gport_validate_f)(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        bcm_gport_t gport,
        bcm_port_t *port);

typedef int (*field_qualify_copytocpureasonhigh_set_f) (
        int unit,
        uint32_t *data,
        uint32_t *mask);

typedef int (*field_qualify_copytocpureasonhigh_get_f) (
        int unit,
        uint32_t *data,
        uint32_t *mask);

typedef int (*field_qual_info_lt_name_get_f) (
        int unit,
        const char *lt_field_name,
        char *num_offset_field_name,
        char *offset_field_name,
        char *width_field_name);

typedef int (*field_policer_attach_f)(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_policer_t policer_id);

typedef int (*field_policer_detach_f)(
        int unit,
        bcm_field_entry_t entry);

typedef int (*field_qual_mask_lt_field_get_f) (
        int unit,
        char *qual_field,
        char **qual_mask_field);

typedef int (*field_action_param_symbol_set_f) (
        int unit,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        char *param_name);

typedef int (*field_action_param_symbol_get_f) (
        int unit,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        char *param_name);

typedef int (*field_qual_value_set_f) (
        int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info);

typedef int (*field_qual_value_get_f) (
        int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info);

typedef int (*field_qual_info_set_f) (
        int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info);

typedef int (*field_entry_table_id_get_f)(
        int unit,
        const char *pt_name,
        uint32_t *table_id,
        uint32_t *entry_idx);

typedef int (*field_aacl_class_id_get_f) (
        int unit,
        bcm_field_qualify_t qid,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        int *class_id,
        bcm_field_qualify_t *pair_qual);

typedef int (*field_action_stat_group_lt_field_validate_f) (
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_hintid_t hintid,
        const char *field_name,
        bool *is_valid);

typedef int (*field_group_udf_qset_update_with_hints_f)(
        int unit,
        bcm_udf_id_t udf_id,
        bcm_field_hint_t *hint,
        bcm_field_qset_t *udf_qset,
        bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap);

typedef int (*field_action_lt_map_valid_info_get_f)(
        int unit,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_entry_t entry_id,
        bcm_field_action_t action,
        uint16_t lt_map_flags,
        uint16_t map_count,
        bool *lt_field_map);

typedef int (*field_action_mirror_set_f)(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport);

typedef int (*field_action_mirror_remove_f)(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport);

typedef int (*field_action_mirror_index_get_f)(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int entry,
    bcm_field_action_t action,
    int mirror_cont,
    bool *enable,
    int *mirror_index);

typedef int (*field_qualify_packetres_set_f) (
        int unit,
        bcm_field_entry_t entry_id,
        uint32_t data,
        uint32_t mask);

typedef int (*field_qualify_packetres_get_f) (
        int unit,
        bcm_field_entry_t entry_id,
        uint32_t *data,
        uint32_t *mask);

typedef int (*field_qualify_mhopcode_set_f) (
        int unit,
        bcm_field_entry_t entry_id,
        uint8_t data,
        uint8_t mask);

typedef int (*field_qualify_mhopcode_get_f) (
        int unit,
        bcm_field_entry_t entry_id,
        uint8_t *data,
        uint8_t *mask);

typedef int (*field_action_default_value_get_f)(
        int unit, bcm_field_entry_t entry_id,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_action_t action,
        bool del_action,
        bcmlt_field_def_t *fld_defs_out);

typedef int (*field_control_lt_info_init_f)(
        int unit,
        bcmint_field_control_info_t *control_info);

typedef int (*field_qualify_port_get_f)(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t *data_modid,
    bcm_module_t *mask_modid,
    bcm_port_t *data_port,
    bcm_port_t *mask_port);

typedef int (*field_qualify_port_set_f)(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t data_modid,
    bcm_module_t mask_modid,
    bcm_port_t data_port,
    bcm_port_t mask_port);

typedef int (*field_qualify_dstl3egress_get_f)(
    int unit,
    bcm_field_entry_t entry,
    bcm_if_t *if_id);

typedef int (*field_qualify_dstl3egress_set_f)(
    int unit,
    bcm_field_entry_t entry,
    bcm_if_t if_id);

typedef int (*field_group_mode_get_f)(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcm_field_group_mode_t *mode);

/*!
 * \brief Field driver structure.
 */
typedef struct mbcm_ltsw_field_drv_s {

    /*! Initialize Field control information. */
    field_control_info_init_f field_control_info_init;

    /*! Initialize Field stage information. */
    field_stage_info_init_f field_stage_info_init;

    /*! Initialize Field stage database information. */
    field_stage_db_init_f field_stage_db_init;

    /*! Initialize Field profile tables. */
    field_ingress_profile_init_f field_ingress_profile_init;

    /*! De-initialize field profile tables. */
    field_ingress_profile_deinit_f field_ingress_profile_deinit;

    /*! Get enum value for given enum, used by enum based qualifiers autogenerated by tooling */
    field_auto_enum_to_lt_enum_value_get_f field_auto_enum_to_lt_enum_value_get;

    /*! Get enum for given enum value, used by enum based qualifiers autogenerated by tooling */
    field_auto_lt_enum_value_to_enum_get_f field_auto_lt_enum_value_to_enum_get;

    /*! Get enum value for given enum, used by enum based qualifiers manually coded */
    field_manual_enum_to_lt_enum_value_get_f field_manual_enum_to_lt_enum_value_get;

    /*! Get enum for given enum value, used by enum based qualifiers manually coded */
    field_manual_lt_enum_value_to_enum_get_f field_manual_lt_enum_value_to_enum_get;

    /*! Set action value. */
    field_action_value_set_f field_action_value_set;

    /*! Get action value. */
    field_action_value_get_f field_action_value_get;

    /*! Get Source Class Mode LT Symbol. */
    field_src_class_enum_to_lt_symbol_get_f field_src_class_enum_to_lt_symbol_get;

    /*! Get Source Class Mode Enum. */
    field_src_class_lt_symbol_to_enum_get_f field_src_class_lt_symbol_to_enum_get;

    /*! Set Encoding of Source class H/W Fields. */
    field_src_class_hw_fields_encode_set_f field_src_class_hw_fields_encode_set;

    /*! Get decoded H/W Fields of Source class. */
    field_src_class_hw_fields_decode_get_f field_src_class_hw_fields_decode_get;

    /*! Get profile index for provided entry & action. */
    field_action_profile_index_get_f field_action_profile_index_get;

    /*! Add action profile. */
    field_action_profile_add_f field_action_profile_add;

    /*! Get action profile. */
    field_action_profile_get_f field_action_profile_get;

    /*! Delete action profile index. */
    field_action_profile_del_f field_action_profile_del;

    /*! Get Mirror container. */
    field_action_meter_cont_get_f field_action_meter_cont_get;

    /*! Set field compression. */
    field_compression_set_f field_compression_set;

    /*! Del field compression type. */
    field_compression_del_f field_compression_del;

    /*! Clear field related LT tables. */
    field_port_filter_enable_set_f field_port_filter_enable_set;

    /*! Get supported values for provided macro based qualifier. */
    field_qualify_macro_values_get_f field_qualify_macro_values_get;

    /*! Clear field IFP related LT tables. */
    field_clear_f field_ifp_clear;

    /*! Clear field VFP related LT tables. */
    field_clear_f field_vfp_clear;

    /*! Clear field EFP related LT tables. */
    field_clear_f field_efp_clear;

    /*! Clear field EFP related LT tables. */
    field_clear_f field_em_clear;

    /*! Clear field FT related LT tables. */
    field_clear_f field_ft_clear;

    /*! Clear field related LT tables. */
    field_clear_f field_clear;

    /*! Clear field related LT tables. */
    field_lt_qual_name_update_f field_lt_qual_name_update;

    /*! Field UDF Qual Set */
    field_udf_qual_set_f field_udf_qual_set;

    /*! Field UDF Qual Get */
    field_udf_qual_get_f field_udf_qual_get;

    /*! Field UDF Qset Set */
    field_udf_qset_set_f field_udf_qset_set;

    /*! Field UDF Objects get */
    field_udf_obj_get_f field_udf_obj_get;

    /*! Field UDF Qset Delete */
    field_udf_qset_set_f field_udf_qset_del;

    /*! Field Group Mode Set */
    field_group_mode_set_f field_group_mode_set;

    /*! Field egress group qset change validate */
    field_egr_group_qset_validate_f field_egr_group_qset_validate;

    /* Field Group Pipe Get */
    field_group_pipe_get_f field_group_pipe_get;

    /* Field Group PBMP Get */
    field_group_pbmp_get_f field_group_pbmp_get;

    /* Field pbmp index Get */
    field_pbmp_index_get_f field_pbmp_index_get;

    /* Field pbmp Get */
    field_pbmp_get_f field_pbmp_get;

    /* Field destination init */
    field_destination_init_f  field_destination_init;

    /* Field destination deinit */
    field_destination_deinit_f  field_destination_deinit;

    /* Field destination mirror index get */
    field_destination_mirror_index_get_f field_destination_mirror_index_get;
    /* Add a field destination entry */
    field_destination_entry_add_f  field_destination_entry_add;

    /* Get a field destination entry */
    field_destination_entry_get_f  field_destination_entry_get;

    /* Traverse field destination entrires */
    field_destination_entry_traverse_f  field_destination_entry_traverse;

    /* Delete a field destination entry */
    field_destination_entry_delete_f  field_destination_entry_delete;

    /* Attach a flex counter to a field destination entry */
    field_destination_flexctr_attach_f  field_destination_flexctr_attach;

    /* Detach a flex counter from a field destination entry */
    field_destination_flexctr_detach_f  field_destination_flexctr_detach;

    /* Get the flex counter info of  a field destination entry */
    field_destination_flexctr_info_get_f  field_destination_flexctr_info_get;

    /* Set the flexctr object value of a field destination entry */
    field_destination_flexctr_object_set_f  field_destination_flexctr_object_set;

    /* Get the flexctr object value of a field destination entry */
    field_destination_flexctr_object_get_f  field_destination_flexctr_object_get;

    /* Get the hook for the dummy entry */
    field_internal_entry_enable_get_f field_internal_entry_enable_get;

    /* validate the gport given as input to qual. */
    field_qual_gport_validate_f field_qual_gport_validate;

    /* set copy to cpu reason */
    field_qualify_copytocpureasonhigh_set_f field_qualify_copytocpureasonhigh_set;

    /* get copy to cpu reason */
    field_qualify_copytocpureasonhigh_get_f field_qualify_copytocpureasonhigh_get;

    /* Field qualifier info LT field name */
    field_qual_info_lt_name_get_f field_qual_info_lt_name_get;

    /* Attach a policer to the field entry. */
    field_policer_attach_f field_policer_attach;

    /* Detach the policer from the field entry. */
    field_policer_detach_f field_policer_detach;

    /*! Field Qual mask LT Field get*/
    field_qual_mask_lt_field_get_f field_qual_mask_lt_field_get;

    /* set action param symbol */
    field_action_param_symbol_set_f field_action_param_symbol_set;

    /* get action param symbol */
    field_action_param_symbol_get_f field_action_param_symbol_get;

    /* Set Field Qual values */
    field_qual_value_set_f field_qual_value_set;

    /* Get Field Qual values */
    field_qual_value_get_f field_qual_value_get;

    /* Set Field Qual Info Struct characteristics */
    field_qual_info_set_f field_qual_info_set;

    /* Get TABLE_ID from PT_NAME. */
    field_entry_table_id_get_f field_entry_table_id_get;

    /* Get aacl class id */
    field_aacl_class_id_get_f field_aacl_class_id_get;

    /* Validate statGroup LT field name */
    field_action_stat_group_lt_field_validate_f
        field_action_stat_group_lt_field_validate;

    /*! Field group udf qset update with Hints bitmap. */
    field_group_udf_qset_update_with_hints_f
        field_group_udf_qset_update_with_hints;

    /*! Get LT mapped fields information for the current action parameters. */
    field_action_lt_map_valid_info_get_f field_action_lt_map_valid_info_get;

    /*! Field action mirror set */
    field_action_mirror_set_f field_action_mirror_set;

    /*! Field action mirror remove */
    field_action_mirror_remove_f field_action_mirror_remove;

    /*! Field action mirror index get */
    field_action_mirror_index_get_f field_action_mirror_index_get;

    /*! set packet resolution */
    field_qualify_packetres_set_f field_qualify_packetres_set;

    /*! get packet resolution */
    field_qualify_packetres_get_f field_qualify_packetres_get;

    /*! Get default action value. */
    field_action_default_value_get_f field_action_default_value_get;

    /*! Initialize Field control LT information. */
    field_control_lt_info_init_f field_control_lt_info_init;

    /*! Get Qualify values of Mod ID and Port */
    field_qualify_port_get_f field_qualify_port_get;

    /*! Set Qualify values of Mod ID and Port */
    field_qualify_port_set_f field_qualify_port_set;

    /*! Get Qualify values of destination L3 egress object. */
    field_qualify_dstl3egress_get_f field_qualify_dstl3egress_get;

    /*! Set Qualify values of destination L3 egress object. */
    field_qualify_dstl3egress_set_f field_qualify_dstl3egress_set;

    /*! Get group mode */
    field_group_mode_get_f field_group_mode_get;
    
    /*! set mhopcode */
    field_qualify_mhopcode_set_f field_qualify_mhopcode_set;

    /*! get mhopcode */
    field_qualify_mhopcode_get_f field_qualify_mhopcode_get;
} mbcm_ltsw_field_drv_t;

/*!
 * \brief Set the Field driver of the device.
 *
 * \param [in] unit Unit Number.
 * \param [in] drv Field driver to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_drv_set(
        int unit,
        mbcm_ltsw_field_drv_t *drv);

/*!
 * \brief Initialize Field control information.
 *
 * \param [in] unit Unit number.
 * \param [out] control_info Field control database.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_control_info_init(
        int unit,
        bcmint_field_control_info_t *control_info);

/*!
 * \brief Get control information.
 *
 * \param [in] unit Unit number.
 * \param [out] control_info control information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_control_info_get(
        int unit,
        bcmint_field_control_info_t *control_info);

/*!
 * \brief Initialize Field stage information.
 *
 * \param [in] unit Unit number.
 * \param [out] stage_info Field stage database.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_stage_info_init(
        int unit,
        bcmint_field_stage_info_t *stage_info);

extern int
mbcm_ltsw_field_stage_db_init(
        int unit,
        bcmint_field_stage_info_t *stage_info);

extern int
mbcm_ltsw_field_ingress_profile_init(
        int unit);

extern int
mbcm_ltsw_field_ingress_profile_deinit(
        int unit);

extern int
mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(
        int unit,
        bcm_field_qualify_t qual,
        uint32 qual_enum,
        bcm_qual_field_t *qual_info);

extern int
mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(
        int unit,
        bcm_field_qualify_t qual,
        bcm_qual_field_t *qual_info,
        uint32 *qual_enum);

extern int
mbcm_ltsw_field_manual_enum_to_lt_enum_value_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        uint32 qual_enum,
        bcm_qual_field_t *qual_info);

extern int
mbcm_ltsw_field_manual_lt_enum_value_to_enum_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        bcm_qual_field_t *qual_info,
        uint32 *qual_enum);

/*!
 * \brief set action value
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_action_value_set(
        int unit, bcm_field_entry_t entry_id,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *action_value);

/*!
 * \brief Get action value
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_action_value_get(
        int unit, bcm_field_entry_t entry_id,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *action_value);

extern int
mbcm_ltsw_field_src_class_enum_to_lt_symbol_get(
        int unit,
        bcm_field_src_class_mode_t mode,
        char **key_fid);

extern int
mbcm_ltsw_field_src_class_lt_symbol_to_enum_get(
        int unit,
        char *key_fid,
        bcm_field_src_class_mode_t *mode);

extern int
mbcm_ltsw_field_src_class_hw_fields_encode_set(
        int unit,
        bcm_field_src_class_mode_t mode,
        bcm_field_src_class_t src_data,
        bcm_field_src_class_t src_mask,
        uint32 *hw_data,
        uint32 *hw_mask);

extern int
mbcm_ltsw_field_src_class_hw_fields_decode_get(
        int unit,
        bcm_field_src_class_mode_t mode,
        uint32 hw_data,
        uint32 hw_mask,
        bcm_field_src_class_t *src_data,
        bcm_field_src_class_t *src_mask);

/*!
 * \brief get profile index for provided action and entry.
 *
 * \param [in] unit          - Unit number.
 *        [in] entry         - Entry identifier.
 *        [in] action        - Action identifier.
 *        [out] act_prof_idx - Associated action profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_action_profile_index_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_action_t action,
        uint16_t *act_prof_idx);

/*!
 * \brief add profile entry for provided action.
 *
 * \param [in] unit              - Unit number.
 *        [in] stage_id         - Stage Id
 *        [in] action            - Action identifier.
 *        [in] params            - Action parameters.
 *        [out] match_config     - Match config.
 *        [out] act_prof_idx_val - action value which is index
 *                                 created for profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_action_profile_add(
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *act_prof_idx_val);

/*!
 * \brief get profile entry for provided action.
 *
 * \param [in] unit             - Unit number.
 *        [in] stage_id         - Stage Id.
 *        [in] action           - Action identifier.
 *        [in] act_prof_idx_val - action value which is index
 *                                to be fetched for profile.
 *        [out] match_config     - Match config.
 *        [out] params          - Action parameters.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_action_profile_get(
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        uint64_t *act_prof_idx_val,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config);

/*!
 * \brief delete profile entry for provided action.
 *
 * \param [in] unit             - Unit number.
 *        [in] stage_id         - Stage Id.
 *        [in] action           - Action identifier.
 *        [in] act_prof_idx_val - action value which is index
 *                                to be fetched for profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_action_profile_del(
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        uint64_t *act_prof_idx_val);

/*!
 * \brief Get FP meter field for a given slice.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_fp_meter_cont_field_get(
        int unit,
        bcmi_ltsw_field_stage_t stage,
        int slice_idx,
        char **cont_field);

/*!
 * \brief Set FP compression.
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
mbcm_ltsw_field_compression_set(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_field_qset_t *qset,
        uint32_t group_flags,
        bcmlt_entry_handle_t templ);

/*!
 * \brief Set FP compression.
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
mbcm_ltsw_field_compression_del(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_qset_t *qset,
        bcmlt_entry_handle_t templ);

/*!
 * \brief Set the filter enable state in the Port Tables.
 *
 * \param [in] unit  Unit Number.
 * \param [in] state TRUE = to enable
 *                   FALSE = to disbale filtering
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_port_filter_enable_set(
        int unit,
        uint32 state,
        bcm_pbmp_t port_pbmp);

/*!
 * \brief get macro flags, value and other data supported for input
 *        macro based qualifier.
 *
 * \param [in] unit                Unit number.
 * \param [in] set_flag            Set_flag(1 for qualify_set and 0 for qualify_get)
 * \param [in] qual_info           Qualifier info.
 * \param [out] qual_info_values   Qualifier values supported.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_qualify_macro_values_get(
        int unit,
        uint8 set_flag,
        bcm_qual_field_t *qual_info,
        bcm_qual_field_values_t *qual_info_values);

/*!
 * \brief Clear field related LT tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_clear(
        int unit);

/*!
 * \Get Field Qualifier name for Group Template
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_field_lt_qual_name_update(
        int unit,
        bool group_map,
        bcmint_field_stage_info_t *stage_info,
        const bcm_field_qset_t *qset,
        const bcm_field_qual_map_info_t *lt_map_ref,
        char **lt_field_name);

extern int
mbcm_ltsw_field_udf_qual_set(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_group_oper_t *group_oper,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_udf_id_t udf_id,
        int length,
        uint8 *data,
        uint8 *mask);

extern int
mbcm_ltsw_field_udf_qual_get(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_group_oper_t *group_oper,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_udf_id_t udf_id,
        int max_len,
        uint8 *data,
        uint8 *mask,
        int *actual_len);

extern int
mbcm_ltsw_field_udf_qset_set(
        int unit,
        int num_objects,
        int objects_list[],
        bcm_field_qset_t *qset);

extern int
mbcm_ltsw_field_udf_obj_get(
        int unit,
        bcm_field_qset_t qset,
        int max,
        int *objects_list,
        int *actual);

extern int
mbcm_ltsw_field_udf_qset_del(
        int unit,
        int num_objects,
        int objects_list[],
        bcm_field_qset_t *qset);

extern int
mbcm_ltsw_field_group_mode_set(
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcmlt_entry_handle_t grp_template,
        bool *mode_auto,
        char **group_mode);

extern int
mbcm_ltsw_field_egr_group_qset_validate(
        int unit,
        bcm_field_group_t group_id,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_qset_t new_qset,
        bcm_field_group_mode_type_t *mode_type,
        bcm_field_group_packet_type_t *packet_type);

extern int
mbcm_ltsw_field_group_pipe_get(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t pbmp,
        uint8_t *field_pipe);

extern int
mbcm_ltsw_field_group_pbmp_get(
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t *pbmp);

extern int
mbcm_ltsw_field_pbmp_index_get(
        int unit,
        bcm_field_group_t group,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t *in_pbmp,
        bcm_pbmp_t *in_pbmp_mask,
        uint8 *out_pbmp_idx,
        uint8 *out_pbmp_idx_mask);

extern int
mbcm_ltsw_field_pbmp_get(
        int unit,
        bcm_field_group_t group,
        bcmint_field_stage_info_t *stage_info,
        uint8 *out_pbmp_idx,
        uint8 *out_pbmp_idx_mask,
        bcm_pbmp_t *in_pbmp,
        bcm_pbmp_t *in_pbmp_mask);


/*!
 * \brief Mbcm field destination init.
 *
 * \param [in]  unit       Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_field_destination_init(
        int unit);

/*!
 * \brief Mbcm field destination deinit.
 *
 * \param [in]  unit       Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_field_destination_deinit(
        int unit);

/*!
 * \brief Get the mirror index used by the Field destination entry.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  match      Match info.
 * \param [out] mirror_index  Mirror index.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_field_destination_mirror_index_get(
        int unit,
        bcm_field_destination_match_t  *match,
        int *mirror_index);

/*!
 * \brief Add a field destination entry.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  options    Entry operation options.
 * \param [in]  match      Match info.
 * \param [in]  action     Action info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_field_destination_entry_add(
        int unit,
        uint32   options,
        bcm_field_destination_match_t  *match,
        bcm_field_destination_action_t  *action);

/*!
 * \brief Get a field destination entry.
 *
 * \param [in]  unit        Unit number.
 * \param [in/out]  match   Match info.
 * \param [out]  action     Action info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_field_destination_entry_get(
        int unit,
        bcm_field_destination_match_t  *match,
        bcm_field_destination_action_t  *action);

/*!
 * \brief Traverse field destination entries.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  callback   User callback.
 * \param [in]  user_data  User cookie.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_field_destination_entry_traverse(
        int unit,
        bcm_field_destination_entry_traverse_cb callback,
        void *user_data);

/*!
 * \brief Delete a field destination entry.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  match      Match info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_field_destination_entry_delete(
        int unit,
        bcm_field_destination_match_t  *match);

/*!
 * \brief Attach a flex counter to the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_field_destination_flexctr_attach(
        int unit,
        bcm_field_destination_match_t *match,
        bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach a flex counter from the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_field_destination_flexctr_detach(
        int unit,
        bcm_field_destination_match_t *match);

/*!
 * \brief Get the info of a flex counter attached to
 *        the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [out] info             Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_field_destination_flexctr_info_get(
        int unit,
        bcm_field_destination_match_t *match,
        bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Set the flexctr object for the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [in] value             Flexctr object value.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_field_destination_flexctr_object_set(
        int unit,
        bcm_field_destination_match_t *match,
        uint32_t value);

/*!
 * \brief Get the flexctr object of the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [out] value            Flexctr object value.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
mbcm_ltsw_field_destination_flexctr_object_get(
        int unit,
        bcm_field_destination_match_t *match,
        uint32_t *value);

extern int
mbcm_ltsw_field_internal_entry_enable_get(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bool *enable);

extern int
mbcm_ltsw_field_qual_gport_validate(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        bcm_gport_t gport,
        bcm_port_t *port);

/*!
 * \brief Set copy to cpu reason high.
 *
 * \param [in]  unit       Unit number.
 * \param [in/out] data
 * \param [in/out] mask
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_field_qualify_copytocpureasonhigh_set(
        int unit,
        uint32_t *data,
        uint32_t *mask);
/*!
 * \brief get copy to cpu reason high.
 *
 * \param [in]  unit       Unit number.
 * \param [in/out] data
 * \param [in/out] mask
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_field_qualify_copytocpureasonhigh_get(
        int unit,
        uint32_t *data,
        uint32_t *mask);

/*!
 * \brief Get qualifier info LT field names.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  lt_field_name         LT field name
 * \param [out] num_offset_field_name Num offset field name.
 * \param [out] offset_field_name     Offset field name.
 * \param [out] width_field_name      Width field name.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */

extern int
mbcm_ltsw_field_qual_info_lt_name_get(
        int unit,
        const char *lt_field_name,
        char *num_offset_field_name,
        char *offset_field_name,
        char *width_field_name);

extern int
mbcm_ltsw_field_entry_policer_attach(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_policer_t policer_id);

extern int
mbcm_ltsw_field_entry_policer_detach(
        int unit,
        bcm_field_entry_t entry);

extern int
mbcm_ltsw_field_qual_mask_lt_field_get(
        int unit,
        char *qual_field,
        char **qual_mask_field);

extern int
mbcm_ltsw_field_action_param_symbol_set(
        int unit,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        char *param_name);
extern int
mbcm_ltsw_field_action_param_symbol_get(
        int unit,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        char *param_name);

extern int
mbcm_ltsw_field_qual_value_set(
        int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info);

extern int
mbcm_ltsw_field_qual_value_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info);

extern int
mbcm_ltsw_field_qual_info_set (
        int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info);

extern int
mbcm_ltsw_field_entry_table_id_get(
        int unit,
        const char *pt_name,
        uint32_t *table_id,
        uint32_t *entry_idx);

extern int
mbcm_ltsw_field_aacl_class_id_get(
        int unit,
        bcm_field_qualify_t qid,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        int *class_id,
        bcm_field_qualify_t *pair_qual);

extern int
mbcm_ltsw_field_action_stat_group_lt_field_validate(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_hintid_t hintid,
        const char *field_name,
        bool *is_valid);

extern int
mbcm_ltsw_field_group_udf_qset_update_with_hints(
        int unit,
        bcm_udf_id_t udf_id,
        bcm_field_hint_t *hint,
        bcm_field_qset_t *udf_qset,
        bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap);

extern int
mbcm_ltsw_field_action_mirror_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport);
extern int
mbcm_ltsw_field_action_mirror_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport);
extern int
mbcm_ltsw_field_action_mirror_index_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int entry,
    bcm_field_action_t action,
    int mirror_cont,
    bool *enable,
    int *mirror_index);

extern int
mbcm_ltsw_field_action_lt_map_valid_info_get(
        int unit,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_entry_t entry_id,
        bcm_field_action_t action,
        uint16_t lt_map_flags,
        uint16_t map_count,
        bool *lt_field_map);

extern int
mbcm_ltsw_field_qualify_packetres_set(
        int unit,
        bcm_field_entry_t entry_id,
        uint32_t data,
        uint32_t mask);

extern int
mbcm_ltsw_field_qualify_packetres_get(
        int unit,
        bcm_field_entry_t entry_id,
        uint32_t *data,
        uint32_t *mask);

extern int
mbcm_ltsw_field_qualify_mhopcode_set(
        int unit,
        bcm_field_entry_t entry_id,
        uint8_t data,
        uint8_t mask);

extern int
mbcm_ltsw_field_qualify_mhopcode_get(
        int unit,
        bcm_field_entry_t entry_id,
        uint8_t *data,
        uint8_t *mask);


extern int
mbcm_ltsw_field_action_default_value_get(
        int unit, bcm_field_entry_t entry_id,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_action_t action,
        bool del_action,
        bcmlt_field_def_t *fld_defs_out);

extern int
mbcm_ltsw_field_control_lt_info_init(
        int unit,
        bcmint_field_control_info_t *ctrl_info);

extern int
mbcm_ltsw_field_qualify_port_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t *data_modid,
    bcm_module_t *mask_modid,
    bcm_port_t *data_port,
    bcm_port_t *mask_port);

extern int
mbcm_ltsw_field_qualify_port_set(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t data_modid,
    bcm_module_t mask_modid,
    bcm_port_t data_port,
    bcm_port_t mask_port);

extern int
mbcm_ltsw_field_qualify_dstl3egress_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_if_t *if_id);

extern int
mbcm_ltsw_field_qualify_dstl3egress_set(
    int unit,
    bcm_field_entry_t entry,
    bcm_if_t if_id);

extern int
mbcm_ltsw_field_group_mode_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcm_field_group_mode_t *mode);

#endif /* BCMINT_LTSW_MBCM_FIELD_H */
