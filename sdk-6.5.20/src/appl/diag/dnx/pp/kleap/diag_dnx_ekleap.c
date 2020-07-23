/** \file diag_dnx_ekleap.c
 *
 * Diagnostic for egress KLEAP stages (ETPP).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGKLEAPDNX

/*
 * INCLUDES
 * {
 */
#include <sal/appl/sal.h>
#include <sal/types.h>

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_str.h>

#include <soc/sand/sand_signals.h>
#include <soc/sand/sand_pparse.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/arr/arr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_visibility.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_signals.h>
#include <appl/diag/sand/diag_sand_utils.h>

#include "diag_dnx_ekleap.h"
#include "../diag_dnx_pp.h"
/*
 * }
 */

/*
 * Utilities
 * {
 */

extern shr_error_e dbal_mdb_table_id_and_result_type_from_etps_format_get(
    int unit,
    uint32 etps_format,
    dbal_tables_e * table_id,
    int *result_type);

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

extern shr_error_e dbal_tables_table_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    dbal_tables_e * table_id);
/**
 * \brief
 *   Adds a signal with it's parsing to a prt table.
 *   if first_row is TRUE, doesn't add row or skip cells.
 */
static void
ekleap_signal_prt(
    int unit,
    signal_output_t * sig,
    uint8 first_row,
    int cells_to_skip,
    int shift,
    prt_control_t * prt_ctr)
{
    signal_output_t *sub_field;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(sig, _SHR_E_NOT_FOUND, "sig");

    if (!first_row)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(cells_to_skip);
    }
    if (!ISEMPTY(sig->expansion) && sal_strncasecmp(sig->expansion, RHNAME(sig), RHNAME_MAX_SIZE))
    {
        PRT_CELL_SET_SHIFT(shift, "%s(%s)", RHNAME(sig), sig->expansion);
    }
    else
    {
        PRT_CELL_SET_SHIFT(shift, "%s", RHNAME(sig));
    }
    PRT_CELL_SET("%s", sig->print_value);
    RHITERATOR(sub_field, sig->field_list)
    {
        ekleap_signal_prt(unit, sub_field, FALSE, cells_to_skip, shift + 1, prt_ctr);
    }
exit:
    SHR_VOID_FUNC_EXIT;
}

/**
 * \brief
 *   Print entry that was parsed by dbal_table_printable_entry_get
 */
static void
ekleap_dbal_entry_info_prt(
    int unit,
    uint8 first_row,
    int cells_to_skip,
    int shift,
    dbal_printable_entry_t * entry,
    prt_control_t * prt_ctr)
{
    int fld;
    SHR_FUNC_INIT_VARS(unit);
    for (fld = 0; fld < entry->nof_res_fields; fld++)
    {
        if ((fld > 0) || (!first_row))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        }
        if (cells_to_skip)
        {
            PRT_CELL_SKIP(cells_to_skip);
        }
        PRT_CELL_SET_SHIFT(shift, "%s", entry->res_fields_info[fld].field_name);
        PRT_CELL_SET("%s", entry->res_fields_info[fld].field_print_value);
    }
exit:
    SHR_VOID_FUNC_EXIT;
}

/**
 * \brief
 *   Unified way to print that a signal is not available
 */
static void
ekleap_signal_not_found_prt(
    int unit,
    uint8 add_row,
    int skip,
    char *name,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);
    if (add_row)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    }
    if (skip)
    {
        PRT_CELL_SKIP(skip);
    }
    if (name)
    {
        PRT_CELL_SET("%s", name);
    }
    PRT_CELL_SET("Signal Unavailable");
exit:
    SHR_VOID_FUNC_EXIT;
}

/**
 * \brief
 *   Replace the ETPS entry signal with a parsing by dbal_table_printable_entry_get
 */
static shr_error_e
ekleap_etps_sig_reparse_with_dbal(
    int unit,
    int core,
    signal_output_t * sig)
{
    /*
     * Decalring an array to appease Coverity
     */
    uint32 entry_type[1];
    dbal_tables_e table_id;
    int dummy_res_type;
    dbal_printable_entry_t *entry_info = NULL;
    int fld;

    SHR_FUNC_INIT_VARS(unit);
    if (diag_pp_bare_metal_check(unit, NULL) != _SHR_E_NONE)
    {
        /*
         * Currently can't use dbal re-parsing in bare metal mode
         */
        SHR_EXIT();
    }
    /*
     * Get entry type using ARR
     */
    SHR_IF_ERR_EXIT_NO_MSG(dnx_etps_arr_decode_data_type_get(unit, sig->value, sig->size, entry_type));
    /*
     * Get table that matches the entry type
     */
    SHR_IF_ERR_EXIT_NO_MSG(dbal_mdb_table_id_and_result_type_from_etps_format_get(unit, entry_type[0], &table_id,
                                                                                  &dummy_res_type));
    /*
     * Parse entry
     */
    entry_info = sal_alloc(sizeof(dbal_printable_entry_t), "dbal_printable_entry");
    SHR_NULL_CHECK(entry_info, _SHR_E_MEMORY, "entry_info");
    sal_memset(entry_info, 0, sizeof(dbal_printable_entry_t));
    if (table_id != DBAL_NOF_TABLES)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dbal_table_printable_entry_get
                               (unit, table_id, NULL, core, sig->size, sig->value, entry_info));
        /*
         * Replace existing parsing with dbal parsing
         */
        sand_signal_list_free(sig->field_list);
        if (NULL == (sig->field_list = utilex_rhlist_create("dbal_fields", sizeof(signal_output_t), 0)))
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Could not allocate field list for dbal parsing");
        }
        for (fld = 0; fld < entry_info->nof_res_fields; ++fld)
        {
            if (!sal_strncmp(entry_info->res_fields_info[fld].field_name, "RESULT_TYPE", DBAL_MAX_LONG_STRING_LENGTH))
            {
                sal_strncpy_s(RHNAME(sig), entry_info->res_fields_info[fld].field_print_value, RHNAME_MAX_SIZE);
            }
            else
            {
                signal_output_t *field_sig = NULL;
                rhhandle_t rhhandle;
                SHR_IF_ERR_EXIT_NO_MSG(utilex_rhlist_entry_add_tail
                                       (sig->field_list, entry_info->res_fields_info[fld].field_name,
                                        RHID_TO_BE_GENERATED, &rhhandle));
                field_sig = rhhandle;
                sal_strncpy_s(field_sig->print_value, entry_info->res_fields_info[fld].field_print_value,
                              RHSTRING_MAX_SIZE);
                sal_memcpy(field_sig->value, entry_info->res_fields_info[fld].field_value,
                           sizeof(uint32) * DSIG_MAX_SIZE_UINT32);
            }
        }
        /*
         * Make sure the signal name is presented and not the structure name
         */
        sig->expansion[0] = '\0';
    }
exit:
    if (entry_info)
    {
        sal_free(entry_info);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Adds parsed ETPS stack to data
 */
static shr_error_e
ekleap_shared_data_etps_get(
    int unit,
    int core,
    ekleap_shared_data_t * data)
{
    int etps_i;
    uint32 valid;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    data->etps_data.sig_list = utilex_rhlist_create("etps_list", sizeof(signal_output_t), 0);
    SHR_NULL_CHECK(data->etps_data.sig_list, _SHR_E_MEMORY, "etps_list");
    /*
     * Get valid bitmap
     */
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "ETPP", "Term", "FWD", "ETPS_Valid", &valid, 4));
    for (etps_i = 0; (valid >> etps_i) > 0; ++etps_i)
    {
        signal_output_t *etps_sig = NULL;
        char signal_name[RHNAME_MAX_SIZE];
        /*
         * Get ETPS entry
         */
        sal_snprintf(signal_name, RHNAME_MAX_SIZE, "ETPS.%d", etps_i + 1);
        SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, "ETPP", "Term", "FWD", signal_name, &etps_sig));
        SHR_NULL_CHECK(etps_sig, _SHR_E_INTERNAL, "etps_sig");
        utilex_rhlist_entry_append(data->etps_data.sig_list, etps_sig);
        /*
         * Re-parse entry
         */
        rv = (ekleap_etps_sig_reparse_with_dbal(unit, core, etps_sig));
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI(("Could not parse ETPS entry '%s' with DBAL\n", RHNAME(etps_sig)));
        }
        data->etps_data.entry_signals[etps_i].entry_sig = etps_sig;

        data->etps_data.entry_signals[etps_i].data_sig = etps_sig;

        data->etps_data.entries_nof++;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get data required for all stages:
 *   - ETPS Stack
 */
shr_error_e
ekleap_shared_data_get(
    int unit,
    int core,
    ekleap_shared_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT_NO_MSG(ekleap_shared_data_etps_get(unit, core, data));
exit:
    SHR_FUNC_EXIT;
}

static void
ekleap_shared_data_free(
    ekleap_shared_data_t * data)
{
    if (data->etps_data.sig_list)
    {
        sand_signal_list_free(data->etps_data.sig_list);
        data->etps_data.sig_list = NULL;
    }
}

/**
 * \brief
 *   Print parser stage data:
 *    - PPMC access: key and value
 *    - Packet parsing
 */
static shr_error_e
ekleap_vis_stage_parser(
    int unit,
    int core,
    uint8 extended,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{
    shr_error_e rv;
    uint32 ppmc_key = 0;
    uint32 ppmc_match = 0;
    signal_output_t *sig_output = NULL;
    dbal_printable_entry_t *entry_info = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * PPMC
     * {
     */
    PRT_ROW_ADD(EKLEAP_PRT_STAGE_SEP);
    PRT_CELL_SET("Parser");
    PRT_CELL_SET("PPMC");

    rv = dpp_dsig_read(unit, core,
#ifdef ADAPTER_SERVER_MODE
                       "ETPP", NULL, "ETParser", "cud_outlif_or_mcdb_ptr",
#else
                       "ETPP", "ETParser", NULL, "FTMH_MCID_or_Out_LIF_0_or_MCDB_Ptr",
#endif
                       &ppmc_key, 4);
    if (rv != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Couldn't read PPMC Key\n");
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(1);
    PRT_CELL_SET_SHIFT(1, "Key");
    PRT_CELL_SET("%08x", ppmc_key);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(1);
    PRT_CELL_SET_SHIFT(1, "Result");
    rv = dpp_dsig_read(unit, core,
#ifdef ADAPTER_SERVER_MODE
                       "ERPP", "ERParser", NULL, "MCDB_Record_Match",
#else
                       "ETPP", "ETParser", NULL, "MCDB_Record_Match",
#endif
                       &ppmc_match, 1);
    if (rv != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Could not read PPMC match\n");
    }
    if (ppmc_match)
    {
        rv = sand_signal_output_find(unit, core, 0,
#ifdef ADAPTER_SERVER_MODE
                                     "ERPP", "ERParser", NULL, "MCDB_Record",
#else
                                     "ETPP", "ETParser", NULL, "MCDB_Record",
#endif
                                     &sig_output);
        if (rv != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Could not read PPMC result\n");
        }
        PRT_CELL_SET("%s", sig_output->print_value);
        entry_info = sal_alloc(sizeof(dbal_printable_entry_t), "dbal_printable_entry");
        SHR_NULL_CHECK(entry_info, _SHR_E_MEMORY, "entry_info");
        sal_memset(entry_info, 0, sizeof(dbal_printable_entry_t));
        SHR_CLI_EXIT_IF_ERR(dbal_table_printable_entry_get(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, NULL, core,
                                                           sig_output->size, sig_output->value, entry_info),
                            "Could not parse PPMC result using DBAL\n");
        ekleap_dbal_entry_info_prt(unit, FALSE, 1, 2, entry_info, prt_ctr);
    }
    else
    {
        PRT_CELL_SET("No Match");
    }
    /*
     * }
     */
    /*
     * Packet header
     * {
     */
    if (extended)
    {
        rv = sand_signal_output_find(unit, core, 0,
#ifdef ADAPTER_SERVER_MODE
                                     "ETPP", "Applet", NULL, "ingress_packet_header",
#else
                                     "ETPP", NULL, "ETParser", "Pkt_Header",
#endif
                                     &sig_output);
        if (rv == _SHR_E_NOT_FOUND)
        {
            ekleap_signal_not_found_prt(unit, TRUE, 1, "Pkt_Header", prt_ctr);
        }
        else
        {
            SHR_CLI_EXIT_IF_ERR(rv, "Something went wrong while trying to read Pkt_Header\n");
            ekleap_signal_prt(unit, sig_output, FALSE, 1, 0, prt_ctr);
        }
    }
    /*
     * }
     */
exit:
    sand_signal_output_free(sig_output);
    if (entry_info)
    {
        sal_free(entry_info);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief Add GLEM access information to the output
 */
static shr_error_e
glem_access_info_add(
    int unit,
    int core,
    prt_control_t * prt_ctr)
{
    uint32 lif_entry_handle_id, rif_entry_handle_id;
    int if_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_CELL_SET("GLEM");
    SHR_IF_ERR_EXIT_NO_MSG(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &lif_entry_handle_id));
    SHR_IF_ERR_EXIT_NO_MSG(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_RIF_EM, &rif_entry_handle_id));
    for (if_idx = 0; if_idx < EKLEAP_GLOBAL_OUT_LIF_MAX_NOF; ++if_idx)
    {
        shr_error_e rv;
        char global_out_lif_sig_name[RHNAME_MAX_SIZE];
        uint32 global_lif;
        uint32 local_lif;

        sal_snprintf(global_out_lif_sig_name, RHNAME_MAX_SIZE, "Global_Out_Lif_%d", if_idx);
        rv = dpp_dsig_read(unit, core, "ERPP", "PRP", NULL, global_out_lif_sig_name, &global_lif, 4);
        if (rv != _SHR_E_NONE)
        {
            break;
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET_SHIFT(1, "%s", global_out_lif_sig_name);
        PRT_CELL_SET("%06x", global_lif);

        /*
         * Set dbal result type according to lif range.
         */
        if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
        {
            dbal_entry_key_field32_set(unit, rif_entry_handle_id, DBAL_FIELD_GLOB_OUT_RIF, global_lif);
            dbal_entry_key_field32_set(unit, rif_entry_handle_id, DBAL_FIELD_CORE_ID, core);
            rv = dbal_entry_get(unit, rif_entry_handle_id, DBAL_GET_ALL_FIELDS);
            if (rv == _SHR_E_NONE)
            {
                local_lif = global_lif;
            }
            SHR_IF_ERR_EXIT_NO_MSG(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GLOBAL_RIF_EM, rif_entry_handle_id));

        }
        else
        {
            dbal_entry_key_field32_set(unit, lif_entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, global_lif);
            dbal_entry_key_field32_set(unit, lif_entry_handle_id, DBAL_FIELD_CORE_ID, core);
            rv = dbal_entry_get(unit, lif_entry_handle_id, DBAL_GET_ALL_FIELDS);
            if (rv == _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_handle_value_field32_get
                                       (unit, lif_entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, &local_lif));
            }
            SHR_IF_ERR_EXIT_NO_MSG(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GLOBAL_LIF_EM, lif_entry_handle_id));
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET_SHIFT(2, "Local_Out_LIF");
        if (rv == _SHR_E_NONE)
        {
            PRT_CELL_SET("%05x", local_lif);
        }
        else if (rv == _SHR_E_NOT_FOUND)
        {
            PRT_CELL_SET("No Match");
        }
        else
        {
            SHR_CLI_EXIT(rv, "Something went wrong :(\n");
        }

    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Add Fwd-Code information to the output
 */
static shr_error_e
fwd_code_info_add(
    int unit,
    int core,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{
    shr_error_e rv;
    signal_output_t *fwd_code_sig = NULL;

    SHR_FUNC_INIT_VARS(unit);

    rv = (sand_signal_output_find(unit, core, 0, "ETPP", "PRP2", NULL, "FWD_Code", &fwd_code_sig));
    if (rv == _SHR_E_NOT_FOUND)
    {
        ekleap_signal_not_found_prt(unit, TRUE, 1, "Fwd_Code", prt_ctr);
    }
    else
    {
        ekleap_signal_prt(unit, fwd_code_sig, FALSE, 1, 0, prt_ctr);
    }

    sand_signal_output_free(fwd_code_sig);
    SHR_FUNC_EXIT;
}



/**
 * \brief
 *   Prints PRP1 stage data:
 *    - GLEM access: Keys and values
 *    - Fwd Code
 */
static shr_error_e
ekleap_vis_stage_prp1(
    int unit,
    int core,
    uint8 extended,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{

    SHR_FUNC_INIT_VARS(unit);
    PRT_ROW_ADD(EKLEAP_PRT_STAGE_SEP);
    PRT_CELL_SET("PRP");
    /*
     * GLEM
     */
    SHR_IF_ERR_EXIT_NO_MSG(glem_access_info_add(unit, core, prt_ctr));
    /*
     * FWD_Code signal
     */
    SHR_IF_ERR_EXIT_NO_MSG(fwd_code_info_add(unit, core, shared_data, prt_ctr));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Get stage context ID and context name
 */
static shr_error_e
stage_ctx_info_get(
    int unit,
    int core,
    ekleap_stage_info_t * stage_info,
    uint32 *ctx_id,
    char *ctx_name)
{
    uint32 ctx_hw_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "ETPP", NULL, stage_info->stage_name, "Context", &ctx_hw_val, 4));
    SHR_IF_ERR_EXIT_NO_MSG(dbal_fields_enum_value_get(unit, stage_info->ctx_field_id, ctx_hw_val, ctx_id));
    if (ctx_name)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dbal_table_field_printable_string_get
                               (unit, stage_info->ctx_table_id, stage_info->ctx_field_id, ctx_id, NULL, 0, TRUE,
                                ctx_name));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ekleap_vis_stage_ctx_prt(
    int unit,
    int core,
    uint8 prt_context_details,
    ekleap_stage_info_t * stage_info,
    prt_control_t * prt_ctr)
{
    char ctx_str[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    uint32 entry_handle_id;
    uint32 ctx_val = 0;
    dbal_fields_e next_field_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Context ID
     * {
     */
    SHR_IF_ERR_EXIT_NO_MSG(stage_ctx_info_get(unit, core, stage_info, &ctx_val, ctx_str));
    PRT_ROW_ADD(EKLEAP_PRT_STAGE_SEP);
    PRT_CELL_SET("%s", stage_info->stage_name);
    PRT_CELL_SET("Context");
    PRT_CELL_SET("%s", ctx_str);
    /*
     * }
     * Context properties
     * {
     */
    SHR_IF_ERR_EXIT_NO_MSG(DBAL_HANDLE_ALLOC(unit, stage_info->ctx_table_id, &entry_handle_id));

    /** setting the key field */
    SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, stage_info->ctx_table_id, DBAL_FIELD_EMPTY, 1, 0, &next_field_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, next_field_id, ctx_val);

    SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, stage_info->ctx_table_id, DBAL_FIELD_EMPTY, 0, 0, &next_field_id));

    while (next_field_id != DBAL_FIELD_EMPTY)
    {
        dbal_field_types_defs_e field_type;
        uint32 field_val = 0;
        SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_handle_value_field32_get(unit, entry_handle_id, next_field_id,
                                                                   INST_SINGLE, &field_val));

        SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, next_field_id, &field_type));
        if (next_field_id == stage_info->etps_shift_field_id)
        {
            stage_info->etps_shift = field_val;
        }
        else if (prt_context_details)
        {
            if ((field_type == DBAL_FIELD_TYPE_DEF_BOOL) && (!field_val))
            {
                /*
                 * Disabled Boolean enabler field
                 */
                SHR_IF_ERR_EXIT(dbal_tables_next_field_get
                                (unit, stage_info->ctx_table_id, next_field_id, 0, 0, &next_field_id));
                continue;
            }
            else
            {
                /*
                 * Enabled boolean enabler, or non-boolean
                 */
                char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
                uint32 field_dbal_value = 0;
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
                PRT_CELL_SET_SHIFT(1, "%s", dbal_field_to_string(unit, next_field_id));
                SHR_IF_ERR_EXIT_NO_MSG(dbal_fields_enum_value_get
                                       (unit, stage_info->ctx_field_id, field_val, &field_dbal_value));
                SHR_IF_ERR_EXIT_NO_MSG(dbal_table_field_printable_string_get
                                       (unit, stage_info->ctx_table_id, next_field_id, &field_dbal_value, NULL,
                                        0, FALSE, buffer_to_print));
                PRT_CELL_SET("%s", buffer_to_print);
            }
        }

        SHR_IF_ERR_EXIT(dbal_tables_next_field_get
                        (unit, stage_info->ctx_table_id, next_field_id, 0, 0, &next_field_id));
    }

    if (stage_info->etps_shift)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("ETPS_Shift");
        PRT_CELL_SET("%d", stage_info->etps_shift);
    }
    /*
     * }
     */
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
ekleap_stage_info_get(
    int unit,
    ekleap_stage_e stage_id,
    ekleap_stage_info_t * stage_info)
{
    SHR_FUNC_INIT_VARS(unit);
    stage_info->stage_id = stage_id;
    stage_info->etps_shift_field_id = DBAL_FIELD_NOF_ENTRIES_TO_SHIFT;
    switch (stage_id)
    {
        case EKLEAP_STAGE_PRP2:
            stage_info->stage_name = "Prp2";
            stage_info->etps_shift_field_id = DBAL_FIELD_EMPTY;
            stage_info->ctx_field_id = DBAL_FIELD_EMPTY;
            stage_info->ctx_table_id = DBAL_TABLE_EMPTY;
            break;
        case EKLEAP_STAGE_TERM:
            stage_info->stage_name = "Term";
            stage_info->etps_shift_field_id = DBAL_FIELD_NOF_ETPS_TO_SHIFT;
            stage_info->ctx_field_id = DBAL_FIELD_TERM_CONTEXT_ID;
            stage_info->ctx_table_id = DBAL_TABLE_EGRESS_ETPP_TERM_CONTEXT_PROPERTIES;
            break;
        case EKLEAP_STAGE_FWD:
            stage_info->stage_name = "Fwd";
            stage_info->etps_shift_field_id = DBAL_FIELD_NOF_ETPS_TO_SHIFT;
            stage_info->ctx_field_id = DBAL_FIELD_FWD_CONTEXT_ID;
            stage_info->ctx_table_id = DBAL_TABLE_EGRESS_ETPP_FWD_CONTEXT_PROPERTIES;
            break;
        case EKLEAP_STAGE_ENC1:
            stage_info->stage_name = "Enc1";
            stage_info->ctx_field_id = DBAL_FIELD_ENCAP_1_CONTEXT_ID;
            stage_info->ctx_table_id = DBAL_TABLE_EGRESS_ETPP_ENCAP_1_CONTEXT_PROPERTIES;
            break;
        case EKLEAP_STAGE_ENC2:
            stage_info->stage_name = "Enc2";
            stage_info->ctx_field_id = DBAL_FIELD_ENCAP_2_CONTEXT_ID;
            stage_info->ctx_table_id = DBAL_TABLE_EGRESS_ETPP_ENCAP_2_CONTEXT_PROPERTIES;
            break;
        case EKLEAP_STAGE_ENC3:
            stage_info->stage_name = "Enc3";
            stage_info->ctx_field_id = DBAL_FIELD_ENCAP_3_CONTEXT_ID;
            stage_info->ctx_table_id = DBAL_TABLE_EGRESS_ETPP_ENCAP_3_CONTEXT_PROPERTIES;
            break;
        case EKLEAP_STAGE_ENC4:
            stage_info->stage_name = "Enc4";
            stage_info->ctx_field_id = DBAL_FIELD_ENCAP_4_CONTEXT_ID;
            stage_info->ctx_table_id = DBAL_TABLE_EGRESS_ETPP_ENCAP_4_CONTEXT_PROPERTIES;
            break;
        case EKLEAP_STAGE_ENC5:
            stage_info->stage_name = "Enc5";
            stage_info->ctx_field_id = DBAL_FIELD_ENCAP_5_CONTEXT_ID;
            stage_info->ctx_table_id = DBAL_TABLE_EGRESS_ETPP_ENCAP_5_CONTEXT_PROPERTIES;
            break;
        case EKLEAP_STAGE_TRAP:
            stage_info->stage_name = "Trap";
            stage_info->ctx_field_id = DBAL_FIELD_TRAP_CONTEXT_ID;
            stage_info->ctx_table_id = DBAL_TABLE_EGRESS_ETPP_TRAP_CONTEXT_PROPERTIES;
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Something went wrong");
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Parse ESEM key according to the App-DB using DBAL.
 */
static shr_error_e
ekleap_esem_key_reparse_with_dbal(
    int unit,
    int core,
    uint32 app_db_id,
    signal_output_t * key_sig)
{
    dbal_printable_entry_t *entry_info = NULL;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Rename the signal to "Key"
     */
    sal_strncpy(RHNAME(key_sig), "Key", RHNAME_MAX_SIZE);
    /*
     * Get table ID
     */
    SHR_IF_ERR_EXIT_NO_MSG(dbal_tables_table_by_mdb_phy_get(unit, DBAL_PHYSICAL_TABLE_ESEM, app_db_id, &table_id));
    /*
     * Parse Key
     */
    entry_info = sal_alloc(sizeof(dbal_printable_entry_t), "dbal_printable_entry");
    SHR_NULL_CHECK(entry_info, _SHR_E_MEMORY, "entry_info");
    sal_memset(entry_info, 0, sizeof(dbal_printable_entry_t));
    SHR_IF_ERR_EXIT_NO_MSG(dbal_table_printable_entry_get(unit, table_id, key_sig->value, core, 0, NULL, entry_info));
    /*
     * Update Key signal
     */
    if (entry_info->nof_key_fields > 0)
    {
        int key_i;
        sand_signal_list_free(key_sig->field_list);
        if (NULL == (key_sig->field_list = utilex_rhlist_create("esem_key_fields", sizeof(signal_output_t), 0)))
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Could not allocate memory to parse ESEM Key fields\n");
        }
        for (key_i = 0; key_i < entry_info->nof_key_fields; key_i++)
        {
            signal_output_t *field_sig = NULL;
            rhhandle_t rhhandle;
            SHR_IF_ERR_EXIT_NO_MSG(utilex_rhlist_entry_add_tail
                                   (key_sig->field_list, entry_info->key_fields_info[key_i].field_name,
                                    RHID_TO_BE_GENERATED, &rhhandle));
            field_sig = rhhandle;
            sal_strncpy_s(field_sig->print_value, entry_info->key_fields_info[key_i].field_print_value,
                          RHSTRING_MAX_SIZE);
        }
    }

exit:
    if (entry_info)
    {
        sal_free(entry_info);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief Remove VMV and shift the entry according to it.
 */
static shr_error_e
ekleap_esem_payload_vmv_peel(
    int unit,
    int core,
    signal_output_t * payload_sig)
{
    uint32 vmv_inst = 0;
    uint32 vmv_shift = 0;

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get 4 MSB from the signal
     */
    SHR_BITCOPY_RANGE(&vmv_inst, 0, payload_sig->value, payload_sig->size - 4, 4);

    /*
     * Get the vmv shift
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_VMV_SHIFT, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ESEM_1_SHIFT_VAL, vmv_inst, &vmv_shift);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * "Chop" the MSB by reducing the size of the signal
     */
    payload_sig->size -= vmv_shift;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
ekleap_esem_accesses_prt(
    int unit,
    int core,
    uint8 extended,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{
    int esem_acc;
    signal_output_t *signal_output = NULL;
    uint32 esem_app_db_null_value;
    SHR_FUNC_INIT_VARS(unit);


    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(1);
    PRT_CELL_SET_SHIFT(0, "ESEM_Accesses");

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_ESEM_APP_DB_ID, DBAL_ENUM_FVAL_ESEM_APP_DB_ID_NULL, &esem_app_db_null_value));
    for (esem_acc = 0; esem_acc < EKLEAP_TERM_ESEM_ACC_NOF; ++esem_acc)
    {
        shr_error_e rv;
        char sig_n[RHNAME_MAX_SIZE];

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET_SHIFT(1, "Interface_%d", esem_acc);
        /*
         * Check validity
         */
        sal_snprintf(sig_n, RHNAME_MAX_SIZE, "Valid_%d", esem_acc);
        rv = (sand_signal_output_find(unit, core, 0, "ETPP", "Term", "ESEM", sig_n, &signal_output));
        if (rv == _SHR_E_NOT_FOUND)
        {
            ekleap_signal_not_found_prt(unit, FALSE, 0, NULL, prt_ctr);
        }
        else
        {
            int app_db, valid;
            SHR_IF_ERR_EXIT_NO_MSG(rv);
            valid = signal_output->value[0];
            PRT_CELL_SET("%s", (valid ? "Valid" : "Disabled"));
            sal_snprintf(sig_n, RHNAME_MAX_SIZE, "Appdb_%d", esem_acc);
            SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find
                                   (unit, core, 0, "ETPP", "Term", "ESEM", sig_n, &signal_output));
            app_db = signal_output->value[0];

            if (valid && (app_db != esem_app_db_null_value))
            {
                /*
                 * Lookup is valid, print key and app-db ID
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
                PRT_CELL_SET_SHIFT(2, "App_DB");
                PRT_CELL_SET("%s", signal_output->print_value);
                app_db = signal_output->value[0];
                sal_snprintf(sig_n, RHNAME_MAX_SIZE, "Key_%d", esem_acc);
                SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find
                                       (unit, core, 0, "ETPP", "Term", "ESEM", sig_n, &signal_output));
                rv = ekleap_esem_key_reparse_with_dbal(unit, core, app_db, signal_output);
                if (rv != _SHR_E_NONE)
                {
                    LOG_CLI(("Could not parse ESEM Key_%d with DBAL\n", esem_acc));
                }
                ekleap_signal_prt(unit, signal_output, FALSE, 1, 2, prt_ctr);
                /*
                 * Check if lookup succeeded
                 */
                sal_snprintf(sig_n, RHNAME_MAX_SIZE, "Err_%d", esem_acc);
                SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find
                                       (unit, core, 0, "ETPP", "ESEM", "Term", sig_n, &signal_output));
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
                PRT_CELL_SET_SHIFT(2, "Payload");
                if (signal_output->value[0])
                {
                    /*
                     * Error occurred in ESEM lookup.
                     */
                    PRT_CELL_SET("Error");
                }
                else
                {
                    sal_snprintf(sig_n, RHNAME_MAX_SIZE, "Found_%d", esem_acc);
                    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find
                                           (unit, core, 0, "ETPP", "ESEM", "Term", sig_n, &signal_output));
                    if (!(signal_output->value[0]))
                    {
                        /*
                         * Entry not found
                         */
                        PRT_CELL_SET("No Match");
                    }
                    else
                    {
                        sal_snprintf(sig_n, RHNAME_MAX_SIZE, "Payload_%d", esem_acc);
                        rv = sand_signal_output_find(unit, core, 0, "ETPP", "ESEM", "Term", sig_n, &signal_output);
                        if (_SHR_E_NOT_FOUND == rv)
                        {
                            ekleap_signal_not_found_prt(unit, FALSE, 0, NULL, prt_ctr);
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT_NO_MSG(rv);
                            SHR_IF_ERR_EXIT_NO_MSG(ekleap_esem_payload_vmv_peel(unit, core, signal_output));
                            rv = ekleap_etps_sig_reparse_with_dbal(unit, core, signal_output);
                            if (rv != _SHR_E_NONE)
                            {
                                LOG_CLI(("Could not parse ETPS entry '%s' with DBAL\n", RHNAME(signal_output)));
                            }
                            ekleap_signal_prt(unit, signal_output, FALSE, 1, 3, prt_ctr);
                        }
                    }
                }
            }
        }
    }
exit:
    sand_signal_output_free(signal_output);
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *   Prints VSI DB access signal
 */
static shr_error_e
ekleap_vsi_db_access_prt(
    int unit,
    int core,
    uint8 extended,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{
    signal_output_t *vsd_sig = NULL;
    SHR_FUNC_INIT_VARS(unit);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(1);
    PRT_CELL_SET("VSD");


    /*
     * Read VSDS signal
     */
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, "ETPP", "Term", NULL, "VSDS", &vsd_sig));
    sal_snprintf(RHNAME(vsd_sig), RHNAME_MAX_SIZE, "Accesses");
    /*
     * Delete expansion of invalid stat commands
     */
    if (vsd_sig && vsd_sig->field_list)
    {
        signal_output_t *field = NULL;
        RHITERATOR(field, vsd_sig->field_list)
        {
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if (!sal_strcasecmp(field->expansion, "Stat_Cmd"))
            {
                signal_output_t *valid = utilex_rhlist_entry_get_by_name(field->field_list, "Valid");
                if (valid)
                {
                    if (!valid->value[0])
                    {
                        sand_signal_list_free(field->field_list);
                        field->field_list = NULL;
                    }
                }
            }
        }
    }
    /*
     * Print it
     */
    ekleap_signal_prt(unit, vsd_sig, FALSE, 1, 1, prt_ctr);
exit:
    sand_signal_output_free(vsd_sig);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Prints Term stage information:
 *   - Context
 *   - ESEM
 *   - VSI DB
 */
static shr_error_e
ekleap_vis_stage_term(
    int unit,
    int core,
    uint8 extended,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{
    ekleap_stage_info_t term_info = { 0 };
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Context
     * {
     */
    SHR_CLI_EXIT_IF_ERR(ekleap_stage_info_get(unit, EKLEAP_STAGE_TERM, &term_info),
                        "Could not read TERM information\n");
    SHR_CLI_EXIT_IF_ERR(ekleap_vis_stage_ctx_prt(unit, core, extended, &term_info, prt_ctr),
                        "Could not print TERM Context data\n");
    /*
     * }
     * ESEM
     * {
     */
    if (diag_pp_bare_metal_check(unit, NULL) == _SHR_E_NONE)
    {
        SHR_CLI_EXIT_IF_ERR(ekleap_esem_accesses_prt(unit, core, extended, shared_data, prt_ctr),
                            "Could not print ESEM accesses\n");
    }
    /*
     * }
     * VSI
     * {
     */
    SHR_CLI_EXIT_IF_ERR(ekleap_vsi_db_access_prt(unit, core, extended, shared_data, prt_ctr),
                        "Could not print VSI DB access\n");
    /*
     * }
     */
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Prints the ETPS top entries (1 or 2 according to the shift count)
 *   used by the stage.
 */
static shr_error_e
ekleap_etps_tos_prt(
    int unit,
    uint8 extended,
    ekleap_stage_info_t * stage_info,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{
    int entry_i;
    SHR_FUNC_INIT_VARS(unit);
    if (stage_info->etps_shift > 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("ETPS_Entr%s_Used", ((stage_info->etps_shift > 1) ? "ies" : "y"));
    }
    for (entry_i = 0; entry_i < stage_info->etps_shift; ++entry_i)
    {
        if (shared_data->shifted_etps >= shared_data->etps_data.entries_nof)
        {
            PRT_CELL_SET("_INVALID_ENTRY_USED_");
        }
        else
        {
            ekleap_signal_prt(unit, shared_data->etps_data.entry_signals[shared_data->shifted_etps].entry_sig,
                              FALSE, 1, 1, prt_ctr);
        }
        shared_data->shifted_etps++;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Prints Fwd, Enc1-5, Trap stage data:
 *   - Context
 *   - Context Properties
 *   - ETPS entry(ies) used
 */
static shr_error_e
ekleap_vis_stage_fwd_enc_trap(
    int unit,
    int core,
    uint8 extended,
    ekleap_stage_e stage,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{
    ekleap_stage_info_t stage_info = { 0 };
#ifdef ADAPTER_SERVER_MODE
    signal_output_t *gen_header_sig = NULL;
#endif
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Context
     * {
     */
    SHR_IF_ERR_EXIT_NO_MSG(ekleap_stage_info_get(unit, stage, &stage_info));
    SHR_IF_ERR_EXIT_NO_MSG(ekleap_vis_stage_ctx_prt(unit, core, extended, &stage_info, prt_ctr));
    /*
     * }
     * ETPS
     * {
     */
    SHR_IF_ERR_EXIT_NO_MSG(ekleap_etps_tos_prt(unit, extended, &stage_info, shared_data, prt_ctr));
    /*
     * }
     */
#ifdef ADAPTER_SERVER_MODE
    /*
     * Gen Header - adapter mode only
     */
    {
        shr_error_e rv =
            sand_signal_output_find(unit, core, 0, "ETPP", stage_info.stage_name, NULL, "gen_header", &gen_header_sig);
        if (rv == _SHR_E_NOT_FOUND)
        {
            ekleap_signal_not_found_prt(unit, TRUE, 1, "gen_header", prt_ctr);
        }
        else
        {
            SHR_CLI_EXIT_IF_ERR(rv, "Something went wrong when trying to read the generated header\n");
            ekleap_signal_prt(unit, gen_header_sig, FALSE, 1, 0, prt_ctr);
        }
    }
#endif
exit:
#ifdef ADAPTER_SERVER_MODE
    sand_signal_output_free(gen_header_sig);
#endif
    SHR_FUNC_EXIT;
}

static shr_error_e
btc_gen_header_description_get(
    int unit,
    int core,
    char **desc)
{
    uint32 ctx;
    ekleap_stage_info_t stage_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(ekleap_stage_info_get(unit, EKLEAP_STAGE_TRAP, &stage_info));
    SHR_IF_ERR_EXIT_NO_MSG(stage_ctx_info_get(unit, core, &stage_info, &ctx, NULL));
    switch (ctx)
    {
        case DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_DROP_AND_CONT:
        case DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_EXT_ENCAP:
        case DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_XTERMINATION:
        case DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_PTCH_ENC:
            *desc = "RCH";
            break;
        case DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RECYCLE_W_PTCH2:
            *desc = "PTCH";
            break;
        case DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___CPU_PORT_TRAPPED:
            *desc = "FTMH_Base";
            break;
        default:
            *desc = "ETH";
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
btc_gen_header_protocols_parse(
    int unit,
    int core,
    uint32 bytes_added,
    signal_output_t * header_sig)
{
    char *header_desc = NULL;
    SHR_FUNC_INIT_VARS(unit);

    sand_signal_list_free(header_sig->field_list);
    header_sig->field_list = NULL;
    SHR_IF_ERR_EXIT_NO_MSG(btc_gen_header_description_get(unit, core, &header_desc));
    SHR_IF_ERR_EXIT_NO_MSG(pparse_decode(unit, core, header_desc, header_sig->value, 8 * bytes_added,
                                         &header_sig->field_list, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief The signal is read with a bunch of zeroes prefixing it. Remove them.
 */
static void
btc_gen_header_data_format(
    int unit,
    uint32 bytes_added,
    signal_output_t * header_sig)
{
    char tmp_buf[DSIG_MAX_SIZE_STR];
    uint32 sig_nibles = sal_strnlen(header_sig->print_value, DSIG_MAX_SIZE_STR);
    uint32 nibbles_ta = 2 * bytes_added;

    if (nibbles_ta > sig_nibles)
    {
        nibbles_ta = sig_nibles;
    }

    /** Copy the last nibbles_ta characters to tmp_buf */
    sal_strncpy(tmp_buf, header_sig->print_value + (sig_nibles - nibbles_ta), DSIG_MAX_SIZE_STR);
    tmp_buf[DSIG_MAX_SIZE_STR - 1] = '\0';

    sal_snprintf(RHNAME(header_sig), RHNAME_MAX_SIZE, "Generated_Header");
    sal_strncpy(header_sig->print_value, tmp_buf, DSIG_MAX_SIZE_STR);
    header_sig->print_value[DSIG_MAX_SIZE_STR - 1] = '\0';
}

/**
 * \brief parse generated header signal to sub-headers and fields
 */
static shr_error_e
btc_gen_header_signal_parse(
    int unit,
    int core,
    uint32 bytes_added,
    signal_output_t * header_sig)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(btc_gen_header_protocols_parse(unit, core, bytes_added, header_sig));
    btc_gen_header_data_format(unit, bytes_added, header_sig);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Add parsed generated header data
 */
static shr_error_e
btc_gen_header_data_add(
    int unit,
    int core,
    uint32 bytes_added,
    prt_control_t * prt_ctr)
{
    signal_output_t *header_sig = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, "ETPP", "BTC", NULL, "Header_to_Add", &header_sig));
    SHR_IF_ERR_EXIT_NO_MSG(btc_gen_header_signal_parse(unit, core, bytes_added, header_sig));
    ekleap_signal_prt(unit, header_sig, 0, 1, 0, prt_ctr);

exit:
    sand_signal_output_free(header_sig);
    SHR_FUNC_EXIT;
}

static shr_error_e
btc_gen_header_empty_add(
    int unit,
    uint8 empty_or_too_long,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(1);
    PRT_CELL_SET("Generated_Header");
    if (empty_or_too_long == 0)
    {
        /** means Empty gen header */
        PRT_CELL_SET("-Empty-");
    }
    else
    {
        /** means gen header is too long to be parsed by the signal */
        PRT_CELL_SET("-Cannot parse, Gen header is larger than %dB -", EKLEAP_GEN_HEADER_MAX_VISIBLE_SIZE_BYTES);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Add generated header information section
 */
static shr_error_e
btc_generated_header_section_add(
    int unit,
    int core,
    uint32 bytes_added,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);

    if (bytes_added == 0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(btc_gen_header_empty_add(unit, 0, prt_ctr));
    }
    else if (bytes_added > EKLEAP_GEN_HEADER_MAX_VISIBLE_SIZE_BYTES)
    {
        SHR_IF_ERR_EXIT_NO_MSG(btc_gen_header_empty_add(unit, 1, prt_ctr));
    }
    else
    {
        SHR_IF_ERR_EXIT_NO_MSG(btc_gen_header_data_add(unit, core, bytes_added, prt_ctr));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Prints BTC stage data:
 *   - Bytes to strip
 *   - Bytes to add
 *   - Generated Header
 */
static shr_error_e
ekleap_vis_stage_btc(
    int unit,
    int core,
    uint8 extended,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{
    uint32 bta, bts;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Bytes to strip
     */
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "ETPP", "BTC", NULL, "Bytes_to_Strip", &bts, 4));
    PRT_ROW_ADD(EKLEAP_PRT_STAGE_SEP);
    PRT_CELL_SET("Btc");
    PRT_CELL_SET("Bytes_to_Strip");
    PRT_CELL_SET("%d", bts);
    /*
     * Bytes to add
     */
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "ETPP", "BTC", NULL, "Bytes_to_Add", &bta, 4));
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(1);
    PRT_CELL_SET("Bytes_to_Add");
    PRT_CELL_SET("%d", bta);
    /*
     * New Header
     */
    SHR_IF_ERR_EXIT_NO_MSG(btc_generated_header_section_add(unit, core, bta, prt_ctr));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Prints Out-Port data:
 *   - PP-Port
 *   - TM-Port
 */
static shr_error_e
ekleap_vis_stage_out_port(
    int unit,
    int core,
    uint8 extended,
    ekleap_shared_data_t * shared_data,
    prt_control_t * prt_ctr)
{
    uint32 out_pp_port, out_tm_port;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Out PP Port
     */
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "ETPP", "PRP2", NULL, "Out_PP_Port", &out_pp_port, 4));
    PRT_ROW_ADD(EKLEAP_PRT_STAGE_SEP);
    PRT_CELL_SET("Out-Port");
    PRT_CELL_SET("PP_Port");
    PRT_CELL_SET("%d (0x%x)", out_pp_port, out_pp_port);
    /*
     * Out TM Port
     */
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "ETPP", "PRP2", NULL, "Out_TM_Port", &out_tm_port, 4));
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(1);
    PRT_CELL_SET("TM_Port");
    PRT_CELL_SET("%d (0x%x)", out_tm_port, out_tm_port);

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
/*
 * Main commands
 * {
 */
/**
 * \brief
 *   Prints eKLEAP information from signals and some processing
 */
shr_error_e
sh_dnx_ekleap_vis_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core, core_arg = 0;
    ekleap_shared_data_t shared_data;
    /*
     * ekleap_stage_e selected_stage;
     */
    uint8 extended;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Clean shared_data in case some error send to 'exit' and ekleap_shared_data_free will be called
     */
    sal_memset(&shared_data, 0, sizeof(ekleap_shared_data_t));
    /*
     * Get arguments
     */
    /*
     * SH_SAND_GET_ENUM(EKLEAP_ARG_KW_STAGE, selected_stage);
     */
    SH_SAND_GET_BOOL(EKLEAP_ARG_KW_EXTENDED, extended);
    SH_SAND_GET_INT32("Core", core_arg);
#ifdef ADAPTER_SERVER_MODE
    /*
     * No CORE-1 in adapter. The world will end
     * if core 1 signals are read.
     */
    core_arg = 0;
#endif
    SH_DNX_CORES_ITER(unit, core_arg, core)
    {
        int stage_col, property_col;
        PRT_TITLE_SET("eKLEAP information");
        PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &stage_col, "Stage");
        PRT_COLUMN_ADDX(PRT_XML_CHILD, stage_col, &property_col, "Property");
        PRT_COLUMN_ADDX_FLEX(PRT_FLEX_BINARY, PRT_XML_ATTRIBUTE, property_col, NULL, "Value");
        PRT_INFO_ADD("Core %d", core);

        if (dnx_debug_block_is_ready(unit, core, DNX_PP_BLOCK_ETPP) != _SHR_E_NONE)
        {
            PRT_INFO_ADD("No visible traffic");
        }
        else
        {
            ekleap_stage_e cur_stage;
            ekleap_stage_e last_stage;
            /*
             * Get global data
             */
            sal_memset(&shared_data, 0, sizeof(ekleap_shared_data_t));
            SHR_CLI_EXIT_IF_ERR(ekleap_shared_data_get(unit, core, &shared_data), "Could not read Encap DB entries\n");
            /*
             * Invocation without Stage or Stage=All argument.
             * All stages should be printed to a single table.
             * It's possible to print specific stage by altering
             * these variables in the future.
             */
            cur_stage = EKLEAP_STAGE_FIRST;
            last_stage = EKLEAP_STAGE_NOF - 1;
            for (; cur_stage <= last_stage; ++cur_stage)
            {
                switch (cur_stage)
                {
                    case EKLEAP_STAGE_PARSER:
                        if (_SHR_E_NONE != ekleap_vis_stage_parser(unit, core, extended, &shared_data, prt_ctr))
                        {
                            LOG_CLI(("Core: %d, Error printing PARSER info\n", core));
                        }
                        break;
                    case EKLEAP_STAGE_PRP1:
                        if (_SHR_E_NONE != ekleap_vis_stage_prp1(unit, core, extended, &shared_data, prt_ctr))
                        {
                            LOG_CLI(("Core: %d, Error printing PRP1 info\n", core));
                        }
                        break;
                    case EKLEAP_STAGE_PRP2:
                        break;
                    case EKLEAP_STAGE_TERM:
                        if (_SHR_E_NONE != ekleap_vis_stage_term(unit, core, extended, &shared_data, prt_ctr))
                        {
                            LOG_CLI(("Core: %d, Error printing TERM info\n", core));
                        }
                        break;
                    case EKLEAP_STAGE_FWD:
                    case EKLEAP_STAGE_ENC1:
                    case EKLEAP_STAGE_ENC2:
                    case EKLEAP_STAGE_ENC3:
                    case EKLEAP_STAGE_ENC4:
                    case EKLEAP_STAGE_ENC5:
                    case EKLEAP_STAGE_TRAP:
                        if (_SHR_E_NONE !=
                            ekleap_vis_stage_fwd_enc_trap(unit, core, extended, cur_stage, &shared_data, prt_ctr))
                        {
                            if (cur_stage == EKLEAP_STAGE_FWD || cur_stage == EKLEAP_STAGE_TRAP)
                            {
                                LOG_CLI(("Core: %d, Error printing %s info\n", core,
                                         (cur_stage == EKLEAP_STAGE_FWD ? "FWD" : "TRAP")));
                            }
                            else
                            {
                                LOG_CLI(("Core: %d, Error printing ENC%d info\n", core,
                                         1 + (cur_stage - EKLEAP_STAGE_ENC1)));
                            }
                        }
                        break;
                    case EKLEAP_STAGE_BTC:
                        if (_SHR_E_NONE != ekleap_vis_stage_btc(unit, core, extended, &shared_data, prt_ctr))
                        {
                            LOG_CLI(("Core: %d, Error printing BTC info\n", core));
                        }
                        break;
                    case EKLEAP_STAGE_OUT_PORT:
                        if (_SHR_E_NONE != ekleap_vis_stage_out_port(unit, core, extended, &shared_data, prt_ctr))
                        {
                            LOG_CLI(("Core: %d, Error printing OUT_PORT info\n", core));
                        }
                        break;
                        /*
                         * Coverity explanation: Added defensive default case that throws an error,
                         * in case the logic of assignment to cur_stage and last_stage ever change.
                         */
                         /* coverity[dead_error_begin:FALSE]  */
                    default:
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong :(\n");
                }
            }
            ekleap_shared_data_free(&shared_data);
        }
        PRT_COMMITX;
    }

exit:
    ekleap_shared_data_free(&shared_data);
    PRT_FREE;
    if (SHR_FUNC_ERR())
    {
        LOG_CLI((BSL_META_U(unit, "Error in command pp vis ekleap\n")));
    }
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/***************************************************************************************
 *      Stuff for shell framework
 */
/**
 * \brief
 *   Manual for eKLeaP VISibility command
 */
sh_sand_man_t dnx_ekleap_vis_man = {
    .brief = "Show eKLEAP visibility information.",
    .full = "prints briefed information on all ETPP processing stages. To get full information on all stages,"
        " add the '" EKLEAP_ARG_KW_EXTENDED "' argument",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "full\n",
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};
/**
 * \brief
 *   Option set for eKLeaP VISibility command
 */
sh_sand_option_t dnx_ekleap_vis_options[] = {
    {
     .keyword = EKLEAP_ARG_KW_EXTENDED,
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Show extended data for all stages",
     .def = "No",
     }
    ,
    {
     NULL}
};
