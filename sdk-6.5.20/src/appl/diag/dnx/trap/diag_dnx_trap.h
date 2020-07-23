/** \file diag_dnx_trap.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_TRAP_H_INCLUDED
#define DIAG_DNX_TRAP_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include <src/soc/dnx/dbal/dbal_internal.h>

/*************
 *  DEFINES  *
 */

#define DIAG_DNX_TRAP_ID_STRING_SIZE                        20
#define DIAG_DNX_TRAP_MTU_NOF_FWD_LAYER_TYPES_PER_TABLE     4
#define DIAG_DNX_TRAP_PROTOCOL_LIST_ALL                     (0xFFFFFF)

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_trap_man;
extern sh_sand_man_t sh_dnx_trap_mtu_cmd_man;
extern sh_sand_man_t sh_dnx_trap_svtag_cmd_man;
extern sh_sand_man_t sh_dnx_trap_lif_cmd_man;
extern sh_sand_cmd_t sh_dnx_trap_cmds[];
extern sh_sand_cmd_t sh_dnx_trap_list_cmds[];
extern sh_sand_cmd_t sh_dnx_trap_map_cmds[];
extern sh_sand_cmd_t sh_dnx_trap_action_cmds[];
extern sh_sand_cmd_t sh_dnx_trap_prog_cmds[];
extern sh_sand_cmd_t sh_dnx_trap_protocol_cmds[];
extern sh_sand_cmd_t sh_dnx_trap_last_pkt_cmds[];
extern sh_sand_cmd_t sh_dnx_trap_mtu_cmds[];

extern const sh_sand_enum_t Trap_block_enum_table[];

typedef struct sh_dnx_trap_mtu_layer_info_s
{
    /** DBAL Table enum */
    dbal_tables_e dbal_table;

    /** Forward Layer Types */
    bcm_field_layer_type_t fwd_layer_type[DIAG_DNX_TRAP_MTU_NOF_FWD_LAYER_TYPES_PER_TABLE];

    /** Number of Forward Layer types per table */
    int nof_fwd_layer_types;

    /** Layer name of the DBAL table */
    char *fwd_layer_name[DIAG_DNX_TRAP_MTU_NOF_FWD_LAYER_TYPES_PER_TABLE];

} sh_dnx_trap_mtu_layer_info_t;

typedef struct sh_dnx_trap_lif_table_info_s
{
    /** DBAL Table enum */
    dbal_tables_e dbal_table;

    /** LIF key field type */
    dbal_fields_e lif_field_type;

} sh_dnx_trap_lif_table_info_t;

/*************
 * FUNCTIONS *
 */

/**
 * \brief
 *    Print Function for all MTU option for "trap mtu".
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e sh_dnx_trap_mtu_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief
 *    A diagnostics function that prints a list of LIFs/RIFs for a given lif_type
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e sh_dnx_trap_lif_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief
 *    A diagnostics function that prints a list of set SVTAG Traps
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e sh_dnx_trap_svtag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief
 *   Example for OutRIF create.
 *
 * \param [in] unit - The unit number.
 * \param [out] rif_p - RIF
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e diag_dnx_rx_trap_outrif_create_example(
    int unit,
    bcm_if_t * rif_p);

/**
 * \brief
 *   RIF destroy function.
 *
 * \param [in] unit - The unit number.
 * \param [in] rif_p - RIF.
 * \param [in] rif_type - ingress or egress RIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e diag_dnx_rx_trap_rif_destroy_example(
    int unit,
    bcm_if_t * rif_p,
    bcm_rx_trap_lif_type_t rif_type);

/**
 * \brief
 *    Example for In-LIF create.
 *
 * \param [in] unit - The unit number.
 * \param [in] lif_p - LIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e diag_dnx_rx_trap_inlif_create_example(
    int unit,
    bcm_gport_t * lif_p);

/**
 * \brief
 *    LIF Destroy function.
 *
 * \param [in] unit - The unit number.
 * \param [in] lif_p - LIF.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
shr_error_e diag_dnx_rx_trap_lif_destroy_example(
    int unit,
    bcm_gport_t * lif_p);

/**
* \brief
*  Translates the given Egress trap_id to a string for printing.
* \par DIRECT INPUT:
*   \param [in] trap_id       -  Trap ID to translate
*   \param [out] trap_id_str  -  Char output variable for printing
* \remark
*   * None
* \see
*   * None
*/
void dnx_egress_trap_id_to_string(
    int trap_id,
    char *trap_id_str);

/**
 * \brief
 *    A function that prints trap snoop action info.
 * \param [in] unit - unit ID
 * \param [in] trap_title - trap title
 * \param [in] trap_type - trap type to print its info
 * \param [in] snp_code - snoop code (HW code) 
 * \param [in] trap_config_p - trap configuration
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_trap_snp_action_info_irpp_print(
    int unit,
    char *trap_title,
    bcm_rx_trap_t trap_type,
    int snp_code,
    bcm_rx_trap_config_t * trap_config_p,
    sh_sand_control_t * sand_control);

/**
 * \brief
 *    A function that prints trap action info.
 * \param [in] unit - unit ID
 * \param [in] trap_title - trap title
 * \param [in] trap_type - trap type to print its info
 * \param [in] trap_id - trap id (HW code) 
 * \param [in] trap_config_p - trap configuration
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_trap_action_info_irpp_print(
    int unit,
    char *trap_title,
    bcm_rx_trap_t trap_type,
    int trap_id,
    bcm_rx_trap_config_t * trap_config_p,
    sh_sand_control_t * sand_control);

/**
 * \brief
 *    A function that prints ERPP trap action info.
 * \param [in] unit - unit ID
 * \param [in] trap_title - trap title 
 * \param [in] trap_type - trap type to print its info
 * \param [in] trap_id - trap id encoded
 * \param [in] trap_config_p - trap infouration
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   * Zero if no error was detected
 *   * Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_trap_action_info_erpp_print(
    int unit,
    char *trap_title,
    bcm_rx_trap_t trap_type,
    int trap_id,
    bcm_rx_trap_config_t * trap_config_p,
    sh_sand_control_t * sand_control);

/**
 * \brief
 *    A function that prints ETPP trap action info.
 * \param [in] unit - unit ID
 * \param [in] trap_title - trap title to print 
 * \param [in] trap_type - trap type to print its info
 * \param [in] trap_id - trap id encoded
 * \param [in] trap_config_p - trap infouration
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_trap_action_info_etpp_print(
    int unit,
    char *trap_title,
    bcm_rx_trap_t trap_type,
    int trap_id,
    bcm_rx_trap_config_t * trap_config_p,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_TRAP_H_INCLUDED */
