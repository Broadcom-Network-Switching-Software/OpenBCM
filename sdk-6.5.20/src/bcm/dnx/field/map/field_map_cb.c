/** \file field_map_cb.c
 *  *
 * Callback routines for presels, quals and acions implementing data conversion
 *
 * Routine type is always the same, and there is predefined knowledge which data type is being converted and what is
 * the size
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/l3/l3.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dbal/dbal_string_apis.h>

#include "field_map_local.h"
/*
 * }Include files
 */

/**
 * \brief  Adds a valid bit to the MSB of a lif.
 * \param [in] unit - Identifier of HW platform.
 * \param [in] lif - The value of the lif
 * \param [in] valid_bit - If TRUE, adds one as valid bit. if FALSE adds zero.
 * \param [out] lif_with_valid_p - The lif plus valid bit
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - One of input parameters is out of range
 *   \retval Other - Other errors as per shr_error_e
 * \remark
 *  We assume that the lif uses less than 32 bits.
 */
static shr_error_e
dnx_field_map_add_valid_to_lif(
    int unit,
    uint32 lif,
    int valid_bit,
    uint32 *lif_with_valid_p)
{
    uint32 nof_bits_in_lif;
    uint32 valid_bit_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_with_valid_p, _SHR_E_PARAM, "lif_with_valid_p");

    nof_bits_in_lif = dnx_data_headers.ftmh.outlif_bits_get(unit);

    /*
     * Sanity check
     */
    if (nof_bits_in_lif >= SAL_UINT32_NOF_BITS || (((int) nof_bits_in_lif) < 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "nof bits in LIF is %d, function does not support more than 31 bits.\n",
                     nof_bits_in_lif);
    }

    valid_bit_value = (valid_bit) ? (1) : (0);

    (*lif_with_valid_p) = lif | (valid_bit_value << nof_bits_in_lif);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief  Combine consecutive fields in an array to one value.
 *         for example, we combine array {0x5, 0x1, 0x1} with field sizes {4, 1, 1} to 0x35.
 * \param [in] unit - Identifier of HW platform.
 * \param [in] nof_elements - the number of elements in the arrays sizes, field_names, and fields.
 * \param [in] sizes - The size in bits of each field.
 * \param [in] field_names - The name of each field.
 * \param [in] combined_name - The name of the combined value.
 * \param [in] bcm_max_array_size - The max array size for in and out arrays (fields and combined_val).
 *             Should BCM_FIELD_ACTION_WIDTH_IN_WORDS or BCM_FIELD_QUAL_WIDTH_IN_WORDS.
 * \param [in] dnx_max_array_size - The max array size for in and out arrays (fields and combined_val).
 *             Should be DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY or DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY.
 * \param [in] fields - The array of the field values.
 * \param [out] combined_val - The encoded value.
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval Other - Other errors as per shr_error_e
 * \remark
 *  The function expects the lists of sizes and fields to be ordered from lsb to msb!
 */
static shr_error_e
dnx_field_map_encode_fields(
    int unit,
    unsigned int nof_elements,
    uint32 *sizes,
    const char *field_names[],
    const char *combined_name,
    uint32 bcm_max_array_size,
    uint32 dnx_max_array_size,
    uint32 *fields,
    uint32 *combined_val)
{
    uint32 *offsets_p = NULL;
    int index;
    uint32 output_array_total_size;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(offsets_p, sizeof(*offsets_p) * nof_elements, "offsets_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** Sanity checks. */
    if (nof_elements <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_elements of \"%s\" is zero.\n", combined_name);
    }
    if (nof_elements > bcm_max_array_size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_elements of \"%s\" (%d) exceeds maximum BCM elements %d.\n",
                     combined_name, nof_elements, bcm_max_array_size);
    }
    if (nof_elements > dnx_max_array_size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_elements of \"%s\" (%d) exceeds maximum DNX elements %d.\n",
                     combined_name, nof_elements, dnx_max_array_size);
    }

    /** Init the arrays. */
    output_array_total_size = (dnx_max_array_size * SAL_UINT32_NOF_BITS);
    offsets_p[0] = 0;
    for (index = 1; index < nof_elements; index++)
    {
        offsets_p[index] = offsets_p[index - 1] + sizes[index - 1];
        /** Sanity check. */
        if (field_names[index][0] == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %d out of %d in \"%s\" has no name.\n",
                         index, nof_elements, combined_name);
        }
        /** Sanity check. */
        if (offsets_p[index] >= output_array_total_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field \"%s\" %d out of %d in \"%s\" has offset (%d) exceeding %d.\n",
                         field_names[index], index, nof_elements, combined_name, offsets_p[index],
                         output_array_total_size);
        }
    }

    sal_memset(combined_val, 0, (sizeof(*combined_val) * dnx_max_array_size));
    /** Combine the fields together. */
    for (index = 0; index < nof_elements; index++)
    {
        /** Sanity check. */
        if (sizes[index] <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field \"%s\" %d out of %d in \"%s\" has size 0.\n",
                         field_names[index], index, nof_elements, combined_name);
        }
        if (sizes[index] != SAL_UINT32_NOF_BITS && ((((fields[index]) & (~(SAL_UPTO_BIT(sizes[index])))) != 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The given value 0x%X for field \"%s\" in \"%s\" is exceeding its size %d bits.\n",
                         fields[index], field_names[index], combined_name, sizes[index]);
        }
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(&fields[index], offsets_p[index], sizes[index], combined_val));
    }

exit:
    SHR_FREE(offsets_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief  Writes to a single subfield of an ace_context_value structure.
 *         If the subfield was not 0 before, updates as such.
 * \param [in] unit - Identifier of HW platform.
 * \param [in] subfield - The subfield to write to.
 * \param [in] value - The value to write to the subfield.
 * \param [in,out] ace_context_value_p - The structure to write to.
 * \param [in,out] is_value_before_p - If the subfield was not empty before, change to TRUE.
 *                                     Otherwise, leave unchanged.
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - One of input parameters is out of range
 *   \retval Other - Other errors as per shr_error_e
 * \remark
 *  We assume that the ACE_CONTEXT_VALUE less than 32 bits.
 */
static shr_error_e
dnx_field_map_add_single_stage_ace_context_value_and_verify(
    int unit,
    dbal_fields_e subfield,
    uint32 value,
    uint32 *ace_context_value_p,
    int *is_value_before_p)
{
    uint32 val_in_field_get;
    uint32 val_in_field_set = value;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ace_context_value_p, _SHR_E_PARAM, "ace_context_value_p");
    SHR_NULL_CHECK(is_value_before_p, _SHR_E_PARAM, "is_value_before_p");

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_ACE_CONTEXT_VALUE, subfield, &val_in_field_get, ace_context_value_p));
    if (val_in_field_get != 0)
    {
        (*is_value_before_p) = TRUE;
    }

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_ACE_CONTEXT_VALUE, subfield, &val_in_field_set, ace_context_value_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_gport_to_dst(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);

    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information(unit, gport, BCM_FORWARD_ENCAP_ID_INVALID,
                                                                    &forward_info));

    if (forward_info.fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported fwd_info_result_type, gport (0x%x)\n", gport);
    }

    *dnx_data = forward_info.destination;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_rif_intf_to_dst(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    bcm_if_t intf;
    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    intf = (bcm_if_t) (*bcm_data);
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport, intf);
    if (gport == _SHR_GPORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only FEC input is supported!\n");
    }

    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information(unit, gport, BCM_FORWARD_ENCAP_ID_INVALID,
                                                                    &forward_info));

    if (forward_info.fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported fwd_info_result_type, interface (0x%x)\n", intf);
    }

    *dnx_data = forward_info.destination;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief  Returns PP port and core for given Gport.
 * \param [in] unit - Identifier of HW platform.
 * \param [in] flags for conversion
 * \param [in] core - The core on which entry will be add.
 * \param [in] gport - Gport
 * \param [out] pp_port - The PP port used by the gport.
 * \param [out] core_id - The core of the port
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval Other - Other errors as per shr_error_e
 * \remark
 *  None
 */
static shr_error_e
dnx_field_gport_to_port_and_core(
    int unit,
    uint32 flags,
    int core,
    bcm_gport_t gport,
    uint32 *pp_port,
    uint32 *core_id)
{
    int port_iter = 0;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(pp_port, _SHR_E_PARAM, "pp_port");
    SHR_NULL_CHECK(core_id, _SHR_E_PARAM, "core_id");

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Support Local port and Trunk Gport Types and convert to PP-port. */
    if ((BCM_GPORT_IS_LOCAL(gport)) || (BCM_GPORT_IS_SYSTEM_PORT(gport)))
    {
        *pp_port = gport_info.internal_port_pp_info.pp_port[0];
        *core_id = gport_info.internal_port_pp_info.core_id[0];
    }
    else if (BCM_GPORT_IS_TRUNK(gport))
    {
        if (gport_info.internal_port_pp_info.nof_pp_ports == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "No PP-port for Trunk gport (0x%x)\n", gport);
        }
        if (core == BCM_CORE_ALL)
        {
            for (port_iter = 0; port_iter < gport_info.internal_port_pp_info.nof_pp_ports; port_iter++)
            {
                /** If one of the ports is different then we return an error. */
                if (gport_info.internal_port_pp_info.pp_port[0] != gport_info.internal_port_pp_info.pp_port[port_iter])
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "The TRUNK gport %d has different pp ports for different cores, only supported for entries by core.\n",
                                 gport);
                }
            }
        }
        for (port_iter = 0; port_iter < gport_info.internal_port_pp_info.nof_pp_ports; port_iter++)
        {
            if (gport_info.internal_port_pp_info.core_id[port_iter] == core || core == BCM_CORE_ALL)
            {
                *pp_port = (uint32) (gport_info.internal_port_pp_info.pp_port[port_iter]);
                *core_id = gport_info.internal_port_pp_info.core_id[port_iter];
                break;
            }
        }
        if (port_iter == gport_info.internal_port_pp_info.nof_pp_ports)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "No PP-port for Trunk gport (0x%x) core %d\n", gport, core);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_gport_to_port_without_core(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);
    SHR_IF_ERR_EXIT(dnx_field_gport_to_port_and_core(unit, flags, core, gport, dnx_data, &core_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_trap_gport(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    uint32 trap_qualifier;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);
    bcm_data++;
    trap_qualifier = *bcm_data;

    if (BCM_GPORT_IS_TRAP(gport))
    {
        *dnx_data = (BCM_GPORT_TRAP_GET_ID(gport) |
                     BCM_GPORT_TRAP_GET_STRENGTH(gport) << (dnx_data_field.
                                                            encoded_qual_actions_offset.trap_strength_offset_get(unit))
                     | trap_qualifier << (dnx_data_field.encoded_qual_actions_offset.trap_qualifier_offset_get(unit)));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_trap_gport_to_hw_dest(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    uint32 destination;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);

    if (!BCM_GPORT_IS_TRAP(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported trap gport_info, gport (0x%x)\n", gport);
    }

    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport(unit, 0, gport, &destination));

    *dnx_data = destination;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_ingress_sniff_gport_to_code(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    uint32 qualifier;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);
    bcm_data++;
    qualifier = *bcm_data;

    /** Expects snoop command */
    if (BCM_GPORT_IS_MIRROR(gport))
    {
        *dnx_data = ((DNX_RX_TRAP_CODE_SNIF_PROFILE_START_IND(unit) + BCM_GPORT_MIRROR_GET(gport)) |
                     qualifier << (dnx_data_field.encoded_qual_actions_offset.mirror_qualifier_offset_get(unit)));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_ingress_snoop_gport_to_code(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    uint32 qualifier;
    uint32 strength;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);
    bcm_data++;
    qualifier = *bcm_data++;
    strength = *bcm_data;

    /** Expects snoop command */
    if (BCM_GPORT_IS_MIRROR(gport))
    {
        /*
         * When mapping a snoop command out of the Mirror, the strength is encoded on an offset determined from the
         * snoop action encoding (currently equal to the trap stregth offset)
         */
        *dnx_data = ((DNX_RX_TRAP_CODE_SNIF_PROFILE_START_IND(unit) + BCM_GPORT_MIRROR_GET(gport)) |
                     strength << (dnx_data_field.encoded_qual_actions_offset.trap_strength_offset_get(unit)) |
                     qualifier << (dnx_data_field.encoded_qual_actions_offset.sniff_qualifier_offset_get(unit)));
    }
    else if (BCM_GPORT_IS_TRAP(gport))
    {
        /** Expects snoop code */
        *dnx_data =
            (BCM_GPORT_TRAP_GET_ID(gport) |
             (BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(gport) <<
              (dnx_data_field.
               encoded_qual_actions_offset.trap_strength_offset_get(unit))) | (qualifier << (dnx_data_field.
                                                                                             encoded_qual_actions_offset.sniff_qualifier_offset_get
                                                                                             (unit))));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_egress_sniff_gport_to_profile(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    int encoded_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);

    if (BCM_GPORT_IS_MIRROR(gport))
    {
        encoded_id = BCM_GPORT_MIRROR_GET(gport);
    }
    else if (BCM_GPORT_IS_TRAP(gport))
    {
        encoded_id = BCM_GPORT_TRAP_GET_ID(gport);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }

    if (DNX_RX_TRAP_EGRESS_ACTION_PROFILE_GET(encoded_id) >= dnx_data_trap.erpp.nof_erpp_snif_profiles_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Given snoop/mirror profile %d is invalid! ERPP snoop/mirror profiles can be between 0 and %d.\n",
                     DNX_RX_TRAP_EGRESS_ACTION_PROFILE_GET(encoded_id),
                     (dnx_data_trap.erpp.nof_erpp_snif_profiles_get(unit) - 1));
    }

    *dnx_data = DNX_RX_TRAP_EGRESS_SNOOP_PROFILE_GET(encoded_id);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_egress_trap_id(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    int encoded_trap_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);

    if (BCM_GPORT_IS_TRAP(gport))
    {
        encoded_trap_id = BCM_GPORT_TRAP_GET_ID(gport);
        *dnx_data = DNX_RX_TRAP_EGRESS_ACTION_PROFILE_GET(encoded_trap_id);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_egress_trap_gport_to_strength(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);

    if (BCM_GPORT_IS_TRAP(gport))
    {
        *dnx_data = BCM_GPORT_TRAP_GET_STRENGTH(gport);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_egress_snoop_gport_to_strength(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);

    if (BCM_GPORT_IS_TRAP(gport))
    {
        *dnx_data = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(gport);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_sys_port_gport_to_port(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));

    if (gport_info.sys_port != -1)
    {
        *dnx_data = gport_info.sys_port;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_gport_to_port_with_core(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    bcm_gport_t gport;
    uint32 core_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);

    if (core == BCM_CORE_ALL && BCM_GPORT_IS_TRUNK(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "You must specify core, when adding entry on CORE_ALL "
                     "with qualifier InPort and value of LAG/TRUNK gport! "
                     "As an alternative you can use bcmFieldQualifyInPortWithoutCore!\n");
    }

    SHR_IF_ERR_EXIT(dnx_field_gport_to_port_and_core(unit, flags, core, gport, dnx_data, &core_id));

    /** In Port Encoding: pp-port[bits 8:1],core[bit 0]. */
    if (nof_cores != 1)
    {
        uint32 core_shift;
        core_shift = utilex_log2_round_up(nof_cores);
        *dnx_data = ((*dnx_data << core_shift) | core_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_gport_to_global_in_lif(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    uint32 gport_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, gport_flags, &gport_hw_resources));

    if (gport_hw_resources.global_in_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }
    *dnx_data = gport_hw_resources.global_in_lif;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_gport_to_global_out_lif(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_gport_t gport;
    uint32 gport_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    gport = (bcm_gport_t) (*bcm_data);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, gport_flags, &gport_hw_resources));

    if (gport_hw_resources.global_out_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x)\n", gport);
    }
    *dnx_data = gport_hw_resources.global_out_lif;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_gport_to_global_out_lif_add_valid(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 lif_value;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    /*
     * Convert Gport to LIF.
     */
    SHR_IF_ERR_EXIT(dnx_field_convert_gport_to_global_out_lif(unit, flags, core, bcm_data, &lif_value));

    /*
     * Add a one to the MSB of the outlif as valif bit. Note we assume the size of global out lif is less than 32 bits.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_add_valid_to_lif(unit, lif_value, TRUE, dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_rif_intf_to_lif(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_if_t intf;
    /*
     * uint8 intf_exists;
     */

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    intf = (bcm_if_t) (*bcm_data);

    /*
     * Check if LIF was created.
     * We are using dnx_l3_check_if_ingress_intf_exists instead of using dnx_algo_gpm_rif_intf_to_hw_resources for the
     * case of BCM_L3_ITF_TYPE_IS_RIF(intf) and dnx_algo_gpm_gport_to_hw_resources for the case of
     * BCM_L3_ITF_TYPE_IS_LIF(intf)
     */
    /*
     * SHR_IF_ERR_EXIT(dnx_l3_check_if_ingress_intf_exists(unit, intf, &intf_exists));
     */
    if (1)
    {
        *dnx_data = BCM_L3_ITF_VAL_GET(intf);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Interface (0x%x) does not exist, or else it is not an L3 interface.\n", intf);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_rif_intf_to_lif_add_valid(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 lif_value;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    /*
     * Convert Gport to LIF.
     */
    SHR_IF_ERR_EXIT(dnx_field_convert_rif_intf_to_lif(unit, flags, core, bcm_data, &lif_value));

    /*
     * Add a one to the MSB of the outlif as valif bit. Note we assume the size of global out lif is less than 32 bits.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_add_valid_to_lif(unit, lif_value, TRUE, dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_rif_intf_to_rpf_out_lif_encoded(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 1, 1, 22 };
    const char *names[] = { "RPF_Route_Valid", "RPF_Default_Route_Found", "RPF_Out_LIF" };
    uint32 lif_value;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    /** Convert L3 interface to LIF. */
    SHR_IF_ERR_EXIT(dnx_field_convert_rif_intf_to_lif(unit, flags, core, &bcm_data[2], &lif_value));

    bcm_data[2] = lif_value;

    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "rpf_out_lif", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_gport_to_rpf_out_lif_encoded(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 1, 1, 22 };
    const char *names[] = { "RPF_Route_Valid", "RPF_Default_Route_Found", "RPF_Out_LIF" };
    uint32 lif_value;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    /** Convert Gport to LIF. */
    SHR_IF_ERR_EXIT(dnx_field_convert_gport_to_global_out_lif(unit, flags, core, &bcm_data[2], &lif_value));

    bcm_data[2] = lif_value;

    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "rpf_out_lif", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_color(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    switch (*bcm_data)
    {
        case BCM_FIELD_COLOR_GREEN:
            *dnx_data = 0;
            break;
        case BCM_FIELD_COLOR_YELLOW:
            *dnx_data = 1;
            break;
        case BCM_FIELD_COLOR_RED:
            *dnx_data = 2;
            break;
        case BCM_FIELD_COLOR_BLACK:
            *dnx_data = 3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported color_type, color (0x%x)\n", *bcm_data);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_stat_lm_index(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    switch (*bcm_data)
    {
        case bcmFieldStatOamLmIndex0:
            *dnx_data = DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_0;
            break;
        case bcmFieldStatOamLmIndex1:
            *dnx_data = DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_1;
            break;
        case bcmFieldStatOamLmIndex2:
            *dnx_data = DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_2;
            break;
        case bcmFieldStatOamLmIndexInvalid:
            *dnx_data = DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_INVALID;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported oam_lm_index (0x%x)\n", *bcm_data);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_forwarding_context(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_field_forward_context_t bcm_fwd_ctxt_type;
    int dnx_fwd_context;
    char *field_name = "EGRESS_FWD_CODE";
    char *enum_val_name = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    /*
     * No verification needed - all done previously - this is strictly internal routine
     */
    bcm_fwd_ctxt_type = (bcm_field_forward_context_t) (*bcm_data);
    if ((bcm_fwd_ctxt_type < 0) || (bcm_fwd_ctxt_type > bcmFieldForwardContextCount))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Layer Type:%d is out of range\n", bcm_fwd_ctxt_type);
    }
    switch (bcm_fwd_ctxt_type)
    {
        case bcmFieldForwardContextEth:
            enum_val_name = "ETHERNET";
            break;
        case bcmFieldForwardContextMirrorOrSs:
            enum_val_name = "MIRROR_OR_SS";
            break;
        case bcmFieldForwardContextIPv4UcR0:
            enum_val_name = "IPV4_UC_R0";
            break;
        case bcmFieldForwardContextIPv4McR0:
            enum_val_name = "IPV4_MC_R0";
            break;
        case bcmFieldForwardContextIPv6UcR0:
            enum_val_name = "IPV6_UC_R0";
            break;
        case bcmFieldForwardContextIPv6McR0:
            enum_val_name = "IPV6_MC_R0";
            break;
        case bcmFieldForwardContextBierMpls:
            enum_val_name = "BIER_MPLS";
            break;
        case bcmFieldForwardContextBierTi:
            enum_val_name = "BIER_TI";
            break;
        case bcmFieldForwardContextIngressTrapLegacy:
            enum_val_name = "INGRESS_TRAPPED_JR1_LEGACY";
            break;
        case bcmFieldForwardContextCpuPort:
            enum_val_name = "CPU_PORT";
            break;
        case bcmFieldForwardContextRchEnc:
            enum_val_name = "RCH_ENC";
            break;
        case bcmFieldForwardContextRchPtchEnc:
            enum_val_name = "RCH_PTCH_ENC";
            break;
        case bcmFieldForwardContextStackingPort:
            enum_val_name = "Stacking_Port";
            break;
        case bcmFieldForwardContextFcoeFcf:
            enum_val_name = "FCoE_FCF";
            break;
        case bcmFieldForwardContextFcoeFcfVft:
            enum_val_name = "FCoE_FCF_VFT";
            break;
        case bcmFieldForwardContextSrv6Endpoint:
            enum_val_name = "SRv6_Endpoint";
            break;
        case bcmFieldForwardContextSrv6EndpointPsp:
            enum_val_name = "SRv6_Endpoint_PSP";
            break;
        case bcmFieldForwardContextSrv6TunnelTerm:
            enum_val_name = "SRv6_Tunnel_Terminated";
            break;
        case bcmFieldForwardContextTm:
            enum_val_name = "TM";
            break;
        case bcmFieldForwardContextRawProcessing:
            enum_val_name = "RAW_PROCESSING";
            break;
        case bcmFieldForwardContextTdm:
            enum_val_name = "TDM";
            break;
        case bcmFieldForwardContextMplsInjectedFromOamp:
            enum_val_name = "MPLS_INJECTED_FROM_OAMP";
            break;
        case bcmFieldForwardContextErppTrap:
            enum_val_name = "ERPP_TRAPPED";
            break;
        case bcmFieldForwardContextDoNotEdit:
            enum_val_name = "DO_NOT_EDIT";
            break;
        case bcmFieldForwardContextMpls:
            enum_val_name = "MPLS";
            break;
        case bcmFieldForwardContextIpv4UcR1:
            enum_val_name = "IPv4_UC_R1";
            break;
        case bcmFieldForwardContextIpv4McR1:
            enum_val_name = "IPv4_MC_R1";
            break;
        case bcmFieldForwardContextIpv6UcR1:
            enum_val_name = "IPv6_UC_R1";
            break;
        case bcmFieldForwardContextIpv6McR1:
            enum_val_name = "IPv6_MC_R1";
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Fwd Ctxt Type is not supported.\n");
            break;
    }
    SHR_IF_ERR_EXIT(dnx_dbal_fields_enum_value_get(unit, field_name, enum_val_name, &dnx_fwd_context));
    *dnx_data = (uint32) dnx_fwd_context;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_forwarding_type(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_field_layer_type_t bcm_layer_type;
    dbal_enum_value_field_layer_types_e dnx_layer_type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    /*
     * No verification needed - all done previously - this is strictly internal routine
     */
    bcm_layer_type = (bcm_field_layer_type_t) (*bcm_data);
    SHR_IF_ERR_EXIT(dnx_field_map_layer_type_bcm_to_dnx(unit, bcm_layer_type, &dnx_layer_type));

    *(dbal_enum_value_field_layer_types_e *) dnx_data = dnx_layer_type;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_tunnel_type(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_field_TunnelType_t bcm_tunnel_type;
    dbal_enum_value_field_ipv4_qualifier_tunnel_type_e dnx_tunnel_type;

    static char *tunnel_type_names[bcmFieldTunnelTypeCount] = BCM_FIELD_TUNNELTYPE_STRINGS;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    /*
     * No verification needed - all done previously - this is strictly internal routine
     */
    bcm_tunnel_type = (bcm_field_TunnelType_t) (*bcm_data);
    if ((bcm_tunnel_type < 0) || (bcm_tunnel_type >= bcmFieldTunnelTypeCount))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel Type:%d is out of range\n", bcm_tunnel_type);
    }
    switch (bcm_tunnel_type)
    {
        case bcmFieldTunnelTypeGre4:
            dnx_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4;
            break;
        case bcmFieldTunnelTypeGre8WithKey:
            dnx_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE8_WITH_KEY;
            break;
        case bcmFieldTunnelTypeGre12:
            dnx_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE12;
            break;
        case bcmFieldTunnelTypeUdp:
            dnx_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_UDP;
            break;
        case bcmFieldTunnelTypeVxlan:
            dnx_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLANOUDP;
            break;
        case bcmFieldTunnelTypeGpe:
            dnx_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLAN_GPEOUDP;
            break;
        case bcmFieldTunnelTypeGeneve:
            dnx_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GENEVEOUDP;
            break;
        case bcmFieldTunnelTypeL2TPv3OverUdp:
            dnx_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_L2TPV3OUDP;
            break;
        case bcmFieldTunnelTypeEsp:
            dnx_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_ESPOIPSEC;
            break;
        case bcmFieldTunnelTypeEspOverUdp:
            dnx_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_ESPOIPSEC_UDP;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Tunnel Type:\"%s\" is not supported.\n", tunnel_type_names[bcm_tunnel_type]);
            break;
    }

    *(dbal_enum_value_field_ipv4_qualifier_tunnel_type_e *) dnx_data = dnx_tunnel_type;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_ace_context(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_field_ace_context_t bcm_ace_context;
    uint32 val_in_field;
    uint32 dnx_ace_context;
    dbal_fields_e subfield;
    int is_val_existing;
    int bcm_val_iter;
    int bcm_val_iter_2;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    /*
     * Note that we assume than for all ace_context_value enums (for all stages),
     * the NULL element has both HW value and index of 0.
     * e.g. DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_NULL and DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_NULL.
     * We also assume bcmFieldAceContextNull is 0 for initialization.
     */

    /*
     * Test that no value other than bcmFieldAceContextNull comes up more than once.
     * Note we make adding an entry take more time just to make the error slightly clearer.
     */
    for (bcm_val_iter = 0; bcm_val_iter < BCM_FIELD_ACTION_WIDTH_IN_WORDS; bcm_val_iter++)
    {
        if (bcm_data[bcm_val_iter] != bcmFieldAceContextNull)
        {
            for (bcm_val_iter_2 = 0; bcm_val_iter_2 < bcm_val_iter; bcm_val_iter_2++)
            {
                if (bcm_data[bcm_val_iter] == bcm_data[bcm_val_iter_2])
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "ACE Context Value %d appears twice, in indices %d and %d.\n",
                                 bcm_data[bcm_val_iter], bcm_val_iter_2, bcm_val_iter);
                }
            }
        }
    }

    dnx_ace_context = 0;
    for (bcm_val_iter = 0; bcm_val_iter < BCM_FIELD_ACTION_WIDTH_IN_WORDS; bcm_val_iter++)
    {
        is_val_existing = FALSE;
        bcm_ace_context = (bcm_field_ace_context_t) (bcm_data[bcm_val_iter]);
        switch (bcm_ace_context)
        {
            case bcmFieldAceContextNull:
            {
                /*
                 * Do nothing, as we assume all values are zero and we do not want
                 * bcmFieldAceContextNull to clash with anything.
                 */
                break;
            }
            case bcmFieldAceContextReflector:
            {
                subfield = DBAL_FIELD_ETPP_TERM_ACE_CTXT_VALUE;
                val_in_field = DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_REFLECTOR_UDP_SWAP;
                SHR_IF_ERR_EXIT(dnx_field_map_add_single_stage_ace_context_value_and_verify
                                (unit, subfield, val_in_field, &dnx_ace_context, &is_val_existing));
                break;
            }
            case bcmFieldAceContextForwardingCodeOverride:
            {
                subfield = DBAL_FIELD_ETPP_FWD_CODE_ACE_CTXT_VALUE;
                val_in_field = DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_J_MODE_FWD_CODE_OVERRIDE;
                SHR_IF_ERR_EXIT(dnx_field_map_add_single_stage_ace_context_value_and_verify
                                (unit, subfield, val_in_field, &dnx_ace_context, &is_val_existing));
                break;
            }
            case bcmFieldAceContextIFATrap:
            {
                subfield = DBAL_FIELD_ETPP_TRAP_ACE_CTXT_VALUE;
                val_in_field = DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_IFA_TRAP;
                SHR_IF_ERR_EXIT(dnx_field_map_add_single_stage_ace_context_value_and_verify
                                (unit, subfield, val_in_field, &dnx_ace_context, &is_val_existing));
                break;
            }
            case bcmFieldAceContextAlternateMarking:
            {
                subfield = DBAL_FIELD_ETPP_ENCAP_1_ACE_CTXT_VALUE;
                val_in_field = DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_RFC8321;
                SHR_IF_ERR_EXIT(dnx_field_map_add_single_stage_ace_context_value_and_verify
                                (unit, subfield, val_in_field, &dnx_ace_context, &is_val_existing));

                subfield = DBAL_FIELD_ETPP_ENCAP_2_ACE_CTXT_VALUE;
                val_in_field = DBAL_ENUM_FVAL_ETPP_ENCAP_2_ACE_CTXT_VALUE_RFC8321;
                SHR_IF_ERR_EXIT(dnx_field_map_add_single_stage_ace_context_value_and_verify
                                (unit, subfield, val_in_field, &dnx_ace_context, &is_val_existing));
                break;
            }
            case bcmFieldAceContextAlternateMarkingIntermediate:
            {
                subfield = DBAL_FIELD_ETPP_ENCAP_1_ACE_CTXT_VALUE;
                val_in_field = DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_PUSH_TWO_MPLS_LABELS;
                SHR_IF_ERR_EXIT(dnx_field_map_add_single_stage_ace_context_value_and_verify
                                (unit, subfield, val_in_field, &dnx_ace_context, &is_val_existing));
                break;
            }
            case bcmFieldAceContextTmToCOE:
            {
                subfield = DBAL_FIELD_ETPP_TERM_ACE_CTXT_VALUE;
                val_in_field =
                    DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_TM_TO_COE_TEMP_OR_MPLS_TRANSIT_CCM_SD_OR_L2_REFLECTOR_IP_SWAP;
                SHR_IF_ERR_EXIT(dnx_field_map_add_single_stage_ace_context_value_and_verify
                                (unit, subfield, val_in_field, &dnx_ace_context, &is_val_existing));
                subfield = DBAL_FIELD_ETPP_PRP_ACE_CTXT_VALUE;
                val_in_field =
                    DBAL_ENUM_FVAL_ETPP_PRP2_ACE_CTXT_VALUE_CANCEL_SYSTEM_HEADER_LIFS_ADD_DEST_PORT_BASED_LIF_LOOKUP;
                SHR_IF_ERR_EXIT(dnx_field_map_add_single_stage_ace_context_value_and_verify
                                (unit, subfield, val_in_field, &dnx_ace_context, &is_val_existing));
                subfield = DBAL_FIELD_ETPP_FWD_CODE_ACE_CTXT_VALUE;
                val_in_field = DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_J_MODE_FWD_CODE_OVERRIDE;
                SHR_IF_ERR_EXIT(dnx_field_map_add_single_stage_ace_context_value_and_verify
                                (unit, subfield, val_in_field, &dnx_ace_context, &is_val_existing));
                break;
            }
            case bcmFieldAceContextMplsTransitCCMSignal:
            {
                subfield = DBAL_FIELD_ETPP_TERM_ACE_CTXT_VALUE;
                val_in_field =
                    DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_TM_TO_COE_TEMP_OR_MPLS_TRANSIT_CCM_SD_OR_L2_REFLECTOR_IP_SWAP;
                SHR_IF_ERR_EXIT(dnx_field_map_add_single_stage_ace_context_value_and_verify
                                (unit, subfield, val_in_field, &dnx_ace_context, &is_val_existing));
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ACE Context Value: %d in index %d is not supported.\n",
                             bcm_ace_context, bcm_val_iter);
                break;
            }
        }
        if (is_val_existing)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "ACE Context Value %d in index %d clashes with an ACE Context Value in a lower index. "
                         "Mutually exclusive ACE Context Values found.\n", bcm_ace_context, bcm_val_iter);
        }
    }

    (*dnx_data) = dnx_ace_context;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_app_type(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_field_AppType_t app_type;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    app_type = (bcm_field_AppType_t) (*bcm_data);

    

    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_profile_get(unit, app_type, dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_app_type_predef_only(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_field_AppType_t app_type;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    app_type = (bcm_field_AppType_t) (*bcm_data);

    if (app_type >= bcmFieldAppTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Given AppType %d, but only predefined AppTypes are supported for action/qualifier.\n", app_type);
    }

    

    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_profile_get(unit, app_type, dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_ext_stat(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 20, 2 };
    const char *names[] = { "object_stat_id", "opcode" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "external_statistics", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_ext_stat_with_valid(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 20, 2, 1 };
    const char *names[] = { "object_stat_id", "opcode", "valid" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "external_statistics", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_latency_flow_id(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 1, 19, 4 };
    const char *names[] = { "valid", "latenct_flow_id", "latenct_flow_profile" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "latency_flow_id", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_stat_profile(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 1, 1, 2, 1 };
    const char *names[] = { "is_meter", "is_lm", "type", "valid" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "stat_profile", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_fwd_domain(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 18, 10 };
    const char *names[] = { "fwd_domain_id", "fwd_domain_profile" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "fwd_domain", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_oam(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 8, 8, 4, 1 };
    const char *names[] = { "oam_stamp_offset", "oam_offset", "oam_sub_type", "oam_up_mep" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "OAM", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_ipt_command(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 2, 8 };
    const char *names[] = { "int_command", "int_profile" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "IPT_command", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_packet_strip(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 2, 8 };
    const char *names[] = { "bytes_to_remove_header", "bytes_to_remove_fix" };
    bcm_field_start_packet_strip_t packet_strip;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    packet_strip = bcm_data[0];
    switch (packet_strip)
    {
        case bcmFieldStartToConfigurableStrip:
        {
            bcm_data[0] = 0;
            break;
        }
        case bcmFieldStartToL2Strip:
        {
            bcm_data[0] = 1;
            break;
        }
        case bcmFieldStartToFwdStrip:
        {
            bcm_data[0] = 2;
            break;
        }
        case bcmFieldStartToFwdNextStrip:
        {
            bcm_data[0] = 3;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Packet strip header of type %d is not supported.\n", packet_strip);
            break;
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "packet_strip", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_ace_stat_meter_object(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 5, 20 };
    const char *names[] = { "stat_meter_obj_command", "stat_meter_obj_id" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "stat_meter_obj", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_ace_stat_counter_object(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 5, 20, 1 };
    const char *names[] = { "stat_counter_obj_command", "stat_counter_obj_id", "valid" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "stat_counter_obj", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_parsing_start_offset(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    if (((bcm_data[0] << 1) >> 1) != bcm_data[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Parsing start offset value 0x%x too large, cannot add valid bit.\n", bcm_data[0]);
    }

    *dnx_data = (bcm_data[0] << 1) | 1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_parsing_start_type(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 val_in_field;
    uint32 dnx_value = 0;
    bcm_field_layer_type_t bcm_start_type_type;
    dbal_enum_value_field_parsing_start_type_encoding_e dnx_layer_type;

    static char *layer_type_names[bcmFieldLayerTypeCount] = BCM_FIELD_LAYER_TYPE_STRINGS;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    bcm_start_type_type = (bcm_field_layer_type_t) (*bcm_data);
    if ((bcm_start_type_type < 0) || (bcm_start_type_type >= bcmFieldLayerTypeCount))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Parsing start Type:%d is out of range\n", bcm_start_type_type);
    }
    switch (bcm_start_type_type)
    {
        case bcmFieldLayerTypeEth:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ETHERNET;
            break;
        case bcmFieldLayerTypeIp4:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV4;
            break;
        case bcmFieldLayerTypeIp6:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV6;
            break;
        case bcmFieldLayerTypeMpls:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS;
            break;
        case bcmFieldLayerTypeArp:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ARP;
            break;
        case bcmFieldLayerTypeFcoe:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_FCOE;
            break;
        case bcmFieldLayerTypePppoe:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_PPP;
            break;
        case bcmFieldLayerTypeSrv6Endpoint:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_ENDPOINT;
            break;
        case bcmFieldLayerTypeSrv6Beyond:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_BEYOND;
            break;
        case bcmFieldLayerTypeTm:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ITMH_VAL;
            break;
        case bcmFieldLayerTypeForwardingMPLS:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS_UNTERM;
            break;
        case bcmFieldLayerTypeUnknown:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_UNKNOWN;
            break;
        case bcmFieldLayerTypeSctp:
            dnx_layer_type = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_INGRESS_SCTP_EGRESS_FTMH;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Parsing start Type:\"%s\"(%d) is not supported.\n",
                         layer_type_names[bcm_start_type_type], bcm_start_type_type);
            break;
    }

    val_in_field = dnx_layer_type;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PARSING_START_TYPE_ACTION, DBAL_FIELD_PARSING_START_TYPE_ENCODING, &val_in_field,
                     &dnx_value));

    val_in_field = 1;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PARSING_START_TYPE_ACTION, DBAL_FIELD_VALID, &val_in_field, &dnx_value));

    (*dnx_data) = dnx_value;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_context_sys_header_profile(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_context_t context_id,
    uint32 bcm_param_value,
    struct dnx_field_context_param_e *context_param,
    dnx_field_dbal_entry_t * field_dbal_entry)
{
    dnx_field_context_sys_hdr_profile_e dnx_param_value;
    bcm_field_system_header_profile_t bcm_sys_hdr_profile;
    SHR_FUNC_INIT_VARS(unit);

    /** No verification needed - all done previously - this is strictly internal routine. */
    bcm_sys_hdr_profile = (bcm_field_system_header_profile_t) bcm_param_value;

    SHR_IF_ERR_EXIT(dnx_field_map_system_header_profile_bcm_to_dnx(unit, bcm_sys_hdr_profile, &dnx_param_value));

    field_dbal_entry->key_dbal_pair[0].field_id = context_param->key_field_id[0];
    field_dbal_entry->key_dbal_pair[0].value = context_id;
    field_dbal_entry->res_dbal_pair[0].field_id = context_param->res_field_id[0];
    field_dbal_entry->res_dbal_pair[0].value = (uint32) dnx_param_value;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_context_param_key_val(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_context_t context_id,
    uint32 param_value,
    struct dnx_field_context_param_e *context_param,
    dnx_field_dbal_entry_t * field_dbal_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    field_dbal_entry->key_dbal_pair[0].field_id = context_param->key_field_id[0];
    field_dbal_entry->key_dbal_pair[0].value = context_id;
    field_dbal_entry->key_dbal_pair[1].field_id = context_param->key_field_id[1];
    field_dbal_entry->key_dbal_pair[1].value = dnx_stage;
    field_dbal_entry->res_dbal_pair[0].field_id = context_param->res_field_id[0];
    field_dbal_entry->res_dbal_pair[0].value = param_value;

    SHR_FUNC_EXIT;
}

#define DNX_FIELD_CONTEXT_BYTES_TO_REMOVE_MASK 0x7F
#define DNX_FIELD_CONTEXT_LAYERS_TO_REMOVE_MASK 0x3
#define DNX_FIELD_CONTEXT_LAYERS_TO_REMOVE(param_val) (param_val&DNX_FIELD_CONTEXT_LAYERS_TO_REMOVE_MASK)
#define DNX_FIELD_CONTEXT_BYTES_TO_REMOVE(param_val) \
    ((param_val >> 4) & DNX_FIELD_CONTEXT_BYTES_TO_REMOVE_MASK)

shr_error_e
dnx_field_convert_context_param_header_strip(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_context_t context_id,
    uint32 param_value,
    struct dnx_field_context_param_e *context_param,
    dnx_field_dbal_entry_t * field_dbal_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    field_dbal_entry->key_dbal_pair[0].field_id = context_param->key_field_id[0];
    field_dbal_entry->key_dbal_pair[0].value = context_id;
    field_dbal_entry->res_dbal_pair[0].field_id = context_param->res_field_id[0];
    field_dbal_entry->res_dbal_pair[0].value = DNX_FIELD_CONTEXT_LAYERS_TO_REMOVE(param_value);
    field_dbal_entry->res_dbal_pair[1].field_id = context_param->res_field_id[1];
    field_dbal_entry->res_dbal_pair[1].value = DNX_FIELD_CONTEXT_BYTES_TO_REMOVE(param_value);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_vlan_format(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    dbal_enum_value_field_incoming_tag_structure_e dnx_vlan_format;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    switch (*bcm_data)
    {
        case BCM_FIELD_VLAN_FORMAT_UNTAGGED:
            dnx_vlan_format = DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_UNTAGGED_1;
            break;
        case BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED:
            dnx_vlan_format = DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_1;
            break;
        case BCM_FIELD_VLAN_FORMAT_INNER_TAGGED:
            dnx_vlan_format = DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_1;
            break;
        case BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED:
            dnx_vlan_format = DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_1;
            break;
        case BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED:
            dnx_vlan_format = DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_1;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Vlan Format:\"%d\" is not supported by DBAL\n", *bcm_data);
            break;
    }

    *(dbal_enum_value_field_incoming_tag_structure_e *) dnx_data = dnx_vlan_format;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_cs_qual_type_cascaded_group(
    int unit,
    uint32 cs_qual_index,
    dnx_field_context_t context_id,
    bcm_field_qualify_t bcm_qual,
    dbal_fields_e * cs_dnx_qual_p)
{
    dnx_field_context_t cascaded_from_context_id;
    dnx_field_key_id_t ipmf1_cascaded_from_key_id;
    dnx_field_group_type_e fg_type;
    dnx_field_stage_e field_stage;
    dnx_field_key_length_type_e key_length;
    dnx_field_group_t fg_id;
    unsigned int is_attached;

    SHR_FUNC_INIT_VARS(unit);

    fg_id = cs_qual_index;

    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    if (field_stage != DNX_FIELD_STAGE_IPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't use FG %d in cascaded preselection: FG should be in stage iPMF1\r\n", fg_id);
    }

    /*
     * Verify that the field group we cascade from is attached to the iPMF1 context.
     */
    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));
    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, fg_id, cascaded_from_context_id, &is_attached));
    if (is_attached == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Can't use FG %d in cascaded preselection for iPMF2 context %d: "
                     "Not attached to iPMF1 context %d. \n", fg_id, context_id, cascaded_from_context_id);
    }

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            /** FIXEM SDK-177457: Remove the if once the EXEM 2msb are usable. */
            if (dnx_data_field.features.exem_vmv_removable_from_payload_get(unit) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "This device does not support EXEM for cascade preselection. fg_id \r\n");
            }
            *cs_dnx_qual_p = DBAL_FIELD_PMF1_EXEM_PAYLOAD_MSB;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Can't use FG %d in cascaded preselection: "
                         "MDB FGs are not supported as preselection cascading.\r\n", fg_id);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            *cs_dnx_qual_p = DBAL_FIELD_STATE_TABLE_DATA_HW;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            dbal_enum_value_field_field_key_e key_id;
            SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get
                            (unit, fg_id, cascaded_from_context_id, &ipmf1_cascaded_from_key_id));
            SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get(unit, fg_id, &key_length));
            if (key_length == DNX_FIELD_KEY_LENGTH_TYPE_HALF)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Can't use FG %d in cascaded preselection: Half key TCAM FGs are not allowed\r\n", fg_id);
            }
            key_id =
                (key_length ==
                 DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE) ? ipmf1_cascaded_from_key_id.
                id[1] : ipmf1_cascaded_from_key_id.id[0];
            switch (key_id)
            {
                case DBAL_ENUM_FVAL_FIELD_KEY_A:
                    *cs_dnx_qual_p = DBAL_FIELD_PMF1_TCAM_ACTION_0_MSB;
                    break;
                case DBAL_ENUM_FVAL_FIELD_KEY_B:
                    *cs_dnx_qual_p = DBAL_FIELD_PMF1_TCAM_ACTION_1_MSB;
                    break;
                case DBAL_ENUM_FVAL_FIELD_KEY_C:
                    *cs_dnx_qual_p = DBAL_FIELD_PMF1_TCAM_ACTION_2_MSB;
                    break;
                case DBAL_ENUM_FVAL_FIELD_KEY_D:
                    *cs_dnx_qual_p = DBAL_FIELD_PMF1_TCAM_ACTION_3_MSB;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "TCAM FG %d is using an invalid key %d\r\n", fg_id, key_id);
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Can't use FG %d in cascaded preselection: "
                         "FG type not supported as preselection cascading\r\n", fg_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_cs_qual_type_cascaded_group_back(
    int unit,
    dnx_field_context_t context_id,
    dbal_fields_e cs_dnx_qual,
    bcm_field_qualify_t * bcm_qual_p,
    uint32 *cs_qual_index_p)
{
    dbal_enum_value_field_field_key_e cascaded_key_id;
    dnx_field_group_t fg_id_ndx;
    uint8 is_fg_allocated;
    dnx_field_group_type_e fg_type;
    dnx_field_stage_e fg_stage;
    unsigned int is_fg_on_context;
    dnx_field_context_t cascaded_from_context_id;
    dnx_field_key_id_t fg_key_ids;
    unsigned int fg_key_ndx;
    int found;

    SHR_FUNC_INIT_VARS(unit);

    switch (cs_dnx_qual)
    {
        case DBAL_FIELD_PMF1_TCAM_ACTION_0_MSB:
            cascaded_key_id = DBAL_ENUM_FVAL_FIELD_KEY_A;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_1_MSB:
            cascaded_key_id = DBAL_ENUM_FVAL_FIELD_KEY_B;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_2_MSB:
            cascaded_key_id = DBAL_ENUM_FVAL_FIELD_KEY_C;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_3_MSB:
            cascaded_key_id = DBAL_ENUM_FVAL_FIELD_KEY_D;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong DNX qualifier for this function: 0x%x. Context %d\r\n",
                         cs_dnx_qual, context_id);
    }

    /*
     * The iPMF1 context we cascade from note we assume the function operated in iPMF2).
     */
    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    found = FALSE;
    for (fg_id_ndx = 0; fg_id_ndx < dnx_data_field.group.nof_fgs_get(unit); fg_id_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_ndx, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            /** FG ID is not allocated, move to next FG ID.*/
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id_ndx, &fg_stage));
        if (fg_stage != DNX_FIELD_STAGE_IPMF1)
        {
            /** FG Does not belong to iPMF1, and cannot be cascaded from. Move to the next FG ID.*/
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id_ndx, &fg_type));
        if ((fg_type != DNX_FIELD_GROUP_TYPE_TCAM) && (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM))
        {
            /** FG does not write payload to TCAM, move to the next FG ID.*/
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr
                        (unit, fg_id_ndx, cascaded_from_context_id, &is_fg_on_context));
        if (is_fg_on_context)
        {
            SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get(unit, fg_id_ndx, cascaded_from_context_id, &fg_key_ids));
            for (fg_key_ndx = 0; fg_key_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX &&
                 fg_key_ids.id[fg_key_ndx] != DNX_FIELD_KEY_ID_INVALID; fg_key_ndx++)
            {
                if (fg_key_ids.id[fg_key_ndx] == cascaded_key_id)
                {
                    found = TRUE;
                    break;
                }
            }
            if (found)
            {
                break;
            }
        }
    }

    if (found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Could not find cascaded from field group for DNX qualifier "
                     "0x%x. Context %d\r\n", cs_dnx_qual, context_id);
    }
    *cs_qual_index_p = fg_id_ndx;
    *bcm_qual_p = bcmFieldQualifyCascadedKeyValue;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_system_header_profile(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    bcm_field_system_header_profile_t bcm_sys_hdr_profile;
    dnx_field_context_sys_hdr_profile_e dnx_sys_hdr_profile;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    /** No verification needed - all done previously - this is strictly internal routine. */
    bcm_sys_hdr_profile = (bcm_field_system_header_profile_t) (*bcm_data);

    SHR_IF_ERR_EXIT(dnx_field_map_system_header_profile_bcm_to_dnx(unit, bcm_sys_hdr_profile, &dnx_sys_hdr_profile));

    *(dnx_field_context_sys_hdr_profile_e *) dnx_data = dnx_sys_hdr_profile;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_learn_info_key_2(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 16, 2 };
    const char *names[] = { "Key_2", "App_DB_Index" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");
    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "learn_info_key_2", BCM_FIELD_ACTION_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_bare_metal_support_check(
    int unit,
    field_data_conversion_cb conversion_cb,
    int *bare_metal_support_p)
{
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(conversion_cb, _SHR_E_PARAM, "conversion_cb");
    SHR_NULL_CHECK(bare_metal_support_p, _SHR_E_PARAM, "bare_metal_support_p");
    *bare_metal_support_p = FALSE;
    rv = conversion_cb(unit, CHECK_BARE_METAL_SUPPORT, 0, NULL, NULL);
    if (rv != _SHR_E_UNAVAIL && rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Conversion CB  doesn't have BareMetal support macro check");
    }
    else if (rv == _SHR_E_NONE)
    {
        *bare_metal_support_p = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_convert_ac_in_lif_wide_data_extended(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data)
{
    uint32 sizes[] = { 32, 26, 1 };
    const char *names[] = { "data_32b", "data_26b", "hit_bit" };

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(flags);

    SHR_NULL_CHECK(bcm_data, _SHR_E_PARAM, "bcm_data");
    SHR_NULL_CHECK(dnx_data, _SHR_E_PARAM, "dnx_data");

    SHR_IF_ERR_EXIT(dnx_field_map_encode_fields(unit, (sizeof(sizes) / sizeof(sizes[0])), sizes, names,
                                                "ac_in_lif_wide_data_extended", BCM_FIELD_QUAL_WIDTH_IN_WORDS,
                                                DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY, bcm_data,
                                                dnx_data));

exit:
    SHR_FUNC_EXIT;
}
