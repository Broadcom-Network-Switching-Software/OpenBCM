/** \file dnx/swstate/auto_generated/diagnostic/reflector_commandline.h
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

#ifndef __REFLECTOR_COMMANDLINE_H__
#define __REFLECTOR_COMMANDLINE_H__

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnx/swstate/auto_generated/types/reflector_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
shr_error_e sh_dnx_swstate_reflector_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_dump_man;
shr_error_e sh_dnx_swstate_reflector_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_size_get_man;
shr_error_e sh_dnx_swstate_reflector_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_diagnostic_operation_counters_man;
extern sh_sand_cmd_t sh_dnx_swstate_reflector_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_man;
extern const char cmd_dnx_swstate_reflector_desc[];

extern sh_sand_cmd_t sh_dnx_swstate_reflector_uc_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_uc_man;
extern const char cmd_dnx_swstate_reflector_uc_desc[];
shr_error_e sh_dnx_swstate_reflector_uc_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_dump_man;


shr_error_e sh_dnx_swstate_reflector_uc_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_size_get_man;


shr_error_e sh_dnx_swstate_reflector_uc_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_reflector_uc_encap_id_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_uc_encap_id_man;
extern const char cmd_dnx_swstate_reflector_uc_encap_id_desc[];
shr_error_e sh_dnx_swstate_reflector_uc_encap_id_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_encap_id_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_encap_id_dump_man;


shr_error_e sh_dnx_swstate_reflector_uc_encap_id_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_encap_id_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_encap_id_size_get_man;


shr_error_e sh_dnx_swstate_reflector_uc_encap_id_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_encap_id_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_encap_id_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_reflector_uc_is_allocated_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_uc_is_allocated_man;
extern const char cmd_dnx_swstate_reflector_uc_is_allocated_desc[];
shr_error_e sh_dnx_swstate_reflector_uc_is_allocated_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_is_allocated_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_is_allocated_dump_man;


shr_error_e sh_dnx_swstate_reflector_uc_is_allocated_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_is_allocated_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_is_allocated_size_get_man;


shr_error_e sh_dnx_swstate_reflector_uc_is_allocated_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_is_allocated_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_is_allocated_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_reflector_uc_l2_int_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_man;
extern const char cmd_dnx_swstate_reflector_uc_l2_int_desc[];
shr_error_e sh_dnx_swstate_reflector_uc_l2_int_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_dump_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_size_get_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_reflector_uc_l2_int_encap_id_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_encap_id_man;
extern const char cmd_dnx_swstate_reflector_uc_l2_int_encap_id_desc[];
shr_error_e sh_dnx_swstate_reflector_uc_l2_int_encap_id_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_encap_id_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_encap_id_dump_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_encap_id_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_encap_id_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_encap_id_size_get_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_encap_id_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_encap_id_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_encap_id_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_reflector_uc_l2_int_is_allocated_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_is_allocated_man;
extern const char cmd_dnx_swstate_reflector_uc_l2_int_is_allocated_desc[];
shr_error_e sh_dnx_swstate_reflector_uc_l2_int_is_allocated_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_is_allocated_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_is_allocated_dump_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_is_allocated_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_is_allocated_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_is_allocated_size_get_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_is_allocated_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_is_allocated_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_is_allocated_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_reflector_uc_l2_int_onepass_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_man;
extern const char cmd_dnx_swstate_reflector_uc_l2_int_onepass_desc[];
shr_error_e sh_dnx_swstate_reflector_uc_l2_int_onepass_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_onepass_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_dump_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_onepass_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_onepass_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_size_get_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_onepass_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_onepass_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_reflector_uc_l2_int_onepass_encap_id_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_encap_id_man;
extern const char cmd_dnx_swstate_reflector_uc_l2_int_onepass_encap_id_desc[];
shr_error_e sh_dnx_swstate_reflector_uc_l2_int_onepass_encap_id_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_onepass_encap_id_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_encap_id_dump_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_onepass_encap_id_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_onepass_encap_id_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_encap_id_size_get_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_onepass_encap_id_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_onepass_encap_id_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_encap_id_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_man;
extern const char cmd_dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_desc[];
shr_error_e sh_dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_dump_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_size_get_man;


shr_error_e sh_dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_uc_l2_int_onepass_is_allocated_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_reflector_sbfd_reflector_cnt_cmds[];
extern sh_sand_man_t dnx_swstate_reflector_sbfd_reflector_cnt_man;
extern const char cmd_dnx_swstate_reflector_sbfd_reflector_cnt_desc[];
shr_error_e sh_dnx_swstate_reflector_sbfd_reflector_cnt_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_sbfd_reflector_cnt_dump_options[];
extern sh_sand_man_t dnx_swstate_reflector_sbfd_reflector_cnt_dump_man;


shr_error_e sh_dnx_swstate_reflector_sbfd_reflector_cnt_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_sbfd_reflector_cnt_size_get_options[];
extern sh_sand_man_t dnx_swstate_reflector_sbfd_reflector_cnt_size_get_man;


shr_error_e sh_dnx_swstate_reflector_sbfd_reflector_cnt_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_reflector_sbfd_reflector_cnt_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_reflector_sbfd_reflector_cnt_diagnostic_operation_counters_man;


#endif /* DNX_SW_STATE_DIAGNOSTIC */

#endif /* __REFLECTOR_COMMANDLINE_H__ */
