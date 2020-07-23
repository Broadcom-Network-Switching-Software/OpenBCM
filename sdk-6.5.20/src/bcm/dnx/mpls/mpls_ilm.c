/** \file mpls_ilm.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_encap.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/qos/qos.h>

#define MPLS_LABEL_BITS           (20)
/*
 * }
 */


static shr_error_e
dnx_mpls_tunnel_switch_ilm_add_verify(
    int unit,
    int result_type,
    bcm_mpls_tunnel_switch_t * info)
{
    uint8 is_physical_port;
    int system_headers_mode;
    int mul_swap_command;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    mul_swap_command = dnx_data_headers.feature.feature_get(unit, dnx_data_headers_feature_fhei_mpls_cmd_from_eei);

    /*
     * Jericho mode beginning
     * {
     */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_NEXT_HEADER_L2) ||
            _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_NEXT_HEADER_IPV4) ||
            _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_NEXT_HEADER_IPV6))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags BCM_MPLS_SWITCH_NEXT_HEADER_XXL are only supported for JR mode");
        }
    }
    /*
     * }
     * Jericho mode end
     */

    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_OUTER_EXP) ||
        _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_OUTER_TTL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "flags BCM_MPLS_SWITCH_OUTER_EXP and BCM_MPLS_SWITCH_OUTER_TTL are not supported, replaced by egress_label.egress_qos_model");
    }
    if ((info->egress_label.egress_qos_model.egress_qos != bcmQosEgressModelUniform)
        && (info->egress_label.egress_qos_model.egress_qos != bcmQosEgressModelPipeNextNameSpace))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "QoS model only support uniform and pipeNextNameSpace");
    }
    if ((info->egress_label.egress_qos_model.egress_ttl != bcmQosEgressModelUniform)
        && (info->egress_label.egress_qos_model.egress_ttl != bcmQosEgressModelPipeMyNameSpace))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TTL model only support uniform and pipeMyNameSpace");
    }
    if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP && DNX_QOS_MAP_PROFILE_GET(info->qos_map_id) != 0)
    {
        if (!mul_swap_command || (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "info->qos_map_id must be 0");
        }

        if (!DNX_QOS_MAP_IS_EGRESS(info->qos_map_id) ||
            !DNX_QOS_MAP_IS_REMARK(info->qos_map_id) ||
            (DNX_QOS_MAP_PROFILE_GET(info->qos_map_id) >= DNX_NOF_SWAP_REMARK_PROFILE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "info->qos_map_id %d is out of range (1~7)",
                         DNX_QOS_MAP_PROFILE_GET(info->qos_map_id));
        }
    }

    switch (result_type)
    {
        case DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_OUTLIF:
        case DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_OUTLIF_STAT:
            /** Valid forwarding scenario: info->egress_if is out lif. Then info->port should be either a FEC pointer or a physical port.  */
            if (BCM_L3_ITF_TYPE_IS_LIF(info->egress_if))
            {
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, info->port, &is_physical_port));

                if (!BCM_GPORT_IS_FORWARD_PORT(info->port) && !is_physical_port)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "info->port (0x%08X) is not valid since it is not a valid destination\r\n",
                                 info->port);
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "info->egress_if (0x%08X) is invalid since L3 interface type is not LIF and different than 0. See BCM_L3_ITF_TYPE_IS_LIF\r\n",
                             info->egress_if);
            }
            break;
        case DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST:
        case DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_STAT:
            if (BCM_L3_ITF_TYPE_IS_LIF(info->egress_if))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Egress interface can't be a lif .\r\n");
            }
            break;
        case DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI:
        case DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI_STAT:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported result type %d .\r\n", result_type);
            break;
    }

    if (((result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI)
         || (result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI_STAT))
        && (info->egress_label.label == BCM_MPLS_LABEL_INVALID) && (info->action == BCM_MPLS_SWITCH_ACTION_SWAP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "egress label should be specified when configuring SWAP action.\r\n");
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_mpls_tunnel_switch_ilm_delete_verify(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * Jericho mode beginning
     * {
     */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        if (info->action == BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH are only supported for JR mode");
        }
    }
    /*
     * }
     * Jericho mode end
     */

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mpls_tunnel_switch_ilm_add(
    int unit,
    int result_type,
    bcm_mpls_tunnel_switch_t * info)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_forward_info_t forward_info;
    uint32 full_field_val = 0;
    bcm_gport_t gport;
    dbal_tables_e dbal_table_id;
    /*
     * Jericho mode beginning
     * {
     */
    int push_cmd = 0;
    /*
     * }
     * Jericho mode end
     */

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verification. */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_switch_ilm_add_verify(unit, result_type, info));

    /** retrieve needed params from info for destination */
    sal_memset(&forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));
    /*
     * if p2mp then destination is MC, otherwise destination is egress-object
     */
    if (info->flags & BCM_MPLS_SWITCH_P2MP)
    {
        BCM_GPORT_MCAST_SET(gport, info->mc_group);
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, gport, &forward_info.destination));
    }
    else
    {
        if (BCM_L3_ITF_TYPE_IS_LIF(info->egress_if) || (info->egress_if == 0)
            || BCM_FORWARD_ENCAP_ID_IS_EEI(info->egress_if))
        {
            SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                            (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, info->port, &forward_info.destination));
        }
        else
        {
            BCM_GPORT_FORWARD_PORT_SET(gport, BCM_L3_ITF_VAL_GET(info->egress_if));
            SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                            (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, gport, &forward_info.destination));
        }
    }

    /** write to table */
    /** prepare key for requested table */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL))
    {
        /** Special match - {Context-label, Mpls-label} */
        dbal_table_id = DBAL_TABLE_MPLS_FWD_PER_IF_UA;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, info->second_label);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_LABEL, info->label);
    }
    else if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF))
    {
        /** Special match - {Context-LIF, Label} */
        dbal_table_id = DBAL_TABLE_MPLS_FWD_PER_IF_UA;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, info->label);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, BCM_L3_ITF_VAL_GET(info->ingress_if));
    }
    else
    {
        /** "Vanilla" single label match */
        dbal_table_id = DBAL_TABLE_MPLS_FWD;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, info->label);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    if ((result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_OUTLIF) ||
        (result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_OUTLIF_STAT))
    {
        forward_info.outlif = BCM_L3_ITF_VAL_GET(info->egress_if);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, forward_info.outlif);
    }
    else if ((result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI)
             || (result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI_STAT))
    {
        int system_headers_mode;
        uint32 eei_value = 0;
        uint32 command;

        system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        if ((info->action == BCM_MPLS_SWITCH_ACTION_PHP) || ((info->action == BCM_MPLS_SWITCH_ACTION_POP_DIRECT)))
        {
            dbal_enum_value_field_model_type_e qos_model;
            dbal_enum_value_field_model_type_e ttl_model;
            dbal_enum_value_field_jr_fwd_code_e upper_layer_type = DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS;

            /*
             * Jericho mode beginning
             * {
             */
            if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            {
                upper_layer_type =
                    _SHR_IS_FLAG_SET(info->flags,
                                     BCM_MPLS_SWITCH_NEXT_HEADER_L2) ? DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE
                    : (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_NEXT_HEADER_IPV4) ?
                       DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC
                       : (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_NEXT_HEADER_IPV6) ?
                          DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC : DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS));
            }
            /*
             * }
             * Jericho mode end
             */
            if (info->egress_label.egress_qos_model.egress_qos == bcmQosEgressModelUniform)
            {
                qos_model = DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM;
            }
            else
            {
                qos_model = DBAL_ENUM_FVAL_MODEL_TYPE_PIPE;
            }
            if (info->egress_label.egress_qos_model.egress_ttl == bcmQosEgressModelUniform)
            {
                ttl_model = DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM;
            }
            else
            {
                ttl_model = DBAL_ENUM_FVAL_MODEL_TYPE_PIPE;
            }

            SHR_IF_ERR_EXIT(dnx_mpls_eei_php_information_fill
                            (unit, info->qos_map_id, qos_model, ttl_model, upper_layer_type, &full_field_val));

            if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            {
                command = DNX_EEI_IDENTIFIER_POP;
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, DBAL_FIELD_EEI_MPLS_POP_MULTI_SWAP, DBAL_FIELD_MPLS_COMMAND, &command,
                                 &eei_value));
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, DBAL_FIELD_EEI_MPLS_POP_MULTI_SWAP, DBAL_FIELD_MPLS_COMMAND_INFO,
                                 &full_field_val, &eei_value));

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, eei_value);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI_MPLS_POP_COMMAND, INST_SINGLE,
                                             full_field_val);
            }
        }
        else if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP)
        {
            if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            {
                uint32 profile = DNX_QOS_MAP_PROFILE_GET(info->qos_map_id);

                 /**command will set to eei identifier*/
                command = DNX_EEI_IDENTIFIER_MULTI_SWAP_START + profile;
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, DBAL_FIELD_EEI_MPLS_POP_MULTI_SWAP, DBAL_FIELD_MPLS_COMMAND, &command,
                                 &eei_value));
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, DBAL_FIELD_EEI_MPLS_POP_MULTI_SWAP, DBAL_FIELD_MPLS_COMMAND_INFO,
                                 &info->egress_label.label, &eei_value));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_SWAP_COMMAND, &info->egress_label.label,
                                 &eei_value));
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, eei_value);
        }
        /*
         * Jericho mode beginning
         * {
         */
        else if (info->action == BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH)
        {
            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_create_push_entry_from_eei(unit, &(info->egress_label), FALSE,
                                                                       _SHR_IS_FLAG_SET(info->flags,
                                                                                        BCM_MPLS_SWITCH_REPLACE),
                                                                       &push_cmd));

            SHR_IF_ERR_EXIT(dnx_mpls_eei_push_information_fill
                            (unit, info->egress_label.label, push_cmd, &full_field_val));

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, INST_SINGLE,
                                         full_field_val);

        }
        /*
         * }
         * Jericho mode end
         */
        /*
         * Jericho mode beginning
         * {
         */
        else if (info->action == BCM_MPLS_SWITCH_ACTION_NOP)
        {
            dnx_mpls_eei_enc_pointer_information_fill(unit, BCM_FORWARD_ENCAP_ID_VAL_GET(info->egress_if), 0,
                                                      &full_field_val);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, INST_SINGLE,
                                         full_field_val);
        }
        /*
         * }
         * Jericho mode end
         */
    }
    /*
     * Nothing extra to be done in case of result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST/STAT (only destination)
     */

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, forward_info.destination);

    if (dnx_stat_pp_result_type_verify(unit, dbal_table_id, result_type) == _SHR_E_NONE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, info->stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(info->stat_pp_profile));
    }

    /*
     * use DBAL_COMMIT_FORCE for both ADD and REPLACE actions
     * FORCE is done for backward compatibility
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_mpls_tunnel_switch_ilm_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    uint32 entry_handle_id, result_type;
    dnx_algo_gpm_forward_info_t forward_info;
    uint32 encap_id;
    uint32 eei;
    dbal_fields_e dbal_eei_type;
    uint32 eei_val = 0;
    bcm_if_t eei_enc_pointer;
    bcm_gport_t gport;
    uint32 stat_pp_profile;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * this is done for the _traverse API as all other APIs expect correct 'action' to be provided
     */
    sal_memset(&forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));
    /*
     * set default ILM action that might be updated later
     */
    info->action = BCM_MPLS_SWITCH_ACTION_NOP;

    /** read from table */
    /** prepare key for requested table */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL))
    {
        /** Special match - {Context-label, Mpls-label} */
        dbal_table_id = DBAL_TABLE_MPLS_FWD_PER_IF_UA;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, info->second_label);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_LABEL, info->label);
    }
    else if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF))
    {
        /** Special match - {Context-LIF, Label} */
        dbal_table_id = DBAL_TABLE_MPLS_FWD_PER_IF_UA;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, info->label);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, BCM_L3_ITF_VAL_GET(info->ingress_if));
    }
    else
    {
        /** "Vanilla" single label match */
        dbal_table_id = DBAL_TABLE_MPLS_FWD;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, info->label);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
    if ((result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_OUTLIF) ||
        (result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_OUTLIF_STAT))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                                            &forward_info.outlif));
    }
    else if ((result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI) ||
             (result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI_STAT))
    {
        dbal_enum_value_field_model_type_e qos_model;
        dbal_enum_value_field_model_type_e ttl_model;
        dbal_enum_value_field_jr_fwd_code_e upper_layer_type;
        uint32 flags = 0;
        int system_headers_mode;
        uint32 profile = 0;
        dnx_mpls_encap_t encap_info;
        bcm_mpls_egress_label_t label_array[2];
        int push_cmd;
        uint32 label;
        uint8 host_idx;

        system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &eei));
        if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            uint32 command;

            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_POP_MULTI_SWAP, DBAL_FIELD_MPLS_COMMAND, &command, &eei));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_POP_MULTI_SWAP, DBAL_FIELD_MPLS_COMMAND_INFO, &eei_val, &eei));
            /*
             * command is eei identifier
             */
            if (command == DNX_EEI_IDENTIFIER_POP)
            {
                dbal_eei_type = DBAL_FIELD_EEI_MPLS_POP_COMMAND;
            }
            else if ((command >= DNX_EEI_IDENTIFIER_MULTI_SWAP_START)
                     && (command <= DNX_EEI_IDENTIFIER_MULTI_SWAP_LAST))
            {
                dbal_eei_type = DBAL_FIELD_EEI_MPLS_SWAP_COMMAND;
                profile = command - DNX_EEI_IDENTIFIER_MULTI_SWAP_START;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "EEI identifier %d is not supported", command);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_EEI, eei, &dbal_eei_type, &eei_val));
        }

        switch (dbal_eei_type)
        {
            case DBAL_FIELD_EEI_MPLS_SWAP_COMMAND:
                /*
                 * this is done for the _traverse API as all other APIs expect correct 'action' to be provided
                 */
                info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
                info->egress_label.label = eei_val;
                /** EEI identifier 1:1 mapping qos profile*/
                if (profile != DNX_QOS_INITIAL_MAP_ID)
                {
                    info->qos_map_id = profile;
                    DNX_QOS_REMARK_MAP_SET(info->qos_map_id);
                    DNX_QOS_EGRESS_MAP_SET(info->qos_map_id);
                }
                info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
                break;
            case DBAL_FIELD_EEI_MPLS_POP_COMMAND:
                /*
                 * this is done for the _traverse API as all other APIs expect correct 'action' to be provided
                 */
                /*
                 * action can be BCM_MPLS_SWITCH_ACTION_POP_DIRECT or BCM_MPLS_SWITCH_ACTION_PHP as they are identical
                 */
                info->action = BCM_MPLS_SWITCH_ACTION_PHP;
                SHR_IF_ERR_EXIT(dnx_mpls_eei_php_information_retrieve
                                (unit, eei_val, &info->qos_map_id, &qos_model, &ttl_model, &upper_layer_type));
                if (qos_model == DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM)
                {
                    info->egress_label.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
                }
                else
                {
                    info->egress_label.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
                }

                if (ttl_model == DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM)
                {
                    info->egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
                }
                else
                {
                    info->egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
                }
                /*
                 * Jericho mode beginning
                 * {
                 */
                if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
                {
                    flags = (upper_layer_type == DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE) ? BCM_MPLS_SWITCH_NEXT_HEADER_L2 :
                        ((upper_layer_type == DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC) ? BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 :
                         ((upper_layer_type ==
                           DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC) ? BCM_MPLS_SWITCH_NEXT_HEADER_IPV6 : 0));
                    info->flags |= flags;
                }
                /*
                 * }
                 * Jericho mode end
                 */
                break;
                /*
                 * Jericho mode beginning
                 * {
                 */
            case DBAL_FIELD_EEI_MPLS_PUSH_COMMAND:
                /*
                 * this is done for the _traverse API as all other APIs expect correct 'action' to be provided
                 */
                info->action = BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH;
                SHR_IF_ERR_EXIT(dnx_mpls_eei_push_information_retrieve(unit, eei, &label, &push_cmd));

                dnx_mpls_encap_t_init(unit, &encap_info);
                bcm_mpls_egress_label_t_init(&label_array[0]);
                bcm_mpls_egress_label_t_init(&label_array[1]);
                encap_info.label_array = label_array;

                SHR_IF_ERR_EXIT(dnx_mpls_encap_get_from_push_cmd(unit, push_cmd, &encap_info));

                sal_memcpy(&(info->egress_label), label_array, sizeof(bcm_mpls_egress_label_t));
                info->egress_label.label = label;

                break;
                /*
                 * }
                 * Jericho mode end
                 */
                /*
                 * Jericho mode beginning
                 * {
                 */
            case DBAL_FIELD_EEI_ENCAPSULATION_POINTER:
                info->action = BCM_MPLS_SWITCH_ACTION_NOP;
                SHR_IF_ERR_EXIT(dnx_mpls_eei_enc_pointer_information_retrieve(unit, eei, &eei_enc_pointer, &host_idx));
                break;
                /*
                 * }
                 * Jericho mode end
                 */
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal_eei_type (%d) is not supported for get api \r\n", dbal_eei_type);
        }
    }
    else if ((result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST) ||
             (result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_STAT))
    {
        /*
         * Nothing extra to be done in case of result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST (only destination)
         */
    }
    
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "result_type (%d) is not supported for get api \r\n", result_type);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                                        &forward_info.destination));

    /*
     * Valid forwarding scenarios are:
     * 1) info->egress_if is out lif. Then info->port should be either a FEC pointer or a physical port.
     * 2) Info->egress_if is LIF. Then info->port must be a FEC pointer.
     * 3) multicast
     * Using algo_gpm_gport_and_encap_from_forward_information with force_port_and_encap=1
     * because we don't need a logical gport here
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_from_forward_information(unit, &gport, &encap_id, &forward_info, 1));

    if (BCM_GPORT_IS_MCAST(gport))
    {
        /** destination is of type MC_ID */
        info->flags |= BCM_MPLS_SWITCH_P2MP;
        info->mc_group = BCM_GPORT_MCAST_GET(gport);
    }
    else if (BCM_GPORT_IS_FORWARD_PORT(gport))
    {
        /*
         * Jericho mode beginning
         * {
         */
        if (((result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI) ||
             (result_type == DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_EEI_STAT)) &&
            (dbal_eei_type == DBAL_FIELD_EEI_ENCAPSULATION_POINTER))
        {
            info->port = gport;
            BCM_FORWARD_ENCAP_ID_VAL_SET(info->egress_if, BCM_FORWARD_ENCAP_ID_TYPE_EEI,
                                         BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER,
                                         BCM_L3_ITF_VAL_GET(eei_enc_pointer));
        }
        /*
         * }
         * Jericho mode end
         */
        else
        {
            /** destination is of type FEC */
            BCM_L3_ITF_SET(info->egress_if, BCM_L3_ITF_TYPE_FEC, BCM_GPORT_FORWARD_PORT_GET(gport));
        }
    }
    else
    {
        /** destination is of type PORT_ID or other */
        info->port = gport;
        if (encap_id != 0)
        {
            BCM_L3_ITF_SET(info->egress_if, BCM_L3_ITF_TYPE_LIF, encap_id);
        }
    }

    info->flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;

    if (dnx_stat_pp_result_type_verify(unit, dbal_table_id, result_type) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id,
                                                            DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));
        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                &info->stat_id));

            STAT_PP_ENGINE_PROFILE_SET(info->stat_pp_profile, stat_pp_profile, bcmStatCounterInterfaceIngressReceivePp);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_mpls_tunnel_switch_ilm_delete(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    uint32 entry_handle_id;
    uint32 field_value;
    int push_cmd;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verification. */
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_tunnel_switch_ilm_delete_verify(unit, info));

    /** prepare key for requested table */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL))
    {
        /** Special match - {Context-label, Mpls-label} */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_FWD_PER_IF_UA, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, info->second_label);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_LABEL, info->label);
    }
    else if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF))
    {
        /** Special match - {Context-LIF, Label} */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_FWD_PER_IF_UA, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, info->label);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, BCM_L3_ITF_VAL_GET(info->ingress_if));
    }
    else
    {
        /** "Vanilla" single label match */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_FWD, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, info->label);
    }

    if (info->action == BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH)
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &field_value));

        /** Get the push command from EEI[22:20] */
        push_cmd = field_value >> MPLS_LABEL_BITS;
        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_delete_push_entry_from_eei(unit, push_cmd));
    }
    /** delete from table */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_mpls_tunnel_switch_ilm_delete_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** clear tables */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_MPLS_FWD));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_MPLS_FWD_PER_IF_UA));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all MPLS tunnels in the ingress for forwarding (swap) and run a callback function
 * provided by the user for each one.
 * \param [in] unit - The unit number.
 * \param [in] cb   - A pointer to callback function,
 *          it receives the value of the user_data variable and
 *          all MPLS tunnel objects that were iterated
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * DBAL_TABLE_MPLS_FWD
 *  * DBAL_TABLE_MPLS_FWD_PER_IF_UA
 */
shr_error_e
dnx_mpls_tunnel_switch_ilm_traverse(
    int unit,
    bcm_mpls_tunnel_switch_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 dbal_tables[] = { DBAL_TABLE_MPLS_FWD, DBAL_TABLE_MPLS_FWD_PER_IF_UA };
    uint32 field_value[1];
    uint32 nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    uint32 table;
    int is_end;
    bcm_mpls_tunnel_switch_t info;
    uint32 max_rsv_label = MPLS_MAX_RESERVED_LABEL;
    dbal_fields_e sub_field_id;
    uint32 sub_field_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Iterate over all tables and all their entries
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[0], &entry_handle_id));
    for (table = 0; table < nof_tables; table++)
    {
        /*
         * Allocate handle to the table of the iteration and initialize an iterator entity.
         * The iterator is in mode ALL without default, which means that it will consider all entries except
         * default entries.
         */
        if (table > 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_tables[table], entry_handle_id));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        /** Add KEY rule to skip MPLS reserved label */
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, DBAL_CONDITION_BIGGER_THAN, &max_rsv_label,
                         NULL));
        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            bcm_mpls_tunnel_switch_t_init(&info);
            if (dbal_tables[table] == DBAL_TABLE_MPLS_FWD_PER_IF_UA)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_MPLS_EXTENDED_FODO, field_value));
                SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                                (unit, DBAL_FIELD_MPLS_EXTENDED_FODO, field_value, &sub_field_id, &sub_field_val));
                if (sub_field_id == DBAL_FIELD_GLOB_IN_LIF)
                {
                    /** Special match - {Context-LIF, Label},  set */
                    info.ingress_if = sub_field_val;
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, field_value));
                    info.label = field_value[0];
                    info.flags |= BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF;
                }
                else if (sub_field_id == DBAL_FIELD_CONTEXT_LABEL)
                {
                    /** Special match - {Context-label, Mpls-label} */
                    info.label = sub_field_val;
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, field_value));
                    info.second_label = field_value[0];
                    info.flags |= BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid sub field id %d in MPLS ILM entry.", sub_field_id);
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, field_value));
                info.label = field_value[0];
            }

            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_ilm_get(unit, &info));
            SHR_IF_ERR_EXIT((*cb) (unit, &info, user_data));
            /*
             * Receive next entry in table.
             */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
