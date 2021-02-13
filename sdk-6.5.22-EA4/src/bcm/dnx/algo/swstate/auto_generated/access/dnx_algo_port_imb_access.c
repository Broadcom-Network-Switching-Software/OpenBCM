/** \file algo/swstate/auto_generated/access/dnx_algo_port_imb_access.c
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

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_imb_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/diagnostic/dnx_algo_port_imb_diagnostic.h>
/*
 * Global Variables
 */

/* dummy variable for gdb to know this type */
dnx_algo_port_imb_db_t* dnx_algo_port_imb_db_dummy = NULL;
/*
 * FUNCTIONs
 */

/*
 *
 * is_init function for the variable dnx_algo_port_imb_db
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_algo_port_imb_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]),
        "dnx_algo_port_imb_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * init function for the variable dnx_algo_port_imb_db
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_algo_port_imb_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_algo_port_imb_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_port_imb_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]),
        "dnx_algo_port_imb_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        dnx_algo_port_imb_db_info,
        DNX_ALGO_PORT_IMB_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_algo_port_imb_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * set function for the variable rmc_id_res
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_algo_port_imb_db_rmc_id_res_set(int unit, uint32 rmc_id_res_idx_0, uint32 rmc_id_res_idx_1, dnx_algo_res_t rmc_id_res)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res,
        rmc_id_res_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0],
        rmc_id_res_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        rmc_id_res,
        dnx_algo_res_t,
        0,
        "dnx_algo_port_imb_db_rmc_id_res_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        &rmc_id_res,
        "dnx_algo_port_imb_db[%d]->rmc_id_res[%d][%d]",
        unit, rmc_id_res_idx_0, rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        dnx_algo_port_imb_db_info,
        DNX_ALGO_PORT_IMB_DB_RMC_ID_RES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * get function for the variable rmc_id_res
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_algo_port_imb_db_rmc_id_res_get(int unit, uint32 rmc_id_res_idx_0, uint32 rmc_id_res_idx_1, dnx_algo_res_t *rmc_id_res)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res,
        rmc_id_res_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        rmc_id_res);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0],
        rmc_id_res_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID);

    *rmc_id_res = ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        &((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        "dnx_algo_port_imb_db[%d]->rmc_id_res[%d][%d]",
        unit, rmc_id_res_idx_0, rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        dnx_algo_port_imb_db_info,
        DNX_ALGO_PORT_IMB_DB_RMC_ID_RES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * alloc function for the variable rmc_id_res
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_algo_port_imb_db_rmc_id_res_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        dnx_data_device.general.nof_cores_get(unit),
        dnx_data_nif.eth.ethu_nof_per_core_get(unit),
        sizeof(dnx_algo_res_t*));

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res,
        dnx_algo_res_t*,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_port_imb_db_rmc_id_res_alloc");

    for(uint32 rmc_id_res_idx_0 = 0;
         rmc_id_res_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         rmc_id_res_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0],
        dnx_algo_res_t,
        dnx_data_nif.eth.ethu_nof_per_core_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_port_imb_db_rmc_id_res_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res,
        "dnx_algo_port_imb_db[%d]->rmc_id_res",
        unit,
        dnx_data_nif.eth.ethu_nof_per_core_get(unit) * sizeof(dnx_algo_res_t) / sizeof(*((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res)+(dnx_data_nif.eth.ethu_nof_per_core_get(unit) * sizeof(dnx_algo_res_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        dnx_algo_port_imb_db_info,
        DNX_ALGO_PORT_IMB_DB_RMC_ID_RES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * create function for the variable rmc_id_res
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_algo_port_imb_db_rmc_id_res_create(int unit, uint32 rmc_id_res_idx_0, uint32 rmc_id_res_idx_1, dnx_algo_res_create_data_t * data, void *extra_arguments)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res,
        rmc_id_res_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0],
        rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID);

    DNX_ALGO_RES_MNGR_CREATE(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        data,
        extra_arguments,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RES_MNGR_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_RES_MNGRCREATE,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        &((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        "dnx_algo_port_imb_db[%d]->rmc_id_res[%d][%d]",
        unit, rmc_id_res_idx_0, rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        dnx_algo_port_imb_db_info,
        DNX_ALGO_PORT_IMB_DB_RMC_ID_RES_INFO,
        DNX_SW_STATE_DIAG_RES_MNGR,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * allocate_single function for the variable rmc_id_res
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_algo_port_imb_db_rmc_id_res_allocate_single(int unit, uint32 rmc_id_res_idx_0, uint32 rmc_id_res_idx_1, uint32 flags, void *extra_arguments, int *element)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        SW_STATE_FUNC_ALLOCATE_SINGLE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res,
        rmc_id_res_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0],
        rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID);

    DNX_ALGO_RES_MNGR_ALLOCATE_SINGLE(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        flags,
        extra_arguments,
        element);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RES_MNGR_ALLOCATE_SINGLE_LOGGING,
        BSL_LS_SWSTATEDNX_RES_MNGRALLOCATE_SINGLE,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        &((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        "dnx_algo_port_imb_db[%d]->rmc_id_res[%d][%d]",
        unit, rmc_id_res_idx_0, rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        dnx_algo_port_imb_db_info,
        DNX_ALGO_PORT_IMB_DB_RMC_ID_RES_INFO,
        DNX_SW_STATE_DIAG_RES_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * free_single function for the variable rmc_id_res
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_algo_port_imb_db_rmc_id_res_free_single(int unit, uint32 rmc_id_res_idx_0, uint32 rmc_id_res_idx_1, int element, void *extra_arguments)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        SW_STATE_FUNC_FREE_SINGLE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res,
        rmc_id_res_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0],
        rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID);

    DNX_ALGO_RES_MNGR_FREE_SINGLE(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        element,
        extra_arguments);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RES_MNGR_FREE_SINGLE_LOGGING,
        BSL_LS_SWSTATEDNX_RES_MNGRFREE_SINGLE,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        &((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        "dnx_algo_port_imb_db[%d]->rmc_id_res[%d][%d]",
        unit, rmc_id_res_idx_0, rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        dnx_algo_port_imb_db_info,
        DNX_ALGO_PORT_IMB_DB_RMC_ID_RES_INFO,
        DNX_SW_STATE_DIAG_RES_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * is_allocated function for the variable rmc_id_res
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_algo_port_imb_db_rmc_id_res_is_allocated(int unit, uint32 rmc_id_res_idx_0, uint32 rmc_id_res_idx_1, int element, uint8 *is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        SW_STATE_FUNC_IS_ALLOCATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res,
        rmc_id_res_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0],
        rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID);

    DNX_ALGO_RES_MNGR_IS_ALLOCATED(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        element,
        is_allocated);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RES_MNGR_IS_ALLOCATED_LOGGING,
        BSL_LS_SWSTATEDNX_RES_MNGRIS_ALLOCATED,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        &((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        "dnx_algo_port_imb_db[%d]->rmc_id_res[%d][%d]",
        unit, rmc_id_res_idx_0, rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        dnx_algo_port_imb_db_info,
        DNX_ALGO_PORT_IMB_DB_RMC_ID_RES_INFO,
        DNX_SW_STATE_DIAG_RES_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * nof_free_elements_get function for the variable rmc_id_res
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_algo_port_imb_db_rmc_id_res_nof_free_elements_get(int unit, uint32 rmc_id_res_idx_0, uint32 rmc_id_res_idx_1, int *nof_free_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        SW_STATE_FUNC_NOF_FREE_ELEMENTS_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res,
        rmc_id_res_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0],
        rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID);

    DNX_ALGO_RES_MNGR_NOF_FREE_ELEMENTS_GET(
        DNX_ALGO_PORT_IMB_MODULE_ID,
        ((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        nof_free_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RES_MNGR_NOF_FREE_ELEMENTS_GET_LOGGING,
        BSL_LS_SWSTATEDNX_RES_MNGRNOF_FREE_ELEMENTS_GET,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        &((dnx_algo_port_imb_db_t*)sw_state_roots_array[unit][DNX_ALGO_PORT_IMB_MODULE_ID])->rmc_id_res[rmc_id_res_idx_0][rmc_id_res_idx_1],
        "dnx_algo_port_imb_db[%d]->rmc_id_res[%d][%d]",
        unit, rmc_id_res_idx_0, rmc_id_res_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_ALGO_PORT_IMB_MODULE_ID,
        dnx_algo_port_imb_db_info,
        DNX_ALGO_PORT_IMB_DB_RMC_ID_RES_INFO,
        DNX_SW_STATE_DIAG_RES_MNGR,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 * Global Variables
 */

dnx_algo_port_imb_db_cbs dnx_algo_port_imb_db = 	{
	
	dnx_algo_port_imb_db_is_init,
	dnx_algo_port_imb_db_init,
		{
		
		dnx_algo_port_imb_db_rmc_id_res_set,
		dnx_algo_port_imb_db_rmc_id_res_get,
		dnx_algo_port_imb_db_rmc_id_res_alloc,
		dnx_algo_port_imb_db_rmc_id_res_create,
		dnx_algo_port_imb_db_rmc_id_res_allocate_single,
		dnx_algo_port_imb_db_rmc_id_res_free_single,
		dnx_algo_port_imb_db_rmc_id_res_is_allocated,
		dnx_algo_port_imb_db_rmc_id_res_nof_free_elements_get}
	}
;
#undef BSL_LOG_MODULE
