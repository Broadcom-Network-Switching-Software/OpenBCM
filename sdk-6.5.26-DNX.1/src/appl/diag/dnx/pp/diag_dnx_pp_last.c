/** \file diag_dnx_pp_last.c
 *
 * Last PP Diagnostics File
 * Shows IRPP information on last packet that passed in pipe
 */

/*************
 * INCLUDES  *
 *************/
#include "diag_dnx_pp.h"
#include <bcm_int/dnx/instru/instru_visibility.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>

/*************
 * DEFINES   *
 *************/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

#define PP_LAST_MAX_NOF_INSTRU_SIGNALS         5

/** max number of lookup terms in all fwd layers */
#define PP_LAST_MAX_NOF_LAYER_LOOKUP_TERMS     4

#define PP_LAST_SRV6_SEGMENT_SIZE_BYTES        16

/*************
 *  MACROS  *
 *************/

/** reads the FWD layer lookup terms signal, and also extra lookup terms (PP_LAST_FODO, Global LIF) and coppies to a structure */
#define __PP_LAST_GET_FWD_LAYER_LOOKUP_TERMS(__fwd_layer)                                                                                                                           \
    current_fwd_layer_lookup_terms.nof_lookup_terms_signals = pp_last_fwd_layers_lookup_terms[__fwd_layer].nof_lookup_terms_signals;                                                \
    current_fwd_layer_lookup_terms.nof_lookup_terms_extra = pp_last_fwd_layers_lookup_terms[__fwd_layer].nof_lookup_terms_extra;                                                    \
                                                                                                                                                                            \
    for (i_sig_fwd_lookup_term = 0; i_sig_fwd_lookup_term < pp_last_fwd_layers_lookup_terms[__fwd_layer].nof_lookup_terms_signals; i_sig_fwd_lookup_term++)                         \
    {                                                                                                                                                                       \
        if (pkt_header_sig && pkt_header_sig->field_list)                                                                                                                                              \
        {                                                                                                                                                                            \
            field_header_sig = utilex_rhlist_entry_get_by_name(pkt_header_sig->field_list, pp_last_fwd_layers_lookup_terms[__fwd_layer].lookup_term_signals[i_sig_fwd_lookup_term]); \
        }                                                                                                                                                                            \
        else                                                                                                                                                                         \
        {                                                                                                                                                                            \
            break; \
        }                                                                                                                                                                            \
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
            sal_strncat_s(current_fwd_layer_lookup_terms.extra_lookup_term_str[i_extra_fwd_lookup_term], global_lif_str, RHSTRING_MAX_SIZE);                                \
        }                                                                                                                                                                   \
    }

/*************
 * TYPEDEFS  *
 *************/

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
 *************/

/** set all tables to unoccupied - if a table is filled its indication is set */
uint8 pp_last_occupied_tables[PP_LAST_NOF_TABLES] = { 0 };

/*************
 * FUNCTIONS *
 *************/
extern shr_error_e sh_dnx_pp_termi_info(
    int unit,
    int core_id,
    char *packet_struct_str);
shr_error_e dnx_pp_vis_fer_info(
    int unit,
    int core_id,
    char *packet_struct_str);

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
    if (dnx_data_headers.
        system_headers.feature_get(unit, dnx_data_headers_system_headers_system_headers_ibch1_supported))
    {
        PRT_COLUMN_ADD_FLEX_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, PRT_FLEX_ASCII, "PTC");
    }
    else
    {
        PRT_COLUMN_ADD_FLEX_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, PRT_FLEX_ASCII, "IN TM Port");
    }
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, PRT_FLEX_ASCII, "Sys Port");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, PRT_FLEX_ASCII, "PRT Qualifier");
    PRT_ROW_ADD_PL(PP_LAST_PORT_TERMINATION_TABLE_ID, PRT_ROW_SEP_NONE);

    PRT_TITLE_SET_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, "<VTT Summary>");
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, PRT_FLEX_ASCII, "Terminated Headers");
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
    PRT_COLUMN_ADD_FLEX_PL(PP_LAST_FEC_TABLE_ID, PRT_FLEX_ASCII, "Hiearchies Hit");
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
 * \brief - Update the forwarding DIP in case of SRV6
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core ID
 *   \param [in,out] srv6_printable_dip - printable DIP string.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_pp_last_srv6_dip_update(
    int unit,
    int core_id,
    char srv6_printable_dip[RHSTRING_MAX_SIZE])
{
    signal_output_t *udh_data_sig = NULL;
    signal_output_t *tmp_sig = NULL;
    signal_output_t *fwd2_context_sig = NULL;
    uint32 fwd2_context;
    uint32 ipv6_addr[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 }, index, value;
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE] = "";
    char udh_header_name[RHSTRING_MAX_SIZE];
    dbal_enum_value_field_fwd2_context_id_e srv6_contexts_in_fwd2[] = {
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___PRIVATE_UC,
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX,
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_PRIVATE_UC,
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NOP_CTX,
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NO_DEFAULT_PRIVATE_UC,
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NO_DEFAULT_NOP_CTX,
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_48B_PREFIX_PRIVATE_UC,
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_48B_PREFIX_NO_DEFAULT_PRIVATE_UC,
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_NOP_CTX,
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_PRIVATE_UC,
        DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_64B_PREFIX_PRIVATE_UC
    };

    int is_srv6_context = FALSE, ctx_idx = 0, nof_srv6_context = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** If it's SRV6, need to update the DIP with the right SegmentID*/
    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "FWD1", "FWD2", "Context",
                                                &fwd2_context_sig), "no signal record for fwd2_context_sig\n");
    SHR_IF_ERR_EXIT(dbal_fields_enum_value_get
                    (unit, DBAL_FIELD_FWD2_CONTEXT_ID, fwd2_context_sig->value[0], &fwd2_context));

    nof_srv6_context = sizeof(srv6_contexts_in_fwd2) / sizeof(dbal_enum_value_field_fwd2_context_id_e);

    for (ctx_idx = 0; ctx_idx < nof_srv6_context; ctx_idx++)
    {
        if (fwd2_context == srv6_contexts_in_fwd2[ctx_idx])
        {
            is_srv6_context = TRUE;
            break;
        }
    }

    if (is_srv6_context)
    {
        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                            (unit, core_id, 0, "IRPP", "LBP", "ITMR", "Header_to_Add.PPH_Base.PPH_Parsing_Start_Type",
                             &tmp_sig), "no signal record for %s\n", "PPH_Parsing_Start_Type");

        SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_PARSING_START_TYPE, tmp_sig->value[0], &value));
        if (value == DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_ENDPOINT_PSP)
        {
            signal_output_t *sid0_sig = NULL;
            signal_output_t *header_sig = NULL;

            SHR_CLI_EXIT_IF_ERR(dnx_pp_ingress_header_sig_get(unit, core_id, &tmp_sig),
                                "no signal record for header\n");
            header_sig = utilex_rhlist_entry_get_by_name(tmp_sig->field_list, "SRv6");
            if (header_sig == NULL)
            {
                 /* coverity[dead_error_line:FALSE]  */
                SHR_CLI_EXIT_IF_ERR(_SHR_E_INTERNAL, "SRv6 header is not found!\n");
            }
            sid0_sig = utilex_rhlist_entry_get_by_name(header_sig->field_list, "Segment_ID_0");
            if (sid0_sig)
            {
                ipv6_addr[0] = sid0_sig->value[0];
                ipv6_addr[1] = sid0_sig->value[1];
                ipv6_addr[2] = sid0_sig->value[2];
                ipv6_addr[3] = sid0_sig->value[3];
            }
        }
        else
        {
            for (index = 0; index < PP_LAST_SRV6_SEGMENT_SIZE_BYTES / 4; index++)
            {
                sal_snprintf(udh_header_name, RHNAME_MAX_SIZE, "Header_to_Add.UDH_DATA_%d", index);
                SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0, "IRPP", "LBP", "ITMR", udh_header_name,
                                                            &udh_data_sig), "no signal record for %s\n",
                                    udh_header_name);
                ipv6_addr[PP_LAST_SRV6_SEGMENT_SIZE_BYTES / 4 - 1 - index] = udh_data_sig->value[0];
            }
        }

        /** Format the IPv6 address by a router table*/
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST, DBAL_FIELD_IPV6, 0, ipv6_addr, NULL, 0, TRUE,
                         buffer_to_print));
        sal_strncpy_s(srv6_printable_dip, buffer_to_print, RHSTRING_MAX_SIZE - 1);
    }

exit:
    sand_signal_output_free(fwd2_context_sig);
    sand_signal_output_free(udh_data_sig);
    sand_signal_output_free(tmp_sig);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function returns if packet should be skipped with its layer offsets signals checks
 *          -Case (1) - In case of SRv6 packet - the Parser offsets can be wrong and not match PParse
 *          -Case (2) - Currently skip at all cases this check
 *
 * \par DIRECT_INPUT:
 *   \param [in] packet_structure_str -packet structure string
 *
 * \par DIRECT OUTPUT:
 *   uint8
 */
static uint8
dnx_pp_last_skip_layer_offsets_check(
    char *packet_structure_str)
{
    uint8 skip_check = TRUE; /** currently always skip offsets checks for future cases */

    if (sal_strnstr(packet_structure_str, "SRv6", DSIG_MAX_SIZE_STR))
    {
        skip_check = TRUE;
    }

    return skip_check;

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
    signal_output_t *protocols_sig_head = NULL;
    signal_output_t *protocols_sig = NULL;
    signal_output_t *pkt_header_sig_head = NULL;
    signal_output_t *pkt_header_sig = NULL;
    signal_output_t *layer_offsets_sig_head = NULL;
    signal_output_t *layer_offsets_sig = NULL;
    char packet_structure_str[DSIG_MAX_SIZE_STR] = "";

    bcm_instru_vis_signal_key_t signal_key[PP_LAST_MAX_NOF_INSTRU_SIGNALS];
    bcm_instru_vis_signal_result_t signal_result[PP_LAST_MAX_NOF_INSTRU_SIGNALS];
    int i_sig, nof_signals, i_sig_fwd_lookup_term, i_extra_fwd_lookup_term;
    int fwd_layer_index = 0;
    char val_string[RHSTRING_MAX_SIZE] = "Not Found";
    char fwd_layer_string[RHSTRING_MAX_SIZE] = "";

    int pparse_layer_offset = 0;
    int is_swap_enable = 0, swap_size = 0;

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
     **************************************************************************************************/
    SHR_CLI_EXIT_IF_ERR(dnx_pp_ingress_header_sig_get(unit, core_id, &sig_list_element),
                        "no signal record for header\n");

    /** get the overall packet structure and return the packet structure string + update layer linked list for 'next layer' info */
    SHR_IF_ERR_EXIT(construct_packet_structure(unit, sig_list_element->field_list, packet_structure_str, NULL, NULL));

    PRT_INFO_ADD_PL(PP_LAST_PACKET_STRUCTURE_TABLE_ID, "%s", packet_structure_str);
    pp_last_occupied_tables[PP_LAST_PACKET_STRUCTURE_TABLE_ID] = TRUE;

    /*******************************
     * fill Port Termination table *
     *******************************/
    i_sig = 0;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, dnx_data_pp.debug.from_signal_str_get(unit)->in_port,
                  BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "In_Port", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, dnx_data_pp.debug.from_signal_str_get(unit)->ptc,
                  BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "PTC", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, dnx_data_pp.debug.from_signal_str_get(unit)->in_port,
                  BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Src_Sys_Port_ID", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

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

    /*
     * check the swap size only for local port
     * SSP signal is 16bits, bit15=1 indicated this is a LAG ssp. 
     */
    if (!(signal_result[2].value[0] & 0x8000))
    {
        bcm_gport_t sys_gport;
        dnx_algo_gpm_gport_phy_info_t gport_info;

        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, signal_result[2].value[0]);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, sys_gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        /** handle for swap case */
        SHR_IF_ERR_EXIT(bcm_port_control_get
                        (unit, gport_info.local_port, bcmPortControlExtenderEnable, &is_swap_enable));
        if (is_swap_enable)
        {
            /**store as char*/
            swap_size = 4;
        }
    }

    pp_last_occupied_tables[PP_LAST_PORT_TERMINATION_TABLE_ID] = TRUE;

    /**************************
     * fill VTT Summary table *
     **************************/
    /*
     * Terminated Headers 
     */
    sal_memset(packet_structure_str, 0, sizeof(packet_structure_str));
    SHR_IF_ERR_EXIT(sh_dnx_pp_termi_info(unit, core_id, packet_structure_str));
    PRT_CELL_SET_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, "%s", packet_structure_str);

    i_sig = 0;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, dnx_data_pp.debug.from_signal_str_get(unit)->in_lif,
                  BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "In_LIFs.0", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, dnx_data_pp.debug.from_signal_str_get(unit)->fwd_domain,
                  BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Fwd_Domain_ID.type", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, dnx_data_pp.debug.from_signal_str_get(unit)->fwd_domain,
                  BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].signal, "Fwd_Domain_ID.value", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    i_sig++;
    sal_memset(&signal_key[i_sig], 0, sizeof(bcm_instru_vis_signal_key_t));
    sal_strncpy_s(signal_key[i_sig].block, "IRPP", BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy_s(signal_key[i_sig].from, dnx_data_pp.debug.from_signal_str_get(unit)->ingress_layers,
                  BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
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
                sal_strncat_s(val_string, signal_result[i_sig].print_value, sizeof(val_string));

                /** store the Global LIF for FWD stage lookup terms  */
                sal_strncpy_s(global_lif_str, val_string, RHSTRING_MAX_SIZE - 1);
            }
            /** for PP_LAST_FODO construct a string of PP_LAST_FODO Type + Value, and store for FWD stage lookup terms */
            else if (i_sig == 1)
            {
                sal_strncpy_s(val_string, signal_result[1].print_value, RHSTRING_MAX_SIZE - 1);
                sal_strncat_s(val_string, " = 0x", sizeof(val_string));
                sal_strncat_s(val_string, signal_result[2].status == BCM_E_NONE ? signal_result[2].print_value : "N/A",
                              sizeof(val_string));

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

    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                        (unit, core_id, 0, "IRPP", dnx_data_pp.debug.from_signal_str_get(unit)->ingress_layers, "",
                         "layer_protocols", &protocols_sig_head), "no signal record for layer_protocols\n");

    SHR_CLI_EXIT_IF_ERR(dnx_pp_ingress_header_sig_get(unit, core_id, &pkt_header_sig_head),
                        "no signal record for header\n");

    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find
                        (unit, core_id, 0, "IRPP", dnx_data_pp.debug.from_signal_str_get(unit)->ingress_layers, "",
                         "layer_offsets", &layer_offsets_sig_head), "no signal record for layer_offsets\n");

    /** set initial value of fwd_layer to the name of the 1st layer */
    i_sig = 0;
    sal_strncpy_s(val_string, "idx(", RHSTRING_MAX_SIZE - 1);
    sal_strncat_s(val_string, signal_result[3].print_value, sizeof(val_string));
    sal_strncat_s(val_string, ") - ", sizeof(val_string));
    sal_strncpy_s(fwd_layer_string, protocols_sig_head->print_value, RHSTRING_MAX_SIZE - 1);

    pkt_header_sig = utilex_rhlist_entry_get_first(pkt_header_sig_head->field_list);
    layer_offsets_sig = utilex_rhlist_entry_get_first(layer_offsets_sig_head->field_list);
    pparse_layer_offset += swap_size;
    RHITERATOR(protocols_sig, protocols_sig_head->field_list)
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
            if (!(dnx_pp_last_skip_layer_offsets_check(packet_structure_str)) &&
                (pparse_layer_offset != parser_fwd_layer_offset))
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "could not match FWD layer offset in pkt_header (%d) to that of parser layer_offsets(%d) !\n",
                             pparse_layer_offset, parser_fwd_layer_offset);
            }

            sal_strncpy_s(fwd_layer_string, protocols_sig->print_value, RHSTRING_MAX_SIZE - 1);

            /*
             * read the relevant FWD lookup terms from signals - by the type of the FWD layer (ETH/IPv4/IPv6/MPLS)
             */
            if (sal_strnstr(fwd_layer_string, "ETH", sizeof(fwd_layer_string)))
            {
                __PP_LAST_GET_FWD_LAYER_LOOKUP_TERMS(PP_LAST_ETH_FWD_LAYER);
                fwd_layer_type = PP_LAST_ETH_FWD_LAYER;
            }
            else if (sal_strnstr(fwd_layer_string, "IPV4", sizeof(fwd_layer_string)))
            {
                __PP_LAST_GET_FWD_LAYER_LOOKUP_TERMS(PP_LAST_IPV4_FWD_LAYER);
                fwd_layer_type = PP_LAST_IPV4_FWD_LAYER;
            }
            else if (sal_strnstr(fwd_layer_string, "IPV6", sizeof(fwd_layer_string)))
            {
                __PP_LAST_GET_FWD_LAYER_LOOKUP_TERMS(PP_LAST_IPV6_FWD_LAYER);
                fwd_layer_type = PP_LAST_IPV6_FWD_LAYER;

                /** If it's SRV6, need to update the DIP with the right SegmentID*/
                for (i_sig_fwd_lookup_term = 0;
                     i_sig_fwd_lookup_term <
                     pp_last_fwd_layers_lookup_terms[PP_LAST_IPV6_FWD_LAYER].nof_lookup_terms_signals;
                     i_sig_fwd_lookup_term++)
                {
                    if (sal_strnstr
                        (pp_last_fwd_layers_lookup_terms[PP_LAST_IPV6_FWD_LAYER].lookup_term_signals
                         [i_sig_fwd_lookup_term], "DIP", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
                    {
                        SHR_IF_ERR_EXIT(dnx_pp_last_srv6_dip_update
                                        (unit, core_id,
                                         current_fwd_layer_lookup_terms.lookup_term_signals[i_sig_fwd_lookup_term]));
                        break;
                    }
                }
            }
            else if (sal_strnstr(fwd_layer_string, "MPLS_UNTERM", sizeof(fwd_layer_string)))
            {
                __PP_LAST_GET_FWD_LAYER_LOOKUP_TERMS(PP_LAST_MPLS_FWD_LAYER);
                fwd_layer_type = PP_LAST_MPLS_FWD_LAYER;
            }

            break;
        }

        i_sig++;
        layer_offsets_sig = utilex_rhlist_entry_get_next(layer_offsets_sig);
    }

    sal_strncat_s(val_string, fwd_layer_string, sizeof(val_string));
    PRT_CELL_SET_PL(PP_LAST_VTT_SUMMARY_TABLE_ID, "%s", val_string);
    pp_last_occupied_tables[PP_LAST_VTT_SUMMARY_TABLE_ID] = TRUE;

    /*************************
     * fill FWD Lookup table *
     *************************/

    /*
     * print the FWD Layer lookup terms
     */

    val_string[0] = '\0';
    for (i_sig_fwd_lookup_term = 0; i_sig_fwd_lookup_term < current_fwd_layer_lookup_terms.nof_lookup_terms_signals;
         i_sig_fwd_lookup_term++)
    {
        sal_strncat_s(val_string,
                      pp_last_fwd_layers_lookup_terms[fwd_layer_type].lookup_term_signals[i_sig_fwd_lookup_term],
                      sizeof(val_string));
        sal_strncat_s(val_string, " = ", sizeof(val_string));
        sal_strncat_s(val_string, current_fwd_layer_lookup_terms.lookup_term_signals[i_sig_fwd_lookup_term],
                      sizeof(val_string));
        sal_strncat_s(val_string, "\n", sizeof(val_string));
    }

    for (i_extra_fwd_lookup_term = 0; i_extra_fwd_lookup_term < current_fwd_layer_lookup_terms.nof_lookup_terms_extra;
         i_extra_fwd_lookup_term++)
    {
        sal_strncat_s(val_string, current_fwd_layer_lookup_terms.extra_lookup_term_str[i_extra_fwd_lookup_term],
                      sizeof(val_string));
        if (i_extra_fwd_lookup_term < current_fwd_layer_lookup_terms.nof_lookup_terms_extra - 1)
        {
            sal_strncat_s(val_string, "\n", sizeof(val_string));
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
     ******************/

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
    if (sal_strncasecmp(signal_result[0].print_value, "FEC", sizeof(signal_result[0].print_value)) == 0)
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
            int result_str_length = sal_strnlen(fec_outlifs_values_str, sizeof(fec_outlifs_values_str));

            fec_outlifs_values_str[result_str_length] = outlif_idx + '0';
            fec_outlifs_values_str[result_str_length + 1] = '\0';
            sal_strncat_s(fec_outlifs_values_str, ": ", sizeof(fec_outlifs_values_str));

            if (signal_result[i_sig].status == BCM_E_NONE)
            {
                sal_strncat_s(fec_outlifs_values_str, signal_result[i_sig].print_value, sizeof(fec_outlifs_values_str));
            }
            else
            {
                sal_strncat_s(fec_outlifs_values_str, "N/A", sizeof(fec_outlifs_values_str));
            }

            sal_strncat_s(fec_outlifs_values_str, "\n ", sizeof(fec_outlifs_values_str));
            outlif_idx++;
        }

        PRT_CELL_SET_PL(PP_LAST_FEC_TABLE_ID, "%s", fec_outlifs_values_str);
        pp_last_occupied_tables[PP_LAST_FEC_TABLE_ID] = TRUE;

    } /** of if IPMF1 destination is FEC */

    /*
     * Hiearchies Hit 
     */
    sal_memset(packet_structure_str, 0, sizeof(packet_structure_str));
    SHR_IF_ERR_EXIT(dnx_pp_vis_fer_info(unit, core_id, packet_structure_str));
    PRT_CELL_SET_PL(PP_LAST_FEC_TABLE_ID, "%s", packet_structure_str);

    /******************************
     * fill Trap Resolution table *
     ******************************/
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
     ************************************/
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
    sand_signal_output_free(protocols_sig_head);
    sand_signal_output_free(pkt_header_sig_head);
    sand_signal_output_free(layer_offsets_sig_head);
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
