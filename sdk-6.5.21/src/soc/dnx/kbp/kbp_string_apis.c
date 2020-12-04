/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <shared/swstate/sw_state_workarounds.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/swstate/auto_generated/types/kbp_types.h>

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)

shr_error_e
dnx_kbp_db_counter_read_initiate(
    int unit,
    int *db)
{

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_TRY(kbp_db_counter_read_initiate((kbp_db_t *) db));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kbp_entry_get_counter_value(
    int unit,
    int *db,
    int *entry,
    uint64 *value)
{
    kbp_entry_t *kbp_entry;
    SHR_FUNC_INIT_VARS(unit);

    kbp_entry = INT_TO_PTR(*entry);
    DNX_KBP_TRY(kbp_entry_get_counter_value((kbp_db_t *) db, kbp_entry, value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kbp_db_is_counter_read_complete(
    int unit,
    int *db,
    int *is_complete)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_TRY(kbp_db_is_counter_read_complete((kbp_db_t *) db, is_complete));

exit:
    SHR_FUNC_EXIT;
}

#else

shr_error_e
dnx_kbp_db_counter_read_initiate(
    int unit,
    int *db)
{
DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("dnx_kbp_db_counter_read_initiate")}
shr_error_e
dnx_kbp_entry_get_counter_value(
    int unit,
    int *db,
    int *entry,
    uint64 *value)
{
DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("dnx_kbp_entry_get_counter_value")}
shr_error_e
dnx_kbp_db_is_counter_read_complete(
    int unit,
    int *db,
    int *is_complete)
{
DNX_KBP_DUMMY_PROTOTYPE_ERR_MSG("dnx_kbp_db_is_counter_read_complete")}

#endif
