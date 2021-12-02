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
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LSM

/*
 * INCLUDE FILES:
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
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

    SHR_IF_ERR_EXIT(appl_ref_lsm_init(unit, &init_info));

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
    int packet_size;
    char *packet_data;

    lsm_event_info_t info;
    SHR_FUNC_INIT_VARS(unit);

    info.event = lsm_packet_event;

    packet_size = MAX_PKT_HDR_SIZE;
    /** Packet allocation according to max size. If allocation is succesful free in end of function */
    SHR_IF_ERR_EXIT(bcm_pkt_alloc(unit, packet_size, 0, &info.pkt));
    sal_memset(info.pkt->pkt_data, 0x0, sizeof(bcm_pkt_blk_t));

    info.pkt->pkt_data->len = MAX_PKT_HDR_SIZE;

    SH_SAND_GET_STR("DaTa", packet_data);
    info.pkt->pkt_data->data = (uint8 *) packet_data;

    SH_SAND_GET_INT32("PtchSRCport", info.pkt->src_port);
    SHR_IF_ERR_EXIT(appl_ref_lsm_run(unit, &info));

exit:
    bcm_pkt_free(unit, info.pkt);
    info.pkt = NULL;

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

    appl_ref_lsm_show_actions(unit, action_table_id);
    SHR_EXIT();

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
    appl_ref_lsm_show_traps(unit);
    SHR_EXIT();

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
    .synopsis = "[Port=<CPU_port>] [LOGPath=<directory to place logs>] [XMLPath=<directory to load xmls froms>] [NO_LOG=<disable log>]",
    .examples = "Port=0\n LOGPath=my_logs/\n XMLPath=jericho2_a0/lsm/\n NO_LOG\n",
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
    .full      = NULL
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
        {"NO_Log", SAL_FIELD_TYPE_BOOL, "full path to directoy containing logs", "true"},
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
    {NULL}
};


sh_sand_cmd_t Dnx_lsm_cmds[] = {
    /**keyword,      action,         sub-command,                           options,                    man                  */
    {"INiT",     cmd_dnx_lsm_init,    NULL,                          Dnx_lsm_init_options,     &Dnx_lsm_init_man},
    {"EVeNT",    cmd_dnx_lsm_event,   NULL,                          Dnx_lsm_event_options,    &Dnx_lsm_event_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {"deINiT",   cmd_dnx_lsm_deinit,  NULL,                              NULL,                   &Dnx_lsm_deinit_man},
    {"SHOW",             NULL,          Dnx_lsm_show_cmds,               NULL,                   &Dnx_lsm_show_man},
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
