/** \file tunnel_term_srv6.c
 *  SRV6 APIs used for DNX TUNNEL termination functionality.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/tunnel/algo_tunnel.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_in_lif_profile.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include "tunnel_term_srv6_utils.h"
#include "tunnel_term_srv6.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_srv6.h>
#include <include/bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>

/*
 * }
 */

/*
 * MACROs and ENUMs
 * {
 */

#define DNX_TUNNEL_TERM_SRV6_SID_LOCATOR_END_BIT                      (UTILEX_PP_IPV6_ADDRESS_NOF_BITS - 1)
#define DNX_TUNNEL_TERM_SRV6_SID_LOCATOR_START_BIT(_locator_nof_bits) (DNX_TUNNEL_TERM_SRV6_SID_LOCATOR_END_BIT - _locator_nof_bits + 1)
#define DNX_TUNNEL_TERM_SRV6_SID_FUNCT_END_BIT(_locator_nof_bits)     (DNX_TUNNEL_TERM_SRV6_SID_LOCATOR_START_BIT(_locator_nof_bits) - 1)
#define DNX_TUNNEL_TERM_SRV6_SID_FUNCT_START_BIT(_locator_nof_bits, _funct_nof_bits) (DNX_TUNNEL_TERM_SRV6_SID_FUNCT_END_BIT(_locator_nof_bits) - _funct_nof_bits + 1)
#define DNX_TUNNEL_TERM_SRV6_SID_ARG_END_BIT(_locator_nof_bits, _funct_nof_bits) (DNX_TUNNEL_TERM_SRV6_SID_FUNCT_START_BIT(_locator_nof_bits, _funct_nof_bits) - 1)
#define DNX_TUNNEL_TERM_SRV6_SID_ARG_START_BIT                        (0)
#define DNX_TUNNEL_TERM_SRV6_GSID_LOCATOR_PART_SIZE_BIT               (16)
#define DNX_TUNNEL_TERM_SRV6_USID_LOCATOR_PART_SIZE_BIT               (16)

/*
 * End of MACROs
 * }
 */

typedef struct dnx_tunnel_term_range_s
{
    uint32 start_range;
    uint32 end_range;
} dnx_tunnel_term_srv6_range_t;

shr_error_e
dnx_tunnel_terminator_srv6_dip_idx_type_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    dbal_enum_value_field_ipv6_dip_idx_type_e * dip_idx_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_96_LOCATOR(info))
    {
        if (DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
        {
            *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT;
        }
        else
        {
            *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16;
        }
    }
    else if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_64_LOCATOR(info))
    {
        if (DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
        {
            *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT;
        }
        else
        {
            *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16;
        }
    }
    else if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(info))
    {
        if (DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
        {
            *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16_NO_DEFAULT;
        }
        else
        {
            *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16;
        }
    }
    else if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_MICRO_SEGMENT_ID(info))
    {
        if (DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
        {
            *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16_NO_DEFAULT;
        }
        else
        {
            *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported sid format");
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_gport_to_dip_idx_type_add(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRV6_DIP_IDX_TYPES_PER_TUNNEL_GPORT_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, info->tunnel_id);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_type_get(unit, info, &dip_idx_type));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_TYPE, INST_SINGLE, dip_idx_type);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_gport_to_dip_idx_type_get(
    int unit,
    bcm_gport_t gport,
    dbal_enum_value_field_ipv6_dip_idx_type_e * dip_idx_type)
{
    uint32 entry_handle_id;
    dbal_tables_e table = DBAL_TABLE_SRV6_DIP_IDX_TYPES_PER_TUNNEL_GPORT_SW;
    uint32 dip_idx_type_uint32;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, gport);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_TYPE, INST_SINGLE, &dip_idx_type_uint32));
    *dip_idx_type = dip_idx_type_uint32;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_gport_to_dip_idx_type_delete(
    int unit,
    bcm_gport_t gport)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRV6_DIP_IDX_TYPES_PER_TUNNEL_GPORT_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, gport);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_dip_idx_type_to_locator_nof_bits(
    int unit,
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    int *locator_nof_bits)
{

    int gsid_prefix_size, usid_prefix_size;

    SHR_FUNC_INIT_VARS(unit);

    switch (dip_idx_type)
    {
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT:
            *locator_nof_bits = 96;
            break;
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16_NO_DEFAULT:
        {
            /** get the GSID Prefix (48/64), locator is Prefix + 16b Function */
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_get(unit, bcmSwitchSrv6GsidPrefixBits, &gsid_prefix_size));

            *locator_nof_bits = gsid_prefix_size + DNX_TUNNEL_TERM_SRV6_GSID_LOCATOR_PART_SIZE_BIT;

            break;
        }
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT:
            *locator_nof_bits = 64;
            break;
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16_NO_DEFAULT:
        {
            /** get the GSID Prefix (48/64), locator is Prefix + 16b Function */
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_get(unit, bcmSwitchSrv6UsidPrefixBits, &usid_prefix_size));

            *locator_nof_bits = usid_prefix_size + DNX_TUNNEL_TERM_SRV6_USID_LOCATOR_PART_SIZE_BIT;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Locator nof bits is not supported for this sid format %d", dip_idx_type);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_dip_idx_type_to_funct_nof_bits(
    int unit,
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    int *funct_nof_bits)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (dip_idx_type)
    {
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16_NO_DEFAULT:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT:
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16_NO_DEFAULT:
            *funct_nof_bits = 16;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Funct nof bits is not supported for this sid format %d", dip_idx_type);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_funct_nof_bits(
    int unit,
    bcm_tunnel_terminator_t * info,
    int *funct_nof_bits)
{
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_gport_to_dip_idx_type_get(unit, info->default_tunnel_id, &dip_idx_type));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_type_to_funct_nof_bits(unit, dip_idx_type, funct_nof_bits));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_gport_to_locator_nof_bits(
    int unit,
    bcm_gport_t gport,
    int *locator_nof_bits)
{
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_gport_to_dip_idx_type_get(unit, gport, &dip_idx_type));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_type_to_locator_nof_bits(unit, dip_idx_type, locator_nof_bits));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_funct_range(
    int unit,
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    dnx_tunnel_term_srv6_range_t * funct_range)
{
    int locator_nof_bits;
    int funct_nof_bits;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_type_to_locator_nof_bits(unit, dip_idx_type, &locator_nof_bits));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_type_to_funct_nof_bits(unit, dip_idx_type, &funct_nof_bits));
    funct_range->start_range = DNX_TUNNEL_TERM_SRV6_SID_FUNCT_START_BIT(locator_nof_bits, funct_nof_bits);
    funct_range->end_range = DNX_TUNNEL_TERM_SRV6_SID_FUNCT_END_BIT(locator_nof_bits);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_funct_dip6_mask_expected_unmasked_lsb_range(
    int unit,
    bcm_tunnel_terminator_t * info,
    dnx_tunnel_term_srv6_range_t * expected_unmasked_range)
{
    int locator_nof_bits = 0;
    int funct_nof_bits = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_gport_to_locator_nof_bits
                    (unit, info->default_tunnel_id, &locator_nof_bits));
    expected_unmasked_range->start_range = DNX_TUNNEL_TERM_SRV6_SID_ARG_START_BIT;
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_nof_bits(unit, info, &funct_nof_bits));
    expected_unmasked_range->end_range = DNX_TUNNEL_TERM_SRV6_SID_ARG_END_BIT(locator_nof_bits, funct_nof_bits);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_funct_dip6_mask_expected_unmasked_msb_range(
    int unit,
    bcm_tunnel_terminator_t * info,
    dnx_tunnel_term_srv6_range_t * expected_unmasked_range)
{
    int locator_nof_bits = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_gport_to_locator_nof_bits
                    (unit, info->default_tunnel_id, &locator_nof_bits));
    expected_unmasked_range->start_range = DNX_TUNNEL_TERM_SRV6_SID_LOCATOR_START_BIT(locator_nof_bits);
    expected_unmasked_range->end_range = DNX_TUNNEL_TERM_SRV6_SID_LOCATOR_END_BIT;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_dip6_mask_is_unmasked_in_range(
    int unit,
    bcm_tunnel_terminator_t * info,
    dnx_tunnel_term_srv6_range_t * expected_unmasked_range,
    uint8 *is_unmasked_in_range)
{
    uint32 dip6_mask_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S];
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_struct_to_long(info->dip6_mask, dip6_mask_long));
    *is_unmasked_in_range =
        (!utilex_bitstream_have_one_in_range
         (dip6_mask_long, expected_unmasked_range->start_range, expected_unmasked_range->end_range));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_funct_dip6_mask_expected_unmask_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    dnx_tunnel_term_srv6_range_t expected_unmasked_ranges[2];
    uint8 is_unmasked_in_range = FALSE;
    int expected_unmasked_range_index = 0;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(expected_unmasked_ranges, 0, sizeof(expected_unmasked_ranges));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_dip6_mask_expected_unmasked_lsb_range
                    (unit, info, &expected_unmasked_ranges[0]));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_dip6_mask_expected_unmasked_msb_range
                    (unit, info, &expected_unmasked_ranges[1]));

    for (expected_unmasked_range_index = 0; expected_unmasked_range_index < 2; expected_unmasked_range_index++)
    {
        dnx_tunnel_term_srv6_range_t expected_unmasked_range = expected_unmasked_ranges[expected_unmasked_range_index];
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip6_mask_is_unmasked_in_range
                        (unit, info, &expected_unmasked_range, &is_unmasked_in_range));
        if (!is_unmasked_in_range)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "dip6 mask is not expected to have masked bit between %d and %d",
                         expected_unmasked_range.start_range, expected_unmasked_range.end_range);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_funct_dip6_mask_expected_mask_range(
    int unit,
    bcm_tunnel_terminator_t * info,
    dnx_tunnel_term_srv6_range_t * expected_mask_range)
{
    int locator_nof_bits = 0;
    int funct_nof_bits = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_gport_to_locator_nof_bits
                    (unit, info->default_tunnel_id, &locator_nof_bits));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_nof_bits(unit, info, &funct_nof_bits));
    expected_mask_range->start_range = DNX_TUNNEL_TERM_SRV6_SID_FUNCT_START_BIT(locator_nof_bits, funct_nof_bits);
    expected_mask_range->end_range = DNX_TUNNEL_TERM_SRV6_SID_FUNCT_END_BIT(locator_nof_bits);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_create and to verify specifically for srv6 that the type is not being replaced.
 */
shr_error_e
dnx_tunnel_terminator_srv6_l2vpn_type_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 fodo,
    bcm_tunnel_type_t * tunnel_type)
{
    uint32 vsi;
    dbal_fields_e sub_field_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_FODO, fodo, &sub_field_id, &vsi));
    *tunnel_type = info->type;

    if (sub_field_id == DBAL_FIELD_VSI)
    {
        if (info->type == bcmTunnelTypeIpAnyIn6)
        {
            *tunnel_type = bcmTunnelTypeEthIn6;
        }
        else if (info->type == bcmTunnelTypeCascadedFunct)
        {
            *tunnel_type = bcmTunnelTypeEthCascadedFunct;
        }
    }
    else if (sub_field_id == DBAL_FIELD_VRF)
    {
        if (info->type == bcmTunnelTypeEthIn6)
        {
            *tunnel_type = bcmTunnelTypeIpAnyIn6;
        }
        else if (info->type == bcmTunnelTypeEthCascadedFunct)
        {
            *tunnel_type = bcmTunnelTypeCascadedFunct;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_create and to verify specifically for srv6 that the type is not being replaced.
 */
shr_error_e
dnx_tunnel_terminator_srv6_l2vpn_replace_type_verify(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 fodo)
{
    bcm_tunnel_type_t tunnel_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_l2vpn_type_get(unit, info, fodo, &tunnel_type));
    /** Check if the tunnel_type hasn't been updated based on the FODO output*/
    if (tunnel_type != info->type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Replacing the tunnel type is not allowed");
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_dip6_mask_is_fully_masked_in_range(
    int unit,
    bcm_tunnel_terminator_t * info,
    dnx_tunnel_term_srv6_range_t * expected_masked_range,
    uint8 *is_fully_masked_in_range)
{
    uint32 dip6_mask_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S];
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_struct_to_long(info->dip6_mask, dip6_mask_long));
    *is_fully_masked_in_range =
        (!utilex_bitstream_have_zero_in_range
         (dip6_mask_long, expected_masked_range->start_range, expected_masked_range->end_range));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_funct_dip6_mask_expected_mask_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    dnx_tunnel_term_srv6_range_t expected_mask_range;
    uint8 is_fully_masked_in_range = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&expected_mask_range, 0, sizeof(dnx_tunnel_term_srv6_range_t));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_dip6_mask_expected_mask_range(unit, info, &expected_mask_range));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip6_mask_is_fully_masked_in_range
                    (unit, info, &expected_mask_range, &is_fully_masked_in_range));
    if (!is_fully_masked_in_range)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "dip6 mask is expected to be fully mased between %d and %d",
                     expected_mask_range.start_range, expected_mask_range.end_range);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_funct_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_dip6_mask_expected_unmask_verify(unit, info));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_dip6_mask_expected_mask_verify(unit, info));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_info_to_dip_idx_type(
    int unit,
    bcm_tunnel_terminator_t * info,
    dbal_enum_value_field_ipv6_dip_idx_type_e * dip_idx_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_96_LOCATOR(info))
    {
        *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16;
    }
    else if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_64_LOCATOR(info))
    {
        *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16;
    }
    else if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(info))
    {
        *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16;
    }
    else if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_MICRO_SEGMENT_ID(info))
    {
        *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Sid format not supported for this flags %d", info->flags);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_locator_nof_bits(
    int unit,
    bcm_tunnel_terminator_t * info,
    int *locator_nof_bits)
{
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_info_to_dip_idx_type(unit, info, &dip_idx_type));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_type_to_locator_nof_bits(unit, dip_idx_type, locator_nof_bits));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_locator_dip6_mask_expected_unmasked_range(
    int unit,
    bcm_tunnel_terminator_t * info,
    dnx_tunnel_term_srv6_range_t * expected_unmasked_range)
{
    int locator_nof_bits = 0;
    SHR_FUNC_INIT_VARS(unit);
    expected_unmasked_range->start_range = DNX_TUNNEL_TERM_SRV6_SID_ARG_START_BIT;
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_locator_nof_bits(unit, info, &locator_nof_bits));
    expected_unmasked_range->end_range = DNX_TUNNEL_TERM_SRV6_SID_FUNCT_END_BIT(locator_nof_bits);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_locator_dip6_mask_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    dnx_tunnel_term_srv6_range_t expected_unmasked_range;
    uint8 is_unmasked_in_range = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&expected_unmasked_range, 0, sizeof(dnx_tunnel_term_srv6_range_t));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_locator_dip6_mask_expected_unmasked_range
                    (unit, info, &expected_unmasked_range));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip6_mask_is_unmasked_in_range
                    (unit, info, &expected_unmasked_range, &is_unmasked_in_range));
    if (!is_unmasked_in_range)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "dip6 mask is not expected to have masked bit between %d and %d",
                     expected_unmasked_range.start_range, expected_unmasked_range.end_range);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_locator_flags_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint32 locator_flags_array[] =
        { BCM_TUNNEL_TERM_UP_TO_96_LOCATOR_SEGMENT_ID, BCM_TUNNEL_TERM_UP_TO_64_LOCATOR_SEGMENT_ID,
        BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID, BCM_TUNNEL_TERM_MICRO_SEGMENT_ID
    };
    int locator_flags_count_set = 0, index;
    SHR_FUNC_INIT_VARS(unit);
    for (index = 0; index < (sizeof(locator_flags_array) / sizeof(uint32)); index++)
    {
        if (_SHR_IS_FLAG_SET(info->flags, (locator_flags_array[index])))
        {
            locator_flags_count_set++;
        }
    }
    if (locator_flags_count_set > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "too many locator flags are set at the same time");
    }
    if (DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
    {
        if ((DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_96_LOCATOR(info) ||
             DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_64_LOCATOR(info) ||
             DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(info) ||
             DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_MICRO_SEGMENT_ID(info)) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM must be set with 96/64 or GSID/USID locator");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_locator_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_locator_dip6_mask_verify(unit, info));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_locator_flags_verify(unit, info));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_locator_funct_conflict_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that GSID End-of-Compression flag can come either with is GSID flag (i.e Locator LIF), or with
     * Cascaded flag (i.e as Function LIF), but not both
     */
    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID_EOC(info) &&
        !DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(info) &&
        !DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_FUNCT(info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID_END_OF_COMPRESSION flag cannot exist without either BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID"
                     " or bcmTunnelTypeCascadedFunct");
    }

    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID_EOC(info) &&
        DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(info) && DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_FUNCT(info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID_END_OF_COMPRESSION flag cannot exist with both BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID"
                     " and CascadedFunct");
    }

    /** Verification for gSID: If the cascaded LIF is gSID EOC, the default LIF must be gSID*/
    if (BCM_GPORT_IS_TUNNEL(info->default_tunnel_id) &&
        DNX_TUNNEL_TERM_SRV6_IS_CASCADED_TYPE(info->type) &&
        DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID_EOC(info))
    {
        uint32 global_inlif = BCM_GPORT_TUNNEL_ID_GET(info->default_tunnel_id);
        uint32 dip_idx_type_flags = 0;

        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_dip_idx_type_flag_get
                        (unit, global_inlif, &dip_idx_type_flags));
        if (!_SHR_IS_FLAG_SET(dip_idx_type_flags, BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The cascaded LIF is gSID EOC, but the default LIF (gport: 0x%08X) is not gSID!",
                         info->default_tunnel_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_key_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** check that locator and function are not conflicting in flags and definitions */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_locator_funct_conflict_verify(unit, info));

    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_LOCATOR(info) || DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_locator_verify(unit, info));
    }

    /** check if the SID is compatible with expected function bits */
    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_FUNCT(info))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_verify(unit, info));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_create_vsi_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint8 tunnel_type_is_l2vpn = DNX_TUNNEL_TERM_SRV6_IS_L2_VPN(info->type);

    SHR_FUNC_INIT_VARS(unit);

    if ((info->vlan != 0) && (!tunnel_type_is_l2vpn))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "vlan (%d) is supported only for tunnel types bcmTunnelTypeEthIn6 and bcmTunnelTypeEthCascadedFunct",
                     info->vlan);
    }
    if ((info->vlan == 0) && tunnel_type_is_l2vpn && !(_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_CROSS_CONNECT)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "tunnel types bcmTunnelTypeEthIn6 and bcmTunnelTypeEthCascadedFunct must set with a non-zero vlan if CROSS_CONNECT flag is not present");
    }

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_tunnel_terminator_srv6_type_is_supported_with_usid(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint8 *is_supported_with_usid)
{
    switch (tunnel_type)
    {
        case bcmTunnelTypeUdp6:
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeEthIn6:
            *is_supported_with_usid = TRUE;
            break;
        default:
            *is_supported_with_usid = FALSE;
            break;
    }
}

static void
dnx_tunnel_terminator_srv6_type_is_supported_with_extended_termination_and_cross_connect(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint8 *is_supported_with_extended_term_and_cross_connect)
{
    switch (tunnel_type)
    {
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeEthIn6:
        case bcmTunnelTypeCascadedFunct:
        case bcmTunnelTypeEthCascadedFunct:
            *is_supported_with_extended_term_and_cross_connect = TRUE;
            break;
        default:
            *is_supported_with_extended_term_and_cross_connect = FALSE;
            break;
    }
}

shr_error_e
dnx_tunnel_terminator_srv6_lif_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint8 type_is_supported_with_usid, type_is_supported_with_extended_termination_and_cross_connect;

    SHR_FUNC_INIT_VARS(unit);

    dnx_tunnel_terminator_srv6_type_is_supported_with_extended_termination_and_cross_connect(unit, info->type,
                                                                                             &type_is_supported_with_extended_termination_and_cross_connect);

    /** Check if the USID flag is used with tunnel type, which supports it*/
    dnx_tunnel_terminator_srv6_type_is_supported_with_usid(unit, info->type, &type_is_supported_with_usid);
    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_MICRO_SEGMENT_ID(info) && !type_is_supported_with_usid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "uSID is supported only with tunnel type bcmTunnelTypeIpAnyIn6 and bcmTunnelTypeUdp6");
    }

    /** Verify Ext termination is used only for SRv6 tunnels*/
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_EXTENDED_TERMINATION)
        && !type_is_supported_with_extended_termination_and_cross_connect)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_TUNNEL_TERM_EXTENDED_TERMINATION is supported only with tunnel types bcmTunnelTypeIpAnyIn6 and bcmTunnelTypeEthIn6 not with %d",
                     info->type);
    }

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_create_vsi_verify(unit, info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_funct_value_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *funct)
{
    int locator_nof_bits = 0;
    int funct_nof_bits = 0;
    uint32 start_bit;
    uint32 dip6_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_gport_to_locator_nof_bits
                    (unit, info->default_tunnel_id, &locator_nof_bits));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_nof_bits(unit, info, &funct_nof_bits));
    start_bit = UTILEX_PP_IPV6_ADDRESS_NOF_BITS - locator_nof_bits - funct_nof_bits;
    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_struct_to_long(info->dip6, dip6_long));
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(dip6_long, start_bit, funct_nof_bits, funct));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ipv6_srv6_set_funct_bits_in_ip6(
    int unit,
    bcm_tunnel_terminator_t * tunnel_info,
    uint32 funct)
{
    int locator_nof_bits = 0;
    int funct_nof_bits = 0;
    uint32 start_bit;
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_gport_to_dip_idx_type_get
                    (unit, tunnel_info->default_tunnel_id, &dip_idx_type));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_type_to_locator_nof_bits(unit, dip_idx_type, &locator_nof_bits));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_type_to_funct_nof_bits(unit, dip_idx_type, &funct_nof_bits));
    start_bit = UTILEX_PP_IPV6_ADDRESS_NOF_BITS - locator_nof_bits - funct_nof_bits;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field((uint32 *) (tunnel_info->dip6), start_bit, funct_nof_bits, funct));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_dip_idx_data_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *dip_idx_data)
{
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t tunnel_term_gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tunnel_term_gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, info->default_tunnel_id, lif_flags, &tunnel_term_gport_hw_resources));
    *dip_idx_data = tunnel_term_gport_hw_resources.local_in_lif;
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * For additional headers SRv6 we have two cases:
 * DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6 and DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6_SL_0
 * Both are valid under that tunnel_type, and they differ by the LSB only.
 * Thus use mask of 5b'1110
 */
void
dnx_tunnel_terminator_srv6_ipv6_tcam_table_key_set_first_additional_header_set(
    int unit,
    uint32 entry_handle_id,
    uint32 first_additional_header)
{
    int additional_header_srv6_mask = 0x1E;
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                                      first_additional_header, additional_header_srv6_mask);
}

void
dnx_tunnel_terminator_srv6_ipv6_tcam_table_result_is_lif(
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    uint8 *result_is_lif)
{
    *result_is_lif = UTILEX_NUM2BOOL((dip_idx_type ==
                                      DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16)
                                     || (dip_idx_type ==
                                         DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT)
                                     || (dip_idx_type ==
                                         DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16)
                                     || (dip_idx_type ==
                                         DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT)
                                     || (dip_idx_type == DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16)
                                     || (dip_idx_type ==
                                         DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16_NO_DEFAULT)
                                     || (dip_idx_type == DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16)
                                     || (dip_idx_type ==
                                         DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16_NO_DEFAULT));
}

void
dnx_tunnel_terminator_srv6_ipv6_tcam_result_flag_resolve(
    int unit,
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    uint32 *flags)
{
    if (dip_idx_type == DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT ||
        dip_idx_type == DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT ||
        dip_idx_type == DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16_NO_DEFAULT ||
        dip_idx_type == DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16_NO_DEFAULT)
    {
        *flags |= BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM;
    }
}

void
dnx_tunnel_terminator_srv6_to_ipv6_tcam_table_result_dbal_field(
    bcm_tunnel_terminator_t * info,
    dbal_enum_value_field_ipv6_dip_idx_type_e * dip_idx_type_value)
{
    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_96_LOCATOR(info))
    {
        if (DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
        {
            *dip_idx_type_value = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT;
        }
        else
        {
            *dip_idx_type_value = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16;
        }
    }
    else if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_64_LOCATOR(info))
    {
        if (DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
        {
            *dip_idx_type_value = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT;
        }
        else
        {
            *dip_idx_type_value = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16;
        }
    }
    else if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(info))
    {
        if (DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
        {
            *dip_idx_type_value = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16_NO_DEFAULT;
        }
        else
        {
            *dip_idx_type_value = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16;
        }
    }
    else if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_MICRO_SEGMENT_ID(info))
    {
        if (DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
        {
            *dip_idx_type_value = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16_NO_DEFAULT;
        }
        else
        {
            *dip_idx_type_value = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16;
        }
    }
    else
    {
        *dip_idx_type_value = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_LIF_ELIGIBLE;
    }
}

static shr_error_e
dnx_tunnel_terminator_srv6_lookup_ipv6_cascaded_funct_key_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info)
{
    uint32 funct = 0, dip_idx_data = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_value_get(unit, info, &funct));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_16, funct);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_data_get(unit, info, &dip_idx_data));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_MP_TUNNEL_IDX, dip_idx_data);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_lookup_add_or_delete_ipv6_cascaded_funct_key_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_lookup_ipv6_cascaded_funct_key_set(unit, entry_handle_id, info));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_lookup_add_ipv6_cascaded_funct(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 local_inlif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_TT_MP_EM_16, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_lookup_add_or_delete_ipv6_cascaded_funct_key_set
                    (unit, entry_handle_id, info));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);
    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_lookup_delete_ipv6_cascaded_funct(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_TT_MP_EM_16, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_lookup_add_or_delete_ipv6_cascaded_funct_key_set
                    (unit, entry_handle_id, info));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_lookup_get_ipv6_cascaded_funct_key_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_lookup_ipv6_cascaded_funct_key_set(unit, entry_handle_id, info));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_lookup_get_ipv6_cascaded_funct(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *local_inlif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_TT_MP_EM_16, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_lookup_get_ipv6_cascaded_funct_key_set(unit, entry_handle_id, info));
    /** Set pointer to receive field - {in LIF} */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);
    /** Read from DBAL - check the returned value in calling function */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_optional_in_lif_profile_info_set(
    int unit,
    bcm_tunnel_terminator_t * info,
    in_lif_profile_info_t * in_lif_profile_info)
{
    uint32 flags = 0;
    uint32 global_inlif = BCM_GPORT_TUNNEL_ID_GET(info->default_tunnel_id);
    SHR_FUNC_INIT_VARS(unit);

    /** Used for the function lif creation - if we created the default LIF with USID type,
     * then we should allow TCP/UDP termination for  the function LIF as well
     */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_dip_idx_type_flag_get(unit, global_inlif, &flags));

    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID_EOC(info))
    {
        in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags = DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_SRV6_GSID;
    }
    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID_EOC(info)
        || DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_MICRO_SEGMENT_ID(info)
        || (_SHR_IS_FLAG_SET(flags, BCM_TUNNEL_TERM_MICRO_SEGMENT_ID)))
    {
        in_lif_profile_info->ingress_fields.allow_udp_tcp_above_termination = TRUE;
    }
exit:
    SHR_FUNC_EXIT;
}

void
dnx_tunnel_terminator_srv6_optional_in_lif_profile_update(
    bcm_tunnel_terminator_t * info,
    in_lif_profile_info_t * in_lif_profile_info)
{
    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID_EOC(info))
    {
        in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags = DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_SRV6_GSID;
    }
    else
    {
        in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags &=
            ~DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_SRV6_GSID;
    }
}
shr_error_e
dnx_tunnel_terminator_srv6_lif_reclassify(
    int unit,
    int global_lif,
    uint32 local_inlif)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_global_lif_reclassify_write(unit, global_lif, DBAL_CORE_ALL, local_inlif, FALSE));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_optional_extended_termination_flags_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 global_inlif,
    uint32 *srv6_flags)
{
    uint8 type_is_supported_with_extended_termination_and_cross_connect;

    SHR_FUNC_INIT_VARS(unit);

    dnx_tunnel_terminator_srv6_type_is_supported_with_extended_termination_and_cross_connect(unit, info->type,
                                                                                             &type_is_supported_with_extended_termination_and_cross_connect);
    if (type_is_supported_with_extended_termination_and_cross_connect)
    {
        uint8 ext_term_found;
        SHR_IF_ERR_EXIT(dnx_global_lif_reclassify_get(unit, global_inlif, DBAL_CORE_DEFAULT, &ext_term_found));
        if (ext_term_found)
        {
            *srv6_flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_optional_gsid_eoc_flags_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 in_lif_profile,
    uint32 *srv6_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check if it is a GSID EOC
     */
    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(info) || DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_FUNCT(info))
    {
        in_lif_profile_info_t in_lif_profile_info;
        SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                        (unit, in_lif_profile, &in_lif_profile_info, DBAL_TABLE_IN_LIF_IPvX_TUNNELS, LIF));
        if (in_lif_profile_info.
            ingress_fields.cs_in_lif_profile_flags & DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_WITH_SRV6_GSID)
        {
            *srv6_flags |= BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID_END_OF_COMPRESSION;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_optional_flags_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 global_inlif,
    uint32 in_lif_profile,
    uint32 *srv6_flags)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_extended_termination_flags_get
                    (unit, info, global_inlif, srv6_flags));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_gsid_eoc_flags_get(unit, info, in_lif_profile, srv6_flags));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_optional_gport_to_dip_idx_type_delete(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 global_inlif)
{
    SHR_FUNC_INIT_VARS(unit);
    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_LOCATOR(info))
    {
        bcm_gport_t tunnel_gport;
        BCM_GPORT_TUNNEL_ID_SET(tunnel_gport, global_inlif);
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_gport_to_dip_idx_type_delete(unit, tunnel_gport));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_optional_reclassify_delete(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 global_inlif)
{
    uint8 type_is_supported_with_extended_termination_and_cross_connect;

    SHR_FUNC_INIT_VARS(unit);
    dnx_tunnel_terminator_srv6_type_is_supported_with_extended_termination_and_cross_connect(unit, info->type,
                                                                                             &type_is_supported_with_extended_termination_and_cross_connect);
    if (type_is_supported_with_extended_termination_and_cross_connect)
    {
        uint8 ext_term_found;
        SHR_IF_ERR_EXIT(dnx_global_lif_reclassify_get(unit, global_inlif, DBAL_CORE_DEFAULT, &ext_term_found));
        if (ext_term_found)
        {
            SHR_IF_ERR_EXIT(dnx_global_lif_reclassify_delete(unit, global_inlif, DBAL_CORE_ALL));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_optional_delete(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 global_inlif)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_reclassify_delete(unit, info, global_inlif));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_gport_to_dip_idx_type_delete(unit, info, global_inlif));
exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_tunnel_terminator_get_dip6_as_funct(
    int unit,
    uint32 funct,
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    bcm_ip6_t dip6)
{
    uint32 dip6_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S];
    int funct_nof_bits;
    dnx_tunnel_term_srv6_range_t funct_range;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&funct_range, 0, sizeof(dnx_tunnel_term_srv6_range_t));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_range(unit, dip_idx_type, &funct_range));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_type_to_funct_nof_bits(unit, dip_idx_type, &funct_nof_bits));
    sal_memset(dip6_long, 0, sizeof(dip6_long));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(&funct, funct_range.start_range, funct_nof_bits, dip6_long));
    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_long_to_struct(dip6_long, dip6));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_get_dip6_mask_as_funct(
    int unit,
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    bcm_ip6_t dip6_mask)
{
    uint32 dip6_mask_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S];
    dnx_tunnel_term_srv6_range_t funct_range;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&funct_range, 0, sizeof(dnx_tunnel_term_srv6_range_t));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_funct_range(unit, dip_idx_type, &funct_range));
    sal_memset(dip6_mask_long, 0, sizeof(dip6_mask_long));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(dip6_mask_long, funct_range.start_range, funct_range.end_range));
    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_long_to_struct(dip6_mask_long, dip6_mask));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_dip_idx_data_to_gport(
    int unit,
    uint32 dip_idx_data,
    bcm_gport_t * gport)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, _SHR_CORE_ALL, dip_idx_data, gport));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_traverse_funct_key_fields(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info)
{
    uint32 funct = 0, dip_idx_data = 0;
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get default tunnel id
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_MP_TUNNEL_IDX, &dip_idx_data));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_dip_idx_data_to_gport(unit, dip_idx_data, &info->default_tunnel_id));
    /*
     * get funct bits in dip6
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_16, &funct));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_gport_to_dip_idx_type_get(unit, info->default_tunnel_id, &dip_idx_type));

    /*
     * update dip6 
     */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_get_dip6_as_funct(unit, funct, dip_idx_type, info->dip6));
    /*
     * update dip6 mask 
     */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_get_dip6_mask_as_funct(unit, dip_idx_type, info->dip6_mask));

    info->type = bcmTunnelTypeCascadedFunct;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_gport_to_dip_idx_type_get_if_exist(
    int unit,
    bcm_gport_t gport,
    dbal_enum_value_field_ipv6_dip_idx_type_e * dip_idx_type,
    uint8 *is_found)
{
    uint32 entry_handle_id;
    uint32 dip_idx_type_uint32;
    shr_error_e rv;
    dbal_tables_e table = DBAL_TABLE_SRV6_DIP_IDX_TYPES_PER_TUNNEL_GPORT_SW;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, gport);
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    *is_found = (rv == _SHR_E_NONE);
    if (!*is_found)
    {
        /*
         * default value to avoid compilation errors
         */
        *dip_idx_type = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_TYPE, INST_SINGLE, &dip_idx_type_uint32));
    *dip_idx_type = dip_idx_type_uint32;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_srv6_dip_idx_type_flags_get(
    int unit,
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (dip_idx_type)
    {
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16:
            *flags |= BCM_TUNNEL_TERM_UP_TO_96_LOCATOR_SEGMENT_ID;
            break;
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16:
            *flags |= BCM_TUNNEL_TERM_UP_TO_64_LOCATOR_SEGMENT_ID;
            break;
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16:
            *flags |= BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID;
            break;
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16:
            *flags |= BCM_TUNNEL_TERM_MICRO_SEGMENT_ID;
            break;
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT:
            *flags |= BCM_TUNNEL_TERM_UP_TO_96_LOCATOR_SEGMENT_ID | BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM;
            break;
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT:
            *flags |= BCM_TUNNEL_TERM_UP_TO_64_LOCATOR_SEGMENT_ID | BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM;
            break;
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_GSID_16_NO_DEFAULT:
            *flags |= BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID | BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM;
            break;
        case DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_USID_16_NO_DEFAULT:
            *flags |= BCM_TUNNEL_TERM_MICRO_SEGMENT_ID | BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported sid format");
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_srv6_optional_dip_idx_type_flag_get(
    int unit,
    uint32 global_inlif,
    uint32 *flags)
{
    bcm_gport_t tunnel_id;
    /*
     * default value to avoid compilation errors
     */
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type =
        DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16;
    uint8 is_found_gport_to_dip_idx_type = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * global inlif to gport
     */
    if (global_inlif == LIF_MNGR_INVALID)
    {
        SHR_EXIT();
    }
    BCM_GPORT_TUNNEL_ID_SET(tunnel_id, global_inlif);
    /*
     * gport to sid format
     */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_gport_to_dip_idx_type_get_if_exist
                    (unit, tunnel_id, &dip_idx_type, &is_found_gport_to_dip_idx_type));
    if (!is_found_gport_to_dip_idx_type)
    {
        SHR_EXIT();
    }
    /*
     * flag update
     */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_dip_idx_type_flags_get(unit, dip_idx_type, flags));

exit:
    SHR_FUNC_EXIT;
}
