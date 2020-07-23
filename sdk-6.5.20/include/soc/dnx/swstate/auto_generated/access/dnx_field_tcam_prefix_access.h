
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_PREFIX_ACCESS_H__
#define __DNX_FIELD_TCAM_PREFIX_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_prefix_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>



typedef int (*dnx_field_tcam_prefix_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_tcam_prefix_sw_init_cb)(
    int unit);


typedef int (*dnx_field_tcam_prefix_sw_banks_prefix_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_prefix_sw_banks_prefix_prefix_handler_map_set_cb)(
    int unit, uint32 banks_prefix_idx_0, uint32 prefix_handler_map_idx_0, uint32 prefix_handler_map);


typedef int (*dnx_field_tcam_prefix_sw_banks_prefix_prefix_handler_map_get_cb)(
    int unit, uint32 banks_prefix_idx_0, uint32 prefix_handler_map_idx_0, uint32 *prefix_handler_map);


typedef int (*dnx_field_tcam_prefix_sw_banks_prefix_prefix_handler_map_alloc_cb)(
    int unit, uint32 banks_prefix_idx_0);




typedef struct {
    dnx_field_tcam_prefix_sw_banks_prefix_prefix_handler_map_set_cb set;
    dnx_field_tcam_prefix_sw_banks_prefix_prefix_handler_map_get_cb get;
    dnx_field_tcam_prefix_sw_banks_prefix_prefix_handler_map_alloc_cb alloc;
} dnx_field_tcam_prefix_sw_banks_prefix_prefix_handler_map_cbs;


typedef struct {
    dnx_field_tcam_prefix_sw_banks_prefix_alloc_cb alloc;
    
    dnx_field_tcam_prefix_sw_banks_prefix_prefix_handler_map_cbs prefix_handler_map;
} dnx_field_tcam_prefix_sw_banks_prefix_cbs;


typedef struct {
    dnx_field_tcam_prefix_sw_is_init_cb is_init;
    dnx_field_tcam_prefix_sw_init_cb init;
    
    dnx_field_tcam_prefix_sw_banks_prefix_cbs banks_prefix;
} dnx_field_tcam_prefix_sw_cbs;





extern dnx_field_tcam_prefix_sw_cbs dnx_field_tcam_prefix_sw;

#endif 
