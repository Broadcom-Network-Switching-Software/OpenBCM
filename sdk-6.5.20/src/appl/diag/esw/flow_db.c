/*
 *  $Id: flow_db.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  File: flow db.c
 *  Purpose:  diag commands for flow db
 */

#ifndef NO_SAL_APPL

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <soc/drv.h>
#include <soc/esw/cancun.h>
#include <soc/esw/flow_db.h>
#include <soc/esw/flow_db_core.h>

char cmd_flow_db_usage [] =
    "Parameters: [flow_info|flow|flow_option|ffo|flow_logical_field|view|help] <args...>\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "  Flow DB database for flexible tables.\n"\
    "\t flow_info show - Detailed per flow information. \n"\
    "\t flow show  - Flow table, Displays available flows and associated flow handles.\n"\
    "\t flow_option show - Flow option table, Displays avaiable options per flow\n"\
    "\t ffo show - Flow function option tuple table\n\n"\
    "\t ffo entry [<flow_id>] [<function_id>] [<option_id> \n"\
               " \t\t- Display the associated view id for flow, function, option combination \n"\
    "\t flow_logical_field show - Logical field table, displays logical fields per flow. \n"\
    "\t view show - View table \n"\
    "\t view entry [<view_id>] - \n"\
               " \t\t- View information.\n"\
    "\t view field entry [<view_id> <field_id>] - \n"\
               " \t\t- Field information. \n"\
    "\t help\t - display this usage page.\n"\
    "\n"\
    "  arguments for each operation:\n"
    #endif
    ;
STATIC cmd_result_t _cmd_flow_db_flow_tbl(int unit);
STATIC cmd_result_t _cmd_flow_db_flow_op_tbl(int unit);
STATIC cmd_result_t _cmd_flow_db_flow_lg_tbl(int unit);
STATIC cmd_result_t _cmd_flow_db_ffo_tpl_tbl(int unit,
                                             int flow_handle,
                                             int func_id,
                                             int option_id);
STATIC cmd_result_t
_cmd_flow_db_view_tbl(int unit, int view_id, int field_id);
STATIC cmd_result_t _cmd_flow_db_flow_info_show(int unit,
                                          char *flow_name);

/*
 * function:   cmd_flow_db
 * purpose:    flow db for flexible tables.
 * parameters: unit - unit
 *             a - args, each of the files to be displayed.
 * returns:    CMD_OK/CMD_FAIL/CMD_USAGE
 */
cmd_result_t
cmd_flow_db(int unit, args_t *a)
{
    char *c;
    int flow_handle = -1;
    int func_id = -1;
    int option_id = -1;
    int view_id = -1;
    int field_id = -1;
    int rv = 0;

    if (!soc_feature(unit, soc_feature_cancun)) {
        return (CMD_NFND);
    }


    if (!soc_feature(unit,soc_feature_flex_flow)) {
        return (CMD_NFND);
    }

    c = ARG_GET(a);
    if(!c) {
        return (CMD_USAGE);
    } else if(!sal_strcasecmp(c, "help")) {
        return (CMD_USAGE);
    }

    if (!sal_strcmp(c, "flow_info")) {
               c = ARG_GET(a);
        if (c) {
            if (sal_strcmp(c, "show")) {
                return (CMD_FAIL);
            }
            c = ARG_GET(a);
            if (!c) {
                return (CMD_FAIL);
            }
            rv = (int) _cmd_flow_db_flow_info_show(unit, c);
        }else
              return (CMD_FAIL);
     } else if (!sal_strcmp(c, "flow")) {
        c = ARG_GET(a);
        if (c) {
            if (sal_strcmp(c, "show")) {
                return (CMD_FAIL);
            }
          rv = (int) _cmd_flow_db_flow_tbl(unit);
        } else
              return (CMD_FAIL);
    } else if (!sal_strcmp(c, "flow_option")) {
               c = ARG_GET(a);
        if (c) {
            if (sal_strcmp(c, "show")) {
                return (CMD_FAIL);
            }
            rv = (int) _cmd_flow_db_flow_op_tbl(unit);
        } else
              return (CMD_FAIL);
    } else if (!sal_strcmp(c, "ffo")) {
        c = ARG_GET(a);
        if (c) {
            if (!sal_strcmp(c, "show")) {
                flow_handle = func_id = option_id = -1;
            } else if (!sal_strcmp(c, "entry")) {
                flow_handle = sal_ctoi(ARG_GET(a), 0);
                func_id = sal_ctoi(ARG_GET(a), 0);
                option_id = sal_ctoi(ARG_GET(a), 0);
            } else {
                return (CMD_FAIL);
            }
            rv = (int) _cmd_flow_db_ffo_tpl_tbl(unit,
                    flow_handle, func_id, option_id);
        } else
              return (CMD_FAIL);
    } else if (!sal_strcmp(c, "flow_logical_field")) {
        c = ARG_GET(a);
        if (c) {
            if (sal_strcmp(c, "show")) {
                return (CMD_FAIL);
            }
            rv = (int) _cmd_flow_db_flow_lg_tbl(unit);
        } else
              return (CMD_FAIL);
    } else if (!sal_strcmp(c, "view")) {
       c = ARG_GET(a);
       if (c) {
           if (!sal_strcmp(c, "show")) {
               view_id = -1;
               field_id = -1;
           } else if (!sal_strcmp(c, "entry")) {
               view_id = sal_ctoi(ARG_GET(a), 0);
               field_id = -1;
           } else if (!sal_strcmp(c, "field")) {
               c = ARG_GET(a);
               if (!sal_strcmp(c, "entry")) {
                   view_id = sal_ctoi(ARG_GET(a), 0);
                   field_id = sal_ctoi(ARG_GET(a), 0);
               } else {
                  return (CMD_FAIL);
               }
           } else {
               return (CMD_FAIL);
           }
           rv = (int) _cmd_flow_db_view_tbl(unit, view_id, field_id);
       } else
            return (CMD_FAIL);
    } else {
       rv = SOC_E_NOT_FOUND;
    }

    if (rv != 0) {
        return (CMD_FAIL);
    }

    return (CMD_OK);
}

STATIC cmd_result_t
_cmd_flow_db_flow_tbl(int unit)
{
    soc_cancun_t *cc = NULL;
    soc_flow_db_t *flow_db = NULL;
    soc_flow_db_tbl_map_t *tbl_map = NULL;
    soc_flow_db_legacy_flow_rec_t *fmap = NULL;
    soc_flow_db_legacy_flow_rec_t *legacy_entry = NULL;
    soc_flow_db_flow_rec_t *entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 j = 0;
    uint32 num_hash_entries = 0;
    uint32 fmap_entries = 0;
    char *str_tbl;

    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    if ((soc_cancun_info_get(unit, &cc)) != SOC_E_NONE)
    {
        return(CMD_FAIL);
    }

    flow_db = cc->flow_db;

    if (flow_db == NULL) {
        return(CMD_FAIL) ;
    }

    tbl_map = SOC_FLOW_DB_FLOW_TBL_MAP(unit);

    if (tbl_map == NULL) {
        return(CMD_FAIL);
    }

    if (soc_flow_db_legacy_flow_map_get(unit, &fmap, &fmap_entries) != SOC_E_NONE) {
        return(CMD_FAIL);
    }

    num_hash_entries = tbl_map->pe;
    hash_entry = &tbl_map->tbl_entry;
    cli_out("\n    FLOW TABLE\n");
    cli_out("=========================================="\
            "======================================\n");
    cli_out("%-48s  %-10s \n", "flow_name", "flow_handle");
    cli_out("========================================"\
            "========================================\n");

    for (i = 0, legacy_entry = fmap;
         i < fmap_entries;
         i++, legacy_entry++) {
        cli_out("%-48s  %-10d \n",
                      legacy_entry->name,
                      legacy_entry->flow_handle);

    }

    for (i = 0; i < num_hash_entries; i++) {
        offset = *(hash_entry + i);
        if (offset == 0) {
            continue;
        }

        p = hash_entry + offset;
        entry_num = *p++;
        entry = (soc_flow_db_flow_rec_t *) p;
        if (entry_num !=0) {
            for (j = 0; j < entry_num; j++, entry++) {
                cli_out("%-48s  %-10d \n",
                         str_tbl + entry->name_offset,
                         entry->flow_handle);
            }
        }

    }
    return(CMD_OK);
}

STATIC cmd_result_t
_cmd_flow_db_flow_op_tbl(int unit)
{

    soc_cancun_t *cc = NULL;
    soc_flow_db_t *flow_db = NULL;
    soc_flow_db_tbl_map_t *tbl_map = NULL;
    soc_flow_db_flow_option_rec_t *entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 j = 0;
    uint32 num_hash_entries = 0;
    char *str_tbl;

    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    if ((soc_cancun_info_get(unit, &cc)) != SOC_E_NONE)
    {
        return(CMD_FAIL);
    }

    flow_db = cc->flow_db;

    if (flow_db == NULL) {
        return(CMD_FAIL) ;
    }

    tbl_map = SOC_FLOW_DB_FLOW_OP_TBL_MAP(unit);

    if (tbl_map == NULL) {
        return(CMD_FAIL);
    }

    num_hash_entries = tbl_map->pe;
    hash_entry = &tbl_map->tbl_entry;
    cli_out("\n    FLOW OPTION TABLE \n");
    cli_out("========================================="\
            "===================================================\n");
    cli_out("%-13s %-64s %-10s \n", "FLOW_HANDLE", "OPTION_NAME", "OPTION_ID");
    cli_out("========================================="\
            "===================================================\n");
    for (i = 0; i < num_hash_entries; i++) {
        offset = *(hash_entry + i);
        if (offset == 0) {
            continue;
        }

        p = hash_entry + offset;
        entry_num = *p++;
        entry = (soc_flow_db_flow_option_rec_t *) p;
        if (entry_num !=0) {
            for (j = 0; j < entry_num; j++, entry++) {
                cli_out("%-13d %-64s %-10d \n",
                         entry->flow_handle,
                         str_tbl + entry->name_offset,
                         entry->option_id);
            }
        }

    }
    return(CMD_OK);
}

STATIC cmd_result_t _cmd_flow_db_ffo_tpl_tbl(int unit,
                                             int flow_handle,
                                             int func_id,
                                             int option_id)
{

    soc_cancun_t *cc = NULL;
    soc_flow_db_t *flow_db = NULL;
    soc_flow_db_tbl_map_t *tbl_map = NULL;
    soc_flow_db_ffo_tuple_rec_t *entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 j = 0;
    uint32 num_hash_entries = 0;
    uint32 view_id = 0;
    int rv = -1;


    if ((func_id != -1) && (flow_handle != -1) &&
        (option_id != -1)) {
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                                     flow_handle,
                                     option_id,
                                     func_id,
                                     &view_id);
        if (SOC_SUCCESS(rv)) {
            cli_out("%-s : %d\n", "FLOW_HANDLE", flow_handle);
            cli_out("%-s : %d\n", "FUNCTION_ID", func_id);
            cli_out("%-s : %d\n", "OPTION_ID", option_id);
            cli_out("%-s : %d\n", "VIEW_ID", view_id);
            cli_out("\n");
        }
    } else {
        if ((soc_cancun_info_get(unit, &cc)) != SOC_E_NONE)
        {
            return(CMD_FAIL);
        }

        flow_db = cc->flow_db;

        if (flow_db == NULL) {
            return(CMD_FAIL) ;
        }

        tbl_map = SOC_FLOW_DB_FFO_TPL_TBL_MAP(unit);

        if (tbl_map == NULL) {
            return(CMD_FAIL);
        }

        num_hash_entries = tbl_map->pe;
        hash_entry = &tbl_map->tbl_entry;
        cli_out("\n    FLOW FUNCTION OPTION TUPLE TABLE \n");
        cli_out("========================================="\
                "==================================\n");
        cli_out("%-13s %-13s %-10s %-10s\n",
                 "FLOW_HANDLE", "FUNCTION_ID", "OPTION_ID", "VIEW_ID");
        cli_out("========================================="\
                "==================================\n");

        for (i = 0; i < num_hash_entries; i++) {
            offset = *(hash_entry + i);
            if (offset == 0) {
                continue;
            }

            p = hash_entry + offset;
            entry_num = *p++;
            entry = (soc_flow_db_ffo_tuple_rec_t *) p;
            if (entry_num !=0) {
                for (j = 0; j < entry_num; j++, entry++) {
                    cli_out("%-13d %-13d %-10d %-10d \n",
                             entry->flow_handle,
                             entry->function_id,
                             entry->option_id,
                             entry->view_id);
                }
            }

        }
    }
    return(CMD_OK);
}

STATIC cmd_result_t
_cmd_flow_db_flow_lg_tbl(int unit)
{

    soc_cancun_t *cc = NULL;
    soc_flow_db_t *flow_db = NULL;
    soc_flow_db_tbl_map_t *tbl_map = NULL;
    soc_flow_db_flow_lg_field_rec_t *entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 j = 0;
    uint32 num_hash_entries = 0;
    char *str_tbl;

    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    if ((soc_cancun_info_get(unit, &cc)) != SOC_E_NONE)
    {
        return(CMD_FAIL);
    }

    flow_db = cc->flow_db;

    if (flow_db == NULL) {
        return(CMD_FAIL) ;
    }

    tbl_map = SOC_FLOW_DB_FLOW_LG_FL_TBL_MAP(unit);

    if (tbl_map == NULL) {
        return(CMD_FAIL);
    }

    num_hash_entries = tbl_map->pe;
    hash_entry = &tbl_map->tbl_entry;
    cli_out("\n    FLOW LOGICAL FIELDS TABLE \n");
    cli_out("==================================="\
            "=============================================\n");
    cli_out("%-13s %-48s %-10s \n", "FLOW_HANDLE", "LOGICAL FIELD", "FIELD_ID");
    cli_out("==================================="\
            "=============================================\n");

    for (i = 0; i < num_hash_entries; i++) {
        offset = *(hash_entry + i);
        if (offset == 0) {
            continue;
        }

        p = hash_entry + offset;
        entry_num = *p++;
        entry = (soc_flow_db_flow_lg_field_rec_t *) p;
        if (entry_num !=0) {
            for (j = 0; j < entry_num; j++, entry++) {
                cli_out("%-13d %-48s %-10d \n",
                         entry->flow_handle,
                         str_tbl + entry->name_offset,
                         entry->field_id);
            }
        }

    }
    return(CMD_OK);
}

STATIC cmd_result_t
_cmd_flow_db_view_tbl(int unit, int view_id, int field_id)
{
    soc_cancun_t *cc = NULL;
    soc_flow_db_t *flow_db = NULL;
    soc_flow_db_tbl_map_t *tbl_map = NULL;
    soc_flow_db_view_rec_t *entry = NULL;
    soc_flow_db_field_rec_t *f_entry = NULL;
    int i = 0;
    int offset = -1;
    uint32 *p = NULL;
    uint32 *pf = NULL;
    uint32 *hash_entry = NULL;
    uint32 entry_num = 0;
    uint32 j = 0;
    uint32 num_hash_entries = 0;
    soc_flow_db_mem_view_info_t view_info;
    soc_flow_db_mem_view_field_info_t field_info;
    int rv = -1;
    uint32 k = 0;
    soc_flow_db_ffo_t ffo[SOC_FLOW_DB_MAX_VIEW_FFO_TPL];
    uint32 num_ffo = 0;
    char *field_name = NULL;
    char *mem_name = NULL;
    uint32 size = SOC_FLOW_DB_MAX_VIEW_FIELDS;
    uint32 fields[SOC_FLOW_DB_MAX_VIEW_FIELDS];
    uint32 field_count = 0;
    uint32 field_type[] = {SOC_FLOW_DB_FIELD_TYPE_KEY,
                           SOC_FLOW_DB_FIELD_TYPE_CONTROL,
                           SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                           SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA_ENCODING,
                           SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                           SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                           SOC_FLOW_DB_FIELD_TYPE_DATA_FRAGMENT,
                           };
    char *_t_names[] = BCM_FEILD_TYPE_NAME_INITIALIZER;
    char *t_name = NULL;
    char *str_tbl;

    str_tbl      = SOC_FLOW_DB_STR_TBL(unit);

    if ((view_id != -1) ||
        (field_id != -1)) {

        if (field_id == -1) {
            /* only view infromation */
            cli_out("\n============================VIEW INFORAMTION===="\
                    "=============================\n");
            rv = soc_flow_db_mem_view_info_get(unit, view_id,
                                               &view_info);
            if (SOC_FAILURE(rv)) {
                return (CMD_FAIL);
            }
            rv = soc_flow_db_mem_view_mem_name_get(unit, view_id,
                                               &mem_name);
            if (SOC_FAILURE(rv)) {
                return (CMD_FAIL);
            }

            cli_out("%-10s : %d\n", "VIEW_ID", view_info.mem_view_id);
            cli_out("%-10s : %s\n", "PHY MEM", mem_name);
            cli_out("%-10s : %d\n", "WIDTH", view_info.width);

            switch(view_info.type) {
                case SOC_FLOW_DB_VIEW_TYPE_DIRECT:
                     cli_out("%-10s : %s\n", "TYPE", "SOC_FLOW_DB_VIEW_TYPE_DIRECT");
                     break;
                case SOC_FLOW_DB_VIEW_TYPE_HASH:
                     cli_out("%-10s : %s\n", "TYPE", "SOC_FLOW_DB_VIEW_TYPE_HASH");
                     break;
                case SOC_FLOW_DB_VIEW_TYPE_TCAM:
                     cli_out("%-10s : %s\n", "TYPE", "SOC_FLOW_DB_VIEW_TYPE_TCAM");
                     break;
                default:
                     cli_out("%-10s : %s\n", "TYPE", "SOC_FLOW_DB_VIEW_TYPE_INVALID");
                     break;
            }

            cli_out("\n    FIELD TABLE\n");
            cli_out("======================================================="\
                    "==================================================\n");
            cli_out("%-10s  %-48s  %-32s\n", "FEILD_ID",
                     "FIELD_NAME", "FIELD_TYPE");
            cli_out("======================================================="\
                    "==================================================\n");
            for (i = 0; i < sizeof(field_type)/sizeof(field_type[0]);
                 i++) {

                SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_field_list_get(
                                    unit,
                                    view_id,
                                    field_type[i],
                                    size,
                                    fields,
                                    &field_count));
                for (j = 0; j < field_count; j++) {
                    /* field table per view */
                    rv = soc_flow_db_mem_view_field_info_get(unit, view_id,
                                             fields[j], &field_info);
                    if (SOC_FAILURE(rv)) {
                        return (CMD_FAIL);
                    }

                    rv = soc_flow_db_mem_view_field_name_get(unit, view_id,
                                      field_info.field_id, &field_name);

                    if (SOC_FAILURE(rv)) {
                        return (CMD_FAIL);
                    }
                    if ((field_info.type < SOC_FLOW_DB_FIELD_TYPE_INVALID) ||
                       (field_info.type > SOC_FLOW_DB_FIELD_TYPE_MAX)) {
                        return (CMD_FAIL);
                    }
                    t_name = _t_names[field_info.type];
                    cli_out("%-10d  %-48s  %-32s\n", field_info.field_id, field_name, t_name);
                } /* end of field list per type */
            } /* end of field  types - table */
            /* ffo table per view */
            cli_out("\n");

            rv = soc_flow_db_mem_view_to_ffo_get(unit,
                                             view_id,
                                             SOC_FLOW_DB_MAX_VIEW_FFO_TPL,
                                             ffo,
                                             &num_ffo);
            if (SOC_FAILURE(rv)) {
                return (CMD_FAIL);
            }
            cli_out("\n    FFO TUPLE INFORMATION\n");
            cli_out("=================================="\
                    "======================\n");
            cli_out("%-13s  %-13s  %-13s\n",
                    "FLOW_HANDLE ", "FUNCTION_ID", "OPTION_ID");
            cli_out("=======================\n"\
                    "=================================\n");
            for (i = 0; i < num_ffo; i++) {
                 cli_out("%-13d  %-13d  %-13d\n",
                         ffo[i].flow_handle,
                         ffo[i].function_id,
                         ffo[i].option_id);

            }
            cli_out("\n");

        } else {
           /* field information */
           rv = soc_flow_db_mem_view_field_info_get(unit, view_id,
                                     field_id, &field_info);
            if (SOC_FAILURE(rv)) {
                return (CMD_FAIL);
            }
            rv = soc_flow_db_mem_view_field_name_get(unit, view_id,
                              field_info.field_id, &field_name);

            if (SOC_FAILURE(rv)) {
                return (CMD_FAIL);
            }

            cli_out("%-10s : %d\n", "FIELD_ID", field_info.field_id);
            cli_out("%-10s : %s\n", "NAME", field_name);
            switch(field_info.type) {
                case SOC_FLOW_DB_FIELD_TYPE_CONTROL:
                     cli_out("%-10s : %s\n",
                             "TYPE", "SOC_FLOW_DB_FIELD_TYPE_CONTROL");
                     break;
                case SOC_FLOW_DB_FIELD_TYPE_KEY:
                     cli_out("%-10s : %s\n",
                             "TYPE", "SOC_FLOW_DB_FIELD_TYPE_KEY");
                     break;
                case SOC_FLOW_DB_FIELD_TYPE_VALID:
                     cli_out("%-10s : %s\n",
                             "TYPE", "SOC_FLOW_DB_FIELD_TYPE_VALID");
                     break;
                case SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA:
                     cli_out("%-10s : %s\n",
                             "TYPE", "SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA");
                     break;
                case SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA_ENCODING:
                     cli_out("%-10s : %s\n",
                          "TYPE", "SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA_ENCODING");
                     break;
                case SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY:
                     cli_out("%-10s : %s\n",
                          "TYPE", "SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY");
                     break;
                case SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA:
                     cli_out("%-10s : %s\n",
                          "TYPE", "SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA");
                     break;
                case SOC_FLOW_DB_FIELD_TYPE_DATA_FRAGMENT:
                     cli_out("%-10s : %s\n",
                          "TYPE", "SOC_FLOW_DB_FIELD_TYPE_DATA_FRAGMENT");
                     break;
                default:
                     cli_out("%-10s : %s\n",
                          "TYPE", "SOC_FLOW_DB_FIELD_TYPE_INVALID");
                     break;
            }

            cli_out("%-10s : %d\n", "OFFSET", field_info.offset);
            cli_out("%-10s : %d\n", "width", field_info.width);
            cli_out("%-10s : %d\n", "flags", field_info.flags);
            cli_out("%-10s : %d\n", "value", field_info.value);
            cli_out("\n");
        }

    } else {
        if ((soc_cancun_info_get(unit, &cc)) != SOC_E_NONE)
        {
            return(CMD_FAIL);
        }
        flow_db = cc->flow_db;

        if (flow_db == NULL) {
            return(CMD_FAIL) ;
        }

        tbl_map = SOC_FLOW_DB_VIEW_TBL_MAP(unit);

        if (tbl_map == NULL) {
            return(CMD_FAIL);
        }

        num_hash_entries = tbl_map->pe;
        hash_entry = &tbl_map->tbl_entry;
        cli_out("\n    VIEW TABLE \n");
        cli_out("========================================"\
                "=====================================\n");
        cli_out("%-10s  %-10s \n", "VIEW ID ", "PHY_MEM");
        cli_out("========================================"\
                "=====================================\n");

        for (i = 0; i < num_hash_entries; i++) {
            offset = *(hash_entry + i);
            if (offset == 0) {
                continue;
            }

            p = hash_entry + offset;
            entry_num = *p++;
            entry = (soc_flow_db_view_rec_t *) p;
            if (entry_num !=0) {
                for (j = 0; j < entry_num; j++, entry++) {
                    cli_out("%-10d  %-10d \n",
                             entry->view_id,
                             entry->mem);
                    cli_out("%-12s: %d\n", "NFields",entry->nFields);
                    pf = &entry->field_entry;
                    f_entry = (soc_flow_db_field_rec_t *)pf;
                    for (k = 0; k < entry->nFields; k++, f_entry++) {
                        cli_out("%-12s : %d\n", "Field_id",f_entry->field_id);
                        cli_out("%-12s : %s\n","Field_name",
                                         str_tbl + f_entry->name_offset);
                    }
                }
            }

        }
    }
    return(CMD_OK);
}

STATIC cmd_result_t
_cmd_flow_db_flow_info_show(int unit, char *flow_name)
{
    uint32 handle;
    soc_cancun_t *cc = NULL;
    soc_flow_db_t *flow_db = NULL;
    soc_flow_db_tbl_map_t *ffo_tbl_map = NULL;
    soc_flow_db_ffo_tuple_rec_t *ffo_entry = NULL;
    int offset1 = -1;
    uint32 *ffo = NULL;
    uint32 *ffo_hash_entry = NULL;
    uint32 ffo_entry_num = 0;
    uint32 ffo_num_hash_entries = 0;
    char *_func_names[] = BCM_FUNCTION_NAME_INITIALIZER;
    char *fname = NULL;
    int rv = -1;
    int k, l, m;
    uint32 func_type = SOC_FLOW_DB_FUNC_ID_START;
    char *option_name;
    char *mem_name;

    if ((soc_cancun_info_get(unit, &cc)) != SOC_E_NONE)
    {
        return(CMD_FAIL);
    }

    rv = soc_flow_db_flow_handle_get(unit, flow_name, &handle);
    if (rv != SOC_E_NONE) {
        return CMD_FAIL;
    }

    flow_db = cc->flow_db;
    if (flow_db == NULL) {
        return(CMD_FAIL) ;
    }


    cli_out ("\n FLOW : %s \n", flow_name);
    cli_out("================================================================"\
        "==================================================="\
        "===================================\n");
    cli_out("%-60s  %-20s  %-7s  %-s\n",
          "OPTION_NAME", "FUNCTION", "VIEW_ID", "MEMORY");
    cli_out("================================================================"\
        "===================================================="\
        "===================================\n");


    /* flow match */

    for (k = SOC_FLOW_DB_FUNC_ID_START+1;
         k < SOC_FLOW_DB_FUNC_ID_END;
         k++) {
        func_type = k;
        fname = _func_names[k];
        ffo_tbl_map = SOC_FLOW_DB_FFO_TPL_TBL_MAP(unit);
        if (ffo_tbl_map == NULL) {
            return(CMD_FAIL);
        }
        ffo_num_hash_entries = ffo_tbl_map->pe;
        ffo_hash_entry = &ffo_tbl_map->tbl_entry;
        for (l = 0; l < ffo_num_hash_entries; l++) {
            offset1 = *(ffo_hash_entry + l);
            if (offset1 == 0) {
                continue;
            }

            ffo = ffo_hash_entry + offset1;
            ffo_entry_num = *ffo++;
            ffo_entry = (soc_flow_db_ffo_tuple_rec_t *) ffo;
            for (m = 0; (ffo_entry_num != 0) &&
                (m < ffo_entry_num);
                 m++, ffo_entry++) {
                if ((ffo_entry->flow_handle  ==
                    handle) &&
                    (ffo_entry->function_id == func_type)) {
                   /*Get the option name from option id */
                   rv = soc_flow_db_mem_view_option_name_get(unit,
                             ffo_entry->flow_handle,
                             ffo_entry->option_id,
                             &option_name);
                   if (rv != SOC_E_NONE) {
                       return (CMD_FAIL);
                   }
                   rv = soc_flow_db_mem_view_mem_name_get(unit,
                                           ffo_entry->view_id,
                                           &mem_name);
                   if (rv != SOC_E_NONE) {
                       return (CMD_FAIL);
                   }

                   fname = _func_names[ffo_entry->function_id];
                   cli_out("%-60s  %-20s  %-7d  %-s\n",option_name,
                                fname, ffo_entry->view_id,
                                mem_name);

                }
            }

        } /* end of ffo Table */

    }  /* end of function type */

    return(CMD_OK);
}
#endif /* NO_SAL_APPL */
