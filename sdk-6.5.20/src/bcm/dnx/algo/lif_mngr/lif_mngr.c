/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file lif_mngr.c
 *
 *  Lif algorithms initialization and deinitialization.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

/*************
 * INCLUDES  *
 */
/*
 * {
 */
#include <sal/appl/sal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include "global_lif_allocation.h"
#include "lif_mngr_internal.h"
#include "lif_table_mngr.h"
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/mdb.h>
#include <soc/dnxc/drv_dnxc_utils.h>
/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */

#define DNX_ALGO_LIF_MNGR_REDUCED_MODE_NOF_LIFS 100

#define DNX_ALGO_LIF_OUTLIF_EXPLICIT_STAGE_STRING "EXPLICIT_"

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
extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

/*
 * }
 */
/*************
 * GLOBALS   *
 */
/*
 * {
 */

static const char *lif_mngr_outlif_phase_name[LIF_MNGR_OUTLIF_PHASE_COUNT] = {
    "PHASE_AC",
    "PHASE_NATIVE_AC",
    "PHASE_ARP",
    "PHASE_NATIVE_ARP",
    "PHASE_MPLS_TUNNEL_1",
    "PHASE_MPLS_TUNNEL_2",
    "PHASE_MPLS_TUNNEL_3",
    "PHASE_MPLS_TUNNEL_4",
    "PHASE_SRV6_BASE",
    "PHASE_IPV6_RAW_SRV6_TUNNEL",
    "PHASE_IP_TUNNEL_1",
    "PHASE_IP_TUNNEL_2",
    "PHASE_IP_TUNNEL_3",
    "PHASE_IP_TUNNEL_4",
    "PHASE_VPLS_1",
    "PHASE_VPLS_2",
    "PHASE_RIF",
    "PHASE_SFLOW",
    "PHASE_SFLOW_RAW_1",
    "PHASE_SFLOW_RAW_2",
    "PHASE_REFLECTOR",
    "PHASE_RCH"
};

/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */

shr_error_e
dnx_lif_mngr_phase_string_to_id(
    int unit,
    char *outlif_phase_str,
    lif_mngr_outlif_phase_e * outlif_phase)
{
    int outlif_phase_index, strlen_input, strlen_explicit;

    SHR_FUNC_INIT_VARS(unit);

    strlen_input = sal_strlen(outlif_phase_str);
    strlen_explicit = sal_strlen(DNX_ALGO_LIF_OUTLIF_EXPLICIT_STAGE_STRING) + 1;

    if ((strlen_input == strlen_explicit) &&
        (sal_strstr(outlif_phase_str, DNX_ALGO_LIF_OUTLIF_EXPLICIT_STAGE_STRING) != NULL) &&
        (outlif_phase_str[strlen_input - 1] - '1' >= 0) && (outlif_phase_str[strlen_input - 1] - '1' <= 7))
    {
        /*
         * Explicit phase. The last character is the phase.
         */
        outlif_phase_index = outlif_phase_str[strlen_explicit - 1] - '1';
        *outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + outlif_phase_index;
        SHR_EXIT();
    }

    /*
     * If it's not explicit phase, then search the array for a matching string.
     */
    for (outlif_phase_index = 0; outlif_phase_index < LIF_MNGR_OUTLIF_PHASE_COUNT; outlif_phase_index++)
    {
        if (sal_strcasecmp(lif_mngr_outlif_phase_name[outlif_phase_index], outlif_phase_str) == 0)
        {
            *outlif_phase = outlif_phase_index;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "outlif phase type not found (%s)\n", outlif_phase_str);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_eedb_table_rt_has_linked_list(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    uint8 *has_link_list)
{
    uint8 found_next_ptr = FALSE;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (res_type_idx < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "result type cannot have negative value at that point (%d)\n", res_type_idx);
    }

    if ((table->access_method == DBAL_ACCESS_METHOD_MDB) && dbal_table_is_out_lif(table))
    {
        if (res_type_idx < table->nof_result_types)
        {
            found_next_ptr = table->multi_res_info[res_type_idx].has_link_list;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal result type index %d for table %s. num of result types is %d\n",
                         res_type_idx, table->table_name, table->nof_result_types);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "API can not called with table which is not EEDB table. table %s.\n",
                     table->table_name);
    }
    *has_link_list = found_next_ptr;

exit:
    SHR_FUNC_EXIT;
}

/*
 * Set table id and result type from SW state
 * by local inlif
 */
shr_error_e
dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_set(
    int unit,
    int local_in_lif,
    int core_id,
    dbal_tables_e table_id,
    uint32 result_type,
    uint32 fixed_entry_size)
{
    dbal_physical_tables_e physical_table_id;
    ingress_lif_info_t ingress_lif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify that the configuration is for all the cores
     */
    if ((core_id != _SHR_CORE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error: local In-LIF 0x%08X Accessed with wrong core value - %d", local_in_lif, core_id);
    }

    /*
     * Get the physical table associated with this lif format.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /*
     * format the set data to the SW state structure
     */
    ingress_lif_info.dbal_table_id = table_id;
    ingress_lif_info.dbal_result_type = result_type;

    /*
     * Set the appropriate local In-LIF SW State table
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.ingress_lif1.set(unit, local_in_lif, &ingress_lif_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.ingress_lif2.set(unit, local_in_lif, &ingress_lif_info));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(
    int unit,
    int local_in_lif,
    int core_id,
    dbal_physical_tables_e physical_table_id,
    dbal_tables_e * table_id,
    uint32 *result_type,
    uint32 *fixed_entry_size)
{
    ingress_lif_info_t ingress_lif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the local In-LIF info from the appropriate SW State table
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.ingress_lif1.get(unit, local_in_lif, &ingress_lif_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.ingress_lif2.get(unit, local_in_lif, &ingress_lif_info));

    }
    if (ingress_lif_info.dbal_table_id == DBAL_TABLE_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error: Failed to retrieve valid LIF Manager info for local In-LIF 0x%08X", local_in_lif);
    }

    /*
     * Retrieve the returned parameters from the SW state structure
     */
    SHR_IF_NOT_NULL_FILL(table_id, ingress_lif_info.dbal_table_id);
    SHR_IF_NOT_NULL_FILL(result_type, ingress_lif_info.dbal_result_type);
    SHR_IF_NOT_NULL_FILL(fixed_entry_size, 0);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_delete(
    int unit,
    int core_id,
    uint32 local_in_lif,
    dbal_tables_e table_id)
{
    dbal_physical_tables_e physical_table_id;
    ingress_lif_info_t ingress_lif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify that the deltetion is from all the cores
     */
    if (core_id != _SHR_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error: local In-LIF 0x%08X Accessed with wrong core value - %d", local_in_lif, core_id);
    }

    /*
     * Get the physical table associated with this lif format.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /*
     * Set the entry to reset values
     */
    ingress_lif_info.dbal_table_id = DBAL_TABLE_EMPTY;
    ingress_lif_info.dbal_result_type = 0;

    /*
     * Reset the entry from the relevant SW State
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.ingress_lif1.set(unit, local_in_lif, &ingress_lif_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.ingress_lif2.set(unit, local_in_lif, &ingress_lif_info));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Set table id and result type from SW state
 * by local outlif
 */
shr_error_e
dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int local_out_lif,
    dbal_tables_e table_id,
    uint32 result_type,
    uint32 fixed_entry_size)
{
    egress_lif_info_t egress_lif_info;
    uint8 found, ll_exists;
    dnx_local_outlif_ll_indication_e ll_indication = DNX_LOCAL_OUTLIF_LL_OPTIONAL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Set the logical phase. Required prior to LL-exist retrieval
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num
                    (unit, outlif_info->outlif_phase,
                     (dnx_algo_local_outlif_logical_phase_e *) (&egress_lif_info.logical_phase)));

    /*
     * Retrieve the LL exists indication for the Out-LIF
     */
    if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST))
    {
        ll_exists = TRUE;
    }
    else if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST))
    {
        ll_exists = FALSE;
    }
    else                                                                                    /**No flag*/
    {
        if (!_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH)
            && (table_id != DBAL_TABLE_EEDB_DATA_ENTRY))
        {
            dbal_to_phase_info_t dbal_to_phase_info = { 0 };
            dbal_to_phase_info.dbal_result_type = result_type;
            dbal_to_phase_info.dbal_table = table_id;
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                            dbal_valid_combinations.find(unit, egress_lif_info.logical_phase, &dbal_to_phase_info,
                                                         &ll_indication, &found));

            if (!found)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "entry not found in logical phases hash table: logical table %d, table_id %s, result type %s",
                             egress_lif_info.logical_phase + 1, dbal_logical_table_to_string(unit, table_id),
                             dbal_result_type_to_string(unit, outlif_info->dbal_table_id, result_type));
            }
        }

        if ((table_id == DBAL_TABLE_EEDB_DATA_ENTRY) || (ll_indication == DNX_LOCAL_OUTLIF_LL_OPTIONAL)
            ||
            (_SHR_IS_FLAG_SET
             (outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH)))
        {
            SHR_IF_ERR_EXIT(dnx_lif_mngr_eedb_table_rt_has_linked_list(unit, table_id, result_type, &ll_exists));
        }
        else if (ll_indication == DNX_LOCAL_OUTLIF_LL_ALWAYS)
        {
            ll_exists = TRUE;
        }
        else if (ll_indication == DNX_LOCAL_OUTLIF_LL_NEVER)
        {
            ll_exists = FALSE;
        }
    }

    /*
     * Set the rest of the SW DB table structure parameters that shoule be initiated ar creation
     */
    egress_lif_info.dbal_table_id = table_id;
    egress_lif_info.dbal_result_type = result_type;
    egress_lif_info.ll_exists = ll_exists;

    /*
     * Insert the entry to the SW DB
     */
    SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.egress_lif_htb.insert(unit, &local_out_lif, &egress_lif_info, &found));
    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: Failed to store LIF manager info for local Out-LIF 0x%08X, found - %d",
                     local_out_lif, found);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_outlif_sw_info_get(
    int unit,
    int local_out_lif,
    dbal_tables_e * dbal_table_id,
    uint32 *result_type,
    lif_mngr_outlif_phase_e * outlif_phase,
    uint32 *fixed_entry_size,
    uint8 *has_ll,
    uint8 *is_count_profile)
{
    egress_lif_info_t egress_lif_info;
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the entry from the SW DB
     */
    SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.egress_lif_htb.find(unit, &local_out_lif, &egress_lif_info, &found));
    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error: Failed to retrieve LIF manager info for local Out-LIF 0x%08X, found - %d", local_out_lif,
                     found);
    }

    /*
     * Format the requested fields from the SW DB structure
     */
    SHR_IF_NOT_NULL_FILL(dbal_table_id, egress_lif_info.dbal_table_id);
    SHR_IF_NOT_NULL_FILL(result_type, egress_lif_info.dbal_result_type);
    SHR_IF_NOT_NULL_FILL(has_ll, egress_lif_info.ll_exists);
    SHR_IF_NOT_NULL_FILL(is_count_profile, egress_lif_info.stat_pp_etpp_is_count_profile);
    SHR_IF_NOT_NULL_FILL(fixed_entry_size, 0);

    /*
     * Retrieve the Out-LIF phase
     */
    if (outlif_phase)
    {
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_to_eedb_logical_phase
                        (unit, local_out_lif, egress_lif_info.dbal_table_id, outlif_phase));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(
    int unit,
    int local_out_lif)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Delete the entry from the SW DB
     */
    SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.egress_lif_htb.delete(unit, &local_out_lif));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_outlif_sw_info_is_count_profile_set(
    int unit,
    int local_out_lif)
{
    egress_lif_info_t egress_lif_info;
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the local Out-LIF information
     */
    SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.egress_lif_htb.find(unit, &local_out_lif, &egress_lif_info, &found));
    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error: Failed to retrieve LIF manager info for local Out-LIF 0x%08X, found - %d", local_out_lif,
                     found);
    }

    /*
     * Delete the local Out-LIF information
     */
    SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.egress_lif_htb.delete(unit, &local_out_lif));

    /*
     * Add an entry for the local Out-LIF information, updated with is_count_profile indication
     */
    egress_lif_info.stat_pp_etpp_is_count_profile = TRUE;
    SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.egress_lif_htb.insert(unit, &local_out_lif, &egress_lif_info, &found));
    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: Failed to insert LIF manager info for local Out-LIF 0x%08X, found - %d",
                     local_out_lif, found);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify inlif info struct: \ref lif_mngr_local_inlif_info_t.
 *  By default, all fields are always checked, according to the dbal/device data legal values.
 *
 *  This functions assumes that for DPC lifs all core id options are legal, so if only a specific
 *  core is required, it must be checked outside this function.
 *
 *  The field 'dbal_result_type' is not required in most APIs that use this sturct, and it can be set
 *     to 0 if it's not required.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] ignore_id  - If this boolean arguemnt is TRUE, then the 'local_inlif' field of 'inlif_info' \n
 *                          will not be verified.
 * \param [in] inlif_info - The fields in this struct will be checked as described above. \n
 *                          See \ref lif_mngr_local_inlif_info_t for description of specific fields.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   lif_mngr_local_inlif_info_t
 */
static shr_error_e
dnx_lif_mngr_local_inlif_info_verify(
    int unit,
    int ignore_id,
    lif_mngr_local_inlif_info_t * inlif_info)
{
    dbal_physical_tables_e physical_table_id;
    int is_replace, is_reserve;
    int entry_size_bits;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(inlif_info, _SHR_E_INTERNAL, "inlif_info");

    /*
     * Verify the REPLACE and RESERVE flags.
     */
    is_reserve = _SHR_IS_FLAG_SET(inlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE);
    is_replace = _SHR_IS_FLAG_SET(inlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE);

    if (is_reserve || is_replace)
    {
        /**Verify only if old_result type is not 0*/
        if (inlif_info->old_result_type != 0)
        {
            /*
             * Verify alternative result type.
             */
            SHR_IF_ERR_EXIT(dbal_tables_payload_size_get
                            (unit, inlif_info->dbal_table_id, inlif_info->old_result_type, &entry_size_bits));
        }

    }

    /*
     * Get the physical table associated with this lif format. The physical table will tell us
     *  whether it's a DPC or SBC lif table.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, inlif_info->dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                   &physical_table_id));

    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        /*
         * This is the SBC table. Verify that core is _SHR_CORE_ALL.
         */
        if (inlif_info->core_id != _SHR_CORE_ALL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Dbal logical table %s is shared by cores, so core_id must be _SHR_CORE_ALL. Given %d.",
                         dbal_logical_table_to_string(unit, inlif_info->dbal_table_id), inlif_info->core_id);
        }

        if (ignore_id == FALSE)
        {
            /*
             * If required, verify that lif id in index 0 is legal.
             * We only check this ID because this is the ID shared by cores inlif table is using.
             * Lif IDs in different array indexes are not used.
             */
            LIF_MNGR_LOCAL_SBC_IN_LIF_VERIFY(unit, inlif_info->local_inlif);
        }
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        /*
         * This is a DPC table, so just verify that the core is legal. _SHR_CORE_ALL is also legal.
         */
        DNXCMN_CORE_VALIDATE(unit, inlif_info->core_id, TRUE);

        if (ignore_id == FALSE)
        {
            uint8 is_allocated_on_all_cores;
            /*
             * Check that the lif id is legal, and that it's either allocated on all cores or on a single core, and compare it to
             * the input.
             */
            LIF_MNGR_LOCAL_DPC_IN_LIF_VERIFY(unit, inlif_info->local_inlif);
            /*
             * For save memory, inlif_table_dpc_all_cores_indication resource isn't allocated
             */
            if (DNXCMN_CHIP_IS_MULTI_CORE(unit))
            {
                SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                                inlif_table_dpc_all_cores_indication.bit_get(unit, inlif_info->local_inlif,
                                                                             &is_allocated_on_all_cores));
            }
            else
            {
                is_allocated_on_all_cores = 1;
            }

            if (!is_allocated_on_all_cores && inlif_info->core_id == _SHR_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Inlif is indicated to be allocated on all cores, but it's actually allocated"
                             "on every core separately.");
            }
            else if (is_allocated_on_all_cores && inlif_info->core_id != _SHR_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Inlif is indicated to be allocated only on core %d, but it's actually allocated"
                             "on all cores at once.", inlif_info->core_id);

            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong dbal logical table: %s",
                     dbal_logical_table_to_string(unit, inlif_info->dbal_table_id));
    }

    /*
     * Only check result type if it's > 0, otherwise it's always legal.
     */
    if (inlif_info->dbal_result_type != 0)
    {
        /*
         * Calling the get function is enough, because it has a built in error indication if the result type
         * is illegal.
         */
        SHR_IF_ERR_EXIT(dbal_tables_payload_size_get
                        (unit, inlif_info->dbal_table_id, inlif_info->dbal_result_type, &entry_size_bits));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify out info struct: \ref lif_mngr_local_outlif_info_t.
 *  By default, all fields are always checked, according to the dbal/device data legal values.
 *
 *  The field 'dbal_result_type' is not required in most APIs that use this sturct, and it can be set
 *     to 0 if it's not required.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] ignore_id - If this boolean arguemnt is TRUE, then the 'local_outlif' field of 'outlif_info' \n
 *                          will not be verified.
 * \param [in] outlif_info - The fields in this struct will be checked as described above. \n
 *                          See \ref lif_mngr_local_outlif_info_t for description of specific fields.
 * \param [in] outlif_hw_info - This struct will be NULL checked. \n
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   lif_mngr_local_outlif_info_t
 */
static shr_error_e
dnx_lif_mngr_local_outlif_info_verify(
    int unit,
    int ignore_id,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    dbal_physical_tables_e physical_table_id;
    dnx_algo_local_outlif_logical_phase_e outlif_logical_phase;
    int is_reserve, is_replace;
    int entry_size_bits;
    uint8 found;
    dnx_local_outlif_ll_indication_e ll_indication;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(outlif_info, _SHR_E_INTERNAL, "outlif_info");
    SHR_NULL_CHECK(outlif_hw_info, _SHR_E_INTERNAL, "outlif_hw_info");

    /*
     * Check outlif phase.
     * Calling the get function is enough, because it has a built in error indication if the no phase mapping
     * was found.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num
                    (unit, outlif_info->outlif_phase, &outlif_logical_phase));

    if (!_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH)
        && (outlif_info->dbal_table_id != DBAL_TABLE_EEDB_DATA_ENTRY))
    {
        /*
         * Verify {logical phase, dbal table, result type} valid combinations
         */
        dbal_to_phase_info_t dbal_to_phase_info = { 0 };
        dbal_to_phase_info.dbal_result_type = outlif_info->dbal_result_type;
        dbal_to_phase_info.dbal_table = outlif_info->dbal_table_id;
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        dbal_valid_combinations.find(unit, outlif_logical_phase, &dbal_to_phase_info, &ll_indication,
                                                     &found));

        if (found == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For logical phase %d, the combination: {%s,%s} is not valid",
                         outlif_logical_phase + 1, dbal_logical_table_to_string(unit, outlif_info->dbal_table_id),
                         dbal_result_type_to_string(unit, outlif_info->dbal_table_id, outlif_info->dbal_result_type));
        }

        if (ll_indication != DNX_LOCAL_OUTLIF_LL_OPTIONAL)
        {
            if ((_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST))
                && (ll_indication == DNX_LOCAL_OUTLIF_LL_NEVER))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST flag is set but for phsae %d: {%s, %s} link list indication is set to NEVER",
                             outlif_logical_phase, dbal_logical_table_to_string(unit, outlif_info->dbal_table_id),
                             dbal_result_type_to_string(unit, outlif_info->dbal_table_id,
                                                        outlif_info->dbal_result_type));
            }
            if ((_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST))
                && (ll_indication == DNX_LOCAL_OUTLIF_LL_ALWAYS))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST flag is set but for phsae %d: {%s, %s} link list indication is set to ALWAYS",
                             outlif_logical_phase, dbal_logical_table_to_string(unit, outlif_info->dbal_table_id),
                             dbal_result_type_to_string(unit, outlif_info->dbal_table_id,
                                                        outlif_info->dbal_result_type));
            }
        }
    }

    /*
     * Get the physical table associated with this lif format. Verify that the physical table is EEDB.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, outlif_info->dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                   &physical_table_id));

    if (physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_1
        && physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_2
        && physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_3 && physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_4
        && physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_5 && physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_6
        && physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_7 && physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_8)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong dbal logical table: %s",
                     dbal_logical_table_to_string(unit, outlif_info->dbal_table_id));
    }

    /*
     * Only check result type if it's >0, otherwise it's not interesting.
     */
    if (outlif_info->dbal_result_type != 0)
    {
        /*
         * Calling the get function is enough, because it has a built in error indication if the result type
         * is illegal.
         */
        SHR_IF_ERR_EXIT(dbal_tables_payload_size_get
                        (unit, outlif_info->dbal_table_id, outlif_info->dbal_result_type, &entry_size_bits));
    }

    /*
     * Verify the REPLACE and RESERVE flags.
     */
    is_reserve = _SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE);
    is_replace = _SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE);

    if (is_reserve && is_replace)
    {
        int ll_match;
        /*
         * Verify alternative result type.
         */
        SHR_IF_ERR_EXIT(dbal_tables_payload_size_get
                        (unit, outlif_info->dbal_table_id, outlif_info->alternative_result_type, &entry_size_bits));

        /*
         * Verify ll match.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_ll_match_check(unit, outlif_info, &ll_match));

        if (!ll_match)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Can't have a new result type with linked list and old type without linked list.");
        }

    }

    if (!ignore_id)
    {
        LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, outlif_info->local_outlif);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verification fuction for dnx_lif_mngr_lif_allocate.
 * Note - it's possible to verify the global lif id if the WITH_ID flag was given,
 *  but we currently skip it because it's checked in the main function by calling the global lif
 *  verify function.
 */
static shr_error_e
dnx_lif_mngr_lif_allocate_verify(
    int unit,
    uint32 flags,
    int *global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    int is_ingress, is_egress, dont_allocate_global_lif;
    SHR_FUNC_INIT_VARS(unit);

    dont_allocate_global_lif = _SHR_IS_FLAG_SET(flags, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF);

    is_ingress = (inlif_info != NULL);
    is_egress = (outlif_info != NULL);

    if ((!is_ingress) && (!is_egress))
    {
        /*
         * A lif can be either an inlif or an outlif, or both (symmetric). If neither side is given as
         *   input, then the lif has no meaning.
         */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "At least one of inlif_info or outlif_info must be not NULL.");
    }

    if (is_ingress && is_egress && dont_allocate_global_lif)
    {
        /*
         * If there's no global lif, then allocating both ingress and egress global lifs is meaningless,
         * since without global lifs they can't be symmetric. Instead, allocate them in two separate calls.
         */
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "If don't allocate global lif was indicated, only one direction (ingress or egress) "
                     "can be used.");
    }

    /*
     * Verify inlif info.
     */
    if (is_ingress)
    {
        /*
         * We set the argument 'ignore_ids' to TRUE because local lifs can't be allocated WITH_ID,
         * so the IDs are ignored and shouldn't be verified.
         */
        SHR_IF_ERR_EXIT(dnx_lif_mngr_local_inlif_info_verify(unit, TRUE, inlif_info));
    }

    /*
     * Verify outlif info.
     */
    if (is_egress)
    {
        /*
         * We set the argument 'ignore_ids' to TRUE because local lifs can't be allocated WITH_ID,
         * so the IDs are ignored and shouldn't be verified.
         */
        SHR_IF_ERR_EXIT(dnx_lif_mngr_local_outlif_info_verify(unit, TRUE, outlif_info, outlif_hw_info));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_lif_allocate(
    int unit,
    uint32 flags,
    int *global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    int global_lif_tmp;
    /*
     * The is_ingress, is_egress flags are used to indicate whether we'll allocate ingress/egress lifs in this function.
     * In some cases, we might choose not to allocate the egress global lif. In this case, the allocate_egress_global_lif flags
     * will be set to FALSE.
     */
    int is_ingress, is_egress, allocate_global_lif;
    uint32 global_lif_allocation_flags, global_lif_direction_flags;
    lif_mapping_local_lif_info_t local_lif_info;
    uint32 dbal_result_type;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Step 0: Verify input and set allocation flags.
     *
     * Verify local lif info. We verify global lif info below so we don't need to calculate all the
     * flags twice.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_lif_mngr_lif_allocate_verify
                          (unit, flags, global_lif, inlif_info, outlif_info, outlif_hw_info));

    is_ingress = (inlif_info != NULL);
    is_egress = (outlif_info != NULL);
    global_lif_allocation_flags = 0;
    global_lif_direction_flags = 0;

    /*
     * Don't allocate egress global lif if don't allocate global lif flag is set.
     */
    allocate_global_lif = !_SHR_IS_FLAG_SET(flags, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF);

    if (allocate_global_lif)
    {
        SHR_NULL_CHECK(global_lif, _SHR_E_INTERNAL, "global_lif");
    }

    if (allocate_global_lif)
    {
        /*
         * Take only the flags relevant for global lif allocation.
         */
        global_lif_allocation_flags |=
            (flags &
             (LIF_MNGR_GLOBAL_LIF_WITH_ID | LIF_MNGR_L2_GPORT_GLOBAL_LIF | LIF_MNGR_ONE_SIDED_SYMMETRIC_GLOBAL_LIF));

        /*
         * Set DNX_ALGO_LIF_INGRESS/EGRESS flags according to the required allocation.
         */
        global_lif_direction_flags = DNX_ALGO_GLOBAL_LIF_ALLOCATION_SET_DIRECTION_FLAGS(is_ingress, is_egress);

        /*
         * We use a temporary variable for global lif throughout the function for code simplicity.
         */
        global_lif_tmp = *global_lif;

        /*
         * Verify that all global lif flags are legal, and if allocation is WITH_ID, verify the ID as well.
         */
        SHR_INVOKE_VERIFY_DNX(dnx_algo_global_lif_allocation_verify
                              (unit, global_lif_allocation_flags, global_lif_direction_flags, global_lif_tmp));
    }

    /*
     * Step 1: Allocate lifs.
 */

    /*
     * Allocate local inlif.
     */
    if (is_ingress)
    {
        /*
         * Allocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_inlif_allocate(unit, inlif_info));
    }

    /*
     * Allocate local outlif.
     */
    if (is_egress)
    {
        /*
         * Allocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocate(unit, outlif_info, outlif_hw_info));
    }

    /*
     * Allocate global lif.
     */
    if (allocate_global_lif)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate(unit, global_lif_allocation_flags,
                                                                global_lif_direction_flags, &global_lif_tmp));

        *global_lif = global_lif_tmp;
    }
    else
    {
        /*
         * If no global lif was required, then if the user even passed a global lif pointer, fill it
         * with invalid indication.
         */
        if (global_lif)
        {
            *global_lif = LIF_MNGR_INVALID;
        }
    }

    /*
     * Step 2: Create lif mapping.
     */
    if (is_ingress && allocate_global_lif)
    {
        /*
         * Create ingress mapping.
         */
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = inlif_info->local_inlif;

        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, inlif_info->dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                       &local_lif_info.phy_table));

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create(unit, DNX_ALGO_LIF_INGRESS, global_lif_tmp, &local_lif_info));
    }

    if (is_egress && allocate_global_lif)
    {
        /*
         * Create egress mapping.
         * Don't do it if we didn't allocate egress global lif.
         */
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = outlif_info->local_outlif;
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create(unit, DNX_ALGO_LIF_EGRESS, global_lif_tmp, &local_lif_info));
    }

    /*
     * Step 3: Save LIF TABLE DBAL ID and type in SW state.
     */
    if (is_ingress)
    {
        if ((_SHR_IS_FLAG_SET(inlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE))
            && !(_SHR_IS_FLAG_SET(inlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE)))
        {
            /*
             * If reserving an entry, then the "active" result type is the old one.
             * Write it.
             */
            dbal_result_type = inlif_info->old_result_type;
        }
        else
        {
            dbal_result_type = inlif_info->dbal_result_type;
        }

        if (_SHR_IS_FLAG_SET(inlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE))
        {
            /*
             * If the replace flag is set, then remove the old result type before writing the new one.
             */
            SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_delete
                            (unit, inlif_info->core_id, inlif_info->local_inlif, inlif_info->dbal_table_id));
        }

        /*
         * Store info in SW state
         */
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_set
                        (unit, inlif_info->local_inlif, inlif_info->core_id, inlif_info->dbal_table_id,
                         dbal_result_type, inlif_info->fixed_entry_size));
    }
    if (is_egress)
    {
        if ((_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE))
            && !(_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE)))
        {
            /*
             * If reserving an entry, then the "active" result type is the alternative one.
             * Write it.
             */
            dbal_result_type = outlif_info->alternative_result_type;
        }
        else
        {
            dbal_result_type = outlif_info->dbal_result_type;
        }

        if (_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE))
        {
            /*
             * If the replace flag is set, then remove the old result type before writing the new one.
             */
            SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete
                            (unit, outlif_info->local_outlif));

        }

        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set
                        (unit, outlif_info, outlif_info->local_outlif, outlif_info->dbal_table_id, dbal_result_type,
                         outlif_info->fixed_entry_size));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_mngr_lif_free_verify(
    int unit,
    int global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    int is_ingress, is_egress, free_global_lif;
    uint32 global_lif_direction_flags;
    SHR_FUNC_INIT_VARS(unit);

    is_ingress = (inlif_info != NULL);
    is_egress = (outlif_info != NULL);
    free_global_lif = (global_lif != LIF_MNGR_INVALID);

    if (!is_ingress && !is_egress)
    {
        /*
         * A lif can be either an inlif or an outlif, or both (symmetric). If neither side is given as
         *   input, then the lif has no meaning.
         */
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "inlif_info is NULL and local_outlif is LIF_MNGR_INVALID. At least one must be valid.");
    }

    if (!free_global_lif && is_ingress && is_egress)
    {
        /*
         * If there's no global lif, then freeing both ingress and egress global lifs is meaningless,
         * since without global lifs they can't be symmetric. Instead, free them in two separate calls.
         */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "If don't free global lif was indicated, only one direction (ingress or egress) "
                     "can be used.");
    }

    /*
     * Verify global lif is it's in use.
     */
    if (free_global_lif)
    {
        /*
         * Set DNX_ALGO_LIF_INGRESS/EGRESS flags according to the required deallocation.
         */
        global_lif_direction_flags = DNX_ALGO_GLOBAL_LIF_ALLOCATION_SET_DIRECTION_FLAGS(is_ingress, is_egress);

        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_verify
                        (unit, DNX_ALGO_GLOBAL_LIF_ALLOCATION_DEALLOCATE_VERIFY, global_lif_direction_flags,
                         global_lif));
    }

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_local_inlif_info_verify(unit, FALSE, inlif_info));
    }

    if (is_egress)
    {
        LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, outlif_info->local_outlif);

        SHR_NULL_CHECK(outlif_hw_info, _SHR_E_INTERNAL, "outlif_hw_info");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_lif_free(
    int unit,
    int global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    int is_ingress, is_egress, free_global_lif;
    uint32 global_lif_direction_flags;
    SHR_FUNC_INIT_VARS(unit);

    is_ingress = (inlif_info != NULL);
    is_egress = (outlif_info != NULL);
    free_global_lif = (global_lif != LIF_MNGR_INVALID);

    /*
     * 0. Verify lif information.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_lif_mngr_lif_free_verify(unit, global_lif, inlif_info, outlif_info, outlif_hw_info));

    /*
     * Steps 1,2: Dellocate lifs, and remove mapping.
     */

    /*
     * Free global lif is it's in use.
     */
    if (free_global_lif)
    {
        /*
         * Set DNX_ALGO_LIF_INGRESS/EGRESS flags according to the required deallocation.
         */
        global_lif_direction_flags = DNX_ALGO_GLOBAL_LIF_ALLOCATION_SET_DIRECTION_FLAGS(is_ingress, is_egress);

        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, global_lif_direction_flags, global_lif));
    }

    /*
     * Deallocate and unmap inlif.
     */
    if (is_ingress)
    {
        /*
         * Deallocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_inlif_free(unit, inlif_info));

        if (free_global_lif)
        {
            /*
             * Remove ingress mapping.
             */
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, global_lif));
        }
    }

    /*
     * Deallocate and unmap outlif.
     */
    if (is_egress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                        (unit, outlif_info->local_outlif, &outlif_info->dbal_table_id,
                         &outlif_info->dbal_result_type, &outlif_info->outlif_phase, NULL, NULL, NULL));

        /*
         * Deallocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_free(unit, outlif_info, outlif_hw_info));
        /*
         * Remove egress mapping, only if the global lif exists.
         */
        if (free_global_lif)
        {
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, global_lif));
        }
    }

    /*
     * Delete from SW state LIF to DBAL table id and type mapping.
     */
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_delete
                        (unit, inlif_info->core_id, inlif_info->local_inlif, inlif_info->dbal_table_id));
    }

    if (is_egress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(unit, outlif_info->local_outlif));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mngr_nof_glem_lines_get(
    int unit,
    int *nof_glem_lines)
{
    int nof_phy_glem_1_lines, nof_phy_glem_2_lines;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_glem_lines, _SHR_E_INTERNAL, "nof_glem_lines");

    if (dnx_data_lif.global_lif.use_mdb_size_get(unit))
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_GLEM_1, &nof_phy_glem_1_lines));
        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_GLEM_2, &nof_phy_glem_2_lines));
        *nof_glem_lines = UTILEX_MIN(nof_phy_glem_1_lines, nof_phy_glem_2_lines);
    }
    else
    {
        *nof_glem_lines = DNX_ALGO_LIF_MNGR_REDUCED_MODE_NOF_LIFS;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mngr_nof_local_outlifs_get(
    int unit,
    int *nof_local_outlifs)
{
    int phy_eedb_max_capacity;
    dbal_physical_tables_e dbal_physical_table_id;
    uint32 max_entries_per_bank = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_local_outlifs, _SHR_E_INTERNAL, "nof_local_outlifs");

    /*
     * Calculate the maximum number of outlifs stored in EEDB banks
     * Find the phase with smallest granularity, max entries per bank
     * Multiply number of banks by max entries per bank
     */
    for (dbal_physical_table_id = DBAL_PHYSICAL_TABLE_EEDB_1; dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8;
         dbal_physical_table_id++)
    {
        uint32 phase_entries_per_bank;

        SHR_IF_ERR_EXIT(mdb_eedb_table_nof_entries_per_cluster_type_get
                        (unit, dbal_physical_table_id, MDB_EEDB_BANK, &phase_entries_per_bank));

        if (phase_entries_per_bank > max_entries_per_bank)
        {
            max_entries_per_bank = phase_entries_per_bank;
        }
    }
    *nof_local_outlifs = dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_clusters * max_entries_per_bank;

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_EEDB_1, &phy_eedb_max_capacity));
    *nof_local_outlifs += phy_eedb_max_capacity;
    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_EEDB_3, &phy_eedb_max_capacity));
    *nof_local_outlifs += phy_eedb_max_capacity;
    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_EEDB_5, &phy_eedb_max_capacity));
    *nof_local_outlifs += phy_eedb_max_capacity;
    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_EEDB_7, &phy_eedb_max_capacity));
    *nof_local_outlifs += phy_eedb_max_capacity;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mngr_nof_inlifs_get(
    int unit,
    int *nof_inlif1_lifs,
    int *nof_inlif2_lifs)
{
    int use_mdb_size;
    SHR_FUNC_INIT_VARS(unit);

    use_mdb_size = dnx_data_lif.global_lif.use_mdb_size_get(unit);
    if (nof_inlif1_lifs)
    {
        if (use_mdb_size)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_INLIF_1, nof_inlif1_lifs));
        }
        else
        {
            *nof_inlif1_lifs = DNX_ALGO_LIF_MNGR_REDUCED_MODE_NOF_LIFS;
        }
    }

    if (nof_inlif2_lifs)
    {
        if (use_mdb_size)
        {
            int nof_inlif3_lifs;
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_INLIF_2, nof_inlif2_lifs));
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_INLIF_3, &nof_inlif3_lifs));
            *nof_inlif2_lifs = UTILEX_MIN(*nof_inlif2_lifs, nof_inlif3_lifs);
        }
        else
        {
            *nof_inlif2_lifs = DNX_ALGO_LIF_MNGR_REDUCED_MODE_NOF_LIFS;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t inlif_data, outlif_data;
    uint8 is_init;

    uint8 is_std_1 = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&inlif_data, 0, sizeof(dnx_algo_res_create_data_t));
    sal_memset(&outlif_data, 0, sizeof(dnx_algo_res_create_data_t));

    /*
     * Initialize lif mngr sw state.
     */
    SHR_IF_ERR_EXIT(lif_mngr_db.is_init(unit, &is_init));

    if (!is_init)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.init(unit));
    }

    /*
     * Initialize the lif mapping module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_init(unit));

    /*
     * Initialize the local inlif allocation module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_inlif_allocation_init(unit));

    /*
     * Initialize the local outlif allocation module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocation_init(unit));

    /*
     * Initialize the global lif allocation module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_init(unit));

    /*
     * Initialize the LIF Table Manager module.
     */
    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
    if (is_std_1)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lif_table_mngr_init(unit));
    }

    /*
     * Check that GLEM is allocated
     */
    {
        uint8 is_glem1_allocated, is_glem2_allocated;

        SHR_IF_ERR_EXIT(mdb_table_is_allocated_get(unit, DBAL_PHYSICAL_TABLE_GLEM_1, &is_glem1_allocated));
        SHR_IF_ERR_EXIT(mdb_table_is_allocated_get(unit, DBAL_PHYSICAL_TABLE_GLEM_2, &is_glem2_allocated));
        if (!is_glem1_allocated && !is_glem2_allocated)
        {
            
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Removing GLEM resources is currently not supported. Please make sure MDB profile contains GLEM.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mngr_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deinitialize the local outlif allocation module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocation_deinit(unit));

    /*
     * Deinitialize the lif mapping module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_deinit(unit));

    /*
     * sw state module deinit is done automatically at device deinit
     */

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
