/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include "dbal_internal.h"
#include <soc/dnx/mdb.h>
#include <soc/dnx/mdb_internal_shared.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l2/l2.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>

#define DELETE_MSG "delete from"
#define ADD_MSG "add to"

#define DBAL_MDB_ACCESS_APP_ID_GET(_unit, _entry_handle, _app_id)                                                 \
    if (DBAL_TABLE_IS_TCAM(_entry_handle->table))                                                                 \
    {                                                                                                             \
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, _entry_handle->table_id, &_app_id));     \
    }                                                                                                             \
    else                                                                                                          \
    {                                                                                                             \
        _app_id = _entry_handle->table->app_id;                                                                   \
    }

#define DBAL_MDB_ACCESS_ACTION_VALIDATE(action, phy_table)                                                         \
    if (action == NULL)                                                                                 \
    {                                                                                                   \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Missing implementation %s\n", dbal_physical_table_to_string(unit, phy_table));\
    }

#define DBAL_MDB_ACCESS_NUM_ERPP_PROFILES_IN_ENTRY 4

#define DBAL_MDB_ACCESS_GREM_ZERO_PAD (dnx_data_lif.out_lif.glem_result_get(unit) - (DBAL_MDB_ACCESS_NUM_ERPP_PROFILES_IN_ENTRY * dnx_data_lif.out_lif.outrif_profile_width_get(unit)))

#define DBAL_MDB_ACCESS_ERPP_PROFILE_BIT_MASK   0xF

#define DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif) (global_rif % DBAL_MDB_ACCESS_NUM_ERPP_PROFILES_IN_ENTRY)

#define DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif)    \
        (DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif) * dnx_data_lif.out_lif.outrif_profile_width_get(unit) + DBAL_MDB_ACCESS_GREM_ZERO_PAD)

#define DBAL_MDB_ACCESS_ALIGNED_PAYLOAD_GET(payload, global_rif) \
        payload = DBAL_MDB_ACCESS_ERPP_PROFILE_BIT_MASK & (payload >> DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif))

#define DBAL_MDB_ACCESS_ALIGNED_PAYLOAD_SET(payload, global_rif)    \
        payload = payload << DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif)

extern uint32 G_dbal_field_full_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];

static dbal_physical_table_actions_t mdb_empty_actions = {

    NULL,

    NULL,

    NULL,

    NULL,

    NULL,

    NULL,

    NULL,

    NULL,

    NULL,

    NULL
};

static dbal_physical_table_actions_t mdb_em_actions = {

    mdb_em_entry_add,

    mdb_em_entry_get,

    mdb_em_entry_delete,

    mdb_em_table_clear,

    NULL,

    NULL,

    NULL,

    mdb_em_iterator_init,

    mdb_em_iterator_get_next,

    mdb_em_iterator_deinit
};

static dbal_physical_table_actions_t mdb_direct_actions = {

    mdb_direct_table_entry_add,

    mdb_direct_table_entry_get,

    mdb_direct_table_entry_delete,

    mdb_direct_table_clear,

    NULL,

    NULL,

    NULL,

    mdb_direct_table_iterator_init,

    mdb_direct_table_iterator_get_next,

    mdb_direct_table_iterator_deinit
};

static dbal_physical_table_actions_t mdb_eedb_actions = {

    mdb_eedb_table_entry_add,

    mdb_eedb_table_entry_get,

    mdb_eedb_table_entry_delete,

    mdb_eedb_table_clear,

    NULL,

    NULL,

    NULL,

    mdb_eedb_table_iterator_init,

    mdb_eedb_table_iterator_get_next,

    mdb_eedb_table_iterator_deinit
};

static dbal_physical_table_actions_t mdb_tcam_actions = {

    dnx_field_tcam_access_entry_add,

    dnx_field_tcam_access_entry_get,

    dnx_field_tcam_access_entry_delete,

    NULL,

    NULL,

    NULL,

    NULL,

    dnx_field_tcam_access_iterator_init,

    dnx_field_tcam_access_iterator_get_next,

    dnx_field_tcam_access_iterator_deinit
};

static dbal_physical_table_actions_t mdb_lpm_actions = {

    mdb_lpm_entry_add,

    mdb_lpm_entry_get,

    mdb_lpm_entry_delete,

    mdb_lpm_table_clear,

    NULL,

    NULL,

    NULL,

    mdb_lpm_iterator_init,

    mdb_lpm_iterator_get_next,

    mdb_lpm_iterator_deinit
};

static dbal_physical_table_actions_t *dbal_mdb_table_actions_mngr[DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES] = {
    &mdb_em_actions,
    &mdb_direct_actions,
    &mdb_tcam_actions,
    &mdb_lpm_actions,
    &mdb_eedb_actions,
    &mdb_empty_actions
};

static shr_error_e
dbal_physical_entry_print(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_tables_e physical_table_id,
    dbal_physical_entry_t * entry,
    uint32 app_id,
    uint8 print_only_key,
    char *action)
{
    int ii, bsl_severity;
    int key_size_in_words = BITS2WORDS(entry->key_size);
    int payload_size_in_words = BITS2WORDS(entry->payload_size);
    int logger_action = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    if (bsl_severity >= bslSeverityInfo)
    {
        if (dbal_logger_is_enable(unit, entry_handle->table_id))
        {
            if ((!sal_strncasecmp(action, "get from", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
                || (!sal_strncasecmp(action, "get next from", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
                || (!sal_strncasecmp(action, "Access ID get", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)))
            {
                logger_action = 1;
            }
            else if (!sal_strncasecmp(action, DELETE_MSG, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
            {
                logger_action = 2;
            }
            else
            {
                logger_action = 0;
            }

            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_MDB, logger_action);

            SHR_IF_ERR_EXIT(dbal_mdb_phy_table_is_valid(unit, physical_table_id));
            LOG_CLI((BSL_META("Entry %s Physical Table: %s\n"), action,
                     dbal_physical_table_to_string(unit, physical_table_id)));

            LOG_CLI((BSL_META("Acccess ID %d\n"), entry->entry_hw_id));

            LOG_CLI((BSL_META("Phy. entry buffer:\n")));
            LOG_CLI((BSL_META("Key(%3d bits): 0x"), entry->key_size));
            for (ii = key_size_in_words - 1; ii >= 0; ii--)
            {
                LOG_CLI((BSL_META("%08x"), entry->key[ii]));
            }
            LOG_CLI((BSL_META("\n")));

            LOG_CLI((BSL_META("Key Mask     : 0x")));
            for (ii = key_size_in_words - 1; ii >= 0; ii--)
            {
                LOG_CLI((BSL_META("%08x"), entry->k_mask[ii]));
            }
            LOG_CLI((BSL_META("\n")));

            if (entry->prefix_length != 0)
            {

                LOG_CLI((BSL_META("Prefix length: %u\n"), entry->prefix_length));
            }

            if (!print_only_key)
            {

                LOG_CLI((BSL_META("Payload(%3d bits): 0x"), entry->payload_size));
                for (ii = payload_size_in_words - 1; ii >= 0; ii--)
                {
                    LOG_CLI((BSL_META("%08x"), entry->payload[ii]));
                }
                LOG_CLI((BSL_META("\n")));
                LOG_CLI((BSL_META("Payload Mask     : 0x")));
                for (ii = payload_size_in_words - 1; ii >= 0; ii--)
                {
                    LOG_CLI((BSL_META("%08x"), entry->p_mask[ii]));
                }
                LOG_CLI((BSL_META("\n")));
            }
            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_MDB, logger_action);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_outlif_sw_info_update_payload_size(
    int unit,
    dbal_tables_e table_id,
    int result_type,
    dbal_physical_entry_t * phy_entry,
    uint8 *rt_has_link_list,
    dbal_physical_tables_e * physical_table,
    uint8 *is_eedb_ll)
{
    dnx_algo_local_outlif_logical_phase_e logical_phase = 0;
    int physical_phase;
    SHR_FUNC_INIT_VARS(unit);

    {

        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                        (unit, phy_entry->key[0], NULL, NULL, &logical_phase, NULL, is_eedb_ll, NULL));
        physical_phase =
            (dnx_data_lif.out_lif.logical_to_physical_phase_map_get(unit, logical_phase + 1))->physical_phase;
        *physical_table = physical_phase + DBAL_PHYSICAL_TABLE_EEDB_1;

        if (!(*is_eedb_ll))
        {
            SHR_IF_ERR_EXIT(dnx_lif_mngr_eedb_table_rt_has_linked_list(unit, table_id, result_type, rt_has_link_list));
            if (*rt_has_link_list)
            {
                phy_entry->payload_size -= MDB_DIRECT_BASIC_ENTRY_SIZE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_fec_tables_init(
    int unit)
{
    uint32 fec_db_iter;
    const dnx_data_l3_fec_fec_physical_db_t *info;

    SHR_FUNC_INIT_VARS(unit);

    info = dnx_data_l3.fec.fec_physical_db_get(unit);
    for (fec_db_iter = 0; fec_db_iter < dnx_data_mdb.direct.nof_fec_dbs_get(unit); fec_db_iter++)
    {
        uint32 range_start, range_size, max_range = 0, min_range = 0;

        SHR_IF_ERR_EXIT(mdb_init_fec_ranges_allocation_info_get
                        (unit, info->physical_table[fec_db_iter], &range_start, &range_size));

        if (range_size > 0)
        {
            min_range = (range_start / dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit));
            max_range = (range_start + range_size - 1) / dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit);
        }

        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                        (unit, dnx_data_l3.fec.fec_tables_info_get(unit, info->physical_table[fec_db_iter])->dbal_table,
                         DBAL_FIELD_SUPER_FEC_ID, TRUE, 0, 0, DBAL_PREDEF_VAL_MIN_VALUE, min_range));
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                        (unit, dnx_data_l3.fec.fec_tables_info_get(unit, info->physical_table[fec_db_iter])->dbal_table,
                         DBAL_FIELD_SUPER_FEC_ID, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, max_range));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_init(
    int unit)
{
    int table_idx;
    dbal_logical_table_t *table;

    int table_capacity[DBAL_NOF_PHYSICAL_TABLES];
    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(mdb_pre_init_step(unit));
    }

    for (table_idx = 0; table_idx < DBAL_NOF_PHYSICAL_TABLES; table_idx++)
    {
        dbal_physical_table_actions_t *physical_table_actions;

        if ((table_idx == DBAL_PHYSICAL_TABLE_TCAM)
            || (table_idx == DBAL_PHYSICAL_TABLE_KBP)
            || (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, table_idx)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_NONE))
        {
            table_capacity[table_idx] = 0;
            if (!sw_state_is_warm_boot(unit))
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.capacity.set(unit, table_idx, 0));
            }
            continue;
        }

        SHR_IF_ERR_EXIT(mdb_get_capacity(unit, table_idx, &table_capacity[table_idx]));

        if (!sw_state_is_warm_boot(unit))
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.set(unit, table_idx, table_capacity[table_idx]));
        }

        SHR_IF_ERR_EXIT(dbal_mdb_phy_table_actions_get(unit, table_idx, &physical_table_actions));
        if (physical_table_actions->table_init)
        {
            SHR_IF_ERR_EXIT(physical_table_actions->table_init(unit));
        }
    }

    for (table_idx = 0; table_idx < DBAL_NOF_TABLES; table_idx++)
    {
        uint32 max_capacity = 0;
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_idx, &table));

        if ((table->access_method != DBAL_ACCESS_METHOD_MDB) || DBAL_TABLE_IS_TCAM(table))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_mdb_table_total_capacity_get(unit, table, &max_capacity));
        table->max_capacity = max_capacity;

    }

    SHR_IF_ERR_EXIT(dbal_mdb_fec_tables_init(unit));

    if (!sw_state_is_warm_boot(unit))
    {

        SHR_IF_ERR_EXIT(dbal_flush_init(unit));
    }

    if (dnx_data_l2.age_and_flush_machine.feature_get(unit, dnx_data_l2_age_and_flush_machine_flush_init_enable))
    {
        SHR_IF_ERR_EXIT(dbal_flush_buffer_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_total_capacity_get(
    int unit,
    dbal_logical_table_t * table,
    uint32 *total_capacity)
{
    int jj;
    int phys_table_max_capacity;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, table->physical_db_id[0], (int *) total_capacity));

    for (jj = 1; jj < table->nof_physical_tables; jj++)
    {
        if (table->core_mode == DBAL_CORE_MODE_SBC)
        {

            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, table->physical_db_id[jj], &phys_table_max_capacity));
            if (phys_table_max_capacity < *total_capacity)
            {
                *total_capacity = phys_table_max_capacity;
            }
        }
        else if (table->core_mode == DBAL_CORE_MODE_DPC)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, table->physical_db_id[jj], &phys_table_max_capacity));
            *total_capacity += phys_table_max_capacity;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_deinit(
    int unit)
{
    int table_idx;

    SHR_FUNC_INIT_VARS(unit);

    for (table_idx = 0; table_idx < DBAL_NOF_PHYSICAL_TABLES; table_idx++)
    {
        dbal_physical_table_actions_t *physical_table_actions;

        SHR_IF_ERR_EXIT(dbal_mdb_phy_table_actions_get(unit, table_idx, &physical_table_actions));

        if (physical_table_actions->table_deinit)
        {
            SHR_IF_ERR_EXIT(physical_table_actions->table_deinit(unit));
        }
    }

    dbal_flush_buffer_deinit(unit);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_phy_table_is_valid(
    int unit,
    dbal_physical_tables_e physical_table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (physical_table_id >= DBAL_NOF_PHYSICAL_TABLES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR physical_table_id %d is not valid\n", physical_table_id);
    }

exit:
    SHR_FUNC_EXIT;
}

dbal_core_mode_e
dbal_mdb_phy_table_core_mode_get(
    int unit,
    dbal_physical_tables_e physical_table_id)
{
    return dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, physical_table_id)->core_mode;
}

shr_error_e
dbal_mdb_phy_table_actions_get(
    int unit,
    dbal_physical_tables_e physical_table_id,
    dbal_physical_table_actions_t ** physical_table_actions)
{
    dbal_enum_value_field_mdb_db_type_e db_type;

    SHR_FUNC_INIT_VARS(unit);

    *physical_table_actions = NULL;
    SHR_IF_ERR_EXIT(dbal_mdb_phy_table_is_valid(unit, physical_table_id));

    db_type = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, physical_table_id)->db_type;

    *physical_table_actions = dbal_mdb_table_actions_mngr[db_type];
exit:
    if (*physical_table_actions == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_mdb_hw_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size)
{
    int payload_size;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "API can not called with table which is not MDB table. table %s.\n",
                     table->table_name);
    }

    if (res_type_idx < table->nof_result_types)
    {
        payload_size = table->multi_res_info[res_type_idx].entry_payload_size;
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
        {
            payload_size -= table->multi_res_info[res_type_idx].results_info[0].field_nof_bits;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal result type index %d for table %s. num of result types is %d\n",
                     res_type_idx, table->table_name, table->nof_result_types);
    }

    *p_size = payload_size;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    dbal_tables_e * table_id)
{
    int ii, jj, num_of_tables;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    num_of_tables = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    if (phy_db_id < DBAL_NOF_PHYSICAL_TABLES)
    {
        if (app_db_id < DBAL_NOF_APP_IDS_PER_PHYSICAL_TABLE)
        {
            for (ii = 0; ii < num_of_tables; ii++)
            {
                int is_table_active;
                SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));
                if (table->access_method != DBAL_ACCESS_METHOD_MDB)
                {
                    continue;
                }

                SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, ii, &is_table_active));

                if (!is_table_active)
                {
                    continue;
                }

                if ((sal_strnstr(table->table_name, "EXAMPLE", sizeof(table->table_name)) != NULL))
                {
                    continue;
                }

                for (jj = 0; jj < table->nof_physical_tables; jj++)
                {
                    if (table->physical_db_id[jj] == phy_db_id)
                    {
                        if (phy_db_id == DBAL_PHYSICAL_TABLE_TCAM)
                        {
                            if ((table->app_id == app_db_id) ||
                                ((table->app_id + 1 == app_db_id) && (table->key_size > 160)))
                            {
                                *table_id = (dbal_tables_e) ii;
                                SHR_EXIT();
                            }
                        }
                        else
                        {
                            uint32 app_id_1 = 0;
                            uint32 app_id_2 = 0;

                            app_id_1 = table->app_id & ((1 << table->app_id_size) - 1);
                            app_id_2 = app_db_id & ((1 << table->app_id_size) - 1);

                            if (app_id_1 == app_id_2)
                            {
                                *table_id = (dbal_tables_e) ii;
                                SHR_EXIT();
                            }
                        }
                    }
                }
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal AppDbId %d, Max Value is %d\n", app_db_id,
                         DBAL_NOF_APP_IDS_PER_PHYSICAL_TABLE - 1);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal phyDB %d, DBAL_NOF_PHYSICAL_TABLES=%d\n", phy_db_id,
                     DBAL_NOF_PHYSICAL_TABLES);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_id_and_result_type_from_etps_format_get(
    int unit,
    uint32 etps_format,
    dbal_tables_e * table_id,
    int *result_type)
{
    int ii, jj;
    CONST dbal_logical_table_t *table;
    dbal_table_status_e table_status;
    dbal_tables_e table_id_tmp = DBAL_NOF_TABLES;
    int result_type_tmp = -1;

    int allow_esem_default = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_application_v2))
    {
        allow_esem_default = 1;
    }

    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, ii, &table_status));
        if (table_status == DBAL_TABLE_INCOMPATIBLE_IMAGE)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));
        if (table->access_method != DBAL_ACCESS_METHOD_MDB && ii != DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE)
        {
            continue;
        }

        if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, DBAL_PHYSICAL_TABLE_ESEM)->db_type !=
            DBAL_ENUM_FVAL_MDB_DB_TYPE_NONE)
        {
            if ((!dbal_table_is_out_lif(table)) && (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_ESEM)
                && ii != DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE
                && (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_SEXEM_2))
            {
                continue;
            }

            if ((table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_SEXEM_2)
                && (table->multi_res_info[0].reference_field_id != DBAL_FIELD_TYPE_DEF_ETPP_ETPS_FORMATS))
            {
                continue;
            }
        }

        if ((ii == DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE) && (!allow_esem_default))
        {
            continue;
        }

        for (jj = 0; jj < table->nof_result_types; jj++)
        {

            uint32 hw_value_aligned_to_etps_size;
            uint32 etps_format_no_masked_bits;
            uint32 nof_masked_bits;
            uint32 masked_bits_mask;

            if (table->multi_res_info[jj].nof_result_fields == 0)
            {
                continue;
            }

            nof_masked_bits =
                dnx_data_pp.ETPP.etps_type_size_bits_get(unit) -
                table->multi_res_info[jj].results_info[0].field_nof_bits;
            masked_bits_mask = (1 << nof_masked_bits) - 1;
            masked_bits_mask = ~masked_bits_mask;

            hw_value_aligned_to_etps_size = table->multi_res_info[jj].result_type_hw_value[0] << nof_masked_bits;

            etps_format_no_masked_bits = etps_format & masked_bits_mask;

            if ((hw_value_aligned_to_etps_size == etps_format_no_masked_bits) &&
                (table->multi_res_info[jj].is_disabled == FALSE) &&
                (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW)))
            {

                if (nof_masked_bits == 0)
                {
                    *table_id = ii;
                    *result_type = jj;
                    SHR_EXIT();
                }
                else
                {
                    table_id_tmp = ii;
                    result_type_tmp = jj;
                }
            }
        }
    }
    if (result_type_tmp == -1)
    {
        *table_id = DBAL_NOF_TABLES;
        *result_type = 0;
    }
    else
    {
        *table_id = table_id_tmp;
        *result_type = result_type_tmp;
    }

exit:
    SHR_FUNC_EXIT;
}

uint8
dbal_table_is_in_lif(
    CONST dbal_logical_table_t * table)
{
    if ((table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_INLIF_1) ||
        ((table->physical_db_id[0] >= DBAL_PHYSICAL_TABLE_INLIF_2)
         && (table->physical_db_id[0] <= DBAL_PHYSICAL_TABLE_INLIF_3)))
    {
        return TRUE;
    }

    return FALSE;
}

uint8
dbal_table_is_out_lif(
    CONST dbal_logical_table_t * table)
{
    if (((table->physical_db_id[0] >= DBAL_PHYSICAL_TABLE_EEDB_1)
         && (table->physical_db_id[0] <= DBAL_PHYSICAL_TABLE_EEDB_8))
        && ((table->allocator_field_id == DBAL_FIELD_OUT_RIF) || (table->allocator_field_id == DBAL_FIELD_OUT_LIF)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8
dbal_table_is_esem(
    CONST dbal_logical_table_t * table)
{
    if (table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_ESEM)
    {
        return TRUE;
    }

    return FALSE;
}

uint8
dbal_logical_table_is_fec(
    int unit,
    CONST dbal_logical_table_t * table)
{
    if ((table->physical_db_id[0] >= DBAL_PHYSICAL_TABLE_FEC_1)
        && (table->physical_db_id[0] <= DBAL_PHYSICAL_TABLE_FEC_3))
    {
        return TRUE;
    }

    return FALSE;
}

uint8
dbal_logical_table_is_mact(
    int unit,
    dbal_tables_e table_id)
{
    uint8 is_mact = FALSE;

    if (dbal_image_name_is_std_1(unit))
    {
        if ((table_id == DBAL_TABLE_FWD_MACT) || (table_id == DBAL_TABLE_FWD_MACT_IVL))
        {
            is_mact = TRUE;
        }

    }

    return is_mact;
}

shr_error_e
dbal_physical_table_app_id_is_mact(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    uint32 *is_mact)
{
    int app_db_size;

    SHR_FUNC_INIT_VARS(unit);

    *is_mact = FALSE;

    if (!dbal_image_name_is_std_1(unit))
    {
        SHR_EXIT();
    }

    {
        if (DBAL_IS_DNX2(unit))
        {
            uint32 mact_app_id, mact_ivl_app_id, mact_pcc_app_id;

            SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_FWD_MACT, &mact_app_id, &app_db_size));
            SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_FWD_MACT_IVL, &mact_ivl_app_id, &app_db_size));
            SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_PCC_STATE, &mact_pcc_app_id, &app_db_size));
            if ((dbal_physical_table == DBAL_PHYSICAL_TABLE_LEM)
                && ((app_id == mact_app_id) || (app_id == mact_ivl_app_id) || (app_id == mact_pcc_app_id)))
            {
                *is_mact = TRUE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
static shr_error_e
dbal_mdb_mact_dynamic_entry_check(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 *is_mact_dynamic)
{
    uint32 strength_value;
    dbal_fields_e field_id;
    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);
    *is_mact_dynamic = FALSE;

    if (dbal_logical_table_is_mact(unit, entry_handle->table_id))
    {
        if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_learn_payload_res_optimize))
        {
            field_id = DBAL_FIELD_MACT_ENTRY_AGE_PROFILE;
        }
        else
        {
            field_id = DBAL_FIELD_MAC_STRENGTH;
        }
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                        (unit, entry_handle->table_id, field_id, 0, entry_handle->cur_res_type, 0, &table_field_info));
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                        (unit, &table_field_info, field_id, entry_handle->phy_entry.payload, &strength_value));
        if (strength_value == DBAL_ENUM_FVAL_MAC_STRENGTH_1)
        {
            *is_mact_dynamic = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dbal_mdb_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 resolve_from_sw)
{
    int jj;
    int curr_res_type = 0;
    uint32 field_value[1] = { 0 };
    dbal_logical_table_t *table = entry_handle->table;
    dbal_table_field_info_t table_field_info;
    int result_type_resolved = 0;

    SHR_FUNC_INIT_VARS(unit);

    if ((dbal_table_is_out_lif(table)) || (dbal_table_is_in_lif(table)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No need to do result type resolution for lif %s",
                     entry_handle->table->table_name);
    }

    dbal_logger_internal_disable_set(unit);
    if (resolve_from_sw != SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB RT resolution, mismatch with result from SW flag. table = %s\n",
                     table->table_name);
    }

    while ((curr_res_type < table->nof_result_types) && (result_type_resolved == 0))
    {
        if (resolve_from_sw)
        {
            SHR_SET_CURRENT_ERR(dbal_sw_res_type_resolution(unit, entry_handle));
            curr_res_type = entry_handle->cur_res_type;
            result_type_resolved = 1;
        }
        else
        {

            if (entry_handle->table->multi_res_info[curr_res_type].is_disabled)
            {
                curr_res_type++;
                continue;
            }

            SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id,
                                                       DBAL_FIELD_RESULT_TYPE, 0, curr_res_type, 0, &table_field_info));

            if (table_field_info.field_nof_bits > 0)
            {

                table_field_info.bits_offset_in_buffer =
                    entry_handle->table->max_payload_size - table_field_info.field_nof_bits;

                SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                                (unit, &table_field_info, DBAL_FIELD_RESULT_TYPE, entry_handle->phy_entry.payload,
                                 field_value));

                for (jj = 0; jj < entry_handle->table->multi_res_info[curr_res_type].result_type_nof_hw_values; jj++)
                {
                    if (entry_handle->table->multi_res_info[curr_res_type].result_type_hw_value[jj] == field_value[0])
                    {

                        entry_handle->cur_res_type = curr_res_type;
                        result_type_resolved = 1;
                        break;
                    }
                }
            }
            else
            {
                entry_handle->cur_res_type = curr_res_type;
                result_type_resolved = 1;
            }

            if (result_type_resolved)
            {
                break;
            }
            curr_res_type++;
        }
    }
    if (curr_res_type == table->nof_result_types)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    if (!SHR_FUNC_ERR())
    {
        if ((entry_handle->table->multi_res_info[curr_res_type].entry_payload_size <
             entry_handle->table->max_payload_size))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                            (unit,
                             entry_handle->table->max_payload_size -
                             entry_handle->table->multi_res_info[curr_res_type].entry_payload_size, TRUE,
                             entry_handle));
        }
    }

exit:
    dbal_logger_internal_disable_clear(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    if (table_id >= dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal table_id %d, max=%d\n", table_id,
                     dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

    if (physical_tbl_index >= table->nof_physical_tables)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "physical_tbl_index (%d) is out of range. max for %s is %d\n",
                     physical_tbl_index, table->table_name, table->nof_physical_tables);
    }
    *physical_table_id = table->physical_db_id[physical_tbl_index];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_table_actions_t *physical_table_actions = NULL;
    dbal_physical_tables_e physical_db_id;
    uint32 app_id;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_iterator_init(unit, entry_handle));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, entry_handle->table_id,
                                                       iterator_info->physical_db_index, &physical_db_id));

        sal_memset(&iterator_info->mdb_iterator, 0x0, sizeof(dbal_physical_entry_iterator_t));

        SHR_IF_ERR_EXIT(dbal_mdb_phy_table_actions_get(unit, physical_db_id, &physical_table_actions));

        DBAL_MDB_ACCESS_ACTION_VALIDATE(physical_table_actions->iterator_init, physical_db_id);

        DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

        SHR_IF_ERR_EXIT(physical_table_actions->iterator_init(unit, physical_db_id, app_id,
                                                              &iterator_info->mdb_iterator));

        if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
        {
            iterator_info->max_num_of_iterations = dnx_data_l3.rif.nof_rifs_get(unit);;
        }
        else if (table->allocator_field_id != DBAL_FIELD_EMPTY)
        {
            dbal_table_field_info_t table_field_info = { 0 };

            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, entry_handle->table_id, table->allocator_field_id, 1, entry_handle->cur_res_type, 0,
                             &table_field_info));

            iterator_info->max_num_of_iterations = table_field_info.max_value;
        }
        else if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED))
        {
            SHR_IF_ERR_EXIT(dbal_tables_max_key_value_get
                            (unit, entry_handle->table_id, &iterator_info->max_num_of_iterations));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_iterator_rules_shadow_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 actions_bitmap,
    uint32 result_types_in_flush_bitmap,
    int nof_flush_rules)
{
    int ii, rule_counter = 0;
    int nof_result_types;
    uint8 has_rt_rule = FALSE;
    int result_type_rule_value = 0;
    dbal_flush_shadow_info_t *flush_shadow = NULL;
    dbal_iterator_info_t *iterator_info;
    dbal_iterator_attribute_info_t *iterator_attrib_info;
    dbal_physical_entry_iterator_t *mdb_iterator_info;
    uint8 has_result_rule_or_action = FALSE;
    uint8 has_result_rule_or_action_on_aligned_fields_only = TRUE;
    uint32 mdb_entry_size_bitmap = 0;
    uint32 forbidden_rule_entry_encoding_bitmap = 0;
    uint8 set_rule_per_entry_encoding = 0;
    dbal_physical_tables_e lem_physical_table_id;
    dnx_l2_action_type_t l2_action_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    l2_action_type = dnx_data_l2.general.lem_table_map_l2_action_get(unit, entry_handle->table_id)->l2_action_type;
    lem_physical_table_id = dnx_data_l2.general.fwd_mact_info_get(unit, l2_action_type, FALSE)->physical_data_base;

    iterator_attrib_info = &iterator_info->attrib_info;
    mdb_iterator_info = &iterator_info->mdb_iterator;
    nof_result_types = entry_handle->table->nof_result_types;

    SHR_ALLOC_SET_ZERO(flush_shadow, sizeof(dbal_flush_shadow_info_t) * nof_flush_rules,
                       "Flush shadow allocation", "%s%s%s\r\n", entry_handle->table->table_name, EMPTY, EMPTY);

    for (ii = 0; ii < iterator_info->nof_val_rules; ii++)
    {
        has_result_rule_or_action = TRUE;
        if ((iterator_info->val_rules_info[ii].field_id != DBAL_FIELD_DESTINATION) &&
            (iterator_info->val_rules_info[ii].field_id != DBAL_FIELD_MAC_STRENGTH) &&
            (iterator_info->val_rules_info[ii].field_id != DBAL_FIELD_ENTRY_GROUPING) &&
            (iterator_info->val_rules_info[ii].field_id != DBAL_FIELD_MACT_ENTRY_AGE_PROFILE))
        {
            has_result_rule_or_action_on_aligned_fields_only = FALSE;
            break;
        }
    }

    for (ii = 0; ii < iterator_info->nof_actions; ii++)
    {
        if (iterator_info->actions_info[ii].action_type == DBAL_ITER_ACTION_UPDATE)
        {
            has_result_rule_or_action = TRUE;

            if ((iterator_info->actions_info[ii].field_id != DBAL_FIELD_DESTINATION) &&
                (iterator_info->actions_info[ii].field_id != DBAL_FIELD_MAC_STRENGTH) &&
                (iterator_info->actions_info[ii].field_id != DBAL_FIELD_ENTRY_GROUPING) &&
                (iterator_info->actions_info[ii].field_id != DBAL_FIELD_MACT_ENTRY_AGE_PROFILE))
            {
                has_result_rule_or_action_on_aligned_fields_only = FALSE;
                break;
            }
        }
    }

    if (!has_result_rule_or_action)
    {
        has_result_rule_or_action_on_aligned_fields_only = FALSE;
    }

    for (ii = 0; ii < iterator_info->nof_val_rules; ii++)
    {
        if (iterator_info->val_rules_info[ii].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            if (!has_rt_rule)
            {
                has_rt_rule = TRUE;
                result_type_rule_value = iterator_info->val_rules_info[ii].value[0];
                if (nof_flush_rules != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found result type rule, but more than a single flush rule.\n");
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found two result type rules.\n");
            }
        }
    }

    for (ii = 0; ii < nof_result_types; ii++)
    {
        int jj;
        uint8 field_not_exists = FALSE;
        uint32 build_rules_entry_handle_id = 0;
        dbal_access_condition_info_t *key_rules, *value_rules;
        dbal_iterator_actions_info_t *actions;
        dbal_entry_handle_t *build_rules_entry_handle;

        if (!utilex_bitstream_test_bit(&result_types_in_flush_bitmap, ii))
        {
            continue;
        }

        if ((has_result_rule_or_action && has_result_rule_or_action_on_aligned_fields_only) ||
            (!has_result_rule_or_action))
        {

            mdb_em_entry_encoding_e entry_encoding = MDB_EM_NOF_ENTRY_ENCODINGS;

            set_rule_per_entry_encoding = 0;

            SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                            (unit, lem_physical_table_id, entry_handle->table->key_size,
                             entry_handle->table->multi_res_info[ii].entry_payload_size, entry_handle->table->app_id,
                             entry_handle->table->app_id_size, &entry_encoding));

            if (!utilex_bitstream_test_bit(&forbidden_rule_entry_encoding_bitmap, entry_encoding) &&
                !utilex_bitstream_test_bit(&mdb_entry_size_bitmap, entry_encoding))
            {

                mdb_em_entry_encoding_e entry_encoding_other = MDB_EM_NOF_ENTRY_ENCODINGS;
                for (jj = 0; jj < nof_result_types; ++jj)
                {
                    if (jj != ii)
                    {
                        SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                                        (unit, lem_physical_table_id, entry_handle->table->key_size,
                                         entry_handle->table->multi_res_info[jj].entry_payload_size,
                                         entry_handle->table->app_id, entry_handle->table->app_id_size,
                                         &entry_encoding_other));
                        if ((entry_encoding_other == entry_encoding)
                            && utilex_bitstream_test_bit(&mdb_iterator_info->result_types_in_non_flush_bitmap, jj))
                        {
                            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit
                                            (&forbidden_rule_entry_encoding_bitmap, entry_encoding));
                            break;
                        }
                    }
                }
            }

            if (!utilex_bitstream_test_bit(&mdb_entry_size_bitmap, entry_encoding) &&
                !utilex_bitstream_test_bit(&forbidden_rule_entry_encoding_bitmap, entry_encoding))
            {
                set_rule_per_entry_encoding = 1;
                utilex_bitstream_set_bit(&mdb_entry_size_bitmap, entry_encoding);
            }
            else if (utilex_bitstream_test_bit(&mdb_entry_size_bitmap, entry_encoding))
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry_handle->table_id, &build_rules_entry_handle_id));

        key_rules = iterator_info->key_rules_info;
        value_rules = iterator_info->val_rules_info;
        actions = iterator_info->actions_info;

        for (jj = 0; jj < iterator_info->nof_key_rules; jj++)
        {
            dbal_entry_key_field_set(unit, build_rules_entry_handle_id, key_rules[jj].field_id, key_rules[jj].value,
                                     key_rules[jj].mask, DBAL_POINTER_TYPE_ARR_UINT32);
        }

        if (!has_rt_rule)
        {
            uint32 rt_value[1] = { 0 };
            rt_value[0] = ii;
            dbal_entry_value_field_set(unit, build_rules_entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                       0, DBAL_POINTER_TYPE_ARR_UINT32, rt_value, G_dbal_field_full_mask);
        }
        else if (ii != result_type_rule_value)
        {
            continue;
        }

        for (jj = 0; jj < iterator_info->nof_val_rules; jj++)
        {
            int rv;
            dbal_table_field_info_t field_info;

            rv = dbal_tables_field_info_get_no_err(unit, entry_handle->table_id, value_rules[jj].field_id,
                                                   0, ii, 0, &field_info);
            if (rv == _SHR_E_NOT_FOUND)
            {
                field_not_exists = TRUE;
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            dbal_entry_value_field_set(unit, build_rules_entry_handle_id, value_rules[jj].field_id,
                                       0, DBAL_POINTER_TYPE_ARR_UINT32, value_rules[jj].value, value_rules[jj].mask);
        }

        if (field_not_exists)
        {
            continue;
        }

        flush_shadow[rule_counter].key_rule_size = entry_handle->table->key_size;
        flush_shadow[rule_counter].value_rule_size = entry_handle->table->multi_res_info[ii].entry_payload_size;

        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, build_rules_entry_handle_id, &build_rules_entry_handle));
        sal_memcpy(flush_shadow[rule_counter].key_rule_buf, build_rules_entry_handle->phy_entry.key,
                   DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);
        sal_memcpy(flush_shadow[rule_counter].key_rule_mask_buf, build_rules_entry_handle->phy_entry.k_mask,
                   DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.payload, 0,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_rule_buf));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.p_mask, 0,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_rule_mask_buf));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, entry_handle->table_id, build_rules_entry_handle_id));
        for (jj = 0; jj < iterator_info->nof_actions; jj++)
        {
            int rv;
            dbal_table_field_info_t field_info;

            if (actions[jj].field_id == DBAL_FIELD_RESULT_TYPE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal action for field RESULT_TYPE\n");
            }
            rv = dbal_tables_field_info_get_no_err(unit, entry_handle->table_id, actions[jj].field_id,
                                                   0, ii, 0, &field_info);
            if (rv == _SHR_E_NOT_FOUND)
            {
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            if (jj == 0)
            {
                uint32 rt_value[1] = { 0 };
                rt_value[0] = ii;
                dbal_entry_value_field_set(unit, build_rules_entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                           0, DBAL_POINTER_TYPE_ARR_UINT32, rt_value, G_dbal_field_full_mask);
            }
            dbal_entry_value_field_set(unit, build_rules_entry_handle_id, actions[jj].field_id,
                                       0, DBAL_POINTER_TYPE_ARR_UINT32, actions[jj].value, actions[jj].mask);
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.payload, 0,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_action_buf));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.p_mask, 0,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_action_mask_buf));

        if (iterator_attrib_info->hit_bit_rule_valid)
        {
            flush_shadow[rule_counter].hit_indication_rule_mask = TRUE;
            flush_shadow[rule_counter].hit_indication_rule = iterator_attrib_info->hit_bit_rule_is_hit;
        }

        flush_shadow[rule_counter].action_types_btmp = actions_bitmap & 0xF;
        flush_shadow[rule_counter].value_action_size = entry_handle->table->max_payload_size;

        if (actions_bitmap & DBAL_ITER_ACTION_READ_TO_DMA)
        {
            flush_shadow[rule_counter].action_types_btmp |= DBAL_ITER_ACTION_GET;
        }

        if (set_rule_per_entry_encoding)
        {

            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                            (flush_shadow[rule_counter].value_rule_mask_buf,
                             entry_handle->table->multi_res_info[ii].entry_payload_size -
                             entry_handle->table->multi_res_info[ii].results_info[0].field_nof_bits,
                             entry_handle->table->multi_res_info[ii].entry_payload_size - 1));
            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                            (flush_shadow[rule_counter].value_action_mask_buf,
                             entry_handle->table->multi_res_info[ii].entry_payload_size -
                             entry_handle->table->multi_res_info[ii].results_info[0].field_nof_bits,
                             entry_handle->table->multi_res_info[ii].entry_payload_size - 1));
        }

        rule_counter++;
    }
    SHR_IF_ERR_EXIT(dbal_flush_rules_set
                    (unit, entry_handle->table->app_id, entry_handle->table_id, rule_counter, flush_shadow));

exit:
    SHR_FREE(flush_shadow);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_iterator_is_flush_machine_can_be_used(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 *use_flush_out,
    uint32 *result_types_in_flush_bitmap,
    int *nof_flush_rules)
{
    int ii, result_type_value_in_rule = 0;
    uint8 has_rt_rule = FALSE;
    dbal_iterator_info_t *iterator_info;
    dbal_physical_entry_iterator_t *mdb_iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    mdb_iterator_info = &iterator_info->mdb_iterator;

    *result_types_in_flush_bitmap = 0;
    mdb_iterator_info->result_types_in_non_flush_bitmap = 0;
    *nof_flush_rules = 0;

    *use_flush_out = TRUE;

    if (!dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_flush_machine_support) ||
        !dbal_logical_table_is_mact(unit, entry_handle->table_id))
    {
        *use_flush_out = FALSE;
    }

    for (ii = 0; (ii < iterator_info->nof_key_rules) && *use_flush_out; ii++)
    {
        if (iterator_info->key_rules_info[ii].type != DBAL_CONDITION_EQUAL_TO)
        {
            *use_flush_out = FALSE;
        }
    }

    for (ii = 0; (ii < iterator_info->nof_val_rules) && *use_flush_out; ii++)
    {
        if (iterator_info->val_rules_info[ii].type != DBAL_CONDITION_EQUAL_TO)
        {
            *use_flush_out = FALSE;
        }
    }
    if (entry_handle->table->key_size > dnx_data_mdb.em.flush_max_supported_key_get(unit))
    {
        *use_flush_out = FALSE;
    }

    for (ii = 0; ii < entry_handle->table->nof_result_types; ii++)
    {
        if (*use_flush_out)
        {
            int result_size = entry_handle->table->multi_res_info[ii].entry_payload_size;
            if ((result_size <= dnx_data_mdb.em.flush_max_supported_payload_get(unit)) &&
                ((entry_handle->table->key_size + result_size) <=
                 dnx_data_mdb.em.flush_max_supported_key_plus_payload_get(unit)))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(result_types_in_flush_bitmap, ii));
                ++(*nof_flush_rules);
            }
            else
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&mdb_iterator_info->result_types_in_non_flush_bitmap, ii));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&mdb_iterator_info->result_types_in_non_flush_bitmap, ii));
        }
    }

    for (ii = 0; ii < iterator_info->nof_val_rules; ii++)
    {
        if (iterator_info->val_rules_info[ii].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            if (has_rt_rule == FALSE)
            {

                has_rt_rule = TRUE;

                result_type_value_in_rule = iterator_info->val_rules_info[ii].value[0];

                if (*use_flush_out
                    && utilex_bitstream_test_bit(result_types_in_flush_bitmap, result_type_value_in_rule))
                {
                    *nof_flush_rules = 1;
                    *result_types_in_flush_bitmap = (1 << result_type_value_in_rule);
                    mdb_iterator_info->result_types_in_non_flush_bitmap = 0;
                }
                else
                {
                    *nof_flush_rules = 0;
                    *result_types_in_flush_bitmap = 0;
                    mdb_iterator_info->result_types_in_non_flush_bitmap = (1 << result_type_value_in_rule);
                }
            }
            else if (*use_flush_out)
            {

                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Two Result type rules with different values cannot be set. val1=%d, val2=%d",
                             result_type_value_in_rule, iterator_info->val_rules_info[ii].value[0]);
            }
        }
    }

    if (*nof_flush_rules == 0)
    {
        *use_flush_out = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_global_rif_add(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_table_actions_t * physical_table_actions,
    uint32 app_id,
    dbal_physical_tables_e * physical_db_ids,
    uint32 entry_flags)
{
    int global_rif, profile_width;
    dbal_table_field_info_t field_info;
    dbal_entry_handle_t *tmp_entry_handle = NULL;
    int entry_exists;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(tmp_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);

    sal_memcpy(tmp_entry_handle, entry_handle, sizeof(*entry_handle));
    global_rif = entry_handle->phy_entry.key[0];
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, entry_handle->table_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, 0, entry_handle->cur_res_type, 0,
                     &field_info));

    profile_width = field_info.field_nof_bits;

    tmp_entry_handle->phy_entry.payload_size = dnx_data_lif.out_lif.glem_result_get(unit);

    if (dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit))
    {
        int idx, entry_base = global_rif - (global_rif % profile_width);
        tmp_entry_handle->phy_entry.payload_size = dnx_data_lif.out_lif.glem_result_get(unit);

        for (idx = 0; idx < 4; idx++)
        {
            tmp_entry_handle->phy_entry.key[0] = entry_base + idx;
            SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, tmp_entry_handle, &entry_exists));
            if (entry_exists)
            {
                break;
            }
        }
        tmp_entry_handle->phy_entry.key[0] = global_rif - (global_rif % profile_width);
        if (!entry_exists)
        {

            tmp_entry_handle->phy_entry.payload[0] =
                tmp_entry_handle->phy_entry.payload[0] << (DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif));
        }
        else
        {
            uint32 payload = tmp_entry_handle->phy_entry.payload[0];
            int start_bit = DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif);

            SHR_IF_ERR_EXIT(physical_table_actions->entry_get
                            (unit, physical_db_ids[0], app_id, &tmp_entry_handle->phy_entry, 0));

            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                            (tmp_entry_handle->phy_entry.payload, start_bit,
                             start_bit + dnx_data_lif.out_lif.outrif_profile_width_get(unit) - 1));

            tmp_entry_handle->phy_entry.payload[0] |= (payload << (DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif)));
        }
    }
    else
    {

        DBAL_MDB_ACCESS_ALIGNED_PAYLOAD_SET(tmp_entry_handle->phy_entry.payload[0], global_rif);
    }
    tmp_entry_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_FORCE;
    SHR_IF_ERR_EXIT(physical_table_actions->entry_add
                    (unit, physical_db_ids, app_id, &tmp_entry_handle->phy_entry, entry_flags));

exit:
    SHR_FREE(tmp_entry_handle);
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_global_rif_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_table_actions_t * physical_table_actions,
    uint32 app_id,
    dbal_physical_tables_e * physical_db_id,
    uint32 entry_flags)
{
    int global_rif;
    int entry_exists;
    int payload_size = entry_handle->phy_entry.payload_size;

    SHR_FUNC_INIT_VARS(unit);

    global_rif = entry_handle->phy_entry.key[0];

    SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, entry_handle, &entry_exists));
    if (!entry_exists)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    entry_handle->phy_entry.payload_size = dnx_data_lif.out_lif.glem_result_get(unit);
    if (dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit))
    {
        entry_handle->phy_entry.key[0] = global_rif - DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif);
    }

    SHR_IF_ERR_EXIT(physical_table_actions->entry_get(unit, physical_db_id[0], app_id, &entry_handle->phy_entry,
                                                      entry_flags));

    DBAL_MDB_ACCESS_ALIGNED_PAYLOAD_GET(entry_handle->phy_entry.payload[0], global_rif);

exit:
    entry_handle->phy_entry.key[0] = global_rif;
    entry_handle->phy_entry.payload_size = payload_size;
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_global_rif_delete(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_table_actions_t * physical_table_actions,
    uint32 app_id,
    dbal_physical_tables_e * physical_db_id,
    uint32 entry_flags)
{
    int global_rif, profile_width;
    dbal_table_field_info_t field_info;
    int entry_exists;
    int payload_size = entry_handle->phy_entry.payload_size;

    SHR_FUNC_INIT_VARS(unit);

    global_rif = entry_handle->phy_entry.key[0];
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, entry_handle->table_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, 0, entry_handle->cur_res_type, 0,
                     &field_info));

    profile_width = field_info.field_nof_bits;

    SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, entry_handle, &entry_exists));
    if (!entry_exists)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    entry_handle->phy_entry.payload_size = dnx_data_lif.out_lif.glem_result_get(unit);
    if (dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit))
    {
        entry_handle->phy_entry.key[0] = global_rif - DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif);
    }

    if (dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit))
    {
        dbal_entry_handle_t tmp_entry_handle;
        int num_entries = 0;
        int idx, entry_base = global_rif - DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif);

        sal_memcpy(&tmp_entry_handle, entry_handle, sizeof(*entry_handle));
        tmp_entry_handle.phy_entry.payload_size = dnx_data_lif.out_lif.glem_result_get(unit);

        for (idx = 0; idx < 4; idx++)
        {
            tmp_entry_handle.phy_entry.key[0] = entry_base + idx;
            SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, &tmp_entry_handle, &entry_exists));
            if (entry_exists)
            {
                num_entries++;
            }
        }

        if (num_entries == 1)
        {
            SHR_IF_ERR_EXIT(physical_table_actions->entry_delete(unit, physical_db_id, app_id, &entry_handle->phy_entry,
                                                                 entry_flags));
        }

        else
        {

            tmp_entry_handle.phy_entry.key[0] = global_rif - DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif);
            SHR_IF_ERR_EXIT(physical_table_actions->entry_get
                            (unit, physical_db_id[0], app_id, &tmp_entry_handle.phy_entry, 0));
            SHR_IF_ERR_EXIT(utilex_bitstream_clear
                            (&tmp_entry_handle.phy_entry.payload[0] + DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif),
                             profile_width));

            tmp_entry_handle.phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_UPDATE;
            SHR_IF_ERR_EXIT(physical_table_actions->entry_add(unit, physical_db_id, app_id, &tmp_entry_handle.phy_entry,
                                                              entry_flags));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(physical_table_actions->entry_delete(unit, physical_db_id, app_id, &entry_handle->phy_entry,
                                                             entry_flags));
    }

exit:
    entry_handle->phy_entry.key[0] = global_rif;
    entry_handle->phy_entry.payload_size = payload_size;
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_get_next(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_entry_t * phy_entry,
    dbal_iterator_info_t * iterator_info)
{
    uint8 entry_found = FALSE;
    dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    while (!entry_found && !iterator_info->is_end)
    {

        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_next(unit, entry_handle));
            iterator_info->used_first_key = 1;
            if (!iterator_info->is_end)
            {
                SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get(unit, entry_handle));
                entry_found = TRUE;
                if (table->core_mode == DBAL_CORE_MODE_DPC)
                {
                    iterator_info->physical_db_index = entry_handle->core_id;
                }
            }
        }
        else if (table->allocator_field_id != DBAL_FIELD_EMPTY)
        {

            int key_bigger_then_capacity = 0;
            entry_found = TRUE;
            if (iterator_info->used_first_key)
            {
                entry_handle->phy_entry.key[0]++;
                if (entry_handle->phy_entry.key[0] > iterator_info->max_num_of_iterations)
                {
                    if ((table->core_mode == DBAL_CORE_MODE_SBC) || (entry_handle->core_id == 0))
                    {
                        key_bigger_then_capacity = 1;
                        entry_found = FALSE;
                    }
                    else
                    {
                        --entry_handle->core_id;
                        entry_handle->phy_entry.key[0] = 0;
                    }
                }
            }
            iterator_info->used_first_key = 1;

            if (!key_bigger_then_capacity)
            {
                if ((dbal_table_is_out_lif(table)) || (dbal_table_is_in_lif(table)))
                {
                    entry_handle->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;
                }
                SHR_IF_ERR_EXIT(dbal_iterator_increment_by_allocator(unit, entry_handle, &is_valid_entry));
            }

            if ((is_valid_entry == DBAL_KEY_IS_VALID) && (entry_found))
            {
                int rv = dbal_mdb_table_entry_get(unit, entry_handle);
                if (!rv)
                {
                    if (sal_memcmp
                        (zero_buffer_to_compare, entry_handle->phy_entry.payload,
                         DBAL_TABLE_BUFFER_IN_BYTES(entry_handle->table)) == 0)
                    {

                        entry_found = FALSE;
                    }
                    else
                    {
                        entry_found = TRUE;
                    }
                }
                else
                {
                    if (rv == _SHR_E_NOT_FOUND)
                    {

                        entry_found = FALSE;
                    }
                    else
                    {

                        SHR_ERR_EXIT(rv, "internal error in entry get");
                    }
                }
            }
            else
            {
                iterator_info->is_end = 1;
            }
        }
        else
        {

            dbal_physical_table_actions_t *physical_table_actions = NULL;
            SHR_IF_ERR_EXIT(dbal_mdb_phy_table_actions_get(unit, physical_db_id, &physical_table_actions));

            DBAL_MDB_ACCESS_ACTION_VALIDATE(physical_table_actions->iterator_get_next, physical_db_id);

            if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
            {
                entry_handle->cur_res_type = 0;
            }

            SHR_IF_ERR_EXIT(physical_table_actions->iterator_get_next
                            (unit, physical_db_id, app_id, &iterator_info->mdb_iterator, &entry_handle->phy_entry,
                             &iterator_info->is_end));
            iterator_info->used_first_key = 1;
            if (!iterator_info->is_end)
            {
                if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
                {
                    int rv;

                    rv = dbal_mdb_res_type_resolution(unit, entry_handle, FALSE);
                    if (rv == _SHR_E_NONE)
                    {
                        entry_found = TRUE;
                    }
                    else if (rv != _SHR_E_NOT_FOUND)
                    {
                        SHR_IF_ERR_EXIT(rv);
                    }
                }
                else
                {
                    entry_found = TRUE;
                }
            }
        }
    }

    if (table->table_type == DBAL_TABLE_TYPE_TCAM)
    {
        entry_handle->access_id_set = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_iterator_user_info_copy(
    int unit,
    dbal_physical_entry_iterator_t * src,
    dbal_physical_entry_iterator_t * dst)
{
    SHR_FUNC_INIT_VARS(unit);

    dst->age_flags = src->age_flags;
    dst->mdb_action_apply = src->mdb_action_apply;
    dst->hit_bit_flags = src->hit_bit_flags;

    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_first_iteration_init(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_iterator_info_t * iterator_info)
{
    uint32 is_action_get;
    uint32 is_action_read_to_dma;
    uint8 is_non_blocking = 0;
    uint32 result_types_in_flush_bitmap[1];
    int nof_flush_rules = 0;

    SHR_FUNC_INIT_VARS(unit);

    is_action_get = iterator_info->actions_bitmap & DBAL_ITER_ACTION_GET;
    is_action_read_to_dma = iterator_info->actions_bitmap & DBAL_ITER_ACTION_READ_TO_DMA;

    if (iterator_info->mode == DBAL_ITER_MODE_ACTION_NON_BLOCKING)
    {
        is_non_blocking = 1;
    }

    iterator_info->mdb_iterator.hit_bit_flags = entry_handle->phy_entry.hitbit;
    SHR_IF_ERR_EXIT(dbal_mdb_iterator_is_flush_machine_can_be_used
                    (unit, entry_handle, &iterator_info->mdb_iterator.iterate_in_flush_machine,
                     result_types_in_flush_bitmap, &nof_flush_rules));

    if (is_non_blocking && !iterator_info->mdb_iterator.iterate_in_flush_machine)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Non-blocking iterator cannot work with given iterator information. "
                     "Iterator information cannot be run over flush machine, table %s\n",
                     entry_handle->table->table_name);
    }

    if (iterator_info->mdb_iterator.iterate_in_flush_machine)
    {
        dbal_tables_e table_id = entry_handle->table_id;
        dnx_l2_action_type_t l2_action_type =
            dnx_data_l2.general.lem_table_map_l2_action_get(unit, table_id)->l2_action_type;
        SHR_IF_ERR_EXIT(dbal_iterator_rules_shadow_set
                        (unit, entry_handle, iterator_info->actions_bitmap, result_types_in_flush_bitmap[0],
                         nof_flush_rules));

        SHR_IF_ERR_EXIT(dbal_flush_start
                        (unit, FALSE, (uint8) !(is_action_get || is_action_read_to_dma), is_non_blocking,
                         l2_action_type));

        if (!is_action_get && !is_non_blocking)
        {

            SHR_IF_ERR_EXIT(dbal_flush_end(unit, l2_action_type));
        }
        if (is_non_blocking)
        {
            iterator_info->is_end = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_entry_hitbit_get_over_physical_tables(
    int unit,
    dbal_logical_table_t * table,
    dbal_physical_entry_t * phy_entry,
    uint32 physical_table_idx_to_start,
    uint32 app_id)
{
    uint32 physical_table_iter = physical_table_idx_to_start;
    dbal_physical_table_actions_t *physical_table_actions = NULL;
    uint16 hitbit_temp = phy_entry->hitbit;

    SHR_FUNC_INIT_VARS(unit);

    while (((hitbit_temp & DBAL_PHYSICAL_KEY_PRIMARY_AND_SECONDARY_HITBIT_ACCESSED) !=
            DBAL_PHYSICAL_KEY_PRIMARY_AND_SECONDARY_HITBIT_ACCESSED)
           && (physical_table_iter < table->nof_physical_tables))
    {
        SHR_IF_ERR_EXIT(dbal_mdb_phy_table_actions_get
                        (unit, table->physical_db_id[physical_table_iter], &physical_table_actions));

        DBAL_MDB_ACCESS_ACTION_VALIDATE(physical_table_actions->entry_get, table->physical_db_id[physical_table_iter]);

        SHR_IF_ERR_EXIT(physical_table_actions->entry_get(unit,
                                                          table->physical_db_id[physical_table_iter],
                                                          app_id, phy_entry, 0));

        hitbit_temp |= phy_entry->hitbit;
        physical_table_iter++;
    }

    phy_entry->hitbit = hitbit_temp;

exit:
    SHR_FUNC_EXIT;
}

#define DBAL_MDB_UPDATE_ACTION_APPLY(unit, entry_handle)\
    {\
        uint8 is_mact_dynamic = 0;\
        SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_check(unit, entry_handle, &is_mact_dynamic));\
        if (is_mact_dynamic)\
        {\
            entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_NONE;\
        }\
    }(void)0

static shr_error_e
dbal_mdb_table_entry_get_next_by_sw(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *continue_calling_mdb_get_next)
{
    dbal_logical_table_t *table;
    dbal_iterator_info_t *iterator_info;
    dbal_physical_entry_iterator_t *mdb_iterator_info;
    dbal_physical_entry_t *phy_entry;
    uint32 result_types_to_find = 0xFFFFFFFF;
    uint32 app_id;
    uint32 is_action_get;
    uint32 perform_action;

    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;

    *continue_calling_mdb_get_next = FALSE;

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    phy_entry = &entry_handle->phy_entry;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    is_action_get = iterator_info->actions_bitmap & DBAL_ITER_ACTION_GET;
    perform_action = iterator_info->actions_bitmap & ~DBAL_ITER_ACTION_GET;
    mdb_iterator_info = &iterator_info->mdb_iterator;

    if (iterator_info->attrib_info.age_rule_valid)
    {
        iterator_info->mdb_iterator.age_flags = DBAL_PHYSICAL_KEY_AGE_GET;
    }
    SHR_IF_ERR_EXIT(dbal_mdb_access_get_next(unit, app_id, table->physical_db_id[iterator_info->physical_db_index],
                                             entry_handle, phy_entry, iterator_info));

    if (((!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
         && (table->core_mode == DBAL_CORE_MODE_DPC) && (DBAL_MAX_NUM_OF_CORES > 1)))
    {
        dbal_physical_entry_iterator_t iterator_info_bkp;
        while (iterator_info->is_end)
        {
            SHR_IF_ERR_EXIT(dbal_mdb_table_iterator_deinit(unit, entry_handle));

            if (!dnx_data_dbal.table.feature_get(unit, dnx_data_dbal_table_support_mdb_general_phys_db))
            {
                if (iterator_info->physical_db_index < (table->nof_physical_tables - 1))
                {
                    ++iterator_info->physical_db_index;
                }
                else
                {
                    break;
                }
            }

            else if (iterator_info->core_id < (DBAL_MAX_NUM_OF_CORES - 1))
            {
                ++iterator_info->core_id;
            }
            else
            {
                break;
            }

            iterator_info->is_end = FALSE;

            SHR_IF_ERR_EXIT(dbal_mdb_iterator_user_info_copy(unit, &iterator_info->mdb_iterator, &iterator_info_bkp));
            SHR_IF_ERR_EXIT(dbal_mdb_table_iterator_init(unit, entry_handle));
            SHR_IF_ERR_EXIT(dbal_mdb_iterator_user_info_copy(unit, &iterator_info_bkp, &iterator_info->mdb_iterator));
            SHR_IF_ERR_EXIT(dbal_mdb_access_get_next
                            (unit, app_id, table->physical_db_id[iterator_info->physical_db_index], entry_handle,
                             phy_entry, iterator_info));
        }
    }

    if (!iterator_info->is_end)
    {
        dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;

        if (mdb_iterator_info->iterate_in_flush_machine)
        {
            result_types_to_find = mdb_iterator_info->result_types_in_non_flush_bitmap;
        }

        if (((table->core_mode == DBAL_CORE_MODE_DPC) && (DBAL_MAX_NUM_OF_CORES > 1)) &&
            (table->allocator_field_id == DBAL_FIELD_EMPTY) &&
            (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW)))
        {
            entry_handle->core_id = iterator_info->physical_db_index;
        }
        SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
        if (is_valid_entry == DBAL_KEY_IS_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB iterator found invalid entry\n");
        }
        else if (is_valid_entry == DBAL_KEY_IS_OUT_OF_ITERATOR_RULE)
        {
            *continue_calling_mdb_get_next = TRUE;
            SHR_EXIT();
        }

        else if (!utilex_bitstream_test_bit(&result_types_to_find, entry_handle->cur_res_type))
        {

            if (!is_action_get && perform_action)
            {
                entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
                DBAL_MDB_UPDATE_ACTION_APPLY(unit, entry_handle);
#endif
            }
            else
            {
                *continue_calling_mdb_get_next = TRUE;
            }
            SHR_EXIT();
        }
        *continue_calling_mdb_get_next = FALSE;

        entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;
        SHR_IF_ERR_EXIT(dbal_physical_entry_print(unit, entry_handle,
                                                  table->physical_db_id[iterator_info->physical_db_index],
                                                  &entry_handle->phy_entry, app_id, FALSE, "get next from"));
        if ((((table->core_mode == DBAL_CORE_MODE_SBC) || (DBAL_MAX_NUM_OF_CORES == 1)))
            && (iterator_info->mdb_iterator.hit_bit_flags & DBAL_PHYSICAL_KEY_HITBIT_GET))
        {

            SHR_IF_ERR_EXIT(dbal_mdb_entry_hitbit_get_over_physical_tables
                            (unit, table, phy_entry, iterator_info->physical_db_index + 1, app_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_table_entry_get_next_by_flush(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;
    dbal_iterator_attribute_info_t *iterator_attrib_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    iterator_attrib_info = &iterator_info->attrib_info;

    SHR_IF_ERR_EXIT(dbal_flush_entry_get(unit, entry_handle));
    if (!iterator_info->is_end)
    {

        if (iterator_attrib_info->hit_bit_action_get || iterator_attrib_info->age_rule_valid)
        {
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get(unit, entry_handle));
        }

        dbal_entry_handle_status_performed_set(entry_handle);

        entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
        DBAL_MDB_UPDATE_ACTION_APPLY(unit, entry_handle);
#endif
    }
    else
    {
        dbal_tables_e table_id = entry_handle->table_id;
        dnx_l2_action_type_t l2_action_type =
            dnx_data_l2.general.lem_table_map_l2_action_get(unit, table_id)->l2_action_type;

        SHR_IF_ERR_EXIT(dbal_flush_end(unit, l2_action_type));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_table_entry_get_next_inner(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *continue_calling_mdb_get_next)
{
    dbal_iterator_info_t *iterator_info;
    dbal_physical_entry_iterator_t *mdb_iterator_info;
    uint8 is_non_blocking = 0;
    uint32 app_id;
    uint32 is_action_get;
    uint8 to_read_entries_from_dma_only;

    SHR_FUNC_INIT_VARS(unit);

    *continue_calling_mdb_get_next = FALSE;

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    mdb_iterator_info = &iterator_info->mdb_iterator;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    is_action_get = iterator_info->actions_bitmap & DBAL_ITER_ACTION_GET;
    to_read_entries_from_dma_only = iterator_info->mode == DBAL_ITER_MODE_GET_ENTRIES_FROM_DMA_NO_TRIGGER;

    if (!iterator_info->used_first_key && !to_read_entries_from_dma_only)
    {
        if (iterator_info->mode == DBAL_ITER_MODE_ACTION_NON_BLOCKING)
        {
            is_non_blocking = 1;
        }
    }

    if ((is_action_get && !mdb_iterator_info->start_non_flush_iteration) || to_read_entries_from_dma_only)
    {
        SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next_by_flush(unit, entry_handle));
    }

    if ((is_non_blocking == 0) && !to_read_entries_from_dma_only &&
        ((iterator_info->is_end) || !is_action_get || mdb_iterator_info->start_non_flush_iteration))
    {
        if (!mdb_iterator_info->start_non_flush_iteration)
        {
            iterator_info->is_end = FALSE;
        }

        mdb_iterator_info->start_non_flush_iteration = TRUE;

        SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next_by_sw(unit, entry_handle, continue_calling_mdb_get_next));

        if (iterator_info->is_end)
        {
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;
    int continue_calling_mdb_get_next;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (!iterator_info->used_first_key && !(iterator_info->mode == DBAL_ITER_MODE_GET_ENTRIES_FROM_DMA_NO_TRIGGER))
    {
        SHR_IF_ERR_EXIT(dbal_mdb_first_iteration_init(unit, entry_handle, iterator_info));
    }

    if (iterator_info->mdb_iterator.iterate_in_flush_machine ||
        (iterator_info->mode == DBAL_ITER_MODE_GET_ENTRIES_FROM_DMA_NO_TRIGGER))
    {
        SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next_inner(unit, entry_handle, &continue_calling_mdb_get_next));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next_by_sw(unit, entry_handle, &continue_calling_mdb_get_next));
    }

    while (continue_calling_mdb_get_next == TRUE)
    {
        continue_calling_mdb_get_next = FALSE;
        if (iterator_info->mdb_iterator.iterate_in_flush_machine ||
            (iterator_info->mode == DBAL_ITER_MODE_GET_ENTRIES_FROM_DMA_NO_TRIGGER))
        {
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next_inner(unit, entry_handle, &continue_calling_mdb_get_next));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next_by_sw(unit, entry_handle, &continue_calling_mdb_get_next));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dbal_mdb_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_table_actions_t *physical_table_actions = NULL;
    dbal_physical_tables_e physical_db_id;
    uint32 app_id;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_iterator_init(unit, entry_handle));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, entry_handle->table_id,
                                                       iterator_info->physical_db_index, &physical_db_id));

        SHR_IF_ERR_EXIT(dbal_mdb_phy_table_actions_get(unit, physical_db_id, &physical_table_actions));

        DBAL_MDB_ACCESS_ACTION_VALIDATE(physical_table_actions->iterator_deinit, physical_db_id);

        DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

        SHR_IF_ERR_EXIT(physical_table_actions->iterator_deinit(unit, physical_db_id, app_id,
                                                                &iterator_info->mdb_iterator));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_action_apply_update(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_entry_t *phy_entry = &entry_handle->phy_entry;
    uint8 skip_shadow = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_LPM)
    {

        phy_entry->mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;
        SHR_EXIT();
    }

    if (DBAL_TABLE_IS_TCAM(entry_handle->table))
    {

        SHR_EXIT();
    }

    if (phy_entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
    {
        SHR_IF_ERR_EXIT(dbal_db.mdb_access.skip_read_from_shadow.get(unit, &skip_shadow));
        if (skip_shadow)
        {

            phy_entry->mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
        }
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
        else
        {
            if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_LEARNING_EN))
            {

                phy_entry->mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
            }
        }
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_inlif_res_type_handle_after_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int cur_res_type = entry_handle->cur_res_type;
    SHR_FUNC_INIT_VARS(unit);

    if (cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No need to do result type resolution for inlif %s",
                     entry_handle->table->table_name);
    }

    if ((entry_handle->table->multi_res_info[cur_res_type].entry_payload_size < entry_handle->table->max_payload_size))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                        (unit,
                         entry_handle->table->max_payload_size -
                         entry_handle->table->multi_res_info[cur_res_type].entry_payload_size, TRUE, entry_handle));
        entry_handle->phy_entry.payload_size = entry_handle->table->multi_res_info[cur_res_type].entry_payload_size;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_outlif_res_type_handle_after_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 is_eedb_ll)
{
    uint8 rt_has_link_list = FALSE;
    dbal_physical_entry_t *phy_entry = &entry_handle->phy_entry;

    SHR_FUNC_INIT_VARS(unit);

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        dbal_table_field_info_t *table_field_info;
        int curr_res_type = entry_handle->cur_res_type;

        table_field_info = &(entry_handle->table->multi_res_info[curr_res_type].results_info[0]);

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (&entry_handle->table->multi_res_info[curr_res_type].result_type_hw_value[0],
                         table_field_info->bits_offset_in_buffer, table_field_info->field_nof_bits,
                         phy_entry->payload));
    }

    SHR_IF_ERR_EXIT(dnx_lif_mngr_eedb_table_rt_has_linked_list
                    (unit, entry_handle->table_id, entry_handle->cur_res_type, &rt_has_link_list));
    if ((is_eedb_ll == FALSE) && rt_has_link_list)
    {

        SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align(unit, MDB_DIRECT_BASIC_ENTRY_SIZE, FALSE, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_access_get(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle,
    uint8 is_eedb_ll)
{
    int rv;
    dbal_physical_table_actions_t *physical_table_actions = NULL;
    dbal_physical_entry_t *phy_entry = &entry_handle->phy_entry;
    uint32 entry_flags[1] = { 0 };
    uint8 orig_access_apply_type = phy_entry->mdb_action_apply;

    SHR_FUNC_INIT_VARS(unit);

    if (is_eedb_ll)
    {
        SHR_BITSET(entry_flags, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS);
    }

    SHR_IF_ERR_EXIT(dbal_mdb_phy_table_actions_get(unit, physical_db_id, &physical_table_actions));

    DBAL_MDB_ACCESS_ACTION_VALIDATE(physical_table_actions->entry_get, physical_db_id);

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal result type for entry table %s\n", entry_handle->table->table_name);
        }

        phy_entry->payload_size -= DBAL_RES_INFO.results_info[0].field_nof_bits;
    }

    SHR_IF_ERR_EXIT(dbal_mdb_access_action_apply_update(unit, entry_handle));

    if ((entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM) && (entry_handle->access_id_set == 0))
    {
        SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(dbal_mdb_table_access_id_by_key_get(unit, entry_handle), _SHR_E_NOT_FOUND);
    }

    if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
    {

        rv = dbal_mdb_global_rif_get(unit, entry_handle, physical_table_actions, app_id, &physical_db_id,
                                     entry_flags[0]);
    }
    else
    {
        rv = physical_table_actions->entry_get(unit, physical_db_id, app_id, phy_entry, entry_flags[0]);
        if ((rv != _SHR_E_NOT_FOUND) &&
            (((entry_handle->table->core_mode == DBAL_CORE_MODE_SBC) || (DBAL_MAX_NUM_OF_CORES == 1))
             && (entry_handle->phy_entry.hitbit & DBAL_PHYSICAL_KEY_HITBIT_GET)))
        {

            SHR_IF_ERR_EXIT(dbal_mdb_entry_hitbit_get_over_physical_tables
                            (unit, entry_handle->table, phy_entry, 1, app_id));
        }

    }

    phy_entry->mdb_action_apply = orig_access_apply_type;
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (DBAL_TABLE_IS_TCAM(entry_handle->table) && entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        entry_handle->core_id = phy_entry->core_id;
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        if (dbal_table_is_out_lif(entry_handle->table))
        {
            {
                SHR_IF_ERR_EXIT(dbal_mdb_outlif_res_type_handle_after_get(unit, entry_handle, is_eedb_ll));
            }
        }
        else
        {
            if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
            {
                dbal_table_field_info_t *table_field_info;
                int curr_res_type = entry_handle->cur_res_type;

                if (entry_handle->table->multi_res_info[curr_res_type].entry_payload_size <
                    entry_handle->table->max_payload_size)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                                    (unit,
                                     entry_handle->table->max_payload_size -
                                     entry_handle->table->multi_res_info[curr_res_type].entry_payload_size, TRUE,
                                     entry_handle));
                }

                table_field_info = &(entry_handle->table->multi_res_info[curr_res_type].results_info[0]);

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (&entry_handle->table->multi_res_info[curr_res_type].result_type_hw_value[0],
                                 table_field_info->bits_offset_in_buffer, table_field_info->field_nof_bits,
                                 phy_entry->payload));
            }
            else if (dbal_table_is_in_lif(entry_handle->table))
            {
                SHR_IF_ERR_EXIT(dbal_mdb_inlif_res_type_handle_after_get(unit, entry_handle));
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(dbal_mdb_res_type_resolution(unit, entry_handle, FALSE),
                                                   _SHR_E_NOT_FOUND);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_physical_entry_t *phy_entry;
    uint32 app_id;
    uint8 rt_has_link_list = FALSE;
    uint8 is_eedb_ll = FALSE;
    dbal_physical_tables_e eedb_physical_table, physical_table;

    SHR_FUNC_INIT_VARS(unit);
    table = entry_handle->table;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    phy_entry = &entry_handle->phy_entry;
    phy_entry->core_id = entry_handle->core_id;
    phy_entry->payload_size = entry_handle->table->max_payload_size;
    phy_entry->key_size = entry_handle->table->key_size;
    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        phy_entry->key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

    if (dbal_table_is_out_lif(table))
    {
        if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "handle result type not initialized table %s\n",
                         entry_handle->table->table_name);
        }

        phy_entry->payload_size = DBAL_RES_INFO.entry_payload_size;

        SHR_IF_ERR_EXIT(dbal_outlif_sw_info_update_payload_size
                        (unit, entry_handle->table_id, entry_handle->cur_res_type, phy_entry, &rt_has_link_list,
                         &eedb_physical_table, &is_eedb_ll));

        physical_table = eedb_physical_table;
    }
    else if (!dnx_data_dbal.table.feature_get(unit, dnx_data_dbal_table_support_mdb_general_phys_db)
             && (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC) && (entry_handle->core_id != DBAL_CORE_ALL)
             && (DBAL_MAX_NUM_OF_CORES > 1))
    {
        physical_table = table->physical_db_id[entry_handle->phy_entry.core_id];
    }
    else
    {

        physical_table = table->physical_db_id[0];
    }

    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(dbal_mdb_access_get
                                       (unit, app_id, physical_table, entry_handle, is_eedb_ll), _SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT(dbal_physical_entry_print
                    (unit, entry_handle, physical_table, &entry_handle->phy_entry, app_id, FALSE, "get from"));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_delete_one_table(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_ids[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    dbal_entry_handle_t * entry_handle,
    dbal_physical_entry_t * phy_entry,
    uint8 is_eedb_ll)
{
    dbal_physical_table_actions_t *physical_table_actions = NULL;
    uint32 entry_flags[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_mdb_phy_table_actions_get(unit, physical_db_ids[0], &physical_table_actions));

    DBAL_MDB_ACCESS_ACTION_VALIDATE(physical_table_actions->entry_delete, physical_db_ids[0]);

    if (is_eedb_ll)
    {
        SHR_BITSET(entry_flags, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS);
    }

    if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
    {

        SHR_IF_ERR_EXIT(dbal_mdb_global_rif_delete
                        (unit, entry_handle, physical_table_actions, app_id, physical_db_ids, entry_flags[0]));
    }
    else
    {
        SHR_IF_ERR_EXIT(physical_table_actions->entry_delete(unit, physical_db_ids, app_id, phy_entry, entry_flags[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_out_lif_table_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint8 rt_has_link_list = FALSE;
    uint8 is_eedb_ll = FALSE;
    dbal_physical_tables_e eedb_physical_table = DBAL_PHYSICAL_TABLE_NONE;
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    uint32 app_id;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);
    entry_handle->phy_entry.payload_size = DBAL_RES_INFO.entry_payload_size;

    SHR_IF_ERR_EXIT(dbal_outlif_sw_info_update_payload_size
                    (unit, entry_handle->table_id, entry_handle->cur_res_type, &entry_handle->phy_entry,
                     &rt_has_link_list, &eedb_physical_table, &is_eedb_ll));
    if ((!is_eedb_ll) && (rt_has_link_list))
    {
        entry_handle->phy_entry.payload_size -= MDB_DIRECT_BASIC_ENTRY_SIZE;
    }

    SHR_IF_ERR_EXIT(dbal_physical_entry_print
                    (unit, entry_handle, eedb_physical_table, &entry_handle->phy_entry, app_id, TRUE, DELETE_MSG));

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_clear(unit, entry_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_delete(unit, entry_handle));
        }

        entry_handle->phy_entry.payload_size -=
            entry_handle->table->multi_res_info[entry_handle->cur_res_type].results_info[0].field_nof_bits;
    }

    physical_tables[0] = eedb_physical_table;
    SHR_IF_ERR_EXIT(dbal_mdb_access_delete_one_table
                    (unit, app_id, physical_tables, entry_handle, &entry_handle->phy_entry, is_eedb_ll));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int ii = 0;
    uint32 app_id;
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    int support_mdb_general_phys_db =
        dnx_data_dbal.table.feature_get(unit, dnx_data_dbal_table_support_mdb_general_phys_db);
    dbal_physical_entry_t *phy_entry = &entry_handle->phy_entry;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    if (!support_mdb_general_phys_db)
    {

        if ((entry_handle->core_id == DBAL_CORE_ALL) || (DBAL_MAX_NUM_OF_CORES == 1))
        {
            for (ii = 0; ii < entry_handle->table->nof_physical_tables; ++ii)
            {
                physical_tables[0] = entry_handle->table->physical_db_id[ii];
                SHR_IF_ERR_EXIT(dbal_mdb_access_delete_one_table
                                (unit, app_id, physical_tables, entry_handle, phy_entry, 0));
            }
        }

        else
        {
            physical_tables[0] = entry_handle->table->physical_db_id[entry_handle->core_id];
            SHR_IF_ERR_EXIT(dbal_mdb_access_delete_one_table
                            (unit, app_id, physical_tables, entry_handle, phy_entry, 0));
        }
    }
    else
    {

        physical_tables[0] = entry_handle->table->physical_db_id[0];
        SHR_IF_ERR_EXIT(dbal_mdb_access_delete_one_table(unit, app_id, physical_tables, entry_handle, phy_entry, 0));
        if (entry_handle->table->core_mode == DBAL_CORE_MODE_SBC)
        {
            for (ii = 1; ii < entry_handle->table->nof_physical_tables; ++ii)
            {
                physical_tables[0] = entry_handle->table->physical_db_id[ii];
                SHR_IF_ERR_EXIT(dbal_mdb_access_delete_one_table
                                (unit, app_id, physical_tables, entry_handle, phy_entry, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_physical_entry_t *phy_entry;
    uint32 app_id;
    uint32 payload_size_to_delete = 0;
    dbal_physical_tables_e first_phy_table = DBAL_PHYSICAL_TABLE_NONE;
    int support_mdb_general_phys_db =
        dnx_data_dbal.table.feature_get(unit, dnx_data_dbal_table_support_mdb_general_phys_db);

    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    phy_entry = &entry_handle->phy_entry;
    phy_entry->key_size = table->key_size;
    phy_entry->core_id = entry_handle->core_id;

    if (dbal_table_is_out_lif(table))
    {
        SHR_IF_ERR_EXIT(dbal_mdb_out_lif_table_entry_delete(unit, entry_handle));
        SHR_EXIT();
    }
    else if (dbal_table_is_in_lif(table))
    {
        payload_size_to_delete = DBAL_RES_INFO.entry_payload_size;
    }
    else
    {
        payload_size_to_delete = table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;

        if (table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_PPMC)
        {
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get(unit, entry_handle));
        }
    }

    phy_entry->payload_size = payload_size_to_delete;

    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        phy_entry->key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

    first_phy_table = table->physical_db_id[0];
    if (!support_mdb_general_phys_db
        && (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC) && (entry_handle->core_id != DBAL_CORE_ALL))
    {
        first_phy_table = table->physical_db_id[entry_handle->core_id];
    }

    SHR_IF_ERR_EXIT(dbal_physical_entry_print
                    (unit, entry_handle, first_phy_table, phy_entry, app_id, TRUE, DELETE_MSG));

    if ((entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM) && (entry_handle->access_id_set == 0))
    {
        SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(dbal_mdb_table_access_id_by_key_get(unit, entry_handle), _SHR_E_NOT_FOUND);
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_clear(unit, entry_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_delete(unit, entry_handle));
        }

        phy_entry->payload_size -=
            entry_handle->table->multi_res_info[entry_handle->cur_res_type].results_info[0].field_nof_bits;
    }

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_LPM)
    {
        phy_entry->hitbit = SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_HITBIT_EN);
        phy_entry->key_size = dnx_data_mdb.kaps.key_width_in_bits_get(unit) -
            entry_handle->table->keys_info[entry_handle->table->nof_key_fields - 1].bits_offset_in_buffer;
    }

    SHR_IF_ERR_EXIT(dbal_mdb_access_delete(unit, entry_handle));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_write_one_table(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_ids[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    dbal_entry_handle_t * entry_handle,
    uint8 is_eedb_ll)
{
    dbal_physical_table_actions_t *physical_table_actions = NULL;
    dbal_physical_entry_t *phy_entry = &entry_handle->phy_entry;
    uint32 entry_flags[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_mdb_phy_table_actions_get(unit, physical_db_ids[0], &physical_table_actions));

    DBAL_MDB_ACCESS_ACTION_VALIDATE(physical_table_actions->entry_add, physical_db_ids[0]);

    if (is_eedb_ll)
    {
        SHR_BITSET(entry_flags, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS);
    }

    if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
    {

        SHR_IF_ERR_EXIT(dbal_mdb_global_rif_add
                        (unit, entry_handle, physical_table_actions, app_id, physical_db_ids, entry_flags[0]));
    }
    else
    {
        SHR_IF_ERR_EXIT(physical_table_actions->entry_add(unit, physical_db_ids, app_id, phy_entry, entry_flags[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_out_lif_table_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint8 rt_has_link_list = FALSE;
    uint8 is_eedb_ll = FALSE;
    dbal_physical_tables_e eedb_physical_table = DBAL_PHYSICAL_TABLE_NONE;
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    int bsl_severity = 0;
    uint32 app_id;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    SHR_IF_ERR_EXIT(dbal_outlif_sw_info_update_payload_size
                    (unit, entry_handle->table_id, entry_handle->cur_res_type, &entry_handle->phy_entry,
                     &rt_has_link_list, &eedb_physical_table, &is_eedb_ll));
    if ((!is_eedb_ll) && (rt_has_link_list))
    {

        SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align(unit, MDB_DIRECT_BASIC_ENTRY_SIZE, TRUE, entry_handle));
    }
    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);
    if (bsl_severity >= bslSeverityInfo)
    {
        SHR_IF_ERR_EXIT(dbal_physical_entry_print
                        (unit, entry_handle, eedb_physical_table, &entry_handle->phy_entry, app_id, FALSE, ADD_MSG));
    }

    physical_tables[0] = eedb_physical_table;
    SHR_IF_ERR_EXIT(dbal_mdb_access_write_one_table(unit, app_id, physical_tables, entry_handle, is_eedb_ll));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_write(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int ii = 0;
    uint32 app_id;
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    int support_mdb_general_phys_db =
        dnx_data_dbal.table.feature_get(unit, dnx_data_dbal_table_support_mdb_general_phys_db);

    SHR_FUNC_INIT_VARS(unit);

    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    if (!support_mdb_general_phys_db)
    {

        if ((entry_handle->core_id == DBAL_CORE_ALL) || (DBAL_MAX_NUM_OF_CORES == 1))
        {
            for (ii = 0; ii < entry_handle->table->nof_physical_tables; ++ii)
            {
                physical_tables[0] = entry_handle->table->physical_db_id[ii];
                SHR_IF_ERR_EXIT(dbal_mdb_access_write_one_table(unit, app_id, physical_tables, entry_handle, 0));
            }
        }

        else
        {
            physical_tables[0] = entry_handle->table->physical_db_id[entry_handle->core_id];
            SHR_IF_ERR_EXIT(dbal_mdb_access_write_one_table(unit, app_id, physical_tables, entry_handle, 0));
        }
    }
    else
    {

        physical_tables[0] = entry_handle->table->physical_db_id[0];
        SHR_IF_ERR_EXIT(dbal_mdb_access_write_one_table(unit, app_id, physical_tables, entry_handle, 0));
        if (entry_handle->table->core_mode == DBAL_CORE_MODE_SBC)
        {
            for (ii = 1; ii < entry_handle->table->nof_physical_tables; ++ii)
            {
                physical_tables[0] = entry_handle->table->physical_db_id[ii];
                SHR_IF_ERR_EXIT(dbal_mdb_access_write_one_table(unit, app_id, physical_tables, entry_handle, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_physical_entry_t *phy_entry;
    uint32 app_id;
    dbal_physical_tables_e first_phy_table = DBAL_PHYSICAL_TABLE_NONE;
    int bsl_severity = 0;
    int support_mdb_general_phys_db =
        dnx_data_dbal.table.feature_get(unit, dnx_data_dbal_table_support_mdb_general_phys_db);

    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);
    phy_entry = &entry_handle->phy_entry;
    phy_entry->key_size = table->key_size;
    phy_entry->core_id = entry_handle->core_id;

    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        phy_entry->key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

    if (!(phy_entry->attr_flags & DBAL_ENTRY_ATTR_FLAG_NO_PAYLOAD))
    {
        phy_entry->payload_size = table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;
        if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
        {
            phy_entry->payload_size -= DBAL_RES_INFO.results_info[0].field_nof_bits;
        }
    }

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_LPM)
    {
        int hitbit;

        hitbit = SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_HITBIT_EN);
        phy_entry->hitbit = (hitbit != 0) ? DBAL_PHYSICAL_KEY_HITBIT_ACTION : 0;
        phy_entry->key_size = dnx_data_mdb.kaps.key_width_in_bits_get(unit) -
            entry_handle->table->keys_info[entry_handle->table->nof_key_fields - 1].bits_offset_in_buffer;
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_set(unit, entry_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_add(unit, entry_handle));
        }
    }

    if (dbal_table_is_out_lif(table))
    {
        SHR_IF_ERR_EXIT(dbal_mdb_out_lif_table_entry_add(unit, entry_handle));
        SHR_EXIT();
    }

    first_phy_table = table->physical_db_id[0];
    if (!support_mdb_general_phys_db
        && (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC) && (entry_handle->core_id != DBAL_CORE_ALL))
    {
        first_phy_table = table->physical_db_id[entry_handle->core_id];
    }

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);
    if (bsl_severity >= bslSeverityInfo)
    {
        SHR_IF_ERR_EXIT(dbal_physical_entry_print
                        (unit, entry_handle, first_phy_table, phy_entry, app_id, FALSE, ADD_MSG));
    }

    SHR_IF_ERR_EXIT(dbal_mdb_access_write(unit, entry_handle));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_table_clear(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_table_actions_t *physical_table_actions = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_mdb_phy_table_actions_get(unit, physical_db_id, &physical_table_actions));
    DBAL_MDB_ACCESS_ACTION_VALIDATE(physical_table_actions->table_clear, physical_db_id);

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_clear(unit, entry_handle));
    }

    physical_tables[0] = physical_db_id;

    SHR_IF_ERR_EXIT(physical_table_actions->table_clear(unit, physical_tables, app_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_access_table_clear_by_iter(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 iter_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry_handle->table_id, &iter_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, iter_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, iter_handle_id, DBAL_ITER_ACTION_DELETE));
    SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, iter_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table = entry_handle->table;
    uint32 app_id;
    int ii = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (table->table_type != DBAL_TABLE_TYPE_LPM)
    {
        SHR_IF_ERR_EXIT(dbal_mdb_access_table_clear_by_iter(unit, entry_handle));
    }
    else
    {
        DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

        for (ii = 0; ii < entry_handle->table->nof_physical_tables; ++ii)
        {
            SHR_IF_ERR_EXIT(dbal_mdb_access_table_clear(unit, app_id, table->physical_db_id[ii], entry_handle));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 tcam_handler_id;
    uint32 access_id;
    int core = 0;
    SHR_FUNC_INIT_VARS(unit);
    if (!DBAL_TABLE_IS_TCAM(entry_handle->table))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "get_access_id supported only for TCAM\n");
    }
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, entry_handle->table_id, &tcam_handler_id));
    SHR_IF_ERR_EXIT(dbal_physical_entry_print
                    (unit, entry_handle, entry_handle->table->physical_db_id[0], &entry_handle->phy_entry,
                     tcam_handler_id, TRUE, "Access ID get"));
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(dnx_field_tcam_access_key_to_access_id
                                       (unit, core, tcam_handler_id, entry_handle->phy_entry.key,
                                        entry_handle->phy_entry.k_mask, &access_id), _SHR_E_NOT_FOUND);
    entry_handle->phy_entry.entry_hw_id = (int) access_id;

exit:
    SHR_FUNC_EXIT;
}
