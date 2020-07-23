/** \file diag_dnx_tm.c
 * 
 * main file fot tm diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>

/** local */
#include "diag_dnx_tm.h"
#include "diag_dnx_ecgm.h"
#include "diag_dnx_ingress_reassembly.h"
#include "diag_dnx_ingress_compensation.h"
#include "diag_dnx_ingress_congestion.h"
#include "diag_dnx_ingress_rate.h"
#include "diag_dnx_local_to_sys.h"
#include "diag_dnx_fc.h"
#include "diag_dnx_egr_queuing.h"
#include "diag_dnx_ingress_prd.h"
#include "diag_dnx_queue.h"
#include "diag_dnx_scheduler.h"
#include "diag_dnx_snif.h"
#include "diag_dnx_lag.h"
#include "diag_dnx_group.h"
#include "diag_dnx_system_red.h"
/*
 * }
 */

/**
 * \brief DNX TM ingress rate calculation
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for ingress rate calculation commands
 */
static sh_sand_cmd_t sh_dnx_ingress_rate_cmds[] = {
    /*
     * keyword, action, command, options, man 
     */
    {"CGM", sh_dnx_ingress_cgm_rate_cmd, NULL, sh_dnx_ingress_cgm_rate_options, &sh_dnx_ingress_cgm_rate_man},
    {"IPT", sh_dnx_ingress_ipt_rate_cmd, NULL, NULL, &sh_dnx_ingress_ipt_rate_man},
    {"SQM", sh_dnx_ingress_sqm_rate_cmd, NULL, NULL, &sh_dnx_ingress_sqm_rate_man},
    {"IRE", sh_dnx_ingress_ire_rate_cmd, NULL, NULL, &sh_dnx_ingress_ire_rate_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_ingress_rate_man = {
    .brief = "TM Ingress rate calculation commands"
};

/**
 * \brief DNX TM ingress diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for TM diagnostic commands 
 */
 /* *INDENT-OFF* */
static sh_sand_cmd_t sh_dnx_tm_ingress_cmds[] = {
    /*keyword,       action,                          command,                  options,                             man                    */
    {"reassembly",   sh_dnx_ingr_reassembly_cmd,      NULL,                     sh_dnx_ingress_reassembly_options,   &sh_dnx_ingress_reassembly_man},
    {"queue",        NULL,                            sh_dnx_tm_queue_cmds,     NULL,                                &sh_dnx_tm_queue_man},
    {"vsq",          NULL,                            sh_dnx_tm_vsq_cmds,       NULL,                                &sh_dnx_tm_vsq_man},
    {"congestion",   sh_dnx_ingress_congestion_cmd,   NULL,                     sh_dnx_ingress_congestion_options,   &sh_dnx_ingress_congestion_man},
    {"rate_class",   sh_dnx_ingress_congestion_rate_class_cmd, NULL,            sh_dnx_ingress_congestion_rate_class_options,  &sh_dnx_ingress_congestion_rate_class_man},
    {"local_to_sys", sh_dnx_ingress_local_to_sys_cmd, NULL,                     NULL,                                &sh_dnx_ingress_local_to_sys_man},
    {"portdrop",     sh_dnx_ingress_port_drop_cmd,    NULL,                     sh_dnx_ingress_port_drop_options,    &sh_dnx_ingress_port_drop_man},
    {"compensation", sh_dnx_ingress_compensation_cmd, NULL,                     sh_dnx_ingress_compensation_options, &sh_dnx_ingress_compensation_man},
    {"system_red",   NULL,                            sh_dnx_tm_sys_red_cmds,   NULL,                                &sh_dnx_tm_sys_red_man},
    {"rate",         NULL,                            sh_dnx_ingress_rate_cmds, NULL,                                &sh_dnx_ingress_rate_man},
    {NULL}
};
static sh_sand_man_t sh_dnx_tm_ingress_man = {
    .brief    = "TM Ingress commands"
};

/**
 * \brief DNX TM egress rate calculation
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for egress rate calculation commands
 */
static sh_sand_cmd_t sh_dnx_egress_rate_cmds[] = {
    /*keyword,     action,                        command,    options,                             man                    */
    {"RQP",        sh_dnx_egr_rqp_rate_cmd,        NULL,       NULL,                               &sh_dnx_egr_rqp_rate_man},
    {"PQP",        sh_dnx_egr_pqp_rate_cmd,        NULL,       sh_dnx_egr_pqp_rate_options,        &sh_dnx_egr_pqp_rate_man},
    {"EPEPort",    sh_dnx_egr_epep_rate_cmd,       NULL,       sh_dnx_egr_epep_rate_options,       &sh_dnx_egr_epep_rate_man},
    {"EPNI",       sh_dnx_egr_epni_rate_cmd,       NULL,       sh_dnx_egr_epni_rate_options,       &sh_dnx_egr_epni_rate_man},
    {"EGQ",        sh_dnx_egr_queuing_rate_cmd,    NULL,       sh_dnx_egr_queuing_rate_options,    &sh_dnx_egr_queuing_rate_man},
    {"interface",  sh_dnx_egr_interface_rate_cmd,  NULL,       sh_dnx_egr_interface_rate_options,  &sh_dnx_egr_interface_rate_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_egress_rate_man = {
    .brief    = "TM Egress rate calculation commands"
};

/**
 * \brief DNX TM egress diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for TM diagnostic commands
 */
static sh_sand_cmd_t sh_dnx_tm_egress_cmds[] = {
    /*keyword,       action,                            command,                  options,                                man                    */
    {"shaper",       sh_dnx_egq_shaping_cmd,            NULL,                     sh_dnx_egq_shaping_options,             &sh_dnx_egq_shaping_man},
    {"compensation", sh_dnx_egq_compensation_cmd,       NULL,                     sh_dnx_egq_compensation_options,        &sh_dnx_egq_compensation_man},
    {"congestion",   sh_dnx_tm_egress_congestion_cmd,   NULL,                     sh_dnx_tm_egress_congestion_options,    &sh_dnx_tm_egress_congestion_man},
    {"rate",         NULL,                              sh_dnx_egress_rate_cmds,  NULL,                                   &sh_dnx_egress_rate_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_tm_egress_man = {
    .brief    = "TM Egress commands"
};

/**
 * \brief DNX TM scheduler rate calculation
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for scheduler rate calculation commands
 */
static sh_sand_cmd_t sh_dnx_scheduler_rate_cmds[] = {
    /*keyword,            action,                             command,    options,                                        man                    */
    {"PortScheDuler",     sh_dnx_scheduler_port_rate_cmd,     NULL,       sh_dnx_scheduler_port_rate_options,    &sh_dnx_scheduler_port_rate_man},
    {"FLOW",              sh_dnx_scheduler_flow_rate_cmd,     NULL,       sh_dnx_scheduler_flow_rate_options,    &sh_dnx_scheduler_flow_rate_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_scheduler_rate_man = {
    .brief    = "TM Scheduler rate calculation commands"
};

/**
 * \brief DNX TM scheduler FMS
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for scheduler rate calculation commands
 */
static sh_sand_cmd_t sh_dnx_scheduler_fsm_cmds[] = {
    /*keyword,            action,                             command,              options,                                       man                              */
    {"ConFiG",            sh_dnx_scheduler_fsm_config_cmd,    NULL,                 sh_dnx_scheduler_fsm_config_options,           &sh_dnx_scheduler_fsm_config_man},
    {"read",              sh_dnx_scheduler_fsm_read_cmd,      NULL,                 NULL,                                          &sh_dnx_scheduler_fsm_read_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_scheduler_fsm_man = {
    .brief    = "TM Scheduler FSM commands"
};

/**
 * \brief DNX TM scheduler diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for scheduler diagnostic commands
 */
static sh_sand_cmd_t sh_dnx_tm_scheduler_cmds[] = {
    /*keyword,            action,                                 command,                    options,                                    man                    */
    {"connection",        sh_dnx_scheduler_connection_cmd,        NULL,                       sh_dnx_scheduler_connection_options,        &sh_dnx_scheduler_connection_man},
    {"gport",             sh_dnx_scheduler_gport_cmd,             NULL,                       sh_dnx_scheduler_gport_options,             &sh_dnx_scheduler_gport_man},
    {"print_flow_and_up", sh_dnx_scheduler_print_flow_and_up_cmd, NULL,                       sh_dnx_scheduler_print_flow_and_up_options, &sh_dnx_scheduler_print_flow_and_up_man},
    {"portinfo",          sh_dnx_scheduler_port_info_cmd,         NULL,                       sh_dnx_scheduler_port_info_options,         &sh_dnx_scheduler_port_info_man},
    {"group",             NULL,                                   sh_dnx_tm_sch_group_cmds,   NULL,                                       &sh_dnx_tm_sch_group_man},
    {"rate",              NULL,                                   sh_dnx_scheduler_rate_cmds, NULL,                                       &sh_dnx_scheduler_rate_man},
    {"FSM",               NULL,                                   sh_dnx_scheduler_fsm_cmds,  NULL,                                       &sh_dnx_scheduler_fsm_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_tm_scheduler_man = {
    .brief    = "TM Scheduler commands"
};

/**
 * \brief DNX TM diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for TM diagnostic commands 
 */
sh_sand_cmd_t sh_dnx_tm_cmds[] = {
    /*keyword,       action,          command,                  options,              man                    */
    {"ingress",      NULL,            sh_dnx_tm_ingress_cmds,   NULL,                 &sh_dnx_tm_ingress_man},
    {"egress",       NULL,            sh_dnx_tm_egress_cmds,    NULL,                 &sh_dnx_tm_egress_man},
    {"lag",          sh_dnx_lag_cmd,  NULL,                     sh_dnx_lag_arguments, &sh_dnx_lag_man},
    {"flow_control", NULL,            sh_dnx_fc_cmds,           NULL,                 &sh_dnx_fc_man},
    {"scheduler",    NULL,            sh_dnx_tm_scheduler_cmds, NULL,                 &sh_dnx_tm_scheduler_man},
    {"snif",         sh_dnx_snif_cmd, NULL,                     sh_dnx_snif_options,  &sh_dnx_snif_man},
    {NULL}
};

sh_sand_man_t sh_dnx_tm_man = {
    .brief    = "TM commands"
};

/* *INDENT-ON* */
