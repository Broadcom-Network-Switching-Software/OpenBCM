/** \file dnx/swstate/auto_generated/diagnostic/algo_sat_commandline.h
 *
 * sw state functions declarations
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __ALGO_SAT_COMMANDLINE_H__
#define __ALGO_SAT_COMMANDLINE_H__

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/algo_sat_types.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_internal.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <bcm_int/dnx/sat/sat.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
shr_error_e sh_dnx_swstate_algo_sat_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_dump_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_dump_man;
shr_error_e sh_dnx_swstate_algo_sat_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_size_get_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_size_get_man;
shr_error_e sh_dnx_swstate_algo_sat_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_diagnostic_operation_counters_man;
extern sh_sand_cmd_t sh_dnx_swstate_algo_sat_cmds[];
extern sh_sand_man_t dnx_swstate_algo_sat_man;
extern const char cmd_dnx_swstate_algo_sat_desc[];

extern sh_sand_cmd_t sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_cmds[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_man;
extern const char cmd_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_desc[];
shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_dump_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_dump_man;


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_size_get_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_size_get_man;


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_cmds[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_man;
extern const char cmd_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_desc[];
shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_dump_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_dump_man;


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_size_get_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_size_get_man;


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_cmds[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_man;
extern const char cmd_dnx_swstate_algo_sat_sat_res_alloc_trap_id_desc[];
shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_dump_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_dump_man;


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_size_get_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_size_get_man;


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_cmds[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_man;
extern const char cmd_dnx_swstate_algo_sat_sat_res_alloc_trap_data_desc[];
shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_dump_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_dump_man;


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_size_get_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_size_get_man;


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_algo_sat_sat_pkt_header_cmds[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_pkt_header_man;
extern const char cmd_dnx_swstate_algo_sat_sat_pkt_header_desc[];
shr_error_e sh_dnx_swstate_algo_sat_sat_pkt_header_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_pkt_header_dump_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_pkt_header_dump_man;


shr_error_e sh_dnx_swstate_algo_sat_sat_pkt_header_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_pkt_header_size_get_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_pkt_header_size_get_man;


shr_error_e sh_dnx_swstate_algo_sat_sat_pkt_header_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_algo_sat_sat_pkt_header_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_algo_sat_sat_pkt_header_diagnostic_operation_counters_man;


#endif /* DNX_SW_STATE_DIAGNOSTIC */

#endif /* __ALGO_SAT_COMMANDLINE_H__ */
