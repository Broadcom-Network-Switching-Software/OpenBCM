/** \file field_map_data_dispatch.c
 * 
 *
 * Data for bcm to dnx and dnx to HW mapping
 *
 * This file implements mapping from BCM to DNX qualifiers and access for per DNX qualifier/action information
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field_map.h>


#include "field_map_local.h"
/*
 * }Include files
 */

shr_error_e
dnx_field_map_port_profile_info_get_dispatch(
    int unit,
    bcm_port_class_t bcm_port_class,
    dnx_field_map_port_profile_info_t * port_profile_info_p)
{

    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_port_profile_info_get(unit, bcm_port_class, port_profile_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_port_profile_type_get_dispatch(
    int unit,
    bcm_port_class_t bcm_port_class,
    dnx_field_port_porfile_type_e * port_profile_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_port_profile_type_get(unit, bcm_port_class, port_profile_type_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_global_action_map_get_dispatch(
    int unit,
    bcm_field_action_t bcm_action,
    dnx_field_action_map_t * global_action_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_global_action_map_get(unit, bcm_action, global_action_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_global_qual_map_get_dispatch(
    int unit,
    bcm_field_qualify_t bcm_qual,
    dnx_field_qual_map_t * global_qual_map_p)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_global_qual_map_get(unit, bcm_qual, global_qual_map_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_stage_info_get_dispatch(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_map_stage_info_t * map_stage_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_stage_info_get(unit, stage, map_stage_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_static_action_map_get_dispatch(
    int unit,
    dnx_field_stage_e stage,
    bcm_field_action_t bcm_action,
    dnx_field_action_map_t * static_action_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_static_action_map_get(unit, stage, bcm_action, static_action_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_meta_qual_map_get_dispatch(
    int unit,
    dnx_field_stage_e stage,
    bcm_field_qualify_t bcm_qual,
    dnx_field_qual_map_t * meta_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_meta_qual_map_get(unit, stage, bcm_qual, meta_qual_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_cs_qual_map_get_dispatch(
    int unit,
    dnx_field_stage_e stage,
    bcm_field_qualify_t bcm_qual,
    dnx_field_cs_qual_map_t * cs_qual_map_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_map_get(unit, stage, bcm_qual, cs_qual_map_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_meta_qual_info_get_dispatch(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_id_t qual_id,
    dnx_field_meta_qual_info_t * meta_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_meta_qual_info_get(unit, stage, qual_id, meta_qual_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_layer_record_qual_info_get_dispatch(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_id_t qual_id,
    dnx_field_layer_record_qual_info_t * layer_record_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_layer_record_qual_info_get(unit, stage, qual_id, layer_record_qual_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_static_action_info_get_dispatch(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_id_t action_id,
    dnx_field_base_action_info_t * static_action_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_static_action_info_get(unit, stage, action_id, static_action_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_context_param_info_get_dispatch(
    int unit,
    bcm_field_context_param_type_t param_type,
    bcm_field_stage_t stage,
    dnx_field_context_param_t * context_param_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_param_info_get(unit, param_type, stage, context_param_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_range_map_legacy_get_dispatch(
    int unit,
    dnx_field_range_type_e range_type,
    dnx_field_stage_e stage,
    dnx_field_range_map_t * range_map_legacy_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_range_map_legacy_get(unit, range_type, stage, range_map_legacy_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_pp_app_map_get_dispatch(
    int unit,
    bcm_field_app_db_t app_db,
    dnx_field_pp_app_map_t * pp_app_map_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_pp_app_map_get(unit, app_db, pp_app_map_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_input_type_info_get_dispatch(
    int unit,
    dnx_field_input_type_e input_type,
    dnx_field_qual_input_type_info_t * input_type_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_input_type_info_get(unit, input_type, input_type_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_header_qual_info_get_dispatch(
    int unit,
    dnx_field_qual_id_t qual_id,
    dnx_field_header_qual_info_t * header_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_header_qual_info_get(unit, qual_id, header_qual_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_sw_qual_info_get_dispatch(
    int unit,
    dnx_field_qual_id_t qual_id,
    dnx_field_sw_qual_info_t * sw_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_sw_qual_info_get(unit, qual_id, sw_qual_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map.h */
shr_error_e
dnx_field_map_sw_action_info_get_dispatch(
    int unit,
    dnx_field_action_id_t action_id,
    dnx_field_base_action_info_t * sw_action_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_sw_action_info_get(unit, action_id, sw_action_info_p));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
dnx_field_action_description_get_dispatch(
    int unit,
    bcm_field_action_t bcm_action,
    char **action_description)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_action_description_get(unit, bcm_action, action_description));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_action_description_is_empty_dispatch(
    int unit,
    bcm_field_action_t bcm_action,
    uint8 *is_empty)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_action_description_is_empty(unit, bcm_action, is_empty));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_qual_description_get_dispatch(
    int unit,
    bcm_field_qualify_t bcm_qual,
    char **qual_description)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_qual_description_get(unit, bcm_qual, qual_description));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_qual_description_is_empty_dispatch(
    int unit,
    bcm_field_qualify_t bcm_qual,
    uint8 *is_empty)
{
    SHR_FUNC_INIT_VARS(unit);
#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(dnx_field_qual_description_is_empty(unit, bcm_qual, is_empty));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_static_opcode_to_apptype_get_dispatch(
    int unit,
    uint32 static_opcode_id,
    bcm_field_AppType_t * apptype_p)
{
    shr_error_e rv = _SHR_E_INTERNAL;
    SHR_FUNC_INIT_VARS(unit);
    {
#if defined(BCM_DNX2_SUPPORT)
        rv = dnx_field_map_static_opcode_to_apptype_get(unit, static_opcode_id, apptype_p);
#endif
    }

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_static_opcode_to_cs_profile_get_dispatch(
    int unit,
    uint32 static_opcode_id,
    uint32 *cs_profile_p)
{
    shr_error_e rv = _SHR_E_INTERNAL;
    SHR_FUNC_INIT_VARS(unit);
    {
#if defined(BCM_DNX2_SUPPORT)
        rv = dnx_field_map_static_opcode_to_cs_profile_get(unit, static_opcode_id, cs_profile_p);
#endif
    }

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_standard_1_predefined_apptype_to_opcode_get_dispatch(
    int unit,
    bcm_field_AppType_t apptype,
    uint32 *static_opcode_id_p)
{
    shr_error_e rv = _SHR_E_INTERNAL;
    SHR_FUNC_INIT_VARS(unit);
    {
#if defined(BCM_DNX2_SUPPORT)
        rv = dnx_field_map_standard_1_predefined_apptype_to_opcode_get(unit, apptype, static_opcode_id_p);
#endif
    }

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}
