/** \file res_mngr.c
 *
 * Resource management functions for sw state.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR
/**
* INCLUDE FILES:
* {
*/

/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <bcm/types.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_internal.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
/*
 * }
 */

/*
 * Other include files.
 * {
 */
/*
 * }
 */

extern const dnx_algo_res_advanced_alogrithm_cb_t Resource_callbacks_map[DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_COUNT];

/**
 * }
 */

/*
 *  Verifies if the resource actually exists.
 */
static shr_error_e
dnx_algo_res_verify(
    int unit,
    dnx_algo_res_t algo_res)
{
    SHR_FUNC_INIT_VARS(unit);

    if (algo_res == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Resource doesn't exist.");
    }

    /*
     * log this access in multi threading analyzer tool 
     */
    /*
     * for now always log write=TRUE 
     */
    DNXC_MTA(SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_log_resource_use(unit,
                                                                        MTA_RESOURCE_RES_MGR,
                                                                        dnxc_multithread_analyzer_dynamic_sub_res_get
                                                                        (unit, MTA_DYN_SUB_RES_RES_MGR, algo_res->name),
                                                                        TRUE)));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify input for dnx_algo_res_create.
 */
static shr_error_e
dnx_algo_res_create_verify(
    int unit,
    dnx_algo_res_create_data_t * data)
{
    uint8 use_advanced_algorithm;
    int max_name_length = DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data, _SHR_E_PARAM, "create_data");

    /*
     * Verify generic data.
     */

    if (sal_strlen(data->name) > max_name_length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name %s is too long. Must be at most %d characters", data->name, max_name_length);
    }

    if (data->first_element < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "First element must be at least 0. Got: %d", data->first_element);
    }

    if (data->nof_elements < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of elements must be at least 1. Got: %d", data->nof_elements);
    }

    /*
     * Verify callbacks if advanced algorithm is in use.
     */
    use_advanced_algorithm = _SHR_IS_FLAG_SET(data->flags, DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM);

    if (use_advanced_algorithm)
    {
        if (data->advanced_algorithm < DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC
            || data->advanced_algorithm >= DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_COUNT)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The advanced algorithm is invalid. Must be at least %d and no bigger than but %d we got: %d",
                         DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC, DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_COUNT,
                         data->advanced_algorithm);
        }
    }
    else if (!use_advanced_algorithm)
    {
        if (data->advanced_algorithm != DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC)
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "If the DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM flag is not set, the advanced algorithm should be %d, but it is %d",
                         DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC, data->advanced_algorithm);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Register the resource's data upon creation.
 */
static shr_error_e
dnx_algo_res_create_register_data(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    dnx_algo_res_create_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

    /*
     * Simply save the data to the sw state structure.
     */
    DNX_SW_STATE_MEMCPY(unit, module_id, algo_res->flags, &(data->flags), algo_res->flags, 0, "Resource flags.");
    DNX_SW_STATE_MEMCPY(unit, module_id, algo_res->first_element, &(data->first_element), algo_res->first_element,
                        0, "Resource first_element.");
    DNX_SW_STATE_MEMCPY(unit, module_id, algo_res->nof_elements, &(data->nof_elements), algo_res->nof_elements, 0,
                        "Resource nof_elements.");
    DNX_SW_STATE_MEMCPY(unit, module_id, algo_res->advanced_algorithm, &(data->advanced_algorithm),
                        algo_res->advanced_algorithm, 0, "Resource advanced_algorithm.");

    sw_state_string_strncpy
        (unit, module_id, algo_res->name, sizeof(char) * (DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1), data->name);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_create(
    int unit,
    uint32 module_id,
    dnx_algo_res_t * algo_res,
    dnx_algo_res_create_data_t * data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META
                     ("\n dnx_algo_res_create(unit:%d, data->name:%s, data->flags:%d, data->first_element:%d, data->nof_elements:%d, data->advanced_algorithm:%d, extra_arguments:%p)\n"),
                     unit, data->name, data->flags, data->first_element, data->nof_elements, data->advanced_algorithm,
                     extra_arguments));
    }

    /*
     * 0. Verify input.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_create_verify(unit, data));

    DNX_SW_STATE_ALLOC(unit, module_id, *algo_res, **algo_res, 1, 0, "*algo_res")
        /*
         * 1.1 Call the create function.
         */
        SHR_IF_ERR_EXIT(Resource_callbacks_map[data->advanced_algorithm].create_cb
                        (unit, module_id, &((*algo_res)->res_tag_bitmap), data,
                         extra_arguments, nof_elements, alloc_flags));

    /*
     * 1.2 Register the resource data.
     */
    SHR_IF_ERR_EXIT(dnx_algo_res_create_register_data(unit, module_id, (*algo_res), data));

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_create(name:%s) Result:%d\n"), data->name, SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

/*
 *  The only thing that needs to be verified is that the resource actually exists.
 *  Otherwise, we don't care about its properties.
 */
static shr_error_e
dnx_algo_res_destroy_verify(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_destroy(
    int unit,
    uint32 module_id,
    dnx_algo_res_t * algo_res,
    void *extra_arguments)
{
    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_destroy(unit:%d, name:%s, extra_arguemnts:%p)\n"), unit,
                     (*algo_res)->name, extra_arguments));
    }

    /*
     * 0. Verify input and get res index.
     */

    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_destroy_verify(unit, module_id, (*algo_res)));

    SHR_IF_ERR_EXIT(Resource_callbacks_map[(*algo_res)->advanced_algorithm].destroy_cb
                    (unit, module_id, &((*algo_res)->res_tag_bitmap), extra_arguments));

    DNX_SW_STATE_FREE(unit, module_id, *algo_res, "sw_state algo_res");

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_destroy(name:%s) Result:%d\n"), (*algo_res)->name,
                     SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

/*
 * Verify that an element is within legal range of the resource.
 */
static shr_error_e
dnx_algo_res_element_verify(
    int unit,
    dnx_algo_res_t algo_res,
    int element)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

    /*
     * Verify.
     */
    if (element < algo_res->first_element)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "resource %s Element is too low. Element is %d and minimum is %d ", algo_res->name,
                     element, algo_res->first_element);
    }

    if (element >= algo_res->first_element + algo_res->nof_elements)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "resource %s Element is too high. Element is %d and maximum is %d ", algo_res->name,
                     element, (algo_res->first_element + algo_res->nof_elements - 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify input for allocate function.
 */
static shr_error_e
dnx_algo_res_allocate_verify(
    int unit,
    dnx_algo_res_t algo_res,
    uint32 flags,
    int *element,
    int nof_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID))
    {
        SHR_IF_ERR_EXIT(dnx_algo_res_element_verify(unit, algo_res, *element));
    }

    if (Resource_callbacks_map[algo_res->advanced_algorithm].allocate_several_cb == NULL && nof_elements != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of elements must be 1 if allocate several call back is not implemented: \"%d.\"",
                     nof_elements);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
dnx_algo_res_allocate(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_RES_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META
                     ("\n dnx_algo_res_allocate(unit:%d, name:%s, extra_arguments:%p"), unit, algo_res->name,
                     extra_arguments));
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID))
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", element:%d"), *element));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(")\n")));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_allocate_verify(unit, algo_res, flags, element, 1));

    SHR_SET_CURRENT_ERR(dnx_algo_res_allocate_several
                        (unit, module_id, algo_res, flags | DNX_ALGO_RES_ALLOCATE_USING_SINGLE_ALLOCATE_CB, 1,
                         extra_arguments, element));

    SHR_EXIT();

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META("\n dnx_algo_res_allocate(name:%s"), algo_res->name));
        if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", allocated element:%d"), *element));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_RES_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * Common verification function for is_allocated and _delete, since they check similar inputs.
 */
static shr_error_e
dnx_algo_res_get_delete_verify(
    int unit,
    dnx_algo_res_t algo_res,
    uint8 nof_elements,
    int element)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

    /*
     * Verify element.
     */
    SHR_IF_ERR_EXIT(dnx_algo_res_element_verify(unit, algo_res, element));

    if (Resource_callbacks_map[algo_res->advanced_algorithm].free_several_cb == NULL && nof_elements != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of elements must be 1 if free several call back is not implemented: \"%d.\"",
                     nof_elements);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verification for _is_allocated API.
 */
shr_error_e
dnx_algo_res_get_verify(
    int unit,
    dnx_algo_res_t algo_res,
    int element,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_allocated, _SHR_E_PARAM, "is_allocated");

    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_get_delete_verify(unit, algo_res, 1, element));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See description in .h file.
 */
shr_error_e
dnx_algo_res_is_allocated(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    int element,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_RES_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META
                     ("\n dnx_algo_res_is_allocated(unit:%d, name:%s, element:%d)\n"), unit, algo_res->name, element));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_get_verify(unit, algo_res, element, is_allocated));

    SHR_IF_ERR_EXIT(Resource_callbacks_map[algo_res->advanced_algorithm].is_allocated_cb
                    (unit, module_id, algo_res->res_tag_bitmap, element, is_allocated));

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META("\n dnx_algo_res_is_allocated(name:%s"), algo_res->name));
        if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", is_allocated:%d"), *is_allocated));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_RES_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * See description in .h file.
 */
shr_error_e
dnx_algo_res_free(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    int element)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_RES_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_free(unit:%d, name:%s, element:%d)\n"), unit, algo_res->name, element));
    }

    SHR_IF_ERR_EXIT(dnx_algo_res_element_verify(unit, algo_res, element));

    SHR_IF_ERR_EXIT(dnx_algo_res_free_several(unit, module_id, algo_res, 1, element));

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_free(name:%s,) Result:%d\n"), algo_res->name, SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_RES_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * Verification function for dnx_algo_res_nof_free_elements_get.
 * Check taht pointer is not NULL and that resource is valid.
 */
static shr_error_e
dnx_algo_res_nof_free_elements_get_verify(
    int unit,
    dnx_algo_res_t algo_res,
    int *nof_free_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_free_elements, _SHR_E_PARAM, "nof_free_elements");

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_nof_free_elements_get(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    int *nof_free_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_nof_free_elements_get(unit:%d, name:%s)\n"), unit, algo_res->name));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_nof_free_elements_get_verify(unit, algo_res, nof_free_elements));

    SHR_IF_ERR_EXIT(Resource_callbacks_map[algo_res->advanced_algorithm].nof_free_elements_get_cb
                    (unit, module_id, algo_res->res_tag_bitmap, nof_free_elements));

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META("\n dnx_algo_res_nof_free_elements_get(name:%s"), algo_res->name));
        if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", nof_free_elements:%d"), *nof_free_elements));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_clear(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res)
{
    int current_element;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_clear(unit:%d, name:%s)\n"), unit, algo_res->name));
    }

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

    if (Resource_callbacks_map[algo_res->advanced_algorithm].clear_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Resource_callbacks_map[algo_res->advanced_algorithm].clear_cb
                        (unit, module_id, algo_res->res_tag_bitmap));
    }
    else
    {

        current_element = algo_res->first_element;

        for (; current_element < algo_res->nof_elements; current_element++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_is_allocated(unit, module_id, algo_res, current_element, &is_allocated));

            if (is_allocated == 1)
            {
                SHR_IF_ERR_EXIT(Resource_callbacks_map[algo_res->advanced_algorithm].free_cb
                                (unit, module_id, algo_res->res_tag_bitmap, current_element));
            }
        }
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_clear(name:%s) Result:%d\n"), algo_res->name, SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_res_get_next_verify(
    int unit,
    dnx_algo_res_t algo_res,
    int *element)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

    /*
     * Verify element.
     */
    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");

    if (*element < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The given element has negative value: \"%d.\"", *element);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_get_next(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    int *element)
{
    int current_element;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META
                     ("\n dnx_algo_res_get_next(unit:%d, name:%s, element:%d)\n"), unit, algo_res->name, *element));
    }

    SHR_IF_ERR_EXIT(dnx_algo_res_get_next_verify(unit, algo_res, element));

    if (Resource_callbacks_map[algo_res->advanced_algorithm].get_next_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Resource_callbacks_map[algo_res->advanced_algorithm].get_next_cb
                        (unit, module_id, algo_res->res_tag_bitmap, element));
    }
    else
    {

        current_element = UTILEX_MAX(algo_res->first_element, *element);

        for (; current_element < algo_res->first_element + algo_res->nof_elements; current_element++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_is_allocated(unit, module_id, algo_res, current_element, &is_allocated));
            if (is_allocated == 1)
            {
                break;
            }
        }

        if (current_element < algo_res->first_element + algo_res->nof_elements)
        {
            *element = current_element;
        }
        else
        {
            *element = DNX_ALGO_RES_ILLEGAL_ELEMENT;
        }
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META("\n dnx_algo_res_get_next(name:%s"), algo_res->name));
        if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", element:%d"), *element));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
dnx_algo_res_allocate_several(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_RES_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META
                     ("\n dnx_algo_res_allocate_several(unit:%d, name:%s, flags:%d, nof_elements:%d, extra_arguments:%p"),
                     unit, algo_res->name, flags, nof_elements, extra_arguments));
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID))
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", element:%d"), *element));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(")\n")));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_allocate_verify(unit, algo_res, flags, element, nof_elements));

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_USING_SINGLE_ALLOCATE_CB)
        || Resource_callbacks_map[algo_res->advanced_algorithm].allocate_several_cb == NULL)
    {
        rv = Resource_callbacks_map[algo_res->advanced_algorithm].allocate_cb(unit, module_id,
                                                                              algo_res->res_tag_bitmap,
                                                                              flags, extra_arguments, element);
    }
    else
    {
        rv = Resource_callbacks_map[algo_res->advanced_algorithm].allocate_several_cb(unit, module_id,
                                                                                      algo_res->res_tag_bitmap, flags,
                                                                                      nof_elements, extra_arguments,
                                                                                      element);
    }

    if ((rv == _SHR_E_RESOURCE) && (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID)))
    {
        /*
         * Some advanced algorithms may use res_bitmap as an internal implementation.
         * It returns E_RESOURCE when an element WITH_ID already exists. This error
         * code is irrelevant, so replace it with E_EXISTS.
         */
        rv = _SHR_E_EXISTS;
    }

    if ((_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_SIMULATION))
        && ((rv == _SHR_E_RESOURCE)
            || ((rv == _SHR_E_EXISTS) && (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID)))))
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META("\n dnx_algo_res_allocate_several(name:%s"), algo_res->name));
        if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", element:%d"), *element));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_RES_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * See description in .h file.
 */
shr_error_e
dnx_algo_res_free_several(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    uint32 nof_elements,
    int element)
{
    int rv = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_RES_MNGR_START(unit, DNX_INIT_TIME_ANALYZER_RES_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META
                     ("\n dnx_algo_res_free_several(unit:%d, name:%s, nof_elements:%d, element:%d)\n"),
                     unit, algo_res->name, nof_elements, element));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_get_delete_verify(unit, algo_res, nof_elements, element));

    if (Resource_callbacks_map[algo_res->advanced_algorithm].free_cb != NULL && nof_elements == 1)
    {
        rv = Resource_callbacks_map[algo_res->advanced_algorithm].free_cb(unit, module_id, algo_res->res_tag_bitmap,
                                                                          element);
    }
    else
    {
        rv = Resource_callbacks_map[algo_res->advanced_algorithm].free_several_cb(unit, module_id,
                                                                                  algo_res->res_tag_bitmap,
                                                                                  nof_elements, element);
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    DNX_INIT_TIME_ANALYZER_RES_MNGR_STOP(unit, DNX_INIT_TIME_ANALYZER_RES_MNGR);
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_free_several(name:%s) Result:%d\n"), algo_res->name,
                     SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_dump_info_get(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    dnx_algo_res_dump_data_t * info)
{
    int nof_free_elements = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_dump(unit:%d, name:%s)\n"), unit, algo_res->name));
    }

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

    info->create_data.flags = algo_res->flags;
    info->create_data.first_element = algo_res->first_element;
    info->create_data.nof_elements = algo_res->nof_elements;
    info->create_data.advanced_algorithm = algo_res->advanced_algorithm;

    SHR_IF_ERR_EXIT(dnx_algo_res_nof_free_elements_get(unit, module_id, algo_res, &nof_free_elements));

    info->nof_used_elements = info->create_data.nof_elements - nof_free_elements;

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_dump(name:%s) Result:%d\n"), algo_res->name, SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

int
dnx_algo_res_print(
    int unit,
    uint32 module_id,
    dnx_algo_res_t * algo_res)
{
    dnx_algo_res_dump_data_t data;
    int end, current_used_entry_count, entry_id;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /*
     * Verify that the resource exist.
     */
    if (*algo_res == NULL)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_algo_res_dump_info_get(unit, module_id, (*algo_res), &data));

    DNX_SW_STATE_PRINT(unit, "\nResource :%s", (*algo_res)->name);
    DNX_SW_STATE_PRINT(unit, " (Advanced Algorithm - %s)",
                       (data.create_data.advanced_algorithm ==
                        DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC ? "No" : "Yes"));

    DNX_SW_STATE_PRINT(unit, "\n|");

    DNX_SW_STATE_PRINT(unit, " Number of entries : %d |", data.create_data.nof_elements);
    DNX_SW_STATE_PRINT(unit, " Number of used entries : %d |", data.nof_used_elements);
    DNX_SW_STATE_PRINT(unit, " First entry ID : %d |\n", data.create_data.first_element);

    if (data.nof_used_elements)
    {
        DNX_SW_STATE_PRINT(unit, "\tUsed Entries:");

        entry_id = data.create_data.first_element;
        end = data.create_data.nof_elements + data.create_data.first_element;
        current_used_entry_count = 0;
        while (1)
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_get_next(unit, module_id, (*algo_res), &entry_id));

            if ((entry_id == DNX_ALGO_RES_ILLEGAL_ELEMENT) || (entry_id > end))
            {
                break;
            }
            DNX_SW_STATE_PRINT(unit, " %d |", entry_id);
            current_used_entry_count++;

            if (current_used_entry_count % 25 == 0)
            {
                DNX_SW_STATE_PRINT(unit, "\n\t\t     ");
            }

            entry_id++;
        }
        DNX_SW_STATE_PRINT(unit, "\n");
    }

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * Verification function for dnx_algo_res_nof_elements_in_range.
 * Check that pointer is not NULL and that resource is valid.
 */
static shr_error_e
dnx_algo_res_nof_allocated_elements_in_range_get_verify(
    int unit,
    dnx_algo_res_t algo_res,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

    SHR_NULL_CHECK(nof_allocated_elements, _SHR_E_PARAM, "nof_allocated_elements");

    if (range_start < algo_res->first_element)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range start(%d) is lower than resource first element(%d).\n", range_start,
                     algo_res->first_element);
    }

    if (range_start > (algo_res->first_element + algo_res->nof_elements))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range start(%d) bigger than resource last element(%d).\n", range_start,
                     (algo_res->first_element + algo_res->nof_elements));
    }

    if ((range_start + nof_elements_in_range) > (algo_res->first_element + algo_res->nof_elements))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Last checked element(%d) is bigger than resource's last element(%d).\n",
                     (range_start + nof_elements_in_range), (algo_res->first_element + algo_res->nof_elements));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_nof_allocated_elements_in_range_get(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_nof_elements_in_range(unit:%d, name:%s)\n"), unit, algo_res->name));
    }

    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_nof_allocated_elements_in_range_get_verify
                          (unit, algo_res, range_start, nof_elements_in_range, nof_allocated_elements));

    if (Resource_callbacks_map[algo_res->advanced_algorithm].nof_allocated_elements_in_range_get_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Resource_callbacks_map[algo_res->advanced_algorithm].nof_allocated_elements_in_range_get_cb
                        (unit, module_id, algo_res->res_tag_bitmap, range_start, nof_elements_in_range,
                         nof_allocated_elements));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "nof_allocated_elements_in_range_get_cb is NULL for resource: \"%s.\"",
                     algo_res->name);
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META("\n dnx_algo_res_nof_elements_in_range(name:%s"), algo_res->name));
        if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", nof_elements:%d"), *nof_allocated_elements));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_advanced_algorithm_info_set(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    void *algorithm_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META
                     ("\n dnx_algo_res_advanced_algorithm_info_set(unit:%d, name:%s)\n"), unit, algo_res->name));
    }

    if (Resource_callbacks_map[algo_res->advanced_algorithm].algorithm_info_set != NULL)
    {
        SHR_IF_ERR_EXIT(Resource_callbacks_map[algo_res->advanced_algorithm].algorithm_info_set(unit,
                                                                                                module_id,
                                                                                                algo_res->res_tag_bitmap,
                                                                                                algorithm_info));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "advanced_algorithm_info_set callback is NULL.");
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_advanced_algorithm_info_set(name:%s"), algo_res->name));
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_advanced_algorithm_info_get(
    int unit,
    uint32 module_id,
    dnx_algo_res_t algo_res,
    void *algorithm_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_res_verify(unit, algo_res));

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META
                     ("\n dnx_algo_res_advanced_algorithm_info_get(unit:%d, name:%s)\n"), unit, algo_res->name));
    }

    if (Resource_callbacks_map[algo_res->advanced_algorithm].algorithm_info_get != NULL)
    {
        SHR_IF_ERR_EXIT(Resource_callbacks_map[algo_res->advanced_algorithm].algorithm_info_get(unit,
                                                                                                module_id,
                                                                                                algo_res->res_tag_bitmap,
                                                                                                algorithm_info));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "advanced_algorithm_info_get callback is NULL.");
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_advanced_algorithm_info_get(name:%s"), algo_res->name));
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

#else
/*
 * {
 */

/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e
dnx_algo_res_dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/*
 * }
 */
#endif /* BCM_DNX_SUPPORT */
