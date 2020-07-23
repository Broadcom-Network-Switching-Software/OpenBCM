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
#include <stdlib.h>
#include <shared/bsl.h>
#include "dbal_internal.h"
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <sal/appl/sal.h>
#include <soc/dnx/recovery/generic_state_journal.h>

#ifdef BCM_DNX_SUPPORT
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_hw_entity_porting.h>
#endif
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif

#include <bcm_int/dnx/init/init_time_analyzer.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <sal/appl/config.h>


static dbal_logical_tables_info_t *logical_tables_info[BCM_MAX_NUM_UNITS];

#define DBAL_TABLES_DYNAMIC_TABLES_PATH "/dbal_dynamic/tables"

#define DBAL_TABLE_ID_VALIDATE( table_id)                       \
        if (table_id >= dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))     \
        {                                                       \
            SHR_ERR_EXIT(_SHR_E_PARAM,"Illegal table_id %d, max=%d\n", table_id, dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit));\
        }

extern char *dbal_table_strings[DBAL_NOF_TABLES];

static shr_error_e dbal_tables_sw_state_restore(
    int unit);




static shr_error_e
dbal_tables_generic_table_xmls_info_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!DBAL_IS_JR2_A0)
    {

#ifdef BCM_DNX_SUPPORT
        if (DBAL_IS_DNX2(unit))
        {
            SHR_IF_ERR_EXIT(dbal_init_hw_entity_porting_init(unit, &(logical_tables_info[unit]->hl_porting_info)));
        }
#endif
    }
    SHR_IF_ERR_EXIT(dbal_db_init_hw_element_group_info_init(unit, &(logical_tables_info[unit]->groups_info)));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_tables_alloc(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(logical_tables_info[unit], sizeof(dbal_logical_tables_info_t), "dbal_logical_tables_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
    
    if (dnxc_wb_deinit_init_during_wb_test_get(unit) < 1)
    {
#endif 
        SHR_IF_ERR_EXIT(dbal_tables_alloc(unit));
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
    }
#endif 

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_DYNAMIC_INIT);
    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dbal_db_init_dynamic_dbal_tables_sw_state(unit));
    }
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_DYNAMIC_INIT);

    logical_tables_info[unit]->nof_tables_with_error = 0;
    logical_tables_info[unit]->hl_porting_info.nof_memories = 0;
    logical_tables_info[unit]->hl_porting_info.nof_registers = 0;

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_ALLOC_INIT);
    SHR_IF_ERR_EXIT(dbal_db_init_table_alloc(unit, logical_tables_info[unit]->logical_tables));
    SHR_IF_ERR_EXIT(dbal_db_init_dynamic_dbal_tables_memory(unit));
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_ALLOC_INIT);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_GENERIC_INIT);
    SHR_IF_ERR_EXIT(dbal_tables_generic_table_xmls_info_init(unit));
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_GENERIC_INIT);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT);
    SHR_IF_ERR_EXIT(dbal_db_init_logical_tables(unit, logical_tables_info[unit]->logical_tables));
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_VALIDATIONS);
    SHR_IF_ERR_EXIT(dbal_db_init_tables_logical_validation(unit, logical_tables_info[unit]->logical_tables));
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_VALIDATIONS);

    if (DBAL_DYNAMIC_XML_TABLES_ENABLED)
    {
        char file_path[RHNAME_MAX_SIZE] = { 0 };
        char *db_dir;
        uint8 dir_override = TRUE;

        
        db_dir = sal_config_get("dpp_db_path");
        if (ISEMPTY(db_dir) || (dbx_file_dir_exists(db_dir) == FALSE))
        {
            db_dir = getenv("DPP_DB_PATH");
            if (ISEMPTY(db_dir) || (dbx_file_dir_exists(db_dir) == FALSE))
            {
                dir_override = FALSE;
                sal_strncpy(file_path, "./db", RHNAME_MAX_SIZE - 1);
            }
        }

        if (dir_override)
        {
            sal_strncpy(file_path, db_dir, RHNAME_MAX_SIZE - 1);
        }

        sal_strncat_s(file_path, DBAL_TABLES_DYNAMIC_TABLES_PATH, sizeof(file_path));

        
        SHR_IF_ERR_EXIT(dbal_dynamic_updates_dispatcher
                        (unit, file_path, dnx_dynamic_xml_tables_update, "AppDbCatalog"));
    }

    if (logical_tables_info[unit]->nof_tables_with_error)
    {
        LOG_CLI((BSL_META("WARNING, %d DBAL tables had HW error, list of tables: dbal table info status=hwerr\n"),
                 logical_tables_info[unit]->nof_tables_with_error));
    }

    if (sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dbal_tables_sw_state_restore(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_porting_info_get(
    int unit,
    hl_porting_info_t ** hl_porting_info)
{
    SHR_FUNC_INIT_VARS(unit);

    (*hl_porting_info) = &(logical_tables_info[unit]->hl_porting_info);

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_groups_info_get(
    int unit,
    hl_groups_info_t ** hl_groups_info)
{
    SHR_FUNC_INIT_VARS(unit);

    (*hl_groups_info) = &(logical_tables_info[unit]->groups_info);

    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tables_group_info_verify(
    int unit,
    hl_groups_info_t * groups_info)
{

    SHR_FUNC_INIT_VARS(unit);

    if (groups_info->nof_groups == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "HL groups # is %d\n", groups_info->nof_groups);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_group_info_get(
    int unit,
    dbal_hw_entity_group_e hw_entity_group_id,
    hl_group_info_t ** group_info)
{
    hl_groups_info_t *groups_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_groups_info_get(unit, &groups_info));

    SHR_IF_ERR_EXIT(dbal_tables_group_info_verify(unit, groups_info));

    *group_info = &groups_info->group_info[hw_entity_group_id];

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tables_dbal_logical_table_t_clear(
    int unit,
    dbal_logical_table_t * table)
{

    dbal_table_field_info_t *results_info;
    int multi_res_index;
    int key_info_data_size_bytes;
    int results_info_data_size_bytes;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(table, _SHR_E_PARAM, "table");

    key_info_data_size_bytes =
        dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit) * sizeof(dbal_table_field_info_t);
    results_info_data_size_bytes =
        dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t);
    sal_memset(table->keys_info, 0, key_info_data_size_bytes);
    if (table->multi_res_info != NULL)
    {
        for (multi_res_index = 0; multi_res_index < table->nof_result_types; multi_res_index++)
        {
            results_info = table->multi_res_info[multi_res_index].results_info;
            sal_memset(&table->multi_res_info[multi_res_index], 0, sizeof(multi_res_info_t));
            table->multi_res_info[multi_res_index].results_info = results_info;
            sal_memset(table->multi_res_info[multi_res_index].results_info, 0, results_info_data_size_bytes);
        }
    }
#if defined(INCLUDE_KBP)
    if (table->kbp_handles)
    {
        sal_memset(table->kbp_handles, 0, sizeof(kbp_db_handles_t));
    }
#endif

    SHR_IF_ERR_EXIT(dbal_logical_dynamic_table_t_init(unit, table));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_deinit(
    int unit)
{
    int ii, jj, kk, qq;
    dbal_logical_table_t *table_entry;
    int nof_dynamic_tables_multi_result_types = dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit);
    int nof_tables = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (logical_tables_info[unit] == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "logical_tables_info not initialized for unit %d\n", unit);
    }

    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        table_entry = &logical_tables_info[unit]->logical_tables[ii];

        if (table_entry->table_labels)
        {
            SHR_FREE(table_entry->table_labels);
        }

        if (table_entry->keys_info)
        {
            SHR_FREE(table_entry->keys_info);
        }
        if (table_entry->multi_res_info)
        {
            for (jj = 0; jj < table_entry->nof_result_types; jj++)
            {
                SHR_FREE(table_entry->multi_res_info[jj].results_info);
            }
            
            if (SHR_IS_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE)
                && (ii < DBAL_NOF_STATIC_TABLES)
                && (sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") != 0))
            {
                SHR_FREE(table_entry->multi_res_info[table_entry->nof_result_types].results_info);
            }
            else if (sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") == 0)
            {
                int nof_multi_res_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
                for (jj = table_entry->nof_result_types; jj < nof_multi_res_types; jj++)
                {
                    SHR_FREE(table_entry->multi_res_info[jj].results_info);
                }
            }
            SHR_FREE(table_entry->multi_res_info);
        }

        if (table_entry->iterator_optimized)
        {
            int core_index;
            for (core_index = 0; core_index < DBAL_MAX_NUM_OF_CORES; core_index++)
            {
                SHR_FREE(table_entry->iterator_optimized[core_index]);
            }
            SHR_FREE(table_entry->iterator_optimized);
        }

        if (table_entry->access_method == DBAL_ACCESS_METHOD_TCAM_CS)
        {
            if (table_entry->tcam_cs_mapping)
            {
                SHR_FREE(table_entry->tcam_cs_mapping);
            }
        }

        if (table_entry->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
        {
            if (table_entry->hl_mapping_multi_res)
            {
                for (jj = 0; jj < table_entry->nof_result_types; jj++)
                {
                    for (kk = 0; kk < DBAL_NOF_HL_ACCESS_TYPES; kk++)
                    {
                        if (table_entry->hl_mapping_multi_res[jj].l2p_hl_info[kk].num_of_access_fields > 0)
                        {
                            for (qq = 0;
                                 qq < table_entry->hl_mapping_multi_res[jj].l2p_hl_info[kk].num_of_access_fields; qq++)
                            {
                                dbal_hl_l2p_field_info_t *l2p_fields_info =
                                    &table_entry->hl_mapping_multi_res[jj].l2p_hl_info[kk].l2p_fields_info[qq];

                                if (l2p_fields_info->reg != NULL)
                                {
                                    SHR_FREE(l2p_fields_info->reg);
                                }
                                else if (l2p_fields_info->memory != NULL)
                                {
                                    SHR_FREE(l2p_fields_info->memory);
                                }

                                if (l2p_fields_info->nof_conditions > 0)
                                {
                                    SHR_FREE(l2p_fields_info->mapping_condition);
                                }
                                SHR_FREE(l2p_fields_info->array_offset_info.formula);
                                SHR_FREE(l2p_fields_info->entry_offset_info.formula);
                                SHR_FREE(l2p_fields_info->data_offset_info.formula);
                                SHR_FREE(l2p_fields_info->block_index_info.formula);
                                SHR_FREE(l2p_fields_info->group_offset_info.formula);
                                SHR_FREE(l2p_fields_info->alias_data_offset_info.formula);
                            }
                            SHR_FREE(table_entry->hl_mapping_multi_res[jj].l2p_hl_info[kk].l2p_fields_info);
                        }
                    }
                }
                SHR_FREE(table_entry->hl_mapping_multi_res);
            }
        }
        if (table_entry->pemla_mapping.key_fields_mapping)
        {
            SHR_FREE(table_entry->pemla_mapping.key_fields_mapping);
        }
        if (table_entry->pemla_mapping.result_fields_mapping)
        {
            SHR_FREE(table_entry->pemla_mapping.result_fields_mapping);
        }
#if defined(INCLUDE_KBP)
        if (table_entry->kbp_handles)
        {
            SHR_FREE(table_entry->kbp_handles);
        }
#endif
        if (table_entry->sw_access_info.table_size_str)
        {
            SHR_FREE(table_entry->sw_access_info.table_size_str);
        }
        table_entry->maturity_level = DBAL_MATURITY_LOW;
    }

    
    for (ii = DBAL_NOF_TABLES; ii < nof_tables; ii++)
    {
        table_entry = &logical_tables_info[unit]->logical_tables[ii];

        if (table_entry->table_labels)
        {
            SHR_FREE(table_entry->table_labels);
        }

        if (table_entry->keys_info)
        {
            SHR_FREE(table_entry->keys_info);
        }
        if (table_entry->multi_res_info)
        {
            for (jj = 0; jj < nof_dynamic_tables_multi_result_types; jj++)
            {
                SHR_FREE(table_entry->multi_res_info[jj].results_info);
            }
            SHR_FREE(table_entry->multi_res_info);
        }
#if defined(INCLUDE_KBP)
        if (table_entry->kbp_handles)
        {
            SHR_FREE(table_entry->kbp_handles);
        }
#endif
        table_entry->maturity_level = DBAL_MATURITY_LOW;
    }

    if (logical_tables_info[unit]->groups_info.nof_groups > 0)
    {
        SHR_FREE(logical_tables_info[unit]->groups_info.group_info);
    }

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
    
    if (dnxc_wb_deinit_init_during_wb_test_get(unit) < 1)
    {
#endif 
        if (logical_tables_info[unit] != NULL)
        {
            SHR_FREE(logical_tables_info[unit]);
        }
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_tables_available_table_id_get(
    int unit,
    int is_xml_dynamic_loading,
    dbal_tables_e * table_id)
{
    dbal_table_status_e table_status;
    int table_index;
    int table_start_index;
    int nof_tables;

    SHR_FUNC_INIT_VARS(unit);

    table_start_index = (is_xml_dynamic_loading) ? DBAL_NOF_STATIC_TABLES : DBAL_NOF_TABLES;
    nof_tables =
        (is_xml_dynamic_loading) ? DBAL_NOF_TABLES : dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    for (table_index = table_start_index; table_index < nof_tables; table_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_index, &table_status));
        if (table_status == DBAL_TABLE_NOT_INITIALIZED)
        {
            (*table_id) = table_index;
            break;
        }
    }

    if (table_index == nof_tables)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No available table IDs\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_access_info_set(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e access_method,
    void *access_info)
{
    dbal_logical_table_t *table;
    int sw_state_table_index;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    if (table->access_method != access_method)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incompatible access methods table %s input %s\n",
                     dbal_access_method_to_string(unit, table->access_method),
                     dbal_access_method_to_string(unit, access_method));
    }

    
    if (table_id >= DBAL_NOF_TABLES)
    {
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_dbal_table_info_change(unit, table_id));
    }

    switch (access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            table->app_id = ((dbal_table_mdb_access_info_t *) (access_info))->app_id;
            table->nof_physical_tables = ((dbal_table_mdb_access_info_t *) (access_info))->nof_physical_tables;
            sal_memcpy(table->physical_db_id, ((dbal_table_mdb_access_info_t *) (access_info))->physical_db_id,
                       sizeof(dbal_physical_tables_e) * DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE);
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.set(unit, table_id,
                                                                       ((dbal_table_mdb_access_info_t
                                                                         *) (access_info))->tcam_handler_id));
            table->app_id_size = ((dbal_table_mdb_access_info_t *) (access_info))->app_id_size;

            
            if (DBAL_TABLE_IS_TCAM(table) && (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_TCAM))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "TCAM table must have DBAL_PHYSICAL_TABLE_TCAM in physical_db_id\n");
            }

            
            if (table_id >= DBAL_NOF_TABLES)
            {
                sw_state_table_index = table_id - DBAL_NOF_TABLES;

                SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.
                                data.set(unit, sw_state_table_index, (CONST dbal_logical_table_t *) table));
            }
            break;

        case DBAL_ACCESS_METHOD_KBP:
#if defined(INCLUDE_KBP)
            if (table->kbp_handles)
            {
                sal_memcpy(((kbp_db_handles_t *) table->kbp_handles), ((kbp_db_handles_t *) access_info),
                           sizeof(kbp_db_handles_t));
                
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "kbp_handles not allocated for table %s\n", table->table_name);
            }
#else
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP not compiled\n");
#endif
            break;
        case DBAL_ACCESS_METHOD_PEMLA:
        case DBAL_ACCESS_METHOD_TCAM_CS:
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        case DBAL_ACCESS_METHOD_SW_STATE:

            SHR_ERR_EXIT(_SHR_E_INTERNAL, "setting info not supported for %s \n",
                         dbal_access_method_to_string(unit, access_method));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal access method %d \n", access_method);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_access_info_get(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e access_method,
    void *access_info)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    if (table->access_method != access_method)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incompatible access methods table %s input %s\n",
                     dbal_access_method_to_string(unit, table->access_method),
                     dbal_access_method_to_string(unit, access_method));
    }

    switch (access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            ((dbal_table_mdb_access_info_t *) (access_info))->app_id = table->app_id;
            ((dbal_table_mdb_access_info_t *) (access_info))->nof_physical_tables = table->nof_physical_tables;
            sal_memcpy(((dbal_table_mdb_access_info_t *) (access_info))->physical_db_id, table->physical_db_id,
                       sizeof(dbal_physical_tables_e) * DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE);
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, table_id,
                                                                       &(((dbal_table_mdb_access_info_t
                                                                           *) (access_info))->tcam_handler_id)));
            ((dbal_table_mdb_access_info_t *) (access_info))->app_id_size = table->app_id_size;
            break;

        case DBAL_ACCESS_METHOD_KBP:
#if defined(INCLUDE_KBP)
            sal_memcpy(((kbp_db_handles_t *) (access_info)), ((kbp_db_handles_t *) (table->kbp_handles)),
                       sizeof(kbp_db_handles_t));
            break;
#endif
        case DBAL_ACCESS_METHOD_PEMLA:
        case DBAL_ACCESS_METHOD_TCAM_CS:
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        case DBAL_ACCESS_METHOD_SW_STATE:

            SHR_ERR_EXIT(_SHR_E_INTERNAL, "getting info not supported for %s \n",
                         dbal_access_method_to_string(unit, access_method));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "illegal access method %d \n", access_method);
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_table_res_type_sw_state_add(
    int unit,
    dbal_tables_e table_id,
    int res_type_index)
{
    dbal_logical_table_t *table;
    uint8 res_field_index;
    int res_type_wr_index;
    int nof_max_dynamic_result_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
    multi_res_info_t multi_res_info;
    dbal_multi_res_info_status_e res_info_status;

    SHR_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only table %s is supported. Table given %s.\n",
                     dbal_logical_table_to_string(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION),
                     dbal_logical_table_to_string(unit, table_id));
    }

    table = &logical_tables_info[unit]->logical_tables[table_id];

    
    for (res_type_wr_index = 0; res_type_wr_index < nof_max_dynamic_result_types; res_type_wr_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.
                        multi_res_info_status.get(unit, res_type_wr_index, &res_info_status));
        if (res_info_status == DBAL_MULTI_RES_INFO_AVAIL)
        {
            break;
        }
    }
    if (res_type_wr_index == nof_max_dynamic_result_types)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Cannot write any more dynamic result types to SW state. Table %s.\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    
    for (res_field_index = 0; res_field_index < table->multi_res_info[res_type_index].nof_result_fields;
         res_field_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.res_info_data.set
                        (unit, res_type_wr_index, res_field_index,
                         table->multi_res_info[res_type_index].results_info[res_field_index]));
    }
    
    multi_res_info = table->multi_res_info[res_type_index];

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.multi_res_info_data.set
                    (unit, res_type_wr_index, (CONST multi_res_info_t *) & multi_res_info));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.
                    multi_res_info_status.set(unit, res_type_wr_index, DBAL_MULTI_RES_INFO_USED));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_table_res_type_sw_state_get(
    int unit,
    dbal_tables_e table_id,
    int index,
    multi_res_info_t * multi_res_info)
{
    uint8 res_field_index;
    dbal_multi_res_info_status_e res_info_status;

    CONST multi_res_info_t *multi_res_swstate_info;

    SHR_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only table %s is supported. Table given %s.\n",
                     dbal_logical_table_to_string(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION),
                     dbal_logical_table_to_string(unit, table_id));
    }

    if (index >= dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.get(unit, index, &res_info_status));
    if (res_info_status == DBAL_MULTI_RES_INFO_AVAIL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else if (res_info_status != DBAL_MULTI_RES_INFO_USED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected value in res_info_status of index %d: %d. Table %s.\n",
                     index, res_info_status, dbal_logical_table_to_string(unit, table_id));
    }

    if (multi_res_info != NULL)
    {
        dbal_table_field_info_t *results_info;

        results_info = multi_res_info->results_info;

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.multi_res_info_data.get
                        (unit, index, &multi_res_swstate_info));
        sal_memcpy(multi_res_info, multi_res_swstate_info, sizeof(multi_res_info_t));

        multi_res_info->results_info = results_info;

        for (res_field_index = 0; res_field_index < multi_res_info->nof_result_fields; res_field_index++)
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.res_info_data.get
                            (unit, index, res_field_index, &(multi_res_info->results_info[res_field_index])));
        }

    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_table_res_type_sw_state_del(
    int unit,
    dbal_tables_e table_id,
    int res_type_hw_value)
{
    int res_type_del_index;
    int res_type_shift_index;
    int hw_value_index;
    int nof_max_dynamic_result_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
    multi_res_info_t multi_res_info;
    CONST multi_res_info_t *multi_res_swstate_info;

    dbal_multi_res_info_status_e res_info_status;
    uint8 res_field_index;
    dbal_table_field_info_t result_info;
    SHR_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only table %s is supported. Table given %s.\n",
                     dbal_logical_table_to_string(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION),
                     dbal_logical_table_to_string(unit, table_id));
    }

    for (res_type_del_index = 0; res_type_del_index < nof_max_dynamic_result_types; res_type_del_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.get
                        (unit, res_type_del_index, &res_info_status));
        if (res_info_status == DBAL_MULTI_RES_INFO_USED)
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.multi_res_info_data.get
                            (unit, res_type_del_index, &multi_res_swstate_info));
            sal_memcpy(&multi_res_info, multi_res_swstate_info, sizeof(multi_res_info_t));

            if (multi_res_info.result_type_nof_hw_values <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "result_type_nof_hw_values of index %d in sw state is %d, "
                             "must be at least one. Table %s.\n",
                             res_type_del_index, multi_res_info.result_type_nof_hw_values,
                             dbal_logical_table_to_string(unit, table_id));
            }
            for (hw_value_index = 0; hw_value_index < multi_res_info.result_type_nof_hw_values; hw_value_index++)
            {
                if (res_type_hw_value == multi_res_info.result_type_hw_value[hw_value_index])
                {
                    break;
                }
            }
            if (hw_value_index < multi_res_info.result_type_nof_hw_values)
            {
                break;
            }
        }
    }
    if (res_type_del_index == nof_max_dynamic_result_types)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "trying to delete unallocated result type hw value %d\n", res_type_hw_value);
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.set
                    (unit, res_type_del_index, DBAL_MULTI_RES_INFO_AVAIL));
    
    for (res_type_shift_index = res_type_del_index; res_type_shift_index < (nof_max_dynamic_result_types - 1);
         res_type_shift_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.get
                        (unit, res_type_shift_index + 1, &res_info_status));
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.set
                        (unit, res_type_shift_index, res_info_status));
        if (res_info_status == DBAL_MULTI_RES_INFO_USED)
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.multi_res_info_data.get
                            (unit, res_type_shift_index + 1, &multi_res_swstate_info));
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.multi_res_info_data.set
                            (unit, res_type_shift_index, multi_res_swstate_info));
            for (res_field_index = 0; res_field_index < multi_res_info.nof_result_fields; res_field_index++)
            {
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.res_info_data.get
                                (unit, res_type_shift_index + 1, res_field_index, &result_info));
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.res_info_data.set
                                (unit, res_type_shift_index, res_field_index, result_info));
            }
        }
        else if (res_info_status != DBAL_MULTI_RES_INFO_AVAIL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "result type %d in SW state has unrecognized status %d Table %s.\n",
                         res_info_status, res_info_status, dbal_logical_table_to_string(unit, table_id));
        }
    }
    
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.set
                    (unit, res_type_shift_index, DBAL_MULTI_RES_INFO_AVAIL));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_table_res_type_sw_state_delete_all(
    int unit,
    dbal_tables_e table_id)
{
    int res_type_index;
    int nof_max_dynamic_result_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only table %s is supported. Table given %s.\n",
                     dbal_logical_table_to_string(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION),
                     dbal_logical_table_to_string(unit, table_id));
    }

    for (res_type_index = 0; res_type_index < nof_max_dynamic_result_types; res_type_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.
                        multi_res_info_status.set(unit, res_type_index, DBAL_MULTI_RES_INFO_AVAIL));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_table_interface_to_sw_state_add(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;
    int sw_state_table_index;
    int label_index;
    int key_index;
    uint8 res_field_index;
    uint8 multi_res_type_index;
    int char_index;
    char eos = '\0';

    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    sw_state_table_index = table_id - DBAL_NOF_TABLES;

    SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.table_id.set(unit, sw_state_table_index, table_id));

    
    for (char_index = 0; char_index < sal_strlen(table->table_name); char_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.
                        table_name.set(unit, sw_state_table_index, char_index, &table->table_name[char_index]));
    }

    SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.table_name.set(unit, sw_state_table_index, char_index, &eos));

    for (label_index = 0; label_index < table->nof_labels; label_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.
                        labels.set(unit, sw_state_table_index, label_index, table->table_labels[label_index]));
    }

    SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.
                    data.set(unit, sw_state_table_index, (CONST dbal_logical_table_t *) table));

    for (key_index = 0; key_index < table->nof_key_fields; key_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.key_info_data.set(unit, sw_state_table_index, key_index,
                                                                       (CONST dbal_table_field_info_t *) &
                                                                       table->keys_info[key_index]));
    }

    for (multi_res_type_index = 0; multi_res_type_index < table->nof_result_types; multi_res_type_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.
                        multi_res_info_data.set(unit, sw_state_table_index, multi_res_type_index,
                                                (CONST multi_res_info_t *) & table->
                                                multi_res_info[multi_res_type_index]));
    }

    for (multi_res_type_index = 0; multi_res_type_index < table->nof_result_types; multi_res_type_index++)
    {
        for (res_field_index = 0; res_field_index < table->multi_res_info[multi_res_type_index].nof_result_fields;
             res_field_index++)
        {
            SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.
                            res_info_data.set(unit, sw_state_table_index, multi_res_type_index, res_field_index,
                                              (CONST dbal_table_field_info_t *) (&table->
                                                                                 multi_res_info
                                                                                 [multi_res_type_index].results_info
                                                                                 [res_field_index])));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_restore(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;
    dbal_logical_table_t *table_data;
    int sw_state_table_index;
    uint8 res_field_index;
    uint8 key_index;
    uint8 lable_index;
    uint8 multi_res_type_index;
    CONST char *table_name;
    dbal_labels_e *table_labels;
    dbal_table_field_info_t *keys_info;
    multi_res_info_t *multi_res_info;
    dbal_hl_access_info_t *hl_mapping_multi_res;
#if defined(INCLUDE_KBP)
    kbp_db_handles_t *kbp_handles;
#endif

    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    SHR_IF_ERR_EXIT(dbal_tables_dbal_logical_table_t_clear(unit, table));

    sw_state_table_index = table_id - DBAL_NOF_TABLES;

    DBAL_DYNAMIC_TBL_IN_SW_STATE.table_name.get(unit, sw_state_table_index, 0, &table_name);
    sal_strncpy_s(table->table_name, table_name, DBAL_MAX_STRING_LENGTH);

    SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.
                    data.get(unit, sw_state_table_index, (CONST dbal_logical_table_t **) & table_data));

    
    table_labels = table->table_labels;
    keys_info = table->keys_info;
    multi_res_info = table->multi_res_info;
    hl_mapping_multi_res = table->hl_mapping_multi_res;
#if defined(INCLUDE_KBP)
    kbp_handles = table->kbp_handles;
#endif

    sal_memcpy(table, table_data, sizeof(dbal_logical_table_t));

    
    table->table_labels = table_labels;
    table->keys_info = keys_info;
    table->multi_res_info = multi_res_info;
    table->hl_mapping_multi_res = hl_mapping_multi_res;
#if defined(INCLUDE_KBP)
    table->kbp_handles = kbp_handles;
#endif

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.get(unit, table_id, table->indications_bm));

    for (lable_index = 0; lable_index < table->nof_labels; lable_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.
                        labels.get(unit, sw_state_table_index, lable_index, &table->table_labels[lable_index]));
    }

    for (key_index = 0; key_index < table->nof_key_fields; key_index++)
    {
        dbal_table_field_info_t *keys_info;
        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.key_info_data.get(unit, sw_state_table_index, key_index,
                                                                       (CONST dbal_table_field_info_t **) & keys_info));
        sal_memcpy(&table->keys_info[key_index], keys_info, sizeof(dbal_table_field_info_t));
    }

    if (table->nof_result_types)
    {
        for (multi_res_type_index = 0; multi_res_type_index < table->nof_result_types; multi_res_type_index++)
        {
            multi_res_info_t *multi_res_info;
            dbal_table_field_info_t *results_info;

            
            results_info = table->multi_res_info[multi_res_type_index].results_info;

            SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.
                            multi_res_info_data.get(unit, sw_state_table_index, multi_res_type_index,
                                                    (CONST multi_res_info_t **) & multi_res_info));
            sal_memcpy(&table->multi_res_info[multi_res_type_index], multi_res_info, sizeof(multi_res_info_t));

            
            table->multi_res_info[multi_res_type_index].results_info = results_info;

            for (res_field_index = 0; res_field_index < table->multi_res_info[multi_res_type_index].nof_result_fields;
                 res_field_index++)
            {
                SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.res_info_data.get(unit, sw_state_table_index,
                                                                               multi_res_type_index, res_field_index,
                                                                               (CONST dbal_table_field_info_t **) &
                                                                               results_info));
                sal_memcpy(&table->multi_res_info[multi_res_type_index].results_info[res_field_index], results_info,
                           sizeof(dbal_table_field_info_t));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tables_sw_state_restore(
    int unit)
{
    dbal_table_status_e table_status;
    dbal_tables_e table_id;
    int nof_tables = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = DBAL_NOF_TABLES; table_id < nof_tables; table_id++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
        if (table_status != DBAL_TABLE_NOT_INITIALIZED)
        {
            SHR_IF_ERR_EXIT(dbal_tables_table_restore(unit, table_id));
        }
    }

    
    SHR_IF_ERR_EXIT(dbal_tables_generic_table_xmls_info_init(unit));

    
    SHR_IF_ERR_EXIT(dbal_tables_result_type_restore(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tables_table_create_keys_set(
    int unit,
    int nof_key_fields,
    dbal_table_field_input_info_t * keys_info,
    dbal_logical_table_t * table)
{
    int iter;
    int nof_kbp_range_key_fields = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (nof_key_fields > DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal number of key fields %d max value %d\n", nof_key_fields,
                     DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS);
    }

    table->nof_key_fields = nof_key_fields;

    for (iter = 0; iter < nof_key_fields; iter++)
    {
        dbal_field_types_basic_info_t *field_type_info;
        uint32 field_max_value;
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, keys_info[iter].field_id, &field_type_info));
        SHR_IF_ERR_EXIT(dbal_fields_field_type_get
                        (unit, keys_info[iter].field_id, &(table->keys_info[iter].field_type)));

        if (keys_info[iter].is_ranged)
        {
            if ((table->access_method != DBAL_ACCESS_METHOD_KBP) || (!(DBAL_TABLE_IS_TCAM(table))))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Field - is_ranged - supports only access method KBP with type TCAM or TCAM_DIRECT (key %s)\n",
                             dbal_field_to_string(unit, keys_info[iter].field_id));
            }

            nof_kbp_range_key_fields++;
            if (nof_kbp_range_key_fields > DNX_KBP_MAX_NOF_RANGES)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Number of range key fields allowed per KBP table (%d) is exceeded\n",
                             DNX_KBP_MAX_NOF_RANGES);
            }
            SHR_BITSET(table->keys_info[iter].field_indication_bm, DBAL_FIELD_IND_IS_RANGED);
        }

        if (keys_info[iter].is_packed)
        {
            if ((table->access_method != DBAL_ACCESS_METHOD_KBP) || (!(DBAL_TABLE_IS_TCAM(table))))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Field - is_packed - supports only access method KBP with type TCAM or TCAM_DIRECT (key %s)\n",
                             dbal_field_to_string(unit, keys_info[iter].field_id));
            }
            if (iter == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Field - is_packed - cannot be the first field as i indicated packing with the previous field (key %s)\n",
                             dbal_field_to_string(unit, keys_info[iter].field_id));
            }

            SHR_BITSET(table->keys_info[iter].field_indication_bm, DBAL_FIELD_IND_IS_PACKED);
        }

        if (keys_info[iter].is_valid_indication_needed)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Field - is_valid_indication_needed - cannot be set for a key (%s)\n",
                         dbal_field_to_string(unit, keys_info[iter].field_id));
        }

        table->keys_info[iter].field_id = keys_info[iter].field_id;
        if (keys_info[iter].is_ranged && (keys_info[iter].field_nof_bits != DNX_KBP_MAX_RANGE_SIZE_IN_BITS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "KBP range fields can be only %d bits in size\n",
                         DNX_KBP_MAX_RANGE_SIZE_IN_BITS);
        }
        else if (keys_info[iter].field_nof_bits == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Field %s size was set to zero. Key field with size zero is not supported.\n",
                         dbal_field_to_string(unit, keys_info[iter].field_id));
        }
        else if (keys_info[iter].field_nof_bits != DBAL_USE_DEFAULT_SIZE)
        {
            table->keys_info[iter].field_nof_bits = keys_info[iter].field_nof_bits;
        }
        else
        {
            if (field_type_info->max_size == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Field %s size must be set or use a field type that defines a size.\n",
                             dbal_field_to_string(unit, keys_info[iter].field_id));
            }
            table->keys_info[iter].field_nof_bits = field_type_info->max_size;
        }

        SHR_IF_ERR_EXIT(dbal_fields_max_value_get(unit, keys_info[iter].field_id, &field_max_value));
        table->keys_info[iter].max_value =
            UTILEX_MIN(field_max_value, utilex_power_of_2(table->keys_info[iter].field_nof_bits) - 1);
        table->keys_info[iter].nof_instances = 1;
        if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR))
        {
            SHR_BITSET(table->keys_info[iter].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR);
            table->allocator_field_id = table->keys_info[iter].field_id;
        }

        if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
        {
            table->keys_info[iter].max_value = field_type_info->const_value;
            table->keys_info[iter].min_value = field_type_info->const_value;
            table->keys_info[iter].const_value = field_type_info->const_value;
            SHR_BITSET(table->keys_info[iter].field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID);
        }
        table->key_size += table->keys_info[iter].field_nof_bits;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tables_table_create_result_type_set_internal(
    int unit,
    int nof_result_fields,
    uint32 result_type_index,
    dbal_table_field_input_info_t * results_info,
    dbal_logical_table_t * table)
{
    int iter;
    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < nof_result_fields; iter++)
    {
        uint32 field_max_value;
        dbal_field_types_basic_info_t *field_type_info;

        if (results_info[iter].is_ranged)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Field - is_ranged - is supported only for key fields with access method KBP and type TCAM or TCAM_DIRECT (field %s)\n",
                         dbal_field_to_string(unit, results_info[iter].field_id));
        }
        if (results_info[iter].is_ranged)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Field - is_packed - is supported only for key fields with access method KBP and type TCAM or TCAM_DIRECT (field %s)\n",
                         dbal_field_to_string(unit, results_info[iter].field_id));
        }

        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, results_info[iter].field_id, &field_type_info));

        table->multi_res_info[result_type_index].results_info[iter].field_id = results_info[iter].field_id;

        if (results_info[iter].field_nof_bits != DBAL_USE_DEFAULT_SIZE)
        {
            table->multi_res_info[result_type_index].results_info[iter].field_nof_bits =
                results_info[iter].field_nof_bits;
        }
        else
        {
            table->multi_res_info[result_type_index].results_info[iter].field_nof_bits = field_type_info->max_size;
        }

        
        SHR_IF_ERR_EXIT(dbal_fields_max_value_get(unit, results_info[iter].field_id, &field_max_value));
        table->multi_res_info[result_type_index].results_info[iter].max_value =
            UTILEX_MIN(field_max_value,
                       utilex_power_of_2(table->multi_res_info[result_type_index].results_info[iter].field_nof_bits) -
                       1);

        if (results_info[iter].is_valid_indication_needed)
        {
            if (table->multi_res_info[result_type_index].results_info[iter].field_nof_bits > 32)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Field %s : Valid indication not supported for field_nof_bits > 32\n",
                             dbal_field_to_string(unit, results_info[iter].field_id));
            }
            else
            {
                
                table->multi_res_info[result_type_index].results_info[iter].field_nof_bits += 1;
                SHR_BITSET(table->multi_res_info[result_type_index].results_info[iter].field_indication_bm,
                           DBAL_FIELD_IND_IS_VALID_INDICATION);
            }
        }

        table->multi_res_info[result_type_index].results_info[iter].nof_instances = 1;
        table->multi_res_info[result_type_index].entry_payload_size +=
            table->multi_res_info[result_type_index].results_info[iter].field_nof_bits;
        table->multi_res_info[result_type_index].results_info[iter].arr_prefix = results_info[iter].arr_prefix;
        table->multi_res_info[result_type_index].results_info[iter].arr_prefix_size =
            results_info[iter].arr_prefix_size;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tables_table_create_results_set(
    int unit,
    int *nof_result_fields,
    char result_types_names[][DBAL_MAX_STRING_LENGTH],
    dbal_table_field_input_info_t * results_info,
    dbal_logical_table_t * table)
{
    int result_type_iter;
    SHR_FUNC_INIT_VARS(unit);

    for (result_type_iter = 0; result_type_iter < table->nof_result_types; result_type_iter++)
    {
        if (nof_result_fields[result_type_iter] > DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal number of result fields %d max value %d\n",
                         nof_result_fields[result_type_iter], DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);
        }

        table->multi_res_info[result_type_iter].nof_result_fields = nof_result_fields[result_type_iter];

        if (nof_result_fields[result_type_iter] > DBAL_NOF_DYNAMIC_TABLE_RESULT_FIELDS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "num of result fields for dynamic table is limited to %d , requested %d\n",
                         DBAL_NOF_DYNAMIC_TABLE_RESULT_FIELDS, nof_result_fields[result_type_iter]);
        }

        if (result_types_names[result_type_iter])
        {
            sal_strncpy(table->multi_res_info[result_type_iter].result_type_name, result_types_names[result_type_iter],
                        (DBAL_MAX_STRING_LENGTH - 1));
        }

        if (table->nof_result_types > 1)
        {
            SHR_FREE(table->multi_res_info[result_type_iter].results_info);
            SHR_ALLOC_SET_ZERO(table->multi_res_info[result_type_iter].results_info,
                               nof_result_fields[result_type_iter] * sizeof(dbal_table_field_info_t),
                               "results fields info allocation", "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);
        }

        SHR_IF_ERR_EXIT(dbal_tables_table_create_result_type_set_internal
                        (unit, nof_result_fields[result_type_iter], result_type_iter,
                         &results_info[result_type_iter * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE], table));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_tables_table_create_internal(
    int unit,
    dbal_access_method_e access_method,
    dbal_table_type_e table_type,
    dbal_core_mode_e core_mode,
    int nof_key_fields,
    dbal_table_field_input_info_t * keys_info,
    int *nof_result_fields,
    dbal_table_field_input_info_t * results_info,
    int nof_result_types,
    char result_types_names[][DBAL_MAX_STRING_LENGTH],
    char *table_name,
    int is_xml_dynamic_loading,
    dbal_tables_e * table_id)
{
    dbal_logical_table_t *table = NULL;

    SHR_FUNC_INIT_VARS(unit);

    
    if (sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, table_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_available_table_id_get(unit, is_xml_dynamic_loading, table_id));
    }

    table = &logical_tables_info[unit]->logical_tables[(*table_id)];

    SHR_IF_ERR_EXIT(dbal_tables_dbal_logical_table_t_clear(unit, table));

    table->maturity_level = DBAL_MATURITY_HIGH;

    if (table_name != NULL && table_name[0] != '\0')
    {
        dbal_tables_e log_table_id;
        _shr_error_t rv;
        rv = dbal_logical_table_string_to_id_no_error(unit, table_name, &log_table_id);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "A table with the name \"%.*s\" already exists: %d.\n",
                         DBAL_MAX_STRING_LENGTH, table_name, log_table_id);
        }
        sal_strncpy_s(table->table_name, table_name, DBAL_MAX_STRING_LENGTH);
    }
    else
    {
        sal_snprintf(table->table_name, DBAL_MAX_STRING_LENGTH, "DYNAMIC_%d", (*table_id));
    }

    table->access_method = access_method;

    table->table_type = table_type;

    table->nof_result_types = nof_result_types;
    if (nof_result_types > 1)
    {
        
        SHR_FREE(table->multi_res_info);
        SHR_ALLOC_SET_ZERO(table->multi_res_info, (nof_result_types * sizeof(multi_res_info_t)),
                           "multiple results info allocation", "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);
    }

    if (DBAL_TABLE_IS_TCAM(table) && (access_method == DBAL_ACCESS_METHOD_MDB))
    {
        
        table->physical_db_id[0] = DBAL_PHYSICAL_TABLE_TCAM;
        table->nof_physical_tables = 1;
    }

    table->core_mode = core_mode;

    SHR_IF_ERR_EXIT(dbal_tables_table_create_keys_set(unit, nof_key_fields, keys_info, table));

    SHR_IF_ERR_EXIT(dbal_tables_table_create_results_set
                    (unit, nof_result_fields, result_types_names, results_info, table));

    SHR_IF_ERR_EXIT(dbal_db_init_table_mutual_interface_validation(unit, *table_id, table));
    
    if ((access_method == DBAL_ACCESS_METHOD_MDB) || (access_method == DBAL_ACCESS_METHOD_KBP))
    {
        SHR_IF_ERR_EXIT(dbal_db_init_table_mdb_validation(unit, *table_id, table));
    }

    if (!is_xml_dynamic_loading)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_interface_to_sw_state_add(unit, *table_id));
    }
    else
    {
        
        int char_index;
        char eos = '\0';
        int sw_state_table_index = *table_id - DBAL_NOF_STATIC_TABLES;

        for (char_index = 0; char_index < sal_strlen(table->table_name); char_index++)
        {
            SHR_IF_ERR_EXIT(DBAL_DYNAMIC_XML_TBL_IN_SW_STATE.
                            table_name.set(unit, sw_state_table_index, char_index, &table->table_name[char_index]));
        }

        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_XML_TBL_IN_SW_STATE.table_name.set(unit, sw_state_table_index, char_index, &eos));
    }

    if (!sw_state_is_warm_boot(unit))
    {
        
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, *table_id, DBAL_TABLE_INITIALIZED));
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.set(unit, *table_id, table->indications_bm[0]));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dynamic_xml_table_access_info_load(
    int unit,
    int has_multiple_result_types,
    int nof_result_types,
    dbal_field_types_defs_e reference_field_type_id,
    dbal_tables_e table_id,
    xml_node appdb_to_phy_mapping_node)
{
    dbal_logical_table_t *table_entry = NULL;
    xml_node cur_node;
    int appid, ii;
    char str[DBAL_MAX_EXTRA_LONG_STRING_LENGTH] = { '\0' };

    SHR_FUNC_INIT_VARS(unit);
    
    table_entry = &logical_tables_info[unit]->logical_tables[table_id];

    
    SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE);

    if (has_multiple_result_types)
    {
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        for (ii = 0; ii < nof_result_types; ii++)
        {
            table_entry->multi_res_info[ii].result_type_nof_hw_values = 1;
            if (reference_field_type_id != DBAL_FIELD_TYPE_DEF_EMPTY)
            {
                SHR_IF_ERR_EXIT(dbal_field_types_enum_name_to_hw_value_get
                                (unit, reference_field_type_id, table_entry->multi_res_info[ii].result_type_name,
                                 &table_entry->multi_res_info[ii].result_type_hw_value[0]));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_result_type_string_to_id
                                (unit, table_id, table_entry->multi_res_info[ii].result_type_name,
                                 &table_entry->multi_res_info[ii].result_type_hw_value[0]));
            }
        }
    }

    if (table_entry->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        dbal_physical_table_def_t *physical_table;
        table_entry->nof_physical_tables = 1;
        RHDATA_ITERATOR(cur_node, appdb_to_phy_mapping_node, "AppDbId")
        {
            RHDATA_GET_INT_STOP(cur_node, "Size", table_entry->app_id_size);
            RHDATA_GET_INT_STOP(cur_node, "Value", appid);
            table_entry->app_id = appid;
        }
        sal_memset(str, '\0', DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
        dbx_xml_child_get_content_str(appdb_to_phy_mapping_node, "PhyDb", str, RHNAME_MAX_SIZE);
        SHR_IF_ERR_EXIT(dbal_physical_table_string_to_id(unit, str, table_entry->physical_db_id));

        SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, table_entry->physical_db_id[0], &physical_table));

        
        if (DBAL_MAX_NUM_OF_CORES > 1)
        {
            if ((table_entry->core_mode == DBAL_CORE_MODE_DPC)
                || (physical_table->physical_core_mode == DBAL_CORE_MODE_DPC))
            {
                int core_idx;
                for (core_idx = 1; core_idx < DBAL_MAX_NUM_OF_CORES; core_idx++)
                {
                    table_entry->physical_db_id[core_idx] = table_entry->physical_db_id[0] + core_idx;
                }
                table_entry->nof_physical_tables = DBAL_MAX_NUM_OF_CORES;
            }
        }
        else
        {
            
            int num_of_dbs_to_duplicate = 2;
            if ((table_entry->core_mode == DBAL_CORE_MODE_DPC)
                || (physical_table->physical_core_mode == DBAL_CORE_MODE_DPC))
            {
                int core_idx;
                for (core_idx = 1; core_idx < num_of_dbs_to_duplicate; core_idx++)
                {
                    table_entry->physical_db_id[core_idx] = table_entry->physical_db_id[0] + core_idx;
                }
                table_entry->nof_physical_tables = num_of_dbs_to_duplicate;
            }
        }
        table_entry->max_capacity = 0;
    }

    if (table_entry->access_method == DBAL_ACCESS_METHOD_PEMLA)
    {
        int nof_key_fields, nof_result_fields;

        if (has_multiple_result_types)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic XML PEMLA table with multiple result types is not supported\n");
        }

        nof_key_fields = table_entry->nof_key_fields;
        nof_result_fields = table_entry->multi_res_info[0].nof_result_fields;

        SHR_ALLOC_SET_ZERO(table_entry->pemla_mapping.key_fields_mapping, nof_key_fields * sizeof(uint32),
                           "pemla key mapping allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(table_entry->pemla_mapping.result_fields_mapping, nof_result_fields * sizeof(uint32),
                           "pemla result mapping allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

        for (ii = 0; ii < nof_key_fields; ii++)
        {
            table_entry->pemla_mapping.key_fields_mapping[ii] = ii;
        }

        for (ii = 0; ii < nof_result_fields; ii++)
        {
            table_entry->pemla_mapping.result_fields_mapping[ii] = ii;
        }
        table_entry->max_capacity = 1 << table_entry->key_size;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dynamic_xml_tables_update(
    int unit,
    xml_node top_node)
{
    xml_node keys_node, cur_node, multiple_results_node, appdb_interface_node, results_node, appdb_node,
        appdb_to_phy_mapping_node;
    char str[DBAL_MAX_EXTRA_LONG_STRING_LENGTH] = { '\0' };
    char table_name[DBAL_MAX_STRING_LENGTH] = { '\0' };
    char field_type_name[DBAL_MAX_STRING_LENGTH] = { '\0' };
    char result_type_names[DBAL_MAX_NUMBER_OF_RESULT_TYPES][DBAL_MAX_STRING_LENGTH];
    dbal_field_types_defs_e field_type;
    int nof_result_types = 0;
    dbal_tables_e table_id;
    dbal_table_field_input_info_t *fields_info = NULL;
    dbal_table_field_input_info_t keys_info[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];
    dbal_access_method_e access_method;
    dbal_core_mode_e core_mode;
    dbal_table_type_e table_type;
    int nof_fields[DBAL_MAX_NUMBER_OF_RESULT_TYPES];
    uint32 nof_keys;
    int multiple_results = TRUE;
    int field_index;
    int nof_bits;
    CONST dbal_field_types_basic_info_t *field_type_info;
    dbal_db_int_or_dnx_data_info_struct_t dnx_arr_prefix, dnx_arr_width;
    dbal_field_types_defs_e reference_field_type_id;
    uint32 field_info_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_NULL_CHECK(top_node, _SHR_E_PARAM, "top_node");

    SHR_ALLOC_SET_ZERO(fields_info,
                       (sizeof(dbal_table_field_input_info_t) * DBAL_MAX_NUMBER_OF_RESULT_TYPES *
                        DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE), "fields_info", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);

    
    RHDATA_ITERATOR(appdb_node, top_node, "AppDB")
    {
        sal_memset(table_name, '\0', DBAL_MAX_STRING_LENGTH);
        RHDATA_GET_XSTR_STOP(appdb_node, "Name", table_name, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
        table_id = DBAL_TABLE_EMPTY;
        
        SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_logical_table_string_to_id_no_error(unit, table_name, &table_id),
                                  _SHR_E_NOT_FOUND);
        if (table_id != DBAL_TABLE_EMPTY)
        {
            
            continue;
        }

        
        dbx_xml_child_get_content_str(appdb_node, "AccessMethod", str, RHNAME_MAX_SIZE);
        SHR_IF_ERR_EXIT(dbal_access_method_string_to_id(unit, str, &access_method));
        if ((access_method != DBAL_ACCESS_METHOD_MDB) && (access_method != DBAL_ACCESS_METHOD_KBP)
            && (access_method != DBAL_ACCESS_METHOD_PEMLA))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic tables loading only supports MDB, KBP, PEMLA access methods");
        }

        
        sal_memset(str, '\0', DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
        appdb_to_phy_mapping_node = dbx_xml_child_get_first(appdb_node, "AppToPhyDbMapping");
        if (appdb_to_phy_mapping_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "expected xml tag AppToPhyDbMapping not found");
        }
        cur_node = dbx_xml_child_get_first(appdb_to_phy_mapping_node, "CoreMode");
        if (cur_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "expected xml tag CoreMode not found");
        }
        RHDATA_GET_STR_STOP(cur_node, "Mode", str);
        SHR_IF_ERR_EXIT(dbal_core_mode_string_to_id(unit, str, &core_mode));

        cur_node = dbx_xml_child_get_first(appdb_to_phy_mapping_node, "ReferenceField");
        if (cur_node)
        {
            reference_field_type_id = DBAL_FIELD_TYPE_DEF_EMPTY;
            sal_memset(str, '\0', DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
            RHDATA_GET_STR_STOP(cur_node, "Name", str);
            SHR_IF_ERR_EXIT(dbal_field_types_def_string_to_id(unit, str, &reference_field_type_id));
        }

        
        multiple_results_node = NULL;
        results_node = NULL;
        multiple_results = TRUE;

        sal_memset(str, '\0', DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
        appdb_interface_node = dbx_xml_child_get_first(appdb_node, "AppDbInterface");
        RHDATA_GET_STR_STOP(appdb_interface_node, "Type", str);
        SHR_IF_ERR_EXIT(dbal_logical_table_type_string_to_id(unit, str, &table_type));
        multiple_results_node = dbx_xml_child_get_first(appdb_interface_node, "MultipleResults");
        results_node = dbx_xml_child_get_first(appdb_interface_node, "Result");
        keys_node = dbx_xml_child_get_first(appdb_interface_node, "Key");

        if (((results_node == NULL) && (multiple_results_node == NULL)) || (keys_node == NULL))
        {
            
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Couldn't find fields and/or keys nodes");
        }

        nof_keys = 0;
        
        RHDATA_ITERATOR(cur_node, keys_node, "Field")
        {
            sal_memset(str, '\0', DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
            nof_bits = 0;

            sal_memset(&(keys_info[nof_keys]), 0x0, sizeof(dbal_table_field_input_info_t));

            
            RHDATA_GET_STR_STOP(cur_node, "Name", str);

            
            sal_memset(field_type_name, 0, DBAL_MAX_STRING_LENGTH);
            SHR_IF_ERR_EXIT_EXCEPT_IF(dbx_xml_property_get_str(cur_node, "Type", field_type_name, RHNAME_MAX_SIZE),
                                      _SHR_E_NOT_FOUND);
            if (!sal_strcmp(field_type_name, EMPTY))
            {
                
                sal_strncpy(field_type_name, str, RHNAME_MAX_SIZE);
            }
            field_type = DBAL_FIELD_TYPE_DEF_EMPTY;
            SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_field_types_def_string_to_id(unit, field_type_name, &field_type),
                                      _SHR_E_NOT_FOUND);
            if (field_type == DBAL_FIELD_TYPE_DEF_EMPTY)
            {   
                field_type = DBAL_FIELD_TYPE_DEF_UINT;
            }

            
            keys_info[nof_keys].field_id = DBAL_FIELD_EMPTY;
            SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_field_string_to_id_no_error(unit, str, &(keys_info[nof_keys].field_id)),
                                      _SHR_E_NOT_FOUND);
            
            if (keys_info[nof_keys].field_id == DBAL_FIELD_EMPTY)
            {
                
                SHR_IF_ERR_EXIT(dbal_fields_field_create(unit, field_type, str, &(keys_info[nof_keys].field_id)));
                RHDATA_GET_INT_STOP(cur_node, "Size", nof_bits);
                keys_info[nof_keys].field_nof_bits = (uint32) nof_bits;
            }

            
            SHR_IF_ERR_EXIT_EXCEPT_IF(dbx_xml_property_get_int(cur_node, "Size", &nof_bits), _SHR_E_NOT_FOUND);
            if (nof_bits == 0)
            {
                
                SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_type, &field_type_info));
                nof_bits = field_type_info->max_size;
            }
            keys_info[nof_keys].field_nof_bits = (uint32) nof_bits;

            nof_keys++;
        }

        nof_result_types = 0;

        if (results_node != NULL)
        {
            
            multiple_results = FALSE;
            multiple_results_node = appdb_interface_node;
        }

        RHDATA_ITERATOR(results_node, multiple_results_node, "Result")
        {
            nof_fields[nof_result_types] = 0;
            RHDATA_ITERATOR(cur_node, results_node, "Field")
            {
                field_index = nof_fields[nof_result_types];
                field_info_offset = nof_result_types * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE + field_index;

                sal_memset(&fields_info[field_info_offset], 0x0, sizeof(dbal_table_field_input_info_t));
                sal_memset(str, '\0', DBAL_MAX_EXTRA_LONG_STRING_LENGTH);

                
                RHDATA_GET_STR_STOP(cur_node, "Name", str);

                
                sal_memset(field_type_name, 0, DBAL_MAX_STRING_LENGTH);
                SHR_IF_ERR_EXIT_EXCEPT_IF(dbx_xml_property_get_str(cur_node, "Type", field_type_name, RHNAME_MAX_SIZE),
                                          _SHR_E_NOT_FOUND);
                if (!sal_strcmp(field_type_name, EMPTY))
                {
                    
                    sal_strncpy(field_type_name, str, RHNAME_MAX_SIZE);
                }

                field_type = DBAL_FIELD_TYPE_DEF_EMPTY;
                SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_field_types_def_string_to_id(unit, field_type_name, &field_type),
                                          _SHR_E_NOT_FOUND);
                if (field_type == DBAL_FIELD_TYPE_DEF_EMPTY)
                {   
                    field_type = DBAL_FIELD_TYPE_DEF_UINT;
                }

                
                SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_field_string_to_id_no_error
                                          (unit, str, &fields_info[field_info_offset].field_id), _SHR_E_NOT_FOUND);
                if (fields_info[field_info_offset].field_id == DBAL_FIELD_EMPTY)
                {
                    
                    SHR_IF_ERR_EXIT(dbal_fields_field_create
                                    (unit, field_type, str, &(fields_info[field_info_offset].field_id)));
                }
                else if (fields_info[field_info_offset].field_id == DBAL_FIELD_RESULT_TYPE)
                {
                    sal_memset(result_type_names[nof_result_types], '\0', DBAL_MAX_STRING_LENGTH);
                    RHDATA_GET_STR_STOP(cur_node, "Value", result_type_names[nof_result_types]);
                }

                
                nof_bits = 0;
                SHR_IF_ERR_EXIT_EXCEPT_IF(dbx_xml_property_get_int(cur_node, "Size", &nof_bits), _SHR_E_NOT_FOUND);
                if (nof_bits == 0)
                {
                    
                    SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_type, &field_type_info));
                    nof_bits = field_type_info->max_size;
                }
                fields_info[field_info_offset].field_nof_bits = (uint32) nof_bits;

                
                sal_memset(str, 0, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
                SHR_IF_ERR_EXIT_EXCEPT_IF(dbx_xml_property_get_str(cur_node, "ArrPrefixValue", str, RHSTRING_MAX_SIZE),
                                          _SHR_E_NOT_FOUND);
                if (sal_strcmp(str, EMPTY))
                {
                    
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &dnx_arr_prefix, str, FALSE, 0, "", ""));

                    
                    sal_memset(str, 0, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
                    RHDATA_GET_STR_STOP(cur_node, "ArrPrefixWidth", str);
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &dnx_arr_width, str, FALSE, 0, "", ""));

                    SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_type, &field_type_info));

                    
                    fields_info[field_info_offset].arr_prefix_size =
                        (field_type_info->max_size - fields_info[field_info_offset].field_nof_bits);

                    fields_info[field_info_offset].arr_prefix =
                        (dnx_arr_prefix.int_val >> (dnx_arr_width.int_val -
                                                    fields_info[field_info_offset].arr_prefix_size));
                }

                nof_fields[nof_result_types]++;
            }

            nof_result_types++;
            if (!multiple_results)
            {
                break;
            }
        }

        
        SHR_IF_ERR_EXIT(dbal_tables_table_create_internal
                        (unit, access_method, table_type, core_mode, nof_keys, keys_info, nof_fields, fields_info,
                         nof_result_types, result_type_names, table_name, TRUE, &table_id));

        
        SHR_IF_ERR_EXIT(dnx_dynamic_xml_table_access_info_load
                        (unit, multiple_results, nof_result_types, reference_field_type_id, table_id,
                         appdb_to_phy_mapping_node));

    }

exit:
    if (fields_info)
    {
        sal_free(fields_info);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_create(
    int unit,
    dbal_access_method_e access_method,
    dbal_table_type_e table_type,
    dbal_core_mode_e core_mode,
    int nof_key_fields,
    dbal_table_field_input_info_t * keys_info,
    int nof_result_fields,
    dbal_table_field_input_info_t * results_info,
    char *table_name,
    dbal_tables_e * table_id)
{
    dbal_table_field_input_info_t internal_results_info[1][DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];
    SHR_FUNC_INIT_VARS(unit);

    sal_memcpy(&internal_results_info[0], results_info, nof_result_fields * (sizeof(dbal_table_field_input_info_t)));
    if ((nof_key_fields > dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit)) ||
        (nof_result_fields > dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal number of fields: key: req[%d]avail[%d], result: req[%d]avail[%d]\n",
                     nof_key_fields, dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit),
                     nof_result_fields, dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit));
    }

    if ((access_method != DBAL_ACCESS_METHOD_MDB) && (access_method != DBAL_ACCESS_METHOD_KBP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal Access method, Supported only Access methods MDB / KBP \n");
    }

    if ((table_type != DBAL_TABLE_TYPE_TCAM_BY_ID) &&
        (table_type != DBAL_TABLE_TYPE_EM) && (table_type != DBAL_TABLE_TYPE_DIRECT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Illegal table_type, Supported only DBAL_TABLE_TYPE_TCAM_BY_ID, DBAL_TABLE_TYPE_EM \n");
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_create_internal
                    (unit, access_method, table_type, core_mode, nof_key_fields, keys_info, &nof_result_fields,
                     &internal_results_info[0][0], 1,
                                                              
                     NULL,                                 
                     table_name, FALSE,
                                                           
                     table_id));

    
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_dbal_table_create(unit, *table_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "dbal_tables_table_create table_id=%d, table_type %d access_method %d %s\n ",
                 *table_id, table_type, access_method, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_result_type_add(
    int unit,
    dbal_tables_e table_id,
    int nof_result_fields,
    dbal_table_field_input_info_t * results_info,
    char *res_type_name,
    int result_type_size,
    int result_type_hw_value,
    int *result_type_index)
{
    dbal_logical_table_t *table = NULL;
    int new_res_type_idx = 0, result_total_size = 0, result_offset = 0;
    mdb_em_entry_encoding_e entry_encoding;
    int res_type_iter;
    int res_hw_val_iter;
    int res_field_iter;
    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    
    if (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_PPMC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Currently, this API is to support PPMC (MC-ID) table only, table %s \n",
                     table->table_name);
    }

    if (table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Adding result type is available for MDB only, table %s\n", table->table_name);
    }

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot add result type to tables without already having result type, table %s\n",
                     table->table_name);
    }

    if (table->nof_result_types == dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Cannot add result type to tables, maximum number of result types [%d] used, table %s\n",
                     dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit), table->table_name);
    }

    if (table->multi_res_info[0].results_info[0].field_nof_bits != result_type_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Cannot add a result type with different size of RESULT_TYPE Field. New %d, Exist %d, table %s\n",
                     result_type_size, table->multi_res_info[0].results_info[0].field_nof_bits, table->table_name);
    }

    if (nof_result_fields <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Adding result type to table %s, without having RESULT_TYPE field in leading field "
                     "(nof_result_fields is %d)\n", table->table_name, nof_result_fields);
    }

    if (results_info[0].field_id != DBAL_FIELD_RESULT_TYPE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Adding result type to table %s, without having RESULT_TYPE field in leading field\n",
                     table->table_name);
    }

    if (results_info[0].is_valid_indication_needed)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Adding result type with valid bit is not supported. Table %s.\n", table->table_name);
    }

    for (res_type_iter = 0; res_type_iter < table->nof_result_types; res_type_iter++)
    {
        for (res_hw_val_iter = 0; res_hw_val_iter < table->multi_res_info[res_type_iter].result_type_nof_hw_values;
             res_hw_val_iter++)
        {
            if (result_type_hw_value == table->multi_res_info[res_type_iter].result_type_hw_value[res_hw_val_iter])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "New result type HW value (%d) already exists in table, in result type index %d "
                             " HW value index %d. table %s\n",
                             result_type_hw_value, res_type_iter, res_hw_val_iter, table->table_name);
            }
        }

    }
    new_res_type_idx = table->nof_result_types;
    table->nof_result_types += 1;

    table->multi_res_info[new_res_type_idx].result_type_nof_hw_values = 1;
    table->multi_res_info[new_res_type_idx].result_type_hw_value[0] = result_type_hw_value;
    if (res_type_name == NULL)
    {
        sal_snprintf(table->multi_res_info[new_res_type_idx].result_type_name, DBAL_MAX_STRING_LENGTH,
                     "DYN_RES_%d", new_res_type_idx);
    }
    else
    {
        sal_snprintf(table->multi_res_info[new_res_type_idx].result_type_name, DBAL_MAX_STRING_LENGTH,
                     "DYN_RES_%s", res_type_name);
    }
    table->multi_res_info[new_res_type_idx].nof_result_fields = nof_result_fields;

    
    SHR_IF_ERR_EXIT(dbal_tables_table_create_result_type_set_internal
                    (unit, nof_result_fields, new_res_type_idx, results_info, table));

    result_total_size = table->multi_res_info[new_res_type_idx].entry_payload_size;

    if (result_total_size > table->max_payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Adding result type which is longer (%d bits) than all existing types (%d bits) is not supported. table %s\n",
                     result_total_size, table->max_payload_size, table->table_name);
    }
    
    result_offset = 0;
    for (res_field_iter = nof_result_fields - 1; res_field_iter >= 0; res_field_iter--)
    {
        table->multi_res_info[new_res_type_idx].results_info[res_field_iter].bits_offset_in_buffer = result_offset;
        result_offset += table->multi_res_info[new_res_type_idx].results_info[res_field_iter].field_nof_bits;
    }
    
    SHR_IF_ERR_EXIT(mdb_em_get_min_entry_encoding(unit, DBAL_PHYSICAL_TABLE_PPMC, table->key_size,
                                                  table->multi_res_info[new_res_type_idx].entry_payload_size, 0, 0, 0,
                                                  &entry_encoding));
    SHR_IF_ERR_EXIT(mdb_em_set_vmv_size_value
                    (unit, DBAL_PHYSICAL_TABLE_PPMC, entry_encoding, 6,
                     table->multi_res_info[new_res_type_idx].result_type_hw_value[0]));

    *result_type_index = new_res_type_idx;

    SHR_IF_ERR_EXIT(dbal_tables_table_res_type_sw_state_add(unit, table_id, new_res_type_idx));

    
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_result_type_add(unit, table_id, new_res_type_idx));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_result_type_restore_rollback(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only table %s is supported. Table given %s.\n",
                     dbal_logical_table_to_string(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION),
                     dbal_logical_table_to_string(unit, table_id));
    }

    table = &logical_tables_info[unit]->logical_tables[table_id];

    
    table->nof_result_types = ETM_PP_DESCRIPTOR_EXPANSION_NOF_STATIC_RES_TYPES;

    SHR_IF_ERR_EXIT(dbal_tables_result_type_restore(unit, table_id));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_result_type_restore(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;
    uint8 multi_res_type_index;
    uint8 multi_res_type_index_sw;
    int nof_dynamic_res_types = 0;
    int nof_max_dynamic_res_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
    shr_error_e rv = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    
    if (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_PPMC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Currently, this API is to support PPMC (MC-ID) table only, table %s \n",
                     table->table_name);
    }

    
    nof_dynamic_res_types = 0;
    for (multi_res_type_index_sw = 0; multi_res_type_index_sw < nof_max_dynamic_res_types; multi_res_type_index_sw++)
    {
        rv = dbal_tables_table_res_type_sw_state_get(unit, table_id, nof_dynamic_res_types, NULL);
        if (rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
        nof_dynamic_res_types++;
    }

    

    if (nof_dynamic_res_types > 0)
    {
        table = &logical_tables_info[unit]->logical_tables[table_id];

        multi_res_type_index_sw = 0;
        for (multi_res_type_index = table->nof_result_types;
             multi_res_type_index < (table->nof_result_types + nof_dynamic_res_types); multi_res_type_index++)
        {
            for (; multi_res_type_index_sw < nof_max_dynamic_res_types; multi_res_type_index_sw++)
            {
                rv = dbal_tables_table_res_type_sw_state_get(unit, table_id, multi_res_type_index_sw,
                                                             &table->multi_res_info[multi_res_type_index]);
                if (rv == _SHR_E_NOT_FOUND)
                {
                    continue;
                }
                else
                {
                    SHR_IF_ERR_EXIT(rv);
                    multi_res_type_index_sw++;
                    break;
                }
            }
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached result type index %d, number of static result types %d, "
                             "number of dynamic result types %d, but could not find corresponding result type in "
                             "SW state.\n", multi_res_type_index, table->nof_result_types, nof_dynamic_res_types);
            }
        }
    }

    table->nof_result_types += nof_dynamic_res_types;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_destroy(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;
    dbal_table_status_e table_status;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));

    if (table_status == DBAL_TABLE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table %d does not exists.\n", table_id);
    }

    if (table_id < DBAL_NOF_TABLES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table %d is not dynamic.\n", table_id);
    }

    
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_dbal_table_info_change(unit, table_id));

    SHR_IF_ERR_EXIT(dbal_tables_dbal_logical_table_t_clear(unit, table));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, table_id, DBAL_TABLE_NOT_INITIALIZED));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_dynamic_result_type_delete(
    int unit,
    dbal_tables_e table_id,
    int result_type_index)
{
    dbal_logical_table_t *table = NULL;
    dbal_table_field_info_t *results_info = NULL;
    uint32 del_res_type_hw_value;

    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    
    if (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_PPMC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Currently, this API is to support PPMC (MC-ID) table only, table %s \n",
                     table->table_name);
    }

    if (table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic result type is available for MDB only, table %s\n", table->table_name);
    }

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot destroy result type in table without RESULT_TYPE, table %s\n",
                     table->table_name);
    }

    
    if (sal_strstr(table->multi_res_info[result_type_index].result_type_name, "DYN_RES_") == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Result type index, %d, is not a dynamic result type. result name=%s. table %s\n",
                     result_type_index, table->multi_res_info[result_type_index].result_type_name, table->table_name);
    }

    del_res_type_hw_value = table->multi_res_info[result_type_index].result_type_hw_value[0];

    
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_result_type_delete(unit, table_id));

    
    results_info = table->multi_res_info[result_type_index].results_info;
    
    sal_memset(results_info, 0,
               dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t));

    if (result_type_index == table->nof_result_types - 1)
    {
        
        sal_memset(&table->multi_res_info[result_type_index], 0, sizeof(multi_res_info_t));
        table->multi_res_info[result_type_index].results_info = results_info;
    }
    else
    {
        int nof_multi_res_types = table->nof_result_types;
        
        sal_memcpy(&table->multi_res_info[result_type_index], &table->multi_res_info[nof_multi_res_types - 1],
                   sizeof(multi_res_info_t));
        sal_memset(&table->multi_res_info[nof_multi_res_types - 1], 0, sizeof(multi_res_info_t));
        table->multi_res_info[nof_multi_res_types - 1].results_info = results_info;

    }
    table->nof_result_types -= 1;
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.
                    multi_res_info_status.set(unit, table->nof_result_types, DBAL_MULTI_RES_INFO_AVAIL));

    SHR_IF_ERR_EXIT(dbal_tables_table_res_type_sw_state_del(unit, table_id, del_res_type_hw_value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_dynamic_result_type_destroy_all(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table = NULL;
    int iter, res_type_index;
    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    
    if (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_PPMC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Currently, this API is to support PPMC (MC-ID) table only, table %s \n",
                     table->table_name);
    }

    if (table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic result type is available for MDB only, table %s\n", table->table_name);
    }

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot destroy result type in table without RESULT_TYPE, table %s\n",
                     table->table_name);
    }

    
    for (iter = 0; iter < table->nof_result_types; iter++)
    {
        if (sal_strstr(table->multi_res_info[iter].result_type_name, "DYN_RES_") != NULL)
        {
            break;
        }
    }
    if (iter == table->nof_result_types)
    {
        SHR_ERR_EXIT(_SHR_E_EMPTY, "table %s does not contain any dynamic result type, Illegal destroy\n",
                     table->table_name);
    }
    if (iter == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s must have  at least on static result type.\n", table->table_name);
    }

    for (res_type_index = iter; res_type_index < table->nof_result_types; res_type_index++)
    {
        dbal_table_field_info_t *results_info;

        results_info = table->multi_res_info[table->nof_result_types].results_info;

        sal_memset(&table->multi_res_info[table->nof_result_types], 0, sizeof(multi_res_info_t));
        table->multi_res_info[table->nof_result_types].results_info = results_info;

        sal_memset(table->multi_res_info[table->nof_result_types].results_info, 0,
                   dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t));
    }

    table->nof_result_types = iter;

    SHR_IF_ERR_EXIT(dbal_tables_table_res_type_sw_state_delete_all(unit, table_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_get_internal(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t ** table)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    (*table) = &logical_tables_info[unit]->logical_tables[table_id];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_nof_res_type_get(
    int unit,
    dbal_tables_e table_id,
    int *nof_res_type)
{
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    (*nof_res_type) = table->nof_result_types;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_get(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t ** table)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    (*table) = &logical_tables_info[unit]->logical_tables[table_id];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_is_key_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 *is_key)
{
    int field_index;
    dbal_logical_table_t *table;
    uint8 is_sub_field_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit]->logical_tables[table_id];
    *is_key = FALSE;

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        if (table->keys_info[field_index].field_id == field_id)
        {
            *is_key = TRUE;
            break;
        }
        if (SHR_BITGET(table->keys_info[field_index].field_indication_bm, DBAL_FIELD_IND_IS_PARENT_FIELD))
        {
            SHR_IF_ERR_EXIT(dbal_fields_sub_field_match(unit, table->keys_info[field_index].field_id,
                                                        field_id, &is_sub_field_found));
            if (is_sub_field_found)
            {
                *is_key = TRUE;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_info_get_internal(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t ** field_info,
    int *field_index_in_table,
    dbal_fields_e * parent_field_id)
{
    int iter;
    int nof_fields;
    dbal_logical_table_t *table;
    dbal_table_field_info_t *fields_db;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    (*field_info) = NULL;
    (*parent_field_id) = DBAL_FIELD_EMPTY;
    (*field_index_in_table) = -1;

    if (is_key)
    {
        nof_fields = table->nof_key_fields;
        fields_db = table->keys_info;
    }
    else
    {
        if (table->multi_res_info == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Table %s has a NULL value in 'multi_res_info'\n",
                         dbal_logical_table_to_string(unit, table_id));
        }
        nof_fields = table->multi_res_info[result_type_idx].nof_result_fields;
        fields_db = table->multi_res_info[result_type_idx].results_info;
    }

    if (inst_idx == INST_SINGLE)
    {
        inst_idx = 0;
    }

    for (iter = 0; iter < nof_fields; iter++)
    {
        uint8 is_sub_field_found = FALSE;

        if (fields_db[iter].field_id == field_id)
        {
            if ((inst_idx >= fields_db[iter].nof_instances) || (inst_idx < 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Instance idx is out of range. inst=%d, field %s, table %s\n", inst_idx,
                             dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id));
            }
            *field_info = &(fields_db[iter + inst_idx]);
            (*field_index_in_table) = iter + inst_idx;
            field_id += inst_idx;
            break;
        }

        if (SHR_BITGET(fields_db[iter].field_indication_bm, DBAL_FIELD_IND_IS_PARENT_FIELD))
        {
            SHR_IF_ERR_EXIT(dbal_fields_sub_field_match(unit, fields_db[iter].field_id, field_id, &is_sub_field_found));
            if (is_sub_field_found)
            {
                if (inst_idx >= fields_db[iter].nof_instances)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Instance id is out of range for child field. inst=%d, field %s, table %s\n", inst_idx,
                                 dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id));
                }
                (*parent_field_id) = fields_db[iter + inst_idx].field_id;
                (*field_index_in_table) = iter + inst_idx;
                *field_info = &(fields_db[iter + inst_idx]);
                break;
            }
        }
    }

    if (!(*field_info))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_info_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t * field_info)
{
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, &field_index_in_table, &parent_field_id);
    SHR_SET_CURRENT_ERR(rv);
    if (rv)
    {
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "field %s not found in table %s res type %d as %s field\n",
                         dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id),
                         result_type_idx, is_key ? "key" : "result");
        }
    }
    else
    {
        (*field_info) = (*field_info_p);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_size_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_size)
{
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, &field_index_in_table, &parent_field_id);
    SHR_SET_CURRENT_ERR(rv);
    if (rv)
    {
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "error in field %s table %s res type %d as %s field\n",
                         dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id),
                         result_type_idx, is_key ? "key" : "result");
        }
    }
    else
    {
        (*field_size) = field_info_p->field_nof_bits;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_max_value_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_size)
{
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, &field_index_in_table, &parent_field_id);
    SHR_SET_CURRENT_ERR(rv);
    if (rv)
    {
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "error in field %s table %s res type %d as %s field\n",
                         dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id),
                         result_type_idx, is_key ? "key" : "result");
        }
    }
    else
    {
        if (field_info_p->field_nof_bits > 32)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "cannot receive max value for field bigger than 32 bit field %s table %s \n",
                         dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id));
        }
        (*field_size) = field_info_p->max_value;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_hw_value_result_type_get(
    int unit,
    dbal_tables_e table_id,
    uint32 hw_value,
    int *result_type_idx)
{
    int res_type_iter;
    int hw_value_iter;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);
    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table %s has doesn't have result types.\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    for (res_type_iter = 0; res_type_iter < table->nof_result_types; res_type_iter++)
    {
        
        if (table->multi_res_info[res_type_iter].is_disabled)
        {
            continue;
        }
        for (hw_value_iter = 0; hw_value_iter < table->multi_res_info[res_type_iter].result_type_nof_hw_values;
             hw_value_iter++)
        {
            if (table->multi_res_info[res_type_iter].result_type_hw_value[hw_value_iter] == hw_value)
            {
                
                (*result_type_idx) = res_type_iter;
                SHR_EXIT();
            }
        }
    }

    
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_result_type_by_name_get(
    int unit,
    dbal_tables_e table_id,
    char *result_type_name,
    int *result_type_idx)
{
    int res_type_iter;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);
    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table %s doesn't have result types.\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    for (res_type_iter = 0; res_type_iter < table->nof_result_types; res_type_iter++)
    {
        if (sal_strncmp(table->multi_res_info[res_type_iter].result_type_name, result_type_name, DBAL_MAX_STRING_LENGTH)
            == 0)
        {
            
            (*result_type_idx) = res_type_iter;
            SHR_EXIT();
        }
    }

    
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_offset_in_hw_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_offset)
{
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, &field_index_in_table, &parent_field_id);
    SHR_SET_CURRENT_ERR(rv);
    if (rv)
    {
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "field %s not found in table %s res type %d as %s field\n",
                         dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id),
                         result_type_idx, is_key ? "key" : "result");
        }
    }
    else
    {
        (*field_offset) = field_info_p->bits_offset_in_buffer;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_is_table_active(
    int unit,
    dbal_tables_e table_id,
    int *is_table_active)
{
    dbal_table_status_e table_status;
    dbal_logical_table_t *table = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));

    *is_table_active = 1;
    table = &logical_tables_info[unit]->logical_tables[table_id];

    if ((table_status == DBAL_TABLE_NOT_INITIALIZED) || (table_status == DBAL_TABLE_INCOMPATIBLE_IMAGE)
        || (table_status == DBAL_TABLE_HW_ERROR))
    {
        *is_table_active = 0;
    }

    if (table->maturity_level == DBAL_MATURITY_LOW)
    {
        *is_table_active = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_next_table_get(
    int unit,
    dbal_tables_e table_id,
    dbal_labels_e label,
    dbal_access_method_e access_method,
    dbal_physical_tables_e mdb_physical_db,
    dbal_table_type_e table_type,
    dbal_tables_e * next_table_id)
{
    dbal_tables_e table_counter;
    int jj;
    dbal_logical_table_t *table = NULL;
    int nof_tables = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if ((mdb_physical_db != DBAL_PHYSICAL_TABLE_NONE) && (access_method != DBAL_ACCESS_METHOD_MDB))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "if access_method is not MDB, mdb_physical_db must be NONE");
    }

    (*next_table_id) = DBAL_TABLE_EMPTY;

    for (table_counter = table_id + 1; table_counter < nof_tables; table_counter++)
    {
        int is_table_active;

        SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, table_counter, &is_table_active));

        if (!is_table_active)
        {
            continue;
        }

        table = &logical_tables_info[unit]->logical_tables[table_counter];

        if (label != DBAL_LABEL_NONE)
        {
            for (jj = 0; jj < table->nof_labels; jj++)
            {
                if ((table->table_labels[jj] == label))
                {
                    break;
                }
            }
            if (jj == table->nof_labels)
            {
                continue;
            }
        }

        if ((access_method != DBAL_NOF_ACCESS_METHODS) && (table->access_method != access_method))
        {
            continue;
        }

        if ((table_type != DBAL_TABLE_TYPE_NONE) && (table->table_type != table_type))
        {
            continue;
        }

        if ((mdb_physical_db != DBAL_PHYSICAL_TABLE_NONE) && (table->physical_db_id[0] != mdb_physical_db))
        {
            continue;
        }

        (*next_table_id) = table_counter;
        break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_next_field_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    int is_key,
    int result_type,
    dbal_fields_e * next_field_id)
{
    int fields_counter;
    dbal_logical_table_t *table = NULL;
    int nof_fields;
    dbal_table_field_info_t *fields_db;
    int field_found = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));
    (*next_field_id) = DBAL_FIELD_EMPTY;

    if (is_key)
    {
        nof_fields = table->nof_key_fields;
        fields_db = table->keys_info;
    }
    else
    {
        if (result_type > table->nof_result_types)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "result type not valid %d max legal value %d\n", result_type,
                         table->nof_result_types - 1);
        }
        nof_fields = table->multi_res_info[result_type].nof_result_fields;
        fields_db = table->multi_res_info[result_type].results_info;
    }

    
    if (DBAL_FIELD_EMPTY == field_id)
    {
        
        if ((table->access_method == DBAL_ACCESS_METHOD_PEMLA) && !is_key)
        {
            int ii;
            for (ii = 0; ii < nof_fields; ii++)
            {
                if (table->pemla_mapping.result_fields_mapping[ii] == DBAL_PEMLA_FIELD_MAPPING_INVALID)
                {
                    continue;
                }
                (*next_field_id) = fields_db[ii].field_id;
                field_found = 1;
                break;
            }
            if (field_found == 0)
            {
                field_found = 1;
                (*next_field_id) = DBAL_FIELD_EMPTY;
            }
        }
        else
        {
            
            (*next_field_id) = fields_db[0].field_id;
            field_found = 1;
        }
    }
    else
    {
        for (fields_counter = 0; fields_counter < nof_fields; fields_counter++)
        {
            if (fields_db[fields_counter].field_id == field_id)
            {
                
                if (fields_counter == nof_fields - 1)
                {
                    (*next_field_id) = DBAL_FIELD_EMPTY;
                    field_found = 1;
                    break;
                }
                else
                {
                    
                    if ((table->access_method == DBAL_ACCESS_METHOD_PEMLA) && !is_key)
                    {
                        int ii;
                        for (ii = fields_counter + 1; ii < nof_fields; ii++)
                        {
                            if (table->pemla_mapping.result_fields_mapping[ii] == DBAL_PEMLA_FIELD_MAPPING_INVALID)
                            {
                                continue;
                            }
                            (*next_field_id) = fields_db[ii].field_id;
                            field_found = 1;
                            break;
                        }
                        if (field_found == 0)
                        {
                            field_found = 1;
                            (*next_field_id) = DBAL_FIELD_EMPTY;
                            break;
                        }
                    }
                    else
                    {
                        (*next_field_id) = fields_db[fields_counter + 1].field_id;
                        field_found = 1;
                    }
                }
                break;
            }
        }
    }

    if (!field_found)
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal field_id %s, expected input is a field from table %s or DBAL_FIELD_EMPTY\n",
                     dbal_field_to_string(unit, field_id), table->table_name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_info_get_no_err(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t * field_info)
{
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, &field_index_in_table, &parent_field_id);
    if (rv)
    {
        SHR_SET_CURRENT_ERR(rv);
    }
    else
    {
        (*field_info) = (*field_info_p);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_index_in_table_get_no_err(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_index_in_table)
{
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    (*field_index_in_table) = -1;

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, field_index_in_table, &parent_field_id);
    if (rv)
    {
        SHR_SET_CURRENT_ERR(rv);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_type_get(
    int unit,
    dbal_tables_e table_id,
    dbal_table_type_e * table_type)
{

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    *table_type = logical_tables_info[unit]->logical_tables[table_id].table_type;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_app_id_set(
    int unit,
    dbal_tables_e table_id,
    uint32 app_db_id)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if (logical_tables_info[unit]->logical_tables[table_id].access_method != DBAL_ACCESS_METHOD_KBP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "app DB get is not applicable for table %s\n",
                     logical_tables_info[unit]->logical_tables[table_id].table_name);
    }

    
    if (table_id >= DBAL_NOF_TABLES)
    {
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_dbal_table_info_change(unit, table_id));
    }

    logical_tables_info[unit]->logical_tables[table_id].app_id = app_db_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_app_db_id_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *app_db_id,
    int *app_db_size)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if ((logical_tables_info[unit]->logical_tables[table_id].access_method != DBAL_ACCESS_METHOD_MDB) &&
        (logical_tables_info[unit]->logical_tables[table_id].access_method != DBAL_ACCESS_METHOD_KBP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "app DB get is not applicable for table %s\n",
                     logical_tables_info[unit]->logical_tables[table_id].table_name);
    }

    
    if ((logical_tables_info[unit]->logical_tables[table_id].access_method == DBAL_ACCESS_METHOD_KBP) &&
        (logical_tables_info[unit]->logical_tables[table_id].app_id == 0))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    *app_db_id = logical_tables_info[unit]->logical_tables[table_id].app_id;

    
    if (logical_tables_info[unit]->logical_tables[table_id].access_method != DBAL_ACCESS_METHOD_KBP)
    {
        *app_db_size = logical_tables_info[unit]->logical_tables[table_id].app_id_size;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_core_mode_get(
    int unit,
    dbal_tables_e table_id,
    dbal_core_mode_e * core_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    *core_mode = logical_tables_info[unit]->logical_tables[table_id].core_mode;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_result_type_hw_value_get(
    int unit,
    dbal_tables_e table_id,
    int result_type_idx,
    uint32 *hw_value)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);
    if (logical_tables_info[unit]->logical_tables[table_id].nof_result_types <= result_type_idx)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Result type index=%d is not existed in table %s. Nof result types is %d\n",
                     result_type_idx, logical_tables_info[unit]->logical_tables[table_id].table_name,
                     logical_tables_info[unit]->logical_tables[table_id].nof_result_types);
    }
    *hw_value =
        logical_tables_info[unit]->logical_tables[table_id].multi_res_info[result_type_idx].result_type_hw_value[0];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_capacity_get(
    int unit,
    dbal_tables_e table_id,
    int *max_capacity)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    *max_capacity = table->max_capacity;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_tcam_handler_id_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *tcam_handler_id)
{
    uint32 tcam_han_id;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    if (!DBAL_TABLE_IS_TCAM(table))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tcam_handler_id is invalid when Table type is not TCAM (table %s)\n",
                     logical_tables_info[unit]->logical_tables[table_id].table_name);
    }
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, table_id, &tcam_han_id));
    *tcam_handler_id = tcam_han_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_tcam_handler_id_set(
    int unit,
    dbal_tables_e table_id,
    uint32 tcam_handler_id)
{

    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    if (!DBAL_TABLE_IS_TCAM(table))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tcam_handler_id is invalid when Table type is not TCAM (table %s)\n",
                     logical_tables_info[unit]->logical_tables[table_id].table_name);
    }

    
    if (table_id >= DBAL_NOF_TABLES)
    {
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_dbal_table_info_change(unit, table_id));
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.set(unit, table_id, tcam_handler_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_indication_get(
    int unit,
    dbal_tables_e table_id,
    dbal_table_indications_e ind_type,
    uint32 *ind_val)
{
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    switch (ind_type)
    {
        case DBAL_TABLE_IND_IS_HITBIT_EN:
        case DBAL_TABLE_IND_IS_HOOK_ACTIVE:
        case DBAL_TABLE_IND_HAS_RESULT_TYPE:
        case DBAL_TABLE_IND_RANGE_SET_SUPPORTED:
        case DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW:
        case DBAL_TABLE_IND_IS_PRIORITY_SUPPORTED:
        case DBAL_TABLE_IND_IS_LEARNING_EN:
        case DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED:
        case DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE:
        case DBAL_TABLE_IND_IS_TABLE_DIRTY:
        case DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED:
        case DBAL_TABLE_IND_IS_DEFAULT_NON_STANDARD:
        case DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED:
        case DBAL_TABLE_IND_NO_VALIDATIONS:
        case DBAL_TABLE_IND_COLLECT_MODE:
            *ind_val = SHR_IS_BITSET(table->indications_bm, ind_type);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "table indication %d is invalid\n", ind_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_indication_set(
    int unit,
    dbal_tables_e table_id,
    dbal_table_indications_e ind_type,
    uint32 ind_val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (ind_type)
    {
        case DBAL_TABLE_IND_IS_HITBIT_EN:
            SHR_IF_ERR_EXIT(dbal_tables_hitbit_enable(unit, table_id, ind_val));
            break;

        case DBAL_TABLE_IND_IS_LEARNING_EN:
            SHR_IF_ERR_EXIT(dbal_tables_learning_enable(unit, table_id, ind_val));
            break;

        case DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED:
            SHR_IF_ERR_EXIT(dbal_tables_optimize_commit_mode_set(unit, table_id, ind_val));
            break;

        case DBAL_TABLE_IND_IS_TABLE_DIRTY:
            SHR_IF_ERR_EXIT(dbal_tables_dirty_mode_set(unit, table_id, ind_val));
            break;

        case DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED:
            SHR_IF_ERR_EXIT(dbal_tables_protection_enable_set(unit, table_id, ind_val));
            break;

        case DBAL_TABLE_IND_NO_VALIDATIONS:
            SHR_IF_ERR_EXIT(dbal_tables_validation_set(unit, table_id, ind_val));
            break;

        case DBAL_TABLE_IND_COLLECT_MODE:
            SHR_IF_ERR_EXIT(dbal_tables_collection_mode_set(unit, table_id, ind_val));
            break;

        case DBAL_TABLE_IND_IS_HOOK_ACTIVE:
        case DBAL_TABLE_IND_HAS_RESULT_TYPE:
        case DBAL_TABLE_IND_RANGE_SET_SUPPORTED:
        case DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW:
        case DBAL_TABLE_IND_IS_PRIORITY_SUPPORTED:
        case DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE:
        case DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED:
        case DBAL_TABLE_IND_IS_DEFAULT_NON_STANDARD:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "table indication %d cannot be modified\n", ind_type);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "table indication %d is invalid\n", ind_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_collection_mode_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    if (mode > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table %s: only 0/1 is legal for collection mode\n",
                     logical_tables_info[unit]->logical_tables[table_id].table_name);
    }

    if (!sw_state_is_warm_boot(unit))
    {
        
        SHR_IF_ERR_EXIT(dbal_tables_validation_set(unit, table_id, mode));

        if (mode)
        {
            if (dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid call when pool size is 0 table %s\n",
                             logical_tables_info[unit]->logical_tables[table_id].table_name);
            }
            SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_COLLECT_MODE);
        }
        else
        {
            SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_COLLECT_MODE);
        }
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.set(unit, table_id, table->indications_bm[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_validation_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    if (mode > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table %s: only 0/1 is legal for validation mode\n",
                     logical_tables_info[unit]->logical_tables[table_id].table_name);
    }

    if (!sw_state_is_warm_boot(unit))
    {
        if (mode)
        {
            SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_NO_VALIDATIONS);
        }
        else
        {
            SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_NO_VALIDATIONS);
        }
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.set(unit, table_id, table->indications_bm[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_hitbit_enable(
    int unit,
    dbal_tables_e table_id,
    uint32 hitbit)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    
    if (table->access_method == DBAL_ACCESS_METHOD_KBP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "hitbit enable is not supported for for KBP tables (table %s)\n", table->table_name);
    }

    if ((table->access_method != DBAL_ACCESS_METHOD_MDB) && (table->access_method != DBAL_ACCESS_METHOD_KBP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "hitbit enable is not supported for tables that are not of type MDB or KBP (table %s)\n",
                     table->table_name);
    }
    if (!sw_state_is_warm_boot(unit))
    {
        if (hitbit)
        {
            SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HITBIT_EN);
        }
        else
        {
            SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_IS_HITBIT_EN);
        }
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.set(unit, table_id, table->indications_bm[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_optimize_commit_mode_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    if (!(DBAL_TABLE_IS_NONE_DIRECT(table)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "supported only for non direct tables (table %s)\n", table->table_name);
    }

    if (!sw_state_is_warm_boot(unit))
    {
        if (mode)
        {
            SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED);
        }
        else
        {
            SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED);
        }
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.set(unit, table_id, table->indications_bm[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_dirty_mode_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    if (mode > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table %s only 0/1 is legal for mode = %d\n",
                     logical_tables_info[unit]->logical_tables[table_id].table_name, mode);
    }

    if (!sw_state_is_warm_boot(unit))
    {
        if (mode)
        {
            SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED);
        }
        else
        {
            SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED);
        }
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.set(unit, table_id, table->indications_bm[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_protection_enable_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    if (mode > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table %s only 0/1 is legal for mode = %d\n",
                     logical_tables_info[unit]->logical_tables[table_id].table_name, mode);
    }

    if (!sw_state_is_warm_boot(unit))
    {
        if (mode)
        {
            if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Mutex already allocated for table %s, cannot alloc another one\n", table->table_name);
            }
            SHR_IF_ERR_EXIT(dbal_per_table_mutex_reserve(unit, table_id, &(table->mutex_id)));
            SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED);
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_per_table_mutex_release(unit, table_id, table->mutex_id));
            SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED);
        }
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.set(unit, table_id, table->indications_bm[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_learning_enable(
    int unit,
    dbal_tables_e table_id,
    uint32 learning)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    if (table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "learning is not supported for tables that are not access method MDB (table %s)\n",
                     logical_tables_info[unit]->logical_tables[table_id].table_name);
    }

    if (!sw_state_is_warm_boot(unit))
    {
        if (learning)
        {
            SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_LEARNING_EN);
        }
        else
        {
            SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_IS_LEARNING_EN);
        }
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.set(unit, table_id, table->indications_bm[0]));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_max_key_value_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *key_buffer)
{
    int ii;
    uint32 key_buffer_local[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if (BITS2BYTES(logical_tables_info[unit]->logical_tables[table_id].key_size -
                   logical_tables_info[unit]->logical_tables[table_id].core_id_nof_bits) > sizeof(int))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key size for table %s too long for usage %d, max key size is 32bit \n",
                     dbal_logical_table_to_string(unit, table_id),
                     BITS2BYTES(logical_tables_info[unit]->logical_tables[table_id].key_size));
    }
    sal_memset(key_buffer, 0, BITS2BYTES(logical_tables_info[unit]->logical_tables[table_id].key_size));

    for (ii = 0; ii < logical_tables_info[unit]->logical_tables[table_id].nof_key_fields; ii++)
    {
        dbal_logical_table_t *table = &(logical_tables_info[unit]->logical_tables[table_id]);

        uint32 key_field_max_val = table->keys_info[ii].max_value;
        int key_field_offset = table->keys_info[ii].bits_offset_in_buffer;
        int key_field_length = table->keys_info[ii].field_nof_bits;

        if (table->keys_info[ii].field_id == DBAL_FIELD_CORE_ID)
        {
            continue;
        }

        if (SHR_BITGET((table->keys_info[ii].field_indication_bm), DBAL_FIELD_IND_IS_FIELD_ENUM))
        {
            
            int enum_iter;
            dbal_field_types_basic_info_t *field_type_info;
            key_field_max_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, table->keys_info[ii].field_id, &field_type_info));
            for (enum_iter = 0; enum_iter < field_type_info->nof_enum_values; enum_iter++)
            {
                if ((key_field_max_val < field_type_info->enum_val_info[enum_iter].value) &&
                    (!field_type_info->enum_val_info[enum_iter].is_invalid))
                {
                    key_field_max_val = field_type_info->enum_val_info[enum_iter].value;
                }
            }
        }

        if (key_field_offset + key_field_length > 32)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key size for table %s too long for usage \n",
                         dbal_logical_table_to_string(unit, table_id));
        }
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                        (key_buffer_local, key_field_offset, key_field_length, key_field_max_val));
    }

    (*key_buffer) = key_buffer_local[0];

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_field_predefine_value_update(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    int is_key,
    int result_type,
    int instance_id,
    dbal_field_predefine_value_type_e value_type,
    uint32 predef_value)
{
    int nof_fields, iter;
    dbal_logical_table_t *table;
    dbal_table_field_info_t *fields_info;

    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit]->logical_tables[table_id];

    if (result_type > table->nof_result_types)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal result_type %d, max value %d\n", result_type, table->nof_result_types);
    }
    if (instance_id == INST_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "instance all not supported\n");
    }

    if (instance_id == INST_SINGLE)
    {
        instance_id = 0;
    }

    field_id = field_id + instance_id;

    if (is_key)
    {
        nof_fields = table->nof_key_fields;
        fields_info = table->keys_info;
    }
    else
    {
        nof_fields = table->multi_res_info[result_type].nof_result_fields;
        fields_info = table->multi_res_info[result_type].results_info;
    }

    for (iter = 0; iter < nof_fields; iter++)
    {
        if (fields_info[iter].field_id == field_id)
        {
            break;
        }
    }

    if (iter == nof_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "field not found %s\n", dbal_field_to_string(unit, field_id));
    }
    switch (value_type)
    {
        case DBAL_PREDEF_VAL_MIN_VALUE:
            if (predef_value > fields_info[iter].max_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "min value bigger then max value %d\n", fields_info[iter].max_value);
            }
            fields_info[iter].min_value = predef_value;
            break;

        case DBAL_PREDEF_VAL_MAX_VALUE:
            if (predef_value < fields_info[iter].min_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "max value lower then min value %d\n", fields_info[iter].min_value);
            }

            fields_info[iter].max_value = predef_value;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "illegal predef type %d\n", value_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_nof_instance_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int res_type,
    int *nof_inst)
{
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, field_id, is_key, res_type, 0, &field_info));
    *nof_inst = field_info.nof_instances;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_entry_counter_update(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries_to_update,
    uint8 is_add_operation)
{
    int jj;
    dbal_logical_table_t *table;
    int nof_entries;

    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;

    
    if ((dbal_tables_is_non_direct(unit, entry_handle->table_id) == FALSE) &&
        (table->access_method != DBAL_ACCESS_METHOD_KBP))
    {
        SHR_EXIT();
    }

     
    if (table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        dbal_physical_table_def_t *PhysicalTable;
        for (jj = 0; jj < table->nof_physical_tables; jj++)
        {
            if ((entry_handle->core_id != DBAL_CORE_ALL) && (jj != entry_handle->core_id))
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, table->physical_db_id[jj], &PhysicalTable));

            if (num_of_entries_to_update < 0)
            {
                
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, entry_handle->table_id, &nof_entries));
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_MDB_PHY_TBL.
                                nof_entries.dec(unit, table->physical_db_id[jj], nof_entries));
            }
            else if (is_add_operation)
            {
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_MDB_PHY_TBL.
                                nof_entries.inc(unit, table->physical_db_id[jj], num_of_entries_to_update));
            }
            else
            {
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_MDB_PHY_TBL.
                                nof_entries.dec(unit, table->physical_db_id[jj], num_of_entries_to_update));
            }
        }
    }

    if ((table->core_mode == DBAL_CORE_MODE_DPC) && ((entry_handle->core_id == DBAL_CORE_ALL)))
    {
        num_of_entries_to_update *= DBAL_MAX_NUM_OF_CORES;
    }
    
    if (num_of_entries_to_update < 0)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.set(unit, entry_handle->table_id, 0));
    }
    else if (is_add_operation)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.inc(unit, entry_handle->table_id, num_of_entries_to_update));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.dec(unit, entry_handle->table_id, num_of_entries_to_update));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if (res_type_idx < logical_tables_info[unit]->logical_tables[table_id].nof_result_types)
    {
        *p_size = logical_tables_info[unit]->logical_tables[table_id].multi_res_info[res_type_idx].entry_payload_size;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal result type index %d for table %s. num of result types is %d\n",
                     res_type_idx, logical_tables_info[unit]->logical_tables[table_id].table_name,
                     logical_tables_info[unit]->logical_tables[table_id].nof_result_types);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_predefine_value_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_field_predefine_value_type_e value_type,
    uint32 *predef_value)
{
    dbal_table_field_info_t table_field_info;
    dbal_field_types_basic_info_t *field_type_info;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_type_info));
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, field_id, is_key,
                                               result_type_idx, inst_idx, &table_field_info));
    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

    switch (value_type)
    {
        case DBAL_PREDEF_VAL_MIN_VALUE:
            (*predef_value) = table_field_info.min_value;
            if (table_field_info.arr_prefix_size != 0)
            {
                if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Min value of Enum fields with ARR prefix cannot be resolved. field: %s",
                                 dbal_field_to_string(unit, table_field_info.field_id));
                }

                SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                          table_field_info.field_nof_bits,
                                                          table_field_info.field_nof_bits +
                                                          table_field_info.arr_prefix_size, predef_value));
            }
            break;

        case DBAL_PREDEF_VAL_MAX_VALUE:
            if (table_field_info.field_nof_bits > UTILEX_NOF_BITS_IN_UINT32)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Not supported for fields bigger than 32bit. the max value should be according to the field size\n");
            }
            (*predef_value) = table_field_info.max_value;
            if (table_field_info.arr_prefix_size != 0)
            {
                if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Max value of Enum fields with ARR prefix cannot be resolved. field: %s",
                                 dbal_field_to_string(unit, table_field_info.field_id));
                }

                SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                          table_field_info.field_nof_bits,
                                                          table_field_info.field_nof_bits +
                                                          table_field_info.arr_prefix_size, predef_value));
            }
            break;

        case DBAL_PREDEF_VAL_DEFAULT_VALUE:

            if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_DEFAULT_VALUE_VALID))
            {
                (*predef_value) = field_type_info->default_value;
            }
            else
            {
                if (field_type_info->max_size <= 32)
                {
                    if (SHR_BITGET(table_field_info.field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
                    {
                        *predef_value = table_field_info.const_value;
                    }
                    else
                    {
                        (*predef_value) = 0;
                        if ((!is_key) && (table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC))
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "Default value for HL table result fields might be depended by the key value."
                                         " Need to use: dbal_fields_predefine_value_get, table %s field %s\n",
                                         table->table_name, dbal_field_to_string(unit, field_id));
                        }

                        if (table_field_info.arr_prefix_size != 0)
                        {
                            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                                      table_field_info.field_nof_bits,
                                                                      table_field_info.field_nof_bits +
                                                                      table_field_info.arr_prefix_size, predef_value));

                            if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                            {
                                int ii;
                                for (ii = 0; ii < field_type_info->nof_enum_values; ii++)
                                {
                                    if ((*predef_value) == field_type_info->enum_val_info[ii].value)
                                    {
                                        (*predef_value) = ii;
                                        break;
                                    }
                                }

                                if (ii == field_type_info->nof_enum_values)
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                                 "Enum with ARR prefix, default value couldn't be resolved. field: %s",
                                                 dbal_field_to_string(unit, table_field_info.field_id));
                                }
                            }
                        }
                    }
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "default value is not supported for fields bigger than 32bits: %s",
                                 dbal_field_to_string(unit, table_field_info.field_id));

                }
            }
            break;

        case DBAL_PREDEF_VAL_RESET_VALUE:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error, DBAL_PREDEF_VAL_RESET_VALUE is not supported \n");
            break;

        default:
            
            break;
    }

exit:
    SHR_FUNC_EXIT;
}


uint8
dbal_tables_is_non_direct(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table = &logical_tables_info[unit]->logical_tables[table_id];

    return DBAL_TABLE_IS_NONE_DIRECT(table);
}


uint8
dbal_tables_is_tcam(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table = &logical_tables_info[unit]->logical_tables[table_id];

    return DBAL_TABLE_IS_TCAM(table);
}


uint8
dbal_tables_is_merge_entries_supported(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table = &logical_tables_info[unit]->logical_tables[table_id];

    if (table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
    {
        if (table->sw_access_info.sw_payload_length_bytes > 0)
        {
            return 1;
        }

        if (table->hl_mapping_multi_res->l2p_hl_info[DBAL_HL_ACCESS_REGISTER].is_packed_fields ||
            table->hl_mapping_multi_res->l2p_hl_info[DBAL_HL_ACCESS_MEMORY].is_packed_fields)
        {
            return 1;
        }

        if (table->table_type != DBAL_TABLE_TYPE_DIRECT)
        {
            return 1;
        }
        return 0;
    }

    if (table->access_method == DBAL_ACCESS_METHOD_PEMLA && table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        return 0;
    }

    if (table->access_method == DBAL_ACCESS_METHOD_SW_STATE && table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        return 0;
    }

    return 1;
}

shr_error_e
dbal_tables_update_hw_error(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

    logical_tables_info[unit]->nof_tables_with_error++;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "DBAL table %s has HW issue HW elements are invalid \n"), table->table_name));
    
    if (!sw_state_is_warm_boot(unit))
    {
        if (DBAL_DEVICE_STATE_UNDER_DEVELOPMENT == dnx_data_dbal.db_init.dbal_device_state_get(unit) ||
            
            DBAL_ACCESS_METHOD_PEMLA == table->access_method)
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, table_id, DBAL_TABLE_HW_ERROR));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "HW error in table %s", dbal_logical_table_to_string(unit, table_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_nof_hw_error_tables_get(
    int unit,
    int *not_tables)
{
    SHR_FUNC_INIT_VARS(unit);

    (*not_tables) = logical_tables_info[unit]->nof_tables_with_error;

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_table_field_printable_string_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 buffer_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 buffer_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    int result_type_idx,
    uint8 is_key,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, field_id, table_id, buffer_val, buffer_mask,
                                                             result_type_idx, is_key, 0, buffer_to_print));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_table_printable_entry_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *key_buffer,
    int core_id,
    int payload_size,
    uint32 *payload_buffer,
    dbal_printable_entry_t * entry_print_info)
{
    int rv, ii;
    uint32 entry_handle_id;
    dbal_logical_table_t *table;
    dbal_entry_handle_t *entry_handle;
    char *field_value_to_print = NULL;
    uint32 *field_value = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(field_value_to_print, DBAL_MAX_PRINTABLE_BUFFER_SIZE,
                       "field_value_to_print", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(field_value, WORDS2BYTES(DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS),
                       "field_value", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    table = entry_handle->table;

    rv = dbal_entry_handle_key_payload_update(unit, entry_handle_id, key_buffer, NULL, core_id, payload_size,
                                              payload_buffer);

    if ((rv == _SHR_E_NOT_FOUND) || (rv == _SHR_E_NONE))
    {
        if (key_buffer)
        {
                        
                        
            entry_print_info->nof_key_fields = 0;
            for (ii = 0; ii < table->nof_key_fields; ii++)
            {
                int parsed_key_idx = entry_print_info->nof_key_fields;
                dbal_fields_e cur_field_id = table->keys_info[ii].field_id;

                sal_memset(field_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);

                if (cur_field_id == DBAL_FIELD_CORE_ID)
                {
                    continue;
                }
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, cur_field_id, field_value));
                SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                                (unit, table_id, cur_field_id, field_value, NULL, 0, TRUE, field_value_to_print));

                entry_print_info->key_fields_info[parsed_key_idx].field_id = cur_field_id;
                sal_strncpy_s(entry_print_info->key_fields_info[parsed_key_idx].field_name,
                              dbal_field_to_string(unit, cur_field_id), DBAL_MAX_LONG_STRING_LENGTH);
                sal_memcpy(entry_print_info->key_fields_info[parsed_key_idx].field_value, field_value,
                           DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
                sal_strncpy_s(entry_print_info->key_fields_info[parsed_key_idx].field_print_value, field_value_to_print,
                              DBAL_MAX_PRINTABLE_BUFFER_SIZE);
                entry_print_info->nof_key_fields++;
            }
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    if (rv == _SHR_E_NONE)
    {
        
        if (payload_buffer)
        {
            if (entry_handle->cur_res_type != DBAL_RESULT_TYPE_NOT_INITIALIZED)
            {
                entry_print_info->nof_res_fields = 0;
                for (ii = 0; ii < DBAL_RES_INFO.nof_result_fields; ii++)
                {
                    int parsed_res_idx = entry_print_info->nof_res_fields;
                    dbal_fields_e cur_field_id = DBAL_RES_INFO.results_info[ii].field_id;

                    sal_memset(field_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                    (unit, entry_handle_id, cur_field_id, INST_SINGLE, field_value));
                    SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                                    (unit, table_id, cur_field_id, field_value, NULL, entry_handle->cur_res_type, FALSE,
                                     field_value_to_print));

                    entry_print_info->res_fields_info[parsed_res_idx].field_id = cur_field_id;
                    sal_strncpy_s(entry_print_info->res_fields_info[parsed_res_idx].field_name,
                                  dbal_field_to_string(unit, cur_field_id), DBAL_MAX_LONG_STRING_LENGTH);
                    sal_memcpy(entry_print_info->res_fields_info[parsed_res_idx].field_value, field_value,
                               DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
                    sal_strncpy_s(entry_print_info->res_fields_info[parsed_res_idx].field_print_value,
                                  field_value_to_print, DBAL_MAX_PRINTABLE_BUFFER_SIZE);
                    entry_print_info->nof_res_fields++;
                }
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FREE(field_value_to_print);
    SHR_FREE(field_value);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_table_field_is_ranged_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 *is_ranged)
{
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, field_id, TRUE, 0, INST_SINGLE, &field_info));

    if (SHR_BITGET(field_info.field_indication_bm, DBAL_FIELD_IND_IS_RANGED))
    {
        (*is_ranged) = TRUE;
    }
    else
    {
        (*is_ranged) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_table_field_is_packed_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 *is_packed)
{
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, field_id, TRUE, 0, INST_SINGLE, &field_info));

    if (SHR_BITGET(field_info.field_indication_bm, DBAL_FIELD_IND_IS_PACKED))
    {
        (*is_packed) = TRUE;
    }
    else
    {
        (*is_packed) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_access_method_get(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e * access_method)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    (*access_method) = table->access_method;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_sizes_get(
    int unit,
    dbal_tables_e table_id,
    int *key_size,
    int *max_pld_size)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        *max_pld_size = table->max_payload_size - table->multi_res_info[0].results_info[0].field_nof_bits;
    }
    else
    {
        *max_pld_size = table->max_payload_size;
    }
    *key_size = (table->key_size - table->core_id_nof_bits);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_is_direct(
    int unit,
    dbal_tables_e table_id,
    int *is_table_direct)
{
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    *is_table_direct = 1;

    table = &(logical_tables_info[unit]->logical_tables[table_id]);

    if (DBAL_TABLE_IS_NONE_DIRECT(table))
    {
        *is_table_direct = 0;
    }

exit:
    SHR_FUNC_EXIT;

}
