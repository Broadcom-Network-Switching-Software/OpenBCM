
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FABRIC_ACCESS_H__
#define __DNX_FABRIC_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_fabric_types.h>
#include <include/shared/periodic_event.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>



typedef int (*dnx_fabric_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_fabric_db_init_cb)(
    int unit);


typedef int (*dnx_fabric_db_modids_to_group_map_alloc_cb)(
    int unit);


typedef int (*dnx_fabric_db_modids_to_group_map_nof_modids_set_cb)(
    int unit, uint32 modids_to_group_map_idx_0, int nof_modids);


typedef int (*dnx_fabric_db_modids_to_group_map_nof_modids_get_cb)(
    int unit, uint32 modids_to_group_map_idx_0, int *nof_modids);


typedef int (*dnx_fabric_db_modids_to_group_map_modids_set_cb)(
    int unit, uint32 modids_to_group_map_idx_0, uint32 modids_idx_0, int modids);


typedef int (*dnx_fabric_db_modids_to_group_map_modids_get_cb)(
    int unit, uint32 modids_to_group_map_idx_0, uint32 modids_idx_0, int *modids);


typedef int (*dnx_fabric_db_modids_to_group_map_modids_alloc_cb)(
    int unit, uint32 modids_to_group_map_idx_0);


typedef int (*dnx_fabric_db_links_alloc_cb)(
    int unit);


typedef int (*dnx_fabric_db_links_is_link_allowed_set_cb)(
    int unit, uint32 links_idx_0, int is_link_allowed);


typedef int (*dnx_fabric_db_links_is_link_allowed_get_cb)(
    int unit, uint32 links_idx_0, int *is_link_allowed);


typedef int (*dnx_fabric_db_links_is_link_isolated_set_cb)(
    int unit, uint32 links_idx_0, int is_link_isolated);


typedef int (*dnx_fabric_db_links_is_link_isolated_get_cb)(
    int unit, uint32 links_idx_0, int *is_link_isolated);


typedef int (*dnx_fabric_db_load_balancing_periodic_event_handler_set_cb)(
    int unit, periodic_event_handler_t load_balancing_periodic_event_handler);


typedef int (*dnx_fabric_db_load_balancing_periodic_event_handler_get_cb)(
    int unit, periodic_event_handler_t *load_balancing_periodic_event_handler);




typedef struct {
    dnx_fabric_db_modids_to_group_map_nof_modids_set_cb set;
    dnx_fabric_db_modids_to_group_map_nof_modids_get_cb get;
} dnx_fabric_db_modids_to_group_map_nof_modids_cbs;


typedef struct {
    dnx_fabric_db_modids_to_group_map_modids_set_cb set;
    dnx_fabric_db_modids_to_group_map_modids_get_cb get;
    dnx_fabric_db_modids_to_group_map_modids_alloc_cb alloc;
} dnx_fabric_db_modids_to_group_map_modids_cbs;


typedef struct {
    dnx_fabric_db_modids_to_group_map_alloc_cb alloc;
    
    dnx_fabric_db_modids_to_group_map_nof_modids_cbs nof_modids;
    
    dnx_fabric_db_modids_to_group_map_modids_cbs modids;
} dnx_fabric_db_modids_to_group_map_cbs;


typedef struct {
    dnx_fabric_db_links_is_link_allowed_set_cb set;
    dnx_fabric_db_links_is_link_allowed_get_cb get;
} dnx_fabric_db_links_is_link_allowed_cbs;


typedef struct {
    dnx_fabric_db_links_is_link_isolated_set_cb set;
    dnx_fabric_db_links_is_link_isolated_get_cb get;
} dnx_fabric_db_links_is_link_isolated_cbs;


typedef struct {
    dnx_fabric_db_links_alloc_cb alloc;
    
    dnx_fabric_db_links_is_link_allowed_cbs is_link_allowed;
    
    dnx_fabric_db_links_is_link_isolated_cbs is_link_isolated;
} dnx_fabric_db_links_cbs;


typedef struct {
    dnx_fabric_db_load_balancing_periodic_event_handler_set_cb set;
    dnx_fabric_db_load_balancing_periodic_event_handler_get_cb get;
} dnx_fabric_db_load_balancing_periodic_event_handler_cbs;


typedef struct {
    dnx_fabric_db_is_init_cb is_init;
    dnx_fabric_db_init_cb init;
    
    dnx_fabric_db_modids_to_group_map_cbs modids_to_group_map;
    
    dnx_fabric_db_links_cbs links;
    
    dnx_fabric_db_load_balancing_periodic_event_handler_cbs load_balancing_periodic_event_handler;
} dnx_fabric_db_cbs;





extern dnx_fabric_db_cbs dnx_fabric_db;

#endif 
