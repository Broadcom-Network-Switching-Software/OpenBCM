/** \file algo/swstate/auto_generated/access/algo_l2_access.h
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

#ifndef __ALGO_L2_ACCESS_H__
#define __ALGO_L2_ACCESS_H__

#include <bcm_int/dnx/algo/swstate/auto_generated/types/algo_l2_types.h>
#include <bcm_int/dnx/algo/l2/algo_l2.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
/*
 * TYPEDEFs
 */

/**
 * implemented by: algo_l2_db_is_init
 */
typedef int (*algo_l2_db_is_init_cb)(
    int unit, uint8 *is_init);

/**
 * implemented by: algo_l2_db_init
 */
typedef int (*algo_l2_db_init_cb)(
    int unit);

/**
 * implemented by: algo_l2_db_event_forwarding_profile_set
 */
typedef int (*algo_l2_db_event_forwarding_profile_set_cb)(
    int unit, dnx_algo_template_t event_forwarding_profile);

/**
 * implemented by: algo_l2_db_event_forwarding_profile_get
 */
typedef int (*algo_l2_db_event_forwarding_profile_get_cb)(
    int unit, dnx_algo_template_t *event_forwarding_profile);

/**
 * implemented by: algo_l2_db_event_forwarding_profile_create
 */
typedef int (*algo_l2_db_event_forwarding_profile_create_cb)(
    int unit, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: algo_l2_db_event_forwarding_profile_allocate_single
 */
typedef int (*algo_l2_db_event_forwarding_profile_allocate_single_cb)(
    int unit, uint32 flags, dnx_event_forwarding_profile_info_t *profile_data, void *extra_arguments, int *profile, uint8 *first_reference);

/**
 * implemented by: algo_l2_db_event_forwarding_profile_free_single
 */
typedef int (*algo_l2_db_event_forwarding_profile_free_single_cb)(
    int unit, int profile, uint8 *last_reference);

/**
 * implemented by: algo_l2_db_event_forwarding_profile_replace_data
 */
typedef int (*algo_l2_db_event_forwarding_profile_replace_data_cb)(
    int unit, int profile,  const dnx_event_forwarding_profile_info_t *new_profile_data);

/**
 * implemented by: algo_l2_db_event_forwarding_profile_profile_data_get
 */
typedef int (*algo_l2_db_event_forwarding_profile_profile_data_get_cb)(
    int unit, int profile, int *ref_count, dnx_event_forwarding_profile_info_t *profile_data);

/**
 * implemented by: algo_l2_db_event_forwarding_profile_clear
 */
typedef int (*algo_l2_db_event_forwarding_profile_clear_cb)(
    int unit);

/**
 * implemented by: algo_l2_db_vsi_learning_profile_set
 */
typedef int (*algo_l2_db_vsi_learning_profile_set_cb)(
    int unit, dnx_algo_template_t vsi_learning_profile);

/**
 * implemented by: algo_l2_db_vsi_learning_profile_get
 */
typedef int (*algo_l2_db_vsi_learning_profile_get_cb)(
    int unit, dnx_algo_template_t *vsi_learning_profile);

/**
 * implemented by: algo_l2_db_vsi_learning_profile_create
 */
typedef int (*algo_l2_db_vsi_learning_profile_create_cb)(
    int unit, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: algo_l2_db_vsi_learning_profile_exchange
 */
typedef int (*algo_l2_db_vsi_learning_profile_exchange_cb)(
    int unit, uint32 flags, const dnx_vsi_learning_profile_info_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: algo_l2_db_vsi_learning_profile_replace_data
 */
typedef int (*algo_l2_db_vsi_learning_profile_replace_data_cb)(
    int unit, int profile,  const dnx_vsi_learning_profile_info_t *new_profile_data);

/**
 * implemented by: algo_l2_db_vsi_learning_profile_profile_data_get
 */
typedef int (*algo_l2_db_vsi_learning_profile_profile_data_get_cb)(
    int unit, int profile, int *ref_count, dnx_vsi_learning_profile_info_t *profile_data);

/**
 * implemented by: algo_l2_db_vsi_learning_profile_clear
 */
typedef int (*algo_l2_db_vsi_learning_profile_clear_cb)(
    int unit);

/**
 * implemented by: algo_l2_db_vsi_aging_profile_set
 */
typedef int (*algo_l2_db_vsi_aging_profile_set_cb)(
    int unit, dnx_algo_template_t vsi_aging_profile);

/**
 * implemented by: algo_l2_db_vsi_aging_profile_get
 */
typedef int (*algo_l2_db_vsi_aging_profile_get_cb)(
    int unit, dnx_algo_template_t *vsi_aging_profile);

/**
 * implemented by: algo_l2_db_vsi_aging_profile_create
 */
typedef int (*algo_l2_db_vsi_aging_profile_create_cb)(
    int unit, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: algo_l2_db_vsi_aging_profile_allocate_single
 */
typedef int (*algo_l2_db_vsi_aging_profile_allocate_single_cb)(
    int unit, uint32 flags, dnx_aging_profile_info_t *profile_data, void *extra_arguments, int *profile, uint8 *first_reference);

/**
 * implemented by: algo_l2_db_vsi_aging_profile_free_single
 */
typedef int (*algo_l2_db_vsi_aging_profile_free_single_cb)(
    int unit, int profile, uint8 *last_reference);

/**
 * implemented by: algo_l2_db_vsi_aging_profile_replace_data
 */
typedef int (*algo_l2_db_vsi_aging_profile_replace_data_cb)(
    int unit, int profile,  const dnx_aging_profile_info_t *new_profile_data);

/**
 * implemented by: algo_l2_db_vsi_aging_profile_profile_data_get
 */
typedef int (*algo_l2_db_vsi_aging_profile_profile_data_get_cb)(
    int unit, int profile, int *ref_count, dnx_aging_profile_info_t *profile_data);

/**
 * implemented by: algo_l2_db_vsi_aging_profile_clear
 */
typedef int (*algo_l2_db_vsi_aging_profile_clear_cb)(
    int unit);

/*
 * STRUCTs
 */

/**
 * This structure holds the access functions for the variable event_forwarding_profile
 */
typedef struct {
    algo_l2_db_event_forwarding_profile_set_cb set;
    algo_l2_db_event_forwarding_profile_get_cb get;
    algo_l2_db_event_forwarding_profile_create_cb create;
    algo_l2_db_event_forwarding_profile_allocate_single_cb allocate_single;
    algo_l2_db_event_forwarding_profile_free_single_cb free_single;
    algo_l2_db_event_forwarding_profile_replace_data_cb replace_data;
    algo_l2_db_event_forwarding_profile_profile_data_get_cb profile_data_get;
    algo_l2_db_event_forwarding_profile_clear_cb clear;
} algo_l2_db_event_forwarding_profile_cbs;

/**
 * This structure holds the access functions for the variable vsi_learning_profile
 */
typedef struct {
    algo_l2_db_vsi_learning_profile_set_cb set;
    algo_l2_db_vsi_learning_profile_get_cb get;
    algo_l2_db_vsi_learning_profile_create_cb create;
    algo_l2_db_vsi_learning_profile_exchange_cb exchange;
    algo_l2_db_vsi_learning_profile_replace_data_cb replace_data;
    algo_l2_db_vsi_learning_profile_profile_data_get_cb profile_data_get;
    algo_l2_db_vsi_learning_profile_clear_cb clear;
} algo_l2_db_vsi_learning_profile_cbs;

/**
 * This structure holds the access functions for the variable vsi_aging_profile
 */
typedef struct {
    algo_l2_db_vsi_aging_profile_set_cb set;
    algo_l2_db_vsi_aging_profile_get_cb get;
    algo_l2_db_vsi_aging_profile_create_cb create;
    algo_l2_db_vsi_aging_profile_allocate_single_cb allocate_single;
    algo_l2_db_vsi_aging_profile_free_single_cb free_single;
    algo_l2_db_vsi_aging_profile_replace_data_cb replace_data;
    algo_l2_db_vsi_aging_profile_profile_data_get_cb profile_data_get;
    algo_l2_db_vsi_aging_profile_clear_cb clear;
} algo_l2_db_vsi_aging_profile_cbs;

/**
 * This structure holds the access functions for the variable algo_l2_sw_state_t
 */
typedef struct {
    algo_l2_db_is_init_cb is_init;
    algo_l2_db_init_cb init;
    /**
     * Access struct for event_forwarding_profile
     */
    algo_l2_db_event_forwarding_profile_cbs event_forwarding_profile;
    /**
     * Access struct for vsi_learning_profile
     */
    algo_l2_db_vsi_learning_profile_cbs vsi_learning_profile;
    /**
     * Access struct for vsi_aging_profile
     */
    algo_l2_db_vsi_aging_profile_cbs vsi_aging_profile;
} algo_l2_db_cbs;

/*
 * Global Variables
 */

/*
 * Global Variables
 */

extern algo_l2_db_cbs algo_l2_db;

#endif /* __ALGO_L2_ACCESS_H__ */
