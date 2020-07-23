/** \file algo_field_context.c
 * 
 *
 * Field procedures for DNX.
 *
 * Will hold the needed algorithm functions for Field module
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_access.h>

/*
 * }
 */

/**
* \brief
*  Deallocates the given link profile number.
*
* \param[in] unit        - Device ID
* \param[in] profile_num - The link profile number to allocate
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_link_profile_dealloc(
    int unit,
    int profile_num)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.link_profiles.free_single(unit, profile_num));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Allocates a new link profile number.
*
* \param[in] unit           - Device ID
* \param[out] profile_num_p - The newly allocated profile number
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_link_profile_alloc(
    int unit,
    int *profile_num_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.link_profiles.allocate_single(unit, 0, NULL, profile_num_p));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the given link profile number (context_profile) in HW for the given context_id.
*
* \param[in] unit        - Device ID
* \param[in] context_id  - Context ID to write the link profile number for
* \param[in] profile_num - Link profile number to write
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_link_profile_hw_set(
    int unit,
    dnx_field_context_t context_id,
    int profile_num)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_IPMF1_CONTEXT_GENERAL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_CONTEXT_PROFILE, INST_SINGLE, profile_num);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_context_links_inc(
    int unit,
    dnx_field_context_t context_id)
{
    int nof_linked;
    int profile_num;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.profile_num.get(unit, context_id, &profile_num));
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.nof_linked.get(unit, context_id, &nof_linked));
    if (nof_linked >= dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts - 1)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Links increase error: Can't add more than max number of contexts - 1 (%d) links",
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts - 1);
    }
    if (nof_linked == 0)
    {
        /*
         * Sanity check
         */
        if (profile_num != DNX_ALGO_FIELD_CONTEXT_LINK_PROFILE_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Link profile was not inited or wasn't freed for context %d", context_id);
        }
        /*
         * Allocate new link profile number and set it when first link is created
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_link_profile_alloc(unit, &profile_num));
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.profile_num.set(unit, context_id, profile_num));
        SHR_IF_ERR_EXIT(dnx_algo_field_link_profile_hw_set(unit, context_id, profile_num));
        LOG_DEBUG_EX(BSL_LOG_MODULE, "First link, allocated profile number: %d%s%s%s\r\n", profile_num, EMPTY, EMPTY,
                     EMPTY);
    }
    nof_linked++;
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Links increase for context %d, new nof_linked: %d%s%s\r\n", context_id, nof_linked,
                 EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.nof_linked.set(unit, context_id, nof_linked));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_context_links_dec(
    int unit,
    dnx_field_context_t context_id)
{
    int nof_linked;
    int profile_num;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.profile_num.get(unit, context_id, &profile_num));
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.nof_linked.get(unit, context_id, &nof_linked));
    if (nof_linked <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't decrease links: Context %d has no cascading links", context_id);
    }
    /*
     * Sanity check
     */
    if (profile_num == DNX_ALGO_FIELD_CONTEXT_LINK_PROFILE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Link profile was not allocated for context %d", context_id);
    }
    nof_linked--;
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Links decrease for context %d, new nof_linked: %d%s%s\r\n", context_id, nof_linked,
                 EMPTY, EMPTY);
    if (nof_linked == 0)
    {
        /*
         * Deallocate the link profile number when no more links left
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_link_profile_dealloc(unit, profile_num));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "No more links deallocating profile number %d%s%s%s\r\n", profile_num, EMPTY,
                     EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.
                        profile_num.set(unit, context_id, DNX_ALGO_FIELD_CONTEXT_LINK_PROFILE_INVALID));
        SHR_IF_ERR_EXIT(dnx_algo_field_link_profile_hw_set
                        (unit, context_id, DNX_ALGO_FIELD_CONTEXT_LINK_PROFILE_INVALID));
    }
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.nof_linked.set(unit, context_id, nof_linked));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_link_profile_get(
    int unit,
    dnx_field_context_t context_id,
    int *profile_num_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(profile_num_p, _SHR_E_PARAM, "profile_num_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.profile_num.get(unit, context_id, profile_num_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Inits the link profiles SW State used to allow "cascading" between forgien contexts
*
* \param[in] unit - Device ID
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_link_profiles_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    dnx_field_context_t context_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init link profiles
     */
    sal_memset(&data, 0, sizeof(data));

    /*
     * Our range is 1-31 since 0 is used as the invalid value
     */
    data.first_element = 1;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_link_profiles - 1;
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_FIELD_LINK_PROFILES, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.link_profiles.create(unit, &data, NULL));

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.alloc(unit));

    /*
     * Init context links
     */
    for (context_id = 0; context_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
         context_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.profile_num.set(unit, context_id,
                                                                               DNX_ALGO_FIELD_CONTEXT_LINK_PROFILE_INVALID));
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_contexts_link.nof_linked.set(unit, context_id, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for PMF Context Id for IPMF1 and IPMF2.
*  The context allocation for the two stages in connected.
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ipmf12_context_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    /*
     * Verify that the two stages have the same number of context IDs.
     */
    if (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts !=
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_contexts)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "IPMF1 and IPMF2 must have the same number of contexts. "
                     "IPMF1 has %d, and IPMF2 has %d.\r\n",
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts,
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_contexts);
    }

    /*
     * Create resource for IPMF1.
     */
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF1_CONTEXT_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_context_id.create(unit, &data, NULL));

    /*
     * Create resource for IPMF2.
     */
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_contexts;
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF2_CONTEXT_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf2_context_id.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for PMF Context Id for IPMF3
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ipmf3_context_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF3)->nof_contexts;
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF3_CONTEXT_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf3_context_id.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for PMF Context Id for EPMF
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_epmf_context_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EPMF)->nof_contexts;
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_FIELD_EPMF_CONTEXT_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_context_id.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for user defined Apptypes
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_context_apptype_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = dnx_data_field.kbp.apptype_user_1st_get(unit);
    data.nof_elements = dnx_data_field.kbp.apptype_user_nof_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_FIELD_KBP_USER_APPTYPE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_apptype_id.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  De-Init simple alloc algorithm for link profiles for IPMF1
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_link_profiles_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state resources will be destroyed together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  De-Init simple alloc algorithm for PMF Context Id for IPMF1 and IPMF2
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ipmf12_context_id_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state resources will be destroyed together.
     */

    /*
     * Destroy resource for IPMF1
     */
    /*
     * Destroy resource for IPMF2
     */
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*  De-Init simple alloc algorithm for PMF Context Id for IPMF3
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ipmf3_context_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state resources will be destroyed together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  De-Init simple alloc algorithm for PMF Context Id for EPMF
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_epmf_context_id_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state resources will be destroyed together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  De-Init simple alloc algorithm for user defined apptypes
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_context_apptype_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sw state resources will be destroyed together.
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Allocate field Context for IPMF1 or IPMF2.
 *  These two stages are connected and require a separate logic.
 * \param [in] unit          - Device id
 * \param [in] alloc_flags   - Alloc Flags (WITH_ID)
 * \param [in] stage         - Field Stage to allocate context to
 * \param [in,out] context_alloc_id_p  - pointer to integer representing the context ID.
 *                                       as input -  the context id to allocate when WITH_ID flag is set
 *                                       as output - allocated Context id
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
*/
shr_error_e
dnx_algo_field_context_id_allocate_ipmf12(
    int unit,
    int alloc_flags,
    dnx_field_stage_e stage,
    int *context_alloc_id_p)
{
    uint8 is_allocated_ipmf1;
    uint8 is_allocated_ipmf2;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_alloc_id_p, _SHR_E_PARAM, "context_alloc_id_p");

    if (alloc_flags & DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID)
    {
        /*
         * Allocate a given context ID.
         */
        if (stage == DNX_FIELD_STAGE_IPMF1)
        {
            /*
             * If we want to allocate a context ID for IPMF1, we allocate for both IPMF1 and IPMF2
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated
                            (unit, DNX_FIELD_STAGE_IPMF1, *context_alloc_id_p, &is_allocated_ipmf1));
            if (is_allocated_ipmf1)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Context ID %d is already allocated for iPMF1.\r\n", *context_alloc_id_p);
            }
            SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated
                            (unit, DNX_FIELD_STAGE_IPMF2, *context_alloc_id_p, &is_allocated_ipmf2));
            if (is_allocated_ipmf2)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Context ID %d is already allocated for iPMF2. "
                             "When creating a context ID for IPMF1, "
                             "we also create the same context ID for IPMF2.\r\n", *context_alloc_id_p);
            }
            /*
             * Allocate the context ID for both iPMF1 and iPMF2.
             */
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_context_id.allocate_single
                            (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, context_alloc_id_p));
            SHR_IF_ERR_EXIT(algo_field_info_sw.
                            ipmf2_context_id.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL,
                                                             context_alloc_id_p));
        }
        else if (stage == DNX_FIELD_STAGE_IPMF2)
        {
            /*
             * If we want to allocate a context ID for IPMF2, we allocate for IPMF2 only,
             * but we cannot allocate to iPMF2 if the same context ID exists in IPMF1.
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated
                            (unit, DNX_FIELD_STAGE_IPMF1, *context_alloc_id_p, &is_allocated_ipmf1));
            SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated
                            (unit, DNX_FIELD_STAGE_IPMF2, *context_alloc_id_p, &is_allocated_ipmf2));
            if (is_allocated_ipmf2)
            {
                if (is_allocated_ipmf1)
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Context ID %d is already allocated for iPMF1. "
                                 "When a context ID is created for IPMF1, "
                                 "it is also created for IPMF2.\r\n", *context_alloc_id_p);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Context ID %d is already allocated for iPMF2.\r\n ",
                                 *context_alloc_id_p);
                }
            }
            else
            {
                if (is_allocated_ipmf1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context ID %d is allocated for iPMF1, but not for IPMF2. "
                                 "This scenarion should not happen.\r\n", *context_alloc_id_p);
                }
            }
            /*
             * Allocate the context ID for iPMF2.
             */
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf2_context_id.allocate_single
                            (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, context_alloc_id_p));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached a function for stages iPMF1/2 only with stage %d.\r\n", stage);
        }
    }
    else
    {
        /*
         * Allocating without a WITH_ID flag.
         * Whether we allocate for iPMF1 of iPMF2, the iPMF2 context ID is allocated.
         * So we allocate for iPMF2 first and then the parallel context ID in iPMF1 if we allocate for iPMF1.
         */
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf2_context_id.allocate_single(unit, 0, NULL, context_alloc_id_p));
        /*
         * Sanity check, verify that the parallel iPMF1 context ID isn't allocated.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated
                        (unit, DNX_FIELD_STAGE_IPMF1, *context_alloc_id_p, &is_allocated_ipmf1));
        if (is_allocated_ipmf1)
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf2_context_id.free_single(unit, *context_alloc_id_p));
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Context ID %d was allocated for iPMF1, but not for IPMF2. "
                         "This scenarion should not happen.\r\n", *context_alloc_id_p);
        }
        /*
         * If we allocate for iPMF1, allocate the iPMF1 context too.
         */
        if (stage == DNX_FIELD_STAGE_IPMF1)
        {
            SHR_IF_ERR_EXIT(algo_field_info_sw.
                            ipmf1_context_id.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL,
                                                             context_alloc_id_p));
        }
        else if (stage != DNX_FIELD_STAGE_IPMF2)
        {
            /*
             * Extra sanity check to match the WITH_ID scenario, verify the stage.
             */
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached a function for stages iPMF1/2 only with stage %d.\r\n", stage);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_context_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_stage_e stage,
    dnx_field_context_t * context_id_p)
{
    int alloc_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_id_p, _SHR_E_PARAM, "context_id_p");

    if (alloc_flags & DNX_ALGO_RES_ALLOCATE_WITH_ID)
    {
        alloc_id = *context_id_p;
    }

    switch (stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_context_id_allocate_ipmf12(unit, alloc_flags, stage, &alloc_id));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf3_context_id.allocate_single(unit, alloc_flags, NULL, &alloc_id));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_context_id.allocate_single(unit, alloc_flags, NULL, &alloc_id));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Context_ID allocation for Stage=%d not supported /n", stage);
            break;
    }

    /**In case WITH id flag was not set*/
    *context_id_p = alloc_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Deallocate the Context ID for Field module for either stage IPMF1 or IPMF2. These two stages are connected.
 * \param [in] unit        - Device id
 * \param [in] stage       - Field Stage to deallocate context in
 * \param [in] context_id  - Context id to deallocate
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_algo_field_context_id_deallocate_ipmf12(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id)
{
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    if (stage == DNX_FIELD_STAGE_IPMF1)
    {
        /*
         * For IMPF1, free the context from both IPMF1 and IPMF2.
         */
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_context_id.free_single(unit, context_id));
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf2_context_id.free_single(unit, context_id));
    }
    else if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        /*
         * For IPMF2, free the context only from IPMF2. But before that, verify that it doesn't have
         * a parallel in IPMF1 (it shouldn't have, as it should be its 'cascaded_from"" and context destroy verify
         * should block this scenario from happening).
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated(unit, DNX_FIELD_STAGE_IPMF1, context_id, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot delete context ID %d from IPMF2 "
                         "without first deleting it from IPMF1.\r\n", context_id);
        }
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf2_context_id.free_single(unit, context_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_context_id_deallocate(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_context_id_deallocate_ipmf12(unit, stage, context_id));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf3_context_id.free_single(unit, context_id));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_context_id.free_single(unit, context_id));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context_ID allocation for Stage=%d not supported /n", stage);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_context_id_is_allocated(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");

    switch (stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_context_id.is_allocated(unit, context_id, is_allocated_p));
            break;
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf2_context_id.is_allocated(unit, context_id, is_allocated_p));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf3_context_id.is_allocated(unit, context_id, is_allocated_p));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_context_id.is_allocated(unit, context_id, is_allocated_p));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context_ID allocation for Stage=%d not supported /n", stage);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_context_apptype_allocate(
    int unit,
    int alloc_flags,
    bcm_field_AppType_t * apptype_p)
{
    int alloc_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(apptype_p, _SHR_E_PARAM, "apptype_p");

    if (alloc_flags & DNX_ALGO_RES_ALLOCATE_WITH_ID)
    {
        alloc_id = *apptype_p;
    }

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_apptype_id.allocate_single(unit, alloc_flags, NULL, &alloc_id));

    /**In case WITH id flag was not set*/
    *apptype_p = alloc_id;

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_context_apptype_deallocate(
    int unit,
    bcm_field_AppType_t apptype)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_apptype_id.free_single(unit, apptype));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_context_apptype_is_allocated(
    int unit,
    bcm_field_AppType_t apptype,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_apptype_id.is_allocated(unit, apptype, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_context_res_mngr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Link profiles resource manager init */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_link_profiles_init(unit));
    /**iPMF12 PMF_PGM_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf12_context_id_init(unit));
    /**iPMF3 PMF_PGM_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf3_context_id_init(unit));
    /**EPMF PMF_PGM_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_epmf_context_id_init(unit));
    /**User defined Apptypes*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_context_apptype_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_context_res_mngr_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Link profiles resource manager deinit */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_link_profiles_deinit(unit));
    /**iPMF12 PMF_PGM_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf12_context_id_deinit(unit));
    /**iPMF3 PMF_PGM_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf3_context_id_deinit(unit));
    /**EPMF PMF_PGM_ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_epmf_context_id_deinit(unit));
    /**User defined Apptypes*/
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_context_apptype_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}
