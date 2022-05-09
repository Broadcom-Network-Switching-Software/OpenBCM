/*! \file diag_dnx_ingress_prd.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_INGRESS_PRD_H_INCLUDED
#define DIAG_DNX_INGRESS_PRD_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <appl/diag/sand/diag_sand_prt.h>
/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/

extern sh_sand_option_t sh_dnx_ingress_port_drop_options[];
extern sh_sand_man_t sh_dnx_ingress_port_drop_show_man;
extern sh_sand_cmd_t sh_dnx_tm_ingress_prd_cmds[];

/*************
 * FUNCTIONS *
 *************/

shr_error_e sh_dnx_ingress_port_drop_add_row(
    int unit,
    int port,
    int prio_group_idx,
    uint32 src_prio,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 fifo_size,
    uint64 drop_count,
    char *header_str,
    prt_control_t * prt_ctr,
    int counter_id);

shr_error_e sh_dnx_ingress_port_drop_header_type_get(
    int unit,
    int port,
    char *header_str);

shr_error_e sh_dnx_ingress_port_drop_ilkn(
    int unit,
    int port,
    prt_control_t * prt_ctr);

shr_error_e sh_dnx_ingress_port_drop_eth_flexe(
    int unit,
    int port,
    prt_control_t * prt_ctr);

/**
 * \brief - Dump the port PRD info
 */
shr_error_e sh_dnx_ingress_port_drop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e sh_dnx_ingress_global_port_drop_add_row(
    int unit,
    uint32 priority,
    uint32 priority_group,
    prt_control_t * prt_ctr);

shr_error_e sh_dnx_ingress_global_port_drop(
    int unit,
    prt_control_t * prt_ctr);

shr_error_e sh_dnx_ingress_prd_last_packet_priority(
    int unit,
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control);

shr_error_e sh_dnx_map_counter(
    int unit,
    uint32 counter,
    uint32 port,
    uint32 priority_group);

#endif /** DIAG_DNX_INGRESS_PRD_H_INCLUDED */
