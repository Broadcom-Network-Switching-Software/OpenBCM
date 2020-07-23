
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/access/kbp_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/kbp_diagnostic.h>



kbp_sw_state_t* kbp_sw_state_dummy = NULL;



int
kbp_sw_state_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID]),
        "kbp_sw_state[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        KBP_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        kbp_sw_state_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "kbp_sw_state_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID]),
        "kbp_sw_state[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(kbp_sw_state_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_is_device_locked_set(int unit, int is_device_locked)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->is_device_locked,
        is_device_locked,
        int,
        0,
        "kbp_sw_state_is_device_locked_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        &is_device_locked,
        "kbp_sw_state[%d]->is_device_locked",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_IS_DEVICE_LOCKED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_is_device_locked_get(int unit, int *is_device_locked)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        is_device_locked);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *is_device_locked = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->is_device_locked;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->is_device_locked,
        "kbp_sw_state[%d]->is_device_locked",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_IS_DEVICE_LOCKED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_set(int unit, uint32 db_handles_info_idx_0, CONST kbp_db_handles_t *db_handles_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0],
        db_handles_info,
        kbp_db_handles_t,
        0,
        "kbp_sw_state_db_handles_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        db_handles_info,
        "kbp_sw_state[%d]->db_handles_info[%d]",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_get(int unit, uint32 db_handles_info_idx_0, kbp_db_handles_t *db_handles_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        db_handles_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *db_handles_info = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0],
        "kbp_sw_state[%d]->db_handles_info[%d]",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_caching_bmp_set(int unit, uint32 db_handles_info_idx_0, int caching_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].caching_bmp,
        caching_bmp,
        int,
        0,
        "kbp_sw_state_db_handles_info_caching_bmp_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        &caching_bmp,
        "kbp_sw_state[%d]->db_handles_info[%d].caching_bmp",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CACHING_BMP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_caching_bmp_get(int unit, uint32 db_handles_info_idx_0, int *caching_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        caching_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *caching_bmp = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].caching_bmp;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].caching_bmp,
        "kbp_sw_state[%d]->db_handles_info[%d].caching_bmp",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CACHING_BMP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_db_p_set(int unit, uint32 db_handles_info_idx_0, kbp_db_t_p db_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].db_p,
        db_p,
        kbp_db_t_p,
        0,
        "kbp_sw_state_db_handles_info_db_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        &db_p,
        "kbp_sw_state[%d]->db_handles_info[%d].db_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_DB_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_db_p_get(int unit, uint32 db_handles_info_idx_0, kbp_db_t_p *db_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        db_p);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *db_p = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].db_p;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].db_p,
        "kbp_sw_state[%d]->db_handles_info[%d].db_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_DB_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_ad_db_zero_size_p_set(int unit, uint32 db_handles_info_idx_0, kbp_ad_db_t_p ad_db_zero_size_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_zero_size_p,
        ad_db_zero_size_p,
        kbp_ad_db_t_p,
        0,
        "kbp_sw_state_db_handles_info_ad_db_zero_size_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        &ad_db_zero_size_p,
        "kbp_sw_state[%d]->db_handles_info[%d].ad_db_zero_size_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_DB_ZERO_SIZE_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_ad_db_zero_size_p_get(int unit, uint32 db_handles_info_idx_0, kbp_ad_db_t_p *ad_db_zero_size_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        ad_db_zero_size_p);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *ad_db_zero_size_p = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_zero_size_p;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_zero_size_p,
        "kbp_sw_state[%d]->db_handles_info[%d].ad_db_zero_size_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_DB_ZERO_SIZE_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_ad_entry_zero_size_p_set(int unit, uint32 db_handles_info_idx_0, kbp_ad_entry_t_p ad_entry_zero_size_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_entry_zero_size_p,
        ad_entry_zero_size_p,
        kbp_ad_entry_t_p,
        0,
        "kbp_sw_state_db_handles_info_ad_entry_zero_size_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        &ad_entry_zero_size_p,
        "kbp_sw_state[%d]->db_handles_info[%d].ad_entry_zero_size_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_ENTRY_ZERO_SIZE_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_ad_entry_zero_size_p_get(int unit, uint32 db_handles_info_idx_0, kbp_ad_entry_t_p *ad_entry_zero_size_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        ad_entry_zero_size_p);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *ad_entry_zero_size_p = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_entry_zero_size_p;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_entry_zero_size_p,
        "kbp_sw_state[%d]->db_handles_info[%d].ad_entry_zero_size_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_ENTRY_ZERO_SIZE_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_ad_db_p_set(int unit, uint32 db_handles_info_idx_0, uint32 ad_db_p_idx_0, kbp_ad_db_t_p ad_db_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ad_db_p_idx_0,
        DNX_KBP_NOF_AD_DB_INDEX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_p[ad_db_p_idx_0],
        ad_db_p,
        kbp_ad_db_t_p,
        0,
        "kbp_sw_state_db_handles_info_ad_db_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        &ad_db_p,
        "kbp_sw_state[%d]->db_handles_info[%d].ad_db_p[%d]",
        unit, db_handles_info_idx_0, ad_db_p_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_DB_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_ad_db_p_get(int unit, uint32 db_handles_info_idx_0, uint32 ad_db_p_idx_0, kbp_ad_db_t_p *ad_db_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        ad_db_p);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ad_db_p_idx_0,
        DNX_KBP_NOF_AD_DB_INDEX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *ad_db_p = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_p[ad_db_p_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_p[ad_db_p_idx_0],
        "kbp_sw_state[%d]->db_handles_info[%d].ad_db_p[%d]",
        unit, db_handles_info_idx_0, ad_db_p_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_DB_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_opt_result_size_set(int unit, uint32 db_handles_info_idx_0, int opt_result_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].opt_result_size,
        opt_result_size,
        int,
        0,
        "kbp_sw_state_db_handles_info_opt_result_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        &opt_result_size,
        "kbp_sw_state[%d]->db_handles_info[%d].opt_result_size",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_OPT_RESULT_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_opt_result_size_get(int unit, uint32 db_handles_info_idx_0, int *opt_result_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        opt_result_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *opt_result_size = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].opt_result_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].opt_result_size,
        "kbp_sw_state[%d]->db_handles_info[%d].opt_result_size",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_OPT_RESULT_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_cloned_db_id_set(int unit, uint32 db_handles_info_idx_0, int cloned_db_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].cloned_db_id,
        cloned_db_id,
        int,
        0,
        "kbp_sw_state_db_handles_info_cloned_db_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        &cloned_db_id,
        "kbp_sw_state[%d]->db_handles_info[%d].cloned_db_id",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CLONED_DB_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_cloned_db_id_get(int unit, uint32 db_handles_info_idx_0, int *cloned_db_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        cloned_db_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *cloned_db_id = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].cloned_db_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].cloned_db_id,
        "kbp_sw_state[%d]->db_handles_info[%d].cloned_db_id",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CLONED_DB_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_associated_dbal_table_id_set(int unit, uint32 db_handles_info_idx_0, int associated_dbal_table_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].associated_dbal_table_id,
        associated_dbal_table_id,
        int,
        0,
        "kbp_sw_state_db_handles_info_associated_dbal_table_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        &associated_dbal_table_id,
        "kbp_sw_state[%d]->db_handles_info[%d].associated_dbal_table_id",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_ASSOCIATED_DBAL_TABLE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_associated_dbal_table_id_get(int unit, uint32 db_handles_info_idx_0, int *associated_dbal_table_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        associated_dbal_table_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *associated_dbal_table_id = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].associated_dbal_table_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].associated_dbal_table_id,
        "kbp_sw_state[%d]->db_handles_info[%d].associated_dbal_table_id",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_ASSOCIATED_DBAL_TABLE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_instruction_info_set(int unit, uint32 instruction_info_idx_0, CONST kbp_instruction_handles_t *instruction_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0],
        instruction_info,
        kbp_instruction_handles_t,
        0,
        "kbp_sw_state_instruction_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        instruction_info,
        "kbp_sw_state[%d]->instruction_info[%d]",
        unit, instruction_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_INSTRUCTION_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_instruction_info_get(int unit, uint32 instruction_info_idx_0, kbp_instruction_handles_t *instruction_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        instruction_info_idx_0,
        DNX_KBP_NOF_INSTRUCTIONS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        instruction_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *instruction_info = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0],
        "kbp_sw_state[%d]->instruction_info[%d]",
        unit, instruction_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_INSTRUCTION_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_instruction_info_inst_p_set(int unit, uint32 instruction_info_idx_0, uint32 inst_p_idx_0, kbp_instruction_t_p inst_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        instruction_info_idx_0,
        DNX_KBP_NOF_INSTRUCTIONS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        inst_p_idx_0,
        DNX_KBP_NOF_SMTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        KBP_MODULE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0].inst_p[inst_p_idx_0],
        inst_p,
        kbp_instruction_t_p,
        0,
        "kbp_sw_state_instruction_info_inst_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        KBP_MODULE_ID,
        &inst_p,
        "kbp_sw_state[%d]->instruction_info[%d].inst_p[%d]",
        unit, instruction_info_idx_0, inst_p_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_INSTRUCTION_INFO_INST_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_instruction_info_inst_p_get(int unit, uint32 instruction_info_idx_0, uint32 inst_p_idx_0, kbp_instruction_t_p *inst_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        KBP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        KBP_MODULE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        instruction_info_idx_0,
        DNX_KBP_NOF_INSTRUCTIONS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        KBP_MODULE_ID,
        inst_p);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        inst_p_idx_0,
        DNX_KBP_NOF_SMTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        KBP_MODULE_ID);

    *inst_p = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0].inst_p[inst_p_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        KBP_MODULE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0].inst_p[inst_p_idx_0],
        "kbp_sw_state[%d]->instruction_info[%d].inst_p[%d]",
        unit, instruction_info_idx_0, inst_p_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        KBP_MODULE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_INSTRUCTION_INFO_INST_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




const char *
dnx_kbp_ad_db_index_e_get_name(dnx_kbp_ad_db_index_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_KBP_AD_DB_INDEX_REGULAR", value, DNX_KBP_AD_DB_INDEX_REGULAR);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_KBP_AD_DB_INDEX_OPTIMIZED", value, DNX_KBP_AD_DB_INDEX_OPTIMIZED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_KBP_NOF_AD_DB_INDEX", value, DNX_KBP_NOF_AD_DB_INDEX);

    return NULL;
}





kbp_sw_state_cbs kbp_sw_state = 	{
	
	kbp_sw_state_is_init,
	kbp_sw_state_init,
		{
		
		kbp_sw_state_is_device_locked_set,
		kbp_sw_state_is_device_locked_get}
	,
		{
		
		kbp_sw_state_db_handles_info_set,
		kbp_sw_state_db_handles_info_get,
			{
			
			kbp_sw_state_db_handles_info_caching_bmp_set,
			kbp_sw_state_db_handles_info_caching_bmp_get}
		,
			{
			
			kbp_sw_state_db_handles_info_db_p_set,
			kbp_sw_state_db_handles_info_db_p_get}
		,
			{
			
			kbp_sw_state_db_handles_info_ad_db_zero_size_p_set,
			kbp_sw_state_db_handles_info_ad_db_zero_size_p_get}
		,
			{
			
			kbp_sw_state_db_handles_info_ad_entry_zero_size_p_set,
			kbp_sw_state_db_handles_info_ad_entry_zero_size_p_get}
		,
			{
			
			kbp_sw_state_db_handles_info_ad_db_p_set,
			kbp_sw_state_db_handles_info_ad_db_p_get}
		,
			{
			
			kbp_sw_state_db_handles_info_opt_result_size_set,
			kbp_sw_state_db_handles_info_opt_result_size_get}
		,
			{
			
			kbp_sw_state_db_handles_info_cloned_db_id_set,
			kbp_sw_state_db_handles_info_cloned_db_id_get}
		,
			{
			
			kbp_sw_state_db_handles_info_associated_dbal_table_id_set,
			kbp_sw_state_db_handles_info_associated_dbal_table_id_get}
		}
	,
		{
		
		kbp_sw_state_instruction_info_set,
		kbp_sw_state_instruction_info_get,
			{
			
			kbp_sw_state_instruction_info_inst_p_set,
			kbp_sw_state_instruction_info_inst_p_get}
		}
	}
;
#endif  
#endif  
#undef BSL_LOG_MODULE
