/** \file dnx/swstate/auto_generated/diagnostic/dnx_ipq_alloc_mngr_commandline.h
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

#ifndef __DNX_IPQ_ALLOC_MNGR_COMMANDLINE_H__
#define __DNX_IPQ_ALLOC_MNGR_COMMANDLINE_H__

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_ipq_alloc_mngr_types.h>
#include <bcm_int/dnx/algo/ipq/ipq_alloc_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_internal.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <bcm_int/dnx/cosq/cosq.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_dump_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_dump_man;
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_size_get_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_size_get_man;
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_diagnostic_operation_counters_man;
extern sh_sand_cmd_t sh_dnx_swstate_dnx_ipq_alloc_mngr_cmds[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_man;
extern const char cmd_dnx_swstate_dnx_ipq_alloc_mngr_desc[];

extern sh_sand_cmd_t sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_cmds[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_man;
extern const char cmd_dnx_swstate_dnx_ipq_alloc_mngr_regions_desc[];
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_regions_dump_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_dump_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_regions_size_get_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_size_get_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_regions_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_type_cmds[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_type_man;
extern const char cmd_dnx_swstate_dnx_ipq_alloc_mngr_regions_type_desc[];
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_type_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_regions_type_dump_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_type_dump_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_type_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_regions_type_size_get_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_type_size_get_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_type_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_regions_type_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_type_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_cmds[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_man;
extern const char cmd_dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_desc[];
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_dump_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_dump_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_size_get_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_size_get_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_regions_nof_allocated_queue_quartets_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_cmds[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_man;
extern const char cmd_dnx_swstate_dnx_ipq_alloc_mngr_fmq_desc[];
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_dump_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_dump_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_size_get_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_size_get_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_cmds[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_man;
extern const char cmd_dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_desc[];
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_dump_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_dump_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_size_get_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_size_get_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_is_enhance_scheduler_mode_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_cmds[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_man;
extern const char cmd_dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_desc[];
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_dump_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_dump_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_size_get_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_size_get_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_fmq_max_multicast_queue_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_cmds[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_man;
extern const char cmd_dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_desc[];
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_dump_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_dump_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_size_get_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_size_get_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_ipq_alloc_mngr_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_cmds[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_man;
extern const char cmd_dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_desc[];
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_dump_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_dump_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_size_get_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_size_get_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_tc_voq_flow_map_diagnostic_operation_counters_man;



extern sh_sand_cmd_t sh_dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_cmds[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_man;
extern const char cmd_dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_desc[];
shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_dump_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_dump_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_size_get_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_size_get_man;


shr_error_e sh_dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control);
extern sh_sand_option_t dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_diagnostic_operation_counters_options[];
extern sh_sand_man_t dnx_swstate_dnx_ipq_alloc_mngr_tc_sysport_map_diagnostic_operation_counters_man;


#endif /* DNX_SW_STATE_DIAGNOSTIC */

#endif /* __DNX_IPQ_ALLOC_MNGR_COMMANDLINE_H__ */
