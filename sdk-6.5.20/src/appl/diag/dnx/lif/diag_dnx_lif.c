/** \file diag_dnx_lif.c
 *
 * Diagnostics for the lif.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*************
 * INCLUDES  *
 */
/*shared*/
#include <shared/bsl.h>
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/shell.h>
/*bcm*/
#include <bcm_int/dnx/algo/algo_gpm.h>
/*soc*/
#include <soc/dnx/dbal/dbal_string_mgmt.h>
/*sal*/
#include <sal/appl/sal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <shared/bslnames.h>
#include <appl/diag/bslenable.h>
#include <src/soc/dnx/dbal/dbal_internal.h>

/*************
 *  DEFINES  *
 */

#define CHAR_BUFF_SIZE 500
#define DIAG_LIF_MAX_NUM_RESULT_TYPE 30

/*************
 *  MACROES  *
 */

/*************
 * TYPEDEFS  *
 */
extern const char *strcaseindex(
    const char *s,
    const char *sub);

typedef struct alloc_sim_prop_s
{
    dbal_tables_e table_id;
    int res_type;
    lif_mngr_outlif_phase_e phase_id;
    uint8 with_ll;
    uint32 num_requested_allocs;
    int alloc_count;
} alloc_sim_prop_t;

/*************
 * FUNCTIONS *
 */

shr_error_e
dnx_lif_diag_logger_close(
    int unit,
    bsl_severity_t * orig_severity)
{
    SHR_FUNC_INIT_VARS(unit);

    *orig_severity = bslenable_get(bslLayerBcmdnx, bslSourceLif);
    bslenable_set(bslLayerBcmdnx, bslSourceLif, bslSeverityOff);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_diag_logger_restore(
    int unit,
    bsl_severity_t orig_severity)
{
    SHR_FUNC_INIT_VARS(unit);

    bslenable_set(bslLayerBcmdnx, bslSourceLif, orig_severity);

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_outlif_allocation_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int outlif_bank, nof_outlifs_per_entry, nof_eedb_banks_per_outlif_bank, current_eedb_bank_array_index,
        nof_free_elements;
    uint8 logical_phase, ll_index, address_granularity, assigned, in_use, ll_in_use, current_eedb_bank;
    uint32 nof_outlif_banks, free_cluster_outlif_banks = 0, not_assigned = 0, free_eedb_outlif_banks = 0;
    char eedb_banks[256];
    size_t strn_size = 256;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_outlif_banks = DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit);

    PRT_TITLE_SET("Outlif banks summary");

    PRT_COLUMN_ADD("Outlif bank");
    PRT_COLUMN_ADD("Free entries/total entries");
    PRT_COLUMN_ADD("HW association");
    PRT_COLUMN_ADD("Logical phase");
    PRT_COLUMN_ADD("LL in use");
    PRT_COLUMN_ADD("EEDB bank LL index");
    PRT_COLUMN_ADD("Granularity");
    PRT_COLUMN_ADD("EEDB banks used for data");

    for (outlif_bank = 0; outlif_bank < nof_outlif_banks; outlif_bank++)
    {
        /**Bank/cluster*/
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.assigned.get(unit, outlif_bank, &assigned));

        /**LL in use*/
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.in_use.get(unit, outlif_bank, &in_use));

        if (!in_use)
        {
            if (assigned == DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB)
            {
                free_cluster_outlif_banks++;
            }
            else if (assigned == DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB)
            {
                free_eedb_outlif_banks++;
            }
            else
            {
                not_assigned++;
            }
            continue;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%d", outlif_bank);

        /**Free entries/Total entries*/
        SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.nof_free_elements_get(unit, outlif_bank, &nof_free_elements));
        PRT_CELL_SET("%d/%d", nof_free_elements, DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit));

        PRT_CELL_SET("%s", (assigned == DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB) ? "Cluster" : "EEDB bank");

        /**Logical phase*/
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.used_logical_phase.get(unit, outlif_bank, &logical_phase));
        PRT_CELL_SET("%d", (logical_phase + 1));

        /**LL in use*/
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_in_use.get(unit, outlif_bank, &ll_in_use));
        PRT_CELL_SET("%s", (ll_in_use) ? "TRUE" : "FALSE");

        /**LL index*/
        if (ll_in_use)
        {
            ll_index = 0;
            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_index.get(unit, outlif_bank, &ll_index));
            PRT_CELL_SET("%d", ll_index);
        }
        else
        {
            PRT_CELL_SET("NA");
        }

        /**Granularity*/
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        address_granularity.get(unit, logical_phase, &address_granularity));
        PRT_CELL_SET("%d", address_granularity);

        sal_snprintf(eedb_banks, strn_size, "{ ");
        nof_outlifs_per_entry = EEDB_MAX_DATA_ENTRY_SIZE / address_granularity;
        nof_eedb_banks_per_outlif_bank =
            DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) /
            (dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows * nof_outlifs_per_entry);

        if (assigned == DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB)
        {
            for (current_eedb_bank_array_index = 0; current_eedb_bank_array_index < nof_eedb_banks_per_outlif_bank;
                 current_eedb_bank_array_index++)
            {
                SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.
                                eedb_data_banks.get(unit, outlif_bank, current_eedb_bank_array_index,
                                                    &current_eedb_bank));

                if (current_eedb_bank != (uint8) DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
                {
                    sal_snprintf(eedb_banks, strn_size, "%s%d ", eedb_banks, current_eedb_bank);
                }
            }
            sal_snprintf(eedb_banks, strn_size, "%s}", eedb_banks);
            PRT_CELL_SET("%s", eedb_banks);
        }
        else
        {
            PRT_CELL_SET("NA");
        }
    }

    PRT_INFO_ADD("Total number of free cluster outlif banks: %d", free_cluster_outlif_banks);
    PRT_INFO_ADD("Total number of free EEDB outlif banks: %d", free_eedb_outlif_banks);
    PRT_INFO_ADD("Total number of not assigned EEDB outlif banks: %d", not_assigned);

    PRT_COMMITX;
exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_outlif_allocation_phase_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 ll_in_use, ll_index, address_granularity, current_eedb_bank, first_eedb_bank, last_eedb_bank,
        eedb_bank_is_used, bank_logical_phase, current_bank, in_use;
    uint32 phase_id, first_bank_for_phase = 0, last_bank_for_phase = 0;
    int nof_free_elements, nof_eedb_banks_per_outlif_bank, current_eedb_bank_array_index, nof_outlifs_per_entry,
        used_eedb_banks_idx = 0, j = 0, i;
    sw_state_ll_node_t current_node;
    char taken_eedb_banks[256], eedb_banks[256], taken_cluster_banks[256];
    size_t strn_size = 256;
    int used_eedb_banks[256] = { -1 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("phase", phase_id);

    PRT_TITLE_SET("Phase %d - outlif banks summary", phase_id);
    phase_id--;
    PRT_COLUMN_ADD("Outlif bank");
    PRT_COLUMN_ADD("Free entries/total entries in bank");
    PRT_COLUMN_ADD("HW association");
    PRT_COLUMN_ADD("LL in use");
    PRT_COLUMN_ADD("EEDB bank index for LL");
    PRT_COLUMN_ADD("Granularity");
    PRT_COLUMN_ADD("EEDB banks used for data");

    /*
     * Iterate over the banks.
     */
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_bank.get(unit, phase_id, &first_bank_for_phase));
    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_bank.get(unit, phase_id, &last_bank_for_phase));

    /**Iterate over cluster outlif banks*/
    sal_snprintf(taken_cluster_banks, strn_size, "{ ");
    for (current_bank = first_bank_for_phase; current_bank <= last_bank_for_phase; current_bank++)
    {
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.in_use.get(unit, current_bank, &in_use));
        if (!in_use)
        {
            continue;
        }

        /*
         * If bank is already in use, check that it's used by the correct logical phase.
         */
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.
                        used_logical_phase.get(unit, current_bank, &bank_logical_phase));

        if (bank_logical_phase != phase_id)
        {
            sal_snprintf(taken_cluster_banks, strn_size, "%s%d ", taken_cluster_banks, current_bank);
            continue;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%d", current_bank);

        /**Free entries/Total entries*/
        SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.nof_free_elements_get(unit, current_bank, &nof_free_elements));
        PRT_CELL_SET("%d/%d", nof_free_elements, DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit));

        PRT_CELL_SET("Cluster");

        /**LL in use*/
        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_in_use.get(unit, current_bank, &ll_in_use));
        PRT_CELL_SET("%s", (ll_in_use) ? "TRUE" : "FALSE");

        /**LL index*/
        if (ll_in_use)
        {
            ll_index = 0;
            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_index.get(unit, current_bank, &ll_index));
            PRT_CELL_SET("%d", ll_index);
            used_eedb_banks[used_eedb_banks_idx++] = ll_index;
            used_eedb_banks[used_eedb_banks_idx] = -1;
        }
        else
        {
            PRT_CELL_SET("NA");
        }

        /**Granularity*/
        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                        address_granularity.get(unit, phase_id, &address_granularity));
        PRT_CELL_SET("%d", address_granularity);
    }
    sal_snprintf(taken_cluster_banks, strn_size, "%s}", taken_cluster_banks);

    /**Get first EEDB outlif bank*/
    SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                    used_data_bank_per_logical_phase.get_first(unit, phase_id, &current_node));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(current_node))
    {
        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                        used_data_bank_per_logical_phase.node_value(unit, current_node, &current_bank));

        SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.
                        used_logical_phase.get(unit, current_bank, &bank_logical_phase));

        if (bank_logical_phase == phase_id)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%d", current_bank);

            /**Free entries/Total entries*/
            SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.nof_free_elements_get(unit, current_bank, &nof_free_elements));
            PRT_CELL_SET("%d/%d", nof_free_elements, DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit));

            PRT_CELL_SET("EEDB bank");

            /**LL in use*/
            SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_in_use.get(unit, current_bank, &ll_in_use));
            PRT_CELL_SET("%s", (ll_in_use) ? "TRUE" : "FALSE");

            /**LL index*/
            if (ll_in_use)
            {
                ll_index = 0;
                SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.ll_index.get(unit, current_bank, &ll_index));
                PRT_CELL_SET("%d", ll_index);
                used_eedb_banks[used_eedb_banks_idx++] = ll_index;
                used_eedb_banks[used_eedb_banks_idx] = -1;
            }
            else
            {
                PRT_CELL_SET("NA");
            }

            /**Granularity*/
            SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                            address_granularity.get(unit, phase_id, &address_granularity));
            PRT_CELL_SET("%d", address_granularity);

            sal_snprintf(eedb_banks, strn_size, "{ ");
            nof_outlifs_per_entry = EEDB_MAX_DATA_ENTRY_SIZE / address_granularity;
            nof_eedb_banks_per_outlif_bank =
                DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) /
                (dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows * nof_outlifs_per_entry);

            for (current_eedb_bank_array_index = 0; current_eedb_bank_array_index < nof_eedb_banks_per_outlif_bank;
                 current_eedb_bank_array_index++)
            {
                SHR_IF_ERR_EXIT(local_outlif_info.outlif_bank_info.
                                eedb_data_banks.get(unit, current_bank, current_eedb_bank_array_index,
                                                    &current_eedb_bank));

                if (current_eedb_bank != (uint8) DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK)
                {
                    used_eedb_banks[used_eedb_banks_idx++] = current_eedb_bank;
                    used_eedb_banks[used_eedb_banks_idx] = -1;
                    sal_snprintf(eedb_banks, strn_size, "%s%d ", eedb_banks, current_eedb_bank);
                }
            }
            sal_snprintf(eedb_banks, strn_size, "%s}", eedb_banks);
            PRT_CELL_SET("%s", eedb_banks);
        }
        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.
                        used_data_bank_per_logical_phase.next_node(unit, phase_id, current_node, &current_node));
    }

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.first_ll_bank.get(unit, phase_id, &first_eedb_bank));

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.last_ll_bank.get(unit, phase_id, &last_eedb_bank));

    sal_snprintf(taken_eedb_banks, strn_size, "{ ");
    for (i = first_eedb_bank; i < last_eedb_bank; i++)
    {
        SHR_IF_ERR_EXIT(local_outlif_info.eedb_banks_info.bank_occupation_bitmap.bit_get(unit, i, &eedb_bank_is_used));
        if (!eedb_bank_is_used)
        {
            continue;
        }

        j = 0;
        while (used_eedb_banks[j] != -1)
        {
            if (used_eedb_banks[j] == i)
            {
                break;
            }
            j++;
        }
        /**Bank is used by another phase*/
        if (used_eedb_banks[j] == -1)
        {
            sal_snprintf(taken_eedb_banks, strn_size, "%s%d ", taken_eedb_banks, i);
        }
    }
    sal_snprintf(taken_eedb_banks, strn_size, "%s}", taken_eedb_banks);

    PRT_INFO_ADD("EEDB banks mapped available to phase %d: {%d-%d}", (phase_id + 1), first_eedb_bank, last_eedb_bank);
    PRT_INFO_ADD("EEDB banks in range taken by other phases: %s", taken_eedb_banks);
    PRT_INFO_ADD("Cluster mapped outlif banks available to phase %d: {%d-%d}", (phase_id + 1), first_bank_for_phase,
                 last_bank_for_phase);
    PRT_INFO_ADD("Cluster mapped outlif banks taken by other phases: %s", taken_cluster_banks);

    PRT_COMMITX;
exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_outlif_allocation_phase_cnt_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 phase_idx, table_idx;
    dbal_tables_e table_id;
    uint32 max_nof_result_types = DIAG_LIF_MAX_NUM_RESULT_TYPE;
    int phase_iter, table_iter, res_type_iter, phase_id;
    char *table_name;
    dbal_enum_value_field_mdb_eedb_phase_e phase;
    int phase_iter_start = 0, phase_iter_end = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT - 1;
    int table_iter_start = 0, table_iter_end = DBAL_NOF_TABLES - 1;
    uint32 ***counter_array = NULL;
    SW_STATE_HASH_TABLE_ITER iter;
    egress_lif_info_t egress_lif_info;
    int local_outlif_id = 0;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    counter_array =
        (uint32 ***) sal_alloc(DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT * sizeof(uint32 **), "counter array");

    for (phase_idx = 0; phase_idx < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; phase_idx++)
    {
        counter_array[phase_idx] = (uint32 **) sal_alloc((DBAL_NOF_TABLES) * sizeof(uint32 *), "counter array");
        for (table_idx = 0; table_idx < DBAL_NOF_TABLES; table_idx++)
        {
            counter_array[phase_idx][table_idx] =
                (uint32 *) sal_alloc(DIAG_LIF_MAX_NUM_RESULT_TYPE * sizeof(uint32), "counter array");
            sal_memset(counter_array[phase_idx][table_idx], 0, sizeof(uint32) * DIAG_LIF_MAX_NUM_RESULT_TYPE);
        }
    }

    SH_SAND_GET_ENUM("phase", phase);
    SH_SAND_GET_STR("table", table_name);

    PRT_TITLE_SET("local outlifs counter status");
    PRT_COLUMN_ADD("Logical phase");
    PRT_COLUMN_ADD("DBAL Table");
    PRT_COLUMN_ADD("Result type");
    PRT_COLUMN_ADD("Num of entries");

    /**Iterate over all the local Out-LIFs and fill the counters 
      * table */
    DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_START(&iter);
    DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_NEXT(unit, &iter, &local_outlif_id, &egress_lif_info);

    while (!DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_IS_END(&iter))
    {
        if ((egress_lif_info.dbal_table_id > DBAL_NOF_TABLES)
            || (egress_lif_info.dbal_result_type > max_nof_result_types)
            || (egress_lif_info.logical_phase > DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid value received");
        }

        counter_array[egress_lif_info.logical_phase][egress_lif_info.dbal_table_id][egress_lif_info.dbal_result_type]++;
        DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_NEXT(unit, &iter, &local_outlif_id, &egress_lif_info);
    }

    if (!ISEMPTY(table_name))
    {
        SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));
        if (table_id > DBAL_NOF_TABLES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal dbal table %s", table_name);
        }
        table_iter_start = table_id;
        table_iter_end = table_id;
    }

    phase_id = phase;
    if ((phase_id > DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT)
        || (phase_id < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal phase %d", phase_id);
    }
    if (phase_id != DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
    {
        phase_iter_start = phase_id;
        phase_iter_end = phase_id;
    }

    /**Now - iterate over the array and fill the diag table*/
    for (phase_iter = phase_iter_start; phase_iter <= phase_iter_end; phase_iter++)
    {
        for (table_iter = table_iter_start; table_iter <= table_iter_end; table_iter++)
        {
            for (res_type_iter = 0; res_type_iter < max_nof_result_types; res_type_iter++)
            {
                if (counter_array[phase_iter][table_iter][res_type_iter] == 0)
                {
                    continue;
                }
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                /**outlif phase*/
                PRT_CELL_SET("%d", phase_iter + 1);
                /**dbal table*/
                PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, table_iter));
                /**dbal result type*/
                PRT_CELL_SET("%s", dbal_result_type_to_string(unit, table_iter, res_type_iter));
                /**counter_value*/
                PRT_CELL_SET("%d", counter_array[phase_iter][table_iter][res_type_iter]);
            }
        }
    }

    PRT_COMMITX;
exit:

    for (phase_idx = 0; phase_idx < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; phase_idx++)
    {
        for (table_idx = 0; table_idx < DBAL_NOF_TABLES; table_idx++)
        {
            sal_free(counter_array[phase_idx][table_idx]);
        }
        sal_free(counter_array[phase_idx]);
    }
    sal_free(counter_array);

    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_outlif_allocation_validate_ll(
    int unit,
    alloc_sim_prop_t * alloc_sim_prop)
{
    dbal_to_phase_info_t dbal_to_phase_info;
    uint8 found;
    dnx_local_outlif_ll_indication_e ll_indication;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&dbal_to_phase_info, 0, sizeof(dbal_to_phase_info));
    dbal_to_phase_info.dbal_result_type = alloc_sim_prop->res_type;
    dbal_to_phase_info.dbal_table = alloc_sim_prop->table_id;

    if ((alloc_sim_prop->phase_id < 1) || (alloc_sim_prop->phase_id > 8))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal phase %d given", alloc_sim_prop->phase_id);
    }

    SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                    dbal_valid_combinations.find(unit, alloc_sim_prop->phase_id, &dbal_to_phase_info, &ll_indication,
                                                 &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal combination: %s, %s, %d",
                     dbal_logical_table_to_string(unit, alloc_sim_prop->table_id), dbal_result_type_to_string(unit,
                                                                                                              alloc_sim_prop->table_id,
                                                                                                              alloc_sim_prop->res_type),
                     alloc_sim_prop->phase_id + 1);
    }

    if (((ll_indication == DNX_LOCAL_OUTLIF_LL_ALWAYS) && (!alloc_sim_prop->with_ll))
        || ((ll_indication == DNX_LOCAL_OUTLIF_LL_NEVER) && (alloc_sim_prop->with_ll)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "LL mismatch for combination %s, %s, %d",
                     dbal_logical_table_to_string(unit, alloc_sim_prop->table_id), dbal_result_type_to_string(unit,
                                                                                                              alloc_sim_prop->table_id,
                                                                                                              alloc_sim_prop->res_type),
                     alloc_sim_prop->phase_id + 1);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_outlif_allocation_simulate_alloc(
    int unit,
    alloc_sim_prop_t alloc_sim_prop[],
    int num_requested_comb,
    int *success)
{
    lif_mngr_local_outlif_info_t outlif_info = { 0 };
    uint32 comb_idx;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = _SHR_E_NONE;

    if (success)
    {
        *success = TRUE;
    }

    /**Save current status in journal*/
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));
    outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_UPDATE_HW;

    /**Allocation*/
    for (comb_idx = 0; comb_idx < num_requested_comb; comb_idx++)
    {
        outlif_info.dbal_table_id = alloc_sim_prop[comb_idx].table_id;
        outlif_info.dbal_result_type = alloc_sim_prop[comb_idx].res_type;
        outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + alloc_sim_prop[comb_idx].phase_id;

        alloc_sim_prop[comb_idx].alloc_count = 0;
        while (rv == _SHR_E_NONE)
        {
            rv = dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info);
            if (rv != _SHR_E_NONE)
            {
                break;
            }
            alloc_sim_prop[comb_idx].alloc_count += 1;
            /**If we reached the requested allocations, break*/
            if ((alloc_sim_prop[comb_idx].alloc_count >= alloc_sim_prop[comb_idx].num_requested_allocs)
                && (alloc_sim_prop[comb_idx].num_requested_allocs))
            {
                break;
            }
        }
        if (success)
        {
            if ((alloc_sim_prop[comb_idx].num_requested_allocs)
                && (alloc_sim_prop[comb_idx].alloc_count < alloc_sim_prop[comb_idx].num_requested_allocs))
            {
                *success = FALSE;
            }
        }
    }

exit:
    /*
     * rollback modifications done through journal
     */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));
    SHR_FUNC_EXIT;
}

cmd_result_t
dnx_outlif_eedb_dbal_table_string_to_next_table_id_get(
    int unit,
    char *substr_match,
    dbal_tables_e curr_table_id,
    dbal_tables_e * table_id,
    int *is_full_match)
{
    int iter;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    *table_id = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);
    *is_full_match = 0;

    curr_table_id++;

    if ((substr_match == NULL) || (curr_table_id > dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input substr_match not exists or table_id too big\n");
    }

    for (iter = curr_table_id; iter < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); iter++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, iter, &table));

        /**Iterate only over eedb tables*/
        if (!dbal_table_is_out_lif(table))
        {
            continue;
        }

        if (!sal_strcasecmp(substr_match, table->table_name))
        {
            *is_full_match = 1;
            *table_id = iter;
            break;
        }

        if (strcaseindex(table->table_name, substr_match))
        {
            *table_id = iter;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**Get dbal table from provided string.
 * In case the string is a partial match and there're choises - display all options*/
static shr_error_e
dnx_outlif_eedb_dbal_table_from_string(
    int unit,
    char *table_name,
    dbal_tables_e * table_id,
    sh_sand_control_t * sand_control)
{
    int is_full_match = 0;
    dbal_tables_e first_table_id, iter_table_id = DBAL_TABLE_EMPTY;
    int nof_tables_found = 0;
    CONST dbal_logical_table_t *table;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Possible EEDB tables");

    PRT_COLUMN_ADD("Table Name");

    /**Get first possible match*/
    if (table_name)
    {
        dnx_outlif_eedb_dbal_table_string_to_next_table_id_get(unit, table_name, -1, table_id, &is_full_match);
    }

    /**No matching tables found or no table given - display all eedb tables*/
    if (((*table_id) == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit)) || (table_name == NULL))
    {
        while (iter_table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
        {
            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, *table_id, &table));
            /**Iterate only over eedb tables*/
            if (!dbal_table_is_out_lif(table))
            {
                iter_table_id++;
                continue;
            }
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, iter_table_id++));
        }
        SHR_EXIT();
    }

    /**Table found. now - iterate to find full match. If no full match and there's more than one option - display all possible options*/
    first_table_id = (*table_id);

    while ((*table_id) != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        nof_tables_found++;
                /** full match - use this table */
        if (is_full_match)
        {
            SHR_EXIT();
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, (*table_id)));
        dnx_outlif_eedb_dbal_table_string_to_next_table_id_get(unit, table_name, (*table_id), table_id, &is_full_match);
    }

        /** only one table found use it */
    if (nof_tables_found == 1)
    {
        (*table_id) = first_table_id;
        SHR_EXIT();
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_outlif_allocation_get_combinations(
    int unit,
    alloc_sim_prop_t alloc_sim_prop[],
    int *num_combinations)
{
    uint32 entry_handle_id;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *num_combinations = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_OUT_LIF_SIMULATION, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME,
                         (uint32 *) &alloc_sim_prop[*num_combinations].table_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE,
                         (uint8 *) &alloc_sim_prop[*num_combinations].res_type));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOGICAL_PHASE,
                         (uint32 *) &alloc_sim_prop[*num_combinations].phase_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_LL_INDICATION, &alloc_sim_prop[*num_combinations].with_ll));
        SHR_IF_ERR_EXIT(dnx_outlif_allocation_validate_ll(unit, &alloc_sim_prop[*num_combinations]));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NOF_REQUESTED_LIFS, INST_SINGLE,
                         &alloc_sim_prop[*num_combinations].num_requested_allocs));
        *num_combinations += 1;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**Get result type from string. Expected a full match. In case of no full match - display all possible result types for the given table*/
static shr_error_e
dnx_outlif_eedb_dbal_result_type_from_string(
    int unit,
    dbal_tables_e table_id,
    char *result_type_name,
    uint32 *res_type,
    sh_sand_control_t * sand_control)
{
    int rv, res_type_index;
    CONST dbal_logical_table_t *table;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Possible result types for table %s", dbal_logical_table_to_string(unit, table_id));

    PRT_COLUMN_ADD("result type Name");

    rv = dbal_result_type_string_to_id_no_err(unit, table_id, result_type_name, res_type);

    /**Result type found or another error*/
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }

    SHR_SET_CURRENT_ERR(rv);
    /**Result type not found - display all possible result types*/
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", table->multi_res_info[res_type_index].result_type_name);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**Display requested alloc combinations table*/
static shr_error_e
dnx_outlif_allocation_alloc_combinations_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id, res_type, nof_requested_lifs;
    int ll_exists, is_end;
    dbal_enum_value_field_mdb_eedb_phase_e phase;
    dbal_tables_e table_id;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Requested alloc combinations status");
    PRT_COLUMN_ADD("App DB");
    PRT_COLUMN_ADD("Result type");
    PRT_COLUMN_ADD("Logical phase");
    PRT_COLUMN_ADD("LL exists");
    PRT_COLUMN_ADD("nof requested lifs");

    /**Iterate over all lifs and fill the counters table*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_OUT_LIF_SIMULATION, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    if (is_end)
    {
        sal_printf("No current combinations\n");
        SHR_EXIT();
    }

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME, (uint32 *) &table_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, &res_type));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOGICAL_PHASE, (uint32 *) &phase));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_LL_INDICATION, (uint8 *) &ll_exists));
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NOF_REQUESTED_LIFS, INST_SINGLE,
                                            &nof_requested_lifs);
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        /**App DB*/
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, table_id));
        /**Result type*/
        PRT_CELL_SET("%s", dbal_result_type_to_string(unit, table_id, res_type));
        /**Logical phase*/
        PRT_CELL_SET("%d", phase + 1);
        /**ll exists*/
        PRT_CELL_SET("%s", (ll_exists) ? "YES" : "NO");
        /**requested lifs*/
        PRT_CELL_SET("%d", nof_requested_lifs);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**Add a new entry to the alloc combinations table*/
static shr_error_e
dnx_outlif_allocation_alloc_combinations_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id, num_lifs, res_type;
    char *table_name;
    char *result_type_name;
    int ll_exists, rv;
    dbal_enum_value_field_mdb_eedb_phase_e phase;
    dbal_tables_e table_id;
    alloc_sim_prop_t alloc_sim_prop = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("ll_exists", ll_exists);
    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_STR("result_type", result_type_name);
    SH_SAND_GET_ENUM("phase", phase);
    SH_SAND_GET_UINT32("num_lifs", num_lifs);

    SHR_IF_ERR_EXIT(dnx_outlif_eedb_dbal_table_from_string(unit, table_name, &table_id, sand_control));
    if (ISEMPTY(table_name) || (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit)))
    {
        SHR_EXIT();
    }

    rv = dnx_outlif_eedb_dbal_result_type_from_string(unit, table_id, result_type_name, &res_type, sand_control);
    if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }

    alloc_sim_prop.phase_id = phase;
    alloc_sim_prop.table_id = table_id;
    alloc_sim_prop.res_type = res_type;
    alloc_sim_prop.with_ll = ll_exists;
    SHR_IF_ERR_EXIT(dnx_outlif_allocation_validate_ll(unit, &alloc_sim_prop));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_OUT_LIF_SIMULATION, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME, table_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, res_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PHASE, phase);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LL_INDICATION, ll_exists);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_REQUESTED_LIFS, INST_SINGLE, num_lifs);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**Get an entry from the alloc combinations table*/
static shr_error_e
dnx_outlif_allocation_alloc_combinations_get_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    char *table_name;
    char *result_type_name;
    int ll_exists, rv, res_type;
    dbal_enum_value_field_mdb_eedb_phase_e phase;
    dbal_tables_e table_id;
    uint32 nof_requested_lifs;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("ll_exists", ll_exists);
    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_STR("result_type", result_type_name);
    SH_SAND_GET_ENUM("phase", phase);

    if (ISEMPTY(table_name) || ISEMPTY(result_type_name))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dbal table and result type must be given\n");
    }

    SHR_IF_ERR_EXIT(dnx_outlif_eedb_dbal_table_from_string(unit, table_name, &table_id, sand_control));
    if (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_EXIT();
    }

    rv = dnx_outlif_eedb_dbal_result_type_from_string(unit, table_id, result_type_name, (uint32 *) &res_type,
                                                      sand_control);
    if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_OUT_LIF_SIMULATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME, table_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, res_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PHASE, phase);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LL_INDICATION, ll_exists);

    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(rv, "Non-existing entry");
    }
    else if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_NOF_REQUESTED_LIFS,
                                                        INST_SINGLE, (uint32 *) &nof_requested_lifs));

    sal_printf("Allocation simulation: Nof requested lifs for requested combination: %d\n", nof_requested_lifs);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**Clear all of the alloc combinations table*/
static shr_error_e
dnx_outlif_allocation_alloc_combinations_clear_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_LOCAL_OUT_LIF_SIMULATION));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**Remove a specific entry from the alloc combinations table*/
static shr_error_e
dnx_outlif_allocation_alloc_combinations_delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    char *table_name;
    char *result_type_name;
    int ll_exists, rv, res_type;
    dbal_enum_value_field_mdb_eedb_phase_e phase;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("ll_exists", ll_exists);
    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_STR("result_type", result_type_name);
    SH_SAND_GET_ENUM("phase", phase);

    if (ISEMPTY(table_name) || ISEMPTY(result_type_name))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dbal table and result type must be given\n");
    }

    SHR_IF_ERR_EXIT(dnx_outlif_eedb_dbal_table_from_string(unit, table_name, &table_id, sand_control));
    if (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_EXIT();
    }

    rv = dnx_outlif_eedb_dbal_result_type_from_string(unit, table_id, result_type_name, (uint32 *) &res_type,
                                                      sand_control);
    if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_OUT_LIF_SIMULATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME, table_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, res_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PHASE, phase);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LL_INDICATION, ll_exists);

    rv = dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry not found");
    }

    SHR_SET_CURRENT_ERR(rv);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**Run the alloc combinations simulation*/
static shr_error_e
dnx_outlif_allocation_alloc_combinations_run_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    alloc_sim_prop_t alloc_sim_prop[100];
    int num_combinations, success, idx;
    bsl_severity_t severity = bslSeverityOff;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /**Don't show error messages*/
    SHR_IF_ERR_EXIT(dnx_lif_diag_logger_close(unit, &severity));

    SHR_IF_ERR_EXIT(dnx_outlif_allocation_get_combinations(unit, alloc_sim_prop, &num_combinations));
    if (!num_combinations)
    {
        sal_printf("Allocation simulation: No entries in LOCAL_OUT_LIF_SIMULATION table\n");
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_outlif_allocation_simulate_alloc(unit, alloc_sim_prop, num_combinations, &success));
    if (success)
    {
        sal_printf("Allocation simulation: SUCCESS :)\n");
        SHR_EXIT();
    }
    /**Requested allocation failed. Display what we did manage to allocate*/
    sal_printf("Allocation simulation: FAILURE :(\n");

    PRT_TITLE_SET("local outlifs allocation simulation summary");
    PRT_COLUMN_ADD("DBAL Table");
    PRT_COLUMN_ADD("Result type");
    PRT_COLUMN_ADD("Logical phase");
    PRT_COLUMN_ADD("With ll");
    PRT_COLUMN_ADD("Num of entries requested");
    PRT_COLUMN_ADD("Num of entries allocated");

    for (idx = 0; idx < num_combinations; idx++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        /**dbal table*/
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, alloc_sim_prop[idx].table_id));
        /**dbal result type*/
        PRT_CELL_SET("%s",
                     dbal_result_type_to_string(unit, alloc_sim_prop[idx].table_id, alloc_sim_prop[idx].res_type));
        /**outlif phase*/
        PRT_CELL_SET("%d", alloc_sim_prop[idx].phase_id + 1);
        /**with ll*/
        PRT_CELL_SET("%s", (alloc_sim_prop[idx].with_ll) ? "TRUE" : "FALSE");
        /**requested number of lifs*/
        PRT_CELL_SET("%d", alloc_sim_prop[idx].num_requested_allocs);
        /**successful allocations*/
        PRT_CELL_SET("%d", alloc_sim_prop[idx].alloc_count);
    }
    PRT_COMMITX;

exit:
    SHR_IF_ERR_EXIT(dnx_lif_diag_logger_restore(unit, severity));
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_outlif_allocation_alloc_sim_max_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *table_name;
    char *result_type_name;
    int ll_exists, rv;
    uint32 res_type;
    dbal_enum_value_field_mdb_eedb_phase_e phase;
    alloc_sim_prop_t alloc_sim_prop = { 0 };
    dbal_tables_e table_id;
    bsl_severity_t severity = bslSeverityOff;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("ll_exists", ll_exists);
    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_STR("result_type", result_type_name);
    SH_SAND_GET_ENUM("phase", phase);

    /**Don't show error messages*/
    SHR_IF_ERR_EXIT(dnx_lif_diag_logger_close(unit, &severity));

    SHR_IF_ERR_EXIT(dnx_outlif_eedb_dbal_table_from_string(unit, table_name, &table_id, sand_control));
    if (ISEMPTY(table_name) || (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit)))
    {
        SHR_EXIT();
    }

    rv = dnx_outlif_eedb_dbal_result_type_from_string(unit, table_id, result_type_name, &res_type, sand_control);
    if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }

    alloc_sim_prop.phase_id = phase;
    alloc_sim_prop.with_ll = ll_exists;
    alloc_sim_prop.table_id = table_id;
    alloc_sim_prop.res_type = res_type;

    SHR_IF_ERR_EXIT(dnx_outlif_allocation_validate_ll(unit, &alloc_sim_prop));
    SHR_IF_ERR_EXIT(dnx_outlif_allocation_simulate_alloc(unit, &alloc_sim_prop, 1, NULL));
    sal_printf("Allocation simulation result: %d possible lifs \n", alloc_sim_prop.alloc_count);

exit:
    SHR_IF_ERR_EXIT(dnx_lif_diag_logger_restore(unit, severity));
    SHR_FUNC_EXIT;
}

/**
 * List of the supported commands, pointer to command function and command usage function.
 */

static sh_sand_man_t sh_dnx_outlif_allocation_check_dump_man = {
    .brief = "Display outlif banks info."
};

static sh_sand_man_t sh_dnx_outlif_allocation_phase_check_man = {
    .brief = "Per given phase - display outlif banks info.",
    .examples = "phase=1"
};

static sh_sand_man_t sh_dnx_outlif_allocation_alloc_combinations_man = {
    .brief = "alloc combinations menu",
    .full = NULL
};

static sh_sand_man_t sh_dnx_outlif_allocation_simulate_man = {
    .brief = "alloc combinations menu",
    .full = NULL
};

static sh_sand_man_t sh_dnx_outlif_allocation_man = {
    .brief = "Display outlif banks info.",
    .full = "Outlif bank is a SW abstraction of the EEDB banks and clusters management.\n"
        "Each bank is per-phase with {granularity,has-ll,assigned(eedb bank/cluster)} properties.\n"
        "The outlifs which are mapped to clusters are allocated at init.\n"
        "The ones that are mapped to eedb banks are dynamically mapped.\n"
        "EEDB bank assigned to an outlif bank will only be valid per the outlif bank's phase.\n"
        "EEDB banks used for ll are dedicated per outlif banks (if needed)\n"
};

static sh_sand_man_t sh_dnx_outlif_allocation_phase_cnt_dump_man = {
    .brief = "Display for {logical phase, dbal table, result type} the num of existing entries.",
    .full = NULL,
    .synopsis = NULL,
    .examples = "phase=8\n" "table=EEDB_OAM_REFLECTOR\n" "phase=8 table=EEDB_OAM_REFLECTOR"
};

static sh_sand_man_t sh_dnx_outlif_allocation_alloc_combinations_add_man = {
    .brief = "Outlifs allocation simulation - add a specific entry to the requested combinations.",
    .synopsis = NULL,
    .examples = "table=mpls_tunnel result_type=etps_mpls_1 phase=7 ll_exists num_lifs=1"
};

static sh_sand_man_t sh_dnx_outlif_allocation_alloc_combinations_get_man = {
    .brief = "Outlifs allocation simulation - get a specific entry from requested combinations.",
    .synopsis = NULL,
    .examples = "table=mpls_tunnel result_type=etps_mpls_1 phase=7 ll_exists"
};

static sh_sand_man_t sh_dnx_outlif_allocation_alloc_combinations_delete_man = {
    .brief = "Outlifs allocation simulation - delete a specific entry from requested combinations.",
    .synopsis = NULL,
    .examples = "table=mpls_tunnel result_type=etps_mpls_1 phase=7 ll_exists"
};

static sh_sand_man_t sh_dnx_outlif_allocation_alloc_combinations_clear_man = {
    .brief = "Outlifs allocation simulation - clear all requested combinations.",
    .synopsis = NULL,
    .examples = ""
};

static sh_sand_man_t sh_dnx_outlif_allocation_alloc_combinations_status_man = {
    .brief = "Outlifs allocation simulation - show current combinations requested.",
    .full = "Output the combinations requested for the alloc combinations simulation\n",
    .synopsis = NULL,
    .examples = ""
};

static sh_sand_man_t sh_dnx_outlif_allocation_alloc_combinations_run_man = {
    .brief = "Outlifs allocation simulation.",
    .full =
        "Based on what was set prior using the add command, run a simulation and see if can be allocated. If not - display status of possible allocations\n"
        "Important note: The simulation is performed based on the order of the entries added (i.e - order can affect the results)\n",
    .synopsis = NULL,
    .examples = ""
};

static sh_sand_man_t sh_dnx_outlif_allocation_simulate_max_man = {
    .brief = "Outlifs max allocation simulation.",
    .full = "Simulate how many entries can be allocated per {dbal table, result type, phase, num allocs} - \n"
        "First - user need to update LOCAL_OUT_LIF_SIMULATION with the type of entries he wishes to simulate and the number of wanted lifs\n"
        "Then - run the diagnostic\n"
        "NOTE: The simulation runs in the order in which the entries are stored in the simulation table",
    .synopsis = NULL,
    .examples = "table=EEDB_ARP result_type=ETPS_ARP phase=7 ll_exists"
};

static sh_sand_option_t sh_dnx_outlif_allocation_phase_check_options[] = {
    {"phase", SAL_FIELD_TYPE_INT32, "phase_id", NULL, NULL, "1-8"},
    {NULL}
};

static sh_sand_enum_t sh_dnx_outlif_allocation_phase_enum_table[] = {
    {"1", DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1, "phase 1"},
    {"2", DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2, "phase 2"},
    {"3", DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3, "phase 3"},
    {"4", DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4, "phase 4"},
    {"5", DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5, "phase 5"},
    {"6", DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6, "phase 6"},
    {"7", DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7, "phase 7"},
    {"8", DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8, "phase 8"},
    {"all_phases", DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES, "All phases"},
    {NULL}
};

static sh_sand_option_t sh_dnx_outlif_allocation_count_check_options[] = {
    {"Table", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"Phase", SAL_FIELD_TYPE_ENUM, "outlif logical phase (1-8)", "all_phases",
     (void *) sh_dnx_outlif_allocation_phase_enum_table},
    {NULL}
};

static sh_sand_option_t sh_dnx_outlif_allocation_alloc_combinations_add_check_options[] = {
    {"Table", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"result_type", SAL_FIELD_TYPE_STR, "dbal result type", ""},
    {"Phase", SAL_FIELD_TYPE_ENUM, "outlif logical phase (1-8)", "1",
     (void *) sh_dnx_outlif_allocation_phase_enum_table},
    {"ll_exists", SAL_FIELD_TYPE_BOOL, "ll exists in combination {yes, no}", "Yes"},
    {"num_lifs", SAL_FIELD_TYPE_UINT32, "number of requested lifs for this combination", "10"},
    {NULL}
};

static sh_sand_option_t sh_dnx_outlif_allocation_alloc_combinations_get_check_options[] = {
    {"Table", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"result_type", SAL_FIELD_TYPE_STR, "dbal result type", ""},
    {"Phase", SAL_FIELD_TYPE_ENUM, "outlif logical phase (1-8)", "1",
     (void *) sh_dnx_outlif_allocation_phase_enum_table},
    {"ll_exists", SAL_FIELD_TYPE_BOOL, "ll exists in combination {yes, no}", "Yes"},
    {NULL}
};

static sh_sand_option_t sh_dnx_outlif_allocation_alloc_combinations_delete_check_options[] = {
    {"Table", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"result_type", SAL_FIELD_TYPE_STR, "dbal result type", ""},
    {"Phase", SAL_FIELD_TYPE_ENUM, "outlif logical phase (1-8)", "1",
     (void *) sh_dnx_outlif_allocation_phase_enum_table},
    {"ll_exists", SAL_FIELD_TYPE_BOOL, "ll exists in combination {yes, no}", "Yes"},
    {NULL}
};

static sh_sand_option_t sh_dnx_outlif_allocation_alloc_combinations_clear_check_options[] = {
    {NULL}
};

static sh_sand_option_t sh_dnx_outlif_allocation_alloc_combinations_status_check_options[] = {
    {NULL}
};

static sh_sand_option_t sh_dnx_outlif_allocation_alloc_combinations_run_check_options[] = {
    {NULL}
};

static sh_sand_option_t sh_dnx_outlif_allocation_simulate_max_check_options[] = {
    {"table", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"result_type", SAL_FIELD_TYPE_STR, "dbal result type", ""},
    {"Phase", SAL_FIELD_TYPE_ENUM, "outlif logical phase (1-8)", "1",
     (void *) sh_dnx_outlif_allocation_phase_enum_table},
    {"ll_exists", SAL_FIELD_TYPE_BOOL, "ll exists in combination {yes, no}", "No"},
    {NULL}
};

static sh_sand_cmd_t sh_dnx_local_outlif_phase_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"dump", dnx_outlif_allocation_phase_dump_cmd, NULL, sh_dnx_outlif_allocation_phase_check_options,
     &sh_dnx_outlif_allocation_phase_check_man},
    {NULL}
};

static sh_sand_cmd_t dnx_outlif_allocation_alloc_combinations_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"add", dnx_outlif_allocation_alloc_combinations_add_cmd, NULL,
     sh_dnx_outlif_allocation_alloc_combinations_add_check_options,
     &sh_dnx_outlif_allocation_alloc_combinations_add_man},
    {"get", dnx_outlif_allocation_alloc_combinations_get_cmd, NULL,
     sh_dnx_outlif_allocation_alloc_combinations_get_check_options,
     &sh_dnx_outlif_allocation_alloc_combinations_get_man},
    {"delete", dnx_outlif_allocation_alloc_combinations_delete_cmd, NULL,
     sh_dnx_outlif_allocation_alloc_combinations_delete_check_options,
     &sh_dnx_outlif_allocation_alloc_combinations_delete_man},
    {"clear", dnx_outlif_allocation_alloc_combinations_clear_cmd, NULL,
     sh_dnx_outlif_allocation_alloc_combinations_clear_check_options,
     &sh_dnx_outlif_allocation_alloc_combinations_clear_man},
    {"status", dnx_outlif_allocation_alloc_combinations_status_cmd, NULL,
     sh_dnx_outlif_allocation_alloc_combinations_status_check_options,
     &sh_dnx_outlif_allocation_alloc_combinations_status_man},
    {"run", dnx_outlif_allocation_alloc_combinations_run_cmd, NULL,
     sh_dnx_outlif_allocation_alloc_combinations_run_check_options,
     &sh_dnx_outlif_allocation_alloc_combinations_run_man},
    {NULL}
};

static sh_sand_cmd_t sh_dnx_local_outlif_simulate_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"max_alloc", dnx_outlif_allocation_alloc_sim_max_cmd, NULL, sh_dnx_outlif_allocation_simulate_max_check_options,
     &sh_dnx_outlif_allocation_simulate_max_man},
    {"alloc_combinations", NULL, dnx_outlif_allocation_alloc_combinations_cmds, NULL,
     &sh_dnx_outlif_allocation_alloc_combinations_man},
    {NULL}
};

static sh_sand_cmd_t sh_dnx_local_outlif_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"phase", NULL, sh_dnx_local_outlif_phase_cmds, NULL, &sh_dnx_outlif_allocation_phase_check_man},
    {"dump", dnx_outlif_allocation_dump_cmd, NULL, NULL, &sh_dnx_outlif_allocation_check_dump_man},
    {"count", dnx_outlif_allocation_phase_cnt_status_cmd, NULL, sh_dnx_outlif_allocation_count_check_options,
     &sh_dnx_outlif_allocation_phase_cnt_dump_man},
    {"simulate", NULL, sh_dnx_local_outlif_simulate_cmds, NULL, &sh_dnx_outlif_allocation_simulate_man},
    {NULL}
};

sh_sand_cmd_t sh_dnx_lif_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"LocalOutLIF", NULL, sh_dnx_local_outlif_cmds, NULL, &sh_dnx_outlif_allocation_man},
    {NULL}
};

sh_sand_man_t sh_dnx_lif_man = {
    .brief = "Display lif information"
};
