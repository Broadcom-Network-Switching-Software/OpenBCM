/** \file dnx/swstate/auto_generated/access/dnx_fabric_access.h
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

#ifndef __DNX_FABRIC_ACCESS_H__
#define __DNX_FABRIC_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_fabric_types.h>
#include <include/shared/shr_thread_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
/*
 * TYPEDEFs
 */

/**
 * implemented by: dnx_fabric_db_is_init
 */
typedef int (*dnx_fabric_db_is_init_cb)(
    int unit, uint8 *is_init);

/**
 * implemented by: dnx_fabric_db_init
 */
typedef int (*dnx_fabric_db_init_cb)(
    int unit);

/**
 * implemented by: dnx_fabric_db_modids_to_group_map_alloc
 */
typedef int (*dnx_fabric_db_modids_to_group_map_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_fabric_db_modids_to_group_map_nof_modids_set
 */
typedef int (*dnx_fabric_db_modids_to_group_map_nof_modids_set_cb)(
    int unit, uint32 modids_to_group_map_idx_0, int nof_modids);

/**
 * implemented by: dnx_fabric_db_modids_to_group_map_nof_modids_get
 */
typedef int (*dnx_fabric_db_modids_to_group_map_nof_modids_get_cb)(
    int unit, uint32 modids_to_group_map_idx_0, int *nof_modids);

/**
 * implemented by: dnx_fabric_db_modids_to_group_map_modids_set
 */
typedef int (*dnx_fabric_db_modids_to_group_map_modids_set_cb)(
    int unit, uint32 modids_to_group_map_idx_0, uint32 modids_idx_0, int modids);

/**
 * implemented by: dnx_fabric_db_modids_to_group_map_modids_get
 */
typedef int (*dnx_fabric_db_modids_to_group_map_modids_get_cb)(
    int unit, uint32 modids_to_group_map_idx_0, uint32 modids_idx_0, int *modids);

/**
 * implemented by: dnx_fabric_db_modids_to_group_map_modids_alloc
 */
typedef int (*dnx_fabric_db_modids_to_group_map_modids_alloc_cb)(
    int unit, uint32 modids_to_group_map_idx_0);

/**
 * implemented by: dnx_fabric_db_links_alloc
 */
typedef int (*dnx_fabric_db_links_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_fabric_db_links_is_link_allowed_set
 */
typedef int (*dnx_fabric_db_links_is_link_allowed_set_cb)(
    int unit, uint32 links_idx_0, int is_link_allowed);

/**
 * implemented by: dnx_fabric_db_links_is_link_allowed_get
 */
typedef int (*dnx_fabric_db_links_is_link_allowed_get_cb)(
    int unit, uint32 links_idx_0, int *is_link_allowed);

/**
 * implemented by: dnx_fabric_db_links_is_link_isolated_set
 */
typedef int (*dnx_fabric_db_links_is_link_isolated_set_cb)(
    int unit, uint32 links_idx_0, int is_link_isolated);

/**
 * implemented by: dnx_fabric_db_links_is_link_isolated_get
 */
typedef int (*dnx_fabric_db_links_is_link_isolated_get_cb)(
    int unit, uint32 links_idx_0, int *is_link_isolated);

/**
 * implemented by: dnx_fabric_db_load_balancing_shr_thread_manager_handler_set
 */
typedef int (*dnx_fabric_db_load_balancing_shr_thread_manager_handler_set_cb)(
    int unit, shr_thread_manager_handler_t load_balancing_shr_thread_manager_handler);

/**
 * implemented by: dnx_fabric_db_load_balancing_shr_thread_manager_handler_get
 */
typedef int (*dnx_fabric_db_load_balancing_shr_thread_manager_handler_get_cb)(
    int unit, shr_thread_manager_handler_t *load_balancing_shr_thread_manager_handler);

/**
 * implemented by: dnx_fabric_db_system_upgrade_state_0_set
 */
typedef int (*dnx_fabric_db_system_upgrade_state_0_set_cb)(
    int unit, uint32 state_0);

/**
 * implemented by: dnx_fabric_db_system_upgrade_state_0_get
 */
typedef int (*dnx_fabric_db_system_upgrade_state_0_get_cb)(
    int unit, uint32 *state_0);

/**
 * implemented by: dnx_fabric_db_system_upgrade_state_1_set
 */
typedef int (*dnx_fabric_db_system_upgrade_state_1_set_cb)(
    int unit, uint32 state_1);

/**
 * implemented by: dnx_fabric_db_system_upgrade_state_1_get
 */
typedef int (*dnx_fabric_db_system_upgrade_state_1_get_cb)(
    int unit, uint32 *state_1);

/**
 * implemented by: dnx_fabric_db_system_upgrade_state_2_set
 */
typedef int (*dnx_fabric_db_system_upgrade_state_2_set_cb)(
    int unit, uint32 state_2);

/**
 * implemented by: dnx_fabric_db_system_upgrade_state_2_get
 */
typedef int (*dnx_fabric_db_system_upgrade_state_2_get_cb)(
    int unit, uint32 *state_2);

/*
 * STRUCTs
 */

/**
 * This structure holds the access functions for the variable nof_modids
 */
typedef struct {
    dnx_fabric_db_modids_to_group_map_nof_modids_set_cb set;
    dnx_fabric_db_modids_to_group_map_nof_modids_get_cb get;
} dnx_fabric_db_modids_to_group_map_nof_modids_cbs;

/**
 * This structure holds the access functions for the variable modids
 */
typedef struct {
    dnx_fabric_db_modids_to_group_map_modids_set_cb set;
    dnx_fabric_db_modids_to_group_map_modids_get_cb get;
    dnx_fabric_db_modids_to_group_map_modids_alloc_cb alloc;
} dnx_fabric_db_modids_to_group_map_modids_cbs;

/**
 * This structure holds the access functions for the variable dnx_fabric_modids_group_map_t
 */
typedef struct {
    dnx_fabric_db_modids_to_group_map_alloc_cb alloc;
    /**
     * Access struct for nof_modids
     */
    dnx_fabric_db_modids_to_group_map_nof_modids_cbs nof_modids;
    /**
     * Access struct for modids
     */
    dnx_fabric_db_modids_to_group_map_modids_cbs modids;
} dnx_fabric_db_modids_to_group_map_cbs;

/**
 * This structure holds the access functions for the variable is_link_allowed
 */
typedef struct {
    dnx_fabric_db_links_is_link_allowed_set_cb set;
    dnx_fabric_db_links_is_link_allowed_get_cb get;
} dnx_fabric_db_links_is_link_allowed_cbs;

/**
 * This structure holds the access functions for the variable is_link_isolated
 */
typedef struct {
    dnx_fabric_db_links_is_link_isolated_set_cb set;
    dnx_fabric_db_links_is_link_isolated_get_cb get;
} dnx_fabric_db_links_is_link_isolated_cbs;

/**
 * This structure holds the access functions for the variable dnx_fabric_link_t
 */
typedef struct {
    dnx_fabric_db_links_alloc_cb alloc;
    /**
     * Access struct for is_link_allowed
     */
    dnx_fabric_db_links_is_link_allowed_cbs is_link_allowed;
    /**
     * Access struct for is_link_isolated
     */
    dnx_fabric_db_links_is_link_isolated_cbs is_link_isolated;
} dnx_fabric_db_links_cbs;

/**
 * This structure holds the access functions for the variable load_balancing_shr_thread_manager_handler
 */
typedef struct {
    dnx_fabric_db_load_balancing_shr_thread_manager_handler_set_cb set;
    dnx_fabric_db_load_balancing_shr_thread_manager_handler_get_cb get;
} dnx_fabric_db_load_balancing_shr_thread_manager_handler_cbs;

/**
 * This structure holds the access functions for the variable state_0
 */
typedef struct {
    dnx_fabric_db_system_upgrade_state_0_set_cb set;
    dnx_fabric_db_system_upgrade_state_0_get_cb get;
} dnx_fabric_db_system_upgrade_state_0_cbs;

/**
 * This structure holds the access functions for the variable state_1
 */
typedef struct {
    dnx_fabric_db_system_upgrade_state_1_set_cb set;
    dnx_fabric_db_system_upgrade_state_1_get_cb get;
} dnx_fabric_db_system_upgrade_state_1_cbs;

/**
 * This structure holds the access functions for the variable state_2
 */
typedef struct {
    dnx_fabric_db_system_upgrade_state_2_set_cb set;
    dnx_fabric_db_system_upgrade_state_2_get_cb get;
} dnx_fabric_db_system_upgrade_state_2_cbs;

/**
 * This structure holds the access functions for the variable dnx_fabric_system_upgrade_t
 */
typedef struct {
    /**
     * Access struct for state_0
     */
    dnx_fabric_db_system_upgrade_state_0_cbs state_0;
    /**
     * Access struct for state_1
     */
    dnx_fabric_db_system_upgrade_state_1_cbs state_1;
    /**
     * Access struct for state_2
     */
    dnx_fabric_db_system_upgrade_state_2_cbs state_2;
} dnx_fabric_db_system_upgrade_cbs;

/**
 * This structure holds the access functions for the variable dnx_fabric_db_t
 */
typedef struct {
    dnx_fabric_db_is_init_cb is_init;
    dnx_fabric_db_init_cb init;
    /**
     * Access struct for modids_to_group_map
     */
    dnx_fabric_db_modids_to_group_map_cbs modids_to_group_map;
    /**
     * Access struct for links
     */
    dnx_fabric_db_links_cbs links;
    /**
     * Access struct for load_balancing_shr_thread_manager_handler
     */
    dnx_fabric_db_load_balancing_shr_thread_manager_handler_cbs load_balancing_shr_thread_manager_handler;
    /**
     * Access struct for system_upgrade
     */
    dnx_fabric_db_system_upgrade_cbs system_upgrade;
} dnx_fabric_db_cbs;

/*
 * Global Variables
 */

/*
 * Global Variables
 */

extern dnx_fabric_db_cbs dnx_fabric_db;

#endif /* __DNX_FABRIC_ACCESS_H__ */
