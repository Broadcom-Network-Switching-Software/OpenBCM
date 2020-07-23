/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_l2.c
 *
 * Wrapper functions for utilex_multi_set.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/algo/l2/algo_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l2_access.h>
/**
 * }
 */

void
dnx_l2_vsi_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_vsi_learning_profile_info_t *vsi_profile_info = (dnx_vsi_learning_profile_info_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Aging profile",
                                   vsi_profile_info->aging_profile, "Aging profile", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Forwarding profile",
                                   vsi_profile_info->event_forwarding_profile, "Forwarding profile", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Limit", vsi_profile_info->limit, "Limit", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Message enable",
                                   vsi_profile_info->message_enable, "Message enable", NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}

void
dnx_l2_vsi_aging_print_cb(
    int unit,
    const void *data)
{
    dnx_aging_profile_info_t *vsi_aging_profile_info = (dnx_aging_profile_info_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Ageout", vsi_aging_profile_info->ageout_age,
                                   "Ageout age", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Delete", vsi_aging_profile_info->delete_age,
                                   "Delete age", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Refresh", vsi_aging_profile_info->refresh_age,
                                   "Refresh age", NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}

void
dnx_l2_event_forwarding_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_event_forwarding_profile_info_t *event_forwarding_profile_info = (dnx_event_forwarding_profile_info_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Ack",
                                   event_forwarding_profile_info->ack_distribution, "Ack event distribution", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Delete",
                                   event_forwarding_profile_info->ack_distribution, "Delete event distribution", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Insert",
                                   event_forwarding_profile_info->ack_distribution, "Insert event distribution", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Learn",
                                   event_forwarding_profile_info->ack_distribution, "Learn event distribution", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Refresh",
                                   event_forwarding_profile_info->ack_distribution, "Refresh event distribution", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Transplant",
                                   event_forwarding_profile_info->ack_distribution,
                                   "Transplant event distribution", NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \brief - 
 * Create Template for VSI profiles. Each profile holds the following properties: 
 * 1. limit - limit on the number of entries for the VSI. 
 * 2. Forwrding profile - pointer to the forwarding profile of the VSI. 
 * 3. Age profile - pointer to the age profile of the VSI. 
 * 4. message enable - whether to send a message on limit cross. 
 *  
 * \param [in] unit - unit id
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
dnx_algo_l2_vsi_profile_template_create(
    int unit)
{
    dnx_algo_template_create_data_t data;
    dnx_vsi_learning_profile_info_t vsi_profile_info;
    uint32 predef_max_limit;
    uint32 entry_handle_id;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_LEARNING_PROFILE_INFO, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_LEARN_LIMIT,
                                                    0, DBAL_PREDEF_VAL_MAX_VALUE, &predef_max_limit));

    /*
     * Set a template for vsi profiles
     */
    sal_memset(&data, 0, sizeof(data));
    sal_memset(&vsi_profile_info, 0, sizeof(dnx_vsi_learning_profile_info_t));
    vsi_profile_info.limit = predef_max_limit;

    data.data_size = sizeof(dnx_vsi_learning_profile_info_t);
    data.first_profile = 0;
    data.nof_profiles = dnx_data_l2.vsi.nof_vsi_learning_profiles_get(unit);
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.default_data = &vsi_profile_info;
    data.default_profile = DNX_VSI_LEARNING_PROFILE_DEFAULT_PROFILE_ID;
    /*
     * Each VSI can point to one VSI profile.
     */
    data.max_references = nof_vsis;
    sal_strncpy(data.name, DNX_ALGO_L2_VSI_LEARNING_PROFILE_TEMPLATE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.create(unit, &data, NULL));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Increase the reference of a forwarding profile using get and allocate on this profile.
 *
 * \param [in] unit - unit id
 * \param [in] forwarding_profile - profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_algo_l2_event_forwarding_profile_increase_ref(
    int unit,
    int forwarding_profile)
{
    int alloc_flags;
    dnx_event_forwarding_profile_info_t forwarding_profile_info;
    uint8 first_reference;
    int ref_count;

    SHR_FUNC_INIT_VARS(unit);

    alloc_flags = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;

    /** Get the forwarding profile's data */
    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.profile_data_get
                    (unit, forwarding_profile, &ref_count, (void *) &forwarding_profile_info));

    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.allocate_single(unit, alloc_flags,
                                                                        &forwarding_profile_info, NULL,
                                                                        &forwarding_profile, &first_reference));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Decrease the reference of a forwarding profile.
 *
 * \param [in] unit - unit id
 * \param [in] forwarding_profile - profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_algo_l2_event_forwarding_profile_decrease_ref(
    int unit,
    int forwarding_profile)
{
    uint8 last_reference;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.free_single(unit, forwarding_profile, &last_reference));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Increase the reference of a aging profile using get and allocate on this profile.
 *
 * \param [in] unit - unit id
 * \param [in] aging_profile - profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_algo_l2_aging_profile_increase_ref(
    int unit,
    int aging_profile)
{
    int alloc_flags;
    dnx_aging_profile_info_t aging_profile_info;
    uint8 first_reference;
    int ref_count;

    SHR_FUNC_INIT_VARS(unit);

    alloc_flags = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;

    /** Get the aging profile's data */
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.profile_data_get
                    (unit, aging_profile, &ref_count, (void *) &aging_profile_info));

    SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.allocate_single(unit, alloc_flags,
                                                                 &aging_profile_info, NULL,
                                                                 &aging_profile, &first_reference));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Decrease the reference of a aging profile.
 *
 * \param [in] unit - unit id
 * \param [in] aging_profile - profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_algo_l2_aging_profile_decrease_ref(
    int unit,
    int aging_profile)
{
    uint8 last_reference;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.free_single(unit, aging_profile, &last_reference));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Create Template for VSI forwarding profiles. Each forwarding profile holds the distribution data of all the OLP events: 
 * insert, transplant, delete, refresh, learn and ACK. 
 * 
 * \param [in] unit - unit id
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
dnx_algo_l2_event_forwarding_profile_template_create(
    int unit)
{
    dnx_algo_template_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set a template for vsi profiles
     */
    sal_memset(&data, 0, sizeof(data));

    data.data_size = sizeof(dnx_event_forwarding_profile_info_t);
    data.nof_profiles = dnx_data_l2.vsi.nof_event_forwarding_profiles_get(unit);

    /*
     * Each VSI profile can point to one VSI forwarding profile.
     */
    /*
     * Allow one reference more than needed since it solves a temporary glitch above the maximum. Anyways it is impossible to cross the maximum since each reference is attached to a VSI learn profile.
     */
    data.max_references = dnx_data_l2.vsi.nof_vsi_learning_profiles_get(unit) + 1;
    sal_strncpy(data.name, DNX_ALGO_L2_EVENT_FORWARDING_PROFILE_TEMPLATE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Create Template for VSI aging profiles. Each profile holds the following properties: 
 * 1. ageout age - age to raise an age out event. 
 * 2. delete age - age to raise a delete event. 
 * 3. refresh age - age to raise a refresh event.
 *  
 * \param [in] unit - unit id
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
dnx_algo_l2_vsi_aging_profile_template_create(
    int unit)
{
    dnx_algo_template_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set a template for vsi profiles
     */
    sal_memset(&data, 0, sizeof(data));

    data.data_size = sizeof(dnx_aging_profile_info_t);
    data.nof_profiles = dnx_data_l2.vsi.nof_vsi_aging_profiles_get(unit);

    /*
     * Each VSI learning profile can point to one aging profile.
     */
    /*
     * Allow one reference more than needed since it solves a temporary glitch above the maximum. Anyways it is impossible to cross the maximum since each reference is attached to a VSI learn profile.
     */
    data.max_references = dnx_data_l2.vsi.nof_vsi_learning_profiles_get(unit) + 1;
    sal_strncpy(data.name, DNX_ALGO_L2_VSI_AGING_PROFILE_TEMPLATE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_l2_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l2_db.init(unit));
    /*
     * Initialize the VSI forwarding profile template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l2_event_forwarding_profile_template_create(unit));

    /*
     * Initialize the VSI aging profile template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l2_vsi_aging_profile_template_create(unit));

    /*
     * Initialize the VSI profile template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l2_vsi_profile_template_create(unit));

exit:
    SHR_FUNC_EXIT;
}
