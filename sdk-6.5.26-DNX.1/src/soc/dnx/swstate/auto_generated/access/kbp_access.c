
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef INCLUDE_KBP
#include <soc/dnx/swstate/auto_generated/access/kbp_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/kbp_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/kbp_layout.h>





int
kbp_sw_state_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_KBP_SW_STATE,
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
        DNX_SW_STATE_KBP_SW_STATE,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_KBP_SW_STATE,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        kbp_sw_state_t,
        DNX_SW_STATE_KBP_SW_STATE_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "kbp_sw_state_init",
        DNX_SW_STATE_DIAG_ALLOC,
        kbp_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_KBP_SW_STATE,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID]),
        "kbp_sw_state[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE,
        kbp_sw_state_info,
        KBP_SW_STATE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID]),
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_fwd_caching_enabled_set(int unit, int fwd_caching_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->fwd_caching_enabled,
        fwd_caching_enabled,
        int,
        0,
        "kbp_sw_state_fwd_caching_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        &fwd_caching_enabled,
        "kbp_sw_state[%d]->fwd_caching_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        kbp_sw_state_info,
        KBP_SW_STATE_FWD_CACHING_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_fwd_caching_enabled_get(int unit, int *fwd_caching_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        fwd_caching_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        DNX_SW_STATE_DIAG_READ);

    *fwd_caching_enabled = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->fwd_caching_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->fwd_caching_enabled,
        "kbp_sw_state[%d]->fwd_caching_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED,
        kbp_sw_state_info,
        KBP_SW_STATE_FWD_CACHING_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_is_device_locked_set(int unit, int is_device_locked)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->is_device_locked,
        is_device_locked,
        int,
        0,
        "kbp_sw_state_is_device_locked_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        &is_device_locked,
        "kbp_sw_state[%d]->is_device_locked",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        kbp_sw_state_info,
        KBP_SW_STATE_IS_DEVICE_LOCKED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_is_device_locked_get(int unit, int *is_device_locked)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        is_device_locked);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        DNX_SW_STATE_DIAG_READ);

    *is_device_locked = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->is_device_locked;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->is_device_locked,
        "kbp_sw_state[%d]->is_device_locked",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED,
        kbp_sw_state_info,
        KBP_SW_STATE_IS_DEVICE_LOCKED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_set(int unit, uint32 db_handles_info_idx_0, CONST kbp_db_handles_t *db_handles_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0],
        db_handles_info,
        kbp_db_handles_t,
        0,
        "kbp_sw_state_db_handles_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        db_handles_info,
        "kbp_sw_state[%d]->db_handles_info[%d]",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_get(int unit, uint32 db_handles_info_idx_0, kbp_db_handles_t *db_handles_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        db_handles_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        DNX_SW_STATE_DIAG_READ);

    *db_handles_info = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0],
        "kbp_sw_state[%d]->db_handles_info[%d]",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_caching_bmp_set(int unit, uint32 db_handles_info_idx_0, int caching_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].caching_bmp,
        caching_bmp,
        int,
        0,
        "kbp_sw_state_db_handles_info_caching_bmp_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        &caching_bmp,
        "kbp_sw_state[%d]->db_handles_info[%d].caching_bmp",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CACHING_BMP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_caching_bmp_get(int unit, uint32 db_handles_info_idx_0, int *caching_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        caching_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        DNX_SW_STATE_DIAG_READ);

    *caching_bmp = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].caching_bmp;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].caching_bmp,
        "kbp_sw_state[%d]->db_handles_info[%d].caching_bmp",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CACHING_BMP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_core_id_set(int unit, uint32 db_handles_info_idx_0, uint8 core_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].core_id,
        core_id,
        uint8,
        0,
        "kbp_sw_state_db_handles_info_core_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        &core_id,
        "kbp_sw_state[%d]->db_handles_info[%d].core_id",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CORE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_core_id_get(int unit, uint32 db_handles_info_idx_0, uint8 *core_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        core_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        DNX_SW_STATE_DIAG_READ);

    *core_id = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].core_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].core_id,
        "kbp_sw_state[%d]->db_handles_info[%d].core_id",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CORE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_caching_enabled_set(int unit, uint32 db_handles_info_idx_0, int caching_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].caching_enabled,
        caching_enabled,
        int,
        0,
        "kbp_sw_state_db_handles_info_caching_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        &caching_enabled,
        "kbp_sw_state[%d]->db_handles_info[%d].caching_enabled",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CACHING_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_caching_enabled_get(int unit, uint32 db_handles_info_idx_0, int *caching_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        caching_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        DNX_SW_STATE_DIAG_READ);

    *caching_enabled = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].caching_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].caching_enabled,
        "kbp_sw_state[%d]->db_handles_info[%d].caching_enabled",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CACHING_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_db_p_set(int unit, uint32 db_handles_info_idx_0, kbp_db_t_p db_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].db_p,
        db_p,
        kbp_db_t_p,
        0,
        "kbp_sw_state_db_handles_info_db_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        &db_p,
        "kbp_sw_state[%d]->db_handles_info[%d].db_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_DB_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_db_p_get(int unit, uint32 db_handles_info_idx_0, kbp_db_t_p *db_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        db_p);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        DNX_SW_STATE_DIAG_READ);

    *db_p = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].db_p;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].db_p,
        "kbp_sw_state[%d]->db_handles_info[%d].db_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_DB_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_ad_db_zero_size_p_set(int unit, uint32 db_handles_info_idx_0, kbp_ad_db_t_p ad_db_zero_size_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_zero_size_p,
        ad_db_zero_size_p,
        kbp_ad_db_t_p,
        0,
        "kbp_sw_state_db_handles_info_ad_db_zero_size_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        &ad_db_zero_size_p,
        "kbp_sw_state[%d]->db_handles_info[%d].ad_db_zero_size_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_DB_ZERO_SIZE_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_ad_db_zero_size_p_get(int unit, uint32 db_handles_info_idx_0, kbp_ad_db_t_p *ad_db_zero_size_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        ad_db_zero_size_p);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        DNX_SW_STATE_DIAG_READ);

    *ad_db_zero_size_p = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_zero_size_p;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_zero_size_p,
        "kbp_sw_state[%d]->db_handles_info[%d].ad_db_zero_size_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_DB_ZERO_SIZE_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_ad_entry_zero_size_p_set(int unit, uint32 db_handles_info_idx_0, kbp_ad_entry_t_p ad_entry_zero_size_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_entry_zero_size_p,
        ad_entry_zero_size_p,
        kbp_ad_entry_t_p,
        0,
        "kbp_sw_state_db_handles_info_ad_entry_zero_size_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        &ad_entry_zero_size_p,
        "kbp_sw_state[%d]->db_handles_info[%d].ad_entry_zero_size_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_ENTRY_ZERO_SIZE_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_ad_entry_zero_size_p_get(int unit, uint32 db_handles_info_idx_0, kbp_ad_entry_t_p *ad_entry_zero_size_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        ad_entry_zero_size_p);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        DNX_SW_STATE_DIAG_READ);

    *ad_entry_zero_size_p = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_entry_zero_size_p;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_entry_zero_size_p,
        "kbp_sw_state[%d]->db_handles_info[%d].ad_entry_zero_size_p",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_ENTRY_ZERO_SIZE_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_ad_db_p_set(int unit, uint32 db_handles_info_idx_0, uint32 ad_db_p_idx_0, kbp_ad_db_t_p ad_db_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ad_db_p_idx_0,
        DNX_KBP_NOF_AD_DB_INDEX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_p[ad_db_p_idx_0],
        ad_db_p,
        kbp_ad_db_t_p,
        0,
        "kbp_sw_state_db_handles_info_ad_db_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        &ad_db_p,
        "kbp_sw_state[%d]->db_handles_info[%d].ad_db_p[%d]",
        unit, db_handles_info_idx_0, ad_db_p_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_DB_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_ad_db_p_get(int unit, uint32 db_handles_info_idx_0, uint32 ad_db_p_idx_0, kbp_ad_db_t_p *ad_db_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        ad_db_p);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        ad_db_p_idx_0,
        DNX_KBP_NOF_AD_DB_INDEX);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        DNX_SW_STATE_DIAG_READ);

    *ad_db_p = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_p[ad_db_p_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].ad_db_p[ad_db_p_idx_0],
        "kbp_sw_state[%d]->db_handles_info[%d].ad_db_p[%d]",
        unit, db_handles_info_idx_0, ad_db_p_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_AD_DB_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_opt_result_size_set(int unit, uint32 db_handles_info_idx_0, int opt_result_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].opt_result_size,
        opt_result_size,
        int,
        0,
        "kbp_sw_state_db_handles_info_opt_result_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        &opt_result_size,
        "kbp_sw_state[%d]->db_handles_info[%d].opt_result_size",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_OPT_RESULT_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_opt_result_size_get(int unit, uint32 db_handles_info_idx_0, int *opt_result_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        opt_result_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *opt_result_size = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].opt_result_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].opt_result_size,
        "kbp_sw_state[%d]->db_handles_info[%d].opt_result_size",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_OPT_RESULT_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_large_opt_result_size_set(int unit, uint32 db_handles_info_idx_0, int large_opt_result_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].large_opt_result_size,
        large_opt_result_size,
        int,
        0,
        "kbp_sw_state_db_handles_info_large_opt_result_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        &large_opt_result_size,
        "kbp_sw_state[%d]->db_handles_info[%d].large_opt_result_size",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_LARGE_OPT_RESULT_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_large_opt_result_size_get(int unit, uint32 db_handles_info_idx_0, int *large_opt_result_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        large_opt_result_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *large_opt_result_size = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].large_opt_result_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].large_opt_result_size,
        "kbp_sw_state[%d]->db_handles_info[%d].large_opt_result_size",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_LARGE_OPT_RESULT_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_cloned_db_id_set(int unit, uint32 db_handles_info_idx_0, int cloned_db_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].cloned_db_id,
        cloned_db_id,
        int,
        0,
        "kbp_sw_state_db_handles_info_cloned_db_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        &cloned_db_id,
        "kbp_sw_state[%d]->db_handles_info[%d].cloned_db_id",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CLONED_DB_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_cloned_db_id_get(int unit, uint32 db_handles_info_idx_0, int *cloned_db_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        cloned_db_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        DNX_SW_STATE_DIAG_READ);

    *cloned_db_id = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].cloned_db_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].cloned_db_id,
        "kbp_sw_state[%d]->db_handles_info[%d].cloned_db_id",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_CLONED_DB_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_associated_dbal_table_id_set(int unit, uint32 db_handles_info_idx_0, int associated_dbal_table_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].associated_dbal_table_id,
        associated_dbal_table_id,
        int,
        0,
        "kbp_sw_state_db_handles_info_associated_dbal_table_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        &associated_dbal_table_id,
        "kbp_sw_state[%d]->db_handles_info[%d].associated_dbal_table_id",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_ASSOCIATED_DBAL_TABLE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_db_handles_info_associated_dbal_table_id_get(int unit, uint32 db_handles_info_idx_0, int *associated_dbal_table_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_handles_info_idx_0,
        DNX_KBP_NOF_DBS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        associated_dbal_table_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        DNX_SW_STATE_DIAG_READ);

    *associated_dbal_table_id = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].associated_dbal_table_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[db_handles_info_idx_0].associated_dbal_table_id,
        "kbp_sw_state[%d]->db_handles_info[%d].associated_dbal_table_id",
        unit, db_handles_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID,
        kbp_sw_state_info,
        KBP_SW_STATE_DB_HANDLES_INFO_ASSOCIATED_DBAL_TABLE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_instruction_info_set(int unit, uint32 instruction_info_idx_0, CONST kbp_instruction_handles_t *instruction_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        instruction_info_idx_0,
        DNX_KBP_NOF_INSTRUCTIONS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0],
        instruction_info,
        kbp_instruction_handles_t,
        0,
        "kbp_sw_state_instruction_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        instruction_info,
        "kbp_sw_state[%d]->instruction_info[%d]",
        unit, instruction_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        kbp_sw_state_info,
        KBP_SW_STATE_INSTRUCTION_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_instruction_info_get(int unit, uint32 instruction_info_idx_0, kbp_instruction_handles_t *instruction_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        instruction_info_idx_0,
        DNX_KBP_NOF_INSTRUCTIONS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        instruction_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        DNX_SW_STATE_DIAG_READ);

    *instruction_info = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0],
        "kbp_sw_state[%d]->instruction_info[%d]",
        unit, instruction_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO,
        kbp_sw_state_info,
        KBP_SW_STATE_INSTRUCTION_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_instruction_info_inst_p_set(int unit, uint32 instruction_info_idx_0, uint32 inst_p_idx_0, kbp_instruction_t_p inst_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        instruction_info_idx_0,
        DNX_KBP_NOF_INSTRUCTIONS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        inst_p_idx_0,
        DNX_KBP_NOF_SMTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0].inst_p[inst_p_idx_0],
        inst_p,
        kbp_instruction_t_p,
        0,
        "kbp_sw_state_instruction_info_inst_p_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        &inst_p,
        "kbp_sw_state[%d]->instruction_info[%d].inst_p[%d]",
        unit, instruction_info_idx_0, inst_p_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        kbp_sw_state_info,
        KBP_SW_STATE_INSTRUCTION_INFO_INST_P_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
kbp_sw_state_instruction_info_inst_p_get(int unit, uint32 instruction_info_idx_0, uint32 inst_p_idx_0, kbp_instruction_t_p *inst_p)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        sw_state_roots_array[unit][KBP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        instruction_info_idx_0,
        DNX_KBP_NOF_INSTRUCTIONS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        inst_p);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        inst_p_idx_0,
        DNX_KBP_NOF_SMTS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        DNX_SW_STATE_DIAG_READ);

    *inst_p = ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0].inst_p[inst_p_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->instruction_info[instruction_info_idx_0].inst_p[inst_p_idx_0],
        "kbp_sw_state[%d]->instruction_info[%d].inst_p[%d]",
        unit, instruction_info_idx_0, inst_p_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P,
        kbp_sw_state_info,
        KBP_SW_STATE_INSTRUCTION_INFO_INST_P_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][KBP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



const char *
dnx_kbp_ad_db_index_e_get_name(dnx_kbp_ad_db_index_e value)
{
#ifdef INCLUDE_KBP
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_KBP_AD_DB_INDEX_REGULAR", value, DNX_KBP_AD_DB_INDEX_REGULAR);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_KBP_AD_DB_INDEX_OPTIMIZED", value, DNX_KBP_AD_DB_INDEX_OPTIMIZED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_KBP_AD_DB_INDEX_LARGE_OPTIMIZED", value, DNX_KBP_AD_DB_INDEX_LARGE_OPTIMIZED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_KBP_NOF_AD_DB_INDEX", value, DNX_KBP_NOF_AD_DB_INDEX);

#endif  
    return NULL;
}




kbp_sw_state_cbs kbp_sw_state = 	{
	
	kbp_sw_state_is_init,
	kbp_sw_state_init,
		{
		
		kbp_sw_state_fwd_caching_enabled_set,
		kbp_sw_state_fwd_caching_enabled_get}
	,
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
			
			kbp_sw_state_db_handles_info_core_id_set,
			kbp_sw_state_db_handles_info_core_id_get}
		,
			{
			
			kbp_sw_state_db_handles_info_caching_enabled_set,
			kbp_sw_state_db_handles_info_caching_enabled_get}
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
			
			kbp_sw_state_db_handles_info_large_opt_result_size_set,
			kbp_sw_state_db_handles_info_large_opt_result_size_get}
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
#undef BSL_LOG_MODULE
