/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_pp_qos.c
 *
 * Occupation Management Diagnostic Command - Retrieves all allocated in and outlif profile resources.
 */

/*************
 * INCLUDES  *
 */
#include "diag_dnx_pp.h"
#include <appl/diag/system.h>
#include <bcm_int/dnx/qos/qos.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include "kleap/diag_dnx_ekleap.h"
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <soc/dnx/dbal/dbal.h>

/*************
 * DEFINES   *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

#define DNX_PP_COS_VALUE_INVALID            0xFFFFFFFF
#define DNX_PP_COS_PRINT_WIDTH_NONE         0
#define DNX_PP_COS_PRINT_WIDTH_QOS_DP       3
#define DNX_PP_COS_PRINT_WIDTH_DP           1
#define DNX_PP_COS_PRINT_WIDTH_QOS_PROFILE  2
#define DNX_PP_COS_PRINT_WIDTH_OPCODE       2
#define DNX_PP_COS_PRINT_WIDTH_QOS_VAR      2

#define DNX_PP_COS_PRINT_NA_STRING          "-"

/** get signal rhentry per given signal-name.*/
#define _GET_RHENTRY_BY_NAME(_core_id, _rhlist, _sig_name, _list_element)      \
{ \
    _list_element = utilex_rhlist_entry_get_by_name(_rhlist, _sig_name);       \
    if (_list_element == NULL)      \
    {                               \
        SHR_CLI_EXIT(_SHR_E_NONE, "signal %s is not performed for Core_Id=%d\n", _sig_name, _core_id);  \
    }                               \
}

/** get ETPS entry from shared_data according to the given index.*/
#define _GET_ETPS_ENTRY_BY_ID(_core_id, shared_data, entry_idx, _list_element)      \
{ \
    _list_element = shared_data->etps_data.entry_signals[entry_idx].entry_sig; \
    if (_list_element == NULL)      \
    {                               \
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "No ETPS signal on index %d for Core_Id=%d\n", entry_idx, _core_id); \
    }                               \
}

#define _PRINT_CELL_SET_LAYER_HEADER(layer_index, layer_protocol) \
{ \
    if (layer_index != DNX_PP_COS_VALUE_INVALID) \
    {    \
        PRT_CELL_SET("%d (%s)", layer_index, layer_protocol);     \
    }    \
    else \
    {    \
        PRT_CELL_SET("%s", DNX_PP_COS_PRINT_NA_STRING);           \
    }    \
}

/**
 * \brief
 *  QoS cell format
 */
typedef enum
{
    /** print cess format is normal value */
    NORMAL,
    /** print cess format is qos_and_qp */
    QOS_DP,
    /** print cess format is enum string */
    ENUM,
    /** Must be last */
    NOF_FORMAT
} dnx_pp_cos_print_cell_format_e;

/**
 * \brief
 *  QoS diag type
 */
typedef enum
{
    /** diag pp qos ingress remark */
    DNX_QOS_DIAG_INGRESS_REMARK,
    /** diag pp qos ingress phb  */
    DNX_QOS_DIAG_INGRESS_PHB,
    /** Must be last */
    NOF_DNX_QOS_DIAG
} dnx_pp_qos_diag_type_e;

/**
 * \brief - set print cell.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] cell_value - print cell value.
 *   \param [in] print_width - print width for non-string cell.
 *               Used in case of print_cell_format is not "ENUM".
 *   \param [in,out] prt_ctr - print controler.
 *   \param [in] print_cell_format - print cell format.
 *               In case of "ENUM", dbal_table and dbal_field must be provided.
 *   \param [in] dbal_table - dbal table used in resolving field enum string.
 *   \param [in] dbal_field - dbal field used in resolving field enum string.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_print_cell_set(
    int unit,
    uint32 cell_value,
    uint32 print_width,
    prt_control_t * prt_ctr,
    dnx_pp_cos_print_cell_format_e print_cell_format,
    dbal_tables_e dbal_table,
    dbal_fields_e dbal_field)
{
    uint32 qos_value, dp_value;
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if (cell_value == DNX_PP_COS_VALUE_INVALID)
    {
        PRT_CELL_SET("%s", DNX_PP_COS_PRINT_NA_STRING);
    }
    else
    {
        if (print_cell_format == ENUM)
        {
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                            (unit, dbal_table, dbal_field, &cell_value, NULL, 0, FALSE, buffer));
            PRT_CELL_SET("%s", buffer);
        }
        else if (print_cell_format == QOS_DP)
        {
            dp_value = (cell_value >> DNX_QOS_DP_OFFSET);
            qos_value = (cell_value & DNX_QOS_TOS_MASK);
            PRT_CELL_SET("0x%02X, 0x%02X", dp_value, qos_value);
        }
        else
        {
            PRT_CELL_SET("0x%0*X", print_width, cell_value);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get layer protocol.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] layer_index - qos layer index.
 *   \param [out] layer_protocol - layer protocol for print.
 *   \param [out] nof_vlan_tags - vlan tags number.
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_ingress_layer_protocol_get(
    int unit,
    int core,
    uint32 layer_index,
    char *layer_protocol,
    uint32 *nof_vlan_tags)
{
    char *block = NULL, *from = NULL, *to = NULL;
    char sig_name[RHNAME_MAX_SIZE];
    signal_output_t *list_element = NULL, *vlan_list_element;
    uint32 tags_number;
    uint32 layer_header_type = DNX_PP_COS_VALUE_INVALID;
    uint32 layer_type;
    char *layer_header_protocol = DNX_PP_COS_PRINT_NA_STRING;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    block = "IRPP";
    from = "VTT5";
    to = "FWD1";

    /*
     * get the layer header type
     */
    sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "Layer_Protocols.%d", layer_index);

    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, block, from, to, sig_name, &list_element),
                        "Signal:%s From:%s not found\n", sig_name, from);
    layer_header_type = list_element->value[0];
    SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_LAYER_TYPES, layer_header_type, &layer_type));
    tags_number = 0;
    /*
     * translate layer type to protocol
     */
    switch (layer_type)
    {
        case DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET:
        {
            /*
             * get eth layer qualifier
             */
            sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "Layer_Qualifiers.%d", layer_index);
            SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, block, from, to, sig_name, &list_element),
                                "Signal:%s From:%s not found\n", sig_name, from);

            /** check if 1st tag exist */
            _GET_RHENTRY_BY_NAME(core, list_element->field_list, "1st_tpid_exist", vlan_list_element);
            tags_number += (vlan_list_element->value[0] ? 1 : 0);

            /** check if 2nd tag exist */
            _GET_RHENTRY_BY_NAME(core, list_element->field_list, "2nd_tpid_exist", vlan_list_element);
            tags_number += (vlan_list_element->value[0] ? 1 : 0);

            /** check if 3rd tag exist */
            _GET_RHENTRY_BY_NAME(core, list_element->field_list, "3rd_tpid_exist", vlan_list_element);
            tags_number += (vlan_list_element->value[0] ? 1 : 0);
            switch (tags_number)
            {
                case 0:
                    layer_header_protocol = "ETH0";
                    break;
                case 1:
                    layer_header_protocol = "ETH1";
                    break;
                case 2:
                    layer_header_protocol = "ETH2";
                    break;
                default:
                    layer_header_protocol = "ETH3";
            }
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV4:
        {
            layer_header_protocol = "IPV4";
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV6:
        {
            layer_header_protocol = "IPV6";
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS:
        {
            layer_header_protocol = "MPLS";
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM:
        {
            layer_header_protocol = "MPLS_FWD";
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_RCH:
        {
            layer_header_protocol = "RCH";
            break;
        }
        default:
        {
            layer_header_protocol = "In-Port";
        }
    }

    /*
     * output assignment
     */
    if (layer_protocol != NULL)
    {
        sal_strncpy(layer_protocol, layer_header_protocol, RHNAME_MAX_SIZE - 1);
    }

    if (nof_vlan_tags != NULL)
    {
        *nof_vlan_tags = tags_number;
    }

exit:
    sand_signal_output_free(list_element);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get orignal qos value and map context for given layer index.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] diag_type - phb or remark
 *   \param [in] layer_index - qos layer index.
 *   \param [in] cs_profile - qos contxt selection profile.
 *   \param [out] layer_protocol - layer protocol.
 *   \param [out] src_qos_value - original qos value from incoming packet.
 *   \param [out] qos_var_flags - qos map context flags.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_ingress_src_qos_info_get(
    int unit,
    int core,
    dnx_pp_qos_diag_type_e diag_type,
    uint32 layer_index,
    uint32 cs_profile,
    char *layer_protocol,
    uint32 *src_qos_value,
    uint32 *qos_var_flags)
{
    char *block = NULL, *from = NULL, *to = NULL;
    char sig_name[RHNAME_MAX_SIZE];
    char sig_mpls_terminated[RHNAME_MAX_SIZE];
    char sig_strength_name[RHNAME_MAX_SIZE];
    char qos_value_str[RHNAME_MAX_SIZE];
    signal_output_t *list_element = NULL;
    uint32 nof_vlan_tags = 0;
    uint32 is_layer_update = FALSE;
    uint32 layer_offset;
    uint32 qos_value_nibble_offset = 0, qos_value_nof_nibbles = 0, right_shift_nof_bits;
    uint32 tag_nibble_offset = 0;
    uint32 qos_layer_index;
    uint32 qos_value_mask;
    uint32 key_number;
    uint32 is_inner = FALSE;
    uint32 layer_header_type = DNX_PP_COS_VALUE_INVALID;
    uint32 pkt_qos_value = DNX_PP_COS_VALUE_INVALID;
    uint32 qos_var_type_flags = DNX_PP_COS_VALUE_INVALID;
    uint32 is_mpls_terminated = FALSE;
    uint32 layer_type;
    uint32 qos_strength;
    uint32 is_rch_phb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    block = "IRPP";
    from = "VTT5";
    to = "FWD1";

    qos_value_nibble_offset = 0;
    qos_value_nof_nibbles = 0;
    right_shift_nof_bits = 0;
    qos_value_mask = 0;
    key_number = 1;
    is_rch_phb = FALSE;

    /**layer protocol is current qos profile layer, others used for qos key which comes from next layer*/
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_layer_protocol_get(unit, core, layer_index, layer_protocol, &nof_vlan_tags));

    /*
     * get the layer header type
     */
    sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "Layer_Protocols.%d", layer_index);

    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, block, from, to, sig_name, &list_element),
                        "Signal:%s From:%s not found\n", sig_name, from);
    layer_header_type = list_element->value[0];
    SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_LAYER_TYPES, layer_header_type, &layer_type));
    if (diag_type == DNX_QOS_DIAG_INGRESS_PHB)
    {
        sal_snprintf(sig_mpls_terminated, RHNAME_MAX_SIZE - 1, "PHB_is_Terminated_MPLS");
        sal_snprintf(sig_strength_name, RHNAME_MAX_SIZE - 1, "PHB_Strength");
    }
    else
    {
        sal_snprintf(sig_mpls_terminated, RHNAME_MAX_SIZE - 1, "Rm_is_Terminated_MPLS");
        sal_snprintf(sig_strength_name, RHNAME_MAX_SIZE - 1, "Rm_Strength");
    }
    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, block, from, to, sig_mpls_terminated, &list_element),
                        "Signal:%s From:%s not found\n", sig_mpls_terminated, from);
    is_mpls_terminated = list_element->value[0];

    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, block, from, to, sig_strength_name, &list_element),
                        "Signal:%s From:%s not found\n", sig_strength_name, from);
    qos_strength = list_element->value[0];
    if (qos_strength == 0)
    {
        is_mpls_terminated = 0;
    }
    /*
     * get layer header offset and its parsing
     */
    if (is_mpls_terminated &&
        ((layer_type == DBAL_ENUM_FVAL_LAYER_TYPES_MPLS) || (layer_type == DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM)))
    {
        if (diag_type == DNX_QOS_DIAG_INGRESS_PHB)
        {
            sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "PHB_MPLS_Offset");
        }
        else
        {
            sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "Rm_MPLS_Offset");
        }
    }
    else
    {
        sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "Layer_Offsets.%d", layer_index);
    }
    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, block, from, to, sig_name, &list_element),
                        "Signal:%s From:%s not found\n", sig_name, from);
    layer_offset = list_element->value[0];

    switch (cs_profile)
    {
        case DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_DOUBLE:
        {
            /**two tags pcp&dei*/
            key_number = 2;
            break;
        }
        case DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_INNER:
        {
            is_inner = TRUE;
            break;
        }
        case DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2:
        {
            qos_layer_index = layer_index + 1;
            is_layer_update = TRUE;
            break;
        }
        case DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_TYPICAL:
        {
            /** MPLS always occupy two layers*/
            qos_layer_index = layer_index + 2;
            is_layer_update = TRUE;
            /**two tags pcp&dei*/
            nof_vlan_tags = 2;
            key_number = 2;
            break;
        }
        case DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_OUTER:
        {
            /** MPLS always occupy two layers*/
            qos_layer_index = layer_index + 2;
            nof_vlan_tags = 1;
            is_layer_update = TRUE;
            break;
        }
        default:
        {
            qos_layer_index = layer_index;
        }
    }
    if (is_layer_update)
    {
        sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "Layer_Offsets.%d", qos_layer_index);
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, block, from, to, sig_name, &list_element),
                            "Signal:%s From:%s not found\n", sig_name, from);
        layer_offset = list_element->value[0];
        sal_snprintf(sig_name, RHNAME_MAX_SIZE - 1, "Layer_Protocols.%d", qos_layer_index);

        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, block, from, to, sig_name, &list_element),
                            "Signal:%s From:%s not found\n", sig_name, from);
        layer_header_type = list_element->value[0];
        SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_LAYER_TYPES, layer_header_type, &layer_type));
    }

    /*
     * calculate key offset and length
     */
    switch (layer_type)
    {
        case DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET:
        {
            qos_var_type_flags = BCM_QOS_MAP_L2;
             /** PCP_DEI is taken from outer tag*/
            qos_value_nibble_offset = (layer_offset + 12 + 2) * 2;
            qos_value_nof_nibbles = 1;
            qos_value_mask = DNX_QOS_PCP_DEI_MASK;
            switch (nof_vlan_tags)
            {
                case 0:
                {
                    qos_var_type_flags |= BCM_QOS_MAP_L2_UNTAGGED;
                    /** QoS map in explicit mode, no src_qos_value, set it with invalid.*/
                    pkt_qos_value = DNX_PP_COS_VALUE_INVALID;
                    key_number = 0;
                    break;
                }
                case 1:
                {
                    /** PCP_DEI is taken from outer tag*/
                    qos_var_type_flags |= BCM_QOS_MAP_L2_OUTER_TAG;
                    break;
                }
                default:
                {
                    if (is_inner)
                    {
                        qos_var_type_flags |= BCM_QOS_MAP_L2_INNER_TAG;
                        /** shift offset one tag*/
                        tag_nibble_offset = 8;
                    }
                    else if (key_number == 2)
                    {
                        qos_var_type_flags |= BCM_QOS_MAP_L2_TWO_TAGS;
                        /*
                         * copy first inner pcp
                         */
                        tag_nibble_offset = 8;
                        qos_value_mask = (DNX_QOS_PCP_DEI_MASK << DNX_QOS_PCP_DEI_OFFSET) | DNX_QOS_PCP_DEI_MASK;
                    }
                    else
                    {
                        qos_var_type_flags |= BCM_QOS_MAP_L2_OUTER_TAG;
                    }
                }
            }
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV4:
        {
            qos_var_type_flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4;
            /** DSCP is taken from IPv4 header*/
            qos_value_nibble_offset = (layer_offset + 1) * 2;
            qos_value_nof_nibbles = 2;
            right_shift_nof_bits = 0;
            qos_value_mask = DNX_QOS_TOS_MASK;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV6:
        {
            qos_var_type_flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6;
            /** TC is taken from IPv6 header*/
            qos_value_nibble_offset = layer_offset * 2 + 1;
            qos_value_nof_nibbles = 2;
            right_shift_nof_bits = 0;
            qos_value_mask = DNX_QOS_TOS_MASK;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS:
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM:
        {
            qos_var_type_flags = BCM_QOS_MAP_MPLS;

            /** EXP is taken from current layer MPLS*/
            qos_value_nibble_offset = layer_offset * 2 + 5;
            qos_value_nof_nibbles = 1;
            right_shift_nof_bits = 1;
            qos_value_mask = DNX_QOS_EXP_MASK;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_RCH:
        {
            qos_var_type_flags = BCM_QOS_MAP_RCH;

            if (diag_type == DNX_QOS_DIAG_INGRESS_PHB)
            {
                key_number = 1;
                qos_value_nibble_offset = layer_offset * 2 + 8;
                qos_value_nof_nibbles = 2;
                is_rch_phb = TRUE;
                qos_value_mask = (DNX_QOS_TC_MASK << DNX_QOS_TC_OFFSET) | DNX_QOS_DP_MASK;
            }
            else
            {
                qos_value_nibble_offset = layer_offset * 2 + 24;
                qos_value_nof_nibbles = 2;
                qos_value_mask = DNX_QOS_TOS_MASK;
            }
            break;
        }
        default:
        {
            qos_var_type_flags = BCM_QOS_MAP_PORT;
            qos_value_nof_nibbles = 0;
            pkt_qos_value = DNX_PP_COS_VALUE_INVALID;
            key_number = 0;
        }
    }
    /*
     * get packet header and its parsing
     */
    if (sand_signal_output_find(unit, core, 0, "IRPP", "", "", "header", &list_element) == _SHR_E_NONE)
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, block, "NIF", NULL, "Header", &list_element),
                            "Signal:Header From:NIF not found\n");
    }
    else
    {
        /** for special cases where instead of from "NIF" and "InNetworkAdaptor", used another block name as "PRT" */
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, block, "PRT", "", "Pkt_Header",
                                                    &list_element), "no signal record for header\n");
    }

    /** convert the received subfield value to numeric */
    if (key_number != 0)
    {
        sal_strncpy(qos_value_str, list_element->print_value + qos_value_nibble_offset + tag_nibble_offset,
                    qos_value_nof_nibbles);
        if (key_number > 1)
        {
            /**outer PCP&DEI in case double tags*/
            sal_strncpy(qos_value_str + qos_value_nof_nibbles, list_element->print_value + qos_value_nibble_offset,
                        qos_value_nof_nibbles);
            qos_value_nof_nibbles = qos_value_nof_nibbles * 2;
        }
        qos_value_str[qos_value_nof_nibbles] = '\0';
         /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
        pkt_qos_value = 0;
        SHR_IF_ERR_EXIT(utilex_str_xstoul(qos_value_str, &pkt_qos_value));
        if (is_rch_phb == TRUE)
        {
            pkt_qos_value = ((pkt_qos_value & DNX_QOS_TC_MASK) << 2) | ((pkt_qos_value >> 5) & DNX_QOS_DP_MASK);
        }
        pkt_qos_value = (pkt_qos_value >> right_shift_nof_bits) & qos_value_mask;
    }

    /*
     * output assignment
     */
    if (qos_var_flags != NULL)
    {
        *qos_var_flags = qos_var_type_flags;
    }
    if (src_qos_value != NULL)
    {
        *src_qos_value = pkt_qos_value;
    }

exit:
    sand_signal_output_free(list_element);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get ingress qos mapping information for given qos type.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] qos_type - qos map type.
 *   \param [in] cs_profile - qos context selection profile.
 *   \param [out] fwd_layer_index - forward layer index.
 *   \param [out] fwd_layer_protocol - forward layer protocol.
 *   \param [out] qos_layer_index - qos layer index.
 *   \param [out] qos_layer_protocol - qos layer protocol.
 *   \param [out] qos_profile - qos-profile used in qos mapping.
 *   \param [out] map_index - qos map family index.
 *   \param [out] src_qos_value - original qos value from incoming packet.
 *   \param [out] opcode - opcode in current qos mapping.
 *   \param [out] mapped_qos_value - array for mapped qos value.
 *   \param [out] final_qos_value - array for final qos value to ITM.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_ingress_qos_info_get(
    int unit,
    int core,
    char *qos_type,
    uint32 *cs_profile,
    uint32 *fwd_layer_index,
    char *fwd_layer_protocol,
    uint32 *qos_layer_index,
    char *qos_layer_protocol,
    uint32 *qos_profile,
    uint32 *map_index,
    uint32 *src_qos_value,
    uint32 *opcode,
    uint32 *mapped_qos_value,
    uint32 *final_qos_value)
{
    char *s_qos_layer_index;
    char *s_qos_layer_strength;
    char *s_qos_mapping_profile;
    char *s_mapped_qos_value_1, *s_mapped_qos_value_2;
    uint32 qos_strength = 0;
    uint32 entry_handle_id;
    int cs_table_id;
    uint32 opcode_table_id;
    uint32 qos_var_flags = 0;
    dnx_pp_qos_diag_type_e diag_type = DNX_QOS_DIAG_INGRESS_REMARK;
    signal_output_t *list_element = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!sal_strcasecmp(qos_type, "remark"))
    {
        s_qos_layer_index = "rm_layer_index";
        s_qos_layer_strength = "Rm_Strength";
        s_qos_mapping_profile = "rm_mapping_profile";
        s_mapped_qos_value_1 = "nwk_qos";
        s_mapped_qos_value_2 = NULL;
        cs_table_id = DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_INFO;
        opcode_table_id = DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID;
        diag_type = DNX_QOS_DIAG_INGRESS_REMARK;
    }
    else if (!sal_strcasecmp(qos_type, "phb"))
    {
        s_qos_layer_index = "phb_layer_index";
        s_qos_layer_strength = "PHB_Strength";
        s_qos_mapping_profile = "phb_mapping_profile";
        s_mapped_qos_value_1 = "tc";
        s_mapped_qos_value_2 = "dp";
        cs_table_id = DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO;
        opcode_table_id = DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID;
        diag_type = DNX_QOS_DIAG_INGRESS_PHB;
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Please specify some qos type to display\n");
    }

    /*
     * 1. get the fwd_layer_index and its parsing
     */
    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                        (unit, core, 0, "IRPP", "VTT5", "FWD1", "Fwd_Layer_Index", &list_element),
                        "Signal: Fwd_Layer_Index From:VTT5 not found\n");
    *fwd_layer_index = list_element->value[0];

    /** translate the layer index to layer protocol*/
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_layer_protocol_get(unit, core, *fwd_layer_index, fwd_layer_protocol, NULL));

    /*
     * 2. get the qos_layer_index and its parsing
     */
    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                        (unit, core, 0, "IRPP", "VTT5", "FWD1", s_qos_layer_index, &list_element),
                        "Signal:qos_layer_index not found\n");
    *qos_layer_index = list_element->value[0];

    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                        (unit, core, 0, "IRPP", "VTT5", "FWD1", s_qos_layer_strength, &list_element),
                        "Signal:qos_layer_strength not found\n");
    qos_strength = list_element->value[0];

    if (qos_strength == 0)
    {
        *qos_layer_index = *fwd_layer_index;
    }

    /*
     * 3. get the qos mapping profile and its parsing
     */
    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                        (unit, core, 0, "IRPP", "VTT5", "FWD1", s_qos_mapping_profile, &list_element),
                        "Signal:qos_mapping_profile not found\n");
    *qos_profile = list_element->value[0];
    /*
     * 4. get CS profile
     */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_cs_profile_map_get(unit, cs_table_id, *qos_profile, cs_profile));
    /*
     * 5. get qos_layer_protocol and the src_qos_value from qos layer header
     */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_src_qos_info_get(unit, core, diag_type, *qos_layer_index, *cs_profile,
                                                           qos_layer_protocol, src_qos_value, &qos_var_flags));

    /*
     * 6. get map_index (qos map family)
     */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_map_index_get(unit, qos_var_flags, map_index));

    /*
     * 7. get the opcode used by qos mapping if not explicit mode.
     */

    if (!(((qos_var_flags & BCM_QOS_MAP_L2) && (qos_var_flags & BCM_QOS_MAP_L2_UNTAGGED)) ||
          (qos_var_flags & BCM_QOS_MAP_PORT)))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, opcode_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, *qos_profile);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_QOS_MAP_INDEX, *map_index);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, INST_SINGLE, opcode);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * 8. get mapped qos value and its parsing.
     */
    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                        (unit, core, 0, "IRPP", "FWD1", "FWD2", s_mapped_qos_value_1, &list_element),
                        "Signal:fwd qos_mapped_value1 not found\n");
    mapped_qos_value[0] = list_element->value[0];
    if (!ISEMPTY(s_mapped_qos_value_2))
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                            (unit, core, 0, "IRPP", "FWD1", "FWD2", s_mapped_qos_value_2, &list_element),
                            "Signal:fwd qos_mapped_vlaue2 not found\n");
        mapped_qos_value[1] = list_element->value[0];
    }

    /*
     * 9. get the qos value to ITM and its parsing.
     */
    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                        (unit, core, 0, "IRPP", "IPMF3", "LBP", s_mapped_qos_value_1, &list_element),
                        "Signal:lbp qos_mapped_value1 not found\n");
    final_qos_value[0] = list_element->value[0];
    if (!ISEMPTY(s_mapped_qos_value_2))
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                            (unit, core, 0, "IRPP", "IPMF3", "LBP", s_mapped_qos_value_2, &list_element),
                            "Signal:lbp qos_mapped_value2 not found\n");
        final_qos_value[1] = list_element->value[0];
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Prepare qos mapping information for given stage
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] qos_type - qos map type.
 *   \param [in,out] prt_ctr - print controler.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_ingress_qos_type_prepare(
    int unit,
    int core,
    char *qos_type,
    prt_control_t * prt_ctr)
{
    uint32 fwd_layer_index;
    uint32 qos_layer_index;
    uint32 qos_profile;
    uint32 src_qos_value;
    uint32 map_index;
    uint32 cs_profile;
    uint32 opcode;
    uint32 mapped_qos_value[2];
    uint32 final_qos_value[2];
    char fwd_layer_protocol[RHNAME_MAX_SIZE];
    char qos_layer_protocol[RHNAME_MAX_SIZE];

    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    fwd_layer_index = DNX_PP_COS_VALUE_INVALID;
    qos_layer_index = DNX_PP_COS_VALUE_INVALID;
    qos_profile = DNX_PP_COS_VALUE_INVALID;
    map_index = DNX_PP_COS_VALUE_INVALID;
    src_qos_value = DNX_PP_COS_VALUE_INVALID;
    opcode = DNX_PP_COS_VALUE_INVALID;
    mapped_qos_value[0] = DNX_PP_COS_VALUE_INVALID;
    mapped_qos_value[1] = DNX_PP_COS_VALUE_INVALID;
    final_qos_value[0] = DNX_PP_COS_VALUE_INVALID;
    final_qos_value[1] = DNX_PP_COS_VALUE_INVALID;

    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_qos_info_get(unit, core, qos_type, &cs_profile,
                                                       &fwd_layer_index, fwd_layer_protocol,
                                                       &qos_layer_index, qos_layer_protocol, &qos_profile,
                                                       &map_index, &src_qos_value, &opcode, mapped_qos_value,
                                                       final_qos_value));

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    /** 1. qos type */
    PRT_CELL_SET("%s", qos_type);

    /** 2. fwd_layer_index */
    _PRINT_CELL_SET_LAYER_HEADER(fwd_layer_index, fwd_layer_protocol);

    /** 3. qos_layer_index */
    _PRINT_CELL_SET_LAYER_HEADER(qos_layer_index, qos_layer_protocol);

    /** 4. qos_profile */
    PRT_CELL_SET("%d", qos_profile);

    /** 5. context cs profile */
    if (!sal_strcasecmp(qos_type, "remark"))
    {
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_INFO,
                         DBAL_FIELD_QOS_CS_PROFILE, &cs_profile, NULL, 0, FALSE, buffer));
    }
    else if (!sal_strcasecmp(qos_type, "phb"))
    {
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO,
                         DBAL_FIELD_QOS_CS_PROFILE, &cs_profile, NULL, 0, FALSE, buffer));
    }
    PRT_CELL_SET("%s", buffer);

    /** 6. map mode(explicit/mapped) */
    if (map_index == DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT)
    {
        PRT_CELL_SET("%s", "EXPLICIT");
    }
    else
    {
        PRT_CELL_SET("%s", "MAPPED");
    }

    /** 7. src_qos_value */
    /** 8. opcode */
    if (src_qos_value == DNX_PP_COS_VALUE_INVALID)
    {
        /** Explicit mode: no src qos value and opcode.*/
        PRT_CELL_SET("%s", DNX_PP_COS_PRINT_NA_STRING);
        PRT_CELL_SET("%s", DNX_PP_COS_PRINT_NA_STRING);
    }
    else
    {
        PRT_CELL_SET("0x%02X", src_qos_value);
        PRT_CELL_SET("0x%02X", opcode);
    }

    /** 9. nwk_qos */
    if (!sal_strcasecmp(qos_type, "phb"))
    {
        PRT_CELL_SET("%d (TC), %d (DP)", mapped_qos_value[0], mapped_qos_value[1]);
    }
    else
    {
        PRT_CELL_SET("0x%02X (%s)", mapped_qos_value[0], "NWK_QOS");
    }

    /** 10. Final nwk_qos */
    if (!sal_strcasecmp(qos_type, "phb"))
    {
        PRT_CELL_SET("%d (TC), %d (DP)", final_qos_value[0], final_qos_value[1]);
    }
    else
    {
        PRT_CELL_SET("0x%02X (%s)", final_qos_value[0], "NWK_QOS");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - print ingress qos mapping information per qos type(remark/phb)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] qos_type - qos type, currently remark/phb
 *   \param [in] sand_control - according to diag mechanism definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_ingress_qos_info_print(
    int unit,
    int core,
    char *qos_type,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (ISEMPTY(qos_type))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Please specify some qos type to display\n");
    }

    /** Output in table format */
    PRT_TITLE_SET("Ingress qos mapping info through core_%d", core);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "QoS Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "fwd_layer_index");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "qos_layer_index");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "qos_profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "map control type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "map mode\n(explicit/mapped)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "source qos value");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "opcode");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "mapped qos value");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "qos to ITM");

    /** print the table */
    if (strcaseindex(qos_type, "remark") || strcaseindex(qos_type, "rm"))
    {
        SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_qos_type_prepare(unit, core, "ReMark", prt_ctr));
    }

    if (strcaseindex(qos_type, "phb"))
    {
        SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_qos_type_prepare(unit, core, "PHB", prt_ctr));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get ingress qos mapping information for given qos type.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] qos_type - qos map type.
 *   \param [in] stage_name - stage name.(LLR,VTTx (x=1..5) is supported.)
 *   \param [out] port_lif_id - in LLR, it's pp port id, otherwise global lif id.
 *   \param [out] fwd_layer_index - forward layer index.
 *   \param [out] fwd_layer_protocol - forward layer protocol.
 *   \param [out] qos_layer_index - qos layer index.
 *   \param [out] qos_layer_protocol - qos layer protocol.
 *   \param [out] qos_map_profile - qos profile used in qos mapping.
 *   \param [out] propag_profile - qos propagation profile getting in this stage.
 *   \param [out] propag_strength - propagation strength related to propag_profile.
 *   \param [out] comp_strength - compare strength related to propag_profile.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_ingress_propag_info_get(
    int unit,
    int core,
    char *qos_type,
    char *stage_name,
    uint32 *port_lif_id,
    uint32 *fwd_layer_index,
    char *fwd_layer_protocol,
    uint32 *qos_layer_index,
    char *qos_layer_protocol,
    uint32 *qos_map_profile,
    uint32 *propag_profile,
    uint32 *propag_strength,
    uint32 *comp_strength)
{
    char *block = NULL;
    char *sig_qos_layer_index = NULL;
    char *sig_qos_profile = NULL;
    char *sig_port_strength = NULL;

    uint32 global_in_lif, local_in_lif, local_in_lif_core;
    lif_mapping_local_lif_info_t local_lif_info;

    uint32 entry_handle_id;

    dbal_tables_e inlif_dbal_table_id;
    uint32 inlif_dbal_result_type;
    dbal_enum_value_field_qos_ingress_app_type_e qos_ingress_app_type;

    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!sal_strcasecmp(qos_type, "remark"))
    {
        sig_qos_layer_index = "rm_layer_index";
        sig_qos_profile = "rm_mapping_profile";
        qos_ingress_app_type = DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK;
        sig_port_strength = "Rm_Strength";
    }
    else if (!sal_strcasecmp(qos_type, "phb"))
    {
        sig_qos_layer_index = "phb_layer_index";
        sig_qos_profile = "phb_mapping_profile";
        qos_ingress_app_type = DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB;
        sig_port_strength = "PHB_Strength";
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Please specify some qos type to display\n");
    }

    block = "IRPP";

    /*
     * 1. get fwd_layer_index
     */
    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, block, stage_name, NULL, "Fwd_Layer_Index", fwd_layer_index, 1));

    /*
     * 2. get fwd_layer_protocol
     */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_layer_protocol_get(unit, core, *fwd_layer_index, fwd_layer_protocol, NULL));

    /*
     * 3. get qos_layer_index
     */
    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, block, stage_name, NULL, sig_qos_layer_index, qos_layer_index, 1));

    /*
     * 4. get qos_layer_protocol
     */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_layer_protocol_get(unit, core, *qos_layer_index, qos_layer_protocol, NULL));

    /*
     * 5. get qos_map_profile
     */
    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, block, stage_name, NULL, sig_qos_profile, qos_map_profile, 1));

    /** possible stage name is defined in stage_list_prt*/
    if (sal_strcasecmp(stage_name, "LLR"))
    {
        /** get inlif id*/
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, block, stage_name, NULL, "In_LIFs.0", &global_in_lif, 1));

        /*
         * 6. get qos propag_profile
         */
        *propag_profile = 0;
        if (global_in_lif < dnx_data_l3.rif.nof_rifs_get(unit))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, global_in_lif);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                                     propag_profile);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        }
        else
        {
            /** Get local lif. Error will be returned only in case of STRICT flag */
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_algo_lif_mapping_global_to_local_get
                                      (unit, DNX_ALGO_LIF_INGRESS, global_in_lif, &local_lif_info), _SHR_E_NOT_FOUND);
            local_in_lif = local_lif_info.local_lif;
            if (local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
            {
                local_in_lif_core = _SHR_CORE_ALL;
                SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                                (unit, local_in_lif, local_in_lif_core, local_lif_info.phy_table,
                                 &inlif_dbal_table_id, &inlif_dbal_result_type, NULL));

                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, inlif_dbal_table_id, &entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             inlif_dbal_result_type);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
                rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF,
                                                         INST_SINGLE, propag_profile);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            }
        }
        /*
         * 7. get qos propagation strength
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_PROPAG_PROFILE_MAPPING, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, *propag_profile);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INGRESS_APP_TYPE, qos_ingress_app_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_QOS_PROPAG_STRENGTH, INST_SINGLE, propag_strength));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_QOS_COMP_STRENGTH, INST_SINGLE, comp_strength));

        /*
         * 8. get lif-id
         */
        *port_lif_id = global_in_lif;
    }
    else
    {
        /*
         * 6. get qos propag_profile
         *    No this profile, skip
         */

        /*
         * 7. get qos initial strength
         */
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, block, stage_name, NULL, sig_port_strength, propag_strength, 1));

        /*
         * 8. get port-id
         */
#ifndef ADAPTER_SERVER_MODE
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, block, NULL, stage_name, "In_Port", port_lif_id, 1));
#else
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, block, stage_name, NULL, "In_Port", port_lif_id, 1));
#endif
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Translate the propgation type to string.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] qos_propag_type - QoS propagation type. See dnx_qos_propagation_type_e.
 *   \param [out] propag_model_name - QoS propagation name related to the type.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_ingress_propag_type_string(
    int unit,
    dnx_qos_propagation_type_e qos_propag_type,
    char *propag_model_name)
{
    char *model_name;

    SHR_FUNC_INIT_VARS(unit);

    switch (qos_propag_type)
    {
        case DNX_QOS_PROPAGATION_TYPE_UNIFORM:
            model_name = "UNIFORM";
            break;
        case DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE:
            model_name = "SHORT_PIPE";
            break;
        case DNX_QOS_PROPAGATION_TYPE_STUCK:
            model_name = "STUCK";
            break;
        case DNX_QOS_PROPAGATION_TYPE_PIPE:
            model_name = "PIPE";
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "qos propagation type %d is not expected", qos_propag_type);
    }

    if (propag_model_name != NULL)
    {
        sal_strncpy(propag_model_name, model_name, RHNAME_MAX_SIZE - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - prepare ingress qos propagation info
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] stage_name - stage name; (LLR,VTTx (x=1..5) is supported.)
 *   \param [in,out] prt_ctr - print controler.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_ingress_propag_stage_prepare(
    int unit,
    int core,
    char *stage_name,
    prt_control_t * prt_ctr)
{
    uint32 fwd_layer_index, rm_layer_index, phb_layer_index;
    uint32 rm_map_profile, phb_map_profile, propag_profile;
    uint32 rm_propag_strength, rm_comp_strength, phb_propag_strength, phb_comp_strength;
    uint32 port_lif_id, port_width_hex;
    char fwd_layer_protocol[RHNAME_MAX_SIZE];
    char rm_layer_protocol[RHNAME_MAX_SIZE];
    char phb_layer_protocol[RHNAME_MAX_SIZE];
    char propag_model_name[RHNAME_MAX_SIZE];

    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    dnx_qos_propagation_type_e ttl_propagation_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    port_lif_id = DNX_PP_COS_VALUE_INVALID;
    fwd_layer_index = DNX_PP_COS_VALUE_INVALID;
    rm_layer_index = DNX_PP_COS_VALUE_INVALID;
    rm_map_profile = DNX_PP_COS_VALUE_INVALID;
    rm_propag_strength = DNX_PP_COS_VALUE_INVALID;
    rm_comp_strength = DNX_PP_COS_VALUE_INVALID;

    phb_layer_index = DNX_PP_COS_VALUE_INVALID;
    phb_map_profile = DNX_PP_COS_VALUE_INVALID;
    phb_propag_strength = DNX_PP_COS_VALUE_INVALID;
    phb_comp_strength = DNX_PP_COS_VALUE_INVALID;

    propag_profile = DNX_PP_COS_VALUE_INVALID;

    /** get ingress qos propagation info per stage */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_propag_info_get(unit, core, "remark", stage_name,
                                                          &port_lif_id,
                                                          &fwd_layer_index,
                                                          fwd_layer_protocol,
                                                          &rm_layer_index,
                                                          rm_layer_protocol,
                                                          &rm_map_profile,
                                                          &propag_profile, &rm_propag_strength, &rm_comp_strength));
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_propag_info_get(unit, core, "phb", stage_name,
                                                          &port_lif_id,
                                                          &fwd_layer_index,
                                                          fwd_layer_protocol,
                                                          &phb_layer_index,
                                                          phb_layer_protocol,
                                                          &phb_map_profile,
                                                          &propag_profile, &phb_propag_strength, &phb_comp_strength));

    if (propag_profile != DNX_PP_COS_VALUE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get(unit, propag_profile,
                                                                     &phb_propagation_type,
                                                                     &remark_propagation_type,
                                                                     &ecn_propagation_type, &ttl_propagation_type));
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    /** 1. stage name */
    PRT_CELL_SET("%s", stage_name);

    /** 2. port/LIF ID */
    if (!sal_strcasecmp(stage_name, "LLR"))
    {
        port_width_hex = (utilex_log2_round_up(dnx_data_port.general.nof_pp_ports_get(unit)) + 3) / 4;
        PRT_CELL_SET("0x%0*X", port_width_hex, port_lif_id);
    }
    else
    {
        PRT_CELL_SET("0x%06X", port_lif_id);
    }

    /** 3. propag profile*/
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, propag_profile, 1, prt_ctr,
                                                 NORMAL, DBAL_TABLE_EMPTY, DBAL_FIELD_EMPTY));

    /** 4. fwd_layer_index */
    _PRINT_CELL_SET_LAYER_HEADER(fwd_layer_index, fwd_layer_protocol);

    /** segregation Column */
    PRT_CELL_SET("%s", "");

    /*
     * rm propa info
     */
    /** 5. rm_layer_index */
    _PRINT_CELL_SET_LAYER_HEADER(rm_layer_index, rm_layer_protocol);

    /** 6. rm_qos_profile */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, rm_map_profile, 3, prt_ctr,
                                                 NORMAL, DBAL_TABLE_EMPTY, DBAL_FIELD_EMPTY));
    if (!sal_strcasecmp(stage_name, "LLR"))
    {
        /** 7. rm propag model*/
        PRT_CELL_SET("%s", DNX_PP_COS_PRINT_NA_STRING);

        /** 8. rm propag stength*/
        PRT_CELL_SET("(%s, %d)", DNX_PP_COS_PRINT_NA_STRING, rm_propag_strength);
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_propag_type_string(unit, remark_propagation_type, propag_model_name));

        /** 7. rm propag model*/
        PRT_CELL_SET("%s(0x%X)", propag_model_name, remark_propagation_type);

        /** 8. rm propag stength*/
        PRT_CELL_SET("(%d, %d)", rm_comp_strength, rm_propag_strength);
    }

    /** segregation Column */
    PRT_CELL_SET("%s", "");

    /*
     * phb propa info
     */
    /** 9. phb_layer_index */
    _PRINT_CELL_SET_LAYER_HEADER(phb_layer_index, phb_layer_protocol);

    /** 10. phb_qos_profile */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, phb_map_profile, 3, prt_ctr,
                                                 NORMAL, DBAL_TABLE_EMPTY, DBAL_FIELD_EMPTY));
    if (!sal_strcasecmp(stage_name, "LLR"))
    {
        /** 11. phb propag model*/
        PRT_CELL_SET("%s", DNX_PP_COS_PRINT_NA_STRING);

        /** 12. phb propag stength*/
        PRT_CELL_SET("(%s, %d)", DNX_PP_COS_PRINT_NA_STRING, phb_propag_strength);
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_propag_type_string(unit, phb_propagation_type, propag_model_name));

        /** 11. phb propag model*/
        PRT_CELL_SET("%s(0x%X)", propag_model_name, phb_propagation_type);

        /** 12. phb propag stength*/
        PRT_CELL_SET("(%d, %d)", phb_comp_strength, phb_propag_strength);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - print ingress qos propagation process
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] sand_control - according to diag mechanism definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_ingress_qos_propag_print(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    int stage_idx;
    char *stage_list_prt[] = { "LLR", "VTT1", "VTT2", "VTT3", "VTT4", "VTT5", "" };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Output in table format */
    PRT_TITLE_SET("Ingress qos propagation process through core_%d", core);
    PRT_INFO_ADD("  It dispalys the propagation result of each stage. propag-strength = (comp, propag).");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "port/LIF");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "propag_profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "fwd_layer_index");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "rm_layer_index");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "rm_map_profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "rm propag model");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "rm propag strength");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "phb_layer_index");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "phb_map_profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "phb propag model");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "phb propag strength");

    /** print the table */
    for (stage_idx = 0; !ISEMPTY(stage_list_prt[stage_idx]); stage_idx++)
    {
        SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_propag_stage_prepare(unit, core, stage_list_prt[stage_idx], prt_ctr));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get egress qos mapping information for given stage.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] stage - stage id, see ekleap_stage_e.
 *   \param [in] shared_data - ETPS data storage.
 *   \param [out] in_qos_dp - input qos-dp to current stage.
 *   \param [out] qos_profile - remark-profile used in qos mapping.
 *   \param [out] qos_var_type - qos variable type.
 *   \param [out] opcode - opcode in current qos mapping.
 *   \param [out] remark_or_keep - remark or preserve the original qos value, 0: preserve.
 *   \param [out] qos_model - qos model.
 *   \param [out] nwk_qos - mapped qos value, used in remarking/marking in current stage.
 *   \param [out] updated_qos_dp - output qos-dp to next stage.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_egress_qos_info_get(
    int unit,
    int core,
    ekleap_stage_e stage,
    ekleap_shared_data_t * shared_data,
    uint32 *in_qos_dp,
    uint32 *qos_profile,
    uint32 *qos_var_type,
    uint32 *opcode,
    uint32 *remark_or_keep,
    uint32 *qos_model,
    uint32 *nwk_qos,
    uint32 *updated_qos_dp)
{
    signal_output_t *list_element = NULL;
    signal_output_t *etps_element = NULL;
    signal_output_t *etps_1_list_element, *etps_2_list_element;
    dbal_fields_e field_id;
    match_t match;
    ekleap_stage_info_t stage_info, pre_stage_info, next_stage_info;
    char *pre_stage_name, *cur_stage_name, *next_stage_name;
    uint32 curr_qos_var_type;
    dbal_enum_value_field_egress_fwd_code_e fwd_code;
    uint32 entry_handle_id;
    uint32 context;
    uint32 dbal_context;
    uint32 dbal_enum_fwd_code;
    uint32 dp, in_nwk_qos, nwk_qos_idx, ecn_enable = 0, ftmh_cni = 0;
    uint8 use_etps = 0, nwk_qos_preserve = 0;
    uint8 fwd_code_remark_or_keep = TRUE, fwd_code_ecn_is_eligible, fwd_code_ecn_keep, remark_profile_remark_or_keep;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((stage < EKLEAP_STAGE_PRP2) || (stage > EKLEAP_STAGE_TRAP))
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "pp vis cos doesn't support the stage(%d)\n", stage);
    }

    sal_memset(&stage_info, 0, sizeof(stage_info));
    SHR_IF_ERR_EXIT(ekleap_stage_info_get(unit, stage, &stage_info));
    cur_stage_name = stage_info.stage_name;

    sal_memset(&next_stage_info, 0, sizeof(next_stage_info));
    SHR_IF_ERR_EXIT(ekleap_stage_info_get(unit, stage + 1, &next_stage_info));
    next_stage_name = next_stage_info.stage_name;
    if (stage == EKLEAP_STAGE_PRP2)
    {
        /*
         * skip 1,2,3,4,5,6
         * 7. get nwk_qos signal and its parsing
         */
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                            (unit, core, 0, "ETPP", cur_stage_name, next_stage_name, "Nwk_QOS", &list_element),
                            "Signal:nwk_qos not found\n");
        *nwk_qos = list_element->value[0];

        /*
         * 8. get updated_nwk_qos signal and its parsing
         */
        /** get ftmh_dp and its parsing*/
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                            (unit, core, 0, "ETPP", "ETParser", "PRP2", "ftmh_dp", &list_element),
                            "Signal:ftmh_dp not found\n");
        dp = list_element->value[0];
        *updated_qos_dp = (dp << DNX_QOS_DP_OFFSET) | *nwk_qos;
        *nwk_qos = DNX_PP_COS_VALUE_INVALID;
    }
    else if (shared_data->shifted_etps < shared_data->etps_data.entries_nof)
    {
        sal_memset(&pre_stage_info, 0, sizeof(pre_stage_info));
        SHR_IF_ERR_EXIT(ekleap_stage_info_get(unit, stage - 1, &pre_stage_info));
        pre_stage_name = pre_stage_info.stage_name;

        /** get ftmh_ecn_enable and ftmh_cni*/
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                            (unit, core, 0, "ETPP", "ETParser", "PRP2", "ftmh_ecn_enable", &list_element),
                            "Signal:ecn_enable not found\n");
        ecn_enable = list_element->value[0];
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                            (unit, core, 0, "ETPP", "ETParser", "PRP2", "ftmh_cni", &list_element),
                            "Signal:ftmh_cni not found\n");
        ftmh_cni = list_element->value[0];

        if (stage == EKLEAP_STAGE_FWD)
        {
            /*
             * 1. get in_qos_dp
             */
            /** get ftmh_dp and its parsing*/
            SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                                (unit, core, 0, "ETPP", pre_stage_name, cur_stage_name, "ftmh_dp", &list_element),
                                "Signal:ftmh_dp not found\n");
            dp = list_element->value[0];
            /** get network_qos and its parsing*/
            SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                                (unit, core, 0, "ETPP", pre_stage_name, cur_stage_name, "nwk_qos", &list_element),
                                "Signal:nwk_qos not found\n");
            in_nwk_qos = list_element->value[0];
            *in_qos_dp = (dp << DNX_QOS_DP_OFFSET) | in_nwk_qos;

            /*
             * 2. get qos_var_type
             */
            /** get forward_code and its parsing*/
            memset(&match, 0, sizeof(match_t));
            match.block = "ETPP";
            match.from = "PEM";
            match.to = "TERM";
            match.name = "forward_code";
            match.flags = SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_EXACT | SIGNALS_MATCH_RETRY |
                SIGNALS_MATCH_PEM | SIGNALS_MATCH_LEAF;
            if (sand_signal_get(unit, core, &match, NULL, &list_element) != _SHR_E_NONE)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            sal_memcpy(&fwd_code, list_element->value, BITS2WORDS(list_element->size) * 4);

            /** get dbal enum value of egress fwd code*/
            SHR_IF_ERR_EXIT_NO_MSG(dbal_fields_enum_value_get
                                   (unit, DBAL_FIELD_EGRESS_FWD_CODE, fwd_code, &dbal_enum_fwd_code));

            /** read table: QOS_EGRESS_MAP_TYPE_SELECT*/
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_MAP_TYPE_SELECT, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_ECN_ELIGIBLE, ecn_enable);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, dbal_enum_fwd_code);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

            /** get qos_var_type*/
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ETPP_QOS_VAR_TYPE, INST_SINGLE, qos_var_type));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE,
                             &fwd_code_remark_or_keep));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, entry_handle_id, DBAL_FIELD_ECN_IS_ELIGIBLE, INST_SINGLE,
                             &fwd_code_ecn_is_eligible));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, entry_handle_id, DBAL_FIELD_KEEP_ECN_BITS, INST_SINGLE, &fwd_code_ecn_keep));

            /*
             * 3. get remark_profile from etps_1
             */
            /** get the context and its parsing*/
            SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, "ETPP", pre_stage_name, cur_stage_name,
                                                        "context", &list_element), "Signal:context not found\n");
            context = list_element->value[0];
            SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_FWD_CONTEXT_ID, context, &dbal_context));

            /** read context table to check if ETPS was used */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, stage_info.ctx_table_id, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, stage_info.ctx_field_id, dbal_context);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, entry_handle_id, DBAL_FIELD_USE_ETPS_TOS, INST_SINGLE, &use_etps));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, stage_info.etps_shift_field_id, INST_SINGLE,
                             &(stage_info.etps_shift)));
            if (shared_data->shifted_etps + stage_info.etps_shift > shared_data->etps_data.entries_nof)
            {
                SHR_CLI_EXIT(_SHR_E_INTERNAL,
                             "stage(%d), mismatch between nof etps_entries(%d) and context etps_shift(%d)\n", stage,
                             shared_data->etps_data.entries_nof, shared_data->shifted_etps);
            }

            etps_element = NULL;
            if (use_etps)
            {
                _GET_ETPS_ENTRY_BY_ID(core, shared_data, shared_data->shifted_etps, etps_1_list_element);
                etps_element = utilex_rhlist_entry_get_by_name(etps_1_list_element->field_list, "REMARK_PROFILE");
            }

            if (etps_element != NULL)
            {
                *qos_profile = etps_element->value[0];
            }
            else
            {
                /** get remark_profile from port*/
                SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core, "ETPP", cur_stage_name, next_stage_name,
                                              "Editing_per_Port_Attributes.Remark_Profile", qos_profile, 1));
            }

            /*
             * 4. get qos_model from etps_1
             */
            etps_element = NULL;
            if (use_etps)
            {
                _GET_ETPS_ENTRY_BY_ID(core, shared_data, shared_data->shifted_etps, etps_1_list_element);
                etps_element = utilex_rhlist_entry_get_by_name(etps_1_list_element->field_list, "FORWARD_QOS_MODEL");
            }
            if (etps_element != NULL)
            {
                *qos_model = etps_element->value[0];
            }
            else
            {
                *qos_model = 0;
            }

            /** get nwk_qos_idx from etps_1 */
            etps_element = NULL;
            if (use_etps)
            {
                _GET_ETPS_ENTRY_BY_ID(core, shared_data, shared_data->shifted_etps, etps_1_list_element);
                etps_element = utilex_rhlist_entry_get_by_name(etps_1_list_element->field_list, "NWK_QOS_IDX");
            }

            if (etps_element != NULL)
            {
                nwk_qos_idx = etps_element->value[0];
            }
            else
            {
                nwk_qos_idx = 0;
            }
        }
        else
        {
            /** stage == PP_COS_EGRESS_STAGE_ENCx*/
            uint32 remark_profile_index = 0;
            /*
             * 1. get in_qos_dp
             */
            /** get network_qos and its parsing*/
            SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, "ETPP", pre_stage_name, cur_stage_name,
                                                        "Updated_QOS_and_DP", &list_element),
                                "Signal:updataed_qos_and_dp not found\n");
            *in_qos_dp = list_element->value[0];

            /*
             * 2. get qos_var_type
             */
            /** get the context and its parsing*/
            SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, "ETPP", pre_stage_name, cur_stage_name,
                                                        "context", &list_element), "Signal:context not found\n");
            context = list_element->value[0];

            switch (stage)
            {
                case EKLEAP_STAGE_ENC1:
                {
                    field_id = DBAL_FIELD_ENCAP_1_CONTEXT_ID;
                    break;
                }
                case EKLEAP_STAGE_ENC2:
                {
                    field_id = DBAL_FIELD_ENCAP_2_CONTEXT_ID;
                    break;
                }
                case EKLEAP_STAGE_ENC3:
                {
                    field_id = DBAL_FIELD_ENCAP_3_CONTEXT_ID;
                    break;
                }
                case EKLEAP_STAGE_ENC4:
                {
                    field_id = DBAL_FIELD_ENCAP_4_CONTEXT_ID;
                    break;
                }
                case EKLEAP_STAGE_ENC5:
                {
                    field_id = DBAL_FIELD_ENCAP_5_CONTEXT_ID;
                    break;
                }
                default:
                    SHR_EXIT();
            }

            SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, field_id, context, &dbal_context));
            if (((stage == EKLEAP_STAGE_ENC1) && (dbal_context == DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___NOP_CTX))
                || ((stage == EKLEAP_STAGE_ENC2)
                    && (dbal_context == DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___NOP_CTX))
                || ((stage == EKLEAP_STAGE_ENC3)
                    && (dbal_context == DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___NOP_CTX))
                || ((stage == EKLEAP_STAGE_ENC4)
                    && (dbal_context == DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_GENERAL___NOP_CTX))
                || ((stage == EKLEAP_STAGE_ENC5)
                    && (dbal_context == DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_GENERAL___NOP_CTX)))
            {
                /**if context is NOP, do nothing*/
                SHR_EXIT();
            }

            /** read the context table*/
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, stage_info.ctx_table_id, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, stage_info.ctx_field_id, dbal_context);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            /** get qos_var_type*/
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_QOS_VAR_TYPE, INST_SINGLE, &curr_qos_var_type));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE_INDEX, INST_SINGLE,
                             &remark_profile_index));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, stage_info.etps_shift_field_id, INST_SINGLE,
                             &(stage_info.etps_shift)));
            if (shared_data->shifted_etps + stage_info.etps_shift > shared_data->etps_data.entries_nof)
            {
                SHR_CLI_EXIT(_SHR_E_INTERNAL,
                             "stage(%d), mismatch between nof etps_entries(%d) and context etps_shift(%d)\n", stage,
                             shared_data->etps_data.entries_nof, shared_data->shifted_etps);
            }
            if (dnx_data_qos.qos.feature_get(unit, dnx_data_qos_qos_qos_var_type_map_enable))
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ENC_QOS_VAR_TYPE_MAPPING_TABLE, entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_IS_ECN_ELIGIBLE, ecn_enable);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_VAR_TYPE_CURRENT, curr_qos_var_type);
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENC_QOS_VAR_TYPE_NEW, INST_SINGLE,
                                           qos_var_type);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            }
            else
            {
                *qos_var_type = curr_qos_var_type;
            }

            /*
             * 3. get remark_profile from etps_1/2/3
             */
            *qos_profile = 0;
            etps_element = NULL;
            _GET_ETPS_ENTRY_BY_ID(core, shared_data, shared_data->shifted_etps + remark_profile_index,
                                  etps_1_list_element);
            etps_element = utilex_rhlist_entry_get_by_name(etps_1_list_element->field_list, "REMARK_PROFILE");
            if (etps_element != NULL)
            {
                *qos_profile = etps_element->value[0];
            }

            /*
             * 4. get qos_model from etps_1/2
             */
            etps_element = NULL;
            *qos_model = 0;
            _GET_ETPS_ENTRY_BY_ID(core, shared_data, shared_data->shifted_etps, etps_1_list_element);
            etps_element = utilex_rhlist_entry_get_by_name(etps_1_list_element->field_list, "ENCAP_QOS_MODEL");

            if (etps_element == NULL)
            {
                etps_2_list_element = shared_data->etps_data.entry_signals[shared_data->shifted_etps + 1].entry_sig;
                if (etps_2_list_element != NULL)
                {
                    etps_element = utilex_rhlist_entry_get_by_name(etps_2_list_element->field_list, "ENCAP_QOS_MODEL");
                }
            }
            if (etps_element != NULL)
            {
                *qos_model = etps_element->value[0];
            }

            /** get nwk_qos_idx from etps_1/2 */
            _GET_ETPS_ENTRY_BY_ID(core, shared_data, shared_data->shifted_etps, etps_1_list_element);
            etps_element = utilex_rhlist_entry_get_by_name(etps_1_list_element->field_list, "NWK_QOS_IDX");

            if (etps_element == NULL)
            {
                etps_2_list_element = shared_data->etps_data.entry_signals[shared_data->shifted_etps + 1].entry_sig;
                if (etps_2_list_element != NULL)
                {
                    etps_element = utilex_rhlist_entry_get_by_name(etps_2_list_element->field_list, "NWK_QOS_IDX");
                }
            }
            if (etps_element != NULL)
            {
                nwk_qos_idx = etps_element->value[0];
            }
            else
            {
                nwk_qos_idx = 0;
            }
        }

        /*
         * 5. get opcode
         */
         /** Read table: QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_QOS_VAR_TYPE, *qos_var_type);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, *qos_profile);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, INST_SINGLE, opcode));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE,
                         &remark_profile_remark_or_keep));

        /*
         * 6. get remark_or_keep
         */
        *remark_or_keep = 1;
        if ((stage == EKLEAP_STAGE_FWD) &&
            (dnx_data_qos.feature.feature_get(unit, dnx_data_qos_feature_egress_remark_or_keep_enabler)))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_QOS_EGRESS_FWD_HEADER_REMARK_ENABLER, entry_handle_id));
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE_REMARK_OR_PRESERVE,
                                      fwd_code_remark_or_keep);
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE_REMARK_OR_PRESERVE,
                                      remark_profile_remark_or_keep);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE, remark_or_keep));
        }
        else
        {
            *remark_or_keep = remark_profile_remark_or_keep & fwd_code_remark_or_keep;
        }

        /*
         * 7. get nwk_qos
         */
        /** read table: QOS_EGRESS_MODEL_MAPPING_TABLE to get nwk_qos selector*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_QOS_EGRESS_MODEL_MAPPING_TABLE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, *qos_model);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_IS_PRESERVE, INST_SINGLE, &nwk_qos_preserve));
        if (nwk_qos_preserve)
        {
            /** select nwk_qos in pipe mode, read table: QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE*/
            nwk_qos_idx = nwk_qos_idx & 0x3F;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                            (unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_PIPE_PROFILE, nwk_qos_idx);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_PIPE, INST_SINGLE, nwk_qos));
        }
        else
        {
            dp = (*in_qos_dp >> DNX_QOS_DP_OFFSET) & DNX_QOS_DP_MASK;
            in_nwk_qos = *in_qos_dp & DNX_QOS_TOS_MASK;

            /** select nwk_qos in uniform mode, read table: QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, in_nwk_qos);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, *opcode);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_CURRENT_LAYER_NETWORK_QOS, INST_SINGLE, nwk_qos));
        }

        /** in case ECN enabled, nwk_qos comes from opcode b'xxx00*/
        *nwk_qos |= ((ftmh_cni << 1) | ecn_enable);

        /*
         * 8. get updated_qos_dp
         */
        if (stage != EKLEAP_STAGE_ENC5)
        {
            SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, "ETPP", cur_stage_name, next_stage_name,
                                                        "Updated_QOS_and_DP", &list_element),
                                "Signal:updated_qos_and_dp not found\n");
            *updated_qos_dp = list_element->value[0];
        }

        /** update etps index for next stage.*/
        shared_data->shifted_etps += stage_info.etps_shift;
    }

exit:
    sand_signal_output_free(list_element);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Prepare qos mapping information for given stage
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] stage - stage id, see ekleap_stage_e.
 *   \param [in] shared_data - ETPS data storage.
 *   \param [in,out] prt_ctr - print controler.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_egress_stage_prepare(
    int unit,
    int core,
    ekleap_stage_e stage,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{
    uint32 in_qos_dp, qos_profile, opcode, nwk_qos, updated_qos_dp;
    dbal_enum_value_field_etpp_qos_var_type_e qos_var_type;
    dbal_enum_value_field_remark_or_preserve_e remark_or_keep;
    dbal_enum_value_field_qos_model_e qos_model;
    dbal_fields_e qos_model_field;
    ekleap_stage_info_t stage_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    in_qos_dp = DNX_PP_COS_VALUE_INVALID;
    qos_profile = DNX_PP_COS_VALUE_INVALID;
    qos_var_type = DNX_PP_COS_VALUE_INVALID;
    remark_or_keep = DNX_PP_COS_VALUE_INVALID;
    qos_model = DNX_PP_COS_VALUE_INVALID;
    opcode = DNX_PP_COS_VALUE_INVALID;
    nwk_qos = DNX_PP_COS_VALUE_INVALID;
    updated_qos_dp = DNX_PP_COS_VALUE_INVALID;

    sal_memset(&stage_info, 0, sizeof(stage_info));
    SHR_IF_ERR_EXIT(ekleap_stage_info_get(unit, stage, &stage_info));

    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_egress_qos_info_get(unit, core, stage, shared_data,
                                                      &in_qos_dp, &qos_profile, &qos_var_type,
                                                      &opcode, &remark_or_keep, &qos_model, &nwk_qos, &updated_qos_dp));

    qos_model_field = DBAL_FIELD_ENCAP_QOS_MODEL;
    if (stage == EKLEAP_STAGE_FWD)
    {
        qos_model_field = DBAL_FIELD_FORWARD_QOS_MODEL;
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    /** CELL 0: stage name */
    if (!sal_strcasecmp(stage_info.stage_name, "Prp2"))
    {
        PRT_CELL_SET("%s", "Initial");
    }
    else
    {
        PRT_CELL_SET("%s", stage_info.stage_name);
    }

    /** CELL 1: input qos_dp */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, in_qos_dp, DNX_PP_COS_PRINT_WIDTH_NONE, prt_ctr, QOS_DP,
                                                 DBAL_TABLE_EMPTY, DBAL_FIELD_EMPTY));

    /** CELL 2: qos_profile */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, qos_profile, DNX_PP_COS_PRINT_WIDTH_QOS_PROFILE, prt_ctr, NORMAL,
                                                 DBAL_TABLE_EMPTY, DBAL_FIELD_EMPTY));

    /** CELL 3: qos_model */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, qos_model, DNX_PP_COS_PRINT_WIDTH_NONE, prt_ctr, ENUM,
                                                 DBAL_TABLE_EEDB_MPLS_TUNNEL, qos_model_field));

    /** CELL 4: qos_var_type */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, qos_var_type, DNX_PP_COS_PRINT_WIDTH_NONE, prt_ctr, ENUM,
                                                 DBAL_TABLE_QOS_FWD_LAYER_TYPE_SELECT, DBAL_FIELD_FORWARD_TYPE));

    /** CELL 5: opcode */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, opcode, DNX_PP_COS_PRINT_WIDTH_OPCODE, prt_ctr, NORMAL,
                                                 DBAL_TABLE_EMPTY, DBAL_FIELD_EMPTY));

    /** CELL 6: remark_or_keep */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, remark_or_keep, DNX_PP_COS_PRINT_WIDTH_NONE, prt_ctr, ENUM,
                                                 DBAL_TABLE_QOS_EGRESS_MAP_TYPE_SELECT, DBAL_FIELD_REMARK_OR_PRESERVE));

    /** CELL 7: nwk_qos */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, nwk_qos, DNX_PP_COS_PRINT_WIDTH_QOS_VAR, prt_ctr, NORMAL,
                                                 DBAL_TABLE_EMPTY, DBAL_FIELD_EMPTY));

    /** CELL 8: output qos_dp */
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_print_cell_set(unit, updated_qos_dp, DNX_PP_COS_PRINT_WIDTH_NONE, prt_ctr, QOS_DP,
                                                 DBAL_TABLE_EMPTY, DBAL_FIELD_EMPTY));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - print egress qos mapping information per stage
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [in] sand_control - according to diag mechanism definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_cos_egress_qos_info_print(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    ekleap_shared_data_t shared_data;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&shared_data, 0, sizeof(ekleap_shared_data_t));
    SHR_CLI_EXIT_IF_ERR(ekleap_shared_data_get(unit, core, &shared_data), "Could not read Encap DB entries\n");

    /** Output in table format */
    PRT_TITLE_SET("Egress qos mapping info through core_%d", core);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "input\n(dp,qos_var)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "remark_profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "qos_model");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "qos_var_type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "opcode");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "remark_or_preserve");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "qos_var");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "next layer\n(dp,qos_var)");

    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_egress_stage_prepare(unit, core, EKLEAP_STAGE_PRP2, &shared_data, prt_ctr));
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_egress_stage_prepare(unit, core, EKLEAP_STAGE_FWD, &shared_data, prt_ctr));
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_egress_stage_prepare(unit, core, EKLEAP_STAGE_ENC1, &shared_data, prt_ctr));
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_egress_stage_prepare(unit, core, EKLEAP_STAGE_ENC2, &shared_data, prt_ctr));
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_egress_stage_prepare(unit, core, EKLEAP_STAGE_ENC3, &shared_data, prt_ctr));
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_egress_stage_prepare(unit, core, EKLEAP_STAGE_ENC4, &shared_data, prt_ctr));
    SHR_IF_ERR_EXIT(sh_dnx_pp_cos_egress_stage_prepare(unit, core, EKLEAP_STAGE_ENC5, &shared_data, prt_ctr));

    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - main function of ClassOfService
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_cos_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_in, core_id;
    char *pipe = NULL, *print_content = NULL, *qos_type = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** determine if options were given*/
    SH_SAND_GET_INT32("core", core_in);
    SH_SAND_GET_STR("type", print_content);
    SH_SAND_GET_STR("pipe", pipe);

    if (!(strcaseindex(pipe, "ing") ||
          strcaseindex(pipe, "egr") ||
          strcaseindex(pipe, "both")) ||
        !(strcaseindex(print_content, "map") ||
          strcaseindex(print_content, "prop") || strcaseindex(print_content, "all")))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "pp vis cos unsupported options,type : %s, pipe %s\n", print_content, pipe);
    }

    /** Print remark, phb qos mapping info, valid for ingress only*/
    qos_type = "ReMark|PHB";

    /** Print ingress qos mapping info if required.*/
    if (strcaseindex(pipe, "ing") || strcaseindex(pipe, "both"))
    {
        SH_DNX_CORES_ITER(unit, core_in, core_id)
        {
            /** check if core_id is valid */
            DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, TRUE);
            /** print ingress qos propagation process*/
            if (strcaseindex(print_content, "prop") || strcaseindex(print_content, "all"))
            {
                SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_qos_propag_print(unit, core_id, sand_control));
            }

            if (strcaseindex(print_content, "map") || strcaseindex(print_content, "all"))
            {
                SHR_IF_ERR_EXIT(sh_dnx_pp_cos_ingress_qos_info_print(unit, core_id, qos_type, sand_control));
            }
        }
    }

    /** Print egress qos mapping info if required.*/
    if (strcaseindex(pipe, "egr") || strcaseindex(pipe, "both"))
    {
        SH_DNX_CORES_ITER(unit, core_in, core_id)
        {
            /** check if core_id is valid */
            DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_ETPP, TRUE);
            if (strcaseindex(print_content, "map") || strcaseindex(print_content, "all"))
            {
                SHR_IF_ERR_EXIT(sh_dnx_pp_cos_egress_qos_info_print(unit, core_id, sand_control));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_pp_cos_options[] = {
    {"type", SAL_FIELD_TYPE_STR, "map, propagation or all", "map"}
    ,
    {"pipe", SAL_FIELD_TYPE_STR, "ingress, egress or both", "both"}
    ,
    {NULL}
};

sh_sand_man_t dnx_pp_cos_man = {
    .brief = "PP VISibility ClassOfService(cos) diagnostics",
    .full = "Show Qos mapping information in ingress and/or egress pipeline.\n",
    .init_cb = diag_sand_packet_send_simple,
    .synopsis = "[TYpe=PROPagation|MAP|ALL] [PIPE=INGress|EGRess|BOTH]",
    .examples = "\n" "TYpe=PROPagation\n" "PIPE=INGress\n" "PIPE=BOTH CORE=0"
};
