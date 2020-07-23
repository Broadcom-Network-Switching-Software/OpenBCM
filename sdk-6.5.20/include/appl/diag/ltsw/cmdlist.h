/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    cmdlist.h
 * Purpose: Extern declarations for ltsw command functions and
 *          their associated usage strings.
 */

#ifndef _DIAG_LTSW_CMDLIST_H
#define _DIAG_LTSW_CMDLIST_H

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>

#define DCL_LTSW_CMD(_f,_u)  \
    extern cmd_result_t _f(int, args_t *); \
    extern char _u[];

DCL_LTSW_CMD(cmd_bcmlt_shell, cmd_bcmlt_shell_usage)

DCL_LTSW_CMD(cmd_sdklt_debug_shell, cmd_sdklt_debug_shell_usage)

DCL_LTSW_CMD(cmd_ltsw_clear, cmd_ltsw_clear_usage)

DCL_LTSW_CMD(cmd_ltsw_counter, cmd_ltsw_counter_usage)

DCL_LTSW_CMD(cmd_ltsw_custom_stat, cmd_ltsw_custom_stat_usage)

DCL_LTSW_CMD(cmd_ltsw_cos, cmd_ltsw_cos_usage)

DCL_LTSW_CMD(cmd_ltsw_event_stat, cmd_ltsw_event_stat_usage)

DCL_LTSW_CMD(cmd_ltsw_l2, cmd_ltsw_l2_usage)

DCL_LTSW_CMD(cmd_ltsw_l3, cmd_ltsw_l3_usage)

DCL_LTSW_CMD(cmd_ltsw_mirror, cmd_ltsw_mirror_usage)

DCL_LTSW_CMD(cmd_ltsw_show, cmd_ltsw_show_usage)

DCL_LTSW_CMD(if_ltsw_linkscan, if_ltsw_linkscan_usage)

DCL_LTSW_CMD(if_ltsw_trunk,  if_ltsw_trunk_usage)

DCL_LTSW_CMD(cmd_ltsw_port, cmd_ltsw_port_usage)

DCL_LTSW_CMD(cmd_ltsw_port_stat, cmd_ltsw_port_stat_usage)

DCL_LTSW_CMD(cmd_ltsw_pbmp, cmd_ltsw_pbmp_usage)

DCL_LTSW_CMD(cmd_ltsw_flexport, cmd_ltsw_flexport_usage)

DCL_LTSW_CMD(if_ltsw_asf, if_ltsw_asf_usage)

DCL_LTSW_CMD(cmd_ltsw_pvlan, cmd_ltsw_pvlan_usage)

DCL_LTSW_CMD(cmd_ltsw_stg, cmd_ltsw_stg_usage)

DCL_LTSW_CMD(cmd_ltsw_vlan, cmd_ltsw_vlan_usage)

DCL_LTSW_CMD(cmd_ltsw_rate, cmd_ltsw_rate_usage)

DCL_LTSW_CMD(cmd_ltsw_ratebw, cmd_ltsw_ratebw_usage)

DCL_LTSW_CMD(cmd_ltsw_flow_db, cmd_ltsw_flow_db_usage)

DCL_LTSW_CMD(cmd_ltsw_init, cmd_ltsw_init_usage)

DCL_LTSW_CMD(cmd_ltsw_txn, cmd_ltsw_txn_usage)

DCL_LTSW_CMD(cmd_ltsw_mpls, cmd_ltsw_mpls_usage)

DCL_LTSW_CMD(cmd_ltsw_trace, cmd_ltsw_trace_usage)

DCL_LTSW_CMD(cmd_ltsw_pcktwatch, cmd_ltsw_pcktwatch_usage)

DCL_LTSW_CMD(cmd_ltsw_flexctr, cmd_ltsw_flexctr_usage)

DCL_LTSW_CMD(cmd_ltsw_multicast, cmd_ltsw_multicast_usage)

DCL_LTSW_CMD(cmd_ltsw_ipmc, cmd_ltsw_ipmc_usage)

DCL_LTSW_CMD(cmd_ltsw_stk_port_set, cmd_ltsw_stk_port_set_usage)

DCL_LTSW_CMD(cmd_ltsw_stk_port_get, cmd_ltsw_stk_port_get_usage)

DCL_LTSW_CMD(cmd_ltsw_stkmode, cmd_ltsw_stkmode_usage)

DCL_LTSW_CMD(if_ltsw_field_proc, if_ltsw_field_proc_usage)


DCL_LTSW_CMD(cmd_ltsw_switch_control, cmd_ltsw_switch_control_usage)

DCL_LTSW_CMD(cmd_ltsw_dump,  cmd_ltsw_dump_usage)

DCL_LTSW_CMD(cmd_ltsw_ser, cmd_ltsw_ser_usage)

DCL_LTSW_CMD(cmd_ltsw_sram_scan, cmd_ltsw_sram_scan_usage)

DCL_LTSW_CMD(cmd_ltsw_mem_scan, cmd_ltsw_mem_scan_usage)

DCL_LTSW_CMD(cmd_ltsw_flexdigest, cmd_ltsw_flexdigest_usage)

DCL_LTSW_CMD(cmd_ltsw_ha,  cmd_ltsw_ha_usage)

DCL_LTSW_CMD(cmd_ltsw_alpm,  cmd_ltsw_alpm_usage)

#endif /* _DIAG_LTSW_CMDLIST_H */
