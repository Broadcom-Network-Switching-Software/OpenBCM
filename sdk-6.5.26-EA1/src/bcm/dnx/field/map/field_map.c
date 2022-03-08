/** \file field_map.c
 * 
 *
 * Field database procedures for DNX.
 *
 * This file implements mapping from BCM to DNX qualifiers and access for per DNX qualifier/action information
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>

#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/sand/sand_signals.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_actions_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_apptype_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_range_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_context_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>

#include <bcm/field.h>
#include <bcm/switch.h>

#include "field_map_local.h"

#include <include/soc/dnx/pemladrv/pemladrv_virtual_wires.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dbal/dbal_dynamic.h>

#include "auto_generated/dbx_pre_compiled_fwd_app_type_auto_generated_xml_parser.h"

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
#include <bcm_int/dnx/field/field_kbp.h>
#endif
#include <bcm_int/dnx/kbp/kbp_mngr.h>

/*
 * }Include files
 */

extern const dnx_field_context_param_t context_param_set[bcmFieldContextParamCount][bcmFieldStageCount];
extern const dnx_field_range_map_t range_map_legacy[DNX_FIELD_RANGE_TYPE_NOF][DNX_FIELD_STAGE_NOF];
/*
 * See field_map.h
 */
char *
dnx_field_stage_text(
    int unit,
    dnx_field_stage_e stage)
{
    char *stage_n;
    dnx_pp_stage_e pp_stage;

    if (stage == DNX_FIELD_STAGE_INVALID)
    {   /** Intentionally use INVALID(-1) as designation for global objects */
        stage_n = "Global";
    }
    else if (((int) stage < DNX_FIELD_STAGE_FIRST) || (stage >= DNX_FIELD_STAGE_NOF))
    {
        stage_n = "Invalid Stage";
    }
    else if (stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        /**
         * Adding this exception, because the pp_stage name 'FWD2' is different than
         * field stage name "EXTERNAL".
         */
        stage_n = "EXTERNAL";
    }
    else if (stage == DNX_FIELD_STAGE_L4_OPS)
    {
        /**
         * Adding this exception, because no pp_stage is supplied for L4_OPS stage.
         * field stage name "L4_OPS".
         */
        stage_n = "L4_OPS";
    }
    else
    {
        if ((pp_stage = dnx_field_map_stage_info[stage].pp_stage) == DNX_PP_STAGE_INVALID)
        {
            stage_n = "Unnamed Stage";
        }
        else
        {
            /*
             * Temporary keep pp_stage in map_stage_info and use it to fetch dnx_data
             * will be obsolete after replacement of dnx_field_stage by dnx_pp-stage
             */
            stage_n = dnx_pp_stage_name(unit, pp_stage);
        }
    }
    return stage_n;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_stage_text(
    bcm_field_stage_t bcm_stage)
{
    char *stage_n;
    char *bcm_stage_text[bcmFieldStageCount] = BCM_FIELD_STAGE_STRINGS;

    if ((bcm_stage < bcmFieldStageFirst) || (bcm_stage >= bcmFieldStageCount))
    {
        stage_n = "Invalid Stage";
    }
    else
    {
        stage_n = bcm_stage_text[bcm_stage];
    }
    return stage_n;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_layer_type_text(
    bcm_field_layer_type_t bcm_layer_type)
{
    char *layer_type_n;
    char *bcm_layer_type_text[bcmFieldLayerTypeCount] = BCM_FIELD_LAYER_TYPE_STRINGS;

    if ((bcm_layer_type < bcmFieldLayerTypeFirst) || (bcm_layer_type >= bcmFieldLayerTypeCount))
    {
        layer_type_n = "Invalid Layer";
    }
    else
    {
        layer_type_n = bcm_layer_type_text[bcm_layer_type];
    }
    return layer_type_n;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_input_type_text(
    bcm_field_input_types_t bcm_input_type)
{
    char *input_type_name;
    char *bcm_input_type_text[bcmFieldInputTypeCount] = BCM_FIELD_INPUT_TYPES_STRINGS;

    if ((bcm_input_type < bcmFieldInputTypeLayerFwd) || (bcm_input_type >= bcmFieldInputTypeCount))
    {
        input_type_name = "Invalid Input Type";
    }
    else
    {
        input_type_name = bcm_input_type_text[bcm_input_type];
    }
    return input_type_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_packet_remove_layer_text(
    bcm_field_packet_remove_layers_t bcm_packet_remove_layer)
{
    char *remove_layer_name;
    char *bcm_remove_layer_text[bcmFieldPacketRemoveLayerCount] = BCM_FIELD_PACKET_REMOVE_LAYER_STRINGS;

    if ((bcm_packet_remove_layer < bcmFieldPacketRemoveLayerOffset0)
        || (bcm_packet_remove_layer >= bcmFieldPacketRemoveLayerCount))
    {
        remove_layer_name = "Invalid Packet Remove Layer";
    }
    else
    {
        remove_layer_name = bcm_remove_layer_text[bcm_packet_remove_layer];
    }
    return remove_layer_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_tcam_bank_allocation_mode_text(
    bcm_field_tcam_bank_allocation_mode_t bcm_tcam_bank_allocation_mode)
{
    char *tcam_bank_allocation_mode_name;
    /** The array size should be +1, because of the invalid value, which we have in the strings. */
    char *bcm_tcam_bank_allocation_mode_text[bcmFieldTcamBankAllocationModeCount] =
        BCM_FIELD_TCAM_BANK_ALLOCATION_MODE_STRINGS;

    if ((bcm_tcam_bank_allocation_mode < bcmFieldTcamBankAllocationModeAuto)
        || (bcm_tcam_bank_allocation_mode >= bcmFieldTcamBankAllocationModeCount))
    {
        tcam_bank_allocation_mode_name = "Invalid Tcam Bank Allocation Mode";
    }
    else
    {
        tcam_bank_allocation_mode_name = bcm_tcam_bank_allocation_mode_text[bcm_tcam_bank_allocation_mode];
    }
    return tcam_bank_allocation_mode_name;
}

/*
 * See field_map.h
 */
CONST char *
dnx_field_bcm_apptype_text(
    int unit,
    bcm_field_AppType_t bcm_apptype)
{
    CONST char *apptype_name;
    uint8 is_standard_image;
    shr_error_e rv;
    char *image_name;

    rv = dnx_pp_prgm_device_image_name_get(unit, &image_name);
    if (rv != _SHR_E_NONE)
    {
        apptype_name = "Invalid Apptype";
        return apptype_name;
    }

    rv = dnx_pp_prgm_default_image_check(unit, &is_standard_image);
    if (rv != _SHR_E_NONE)
    {
        apptype_name = "Invalid Apptype";
        return apptype_name;
    }

    if (is_standard_image)
    {
        char *bcm_apptype_text[bcmFieldAppTypeCount] = BCM_FIELD_APPTYPE_STRINGS;
        if ((bcm_apptype < bcmFieldAppTypeAny) || (bcm_apptype >= bcmFieldAppTypeCount))
        {
            apptype_name = "Invalid Apptype";
        }
        else
        {
            apptype_name = bcm_apptype_text[bcm_apptype];
        }
    }
    else
    {
        uint32 opcode_enum_val;
        rv = dbal_fields_enum_value_get(unit, DBAL_FIELD_FORWARD_APP_TYPES, bcm_apptype, &opcode_enum_val);
        if (rv != _SHR_E_NONE)
        {
            apptype_name = "Invalid Apptype";
            return apptype_name;
        }
        apptype_name = dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_FORWARD_APP_TYPES, opcode_enum_val);
    }

    return apptype_name;

}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_range_type_text(
    bcm_field_range_type_t bcm_rangetype)
{
    char *rangetype_name;
    char *bcm_rangetype_text[bcmFieldRangeTypeCount] = BCM_FIELD_RANGE_TYPE_STR;

    if ((bcm_rangetype < bcmFieldRangeTypeL4SrcPort) || (bcm_rangetype >= bcmFieldRangeTypeCount))
    {
        rangetype_name = "Invalid RangeType";
    }
    else
    {
        rangetype_name = bcm_rangetype_text[bcm_rangetype];
    }
    return rangetype_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_range_result_map_text(
    bcm_switch_range_result_map_t bcm_rangeresultmap)
{
    char *rangeresultmap_name;
    char *bcm_rangeresultmap_text[bcmSwitchRangeResultMapCount] = BCM_SWITCH_RANGE_RESULT_MAP_STR;

    if ((bcm_rangeresultmap < bcmSwitchRangeResultMapNone) || (bcm_rangeresultmap >= bcmSwitchRangeResultMapCount))
    {
        rangeresultmap_name = "Invalid RangeType";
    }
    else
    {
        rangeresultmap_name = bcm_rangeresultmap_text[bcm_rangeresultmap];
    }
    return rangeresultmap_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_range_operator_text(
    bcm_switch_range_operator_t bcm_rangeoperator)
{
    char *rangeoperator_name;
    char *bcm_rangeoperator_text[bcmSwitchRangeOperatorCount] = BCM_SWITCH_RANGE_OPERATOR_STR;

    if ((bcm_rangeoperator < bcmSwitchRangeOperatorNone) || (bcm_rangeoperator >= bcmSwitchRangeOperatorCount))
    {
        rangeoperator_name = "Invalid RangeOperator";
    }
    else
    {
        rangeoperator_name = bcm_rangeoperator_text[bcm_rangeoperator];
    }
    return rangeoperator_name;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.init(unit));
     /** Assumption all SW state init to 0, hence all action are set to valid=FALSE by default*/
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.init(unit));
     /** Assumption all SW state init to 0, hence all qualifier are set to valid=FALSE by default*/

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Read mapping of apptypes to contexts from XML.
* \param [in] unit - Device ID.
* \param [out] static_opcode_is_valid - Array with DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF elements indicating what
*                                       opcodes are found in the XML.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_map_apptype_to_context_parse_init(
    int unit,
    uint8 static_opcode_is_valid[DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF])
{
    void *catalogue = NULL;
    void *fwd2_app_type_node_iter = NULL;
    void *fwd2_app_type_node = NULL;
    void *app_type_node = NULL;
    char *image_name;
    char *opcodes_path = NULL;
    uint32 opcode_id_enum;
    uint32 opcode_id;
    void *context_iter;
    int context_index;
    uint8 is_standard_image;
    int opcode_index;
    int entered_loop;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    for (opcode_index = 0; opcode_index < DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF; opcode_index++)
    {
        static_opcode_is_valid[opcode_index] = FALSE;
    }

    SHR_ALLOC_SET_ZERO(opcodes_path, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "opcodes_path", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_get(unit, &image_name));
    sal_snprintf(opcodes_path, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "%s/%s_fwd_app_type.xml",
                 ((is_standard_image) ? ("auto_generated") : ("bare_metal/auto_generated/fwd_app_type")), image_name);

    catalogue = dbx_pre_compiled_fwd_app_types_top_get(unit, opcodes_path, "FWD_APP_TYPES", CONF_OPEN_PER_DEVICE);
    if (catalogue == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find FWD_APP_TYPES configuration xml file: %s\n", opcodes_path);
    }
    entered_loop = FALSE;
    RHDATA_ITERATOR(fwd2_app_type_node_iter, catalogue, "FWD2_APP_TYPES")
    {
        fwd2_app_type_node = fwd2_app_type_node_iter;
        if (entered_loop)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found two FWD2_APP_TYPES\n");
        }
        entered_loop = TRUE;
    }
    if (entered_loop == FALSE)
    {
        if (is_standard_image)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Did not find any FWD2_APP_TYPES\n");
        }
        else
        {
            
            SHR_EXIT();
        }
    }
    /** reading each opcode and updating the info */
    RHDATA_ITERATOR(app_type_node, fwd2_app_type_node, "AppType")
    {
        bcm_field_AppType_t apptype = bcmFieldAppTypeCount;
        char opcode_name_full_size[RHSTRING_MAX_SIZE] = { 0 };
        char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };
        int nof_contexts;

        RHDATA_GET_STR_STOP(app_type_node, "name", opcode_name_full_size);

        if (sal_strnlen(opcode_name_full_size, (int) sizeof(opcode_name_full_size)) >= sizeof(opcode_name))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Opcode \"%.*s\" has name with %d charachters, maximum %d.\n",
                         (int) sizeof(opcode_name_full_size), opcode_name_full_size,
                         sal_strnlen(opcode_name_full_size, (int) sizeof(opcode_name_full_size)),
                         (int) sizeof(opcode_name) - 1);
        }

        sal_strncpy_s(opcode_name, opcode_name_full_size, sizeof(opcode_name));

        /** Verify that the opcode name matches the ENUM value. */
        rv = dbal_enum_type_string_to_id_no_err(unit, DBAL_FIELD_TYPE_DEF_FORWARD_APP_TYPES, opcode_name,
                                                &opcode_id_enum);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Opcode \"%.*s\" is not found as an ENUM value.\n",
                         (int) sizeof(opcode_name), opcode_name);
        }
        SHR_IF_ERR_EXIT(rv);
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_FORWARD_APP_TYPES, opcode_id_enum, &opcode_id));

        if ((((int) opcode_id) < 0) || (opcode_id >= DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d (\"%.*s\") is out of range (0-%d).\n",
                         opcode_id, (int) sizeof(opcode_name), opcode_name,
                         DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF - 1);
        }

        /*
         * Check if the context has an apptype associated with it.
         * For bare metal, apptype==opcode.
         */
        if (is_standard_image)
        {
            int apptype_index;
            uint32 mapped_opcode;
            for (apptype_index = 0; apptype_index < bcmFieldAppTypeCount; apptype_index++)
            {
                rv = dnx_field_map_standard_1_predefined_apptype_to_opcode_get(unit, apptype_index, &mapped_opcode);
                if (rv == _SHR_E_NOT_FOUND)
                {
                    continue;
                }
                SHR_IF_ERR_EXIT(rv);
                if (mapped_opcode == opcode_id)
                {
                    apptype = apptype_index;
                    break;
                }
            }
        }
        else
        {
            apptype = opcode_id;
        }
        if (apptype >= bcmFieldAppTypeCount)
        {
            /*
             * For now we tolerate unmapped apptypes and don't send an error.
             */
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Could not map opcode %d (\"%.*s\") to apptype.%s\n",
                         opcode_id, (int) sizeof(opcode_name), opcode_name, EMPTY);
            continue;
        }

        context_index = 0;
        RHDATA_ITERATOR(context_iter, app_type_node, "FwdContext")
        {
            uint32 fwd_context_enum_value;
            uint32 fwd_context_uint32;
            char ctx_name_full_size[RHSTRING_MAX_SIZE] = { 0 };
            char ctx_name[DBAL_MAX_STRING_LENGTH] = { 0 };
            uint8 context_is_invalid = 0;

            RHDATA_GET_STR_STOP(context_iter, "name", ctx_name_full_size);

            if (sal_strnlen(opcode_name_full_size, (int) sizeof(ctx_name_full_size)) >= sizeof(ctx_name))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Context \"%.*s\" has name with %d charachters, maximum %d. "
                             "Mapped to opcode %d (\"%.*s\").\n",
                             (int) sizeof(opcode_name_full_size), opcode_name_full_size,
                             sal_strnlen(opcode_name_full_size, sizeof(ctx_name_full_size)),
                             (int) sizeof(ctx_name) - 1, opcode_id, (int) sizeof(opcode_name), opcode_name);
            }

            sal_strncpy_s(ctx_name, ctx_name_full_size, (int) sizeof(ctx_name));

            if (context_index >= DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Opcode %d (\"%.*s\") is mapped to more than %d contexts.\n",
                             opcode_id, (int) sizeof(opcode_name), opcode_name,
                             DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
            }

            /*
             * Get the context ID from the ENUM HW value.
             * Note that for predefined contexts, FWD and ACL context IDs are the same.
             * Verify the the context ID fits in uint8.
             */
            /** Validate the enum value is valid for the device */
            SHR_IF_ERR_EXIT(dbal_enum_type_string_to_id
                            (unit, DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID, ctx_name, &fwd_context_enum_value));

            SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_context_enum_value,
                                                         &context_is_invalid));
            if (context_is_invalid)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                            (unit, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_context_enum_value, &fwd_context_uint32));
            if (((uint8) fwd_context_uint32) != fwd_context_uint32)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "context %d (\"%.*s\") doesn't fit inside one byte.\n",
                             fwd_context_uint32, (int) sizeof(ctx_name), ctx_name);
            }

            /*
             * Save the context to SW state.
             */
            if (sw_state_is_warm_boot(unit))
            {
                /*
                 * If warmboot do not save to SW state, but verify SW state.
                 */
                uint8 fwd_context_uint32_get;
                SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.
                                fwd2_context_ids.get(unit, apptype, context_index, &fwd_context_uint32_get));
                if (fwd_context_uint32_get != fwd_context_uint32)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "Mismatch XML and SW state. For Apptype %d, context number %d is %d "
                                 "in XML and %d in SW state.\n", apptype, context_index, fwd_context_uint32,
                                 fwd_context_uint32_get);
                }
            }
            else
            {
                /*
                 * If not warmboot save to SW state
                 */
                SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.
                                fwd2_context_ids.set(unit, apptype, context_index, fwd_context_uint32));
            }

            context_index++;
        }
        nof_contexts = context_index;
        /*
         * Save the number of contexts per apptype to SW state. If 0 indicates that the apptype is not in use.
         */
        if (sw_state_is_warm_boot(unit))
        {
            /*
             * If warmboot do not save to SW state, but verify SW state.
             */
            uint8 nof_context_get;
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_fwd2_context_ids.get(unit, apptype, &nof_context_get));
            if (nof_context_get != nof_contexts)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Mismatch XML and SW state. For Apptype %d, %d context in "
                             "XML and %d in SW state.\n", apptype, nof_contexts, nof_context_get);
            }
        }
        else
        {
            /*
             * If not warmboot save to SW state
             */
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_fwd2_context_ids.set(unit, apptype, nof_contexts));
        }

        static_opcode_is_valid[opcode_id] = TRUE;
    }

exit:
    SHR_FREE(opcodes_path);
    dbx_xml_top_close(catalogue);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init the mapping and information of apptypes and opcodes.
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_map_kbp_apptype_opcode_init(
    int unit)
{
    uint32 i_opcode;
    uint32 mapped_opcode;
    bcm_field_AppType_t i_apptype;
    int nof_static_apptypes_standard_1;
    int nof_static_opcodes;
    int nof_static_apptypes;
    int cs_profile_id_counter;
    int i_user_defined_opcode;
    int first_user_defined_apptype = dnx_data_field.external_tcam.apptype_user_1st_get(unit);
    int nof_user_defined_apptypes = dnx_data_field.external_tcam.apptype_user_nof_get(unit);
    uint8 is_standard_image;
    uint8 static_opcode_is_valid[DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF];
    char *image_name;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_get(unit, &image_name));

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));

    nof_static_opcodes = sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0]);
    nof_static_apptypes_standard_1 = bcmFieldAppTypeCount;

    if (nof_static_opcodes != DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Unexpected number of elements in dnx_field_static_opcode_info %d. Expected %d.\n",
                     nof_static_opcodes, DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);
    }
    {
        uint32 apptype_enum_iter;
        SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                        (unit, DBAL_FIELD_FORWARD_APP_TYPES, INVALID_ENUM, &apptype_enum_iter));
        while (apptype_enum_iter != INVALID_ENUM)
        {
            if (apptype_enum_iter >= nof_static_opcodes)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The number of static opcodes (%d) is too small for opcode enum value %d.\n",
                             nof_static_opcodes, apptype_enum_iter);
            }
            SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                            (unit, DBAL_FIELD_FORWARD_APP_TYPES, apptype_enum_iter, &apptype_enum_iter));
        }
    }
    if (nof_static_apptypes_standard_1 != bcmFieldAppTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Unexpected number of elements in dnx_field_static_apptype_map %d. Expected %d.\n",
                     nof_static_apptypes_standard_1, bcmFieldAppTypeCount);
    }
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
    if (nof_static_opcodes > DNX_KBP_MAX_NOF_OPCODES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of elements in dnx_field_static_opcode_info is %d, "
                     "Exceeds maximum number of KBP opcodes %d.\n", nof_static_opcodes, DNX_KBP_MAX_NOF_OPCODES);
    }
#endif
    if (nof_static_apptypes_standard_1 >= first_user_defined_apptype)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of predefined Apptypes (%d) reached the user defined range, starting from %d.\n",
                     nof_static_apptypes_standard_1, first_user_defined_apptype);
    }
    if (DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF >= first_user_defined_apptype)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of predefined opcodes (%d), which in bare metal serve as predefiend "
                     "Apptypes as well, reached the user defined range, starting from %d.\n",
                     DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF, first_user_defined_apptype);
    }

    if (DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID isn't zero, leading to imporoper ititialization of "
                     "dnx_field_static_apptype_map.\n");
    }

    if (is_standard_image)
    {
        nof_static_apptypes = nof_static_apptypes_standard_1;
    }
    else
    {
        nof_static_apptypes = nof_static_opcodes;
    }

    /*
     * Initialize the OPCODE to apptype mapping.
     */
    for (i_opcode = 0; i_opcode < nof_static_opcodes; i_opcode++)
    {
        dnx_field_static_opcode_info[i_opcode].app_type = DNX_BCM_ID_INVALID;
        /** The following two lines should have no effect, as the array is initialized.*/
        dnx_field_static_opcode_info[i_opcode].is_valid = FALSE;
    }

    /*
     * Read the mapping of apptypes to contexts from mapping.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_context_parse_init(unit, static_opcode_is_valid));

    if (static_opcode_is_valid[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID])
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "opcode %d was found, illegal.\r\n", DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID);
    }

    /*
     * Go over all static apptypes and add their mapping to static opcodes.
     */
    for (i_apptype = 0; i_apptype < nof_static_apptypes; i_apptype++)
    {
        uint8 nof_contexts;
        shr_error_e rv;

        if (is_standard_image)
        {
            rv = dnx_field_map_standard_1_predefined_apptype_to_opcode_get(unit, i_apptype, &mapped_opcode);
            if (rv == _SHR_E_NOT_FOUND)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(rv);
        }
        else
        {
            /*
             * For bare metal static opcodes are the same as apptypes
             */
            mapped_opcode = i_apptype;
        }

        if ((int) mapped_opcode < 0 || mapped_opcode >= nof_static_opcodes)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d mapped to opcode %d. Maximum static opcode number is %d\n",
                         i_apptype, mapped_opcode, (nof_static_opcodes - 1));
        }

        if (static_opcode_is_valid[mapped_opcode] == FALSE)
        {
            if (is_standard_image)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Apptype %d is mapped to opcode_id %d, but does not appear in XML.%s%s\r\n",
                             i_apptype, mapped_opcode, EMPTY, EMPTY);
            }
            continue;
        }

        /** Sanity check.*/
        if (mapped_opcode == DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d mapped to opcode %d. Illegal value.\n", i_apptype, mapped_opcode);
        }

        if (mapped_opcode >= DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Apptype %d mapped to opcode %d with HW value %d, above dynamic range (%d).\n",
                         i_apptype, mapped_opcode, mapped_opcode, DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF);
        }

        dnx_field_static_opcode_info[mapped_opcode].app_type = i_apptype;
        dnx_field_static_opcode_info[mapped_opcode].is_valid = TRUE;

        /*
         * If the opcode is not used by any FWD context on this device, it will later be ignored.
         */
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_fwd2_context_ids.get(unit, i_apptype, &nof_contexts));
        if (nof_contexts <= 0)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "opcode_id %d has no contexts. Apptype %d.%s%s\r\n",
                         mapped_opcode, i_apptype, EMPTY, EMPTY);
            continue;
        }
    }

    /*
     * Allocated profile IDs. We allocate profiles for static apptypes with WITH_ID flag so that it will have the
     * same mapping for all units in a multi unit system.
     * We do not allocate profile 0, the default profile.
     */
    cs_profile_id_counter = 1;
    for (i_opcode = 0; i_opcode < nof_static_opcodes; i_opcode++)
    {
        if (dnx_field_static_opcode_info[i_opcode].is_valid)
        {
            if (!sw_state_is_warm_boot(unit))
            {
                uint8 nof_contexts;
                SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.
                                nof_fwd2_context_ids.get(unit, dnx_field_static_opcode_info[i_opcode].app_type,
                                                         &nof_contexts));
                if (nof_contexts > 0)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_external_tcam_opcode_cs_profile_id_allocate
                                    (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &cs_profile_id_counter));
                }
            }
            dnx_field_static_opcode_info[i_opcode].cs_profile_id = cs_profile_id_counter;
            cs_profile_id_counter++;
        }
    }

    /*
     * Now check if we have enough profiles for the user define Apptypes
     */
    if (!sw_state_is_warm_boot(unit))
    {
        int test_profile_id;
        for (i_user_defined_opcode = 0; i_user_defined_opcode < nof_user_defined_apptypes; i_user_defined_opcode++)
        {
            test_profile_id = cs_profile_id_counter + i_user_defined_opcode;
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_field_external_tcam_opcode_cs_profile_id_allocate
                                     (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &test_profile_id),
                                     "Only has room for %d user defined Apptypes, %d is required.%s\n",
                                     i_user_defined_opcode, nof_user_defined_apptypes, EMPTY);
        }
        for (i_user_defined_opcode = 0; i_user_defined_opcode < nof_user_defined_apptypes; i_user_defined_opcode++)
        {
            test_profile_id = cs_profile_id_counter + i_user_defined_opcode;
            SHR_IF_ERR_EXIT(dnx_algo_field_external_tcam_opcode_cs_profile_id_deallocate(unit, test_profile_id));
        }
    }

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
    /*
     * Check that the apptypes, opcodes and contexts are aligned with KBP.
     */
    if (is_standard_image && (dnx_data_field.external_tcam.type_get(unit) == DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP) &&
        (!sw_state_is_warm_boot(unit)))
    {
        int kbp_found;
        SHR_IF_ERR_EXIT(dnx_field_map_is_external_tcam_available(unit, &kbp_found));
        if (kbp_found)
        {
            for (i_apptype = 0; i_apptype < nof_static_apptypes; i_apptype++)
            {
                shr_error_e rv;
                int mapped_nof_contexts;
                dnx_field_context_t
                    mapped_fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];

                sal_memset(mapped_fwd_contexts, 0x0, sizeof(mapped_fwd_contexts));

                rv = dnx_field_map_apptype_to_opcode_bcm_to_dnx_int(unit, i_apptype, &mapped_opcode);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
                if (rv != _SHR_E_NOT_FOUND)
                {
                    int fwd_nof_contexts;
                    int fwd_context_idx;
                    dnx_field_context_t
                        fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
                    dnx_field_context_t
                        acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
                    /** Check that the KBP opcode to contexts mapping agrees with the field mapping.*/
                    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_lookup_fwd_contexts
                                    (unit, i_apptype, &mapped_nof_contexts, mapped_fwd_contexts));
                    if ((!sw_state_is_warm_boot(unit)) && kbp_found)
                    {

                        SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_to_contexts
                                        (unit, mapped_opcode, &fwd_nof_contexts, fwd_contexts, acl_contexts));
                        if ((fwd_nof_contexts != 0) && (mapped_nof_contexts != fwd_nof_contexts))
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "Apptype %d mapped alternatively to %d and %d FWD contexts.\n", i_apptype,
                                         mapped_nof_contexts, fwd_nof_contexts);
                        }
                        if (fwd_nof_contexts > DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_ACL_CONTEXT_NUM)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d mapped to %d FWD contexts, maximum is %d.\n",
                                         i_apptype, fwd_nof_contexts,
                                         DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_ACL_CONTEXT_NUM);
                        }
                        for (fwd_context_idx = 0; fwd_context_idx < fwd_nof_contexts; fwd_context_idx++)
                        {
                            if (mapped_fwd_contexts[fwd_context_idx] != fwd_contexts[fwd_context_idx])
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                             "Apptype %d opcode %d, FWD context number %d is alternatively to %d and %d.\n",
                                             i_apptype, mapped_opcode, fwd_nof_contexts,
                                             mapped_fwd_contexts[fwd_context_idx], fwd_contexts[fwd_context_idx]);
                            }
                        }
                    }
                }
            }
        }
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verifies that the external TCAM defined are aligned with the external TCAM defines.
* \param [in] unit - Device ID.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_map_external_defines_verify(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF != DNX_KBP_MAX_STATIC_OPCODES_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Number of predefined apptypes for external TCAM %d, is not the same as for KBP %d.\n.",
                     DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF, DNX_KBP_MAX_STATIC_OPCODES_NOF);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_map_external_defines_verify(unit));

    SHR_IF_ERR_EXIT(dnx_field_map_kbp_apptype_opcode_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_vw_qual_action_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_qual_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_action_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_ffc_type_to_field(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_ffc_type_e ffc_type,
    dbal_fields_e * ffc_type_field_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_type_field_p, _SHR_E_PARAM, "ffc_type_field_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    *ffc_type_field_p = dnx_field_map_stage_info[stage].ffc_type_field_a[ffc_type];

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_to_ffc_table(
    int unit,
    dnx_field_stage_e stage,
    dbal_tables_e * ffc_table_name_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_table_name_p, _SHR_E_PARAM, "ffc_table_name_p");
    /*
     * TBD: Handle Initial ffc's
     */
    *ffc_table_name_p = dnx_field_map_stage_info[stage].ffc_table;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_to_ffc_instruction(
    int unit,
    dnx_field_stage_e stage,
    dbal_fields_e * ffc_instruction_name_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_instruction_name_p, _SHR_E_PARAM, "ffc_table_name_p");
    *ffc_instruction_name_p = dnx_field_map_stage_info[stage].ffc_instruction;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_bcm_to_dnx(
    int unit,
    bcm_field_stage_t bcm_stage,
    dnx_field_stage_e * dnx_stage_p)
{
    dnx_field_stage_e dnx_stage;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_stage_p, _SHR_E_PARAM, "dnx_stage_p");

    if ((bcm_stage < bcmFieldStageFirst) || (bcm_stage >= bcmFieldStageCount))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Stage:%d \n", bcm_stage);
    }

    DNX_FIELD_STAGE_ITERATOR(dnx_stage)
    {
        if (dnx_field_map_stage_info[dnx_stage].bcm_stage == bcm_stage)
        {
            *dnx_stage_p = dnx_stage;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "BCM Stage:\"%s\" is not supported\n", dnx_field_bcm_stage_text(bcm_stage));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_ctx_id_to_field(
    int unit,
    dnx_field_stage_e stage,
    dbal_fields_e * dbal_ctx_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dbal_ctx_id, _SHR_E_PARAM, "dbal_ctx_id");

    *dbal_ctx_id = dnx_field_map_stage_info[stage].ctx_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_key_to_field(
    int unit,
    dnx_field_stage_e stage,
    dbal_fields_e * dbal_key)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dbal_key, _SHR_E_PARAM, "dbal_key");

    *dbal_key = dnx_field_map_stage_info[stage].key_field;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_dnx_to_bcm(
    int unit,
    dnx_field_stage_e dnx_stage,
    bcm_field_stage_t * bcm_stage_p)
{
    bcm_field_stage_t bcm_stage;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bcm_stage_p, _SHR_E_PARAM, "bcm_stage_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    bcm_stage = dnx_field_map_stage_info[dnx_stage].bcm_stage;

    if ((bcm_stage < bcmFieldStageFirst) || (bcm_stage >= bcmFieldStageCount))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "DNX Stage:\"%s\" has no BCM mapping\n", dnx_field_stage_text(unit, dnx_stage));
    }
    *bcm_stage_p = bcm_stage;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_group_type_bcm_to_dnx(
    int unit,
    bcm_field_group_type_t bcm_fg_type,
    dnx_field_group_type_e * dnx_fg_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_fg_type_p, _SHR_E_PARAM, "dnx_fg_type_p");

    switch (bcm_fg_type)
    {
        case bcmFieldGroupTypeTcam:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            break;
        }
        case bcmFieldGroupTypeDirectTcam:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM;
            break;
        }
        case bcmFieldGroupTypeDirectExtraction:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
            break;
        }
        case bcmFieldGroupTypeExactMatch:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_EXEM;
            break;
        }

        case bcmFieldGroupTypeStateTable:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_STATE_TABLE;
            break;
        }
        case bcmFieldGroupTypeDirectMdb:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB;
            break;
        }
        case bcmFieldGroupTypeExternalTcam:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM;
            break;
        }
        case bcmFieldGroupTypeConst:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_CONST;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Field Group Type:%d \n", bcm_fg_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_group_type_dnx_to_bcm(
    int unit,
    dnx_field_group_type_e dnx_fg_type,
    bcm_field_group_type_t * bcm_fg_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_fg_type_p, _SHR_E_PARAM, "bcm_fg_type_p");

    switch (dnx_fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeTcam;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeDirectTcam;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeDirectExtraction;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeExactMatch;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeStateTable;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeDirectMdb;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeExternalTcam;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeConst;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Field Group Type:%d \n", dnx_fg_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_layer_type_bcm_to_dnx(
    int unit,
    bcm_field_layer_type_t bcm_layer_type,
    dbal_enum_value_field_layer_types_e * dnx_layer_type,
    dbal_enum_value_field_parsing_start_type_encoding_e * parsing_start_type)
{
    static char *layer_type_names[bcmFieldLayerTypeCount] = BCM_FIELD_LAYER_TYPE_STRINGS;
    dbal_enum_value_field_layer_types_e mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION;
    dbal_enum_value_field_parsing_start_type_encoding_e mapped_parsing_start_type =
        DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_INITIALIZATION;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_layer_type == NULL && parsing_start_type == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "both out parameters are NULL.\n");
    }

    if (((int) bcm_layer_type < 0) || (bcm_layer_type >= bcmFieldLayerTypeCount))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Layer Type:%d is out of range\n", bcm_layer_type);
    }
    switch (bcm_layer_type)
    {
        case bcmFieldLayerTypeEth:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ETHERNET;
            break;
        }
        case bcmFieldLayerTypeIp4:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IPV4;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV4;
            break;
        }
        case bcmFieldLayerTypeIp6:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IPV6;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV6;
            break;
        }
        case bcmFieldLayerTypeMpls:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_MPLS;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS;
            break;
        }
        case bcmFieldLayerTypeMplsIfit:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_IFIT;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeArp:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ARP;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ARP;
            break;
        }
        case bcmFieldLayerTypeFcoe:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_FCOE;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_FCOE;
            break;
        }
        case bcmFieldLayerTypeTcp:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_TCP;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeUdp:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_UDP;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeBfdSingleHop:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeBfdMultiHop:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeY1731:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeIcmp:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ICMP;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeBierTi:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_BIER_NON_MPLS;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_BIER_NON_MPLS;
            break;
        }
        case bcmFieldLayerTypeBierMpls:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IN_BIER_BASE_EG_BIER_MPLS;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IN_BIER_BASE_EG_BIER_MPLS;
            break;
        }
        case bcmFieldLayerTypeRch:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_RCH;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_RCH_EXTENDED_ENCAP;
            break;
        }
        case bcmFieldLayerTypeEthEnd:
        {
            /** No layer type mapping.*/
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_DUMMY_ETHERNET;
            break;
        }
        case bcmFieldLayerTypeMplsEnd:
        {
            /** No layer type mapping.*/
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS_DUMMY;
            break;
        }
        case bcmFieldLayerTypePppoe:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_PPP;
            break;
        }
        case bcmFieldLayerTypeLt2pv3:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_L2TP;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeSrv6Endpoint:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_ENDPOINT;
            break;
        }
        case bcmFieldLayerTypeSrv6EndpointPsp:
        {
            /** No layer type mapping.*/
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_ENDPOINT_PSP;
            break;
        }
        case bcmFieldLayerTypeSrv6Beyond:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_BEYOND;
            break;
        }
        case bcmFieldLayerTypeSrv6UspExtPsp:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_RCH_USP_PSP_AND_PSP_EXT;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_RCH_USP_PSP_AND_PSP_EXT;
            break;
        }
        case bcmFieldLayerTypeIgmp:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IGMP;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeIpAny:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IPVX;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeTm:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ITMH;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ITMH_VAL;
            break;
        }
        case bcmFieldLayerTypeTmLegacy:
        {
            if (dnx_data_headers.
                general.feature_get(unit, dnx_data_headers_general_jericho_mode_sys_header_is_supported) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Layer Type:%d is not supported for this device\n", bcm_layer_type);
            }
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_J1_VAL;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeForwardingMPLS:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS_UNTERM;
            break;
        }
        case bcmFieldLayerTypeUnknown:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_UNKNOWN;
            break;
        }
        case bcmFieldLayerTypeSctp:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH;
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_INGRESS_SCTP_EGRESS_FTMH;
            break;
        }
        case bcmFieldLayerTypePtpGeneral:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_PTP;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeGtp:
        {
            mapped_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_GTP;
            /** No parsing start type mapping.*/
            break;
        }
        case bcmFieldLayerTypeMirrorOrSs:
        {
            /** No layer type mapping.*/
            mapped_parsing_start_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_FORCE_MIRROR_OR_SS;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Layer Type:\"%s\"(%d) is not supported.\n",
                         layer_type_names[bcm_layer_type], bcm_layer_type);
            break;
        }
    }

    if (dnx_layer_type != NULL)
    {
        if (mapped_layer_type == DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Layer Type:\"%s\"(%d) is not supported as layer type.\n",
                         layer_type_names[bcm_layer_type], bcm_layer_type);
        }
        (*dnx_layer_type) = mapped_layer_type;
    }

    if (parsing_start_type != NULL)
    {
        if (mapped_parsing_start_type == DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_INITIALIZATION)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Layer Type:\"%s\"(%d) is not supported as parsing start type.\n",
                         layer_type_names[bcm_layer_type], bcm_layer_type);
        }
        (*parsing_start_type) = mapped_parsing_start_type;
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_bank_allocation_mode_bcm_to_dnx(
    int unit,
    bcm_field_tcam_bank_allocation_mode_t bcm_bank_allocation_mode,
    dnx_field_tcam_bank_allocation_mode_e * dnx_bank_allocation_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (bcm_bank_allocation_mode)
    {
        case bcmFieldTcamBankAllocationModeAuto:
        {
            *dnx_bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO;
            break;
        }
        case bcmFieldTcamBankAllocationModeSelect:
        {
            *dnx_bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT;
            break;
        }
        case bcmFieldTcamBankAllocationModeSelectWithLocation:
        {
            *dnx_bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Bank Allocation Mode:%d \n", bcm_bank_allocation_mode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_bank_allocation_mode_dnx_to_bcm(
    int unit,
    dnx_field_tcam_bank_allocation_mode_e dnx_bank_allocation_mode,
    bcm_field_tcam_bank_allocation_mode_t * bcm_bank_allocation_mode_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_bank_allocation_mode_p, _SHR_E_PARAM, "bcm_bank_allocation_mode_p");

    switch (dnx_bank_allocation_mode)
    {
        case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO:
        {
            *bcm_bank_allocation_mode_p = bcmFieldTcamBankAllocationModeAuto;
            break;
        }
        case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT:
        {
            *bcm_bank_allocation_mode_p = bcmFieldTcamBankAllocationModeSelect;
            break;
        }
        case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION:
        {
            *bcm_bank_allocation_mode_p = bcmFieldTcamBankAllocationModeSelectWithLocation;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Bank Allocation Mode:%d \n", dnx_bank_allocation_mode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_cache_mode_bcm_to_dnx(
    int unit,
    bcm_field_group_cache_mode_t bcm_cache_mode,
    dnx_field_group_cache_mode_e * dnx_cache_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (bcm_cache_mode)
    {
        case bcmFieldGroupCacheModeStart:
        {
            *dnx_cache_mode = DNX_FIELD_GROUP_CACHE_MODE_START;
            break;
        }
        case bcmFieldGroupCacheModeInstall:
        {
            *dnx_cache_mode = DNX_FIELD_GROUP_CACHE_MODE_INSTALL;
            break;
        }
        case bcmFieldGroupCacheModeClear:
        {
            *dnx_cache_mode = DNX_FIELD_GROUP_CACHE_MODE_CLEAR;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Cache Mode Mode:%d \n", bcm_cache_mode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_context_compare_type_bcm_to_dnx(
    int unit,
    bcm_field_context_compare_type_t bcm_context_compare_type,
    dnx_field_context_compare_mode_e * dnx_context_compare_mode_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_context_compare_mode_p, _SHR_E_PARAM, "dnx_context_compare_mode_p");

    switch (bcm_context_compare_type)
    {
        case bcmFieldContextCompareTypeNone:
        {
            *dnx_context_compare_mode_p = DNX_FIELD_CONTEXT_COMPARE_MODE_NONE;
            break;
        }
        case bcmFieldContextCompareTypeSingle:
        {
            *dnx_context_compare_mode_p = DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE;
            break;
        }
        case bcmFieldContextCompareTypeDouble:
        {
            *dnx_context_compare_mode_p = DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Context Compare Type:%d \n", bcm_context_compare_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_context_compare_type_dnx_to_bcm(
    int unit,
    dnx_field_context_compare_mode_e dnx_context_compare_mode,
    bcm_field_context_compare_type_t * bcm_context_compare_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_context_compare_type_p, _SHR_E_PARAM, "bcm_context_compare_type_p");

    switch (dnx_context_compare_mode)
    {
        case DNX_FIELD_CONTEXT_COMPARE_MODE_NONE:
        {
            *bcm_context_compare_type_p = bcmFieldContextCompareTypeNone;
            break;
        }
        case DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE:
        {
            *bcm_context_compare_type_p = bcmFieldContextCompareTypeSingle;
            break;
        }
        case DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE:
        {
            *bcm_context_compare_type_p = bcmFieldContextCompareTypeDouble;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Context Compare Mode:%d \n", dnx_context_compare_mode);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_context_state_opcode_bcm_to_dnx(
    int unit,
    bcm_field_state_opcode_t bcm_state_opcode_type,
    dnx_field_context_state_table_opcode_e * dnx_state_opcode_mode_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_state_opcode_mode_p, _SHR_E_PARAM, "dnx_state_opcode_mode_p");

    switch (bcm_state_opcode_type)
    {
        case bcmFieldStateOpcodeQual:
        {
            *dnx_state_opcode_mode_p = DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_QUAL;
            break;
        }
        case bcmFieldStateOpcodeWrite:
        {
            *dnx_state_opcode_mode_p = DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_WRITE;
            break;
        }
        case bcmFieldStateOpcodeAnd:
        {
            *dnx_state_opcode_mode_p = DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_AND;
            break;
        }
        case bcmFieldStateOpcodeOr:
        {
            *dnx_state_opcode_mode_p = DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_OR;
            break;
        }
        case bcmFieldStateOpcodeXor:
        {
            *dnx_state_opcode_mode_p = DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_XOR;
            break;
        }
        case bcmFieldStateOpcodeIncrement:
        {
            *dnx_state_opcode_mode_p = DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_INCREMENT;
            break;
        }
        case bcmFieldStateOpcodeAdd:
        {
            *dnx_state_opcode_mode_p = DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_ADD;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM State opcode Type:%d \n", bcm_state_opcode_type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_context_state_opcode_dnx_to_bcm(
    int unit,
    dnx_field_context_state_table_opcode_e dnx_state_opcode_mode,
    bcm_field_state_opcode_t * bcm_state_opcode_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_state_opcode_type_p, _SHR_E_PARAM, "bcm_state_opcode_type_p");

    switch (dnx_state_opcode_mode)
    {
        case DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_QUAL:
        {
            *bcm_state_opcode_type_p = bcmFieldStateOpcodeQual;
            break;
        }
        case DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_WRITE:
        {
            *bcm_state_opcode_type_p = bcmFieldStateOpcodeWrite;
            break;
        }
        case DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_AND:
        {
            *bcm_state_opcode_type_p = bcmFieldStateOpcodeAnd;
            break;
        }
        case DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_OR:
        {
            *bcm_state_opcode_type_p = bcmFieldStateOpcodeOr;
            break;
        }
        case DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_XOR:
        {
            *bcm_state_opcode_type_p = bcmFieldStateOpcodeXor;
            break;
        }
        case DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_INCREMENT:
        {
            *bcm_state_opcode_type_p = bcmFieldStateOpcodeIncrement;
            break;
        }
        case DNX_FIELD_CONTEXT_STATE_TABLE_OPCODE_ADD:
        {
            *bcm_state_opcode_type_p = bcmFieldStateOpcodeAdd;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX State opcode Type:%d \n", dnx_state_opcode_mode);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_layer_type_dnx_to_bcm(
    int unit,
    dbal_enum_value_field_layer_types_e dnx_layer_type,
    bcm_field_layer_type_t * bcm_layer_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_layer_type_p, _SHR_E_PARAM, "bcm_layer_type_p");

    switch (dnx_layer_type)
    {
        case DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeEth;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV4:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIp4;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV6:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIp6;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeMpls;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_IFIT:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeMplsIfit;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ARP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeArp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_FCOE:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeFcoe;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_TCP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeTcp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_UDP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeUdp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBfdSingleHop;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBfdMultiHop;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeY1731;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ICMP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIcmp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BIER_NON_MPLS:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBierTi;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IN_BIER_BASE_EG_BIER_MPLS:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBierMpls;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_RCH:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeRch;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE:
        {
            *bcm_layer_type_p = bcmFieldLayerTypePppoe;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_L2TP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeLt2pv3;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeSrv6Endpoint;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeSrv6Beyond;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_RCH_USP_PSP_AND_PSP_EXT:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeSrv6UspExtPsp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IGMP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIgmp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPVX:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIpAny;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ITMH:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeTm;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_J1_VAL:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeTmLegacy;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeForwardingMPLS;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeUnknown;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeSctp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_PTP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypePtpGeneral;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_GTP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeGtp;
            break;
        }
        default:
        {
            /** None of the supported types. */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_vlan_format_dnx_to_bcm(
    int unit,
    dbal_enum_value_field_incoming_tag_structure_e dnx_vlan_format,
    uint32 *bcm_vlan_format_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_vlan_format_p, _SHR_E_PARAM, "bcm_vlan_format_p");

    switch (dnx_vlan_format)
    {
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_UNTAGGED_1:
        {
            *bcm_vlan_format_p = bcmTagStructTypeUntag;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_1:
        {
            *bcm_vlan_format_p = bcmTagStructTypeSTag;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_1:
        {
            *bcm_vlan_format_p = bcmTagStructTypeCTag;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_1:
        {
            *bcm_vlan_format_p = bcmTagStructTypeSCTag;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_1:
        {
            *bcm_vlan_format_p = bcmTagStructTypeSPrioCTag;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_S_TAG_1:
        {
            *bcm_vlan_format_p = bcmTagStructTypeSSTag;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_C_TAG_1:
        {
            *bcm_vlan_format_p = bcmTagStructTypeCCTag;
            break;
        }
        default:
        {
            /** None of the supported types. */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_lookup_dbal_table_get(
    int unit,
    dnx_field_stage_e dnx_stage,
    int *has_lookup_enabler,
    dbal_tables_e * table_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(table_id_p, _SHR_E_PARAM, "table_id_p");
    SHR_NULL_CHECK(has_lookup_enabler, _SHR_E_PARAM, "has_lookup_enabler");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    *table_id_p = DBAL_TABLE_EMPTY;
    *has_lookup_enabler = FALSE;

    switch (dnx_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
            *table_id_p = DBAL_TABLE_FIELD_IPMF1_LOOKUP_ENABLER;
            *has_lookup_enabler = TRUE;
            break;
        case DNX_FIELD_STAGE_IPMF2:
            *table_id_p = DBAL_TABLE_FIELD_IPMF2_LOOKUP_ENABLER;
            *has_lookup_enabler = TRUE;
            break;
        case DNX_FIELD_STAGE_IPMF3:
            *table_id_p = DBAL_TABLE_FIELD_IPMF3_LOOKUP_ENABLER;
            *has_lookup_enabler = TRUE;
            break;
        case DNX_FIELD_STAGE_EPMF:
            if (dnx_data_field.features.epmf_has_lookup_enabler_get(unit))
            {
                *has_lookup_enabler = TRUE;

            }
            else
            {
                *table_id_p = DBAL_TABLE_EMPTY;
                *has_lookup_enabler = FALSE;
            }
            break;
        default:
            *table_id_p = DBAL_TABLE_EMPTY;
            *has_lookup_enabler = FALSE;
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_compare_id_bcm_to_dnx(
    int unit,
    int compare_id,
    dnx_field_group_compare_id_e * dnx_compare_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_compare_id_p, _SHR_E_PARAM, "dnx_compare_id_p");

    switch (compare_id)
    {
        case BCM_FIELD_INVALID:
        {
            *dnx_compare_id_p = DNX_FIELD_GROUP_COMPARE_ID_NONE;
            break;
        }
        case 0:
        {
            *dnx_compare_id_p = DNX_FIELD_GROUP_COMPARE_ID_CMP_1;
            break;
        }
        case 1:
        {
            *dnx_compare_id_p = DNX_FIELD_GROUP_COMPARE_ID_CMP_2;
            break;
        }
        default:
        {
            /*
             * None of the supported compare Ids
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid compare_id:%d \n", compare_id);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_context_param_bcm_to_dnx(
    int unit,
    bcm_field_stage_t bcm_stage,
    dnx_field_context_t context_id,
    bcm_field_context_param_info_t * bcm_context_param_set_p,
    dnx_field_dbal_entry_t * field_dbal_entry_p)
{
    const dnx_field_context_param_t *dnx_context_param_set_p;
    dnx_field_stage_e dnx_stage;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_context_param_set_p, _SHR_E_PARAM, "bcm_context_param_set_p");
    SHR_NULL_CHECK(field_dbal_entry_p, _SHR_E_PARAM, "field_dbal_entry_p");

    /** Convert BCM to DNX Field Stage. And verify it on way*/
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));

    dnx_context_param_set_p = &context_param_set[bcm_context_param_set_p->param_type][bcm_stage];

    if (dnx_context_param_set_p == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context Parameter Type %d for stage:%s is not supported",
                     bcm_context_param_set_p->param_type, dnx_field_bcm_stage_text(bcm_stage));
    }

    if (dnx_context_param_set_p->table_id == DBAL_TABLE_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context Parameter Type %d for stage:%s is not supported",
                     bcm_context_param_set_p->param_type, dnx_field_bcm_stage_text(bcm_stage));
    }
    /*
     * First of all initialize to NULL
     */
    sal_memset(field_dbal_entry_p, 0, sizeof(*field_dbal_entry_p));
    /*
     * Fetch table id
     */
    field_dbal_entry_p->table_id = dnx_context_param_set_p->table_id;
    if (dnx_context_param_set_p->conversion_cb != NULL)
    {
        /*
         * Conversion callback will take care of filling dbal fields and their values
         */
        SHR_IF_ERR_EXIT(dnx_context_param_set_p->conversion_cb(unit, dnx_stage, context_id,
                                                               bcm_context_param_set_p->param_val,
                                                               dnx_context_param_set_p, field_dbal_entry_p));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No Data Conversion CB for Context Parameter:\"%d\" stage:\"%s\"\n",
                     bcm_context_param_set_p->param_type, dnx_field_bcm_stage_text(bcm_stage));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_func_bcm_to_dbal(
    int unit,
    bcm_field_context_hash_function_t bcm_hash_func,
    dbal_enum_value_field_context_hash_function_e * dbal_hash_func_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dbal_hash_func_p, _SHR_E_PARAM, "dbal_hash_func_p");

    switch (bcm_hash_func)
    {
        case bcmFieldContextHashFunctionFirstReserved:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_FIRST_RESERVED;
            break;
        }
        case bcmFieldContextHashFunctionSecondReserved:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_SECOND_RESERVED;
            break;
        }
        case bcmFieldContextHashFunctionThirdReserved:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_THIRD_RESERVED;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Bisync:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor1:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR1;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor2:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR2;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor4:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR4;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor8:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR8;
            break;
        }
        case bcmFieldContextHashFunctionXor16:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_XOR16;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Ccitt:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_CCITT;
            break;
        }
        case bcmFieldContextHashFunctionCrc32ALow:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_ALOW;
            break;
        }
        case bcmFieldContextHashFunctionCrc32Ahigh:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_AHIGH;
            break;
        }
        case bcmFieldContextHashFunctionCrc32BLow:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BLOW;
            break;
        }
        case bcmFieldContextHashFunctionCrc32BHigh:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BHIGH;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Hash Function type: %d.\n", bcm_hash_func);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_func_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_context_hash_function_e dbal_hash_func,
    bcm_field_context_hash_function_t * bcm_hash_func_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_hash_func_p, _SHR_E_PARAM, "bcm_hash_func_p");
    switch (dbal_hash_func)
    {
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_FIRST_RESERVED:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionFirstReserved;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_SECOND_RESERVED:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionSecondReserved;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_THIRD_RESERVED:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionThirdReserved;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Bisync;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR1:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor1;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR2:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor2;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR4:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor4;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR8:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor8;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_XOR16:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionXor16;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_CCITT:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Ccitt;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_ALOW:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32ALow;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_AHIGH:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32Ahigh;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BLOW:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32BLow;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BHIGH:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32BHigh;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Function type: %d. \n", dbal_hash_func);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_lb_key_bcm_to_dbal(
    int unit,
    bcm_field_context_hash_action_key_t bcm_hash_lb_key,
    dbal_enum_value_field_field_hash_lb_key_e * dbal_hash_lb_key_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_hash_lb_key_p, _SHR_E_PARAM, "dbal_hash_lb_key_p");
    switch (bcm_hash_lb_key)
    {
        case bcmFieldContextHashActionKeyEcmpLbKey0:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP0_LB;
            break;
        }
        case bcmFieldContextHashActionKeyEcmpLbKey1:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP1_LB;
            break;
        }
        case bcmFieldContextHashActionKeyEcmpLbKey2:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP2_LB;
            break;
        }
        case bcmFieldContextHashActionKeyNetworkLbKey:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_NWK_LB;
            break;
        }
        case bcmFieldContextHashActionKeyLagLbKey:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_LAG_LB;
            break;
        }
        case bcmFieldContextHashActionKeyAdditionalLbKey:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ADDITIONAL_LB;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hashing Load Balancing Key type: %d ."
                         " Valid values from: %d to %d . \n",
                         bcm_hash_lb_key, bcmFieldContextHashActionKeyFirst, (bcmFieldContextHashActionKeyCount - 1));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_lb_key_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_field_hash_lb_key_e dbal_hash_lb_key,
    bcm_field_context_hash_action_key_t * bcm_hash_lb_key_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_hash_lb_key_p, _SHR_E_PARAM, "bcm_hash_lb_key_p");
    switch (dbal_hash_lb_key)
    {
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP0_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyEcmpLbKey0;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP1_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyEcmpLbKey1;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP2_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyEcmpLbKey2;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_NWK_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyNetworkLbKey;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_LAG_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyLagLbKey;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ADDITIONAL_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyAdditionalLbKey;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hashing Load Balancing Key type: %d. \n", dbal_hash_lb_key);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_action_bcm_to_dbal(
    int unit,
    bcm_field_context_hash_action_value_t bcm_hash_action,
    dbal_enum_value_field_hash_action_e * dbal_hash_action_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_hash_action_p, _SHR_E_PARAM, "dbal_hash_action_p");
    switch (bcm_hash_action)
    {
        case bcmFieldContextHashActionValueNone:
        {
            *dbal_hash_action_p = DBAL_ENUM_FVAL_HASH_ACTION_NONE;
            break;
        }
        case bcmFieldContextHashActionValueReplaceCrc:
        {
            *dbal_hash_action_p = DBAL_ENUM_FVAL_HASH_ACTION_REPLACE_CRC;
            break;
        }
        case bcmFieldContextHashActionValueAugmentCrc:
        {
            *dbal_hash_action_p = DBAL_ENUM_FVAL_HASH_ACTION_AUGMENT_CRC;
            break;
        }
        case bcmFieldContextHashActionValueAugmentKey:
        {
            *dbal_hash_action_p = DBAL_ENUM_FVAL_HASH_ACTION_AUGMENT_KEY;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Action type: %d. Valid values from: %d to %d . \n",
                         bcm_hash_action,
                         bcmFieldContextHashActionValueFirst, (bcmFieldContextHashActionValueCount - 1));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_action_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_hash_action_e dbal_hash_action,
    bcm_field_context_hash_action_value_t * bcm_hash_action_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_hash_action_p, _SHR_E_PARAM, "bcm_hash_action_p");
    switch (dbal_hash_action)
    {
        case DBAL_ENUM_FVAL_HASH_ACTION_NONE:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueNone;
            break;
        }
        case DBAL_ENUM_FVAL_HASH_ACTION_REPLACE_CRC:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueReplaceCrc;
            break;
        }
        case DBAL_ENUM_FVAL_HASH_ACTION_AUGMENT_CRC:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueAugmentCrc;
            break;
        }
        case DBAL_ENUM_FVAL_HASH_ACTION_AUGMENT_KEY:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueAugmentKey;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Action type: %d. \n", dbal_hash_action);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_crc_select_bcm_to_dbal(
    int unit,
    bcm_field_crc_select_t bcm_crc_select,
    dbal_enum_value_field_crc_select_e * dbal_crc_select_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_crc_select_p, _SHR_E_PARAM, "dbal_crc_select_p");
    switch (bcm_crc_select)
    {
        case bcmFieldCrcSelectCrc16P0x10039:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10039;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x100d7:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X100D7;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x1015d:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X1015D;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x10939:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10939;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x109e7:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X109E7;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x10ac5:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10AC5;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x1203d:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X1203D;
            break;
        }
        case bcmFieldCrcSelectCrc16P0x12105:
        {
            *dbal_crc_select_p = DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0x12105;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid CRC Select type: %d. Valid values from: %d to %d . \n",
                         bcm_crc_select, bcmFieldCrcSelectFirst, (bcmFieldCrcSelectCount - 1));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_crc_select_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_crc_select_e dbal_crc_select,
    bcm_field_crc_select_t * bcm_crc_select_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_crc_select_p, _SHR_E_PARAM, "bcm_crc_select_p");
    switch (dbal_crc_select)
    {
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10039:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x10039;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X100D7:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x100d7;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X1015D:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x1015d;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10939:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x10939;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X109E7:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x109e7;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X10AC5:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x10ac5;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0X1203D:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x1203d;
            break;
        }
        case DBAL_ENUM_FVAL_CRC_SELECT_CRC16_0x12105:
        {
            *bcm_crc_select_p = bcmFieldCrcSelectCrc16P0x12105;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid CRC Select type: %d. \n", dbal_crc_select);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_range_type_info_legacy(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_range_type_e dnx_range_type,
    dnx_field_map_range_type_info_t * range_type_info_p)
{
    const dnx_field_range_map_t *range_map_legacy_p;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(range_type_info_p, _SHR_E_PARAM, "range_type_info_p");

    range_map_legacy_p = &range_map_legacy[dnx_range_type][field_stage];

    if (range_map_legacy_p == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range Type %d for stage:%s is not supported",
                     dnx_range_type, dnx_field_stage_text(unit, field_stage));
    }

    if (range_map_legacy_p->table_id == DBAL_TABLE_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range Type %d for stage:%s is not supported",
                     dnx_range_type, dnx_field_stage_text(unit, field_stage));
    }
    /**
     * First initialize range_type_info_p
     */
    sal_memset(range_type_info_p, 0, sizeof(*range_type_info_p));
    /**
     * Fetch table id
     */
    range_type_info_p->table_id = range_map_legacy_p->table_id;
    /**
     *  Fill the dbal fields.
     */
    range_type_info_p->range_id_dbal_field = range_map_legacy_p->range_id_dbal_field;
    range_type_info_p->range_min_dbal_field = range_map_legacy_p->range_min_dbal_field;
    range_type_info_p->range_max_dbal_field = range_map_legacy_p->range_max_dbal_field;

    /*
     * Indicate whether the HW returns the range ID or a bitmap of range IDs.
     * Used to determine the default value for the range.
     */
    range_type_info_p->id_not_bitmap = range_map_legacy_p->id_not_bitmap;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_range_type_bcm_to_dnx_int(
    int unit,
    bcm_field_range_type_t bcm_range_type,
    dnx_field_range_type_e * dnx_range_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_range_type_p, _SHR_E_PARAM, "dnx_range_type_p");

    switch (bcm_range_type)
    {
        case bcmFieldRangeTypeL4SrcPort:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_L4_SRC_PORT;
            break;
        }
        case bcmFieldRangeTypeL4DstPort:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_L4_DST_PORT;
            break;
        }
        case bcmFieldRangeTypeOutVport:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_OUT_LIF;
            break;
        }
        case bcmFieldRangeTypePacketHeaderSize:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE;
            break;
        }
        case bcmFieldRangeTypeL4OpsPacketHeaderSize:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE;
            break;
        }
        case bcmFieldRangeTypeInTTL:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_IN_TTL;
            break;
        }
        case bcmFieldRangeTypeUserDefined1Low:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_FFC1_LOW;
            break;
        }
        case bcmFieldRangeTypeUserDefined1High:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_FFC1_HIGH;
            break;
        }
        case bcmFieldRangeTypeUserDefined2Low:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_FFC2_LOW;
            break;
        }
        case bcmFieldRangeTypeUserDefined2High:
        {
            (*dnx_range_type_p) = DNX_FIELD_RANGE_TYPE_FFC2_HIGH;
            break;
        }
        default:
        {
            /**
             * None of the supported types
             */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_range_type_bcm_to_dnx(
    int unit,
    bcm_field_range_type_t bcm_range_type,
    dnx_field_range_type_e * dnx_range_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_range_type_p, _SHR_E_PARAM, "dnx_range_type_p");

    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_range_type_bcm_to_dnx_int(unit, bcm_range_type, dnx_range_type_p),
                             "Invalid Range Type:%d.%s%s\n", bcm_range_type, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_range_type_dnx_to_bcm(
    int unit,
    dnx_field_range_type_e dnx_range_type,
    bcm_field_range_type_t * bcm_range_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bcm_range_type_p, _SHR_E_PARAM, "bcm_range_type_p");

    switch (dnx_range_type)
    {
        case DNX_FIELD_RANGE_TYPE_L4_SRC_PORT:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeL4SrcPort;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_L4_DST_PORT:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeL4DstPort;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_OUT_LIF:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeOutVport;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypePacketHeaderSize;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeL4OpsPacketHeaderSize;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_IN_TTL:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeInTTL;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_FFC1_LOW:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeUserDefined1Low;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_FFC1_HIGH:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeUserDefined1High;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_FFC2_LOW:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeUserDefined2Low;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_FFC2_HIGH:
        {
            (*bcm_range_type_p) = bcmFieldRangeTypeUserDefined2High;
            break;
        }
        default:
        {
            /**
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Range Type:%d \n", dnx_range_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_system_header_profile_bcm_to_dnx(
    int unit,
    bcm_field_system_header_profile_t bcm_sys_hdr_profile,
    dnx_field_context_sys_hdr_profile_e * dnx_sys_hdr_profile_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_sys_hdr_profile_p, _SHR_E_PARAM, "dnx_sys_hdr_profile_p");

    switch (bcm_sys_hdr_profile)
    {
        case bcmFieldSystemHeaderProfileNone:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_EMPTY;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmh:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmhPph:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_PPH;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmhTsh:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmhTshPph:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmhTshPphUdh:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH_UDH;
            break;
        }
        case bcmFieldSystemHeaderProfileFtmhUdh:
        {
            *dnx_sys_hdr_profile_p = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_UDH;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM System Header Profile:\"%d\" is not supported\n", bcm_sys_hdr_profile);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_system_header_profile_dnx_to_bcm(
    int unit,
    dnx_field_context_sys_hdr_profile_e dnx_sys_hdr_profile,
    bcm_field_system_header_profile_t * bcm_sys_hdr_profile_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_sys_hdr_profile_p, _SHR_E_PARAM, "bcm_sys_hdr_profile_p");

    switch (dnx_sys_hdr_profile)
    {
        case DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_EMPTY:
        {
            *bcm_sys_hdr_profile_p = bcmFieldSystemHeaderProfileNone;
            break;
        }
        case DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH:
        {
            *bcm_sys_hdr_profile_p = bcmFieldSystemHeaderProfileFtmh;
            break;
        }
        case DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_PPH:
        {
            *bcm_sys_hdr_profile_p = bcmFieldSystemHeaderProfileFtmhPph;
            break;
        }
        case DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH:
        {
            *bcm_sys_hdr_profile_p = bcmFieldSystemHeaderProfileFtmhTsh;
            break;
        }
        case DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH:
        {
            *bcm_sys_hdr_profile_p = bcmFieldSystemHeaderProfileFtmhTshPph;
            break;
        }
        case DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH_UDH:
        {
            *bcm_sys_hdr_profile_p = bcmFieldSystemHeaderProfileFtmhTshPphUdh;
            break;
        }
        case DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_UDH:
        {
            *bcm_sys_hdr_profile_p = bcmFieldSystemHeaderProfileFtmhUdh;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "DNX System Header Profile:\"%d\" is not supported\n", dnx_sys_hdr_profile);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_apptype_to_opcode_bcm_to_dnx_int(
    int unit,
    bcm_field_AppType_t app_type,
    uint32 *opcode_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(opcode_id_p, _SHR_E_PARAM, "opcode_id_p");

    if ((int) app_type < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Apptype (%d) cannot be negative.\n", app_type);
    }

    if (DNX_FIELD_APPTYPE_IS_USER_DEF(unit, app_type))
    {
        int relative_dyn_apptype = app_type - dnx_data_field.external_tcam.apptype_user_1st_get(unit);
        uint8 is_alloc;
        uint8 opcode_id_uint8;
        SHR_IF_ERR_EXIT(dnx_algo_field_context_apptype_is_allocated(unit, app_type, &is_alloc));
        if (is_alloc == FALSE)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.opcode_id.get(unit, relative_dyn_apptype, &opcode_id_uint8));
        (*opcode_id_p) = opcode_id_uint8;
    }
    else
    {
        uint8 is_standard_image;
        uint32 opcode_id;
        uint8 nof_contexts;
        shr_error_e rv;

        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
        if (is_standard_image)
        {
            /** For standard_1 use mapping convert predefined apptype to opcode.*/
            if (app_type >= bcmFieldAppTypeCount)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Apptype %d is out of range. Range is %d-%d for static Apptypes, and "
                             "%d-%d for dynamic.\n",
                             app_type, 0, bcmFieldAppTypeCount - 1,
                             dnx_data_field.external_tcam.apptype_user_1st_get(unit),
                             dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                             dnx_data_field.external_tcam.apptype_user_nof_get(unit) - 1);
            }

            rv = dnx_field_map_standard_1_predefined_apptype_to_opcode_get(unit, app_type, &opcode_id);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        else
        {
            if (app_type >= DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Apptype %d is out of range. Range is %d-%d for static Apptypes, and "
                             "%d-%d for dynamic.\n",
                             app_type, 0,
                             DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF - 1,
                             dnx_data_field.external_tcam.apptype_user_1st_get(unit),
                             dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                             dnx_data_field.external_tcam.apptype_user_nof_get(unit) - 1);
            }
            /** For bare metal predefined opcode and apptypes are the same..*/
            opcode_id = app_type;
        }
        {
            shr_error_e rv;
            bcm_field_AppType_t mapped_apptype;
            rv = dnx_field_map_static_opcode_to_apptype_get(unit, opcode_id, &mapped_apptype);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            else
            {
                /** Sanity check. */
                if (app_type != mapped_apptype)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Apptype %d is mapped to opcode %d, which is mapped to apptype %d.\n",
                                 app_type, opcode_id, mapped_apptype);
                }
                SHR_IF_ERR_EXIT(rv);
            }
        }
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_fwd2_context_ids.get(unit, app_type, &nof_contexts));
        if (nof_contexts <= 0)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        (*opcode_id_p) = opcode_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_apptype_to_opcode_bcm_to_dnx(
    int unit,
    bcm_field_AppType_t app_type,
    uint32 *opcode_id_p)
{
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(opcode_id_p, _SHR_E_PARAM, "opcode_id_p");

    rv = dnx_field_map_apptype_to_opcode_bcm_to_dnx_int(unit, app_type, opcode_id_p);
    if (rv == _SHR_E_NOT_FOUND)
    {
        if (app_type >= dnx_data_field.external_tcam.apptype_user_1st_get(unit) &&
            app_type <
            dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
            dnx_data_field.external_tcam.apptype_user_nof_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Dynamic Apptype %d is not allocated.\n", app_type);
        }
        else
        {
            uint8 is_standard_image;
            uint8 nof_contexts;
            uint32 opcode_id;

            SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
            if (is_standard_image)
            {
                shr_error_e rv_apptype_map;
                rv_apptype_map = dnx_field_map_standard_1_predefined_apptype_to_opcode_get(unit, app_type, &opcode_id);
                if (rv_apptype_map == _SHR_E_NOT_FOUND)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Unmapped predefined apptype %d.\n", app_type);
                }
                else
                {
                    SHR_IF_ERR_EXIT(rv_apptype_map);
                }
            }
            else
            {
                if (app_type >= DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d is out of range. Range is %d-%d for static Apptypes, and "
                                 "%d-%d for dynamic. Expected to fail before.\n",
                                 app_type, 0,
                                 DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF - 1,
                                 dnx_data_field.external_tcam.apptype_user_1st_get(unit),
                                 dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                                 dnx_data_field.external_tcam.apptype_user_nof_get(unit) - 1);
                }
                opcode_id = app_type;
            }
            {
                shr_error_e rv_opcode_map;
                bcm_field_AppType_t mapped_apptype;
                rv_opcode_map = dnx_field_map_static_opcode_to_apptype_get(unit, opcode_id, &mapped_apptype);
                if (rv_opcode_map == _SHR_E_NOT_FOUND)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Unmapped predefined apptype %d.\n", app_type);
                }
                else
                {
                    /** Sanity check. */
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d is mapped to opcode %d, which is mapped to apptype %d.\n",
                                 app_type, opcode_id, mapped_apptype);
                    SHR_IF_ERR_EXIT(rv_opcode_map);
                }
            }
            /** Sanity check.*/
             /* coverity[unreachable:FALSE]  */
            if (opcode_id == DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal opcode.\n");
            }
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_fwd2_context_ids.get(unit, app_type, &nof_contexts));
            if (nof_contexts <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Predefined apptype \"%s\" (%d) is mapped but not used on device.\n",
                             dnx_field_bcm_apptype_text(unit, app_type), app_type);
            }
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Predefined apptype \"%s\" (%d) is mapped and used but was not found.\n",
                             dnx_field_bcm_apptype_text(unit, app_type), app_type);
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_opcode_to_apptype_dnx_to_bcm(
    int unit,
    uint32 opcode_id,
    bcm_field_AppType_t * app_type_p)
{
    uint32 nof_user_apptypes;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(app_type_p, _SHR_E_PARAM, "app_type_p");

    nof_user_apptypes = dnx_data_field.external_tcam.apptype_user_nof_get(unit);

    if (opcode_id >= DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF)
    {
        bcm_field_AppType_t app_type_index;
        uint8 is_app_type_found = FALSE;
        uint8 opcode_id_uint8;
        for (app_type_index = 0; app_type_index < nof_user_apptypes; app_type_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.opcode_id.get(unit, app_type_index, &opcode_id_uint8));
            if (opcode_id_uint8 == opcode_id)
            {
                (*app_type_p) = dnx_data_field.external_tcam.apptype_user_1st_get(unit) + app_type_index;
                is_app_type_found = TRUE;
                break;
            }
        }

        if (!is_app_type_found)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Dynamic opcode_id %d not found.%s%s%s\r\n", opcode_id, EMPTY, EMPTY, EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }
    else
    {
        bcm_field_AppType_t app_type;
        uint8 nof_contexts;
        shr_error_e rv;

        rv = dnx_field_map_static_opcode_to_apptype_get(unit, opcode_id, &app_type);
        if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Static opcode_id %d not found.%s%s%s\r\n", opcode_id, EMPTY, EMPTY, EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }

        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_fwd2_context_ids.get(unit, app_type, &nof_contexts));
        if (nof_contexts <= 0)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Static opcode_id %d not used by any FWD context.%s%s%s\r\n",
                         opcode_id, EMPTY, EMPTY, EMPTY);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }

        (*app_type_p) = app_type;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_static_opcode_to_profile_get(
    int unit,
    uint32 opcode_id,
    uint32 *cs_profile_id_p)
{
    uint8 nof_contexts;
    uint32 cs_profile;
    bcm_field_AppType_t app_type;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cs_profile_id_p, _SHR_E_PARAM, "cs_profile_id_p");

    SHR_IF_ERR_EXIT(dnx_field_map_static_opcode_to_apptype_get(unit, opcode_id, &app_type));

    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_fwd2_context_ids.get(unit, app_type, &nof_contexts));
    if (nof_contexts <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "opcode_id %d not used by any FWD context.\n", opcode_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_map_static_opcode_to_cs_profile_get(unit, opcode_id, &cs_profile));

    (*cs_profile_id_p) = cs_profile;

    /*
     * Sanity check.
     */
    if ((*cs_profile_id_p) <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d has no CS profile allocated.\n", opcode_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_apptype_to_profile_get(
    int unit,
    bcm_field_AppType_t app_type,
    uint32 *cs_profile_id_p)
{
    uint32 opcode_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cs_profile_id_p, _SHR_E_PARAM, "cs_profile_id_p");

    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, app_type, &opcode_id));

    if ((int) app_type >= 0 && app_type < bcmFieldAppTypeCount)
    {
        /** Static Apptypes.*/
        SHR_IF_ERR_EXIT(dnx_field_map_static_opcode_to_profile_get(unit, opcode_id, cs_profile_id_p));
    }
    else
    {
        int relative_dyn_apptype = app_type - dnx_data_field.external_tcam.apptype_user_1st_get(unit);
        uint8 profile_id_uint8;

        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.
                        profile_id.get(unit, relative_dyn_apptype, &profile_id_uint8));
        (*cs_profile_id_p) = profile_id_uint8;

        /*
         * Sanity check.
         */
        if ((*cs_profile_id_p) <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Dynamic apptype %d has no CS profile allocated.\n", app_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_profile_to_apptype_get(
    int unit,
    uint32 cs_profile_id,
    bcm_field_AppType_t * app_type_p)
{
    uint8 is_alloc;
    int found;
    uint32 opcode_id;
    uint32 cs_profile_id_get;
    uint8 cs_profile_id_get_uint8;
    unsigned int app_type_index;
    int nof_user_defined_apptypes;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(app_type_p, _SHR_E_PARAM, "app_type_p");

    /*
     * Check if the profile is allocated.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_external_tcam_opcode_cs_profile_id_is_allocated(unit, cs_profile_id, &is_alloc));
    if (is_alloc == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Profile ID %d is not allocated.\n", cs_profile_id);
    }

    found = FALSE;

    /*
     * Search all Predefined Apptypes.
     */
    DNX_FIELD_KBP_OPCODE_ITERATOR(opcode_id)
    {
        DNX_FIELD_KBP_OPCODE_NOT_IN_USE_CONTINUE(unit, opcode_id);
        SHR_IF_ERR_EXIT(dnx_field_map_static_opcode_to_profile_get(unit, opcode_id, &cs_profile_id_get));
        if (cs_profile_id == cs_profile_id_get)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, app_type_p));
            found = TRUE;
            break;
        }
    }

    /*
     * Search all user defined Apptypes if not found in predefined.
     */
    if (found == FALSE)
    {
        nof_user_defined_apptypes = dnx_data_field.external_tcam.apptype_user_nof_get(unit);
        for (app_type_index = 0; app_type_index < nof_user_defined_apptypes; app_type_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.
                            profile_id.get(unit, app_type_index, &cs_profile_id_get_uint8));
            if (cs_profile_id == cs_profile_id_get_uint8)
            {
                (*app_type_p) = dnx_data_field.external_tcam.apptype_user_1st_get(unit) + app_type_index;
                found = TRUE;
                break;
            }
        }
    }

    /** Sanity check.*/
    if (found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Profile ID %d is allocated but not used.\n", cs_profile_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_apptype_to_lookup_fwd_contexts(
    int unit,
    bcm_field_AppType_t apptype,
    int *nof_contexts_p,
    dnx_field_context_t lkp_fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE])
{
    uint32 opcode_id;
    unsigned int nof_contexts_stage = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;
    bcm_field_AppType_t base_apptype;
    int context_index;
    uint8 fwd_context;
    uint8 nof_contexts;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_contexts_p, _SHR_E_PARAM, "nof_contexts_p");
    SHR_NULL_CHECK(lkp_fwd_contexts, _SHR_E_PARAM, "lkp_fwd_contexts");

    /*
     * Check that the apptype exists, and get the opcode.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype, &opcode_id));

    if ((int) apptype >= 0 && apptype < bcmFieldAppTypeCount)
    {
        /*
         * Predefined Apptype.
         */
        base_apptype = apptype;
    }
    else
    {
        /*
         * User defined apptype.
         */
        uint8 base_apptype_uint8;
        int apptype_index = apptype - dnx_data_field.external_tcam.apptype_user_1st_get(unit);
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.base_apptype.get(unit, apptype_index, &base_apptype_uint8));
        base_apptype = base_apptype_uint8;
    }

    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_fwd2_context_ids.get(unit, base_apptype, &nof_contexts));
    if (nof_contexts > DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FWD contexts for apptype %d (\"%s\") is %d, maximum is %d.\n",
                     base_apptype, dnx_field_bcm_apptype_text(unit, base_apptype), nof_contexts,
                     DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
    }
    for (context_index = 0; context_index < nof_contexts; context_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.
                        fwd2_context_ids.get(unit, base_apptype, context_index, &fwd_context));
        /** Sanity check */
        if (((unsigned int) fwd_context) >= nof_contexts_stage)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d (\"%s\") opcode %d context number %d is %d. "
                         "Number of contexts in stage is %d.\n",
                         base_apptype, dnx_field_bcm_apptype_text(unit, base_apptype), opcode_id, context_index,
                         fwd_context, nof_contexts_stage);
        }
        lkp_fwd_contexts[context_index] = fwd_context;
    }
    (*nof_contexts_p) = nof_contexts;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_apptype_to_cs_fwd_contexts(
    int unit,
    bcm_field_AppType_t apptype,
    int *nof_contexts_p,
    dnx_field_context_t cs_fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE])
{
    uint32 opcode_id;
    unsigned int nof_contexts_stage = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;
    bcm_field_AppType_t base_apptype;
    int context_index;
    uint8 fwd_context;
    uint8 nof_contexts;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_contexts_p, _SHR_E_PARAM, "nof_contexts_p");
    SHR_NULL_CHECK(cs_fwd_contexts, _SHR_E_PARAM, "cs_fwd_contexts");

    /*
     * Check that the apptype exists, and get the opcode.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype, &opcode_id));

    if ((int) apptype >= 0 && apptype < bcmFieldAppTypeCount)
    {
        /*
         * Predefined Apptype.
         */
        base_apptype = apptype;
    }
    else
    {
        /*
         * User defined apptype.
         */
        uint8 base_apptype_uint8;
        int apptype_index = apptype - dnx_data_field.external_tcam.apptype_user_1st_get(unit);
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.base_apptype.get(unit, apptype_index, &base_apptype_uint8));
        base_apptype = base_apptype_uint8;
    }

    if (dnx_data_field.features.apptype_based_on_fwd2_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.nof_fwd2_context_ids.get(unit, base_apptype, &nof_contexts));
        if (nof_contexts > DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FWD contexts for apptype %d (\"%s\") is %d, maximum is %d.\n",
                         base_apptype, dnx_field_bcm_apptype_text(unit, base_apptype), nof_contexts,
                         DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
        }
        for (context_index = 0; context_index < nof_contexts; context_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_apptype_sw.predef_info.
                            fwd2_context_ids.get(unit, base_apptype, context_index, &fwd_context));
            /** Sanity check */
            if (((unsigned int) fwd_context) >= nof_contexts_stage)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d (\"%s\") opcode %d context number %d is %d. "
                             "Number of contexts in stage is %d.\n",
                             base_apptype, dnx_field_bcm_apptype_text(unit, base_apptype), opcode_id, context_index,
                             fwd_context, nof_contexts_stage);
            }
            cs_fwd_contexts[context_index] = fwd_context;
        }
        (*nof_contexts_p) = nof_contexts;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "option not compiled.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_apptype_to_acl_contexts(
    int unit,
    bcm_field_AppType_t apptype,
    int *nof_contexts_p,
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE])
{
    uint32 opcode_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_contexts_p, _SHR_E_PARAM, "nof_contexts_p");
    SHR_NULL_CHECK(acl_contexts, _SHR_E_PARAM, "acl_contexts");

    /*
     * Check that the apptype exists, and get the opcode.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype, &opcode_id));

    if ((int) apptype >= 0 && apptype < bcmFieldAppTypeCount)
    {
        /*
         * Predefined Apptype. ACL contexts are the same as CS FWD contexts.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_cs_fwd_contexts(unit, apptype, nof_contexts_p, acl_contexts));
    }
    else
    {
        /*
         * User defined Apptype. Take contexts from the designated module.
         */
        switch (dnx_data_field.external_tcam.type_get(unit))
        {
            case DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP:
            {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
                dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
                SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_to_contexts(unit, opcode_id, nof_contexts_p, fwd_contexts,
                                                                 acl_contexts));
#else
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached KBP function but KBP not compiled.\n");
#endif
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown external TCAM device %d.\r\n",
                             dnx_data_field.external_tcam.type_get(unit));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_is_device_locked(
    int unit,
    int *device_locked_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(device_locked_p, _SHR_E_PARAM, "device_locked_p");

    switch (dnx_data_field.external_tcam.type_get(unit))
    {
        case DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP:
        {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
            SHR_IF_ERR_EXIT(dnx_field_kbp_is_device_locked(unit, device_locked_p));
#else
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached KBP function but KBP not compiled.\n");
#endif
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown external TCAM device %d.\r\n",
                         dnx_data_field.external_tcam.type_get(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_is_external_tcam_available(
    int unit,
    int *external_tcam_exists_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(external_tcam_exists_p, _SHR_E_PARAM, "external_tcam_exists_p");

    switch (dnx_data_field.external_tcam.type_get(unit))
    {
        case DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP:
        {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
            SHR_IF_ERR_EXIT(dnx_field_kbp_is_kbp_device_available(unit, external_tcam_exists_p));
#else
            (*external_tcam_exists_p) = FALSE;
#endif
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown external TCAM device %d.\r\n",
                         dnx_data_field.external_tcam.type_get(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_contexts_to_apptype_get(
    int unit,
    uint32 lkp_fwd_context,
    uint32 acl_context,
    bcm_field_AppType_t * app_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(app_type_p, _SHR_E_PARAM, "app_type_p");

    {
        uint32 opcode_id;
        /** Sanity check. */
        if (lkp_fwd_context != ((uint8) lkp_fwd_context))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "fwd_context %d does not fit within one byte.\r\n", lkp_fwd_context);
        }
        if (acl_context != ((uint8) acl_context))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "acl_context %d does not fit within one byte.\r\n", acl_context);
        }

        switch (dnx_data_field.external_tcam.type_get(unit))
        {
            case DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP:
            {
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
                SHR_IF_ERR_EXIT(dnx_field_kbp_contexts_to_opcode_get(unit, lkp_fwd_context, acl_context, &opcode_id));
#else
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached KBP function but KBP not compiled.\n");
#endif
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown external TCAM device %d.\r\n",
                             dnx_data_field.external_tcam.type_get(unit));
            }
        }

        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, app_type_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_state_table_source_get(
    int unit,
    int *use_acr_p,
    dnx_field_stage_e * stage_p,
    dbal_enum_value_field_field_key_e * key_id_p,
    int *lsb_on_key_p,
    int *size_bits_p)
{
    dnx_field_state_table_source_t state_table_source = dnx_data_field.state_table.state_table_stage_key_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(stage_p, _SHR_E_PARAM, "stage_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(lsb_on_key_p, _SHR_E_PARAM, "lsb_on_key_p");

    switch (state_table_source)
    {
        case DNX_FIELD_STATE_TABLE_SOURCE_IPMF1_KEY_J:
        {
            (*use_acr_p) = FALSE;
            (*stage_p) = DNX_FIELD_STAGE_IPMF1;
            (*key_id_p) = DBAL_ENUM_FVAL_FIELD_KEY_J;
            (*size_bits_p) = dnx_data_field.state_table.key_size_get(unit);
            (*lsb_on_key_p) = dnx_data_field.tcam.key_size_single_get(unit) - (*size_bits_p);
            break;
        }
        case DNX_FIELD_STATE_TABLE_SOURCE_IPMF2_KEY_J:
        {
            (*use_acr_p) = FALSE;
            (*stage_p) = DNX_FIELD_STAGE_IPMF2;
            (*key_id_p) = DBAL_ENUM_FVAL_FIELD_KEY_J;
            (*size_bits_p) = dnx_data_field.state_table.key_size_get(unit);
            (*lsb_on_key_p) = dnx_data_field.tcam.key_size_single_get(unit) - (*size_bits_p);
            break;
        }
        case DNX_FIELD_STATE_TABLE_SOURCE_IPMF1_KEY_F:
            /*
             * Key F not supported. If it was:
             * (*use_acr_p) = FALSE;
             * (*stage_p) = DNX_FIELD_STAGE_IPMF1;
             * (*key_id_p) = DBAL_ENUM_FVAL_FIELD_KEY_F;
             * (*size_bits_p) = dnx_data_field.state_table.key_size_get(unit);
             * (*lsb_on_key_p) = dnx_data_field.state_table.address_size_entry_max_get(unit) +
             *                   dnx_data_field.state_table.data_size_entry_max_get(unit) -
             *                   dnx_data_field.state_table.address_size_get(unit) -
             *                   dnx_data_field.state_table.data_size_get(unit);
             */
        default:
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Unsupported option in SOC property pmf_state_table_rmw_source.\r\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_compare_key_id_get(
    int unit,
    dnx_field_context_compare_mode_pair_e compare_pair,
    dnx_field_key_id_t *key_id_1_p,
    dnx_field_key_id_t *key_id_2_p)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_field_key_id_t_init(unit, key_id_1_p);
    dnx_field_key_id_t_init(unit, key_id_2_p);
    /**
     * Compare uses KEY F,G as first pair, and H,I as second pair
     */
    switch (compare_pair)
    {
        case DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1:
            /** First pair gets F and G keys,*/
            key_id_1_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_F;
            key_id_2_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_G;
            break;
        case DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_2:
            /** Second pair gets H and I keys*/
            key_id_1_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_H;
            key_id_2_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_I;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid compare key pair %d", compare_pair);
    }
    key_id_1_p->nof_bits[0] = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size;
    key_id_1_p->offset_on_first_key = dnx_data_field.tcam.key_size_single_get(unit) - key_id_1_p->nof_bits[0];
    key_id_2_p->nof_bits[0] = key_id_1_p->nof_bits[0];
    key_id_2_p->offset_on_first_key = key_id_1_p->offset_on_first_key;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_key_id_get(
    int unit,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    dnx_field_key_id_t_init(unit, key_id_p);
    /*
     * Hash uses initial keys I and J.
     */
    key_id_p->offset_on_first_key = 0;
    key_id_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_I;
    key_id_p->nof_bits[0] = dnx_data_field.tcam.key_size_single_get(unit);
    key_id_p->id[1] = DBAL_ENUM_FVAL_FIELD_KEY_J;
    key_id_p->nof_bits[1] = dnx_data_field.tcam.key_size_single_get(unit);

exit:
    SHR_FUNC_EXIT;
}
