#ifndef DIAG_DNX_NIF_TXRX_H_INCLUDED
#define DIAG_DNX_NIF_TXRX_H_INCLUDED

/*************
 * INCLUDES  *
 *************/

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/

extern sh_sand_cmd_t sh_dnx_nif_rx_cmds[];
extern sh_sand_cmd_t sh_dnx_nif_tx_cmds[];

extern sh_sand_man_t sh_dnx_nif_rx_show_man;
extern sh_sand_option_t sh_dnx_nif_rx_show_options[];

extern sh_sand_man_t sh_dnx_nif_tx_show_man;
extern sh_sand_option_t sh_dnx_nif_tx_show_options[];

extern sh_sand_man_t sh_dnx_nif_ilkn_rx_man;
extern sh_sand_option_t sh_dnx_nif_ilkn_rx_options[];

extern sh_sand_man_t sh_dnx_nif_ilkn_tx_man;
extern sh_sand_option_t sh_dnx_nif_ilkn_tx_options[];

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief - Dump the NIF Tx info
 */
shr_error_e sh_dnx_nif_tx_show_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - Dump the NIF Rx info
 */
shr_error_e sh_dnx_nif_rx_show_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - Dump the NIF ILKN Tx info
 */
shr_error_e sh_dnx_nif_ilkn_tx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - Dump the NIF ILKN Rx info
 */
shr_error_e sh_dnx_nif_ilkn_rx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - Map NIF RX counter to port
 */
shr_error_e sh_dnx_nif_rx_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - Map NIF TX counter to port
 */
shr_error_e sh_dnx_nif_tx_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /** DIAG_DNX_NIF_TXRX_H_INCLUDED */
