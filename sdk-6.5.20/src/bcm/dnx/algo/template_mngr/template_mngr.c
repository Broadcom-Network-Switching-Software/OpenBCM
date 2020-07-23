/** \file template_mngr.c
 *
 * Template management functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR

/**
* INCLUDE FILES:
* {
*/
#include <bcm/types.h>
#include <shared/swstate/sw_state.h>
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_callbacks.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>

/**
 * }
 */

/*
 * Assert is compiled out on official releases.
 */
#if 1
#define TEMPLATE_MANAGER_ASSERT(phrase)
#endif

/*
 * Verify input for dnx_algo_template_create.
 */
static shr_error_e
dnx_algo_template_create_verify(
    int unit,
    dnx_algo_template_create_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data, _SHR_E_PARAM, "create_data");

    /*
     * Verify generic data.
     */
    if (data->first_profile < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "First profile can't be negative. Got: %d", data->first_profile);
    }

    if (data->nof_profiles < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of profiles must be at least 1. Got: %d", data->nof_profiles);
    }

    if (data->data_size < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Data size must be at least 1. Got: %d", (int) data->data_size);
    }

    if (data->max_references < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max references must be at least 1. Got: %d", (int) data->max_references);
    }

    /*
     * Verify advanced algorithm. DNX_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM
     */
    if (_SHR_IS_FLAG_SET(data->flags, DNX_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM))
    {
        if (data->advanced_algorithm < DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC
            || data->advanced_algorithm >= DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_COUNT)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The advanced algorithm is invalid. Must be at least %d and no bigger than but %d we got: %d",
                         DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC, DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_COUNT,
                         data->advanced_algorithm);
        }
    }
    else
    {
        if (data->advanced_algorithm != DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "If the DNX_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM flag is not set, the advanced algorithm should be %d, but it is %d",
                         DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC, data->advanced_algorithm);
        }
    }

    /*
     *  Verify default profile if it's in use.
     */
    if (_SHR_IS_FLAG_SET(data->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        int max_profile;
        if (data->default_profile < data->first_profile)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Default profile is too small. Must be at least %d but got: %d",
                         data->first_profile, data->default_profile);
        }

        max_profile = data->first_profile + data->nof_profiles - 1;
        if (data->default_profile > max_profile)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Default profile is too high. Must be at most %d but got: %d",
                         max_profile, data->default_profile);
        }

        if (data->default_data == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "If default profile is in use, default data must be provided.");
        }
    }

    if (Template_callbacks_map_sw[data->advanced_algorithm].create_cb == NULL
        || Template_callbacks_map_sw[data->advanced_algorithm].allocate_cb == NULL
        || Template_callbacks_map_sw[data->advanced_algorithm].profile_data_get_cb == NULL
        || Template_callbacks_map_sw[data->advanced_algorithm].profile_get_cb == NULL
        || Template_callbacks_map_sw[data->advanced_algorithm].free_cb == NULL)
    {
#ifdef PTRS_ARE_64BITS
/* { */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "All mandatory APIs, create_cb, allocate_cb, profile_data_get_cb, profile_get_cb and free_cb"
                     "must be provided.\n"
                     "Advanced algorithm index is: %d\n"
                     "Pointers are: %p, %p, %p, %p, %p",
                     data->advanced_algorithm,
                     (void *) (uint64) (Template_callbacks_map_sw[data->advanced_algorithm].create_cb),
                     (void *) (uint64) (Template_callbacks_map_sw[data->advanced_algorithm].allocate_cb),
                     (void *) (uint64) (Template_callbacks_map_sw[data->advanced_algorithm].profile_data_get_cb),
                     (void *) (uint64) (Template_callbacks_map_sw[data->advanced_algorithm].profile_get_cb),
                     (void *) (uint64) (Template_callbacks_map_sw[data->advanced_algorithm].free_cb));
/* } */
#else
/* { */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "All mandatory APIs, create_cb, allocate_cb, profile_data_get_cb, profile_get_cb and free_cb"
                     "must be provided.\n"
                     "Advanced algorithm index is: %d\n"
                     "Pointers are: %p, %p, %p, %p, %p",
                     data->advanced_algorithm,
                     (void *) (uint32) (Template_callbacks_map_sw[data->advanced_algorithm].create_cb),
                     (void *) (uint32) (Template_callbacks_map_sw[data->advanced_algorithm].allocate_cb),
                     (void *) (uint32) (Template_callbacks_map_sw[data->advanced_algorithm].profile_data_get_cb),
                     (void *) (uint32) (Template_callbacks_map_sw[data->advanced_algorithm].profile_get_cb),
                     (void *) (uint32) (Template_callbacks_map_sw[data->advanced_algorithm].free_cb));
/* } */
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Register the template's data upon creation.
 * All data is considered to already be checked by dnx_algo_template_create_verify.
 */
static shr_error_e
dnx_algo_template_register_data(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    dnx_algo_template_create_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Simply save the data to the sw state structure.
     */
    DNX_SW_STATE_MEMCPY(unit,
                        module_id,
                        algo_temp->flags, &(data->flags), algo_temp->flags, DNXC_SW_STATE_NO_FLAGS, "Template flags.");

    DNX_SW_STATE_MEMCPY(unit,
                        module_id,
                        algo_temp->first_profile,
                        &(data->first_profile),
                        algo_temp->first_profile, DNXC_SW_STATE_NO_FLAGS, "Template first_profile.");

    DNX_SW_STATE_MEMCPY(unit,
                        module_id,
                        algo_temp->nof_profiles,
                        &(data->nof_profiles),
                        algo_temp->nof_profiles, DNXC_SW_STATE_NO_FLAGS, "Template nof_profiles.");

    DNX_SW_STATE_MEMCPY(unit,
                        module_id,
                        algo_temp->data_size,
                        &(data->data_size), algo_temp->data_size, DNXC_SW_STATE_NO_FLAGS, "Template data_size.");

    DNX_SW_STATE_MEMCPY(unit,
                        module_id,
                        algo_temp->default_profile,
                        &(data->default_profile),
                        algo_temp->default_profile, DNXC_SW_STATE_NO_FLAGS, "Template default_profile.");

    DNX_SW_STATE_MEMCPY(unit,
                        module_id,
                        algo_temp->max_references,
                        &(data->max_references),
                        algo_temp->max_references, DNXC_SW_STATE_NO_FLAGS, "Template max_references.");

    DNX_SW_STATE_MEMCPY(unit,
                        module_id,
                        algo_temp->advanced_algorithm,
                        &(data->advanced_algorithm),
                        algo_temp->advanced_algorithm, DNXC_SW_STATE_NO_FLAGS, "Template advanced_algorithm.");

    sw_state_string_strncpy
        (unit, module_id, algo_temp->name, sizeof(char) * (DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1), data->name);

    if (_SHR_IS_FLAG_SET(data->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        DNX_SW_STATE_ALLOC(unit,
                           module_id,
                           algo_temp->default_data,
                           DNX_SW_STATE_BUFF, data->data_size, DNXC_SW_STATE_NO_FLAGS, "Template default_data.");

        DNX_SW_STATE_MEMWRITE(unit,
                              module_id,
                              data->default_data,
                              algo_temp->default_data,
                              0, data->data_size, DNXC_SW_STATE_NO_FLAGS, "Template default_data.");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_template_create(
    int unit,
    uint32 module_id,
    dnx_algo_template_t * algo_temp,
    dnx_algo_template_create_data_t * data,
    void *extra_arguments,
    uint32 alloc_flags)
{
    uint8 first_reference;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Logging
     */
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META
                     ("\n dnx_algo_template_create(unit:%d, name:%s, data->flags:%d, data->first_profile:%d, data->nof_profiles:%d, data->max_references:%d,"),
                     unit, data->name, data->flags, data->first_profile, data->nof_profiles, data->max_references));
        if (_SHR_IS_FLAG_SET(data->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                        (BSL_META
                         (" data->default_profile:%d, data->default_data:%p,"),
                         data->default_profile, (void *) (data->default_data)));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META(" data->data_size:%d, extra_arguments:%p)\n"), data->data_size,
                     (void *) extra_arguments));
    }

    /*
     * 0. Verify that the template was not allocated, and perform verifications on the create info.
     */
    if (*algo_temp != NULL)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "The template pointer is already allocated.\n");
    }
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_create_verify(unit, data));

    /*
     * 1. Allocate template.
     */
    DNX_SW_STATE_ALLOC(unit, module_id, *algo_temp, **algo_temp, /* number of elements */ 1, /* flags */ 0,
                       "sw_state algo_temp");

    /*
     * 2. Call the create function and register the template data.
     */

    /*
     * If we are using the DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE flag for creating the template and
     * if the DNX_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE is not,
     * the max_references should be increased by one.
     * It is increased only for the purpose of manipulating the underlying cbs implementation,
     * in a few lines we will revert it.
     */
    if (!_SHR_IS_FLAG_SET(data->flags, DNX_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE)
        && _SHR_IS_FLAG_SET(data->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        data->max_references = data->max_references + 1;
    }

    /*
     * 2.1 Call the create function.
     */

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[data->advanced_algorithm].create_cb
                    (unit, module_id, &((*algo_temp)->multi_set_template), data, extra_arguments, alloc_flags));

    if (_SHR_IS_FLAG_SET(data->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[data->advanced_algorithm].allocate_cb
                        (unit, module_id, (*algo_temp)->multi_set_template,
                         DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, data->max_references, data->default_data,
                         NULL, &data->default_profile, &first_reference));
    }

    /*
     * Reverting the value of max references back to the user specified value.
     * (done manipulating the underlying implementation)
     */
    if (!_SHR_IS_FLAG_SET(data->flags, DNX_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE)
        && _SHR_IS_FLAG_SET(data->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        data->max_references = data->max_references - 1;
    }
    /*
     * 2.2 Register the template data.
     *     i.e save the create data given by the user into the template manager instance
     */
    SHR_IF_ERR_EXIT(dnx_algo_template_register_data(unit, module_id, *algo_temp, data));

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_create(name:%s) Result:%d\n"), (*algo_temp)->name,
                     SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_template_destroy(
    int unit,
    uint32 module_id,
    dnx_algo_template_t * algo_temp,
    void *extra_arguments)
{
    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_destroy(unit:%d, name:%s, extra_arguments:%p)\n"), unit,
                     (*algo_temp)->name, (void *) extra_arguments));
    }

    /*
     * Verify that the template exist.
     */
    if ((*algo_temp) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Template doesn't exist.");
    }

    /*
     * 1. Call the destroy function for the algorithm.
     */

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[(*algo_temp)->advanced_algorithm].destroy_cb
                    (unit, module_id, &((*algo_temp)->multi_set_template), extra_arguments));

    DNX_SW_STATE_FREE(unit, module_id, (*algo_temp)->multi_set_template, "sw_state algo_temp multiset");

    DNX_SW_STATE_FREE(unit, module_id, *algo_temp, "sw_state algo_temp");

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_destroy(name:%s) Result:%d\n"), (*algo_temp)->name,
                     SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

/*
 * Verify that a profile is within legal range of the template.
 */
static shr_error_e
dnx_algo_template_profile_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the template exist.
     */
    if (algo_temp == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Template doesn't exist.");
    }

    /*
     * Verify.
     */
    if (profile < algo_temp->first_profile)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Profile is too small. Profile is %d and minimum is %d ", profile,
                     algo_temp->first_profile);
    }

    if (profile >= algo_temp->first_profile + algo_temp->nof_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Profile is too large. Profile is %d and maximum is %d ", profile,
                     algo_temp->first_profile + algo_temp->nof_profiles);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify that template is legal.
 */
static shr_error_e
dnx_algo_template_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the template exist.
     */
    if (algo_temp == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Template doesn't exist.");
    }

    /*
     * log this access in multi threading analyzer tool
     * for now always log write=TRUE
     */
    DNXC_MTA(SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_log_resource_use(unit,
                                                                        MTA_RESOURCE_TMP_MGR,
                                                                        dnxc_multithread_analyzer_dynamic_sub_res_get
                                                                        (unit, MTA_DYN_SUB_RES_TMP_MGR,
                                                                         algo_temp->name), TRUE)));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify input for dnx_algo_template_allocate.
 */
static shr_error_e
dnx_algo_template_allocate_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    uint32 flags,
    void *profile_data,
    int *profile,
    uint8 *first_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile_data, _SHR_E_PARAM, "profile_data");
    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");
    SHR_NULL_CHECK(first_reference, _SHR_E_PARAM, "first_reference")
        /*
         * Verify that the resource exist.
         */
        SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

    if (_SHR_IS_FLAG_SET(algo_temp->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Can't use allocate function when default profile is used: \"%s.\"",
                     algo_temp->name);
    }

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
    {
        SHR_INVOKE_VERIFY_DNX(dnx_algo_template_profile_verify(unit, module_id, algo_temp, *profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in template_mngr.h.
 */
shr_error_e
dnx_algo_template_allocate(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    uint32 flags,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    /*
     * Logging.
     */ if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META
                     ("\n dnx_algo_template_allocate(unit:%d, name:%s, flags:%d, profile_data:%p, extra_arguments:%p"),
                     unit, algo_temp->name, flags, (void *) profile_data, (void *) extra_arguments));
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", profile:%d"), *profile));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(")\n")));
    }

    /*
     * Verify input.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_allocate_verify
                          (unit, module_id, algo_temp, flags, profile_data, profile, first_reference));

    /*
     * call the allocate callback based on the selected (advanced) algorithm.
     */
    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                    (unit, module_id, algo_temp->multi_set_template, flags, 1 /** nof refs */ ,
                     profile_data, extra_arguments, profile, first_reference));

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META("\n dnx_algo_template_allocate(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                        (BSL_META(", allocated_profile:%d, first_reference:%d"), *profile, *first_reference));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * Common verification function for dnx_algo_template_profile_data_get and dnx_algo_template_free.
 */
static shr_error_e
dnx_algo_template_and_profile_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify template.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

    /*
     * Verify profile.
     */
    SHR_IF_ERR_EXIT(dnx_algo_template_profile_verify(unit, module_id, algo_temp, profile));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verification for dnx_algo_template_profile_data_get.
 */
static shr_error_e
dnx_algo_template_profile_data_get_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile,
    int *ref_count,
    void *profile_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (ref_count == NULL && profile_data == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The pointers ref_count=\"%p.\" and profile_data=\"%p.\" can't be NULL at the same time",
                     (void *) ref_count, (void *) profile_data);
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_and_profile_verify(unit, module_id, algo_temp, profile));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See description in template_mngr.h.
 */
shr_error_e
dnx_algo_template_profile_data_get(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile,
    int *ref_count,
    void *profile_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_profile_data_get(unit:%d, name:%s, profile:%d)\n"),
                     unit, algo_temp->name, profile));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_profile_data_get_verify
                          (unit, module_id, algo_temp, profile, ref_count, profile_data));

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                    (unit, module_id, algo_temp->multi_set_template, profile, ref_count, profile_data));

    /*
     * when template was created with default profile, we reduce
     * ref_count by 1 on the default profile so that in the
     * underlying implementation ref_count will never be zero and
     * the default profile will never be deleted (unless
     * ALLOW_DEFAULT_PROFILE_OVERRIDE is also defined)
     */
    if (!_SHR_IS_FLAG_SET(algo_temp->flags, DNX_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE)
        && _SHR_IS_FLAG_SET(algo_temp->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        if (algo_temp->default_profile == profile && ref_count != NULL)
        {
            *ref_count = *ref_count - 1;
        }
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_profile_data_get(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            if (ref_count != NULL)
            {
                LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", ref_count:%d"), *ref_count));
            }

            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", profile_data:%p"), (void *) profile_data));

        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * Verification for dnx_algo_template_free.
 */
static shr_error_e
dnx_algo_template_free_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(last_reference, _SHR_E_PARAM, "last_reference");

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_and_profile_verify(unit, module_id, algo_temp, profile));

    /*
     * Verify that free operation is allowed on this template (not allowed when there's
     *   a default profile)
     */

    if (_SHR_IS_FLAG_SET(algo_temp->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Can't use free function when default profile is used: \"%s.\"", algo_temp->name);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See description in template_mngr.h.
 */
shr_error_e
dnx_algo_template_free(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_free(unit:%d, name:%s, profile:%d)\n"), unit,
                     algo_temp->name, profile));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_free_verify(unit, module_id, algo_temp, profile, last_reference));

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_cb
                    (unit, module_id, algo_temp->multi_set_template, profile, 1, last_reference));

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META("\n dnx_algo_template_free(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", last_reference:%d"), *last_reference));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * Verification for dnx_algo_template_exchange.
 */
static shr_error_e
dnx_algo_template_exchange_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    uint32 flags,
    const void *profile_data,
    int old_profile,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference)
{
    int ref_count, profile;
    int rv = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(new_profile, _SHR_E_PARAM, "new_profile");
    SHR_NULL_CHECK(first_reference, _SHR_E_PARAM, "last_reference");
    SHR_NULL_CHECK(last_reference, _SHR_E_PARAM, "last_reference");

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

    if (!_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA))
    {
        /*
         * If ignore data flag is not set, then profile_data can't be NULL.
         */
        SHR_NULL_CHECK(profile_data, _SHR_E_PARAM, "profile_data");
    }

    /*
     * Verify old profile.
     */
    SHR_IF_ERR_EXIT(dnx_algo_template_profile_verify(unit, module_id, algo_temp, old_profile));

    if (!_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_template_profile_data_get(unit, module_id, algo_temp, old_profile, &ref_count, NULL));
        if (ref_count == 0)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "old_profile %d doesn't exist. Please use IGNORE_NOT_EXIST_OLD_PROFILE if that's intentional",
                         old_profile);
        }
    }

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
    {
        /*
         * Verify new_profile if WITH_ID.
         */
        SHR_INVOKE_VERIFY_DNX(dnx_algo_template_profile_verify(unit, module_id, algo_temp, *new_profile));

        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA))
        {
            SHR_IF_ERR_EXIT(dnx_algo_template_profile_data_get
                            (unit, module_id, algo_temp, *(int *) new_profile, &ref_count, NULL));
            if (ref_count == 0)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "New profile %d doesn't exist and IGNORE_DATA flag is set. Can't add a new profile without data.",
                             *(int *) new_profile);
            }
        }
        else
        {
            rv = dnx_algo_template_profile_get(unit, module_id, algo_temp, profile_data, &profile);
            if (rv != _SHR_E_NOT_FOUND && profile != *(int *) new_profile)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Can't have two profiles with the same data. Given data already exists in entry %d, and required profile is %d",
                             profile, *new_profile);
            }
            else
            {
                rv = _SHR_E_NONE;
            }
        }
    }
    else
    {
        /*
         * Check flags.
         * Full explanation is in template_mngr.h.
         */
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When using DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA flag, "
                         "DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID must also be set.");
        }
    }

    if ((_SHR_IS_FLAG_SET(algo_temp->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
        && (!_SHR_IS_FLAG_SET(algo_temp->flags, DNX_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_template_profile_data_get(unit, module_id, algo_temp, old_profile, &ref_count, NULL));
        if (ref_count == 0 && old_profile == algo_temp->default_profile)
        {
            /*
             * note the we check ref_count == 0 and not == 1 because
             *  profile_data_get is already adjusting to omit one
             *  reference so that default profile is never deleted
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "attempt to free default profile without ALLOW_DEFAULT_PROFILE_OVERRIDE flag.");
        }
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verification for dnx_algo_template_replace_data.
 */
static shr_error_e
dnx_algo_template_replace_data_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile,
    const void *new_profile_data)
{
    int ref_count;
    int rv;
    int tmp_profile;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

    /*
     * The new_profile_data can't be NULL.
     */
    SHR_NULL_CHECK(new_profile_data, _SHR_E_PARAM, "new_profile_data");

    /*
     * Verify the profile.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_profile_verify(unit, module_id, algo_temp, profile));

    SHR_IF_ERR_EXIT(dnx_algo_template_profile_data_get(unit, module_id, algo_temp, profile, &ref_count, NULL));
    if (ref_count == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "We can't change the data of a non-existing profile.");
    }

    rv = dnx_algo_template_profile_get(unit, module_id, algo_temp, new_profile_data, &tmp_profile);
    if (rv == _SHR_E_NONE && tmp_profile != profile)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "Can't have two profiles with the same data. Data already exists in profile %d",
                     tmp_profile);
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        rv = _SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT;
}
/*
 * Replace a data pointer with new data.
 */

static shr_error_e
dnx_algo_template_generic_replace_data(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile,
    const void *new_profile_data)
{
    MULTI_SET_KEY *profile_old_data = NULL;
    uint8 old_data_removed = FALSE;
    uint8 new_data_added = FALSE;
    uint8 last_reference, first_reference;
    int ref_count;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the profile.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_profile_verify(unit, module_id, algo_temp, profile));

    /*
     *  Get the current data at index (and the ref count to it) and save it into the buffer in case of failure.
     */
    profile_old_data = sal_alloc(algo_temp->data_size, "Old data of new profile..");

    SHR_NULL_CHECK(profile_old_data, _SHR_E_MEMORY, "Failed to allocate pointer profile_old_data.");

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                    (unit, module_id, algo_temp->multi_set_template, profile, &ref_count, profile_old_data));

    /*
     *  Free all members using the old data.
     */
    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_cb
                    (unit, module_id, algo_temp->multi_set_template, profile, ref_count, &last_reference));
    old_data_removed = TRUE;

    /*
     *  Reallocate using new data.
     */
    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                    (unit, module_id, algo_temp->multi_set_template, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                     ref_count, (void *) new_profile_data, NULL, &profile, &first_reference));

    new_data_added = TRUE;

exit:
    SHR_FREE(profile_old_data);
    if (old_data_removed == TRUE && new_data_added == FALSE)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                        (unit, module_id, algo_temp->multi_set_template, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                         ref_count, profile_old_data, NULL, &profile, &first_reference));
    }
    SHR_FUNC_EXIT;
}
/*
 * See description in template_mngr.h.
 */
shr_error_e
dnx_algo_template_replace_data(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile,
    const void *new_profile_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META
                     ("\n dnx_algo_template_replace_data(unit:%d, name:%s, profile:%d, new_profile_data:%p)\n"),
                     unit, algo_temp->name, profile, (void *) new_profile_data));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_replace_data_verify(unit, module_id, algo_temp, profile, new_profile_data));

    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].replace_data_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].replace_data_cb
                        (unit, module_id, algo_temp->multi_set_template, profile, new_profile_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_template_generic_replace_data(unit, module_id, algo_temp, profile, new_profile_data));
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_replace_data(name:%s) Result:%d"), algo_temp->name,
                     SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_template_profile_get_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    const void *profile_data,
    int *profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile_data, _SHR_E_PARAM, "profile_data");
    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_template_profile_get(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    const void *profile_data,
    int *profile)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_profile_get(unit:%d, name:%s, profile_data:%p)\n"),
                     unit, algo_temp->name, (void *) profile_data));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_profile_get_verify(unit, module_id, algo_temp, profile_data, profile));

    rv = Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_get_cb(unit, module_id,
                                                                                 algo_temp->multi_set_template,
                                                                                 profile_data, profile);

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(rv);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_profile_get(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", profile:%d"), *profile));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_template_clear(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp)
{
    int current_profile;
    int max_references;
    uint8 first_reference;
    MULTI_SET_KEY *default_data = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_clear(unit:%d, name:%s,)\n"), unit, algo_temp->name));
    }

    current_profile = 0;
    first_reference = 0;
    max_references = algo_temp->max_references;

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].clear_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].clear_cb
                        (unit, module_id, algo_temp->multi_set_template));
    }
    else
    {
        while (TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_template_get_next(unit, module_id, algo_temp, &current_profile));
            if (current_profile == DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE)
            {
                break;
            }
            SHR_IF_ERR_EXIT(dnx_algo_template_free_all(unit, module_id, algo_temp, current_profile++));
        }
    }

    if (_SHR_IS_FLAG_SET(algo_temp->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        if (!_SHR_IS_FLAG_SET(algo_temp->flags, DNX_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE))
        {
            max_references = max_references + 1;
        }

        default_data = sal_alloc(algo_temp->data_size, "Default data backup");
        SHR_NULL_CHECK(default_data, _SHR_E_MEMORY, "default_data");
        sal_memset(default_data, 0, algo_temp->data_size);

        DNX_SW_STATE_MEMREAD(unit,
                             default_data,
                             algo_temp->default_data, 0, algo_temp->data_size, 0, "template_mngr default_data");

        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                        (unit, module_id, algo_temp->multi_set_template,
                         DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, max_references, default_data, NULL,
                         &(algo_temp->default_profile), &first_reference));
    }

exit:
    SHR_FREE(default_data);
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_clear(name:%s,) Result:%d\n"), algo_temp->name,
                     SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    SHR_FUNC_EXIT;
}

/*
 * Verification function for dnx_algo_template_profile_get_next .
 */
static shr_error_e
dnx_algo_template_get_next_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int *profile)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify template.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

    /*
     * Verify profile.
     */
    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "The current profile is not allocated.");

    if (*profile < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The current profile %d is illegal.", *profile);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_template_get_next(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int *current_profile)
{
    int profile_end, profile_iter;
    int ref_count;
    MULTI_SET_KEY *tmp_profile_data = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_get_next(unit:%d, name:%s)\n"), unit, algo_temp->name));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_get_next_verify(unit, module_id, algo_temp, current_profile));

    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].get_next_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].get_next_cb
                        (unit, module_id, algo_temp->multi_set_template, current_profile));
    }
    else
    {
        tmp_profile_data = sal_alloc(algo_temp->data_size, "Data buffer for tmp template data.");

        SHR_NULL_CHECK(tmp_profile_data, _SHR_E_PARAM, "tmp_profile_data");

        profile_end = algo_temp->first_profile + algo_temp->nof_profiles;
        profile_iter = UTILEX_MAX(algo_temp->first_profile, *current_profile);

        for (; profile_iter < profile_end; profile_iter++)
        {
            SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                            (unit, module_id, algo_temp->multi_set_template, profile_iter, &ref_count,
                             tmp_profile_data));
            if (ref_count > 0)
            {
                break;
            }
        }

        if (profile_iter < profile_end)
        {
            *current_profile = profile_iter;
        }
        else
        {
            *current_profile = DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE;
        }
    }

exit:
    SHR_FREE(tmp_profile_data);
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META("\n dnx_algo_template_get_next(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", current_profile:%d"), *current_profile));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * Verification for dnx_algo_template_free_all.
 */
static shr_error_e
dnx_algo_template_free_all_verify(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify template.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

    /*
     * Verify profile.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_profile_verify(unit, module_id, algo_temp, profile));

    /*
     * Verify that free operation is allowed on this template (not allowed when there's
     *   a default profile)
     */

    if (_SHR_IS_FLAG_SET(algo_temp->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Can't use free function when default profile is used: \"%s.\"", algo_temp->name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_template_free_all(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    int profile)
{
    int ref_count;
    uint8 last_reference;
    MULTI_SET_KEY *tmp_profile_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_free_all(unit:%d, name:%s, profile:%d)\n"), unit,
                     algo_temp->name, profile));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_free_all_verify(unit, module_id, algo_temp, profile));

    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_all_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_all_cb
                        (unit, module_id, algo_temp->multi_set_template, profile));
    }
    else
    {
        tmp_profile_data = sal_alloc(algo_temp->data_size, "Data buffer for tmp template data.");
        SHR_NULL_CHECK(tmp_profile_data, _SHR_E_PARAM, "tmp_profile_data");

        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                        (unit, module_id, algo_temp->multi_set_template, profile, &ref_count, tmp_profile_data));

        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_cb
                        (unit, module_id, algo_temp->multi_set_template, profile, ref_count, &last_reference));
    }

exit:
    SHR_FREE(tmp_profile_data);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_free_all(name:%s) Result:%d\n"), algo_temp->name,
                     SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

/*
 * This function frees one reference from old_profile, add one reference to a profile mapped to profile_data.
 */
static shr_error_e
dnx_algo_template_generic_exchange(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    uint32 flags,
    const void *profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference)
{
    MULTI_SET_KEY *old_profile_data = NULL, *new_profile_old_data = NULL;
    int ref_count_old;
    int new_profile_old_ref_count;
    uint8 old_profile_freed = FALSE;
    uint8 add_success = FALSE;
    uint32 nof_refs = 1;
    smart_template_alloc_info_t *extra_alloc_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First, we save the old data, to restore it in case the allocation fails, or in case of test.
     */
    old_profile_data = sal_alloc(algo_temp->data_size, "Data buffer old.");

    SHR_NULL_CHECK(old_profile_data, _SHR_E_PARAM, "old_profile_data");

    extra_alloc_info = (smart_template_alloc_info_t *) extra_arguments;
    if (algo_temp->advanced_algorithm == DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SMART_TEMPLATE)
    {
        if ((extra_alloc_info != NULL) && (extra_alloc_info->nof_refs > 0))
        {
            nof_refs = extra_alloc_info->nof_refs;
        }
    }
    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                    (unit, module_id, algo_temp->multi_set_template, old_profile, &ref_count_old, old_profile_data));

    if ((ref_count_old == 0) && !_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE))
    {
        /*
         * User gave old profile that was empty
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given old_profile %d doesn't exist.", old_profile);
    }

    if (ref_count_old != 0)
    {
        /*
         *  Remove one ref from old profile.
         */
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_cb
                        (unit, module_id, algo_temp->multi_set_template, old_profile, nof_refs, last_reference));
        /*
         *  Mark that one ref was freed in case we want to revert it later.
         */
        old_profile_freed = TRUE;
    }

    /*
     * WITH_ID means that the user would like to exchange the current profile for a specific new profile
     * (as opposed to changing the profile to any new profile containing the supplied data).
     * If IGNORE_DATA is not specified, then the user would also like to change the value of the new template.
     */
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
    {
        new_profile_old_data = sal_alloc(algo_temp->data_size, "Old data of new profile");

        SHR_NULL_CHECK(new_profile_old_data, _SHR_E_PARAM, "new_profile_old_data");

        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                        (unit, module_id, algo_temp->multi_set_template, *new_profile,
                         &new_profile_old_ref_count, new_profile_old_data));
    }

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA))
    {
        /*
         *  If IGNORE_DATA flag is set, take the existing data.
         */
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                        (unit, module_id, algo_temp->multi_set_template, flags, nof_refs,
                         new_profile_old_data, (void *) extra_arguments, new_profile, first_reference));
        add_success = TRUE;
    }
    else
    {
        /*
         *  In this block we perform the allocation of the ref.
         */
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                        (unit, module_id, algo_temp->multi_set_template, flags, nof_refs, (void *) profile_data,
                         (void *) extra_arguments, new_profile, first_reference));

        add_success = TRUE;
    }

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID)
        && (new_profile_old_ref_count > 0) && !_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA))
    {
        /*
         *  It will be weird for someone to modify the template's content on a WITH_ID call.
         *  Let's have this internal assert to catch such cases and analyze.
         *  if it happens, need to add some code that will revert the profile data change in case.
         *  of an error or simulation mode.
         */
        TEMPLATE_MANAGER_ASSERT(!sal_memcmp(new_profile_old_data, profile_data, algo_temp->data_size));
    }

    /*
     * If this is just an exchange test, then free the entries that were just allocated.
     */
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_TEST))
    {
        uint8 test_last_reference;

        /*
         * Deallocate one reference.
         */
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_cb
                        (unit, module_id, algo_temp->multi_set_template, *new_profile, nof_refs, &test_last_reference));

    }

exit:
    if (SHR_FUNC_ERR() || !add_success || _SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_TEST))
    {
        int rv;
        /*
         *  Add new failed, or test. Restore old data.
         */
        if (!add_success && !((SHR_GET_CURRENT_ERR() == _SHR_E_PARAM) || (SHR_GET_CURRENT_ERR() == _SHR_E_INTERNAL)))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_FULL);
        }

        if (old_profile_freed)
        {
            uint8 dummy_first_reference;

            rv = Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb(unit, module_id,
                                                                                      algo_temp->multi_set_template,
                                                                                      DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                                                                                      nof_refs, old_profile_data, NULL,
                                                                                      &old_profile,
                                                                                      &dummy_first_reference);
            if (SHR_FAILURE(rv))
            {
                /*
                 *  Can't restore data. Internal error
                 */
                SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            }
        }
    }

    /*
     * Free pointers.
     */
    SHR_FREE(old_profile_data);
    SHR_FREE(new_profile_old_data);
    SHR_FUNC_EXIT;
}

/*
 * See description in template_mngr.h.
 */
shr_error_e
dnx_algo_template_exchange(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    uint32 flags,
    const void *profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference)
{
    int rv, tmp_profile;

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META
                     ("\n dnx_algo_template_exchange(unit:%d, name:%s, flags:%d, profile_data:%p, old_profile:%d"),
                     unit, algo_temp->name, flags, (void *) profile_data, old_profile));
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", new_profile:%d"), *new_profile));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(")\n")));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_exchange_verify
                          (unit, module_id, algo_temp, flags, profile_data, old_profile, new_profile,
                           first_reference, last_reference));

    /*
     * check the profile that is mapped to the input data
     */
    rv = dnx_algo_template_profile_get(unit, module_id, algo_temp, profile_data, &tmp_profile);
    if (rv == _SHR_E_NOT_FOUND)
    {
        /*
         * If the new data doesn't exist it's OK, just mark the profile as ILLEGAL.
         */
        tmp_profile = DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE;
        rv = _SHR_E_NONE;
    }
    SHR_IF_ERR_EXIT(rv);

    if (tmp_profile == old_profile)
    {
        /*
         * Special case - if the data of the new profile already exists in the old profile,
         * then we just remove and add a pointer to the same profile, which is actually meaningless.
         * In this case, we just return the old profile and no first/last indication.
         * The indications are set to false to save hw access.
         */
        *new_profile = old_profile;
        *first_reference = FALSE;
        *last_reference = FALSE;
    }
    else
    {
        if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].exchange_cb != NULL)
        {
            SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].exchange_cb
                            (unit, module_id, algo_temp->multi_set_template, flags, 1, profile_data,
                             old_profile, extra_arguments, new_profile, first_reference, last_reference));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_template_generic_exchange
                            (unit, module_id, algo_temp, flags, (void *) profile_data, old_profile,
                             (void *) extra_arguments, new_profile, first_reference, last_reference));
        }
    }

    /*
     * If a profile was cleared, then used again, then the template manager returns
     * first_reference indication but omit the last_reference indication
     * There is an assumption that when the user configure the new data due to the first_reference
     * indication, it overides the data of the removed profile and thus there is no need to return
     * last_reference indication
     */
    if (*first_reference && *last_reference && old_profile == *new_profile)
    {
        *last_reference = FALSE;
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META("\n dnx_algo_template_exchange(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                        (BSL_META(", allocated new_profile:%d, first_reference:%d, last_reference:%d"), *new_profile,
                         *first_reference, *last_reference));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_template_dump_info_get(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    dnx_algo_template_dump_data_t * info)
{
    int current_profile;

    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_template_dump_info_get(unit:%d, name:%s)\n"), unit, algo_temp->name));
    }

    /*
     * Verify template.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

    info->create_data.flags = algo_temp->flags;
    info->create_data.first_profile = algo_temp->first_profile;
    info->create_data.nof_profiles = algo_temp->nof_profiles;
    info->create_data.data_size = algo_temp->data_size;
    info->create_data.default_profile = algo_temp->default_profile;
    info->create_data.max_references = algo_temp->max_references;
    info->create_data.advanced_algorithm = algo_temp->advanced_algorithm;

    current_profile = info->create_data.first_profile;
    info->nof_used_profiles = 0;
    info->nof_free_profiles = 0;

    while (1)
    {
        SHR_IF_ERR_EXIT(dnx_algo_template_get_next(unit, module_id, algo_temp, &current_profile));

        if (current_profile == DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE)
        {
            break;
        }

        info->nof_used_profiles++;
        current_profile++;
    }
    info->nof_free_profiles = info->create_data.nof_profiles - info->nof_used_profiles;

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_template_dump_info_get(name:%s) Result:%d\n"), algo_temp->name,
                     SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

int
dnx_algo_template_print(
    int unit,
    uint32 module_id,
    dnx_algo_template_t * algo_temp,
    dnx_algo_template_print_data_cb print_cb)
{
    dnx_algo_template_dump_data_t data;

    void *tmp_data = NULL;
    int end;
    int current_profile;
    int tmp_ref_count;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the template exist.
     */
    if ((*algo_temp) == NULL)
    {
        SHR_EXIT();
    }

    DNX_SW_STATE_PRINT(unit, "\n Template :%s", (*algo_temp)->name);

    SHR_IF_ERR_EXIT(dnx_algo_template_dump_info_get(unit, module_id, (*algo_temp), &data));

    DNX_SW_STATE_PRINT(unit, " (Advanced Algorithm - %s)",
                       (data.create_data.advanced_algorithm ==
                        DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC ? "No" : "Yes"));

    DNX_SW_STATE_PRINT(unit, "| First profile:%d: ", (*algo_temp)->first_profile);
    DNX_SW_STATE_PRINT(unit, "| Nof profiles:%d: ", (*algo_temp)->nof_profiles);
    DNX_SW_STATE_PRINT(unit, "| Nof profiles in use:%d: ", data.nof_used_profiles);
    DNX_SW_STATE_PRINT(unit, "| Nof free profiles:%d:|\n", data.nof_free_profiles);

    if (data.nof_used_profiles)
    {
        current_profile = (*algo_temp)->first_profile;
        end = (*algo_temp)->nof_profiles + (*algo_temp)->first_profile;

        tmp_ref_count = 0;
        tmp_data = sal_alloc((*algo_temp)->data_size, "Tmp data");
        SHR_NULL_CHECK(tmp_data, _SHR_E_MEMORY, "tmp_data");

        while (1)
        {
            SHR_IF_ERR_EXIT(dnx_algo_template_get_next(unit, module_id, (*algo_temp), &current_profile));

            if ((current_profile == DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE) || (current_profile > end))
            {
                break;
            }

            SHR_IF_ERR_EXIT(dnx_algo_template_profile_data_get
                            (unit, module_id, (*algo_temp), current_profile, &tmp_ref_count, tmp_data));

            if (print_cb != NULL)
            {
                DNX_SW_STATE_PRINT(unit, "| Profile ID:%d: |", current_profile);
                DNX_SW_STATE_PRINT(unit, " Nof ref:%d: |", tmp_ref_count);

                print_cb(unit, tmp_data);

            }
            else
            {
                DNX_SW_STATE_PRINT(unit, "The print callback is not set.\n");
            }

            current_profile++;
        }
    }

    DNX_SW_STATE_PRINT(unit, "\n");

exit:
    SHR_FREE(tmp_data);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_template_advanced_algorithm_info_set(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    void *algorithm_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify template.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_advanced_algorithm_info_set(unit:%d, name:%s)\n"), unit,
                     algo_temp->name));
    }

    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].algorithm_info_set != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].algorithm_info_set(unit,
                                                                                                    module_id,
                                                                                                    algo_temp->multi_set_template,
                                                                                                    algorithm_info));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "advanced_algorithm_info_set callback is NULL.");
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_advanced_algorithm_info_set(name:%s)\n"), algo_temp->name));
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_template_advanced_algorithm_info_get(
    int unit,
    uint32 module_id,
    dnx_algo_template_t algo_temp,
    void *algorithm_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify template.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_template_verify(unit, module_id, algo_temp));

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_advanced_algorithm_info_get(unit:%d, name:%s)\n"), unit,
                     algo_temp->name));
    }

    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].algorithm_info_get != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].algorithm_info_get(unit,
                                                                                                    module_id,
                                                                                                    algo_temp->multi_set_template,
                                                                                                    algorithm_info));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "advanced_algorithm_info_get callback is NULL.");
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n dnx_algo_template_advanced_algorithm_info_get(name:%s)\n"), algo_temp->name));
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}
