/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON FABRIC MULTICAST H
 */
 
#ifndef _SOC_RAMON_FABRIC_MULTICAST_H_
#define _SOC_RAMON_FABRIC_MULTICAST_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 

shr_error_e soc_ramon_fabric_multicast_multi_write_range(int unit, int mem_flags, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array);
shr_error_e soc_ramon_fabric_multicast_multi_read_range(int unit, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array);
shr_error_e soc_ramon_fabric_multicast_local_route_default_init(int unit);
shr_error_e soc_ramon_fabric_multicast_local_route_bit_get(int unit, uint32 *mc_local_route_bit);
shr_error_e soc_ramon_fabric_multicast_low_prio_drop_select_priority_set(int unit, soc_dnxf_fabric_priority_t prio);
shr_error_e soc_ramon_fabric_multicast_low_prio_drop_select_priority_get(int unit, soc_dnxf_fabric_priority_t* prio);
shr_error_e soc_ramon_fabric_multicast_low_prio_threshold_validate(int unit, bcm_fabric_control_t type, int arg);
shr_error_e soc_ramon_fabric_multicast_low_prio_threshold_set(int unit, bcm_fabric_control_t type, int arg);
shr_error_e soc_ramon_fabric_multicast_low_prio_threshold_get(int unit, bcm_fabric_control_t type, int* arg);
shr_error_e soc_ramon_fabric_multicast_low_priority_drop_enable_set(int unit, int arg);
shr_error_e soc_ramon_fabric_multicast_low_priority_drop_enable_get(int unit, int* arg);
shr_error_e soc_ramon_fabric_multicast_priority_range_validate(int unit, bcm_fabric_control_t type, int arg);
shr_error_e soc_ramon_fabric_multicast_priority_range_set(int unit, bcm_fabric_control_t type, int arg);
shr_error_e soc_ramon_fabric_multicast_priority_range_get(int unit, bcm_fabric_control_t type, int* arg);
shr_error_e soc_ramon_fabric_multicast_multi_set(int unit, uint32 flags, uint32 ngroups, soc_multicast_t *groups, bcm_fabric_module_vector_t *dest_array, int *local_route_enable_array);
shr_error_e soc_ramon_fabric_multicast_multi_get(int unit, uint32 flags, uint32 ngroups, soc_multicast_t *groups, bcm_fabric_module_vector_t *dest_array, int *local_route_enable_array);
shr_error_e soc_ramon_fabric_multicast_direct_offset_set(int unit, int fap_modid_offset);
shr_error_e soc_ramon_fabric_multicast_direct_offset_get(int unit, int* fap_modid_offset);

#endif
