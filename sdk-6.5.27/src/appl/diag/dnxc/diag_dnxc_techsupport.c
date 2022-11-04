/** \file diag_dnxc_techsupport.c
 *
 * Diagnostic techsupport
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

/* shared */
#include <shared/bsl.h>
/* appl */
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
/*soc*/

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
/*dnx data*/
#include <soc/dnx/dnx_data/auto_generated/dnx_data_techsupport.h>
#endif

#ifdef BCM_DNXF_SUPPORT
/* The list commands exucuted as part of "techsupport tm" for ramon */
char *techsupport_tm_dnxf_cmdlist[] = {
    "fabric queues",
    "fabric connectivity",
    NULL        /* Must be the last element in this structure */
};
#endif

/** Length of delimiter print buffer */
#define DELIMITER_MAX_STR_LEN             80
#define COMMAND_MAX_STR_LEN               100
#define ECC_MAX_BIT_NUM                   2

extern int diag_sand_reg_get_all(
    int unit,
    int is_debug,
    char *name);

#ifdef BCM_DNX_SUPPORT
static reg_val ecc_state[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][ECC_MAX_BIT_NUM];
#endif

static void
clear_all_interrupts(
    int unit)
{
    bcm_switch_event_control_t event;
    int rv;

    cli_out("Clear all interrupts ... \n");
    event.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    event.index = BCM_CORE_ALL;
    event.action = bcmSwitchEventClear;
    rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 1);
    if (BCM_FAILURE(rv))
    {
        cli_out("Clear all interrupts FAILED \n");
    }

    return;
}

static void
techsupport_sh_command(
    int unit,
    char *command)
{
    int diff = 0, diff1 = 0, j = 0;
    char str[DELIMITER_MAX_STR_LEN] = { 0 };
    char *pattern1 = "Output of \"";
    char *pattern2 = "\" start";
    char *pattern3 = "\" end";
    /*
     * Before actually dumping the command output, head string is
     * printed. Example of head string is something like below.
     * >>>>>>>>>>>>>>>>>>>>>>Output of "config show" start>>>>>>>>>>>>>>>>>>>
     * The below logic froms the head string.
     */
    sal_memset(str, 0, DELIMITER_MAX_STR_LEN);
    diff =
        DELIMITER_MAX_STR_LEN - (sal_strnlen(command, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) +
                                 sal_strnlen(pattern1,
                                             SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + sal_strnlen(pattern2,
                                                                                                         SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH));
    diff = (diff % 2) == 0 ? diff : (diff + 1);

    for (j = 0; j < (diff / 2); j++)
    {
        str[j] = '>';
    }

    cli_out("\n%s%s%s%s%s\n\n", str, pattern1, command, pattern2, str);

    sh_process_command(unit, command);

    /*
     * After dumping the command output, tail string is
     * printed. Example of tail string is something like below.
     * <<<<<<<<<<<<<<<<<<<<<<Output of "config show" end<<<<<<<<<<<<<<<<<<<<<<<
     * The below logic froms the tail string.
     */

    sal_memset(str, 0, DELIMITER_MAX_STR_LEN);
    diff1 =
        DELIMITER_MAX_STR_LEN - (sal_strnlen(command, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) +
                                 sal_strnlen(pattern1,
                                             SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + sal_strnlen(pattern3,
                                                                                                         SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH));
    diff1 = (diff1 % 2) == 0 ? diff1 : (diff1 + 1);

    for (j = 0; j < (diff1 / 2); j++)
    {
        str[j] = '<';
    }

    cli_out("\n%s%s%s%s%s\n", str, pattern1, command, pattern3, str);

}

/*EID#8021*/
#ifdef BCM_DNX_SUPPORT
static void
disable_ecc_protection(
    int unit,
    char *block,
    uint32 words32b)
{
    char str_reg[COMMAND_MAX_STR_LEN];
    reg_val mask_all;
    int bit, core;

    /*
     * register structure:
     * <BLOCK>_ECC_ERR_<1/2>B_MONITOR_MEM_MASK.<BLOCK><CORE>
     */
    SOC_REG_ABOVE_64_CLEAR(mask_all);
    sal_memset(ecc_state, 0, sizeof(ecc_state));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        for (bit = 0; bit < ECC_MAX_BIT_NUM; bit++)
        {
            sal_snprintf(str_reg, COMMAND_MAX_STR_LEN, "%s_ECC_ERR_%dB_MONITOR_MEM_MASK.%s%d", block, bit + 1, block,
                         core);
            if (diag_reg_get(unit, str_reg, ecc_state[core][bit]) != BCM_E_NONE)
            {
                cli_out("\nGet %s value failed, \n", str_reg);
            }
            if (diag_reg_set(unit, str_reg, mask_all) != BCM_E_NONE)
            {
                cli_out("\nSet %s value failed \n", str_reg);
            }
        }
    }
}

static void
restore_ecc_protection(
    int unit,
    char *block)
{
    char str_reg[COMMAND_MAX_STR_LEN];
    int core, bit;

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        for (bit = 0; bit < ECC_MAX_BIT_NUM; bit++)
        {
            sal_snprintf(str_reg, COMMAND_MAX_STR_LEN, "%s_ECC_ERR_%dB_MONITOR_MEM_MASK.%s%d", block, bit + 1, block,
                         core);
            if (diag_reg_set(unit, str_reg, ecc_state[core][bit]) != BCM_E_NONE)
            {
                cli_out("\nSet %s value failed \n", str_reg);
            }
        }
    }
}
#endif

/*
 * techsupport init command functions
 */
shr_error_e
techsupport_init_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    (void) clear_all_interrupts(unit);

    diag_sand_reg_get_all(unit, 1, NULL);

    SHR_FUNC_EXIT;
}

/*
 * techsupport init command functions
 */
shr_error_e
techsupport_tm_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int index;
    char sh_command[COMMAND_MAX_STR_LEN];
#ifdef BCM_DNX_SUPPORT
    int block_index;
    char *table_name, *block_name;
    char *tm_command;
    int is_mask_ecc, ecc_mask_words;
    int num_tables, num_tm_command;
#endif

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Step 1: g * debug
     */

    diag_sand_reg_get_all(unit, 1, NULL);

    /*
     * Step 2: dump tm tables
     */
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        num_tables = dnx_data_techsupport.tm.num_of_tables_per_block_get(unit);
        for (block_index = 0; block_index < dnx_techsupport_block_count; block_index++)
        {
            is_mask_ecc = dnx_data_techsupport.tm.tm_block_get(unit, block_index)->is_mask_ecc;
            block_name = dnx_data_techsupport.tm.tm_block_get(unit, block_index)->block_name;
            if (is_mask_ecc)
            {
                ecc_mask_words = dnx_data_techsupport.tm.tm_block_get(unit, block_index)->ecc_mask_words;
                (void) disable_ecc_protection(unit, block_name, ecc_mask_words);
            }

            for (index = 0; index < num_tables; index++)
            {
                table_name = dnx_data_techsupport.tm.tm_mem_get(unit, block_index, index)->name;
                if (table_name != NULL)
                {
                    sal_snprintf(sh_command, COMMAND_MAX_STR_LEN, "dump raw %s", table_name);
                    cli_out("\n sh_command =%s \n", sh_command);
                    techsupport_sh_command(unit, sh_command);
                }
            }

            if (is_mask_ecc)
            {
                (void) restore_ecc_protection(unit, block_name);
            }
        }
    }
#endif

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        sal_snprintf(sh_command, COMMAND_MAX_STR_LEN, "dump raw RTP_RMHMT");
        techsupport_sh_command(unit, sh_command);
    }
#endif
    /*
     * Step 3: diag counter g
     */
    sal_snprintf(sh_command, COMMAND_MAX_STR_LEN, "diag counter g");
    techsupport_sh_command(unit, sh_command);

    /*
     * Step 4: dump tm command
     */
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        num_tm_command = dnx_data_techsupport.tm.num_of_tm_command_get(unit);
        for (index = 0; index < num_tm_command; index++)
        {
            tm_command = dnx_data_techsupport.tm.tm_command_get(unit, index)->command;

            if (tm_command != NULL)
            {
                sal_snprintf(sh_command, COMMAND_MAX_STR_LEN, "%s", tm_command);
                cli_out("\n sh_command = %s \n", sh_command);
                techsupport_sh_command(unit, sh_command);
            }
        }
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        for (index = 0; techsupport_tm_dnxf_cmdlist[index] != NULL; index++)
        {
            techsupport_sh_command(unit, techsupport_tm_dnxf_cmdlist[index]);
        }
    }
#endif

    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_man_t sh_dnxc_techsupport_man = {
    .brief = "Techsupport.",
    .full = "Techsupport.",
    .synopsis = NULL,   /* NULL means autogenerated */
    .examples = "init\n" "tm"
};

/**
 * \brief - Man page for 'bier mode' shell command
 */
static sh_sand_man_t techsupport_init_man = {
    .brief = "techsupport init",
};

/**
 * \brief - Man page for 'bier mode' shell command
 */
static sh_sand_man_t techsupport_tm_man = {
    .brief = "techsupport tm",
};

sh_sand_cmd_t sh_dnxc_techsupport_cmds[] = {
    /*
     * Name | Leaf Action | Node | Options for Leaf | Usage 
     */
    {"init", techsupport_init_cmd, NULL, NULL, &techsupport_init_man},
    {"tm",   techsupport_tm_cmd,   NULL, NULL, &techsupport_tm_man},
    {NULL}
};
/* *INDENT-ON* */
