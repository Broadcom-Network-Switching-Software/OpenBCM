/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * \file algo_l3_egr_pointed.c
 *
 * This file contains the public APIs required for the tables
 * that map 'virtual egress pointed' objects to 'local out lif'
 * and vice versa.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSPOINTED

/*************
 * INCLUDES  *
 */
/*
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/l3/algo_l3_egr_pointed.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */
/*
 * }
 */
/*************
 * TYPE DEFS *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */

/**********************************************
 * FUNCTIONS related to EGR_POINTED objects   *
 */
/*
 * {
 */
/** 
 *  \brief
 *  Verification function for dnx_algo_l3_egr_pointed_local_lif_mapping_add.
 *  'flags' are currently not in use
 *  Check that lifs are in legal ID range.
 */
static shr_error_e
dnx_algo_l3_egr_pointed_local_lif_mapping_add_verify(
    int unit,
    uint32 flags,
    dnx_egr_pointed_t egr_pointed_id,
    int local_lif)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify lif id EGR_POINTED id are legal.
     */
    DNX_EGR_POINTED_CHK_ID(unit, egr_pointed_id);
    LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, local_lif);

exit:
    SHR_FUNC_EXIT;
}
/*
 * See header in algo_l3_egr_pointed.h
 */
shr_error_e
dnx_algo_l3_egr_pointed_local_lif_mapping_add(
    int unit,
    uint32 flags,
    dnx_egr_pointed_t egr_pointed_id,
    int local_lif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_algo_l3_egr_pointed_local_lif_mapping_add_verify(unit, flags, egr_pointed_id, local_lif));
    /*
     *  EGR_POINTED to local
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_POINTED_TO_EGRESS_LIF_MAP_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIRTUAL_EGRESS_POINTED, egr_pointed_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE, local_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     *  Local to EGR_POINTED
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_LIF_TO_EGRESS_POINTED_MAP_SW, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VIRTUAL_EGRESS_POINTED, INST_SINGLE, egr_pointed_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/** 
 *  \brief
 *  Verification function for dnx_algo_l3_egr_pointed_local_lif_mapping_remove.
 *  'flags' ae currently not in use
 *  Check that lifs are in legal ID range.
 */
static shr_error_e
dnx_algo_l3_egr_pointed_local_lif_mapping_remove_verify(
    int unit,
    uint32 flags,
    dnx_egr_pointed_t egr_pointed_id)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify EGR_POINTED id is legal.
     */
    DNX_EGR_POINTED_CHK_ID(unit, egr_pointed_id);
exit:
    SHR_FUNC_EXIT;
}
/*
 * See header in algo_l3_egr_pointed.h
 */
shr_error_e
dnx_algo_l3_egr_pointed_local_lif_mapping_remove(
    int unit,
    uint32 flags,
    dnx_egr_pointed_t egr_pointed_id)
{
    uint32 entry_handle_id;
    int local_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_algo_l3_egr_pointed_local_lif_mapping_remove_verify(unit, flags, egr_pointed_id));
    /*
     *  Remove mapping.
     *
     *  Remove the mapping from global to local lif.
     */
    /*
     *  EGR_POINTED to local
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_POINTED_TO_EGRESS_LIF_MAP_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIRTUAL_EGRESS_POINTED, egr_pointed_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE,
                                                        (uint32 *) (&local_lif)));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    /*
     *  Local to EGR_POINTED
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_LIF_TO_EGRESS_POINTED_MAP_SW, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** 
 *  \brief
 *  Verification function for dnx_algo_l3_local_lif_to_egr_pointed_get().
 *  Ignore 'flags' for now
 *  Check NULL pointers and check lifs to be in legal ID range.
 */
static shr_error_e
dnx_algo_l3_local_lif_to_egr_pointed_get_verify(
    int unit,
    uint32 flags,
    int local_lif,
    dnx_egr_pointed_t * egr_pointed_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(egr_pointed_id_p, _SHR_E_PARAM, "egr_pointed_id_p");
    /*
     * Verify lif IDs are legal.
     */
    LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, local_lif);
exit:
    SHR_FUNC_EXIT;
}
/*
 * See header in algo_l3_egr_pointed.h
 */
shr_error_e
dnx_algo_l3_local_lif_to_egr_pointed_get(
    int unit,
    uint32 flags,
    int local_lif,
    dnx_egr_pointed_t * egr_pointed_id_p)
{
    uint32 entry_handle_id;
    shr_error_e shr_error;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Check input
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_l3_local_lif_to_egr_pointed_get_verify(unit, flags, local_lif, egr_pointed_id_p));
    /*
     * Get EGR_POINTED from the map.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_LIF_TO_EGRESS_POINTED_MAP_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif);
    shr_error = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (shr_error != _SHR_E_NONE)
    {
        /*
         * If dbal_entry_get() returned with an error which is NOT_FOUND then just
         * roll this error to the caller but do not log any error message. Caller
         * is responsible for further handling.
         * Otherwise, apply standard error handling.
         */
        if (shr_error == _SHR_E_NOT_FOUND)
        {
            *egr_pointed_id_p = DNX_ALGO_EGR_POINTED_INVALID;
            SHR_SET_CURRENT_ERR(shr_error);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(shr_error);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VIRTUAL_EGRESS_POINTED, INST_SINGLE,
                     (uint32 *) (egr_pointed_id_p)));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/** 
 *  \brief
 *  Verification function for dnx_algo_l3_egr_pointed_to_local_lif_get().
 *  Ignore 'flags'
 *  Check NULL pointers
 *  Check EGR_POINTED is in legal ID range.
 */
static shr_error_e
dnx_algo_l3_egr_pointed_to_local_lif_get_verify(
    int unit,
    uint32 flags,
    dnx_egr_pointed_t egr_pointed_id,
    int *local_lif_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(local_lif_p, _SHR_E_PARAM, "local_lif_p");
    /*
     * Verify EGR_POINTED is in legal ID range.
     */
    DNX_EGR_POINTED_CHK_ID(unit, egr_pointed_id);
exit:
    SHR_FUNC_EXIT;
}
/*
 * See header in algo_l3_egr_pointed.h
 */
shr_error_e
dnx_algo_l3_egr_pointed_to_local_lif_get(
    int unit,
    int flags,
    dnx_egr_pointed_t egr_pointed_id,
    int *local_lif_p)
{
    uint32 entry_handle_id;
    shr_error_e shr_error;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Check and analyze input. 
     */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_l3_egr_pointed_to_local_lif_get_verify(unit, flags, egr_pointed_id, local_lif_p));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_POINTED_TO_EGRESS_LIF_MAP_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIRTUAL_EGRESS_POINTED, egr_pointed_id);
    shr_error = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (shr_error != _SHR_E_NONE)
    {
        /*
         * If dbal_entry_get() returned with an error which is NOT_FOUND then just
         * roll this error to the caller but do not log any error message. Caller
         * is responsible for further handling.
         * Otherwise, apply standard error handling.
         */
        if (shr_error == _SHR_E_NOT_FOUND)
        {
            *local_lif_p = DNX_ALGO_GPM_LIF_INVALID;
            SHR_SET_CURRENT_ERR(shr_error);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(shr_error);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE,
                                                        (uint32 *) (local_lif_p)));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See header in algo_l3_egr_pointed.h
 */
shr_error_e
dnx_algo_l3_egr_pointed_tables_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Add init functions, if any
     */
    SHR_FUNC_EXIT;
}
/*
 * See header in algo_l3_egr_pointed.h
 */
shr_error_e
dnx_algo_l3_egr_pointed_tables_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Add deinit functions, if any
     */
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * }
 */
