/**
 * \file diag_dnx_lsm.c
 *
 * DNX lsm DIAG PACK - diagnostic pack for module lsm
 *
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LSM

/*
 * INCLUDE FILES:
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/appl/sal.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/bslenable.h>
#include <shared/bslnames.h>

#include "diag_dnx_lsm.h"
#include <appl/reference/dnx/appl_ref_lsm.h>
/*
 * }
 */

/*
 * cmd implementation begin {
 */

/**
 * \brief
 *   This function initializes the lsm diagnostic tool
 *
 * \param [in] unit - The unit number.
 * \param [in] args - Command name.
 * \param [in] sand_control - Control structure for shell command arguments.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
cmd_dnx_lsm_init(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    lsm_init_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_INT32("Port", init_info.port);
    SH_SAND_GET_STR("LOGpath", init_info.log_dir);
    SH_SAND_GET_STR("XMLpath", init_info.xml_dir);
    SH_SAND_GET_BOOL("NO_Log", init_info.disable_log);
    SH_SAND_GET_BOOL("NO_RX_CallBack", init_info.disable_rx_cb);
    SH_SAND_GET_BOOL("NO_INTRrupt_CallBck", init_info.disable_interrupt_callback);

    SHR_IF_ERR_EXIT(appl_ref_lsm_init(unit, &init_info));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function converts string format from user to HEX format
 *
 * \param [in] unit - The unit number.
 * \param [in] packet_string - packet in string format from user
 * \param [out] packet_length - packet length
 * \param [out] packet_data - packet in HEX format. assumed to be zeroed by caller(max size MAX_PKT_HDR_SIZE)
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
cmd_dnx_lsm_event_parse_payload(
    int unit,
    char *packet_string,
    uint32 *packet_length,
    uint8 *packet_data)
{
    int data_len, data_base;
    char tmp, data_iter;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(packet_string, _SHR_E_PARAM, "packet_string");
    SHR_NULL_CHECK(packet_data, _SHR_E_PARAM, "packet_data");

    /*
     * If string data starts with 0x or 0X, skip it
     */
    if ((packet_string[0] == '0') && ((packet_string[1] == 'x') || (packet_string[1] == 'X')))
    {
        data_base = 2;
    }
    else
    {
        data_base = 0;
    }
    data_len = sal_strnlen(packet_string, MAX_PKT_HDR_SIZE) - data_base;
    *packet_length = data_len / 2;
    /*
     * Convert char to value
     */
    for (int i = 0; i < data_len; i++)
    {
        data_iter = packet_string[data_base + i];
        if (('0' <= data_iter) && (data_iter <= '9'))
        {
            tmp = data_iter - '0';
        }
        else if (('a' <= data_iter) && (data_iter <= 'f'))
        {
            tmp = data_iter - 'a' + 10;
        }
        else if (('A' <= data_iter) && (data_iter <= 'F'))
        {
            tmp = data_iter - 'A' + 10;
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Unexpected char: %c\n", data_iter);
        }

        /*
         * String input is in 4b unit. Below we're filling in 8b:
         * offset is /2, and we shift by 4b if the input char is even
         * for 0xab
         *  i=0, tmp=a
         *  packet_data[0 / 2] |= a0
         *   i=1, tmp=b
         *  packet_data[1 / 2] |= 0b => ab
         *
         */
        packet_data[i / 2] |= tmp << (((i + 1) % 2) * 4);
    }

exit:

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function simulates an event to triger the lsm diagnostic tool
 *
 * \param [in] unit - The unit number.
 * \param [in] args - Command name.
 * \param [in] sand_control - Control structure for shell command arguments.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
cmd_dnx_lsm_event(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *packet_string;
    uint8 *packet_data = NULL;
    uint32 length;
    lsm_event_info_t info = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    info.event = lsm_event_packet;
    /** Packet allocation according to max size. If allocation is succesful free in end of function */
    SHR_IF_ERR_EXIT(bcm_pkt_alloc(unit, MAX_PKT_HDR_SIZE, 0, &info.pkt));
    sal_memset(info.pkt->pkt_data, 0x0, sizeof(bcm_pkt_blk_t));

    SHR_ALLOC_ERR_EXIT(packet_data, MAX_PKT_HDR_SIZE, "No memory for packet data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(packet_data, 0, MAX_PKT_HDR_SIZE);

    SH_SAND_GET_STR("DaTa", packet_string);
    SHR_IF_ERR_EXIT(cmd_dnx_lsm_event_parse_payload(unit, packet_string, &length, packet_data));
    info.pkt->pkt_data->len = length;
    SH_SAND_GET_INT32("PtchSRCport", info.pkt->src_port);
    info.pkt->pkt_data->data = packet_data;
    SHR_IF_ERR_EXIT(appl_ref_lsm_run(unit, &info));

exit:
    SHR_FREE(packet_data);
    if (info.pkt)
    {
        bcm_pkt_free(unit, info.pkt);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function deinitializes the lsm diagnostic tool
 *
 * \param [in] unit - The unit number.
 * \param [in] args - Command name.
 * \param [in] sand_control - Control structure for shell command arguments.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
cmd_dnx_lsm_deinit(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_ref_lsm_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays configured qulifiers in the lsm diagnostic tool
 *
 * \param [in] unit - The unit number.
 * \param [in] args - Command name.
 * \param [in] sand_control - Control structure for shell command arguments.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
cmd_dnx_lsm_show_qualifiers(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    appl_ref_lsm_show_qualifiers(unit);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays configured actions in the lsm diagnostic tool
 *
 * \param [in] unit - The unit number.
 * \param [in] args - Command name.
 * \param [in] sand_control - Control structure for shell command arguments.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
cmd_dnx_lsm_show_actions(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 action_table_id;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32("ID", action_table_id);

    SHR_IF_ERR_EXIT(appl_ref_lsm_show_actions(unit, action_table_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays configured traps in the lsm diagnostic tool
 *
 * \param [in] unit - The unit number.
 * \param [in] args - Command name.
 * \param [in] sand_control - Control structure for shell command arguments.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
cmd_dnx_lsm_show_traps(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_ref_lsm_show_traps(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays configured interrupts in the lsm diagnostic tool
 *
 * \param [in] unit - The unit number.
 * \param [in] args - Command name. dummy for future use
 * \param [in] sand_control - Control structure for shell command arguments. dummy for future use
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
cmd_dnx_lsm_show_interrupts(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_ref_lsm_show_interrupts(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * cmd implementation end }
 */

/*
 * cmd definition begin {
 */
/**
 * DNX lsm diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* {*/

/*
 * lsm cmds definition begin {
 */

static sh_sand_man_t Dnx_lsm_init_man = {
    .brief    = "initialize the lsm debug application. This application resends dropped packets and collects information on packet. The init configuers visability mode on all ports to be on and unforced",
    .full      = NULL,
    .synopsis = "[Port=<CPU_port>] [LOGPath=<directory to place logs>] [XMLPath=<directory to load xmls froms>] [NO_LOG=<disable log>] [NO_RX_CallBack=<if specified disables RX CB>] [NO_INTRrupt_CallBck=<disable interrupt callback>]",
    .examples = "Port=0\n LOGPath=my_logs/\n XMLPath=jericho2_a0/lsm/\n NO_LOG\n NO_RX_CallBack\n NO_INTRrupt_CallBck\n",
    .init_cb = appl_ref_lsm_void_init,
    .deinit_cb = appl_ref_lsm_deinit
};

static sh_sand_man_t Dnx_lsm_event_man = {
    .brief    = "simulate a packet event after initializing the application. this will send the packet and collect information",
    .full      = NULL,
    .synopsis = "[PtchSRCport=<CPU_port>] [DaTa=<packet hex>]",
    .examples = "PtchSRCport=0\n PtchSRCport=0 DaTa=001122334455667788445cfcfcfcfccfc\n",
};

static sh_sand_man_t Dnx_lsm_deinit_man = {
    .brief    = "deinit the application. this should restore all confiuration done by init",
    .full      = NULL
};

static sh_sand_man_t Dnx_lsm_show_man = {
    .brief    = "diagnostic tools to print app related information.",
    .full      = NULL
};

static sh_sand_man_t Dnx_lsm_show_qual_man = {
    .brief    = "show all configured qualifiers.",
    .full      = NULL
};

static sh_sand_man_t Dnx_lsm_show_act_man = {
    .brief    = "show actions configured for a specific acion table.",
    .full      = NULL,
    .synopsis = "[ID=<action_table_id>]",
    .examples = "ID=0\n",
    .init_cb = appl_ref_lsm_void_init,
    .deinit_cb = appl_ref_lsm_deinit
};

static sh_sand_man_t Dnx_lsm_show_trap_man = {
    .brief    = "Show all traps registered by application.",
    .full      = NULL,
    .init_cb = appl_ref_lsm_void_init,
    .deinit_cb = appl_ref_lsm_deinit
};

static sh_sand_man_t Dnx_lsm_show_interrupt_man = {
    .brief    = "Show all interrupts registered by application.",
    .full      = NULL,
    .init_cb = appl_ref_lsm_void_init,
    .deinit_cb = appl_ref_lsm_deinit
};

static sh_sand_option_t Dnx_lsm_event_options[] = {
        {"PtchSRCport", SAL_FIELD_TYPE_INT32, "Logical port to set IN PP PORT", "0"},
        {"DaTa", SAL_FIELD_TYPE_STR, "Packet value (Hex)", "001122334455667788774455"},
        {NULL}
};

static sh_sand_option_t Dnx_lsm_init_options[] = {
        {"Port", SAL_FIELD_TYPE_INT32, "CPU port for lsm usage", "0"},
        {"LOGpath", SAL_FIELD_TYPE_STR, "full path to log directoy", "lsm_log"},
        {"XMLpath", SAL_FIELD_TYPE_STR, "relative path to directoy containing action.xml/qualifier.xml. Default xml path if not set will be detirmined according to chip running the app.", ""},
        {"NO_Log", SAL_FIELD_TYPE_BOOL, "Option to initialize tool without LOG enabled(default is with, log to file, enabled", "false"},
        {"NO_RX_CallBack", SAL_FIELD_TYPE_BOOL, "Option to initialize tool without rx callback enabled(default is with, enable RX callback, enabled", "false"},
        {"NO_INTRrupt_CallBck", SAL_FIELD_TYPE_BOOL, "Option to initialize tool without interrupt callback enabled(default is with, enable interrupt callback, enabled", "false"},
        {NULL}
};

static sh_sand_option_t Dnx_lsm_show_action_options[] = {
        {"ID", SAL_FIELD_TYPE_UINT32, "action table id", "0"},
        {NULL}
};


/*
 * lsm cmds list begin {
 */

static sh_sand_cmd_t Dnx_lsm_show_cmds[] = {
    /**keyword,              action,             sub-command,           options,                            man   */
    {"QUALifiers",  cmd_dnx_lsm_show_qualifiers,  NULL,           NULL,                      &Dnx_lsm_show_qual_man},
    {"ACTions",     cmd_dnx_lsm_show_actions,     NULL,   Dnx_lsm_show_action_options,     &Dnx_lsm_show_act_man},
    {"TRAPs",       cmd_dnx_lsm_show_traps,       NULL,             NULL,                    &Dnx_lsm_show_trap_man},
    {"InteRrupts",       cmd_dnx_lsm_show_interrupts,       NULL,             NULL,                    &Dnx_lsm_show_interrupt_man},
    {NULL}
};


sh_sand_cmd_t Dnx_lsm_cmds[] = {
    /**keyword,      action,         sub-command,                           options,                    man                  */
    {"INiT",     cmd_dnx_lsm_init,    NULL,                              Dnx_lsm_init_options,     &Dnx_lsm_init_man,  NULL, NULL, SH_CMD_SKIP_MEMORY_CHECK},
    {"EVeNT",    cmd_dnx_lsm_event,   NULL,                              Dnx_lsm_event_options,    &Dnx_lsm_event_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {"deINiT",   cmd_dnx_lsm_deinit,  NULL,                              NULL,                     &Dnx_lsm_deinit_man},
    {"SHOW",             NULL,          Dnx_lsm_show_cmds,               NULL,                     &Dnx_lsm_show_man},
    {NULL}
};

sh_sand_man_t Dnx_lsm_man = {
    "Diagnostic pack for lsm module"
};
/*
 * lsm cmds definition end }
 */
/*
 * cmd definition end }
 */
