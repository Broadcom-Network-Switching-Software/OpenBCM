/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_pp_last.c
 *
 * Last PP Diagnostics File
 * Shows IRPP information on last packet that passed in pipe
 */

/*************
 * INCLUDES  *
 */
#include "diag_dnx_pp.h"
#include <bcm_int/dnx/instru/instru_visibility.h>

/*************
 * DEFINES   *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

#define PP_LAST_MAX_NOF_INSTRU_SIGNALS         5

/** max number of lookup terms in all fwd layers */
#define PP_LAST_MAX_NOF_LAYER_LOOKUP_TERMS     4

/*************
 *  MACROS  *
 */

/** reads the FWD layer lookup terms signal, and also extra lookup terms (PP_LAST_FODO, Global LIF) and coppies to a structure */
#define __PP_LAST_GET_FWD_LAYER_LOOKUP_TERMS(__fwd_layer)                                                                                                                           \
    current_fwd_layer_lookup_terms.nof_lookup_terms_signals = pp_last_fwd_layers_lookup_terms[__fwd_layer].nof_lookup_terms_signals;                                                \
    current_fwd_layer_lookup_terms.nof_lookup_terms_extra = pp_last_fwd_layers_lookup_terms[__fwd_layer].nof_lookup_terms_extra;                                                    \
                                                                                                                                                                            \
    for (i_sig_fwd_lookup_term = 0; i_sig_fwd_lookup_term < pp_last_fwd_layers_lookup_terms[__fwd_layer].nof_lookup_terms_signals; i_sig_fwd_lookup_term++)                         \
    {                                                                                                                                                                       \
        field_header_sig = utilex_rhlist_entry_get_by_name(pkt_header_sig->field_list, pp_last_fwd_layers_lookup_terms[__fwd_layer].lookup_term_signals[i_sig_fwd_lookup_term]);    \
                                                                                                                                                                            \
        if (field_header_sig)                                                                                                                                               \
        {                                                                                                                                                                   \
            sal_strncpy_s(current_fwd_layer_lookup_terms.lookup_term_signals[i_sig_fwd_lookup_term], field_header_sig->print_value, RHSTRING_MAX_SIZE-1);                   \
        }                                                                                                                                                                   \
        else                                                                                                                                                                \
        {                                                                                                                                                                   \
            sal_strncpy_s(current_fwd_layer_lookup_terms.lookup_term_signals[i_sig_fwd_lookup_term], "Not Found", RHSTRING_MAX_SIZE-1);                                     \
        }                                                                                                                                                                   \
    }                                                                                                                                                                       \
                                                                                                                                                                            \
    for (i_extra_fwd_lookup_term = 0; i_extra_fwd_lookup_term < pp_last_fwd_layers_lookup_terms[__fwd_layer].nof_lookup_terms_extra; i_extra_fwd_lookup_term++)                     \
    {                                                                                                                                                                       \
        if (pp_last_fwd_layers_lookup_terms[__fwd_layer].extra_lookup_term[i_extra_fwd_lookup_term] == PP_LAST_FODO)                                                                        \
        {                                                                                                                                                                   \
            sal_strncpy_s(current_fwd_layer_lookup_terms.extra_lookup_term_str[i_extra_fwd_lookup_term], fodo_str, RHSTRING_MAX_SIZE-1);                                    \
        }                                                                                                                                                                   \
        else if (pp_last_fwd_layers_lookup_terms[__fwd_layer].extra_lookup_term[i_extra_fwd_lookup_term] == PP_LAST_GLOBAL_LIF)                                                             \
        {                                                                                                                                                                   \
            sal_strncpy_s(current_fwd_layer_lookup_terms.extra_lookup_term_str[i_extra_fwd_lookup_term], "Global LIF = ", RHSTRING_MAX_SIZE-1);                             \
            sal_strncat(current_fwd_layer_lookup_terms.extra_lookup_term_str[i_extra_fwd_lookup_term], global_lif_str, RHSTRING_MAX_SIZE-1);                                \
        }                                                                                                                                                                   \
    }

/*************
 * TYPEDEFS  *
 */

/** \brief List of Table types per FEC hierarchy */
typedef enum
{
    PP_LAST_PACKET_STRUCTURE_TABLE_ID,
    PP_LAST_PORT_TERMINATION_TABLE_ID,
    PP_LAST_VTT_SUMMARY_TABLE_ID,
    PP_LAST_FWD_LOOKUP_TABLE_ID,
    PP_LAST_FEC_TABLE_ID,
    PP_LAST_TRAP_RESOLUTION_TABLE_ID,
    PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID,
    PP_LAST_NOF_TABLES
} pp_last_table_ids_t;

/** \brief Supported Fwd Layers */
typedef enum
{
    PP_LAST_ETH_FWD_LAYER,
    PP_LAST_IPV4_FWD_LAYER,
    PP_LAST_IPV6_FWD_LAYER,
    PP_LAST_MPLS_FWD_LAYER,
    PP_LAST_NOF_FWD_LAYERS
} pp_last_fwd_layers_t;

/** \brief additional lookup term for fwd layer - PP_LAST_FODO/Global LIF */
typedef enum
{
    PP_LAST_FODO,
    PP_LAST_GLOBAL_LIF
} pp_last_fwd_layer_lookup_extra_term_t;

/* \brief Strucuture that contains per each fwd layer
 *        - array of signal names of the terms (from the fwd_layer signal), and number of signal terms
 *        - extra lookup term - usually PP_LAST_FODO/Global LIF
 */
typedef struct pp_last_layer_lookup_terms_s
{
    char *lookup_term_signals[PP_LAST_MAX_NOF_LAYER_LOOKUP_TERMS];
    int nof_lookup_terms_signals;
    pp_last_fwd_layer_lookup_extra_term_t extra_lookup_term[PP_LAST_MAX_NOF_LAYER_LOOKUP_TERMS];
    int nof_lookup_terms_extra;
} pp_last_layer_lookup_terms_t;

/** \brief - current fwd layer lookup terms, given all in strings ready for print */
typedef struct pp_last_current_layer_lookup_terms_s
{
    char lookup_term_signals[PP_LAST_MAX_NOF_LAYER_LOOKUP_TERMS][RHSTRING_MAX_SIZE];
    int nof_lookup_terms_signals;
    char extra_lookup_term_str[PP_LAST_MAX_NOF_LAYER_LOOKUP_TERMS][RHSTRING_MAX_SIZE];
    int nof_lookup_terms_extra;
} pp_last_current_layer_lookup_terms_t;

/** Init lookup terms for the forward layer */
pp_last_layer_lookup_terms_t pp_last_fwd_layers_lookup_terms[PP_LAST_NOF_FWD_LAYERS] = {
    /** ETH FWD LAYER lookup terms - PP_LAST_FODO (VSI), SA, DA */
    {
     {"SA", "DA"},
     2,
     {PP_LAST_FODO},
     1},
    /** IPv4 FWD LAYER lookup terms - PP_LAST_FODO (VRF), Global LIF, SIP, DIP */
    {
     {"SIP", "DIP"},
     2,
     {PP_LAST_FODO, PP_LAST_GLOBAL_LIF},
     2},
    /** IPv6 FWD LAYER lookup terms - PP_LAST_FODO (VRF), Global LIF, SIP, DIP */
    {
     {"SIP", "DIP"},
     2,
     {PP_LAST_FODO, PP_LAST_GLOBAL_LIF},
     2},
    /** MPLS FWD LAYER lookup terms - Global LIF, MPLS Label */
    {
     {"Label"},
     1,
     {PP_LAST_GLOBAL_LIF},
     1}
};
/*************
 * GLOBALS   *
 */

/** set all tables to unoccupied - if a table is filled its indication is set */
uint8 pp_last_occupied_tables[PP_LAST_NOF_TABLES] = { 0 };

/*************
 * FUNCTIONS *
 */

/**
 * \brief - Function prints the header and columns of the various tables
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core
 *   \param [in] prt_ctr
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
dnx_pp_last_define_tables(
    int unit,
    prt_control_t * prt_ctr[],
    int core)
{

    SHR_FUNC_INIT_VARS(unit);
    PRT_TITLE_SET_PL(PP_LAST_PACKET_STRUCTURE_TABLE_ID, "<Packet Structure Core=%d>", core);

    PRT_TITLE_SET_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, "<Port Termination>");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, PRT_FLEX_ASCII, "PP Port");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, PRT_FLEX_ASCII, "TM Port");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, PRT_FLEX_ASCII, "Sys Port");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, PRT_FLEX_ASCII, "PRT Qualifier");
    PRT_ROW_ADD_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, PRT_ROW_SEP_NONE);

    PRT_TITLE_SET_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, "<VTT Summary>");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, PRT_FLEX_ASCII, "Global LIF");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, PRT_FLEX_ASCII, "FODO");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, PRT_FLEX_ASCII, "FWD Layer");
    PRT_ROW_ADD_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, PRT_ROW_SEP_NONE);

    PRT_TITLE_SET_PL(PP_LAST_FWD_LOOKUP_TABLE_ID, "<FWD Lookup>");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_FWD_LOOKUP_TABLE_ID, PRT_FLEX_ASCII, "Key Components");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_FWD_LOOKUP_TABLE_ID, PRT_FLEX_ASCII, "Result Destination");
    PRT_ROW_ADD_PL(PP_LAST_FWD_LOOKUP_TABLE_ID, PRT_ROW_SEP_NONE);

    PRT_TITLE_SET_PL(PP_LAST_FEC_TABLE_ID, "<FEC>");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_FEC_TABLE_ID, PRT_FLEX_ASCII, "Destination");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_FEC_TABLE_ID, PRT_FLEX_ASCII, "Outlifs");
    PRT_ROW_ADD_PL(PP_LAST_FEC_TABLE_ID, PRT_ROW_SEP_NONE);

    PRT_TITLE_SET_PL(PP_LAST_TRAP_RESOLUTION_TABLE_ID, "<Trap Resolution>");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_TRAP_RESOLUTION_TABLE_ID, PRT_FLEX_ASCII, "Trap Name");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_TRAP_RESOLUTION_TABLE_ID, PRT_FLEX_ASCII, "Strength");
    PRT_ROW_ADD_PL(PP_LAST_TRAP_RESOLUTION_TABLE_ID, PRT_ROW_SEP_NONE);

    PRT_TITLE_SET_PL(PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID, "<TM Command Resolution>");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID, PRT_FLEX_ASCII, "Destination");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID, PRT_FLEX_ASCII, "Tc");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID, PRT_FLEX_ASCII, "Dp");
    PRT_ROW_ADD_PL(PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID, PRT_ROW_SEP_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function fills the tables with relevant PP information and updates the occupied tables bitmap
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] prt_ctr - multiple tables pointer
 *   \param [in] core_id - core
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
dnx_pp_last_fill_tables(
    int unit,
    prt_control_t * prt_ctr[],
    int core_id)
{
    /** signal element which contains the value of signal and Entry struct */
    signal_output_t *sig_list_element = NULL;
    signal_output_t *protocols_sig = NULL;
    signal_output_t *pkt_header_sig = NULL;
    signal_output_t *layer_offsets_sig = NULL;
    char packet_structure_str[DSIG_MAX_SIZE_STR] = "";

    bcm_instru_vis_signal_key_t signal_key[PP_LAST_MAX_NOF_INSTRU_SIGNALS];
    bcm_instru_vis_signal_result_t signal_result[PP_LAST_MAX_NOF_INSTRU_SIGNALS];
    int i_sig, nof_signals, i_sig_fwd_lookup_term, i_extra_fwd_lookup_term;
    int fwd_layer_index = 0;
    char val_string[RHSTRING_MAX_SIZE] = "Not Found";
    char fwd_layer_string[RHSTRING_MAX_SIZE] = "";

    int pparse_layer_offset = 0;

    /** global lif and PP_LAST_FODO used as lookup up terms of fwd layer in FWD stage print */
    char global_lif_str[RHSTRING_MAX_SIZE] = "Global LIF N/A";
    char fodo_str[RHSTRING_MAX_SIZE] = "FODO N/A";

    /** stores the current fwd_layer */
    pp_last_fwd_layers_t fwd_layer_type = PP_LAST_NOF_FWD_LAYERS;

    /** Init lookup terms for the current forward layer in case FWD layer key term parsing not supported */
    pp_last_current_layer_lookup_terms_t current_fwd_layer_lookup_terms = {
        {"N/A"},
        0,
        {"N/A"},
        0
    };

    SHR_FUNC_INIT_VARS(unit);

    /**************************************************************************************************
     * fill Packet Structure table                                                                    *
     * - get the layer and its parsing                                                                *
 */
    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "NIF", "", "header", &sig_list_element),
                        "no signal record for header from NIF\n");

    /** get the overall packet structure and return the packet structure string + update layer linked list for 'next layer' info */
    SHR_IF_ERR_EXIT(construct_packet_structure(unit, sig_list_element->field_list, packet_structure_str, NULL, NULL));

    PRT_INFO_ADD_PL(PP_LAST_PACKET_STRUCTURE_TABLE_ID, "%s", packet_structure_str);
    pp_last_occupied_tables[PP_LAST_PACKET_STRUCTURE_TABLE_ID] = TRUE;

    /*******************************
     * fill Port Termination table *
 */
    i_sig = 0;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "LBP", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "TM_Cmd.in_pp_port", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "VTT5", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Src_Sys_Port_ID", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "VTT5", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "PTC", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "LLR", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Prt_Qualifier", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    nof_signals = i_sig + 1;
    SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get(unit, core_id, 0, nof_signals, signal_key, signal_result));

    for (i_sig = 0; i_sig < nof_signals; i_sig++)
    {
        if (signal_result[i_sig].status == BCM_E_NONE)
        {
            PRT_CELL_SET_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, "0x%x", signal_result[i_sig].value[0]);
        }
        else
        {
            PRT_CELL_SET_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, "Not Found");
        }
    }

    pp_last_occupied_tables[PP_LAST_PORT_TERMINATION_TABLE_ID] = TRUE;

    /**************************
     * fill VTT Summary table *
 */

    i_sig = 0;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "VTT5", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "In_LIFs.0", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "VTT5", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Fwd_Domain_ID.type", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "VTT5", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Fwd_Domain_ID.value", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "VTT5", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Fwd_Layer_Index", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    nof_signals = i_sig + 1;
    SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get(unit, core_id, 0, nof_signals, signal_key, signal_result));

    /** iterate up to FWD_Layer_Index */
    for (i_sig = 0; i_sig < nof_signals; i_sig++)
    {
        uint8 skip_print = 0;
        sal_strncpy_s(val_string, "Not Found", RHSTRING_MAX_SIZE - 1);

        if (signal_result[i_sig].status == BCM_E_NONE)
        {
            /** Global LIF*/
            if (i_sig == 0)
            {
                sal_strncpy_s(val_string, "0x", 3);
                sal_strncat(val_string, signal_result[i_sig].print_value, RHSTRING_MAX_SIZE - 1);

                /** store the Global LIF for FWD stage lookup terms  */
                sal_strncpy_s(global_lif_str, val_string, RHSTRING_MAX_SIZE - 1);
            }
            /** for PP_LAST_FODO construct a string of PP_LAST_FODO Type + Value, and store for FWD stage lookup terms */
            else if (i_sig == 1)
            {
                sal_strncpy_s(val_string, signal_result[1].print_value, RHSTRING_MAX_SIZE - 1);
                sal_strncat(val_string, " = 0x", RHSTRING_MAX_SIZE - 1);
                sal_strncat(val_string, signal_result[2].status == BCM_E_NONE ? signal_result[2].print_value : "N/A",
                            RHSTRING_MAX_SIZE - 1);

                /** store PP_LAST_FODO for FWD stage lookup terms */
                sal_strncpy_s(fodo_str, val_string, RHSTRING_MAX_SIZE - 1);

                /** skip 1,2 PP_LAST_FODO (type, value) signals */
                i_sig++;
            }
            else if (i_sig == 3)  /** get the Fwd_Layer_Index but skip print */
            {
                fwd_layer_index = signal_result[3].value[0];
                skip_print = 1;
            }
        }

        if (!skip_print)
        {
            PRT_CELL_SET_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, "%s", val_string);
        }
    }

    /*
     * get the FWD layer name + value and print, by iterating it up to FWD_Layer_Index value
     */

    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "VTT5", "", "layer_protocols",
                                                &protocols_sig), "no signal record for layer_protocols\n");

    /** get also the pkt_header for the FWD layer FWD lookup terms - with various options in its signal appearance */
    if (sand_signal_output_find(unit, core_id, 0, "IRPP", "NIF", "", "header", &pkt_header_sig) != _SHR_E_NONE)
    {
        if (sand_signal_output_find(unit, core_id, 0, "IRPP", "", "", "header", &pkt_header_sig) != _SHR_E_NONE)
        {
            /** for special cases where instead of from "NIF" and "InNetworkAdaptor", used another block name as "PRT" */
            SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "PRT", "", "Pkt_Header",
                                                        &pkt_header_sig), "no signal record for header\n");
        }
    }

    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "VTT5", "", "layer_offsets",
                                                &layer_offsets_sig), "no signal record for layer_offsets\n");

    /** set initial value of fwd_layer to the name of the 1st layer */
    i_sig = 0;
    sal_strncpy_s(val_string, "idx(", RHSTRING_MAX_SIZE - 1);
    sal_strncat(val_string, signal_result[3].print_value, RHSTRING_MAX_SIZE - 1);
    sal_strncat(val_string, ") - ", RHSTRING_MAX_SIZE - 1);
    sal_strncpy_s(fwd_layer_string, protocols_sig->print_value, RHSTRING_MAX_SIZE - 1);

    pkt_header_sig = utilex_rhlist_entry_get_first(pkt_header_sig->field_list);
    layer_offsets_sig = utilex_rhlist_entry_get_first(layer_offsets_sig->field_list);
    RHITERATOR(protocols_sig, protocols_sig->field_list)
    {

        /** copy the name of the wanted fwd_layer_index + get its FWD search terms */
        if (i_sig == fwd_layer_index)
        {

            signal_output_t *field_header_sig = NULL;
            /** FWD layer's offset for verification with pparse pkt_header's structure offsets */
            int parser_fwd_layer_offset = layer_offsets_sig->value[0];

            /** Run through the pparse packet_hdr stucture to match by offset to that of the layer_offsets of the parser of current FWD layer */
            while (pkt_header_sig && (pparse_layer_offset < parser_fwd_layer_offset))
            {
                /** pparse offset of current layer */
                pparse_layer_offset += pkt_header_sig->size / 8;
                pkt_header_sig = utilex_rhlist_entry_get_next(pkt_header_sig);
            }

            /** verify that we managed to match the FWD layer in pparser pkt_header to packet structure in parser */
            if (pparse_layer_offset != parser_fwd_layer_offset)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "could not match FWD layer offset in pkt_header (%d) to that of parset layer_offsets(%d) !\n",
                             pparse_layer_offset, parser_fwd_layer_offset);
            }

            sal_strncpy_s(fwd_layer_string, protocols_sig->print_value, RHSTRING_MAX_SIZE - 1);

            /*
             * read the relevant FWD lookup terms from signals - by the type of the FWD layer (ETH/IPv4/IPv6/MPLS)
             */
            if (sal_strstr(fwd_layer_string, "ETH"))
            {
                __PP_LAST_GET_FWD_LAYER_LOOKUP_TERMS(PP_LAST_ETH_FWD_LAYER);
                fwd_layer_type = PP_LAST_ETH_FWD_LAYER;
            }
            else if (sal_strstr(fwd_layer_string, "IPV4"))
            {
                __PP_LAST_GET_FWD_LAYER_LOOKUP_TERMS(PP_LAST_IPV4_FWD_LAYER);
                fwd_layer_type = PP_LAST_IPV4_FWD_LAYER;
            }
            else if (sal_strstr(fwd_layer_string, "IPV6"))
            {
                __PP_LAST_GET_FWD_LAYER_LOOKUP_TERMS(PP_LAST_IPV6_FWD_LAYER);
                fwd_layer_type = PP_LAST_IPV6_FWD_LAYER;
            }
            else if (sal_strstr(fwd_layer_string, "MPLS_UNTERM"))
            {
                __PP_LAST_GET_FWD_LAYER_LOOKUP_TERMS(PP_LAST_MPLS_FWD_LAYER);
                fwd_layer_type = PP_LAST_MPLS_FWD_LAYER;
            }

            break;
        }

        i_sig++;
        layer_offsets_sig = utilex_rhlist_entry_get_next(layer_offsets_sig);
    }

    sal_strncat(val_string, fwd_layer_string, RHSTRING_MAX_SIZE - 1);
    PRT_CELL_SET_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, "%s", val_string);
    pp_last_occupied_tables[PP_LAST_VTT_SUMMARY_TABLE_ID] = TRUE;

    /*************************
     * fill FWD Lookup table *
 */

    /*
     * print the FWD Layer lookup terms
     */

    val_string[0] = '\0';
    for (i_sig_fwd_lookup_term = 0; i_sig_fwd_lookup_term < current_fwd_layer_lookup_terms.nof_lookup_terms_signals;
         i_sig_fwd_lookup_term++)
    {
        sal_strncat(val_string,
                    pp_last_fwd_layers_lookup_terms[fwd_layer_type].lookup_term_signals[i_sig_fwd_lookup_term],
                    RHSTRING_MAX_SIZE - 1);
        sal_strncat(val_string, " = ", RHSTRING_MAX_SIZE - 1);
        sal_strncat(val_string, current_fwd_layer_lookup_terms.lookup_term_signals[i_sig_fwd_lookup_term],
                    RHSTRING_MAX_SIZE - 1);
        sal_strncat(val_string, "\n", RHSTRING_MAX_SIZE - 1);
    }

    for (i_extra_fwd_lookup_term = 0; i_extra_fwd_lookup_term < current_fwd_layer_lookup_terms.nof_lookup_terms_extra;
         i_extra_fwd_lookup_term++)
    {
        sal_strncat(val_string, current_fwd_layer_lookup_terms.extra_lookup_term_str[i_extra_fwd_lookup_term],
                    RHSTRING_MAX_SIZE - 1);
        if (i_extra_fwd_lookup_term < current_fwd_layer_lookup_terms.nof_lookup_terms_extra - 1)
        {
            sal_strncat(val_string, "\n", RHSTRING_MAX_SIZE - 1);
        }
    }

    PRT_CELL_SET_PL(PP_LAST_FWD_LOOKUP_TABLE_ID, "%s", val_string);

    /*
     * print the FWD destination
     */

    i_sig = 0;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "FWD2", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "fwd_action_dst.value", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "FWD2", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "fwd_action_dst.type", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    nof_signals = i_sig + 1;
    SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get(unit, core_id, 0, nof_signals, signal_key, signal_result));

    if (signal_result[0].status == BCM_E_NONE)
    {
        PRT_CELL_SET_PL(PP_LAST_FWD_LOOKUP_TABLE_ID, "0x%x (%s)", signal_result[0].value[0],
                        signal_result[1].status == BCM_E_NONE ? signal_result[1].print_value : "N/A");
    }
    else
    {
        PRT_CELL_SET_PL(PP_LAST_FWD_LOOKUP_TABLE_ID, "Not Found");
    }

    pp_last_occupied_tables[PP_LAST_FWD_LOOKUP_TABLE_ID] = TRUE;

    /******************
     * fill FEC table *
 */

    i_sig = 0;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "IPMF1", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Fwd_Action_Dst.type", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "FER", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Fwd_Action_Dst.value", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "FER", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Fwd_Action_Dst.type", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "FER", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "all_out_lifs.0", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "FER", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "all_out_lifs.1", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    nof_signals = i_sig + 1;
    SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get(unit, core_id, 0, nof_signals, signal_key, signal_result));

    /** allow FEC table print only if PMF1 destination was FEC */
    if (sal_strcasecmp(signal_result[0].print_value, "FEC") == 0)
    {
        char fec_outlifs_values_str[DSIG_MAX_SIZE_STR] = " ";
        int outlif_idx = 0;

        /** print FEC destination */
        if (signal_result[1].status == BCM_E_NONE)
        {
            PRT_CELL_SET_PL(PP_LAST_FEC_TABLE_ID, "0x%x (%s)", signal_result[1].value[0],
                            signal_result[2].status == BCM_E_NONE ? signal_result[2].print_value : "N/A");
        }
        else
        {
            PRT_CELL_SET_PL(PP_LAST_FEC_TABLE_ID, "Not Found");
        }

        /** build the FEC outlifs values string */
        for (i_sig = 3; i_sig < nof_signals; i_sig++)
        {
            int result_str_length = strlen(fec_outlifs_values_str);

            fec_outlifs_values_str[result_str_length] = outlif_idx + '0';
            fec_outlifs_values_str[result_str_length + 1] = '\0';
            sal_strncat_s(fec_outlifs_values_str, ": ", DSIG_MAX_SIZE_STR - 1);

            if (signal_result[i_sig].status == BCM_E_NONE)
            {
                sal_strncat_s(fec_outlifs_values_str, signal_result[i_sig].print_value, DSIG_MAX_SIZE_STR - 1);
            }
            else
            {
                sal_strncat_s(fec_outlifs_values_str, "N/A", DSIG_MAX_SIZE_STR - 1);
            }

            sal_strncat_s(fec_outlifs_values_str, "\n ", DSIG_MAX_SIZE_STR - 1);
            outlif_idx++;
        }

        PRT_CELL_SET_PL(PP_LAST_FEC_TABLE_ID, "%s", fec_outlifs_values_str);
        pp_last_occupied_tables[PP_LAST_FEC_TABLE_ID] = TRUE;

    } /** of if IPMF1 destination is FEC */

    /******************************
     * fill Trap Resolution table *
 */
    i_sig = 0;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "IPMF3", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "fwd_action_cpu_trap_code", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "IPMF3", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "fwd_strength", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    nof_signals = i_sig + 1;
    SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get(unit, core_id, 0, nof_signals, signal_key, signal_result));

    for (i_sig = 0; i_sig < nof_signals; i_sig++)
    {
        if (signal_result[i_sig].status == BCM_E_NONE)
        {
            PRT_CELL_SET_PL(PP_LAST_TRAP_RESOLUTION_TABLE_ID, "%s", signal_result[i_sig].print_value);
        }
        else
        {
            PRT_CELL_SET_PL(PP_LAST_TRAP_RESOLUTION_TABLE_ID, "Not Found");
        }
    }

    pp_last_occupied_tables[PP_LAST_TRAP_RESOLUTION_TABLE_ID] = TRUE;

    /************************************
     * fill TM Command Resolution table *
 */
    i_sig = 0;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "LBP", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "TM_Cmd.Fwd_Action_Dst_Valid", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "LBP", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "TM_Cmd.Fwd_Action_Dst.value", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "LBP", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "TM_Cmd.Fwd_Action_Dst.type", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "LBP", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "TM_Cmd.tc", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, "LBP", BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "TM_Cmd.dp", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    nof_signals = i_sig + 1;
    SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get(unit, core_id, 0, nof_signals, signal_key, signal_result));

    /** print value of Fwd_Action_Dst only if Fwd_Action_Dst_Valid is set */
    if (signal_result[0].value[0] == 1)
    {
        if (signal_result[1].status == BCM_E_NONE)
        {
            PRT_CELL_SET_PL(PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID, "0x%x (%s)", signal_result[1].value[0],
                            signal_result[2].status == BCM_E_NONE ? signal_result[2].print_value : "N/A");
        }
        else
        {
            PRT_CELL_SET_PL(PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID, "Not Found");
        }
    }
    else
    {
        PRT_CELL_SET_PL(PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID, "Not Valid");
    }

    for (i_sig = 3; i_sig < nof_signals; i_sig++)
    {
        if (signal_result[i_sig].status == BCM_E_NONE)
        {
            PRT_CELL_SET_PL(PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID, "0x%x", signal_result[i_sig].value[0]);
        }
        else
        {
            PRT_CELL_SET_PL(PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID, "Not Found");
        }
    }

    pp_last_occupied_tables[PP_LAST_TM_COMMAND_RESOLUTION_TABLE_ID] = TRUE;

exit:
    sand_signal_output_free(sig_list_element);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function commits the tables per the occupied table bitmap
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] prt_ctr
 *   \param [in] sand_control
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
dnx_pp_last_commit_tables(
    int unit,
    prt_control_t * prt_ctr[],
    sh_sand_control_t * sand_control)
{

    pp_last_table_ids_t table_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * commit (i.e print) tables that are marked as occupied
     * - by first freeing onuccpied table
     * - NULL tables are not printed in combined print
     */
    for (table_id = 0; table_id < PP_LAST_NOF_TABLES; table_id++)
    {
        if (!pp_last_occupied_tables[table_id])
        {
            PRT_FREE_PL(table_id);
        }
    }

    /** print combined all tables that weren't freed */
    PRT_COMMITX_COMBINE_PL(PP_LAST_NOF_TABLES);

    /** free the occupied tables as well */
    for (table_id = 0; table_id < PP_LAST_NOF_TABLES; table_id++)
    {
        if (pp_last_occupied_tables[table_id])
        {
            PRT_FREE_PL(table_id);
        }
    }

exit:
    /** setting back the global of occuopied tables to all FALSE */
    sal_memset(pp_last_occupied_tables, 0, PP_LAST_NOF_TABLES * sizeof(uint8));

    SHR_FUNC_EXIT;
}

/**
 * \brief - main function of showing the IRPP information after last packet passing in pipe
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_last_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_in, core_id;
    /*
     * We need explicitly read system arguments and attach them to command in order to simulate regular framework
     */

    /** Allocate the various tables that constitute the pp vis last*/
    PRT_INIT_VARS_PL(PP_LAST_NOF_TABLES);
    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core_in);

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {

        /** Print message and continue to next core if not ready */
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, TRUE);

        SHR_IF_ERR_EXIT(dnx_pp_last_define_tables(unit, prt_ctr, core_id));

        SHR_IF_ERR_EXIT(dnx_pp_last_fill_tables(unit, prt_ctr, core_id));

        SHR_IF_ERR_EXIT(dnx_pp_last_commit_tables(unit, prt_ctr, sand_control));

    } /** of for of core_id */

exit:

    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_pp_last_options[] = {
    {NULL}
};

sh_sand_man_t dnx_pp_last_man = {
    .brief = "PP VISibility LAST diagnostics.\n" "Shows IRPP information on last packet that passed in pipe.\n",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "",
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};
