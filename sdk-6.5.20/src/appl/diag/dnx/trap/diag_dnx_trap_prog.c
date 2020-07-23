/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * diag_dnx_trap_prog.c
 *
 *  Created on: Feb 6, 2018
 *      Author: dp889757
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_trap.h"

/** String values for enable state of programmable trap fields. */
const sh_sand_enum_t Trap_prog_enable_enum_table[] = {
    /**String      Value*/
    {"Disabled", bcmRxTrapProgDisable}
    ,
    {"Match", bcmRxTrapProgEnableMatch}
    ,
    {"Mismatch", bcmRxTrapProgEnableMismatch}
    ,
    {NULL}
};

/**
* \brief
*    Init callback function for "trap prog" diag commands
*    1) Create an ingress user defined trap
*    2) Configure programmable trap conditions and assign a trap to specific prog_index
* \param [in] unit - unit ID
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
static shr_error_e
diag_dnx_trap_prog_init_cb(
    int unit)
{
    bcm_rx_trap_prog_config_t prog_config;
    bcm_mac_t dest_mac = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    bcm_mac_t src_mac = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    bcm_rx_trap_config_t config = { 0 };
    int fwd_strength = 15, snp_strength = 0, trap_id = -1, prog_index = 4;
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&config);
    bcm_rx_trap_prog_config_t_init(&prog_config);

    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));

    sal_memcpy(&prog_config.dest_mac, dest_mac, sizeof(prog_config.dest_mac));
    prog_config.dest_mac_nof_bits = 48;
    prog_config.dest_mac_enable = bcmRxTrapProgEnableMatch;

    sal_memcpy(&prog_config.src_mac, src_mac, sizeof(prog_config.src_mac));
    prog_config.src_mac_nof_bits = 48;
    prog_config.src_mac_enable = bcmRxTrapProgEnableMismatch;

    prog_config.ether_type = 0x0800;
    prog_config.ether_type_enable = bcmRxTrapProgDisable;

    prog_config.sub_type = 0xF;
    prog_config.sub_type_mask = 0xF;
    prog_config.sub_type_enable = bcmRxTrapProgEnableMismatch;

    prog_config.ip_protocol = 6;
    prog_config.ip_protocol_enable = bcmRxTrapProgEnableMatch;

    prog_config.src_port = 200;
    prog_config.src_port_mask = 0xFF;
    prog_config.dest_port = 250;
    prog_config.dest_port_mask = 0xFF;
    prog_config.l4_ports_enable = bcmRxTrapProgEnableMismatch;

    prog_config.tcp_flags = 0x32;
    prog_config.tcp_flags_mask = 0x1FF;
    prog_config.tcp_flags_enable = bcmRxTrapProgEnableMatch;

    prog_config.tcp_seq_is_zero_enable = bcmRxTrapProgDisable;

    BCM_GPORT_TRAP_SET(prog_config.trap_gport, trap_id, fwd_strength, snp_strength);

    SHR_IF_ERR_EXIT(bcm_rx_trap_prog_set(unit, 0, prog_index, &prog_config));

exit:
    SHR_FUNC_EXIT;

}

static sh_sand_man_t dnx_trap_prog_list_man = {
    .brief = "Show programmable traps status\n",
    .full = "Lists of Programmable traps info for every index, if enabled or disabled\n",
    .synopsis = "",
    .examples = "",
    .init_cb = diag_dnx_trap_prog_init_cb,
};

static sh_sand_man_t dnx_trap_prog_info_man = {
    .brief = "Show programmable trap info\n",
    .full = "Programmable trap configuration info per ID\n",
    .synopsis = "[id=0-7]",
    .examples = "id=4",
    .init_cb = diag_dnx_trap_prog_init_cb,
};

static sh_sand_option_t dnx_trap_prog_info_options[] = {
    {"id", SAL_FIELD_TYPE_INT32, "Programmable Trap id", NULL},
    {NULL}
};

/**
 * \brief
 *    A diagnostics function that prints the full information for a programmable
 *    trap per ID.
 * \param [in] unit         - unit ID
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [in] id           - Programmable trap ID(0-7)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_prog_info_index_print(
    int unit,
    sh_sand_control_t * sand_control,
    int id)
{
    bcm_rx_trap_prog_config_t prog_config;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_prog_config_t_init(&prog_config);
    SHR_IF_ERR_EXIT(bcm_rx_trap_prog_get(unit, id, &prog_config));

    PRT_TITLE_SET("Programmable Trap Index : %d", id);
    PRT_COLUMN_ADD("Field");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Mask/Nof Bits");
    PRT_COLUMN_ADD("Enable");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    if (prog_config.src_mac_enable != bcmRxTrapProgDisable)
    {
        PRT_CELL_SET("SRC_MAC");
        PRT_CELL_SET("%02X:%02X:%02X:%02X:%02X:%02X", prog_config.src_mac[0],
                     prog_config.src_mac[1],
                     prog_config.src_mac[2], prog_config.src_mac[3], prog_config.src_mac[4], prog_config.src_mac[5]);
        PRT_CELL_SET("%d(bits)", prog_config.src_mac_nof_bits);
        PRT_CELL_SET("%s", Trap_prog_enable_enum_table[prog_config.src_mac_enable].string);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    }
    if (prog_config.dest_mac_enable != bcmRxTrapProgDisable)
    {
        PRT_CELL_SET("DST_MAC");
        PRT_CELL_SET("%02X:%02X:%02X:%02X:%02X:%02X", prog_config.dest_mac[0],
                     prog_config.dest_mac[1],
                     prog_config.dest_mac[2],
                     prog_config.dest_mac[3], prog_config.dest_mac[4], prog_config.dest_mac[5]);
        PRT_CELL_SET("%d(bits)", prog_config.dest_mac_nof_bits);
        PRT_CELL_SET("%s", Trap_prog_enable_enum_table[prog_config.dest_mac_enable].string);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    }
    if (prog_config.ether_type_enable != bcmRxTrapProgDisable)
    {
        PRT_CELL_SET("ETHER_TYPE");
        PRT_CELL_SET("0x%x", prog_config.ether_type);
        PRT_CELL_SET("N/A");
        PRT_CELL_SET("%s", Trap_prog_enable_enum_table[prog_config.ether_type_enable].string);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    }
    if (prog_config.sub_type_enable != bcmRxTrapProgDisable)
    {
        PRT_CELL_SET("SUB_TYPE");
        PRT_CELL_SET("%d", prog_config.sub_type);
        PRT_CELL_SET("0x%x", prog_config.sub_type_mask);
        PRT_CELL_SET("%s", Trap_prog_enable_enum_table[prog_config.sub_type_enable].string);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    }
    if (prog_config.ip_protocol_enable != bcmRxTrapProgDisable)
    {
        PRT_CELL_SET("IP_PROTOCOL");
        PRT_CELL_SET("%d", prog_config.ip_protocol);
        PRT_CELL_SET("N/A");
        PRT_CELL_SET("%s", Trap_prog_enable_enum_table[prog_config.ip_protocol_enable].string);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    }
    if (prog_config.l4_ports_enable != bcmRxTrapProgDisable)
    {
        PRT_CELL_SET("SRC_PORT");
        PRT_CELL_SET("%d", prog_config.src_port);
        PRT_CELL_SET("0x%x", prog_config.src_port_mask);
        PRT_CELL_SET("%s", Trap_prog_enable_enum_table[prog_config.l4_ports_enable].string);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("DST_PORT");
        PRT_CELL_SET("%d", prog_config.dest_port);
        PRT_CELL_SET("0x%x", prog_config.dest_port_mask);
        PRT_CELL_SET("%s", Trap_prog_enable_enum_table[prog_config.l4_ports_enable].string);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    }
    if (prog_config.tcp_flags_enable != bcmRxTrapProgDisable)
    {
        PRT_CELL_SET("TCP_FLAGS");
        PRT_CELL_SET("%d", prog_config.tcp_flags);
        PRT_CELL_SET("0x%x", prog_config.tcp_flags_mask);
        PRT_CELL_SET("%s", Trap_prog_enable_enum_table[prog_config.tcp_flags_enable].string);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    }
    if (prog_config.tcp_seq_is_zero_enable != bcmRxTrapProgDisable)
    {
        PRT_CELL_SET("TCP_SEQ_IS_ZERO");
        PRT_CELL_SET("N/A");
        PRT_CELL_SET("N/A");
        PRT_CELL_SET("%s", Trap_prog_enable_enum_table[prog_config.tcp_seq_is_zero_enable].string);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    }
    PRT_CELL_SET("INGRESS_TRAP_ID");
    PRT_CELL_SET("0x%x", BCM_GPORT_TRAP_GET_ID(prog_config.trap_gport));
    PRT_CELL_SET("N/A");
    PRT_CELL_SET("N/A");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("TRAP_STRENGTH");
    PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_STRENGTH(prog_config.trap_gport));
    PRT_CELL_SET("N/A");
    PRT_CELL_SET("N/A");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("SNOOP_STRENGTH");
    PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(prog_config.trap_gport));
    PRT_CELL_SET("N/A");
    PRT_CELL_SET("N/A");
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a table of the enabled programmable traps,
 *    including the programmable IDm trap code, trap strength and snoop strength.
 * \param [in] unit - unit ID
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_prog_info_enabled_print(
    int unit,
    sh_sand_control_t * sand_control)
{
    int prog_index;
    bcm_rx_trap_prog_config_t prog_config;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Enabled Programmable Trap Table");
    PRT_COLUMN_ADD("Programmable Trap ID");
    PRT_COLUMN_ADD("Trap Code");
    PRT_COLUMN_ADD("Trap Strength");
    PRT_COLUMN_ADD("Snoop Strength");

    for (prog_index = 0; prog_index < dnx_data_trap.ingress.nof_programmable_traps_get(unit); prog_index++)
    {
        bcm_rx_trap_prog_config_t_init(&prog_config);
        SHR_IF_ERR_EXIT(bcm_rx_trap_prog_get(unit, prog_index, &prog_config));

        if (BCM_GPORT_TRAP_GET_STRENGTH(prog_config.trap_gport) > 0
            || BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(prog_config.trap_gport) > 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%d", prog_index);
            PRT_CELL_SET("0x%x", BCM_GPORT_TRAP_GET_ID(prog_config.trap_gport));
            PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_STRENGTH(prog_config.trap_gport));
            PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(prog_config.trap_gport));
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a table of disabled programmable trap IDs.
 * \param [in] unit - unit ID
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_prog_info_disabled_print(
    int unit,
    sh_sand_control_t * sand_control)
{
    int prog_index;
    bcm_rx_trap_prog_config_t prog_config;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Disabled Programmable Trap Table");
    PRT_COLUMN_ADD("Programmable Trap ID");

    for (prog_index = 0; prog_index < dnx_data_trap.ingress.nof_programmable_traps_get(unit); prog_index++)
    {
        bcm_rx_trap_prog_config_t_init(&prog_config);
        SHR_IF_ERR_EXIT(bcm_rx_trap_prog_get(unit, prog_index, &prog_config));

        if (BCM_GPORT_TRAP_GET_STRENGTH(prog_config.trap_gport) == 0
            && BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(prog_config.trap_gport) == 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%d", prog_index);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that calls the print function for
 *    programmable traps full info, per index.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_prog_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int id = -1;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("id", id);

    if (id >= 0 && id <= 7)
    {
        SHR_IF_ERR_EXIT(sh_dnx_trap_prog_info_index_print(unit, sand_control, id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The supplied id: %d, is out of range for programmable traps. Must be between 0-7!\n", id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that calls print functions
 *    for enabled and disabled programmable traps.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_prog_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_trap_prog_info_enabled_print(unit, sand_control));
    SHR_IF_ERR_EXIT(sh_dnx_trap_prog_info_disabled_print(unit, sand_control));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_trap_prog_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"LiST", sh_dnx_trap_prog_list_cmd, NULL, NULL, &dnx_trap_prog_list_man},
    {"info", sh_dnx_trap_prog_info_cmd, NULL, dnx_trap_prog_info_options, &dnx_trap_prog_info_man},
    {NULL}
};
