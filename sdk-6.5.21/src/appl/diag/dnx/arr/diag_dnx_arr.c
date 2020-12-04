/** \file diag_dnx_arr.c
 *
 * Main diagnostics for ARR All CLI commands, that are related to ARR, are gathered in this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDNX

/*************
 * INCLUDES  *
 */
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <appl/diag/diag.h>
#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_rhlist.h>
#include <soc/sand/sand_signals.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for soc_is_xxx*/
#include <soc/dnx/arr/arr.h>

#include "diag_dnx_arr.h"

/** \brief prints the result of ARR decoding */
static shr_error_e
dnx_arr_decode_print(
    int unit,
    rhlist_t * parsed_info,
    sh_sand_control_t * sand_control)
{
    int field_col_id;
    signal_output_t *field;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Parsed entries");

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &field_col_id, "Field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL, "Value");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL, "size");

    RHITERATOR(field, parsed_info)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", RHNAME(field));
        PRT_CELL_SET("%s", field->print_value);
        PRT_CELL_SET("%3db", field->size);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *      Implementation of the 'arr decode' diag command.
 *  \see
 *      dnx_arr_decode
 */
static shr_error_e
cmd_etps_arr_decode(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 *data;
    uint32 size;

    rhlist_t *flist = NULL;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Read parameters
     */
    SH_SAND_GET_ARRAY32("data", data);
    SH_SAND_GET_UINT32("size", size);
    /*
     * Call parsing service
     */
    SHR_IF_ERR_EXIT(dnx_etps_arr_decode(unit, SOC_CORE_DEFAULT, NULL, data, size, &flist, NULL, NULL));
    if (NULL == flist)
    {
        /*
         * Decoding did not find any suitable format.
         */
        uint32 data_type = 0;
        SHR_IF_ERR_EXIT(dnx_etps_arr_decode_data_type_get(unit, data, size, &data_type));
        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not find suitable formats for data type:\"%d\"\n", data_type);
    }
    /*
     * Print result list
     */
    SHR_IF_ERR_EXIT(dnx_arr_decode_print(unit, flist, sand_control));
exit:
    if (flist)
    {
        sand_signal_list_free(flist);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_etps_arr_dcd_options[] = {
    {"data", SAL_FIELD_TYPE_ARRAY32, "EES/ETPS payload", NULL},
    {"size", SAL_FIELD_TYPE_UINT32, "Payload size in bits", "130"},
    {NULL}
};

static sh_sand_man_t dnx_etps_arr_dcd_man = {
    .brief = "Decode EES/ETPS entry using ARR",
    .full = "Decode EES/ETPS entry using ARR",
    .synopsis = "data=<hex-string>",
    .examples = "data=0x0010032000000000000000000000000000\n" "data=0x02000000000cd1d0006800000000000000 size=64"
};

/** manual for the main menu */
sh_sand_man_t sh_dnx_arr_man = {
    .brief = "Miscellaneous ARR related commands",
    .full = NULL
};

/* INDENT-OFF */
/**
 * \brief DNX ARR diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 */
sh_sand_cmd_t sh_dnx_arr_cmds[] = {
   /***********************************************************************************************
    * CMD_NAME *     CMD_ACTION         * Next       *        Options         *     MAN           *
    *          *                        * Level      *                        *                   *
    *          *                        * CMD        *                        *                   *
 */
    {"ETPS_Decode", cmd_etps_arr_decode, NULL, dnx_etps_arr_dcd_options, &dnx_etps_arr_dcd_man},
    {NULL}
};
/* INDENT-ON */
