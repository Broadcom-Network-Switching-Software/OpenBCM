/** \file algo/swstate/auto_generated/access/dnx_mirror_access.h
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

#ifndef __DNX_MIRROR_ACCESS_H__
#define __DNX_MIRROR_ACCESS_H__

#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_mirror_types.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_internal.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <bcm_int/dnx/mirror/mirror.h>
#include <bcm_int/dnx/mirror/mirror_rcy.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>
/*
 * TYPEDEFs
 */

/**
 * implemented by: dnx_mirror_db_is_init
 */
typedef int (*dnx_mirror_db_is_init_cb)(
    int unit, uint8 *is_init);

/**
 * implemented by: dnx_mirror_db_init
 */
typedef int (*dnx_mirror_db_init_cb)(
    int unit);

/**
 * implemented by: dnx_mirror_db_channel_to_egress_if_set
 */
typedef int (*dnx_mirror_db_channel_to_egress_if_set_cb)(
    int unit, uint32 channel_to_egress_if_idx_0, uint32 channel_to_egress_if_idx_1, int channel_to_egress_if);

/**
 * implemented by: dnx_mirror_db_channel_to_egress_if_get
 */
typedef int (*dnx_mirror_db_channel_to_egress_if_get_cb)(
    int unit, uint32 channel_to_egress_if_idx_0, uint32 channel_to_egress_if_idx_1, int *channel_to_egress_if);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_set
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_set_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, bcm_pbmp_t egress_mirror_priority);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_get
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_get_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, bcm_pbmp_t *egress_mirror_priority);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_neq
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_neq_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, _shr_pbmp_t input_pbmp, uint8 *result);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_eq
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_eq_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, _shr_pbmp_t input_pbmp, uint8 *result);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_member
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_member_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, uint32 _input_port, uint8 *result);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_not_null
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_not_null_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, uint8 *result);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_is_null
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_is_null_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, uint8 *result);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_count
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_count_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, int *result);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_xor
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_xor_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, _shr_pbmp_t input_pbmp);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_remove
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_remove_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, _shr_pbmp_t input_pbmp);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_assign
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_assign_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, _shr_pbmp_t input_pbmp);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_get
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_get_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, _shr_pbmp_t *output_pbmp);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_and
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_and_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, _shr_pbmp_t input_pbmp);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_negate
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_negate_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, _shr_pbmp_t input_pbmp);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_or
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_or_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, _shr_pbmp_t input_pbmp);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_clear
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_clear_cb)(
    int unit, uint32 egress_mirror_priority_idx_0);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_port_add
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_port_add_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, uint32 _input_port);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_port_flip
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_port_flip_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, uint32 _input_port);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_port_remove
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_port_remove_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, uint32 _input_port);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_port_set
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_port_set_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, uint32 _input_port);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_ports_range_add
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_ports_range_add_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, uint32 _first_port, uint32 _range);

/**
 * implemented by: dnx_mirror_db_egress_mirror_priority_pbmp_fmt
 */
typedef int (*dnx_mirror_db_egress_mirror_priority_pbmp_fmt_cb)(
    int unit, uint32 egress_mirror_priority_idx_0, char* _buffer);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_mirror_set
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_mirror_set_cb)(
    int unit, dnx_algo_res_t snif_ingress_profiles_mirror);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_mirror_get
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_mirror_get_cb)(
    int unit, dnx_algo_res_t *snif_ingress_profiles_mirror);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_mirror_create
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_mirror_create_cb)(
    int unit, dnx_algo_res_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_mirror_allocate_single
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_mirror_allocate_single_cb)(
    int unit, uint32 flags, void *extra_arguments, int *element);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_mirror_free_single
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_mirror_free_single_cb)(
    int unit, int element, void *extra_arguments);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_mirror_is_allocated
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_mirror_is_allocated_cb)(
    int unit, int element, uint8 *is_allocated);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_snoop_set
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_snoop_set_cb)(
    int unit, dnx_algo_res_t snif_ingress_profiles_snoop);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_snoop_get
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_snoop_get_cb)(
    int unit, dnx_algo_res_t *snif_ingress_profiles_snoop);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_snoop_create
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_snoop_create_cb)(
    int unit, dnx_algo_res_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_snoop_allocate_single
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_snoop_allocate_single_cb)(
    int unit, uint32 flags, void *extra_arguments, int *element);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_snoop_free_single
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_snoop_free_single_cb)(
    int unit, int element, void *extra_arguments);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_snoop_is_allocated
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_snoop_is_allocated_cb)(
    int unit, int element, uint8 *is_allocated);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_stat_sampling_set
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_stat_sampling_set_cb)(
    int unit, dnx_algo_res_t snif_ingress_profiles_stat_sampling);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_stat_sampling_get
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_stat_sampling_get_cb)(
    int unit, dnx_algo_res_t *snif_ingress_profiles_stat_sampling);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_stat_sampling_create
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_stat_sampling_create_cb)(
    int unit, dnx_algo_res_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_stat_sampling_allocate_single
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_stat_sampling_allocate_single_cb)(
    int unit, uint32 flags, void *extra_arguments, int *element);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_stat_sampling_free_single
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_stat_sampling_free_single_cb)(
    int unit, int element, void *extra_arguments);

/**
 * implemented by: dnx_mirror_db_snif_ingress_profiles_stat_sampling_is_allocated
 */
typedef int (*dnx_mirror_db_snif_ingress_profiles_stat_sampling_is_allocated_cb)(
    int unit, int element, uint8 *is_allocated);

/**
 * implemented by: dnx_mirror_db_snif_egress_vlan_profiles_mirror_set
 */
typedef int (*dnx_mirror_db_snif_egress_vlan_profiles_mirror_set_cb)(
    int unit, uint32 snif_egress_vlan_profiles_mirror_idx_0, dnx_algo_template_t snif_egress_vlan_profiles_mirror);

/**
 * implemented by: dnx_mirror_db_snif_egress_vlan_profiles_mirror_get
 */
typedef int (*dnx_mirror_db_snif_egress_vlan_profiles_mirror_get_cb)(
    int unit, uint32 snif_egress_vlan_profiles_mirror_idx_0, dnx_algo_template_t *snif_egress_vlan_profiles_mirror);

/**
 * implemented by: dnx_mirror_db_snif_egress_vlan_profiles_mirror_alloc
 */
typedef int (*dnx_mirror_db_snif_egress_vlan_profiles_mirror_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_mirror_db_snif_egress_vlan_profiles_mirror_create
 */
typedef int (*dnx_mirror_db_snif_egress_vlan_profiles_mirror_create_cb)(
    int unit, uint32 snif_egress_vlan_profiles_mirror_idx_0, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_mirror_db_snif_egress_vlan_profiles_mirror_allocate_single
 */
typedef int (*dnx_mirror_db_snif_egress_vlan_profiles_mirror_allocate_single_cb)(
    int unit, uint32 snif_egress_vlan_profiles_mirror_idx_0, uint32 flags, bcm_vlan_t *profile_data, void *extra_arguments, int *profile, uint8 *first_reference);

/**
 * implemented by: dnx_mirror_db_snif_egress_vlan_profiles_mirror_free_single
 */
typedef int (*dnx_mirror_db_snif_egress_vlan_profiles_mirror_free_single_cb)(
    int unit, uint32 snif_egress_vlan_profiles_mirror_idx_0, int profile, uint8 *last_reference);

/**
 * implemented by: dnx_mirror_db_snif_egress_vlan_profiles_mirror_profile_get
 */
typedef int (*dnx_mirror_db_snif_egress_vlan_profiles_mirror_profile_get_cb)(
    int unit, uint32 snif_egress_vlan_profiles_mirror_idx_0, const bcm_vlan_t *profile_data, int *profile);

/**
 * implemented by: dnx_mirror_db_snif_ingress_vlan_profiles_mirror_set
 */
typedef int (*dnx_mirror_db_snif_ingress_vlan_profiles_mirror_set_cb)(
    int unit, uint32 snif_ingress_vlan_profiles_mirror_idx_0, dnx_algo_template_t snif_ingress_vlan_profiles_mirror);

/**
 * implemented by: dnx_mirror_db_snif_ingress_vlan_profiles_mirror_get
 */
typedef int (*dnx_mirror_db_snif_ingress_vlan_profiles_mirror_get_cb)(
    int unit, uint32 snif_ingress_vlan_profiles_mirror_idx_0, dnx_algo_template_t *snif_ingress_vlan_profiles_mirror);

/**
 * implemented by: dnx_mirror_db_snif_ingress_vlan_profiles_mirror_alloc
 */
typedef int (*dnx_mirror_db_snif_ingress_vlan_profiles_mirror_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_mirror_db_snif_ingress_vlan_profiles_mirror_create
 */
typedef int (*dnx_mirror_db_snif_ingress_vlan_profiles_mirror_create_cb)(
    int unit, uint32 snif_ingress_vlan_profiles_mirror_idx_0, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_mirror_db_snif_ingress_vlan_profiles_mirror_allocate_single
 */
typedef int (*dnx_mirror_db_snif_ingress_vlan_profiles_mirror_allocate_single_cb)(
    int unit, uint32 snif_ingress_vlan_profiles_mirror_idx_0, uint32 flags, bcm_vlan_t *profile_data, void *extra_arguments, int *profile, uint8 *first_reference);

/**
 * implemented by: dnx_mirror_db_snif_ingress_vlan_profiles_mirror_free_single
 */
typedef int (*dnx_mirror_db_snif_ingress_vlan_profiles_mirror_free_single_cb)(
    int unit, uint32 snif_ingress_vlan_profiles_mirror_idx_0, int profile, uint8 *last_reference);

/**
 * implemented by: dnx_mirror_db_snif_ingress_vlan_profiles_mirror_profile_get
 */
typedef int (*dnx_mirror_db_snif_ingress_vlan_profiles_mirror_profile_get_cb)(
    int unit, uint32 snif_ingress_vlan_profiles_mirror_idx_0, const bcm_vlan_t *profile_data, int *profile);

/**
 * implemented by: dnx_mirror_db_mirror_on_drop_profile_set
 */
typedef int (*dnx_mirror_db_mirror_on_drop_profile_set_cb)(
    int unit, uint32 mirror_on_drop_profile_idx_0, int mirror_on_drop_profile);

/**
 * implemented by: dnx_mirror_db_mirror_on_drop_profile_get
 */
typedef int (*dnx_mirror_db_mirror_on_drop_profile_get_cb)(
    int unit, uint32 mirror_on_drop_profile_idx_0, int *mirror_on_drop_profile);

/**
 * implemented by: dnx_mirror_db_mirror_on_drop_profile_alloc
 */
typedef int (*dnx_mirror_db_mirror_on_drop_profile_alloc_cb)(
    int unit);

/*
 * STRUCTs
 */

/**
 * This structure holds the access functions for the variable channel_to_egress_if
 */
typedef struct {
    dnx_mirror_db_channel_to_egress_if_set_cb set;
    dnx_mirror_db_channel_to_egress_if_get_cb get;
} dnx_mirror_db_channel_to_egress_if_cbs;

/**
 * This structure holds the access functions for the variable egress_mirror_priority
 */
typedef struct {
    dnx_mirror_db_egress_mirror_priority_set_cb set;
    dnx_mirror_db_egress_mirror_priority_get_cb get;
    dnx_mirror_db_egress_mirror_priority_pbmp_neq_cb pbmp_neq;
    dnx_mirror_db_egress_mirror_priority_pbmp_eq_cb pbmp_eq;
    dnx_mirror_db_egress_mirror_priority_pbmp_member_cb pbmp_member;
    dnx_mirror_db_egress_mirror_priority_pbmp_not_null_cb pbmp_not_null;
    dnx_mirror_db_egress_mirror_priority_pbmp_is_null_cb pbmp_is_null;
    dnx_mirror_db_egress_mirror_priority_pbmp_count_cb pbmp_count;
    dnx_mirror_db_egress_mirror_priority_pbmp_xor_cb pbmp_xor;
    dnx_mirror_db_egress_mirror_priority_pbmp_remove_cb pbmp_remove;
    dnx_mirror_db_egress_mirror_priority_pbmp_assign_cb pbmp_assign;
    dnx_mirror_db_egress_mirror_priority_pbmp_get_cb pbmp_get;
    dnx_mirror_db_egress_mirror_priority_pbmp_and_cb pbmp_and;
    dnx_mirror_db_egress_mirror_priority_pbmp_negate_cb pbmp_negate;
    dnx_mirror_db_egress_mirror_priority_pbmp_or_cb pbmp_or;
    dnx_mirror_db_egress_mirror_priority_pbmp_clear_cb pbmp_clear;
    dnx_mirror_db_egress_mirror_priority_pbmp_port_add_cb pbmp_port_add;
    dnx_mirror_db_egress_mirror_priority_pbmp_port_flip_cb pbmp_port_flip;
    dnx_mirror_db_egress_mirror_priority_pbmp_port_remove_cb pbmp_port_remove;
    dnx_mirror_db_egress_mirror_priority_pbmp_port_set_cb pbmp_port_set;
    dnx_mirror_db_egress_mirror_priority_pbmp_ports_range_add_cb pbmp_ports_range_add;
    dnx_mirror_db_egress_mirror_priority_pbmp_fmt_cb pbmp_fmt;
} dnx_mirror_db_egress_mirror_priority_cbs;

/**
 * This structure holds the access functions for the variable snif_ingress_profiles_mirror
 */
typedef struct {
    dnx_mirror_db_snif_ingress_profiles_mirror_set_cb set;
    dnx_mirror_db_snif_ingress_profiles_mirror_get_cb get;
    dnx_mirror_db_snif_ingress_profiles_mirror_create_cb create;
    dnx_mirror_db_snif_ingress_profiles_mirror_allocate_single_cb allocate_single;
    dnx_mirror_db_snif_ingress_profiles_mirror_free_single_cb free_single;
    dnx_mirror_db_snif_ingress_profiles_mirror_is_allocated_cb is_allocated;
} dnx_mirror_db_snif_ingress_profiles_mirror_cbs;

/**
 * This structure holds the access functions for the variable snif_ingress_profiles_snoop
 */
typedef struct {
    dnx_mirror_db_snif_ingress_profiles_snoop_set_cb set;
    dnx_mirror_db_snif_ingress_profiles_snoop_get_cb get;
    dnx_mirror_db_snif_ingress_profiles_snoop_create_cb create;
    dnx_mirror_db_snif_ingress_profiles_snoop_allocate_single_cb allocate_single;
    dnx_mirror_db_snif_ingress_profiles_snoop_free_single_cb free_single;
    dnx_mirror_db_snif_ingress_profiles_snoop_is_allocated_cb is_allocated;
} dnx_mirror_db_snif_ingress_profiles_snoop_cbs;

/**
 * This structure holds the access functions for the variable snif_ingress_profiles_stat_sampling
 */
typedef struct {
    dnx_mirror_db_snif_ingress_profiles_stat_sampling_set_cb set;
    dnx_mirror_db_snif_ingress_profiles_stat_sampling_get_cb get;
    dnx_mirror_db_snif_ingress_profiles_stat_sampling_create_cb create;
    dnx_mirror_db_snif_ingress_profiles_stat_sampling_allocate_single_cb allocate_single;
    dnx_mirror_db_snif_ingress_profiles_stat_sampling_free_single_cb free_single;
    dnx_mirror_db_snif_ingress_profiles_stat_sampling_is_allocated_cb is_allocated;
} dnx_mirror_db_snif_ingress_profiles_stat_sampling_cbs;

/**
 * This structure holds the access functions for the variable snif_egress_vlan_profiles_mirror
 */
typedef struct {
    dnx_mirror_db_snif_egress_vlan_profiles_mirror_set_cb set;
    dnx_mirror_db_snif_egress_vlan_profiles_mirror_get_cb get;
    dnx_mirror_db_snif_egress_vlan_profiles_mirror_alloc_cb alloc;
    dnx_mirror_db_snif_egress_vlan_profiles_mirror_create_cb create;
    dnx_mirror_db_snif_egress_vlan_profiles_mirror_allocate_single_cb allocate_single;
    dnx_mirror_db_snif_egress_vlan_profiles_mirror_free_single_cb free_single;
    dnx_mirror_db_snif_egress_vlan_profiles_mirror_profile_get_cb profile_get;
} dnx_mirror_db_snif_egress_vlan_profiles_mirror_cbs;

/**
 * This structure holds the access functions for the variable snif_ingress_vlan_profiles_mirror
 */
typedef struct {
    dnx_mirror_db_snif_ingress_vlan_profiles_mirror_set_cb set;
    dnx_mirror_db_snif_ingress_vlan_profiles_mirror_get_cb get;
    dnx_mirror_db_snif_ingress_vlan_profiles_mirror_alloc_cb alloc;
    dnx_mirror_db_snif_ingress_vlan_profiles_mirror_create_cb create;
    dnx_mirror_db_snif_ingress_vlan_profiles_mirror_allocate_single_cb allocate_single;
    dnx_mirror_db_snif_ingress_vlan_profiles_mirror_free_single_cb free_single;
    dnx_mirror_db_snif_ingress_vlan_profiles_mirror_profile_get_cb profile_get;
} dnx_mirror_db_snif_ingress_vlan_profiles_mirror_cbs;

/**
 * This structure holds the access functions for the variable mirror_on_drop_profile
 */
typedef struct {
    dnx_mirror_db_mirror_on_drop_profile_set_cb set;
    dnx_mirror_db_mirror_on_drop_profile_get_cb get;
    dnx_mirror_db_mirror_on_drop_profile_alloc_cb alloc;
} dnx_mirror_db_mirror_on_drop_profile_cbs;

/**
 * This structure holds the access functions for the variable dnx_mirror_db_t
 */
typedef struct {
    dnx_mirror_db_is_init_cb is_init;
    dnx_mirror_db_init_cb init;
    /**
     * Access struct for channel_to_egress_if
     */
    dnx_mirror_db_channel_to_egress_if_cbs channel_to_egress_if;
    /**
     * Access struct for egress_mirror_priority
     */
    dnx_mirror_db_egress_mirror_priority_cbs egress_mirror_priority;
    /**
     * Access struct for snif_ingress_profiles_mirror
     */
    dnx_mirror_db_snif_ingress_profiles_mirror_cbs snif_ingress_profiles_mirror;
    /**
     * Access struct for snif_ingress_profiles_snoop
     */
    dnx_mirror_db_snif_ingress_profiles_snoop_cbs snif_ingress_profiles_snoop;
    /**
     * Access struct for snif_ingress_profiles_stat_sampling
     */
    dnx_mirror_db_snif_ingress_profiles_stat_sampling_cbs snif_ingress_profiles_stat_sampling;
    /**
     * Access struct for snif_egress_vlan_profiles_mirror
     */
    dnx_mirror_db_snif_egress_vlan_profiles_mirror_cbs snif_egress_vlan_profiles_mirror;
    /**
     * Access struct for snif_ingress_vlan_profiles_mirror
     */
    dnx_mirror_db_snif_ingress_vlan_profiles_mirror_cbs snif_ingress_vlan_profiles_mirror;
    /**
     * Access struct for mirror_on_drop_profile
     */
    dnx_mirror_db_mirror_on_drop_profile_cbs mirror_on_drop_profile;
} dnx_mirror_db_cbs;

/*
 * Global Variables
 */

/*
 * Global Variables
 */

extern dnx_mirror_db_cbs dnx_mirror_db;

#endif /* __DNX_MIRROR_ACCESS_H__ */
