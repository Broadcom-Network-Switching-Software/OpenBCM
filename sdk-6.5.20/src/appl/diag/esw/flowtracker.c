/*
 *  $Id: flowtracker.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  File: flowtracker.c
 *  Purpose:  diag commands for flowtracker
 */
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/types.h>
#include <bcm/error.h>

#if defined (BCM_TOMAHAWK3_SUPPORT)
#include <bcm/flowtracker.h>
#include <soc/tomahawk3.h>
#endif


#if defined(BCM_FLOWTRACKER_SUPPORT)
#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_export.h>
#include <bcm_int/esw/flowtracker/ft_class.h>
#include <bcm_int/esw/flowtracker/ft_udf.h>
#include <bcm_int/esw/flowtracker/ft_field.h>
#endif

#if defined(INCLUDE_FLOWTRACKER)
#include <bcm_int/esw/flowtracker.h>
#endif /* INCLUDE_FLOWTRACKER */
#if defined(BCM_APPL_SIGNATURE_SUPPORT)
#include <bcm/switch.h>
#include <bcm_int/esw/switch_appl_sign.h>
#endif


char cmd_ft_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  FlowTracker <option> [args...]\n"
#else
#if defined (BCM_TOMAHAWK3_SUPPORT)
    "  ft etrap stats [Clear=<True|False>]\n\t"
    "      - Show the ETRAP statistics\n"
    "  ft etrap dump [Pipe=<pipe>] [StartIndex=<index>] [EndIndex=<index>] [Valid=<True|False>]\n\t"
    "      - Dump Elephant flow table entries\n"
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    "  ft init    - Init FT.\n"
    "  ft detach  - Detach FT.\n"

    "  ft key add <DIP>=<val> <SIP>=<val> <L4DestPort>=<val> <L4SrcPortVRF>=<val>]\n\t"
    "                   <Proto>=<val>] \n\t"
    "                   - Add an entry into FT table with above key\n"
    "  ft key lookup <DIP>=<val> <SIP>=<val> <L4DestPort>=<val> <L4SrcPortVRF>=<val>]\n\t"
    "                   <Proto>=<val>] \n\t"
    "                   - Returns the index associated with the above key.\n"
    "  ft show group <Group=<val> - Dumps the state of group. \n"
    "  ft show session_data <Group=<val> <DIP>=<val> <SIP>=<val> <L4DestPort>=<val> <L4SrcPortVRF>=<val>]\n\t"
    "                   <Proto>=<val>] \n\t"
    "                   - Dumps the Session data corresponding to user provided key.\n"
    "  ft show alu32 <index=<val> - Dump ALU32 entry with index. \n"
    "  ft show alu16 <index=<val> - Dump ALU32 entry with index. \n"
    "  ft show fifo - Dump FT Export FIFO configurations. \n"
    "  ft show appl <type>=<val> id=<val> - Dump Application signature \n"
    "  ft show udf - Dump UDF tacking param mapping configuations \n"
    "  ft show param - Dump tacking params \n"
    "\n"
#endif
#if defined(INCLUDE_FLOWTRACKER)
    "  ft stats learn \n\t"
    "      - Show the Learn statistics\n"
    "  ft stats export \n\t"
    "      - Show the Export statistics\n"
    "  ft stats age \n\t"
    "      - Show the Aging statistics\n"
    "  ft stats elephant \n\t"
    "      - Show the Elephant statistics\n"
#endif /* INCLUDE_FLOWTRACKER */

#endif /* !COMPILER_STRING_CONST_LIMIT */
;

char cmd_qcm_usage[] =
#ifndef COMPILER_STRING_CONST_LIMIT
    "Usages:\n"
#if defined(INCLUDE_FLOWTRACKER)
    "  qcm stats enable \n\t"
    "      - Enable QCM statistics update\n"
    "  qcm stats disable \n\t"
    "      - Disable QCM statistics update\n"
    "  qcm stats learn \n\t"
    "      - Show the QCM Learn statistics\n"
    "  qcm stats scan \n\t"
    "      - Show the scan statistics\n"
    "  qcm stats export \n\t"
    "      - Show the Export statistics\n"
    "  qcm stats age \n\t"
    "      - Show the Aging statistics\n"
#endif /* INCLUDE_FLOWTRACKER */
#endif /* !COMPILER_STRING_CONST_LIMIT */
;

#if defined (BCM_TOMAHAWK3_SUPPORT)
/*
 * function:   cmd_ft_th3_etrap
 * purpose:    TH3 Flowtracker ETRAP diag commands
 * parameters: unit - unit
 *             a - args, each of the files to be displayed.
 * returns:    CMD_OK/CMD_FAIL/CMD_USAGE
 */
cmd_result_t
cmd_ft_th3_etrap(int unit, args_t *a)
{
    char *c;
    int rv;
    int pipe, start_idx, end_idx, valid, clear;
    parse_table_t pt;
    cmd_result_t retCode;

    if (!soc_feature(unit, soc_feature_hw_etrap)) {
        return (CMD_NFND);
    }

    c = ARG_GET(a);
    if(!c) {
        return (CMD_USAGE);
    }
    if (!sal_strcmp(c, "stats")) {
        /* Set  default value */
        clear = FALSE;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Clear",      PQ_DFL|PQ_BOOL, &clear,      &clear,     NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        rv = _bcm_th3_ft_dump_stats(unit, clear);
	if (rv != BCM_E_NONE) {
	    cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}
    } else if (!sal_strcmp(c, "dump")) {
        /* Set up the default values */
        pipe      = 0;
        start_idx = 0;
        end_idx   = -1;
        valid     = TRUE;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Pipe",       PQ_DFL|PQ_INT,  &pipe,      &pipe,      NULL);
        parse_table_add(&pt, "StartIndex", PQ_DFL|PQ_INT,  &start_idx, &start_idx, NULL);
        parse_table_add(&pt, "EndIndex",   PQ_DFL|PQ_INT,  &end_idx,   &end_idx,   NULL);
        parse_table_add(&pt, "Valid",      PQ_DFL|PQ_BOOL, &valid,     &valid,     NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        rv = _bcm_th3_ft_dump_flow_table(unit, pipe, start_idx, end_idx, valid);
	if (rv != BCM_E_NONE) {
	    cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}
    }
    return CMD_OK;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_FLOWTRACKER_SUPPORT)

STATIC cmd_result_t
bcmi_diag_ft_table_insert(int unit, args_t *a)
{
    cmd_result_t      retCode;
    parse_table_t     pt;
/*    int               rv;*/
    bcm_ip_t dip=0, sip=0;
    int l4destport=0, l4srcport=0;
    int proto=0;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "DIP", PQ_DFL|PQ_IP, 0, (void*)&dip, NULL);
    parse_table_add(&pt, "SIP", PQ_DFL|PQ_IP, 0, (void*)&sip, NULL);
    cli_out("DIP = %d: SIP = %d\n", dip, sip);
    parse_table_add(&pt, "L4DestPort", PQ_DFL|PQ_INT, 0, (void*)&l4destport, NULL);
    parse_table_add(&pt, "L4SrcPort", PQ_DFL|PQ_INT, 0, (void*)&l4srcport, NULL);
    parse_table_add(&pt, "PROTO", PQ_DFL|PQ_INT, 0, (void*)&proto, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcmi_ft_table_insert(unit, dip, sip, l4destport, l4srcport, proto);

    return CMD_OK;
}




STATIC cmd_result_t
bcmi_diag_ft_table_lookup(int unit, args_t *a)
{
    cmd_result_t      retCode;
    parse_table_t     pt;
/*    int               rv;*/
    bcm_ip_t dip=0, sip=0;
    int l4destport=0, l4srcport=0;
    int proto=0;
    int index = -1;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "DIP", PQ_DFL|PQ_IP, 0, (void*)&dip, NULL);
    parse_table_add(&pt, "SIP", PQ_DFL|PQ_IP, 0, (void*)&sip, NULL);
    cli_out("DIP = %d: SIP = %d\n", dip, sip);
    parse_table_add(&pt, "L4DestPort", PQ_DFL|PQ_INT, 0, (void*)&l4destport, NULL);
    parse_table_add(&pt, "L4SrcPort", PQ_DFL|PQ_INT, 0, (void*)&l4srcport, NULL);
    parse_table_add(&pt, "PROTO", PQ_DFL|PQ_INT, 0, (void*)&proto, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcmi_ft_table_lookup(unit, 0, dip, sip,
        l4destport, l4srcport, proto, &index);

    cli_out("FT table index is = %d \n", index);

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_ft_show_all(int unit, args_t *a, int brief)
{
    bcmi_ft_state_dump(unit, brief);

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_ft_group_dump(int unit, args_t *a)
{
    cmd_result_t      retCode;
    parse_table_t     pt;
    int group_id = -1;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Group", PQ_DFL|PQ_INT, 0, (void*)&group_id, NULL);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    cli_out("Flowtracker Group Id :\t%5d\n", group_id);

    bcmi_ft_group_state_dump(unit, group_id, FALSE);

    return CMD_OK;
}





STATIC cmd_result_t
bcmi_diag_ft_session_data_dump(int unit, args_t *a)
{
    cmd_result_t      retCode;
    parse_table_t     pt;
    bcm_ip_t dip=0, sip=0;
    int l4destport=0, l4srcport=0;
    int proto=0;
    int index = -1;
    int group_id = -1;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Group", PQ_DFL|PQ_INT, 0, (void*)&group_id, NULL);
    parse_table_add(&pt, "DIP", PQ_DFL|PQ_IP, 0, (void*)&dip, NULL);
    parse_table_add(&pt, "SIP", PQ_DFL|PQ_IP, 0, (void*)&sip, NULL);
    parse_table_add(&pt, "L4DestPort", PQ_DFL|PQ_INT, 0, (void*)&l4destport, NULL);
    parse_table_add(&pt, "L4SrcPort", PQ_DFL|PQ_INT, 0, (void*)&l4srcport, NULL);
    parse_table_add(&pt, "PROTO", PQ_DFL|PQ_INT, 0, (void*)&proto, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcmi_ft_table_lookup(unit, group_id, dip, sip,
        l4destport, l4srcport, proto, &index);

    cli_out("FT table index is = %d \n", index);

    if (index != -1) {

        bcmi_ft_group_session_data_dump(unit, group_id, index);
    }

    return CMD_OK;
}


STATIC cmd_result_t
bcmi_diag_ft_session_data_index_dump(int unit, args_t *a)
{
    cmd_result_t      retCode;
    parse_table_t     pt;
    int index = -1;
    int group_id = -1;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Group", PQ_DFL|PQ_INT, 0, (void*)&group_id, NULL);
    parse_table_add(&pt, "index", PQ_DFL|PQ_INT, 0, (void*)&index, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    cli_out("FT table index is = %d and group = %d \n", index, group_id);

    if (index != -1) {

        bcmi_ft_group_session_data_dump(unit, group_id, index);
    }

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_ft_agg_alu32_dump(int unit, args_t *a)
{
    cmd_result_t      retCode;
    parse_table_t     pt;
    int index = 0;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "index", PQ_DFL|PQ_INT, 0, (void*)&index, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    cli_out("Agg Alu32 index to dump is = %d\n", index);

    bcmi_ft_alu32_dump(unit, bcmiFtAluLoadTypeAggAlu32, index);

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_ft_alu32_dump(int unit, args_t *a)
{
    cmd_result_t      retCode;
    parse_table_t     pt;
    int index = 0;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "index", PQ_DFL|PQ_INT, 0, (void*)&index, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    cli_out("Alu32 index to dump is = %d\n", index);

    bcmi_ft_alu32_dump(unit, bcmiFtAluLoadTypeAlu32, index);

    return CMD_OK;
}


STATIC cmd_result_t
bcmi_diag_ft_alu16_dump(int unit, args_t *a)
{
    cmd_result_t      retCode;
    parse_table_t     pt;
    int index = 0;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "index", PQ_DFL|PQ_INT, 0, (void*)&index, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    cli_out("Alu32 index to dump is = %d\n", index);

    bcmi_ft_alu16_dump(unit, index);

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_ft_alu_hash_dump(int unit, args_t *a)
{
    bcmi_ft_alu_load_hash_dump(unit);

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_ft_aggregate_class_show(int unit, args_t *a)
{
    parse_table_t pt;
    uint32 agg_class = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Id", PQ_DFL|PQ_INT, NULL, &agg_class, NULL);

    if (ARG_CNT(a)) {
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    }
    parse_arg_eq_done(&pt);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    bcmi_ftv2_class_show(unit);
#endif

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    bcmi_ftv3_aggregate_class_show(unit, agg_class);
#endif

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_ft_export_fifo_show(int unit, args_t *a)
{
    bcmi_ft_export_fifo_alloc_show(unit);

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_appl_signature_show(int unit, args_t *a)
{
#if defined(BCM_APPL_SIGNATURE_SUPPORT)
    parse_table_t pt;
    uint32 type, options;
    bcm_switch_appl_signature_t appl_sign_id;

    type = 0;
    appl_sign_id = BCMI_SWITCH_APPL_SIGNATURE_INVALID_ID;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Type", PQ_DFL|PQ_INT, NULL, &type, NULL);
    parse_table_add(&pt, "Id", PQ_DFL|PQ_INT, NULL, &appl_sign_id, NULL);

    if (ARG_CNT(a)) {
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    }
    parse_arg_eq_done(&pt);

    if (type == 1) {
        options = BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD;
    } else {
        options = 0;
    }

    bcmi_esw_switch_appl_signature_show(unit, options, appl_sign_id);
#endif

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_ft_udf_param_info_show(int unit, args_t *a)
{
    bcmi_ft_udf_tracking_param_info_show(unit);

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_ft_tracking_param_show(int unit, args_t *a)
{
    _bcm_field_ft_tracking_param_show(unit);

    return CMD_OK;
}

STATIC cmd_result_t
bcmi_diag_ft_entry_show(int unit, args_t *a)
{
    parse_table_t pt;
    int gid = -1;
    int tbl_id = 0;
    int index = -1;
    int count = -1;
    char *tbl_str = NULL;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Group", PQ_DFL|PQ_INT, NULL, &gid, NULL);
    parse_table_add(&pt, "Table", PQ_DFL|PQ_INT, NULL, &tbl_id, NULL);
    parse_table_add(&pt, "Index", PQ_DFL|PQ_INT, NULL, &index, NULL);
    parse_table_add(&pt, "Count", PQ_DFL|PQ_INT, NULL, &count, NULL);

    if (ARG_CNT(a)) {
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    }
    parse_arg_eq_done(&pt);

    switch(tbl_id) {
        case 1:  tbl_str = "double"; break;
        case 2:  tbl_str = "agg0";   break;
        case 3:  tbl_str = "agg1";   break;
        case 4:  tbl_str = "agg2";   break;
        case 5:  tbl_str = "agg3";   break;
        default: tbl_str = "single"; break;
    }

    cli_out("Tbl ID = %d\n", tbl_id);
    cli_out("Table  = %s\n", tbl_str);
    cli_out("Index  = %d\n", index);
    cli_out("Count  = %d\n", count);

    /* If use have given index and not count, then
       set count to 1
     */
    if ((count == -1) && (index != -1)) {
        count = 1;
    }

    bcmi_ft_group_hw_entries_decode(unit,
            0, gid, tbl_str, index, count);

    return CMD_OK;
}


#endif /* BCM_FLOWTRACKER_SUPPORT */


/*
 * function:   cmd_ft
 * purpose:    Flowtracker diag commands
 * parameters: unit - unit
 *             a - args, each of the files to be displayed.
 * returns:    CMD_OK/CMD_FAIL/CMD_USAGE
 */
cmd_result_t
cmd_ft(int unit, args_t *a)
{
    char *c;
#if defined (BCM_FLOWTRACKER_SUPPORT) || defined (INCLUDE_FLOWTRACKER)
    char *subcmd_s;
#endif /* BCM_FLOWTRACKER_SUPPORT || INCLUDE_FLOWTRACKER */
#if defined (BCM_FLOWTRACKER_SUPPORT)
    int r;
    int ft_key_flag = FALSE;
    int group_dump_enable = FALSE;
#endif

    /* Check valid device to operation on ... */
    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    c = ARG_GET(a);
    if(!c) {
        return (CMD_USAGE);
    } else if(!sal_strcasecmp(c, "help")) {
        return (CMD_USAGE);
    }

#if defined (BCM_TOMAHAWK3_SUPPORT)
    if (!sal_strcmp(c, "etrap")) {
        return cmd_ft_th3_etrap(unit, a);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {

        /* FT init */
        if (sal_strcasecmp(c, "init") == 0) {
            if ((r = bcmi_ft_init(unit)) < 0) {
                cli_out("%s: error initializing: %s\n", ARG_CMD(a), bcm_errmsg(r));
            }
            return CMD_OK;
        } 

        /* FT detach */
        if (sal_strcasecmp(c, "detach") == 0) {
            if ((r = bcmi_ft_cleanup(unit)) < 0) {
                cli_out("%s: error detaching: %s\n", ARG_CMD(a), bcm_errmsg(r));
            }
            return CMD_OK;
        }

        /* Start gettng the key and then we can add entry in hash table.*/
        if ((sal_strcasecmp(c, "key") == 0)) {
            ft_key_flag = TRUE;
            if ((subcmd_s = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }
        }
        if (ft_key_flag) {
            if (sal_strcasecmp(subcmd_s, "add") == 0) {
                return bcmi_diag_ft_table_insert(unit, a);
            }

            if (sal_strcasecmp(subcmd_s, "lookup") == 0) {
                return bcmi_diag_ft_table_lookup(unit, a);
            }

        }


        if ((sal_strcasecmp(c, "s") == 0) ||
            (sal_strcasecmp(c, "show") == 0)) {
            group_dump_enable = TRUE;
            if ((subcmd_s = ARG_GET(a)) == NULL) {
                return bcmi_diag_ft_show_all(unit, a, FALSE);
            }
        }
        if (group_dump_enable) {
            if ((sal_strcasecmp(subcmd_s, "b") == 0) ||
                (sal_strcasecmp(subcmd_s, "brief") == 0)) {
                return bcmi_diag_ft_show_all(unit, a, TRUE);
            }
            if (sal_strcasecmp(subcmd_s, "group") == 0) {
                return bcmi_diag_ft_group_dump(unit, a);
            }

            if (sal_strcasecmp(subcmd_s, "session_data") == 0) {
                return bcmi_diag_ft_session_data_dump(unit, a);
            }

            if (sal_strcasecmp(subcmd_s, "session_data_index") == 0) {
                return bcmi_diag_ft_session_data_index_dump(unit, a);
            }

            if (sal_strcasecmp(subcmd_s, "alu32") == 0) {
                return bcmi_diag_ft_alu32_dump(unit, a);
            }
            if (sal_strcasecmp(subcmd_s, "alu16") == 0) {
                return bcmi_diag_ft_alu16_dump(unit, a);
            }
            if (sal_strcasecmp(subcmd_s, "aggalu32") == 0) {
                return bcmi_diag_ft_agg_alu32_dump(unit, a);
            }
            if (sal_strcasecmp(subcmd_s, "alu_hash") == 0) {
                return bcmi_diag_ft_alu_hash_dump(unit, a);
            }
            if ((sal_strcasecmp(subcmd_s, "e") == 0) ||
                (sal_strcasecmp(subcmd_s, "entry") == 0)) {
                return bcmi_diag_ft_entry_show(unit, a);
            }
            if ((sal_strcasecmp(subcmd_s, "c") == 0) ||
                (sal_strcasecmp(subcmd_s, "class") == 0)) {
                return bcmi_diag_ft_aggregate_class_show(unit, a);
            }
            if (sal_strcasecmp(subcmd_s, "fifo") == 0) {
                return bcmi_diag_ft_export_fifo_show(unit, a);
            }
            if (sal_strcasecmp(subcmd_s, "appl") == 0) {
                return bcmi_diag_appl_signature_show(unit, a);
            }
            if (sal_strcasecmp(subcmd_s, "udf") == 0) {
                return bcmi_diag_ft_udf_param_info_show(unit, a);
            }
            if (sal_strcasecmp(subcmd_s, "param") == 0) {
                return bcmi_diag_ft_tracking_param_show(unit, a);
            }
        }

        /* ft list ... */
        if (sal_strcasecmp(c, "list") == 0) {
            if ((subcmd_s = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }
            if (sal_strcasecmp(subcmd_s, "ift") == 0) {
                bcmi_ft_group_tracking_params_list(unit, BCMI_FT_TYPE_F_IFT);
                return CMD_OK;
            } else if (sal_strcasecmp(subcmd_s, "mft") == 0) {
                bcmi_ft_group_tracking_params_list(unit, BCMI_FT_TYPE_F_MFT);
                return CMD_OK;
            } else if (sal_strcasecmp(subcmd_s, "eft") == 0) {
                bcmi_ft_group_tracking_params_list(unit, BCMI_FT_TYPE_F_EFT);
                return CMD_OK;
            } else if (sal_strcasecmp(subcmd_s, "aift") == 0) {
                bcmi_ft_group_tracking_params_list(unit, BCMI_FT_TYPE_F_AIFT);
                return CMD_OK;
            } else if (sal_strcasecmp(subcmd_s, "amft") == 0) {
                bcmi_ft_group_tracking_params_list(unit, BCMI_FT_TYPE_F_AMFT);
                return CMD_OK;
            } else if (sal_strcasecmp(subcmd_s, "aeft") == 0) {
                bcmi_ft_group_tracking_params_list(unit, BCMI_FT_TYPE_F_AEFT);
                return CMD_OK;
            } else if (sal_strcasecmp(subcmd_s, "all") == 0) {
                bcmi_ft_group_tracking_params_list(unit, BCMI_FT_TYPE_F_ANY);
                return CMD_OK;
            } else {
                return CMD_USAGE;
            }
        }
    }
#endif /* BCM_FLOWTRACKER_SUPPORT */

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) { 
        /* FT app stats */
        if (sal_strcasecmp(c, "stats") == 0) {
            if ((subcmd_s = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }
            if (sal_strcasecmp(subcmd_s, "learn") == 0) {
                _bcm_esw_flowtracker_dump_stats_learn(unit);
            }
            if (sal_strcasecmp(subcmd_s, "export") == 0) {
                _bcm_esw_flowtracker_dump_stats_export(unit);
            }
            if (sal_strcasecmp(subcmd_s, "age") == 0) {
                _bcm_esw_flowtracker_dump_stats_age(unit);
            }
            if ((sal_strcasecmp(subcmd_s, "elephant") == 0) &&
                (!soc_feature(unit, soc_feature_hw_etrap))) {
                 _bcm_esw_flowtracker_dump_stats_elephant(unit);
            }

            return CMD_OK;
        }
    }
#endif /* INCLUDE_FLOWTRACKER */

    return CMD_NFND;
}

/*
 * function:   cmd_qcm
 * purpose:    QCM diag commands
 * parameters: unit - unit
 *             a - args, each of the files to be displayed.
 * returns:    CMD_OK/CMD_FAIL/CMD_USAGE
 */
cmd_result_t
cmd_qcm(int unit, args_t *a)
{
    char *c;
#if defined (INCLUDE_FLOWTRACKER)
    char *subcmd_s;
#endif /* INCLUDE_FLOWTRACKER */

    /* Check valid device to operation on ... */
    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    c = ARG_GET(a);
    if(!c) {
        return (CMD_USAGE);
    } else if(!sal_strcasecmp(c, "help")) {
        return (CMD_USAGE);
    }

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export)) {
        if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
            /* QCM app stats */
            if (sal_strcasecmp(c, "stats") == 0) {
               if ((subcmd_s = ARG_GET(a)) == NULL) {
                    return CMD_USAGE;
               }
               if (sal_strcasecmp(subcmd_s, "enable") == 0) {
                    _bcm_esw_qcm_stats_enable(unit, 1);
               }
               if (sal_strcasecmp(subcmd_s, "disable") == 0) {
                    _bcm_esw_qcm_stats_enable(unit, 0);
               }
               if (sal_strcasecmp(subcmd_s, "learn") == 0) {
                    _bcm_esw_qcm_dump_stats_learn(unit);
               }
               if (sal_strcasecmp(subcmd_s, "export") == 0) {
                    _bcm_esw_qcm_dump_stats_export(unit);
               }
               if (sal_strcasecmp(subcmd_s, "age") == 0) {
                   _bcm_esw_qcm_dump_stats_age(unit);
               }
               if (sal_strcasecmp(subcmd_s, "scan") == 0) {
                   _bcm_esw_qcm_dump_stats_scan(unit);
               }
           }
        }
    }
#endif /* INCLUDE_FLOWTRACKER */
    return CMD_NFND;
}
