/** \file diag_dnx_trap_map.c
 *
 * file for trap list diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "include/bcm_int/dnx/rx/rx_trap.h"
#include "include/bcm_int/dnx/rx/rx_trap_map.h"
#include "diag_dnx_trap.h"

/*
 * }
 */

static sh_sand_man_t dnx_trap_map_pd_man = {
    .brief = "Show mapping of all available predefined traps\n",
    .full = "Map info includes name, description and action index(HW id) of each available predefined traps\n",
    .synopsis = "[block=<IRPP | ETPP>]",
    .examples = "block=IRPP\n" "block=ETPP"
};

static sh_sand_man_t dnx_trap_map_appl_man = {
    .brief = "Show mapping of all available application traps\n",
    .full = "Map info includes name and description of each available predefined traps\n",
    .synopsis = "[block=<ERPP | ETPP>]",
    .examples = "block=ERPP\n" "block=ETPP"
};

static sh_sand_option_t dnx_trap_map_pd_options[] = {
    {"BLock", SAL_FIELD_TYPE_ENUM, "Trap block", "ALL", (void *) Trap_block_enum_table},
    {NULL}
};

static sh_sand_option_t dnx_trap_map_appl_options[] = {
    {"BLock", SAL_FIELD_TYPE_ENUM, "Trap block", "ALL", (void *) Trap_block_enum_table},
    {NULL}
};

/**
 * \brief
 *    A diagnostics function that prints map info of available IRPP predefined traps.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_map_pd_irpp_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_map_type_t *trap_map_list;
    int nof_traps, list_ind;
    char *description[bcmRxTrapCount];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    trap_map_list = sal_alloc(bcmRxTrapCount * sizeof(dnx_rx_trap_map_type_t), "trap_map_list");

    SHR_IF_ERR_EXIT(dnx_rx_trap_map_list(unit, DNX_RX_TRAP_BLOCK_INGRESS, DNX_RX_TRAP_CLASS_PREDEFINED,
                                         description, &trap_map_list, &nof_traps));

    PRT_TITLE_SET("IRPP predefined traps Map");
    PRT_COLUMN_ADD("bcmRxTrap");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    PRT_COLUMN_ADD("Trap id (HW code)");

    for (list_ind = 0; list_ind < nof_traps; ++list_ind)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", trap_map_list[list_ind].trap_name);
        PRT_CELL_SET("%s", description[list_ind]);
        PRT_CELL_SET("0x%x", trap_map_list[list_ind].trap_id);
    }

    PRT_COMMITX;

exit:
    sal_free(trap_map_list);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints map info of available ETPP predefined (OAM) traps.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_map_pd_etpp_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_map_type_t *trap_map_list;
    int nof_traps, list_ind, fwd_act_profile, trap_id;
    char *description[bcmRxTrapCount];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    trap_map_list = sal_alloc(bcmRxTrapCount * sizeof(dnx_rx_trap_map_type_t), "trap_map_list");

    SHR_IF_ERR_EXIT(dnx_rx_trap_map_list(unit, DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM,
                                         description, &trap_map_list, &nof_traps));

    PRT_TITLE_SET("ETPP predefined traps Map");
    PRT_COLUMN_ADD("bcmRxTrap");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    PRT_COLUMN_ADD("Forward Action Profile");
    PRT_COLUMN_ADD("Trap id (Encoded)");

    for (list_ind = 0; list_ind < nof_traps; ++list_ind)
    {
        fwd_act_profile = DNX_RX_TRAP_ETPP_OAM_ACTION_PROFILE_GET(trap_map_list[list_ind].trap_id);
        trap_id = DNX_RX_TRAP_ID_SET(trap_map_list[list_ind].trap_block, trap_map_list[list_ind].trap_class,
                                     trap_map_list[list_ind].trap_id);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", trap_map_list[list_ind].trap_name);
        PRT_CELL_SET("%s", description[list_ind]);
        PRT_CELL_SET("0x%x", fwd_act_profile);
        PRT_CELL_SET("0x%x", trap_id);
    }

    PRT_COMMITX;

exit:
    sal_free(trap_map_list);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints map info of available predefined traps.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_map_pd_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_block_e trap_block;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("block", trap_block);

    switch (trap_block)
    {
        case DNX_RX_TRAP_BLOCK_INGRESS:
            SHR_IF_ERR_EXIT(sh_dnx_trap_map_pd_irpp_print(unit, args, sand_control));
            break;
        case DNX_RX_TRAP_BLOCK_ERPP:
            LOG_CLI_EX("No predefined traps for ERPP block%s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
            break;
        case DNX_RX_TRAP_BLOCK_ETPP:
            SHR_IF_ERR_EXIT(sh_dnx_trap_map_pd_etpp_print(unit, args, sand_control));
            break;
        case DNX_RX_TRAP_BLOCK_NUM_OF:
            SHR_IF_ERR_EXIT(sh_dnx_trap_map_pd_irpp_print(unit, args, sand_control));
            SHR_IF_ERR_EXIT(sh_dnx_trap_map_pd_etpp_print(unit, args, sand_control));
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid input %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints map info of available application traps per egress block.
 * \param [in] unit - unit ID
 * \param [in] trap_block - trap block (ERPP/ETPP)
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_map_appl_print(
    int unit,
    dnx_rx_trap_block_e trap_block,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_map_type_t *trap_map_list;
    int nof_traps, list_ind;
    char *description[bcmRxTrapCount];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    trap_map_list = sal_alloc(bcmRxTrapCount * sizeof(dnx_rx_trap_map_type_t), "trap_map_list");

    SHR_IF_ERR_EXIT(dnx_rx_trap_app_map_list(unit, trap_block, description, &trap_map_list, &nof_traps));

    PRT_TITLE_SET("%s application traps Map", Trap_block_enum_table[trap_block - 1].string);
    PRT_COLUMN_ADD("bcmRxTrap");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");

    for (list_ind = 0; list_ind < nof_traps; list_ind++)
    {

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", trap_map_list[list_ind].trap_name);
        PRT_CELL_SET("%s", description[list_ind]);
    }

    PRT_COMMITX;

exit:
    sal_free(trap_map_list);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints map info of application traps.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_map_appl_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_block_e trap_block;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("block", trap_block);

    switch (trap_block)
    {
        case DNX_RX_TRAP_BLOCK_INGRESS:
            LOG_CLI_EX("No application traps for IRPP block%s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
            break;
        case DNX_RX_TRAP_BLOCK_ERPP:
        case DNX_RX_TRAP_BLOCK_ETPP:
            SHR_IF_ERR_EXIT(sh_dnx_trap_map_appl_print(unit, trap_block, sand_control));
            break;
        case DNX_RX_TRAP_BLOCK_NUM_OF:
            SHR_IF_ERR_EXIT(sh_dnx_trap_map_appl_print(unit, DNX_RX_TRAP_BLOCK_ERPP, sand_control));
            SHR_IF_ERR_EXIT(sh_dnx_trap_map_appl_print(unit, DNX_RX_TRAP_BLOCK_ETPP, sand_control));
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid input %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_trap_map_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"PRE", sh_dnx_trap_map_pd_cmd, NULL, dnx_trap_map_pd_options, &dnx_trap_map_pd_man}
    ,
    {"APPLication", sh_dnx_trap_map_appl_cmd, NULL, dnx_trap_map_appl_options, &dnx_trap_map_appl_man}
    ,
    {NULL}
};
