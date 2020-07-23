/**
 * \file bcm/dnx/field/field_port.c
 *
 * Field Processor utilities related to various port profiles conntected to the field processor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *  Exported functions have their descriptions in the field_port.h file.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

/*
 * Includes
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_port.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>

/*
 * }
 */
/*
 * typedefs
 * {
 */

/*
 * }
 */

/*
 * { MACROs
 */

/*
 * }
 */

/**
 * See procedure header in field_port.h
 */
shr_error_e
dnx_field_port_profile_bits_get(
    int unit,
    dnx_field_port_porfile_type_e dnx_profile_type,
    unsigned int *start_bit_p,
    unsigned int *nof_bits_p)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (dnx_profile_type)
    {
        case DNX_FIELD_PORT_PROFILE_TYPE_PORT:
        {
            (*start_bit_p) = 0;
            (*nof_bits_p) = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
            break;
        }
        case DNX_FIELD_PORT_PROFILE_TYPE_PORT_GENERAL_DATA:
        {
            (*start_bit_p) = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
            if (dnx_data_field.profile_bits.nof_bits_in_ingress_pp_port_general_data_get(unit) > SAL_UINT32_NOF_BITS)
            {
                (*nof_bits_p) = SAL_UINT32_NOF_BITS;
            }
            else
            {
                (*nof_bits_p) = dnx_data_field.profile_bits.nof_bits_in_ingress_pp_port_general_data_get(unit);
            }
            break;
        }
        case DNX_FIELD_PORT_PROFILE_TYPE_PORT_GENERAL_DATA_HIGH:
        {
            if (dnx_data_field.profile_bits.nof_bits_in_ingress_pp_port_general_data_get(unit) > SAL_UINT32_NOF_BITS)
            {
                (*start_bit_p) = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit) + SAL_UINT32_NOF_BITS;
                (*nof_bits_p) = dnx_data_field.profile_bits.nof_bits_in_ingress_pp_port_general_data_get(unit) -
                    SAL_UINT32_NOF_BITS;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Port class type %d. Not enough bits (%d) for \"High\" option.\n",
                             dnx_profile_type,
                             dnx_data_field.profile_bits.nof_bits_in_ingress_pp_port_general_data_get(unit));
            }
            break;
        }
        case DNX_FIELD_PORT_PROFILE_TYPE_IN_LIF_INGRESS:
        {
            /*
             * Note we could have used DNX DATA directly, but instead we are using the defines used by in_lif_profile.
             */
            (*start_bit_p) = IN_LIF_PROFILE_PMF_RESERVED_START_BIT;
            (*nof_bits_p) = IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS;
            break;
        }
        case DNX_FIELD_PORT_PROFILE_TYPE_IN_ETH_RIF_INGRESS:
        {
            /*
             * Note we could have used DNX DATA directly, but instead we are using the defines used by in_lif_profile.
             */
            (*start_bit_p) = ETH_RIF_PROFILE_PMF_RESERVED_START_BIT;
            (*nof_bits_p) = ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS;
            break;
        }
        default:
        {
           /**
            * None of the supported types
            */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid profile Type: %d.\n", dnx_profile_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief        Set the PMF data for inLIF profile attached to a given inLIF.
* \param [in] unit - Device Id
* \param [in] port - InLIf-Id (Gport)
* \param [in] field_profile - PMF data to set in the InLIF profile
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
static shr_error_e
dnx_field_port_profile_inlif_set(
    int unit,
    bcm_gport_t port,
    uint32 field_profile)
{
    int old_in_lif_profile, new_in_lif_profile;
    uint32 result_type;
    in_lif_intf_type_e intf_type;
    unsigned int start_bit, nof_bits;
    int mask;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_port_profile_bits_get
                    (unit, DNX_FIELD_PORT_PROFILE_TYPE_IN_LIF_INGRESS, &start_bit, &nof_bits));

    if (BCM_GPORT_IS_SET(port))
    {
        /*
         * In case of InLIF-profile indexing
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_vsi_profile_get
                        (unit, port, (uint32 *) &old_in_lif_profile, &result_type, &intf_type));
        mask = (((1 << nof_bits) - 1) << start_bit);
        new_in_lif_profile = (old_in_lif_profile & ~mask) | ((field_profile << start_bit) & mask);
        SHR_IF_ERR_EXIT(dnx_in_lif_vsi_profile_set(unit, port, new_in_lif_profile, result_type, &intf_type));

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport (0x%x) is not legal\n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief        Get the PMF data for inLIF profile attached to a given inLIF.
* \param [in] unit - Device Id
* \param [in] port - InLIf-Id (Gport)
* \param [out] field_profile_p - PMF data for the InLIF profile
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
static shr_error_e
dnx_field_port_profile_inlif_get(
    int unit,
    bcm_gport_t port,
    uint32 *field_profile_p)
{
    int in_lif_profile;
    uint32 result_type;
    in_lif_intf_type_e intf_type;
    unsigned int start_bit, nof_bits;
    int mask;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_port_profile_bits_get
                    (unit, DNX_FIELD_PORT_PROFILE_TYPE_IN_LIF_INGRESS, &start_bit, &nof_bits));

    if (BCM_GPORT_IS_SET(port))
    {
        /*
         * In case of InLIF-profile indexing
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_vsi_profile_get(unit, port, (uint32 *) &in_lif_profile, &result_type, &intf_type));
        mask = (((1 << nof_bits) - 1) << start_bit);
        *field_profile_p = (in_lif_profile & mask) >> start_bit;

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport (0x%x) is not legal\n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief        Set the PMF data for outLIF profile attached to a given OutLIF.
* \param [in] unit - Device Id
* \param [in] port -OutLIf-Id (Gport)
* \param [in] field_profile - PMF data to set in the OutLIF profile
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
static shr_error_e
dnx_field_port_profile_outlif_set(
    int unit,
    bcm_gport_t port,
    uint32 field_profile)
{
    int new_out_lif_profile;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_SET(port))
    {

        if (BCM_GPORT_IS_TUNNEL(port))
        {
            /*
             * OUTRIF or TUNNEL ID (4b profile, only msb is reserved for field usage)
             */
            SHR_IF_ERR_EXIT(dnx_out_lif_profile_get(unit, port, &new_out_lif_profile));
            new_out_lif_profile |= (field_profile << (dnx_data_lif.out_lif.outrif_profile_width_get(unit) - 1));
        }
        else
        {
            /*
             * OUTLIF  (1b profile for field)
             */
            uint32 outlif_profile_width = dnx_data_lif.out_lif.outlif_profile_width_get(unit);
            if (outlif_profile_width <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "0 bits alocated for outlif profile (%d bits)\n", field_profile);
            }
            else if ((outlif_profile_width < SAL_UINT32_NOF_BITS) &&
                     (((SAL_UPTO_BIT(outlif_profile_width)) & field_profile) != field_profile))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "profile value 0x%x is too large for outlif profile (%d bits)\n",
                             field_profile, dnx_data_lif.out_lif.outlif_profile_width_get(unit));
            }
            new_out_lif_profile = field_profile;
        }
        /*
         * In case of OutLIF-profile indexing
         */
        SHR_IF_ERR_EXIT(dnx_out_lif_profile_set(unit, port, new_out_lif_profile));

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport (0x%x) is not legal\n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief        Get the PMF data for outLIF profile attached to a given OutLIF.
* \param [in] unit - Device Id
* \param [in] port -OutLIf-Id (Gport)
* \param [out] field_profile_p - PMF data for the OutLIF profile
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
static shr_error_e
dnx_field_port_profile_outlif_get(
    int unit,
    bcm_gport_t port,
    uint32 *field_profile_p)
{
    int out_lif_profile;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_SET(port))
    {
        /*
         * In case of OutLIF-profile indexing
         */
        SHR_IF_ERR_EXIT(dnx_out_lif_profile_get(unit, port, &out_lif_profile));

        if (BCM_GPORT_IS_TUNNEL(port))
        {
            /*
             * OUTRIF or TUNNEL ID (4b profile, only msb is reserved for field usage)
             */
            /*
             * PMF outlif profile is the MSb of the outlif profile
             */
            *field_profile_p =
                (out_lif_profile & (1 << (dnx_data_lif.out_lif.outrif_profile_width_get(unit) - 1))) ? TRUE : FALSE;
        }
        else
        {
            /*
             * OUTLIF  (1b profile for field)
             */
            *field_profile_p = out_lif_profile;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport (0x%x) is not legal\n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the PMF data for port profiles in HW.
*
* \param [in] unit - Device Id.
* \param [in] core_id - Port core Id.
* \param [in] port_without_core_id - TM or PP port it, without the core id.
* \param [in] field_profile - PMF data to set in the port profile.
* \param [in] port_profile_info_p - Port profile info.
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
static shr_error_e
dnx_field_port_profile_port_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 port_without_core_id,
    uint32 field_profile,
    dnx_field_map_port_profile_info_t * port_profile_info_p)
{
    unsigned int start_bit;
    unsigned int nof_bits;
    uint32 entry_handle_id;
    uint32 profile[(DNX_DATA_MAX_FIELD_PROFILE_BITS_MAX_PORT_PROFILE_SIZE + SAL_UINT32_NOF_BITS - 1) /
                   SAL_UINT32_NOF_BITS] = { 0 };
    uint32 profile_value[(DNX_DATA_MAX_FIELD_PROFILE_BITS_MAX_PORT_PROFILE_SIZE + SAL_UINT32_NOF_BITS - 1) /
                         SAL_UINT32_NOF_BITS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the bit location. */
    SHR_IF_ERR_EXIT(dnx_field_port_profile_bits_get
                    (unit, port_profile_info_p->port_profile_type, &start_bit, &nof_bits));

    if ((nof_bits < SAL_UINT32_NOF_BITS) && ((field_profile & SAL_UPTO_BIT(nof_bits)) != field_profile))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Profile 0x%x larger than number of bits %d.\n", field_profile, nof_bits);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, port_profile_info_p->dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, port_profile_info_p->dbal_key_field, port_without_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field_arr32_request(unit, entry_handle_id, port_profile_info_p->dbal_result_field, INST_SINGLE,
                                   profile_value);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** We set the bits allocated for port profile. Note that the HW may actually have less bits than that. */
    profile[0] = field_profile;
    SHR_BITCOPY_RANGE(profile_value, start_bit, profile, 0, nof_bits);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, port_profile_info_p->dbal_result_field, INST_SINGLE,
                                     profile_value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the PMF data for port profiles that aren't LIFs.
*
* \param [in] unit - Device Id
* \param [in] port_class - what kind of profile we are dealing with
* \param [in] gport - The gport of the port with the port profile.
* \param [in] field_profile - PMF data to set in the port profile
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
static shr_error_e
dnx_field_port_profile_port_set(
    int unit,
    bcm_port_class_t port_class,
    bcm_gport_t gport,
    uint32 field_profile)
{
    dnx_field_map_port_profile_info_t port_profile_info;
    bcm_gport_t logical_port;
    bcm_core_t core_id;
    uint32 port_without_core_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_SET(gport) == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, gport));
        BCM_GPORT_LOCAL_SET(gport, gport);
    }
    if (!BCM_GPORT_IS_LOCAL(gport) && !BCM_GPORT_IS_SYSTEM_PORT(gport) && !BCM_GPORT_IS_TRUNK(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x).\n", gport);
    }

    /** Get the dbal table id. */
    SHR_IF_ERR_EXIT(dnx_field_map_port_profile_info_get(unit, port_class, &port_profile_info));

    /** Extract the port and core ID from the Gport and set the HW tables. */
    if (port_profile_info.dbal_key_field == DBAL_FIELD_TM_PORT)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_SYSTEM_PORT(gport))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, &core_id, &port_without_core_id));
            SHR_IF_ERR_EXIT(dnx_field_port_profile_port_hw_set
                            (unit, core_id, port_without_core_id, field_profile, &port_profile_info));
        }
        else if (BCM_GPORT_IS_TRUNK(gport))
        {
            if (_SHR_PBMP_IS_NULL(gport_info.local_port_bmp))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "No TM-port for Trunk gport (0x%x)\n", gport);
            }
            _SHR_PBMP_ITER(gport_info.local_port_bmp, logical_port)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core_id, &port_without_core_id));
                SHR_IF_ERR_EXIT(dnx_field_port_profile_port_hw_set
                                (unit, core_id, port_without_core_id, field_profile, &port_profile_info));
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x).\n", gport);
        }
    }
    else if (port_profile_info.dbal_key_field == DBAL_FIELD_PP_PORT)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
        if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_SYSTEM_PORT(gport))
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_port_hw_set
                            (unit, gport_info.internal_port_pp_info.core_id[0],
                             gport_info.internal_port_pp_info.pp_port[0], field_profile, &port_profile_info));
        }
        else if (BCM_GPORT_IS_TRUNK(gport))
        {
            int port_iter = 0;
            if (gport_info.internal_port_pp_info.nof_pp_ports <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "No PP-port for Trunk gport (0x%x)\n", gport);
            }
            for (port_iter = 0; port_iter < gport_info.internal_port_pp_info.nof_pp_ports; port_iter++)
            {
                SHR_IF_ERR_EXIT(dnx_field_port_profile_port_hw_set
                                (unit, gport_info.internal_port_pp_info.core_id[port_iter],
                                 gport_info.internal_port_pp_info.pp_port[port_iter], field_profile,
                                 &port_profile_info));
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x).\n", gport);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown key field %d.\n", port_profile_info.dbal_key_field);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the PMF port profiles from HW.
*
* \param [in] unit - Device Id.
* \param [in] core_id - Port core Id.
* \param [in] port_without_core_id - TM or PP port it, without the core id.
* \param [in] port_profile_info_p - Port profile info.
* \param [out] field_profile_p - PMF data to set in the port profile.
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
static shr_error_e
dnx_field_port_profile_port_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 port_without_core_id,
    dnx_field_map_port_profile_info_t * port_profile_info_p,
    uint32 *field_profile_p)
{
    unsigned int start_bit;
    unsigned int nof_bits;
    uint32 entry_handle_id;
    uint32 profile[(DNX_DATA_MAX_FIELD_PROFILE_BITS_MAX_PORT_PROFILE_SIZE + SAL_UINT32_NOF_BITS - 1) /
                   SAL_UINT32_NOF_BITS] = { 0 };
    uint32 profile_value[(DNX_DATA_MAX_FIELD_PROFILE_BITS_MAX_PORT_PROFILE_SIZE + SAL_UINT32_NOF_BITS - 1) /
                         SAL_UINT32_NOF_BITS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the bit location. */
    SHR_IF_ERR_EXIT(dnx_field_port_profile_bits_get
                    (unit, port_profile_info_p->port_profile_type, &start_bit, &nof_bits));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, port_profile_info_p->dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, port_profile_info_p->dbal_key_field, port_without_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field_arr32_request(unit, entry_handle_id, port_profile_info_p->dbal_result_field, INST_SINGLE,
                                   profile_value);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_BITCOPY_RANGE(profile, 0, profile_value, start_bit, nof_bits);
    (*field_profile_p) = profile[0];

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief        Get the PMF data for port profiles that aren't LIFs.
* \param [in] unit - Device Id
* \param [in] port_class - what kind of profile we are dealing with
* \param [in] gport - The gport of the port with the port profile.
* \param [in] field_profile_p - PMF data of the port profile
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
static shr_error_e
dnx_field_port_profile_port_get(
    int unit,
    bcm_port_class_t port_class,
    bcm_gport_t gport,
    uint32 *field_profile_p)
{
    dnx_field_map_port_profile_info_t port_profile_info;
    bcm_gport_t logical_port;
    bcm_core_t core_id;
    uint32 port_without_core_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 current_field_profile;
    int first_profile_found;

    SHR_FUNC_INIT_VARS(unit);

    *field_profile_p = 0;
    first_profile_found = FALSE;

    if (BCM_GPORT_IS_SET(gport) == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, gport));
        BCM_GPORT_LOCAL_SET(gport, gport);
    }
    if (!BCM_GPORT_IS_LOCAL(gport) && !BCM_GPORT_IS_SYSTEM_PORT(gport) && !BCM_GPORT_IS_TRUNK(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x).\n", gport);
    }

    /** Get the dbal table id. */
    SHR_IF_ERR_EXIT(dnx_field_map_port_profile_info_get(unit, port_class, &port_profile_info));

    /** Extract the port and core ID from the Gport and set the HW tables. */
    if (port_profile_info.dbal_key_field == DBAL_FIELD_TM_PORT)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_SYSTEM_PORT(gport))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, &core_id, &port_without_core_id));
            SHR_IF_ERR_EXIT(dnx_field_port_profile_port_hw_get
                            (unit, core_id, port_without_core_id, &port_profile_info, field_profile_p));
        }
        else if (BCM_GPORT_IS_TRUNK(gport))
        {
            if (_SHR_PBMP_IS_NULL(gport_info.local_port_bmp))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "No TM-port for Trunk gport (0x%x)\n", gport);
            }
            _SHR_PBMP_ITER(gport_info.local_port_bmp, logical_port)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core_id, &port_without_core_id));
                SHR_IF_ERR_EXIT(dnx_field_port_profile_port_hw_get
                                (unit, core_id, port_without_core_id, &port_profile_info, &current_field_profile));
                if ((first_profile_found != FALSE) && (current_field_profile != *field_profile_p))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Current field_profile %d is different than previous one %d. TM_PORT %d CORE %d\n",
                                 current_field_profile, *field_profile_p, port_without_core_id, core_id);
                }
                *field_profile_p = current_field_profile;
                first_profile_found = TRUE;
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x).\n", gport);
        }
    }
    else if (port_profile_info.dbal_key_field == DBAL_FIELD_PP_PORT)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
        if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_SYSTEM_PORT(gport))
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_port_hw_get
                            (unit, gport_info.internal_port_pp_info.core_id[0],
                             gport_info.internal_port_pp_info.pp_port[0], &port_profile_info, field_profile_p));
        }
        else if (BCM_GPORT_IS_TRUNK(gport))
        {
            int port_iter;
            if (gport_info.internal_port_pp_info.nof_pp_ports <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "No PP-port for Trunk gport (0x%x)\n", gport);
            }
            for (port_iter = 0; port_iter < gport_info.internal_port_pp_info.nof_pp_ports; port_iter++)
            {
                SHR_IF_ERR_EXIT(dnx_field_port_profile_port_hw_get
                                (unit, gport_info.internal_port_pp_info.core_id[port_iter],
                                 gport_info.internal_port_pp_info.pp_port[port_iter],
                                 &port_profile_info, &current_field_profile));
                if ((first_profile_found != FALSE) && (current_field_profile != *field_profile_p))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Current field_profile %d is different than previous one %d. PP_PORT %d CORE %d\n",
                                 current_field_profile, *field_profile_p,
                                 gport_info.internal_port_pp_info.pp_port[port_iter],
                                 gport_info.internal_port_pp_info.core_id[port_iter]);
                }
                *field_profile_p = current_field_profile;
                first_profile_found = TRUE;
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport_info, gport (0x%x).\n", gport);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown key field %d.\n", port_profile_info.dbal_key_field);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_port.h
 */
shr_error_e
dnx_field_port_profile_gport_set(
    int unit,
    bcm_port_class_t port_class,
    bcm_gport_t port,
    uint32 field_profile)
{
    dnx_field_port_porfile_type_e port_profile_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_port_profile_type_get(unit, port_class, &port_profile_type));

    /*
     * Call the relevant function.
     */
    switch (port_profile_type)
    {
        case DNX_FIELD_PORT_PROFILE_TYPE_PORT:
        case DNX_FIELD_PORT_PROFILE_TYPE_PORT_GENERAL_DATA:
        case DNX_FIELD_PORT_PROFILE_TYPE_PORT_GENERAL_DATA_HIGH:
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_port_set(unit, port_class, port, field_profile));
            break;
        }
        case DNX_FIELD_PORT_PROFILE_TYPE_IN_LIF_INGRESS:
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_inlif_set(unit, port, field_profile));
            break;
        }
        case DNX_FIELD_PORT_PROFILE_TYPE_OUT_ETH_RIF_EGRESS:
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_outlif_set(unit, port, field_profile));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown port profile type %d (port class %d).\n",
                         port_profile_type, port_class);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_port.h
 */
shr_error_e
dnx_field_port_profile_gport_get(
    int unit,
    bcm_port_class_t port_class,
    bcm_gport_t port,
    uint32 *field_profile_p)
{
    dnx_field_port_porfile_type_e port_profile_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_port_profile_type_get(unit, port_class, &port_profile_type));

    /*
     * Call the relevant function.
     */
    switch (port_profile_type)
    {
        case DNX_FIELD_PORT_PROFILE_TYPE_PORT:
        case DNX_FIELD_PORT_PROFILE_TYPE_PORT_GENERAL_DATA:
        case DNX_FIELD_PORT_PROFILE_TYPE_PORT_GENERAL_DATA_HIGH:
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_port_get(unit, port_class, port, field_profile_p));
            break;
        }
        case DNX_FIELD_PORT_PROFILE_TYPE_IN_LIF_INGRESS:
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_inlif_get(unit, port, field_profile_p));
            break;
        }
        case DNX_FIELD_PORT_PROFILE_TYPE_OUT_ETH_RIF_EGRESS:
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_outlif_get(unit, port, field_profile_p));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown port profile type %d (port class %d).\n",
                         port_profile_type, port_class);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_port.h
 */
shr_error_e
dnx_field_port_profile_vsi_set(
    int unit,
    int field_profile,
    int *vsi_porfile_p)
{
    int old_vsi_profile;
    unsigned int start_bit, nof_bits;
    int mask;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_port_profile_bits_get
                    (unit, DNX_FIELD_PORT_PROFILE_TYPE_IN_ETH_RIF_INGRESS, &start_bit, &nof_bits));

    old_vsi_profile = *vsi_porfile_p;
    mask = (((1 << nof_bits) - 1) << start_bit);
    *vsi_porfile_p = (old_vsi_profile & ~mask) | ((field_profile << start_bit) & mask);

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_port.h
 */
shr_error_e
dnx_field_port_profile_vsi_get(
    int unit,
    int vsi_profile,
    int *field_profile_p)
{
    unsigned int start_bit, nof_bits;
    int mask;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_port_profile_bits_get
                    (unit, DNX_FIELD_PORT_PROFILE_TYPE_IN_ETH_RIF_INGRESS, &start_bit, &nof_bits));

    mask = (((1 << nof_bits) - 1) << start_bit);
    *field_profile_p = (vsi_profile & mask) >> start_bit;

exit:
    SHR_FUNC_EXIT;
}
