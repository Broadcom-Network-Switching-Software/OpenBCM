
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/types.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/register.h>
#include <soc/dnx/mdb.h>
#include <bcm/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <src/bcm/dnx/init/init_pp_common.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_srv6.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ire_packet_generator_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <src/bcm/dnx/multicast/multicast_pp.h>
#include <sal/core/boot.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_graphical.h>
#include <bcm_int/dnx/switch/switch_em_false_hit.h>
#include <bcm_int/dnx/field/field_init.h>

typedef struct fodo_assignment_mode_mapping_s
{
    uint32 fodo_assignment_mode;
    uint32 pd_result_type;
    uint32 fodo_mask_profile;
} fodo_assignment_mode_mapping_t;

typedef struct fodo_assignment_mask_profile_s
{
    uint32 fodo_mask_profile;
    uint8 fodo_is_masked;
    uint8 pd_result_is_fully_masked;
    uint8 pd_result_is_vlan_masked;
} fodo_assignment_mask_profile_t;

#define PPH_BASE_SIZE 12
#define PPH_BASE_SIZE_LEGACY 7

#define FODO_ASSIGNMENT_MODE_FROM_LIF    (DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF)
#define FODO_ASSIGNMENT_MODE_FROM_VLAN   (DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN)
#define FODO_ASSIGNMENT_MODE_FROM_LOOKUP (DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP)

#define PD_RESULT_TYPE_NOT_FOUND (DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_NOT_FOUND)
#define PD_RESULT_TYPE_FOUND     (DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_FOUND)

#define FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF (DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_LIF_17_0)
#define FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF_FROM_PD (DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_LIF_17_12_FROM_PD_11_0)
#define FODO_ASSIGNMENT_MASK_PROFILE_FROM_PD (DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_PD_17_0)

#define ETHERTYPE_NEXT_PROTOCOL_ETHERNET       0x6558

static const fodo_assignment_mode_mapping_t fodo_assignment_mode_mappings[] = {
    {FODO_ASSIGNMENT_MODE_FROM_LIF, PD_RESULT_TYPE_NOT_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF},
    {FODO_ASSIGNMENT_MODE_FROM_LIF, PD_RESULT_TYPE_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF},
    {FODO_ASSIGNMENT_MODE_FROM_LOOKUP, PD_RESULT_TYPE_NOT_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF},
    {FODO_ASSIGNMENT_MODE_FROM_LOOKUP, PD_RESULT_TYPE_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_PD},
    {FODO_ASSIGNMENT_MODE_FROM_VLAN, PD_RESULT_TYPE_NOT_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF_FROM_PD},
    {FODO_ASSIGNMENT_MODE_FROM_VLAN, PD_RESULT_TYPE_FOUND, FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF_FROM_PD}
};

static const fodo_assignment_mask_profile_t fodo_assignment_mask_profiles[] = {

    {FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF, TRUE, FALSE, FALSE},
    {FODO_ASSIGNMENT_MASK_PROFILE_FROM_PD, FALSE, TRUE, FALSE},
    {FODO_ASSIGNMENT_MASK_PROFILE_FROM_LIF_FROM_PD, TRUE, FALSE, TRUE}
};

shr_error_e
egress_eedb_type_buster_enable_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ESEM_GLOBAL, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_EM_BUSTER, TRUE,
                                                    DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    if (max_value > 0)
    {

        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_EM_BUSTER, 1, DBAL_RANGE_ALL);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EM_BUSTER_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_eedb_forwarding_domain_vsd_enable_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_EEDB_TYPE_VSD_ENABLE, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_VSD_ENABLE, TRUE,
                                                    DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
    if (max_value > 0)
    {

        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_VSD_ENABLE, 1, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSD_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_eedb_svtag_indication_enable_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_value = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_switch.svtag.supported_get(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_EEDB_TYPE_SVTAG_ENABLE, &entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_SVTAG_ENABLE, TRUE,
                                                        DBAL_PREDEF_VAL_MAX_VALUE, &max_value));
        if (max_value > 0)
        {

            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_EEDB_TYPE_SVTAG_ENABLE, 1,
                                             DBAL_RANGE_ALL);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_ENABLE, INST_SINGLE, TRUE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_lbp_vlan_editing_configuration_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LBP_VLAN_EDITING, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_IVE_SIZE_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FHEI_SIZE_5B);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_ippe_parser_context_mapping(
    int unit)
{
    uint32 entry_handle_id;
    uint32 hw_context_val = 0, enum_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    enum_value = INVALID_ENUM;

    SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                    (unit, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, INVALID_ENUM, &enum_value));
    while (enum_value != INVALID_ENUM)
    {

        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, enum_value, &hw_context_val));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IRPP_PARSING_CONTEXT_MAP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_PARSING_CONTEXT, hw_context_val);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, INST_SINGLE,
                                     enum_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                        (unit, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, enum_value, &enum_value));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_ippb_l4_protocols_config(
    int unit)
{
    uint32 entry_handle_id;
    int ipv_tcp_protocol = 6;
    int ipv_udp_protocol = 17;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L3_PROTOCOL_L4_VALUES, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV_TCP_PROTOCL, INST_SINGLE, ipv_tcp_protocol);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV_UDP_PROTOCL, INST_SINGLE, ipv_udp_protocol);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_port_general_configuration_init(
    int unit)
{

    uint32 entry_handle_id, pp_port, mapped_pp_port;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        for (pp_port = 0; pp_port < dnx_data_port.general.nof_pp_ports_get(unit); pp_port++)
        {
            mapped_pp_port =
                ((core_id << utilex_log2_round_up(dnx_data_port.general.nof_pp_ports_get(unit))) | pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, INST_SINGLE, mapped_pp_port);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_da_type_map_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_DA_TYPE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_BC, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_MC, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_BC);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_BC, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_MC, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_BC, FALSE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DA_MC, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_UC);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_stat_fec_ecmp_mapping_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_FEC_ECMP_STATISTICS_PROFILE_MAP, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FEC_STAT_OBJECT_PROFILE, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_STATISTIC_PROFILE_MAP, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FEC_STATISTIC_PROFILE_MAP_TWO_INDIRECT_COUNTER);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_STATISTIC_PROFILE_MAP, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ECMP_STATISTIC_PROFILE_MAP_ECMP_GROUP);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ire_packet_generator_init(
    int unit)
{
    uint8 is_ire_packet_init = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ire_packet_generator_info.is_init(unit, &is_ire_packet_init));
    if (!is_ire_packet_init)
    {
        SHR_IF_ERR_EXIT(ire_packet_generator_info.init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_layer_protocol_allow_speculative_learning(
    int unit,
    int layer_entry,
    int enable)
{
    uint32 entry_handle_id;
    uint8 is_illegal = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_LAYER_TYPES, layer_entry, &is_illegal));
    if (!is_illegal)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_entry);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPECULATIVE_PROTOCOL_ETH_ENABLE, INST_SINGLE,
                                     enable);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_per_layer_protocol_configuration(
    int unit,
    int layer_type_enum_val,
    int is_ipv4,
    int is_ipv6,
    int is_mpls,
    int is_oam,
    int is_tcp,
    int is_udp)
{

    uint32 entry_handle_id;
    uint8 is_illegal = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val, &is_illegal));
    if (is_illegal)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV4_ENABLE, INST_SINGLE, is_ipv4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV6_ENABLE, INST_SINGLE, is_ipv6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_ENABLE, INST_SINGLE, is_mpls);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_OAM_ENABLE, INST_SINGLE, is_oam);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_TCP_ENABLE, INST_SINGLE, is_tcp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_UDP_ENABLE, INST_SINGLE, is_udp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
bare_metal_configuration_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRAP_INGRESS_FWD_ACTION_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_PARSING_INDEX_VALUE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_PARSING_INDEX_OVERWRITE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_per_layer_protocol_configuration(
    int unit,
    int layer_type_enum_val,
    int is_ethernet,
    int is_arp,
    int is_ipv4,
    int is_ipv6,
    int is_mpls_term,
    int is_mpls_fwd,
    int is_udp,
    int is_bfd_single_hop,
    int is_bfd_multi_hop,
    int is_icmpv6,
    int is_igmp,
    int is_8021_x_2,
    int is_icmp,
    int is_bier_mpls,
    int is_bier_ti)
{

    uint32 entry_handle_id;
    int system_headers_mode;
    int allow_speculative_learning;
    uint8 is_illegal = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val, &is_illegal));
    if (is_illegal)
    {
        SHR_EXIT();
    }

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PER_LAYER_PROTOCOL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_type_enum_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETH_ENABLE, INST_SINGLE, is_ethernet);

    if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) &&
        (!dnx_data_headers.
         general.feature_get(unit, dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx)))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETH_MINUS_ONE_ENABLE, INST_SINGLE, 0);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ETH_MINUS_ONE_ENABLE, INST_SINGLE,
                                     is_ethernet);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ARP_ENABLE, INST_SINGLE, is_arp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IP_ENABLE, INST_SINGLE,
                                 (is_ipv4 | is_ipv6));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV4_ENABLE, INST_SINGLE, is_ipv4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IPV6_ENABLE, INST_SINGLE, is_ipv6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_TERM_ENABLE, INST_SINGLE,
                                 is_mpls_term);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_MPLS_FWD_ENABLE, INST_SINGLE, is_mpls_fwd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_UDP_ENABLE, INST_SINGLE, is_udp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_BFD_SINGLE_HOP_ENABLE, INST_SINGLE,
                                 is_bfd_single_hop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_BFD_MULTI_HOP_ENABLE, INST_SINGLE,
                                 is_bfd_multi_hop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ICMPV6_ENABLE, INST_SINGLE, is_icmpv6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_IGMP_ENABLE, INST_SINGLE, is_igmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_8021_X_2_ENABLE, INST_SINGLE, is_8021_x_2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_ICMP_ENABLE, INST_SINGLE, is_icmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_BIER_MPLS_ENABLE, INST_SINGLE,
                                 is_bier_mpls);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTOCOL_BIER_TI_ENABLE, INST_SINGLE, is_bier_ti);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (!dnx_data_mpls.general.mpls_speculative_learning_handling_get(unit))
    {
        allow_speculative_learning = 1;
    }
    else
    {
        allow_speculative_learning = is_ethernet;
    }
    SHR_IF_ERR_EXIT(ingress_layer_protocol_allow_speculative_learning
                    (unit, layer_type_enum_val, allow_speculative_learning));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_udp_next_protocol_enable_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERUDP, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4OUDP_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6OUDP_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_parser_geneve_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERGENEVE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETH, INST_SINGLE,
                                 DBAL_DEFINE_CURRENT_NEXT_PROTOCOL_GENEVE_OVER_UDP);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pph_base_size_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 pph_base_size;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        pph_base_size = PPH_BASE_SIZE_LEGACY;
    }
    else
    {
        pph_base_size = PPH_BASE_SIZE;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_SIZE, INST_SINGLE, pph_base_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_init_pp_layer_types_value_get(
    int unit,
    uint32 prototype)
{
    uint32 hw_enum_value;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_LAYER_TYPES, prototype, &hw_enum_value));

    return hw_enum_value;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_itpp_general_network_header_termination_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_network_header_termination))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ITPP_TERMINATE_NETWORK_HEADERS, &entry_handle_id));
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_STRENGTH, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_TM_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_TERMINATE_NETWORK_HEADERS, INST_SINGLE,
                                     TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_STRENGTH, TRUE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PROFILE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_TERMINATE_NETWORK_HEADERS, INST_SINGLE,
                                     FALSE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_cfg_in_lif_null_value_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 egress_in_lif_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    egress_in_lif_val = dnx_data_lif.global_lif.egress_in_lif_null_value_get(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ETPP_GLOBAL, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_IN_LIF_NULL_VALUE, INST_SINGLE,
                                 egress_in_lif_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_pp_egress_current_next_protocol_config_table_configure(
    int unit,
    dbal_enum_value_field_current_protocol_type_e current_protocol_type,
    dbal_enum_value_field_etpp_next_protocol_namespace_e current_protocol_namespace,
    int current_next_protocol)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_PROTOCOL_TYPE, current_protocol_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PROTOCOL_NAMESPACE, current_protocol_namespace);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_NEXT_PROTOCOL, INST_SINGLE,
                                 current_next_protocol);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_ingress_trapped_by_fhei_configure_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_INGRESS_TRAPPED_BY_FHEI, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_TYPE, DBAL_ENUM_FVAL_FHEI_TYPE_TRAP_SNOOP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAPPED_BY_FHEI_TYPE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_fallback_to_bridge_configuration(
    int unit,
    uint32 fwd_additional_info,
    uint8 ipvx_fallback_to_bridge_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_FORWARDING_ADDITIONAL_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, fwd_additional_info);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IPMC_ELIGIBLE_MAPPING, INST_SINGLE,
                                ipvx_fallback_to_bridge_enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_forwarding_domain_assignment_mask_profiles_set_fodo_mask(
    int unit,
    uint32 entry_handle_id,
    uint8 fodo_is_masked)
{
    SHR_FUNC_INIT_VARS(unit);

    if (fodo_is_masked)
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_FODO_BASE_MASK, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
    }
    else
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_FODO_BASE_MASK, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MIN_VALUE);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_forwarding_domain_assignment_mask_profiles_set_pd_result_mask(
    int unit,
    uint32 entry_handle_id,
    uint8 pd_result_is_fully_masked,
    uint8 pd_result_is_vlan_masked)
{
    SHR_FUNC_INIT_VARS(unit);

    if (pd_result_is_fully_masked)
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_MASK,
                                                   INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);
    }
    else if (pd_result_is_vlan_masked)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_MASK, INST_SINGLE, BCM_VLAN_MAX);
    }
    else
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_PD_RESULT_MASK,
                                                   INST_SINGLE, DBAL_PREDEF_VAL_MIN_VALUE);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
ingress_forwarding_domain_assignment_mask_profiles_configure(
    int unit,
    uint32 fodo_mask_profile,
    uint8 fodo_is_masked,
    uint8 pd_result_is_fully_masked,
    uint8 pd_result_is_vlan_masked)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FODO_ASSIGNMENT_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_DOMAIN_MASK_PROFILE, fodo_mask_profile);
    SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mask_profiles_set_fodo_mask
                    (unit, entry_handle_id, fodo_is_masked));
    SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mask_profiles_set_pd_result_mask
                    (unit, entry_handle_id, pd_result_is_fully_masked, pd_result_is_vlan_masked));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_forwarding_domain_assignment_mask_profiles_mapping_init(
    int unit)
{
    int nof_fodo_assignment_mask_profile_entries;
    int entry;

    SHR_FUNC_INIT_VARS(unit);

    nof_fodo_assignment_mask_profile_entries =
        sizeof(fodo_assignment_mask_profiles) / sizeof(fodo_assignment_mask_profile_t);

    for (entry = 0; entry < nof_fodo_assignment_mask_profile_entries; entry++)
    {
        SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mask_profiles_configure(unit,
                                                                                     fodo_assignment_mask_profiles
                                                                                     [entry].fodo_mask_profile,
                                                                                     fodo_assignment_mask_profiles
                                                                                     [entry].fodo_is_masked,
                                                                                     fodo_assignment_mask_profiles
                                                                                     [entry].pd_result_is_fully_masked,
                                                                                     fodo_assignment_mask_profiles
                                                                                     [entry].pd_result_is_vlan_masked));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_forwarding_domain_assignment_mode_mapping_configure(
    int unit,
    uint32 fodo_assignment_mode,
    uint32 pd_result_type,
    uint32 fodo_mask_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FODO_ASSIGNMENT_MODE_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, fodo_assignment_mode);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_PD_RESULT_TYPE, pd_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_DOMAIN_MASK_PROFILE, INST_SINGLE,
                                 fodo_mask_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
ingress_forwarding_domain_assignment_mode_mapping_init(
    int unit)
{
    int nof_fodo_assignment_mode_mapping_entries;
    int entry;

    SHR_FUNC_INIT_VARS(unit);

    nof_fodo_assignment_mode_mapping_entries =
        sizeof(fodo_assignment_mode_mappings) / sizeof(fodo_assignment_mode_mapping_t);

    for (entry = 0; entry < nof_fodo_assignment_mode_mapping_entries; entry++)
    {
        SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mode_mapping_configure(unit,
                                                                                    fodo_assignment_mode_mappings
                                                                                    [entry].fodo_assignment_mode,
                                                                                    fodo_assignment_mode_mappings
                                                                                    [entry].pd_result_type,
                                                                                    fodo_assignment_mode_mappings
                                                                                    [entry].fodo_mask_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_forwarding_domain_assignment_mode_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mode_mapping_init(unit));
    SHR_IF_ERR_EXIT(ingress_forwarding_domain_assignment_mask_profiles_mapping_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
egress_mapping_forward_context_configure_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_egress_fwd_code_e forward_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_FWD_CONTEXT, &entry_handle_id));

    for (forward_context = 0; forward_context < DBAL_NOF_ENUM_EGRESS_FWD_CODE_VALUES; forward_context++)
    {
        uint8 is_illegal = TRUE;

        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_EGRESS_FWD_CODE, forward_context, &is_illegal));
        if (!is_illegal)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_FWD_CODE, forward_context);
            if ((forward_context == DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ETHERNET)
                || (forward_context == DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER,
                                             INST_SINGLE, TRUE);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CONTEXT_SAME_INTERFACE_FILTER,
                                             INST_SINGLE, FALSE);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_port_outlif_profile_configuration_init(
    int unit)
{
    uint32 pp_port = 0, entry_handle_id = 0;
    uint32 nof_pp_ports;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_pp_ports = dnx_data_port.general.nof_pp_ports_get(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        for (pp_port = 0; pp_port < nof_pp_ports; pp_port++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_OUTLIF_PROFILE, INST_SINGLE,
                                         DNX_OUT_LIF_PROFILE_DEFAULT_PORT_PROFILE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_egress_parser_configuration_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERETH, &entry_handle_id));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ETHTYPEFCOE, INST_SINGLE,
                                 DBAL_DEFINE_ETHERTYPE_FCOE);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ETHTYPEPPP, INST_SINGLE, DBAL_DEFINE_ETHERTYPE_PPP);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ETHTYPEPTP_GENERAL, INST_SINGLE,
                                 DBAL_DEFINE_ETHERTYPE_1588);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_PARSERIPV4, entry_handle_id));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLICMP, INST_SINGLE,
                                 DBAL_DEFINE_NEXT_PROTOCOL_ICMP);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLIGMP, INST_SINGLE,
                                 DBAL_DEFINE_NEXT_PROTOCOL_IGMP);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_PARSERIPV6, entry_handle_id));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLICMPV6, INST_SINGLE,
                                 DBAL_DEFINE_NEXT_PROTOCOL_ICMPV6);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_PARSERUDP, entry_handle_id));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTBFD_SINGLE_HOP, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_BFD_SINGLE_HOP);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTBFD_MULTI_HOP, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_BFD_MULTI_HOP);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTGENERAL_1, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_PTP_EVENT_MSG);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTGENERAL_2, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_PTP_GENERAL_MSG);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTETH, INST_SINGLE, DBAL_DEFINE_UDP_PORT_ETH);

    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTIPV6, INST_SINGLE, DBAL_DEFINE_UDP_PORT_IPV6);

    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTGENEVE, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_GENEVE);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTMICROBFD, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_MICROBFD);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTL2TP, INST_SINGLE, DBAL_DEFINE_UDP_PORT_L2TP);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTVXLAN_1, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_VXLAN);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTVXLAN_2, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_VXLAN_GPE);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTIPV4, INST_SINGLE, DBAL_DEFINE_UDP_PORT_IPV4);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTMPLS, INST_SINGLE, DBAL_DEFINE_UDP_PORT_MPLS);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_DSTPORTBFD_SEAMLESS_INITIATOR, INST_SINGLE,
                                 DBAL_DEFINE_UDP_PORT_SEAMLESS_BFD);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_PARSERVXLAN_GPE, entry_handle_id));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLIPT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VXLAN_NEXT_PROTOCOL_IPT);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLIPV4, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VXLAN_NEXT_PROTOCOL_IPV4);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLIPV6, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VXLAN_NEXT_PROTOCOL_IPV6);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NEXTPROTOCOLMPLS, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VXLAN_NEXT_PROTOCOL_MPLS);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_gre_next_protocol_eth_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERGRE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETH, INST_SINGLE, ETHERTYPE_NEXT_PROTOCOL_ETHERNET);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
egress_exclude_source_filter_configure_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_EXCLUDE_SOURCE_MAPPED, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXCLUDE_SOURCE_MAPPED, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ingress_is_ipv6_header_configuration_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PROTOCOL_IS_IPV6, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_PROTOCOL, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_IPV6, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
