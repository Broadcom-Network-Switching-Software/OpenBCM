/** \file src/bcm/dnx/trunk/trunk_egress/trunk_egress.c
 *
 *
 *  This file contains the implementation of egress trunk
 *  functions.
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

/*
 * Include files.
 * {
 */
#include "trunk_egress.h"
#include "../trunk_dbal_access.h"
#include "../trunk_utils.h"
#include "../trunk_verify.h"
#include "../trunk_sw_db.h"

#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include <bcm_int/dnx/algo/trunk/algo_trunk.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_visibility.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/swstate/auto_generated/access/trunk_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/switch/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <shared/trunk.h>

#ifdef INCLUDE_XFLOW_MACSEC
#include <dnx/dnx_sec.h>
#endif /* INCLUDE_XFLOW_MACSEC */
/*
 * }
 */

/*
 * see header file
 */
shr_error_e
dnx_trunk_active_configuration_to_stand_by_configuration_copy(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    /** if graceful lag modification is supported */
    if (dnx_data_trunk.graceful.feature_get(unit, dnx_data_trunk_graceful_allow))
    {
        uint32 single_configuration_range;
        uint32 single_configuration_msb;
        uint32 single_configuration_mask;
        uint32 get_mask;
        uint32 set_mask;
        uint32 nof_egress_trunks;
        uint32 multiple_configuration_enable;
        uint32 active_configuration_selector;
        int core;

        /** get current configuration */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_active_configuration_selector_get(unit,
                                                                                      &multiple_configuration_enable,
                                                                                      &active_configuration_selector));
        single_configuration_range = dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit);
        /** determine set and get masks for entries, this algo goes
         *  over half the range of a single configuration (when
         *  working in graceful mode half the range is active and
         *  half is standby) and use a mask to manipulate active and
         *  standby ranges, the MSB is used to determine which is
         *  which */
        single_configuration_msb = utilex_msb_bit_on(single_configuration_range - 1);
        single_configuration_mask = 1 << single_configuration_msb;
        if (active_configuration_selector == 0)
        {
            get_mask = 0;
            set_mask = single_configuration_mask;
        }
        else
        {
            get_mask = single_configuration_mask;
            set_mask = 0;
        }
        nof_egress_trunks = dnx_data_trunk.egress_trunk.nof_get(unit);
        /** iterate on all cores */
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            /** iterate on all egress trunks */
            for (int egress_trunk = 0; egress_trunk < nof_egress_trunks; ++egress_trunk)
            {
                /** iterate on half configuation range, and copy
                 *  configuration from active half to inactive half according
                 *  to set and get masks */
                for (uint32 entry = 0; entry < (single_configuration_range / 2); ++entry)
                {
                    uint32 pp_dsp;
                    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_egress_mc_resolution_get
                                    (unit, core, egress_trunk, (entry | get_mask), &pp_dsp));
                    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_egress_mc_resolution_set
                                    (unit, core, egress_trunk, (entry | set_mask), pp_dsp));
                }
            }
        }

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid when lag_graceful_modification is disabled\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_egress_align(
    int unit,
    bcm_trunk_t trunk_id)
{
    int first_in_range = 0;
    int total_range;
    int egress_trunk_id;
    int profile_found;
    int nof_enabled_members;
    int arr_size;
    int nof_unique_members;
    int pool;
    int group;
    int lb_key_ranges_arr_size;
    bcm_trunk_id_info_t id_info;
    dnx_trunk_unique_member_t *unique_members_arr = NULL;
    dnx_algo_trunk_member_lb_key_ranges_t *lb_key_ranges_arr = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /** if input trunk doesn't have a corresponding egress trunk - get out, nothing to do */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_get(unit, trunk_id, &egress_trunk_id, &profile_found));
    if (!profile_found)
    {
        SHR_EXIT();
    }

    total_range = dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit);
    /** if graceful lag modification is supported */
    if (dnx_data_trunk.graceful.feature_get(unit, dnx_data_trunk_graceful_allow))
    {
        uint32 multiple_configuration_enable;
        uint32 active_configuration_selector;
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_active_configuration_selector_get(unit,
                                                                                      &multiple_configuration_enable,
                                                                                      &active_configuration_selector));

        /** range is halved, this is to create a stand by and current configurations */
        total_range /= 2;
        /** set first in range with correct position according to stand-by configuration at this point of time */
        first_in_range = (active_configuration_selector ==
                          DBAL_ENUM_FVAL_GRACEFUL_ACTIVE_CONFIGURATION_GRACEFUL_ACTIVE_CONFIGURATION_FIRST) ?
            total_range : 0;
    }

    /** get number of enabled members in trunk  */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_enabled_members_get(unit, pool, group, &nof_enabled_members));

    /** get unique members info */
    arr_size = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    unique_members_arr = sal_alloc(sizeof(dnx_trunk_unique_member_t) * arr_size, "unique members array");
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_unique_members_info_get(unit, pool, group, unique_members_arr,
                                                            arr_size, &nof_unique_members));
    /** allocate place for returned array of lb key ranges, the allocated size is +1 to cover the trivial case of empty trunk */
    lb_key_ranges_arr = sal_alloc(sizeof(dnx_algo_trunk_member_lb_key_ranges_t) * (nof_unique_members + 1),
                                  "lb key ranges array");
    /** get lb-key distribution per unique member */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_trunk_lb_key_destribution_get(unit, total_range, first_in_range,
                                                                        nof_enabled_members, nof_unique_members,
                                                                        unique_members_arr, &lb_key_ranges_arr_size,
                                                                        lb_key_ranges_arr));
    /** update dbal with received distribution */
    SHR_IF_ERR_EXIT(dnx_trunk_egress_lb_key_ranges_dbal_update(unit, egress_trunk_id, lb_key_ranges_arr_size,
                                                               lb_key_ranges_arr));

exit:
    SHR_FREE(unique_members_arr);
    SHR_FREE(lb_key_ranges_arr);
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_egress_lb_key_ranges_dbal_update(
    int unit,
    int egress_trunk_id,
    int lb_key_ranges_arr_size,
    dnx_algo_trunk_member_lb_key_ranges_t * lb_key_ranges_arr)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);

    for (int lb_key_range_entry = 0; lb_key_range_entry < lb_key_ranges_arr_size; ++lb_key_range_entry)
    {
        /** update dbal - if local with relevant entries, if not with invalid ones */
        dnx_algo_gpm_gport_phy_info_t phy_gport_info;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, lb_key_ranges_arr[lb_key_range_entry].system_port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            int first;
            int range;
            uint32 out_port;
            uint32 tm_core_id, pp_dsp;

            /** Get tm port and core from local port */
            if (phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get
                                (unit, phy_gport_info.local_port, (bcm_core_t *) & tm_core_id, &pp_dsp));
            }

            if ((phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT) && (core == tm_core_id))
            {
                out_port = pp_dsp;
            }
            else
            {
                out_port = dnx_data_trunk.egress_trunk.invalid_pp_dsp_get(unit);
            }
            first = lb_key_ranges_arr[lb_key_range_entry].first_lb_key;
            range = lb_key_ranges_arr[lb_key_range_entry].lb_key_range;
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_egress_range_set(unit, core, egress_trunk_id,
                                                                         first, range, out_port));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - init egress_trunk profile.
 *
 * \param [in] unit - unit number
 * \param [in] egress_trunk_id - egress trunk id
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
dnx_trunk_egress_profile_init(
    int unit,
    int egress_trunk_id)
{
    int first_in_range = 0;
    int range = dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit);
    int out_port = dnx_data_trunk.egress_trunk.invalid_pp_dsp_get(unit);
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /** set range of entire profile to invalid out port */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_egress_range_set(unit, core, egress_trunk_id,
                                                                     first_in_range, range, out_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_egress_create(
    int unit,
    bcm_trunk_t trunk_id,
    int *egress_trunk_id)
{
    uint8 is_first;
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_trunk.egress_trunk.feature_get(unit, dnx_data_trunk_egress_trunk_is_supported))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Current device does not support Egress trunks\n");
    }

    /** verify input */
    /** Verify trunk_id is valid */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** Verify trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));
    /** NULL Check */
    SHR_NULL_CHECK(egress_trunk_id, _SHR_E_PARAM, "egress_trunk_id");

    /** create egress trunk profile */
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_trunk_egress_create(unit, trunk_id, egress_trunk_id, &is_first),
                             "Out of free Egress LAGs %s%s%s\n", EMPTY, EMPTY, EMPTY);

    if (is_first)
    {
        /** init profile in HW (basically clear the HW for this profile) */
        SHR_IF_ERR_EXIT(dnx_trunk_egress_profile_init(unit, *egress_trunk_id));

        /** Align Egress trunk profile's LB Keys distribution with trunk members */
        SHR_IF_ERR_EXIT(dnx_trunk_egress_align(unit, trunk_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_egress_destroy(
    int unit,
    bcm_trunk_t trunk_id)
{
    int egress_trunk_id;
    int profile_found;
    uint8 is_last = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_trunk.egress_trunk.feature_get(unit, dnx_data_trunk_egress_trunk_is_supported))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Current device does not support Egress trunks\n");
    }

    /** Get the  egress profile of the trunk */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_get(unit, trunk_id, &egress_trunk_id, &profile_found));
    if (!profile_found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Trunk ID %d does not have egress profile allocated\n", trunk_id);
    }

    /** Destroy the trunk egress profile. */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_destroy(unit, egress_trunk_id, &is_last));
    if (is_last)
    {
        /** init profile in HW (basically clear the HW for this profile) */
        SHR_IF_ERR_EXIT(dnx_trunk_egress_profile_init(unit, egress_trunk_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_containing_egress_trunk_get(
    int unit,
    int egress_trunk_id,
    bcm_trunk_t * trunk_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(trunk_id, _SHR_E_PARAM, "trunk_id");
    SHR_IF_ERR_EXIT(dnx_algo_trunk_containing_egress_trunk_get(unit, egress_trunk_id, trunk_id));
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_egress_member_hash_range_set(
    int unit,
    uint32 flags,
    bcm_trunk_t trunk_id,
    bcm_gport_t member,
    bcm_trunk_member_range_t range)
{
    int egress_trunk_id;
    int profile_found;
    dnx_algo_trunk_member_lb_key_ranges_t lb_key_range[1];
    uint32 multiple_configuration_enable;
    uint32 active_configuration_selector;
    uint32 single_configuration_msb;
    uint32 single_configuration_mask;
    uint32 hash_mask;
    SHR_FUNC_INIT_VARS(unit);

    /** Make sure the Trunk ID provided has an Egress Trunk allocated and get its ID */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_get(unit, trunk_id, &egress_trunk_id, &profile_found));
    if (!profile_found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given trunk ID 0x%x does not have an Egress Trunk allocated!\n", trunk_id);
    }

    /** Handle Graceful LAG if supported */
    if (dnx_data_trunk.graceful.feature_get(unit, dnx_data_trunk_graceful_allow))
    {
        /** Get the current configuration */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_active_configuration_selector_get(unit,
                                                                                      &multiple_configuration_enable,
                                                                                      &active_configuration_selector));
        /** In Graceful LAG only half of the range is active at a time.
         * Based on the current active configuration a mask is created for the entry index, using the MSB as selector for the correct entry. */
        single_configuration_msb = utilex_msb_bit_on(dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit) - 1);
        single_configuration_mask = 1 << single_configuration_msb;
        hash_mask = (active_configuration_selector == 0) ? 0 : single_configuration_mask;
    }
    else
    {
        hash_mask = 0;
    }

    /** Update DBAL with the received hash range */
    lb_key_range[0].system_port = member;
    lb_key_range[0].first_lb_key = (range.min_hash | hash_mask);
    lb_key_range[0].lb_key_range = (range.max_hash + 1) - range.min_hash;
    SHR_IF_ERR_EXIT(dnx_trunk_egress_lb_key_ranges_dbal_update(unit, egress_trunk_id, 1, lb_key_range));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_egress_member_hash_range_get(
    int unit,
    uint32 flags,
    bcm_trunk_t trunk_id,
    bcm_gport_t member,
    bcm_trunk_member_range_t * range)
{
    int egress_trunk_id;
    int profile_found;
    int core;
    int first_hash, last_hash;

    uint32 multiple_configuration_enable;
    uint32 active_configuration_selector;
    uint32 pp_dsp;
    uint32 pp_dsp_get;
    uint32 single_configuration_msb;
    uint32 single_configuration_mask;
    uint32 hash;
    uint32 hash_mask;
    uint32 nof_hashes_to_check;
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Make sure the Trunk ID provided has an Egress Trunk allocated and get its ID */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_get(unit, trunk_id, &egress_trunk_id, &profile_found));
    if (!profile_found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given trunk ID 0x%x does not have an Egress Trunk allocated!\n", trunk_id);
    }

    /** Handle Graceful LAG if supported */
    if (dnx_data_trunk.graceful.feature_get(unit, dnx_data_trunk_graceful_allow))
    {
        /** Get the current configuration */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_active_configuration_selector_get(unit,
                                                                                      &multiple_configuration_enable,
                                                                                      &active_configuration_selector));
        /** In Graceful LAG only half of the range is active at a time.
         * Based on the current active configuration a mask is created for the entry index, using the MSB as selector for the correct entry. */
        single_configuration_msb = utilex_msb_bit_on(dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit) - 1);
        single_configuration_mask = 1 << single_configuration_msb;
        hash_mask = (active_configuration_selector == 0) ? 0 : single_configuration_mask;
        nof_hashes_to_check = (dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit) / 2);
    }
    else
    {
        hash_mask = 0;
        nof_hashes_to_check = dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit);
    }

    /** Get core and PP DSP */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, member, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, phy_gport_info.local_port, (bcm_core_t *) & core, &pp_dsp));

    /** Iterate over the hashes to find the range that is mapped to the requested member */
    first_hash = -1;
    last_hash = -1;
    for (hash = 0; hash < nof_hashes_to_check; ++hash)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_egress_mc_resolution_get
                        (unit, core, egress_trunk_id, (hash | hash_mask), &pp_dsp_get));
        if (pp_dsp_get == pp_dsp)
        {
            if (first_hash == -1)
            {
                /** First hash for requested member found */
                first_hash = hash;
            }

            last_hash = hash;
        }

        if ((pp_dsp_get != pp_dsp) && (last_hash != -1))
        {
            /** Lash hash was already fiund and current port does not match the requested - no need to continue */
            break;
        }
    }

    if (first_hash == -1)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No valid hash range has been found for given member! \n");
    }
    else
    {
        range->min_hash = first_hash;
        range->max_hash = last_hash;
    }

exit:
    SHR_FUNC_EXIT;
}
