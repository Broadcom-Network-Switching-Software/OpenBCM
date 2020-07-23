
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __STIF_ACCESS_H__
#define __STIF_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/stif_types.h>
#include <include/bcm/stat.h>
#include <include/bcm/types.h>



typedef int (*dnx_stif_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_stif_db_init_cb)(
    int unit);


typedef int (*dnx_stif_db_instance_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1);


typedef int (*dnx_stif_db_instance_logical_port_set_cb)(
    int unit, uint32 instance_idx_0, uint32 instance_idx_1, bcm_port_t logical_port);


typedef int (*dnx_stif_db_instance_logical_port_get_cb)(
    int unit, uint32 instance_idx_0, uint32 instance_idx_1, bcm_port_t *logical_port);


typedef int (*dnx_stif_db_source_mapping_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1);


typedef int (*dnx_stif_db_source_mapping_first_port_set_cb)(
    int unit, uint32 source_mapping_idx_0, uint32 source_mapping_idx_1, bcm_port_t first_port);


typedef int (*dnx_stif_db_source_mapping_first_port_get_cb)(
    int unit, uint32 source_mapping_idx_0, uint32 source_mapping_idx_1, bcm_port_t *first_port);


typedef int (*dnx_stif_db_source_mapping_second_port_set_cb)(
    int unit, uint32 source_mapping_idx_0, uint32 source_mapping_idx_1, bcm_port_t second_port);


typedef int (*dnx_stif_db_source_mapping_second_port_get_cb)(
    int unit, uint32 source_mapping_idx_0, uint32 source_mapping_idx_1, bcm_port_t *second_port);




typedef struct {
    dnx_stif_db_instance_logical_port_set_cb set;
    dnx_stif_db_instance_logical_port_get_cb get;
} dnx_stif_db_instance_logical_port_cbs;


typedef struct {
    dnx_stif_db_instance_alloc_cb alloc;
    
    dnx_stif_db_instance_logical_port_cbs logical_port;
} dnx_stif_db_instance_cbs;


typedef struct {
    dnx_stif_db_source_mapping_first_port_set_cb set;
    dnx_stif_db_source_mapping_first_port_get_cb get;
} dnx_stif_db_source_mapping_first_port_cbs;


typedef struct {
    dnx_stif_db_source_mapping_second_port_set_cb set;
    dnx_stif_db_source_mapping_second_port_get_cb get;
} dnx_stif_db_source_mapping_second_port_cbs;


typedef struct {
    dnx_stif_db_source_mapping_alloc_cb alloc;
    
    dnx_stif_db_source_mapping_first_port_cbs first_port;
    
    dnx_stif_db_source_mapping_second_port_cbs second_port;
} dnx_stif_db_source_mapping_cbs;


typedef struct {
    dnx_stif_db_is_init_cb is_init;
    dnx_stif_db_init_cb init;
    
    dnx_stif_db_instance_cbs instance;
    
    dnx_stif_db_source_mapping_cbs source_mapping;
} dnx_stif_db_cbs;





extern dnx_stif_db_cbs dnx_stif_db;

#endif 
