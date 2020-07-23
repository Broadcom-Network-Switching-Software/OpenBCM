/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * \file diag_dnx_minicon.c
 */

#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*dnx_data_device.general.nof_cores_get(unit)*/

#if DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE > 2
#define DIAG_DNX_ARMOR_NUM_OF_DBS DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE
#else
#define DIAG_DNX_ARMOR_NUM_OF_DBS 2
#endif

/*
 * INCLUDES
 */
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <appl/diag/diag.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/appl/diag/dnx/dbal/diag_dnx_dbal.h>
#include <src/appl/diag/dnx/dbal/diag_dnx_dbal_internal.h>
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <src/appl/reference/dnx/minicontroller/armor/appl_ref_armor.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/field/tcam/tcam_location_algo.h>
#include <bcm_int/dnx/field/tcam/tcam_bank_manager.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <src/appl/diag/dnx/field/diag_dnx_field_utils.h>
#include <src/appl/diag/dnx/field/diag_dnx_field_tcam.h>
#include <appl/diag/dnx/diag_dnx_field.h>

extern shr_error_e dbal_populate_tables_from_xml_file(
    int unit,
    char *filename,
    sh_sand_control_t * sand_control);

extern shr_error_e diag_dbal_table_iterator_rules_read_and_set(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    int *nof_rules,
    args_t * args,
    sh_sand_control_t * sand_control);

typedef struct
{
    int phy_dbs_num;
    int is_tcam;
    CONST char *app_db_name;
    char *phy_db_name[DIAG_DNX_ARMOR_NUM_OF_DBS];
    uint32 key_size;
    uint32 max_payload_size;
    int nof_entries;
    int phy_db_capacity[DIAG_DNX_ARMOR_NUM_OF_DBS];
    uint32 phy_db_entries[DIAG_DNX_ARMOR_NUM_OF_DBS];

} armor_resource_struct_t;

static char algo_path[SH_SAND_MAX_TOKEN_SIZE];
static char res_dir[SH_SAND_MAX_TOKEN_SIZE];

shr_error_e
diag_armor_image_check(
    int unit,
    rhlist_t * list)
{
    uint8 is_armor = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * if init is not done, return that command is available - this is usually happening for autocomplete init
     * this is done to prevent access to DNX-Data before it is init
     */
    if (!dnx_init_is_init_done_get(unit))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_pp_prgm_current_image_check(unit, MINICON_ARMOR_NAME_STR, &is_armor));

    if (!is_armor)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_diag_armor_tcam_bank_used_resources_get(
    int unit,
    int core,
    uint32 *nof_entries)
{

    dnx_field_tcam_access_fg_params_t fg_params;
    uint32 nof_tcam_handlers, handler_id;
    uint32 bank_entries_count[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
    uint32 banks_bmp[1];
    int bank_id_iter;
    uint32 key_size;
    uint32 tmp_entries;
    dnx_field_tcam_handler_state_e state;

    SHR_FUNC_INIT_VARS(unit);

    *nof_entries = 0;

    nof_tcam_handlers = dnx_data_field.tcam.nof_tcam_handlers_get(unit);

    for (handler_id = 0; handler_id < nof_tcam_handlers; handler_id++)
    {

        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_state_get(unit, handler_id, &state));
        if (state != DNX_FIELD_TCAM_HANDLER_STATE_OPEN)
        {
            continue;
        }
        /** Get TCAM handler information. */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

        /** In case we are not DT, we should print the bank and entry usage. */
        if (!fg_params.direct_table)
        {
            key_size = fg_params.key_size;
            /** Retrieve information about entries and used TCAM banks. */
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entries_info(unit, core, handler_id, bank_entries_count, banks_bmp));

            for (bank_id_iter = 0; bank_id_iter < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_id_iter++)
            {

                if (SHR_BITGET(banks_bmp, bank_id_iter))
                {
                    if (key_size <= dnx_data_field.tcam.key_size_half_get(unit))
                    {
                        tmp_entries = (bank_entries_count[bank_id_iter] - 1) / 2 + 1;
                    }
                    else if (key_size > dnx_data_field.tcam.key_size_single_get(unit))
                    {
                        tmp_entries = bank_entries_count[bank_id_iter] * 2;
                    }
                    else
                    {
                        tmp_entries = bank_entries_count[bank_id_iter];
                    }

                    *nof_entries += tmp_entries;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_diag_armor_db_resources_get(
    int unit,
    char *table_ptr,
    armor_resource_struct_t * print_vals,
    sh_sand_control_t * sand_control)
{
    uint32 bank_entries_count[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
    uint32 banks_bmp[1];
    dnx_field_group_full_info_t *fg_info = NULL;
    int core = 0;
    CONST dbal_logical_table_t *table;
    uint32 phy_db_entries;
    int nof_phy_entries;
    dbal_tables_e table_id;
    uint32 tcam_handler_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s", EMPTY, EMPTY, EMPTY);

    sal_memset(print_vals, 0, sizeof(armor_resource_struct_t));

    SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_ptr, &table_id, sand_control));
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    print_vals->phy_db_name[DBAL_PHY_DB_DEFAULT_INDEX] =
        dbal_physical_table_to_string(unit, table->physical_db_id[DBAL_PHY_DB_DEFAULT_INDEX]);

    if (table->physical_db_id[DBAL_PHY_DB_DEFAULT_INDEX] == DBAL_PHYSICAL_TABLE_KBP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP based DB is not supported\n");
    }

    if (table->physical_db_id[DBAL_PHY_DB_DEFAULT_INDEX] == DBAL_PHYSICAL_TABLE_TCAM)
    {
        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, table_id, &tcam_handler_id));
        print_vals->is_tcam = 1;
        print_vals->phy_dbs_num = dnx_data_device.general.nof_cores_get(unit);

        for (core = 0; core < print_vals->phy_dbs_num; core++)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entries_info
                            (unit, core, tcam_handler_id, bank_entries_count, banks_bmp));

            SHR_IF_ERR_EXIT(_diag_armor_tcam_bank_used_resources_get(unit, core, &phy_db_entries));
            print_vals->phy_db_entries[core] = phy_db_entries;
            print_vals->phy_db_capacity[core] = DNX_DATA_MAX_FIELD_TCAM_NOF_ENTRIES_160_BITS;
        }

    }
    else
    {
        for (i = 0; i < DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE; i++)
        {
            if (table->physical_db_id[i] == DBAL_PHYSICAL_TABLE_NONE)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, table->physical_db_id[i],
                                                      &(print_vals->phy_db_capacity[i])));
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_MDB_PHY_TBL.
                            nof_entries.get(unit, table->physical_db_id[i], &nof_phy_entries));
            print_vals->phy_db_entries[i] = nof_phy_entries;
            print_vals->phy_db_name[i] = dbal_physical_table_to_string(unit, table->physical_db_id[i]);
            print_vals->phy_dbs_num++;
        }
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, table_id, &(print_vals->nof_entries)));

    print_vals->app_db_name = table->table_name;
    print_vals->key_size = table->key_size;
    print_vals->max_payload_size = table->max_payload_size;

exit:
    SHR_FREE(fg_info);
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_armor_clear_table(
    int unit,
    dbal_tables_e table_id,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    int nof_rules = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(diag_dbal_table_iterator_rules_read_and_set
                    (unit, entry_handle_id, table_id, &nof_rules, args, sand_control));

    if (nof_rules > 0)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, entry_handle_id, DBAL_ITER_ACTION_DELETE));
        SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_armor_clear_all_tables(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < minicon_armor_get_num_of_tables(unit); i++)
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_from_string
                        (unit, minicon_armor_get_table_name_by_index(unit, i), &table_id, sand_control));
        SHR_IF_ERR_EXIT(diag_dnx_armor_clear_table(unit, table_id, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_armor_clear_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id;
    char *table;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table);
    if (sal_strncasecmp(table, "ALL", sal_strlen(table)) == 0)
    {
        SHR_IF_ERR_EXIT(diag_dnx_armor_clear_all_tables(unit, args, sand_control));
    }
    else
    {
        if (!ISEMPTY(table))
        {
            SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table, &table_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_armor_clear_table(unit, table_id, args, sand_control));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_armor_init_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    char *tmp_algo_path;
    char *tmp_res_dir;
    char *cfg_path;
    bcm_field_context_t context_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("algo_path", tmp_algo_path);
    SH_SAND_GET_STR("result_dir", tmp_res_dir);
    SH_SAND_GET_STR("config_path", cfg_path);
    SH_SAND_GET_UINT32("pmf_context", context_id);

    if (ISEMPTY(tmp_algo_path))
    {
        tmp_algo_path = "/usr/bin";
    }

    if (ISEMPTY(tmp_res_dir))
    {
        tmp_res_dir = "/tmp";
    }

    sal_strncpy(algo_path, tmp_algo_path, sal_strlen(tmp_algo_path));
    sal_strncpy(res_dir, tmp_res_dir, sal_strlen(tmp_res_dir));

    if (!minicon_armor_is_initialized(unit))
    {
        SHR_IF_ERR_EXIT(minicon_armor_init(unit, context_id, cfg_path));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_armor_acl_file_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *acl_file;
    char *format;
    char *action;
    uint32 compression;
    int acl_compress_only = FALSE;
    int convert_only = FALSE;

    char command[SH_SAND_MAX_TOKEN_SIZE];
    char xml_file_ips[SH_SAND_MAX_TOKEN_SIZE];
    char xml_file_tcams[SH_SAND_MAX_TOKEN_SIZE];
    char dbal_file[SH_SAND_MAX_TOKEN_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if (!minicon_armor_is_initialized(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "Feature is not initizalized\n");
    }

    SH_SAND_GET_STR("action", action);
    SH_SAND_GET_STR("file", acl_file);
    SH_SAND_GET_STR("format", format);
    SH_SAND_GET_UINT32("compression", compression);

    if (ISEMPTY(action))
    {
        SHR_EXIT();
    }

    if (ISEMPTY(acl_file))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "File path is empty \n");
    }

    sal_sprintf(xml_file_ips, "%s/armor_ips_file.xml", res_dir);
    sal_sprintf(xml_file_tcams, "%s/armor_tcams_file.xml", res_dir);
    sal_sprintf(dbal_file, "%s/armor_dbal_tcams_file.xml", res_dir);

    if (sal_strncasecmp("load_compressed", action, sal_strlen(action)) == 0)
    {

        SHR_IF_ERR_EXIT(armor_convert_to_dbal_readable(unit, acl_file, dbal_file));
        SHR_IF_ERR_EXIT(dbal_populate_tables_from_xml_file(unit, dbal_file, sand_control));
    }
    else
    {
        if (sal_strncasecmp("compress_only", action, sal_strlen(action)) == 0)
        {
            acl_compress_only = TRUE;
        }
        else if (sal_strncasecmp("compress_and_convert", action, sal_strlen(action)) == 0)
        {
            convert_only = TRUE;
        }
        else if (sal_strncasecmp("compress_and_load", action, sal_strlen(action)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported action \n");
        }

        if (compression > 2)
        {
            compression = 0;
        }
        if (ISEMPTY(format))
        {
            format = "format_1";
        }
        else
        {
            if ((sal_strcasecmp(format, "default") != 0) && (sal_strcasecmp(format, "format_1") != 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported format \n");
            }
        }

        sal_sprintf(command,
                    "%s -acl_file %s -ips_out_file %s -tcam_out_file %s -format %s -compression_rate %d -print_stat",
                    algo_path, acl_file, xml_file_ips, xml_file_tcams, format, compression);

        sal_shell_cmd(command);

        if (convert_only == TRUE)
        {
            SHR_IF_ERR_EXIT(armor_convert_to_dbal_readable(unit, xml_file_tcams, dbal_file));
        }
        else if (acl_compress_only == FALSE)
        {
            SHR_IF_ERR_EXIT(armor_convert_to_dbal_readable(unit, xml_file_tcams, dbal_file));
            SHR_IF_ERR_EXIT(dbal_populate_tables_from_xml_file(unit, xml_file_ips, sand_control));
            SHR_IF_ERR_EXIT(dbal_populate_tables_from_xml_file(unit, dbal_file, sand_control));
        }
    }

exit:
    /*
     * delete tmp xml files
     */
    if (acl_compress_only == FALSE || convert_only == FALSE)
    {
        sal_remove(xml_file_ips);
        sal_remove(xml_file_tcams);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_armor_info_table(
    int unit,
    dbal_tables_e table_id,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!minicon_armor_is_initialized(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "Feature is not initizalized\n");
    }

    if (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_dbal_logical_table_dump(unit, table_id, 1, sand_control));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

static shr_error_e
diag_dnx_armor_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id;
    char *table;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    if (!minicon_armor_is_initialized(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "Feature is not initizalized\n");
    }

    SH_SAND_GET_STR("table", table);
    if (sal_strncasecmp(table, "ALL", sal_strlen(table)) == 0)
    {
        for (i = 0; i < minicon_armor_get_num_of_tables(unit); i++)
        {
            SHR_IF_ERR_EXIT(diag_dbal_table_from_string
                            (unit, minicon_armor_get_table_name_by_index(unit, i), &table_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_armor_info_table(unit, table_id, args, sand_control));

        }
    }
    else
    {
        if (!ISEMPTY(table))
        {
            SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table, &table_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_armor_info_table(unit, table_id, args, sand_control));
        }
    }
exit:
    SHR_FUNC_EXIT;

}

#define MINICON_ARMOR_PRINT_TABLE_RESOURCES(print_vals)                                                                \
do                                                                                                                     \
{                                                                                                                      \
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);                                                                                     \
    PRT_CELL_SET("%s", print_vals.app_db_name);                                                                        \
    PRT_CELL_SET("%d", print_vals.key_size);                                                                           \
    PRT_CELL_SET("%d", print_vals.max_payload_size);                                                                   \
    PRT_CELL_SET("%d", print_vals.nof_entries);                                                                        \
    if (print_vals.phy_dbs_num==1)                                                                                     \
    {                                                                                                                  \
        PRT_CELL_SET("%s", print_vals.phy_db_name[0]);                                                                 \
        PRT_CELL_SET("%d", print_vals.phy_db_capacity[0]);                                                             \
        PRT_CELL_SET("%d", print_vals.phy_db_entries[0]);                                                              \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        if(print_vals.is_tcam)                                                                                         \
        {                                                                                                              \
            PRT_CELL_SET("%s core_0\n%s core_1", print_vals.phy_db_name[0],print_vals.phy_db_name[0])                   \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            PRT_CELL_SET("%s\n%s", print_vals.phy_db_name[0],print_vals.phy_db_name[1])                                \
        }                                                                                                              \
        PRT_CELL_SET("%d\n%d", print_vals.phy_db_capacity[0], print_vals.phy_db_capacity[1]);                          \
        PRT_CELL_SET("%d\n%d", print_vals.phy_db_entries[0], print_vals.phy_db_entries[1]);                            \
    }                                                                                                                  \
}while(0)

static shr_error_e
diag_dnx_armor_resource_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *table;
    char *table_ptr;
    int i;
    armor_resource_struct_t print_vals;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (!minicon_armor_is_initialized(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "Feature is not initizalized\n");
    }

    PRT_TITLE_SET("Table Resource info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "App DB Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "App DB Entries");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Phy DB Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Max Phy DB Capacity");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Num of Phy DB Entries");

    SH_SAND_GET_STR("table", table);
    if (sal_strncasecmp(table, "ALL", sal_strlen(table)) == 0)
    {
        for (i = 0; i < minicon_armor_get_num_of_tables(unit); i++)
        {
            table_ptr = minicon_armor_get_table_name_by_index(unit, i);
            _diag_armor_db_resources_get(unit, table_ptr, &print_vals, sand_control);
            MINICON_ARMOR_PRINT_TABLE_RESOURCES(print_vals);

        }
    }
    else
    {
        if (!ISEMPTY(table))
        {
            table_ptr = table;
            _diag_armor_db_resources_get(unit, table_ptr, &print_vals, sand_control);
            MINICON_ARMOR_PRINT_TABLE_RESOURCES(print_vals);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;

}

static shr_error_e
diag_dnx_armor_dump_table(
    int unit,
    dbal_tables_e table_id,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_dbal_table_entries_print(unit, table_id, 0, 0, args, sand_control));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

static shr_error_e
diag_dnx_armor_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id;
    char *table;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    if (!minicon_armor_is_initialized(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "Feature is not initizalized\n");
    }

    SH_SAND_GET_STR("table", table);
    if (sal_strncasecmp(table, "ALL", sal_strlen(table)) == 0)
    {
        for (i = 0; i < minicon_armor_get_num_of_tables(unit); i++)
        {
            SHR_IF_ERR_EXIT(diag_dbal_table_from_string
                            (unit, minicon_armor_get_table_name_by_index(unit, i), &table_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_armor_dump_table(unit, table_id, args, sand_control));

        }
    }
    else
    {
        if (!ISEMPTY(table))
        {
            SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table, &table_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_armor_dump_table(unit, table_id, args, sand_control));
        }
    }
exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
diag_dnx_armor_counter_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 counter_id;
    int core_id = 0;
    uint64 output_data;

    SHR_FUNC_INIT_VARS(unit);

    if (!minicon_armor_is_initialized(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "Feature is not initizalized\n");
    }

    SH_SAND_GET_INT32("CORE", core_id);
    SH_SAND_GET_UINT32("ID", counter_id);

    SHR_IF_ERR_EXIT(minicon_armor_counter_get(unit, core_id, counter_id, &output_data));
    LOG_CLI((BSL_META("Counter ID:%d Core:%d value: " COMPILER_64_PRINTF_FORMAT " \n"), counter_id, core_id,
             output_data));

exit:
    SHR_FUNC_EXIT;

}

static sh_sand_option_t sh_dnx_armor_acl_options_args[] = {
    /*
     * Name | Type | Description | Default
     */
    {"FoRMat", SAL_FIELD_TYPE_STR, "ACL file input format", ""},
    {"ACTion", SAL_FIELD_TYPE_STR, "Action on input file", ""},
    {"FILE", SAL_FIELD_TYPE_STR, "Path to input file", ""},
    {"COMPRession", SAL_FIELD_TYPE_UINT32, "Compression density of input file", "0"},
    {NULL}      /* End of options list - must be last. */
};

static sh_sand_option_t sh_dnx_armor_init_options_args[] = {
    /*
     * Name | Type | Description | Default
     */
    {"ALGO_Path", SAL_FIELD_TYPE_STR, "Path to ACL algo executable", ""},
    {"ConFiG_Path", SAL_FIELD_TYPE_STR, "Path to ALGO conversion config", ""},
    {"RESult_Dir", SAL_FIELD_TYPE_STR, "Path to result dericetory", ""},
    {"PMF_Context", SAL_FIELD_TYPE_UINT32, "PMF context for the ACL population", "0"},
    {NULL}      /* End of options list - must be last. */
};

static sh_sand_option_t sh_dnx_armor_counter_options_args[] = {
    /*
     * Name | Type | Description | Default
     */
    {"ID", SAL_FIELD_TYPE_UINT32, "Counter ID", "0"},
    {NULL}      /* End of options list - must be last. */
};

sh_sand_man_t sh_dnx_armor_acl_file_man = {
    .brief = "ARMOR algo with ACL file",
    .full = "ARMOR algo operations with ACL file as input\n"
        "Actions:\nload_and_compress - full sequnce of loadining acl file compression and population of it\n"
        "compress_only - partial sequence of compressing acl file but not loading it\n"
        "load_compressed - partial sequence of loading a file, prevously compressed by compress_only stage\n",
    .synopsis =
        "[action=<load_and_compress/compress_only/load_compressed>] [file=<input file path>] [format=<default/format_1>] compression=[0,1,2] 0-best",
    .examples =
        "acl action=compress_only file=file.conf format=format_1 compression=2\n"
        "acl action=load_compressed file=/tmp/output_ips_file.xml"
};

sh_sand_man_t sh_dnx_armor_init_man = {
    .brief = "Init ARMOR algo",
    .full = "Init ARMOR algorith and all ARMOR DBs",
    .synopsis =
        "[algo_path=<path>] [result_dir=<output/tmp directiory path>] [pmf_context=<pmf context>] [config_path=<path for conversion config file>]",
    .examples = "init"
};

static sh_sand_option_t sh_dnx_armor_table_options_args[] = {
    /*
     * Name | Type | Description | Default
     */
    {"table", SAL_FIELD_TYPE_STR, "Apply action to specific ARMOR table or all tables", ""},
    {NULL}      /* End of options list - must be last. */
};

sh_sand_man_t sh_dnx_armor_dump_man = {
    .brief = "ARMOR dump DBs",
    .full = "Dump all ARMOR DBs",
    .synopsis = "[table=<name/all>]",
    .examples = "table=all"
};

sh_sand_man_t sh_dnx_armor_clear_man = {
    .brief = "ARMOR clear all DBs",
    .full = "ARMOR clear (empty) all DBs",
    .synopsis = "[table=<name/all>]",
    .examples = "table=all"
};

sh_sand_man_t sh_dnx_armor_info_man = {
    .brief = "ARMOR DBs info print",
    .full = "Dump a full info of ARMOR DBs",
    .synopsis = "[table=<name/all>]",
    .examples = "table=all"
};

sh_sand_man_t sh_dnx_armor_resource_man = {
    .brief = "ARMOR DBs resources print",
    .full = "Dump a full resources usage of ARMOR DBs",
    .synopsis = "[table=<name/all>]",
    .examples = "table=all"
};

sh_sand_man_t sh_dnx_armor_counter_man = {
    .brief = "ARMOR counters print",
    .full = "Get counter values",
    .synopsis = "[core=<core_id>] [id=<counter_id>]",
    .examples = "core=0 id=0"
};

sh_sand_cmd_t sh_dnx_armor_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"INiT", diag_dnx_armor_init_cmd, NULL, sh_dnx_armor_init_options_args, &sh_dnx_armor_init_man},
    {"ACL", diag_dnx_armor_acl_file_cmd, NULL, sh_dnx_armor_acl_options_args, &sh_dnx_armor_acl_file_man},
    {"DuMP", diag_dnx_armor_dump_cmd, NULL, sh_dnx_armor_table_options_args, &sh_dnx_armor_dump_man},
    {"Clear", diag_dnx_armor_clear_cmd, NULL, sh_dnx_armor_table_options_args, &sh_dnx_armor_clear_man},
    {"Info", diag_dnx_armor_info_cmd, NULL, sh_dnx_armor_table_options_args, &sh_dnx_armor_info_man},
    {"ReSouRCe", diag_dnx_armor_resource_cmd, NULL, sh_dnx_armor_table_options_args, &sh_dnx_armor_resource_man},
    {"COUnter", diag_dnx_armor_counter_cmd, NULL, sh_dnx_armor_counter_options_args, &sh_dnx_armor_counter_man},
    {NULL}
};

sh_sand_man_t sh_dnx_armor_man = {
    .brief = "ARMOR algo operations",
    .full = "ARMOR algo operations on raw file/xml file/dump/cler",
    /*
     * .synopsis = "[RawFile=<path>] [XmlFile=<path>] [Dump=<table>] [Clear=<table>]",
     */
    .examples = "TBD\n"
};

/* *INDENT-ON* */
