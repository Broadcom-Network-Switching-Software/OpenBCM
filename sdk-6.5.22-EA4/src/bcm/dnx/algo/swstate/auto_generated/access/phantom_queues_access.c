/** \file algo/swstate/auto_generated/access/phantom_queues_access.c
 *
 * sw state functions definitions
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/swstate/auto_generated/access/phantom_queues_access.h>
#include <bcm_int/dnx/cosq/egress/phantom_queues.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/diagnostic/phantom_queues_diagnostic.h>
/*
 * Global Variables
 */

/* dummy variable for gdb to know this type */
dnx_phantom_queues_db_t* dnx_phantom_queues_db_dummy = NULL;
/*
 * FUNCTIONs
 */

/*
 *
 * is_init function for the variable dnx_phantom_queues_db
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]),
        "dnx_phantom_queues_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * init function for the variable dnx_phantom_queues_db
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_phantom_queues_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_phantom_queues_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]),
        "dnx_phantom_queues_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_phantom_queues_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * set function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_set(int unit, uint32 phantom_queues_idx_0, dnx_algo_template_t phantom_queues)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        phantom_queues,
        dnx_algo_template_t,
        0,
        "dnx_phantom_queues_db_phantom_queues_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        PHANTOM_QUEUES_MODULE_ID,
        &phantom_queues,
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * get function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_get(int unit, uint32 phantom_queues_idx_0, dnx_algo_template_t *phantom_queues)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        phantom_queues);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    *phantom_queues = ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * create function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_create(int unit, uint32 phantom_queues_idx_0, dnx_algo_template_create_data_t * data, void *extra_arguments)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_CREATE(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        data,
        extra_arguments,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGRCREATE,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * allocate_single function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_allocate_single(int unit, uint32 phantom_queues_idx_0, uint32 flags, uint32 *profile_data, void *extra_arguments, int *profile, uint8 *first_reference)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_ALLOCATE_SINGLE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_ALLOCATE_SINGLE(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        flags,
        profile_data,
        extra_arguments,
        profile,
        first_reference);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_ALLOCATE_SINGLE_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGRALLOCATE_SINGLE,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * free_single function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_free_single(int unit, uint32 phantom_queues_idx_0, int profile, uint8 *last_reference)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_FREE_SINGLE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_FREE_SINGLE(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        profile,
        last_reference);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_FREE_SINGLE_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGRFREE_SINGLE,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * exchange function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_exchange(int unit, uint32 phantom_queues_idx_0, uint32 flags, const uint32 *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_EXCHANGE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_EXCHANGE(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        flags,
        profile_data,
        old_profile,
        extra_arguments,
        new_profile,
        first_reference,
        last_reference);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_EXCHANGE_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGREXCHANGE,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * replace_data function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_replace_data(int unit, uint32 phantom_queues_idx_0, int profile, const uint32 *new_profile_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_REPLACE_DATA,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_REPLACE_DATA(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        profile,
        new_profile_data);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_REPLACE_DATA_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGRREPLACE_DATA,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * profile_data_get function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_profile_data_get(int unit, uint32 phantom_queues_idx_0, int profile, int *ref_count, uint32 *profile_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_PROFILE_DATA_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_PROFILE_DATA_GET(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        profile,
        ref_count,
        profile_data);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_PROFILE_DATA_GET_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGRPROFILE_DATA_GET,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * profile_get function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_profile_get(int unit, uint32 phantom_queues_idx_0, const uint32 *profile_data, int *profile)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_PROFILE_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_PROFILE_GET(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        profile_data,
        profile);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_PROFILE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGRPROFILE_GET,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * get_next function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_get_next(int unit, uint32 phantom_queues_idx_0, int *current_profile)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_GET_NEXT(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        current_profile);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGRGET_NEXT,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * dump_info_get function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_dump_info_get(int unit, uint32 phantom_queues_idx_0, dnx_algo_template_dump_data_t * info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_DUMP_INFO_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_DUMP_INFO_GET(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        info);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_DUMP_INFO_GET_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGRDUMP_INFO_GET,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * advanced_algorithm_info_set function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_advanced_algorithm_info_set(int unit, uint32 phantom_queues_idx_0, void *algorithm_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_ADVANCED_ALGORITHM_INFO_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_ADVANCED_ALGORITHM_INFO_SET(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        algorithm_info);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_ADVANCED_ALGORITHM_INFO_SET_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGRADVANCED_ALGORITHM_INFO_SET,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * advanced_algorithm_info_get function for the variable phantom_queues
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_phantom_queues_db_phantom_queues_advanced_algorithm_info_get(int unit, uint32 phantom_queues_idx_0, void *algorithm_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        SW_STATE_FUNC_ADVANCED_ALGORITHM_INFO_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        phantom_queues_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        PHANTOM_QUEUES_MODULE_ID);

    DNX_ALGO_TEMP_MNGR_ADVANCED_ALGORITHM_INFO_GET(
        PHANTOM_QUEUES_MODULE_ID,
        ((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        algorithm_info);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_TEMP_MNGR_ADVANCED_ALGORITHM_INFO_GET_LOGGING,
        BSL_LS_SWSTATEDNX_TEMP_MNGRADVANCED_ALGORITHM_INFO_GET,
        PHANTOM_QUEUES_MODULE_ID,
        &((dnx_phantom_queues_db_t*)sw_state_roots_array[unit][PHANTOM_QUEUES_MODULE_ID])->phantom_queues[phantom_queues_idx_0],
        "dnx_phantom_queues_db[%d]->phantom_queues[%d]",
        unit, phantom_queues_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        PHANTOM_QUEUES_MODULE_ID,
        dnx_phantom_queues_db_info,
        DNX_PHANTOM_QUEUES_DB_PHANTOM_QUEUES_INFO,
        DNX_SW_STATE_DIAG_TEMP_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 * Global Variables
 */

dnx_phantom_queues_db_cbs dnx_phantom_queues_db = 	{
	
	dnx_phantom_queues_db_is_init,
	dnx_phantom_queues_db_init,
		{
		
		dnx_phantom_queues_db_phantom_queues_set,
		dnx_phantom_queues_db_phantom_queues_get,
		dnx_phantom_queues_db_phantom_queues_create,
		dnx_phantom_queues_db_phantom_queues_allocate_single,
		dnx_phantom_queues_db_phantom_queues_free_single,
		dnx_phantom_queues_db_phantom_queues_exchange,
		dnx_phantom_queues_db_phantom_queues_replace_data,
		dnx_phantom_queues_db_phantom_queues_profile_data_get,
		dnx_phantom_queues_db_phantom_queues_profile_get,
		dnx_phantom_queues_db_phantom_queues_get_next,
		dnx_phantom_queues_db_phantom_queues_dump_info_get,
		dnx_phantom_queues_db_phantom_queues_advanced_algorithm_info_set,
		dnx_phantom_queues_db_phantom_queues_advanced_algorithm_info_get}
	}
;
#endif /* BCM_DNX_SUPPORT*/ 
#undef BSL_LOG_MODULE
