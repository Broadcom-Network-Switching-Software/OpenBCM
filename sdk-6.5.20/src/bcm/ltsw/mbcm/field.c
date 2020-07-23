/*! \file field.c
 *
 * BCM Field module multiplexing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/field.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FP

static mbcm_ltsw_field_drv_t mbcm_ltsw_field_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */
int
mbcm_ltsw_field_drv_set(
    int unit,
    mbcm_ltsw_field_drv_t *drv)
{
    mbcm_ltsw_field_drv_t *local = &mbcm_ltsw_field_drv[unit];

    SHR_FUNC_ENTER(unit);

    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                       (mbcm_ltsw_drv_t *)drv,
                       sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_control_info_init(
    int unit,
    bcmint_field_control_info_t *control_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_control_info_init(unit, control_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_control_info_get(
        int unit,
        bcmint_field_control_info_t *control_info)
{
    SHR_FUNC_ENTER(unit);

    
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_stage_info_init(
        int unit,
        bcmint_field_stage_info_t *stage_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_stage_info_init(unit, stage_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_stage_db_init(
        int unit,
        bcmint_field_stage_info_t *stage_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_stage_db_init(unit, stage_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_ingress_profile_init(
        int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_ingress_profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_ingress_profile_deinit(
        int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_ingress_profile_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_auto_enum_to_lt_enum_value_get(
    int unit,
    bcm_field_qualify_t qual,
    uint32 qual_enum,
    bcm_qual_field_t *qual_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_auto_enum_to_lt_enum_value_get(unit,
                                                                        qual,
                                                                        qual_enum,
                                                                        qual_info));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_auto_lt_enum_value_to_enum_get(
        int unit,
        bcm_field_qualify_t qual,
        bcm_qual_field_t *qual_info,
        uint32 *qual_enum)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_auto_lt_enum_value_to_enum_get(unit,
                                                                        qual,
                                                                        qual_info,
                                                                        qual_enum));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_manual_enum_to_lt_enum_value_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        uint32 qual_enum,
        bcm_qual_field_t *qual_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_manual_enum_to_lt_enum_value_get(unit,
                                                                          entry,
                                                                          qual,
                                                                          qual_enum,
                                                                          qual_info));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_manual_lt_enum_value_to_enum_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        bcm_qual_field_t *qual_info,
        uint32 *qual_enum)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_manual_lt_enum_value_to_enum_get(unit,
                                                                          entry,
                                                                          qual,
                                                                          qual_info,
                                                                          qual_enum));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_value_set(
        int unit,
        bcm_field_entry_t entry_id,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *action_value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_value_set(unit,
                                                          entry_id,
                                                          stage,
                                                          action,
                                                          params,
                                                          match_config,
                                                          action_value));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_value_get(
        int unit,
        bcm_field_entry_t entry_id,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *action_value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_value_get(unit,
                                                          entry_id,
                                                          stage,
                                                          action,
                                                          params,
                                                          match_config,
                                                          action_value));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_src_class_enum_to_lt_symbol_get(
        int unit,
        bcm_field_src_class_mode_t mode,
        char **key_fid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_src_class_enum_to_lt_symbol_get(unit,
                                                                         mode,
                                                                         key_fid));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_src_class_lt_symbol_to_enum_get(
        int unit,
        char *key_fid,
        bcm_field_src_class_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_src_class_lt_symbol_to_enum_get(unit,
                                                                         key_fid,
                                                                         mode));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_src_class_hw_fields_encode_set(
        int unit,
        bcm_field_src_class_mode_t mode,
        bcm_field_src_class_t src_data,
        bcm_field_src_class_t src_mask,
        uint32 *hw_data,
        uint32 *hw_mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_src_class_hw_fields_encode_set(unit,
                                                                        mode,
                                                                        src_data,
                                                                        src_mask,
                                                                        hw_data,
                                                                        hw_mask));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_src_class_hw_fields_decode_get(
        int unit,
        bcm_field_src_class_mode_t mode,
        uint32 hw_data,
        uint32 hw_mask,
        bcm_field_src_class_t *src_data,
        bcm_field_src_class_t *src_mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_src_class_hw_fields_decode_get(unit,
                                                                        mode,
                                                                        hw_data,
                                                                        hw_mask,
                                                                        src_data,
                                                                        src_mask));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_profile_index_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_action_t action,
        uint16_t *act_prof_idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_profile_index_get(unit,
                                                                  entry, action,
                                                                  act_prof_idx));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_profile_add(
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *act_prof_idx_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_profile_add(unit,
                                                            stage_id,
                                                            action, params,
                                                            match_config,
                                                            act_prof_idx_val));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_profile_get(
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        uint64_t *act_prof_idx_val,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_profile_get(unit,
                                                            stage_id,
                                                            action, act_prof_idx_val,
                                                            params,
                                                            match_config));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_profile_del(
        int unit,
        bcmi_ltsw_field_stage_t stage_id,
        bcm_field_action_t action,
        uint64_t *act_prof_idx_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_profile_del(unit, stage_id,
                                                            action, act_prof_idx_val));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_fp_meter_cont_field_get(
        int unit,
        bcmi_ltsw_field_stage_t stage,
        int slice_idx,
        char **cont_field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_meter_cont_get(unit,
                                                               stage,
                                                               slice_idx,
                                                               cont_field));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_compression_set(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_field_qset_t *qset,
        uint32_t group_flags,
        bcmlt_entry_handle_t templ)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_compression_set(unit,
                                                         stage_info,
                                                         entry_oper,
                                                         qset,
                                                         group_flags,
                                                         templ));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_compression_del(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_qset_t *qset,
        bcmlt_entry_handle_t templ)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_compression_del(unit,
                                                         stage_info,
                                                         qset,
                                                         templ));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_port_filter_enable_set(
        int unit,
        uint32 state,
        bcm_pbmp_t port_pbmp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_port_filter_enable_set(unit,
                                                                state,
                                                                port_pbmp));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_macro_values_get(
        int unit,
        uint8 set_flag,
        bcm_qual_field_t *qual_info,
        bcm_qual_field_values_t *qual_info_values)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_macro_values_get(unit,
                                                                  set_flag,
                                                                  qual_info,
                                                                  qual_info_values));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_lt_qual_name_update(
        int unit,
        bool group_map,
        bcmint_field_stage_info_t *stage_info,
        const bcm_field_qset_t *qset,
        const bcm_field_qual_map_info_t *lt_map_ref,
        char **lt_field_name)
{
    SHR_FUNC_ENTER(unit);

   SHR_IF_ERR_EXIT_EXCEPT_IF
        (mbcm_ltsw_field_drv[unit].field_lt_qual_name_update(unit,
                                                             group_map,
                                                             stage_info,
                                                             qset,
                                                             lt_map_ref,
                                                             lt_field_name),
                                                             SHR_E_UNAVAIL);
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_udf_qual_set(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_group_oper_t *group_oper,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_udf_id_t udf_id,
        int length,
        uint8 *data,
        uint8 *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_udf_qual_set(unit,
                                                      stage_info,
                                                      group_oper,
                                                      entry_oper,
                                                      udf_id,
                                                      length,
                                                      data,
                                                      mask));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_udf_qual_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcm_udf_id_t udf_id,
    int max_len,
    uint8 *data,
    uint8 *mask,
    int *actual_len)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_udf_qual_get(unit,
                                                      stage_info,
                                                      group_oper,
                                                      entry_oper,
                                                      udf_id,
                                                      max_len,
                                                      data,
                                                      mask,
                                                      actual_len));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_udf_qset_set(
        int unit,
        int num_objects,
        int objects_list[],
        bcm_field_qset_t *qset)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_udf_qset_set(unit,
                                                      num_objects,
                                                      objects_list,
                                                      qset));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_udf_obj_get(
    int unit,
    bcm_field_qset_t qset,
    int max,
    int *objects_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_udf_obj_get(unit,
                                                     qset,
                                                     max,
                                                     objects_list,
                                                     actual));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_udf_qset_del(
    int unit,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_udf_qset_del(unit,
                                                      num_objects,
                                                      objects_list,
                                                      qset));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_group_mode_set(
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcmlt_entry_handle_t grp_template,
        bool *mode_auto,
        char **group_mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_group_mode_set(unit,
                                                        group_config,
                                                        stage_info,
                                                        grp_template,
                                                        mode_auto,
                                                        group_mode));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qual_gport_validate(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        bcm_gport_t gport,
        bcm_port_t *port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qual_gport_validate(unit,
                                                             entry,
                                                             qual,
                                                             gport,
                                                             port));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_egr_group_qset_validate(
        int unit,
        bcm_field_group_t group_id,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_qset_t new_qset,
        bcm_field_group_mode_type_t *mode_type,
        bcm_field_group_packet_type_t *packet_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_egr_group_qset_validate(unit,
                                                                 group_id,
                                                                 stage_info,
                                                                 new_qset,
                                                                 mode_type,
                                                                 packet_type));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_group_pipe_get(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t pbmp,
        uint8_t *field_pipe)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
            (mbcm_ltsw_field_drv[unit].field_group_pipe_get(unit,
                                                            stage_info,
                                                            pbmp,
                                                            field_pipe));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_group_pbmp_get(
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t *pbmp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_group_pbmp_get(unit,
                                                        group_config,
                                                        stage_info,
                                                        pbmp));
exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_field_pbmp_index_get(
        int unit,
        bcm_field_group_t group,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t *in_pbmp,
        bcm_pbmp_t *in_pbmp_mask,
        uint8 *out_pbmp_idx,
        uint8 *out_pbmp_idx_mask)
{
      SHR_FUNC_ENTER(unit);

      SHR_IF_ERR_EXIT
          (mbcm_ltsw_field_drv[unit].field_pbmp_index_get(unit,
                                                          group,
                                                          stage_info,
                                                          in_pbmp,
                                                          in_pbmp_mask,
                                                          out_pbmp_idx,
                                                          out_pbmp_idx_mask));
exit:
      SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_pbmp_get(
        int unit,
        bcm_field_group_t group,
        bcmint_field_stage_info_t *stage_info,
        uint8 *out_pbmp_idx,
        uint8 *out_pbmp_idx_mask,
        bcm_pbmp_t *in_pbmp,
        bcm_pbmp_t *in_pbmp_mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_pbmp_get(unit,
                                                  group,
                                                  stage_info,
                                                  out_pbmp_idx,
                                                  out_pbmp_idx_mask,
                                                  in_pbmp,
                                                  in_pbmp_mask));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_init(unit));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_deinit(unit));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_mirror_index_get(
    int unit,
    bcm_field_destination_match_t *match,
    int *mirror_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_mirror_index_get(
                                        unit, match, mirror_index));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_entry_add(
    int unit,
    uint32   options,
    bcm_field_destination_match_t  *match,
    bcm_field_destination_action_t  *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_entry_add(unit,
                                                               options,
                                                               match,
                                                               action));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_entry_get(
    int unit,
    bcm_field_destination_match_t  *match,
    bcm_field_destination_action_t  *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_entry_get(unit,
                                                               match,
                                                               action));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_entry_traverse(
    int unit,
    bcm_field_destination_entry_traverse_cb callback,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_entry_traverse(unit,
                                                                    callback,
                                                                    user_data));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_entry_delete(
    int unit,
    bcm_field_destination_match_t  *match)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_entry_delete(unit,
                                                                  match));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_flexctr_attach(
    int unit,
    bcm_field_destination_match_t *match,
    bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_flexctr_attach(unit,
                                                                    match,
                                                                    info));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_flexctr_detach(
    int unit,
    bcm_field_destination_match_t  *match)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_flexctr_detach(unit,
                                                                    match));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_flexctr_info_get(
    int unit,
    bcm_field_destination_match_t *match,
    bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_flexctr_info_get(unit,
                                                                      match,
                                                                      info));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_flexctr_object_set(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_flexctr_object_set(unit,
                                                                        match,
                                                                        value));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_destination_flexctr_object_get(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv[unit].field_destination_flexctr_object_get(unit,
                                                                        match,
                                                                        value));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_internal_entry_enable_get(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bool *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_internal_entry_enable_get(unit,
                                                                   stage_info,
                                                                   enable));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_copytocpureasonhigh_set(
    int unit,
    uint32_t *data,
    uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_copytocpureasonhigh_set(unit,
                                                            data,
                                                            mask));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_copytocpureasonhigh_get(
    int unit,
    uint32_t *data,
    uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_copytocpureasonhigh_get(unit,
                                                            data,
                                                            mask));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qual_info_lt_name_get(
    int unit,
    const char *lt_field_name,
    char *num_offset_field_name,
    char *offset_field_name,
    char *width_field_name)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
            (mbcm_ltsw_field_drv[unit].field_qual_info_lt_name_get(unit,
                                                           lt_field_name,
                                                           num_offset_field_name,
                                                           offset_field_name,
                                                           width_field_name));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_entry_policer_attach(int unit,
                                     bcmint_field_stage_info_t *stage_info,
                                     bcmi_field_ha_entry_oper_t *entry_oper,
                                     bcm_policer_t policer_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_policer_attach(unit,
                                                        stage_info,
                                                        entry_oper,
                                                        policer_id));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_entry_policer_detach(int unit,
                                     bcm_field_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_policer_detach(unit,
                                                        entry));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qual_mask_lt_field_get(
        int unit,
        char *qual_field,
        char **qual_mask_field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qual_mask_lt_field_get(unit,
                                                                qual_field,
                                                                qual_mask_field));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_param_symbol_set(
                 int unit,
                 bcm_field_action_t action,
                 bcm_field_action_params_t *params,
                 char *param_name)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_param_symbol_set(unit,
                                                           action,
                                                           params,
                                                           param_name));
exit:
    SHR_FUNC_EXIT();
}
int
mbcm_ltsw_field_action_param_symbol_get(
                 int unit,
                 bcm_field_action_t action,
                 bcm_field_action_params_t *params,
                 char *param_name)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_param_symbol_get(unit,
                                                           action,
                                                           params,
                                                           param_name));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qual_value_set(
    int unit,
    bcm_field_entry_t entry,
    bcm_qual_field_t *qual_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qual_value_set(unit,
                                                        entry,
                                                        qual_info));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qual_value_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_qual_field_t *qual_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qual_value_get(unit,
                                                        entry,
                                                        qual_info));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qual_info_set(
    int unit,
    bcm_field_entry_t entry,
    bcm_qual_field_t *qual_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qual_info_set(unit,
                                                        entry,
                                                        qual_info));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_entry_table_id_get(
    int unit,
    const char *pt_name,
    uint32_t *table_id,
    uint32_t *entry_idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_entry_table_id_get(
                unit,
                pt_name,
                table_id,
                entry_idx));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_aacl_class_id_get(
    int unit,
    bcm_field_qualify_t qid,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int *class_id,
    bcm_field_qualify_t *pair_qual)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_aacl_class_id_get(unit,
                                                        qid,
                                                        stage_info,
                                                        entry_oper,
                                                        class_id,
                                                        pair_qual));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_stat_group_lt_field_validate(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_hintid_t hintid,
    const char *field_name,
    bool *is_valid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_stat_group_lt_field_validate(
            unit, stage_info, hintid, field_name, is_valid));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_group_udf_qset_update_with_hints(
        int unit,
        bcm_udf_id_t udf_id,
        bcm_field_hint_t *hint,
        bcm_field_qset_t *udf_qset,
        bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_group_udf_qset_update_with_hints(
                                                                          unit,
                                                                          udf_id,
                                                                          hint,
                                                                          udf_qset,
                                                                          udf_qset_bitmap));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_lt_map_valid_info_get(
       int unit,
       bcmi_ltsw_field_stage_t stage,
       bcm_field_entry_t entry_id,
       bcm_field_action_t action,
       uint16_t lt_map_flags,
       uint16_t map_count,
       bool *lt_field_map)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_lt_map_valid_info_get(unit,
                                                                stage,
                                                                entry_id,
                                                                action,
                                                                lt_map_flags,
                                                                map_count,
                                                                lt_field_map));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_mirror_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_mirror_set(unit, stage_info,
                                                           entry, action,
                                                           mirror_gport));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_mirror_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_mirror_remove(unit, stage_info,
                                                              entry, action,
                                                              mirror_gport));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_action_mirror_index_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int entry,
    bcm_field_action_t action,
    int mirror_cont,
    bool *enable,
    int *mirror_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_mirror_index_get(
                unit, stage_info,
                entry, action,
                mirror_cont,
                enable,
                mirror_index));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_packetres_set(
    int unit,
    bcm_field_entry_t entry_id,
    uint32_t data,
    uint32_t mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_packetres_set(unit,
                                                               entry_id,
                                                               data,
                                                               mask));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_packetres_get(
    int unit,
    bcm_field_entry_t entry_id,
    uint32_t *data,
    uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_packetres_get(unit,
                                                               entry_id,
                                                               data,
                                                               mask));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_mhopcode_set(
    int unit,
    bcm_field_entry_t entry_id,
    uint8_t data,
    uint8_t mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_mhopcode_set(unit,
                                                              entry_id,
                                                              data,
                                                              mask));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_mhopcode_get(
    int unit,
    bcm_field_entry_t entry_id,
    uint8_t *data,
    uint8_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_mhopcode_get(unit,
                                                              entry_id,
                                                              data,
                                                              mask));
exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_field_action_default_value_get(
        int unit,
        bcm_field_entry_t entry_id,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_action_t action,
        bool del_action,
        bcmlt_field_def_t *fld_defs_out)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_action_default_value_get(unit,
                                                                  entry_id,
                                                                  stage_info,
                                                                  action,
                                                                  del_action,
                                                                  fld_defs_out));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_control_lt_info_init(
        int unit,
        bcmint_field_control_info_t *ctrl_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_control_lt_info_init(unit, ctrl_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_port_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t *data_modid,
    bcm_module_t *mask_modid,
    bcm_port_t *data_port,
    bcm_port_t *mask_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_port_get(unit,
                                                          entry,
                                                          qual,
                                                          data_modid,
                                                          mask_modid,
                                                          data_port,
                                                          mask_port));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_port_set(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t data_modid,
    bcm_module_t mask_modid,
    bcm_port_t data_port,
    bcm_port_t mask_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_port_set(unit,
                                                          entry,
                                                          qual,
                                                          data_modid,
                                                          mask_modid,
                                                          data_port,
                                                          mask_port));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_dstl3egress_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_if_t *if_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_dstl3egress_get(unit,
                                                                 entry,
                                                                 if_id));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_qualify_dstl3egress_set(
        int unit,
        bcm_field_entry_t entry,
        bcm_if_t if_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_qualify_dstl3egress_set(unit,
                                                                 entry,
                                                                 if_id));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_field_group_mode_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcm_field_group_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv[unit].field_group_mode_get(unit, stage_info,
                                                        group_oper, mode));
exit:
    SHR_FUNC_EXIT();
}

