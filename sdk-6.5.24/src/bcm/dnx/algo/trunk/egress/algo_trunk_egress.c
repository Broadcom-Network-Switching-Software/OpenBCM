/** \file algo/trunk/egress/algo_trunk_egress.c
 *
 * Implementation of egress trunk algos
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TRUNK

/**
* Include files:
* {
*/
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_trunk_access.h>

/*
 * see header file
 */
shr_error_e
dnx_algo_egress_trunk_init(
    int unit)
{
    dnx_algo_template_create_data_t create_data;
    SHR_FUNC_INIT_VARS(unit);

    /** create data */
    sal_memset(&create_data, 0, sizeof(dnx_algo_template_create_data_t));
    create_data.flags = 0;
    create_data.first_profile = 0;
    create_data.nof_profiles = dnx_data_trunk.egress_trunk.nof_get(unit);
    create_data.max_references = dnx_data_multicast.params.nof_mcdb_entries_get(unit);
    /** default profile is not expected to be used - so an invalid value was provided */
    create_data.default_profile = -1;
    create_data.data_size = sizeof(bcm_trunk_t);
    create_data.default_data = NULL;
    sal_strncpy(create_data.name, DNX_ALGO_TRUNK_EGRESSTRUNK_TEMPLATE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_trunk_db.egress_trunk.create(unit, &create_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_egress_create(
    int unit,
    bcm_trunk_t trunk_id,
    int *egress_trunk_id,
    uint8 *is_first)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_trunk_db.egress_trunk.allocate_single
                    (unit, 0, (void *) &trunk_id, NULL, egress_trunk_id, is_first));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_egress_destroy(
    int unit,
    int egress_trunk_id,
    uint8 *is_last)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_last, _SHR_E_PARAM, "is_last");

    SHR_IF_ERR_EXIT(algo_trunk_db.egress_trunk.free_single(unit, egress_trunk_id, is_last));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_egress_get(
    int unit,
    bcm_trunk_t trunk_id,
    int *egress_trunk_id,
    int *profile_found)
{
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(profile_found, _SHR_E_PARAM, "profile_found");
    SHR_NULL_CHECK(egress_trunk_id, _SHR_E_PARAM, "egress_trunk_id");

    rv = algo_trunk_db.egress_trunk.profile_get(unit, (void *) &trunk_id, egress_trunk_id);
    if (rv == _SHR_E_NOT_FOUND)
    {
        *profile_found = 0;
    }
    else
    {
        /** either error or profile found */
        SHR_IF_ERR_EXIT(rv);
        *profile_found = 1;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_containing_egress_trunk_get(
    int unit,
    int egress_trunk_id,
    bcm_trunk_t * trunk_id)
{
    int ref_count;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_trunk_db.egress_trunk.profile_data_get(unit, egress_trunk_id, &ref_count, (void *) trunk_id));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
void
dnx_algo_trunk_egresstrunk_profile_print_cb(
    int unit,
    const void *data)
{
#ifdef DNX_SW_STATE_DIAGNOSTIC
    bcm_trunk_t *trunk_id = (bcm_trunk_t *) data;
#endif
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "trunk_id", *trunk_id, "trunk id", "0x%x");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_egress_trunk_lb_key_destribution_get(
    int unit,
    int profile_lb_key_range_size,
    int profile_first_lb_key_value,
    int trunk_nof_enabled_members,
    int unique_members_array_size,
    dnx_trunk_unique_member_t * unique_members_array,
    int *lb_key_ranges_array_size,
    dnx_algo_trunk_member_lb_key_ranges_t * lb_key_ranges_array)
{
    int range;
    int range_per_member;
    int remaining;
    int member_id;
    SHR_FUNC_INIT_VARS(unit);

    *lb_key_ranges_array_size = 0;

    /** if trunk has no enabled members */
    if (trunk_nof_enabled_members == 0)
    {
        /** set array with 1 member, use invalid system port and give it the whole range */
        *lb_key_ranges_array_size = 1;
        lb_key_ranges_array[0].first_lb_key = profile_first_lb_key_value;
        lb_key_ranges_array[0].lb_key_range = profile_lb_key_range_size;
        BCM_GPORT_SYSTEM_PORT_ID_SET(lb_key_ranges_array[0].system_port,
                                     dnx_data_device.general.invalid_system_port_get(unit));
        SHR_EXIT();
    }

    /** calculate range per member and remaining */
    range_per_member = profile_lb_key_range_size / trunk_nof_enabled_members;
    remaining = profile_lb_key_range_size % trunk_nof_enabled_members;

    for (int unique_member = 0; unique_member < unique_members_array_size; ++unique_member)
    {
        /** Calculation of Member ID - in order to be aligned with Legacy,
         * the unique_members_array need so be accessed in reversed order.
         */
        member_id = (unique_members_array_size - unique_member) - 1;

        if (unique_members_array[member_id].is_enabled == FALSE)
        {
            continue;
        }

        /** get range of lb-keys to update for each member  */
        range = range_per_member * unique_members_array[member_id].nof_replications;
        if (remaining > unique_members_array[member_id].nof_replications)
        {
            range += unique_members_array[member_id].nof_replications;
            remaining -= unique_members_array[member_id].nof_replications;
        }
        else
        {
            range += remaining;
            remaining = 0;
        }

        /** set result array */
        lb_key_ranges_array[*lb_key_ranges_array_size].first_lb_key = profile_first_lb_key_value;
        lb_key_ranges_array[*lb_key_ranges_array_size].lb_key_range = range;
        lb_key_ranges_array[*lb_key_ranges_array_size].system_port = unique_members_array[member_id].system_port;

        /** prepare for next iteration */
        profile_first_lb_key_value += range;
        ++(*lb_key_ranges_array_size);
    }

exit:
    SHR_FUNC_EXIT;
}
