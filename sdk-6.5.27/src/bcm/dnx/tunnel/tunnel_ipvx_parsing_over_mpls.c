/** \file tunnel_ipvx_parsing_over_mpls.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL
/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
/*
 * }
 */

#define MPLS_LABEL_USE_EXPLICIT_NULL_PER_IP_PROTOCOL (-1)
#define MPLS_LABEL_EXPLICIT_NULL_V4                  (0)
#define MPLS_LABEL_EXPLICIT_NULL_V6                  (2)

typedef struct
{
    uint32 is_mc;
    uint32 has_options;
    uint32 is_fragmented;
    uint32 is_1st_fragment;
    uint32 first_add_header_exist;
    dbal_enum_value_field_ipv4_additional_header_e ipv4_1st_add_header;
    uint32 second_add_header_exist;
    dbal_enum_value_field_ipv4_qualifier_tunnel_type_e tunnel_type;
    uint32 fragmented_non_first;
} ipvx_parsing_over_mpls_ipv4_qualifier_fields_t;

typedef struct
{
    uint32 is_mc;
    uint32 first_add_header_exist;
    dbal_enum_value_field_ipv6_additional_header_e ipv6_1st_add_header;
    uint32 second_add_header_exist;
    dbal_enum_value_field_ipv6_additional_header_e ipv6_2nd_add_header;
    uint32 third_add_header_exist;
    uint32 fragmented_non_first;
    uint32 next_micro_sid_is_null;
} ipvx_parsing_over_mpls_ipv6_qualifier_fields_t;

typedef struct
{
    uint32 dst_port;
    uint32 dst_port_mask;
    uint32 src_port;
    uint32 src_port_mask;

    /*
     * Result parameters
     */
    uint32 layer_size;
    dbal_enum_value_field_irpp_2nd_parser_parser_context_e next_parser_context;
    dbal_enum_value_field_layer_types_e next_layer_type;
    ipvx_parsing_over_mpls_ipv4_qualifier_fields_t ipv4_qualifier_fields;
    ipvx_parsing_over_mpls_ipv6_qualifier_fields_t ipv6_qualifier_fields;
} ipvx_parsing_over_mpls_udp_fields_t;

typedef struct
{
    uint32 c_flag;
    uint32 c_flag_mask;
    uint32 k_flag;
    uint32 k_flag_mask;
    uint32 s_flag;
    uint32 s_flag_mask;
    uint32 version;
    uint32 version_mask;
    uint32 protocol;
    uint32 protocol_mask;

    /*
     * Result parameters
     */
    uint32 layer_size;
    dbal_enum_value_field_irpp_2nd_parser_parser_context_e next_parser_context;
    dbal_enum_value_field_layer_types_e next_layer_type;
    ipvx_parsing_over_mpls_ipv4_qualifier_fields_t ipv4_qualifier_fields;
    ipvx_parsing_over_mpls_ipv6_qualifier_fields_t ipv6_qualifier_fields;

} ipvx_parsing_over_mpls_gre_fields_t;

typedef struct
{
    uint32 fragmanet_offset;
    uint32 fragmanet_offset_mask;
    uint32 flags;
    uint32 flags_mask;
    uint32 ihl;
    uint32 ihl_mask;
} ipvx_parsing_over_mpls_ipv4_fields_t;

typedef struct
{
    uint32 label;
    uint32 label_mask;
    uint32 exp;
    uint32 exp_mask;
    uint32 bos;
    uint32 bos_mask;
} ipvx_parsing_over_mpls_mpls_fields_t;

static shr_error_e
dnx_tunnel_ipvx_over_mpls_build_ipv4_v6_qualifier(
    int unit,
    int is_v4,
    ipvx_parsing_over_mpls_ipv4_qualifier_fields_t * ipv4_qualifier_fields,
    ipvx_parsing_over_mpls_ipv6_qualifier_fields_t * ipv6_qualifier_fields,
    uint32 *ipvx_qual)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (is_v4)
    {
        /*
         * <field name="is_mc" bits="0:0"/> <field name="has_option" bits="1:1"/> <field name="is_fragmented"
         * bits="2:2"/> <field name="is_1st_fragment" bits="3:3"/> <field name="1st_add_header_exist" bits="4:4"/>
         * <field name="ipv4_1st_add_header" bits="8:5"/> <field name="2nd_add_header_exist" bits="9:9"/> <field
         * name="tunnel_type" bits="13:10"/> <field name="fragmented_non_first" bits="14:14"/> <field name="reserved"
         * bits="15:15"/> 
         */
        uint32 ipv4_1st_add_header_hw_value = 0;
        uint32 tunnel_type_hw_value = 0;

        SHR_IF_ERR_EXIT(dbal_fields_field_type_32_hw_value_get(unit, DBAL_FIELD_TYPE_DEF_IPV4_ADDITIONAL_HEADER,
                                                               ipv4_qualifier_fields->ipv4_1st_add_header,
                                                               &ipv4_1st_add_header_hw_value));

        SHR_IF_ERR_EXIT(dbal_fields_field_type_32_hw_value_get(unit, DBAL_FIELD_TYPE_DEF_IPV4_QUALIFIER_TUNNEL_TYPE,
                                                               ipv4_qualifier_fields->tunnel_type,
                                                               &tunnel_type_hw_value));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 0, 1, ipv4_qualifier_fields->is_mc));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 1, 1, ipv4_qualifier_fields->has_options));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 2, 1, ipv4_qualifier_fields->is_fragmented));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 3, 1, ipv4_qualifier_fields->is_1st_fragment));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 4, 1, ipv4_qualifier_fields->first_add_header_exist));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 5, 4, ipv4_1st_add_header_hw_value));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 9, 1, ipv4_qualifier_fields->second_add_header_exist));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 10, 4, tunnel_type_hw_value));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 14, 1, ipv4_qualifier_fields->fragmented_non_first));
    }
    else
    {
        /*
         * <structure name="IPv6_standard_Qualifier" size="16"> <field name="is_mc" bits="0:0"/> <field
         * name="1st_add_header_exist" bits="1:1"/> <field name="ipv6_1st_add_header" bits="6:2"/> <field
         * name="2nd_add_header_exist" bits="7:7"/> <field name="ipv6_2nd_add_header" bits="12:8"/> <field
         * name="3rd_add_header_exist" bits="13:13"/> <field name="fragmented_non_first" bits="14:14"/> <field
         * name="next_micro_sid_is_null" bits="15:15"/> </structure> 
         */
        uint32 ipv6_1st_add_header_hw_value = 0;
        uint32 ipv6_2nd_add_header_hw_value = 0;

        SHR_IF_ERR_EXIT(dbal_fields_field_type_32_hw_value_get(unit, DBAL_FIELD_TYPE_DEF_IPV6_ADDITIONAL_HEADER,
                                                               ipv6_qualifier_fields->ipv6_1st_add_header,
                                                               &ipv6_1st_add_header_hw_value));

        SHR_IF_ERR_EXIT(dbal_fields_field_type_32_hw_value_get(unit, DBAL_FIELD_TYPE_DEF_IPV6_ADDITIONAL_HEADER,
                                                               ipv6_qualifier_fields->ipv6_2nd_add_header,
                                                               &ipv6_2nd_add_header_hw_value));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 0, 1, ipv6_qualifier_fields->is_mc));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 1, 1, ipv6_qualifier_fields->first_add_header_exist));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 2, 5, ipv6_1st_add_header_hw_value));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 7, 1, ipv6_qualifier_fields->second_add_header_exist));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 8, 5, ipv6_2nd_add_header_hw_value));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 13, 4, ipv6_qualifier_fields->third_add_header_exist));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(ipvx_qual, 14, 1, ipv6_qualifier_fields->fragmented_non_first));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_ipvx_over_mpls_parsing_predefined_entries_mpls_fields_add(
    int unit,
    ipvx_parsing_over_mpls_mpls_fields_t * mpls_entry,
    uint32 mpls_explicit_null_label,
    uint32 *mpls_info,
    uint32 *mpls_info_mask)
{

    uint32 mpls_label_to_set = 0;

    SHR_FUNC_INIT_VARS(unit);

    mpls_label_to_set = mpls_entry->label == MPLS_LABEL_USE_EXPLICIT_NULL_PER_IP_PROTOCOL ?
        mpls_explicit_null_label : mpls_entry->label;

    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      DBAL_FIELD_TYPE_DEF_MPLS_LABEL_EXP_BOS,
                                                                      DBAL_FIELD_MPLS_LABEL,
                                                                      &mpls_label_to_set,
                                                                      &mpls_entry->label_mask,
                                                                      mpls_info, mpls_info_mask));

    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      DBAL_FIELD_TYPE_DEF_MPLS_LABEL_EXP_BOS,
                                                                      DBAL_FIELD_MPLS_EXP,
                                                                      &mpls_entry->exp,
                                                                      &mpls_entry->exp_mask,
                                                                      mpls_info, mpls_info_mask));

    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      DBAL_FIELD_TYPE_DEF_MPLS_LABEL_EXP_BOS,
                                                                      DBAL_FIELD_MPLS_BOS,
                                                                      &mpls_entry->bos,
                                                                      &mpls_entry->bos_mask,
                                                                      mpls_info, mpls_info_mask));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_ipvx_over_mpls_parsing_add_entry(
    int unit,
    int priority,
    int is_v4,
    dbal_fields_e ipvx_key_field,
    uint32 *ipvx_key_val,
    uint32 *ipvx_key_mask,
    uint32 mpls_key_val,
    uint32 mpls_key_mask,
    uint32 layer_qual,
    uint32 layer_size,
    dbal_enum_value_field_layer_types_e next_layer_type,
    dbal_enum_value_field_irpp_2nd_parser_parser_context_e next_parser_context,
    int is_default)
{
    uint32 entry_handle_id;
    uint32 entry_is_not_default = 0, parsing_layer_index = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_default)
    {
        parsing_layer_index = 3;
        entry_is_not_default = 0;
    }
    else
    {
        parsing_layer_index = 4;
        entry_is_not_default = 1;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPVX_PARSING_TCAM_DB, &entry_handle_id));
    /*
     * Create TCAM access id
     */
    SHR_IF_ERR_EXIT(dbal_entry_attribute_set(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, priority));
    dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, ipvx_key_field, ipvx_key_val, ipvx_key_mask);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_EXP_BOS, mpls_key_val,
                                      mpls_key_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_QUALIFIER, INST_SINGLE, layer_qual);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_SIZE, INST_SINGLE, layer_size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_TYPE, INST_SINGLE, next_layer_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PARSER_CONTEXT, INST_SINGLE,
                                 next_parser_context);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_IPV4, INST_SINGLE, (uint32) is_v4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_IS_NOT_DEFAULT, INST_SINGLE,
                                 entry_is_not_default);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARSING_LAYER_INDEDX, INST_SINGLE,
                                 parsing_layer_index);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_tunnel_ipvx_over_mpls_parsing_predefined_entries_ipv4_fields_add(
    int unit,
    ipvx_parsing_over_mpls_ipv4_fields_t * ipv4_entry,
    uint32 *ipv4_info,
    uint32 *ipv4_info_mask)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      DBAL_FIELD_TYPE_DEF_IPV4_PARSING_KEYS,
                                                                      DBAL_FIELD_FRAGMENT_OFFSET,
                                                                      &ipv4_entry->fragmanet_offset,
                                                                      &ipv4_entry->fragmanet_offset_mask,
                                                                      ipv4_info, ipv4_info_mask));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      DBAL_FIELD_TYPE_DEF_IPV4_PARSING_KEYS,
                                                                      DBAL_FIELD_FLAGS,
                                                                      &ipv4_entry->flags,
                                                                      &ipv4_entry->flags_mask,
                                                                      ipv4_info, ipv4_info_mask));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      DBAL_FIELD_TYPE_DEF_IPV4_PARSING_KEYS,
                                                                      DBAL_FIELD_IHL,
                                                                      &ipv4_entry->ihl,
                                                                      &ipv4_entry->ihl_mask,
                                                                      ipv4_info, ipv4_info_mask));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_ipvx_over_mpls_parsing_predefined_entries_udp_fields_add(
    int unit,
    ipvx_parsing_over_mpls_udp_fields_t * udp_entry,
    int is_v4,
    dbal_field_types_defs_e ipvx_field_type,
    uint32 *ipvx_info,
    uint32 *ipvx_info_mask,
    uint32 *ipvx_qualifier)
{
    uint32 udp_info[4] = { 0 };
    uint32 udp_info_mask[4] = { 0 };
    dbal_field_types_defs_e udp_field_type =
        is_v4 ? DBAL_FIELD_TYPE_DEF_IPV4_PARSING_KEY_UDP : DBAL_FIELD_TYPE_DEF_IPV6_PARSING_KEY_UDP;
    dbal_fields_e udp_field = is_v4 ? DBAL_FIELD_IPV4_PARSING_KEY_UDP : DBAL_FIELD_IPV6_PARSING_KEY_UDP;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      udp_field_type,
                                                                      DBAL_FIELD_UDP_DST_PORT,
                                                                      &udp_entry->dst_port,
                                                                      &udp_entry->dst_port_mask,
                                                                      udp_info, udp_info_mask));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      udp_field_type,
                                                                      DBAL_FIELD_UDP_SRC_PORT,
                                                                      &udp_entry->src_port,
                                                                      &udp_entry->src_port_mask,
                                                                      udp_info, udp_info_mask));

    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      ipvx_field_type,
                                                                      udp_field,
                                                                      udp_info,
                                                                      udp_info_mask, ipvx_info, ipvx_info_mask));

    SHR_IF_ERR_EXIT(dnx_tunnel_ipvx_over_mpls_build_ipv4_v6_qualifier(unit, is_v4,
                                                                      &udp_entry->ipv4_qualifier_fields,
                                                                      &udp_entry->ipv6_qualifier_fields,
                                                                      ipvx_qualifier));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_ipvx_over_mpls_parsing_predefined_entries_gre_fields_add(
    int unit,
    ipvx_parsing_over_mpls_gre_fields_t * gre_entry,
    int is_v4,
    dbal_field_types_defs_e ipvx_field_type,
    uint32 *ipvx_info,
    uint32 *ipvx_info_mask,
    uint32 *ipvx_qualifier)
{

    uint32 gre_info[4] = { 0 };
    uint32 gre_info_mask[4] = { 0 };
    dbal_field_types_defs_e gre_field_type =
        is_v4 ? DBAL_FIELD_TYPE_DEF_IPV4_PARSING_KEY_GRE : DBAL_FIELD_TYPE_DEF_IPV6_PARSING_KEY_GRE;
    dbal_fields_e gre_field = is_v4 ? DBAL_FIELD_IPV4_PARSING_KEY_GRE : DBAL_FIELD_IPV6_PARSING_KEY_GRE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      gre_field_type,
                                                                      DBAL_FIELD_GRE_C_FLAG,
                                                                      &gre_entry->c_flag,
                                                                      &gre_entry->c_flag_mask,
                                                                      gre_info, gre_info_mask));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      gre_field_type,
                                                                      DBAL_FIELD_GRE_K_FLAG,
                                                                      &gre_entry->k_flag,
                                                                      &gre_entry->k_flag_mask,
                                                                      gre_info, gre_info_mask));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      gre_field_type,
                                                                      DBAL_FIELD_GRE_S_FLAG,
                                                                      &gre_entry->s_flag,
                                                                      &gre_entry->s_flag_mask,
                                                                      gre_info, gre_info_mask));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      gre_field_type,
                                                                      DBAL_FIELD_GRE_VERSION,
                                                                      &gre_entry->version,
                                                                      &gre_entry->version_mask,
                                                                      gre_info, gre_info_mask));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      gre_field_type,
                                                                      DBAL_FIELD_GRE_PROTOCOL_TYPE,
                                                                      &gre_entry->protocol,
                                                                      &gre_entry->protocol_mask,
                                                                      gre_info, gre_info_mask));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode_masked(unit,
                                                                      ipvx_field_type,
                                                                      gre_field,
                                                                      gre_info,
                                                                      gre_info_mask, ipvx_info, ipvx_info_mask));
    SHR_IF_ERR_EXIT(dnx_tunnel_ipvx_over_mpls_build_ipv4_v6_qualifier(unit,
                                                                      is_v4,
                                                                      &gre_entry->ipv4_qualifier_fields,
                                                                      &gre_entry->ipv6_qualifier_fields,
                                                                      ipvx_qualifier));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 */
shr_error_e
dnx_tunnel_ipvx_over_mpls_parsing_predefined_entries_add(
    int unit)
{
    int ii, jj, kk = 0;
    int nof_entries_udp, nof_entries_gre, nof_entries_ipv4, nof_entries_ipv6, nof_entries_mpls;
    int is_v4 = TRUE;

    ipvx_parsing_over_mpls_udp_fields_t udp_entries[] = {
        /** VxLAN */
        {
         .dst_port = 4789,.dst_port_mask = 0xFFFF,
         .src_port = 0,.src_port_mask = 0,
         .layer_size = 16,.next_parser_context = DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_ETH_B1,
         .next_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET,
         .ipv4_qualifier_fields = {
                                   /** is_mc is always set to 1. PEM code will set it to 0 if the packekt is not MC) */
                                   .is_mc = 1,.has_options = 0,.is_fragmented = 0,.is_1st_fragment = 0,
                                   .first_add_header_exist = 1,.ipv4_1st_add_header =
                                   DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_UDP,
                                   .second_add_header_exist = 1,.tunnel_type =
                                   DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLANOUDP,
                                   .fragmented_non_first = 0},
         .ipv6_qualifier_fields = {
                                    /** is_mc is always set to 1. PEM code will set it to 0 if the packekt is not MC) */
                                   .is_mc = 1,
                                   .first_add_header_exist = 1,.ipv6_1st_add_header =
                                   DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP,
                                   .second_add_header_exist = 1,.ipv6_2nd_add_header =
                                   DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN,
                                   .third_add_header_exist = 0,.fragmented_non_first = 0,.next_micro_sid_is_null = 0}
         }
    };

    ipvx_parsing_over_mpls_gre_fields_t gre_entries[] = {
        /** Note that this is a dummy GRE entry. */
        {
         .c_flag = 0,.c_flag_mask = 1,
         .k_flag = 0,.k_flag_mask = 1,
         .s_flag = 0,.s_flag_mask = 1,
         .version = 0,.version_mask = 1,
         .protocol = 0,.protocol_mask = 1,
         .layer_size = 16,.next_parser_context = DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_ETH_B1,
         .next_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET,
         .ipv4_qualifier_fields = {
                                   .is_mc = 0,.has_options = 0,.is_fragmented = 0,.is_1st_fragment = 0,
                                   .first_add_header_exist = 0,.ipv4_1st_add_header = 0,
                                   .second_add_header_exist = 0,.tunnel_type = 0,
                                   .fragmented_non_first = 0},
         .ipv6_qualifier_fields = {
                                   .is_mc = 0,
                                   .first_add_header_exist = 1,.ipv6_1st_add_header = 1,
                                   .second_add_header_exist = 1,.ipv6_2nd_add_header = 1,
                                   .third_add_header_exist = 0,.fragmented_non_first = 0,.next_micro_sid_is_null = 0}
         }
    };

    ipvx_parsing_over_mpls_ipv4_fields_t ipv4_entries[] = {
        {
         .fragmanet_offset = 0,.fragmanet_offset_mask = 0xFFFF,
         .flags = 0,.flags_mask = 0xFFFF,
         .ihl = 5,.ihl_mask = 0xFFFF,
         }
    };

    ipvx_parsing_over_mpls_mpls_fields_t mpls_entries[] = {
        {
         .label = MPLS_LABEL_USE_EXPLICIT_NULL_PER_IP_PROTOCOL,.label_mask = 0xFFFFF,
         .exp = 0,.exp_mask = 0x0,
         .bos = 1,.bos_mask = 0x1}
    };

    SHR_FUNC_INIT_VARS(unit);

    nof_entries_mpls = sizeof(mpls_entries) / sizeof(ipvx_parsing_over_mpls_mpls_fields_t);
    nof_entries_udp = sizeof(udp_entries) / sizeof(ipvx_parsing_over_mpls_udp_fields_t);
    nof_entries_gre = sizeof(gre_entries) / sizeof(ipvx_parsing_over_mpls_gre_fields_t);
    nof_entries_ipv4 = sizeof(ipv4_entries) / sizeof(ipvx_parsing_over_mpls_ipv4_fields_t);
    /*
     * Hard coded 1 as the parameters from IPv6 header in the TCAM key are the next protocol and version.
     * Both are set as part of DBAL child encoding
     */
    nof_entries_ipv6 = 1;

    for (ii = 0; ii < (nof_entries_ipv4 + nof_entries_ipv6); ii++)
    {
        uint32 ipvx_info[4] = { 0 };
        uint32 ipvx_info_mask[4] = { 0 };
        uint32 mpls_explicit_null_label = 0;
        dbal_fields_e tcam_parsing_key_field = 0;
        int base_layer_size = 0;
        dbal_field_types_defs_e ipvx_field_type = 0;

        if (ii == nof_entries_ipv4)
        {
            is_v4 = FALSE;
        }

        if (is_v4)
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_ipvx_over_mpls_parsing_predefined_entries_ipv4_fields_add(unit,
                                                                                                 &ipv4_entries[ii],
                                                                                                 ipvx_info,
                                                                                                 ipvx_info_mask));
            base_layer_size = 20;
            mpls_explicit_null_label = MPLS_LABEL_EXPLICIT_NULL_V4;
            tcam_parsing_key_field = DBAL_FIELD_IPV4_PARSING_KEYS;
            ipvx_field_type = DBAL_FIELD_TYPE_DEF_IPV4_PARSING_KEYS;
        }
        else
        {
            base_layer_size = 40;
            mpls_explicit_null_label = MPLS_LABEL_EXPLICIT_NULL_V6;
            tcam_parsing_key_field = DBAL_FIELD_IPV6_PARSING_KEYS;
            ipvx_field_type = DBAL_FIELD_TYPE_DEF_IPV6_PARSING_KEYS;
        }

        for (kk = 0; kk < nof_entries_mpls; kk++)
        {
            uint32 mpls_info = 0, mpls_info_mask = 0;

            SHR_IF_ERR_EXIT(dnx_tunnel_ipvx_over_mpls_parsing_predefined_entries_mpls_fields_add(unit,
                                                                                                 &mpls_entries[kk],
                                                                                                 mpls_explicit_null_label,
                                                                                                 &mpls_info,
                                                                                                 &mpls_info_mask));
            /*
             * Add UDP entries
             */
            for (jj = 0; jj < nof_entries_udp; jj++)
            {
                uint32 ipvx_qualifier = 0;

                SHR_IF_ERR_EXIT(dnx_tunnel_ipvx_over_mpls_parsing_predefined_entries_udp_fields_add(unit,
                                                                                                    &udp_entries[jj],
                                                                                                    is_v4,
                                                                                                    ipvx_field_type,
                                                                                                    ipvx_info,
                                                                                                    ipvx_info_mask,
                                                                                                    &ipvx_qualifier));

                SHR_IF_ERR_EXIT(dnx_tunnel_ipvx_over_mpls_parsing_add_entry
                                (unit, 1, is_v4, tcam_parsing_key_field, ipvx_info, ipvx_info_mask, mpls_info,
                                 mpls_info_mask, ipvx_qualifier, base_layer_size + udp_entries[jj].layer_size,
                                 udp_entries[jj].next_layer_type, udp_entries[jj].next_parser_context, FALSE));
            }

            /*
             * The infrastructure of the API is ready to support GRE, yet no GRE entries were defined by ARCH yet
             */
            continue;
            /*
             * Add GRE entries
             */
            /** coverity[Structurally dead code:FALSE] */
            for (jj = 0; jj < nof_entries_gre; jj++)
            {
                uint32 ipvx_qualifier = 0;

                SHR_IF_ERR_EXIT(dnx_tunnel_ipvx_over_mpls_parsing_predefined_entries_gre_fields_add(unit,
                                                                                                    &gre_entries[jj],
                                                                                                    is_v4,
                                                                                                    ipvx_field_type,
                                                                                                    ipvx_info,
                                                                                                    ipvx_info_mask,
                                                                                                    &ipvx_qualifier));

                SHR_IF_ERR_EXIT(dnx_tunnel_ipvx_over_mpls_parsing_add_entry
                                (unit, 1, is_v4, tcam_parsing_key_field, ipvx_info, ipvx_info_mask, mpls_info,
                                 mpls_info_mask, ipvx_qualifier, base_layer_size + gre_entries[jj].layer_size,
                                 gre_entries[jj].next_layer_type, gre_entries[jj].next_parser_context, FALSE));
            }
        }
    }
    /*
     * Add the default entry
     */
    {
        uint32 ipvx_info[4] = { 0 };
        uint32 ipvx_info_mask[4] = { 0 };
        uint32 mpls_info = 0;
        uint32 mpls_info_mask = 0;

        SHR_IF_ERR_EXIT(dnx_tunnel_ipvx_over_mpls_parsing_add_entry
                        (unit, 2, 0, DBAL_FIELD_IPVX_PARSING_KEYS, ipvx_info, ipvx_info_mask, mpls_info, mpls_info_mask,
                         0, 0, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION,
                         DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_NOP, TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 */
shr_error_e
dnx_tunnel_ipvx_over_mpls_parsing_clear(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_IPVX_PARSING_TCAM_DB));

exit:
    SHR_FUNC_EXIT;
}
