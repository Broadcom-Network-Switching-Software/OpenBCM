/** \file algo/swstate/auto_generated/access/phantom_queues_access.h
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

#ifndef __PHANTOM_QUEUES_ACCESS_H__
#define __PHANTOM_QUEUES_ACCESS_H__

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/swstate/auto_generated/types/phantom_queues_types.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
/*
 * TYPEDEFs
 */

/**
 * implemented by: dnx_phantom_queues_db_is_init
 */
typedef int (*dnx_phantom_queues_db_is_init_cb)(
    int unit, uint8 *is_init);

/**
 * implemented by: dnx_phantom_queues_db_init
 */
typedef int (*dnx_phantom_queues_db_init_cb)(
    int unit);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_set
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_set_cb)(
    int unit, uint32 phantom_queues_idx_0, dnx_algo_template_t phantom_queues);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_get
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_get_cb)(
    int unit, uint32 phantom_queues_idx_0, dnx_algo_template_t *phantom_queues);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_create
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_create_cb)(
    int unit, uint32 phantom_queues_idx_0, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_allocate_single
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_allocate_single_cb)(
    int unit, uint32 phantom_queues_idx_0, uint32 flags, uint32 *profile_data, void *extra_arguments, int *profile, uint8 *first_reference);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_free_single
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_free_single_cb)(
    int unit, uint32 phantom_queues_idx_0, int profile, uint8 *last_reference);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_exchange
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_exchange_cb)(
    int unit, uint32 phantom_queues_idx_0, uint32 flags, const uint32 *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_replace_data
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_replace_data_cb)(
    int unit, uint32 phantom_queues_idx_0, int profile,  const uint32 *new_profile_data);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_profile_data_get
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_profile_data_get_cb)(
    int unit, uint32 phantom_queues_idx_0, int profile, int *ref_count, uint32 *profile_data);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_profile_get
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_profile_get_cb)(
    int unit, uint32 phantom_queues_idx_0, const uint32 *profile_data, int *profile);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_get_next
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_get_next_cb)(
    int unit, uint32 phantom_queues_idx_0, int *current_profile);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_dump_info_get
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_dump_info_get_cb)(
    int unit, uint32 phantom_queues_idx_0, dnx_algo_template_dump_data_t * info);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_advanced_algorithm_info_set
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_advanced_algorithm_info_set_cb)(
    int unit, uint32 phantom_queues_idx_0, void *algorithm_info);

/**
 * implemented by: dnx_phantom_queues_db_phantom_queues_advanced_algorithm_info_get
 */
typedef int (*dnx_phantom_queues_db_phantom_queues_advanced_algorithm_info_get_cb)(
    int unit, uint32 phantom_queues_idx_0, void *algorithm_info);

/*
 * STRUCTs
 */

/**
 * This structure holds the access functions for the variable phantom_queues
 */
typedef struct {
    dnx_phantom_queues_db_phantom_queues_set_cb set;
    dnx_phantom_queues_db_phantom_queues_get_cb get;
    dnx_phantom_queues_db_phantom_queues_create_cb create;
    dnx_phantom_queues_db_phantom_queues_allocate_single_cb allocate_single;
    dnx_phantom_queues_db_phantom_queues_free_single_cb free_single;
    dnx_phantom_queues_db_phantom_queues_exchange_cb exchange;
    dnx_phantom_queues_db_phantom_queues_replace_data_cb replace_data;
    dnx_phantom_queues_db_phantom_queues_profile_data_get_cb profile_data_get;
    dnx_phantom_queues_db_phantom_queues_profile_get_cb profile_get;
    dnx_phantom_queues_db_phantom_queues_get_next_cb get_next;
    dnx_phantom_queues_db_phantom_queues_dump_info_get_cb dump_info_get;
    dnx_phantom_queues_db_phantom_queues_advanced_algorithm_info_set_cb advanced_algorithm_info_set;
    dnx_phantom_queues_db_phantom_queues_advanced_algorithm_info_get_cb advanced_algorithm_info_get;
} dnx_phantom_queues_db_phantom_queues_cbs;

/**
 * This structure holds the access functions for the variable dnx_phantom_queues_db_t
 */
typedef struct {
    dnx_phantom_queues_db_is_init_cb is_init;
    dnx_phantom_queues_db_init_cb init;
    /**
     * Access struct for phantom_queues
     */
    dnx_phantom_queues_db_phantom_queues_cbs phantom_queues;
} dnx_phantom_queues_db_cbs;

/*
 * Global Variables
 */

/*
 * Global Variables
 */

extern dnx_phantom_queues_db_cbs dnx_phantom_queues_db;
#endif /* BCM_DNX_SUPPORT*/ 

#endif /* __PHANTOM_QUEUES_ACCESS_H__ */
