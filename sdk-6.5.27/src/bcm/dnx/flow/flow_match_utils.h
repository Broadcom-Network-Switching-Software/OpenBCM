/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file flow_match_utils.h
 * 
 *
 * This file contains the match utils APIs
 */

#ifndef  INCLUDE_FLOW_MATCH_UTILS_H_INCLUDED
#define  INCLUDE_FLOW_MATCH_UTILS_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

shr_error_e dnx_flow_match_info_add(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_special_fields_t * key_special_fields);

shr_error_e dnx_flow_match_info_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_special_fields_t * key_special_fields);

shr_error_e dnx_flow_match_info_delete(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_special_fields_t * key_special_fields);

shr_error_e dnx_flow_gport_to_match_info_sw_add(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id);

shr_error_e dnx_flow_gport_to_match_info_sw_delete(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * key_special_fields);

/*
 * }
 */

#endif /* INCLUDE_FLOW_MATCH_UTILS_H_INCLUDED */
