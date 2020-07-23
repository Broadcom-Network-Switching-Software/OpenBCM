/** \file src/bcm/dnx/trunk/trunk_init.c
 * 
 * 
 *  This file contains the implementation on init functions of
 *  the trunk module.
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

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/trunk/trunk_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include "trunk_utils.h"
#include "trunk_dbal_access.h"
#include "trunk_sw_db.h"
#include "trunk_temp_structs_to_skip_papi.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_trunk_access.h>

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
 * \brief - set smooth division profiles to the dbal
 * 
 * \param [in] unit - unit number
 * \param [in] profile_array - array of smooth division profiles
 * \param [in] nof_profiles - number of profiles
 * \param [in] entries_per_profile - number of entries per 
 *        profile
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
dnx_trunk_smd_profiles_to_dbal_set(
    int unit,
    int *profile_array,
    int nof_profiles,
    int entries_per_profile)
{
    int profile;
    int entry;
    int value;
    SHR_FUNC_INIT_VARS(unit);

    for (profile = 0; profile < nof_profiles; ++profile)
    {
        for (entry = 0; entry < entries_per_profile; ++entry)
        {
            value = profile_array[profile * entries_per_profile + entry];
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_smooth_division_profile_configuration_set(unit, profile,
                                                                                            entry, value));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - init of the SMD PSC
 * 
 * \param [in] unit - unit number
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
dnx_trunk_smd_init(
    int unit)
{
    int *profile_array;
    int nof_profiles;
    int entries_per_profile;
    SHR_FUNC_INIT_VARS(unit);

    /** the max number of SMD members also defines the max amount of default profiles */
    nof_profiles = dnx_data_trunk.psc.smooth_division_max_nof_member_get(unit);
    entries_per_profile = dnx_data_trunk.psc.smooth_division_entries_per_profile_get(unit);

    /** allocate 2D array in a single allocation as a flat 1D array and access it with pointer aritmetics */
    profile_array = sal_alloc(nof_profiles * entries_per_profile * sizeof(int), "SMD init profile array");
    if (profile_array == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "allocation failed");
    }

    SHR_IF_ERR_EXIT(dnx_algo_trunk_smd_pre_defined_profiles_get(unit, profile_array,
                                                                nof_profiles, entries_per_profile));
    SHR_IF_ERR_EXIT(dnx_trunk_smd_profiles_to_dbal_set(unit, profile_array, nof_profiles, entries_per_profile));

exit:
    SHR_FREE(profile_array);
    SHR_FUNC_EXIT;
}

/**
 * \brief - init port selection criterias
 * 
 * \param [in] unit - unit number
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
dnx_trunk_psc_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_trunk.psc.feature_get(unit, dnx_data_trunk_psc_multiply_and_divide))
    {
        /**
         * init multiply_and_divide - currently a place holder, nothing 
         * to init in this PSC 
         */
    }

    if (dnx_data_trunk.psc.feature_get(unit, dnx_data_trunk_psc_smooth_division))
    {
        /**
         * init smooth_division. 
         * set pre-defined profiles.
         */
        SHR_IF_ERR_EXIT(dnx_trunk_smd_init(unit));
    }

    if (dnx_data_trunk.psc.feature_get(unit, dnx_data_trunk_psc_consistant_hashing))
    {
        /**
         * init consistant_hashing - currently a place holder.
         */
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - init trunk HW
 * 
 * \param [in] unit - unit number
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
dnx_trunk_hw_init(
    int unit)
{
    int nof_pools;
    int pool_index;
    int pool_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** init Trunk pool attributes */
    nof_pools = dnx_data_trunk.parameters.nof_pools_get(unit);
    for (pool_index = 0; pool_index < nof_pools; ++pool_index)
    {
        /** get pool mode */
        pool_mode = dnx_data_trunk.parameters.pool_info_get(unit, pool_index)->pool_hw_mode;
        /** write to DBAL */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_pool_attributes_set(unit, pool_index, pool_mode));
    }

    /** Init Egress trunk attributes */
    if (dnx_data_trunk.egress_trunk.feature_get(unit, dnx_data_trunk_egress_trunk_multiple_egress_trunk_sizes))
    {
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_egress_trunk_attributes_set
                        (unit, dnx_data_trunk.egress_trunk.size_mode_get(unit)));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /**
     * initiate trunk sw db
     */
    SHR_IF_ERR_EXIT(algo_trunk_db.init(unit));

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_init(unit));

    SHR_IF_ERR_EXIT(dnx_trunk_hw_init(unit));

    /**
     * initiate trunk psc features, for each feature check if it is
     * supported and activate init function for it
     */
    SHR_IF_ERR_EXIT(dnx_trunk_psc_init(unit));

    /** trunk algo init */
    SHR_IF_ERR_EXIT(dnx_algo_egress_trunk_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
     * deinit trunk sw db
     */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_deinit(unit));

    /** trunk algo deinit */
    SHR_IF_ERR_EXIT(dnx_algo_egress_trunk_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}
