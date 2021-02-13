/** \file dnx/swstate/auto_generated/diagnostic/flow_commandline.h
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

#ifndef __FLOW_COMMANDLINE_H__
#define __FLOW_COMMANDLINE_H__

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
shr_error_e sh_dnx_swstate_flow_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_dump_options[];
extern sh_sand_man_t dnx_swstate_flow_dump_man;
shr_error_e sh_dnx_swstate_flow_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_size_get_options[];
extern sh_sand_man_t dnx_swstate_flow_size_get_man;
shr_error_e sh_dnx_swstate_flow_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_flow_diagnostic_operation_counters_man;
extern sh_sand_cmd_t sh_dnx_swstate_flow_cmds[];
extern sh_sand_man_t dnx_swstate_flow_man;
extern const char cmd_dnx_swstate_flow_desc[];

extern sh_sand_cmd_t sh_dnx_swstate_flow_flow_application_info_cmds[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_man;
extern const char cmd_dnx_swstate_flow_flow_application_info_desc[];
shr_error_e sh_dnx_swstate_flow_flow_application_info_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_flow_application_info_dump_options[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_dump_man;


shr_error_e sh_dnx_swstate_flow_flow_application_info_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_flow_application_info_size_get_options[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_size_get_man;


shr_error_e sh_dnx_swstate_flow_flow_application_info_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_flow_application_info_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_flow_flow_application_info_is_verify_disabled_cmds[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_is_verify_disabled_man;
extern const char cmd_dnx_swstate_flow_flow_application_info_is_verify_disabled_desc[];
shr_error_e sh_dnx_swstate_flow_flow_application_info_is_verify_disabled_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_flow_application_info_is_verify_disabled_dump_options[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_is_verify_disabled_dump_man;


shr_error_e sh_dnx_swstate_flow_flow_application_info_is_verify_disabled_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_flow_application_info_is_verify_disabled_size_get_options[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_is_verify_disabled_size_get_man;


shr_error_e sh_dnx_swstate_flow_flow_application_info_is_verify_disabled_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_flow_application_info_is_verify_disabled_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_is_verify_disabled_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_cmds[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_man;
extern const char cmd_dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_desc[];
shr_error_e sh_dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_dump_options[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_dump_man;


shr_error_e sh_dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_size_get_options[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_size_get_man;


shr_error_e sh_dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_flow_flow_application_info_is_error_recovery_disabled_diagnostic_operation_counters_man;


#endif /* DNX_SW_STATE_DIAGNOSTIC */

#endif /* __FLOW_COMMANDLINE_H__ */
