/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*! \file diag_dnx_pp.h
 *
 * Main header file for PP Diagnostics command files.
 */

#ifndef DIAG_DNX_PP_INCLUDED
#define DIAG_DNX_PP_INCLUDED

/*************
 * INCLUDES  *
 */
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_str.h>

#include <soc/sand/sand_pparse.h>
#include <soc/sand/sand_signals.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

#include <bcm_int/dnx/port/port_tpid.h>
#include <bcm_int/dnx/switch/switch_tpid.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>

#include <appl/diag/diag.h>
#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_signals.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_utils.h>

/*************
 *  DEFINES  *
 */

/**
 * \brief - the allowed max number of sub hierarchies in a sig get command
 */
#define SH_DNX_PP_MAX_SIGNAL_HIERARCHY 16

/** max allowed number of layers overall */
#define SH_DNX_PP_MAX_LAYERS_NUMBER 100

/** for pp vis context diagnostic */
#define DNX_CONTEXT_HIT_INDICATION_NOF_ROWS     16

/** Compatibility massage when calling a not available diagnostic in bare metal */
#define DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND    "The command is not supported for bare-metal, running not with the standard Image"

/*************
 *  MACROS  *
 */

/**
 * \brief - outputs the signal expansion of signal as its name (if expansion exists), otherwise signal name
 */
#define SH_DNX_PP_GET_SIG_EXP_NAME(__signal_output_var)   ((ISEMPTY(__signal_output_var->expansion)) ? RHNAME(__signal_output_var) : __signal_output_var->expansion)

/*************
 *  TYPDEFS  *
 */

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_pp_man;
extern sh_sand_cmd_t sh_dnx_pp_cmds[];

/** sub-layer names that represent not actual layer */
extern char *sh_dnx_pp_sub_layer_names[];

/** system_header_names */
extern char *dnx_pp_system_header_names[];

/**************************
 * FUNCTIONS DECLARATION  *
 */
/**
 * \brief - check if image is standard image or bare metal
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] list - not in use
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e diag_pp_bare_metal_check(
    int unit,
    rhlist_t * list);

/**
 * \brief - Function receives a string and a string array
 *          it returns TRUE if one of the array strings match or are contained in the input string
 *          and updates the index of the location in the array
 *
 * \par DIRECT_INPUT:
 *   \param [in] str - the string to see match on
 *   \param [in] exact_match - TRUE - exact match, FALSE - check if str contained in one of array strings
 *   \param [in] str_arr - array of strings, assumption is that last string is "" to act as stop condition
 *
 *
 * \par DIRECT OUTPUT:
 *   int - if str found in str_array, it will contain the index location in str_array, otherwise '-1'
 */
int string_in_array(
    char *str,
    uint8 exact_match,
    char **str_arr);

/**
 * \brief - Function gets an rhlist of a packet header signal and constructs its layer headers structure as string,
 *          as well as constructing an array of "next headers" while skipping sub-layers
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit
 *   \param [in] layer_list - rhlist which represents the packet header signal
 *   \param [out] packet_struct_str -this is the output of string of packet structure
 *   \param [out] next_protocol_array - array of strings of next protocols, assumption is that last string is "" to act as stop condition
 *                                      if NULL, it won't be filled
 *   \param [out] mpls_label_array - array of strings containing the MPLS labels relared to 'next_protocol_array'
 *
 * \par DIRECT OUTPUT:
 *   int - if str found in str_array, it will contain the index location in str_array, otherwise '-1'
 */
shr_error_e construct_packet_structure(
    int unit,
    rhlist_t * layer_list,
    char *packet_struct_str,
    char next_protocol_array[SH_DNX_PP_MAX_LAYERS_NUMBER][RHNAME_MAX_SIZE],
    char mpls_label_array[SH_DNX_PP_MAX_LAYERS_NUMBER][DSIG_MAX_SIZE_UINT32]);

/**
 * \brief
 * get ingress header signal from  "NIF" and "InNetworkAdaptor", or "PRT" when missing from "NIF"
 *
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [out] sig_list_element - signal list of header structure
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_pp_ingress_header_sig_get(
    int unit,
    int core_id,
    signal_output_t ** sig_list_element);

/**
 * \brief - the function returns as a full hierarchy string the name, value or size of the signal
 *          usually used to print as a whole as a single cell in a table
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] block
 *   \param [in] from
 *   \param [in] to
 *   \param [in] sig_name_hieararchy
 *   \param [in] print_name - the name to replace the first hierarchy signal name
 *   \param [in] nvs - Name/Value/Size/Offset - 0/1/2/3 respectively
 *   \param [out] result_hierarchial_str - the result which is also an output
 *   \param [in] skip_zero_value - relevant for nvs = 1 only, skip values that are '0' - only from next hierarchy
 *   \param [in] include_first_hierarchy - include the first hierarchy - suppose when not to print the overall signal value, but only parsing values
 *   \param [in] tie_name_to_value - in case near the value should be also printed the name
 *   \param [in] sig_offset - offset to which to add in case offset is of interest (nvs = 3)*
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e get_sig_hierarchy_str(
    int unit,
    int core_id,
    char *block,
    char *from,
    char *to,
    char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY],
    char print_name[RHNAME_MAX_SIZE],
    int nvs,
    char result_hierarchial_str[DSIG_MAX_SIZE_STR],
    uint8 skip_zero_value,
    uint8 include_first_hierarchy,
    uint8 tie_name_to_value,
    int sig_offset);

/**
 * \brief - function to iterate through same hierarchy fields in the field list arugment received
 * print each field, and drill down if it has down hierarchy fields, recursively
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - list of fields to print, and drill down each if it has down hierarchy
 *   \param [in] field_list - list of fields to print, and drill down each if it has down hierarchy
 *   \param [in] prt_ctr - the pointer to print table controller which is used through out the printout
 *   \param [in] shifts - number of shifts (aka. tabs) to be used for hierarchial printing
 *   \param [in] print_name - if not empty, it replaces the signal name
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e recursive_field_print_iterator(
    int unit,
    rhlist_t * field_list,
    prt_control_t * prt_ctr,
    int shifts,
    char *print_name);

/**
 * \brief - prints a sig get signal directly to a table,
 * with an option to use a condition field (same hierarchy as the signal)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] block
 *   \param [in] from
 *   \param [in] to
 *   \param [in] sig_name_hieararchy
 *   \param [in] cond_name_hieararchy
 *   \param [in] condition_field_expected_value
 *   \param [in] print_name - must be to give the table name
 *   \param [in] prt_ctr - table control
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e print_sig_in_table(
    int unit,
    int core_id,
    char *block,
    char *from,
    char *to,
    char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY],
    char *cond_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY],
    uint32 condition_field_expected_value,
    char print_name[RHNAME_MAX_SIZE],
    prt_control_t * prt_ctr);

/**
 * \brief
 *    A diagnostics function that present the ECMP bank allocation data
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e sh_dnx_pp_vis_fer_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_PP_INCLUDED */
