/** \file algo/swstate/auto_generated/access/algo_lif_profile_access.h
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

#ifndef __ALGO_LIF_PROFILE_ACCESS_H__
#define __ALGO_LIF_PROFILE_ACCESS_H__

#include <bcm_int/dnx/algo/swstate/auto_generated/types/algo_lif_profile_types.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_in_lif_profile.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_out_lif_profile.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
/*
 * TYPEDEFs
 */

/**
 * implemented by: algo_lif_profile_db_is_init
 */
typedef int (*algo_lif_profile_db_is_init_cb)(
    int unit, uint8 *is_init);

/**
 * implemented by: algo_lif_profile_db_init
 */
typedef int (*algo_lif_profile_db_init_cb)(
    int unit);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_l3_enablers_set
 */
typedef int (*algo_lif_profile_db_in_lif_profile_l3_enablers_set_cb)(
    int unit, dnx_algo_template_t in_lif_profile_l3_enablers);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_l3_enablers_get
 */
typedef int (*algo_lif_profile_db_in_lif_profile_l3_enablers_get_cb)(
    int unit, dnx_algo_template_t *in_lif_profile_l3_enablers);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_l3_enablers_create
 */
typedef int (*algo_lif_profile_db_in_lif_profile_l3_enablers_create_cb)(
    int unit, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_l3_enablers_exchange
 */
typedef int (*algo_lif_profile_db_in_lif_profile_l3_enablers_exchange_cb)(
    int unit, uint32 flags, const uint32 *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_l3_enablers_profile_data_get
 */
typedef int (*algo_lif_profile_db_in_lif_profile_l3_enablers_profile_data_get_cb)(
    int unit, int profile, int *ref_count, uint32 *profile_data);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_l3_enablers_clear
 */
typedef int (*algo_lif_profile_db_in_lif_profile_l3_enablers_clear_cb)(
    int unit);

/**
 * implemented by: algo_lif_profile_db_in_lif_lb_profile_enablers_set
 */
typedef int (*algo_lif_profile_db_in_lif_lb_profile_enablers_set_cb)(
    int unit, dnx_algo_template_t in_lif_lb_profile_enablers);

/**
 * implemented by: algo_lif_profile_db_in_lif_lb_profile_enablers_get
 */
typedef int (*algo_lif_profile_db_in_lif_lb_profile_enablers_get_cb)(
    int unit, dnx_algo_template_t *in_lif_lb_profile_enablers);

/**
 * implemented by: algo_lif_profile_db_in_lif_lb_profile_enablers_create
 */
typedef int (*algo_lif_profile_db_in_lif_lb_profile_enablers_create_cb)(
    int unit, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: algo_lif_profile_db_in_lif_lb_profile_enablers_exchange
 */
typedef int (*algo_lif_profile_db_in_lif_lb_profile_enablers_exchange_cb)(
    int unit, uint32 flags, const uint32 *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: algo_lif_profile_db_in_lif_lb_profile_enablers_profile_data_get
 */
typedef int (*algo_lif_profile_db_in_lif_lb_profile_enablers_profile_data_get_cb)(
    int unit, int profile, int *ref_count, uint32 *profile_data);

/**
 * implemented by: algo_lif_profile_db_in_lif_lb_profile_enablers_clear
 */
typedef int (*algo_lif_profile_db_in_lif_lb_profile_enablers_clear_cb)(
    int unit);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_set
 */
typedef int (*algo_lif_profile_db_in_lif_profile_set_cb)(
    int unit, dnx_algo_template_t in_lif_profile);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_get
 */
typedef int (*algo_lif_profile_db_in_lif_profile_get_cb)(
    int unit, dnx_algo_template_t *in_lif_profile);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_create
 */
typedef int (*algo_lif_profile_db_in_lif_profile_create_cb)(
    int unit, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_exchange
 */
typedef int (*algo_lif_profile_db_in_lif_profile_exchange_cb)(
    int unit, uint32 flags, const in_lif_template_data_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_profile_data_get
 */
typedef int (*algo_lif_profile_db_in_lif_profile_profile_data_get_cb)(
    int unit, int profile, int *ref_count, in_lif_template_data_t *profile_data);

/**
 * implemented by: algo_lif_profile_db_in_lif_profile_clear
 */
typedef int (*algo_lif_profile_db_in_lif_profile_clear_cb)(
    int unit);

/**
 * implemented by: algo_lif_profile_db_eth_rif_profile_set
 */
typedef int (*algo_lif_profile_db_eth_rif_profile_set_cb)(
    int unit, dnx_algo_template_t eth_rif_profile);

/**
 * implemented by: algo_lif_profile_db_eth_rif_profile_get
 */
typedef int (*algo_lif_profile_db_eth_rif_profile_get_cb)(
    int unit, dnx_algo_template_t *eth_rif_profile);

/**
 * implemented by: algo_lif_profile_db_eth_rif_profile_create
 */
typedef int (*algo_lif_profile_db_eth_rif_profile_create_cb)(
    int unit, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: algo_lif_profile_db_eth_rif_profile_exchange
 */
typedef int (*algo_lif_profile_db_eth_rif_profile_exchange_cb)(
    int unit, uint32 flags, const in_lif_template_data_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: algo_lif_profile_db_eth_rif_profile_profile_data_get
 */
typedef int (*algo_lif_profile_db_eth_rif_profile_profile_data_get_cb)(
    int unit, int profile, int *ref_count, in_lif_template_data_t *profile_data);

/**
 * implemented by: algo_lif_profile_db_eth_rif_profile_clear
 */
typedef int (*algo_lif_profile_db_eth_rif_profile_clear_cb)(
    int unit);

/**
 * implemented by: algo_lif_profile_db_in_lif_da_not_found_destination_set
 */
typedef int (*algo_lif_profile_db_in_lif_da_not_found_destination_set_cb)(
    int unit, dnx_algo_template_t in_lif_da_not_found_destination);

/**
 * implemented by: algo_lif_profile_db_in_lif_da_not_found_destination_get
 */
typedef int (*algo_lif_profile_db_in_lif_da_not_found_destination_get_cb)(
    int unit, dnx_algo_template_t *in_lif_da_not_found_destination);

/**
 * implemented by: algo_lif_profile_db_in_lif_da_not_found_destination_create
 */
typedef int (*algo_lif_profile_db_in_lif_da_not_found_destination_create_cb)(
    int unit, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: algo_lif_profile_db_in_lif_da_not_found_destination_exchange
 */
typedef int (*algo_lif_profile_db_in_lif_da_not_found_destination_exchange_cb)(
    int unit, uint32 flags, const dnx_default_frwrd_dst_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: algo_lif_profile_db_in_lif_da_not_found_destination_profile_data_get
 */
typedef int (*algo_lif_profile_db_in_lif_da_not_found_destination_profile_data_get_cb)(
    int unit, int profile, int *ref_count, dnx_default_frwrd_dst_t *profile_data);

/**
 * implemented by: algo_lif_profile_db_in_lif_da_not_found_destination_clear
 */
typedef int (*algo_lif_profile_db_in_lif_da_not_found_destination_clear_cb)(
    int unit);

/**
 * implemented by: algo_lif_profile_db_etpp_out_lif_profile_set
 */
typedef int (*algo_lif_profile_db_etpp_out_lif_profile_set_cb)(
    int unit, dnx_algo_template_t etpp_out_lif_profile);

/**
 * implemented by: algo_lif_profile_db_etpp_out_lif_profile_get
 */
typedef int (*algo_lif_profile_db_etpp_out_lif_profile_get_cb)(
    int unit, dnx_algo_template_t *etpp_out_lif_profile);

/**
 * implemented by: algo_lif_profile_db_etpp_out_lif_profile_create
 */
typedef int (*algo_lif_profile_db_etpp_out_lif_profile_create_cb)(
    int unit, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: algo_lif_profile_db_etpp_out_lif_profile_exchange
 */
typedef int (*algo_lif_profile_db_etpp_out_lif_profile_exchange_cb)(
    int unit, uint32 flags, const etpp_out_lif_template_data_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: algo_lif_profile_db_etpp_out_lif_profile_profile_data_get
 */
typedef int (*algo_lif_profile_db_etpp_out_lif_profile_profile_data_get_cb)(
    int unit, int profile, int *ref_count, etpp_out_lif_template_data_t *profile_data);

/**
 * implemented by: algo_lif_profile_db_etpp_out_lif_profile_clear
 */
typedef int (*algo_lif_profile_db_etpp_out_lif_profile_clear_cb)(
    int unit);

/**
 * implemented by: algo_lif_profile_db_etpp_out_lif_profile_advanced_algorithm_info_set
 */
typedef int (*algo_lif_profile_db_etpp_out_lif_profile_advanced_algorithm_info_set_cb)(
    int unit, void *algorithm_info);

/**
 * implemented by: algo_lif_profile_db_etpp_out_lif_profile_advanced_algorithm_info_get
 */
typedef int (*algo_lif_profile_db_etpp_out_lif_profile_advanced_algorithm_info_get_cb)(
    int unit, void *algorithm_info);

/**
 * implemented by: algo_lif_profile_db_erpp_out_lif_profile_set
 */
typedef int (*algo_lif_profile_db_erpp_out_lif_profile_set_cb)(
    int unit, dnx_algo_template_t erpp_out_lif_profile);

/**
 * implemented by: algo_lif_profile_db_erpp_out_lif_profile_get
 */
typedef int (*algo_lif_profile_db_erpp_out_lif_profile_get_cb)(
    int unit, dnx_algo_template_t *erpp_out_lif_profile);

/**
 * implemented by: algo_lif_profile_db_erpp_out_lif_profile_create
 */
typedef int (*algo_lif_profile_db_erpp_out_lif_profile_create_cb)(
    int unit, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: algo_lif_profile_db_erpp_out_lif_profile_exchange
 */
typedef int (*algo_lif_profile_db_erpp_out_lif_profile_exchange_cb)(
    int unit, uint32 flags, const erpp_out_lif_template_data_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: algo_lif_profile_db_erpp_out_lif_profile_profile_data_get
 */
typedef int (*algo_lif_profile_db_erpp_out_lif_profile_profile_data_get_cb)(
    int unit, int profile, int *ref_count, erpp_out_lif_template_data_t *profile_data);

/**
 * implemented by: algo_lif_profile_db_erpp_out_lif_profile_clear
 */
typedef int (*algo_lif_profile_db_erpp_out_lif_profile_clear_cb)(
    int unit);

/*
 * STRUCTs
 */

/**
 * This structure holds the access functions for the variable in_lif_profile_l3_enablers
 */
typedef struct {
    algo_lif_profile_db_in_lif_profile_l3_enablers_set_cb set;
    algo_lif_profile_db_in_lif_profile_l3_enablers_get_cb get;
    algo_lif_profile_db_in_lif_profile_l3_enablers_create_cb create;
    algo_lif_profile_db_in_lif_profile_l3_enablers_exchange_cb exchange;
    algo_lif_profile_db_in_lif_profile_l3_enablers_profile_data_get_cb profile_data_get;
    algo_lif_profile_db_in_lif_profile_l3_enablers_clear_cb clear;
} algo_lif_profile_db_in_lif_profile_l3_enablers_cbs;

/**
 * This structure holds the access functions for the variable in_lif_lb_profile_enablers
 */
typedef struct {
    algo_lif_profile_db_in_lif_lb_profile_enablers_set_cb set;
    algo_lif_profile_db_in_lif_lb_profile_enablers_get_cb get;
    algo_lif_profile_db_in_lif_lb_profile_enablers_create_cb create;
    algo_lif_profile_db_in_lif_lb_profile_enablers_exchange_cb exchange;
    algo_lif_profile_db_in_lif_lb_profile_enablers_profile_data_get_cb profile_data_get;
    algo_lif_profile_db_in_lif_lb_profile_enablers_clear_cb clear;
} algo_lif_profile_db_in_lif_lb_profile_enablers_cbs;

/**
 * This structure holds the access functions for the variable in_lif_profile
 */
typedef struct {
    algo_lif_profile_db_in_lif_profile_set_cb set;
    algo_lif_profile_db_in_lif_profile_get_cb get;
    algo_lif_profile_db_in_lif_profile_create_cb create;
    algo_lif_profile_db_in_lif_profile_exchange_cb exchange;
    algo_lif_profile_db_in_lif_profile_profile_data_get_cb profile_data_get;
    algo_lif_profile_db_in_lif_profile_clear_cb clear;
} algo_lif_profile_db_in_lif_profile_cbs;

/**
 * This structure holds the access functions for the variable eth_rif_profile
 */
typedef struct {
    algo_lif_profile_db_eth_rif_profile_set_cb set;
    algo_lif_profile_db_eth_rif_profile_get_cb get;
    algo_lif_profile_db_eth_rif_profile_create_cb create;
    algo_lif_profile_db_eth_rif_profile_exchange_cb exchange;
    algo_lif_profile_db_eth_rif_profile_profile_data_get_cb profile_data_get;
    algo_lif_profile_db_eth_rif_profile_clear_cb clear;
} algo_lif_profile_db_eth_rif_profile_cbs;

/**
 * This structure holds the access functions for the variable in_lif_da_not_found_destination
 */
typedef struct {
    algo_lif_profile_db_in_lif_da_not_found_destination_set_cb set;
    algo_lif_profile_db_in_lif_da_not_found_destination_get_cb get;
    algo_lif_profile_db_in_lif_da_not_found_destination_create_cb create;
    algo_lif_profile_db_in_lif_da_not_found_destination_exchange_cb exchange;
    algo_lif_profile_db_in_lif_da_not_found_destination_profile_data_get_cb profile_data_get;
    algo_lif_profile_db_in_lif_da_not_found_destination_clear_cb clear;
} algo_lif_profile_db_in_lif_da_not_found_destination_cbs;

/**
 * This structure holds the access functions for the variable etpp_out_lif_profile
 */
typedef struct {
    algo_lif_profile_db_etpp_out_lif_profile_set_cb set;
    algo_lif_profile_db_etpp_out_lif_profile_get_cb get;
    algo_lif_profile_db_etpp_out_lif_profile_create_cb create;
    algo_lif_profile_db_etpp_out_lif_profile_exchange_cb exchange;
    algo_lif_profile_db_etpp_out_lif_profile_profile_data_get_cb profile_data_get;
    algo_lif_profile_db_etpp_out_lif_profile_clear_cb clear;
    algo_lif_profile_db_etpp_out_lif_profile_advanced_algorithm_info_set_cb advanced_algorithm_info_set;
    algo_lif_profile_db_etpp_out_lif_profile_advanced_algorithm_info_get_cb advanced_algorithm_info_get;
} algo_lif_profile_db_etpp_out_lif_profile_cbs;

/**
 * This structure holds the access functions for the variable erpp_out_lif_profile
 */
typedef struct {
    algo_lif_profile_db_erpp_out_lif_profile_set_cb set;
    algo_lif_profile_db_erpp_out_lif_profile_get_cb get;
    algo_lif_profile_db_erpp_out_lif_profile_create_cb create;
    algo_lif_profile_db_erpp_out_lif_profile_exchange_cb exchange;
    algo_lif_profile_db_erpp_out_lif_profile_profile_data_get_cb profile_data_get;
    algo_lif_profile_db_erpp_out_lif_profile_clear_cb clear;
} algo_lif_profile_db_erpp_out_lif_profile_cbs;

/**
 * This structure holds the access functions for the variable algo_lif_profile_sw_state_t
 */
typedef struct {
    algo_lif_profile_db_is_init_cb is_init;
    algo_lif_profile_db_init_cb init;
    /**
     * Access struct for in_lif_profile_l3_enablers
     */
    algo_lif_profile_db_in_lif_profile_l3_enablers_cbs in_lif_profile_l3_enablers;
    /**
     * Access struct for in_lif_lb_profile_enablers
     */
    algo_lif_profile_db_in_lif_lb_profile_enablers_cbs in_lif_lb_profile_enablers;
    /**
     * Access struct for in_lif_profile
     */
    algo_lif_profile_db_in_lif_profile_cbs in_lif_profile;
    /**
     * Access struct for eth_rif_profile
     */
    algo_lif_profile_db_eth_rif_profile_cbs eth_rif_profile;
    /**
     * Access struct for in_lif_da_not_found_destination
     */
    algo_lif_profile_db_in_lif_da_not_found_destination_cbs in_lif_da_not_found_destination;
    /**
     * Access struct for etpp_out_lif_profile
     */
    algo_lif_profile_db_etpp_out_lif_profile_cbs etpp_out_lif_profile;
    /**
     * Access struct for erpp_out_lif_profile
     */
    algo_lif_profile_db_erpp_out_lif_profile_cbs erpp_out_lif_profile;
} algo_lif_profile_db_cbs;

/*
 * Global Variables
 */

/*
 * Global Variables
 */

extern algo_lif_profile_db_cbs algo_lif_profile_db;

#endif /* __ALGO_LIF_PROFILE_ACCESS_H__ */
