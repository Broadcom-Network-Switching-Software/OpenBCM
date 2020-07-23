/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 FABRIC MULTICAST H
 */
 
#ifndef _SOC_FE1600_FABRIC_MULTICAST_H_
#define _SOC_FE1600_FABRIC_MULTICAST_H_

#include <bcm/fabric.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>


soc_error_t soc_fe1600_fabric_multicast_low_prio_drop_select_priority_set(int unit, soc_dfe_fabric_priority_t prio);
soc_error_t soc_fe1600_fabric_multicast_low_prio_drop_select_priority_get(int unit, soc_dfe_fabric_priority_t* prio);
soc_error_t soc_fe1600_fabric_multicast_low_prio_threshold_validate(int unit, bcm_fabric_control_t type, int arg);
soc_error_t soc_fe1600_fabric_multicast_low_prio_threshold_set(int unit, bcm_fabric_control_t type, int arg);
soc_error_t soc_fe1600_fabric_multicast_low_prio_threshold_get(int unit, bcm_fabric_control_t type, int* arg);
soc_error_t soc_fe1600_fabric_multicast_low_priority_drop_enable_set(int unit, int arg);
soc_error_t soc_fe1600_fabric_multicast_low_priority_drop_enable_get(int unit, int* arg);
soc_error_t soc_fe1600_fabric_multicast_priority_range_validate(int unit, bcm_fabric_control_t type, int arg);
soc_error_t soc_fe1600_fabric_multicast_priority_range_set(int unit, bcm_fabric_control_t type, int arg);
soc_error_t soc_fe1600_fabric_multicast_priority_range_get(int unit, bcm_fabric_control_t type, int* arg);
soc_error_t soc_fe1600_fabric_multicast_multi_set(int unit, uint32 flags, uint32 ngroups, soc_multicast_t *groups, bcm_fabric_module_vector_t *dest_array);
soc_error_t soc_fe1600_fabric_multicast_multi_get(int unit, uint32 flags, uint32 ngroups, soc_multicast_t *groups, bcm_fabric_module_vector_t *dest_array);
soc_error_t soc_fe1600_fabric_multicast_read_info(int unit, soc_multicast_t group, int *mc_table_index, int *mc_table_entry, int *is_first_half);
soc_error_t soc_fe1600_fabric_multicast_multi_read_info_get(int unit, soc_dfe_multicast_read_range_info_t **info, int *info_size);
soc_error_t soc_fe1600_fabric_multicast_multi_write_range(int unit, int mem_flags, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array);

#endif 
