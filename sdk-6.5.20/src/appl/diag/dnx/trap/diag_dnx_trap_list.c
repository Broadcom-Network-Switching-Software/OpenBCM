/** \file diag_dnx_trap_list.c
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

static sh_sand_man_t dnx_trap_list_pd_man = {
    .brief = "List all predefined traps configured\n",
    .full =
        "List info includes name,action index(HW id),snoop and forward strengths, and is the configured predefined trap Strengthless /n",
    .synopsis = "[block=<IRPP | ETPP>]",
    .examples = "block=IRPP\n" "block=ETPP"
};

static sh_sand_man_t dnx_trap_list_ud_man = {
    .brief = "List number of free and used user-defined traps\n",
    .full = "List number of free and used user-defined traps per block and state.\n",
    .synopsis = "[block=<IRPP | ERPP | ETPP> state=<USED | FREE | ALL>]",
    .examples = "block=IRPP state=USED\n"
        "block=ERPP state=USED\n"
        "block=ETPP state=USED\n"
        "block=IRPP state=FREE\n"
        "block=ERPP state=FREE\n"
        "block=ETPP state=FREE\n" "block=IRPP state=ALL\n" "block=ERPP state=ALL\n" "block=ETPP state=ALL\n"
};

static sh_sand_man_t dnx_trap_list_appl_man = {
    .brief = "List of all application traps set on the device\n",
    .full =
        "List info for application traps currently set on the device. Info includes Action Profile, Trap_ID(Encoded), Forward and Snoop Strengths.\n",
    .synopsis = "[block=<ERPP | ETPP>]",
    .examples = "block=ERPP\n" "block=ETPP"
};

static sh_sand_option_t dnx_trap_list_pd_options[] = {
    {"BLock", SAL_FIELD_TYPE_ENUM, "Trap block", "ALL", (void *) Trap_block_enum_table},
    {NULL}
};

static sh_sand_enum_t sand_diag_trap_state[] = {
    /**String      Value      Description*/
    {"FREE", 0, "Free user defined profiles"},
    {"USED", 1, "Used user defined profiles"},
    {"ALL", 2, "All user defined profiles"},
    {NULL}
};

static sh_sand_option_t dnx_trap_list_ud_options[] = {
    {"BLock", SAL_FIELD_TYPE_ENUM, "Trap block", "ALL", (void *) Trap_block_enum_table},
    {"STaTe", SAL_FIELD_TYPE_ENUM, "Trap state(free/used/all)", "USED", (void *) sand_diag_trap_state},
    {NULL}
};

static sh_sand_option_t dnx_trap_list_appl_options[] = {
    {"BLock", SAL_FIELD_TYPE_ENUM, "Trap block", "ALL", (void *) Trap_block_enum_table},
    {NULL}
};

/**
 * \brief
 *
 *    A diagnostics function that prints a list of IRPP predefined traps configured.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_list_pd_irpp_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_map_type_t *trap_map_list, single_map_entry;
    int nof_traps, list_ind, trap_id;
    int trap_ids[255] = { 0 };
    uint8 is_allocated;
    dnx_rx_trap_class_e trap_class;
    bcm_rx_trap_config_t trap_config;
    char *trap_desc[bcmRxTrapCount];
    char *strengthless = "";
    int ind, ind2, ind_min, temp;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    trap_map_list = sal_alloc(bcmRxTrapCount * sizeof(dnx_rx_trap_map_type_t), "trap_map_list");

    SHR_IF_ERR_EXIT(dnx_rx_trap_map_list(unit, DNX_RX_TRAP_BLOCK_INGRESS, DNX_RX_TRAP_CLASS_PREDEFINED,
                                         trap_desc, &trap_map_list, &nof_traps));

    PRT_TITLE_SET("IRPP predefined traps configured");
    PRT_COLUMN_ADD("bcmRxTrap");
    PRT_COLUMN_ADD("Trap id (HW code)");
    PRT_COLUMN_ADD("Snoop strength");
    PRT_COLUMN_ADD("Forward strength");
    PRT_COLUMN_ADD("Strengtless");

    for (list_ind = 0; list_ind < nof_traps; ++list_ind)
    {
        trap_ids[list_ind] = trap_map_list[list_ind].trap_id;
    }

    for (ind = 0; ind < nof_traps; ++ind)
    {
        ind_min = ind;
        for (ind2 = ind + 1; ind2 < nof_traps; ind2++)
        {
            if (trap_ids[ind2] < trap_ids[ind_min])
            {
                ind_min = ind2;
            }
        }
        temp = trap_ids[ind_min];
        trap_ids[ind_min] = trap_ids[ind];
        trap_ids[ind] = temp;

    }

    for (list_ind = 0; list_ind < nof_traps; ++list_ind)
    {
        trap_id = trap_ids[list_ind];
        SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, trap_id, &is_allocated));
        if (is_allocated == TRUE)
        {
            for (ind2 = 0; ind2 < nof_traps; ++ind2)
            {
                if (trap_id == trap_map_list[ind2].trap_id)
                {
                    sal_memcpy(&single_map_entry, &trap_map_list[ind2], sizeof(dnx_rx_trap_map_type_t));
                }
            }

            if (single_map_entry.is_strengthless)
            {
                strengthless = "TRUE";
            }
            else
            {
                strengthless = "FALSE";
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", single_map_entry.trap_name);
            PRT_CELL_SET("0x%x", trap_id);

            SHR_IF_ERR_EXIT(dnx_rx_trap_ingress_class_get(unit, trap_id, &trap_class));

            if (trap_class == DNX_RX_TRAP_CLASS_PREDEFINED)
            {
                bcm_rx_trap_config_t_init(&trap_config);
                bcm_rx_trap_get(unit, trap_id, &trap_config);
                if (single_map_entry.is_strengthless)
                {
                    PRT_CELL_SET("N/A");
                    PRT_CELL_SET("N/A");
                }
                else
                {
                    PRT_CELL_SET("%d", trap_config.snoop_strength);
                    PRT_CELL_SET("%d", trap_config.trap_strength);
                }
                PRT_CELL_SET("%s", strengthless);
            }
            else
            {
                if (single_map_entry.is_strengthless)
                {
                    PRT_CELL_SET("N/A");
                    PRT_CELL_SET("N/A");
                }
                else
                {
                    PRT_CELL_SET("0");
                    PRT_CELL_SET("0");
                }
                PRT_CELL_SET("%s", strengthless);
            }
        }
    }

    PRT_COMMITX;

exit:
    sal_free(trap_map_list);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a list of ETPP predefined (OAM) traps configured.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_list_pd_etpp_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_map_type_t *trap_map_list;
    int nof_traps, list_ind, fwd_act_profile, trap_id;
    char *trap_desc[bcmRxTrapCount];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    trap_map_list = sal_alloc(bcmRxTrapCount * sizeof(dnx_rx_trap_map_type_t), "trap_map_list");

    SHR_IF_ERR_EXIT(dnx_rx_trap_map_list
                    (unit, DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM, trap_desc, &trap_map_list, &nof_traps));

    PRT_TITLE_SET("ETPP predefined traps configured");
    PRT_COLUMN_ADD("bcmRxTrap");
    PRT_COLUMN_ADD("Forward Action Profile");
    PRT_COLUMN_ADD("Trap id (Encoded)");

    for (list_ind = 0; list_ind < nof_traps; ++list_ind)
    {
        fwd_act_profile = DNX_RX_TRAP_ETPP_OAM_ACTION_PROFILE_GET(trap_map_list[list_ind].trap_id);
        trap_id = DNX_RX_TRAP_ID_SET(trap_map_list[list_ind].trap_block, trap_map_list[list_ind].trap_class,
                                     trap_map_list[list_ind].trap_id);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", trap_map_list[list_ind].trap_name);
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
 *    A diagnostics function that prints a list of predefined traps configured.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_list_pd_cmd(
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
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_pd_irpp_print(unit, args, sand_control));
            break;
        case DNX_RX_TRAP_BLOCK_ERPP:
            LOG_CLI_EX("No predefined traps for ERPP block%s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
            break;
        case DNX_RX_TRAP_BLOCK_ETPP:
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_pd_etpp_print(unit, args, sand_control));
            break;
        case DNX_RX_TRAP_BLOCK_NUM_OF:
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_pd_irpp_print(unit, args, sand_control));
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_pd_etpp_print(unit, args, sand_control));
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid input %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     A diagnostics function that shows ingress user-defined trap info per block and state.
 * \param [in] unit - unit ID
 * \param [in] state - Free/Used/All 
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_list_ud_irpp_print(
    int unit,
    int state,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    int trap_id, alloc_index = 0;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    switch (state)
    {
        case 0:
            PRT_TITLE_SET("IRPP User-defined traps available");
            PRT_COLUMN_ADD("USER_DEFINED_#");
            PRT_COLUMN_ADD("Trap id (HW id)");
            break;
        case 1:
            PRT_TITLE_SET("IRPP User-defined traps configured");
            PRT_COLUMN_ADD("USER_DEFINED_#");
            PRT_COLUMN_ADD("Trap id (HW id)");
            break;
        case 2:
            PRT_TITLE_SET("IRPP User-defined traps");
            PRT_COLUMN_ADD("USER_DEFINED_#");
            PRT_COLUMN_ADD("Trap id (HW id)");
            PRT_COLUMN_ADD("USED/FREE");
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid input %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    while (dnx_rx_trap_map_ingress_ud_alloc[alloc_index] != DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES)
    {
        trap_id = dnx_rx_trap_map_ingress_ud_alloc[alloc_index];
        SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, trap_id, &is_allocated));
        if (state == 2)
        {
            /** Show both used and free ud traps (state == all) */
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("USER_DEFINED_%d", alloc_index);
            PRT_CELL_SET("0x%x", trap_id);
            PRT_CELL_SET("%s", sand_diag_trap_state[is_allocated].string);
        }
        else if (is_allocated == state)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("USER_DEFINED_%d", alloc_index);
            PRT_CELL_SET("0x%x", trap_id);
        }

        alloc_index++;
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that shows egress user-defined trap info per block and state.
 * \param [in] unit - unit ID
 * \param [in] state - Free/Used/All
 * \param [in] trap_block - trap block (ERPP/ETPP)
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_list_ud_egress_print(
    int unit,
    int state,
    dnx_rx_trap_block_e trap_block,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    char trap_id_str[DBAL_MAX_STRING_LENGTH] = "";
    int encoded_trap_id, trap_id, trap_id_index = 0;
    dnx_rx_trap_class_e trap_class = DNX_RX_TRAP_CLASS_USER_DEFINED;
    dnx_rx_trap_map_ud_t ud_block_info;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&ud_block_info, 0, sizeof(dnx_rx_trap_map_ud_t));
    SHR_IF_ERR_EXIT(dnx_rx_trap_user_defined_block_map_info_get(unit, trap_block, &ud_block_info));

    trap_id = ud_block_info.default_trap_id_list[0];

    switch (state)
    {
        case 0:
            PRT_TITLE_SET("%s User-defined traps available", ud_block_info.block_name);
            PRT_COLUMN_ADD("Trap id");
            PRT_COLUMN_ADD("Forward Action Profile");
            break;
        case 1:
            PRT_TITLE_SET("%s User-defined traps configured", ud_block_info.block_name);
            PRT_COLUMN_ADD("Trap id");
            PRT_COLUMN_ADD("Forward Action Profile");
            while (trap_id != -1)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                dnx_egress_trap_id_to_string(trap_id, trap_id_str);
                PRT_CELL_SET("%s", trap_id_str);
                PRT_CELL_SET("%d", DNX_RX_TRAP_ID_TYPE_GET(trap_id));
                trap_id = ud_block_info.default_trap_id_list[++trap_id_index];
            }
            break;
        case 2:
            PRT_TITLE_SET("%s User-defined traps", ud_block_info.block_name);
            PRT_COLUMN_ADD("Trap id");
            PRT_COLUMN_ADD("Forward Action Profile");
            PRT_COLUMN_ADD("USED/FREE");
            while (trap_id != -1)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                dnx_egress_trap_id_to_string(trap_id, trap_id_str);
                PRT_CELL_SET("%s", trap_id_str);
                PRT_CELL_SET("%d", DNX_RX_TRAP_ID_TYPE_GET(trap_id));
                PRT_CELL_SET("USED");
                trap_id = ud_block_info.default_trap_id_list[++trap_id_index];
            }
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid input %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    for (trap_id = ud_block_info.lower_bound_trap_id; trap_id <= ud_block_info.upper_bound_trap_id; trap_id++)
    {
        encoded_trap_id = DNX_RX_TRAP_ID_SET(trap_block, trap_class, trap_id);
        SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, encoded_trap_id, &is_allocated));
        if (state == 2)
        {
            /** Show both used and free ud traps (state == all) */
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("0x%x", encoded_trap_id);
            PRT_CELL_SET("%d", trap_id);
            PRT_CELL_SET("%s", sand_diag_trap_state[is_allocated].string);
        }
        else if (is_allocated == state)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("0x%x", encoded_trap_id);
            PRT_CELL_SET("%d", trap_id);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints configuration of a userdefined trap OR list of traps .
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_list_ud_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_block_e trap_block;
    int state;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("block", trap_block);
    SH_SAND_GET_ENUM("state", state);

    switch (trap_block)
    {
        case DNX_RX_TRAP_BLOCK_INGRESS:
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_ud_irpp_print(unit, state, sand_control));
            break;
        case DNX_RX_TRAP_BLOCK_ERPP:
        case DNX_RX_TRAP_BLOCK_ETPP:
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_ud_egress_print(unit, state, trap_block, sand_control));
            break;
        case DNX_RX_TRAP_BLOCK_NUM_OF:
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_ud_irpp_print(unit, state, sand_control));
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_ud_egress_print(unit, state, DNX_RX_TRAP_BLOCK_ERPP, sand_control));
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_ud_egress_print(unit, state, DNX_RX_TRAP_BLOCK_ETPP, sand_control));
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid input %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a list of application traps per egress block.
 * \param [in] unit - unit ID
 * \param [in] trap_block - trap block (ERPP/ETPP)
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_list_appl_print(
    int unit,
    dnx_rx_trap_block_e trap_block,
    sh_sand_control_t * sand_control)
{
    dnx_rx_trap_map_type_t *trap_map_list;
    int nof_traps, list_ind, trap_id;
    bcm_gport_t trap_gport_empty, trap_gport;
    bcm_rx_trap_t trap_type;
    char *trap_desc[bcmRxTrapCount];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    trap_map_list = sal_alloc(bcmRxTrapCount * sizeof(dnx_rx_trap_map_type_t), "trap_map_list");

    SHR_IF_ERR_EXIT(dnx_rx_trap_app_map_list(unit, trap_block, trap_desc, &trap_map_list, &nof_traps));

    BCM_GPORT_TRAP_SET(trap_gport_empty, 0, 0, 0);

    PRT_TITLE_SET("%s application traps configured", Trap_block_enum_table[trap_block - 1].string);
    PRT_COLUMN_ADD("bcmRxTrap");
    PRT_COLUMN_ADD("Forward action profile");
    PRT_COLUMN_ADD("Trap id (Encoded)");
    PRT_COLUMN_ADD("Forward strength");
    PRT_COLUMN_ADD("Snoop strength");

    for (list_ind = 0; list_ind < nof_traps; list_ind++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_app_type_from_id_get(unit, trap_map_list[list_ind].trap_block,
                                                         trap_map_list[list_ind].trap_id, &trap_type));

        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, trap_type, &trap_gport));

        if (trap_gport != trap_gport_empty)
        {
            trap_id = BCM_GPORT_TRAP_GET_ID(trap_gport);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", trap_map_list[list_ind].trap_name);
            PRT_CELL_SET("0x%x", DNX_RX_TRAP_EGRESS_ACTION_PROFILE_GET(trap_id));
            PRT_CELL_SET("0x%x", trap_id);
            PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_STRENGTH(trap_gport));
            PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport));
        }

    }

    PRT_COMMITX;

exit:
    sal_free(trap_map_list);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints configuration of a userdefined trap OR list of traps .
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_list_appl_cmd(
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
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_appl_print(unit, trap_block, sand_control));
            break;
        case DNX_RX_TRAP_BLOCK_NUM_OF:
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_appl_print(unit, DNX_RX_TRAP_BLOCK_ERPP, sand_control));
            SHR_IF_ERR_EXIT(sh_dnx_trap_list_appl_print(unit, DNX_RX_TRAP_BLOCK_ETPP, sand_control));
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid input %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_trap_list_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"PRE", sh_dnx_trap_list_pd_cmd, NULL, dnx_trap_list_pd_options, &dnx_trap_list_pd_man}
    ,
    {"USeR", sh_dnx_trap_list_ud_cmd, NULL, dnx_trap_list_ud_options, &dnx_trap_list_ud_man}
    ,
    {"APPLication", sh_dnx_trap_list_appl_cmd, NULL, dnx_trap_list_appl_options, &dnx_trap_list_appl_man}
    ,
    {NULL}
};
