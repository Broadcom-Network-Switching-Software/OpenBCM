/** \file src/bcm/dnx/trunk/trunk_dbal_access.c
 *
 *
 *  This file contains the implementation of dbal access
 *  functions of the trunk module.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TRUNK

/*
 * Include files.
 * {
 */

#include <bcm_int/dnx/algo/algo_gpm.h>
#include <shared/shrextend/shrextend_debug.h>
#include "trunk_dbal_access.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/trunk.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_pool_attributes_set(
    int unit,
    int pool,
    int pool_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_POOL_ATTRIBUTES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL_INDEX, (uint32) pool);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL_SIZE_MODE, INST_SINGLE, (uint32) pool_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_egress_trunk_attributes_set(
    int unit,
    int size_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_EGRESS_ATTRIBUTES, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SIZE_MODE, INST_SINGLE, (uint32) size_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_pool_attributes_get(
    int unit,
    int pool,
    int *pool_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_POOL_ATTRIBUTES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL_INDEX, (uint32) pool);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_POOL_SIZE_MODE, INST_SINGLE, (uint32 *) pool_mode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_members_set(
    int unit,
    int pool,
    int group,
    int member,
    uint32 system_port_id)
{
    int calculated_index;
    uint32 entry_handle_id;
    bcm_gport_t system_port_gport;
    uint32 mapped_destination;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** system_port_id to system_port_gport */
    BCM_GPORT_SYSTEM_PORT_ID_SET(system_port_gport, system_port_id);

    /** get destination encoding from system port gport */
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport(unit, 0, system_port_gport, &mapped_destination));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_MEMBERS, &entry_handle_id));
    calculated_index = pool * dnx_data_trunk.parameters.max_nof_members_in_pool_get(unit) +
        group * (dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group) + member;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEMBER_INDEX, calculated_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DST_SYS_PORT_ID, INST_SINGLE, mapped_destination);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_members_get(
    int unit,
    int pool,
    int group,
    int member,
    uint32 *system_port_id)
{
    int calculated_index;
    uint32 entry_handle_id;
    uint32 mapped_destination;
    bcm_gport_t system_port_gport;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_MEMBERS, &entry_handle_id));
    calculated_index = pool * dnx_data_trunk.parameters.max_nof_members_in_pool_get(unit) +
        group * (dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group) + member;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEMBER_INDEX, calculated_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DST_SYS_PORT_ID, INST_SINGLE, &mapped_destination);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** get system_port_gport from encoded destination */
    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit, 0, mapped_destination, &system_port_gport));

    /** get system_port_id from system_port_gport */
    *system_port_id = BCM_GPORT_SYSTEM_PORT_ID_GET(system_port_gport);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - translate PSC values from BCM format to DNX HW
 *        format
 *
 * \param [in] unit - unit number
 * \param [in] bcm_psc - PSC is BCM format
 * \param [in] dnx_psc - PSC in DNX HW format
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_dbal_access_psc_bcm_to_dnx_translate(
    int unit,
    int bcm_psc,
    uint32 *dnx_psc)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (bcm_psc)
    {
        case BCM_TRUNK_PSC_PORTFLOW:
        {
            *dnx_psc = DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_MULTIPLY_AND_DIVIDE;
            break;
        }
        case BCM_TRUNK_PSC_SMOOTH_DIVISION:
        {
            *dnx_psc = DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_SMOOTH_DIVISION;
            break;
        }
        case BCM_TRUNK_PSC_C_LAG:
        {
            *dnx_psc = DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_CONSISTANT_HASHING;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported port selection criteria\n");
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - translate PSC values from DNX HW format to BCM
 *        format
 *
 * \param [in] unit - uint number
 * \param [in] dnx_psc - PSC in DNX HW format
 * \param [in] bcm_psc - PSC in BCM format
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_dbal_access_psc_dnx_to_bcm_translate(
    int unit,
    uint32 dnx_psc,
    int *bcm_psc)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (dnx_psc)
    {
        case DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_MULTIPLY_AND_DIVIDE:
        {
            *bcm_psc = BCM_TRUNK_PSC_PORTFLOW;
            break;
        }
        case DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_SMOOTH_DIVISION:
        {
            *bcm_psc = BCM_TRUNK_PSC_SMOOTH_DIVISION;
            break;
        }
        case DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_CONSISTANT_HASHING:
        {
            *bcm_psc = BCM_TRUNK_PSC_C_LAG;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported port selection criteria\n");
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_attributes_set(
    int unit,
    int pool,
    int group,
    int psc,
    int psc_algo_input)
{
    uint32 entry_handle_id;
    uint32 dnx_psc = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_ATTRIBUTES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL_INDEX, pool);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP_INDEX, group);
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_psc_bcm_to_dnx_translate(unit, psc, &dnx_psc));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dnx_psc);
    switch (dnx_psc)
    {
        case DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_MULTIPLY_AND_DIVIDE:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id,
                                         DBAL_FIELD_MAX_MEMBER, INST_SINGLE, (uint32) psc_algo_input);
            break;
        }
        case DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_SMOOTH_DIVISION:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id,
                                         DBAL_FIELD_SMOOTH_DIVISION_PROFILE, INST_SINGLE, (uint32) psc_algo_input);
            break;
        }
        case DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_CONSISTANT_HASHING:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id,
                                         DBAL_FIELD_C_LAG_PROFILE, INST_SINGLE, (uint32) psc_algo_input);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PSC - %d\n", psc);
            break;
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_attributes_get(
    int unit,
    int pool,
    int group,
    int *psc,
    int *psc_algo_input)
{
    uint32 entry_handle_id;
    uint32 dnx_psc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_ATTRIBUTES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL_INDEX, pool);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP_INDEX, group);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                        DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &dnx_psc));
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_psc_dnx_to_bcm_translate(unit, dnx_psc, psc));
    switch (dnx_psc)
    {
        case DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_MULTIPLY_AND_DIVIDE:
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MAX_MEMBER,
                                                                INST_SINGLE, (uint32 *) psc_algo_input));
            break;
        }
        case DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_SMOOTH_DIVISION:
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                                DBAL_FIELD_SMOOTH_DIVISION_PROFILE, INST_SINGLE,
                                                                (uint32 *) psc_algo_input));
            break;
        }
        case DBAL_RESULT_TYPE_TRUNK_ATTRIBUTES_PSC_CONSISTANT_HASHING:
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_C_LAG_PROFILE,
                                                                INST_SINGLE, (uint32 *) psc_algo_input));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PSC - %d\n", *psc);
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_smooth_division_profile_configuration_set(
    int unit,
    int profile_index,
    int entry_index,
    int member_destination)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_SMOOTH_DIVISION, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_INDEX, profile_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_INDEX, entry_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEMBER_INDEX, INST_SINGLE, member_destination);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_smooth_division_profile_configuration_get(
    int unit,
    int profile_index,
    int entry_index,
    int *member_destination)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_SMOOTH_DIVISION, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_INDEX, profile_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_INDEX, entry_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MEMBER_INDEX,
                               INST_SINGLE, (uint32 *) member_destination);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_trunk_dbal_access_flow_agg_group_map_set(
    int unit,
    int flagg_group_id,
    int flow_id_base,
    int hw_trunk_id,
    int tc_size)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_ERR_EXIT(_SHR_E_PARAM, "API not supported yet - TBD\n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_trunk_dbal_access_flow_agg_group_map_get(
    int unit,
    int flagg_group_id,
    int *flow_id_base,
    int *hw_trunk_id,
    int *tc_size)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_ERR_EXIT(_SHR_E_PARAM, "API not supported yet - TBD\n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_consistent_hashing_profile_configurations_set(
    int unit,
    int profile_index,
    int member_base,
    int member_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_CONSISTENT_HASHING_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_INDEX, profile_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEMBER_BASE, INST_SINGLE, (uint32) member_base);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEMBER_MODE, INST_SINGLE, (uint32) member_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_consistent_hashing_profile_configurations_get(
    int unit,
    int profile_index,
    int *member_base,
    int *member_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_CONSISTENT_HASHING_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_INDEX, profile_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MEMBER_BASE, INST_SINGLE, (uint32 *) member_base);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MEMBER_MODE, INST_SINGLE, (uint32 *) member_mode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_consistent_hashing_members_map_set(
    int unit,
    int hashed_index,
    int is_double_entry,
    uint32 mapped_member)
{
    /** Implementation notes - when working with a profile of upto
     *  16 members - each mapped member represents 2 actual members,
     *  however we can always put the same member in both of them -
     *  probably should create a util function that receieves a
     *  member, check that it is under 16 and return an entry with
     *  double member. this should be utilized if profile mode is on
     *  mode 0 */
    uint32 entry_handle_id;
    uint32 field_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_CONSISTENT_HASHING_MEMBERS_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HASHED_INDEX, hashed_index);
    if (is_double_entry != 0)
    {
        field_value = mapped_member |
            (mapped_member << dnx_data_trunk.psc.consistant_hashing_small_group_size_in_bits_get(unit));
    }
    else
    {
        field_value = mapped_member;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_MEMBER, INST_SINGLE, field_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_consistent_hashing_members_map_get(
    int unit,
    int hashed_index,
    int is_double_entry,
    uint32 *mapped_member)
{
    /** Implementation notes - when working with a profile of upto
     *  16 members - each mapped member represents 2 actual members,
     *  however we can always put the same member in both of them -
     *  probably should create a util function that receieves a
     *  member, check that it is under 16 and return an entry with
     *  double member. this should be utilized if profile mode is on
     *  mode 0 */
    uint32 entry_handle_id;
    uint32 field_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_CONSISTENT_HASHING_MEMBERS_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HASHED_INDEX, hashed_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAPPED_MEMBER, INST_SINGLE, &field_value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (is_double_entry != 0)
    {
        *mapped_member = field_value >> dnx_data_trunk.psc.consistant_hashing_small_group_size_in_bits_get(unit);
    }
    else
    {
        *mapped_member = field_value;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_egress_mc_resolution_set(
    int unit,
    int core,
    int egress_trunk_profile,
    int lb_key,
    uint32 out_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_EGRESS_MC_RESOLUTION, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_TRUNK_INDEX, egress_trunk_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_INDEX, lb_key);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_PORT, INST_SINGLE, out_port);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_egress_mc_resolution_get(
    int unit,
    int core,
    int egress_trunk_profile,
    int lb_key,
    uint32 *out_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_EGRESS_MC_RESOLUTION, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_TRUNK_INDEX, egress_trunk_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_INDEX, lb_key);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_PORT, INST_SINGLE, out_port);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_egress_range_set(
    int unit,
    int core,
    int egress_trunk_profile,
    int first,
    int range,
    uint32 out_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_EGRESS_MC_RESOLUTION, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_TRUNK_INDEX, egress_trunk_profile);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_INDEX, first, first + range - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_PORT, INST_SINGLE, out_port);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_active_configuration_selector_set(
    int unit,
    uint32 multiple_configuration_enable,
    uint32 active_configuration)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_ACTIVE_CONFIGURATION_SELECTOR, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MULTIPLE_CONFIGURATIONS_ENABLED,
                                 INST_SINGLE, multiple_configuration_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GRACEFUL_ACTIVE_CONFIGURATION,
                                 INST_SINGLE, active_configuration);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * see header file
 */
shr_error_e
dnx_trunk_dbal_access_trunk_active_configuration_selector_get(
    int unit,
    uint32 *multiple_configuration_enable,
    uint32 *active_configuration)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRUNK_ACTIVE_CONFIGURATION_SELECTOR, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MULTIPLE_CONFIGURATIONS_ENABLED,
                               INST_SINGLE, multiple_configuration_enable);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GRACEFUL_ACTIVE_CONFIGURATION,
                               INST_SINGLE, active_configuration);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}
