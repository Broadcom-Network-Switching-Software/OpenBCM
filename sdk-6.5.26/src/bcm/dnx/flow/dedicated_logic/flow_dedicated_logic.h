 /** \file flow_dedicated_logic.h
 *
 * All dnx flow dedicated logic str structs, callbacks structures etc.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _DNX_FLOW_DDEDICATED_LOGIC_INCLUDED__
/*
 * {
 */
#define _DNX_FLOW_DDEDICATED_LOGIC_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/** 
 * For each object that require dedicated logic the following APIs should be implemented: 
 * name convetion: yyy - ingress/egresss, xxx - profile name
 * 1. dnx_flow_dl_yyy_xxx_profile_verify (Optional)
 * 2. dnx_flow_dl_yyy_xxx_profile_create 
 * 3. dnx_flow_dl_yyy_xxx_profile_get 
 * 4. dnx_flow_dl_yyy_xxx_profile_replace 
 * 5. dnx_flow_dl_yyy_xxx_profile_destroy 
 * 6. dnx_flow_dl_yyy_xxx_profile_hw_clear (there could be other API that performs this operation) 
 *  
 * Replace Logic 
 * ------------- 
 * *****************************************************************************************
 * * CURRENT ENTRY * USER INPUT * OPERATION                                                *
 * *****************************************************************************************
 * * exists        * exists     * replace profile, return indication if is_last profile    *
 * *****************************************************************************************
 * * exists        * not exists * copy from get_handle the profile value to the new handle *
 * *****************************************************************************************
 * * not exists    * not exists * do nothing                                               *
 * *****************************************************************************************
 * * not exists    * exists     * call create                                              *
 * ***************************************************************************************** 
 *  
 */

shr_error_e dnx_flow_dl_egress_qos_profile_create(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_egress_qos_profile_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_egress_qos_profile_destroy(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources);

shr_error_e dnx_flow_dl_egress_qos_profile_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    int *last_network_qos_profile,
    int *last_ttl_profile);

shr_error_e dnx_flow_dl_egress_qos_profile_validate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * bcm_app_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_ingress_qos_profile_validate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_terminator_info_t * terminator_info);

shr_error_e dnx_flow_dl_ingress_qos_profile_create(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_ingress_qos_profile_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_ingress_qos_profile_destroy(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources);

/** handles all cases of replace: old and new value exists, only old exists and only new exists */
shr_error_e dnx_flow_dl_ingress_qos_profile_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile);

/*** SRC_ADDR_PROFILE APIs */
shr_error_e dnx_flow_dl_egress_src_addr_profile_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_egress_src_addr_profile_destroy(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources);

/** creating a new profile */
shr_error_e dnx_flow_dl_egress_src_addr_profile_create(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_special_fields_t * special_fields);

/** Clear the profile from the HW */
shr_error_e dnx_flow_dl_egress_src_addr_profile_hw_clear(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 profile_id);

/** Handles all the cases of replace, clearing old value, setting new value, replacing old with new */
shr_error_e dnx_flow_dl_egress_src_addr_profile_replace(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile);


shr_error_e dnx_flow_dl_outlif_profile_create(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_outlif_profile_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_initiator_info_t * initiator_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_outlif_profile_destroy(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources);

shr_error_e dnx_flow_dl_outlif_profile_replace(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * initiator_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile);


shr_error_e dnx_flow_dl_ingress_trap_profile_verify(
    int unit,
    bcm_flow_terminator_info_t * terminator_info);

shr_error_e dnx_flow_dl_ingress_trap_profile_get(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_ingress_trap_profile_create(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_ingress_trap_profile_destroy(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources);

shr_error_e dnx_flow_dl_ingress_trap_profile_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile);

shr_error_e dnx_flow_dl_egress_trap_profile_verify(
    int unit,
    bcm_flow_initiator_info_t * initiator_info);

shr_error_e dnx_flow_dl_egress_trap_profile_get(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_egress_trap_profile_create(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_egress_trap_profile_destroy(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources);

shr_error_e dnx_flow_dl_egress_trap_profile_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile);

shr_error_e dnx_flow_dl_ingress_mpls_termination_profile_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 current_entry_handle_id,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_ingress_mpls_termination_profile_destroy(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id);

shr_error_e dnx_flow_dl_ingress_mpls_termination_profile_create(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * special_fields);

shr_error_e dnx_flow_dl_ingress_mpls_termination_profile_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_special_fields_t * special_fields,
    int *last_profile);

#endif  /*_DNX_FLOW_DDEDICATED_LOGIC_INCLUDED__*/
