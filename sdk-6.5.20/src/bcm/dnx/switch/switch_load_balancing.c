
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LB

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <bcm/switch.h>
#include <sal/appl/sal.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/switch/switch_load_balancing.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <src/bcm/dnx/init/init_pp.h>



#define SWITCH_LB_MPLS_LABEL_FULL_MASK SAL_UPTO_BIT(20)


#define SWITCH_LB_MPLS_EL_LABEL_INCLUDED_IN_HASH 0x80

#define SWITCH_MAX_HEADER_HASH_ENABLE_FLAGS 12
 
#define SWITCH_MAX_DBAL_FIELDS_FOR_HASH_FLAG 6





#define SWITCH_IS_FLAGS_SET(flags,flags_bits)  (((flags & flags_bits) == flags_bits) ? TRUE : FALSE)



typedef struct
{
    
    uint32 flag;
    
    uint32 nof_fields;
    
    dbal_fields_e flag_fields[SWITCH_MAX_DBAL_FIELDS_FOR_HASH_FLAG];

} dnx_switch_hash_flag_attr_t;


typedef struct
{
    
    dbal_tables_e dbal_table;
    
    uint32 symmetrical_flag;
    
    int nof_field_flags;
    
    dbal_enum_value_field_hash_field_enablers_header_e field_enablers_header_id;
    
    dnx_switch_hash_flag_attr_t field_flags[SWITCH_MAX_HEADER_HASH_ENABLE_FLAGS];

} dnx_switch_hash_header_attr_t;


static shr_error_e
dnx_switch_control_load_balancing_hash_fields_flags_attr_get(
    int unit,
    bcm_switch_control_t type,
    dnx_switch_hash_header_attr_t * header_info)
{
    int field_counter = 0;
    uint8 is_outer;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(header_info, 0, sizeof(dnx_switch_hash_header_attr_t));

    header_info->dbal_table = DBAL_TABLE_PEMLA_PARSERLOADBALANCING;

    switch (type)
    {
        case bcmSwitchHashIP4OuterField:
        case bcmSwitchHashIP4InnerField:
        {
            header_info->symmetrical_flag = BCM_HASH_FIELD_IPV4_ADDRESS;
            is_outer = (type == bcmSwitchHashIP4OuterField);
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L3IPV4OUTERIPADDRESSENABLE : DBAL_FIELD_L3IPV4INNERIPADDRESSENABLE;
            header_info->field_flags[field_counter++].flag = BCM_HASH_NON_SYMMETRICAL;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L3IPV4OUTERSYMMETRIC : DBAL_FIELD_L3IPV4INNERSYMMETRIC;
            header_info->field_flags[field_counter++].flag = BCM_HASH_FIELD_IPV4_ADDRESS;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L3IPV4OUTERPROTOCOLENABLE : DBAL_FIELD_L3IPV4INNERPROTOCOLENABLE;
            header_info->field_flags[field_counter++].flag = BCM_HASH_FIELD_PROTOCOL;
            header_info->field_enablers_header_id =
                is_outer ? DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP4_OUTER :
                DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP4_INNER;
            break;
        }
        case bcmSwitchHashIP6OuterField:
        case bcmSwitchHashIP6InnerField:
        {
            is_outer = (type == bcmSwitchHashIP6OuterField);
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L3IPV6OUTERIPADDRESSENABLE : DBAL_FIELD_L3IPV6INNERIPADDRESSENABLE;
            header_info->field_flags[field_counter++].flag = BCM_HASH_FIELD_IPV6_ADDRESS;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L3IPV6OUTERFLOWLABELENABLE : DBAL_FIELD_L3IPV6INNERFLOWLABELENABLE;
            header_info->field_flags[field_counter++].flag = BCM_HASH_FIELD_FLOW_LABEL;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L3IPV6OUTERNEXTHEADERENABLE : DBAL_FIELD_L3IPV6INNERNEXTHEADERENABLE;
            header_info->field_flags[field_counter++].flag = BCM_HASH_FIELD_NXT_HDR;
            header_info->field_enablers_header_id =
                is_outer ? DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP6_OUTER :
                DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP6_INNER;
            break;
        }
        case bcmSwitchHashL2OuterField:
        case bcmSwitchHashL2InnerField:
        {
            header_info->symmetrical_flag = BCM_HASH_FIELD_MAC_ADDRESS;
            is_outer = (type == bcmSwitchHashL2OuterField);
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L2ETHOUTERMACADDRESSENABLE : DBAL_FIELD_L2ETHINNERMACADDRESSENABLE;
            header_info->field_flags[field_counter++].flag = BCM_HASH_NON_SYMMETRICAL;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L2ETHOUTERSYMMETRIC : DBAL_FIELD_L2ETHINNERSYMMETRIC;
            header_info->field_flags[field_counter++].flag = BCM_HASH_FIELD_MAC_ADDRESS;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L2ETHOUTERETENABLE : DBAL_FIELD_L2ETHINNERETENABLE;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L2ETHOUTER1STVLANETENABLE : DBAL_FIELD_L2ETHINNER1STVLANETENABLE;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L2ETHOUTER2NDVLANETENABLE : DBAL_FIELD_L2ETHINNER2NDVLANETENABLE;
            header_info->field_flags[field_counter++].flag = BCM_HASH_FIELD_ETHER_TYPE;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L2ETHOUTER1STVLANVIDENABLE : DBAL_FIELD_L2ETHINNER1STVLANVIDENABLE;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                is_outer ? DBAL_FIELD_L2ETHOUTER2NDVLANVIDENABLE : DBAL_FIELD_L2ETHINNER2NDVLANVIDENABLE;
            header_info->field_flags[field_counter++].flag = BCM_HASH_FIELD_VLAN;
            header_info->field_enablers_header_id =
                is_outer ? DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_ETH_OUTER :
                DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_ETH_INNER;
            break;
        }
        case bcmSwitchHashL4Field:
        {
            header_info->symmetrical_flag = BCM_HASH_FIELD_L4;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                DBAL_FIELD_L4TCPPORTSENABLE;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                DBAL_FIELD_L4UDPPORTSENABLE;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                DBAL_FIELD_L4SCTPPORTSENABLE;
            header_info->field_flags[field_counter++].flag = BCM_HASH_NON_SYMMETRICAL;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                DBAL_FIELD_L4SYMMETRIC;
            header_info->field_flags[field_counter++].flag = BCM_HASH_FIELD_L4;
            header_info->field_enablers_header_id = DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_L4;
            break;
        }
        case bcmSwitchHashMPLSField0:
        {
            header_info->dbal_table = DBAL_TABLE_PEMLA_LOADBALANACINGCFG;
            header_info->field_flags[field_counter].flag_fields[header_info->field_flags[field_counter].nof_fields++] =
                DBAL_FIELD_MPLS_LB_KEY;
            header_info->field_flags[field_counter++].flag = BCM_HASH_MPLS_ALL_LABELS;
            header_info->field_enablers_header_id = DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_MPLS;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported type for bcm_switch_control_set %d\n", type);
    }

    if (field_counter > SWITCH_MAX_HEADER_HASH_ENABLE_FLAGS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "NOF of fields is %d while the max number is %d\n", field_counter,
                     SWITCH_MAX_HEADER_HASH_ENABLE_FLAGS);
    }

    header_info->nof_field_flags = field_counter;

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ingress_load_balancing_layer_record_init(
    int unit)
{
    uint32 entry_handle_id;
    uint8 default_enabled = 1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_USE_LAYER_RECORD, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_LAYER_PROTOCOL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PORT_LB_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_LB_PROFILE, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_LAYER_RECORD, INST_SINGLE, default_enabled);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_load_balancing_module_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    if (dnx_data_switch.load_balancing.hashing_selection_per_layer_get(unit))
    {
        SHR_IF_ERR_EXIT(ingress_load_balancing_layer_record_init(unit));
    }
    {
        
        dnx_switch_lb_mpls_reserved_t mpls_reserved;

        mpls_reserved.reserved_current_label_indication =
            dnx_data_switch.load_balancing.initial_reserved_label_get(unit);
        mpls_reserved.reserved_current_label_force_layer =
            dnx_data_switch.load_balancing.initial_reserved_label_force_get(unit);
        mpls_reserved.reserved_next_label_indication =
            dnx_data_switch.load_balancing.initial_reserved_next_label_get(unit);
        mpls_reserved.reserved_next_label_valid = mpls_reserved.reserved_next_label_indication;
        SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_reserved_label_set(unit, &mpls_reserved));
    }
    {
        
        dnx_switch_lb_mpls_identification_t mpls_identification;

        mpls_identification.mpls_identification = 0;
        
        mpls_identification.mpls_identification |= SAL_BIT(DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS);
        mpls_identification.mpls_identification |=
            SAL_BIT(dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM));
        SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_identification_set(unit, &mpls_identification));
    }
    {
        
        uint32 phys_lb_client;
        dnx_switch_lb_tcam_key_t tcam_key;
        dnx_switch_lb_tcam_result_t tcam_result;
        dnx_switch_lb_valid_t tcam_valid;
        int index;

        index = 0;
        
        tcam_key.lb_destination = 0;
        tcam_key.lb_destination_mask = 0;
        tcam_key.outlif_0 = 0;
        tcam_key.outlif_0_mask = 0;

        for (phys_lb_client = 0; phys_lb_client < dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
             phys_lb_client++)
        {
            
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_crc_function_enum_to_hw_get
                            (unit,
                             dnx_data_switch.load_balancing.lb_client_crc_get(unit, phys_lb_client)->crc_hash_func,
                             &tcam_result.client_x_crc_16_sel[phys_lb_client]));

        }
        tcam_valid = 1;
        SHR_IF_ERR_EXIT(dnx_switch_load_balancing_tcam_info_set(unit, index, &tcam_key, &tcam_result, tcam_valid));
    }
    {
        
        bcm_switch_control_t headers[] = { bcmSwitchHashIP4OuterField, bcmSwitchHashIP4InnerField,
            bcmSwitchHashIP6OuterField, bcmSwitchHashIP6InnerField, bcmSwitchHashL2OuterField,
            bcmSwitchHashL2InnerField, bcmSwitchHashL4Field, bcmSwitchHashMPLSField0
        };
        dnx_switch_hash_header_attr_t header_info;
        int header_id;
        int nof_headers = sizeof(headers) / sizeof(headers[0]);

        for (header_id = 0; header_id < nof_headers; header_id++)
        {
            SHR_IF_ERR_EXIT(dnx_switch_control_load_balancing_hash_fields_flags_attr_get
                            (unit, headers[header_id], &header_info));
            switch_db.header_enablers_hashing.set(unit, header_info.field_enablers_header_id,
                                                  dnx_data_switch.load_balancing.lb_field_enablers_get(unit,
                                                                                                       header_info.field_enablers_header_id)->field_enablers_mask);
        }
    }

    if (dnx_data_switch.feature.feature_get(unit, dnx_data_switch_feature_mpls_labels_included_in_hash))
    {
        uint32 entry_handle_id;
        dbal_tables_e mpls_lb_tables[2] = { DBAL_TABLE_LOAD_BALANCING_MPLS_1ST_STACK_FORCE_LABELS_HL_TCAM,
            DBAL_TABLE_LOAD_BALANCING_MPLS_2ND_STACK_FORCE_LABELS_HL_TCAM
        };
        int idx, nof_tables = 2;
        for (idx = 0; idx < nof_tables; idx++)
        {
            
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, mpls_lb_tables[idx], &entry_handle_id));
            
            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, 0));

            
            dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LABEL, 0,
                                              SWITCH_LB_MPLS_LABEL_FULL_MASK);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_LABEL_INDICATION, INST_SINGLE, TRUE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_LABEL_FORCE_LAYER, INST_SINGLE,
                                         TRUE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LABEL_INDICATION, INST_SINGLE, FALSE);
            if (dnx_data_switch.load_balancing.mpls_cam_next_label_valid_field_exists_get(unit))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LABEL_VALID, INST_SINGLE, FALSE);
            }
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
            
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    else
    {
        
        uint32 entry_handle_id;
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_RESERVED_LABELS, &entry_handle_id));
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RSRVD_NEXT_LABEL_INDICATION, INST_SINGLE,
                                     SWITCH_LB_MPLS_EL_LABEL_INCLUDED_IN_HASH);
        
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_control_t_name(
    int unit,
    bcm_switch_control_t switch_control,
    char **name_p)
{
    
    static char *dnx_switch_control_names[] = {
        BCM_SWITCH_CONTROL_STR
    };
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name_p, _SHR_E_PARAM, "name_p");
    if (COUNTOF(dnx_switch_control_names) != bcmSwitch__Count)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error: NAMES ARRAY OUT OF SYNC (num elements on BCM_SWITCH_CONTROL_STR (%d) is not equal to num elements on bcm_switch_control_t (%d)\n",
                     COUNTOF(dnx_switch_control_names), bcmSwitch__Count);
    }
    if (bcmSwitch__Count <= switch_control)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Input switch control (%d) is higher than allowed (%d)\n", switch_control, bcmSwitch__Count);
    }
    *name_p = dnx_switch_control_names[switch_control];
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_control_t_value(
    int unit,
    char *switch_control_name_p,
    uint32 *value_p)
{
    uint32 switch_control_index;
    char *name_p;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(switch_control_name_p, _SHR_E_PARAM, "switch_control_name_p");
    SHR_NULL_CHECK(value_p, _SHR_E_PARAM, "value_p");
    
    *value_p = -1;
    
    for (switch_control_index = 0; switch_control_index < bcmSwitch__Count; switch_control_index++)
    {
        SHR_IF_ERR_EXIT(dnx_switch_control_t_name(unit, switch_control_index, &name_p));
        if (sal_strncasecmp(name_p, switch_control_name_p, SWITCH_CONTROL_NAME_LIMIT) == 0)
        {
            *value_p = switch_control_index;
            SHR_EXIT();
        }
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_lb_physical_client_to_logical(
    int unit,
    uint32 physical_client,
    bcm_switch_control_t * logical_client_p)
{
    bcm_switch_control_t logical_client;
    uint32 client_hw_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(logical_client_p, _SHR_E_PARAM, "logical_client_p");
    if (physical_client >= dnx_data_switch.load_balancing.nof_lb_clients_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "physical_client (%d) is out of range. Should be between %d and %d.\r\n",
                     physical_client, 0, dnx_data_switch.load_balancing.nof_lb_clients_get(unit) - 1);
    }
    client_hw_id = physical_client;
    logical_client = dnx_data_switch.load_balancing.lb_clients_get(unit, client_hw_id)->client_logical_id;
    *logical_client_p = logical_client;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_lb_logical_client_to_physical(
    int unit,
    bcm_switch_control_t logical_client_in,
    uint32 *physical_client_p)
{
    bcm_switch_control_t logical_client;
    uint32 client_hw_id, client_hw_id_max;
    int found;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(physical_client_p, _SHR_E_PARAM, "physical_client_p");
    client_hw_id_max = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
    found = FALSE;
    for (client_hw_id = 0; client_hw_id < client_hw_id_max; client_hw_id++)
    {
        logical_client = dnx_data_switch.load_balancing.lb_clients_get(unit, client_hw_id)->client_logical_id;
        if (logical_client == logical_client_in)
        {
            
            found = TRUE;
            break;
        }
    }
    if (found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "\r\n"
                     "Input logical_client (%d) is not marked as a load_balancing client. No match. Quit.\r\n",
                     logical_client_in);
    }
    
    *physical_client_p = client_hw_id;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_lb_tcam_key_t_init(
    int unit,
    dnx_switch_lb_tcam_key_t * tcam_key_p)
{
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(tcam_key_p, 0, sizeof(*tcam_key_p));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_lb_tcam_result_t_init(
    int unit,
    dnx_switch_lb_tcam_result_t * tcam_result_p)
{
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(tcam_result_p, 0, sizeof(*tcam_result_p));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_lsms_crc_select_set(
    int unit,
    dnx_switch_lb_lsms_crc_select_t * lsms_crc_select_p)
{
    uint32 entry_handle_id;
    uint32 max_client;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(lsms_crc_select_p, _SHR_E_PARAM, "lsms_crc_select_p");
    
    max_client = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
    if (lsms_crc_select_p->lb_client >= max_client)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Identifier of load balancing client on input (%d) is larger than max allowed (%d).\r\n"
                     "  This is illegal. Quit.\r\n", lsms_crc_select_p->lb_client, max_client - 1);
    }
    
    if ((lsms_crc_select_p->lb_selection_bit_map &
         SAL_FROM_BIT(dnx_data_switch.load_balancing.nof_layer_records_from_parser_get(unit))) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Bit selection input (0x%04X) extends beyond max bit allowed (%d).\r\n"
                     "  This is illegal. Quit.\r\n",
                     lsms_crc_select_p->lb_selection_bit_map,
                     dnx_data_switch.load_balancing.nof_layer_records_from_parser_get(unit));
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CLIET_FWD_BIT_SEL, &entry_handle_id));
    
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_LB_CLIENT, lsms_crc_select_p->lb_client);
    
    dbal_entry_value_field16_set(unit, entry_handle_id,
                                 DBAL_FIELD_LB_SELECTION_BIT_MAP, INST_SINGLE, lsms_crc_select_p->lb_selection_bit_map);
    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_lsms_crc_select_get(
    int unit,
    dnx_switch_lb_lsms_crc_select_t * lsms_crc_select_p)
{
    uint32 entry_handle_id;
    uint32 max_client;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(lsms_crc_select_p, _SHR_E_PARAM, "lsms_crc_select_p");
    
    max_client = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
    if (lsms_crc_select_p->lb_client >= max_client)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Identifier of load balancing client on input (%d) is larger than max allowed (%d).\r\n"
                     "  This is illegal. Quit.\r\n", lsms_crc_select_p->lb_client, max_client - 1);
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CLIET_FWD_BIT_SEL, &entry_handle_id));
    
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_LB_CLIENT, lsms_crc_select_p->lb_client);
    
    dbal_value_field16_request(unit, entry_handle_id,
                               DBAL_FIELD_LB_SELECTION_BIT_MAP, INST_SINGLE,
                               &(lsms_crc_select_p->lb_selection_bit_map));
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_crc_seed_set(
    int unit,
    uint32 crc_hw_function_index,
    uint16 lb_crc_seed)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CRC_SEEDS, &entry_handle_id));
    
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_LB_CRC_FUNCTION_INDEX, crc_hw_function_index);
    
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_LB_CRC_SEED, INST_SINGLE, lb_crc_seed);
    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_crc_seed_get(
    int unit,
    uint32 crc_hw_function_index,
    uint16 *lb_crc_seed)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CRC_SEEDS, &entry_handle_id));
    
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_LB_CRC_FUNCTION_INDEX, crc_hw_function_index);
    
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_LB_CRC_SEED, INST_SINGLE, lb_crc_seed);
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_mpls_identification_set(
    int unit,
    dnx_switch_lb_mpls_identification_t * mpls_identification_p)
{
    uint32 entry_handle_id;
    uint32 bit_count, max_bits;
    uint32 bit_stream;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mpls_identification_p, _SHR_E_PARAM, "mpls_identification_p");
    
    bit_stream = (uint32) (mpls_identification_p->mpls_identification);
    bit_count = utilex_nof_on_bits_in_long(bit_stream);
    max_bits = dnx_data_switch.load_balancing.num_valid_mpls_protocols_get(unit);
    if (bit_count > max_bits)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Number of MPLS protocols on input (%d) is larger than max allowed (%d).\r\n"
                     "  This is illegal. Quit.\r\n", bit_count, max_bits);
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LAYER_IDENTIFICATION_BITMAP_CFG, &entry_handle_id));
    
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id, DBAL_FIELD_MPLS_BIT_IDENTIFIER, INST_SINGLE,
                                 mpls_identification_p->mpls_identification);
    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_mpls_identification_get(
    int unit,
    dnx_switch_lb_mpls_identification_t * mpls_identification_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mpls_identification_p, _SHR_E_PARAM, "mpls_identification_p");
    
    sal_memset(mpls_identification_p, 0, sizeof(*mpls_identification_p));
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LAYER_IDENTIFICATION_BITMAP_CFG, &entry_handle_id));
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id, DBAL_FIELD_MPLS_BIT_IDENTIFIER, INST_SINGLE,
                                                        &(mpls_identification_p->mpls_identification)));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_general_seeds_set(
    int unit,
    dnx_switch_lb_general_seeds_t * general_seeds_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(general_seeds_p, _SHR_E_PARAM, "general_seeds_p");
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_GENERAL_SEEDS, &entry_handle_id));
    
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_0, INST_SINGLE,
                                 general_seeds_p->mpls_stack_0);
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_1, INST_SINGLE,
                                 general_seeds_p->mpls_stack_1);
    if (dnx_data_switch.load_balancing.mpls_split_stack_config_get(unit))
    {
        dbal_entry_value_field32_set(unit,
                                     entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_2, INST_SINGLE,
                                     general_seeds_p->mpls_stack_2);
        dbal_entry_value_field32_set(unit,
                                     entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_3, INST_SINGLE,
                                     general_seeds_p->mpls_stack_3);
    }
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_PARSER, INST_SINGLE,
                                 general_seeds_p->parser);
    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_general_seeds_get(
    int unit,
    dnx_switch_lb_general_seeds_t * general_seeds_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(general_seeds_p, _SHR_E_PARAM, "general_seeds_p");
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_GENERAL_SEEDS, &entry_handle_id));
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_0,
                                                        INST_SINGLE, &(general_seeds_p->mpls_stack_0)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_1, INST_SINGLE,
                     &(general_seeds_p->mpls_stack_1)));
    if (dnx_data_switch.load_balancing.mpls_split_stack_config_get(unit))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_2,
                                                            INST_SINGLE, &(general_seeds_p->mpls_stack_2)));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_MPLS_STACK_3, INST_SINGLE,
                         &(general_seeds_p->mpls_stack_3)));
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id, DBAL_FIELD_LB_CRC_SEED_FOR_PARSER, INST_SINGLE,
                                                        &(general_seeds_p->parser)));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_mpls_reserved_label_set(
    int unit,
    dnx_switch_lb_mpls_reserved_t * mpls_reserved_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mpls_reserved_p, _SHR_E_PARAM, "mpls_reserved_p");
    
    if (dnx_data_switch.load_balancing.reserved_next_label_valid_get(unit) &&
        (mpls_reserved_p->reserved_next_label_indication != mpls_reserved_p->reserved_next_label_valid))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "\r\n"
                     "Element 'reserved_next_label_indication' (0x%04X) is not equal to\r\n"
                     "  'reserved_next_label_valid' (0x%04X). This is illegal. Quit.\r\n",
                     mpls_reserved_p->reserved_next_label_indication, mpls_reserved_p->reserved_next_label_valid);
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_RESERVED_LABELS, &entry_handle_id));
    
    dbal_entry_value_field16_set(unit,
                                 entry_handle_id, DBAL_FIELD_RSRVD_CURRENT_LABEL_INDICATION, INST_SINGLE,
                                 mpls_reserved_p->reserved_current_label_indication);
    dbal_entry_value_field16_set(unit,
                                 entry_handle_id, DBAL_FIELD_RSRVD_CURRENT_LABEL_FORCE_LAYER, INST_SINGLE,
                                 mpls_reserved_p->reserved_current_label_force_layer);
    dbal_entry_value_field16_set(unit,
                                 entry_handle_id, DBAL_FIELD_RSRVD_NEXT_LABEL_INDICATION, INST_SINGLE,
                                 mpls_reserved_p->reserved_next_label_indication);
    if (dnx_data_switch.load_balancing.reserved_next_label_valid_get(unit))
    {
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RSRVD_NEXT_LABEL_VALID, INST_SINGLE,
                                     mpls_reserved_p->reserved_next_label_valid);
    }

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_mpls_reserved_label_get(
    int unit,
    dnx_switch_lb_mpls_reserved_t * mpls_reserved_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mpls_reserved_p, _SHR_E_PARAM, "mpls_reserved_p");
    
    sal_memset(mpls_reserved_p, 0, sizeof(*mpls_reserved_p));
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_RESERVED_LABELS, &entry_handle_id));
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get(unit,
                                                        entry_handle_id, DBAL_FIELD_RSRVD_CURRENT_LABEL_INDICATION,
                                                        INST_SINGLE,
                                                        &(mpls_reserved_p->reserved_current_label_indication)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_RSRVD_CURRENT_LABEL_FORCE_LAYER, INST_SINGLE,
                     &(mpls_reserved_p->reserved_current_label_force_layer)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_RSRVD_NEXT_LABEL_INDICATION, INST_SINGLE,
                     &(mpls_reserved_p->reserved_next_label_indication)));
    if (dnx_data_switch.load_balancing.reserved_next_label_valid_get(unit))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get(unit,
                                                            entry_handle_id, DBAL_FIELD_RSRVD_NEXT_LABEL_VALID,
                                                            INST_SINGLE,
                                                            &(mpls_reserved_p->reserved_next_label_valid)));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_load_balancing_crc_function_enum_to_hw_get(
    int unit,
    bcm_switch_hash_config_t crc_enum_value,
    uint32 *hw_val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (crc_enum_value)
    {
        case BCM_HASH_CONFIG_CRC16_0x10039:
            *hw_val = DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x10039;
            break;
        case BCM_HASH_CONFIG_CRC16_0x100d7:
            *hw_val = DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x100D7;
            break;
        case BCM_HASH_CONFIG_CRC16_0x1015d:
            *hw_val = DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x1015D;
            break;
        case BCM_HASH_CONFIG_CRC16_0x10939:
            *hw_val = DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x10939;
            break;
        case BCM_HASH_CONFIG_CRC16_0x109e7:
            *hw_val = DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x109E7;
            break;
        case BCM_HASH_CONFIG_CRC16_0x10ac5:
            *hw_val = DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x10AC5;
            break;
        case BCM_HASH_CONFIG_CRC16_0x1203d:
            *hw_val = DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x1203D;
            break;
        case BCM_HASH_CONFIG_CRC16_0x12105:
            *hw_val = DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x12105;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported enum value for hash configuration CRC %d.\r\n", crc_enum_value);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_load_balancing_crc_function_hw_to_enum_get(
    int unit,
    uint32 hw_val,
    bcm_switch_hash_config_t * crc_enum_value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (hw_val)
    {
        case DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x10039:
            *crc_enum_value = BCM_HASH_CONFIG_CRC16_0x10039;
            break;
        case DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x100D7:
            *crc_enum_value = BCM_HASH_CONFIG_CRC16_0x100d7;
            break;
        case DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x1015D:
            *crc_enum_value = BCM_HASH_CONFIG_CRC16_0x1015d;
            break;
        case DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x10939:
            *crc_enum_value = BCM_HASH_CONFIG_CRC16_0x10939;
            break;
        case DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x109E7:
            *crc_enum_value = BCM_HASH_CONFIG_CRC16_0x109e7;
            break;
        case DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x10AC5:
            *crc_enum_value = BCM_HASH_CONFIG_CRC16_0x10ac5;
            break;
        case DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x1203D:
            *crc_enum_value = BCM_HASH_CONFIG_CRC16_0x1203d;
            break;
        case DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x12105:
            *crc_enum_value = BCM_HASH_CONFIG_CRC16_0x12105;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported CRC function number %d.\r\n", hw_val);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_load_balancing_tcam_info_set(
    int unit,
    int index,
    dnx_switch_lb_tcam_key_t * tcam_key_p,
    dnx_switch_lb_tcam_result_t * tcam_result_p,
    dnx_switch_lb_valid_t tcam_valid)
{
    uint32 entry_handle_id;
    uint32 nof_lb_crc_sel_tcam_entries;
    uint32 ii, jj;
    dnx_switch_ld_destination_t lb_destination_mask;
    dnx_switch_lb_outlif_0_t outlif_0_mask;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tcam_key_p, _SHR_E_PARAM, "tcam_key_p");
    SHR_NULL_CHECK(tcam_result_p, _SHR_E_PARAM, "tcam_result_p");
    nof_lb_crc_sel_tcam_entries = dnx_data_switch.load_balancing.nof_lb_crc_sel_tcam_entries_get(unit);
    if (index >= nof_lb_crc_sel_tcam_entries)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Index of entry to update (%d) is out of range. Should be between %d and %d.\r\n",
                     index, 0, nof_lb_crc_sel_tcam_entries - 1);
    }
    
    for (ii = 0; ii < (dnx_data_switch.load_balancing.nof_lb_clients_get(unit) - 1); ii++)
    {
        bcm_switch_hash_config_t crc_sel;
        crc_sel = tcam_result_p->client_x_crc_16_sel[ii];
        for (jj = (ii + 1); jj < dnx_data_switch.load_balancing.nof_lb_clients_get(unit); jj++)
        {
            if (crc_sel == tcam_result_p->client_x_crc_16_sel[jj])
            {
                bcm_switch_control_t logical_client_numeric[2];
                char *string_enum_id[2];
                SHR_IF_ERR_EXIT(dnx_switch_lb_physical_client_to_logical(unit, ii, &(logical_client_numeric[0])));
                SHR_IF_ERR_EXIT(dnx_switch_lb_physical_client_to_logical(unit, jj, &(logical_client_numeric[1])));
                SHR_IF_ERR_EXIT(dnx_switch_control_t_name(unit, logical_client_numeric[0], &(string_enum_id[0])));
                SHR_IF_ERR_EXIT(dnx_switch_control_t_name(unit, logical_client_numeric[1], &(string_enum_id[1])));
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "\r\n"
                             "Two physical clients (%d, %d) have the same CRC function (%d).\r\n"
                             "==> The corresponding logical clients are (%s, %s).\r\n"
                             "==> This is not allowed. Quit.\r\n",
                             ii, jj, crc_sel, string_enum_id[0], string_enum_id[1]);
            }
        }
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CRC_SELECTION_HL_TCAM, &entry_handle_id));
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, index));
    
    lb_destination_mask = ~(tcam_key_p->lb_destination_mask);
    dbal_entry_key_field32_masked_set(unit,
                                      entry_handle_id, DBAL_FIELD_LB_DESTINATION, tcam_key_p->lb_destination,
                                      lb_destination_mask);
    outlif_0_mask = ~(tcam_key_p->outlif_0_mask);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_0, tcam_key_p->outlif_0, outlif_0_mask);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_0_CRC_16_SEL, INST_SINGLE,
                                 tcam_result_p->client_x_crc_16_sel[0]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_1_CRC_16_SEL, INST_SINGLE,
                                 tcam_result_p->client_x_crc_16_sel[1]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_2_CRC_16_SEL, INST_SINGLE,
                                 tcam_result_p->client_x_crc_16_sel[2]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_3_CRC_16_SEL, INST_SINGLE,
                                 tcam_result_p->client_x_crc_16_sel[3]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_4_CRC_16_SEL, INST_SINGLE,
                                 tcam_result_p->client_x_crc_16_sel[4]);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, tcam_valid);
    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_load_balancing_tcam_info_get(
    int unit,
    int index,
    dnx_switch_lb_tcam_key_t * tcam_key_p,
    dnx_switch_lb_tcam_result_t * tcam_result_p,
    dnx_switch_lb_valid_t * tcam_valid_p)
{
    uint32 entry_handle_id;
    uint32 nof_lb_crc_sel_tcam_entries;
    dnx_switch_ld_destination_t lb_destination_mask[1];
    dnx_switch_lb_outlif_0_t outlif_0_mask[1];
    uint32 field_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tcam_key_p, _SHR_E_PARAM, "physical_client_p");
    SHR_NULL_CHECK(tcam_result_p, _SHR_E_PARAM, "tcam_result_p");
    SHR_NULL_CHECK(tcam_valid_p, _SHR_E_PARAM, "tcam_valid_p");
    nof_lb_crc_sel_tcam_entries = dnx_data_switch.load_balancing.nof_lb_crc_sel_tcam_entries_get(unit);
    if (index >= nof_lb_crc_sel_tcam_entries)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Index of entry to update (%d) is out of range. Should be between %d and %d.\r\n",
                     index, 0, nof_lb_crc_sel_tcam_entries - 1);
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOAD_BALANCING_CRC_SELECTION_HL_TCAM, &entry_handle_id));
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, index));
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get(unit,
                                                                 entry_handle_id, DBAL_FIELD_LB_DESTINATION,
                                                                 &(tcam_key_p->lb_destination), lb_destination_mask));
    tcam_key_p->lb_destination_mask = ~(lb_destination_mask[0]);
    
    SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, DBAL_FIELD_DESTINATION, &field_size));
    tcam_key_p->lb_destination_mask &= SAL_UPTO_BIT(field_size);
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get(unit,
                                                                 entry_handle_id, DBAL_FIELD_OUTLIF_0,
                                                                 &(tcam_key_p->outlif_0), outlif_0_mask));
    tcam_key_p->outlif_0_mask = ~(outlif_0_mask[0]);
    
    SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, DBAL_FIELD_GLOB_OUT_LIF, &field_size));
    tcam_key_p->outlif_0_mask &= SAL_UPTO_BIT(field_size);
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CLIENT_0_CRC_16_SEL, INST_SINGLE,
                     &(tcam_result_p->client_x_crc_16_sel[0])));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CLIENT_1_CRC_16_SEL, INST_SINGLE,
                     &(tcam_result_p->client_x_crc_16_sel[1])));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CLIENT_2_CRC_16_SEL, INST_SINGLE,
                     &(tcam_result_p->client_x_crc_16_sel[2])));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CLIENT_3_CRC_16_SEL, INST_SINGLE,
                     &(tcam_result_p->client_x_crc_16_sel[3])));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CLIENT_4_CRC_16_SEL, INST_SINGLE,
                     &(tcam_result_p->client_x_crc_16_sel[4])));
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, tcam_valid_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_load_balancing_nibble_speculation_attr_get(
    int unit,
    int nibble,
    dbal_fields_e * dbal_field,
    bcm_switch_mpls_next_protocol_t * enable_protocol,
    bcm_switch_mpls_next_protocol_t * disable_protocol)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (nibble)
    {
        case 0:
        {
            *enable_protocol = bcmSwitchMplsNextProtocolControlWord;
            *disable_protocol = bcmSwitchMplsNextProtocolDefault;
            *dbal_field = DBAL_FIELD_CW_ENABLE;
            break;
        }
        case 1:
        {
            *enable_protocol = bcmSwitchMplsNextProtocolControlWord;
            *disable_protocol = bcmSwitchMplsNextProtocolDefault;
            *dbal_field = DBAL_FIELD_GACH_ENABLE;
            break;
        }
        case 4:
        {
            *enable_protocol = bcmSwitchMplsNextProtocolIpv4;
            *disable_protocol = bcmSwitchMplsNextProtocolDefault;
            *dbal_field = DBAL_FIELD_VALUE_OF_4;
            break;
        }
        case 5:
        {
            *enable_protocol = bcmSwitchMplsNextProtocolBierMpls;
            *disable_protocol = bcmSwitchMplsNextProtocolDefault;
            *dbal_field = DBAL_FIELD_VALUE_OF_5;
            break;
        }
        case 6:
        {
            *enable_protocol = bcmSwitchMplsNextProtocolIpv6;
            *disable_protocol = bcmSwitchMplsNextProtocolDefault;
            *dbal_field = DBAL_FIELD_VALUE_OF_6;
            break;
        }
        case BCM_SWITCH_DEFAULT_NIBBLE_INDEX:
        {
            *enable_protocol = bcmSwitchMplsNextProtocolEthernet;
            *disable_protocol = bcmSwitchMplsNextProtocolNone;
            *dbal_field = DBAL_FIELD_VALUE_OF_0;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Nibble %d isn't supported for configuration\n", nibble);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_load_balancing_nibble_speculation_set(
    int unit,
    int nibble,
    bcm_switch_mpls_next_protocol_t protocol)
{
    uint32 entry_handle_id;
    dbal_fields_e dbal_field;
    bcm_switch_mpls_next_protocol_t enable_protocol, disable_protocol;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_nibble_speculation_attr_get
                    (unit, nibble, &dbal_field, &enable_protocol, &disable_protocol));

    if ((protocol == enable_protocol) || (protocol == disable_protocol))
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERIN1_MPLS_SPECULATIVE, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field, INST_SINGLE,
                                     (protocol == enable_protocol) ? TRUE : FALSE);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported protocol option %d for nibble %d\n", protocol, nibble);
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_load_balancing_nibble_speculation_get(
    int unit,
    int nibble,
    bcm_switch_mpls_next_protocol_t * protocol)
{
    uint32 entry_handle_id;
    uint32 value;
    dbal_fields_e dbal_field;
    bcm_switch_mpls_next_protocol_t enable_protocol, disable_protocol;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_nibble_speculation_attr_get
                    (unit, nibble, &dbal_field, &enable_protocol, &disable_protocol));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERIN1_MPLS_SPECULATIVE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, dbal_field, INST_SINGLE, &value));
    *protocol = (value != 0) ? enable_protocol : disable_protocol;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_switch_control_load_balancing_hash_flag_verify(
    int unit,
    uint32 flags,
    dnx_switch_hash_header_attr_t * header_info)
{
    int flag_iter;
    SHR_FUNC_INIT_VARS(unit);

    
    if ((header_info->symmetrical_flag != 0) && _SHR_IS_FLAG_SET(flags, BCM_HASH_NON_SYMMETRICAL)
        && !SWITCH_IS_FLAGS_SET(flags, header_info->symmetrical_flag))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The BCM_HASH_NON_SYMMETRICAL must be set with the intended un-symmetrical header field flag  0x%x.\n",
                     header_info->symmetrical_flag);
    }

    
    for (flag_iter = 0; flag_iter < header_info->nof_field_flags; flag_iter++)
    {
        flags &=
            (SWITCH_IS_FLAGS_SET(flags, header_info->field_flags[flag_iter].flag)
             ? (~(header_info->field_flags[flag_iter].flag)) : UTILEX_U32_MAX);
    }

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flag option 0x%x .\n", flags);
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_control_load_balancing_hash_fields_masking_set(
    int unit,
    bcm_switch_control_t type,
    uint32 mask)
{
    int flag_iter;
    int field_iter;
    int dbal_commit = FALSE;
    uint32 entry_handle_id;
    dnx_switch_hash_header_attr_t header_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_control_load_balancing_hash_fields_flags_attr_get(unit, type, &header_info));

    SHR_IF_ERR_EXIT(dnx_switch_control_load_balancing_hash_flag_verify(unit, mask, &header_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, header_info.dbal_table, &entry_handle_id));

    
    if (header_info.symmetrical_flag != 0)
    {
        for (flag_iter = 0; flag_iter < header_info.nof_field_flags; flag_iter++)
        {
            if ((header_info.field_flags[flag_iter].flag == BCM_HASH_NON_SYMMETRICAL)
                || (header_info.field_flags[flag_iter].flag == header_info.symmetrical_flag))
            {
                for (field_iter = 0; field_iter < header_info.field_flags[flag_iter].nof_fields; field_iter++)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id,
                                                 header_info.field_flags[flag_iter].flag_fields[field_iter],
                                                 INST_SINGLE, 0);
                }
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, header_info.dbal_table, entry_handle_id));
    }
    switch_db.header_enablers_hashing.set(unit, header_info.field_enablers_header_id, mask);
    
    if (_SHR_IS_FLAG_SET(mask, BCM_HASH_NON_SYMMETRICAL))
    {
        mask &= ~(header_info.symmetrical_flag);
    }
    
    for (flag_iter = 0; flag_iter < header_info.nof_field_flags; flag_iter++)
    {
        uint32 value = SWITCH_IS_FLAGS_SET(mask, header_info.field_flags[flag_iter].flag);

        
        if (((!SWITCH_IS_FLAGS_SET(BCM_HASH_NON_SYMMETRICAL, header_info.field_flags[flag_iter].flag))
             && (!SWITCH_IS_FLAGS_SET(header_info.symmetrical_flag, header_info.field_flags[flag_iter].flag))) || value)
        {
            for (field_iter = 0; field_iter < header_info.field_flags[flag_iter].nof_fields; field_iter++)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id,
                                             header_info.field_flags[flag_iter].flag_fields[field_iter], INST_SINGLE,
                                             value);

                dbal_commit = TRUE;
            }
        }
    }

    
    if (dbal_commit)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_control_load_balancing_hash_fields_masking_get(
    int unit,
    bcm_switch_control_t type,
    uint32 *mask)
{
    dnx_switch_hash_header_attr_t header_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_control_load_balancing_hash_fields_flags_attr_get(unit, type, &header_info));
    switch_db.header_enablers_hashing.get(unit, header_info.field_enablers_header_id, mask);
exit:
    SHR_FUNC_EXIT;
}
