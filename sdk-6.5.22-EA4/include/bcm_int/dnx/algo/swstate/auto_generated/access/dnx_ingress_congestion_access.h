/** \file algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h
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

#ifndef __DNX_INGRESS_CONGESTION_ACCESS_H__
#define __DNX_INGRESS_CONGESTION_ACCESS_H__

#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_ingress_congestion_types.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_internal.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
/*
 * TYPEDEFs
 */

/**
 * implemented by: dnx_ingress_congestion_db_is_init
 */
typedef int (*dnx_ingress_congestion_db_is_init_cb)(
    int unit, uint8 *is_init);

/**
 * implemented by: dnx_ingress_congestion_db_init
 */
typedef int (*dnx_ingress_congestion_db_init_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_voq_rate_class_ref_count_set
 */
typedef int (*dnx_ingress_congestion_db_voq_rate_class_ref_count_set_cb)(
    int unit, uint32 ref_count_idx_0, uint32 ref_count_idx_1, uint32 ref_count);

/**
 * implemented by: dnx_ingress_congestion_db_voq_rate_class_ref_count_get
 */
typedef int (*dnx_ingress_congestion_db_voq_rate_class_ref_count_get_cb)(
    int unit, uint32 ref_count_idx_0, uint32 ref_count_idx_1, uint32 *ref_count);

/**
 * implemented by: dnx_ingress_congestion_db_voq_rate_class_ref_count_inc
 */
typedef int (*dnx_ingress_congestion_db_voq_rate_class_ref_count_inc_cb)(
    int unit, uint32 ref_count_idx_0, uint32 ref_count_idx_1, uint32 inc_value);

/**
 * implemented by: dnx_ingress_congestion_db_voq_rate_class_ref_count_dec
 */
typedef int (*dnx_ingress_congestion_db_voq_rate_class_ref_count_dec_cb)(
    int unit, uint32 ref_count_idx_0, uint32 ref_count_idx_1, uint32 dec_value);

/**
 * implemented by: dnx_ingress_congestion_db_voq_rate_class_ref_count_alloc
 */
typedef int (*dnx_ingress_congestion_db_voq_rate_class_ref_count_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_voq_rate_class_is_created_set
 */
typedef int (*dnx_ingress_congestion_db_voq_rate_class_is_created_set_cb)(
    int unit, uint32 is_created_idx_0, uint32 is_created);

/**
 * implemented by: dnx_ingress_congestion_db_voq_rate_class_is_created_get
 */
typedef int (*dnx_ingress_congestion_db_voq_rate_class_is_created_get_cb)(
    int unit, uint32 is_created_idx_0, uint32 *is_created);

/**
 * implemented by: dnx_ingress_congestion_db_voq_rate_class_is_created_alloc
 */
typedef int (*dnx_ingress_congestion_db_voq_rate_class_is_created_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_guaranteed_voq_guaranteed_set
 */
typedef int (*dnx_ingress_congestion_db_guaranteed_voq_guaranteed_set_cb)(
    int unit, uint32 voq_guaranteed_idx_0, uint32 voq_guaranteed_idx_1, uint32 voq_guaranteed);

/**
 * implemented by: dnx_ingress_congestion_db_guaranteed_voq_guaranteed_get
 */
typedef int (*dnx_ingress_congestion_db_guaranteed_voq_guaranteed_get_cb)(
    int unit, uint32 voq_guaranteed_idx_0, uint32 voq_guaranteed_idx_1, uint32 *voq_guaranteed);

/**
 * implemented by: dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_set
 */
typedef int (*dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_set_cb)(
    int unit, uint32 vsq_guaranteed_idx_0, uint32 vsq_guaranteed_idx_1, uint32 vsq_guaranteed);

/**
 * implemented by: dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_get
 */
typedef int (*dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_get_cb)(
    int unit, uint32 vsq_guaranteed_idx_0, uint32 vsq_guaranteed_idx_1, uint32 *vsq_guaranteed);

/**
 * implemented by: dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_inc
 */
typedef int (*dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_inc_cb)(
    int unit, uint32 vsq_guaranteed_idx_0, uint32 vsq_guaranteed_idx_1, uint32 inc_value);

/**
 * implemented by: dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_dec
 */
typedef int (*dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_dec_cb)(
    int unit, uint32 vsq_guaranteed_idx_0, uint32 vsq_guaranteed_idx_1, uint32 dec_value);

/**
 * implemented by: dnx_ingress_congestion_db_port_based_vsq_alloc
 */
typedef int (*dnx_ingress_congestion_db_port_based_vsq_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_alloc
 */
typedef int (*dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_alloc_cb)(
    int unit, uint32 port_based_vsq_idx_0);

/**
 * implemented by: dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_src_port_set
 */
typedef int (*dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_src_port_set_cb)(
    int unit, uint32 port_based_vsq_idx_0, uint32 vsq_e_mapping_idx_0, int src_port);

/**
 * implemented by: dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_src_port_get
 */
typedef int (*dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_src_port_get_cb)(
    int unit, uint32 port_based_vsq_idx_0, uint32 vsq_e_mapping_idx_0, int *src_port);

/**
 * implemented by: dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_alloc
 */
typedef int (*dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_alloc_cb)(
    int unit, uint32 port_based_vsq_idx_0);

/**
 * implemented by: dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_src_port_set
 */
typedef int (*dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_src_port_set_cb)(
    int unit, uint32 port_based_vsq_idx_0, uint32 vsq_f_mapping_idx_0, int src_port);

/**
 * implemented by: dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_src_port_get
 */
typedef int (*dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_src_port_get_cb)(
    int unit, uint32 port_based_vsq_idx_0, uint32 vsq_f_mapping_idx_0, int *src_port);

/**
 * implemented by: dnx_ingress_congestion_db_port_based_vsq_lossless_pool_id_set
 */
typedef int (*dnx_ingress_congestion_db_port_based_vsq_lossless_pool_id_set_cb)(
    int unit, uint32 port_based_vsq_idx_0, int lossless_pool_id);

/**
 * implemented by: dnx_ingress_congestion_db_port_based_vsq_lossless_pool_id_get
 */
typedef int (*dnx_ingress_congestion_db_port_based_vsq_lossless_pool_id_get_cb)(
    int unit, uint32 port_based_vsq_idx_0, int *lossless_pool_id);

/**
 * implemented by: dnx_ingress_congestion_db_pg_numq_set
 */
typedef int (*dnx_ingress_congestion_db_pg_numq_set_cb)(
    int unit, uint32 pg_numq_idx_0, int pg_numq);

/**
 * implemented by: dnx_ingress_congestion_db_pg_numq_get
 */
typedef int (*dnx_ingress_congestion_db_pg_numq_get_cb)(
    int unit, uint32 pg_numq_idx_0, int *pg_numq);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqe_id_set
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqe_id_set_cb)(
    int unit, uint32 vsqe_id_idx_0, dnx_algo_res_t vsqe_id);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqe_id_get
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqe_id_get_cb)(
    int unit, uint32 vsqe_id_idx_0, dnx_algo_res_t *vsqe_id);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqe_id_alloc
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqe_id_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqe_id_create
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqe_id_create_cb)(
    int unit, uint32 vsqe_id_idx_0, dnx_algo_res_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqe_id_allocate_single
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqe_id_allocate_single_cb)(
    int unit, uint32 vsqe_id_idx_0, uint32 flags, void *extra_arguments, int *element);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqe_id_free_single
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqe_id_free_single_cb)(
    int unit, uint32 vsqe_id_idx_0, int element, void *extra_arguments);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqf_id_set
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqf_id_set_cb)(
    int unit, uint32 vsqf_id_idx_0, dnx_algo_res_t vsqf_id);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqf_id_get
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqf_id_get_cb)(
    int unit, uint32 vsqf_id_idx_0, dnx_algo_res_t *vsqf_id);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqf_id_alloc
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqf_id_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqf_id_create
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqf_id_create_cb)(
    int unit, uint32 vsqf_id_idx_0, dnx_algo_res_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqf_id_allocate_several
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqf_id_allocate_several_cb)(
    int unit, uint32 vsqf_id_idx_0, uint32 flags, uint32 nof_elements, void *extra_arguments, int *element);

/**
 * implemented by: dnx_ingress_congestion_db_algo_res_vsqf_id_free_several
 */
typedef int (*dnx_ingress_congestion_db_algo_res_vsqf_id_free_several_cb)(
    int unit, uint32 vsqf_id_idx_0, uint32 nof_elements, int element, void *extra_arguments);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_set
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_set_cb)(
    int unit, uint32 vsq_rate_cls_a_c_idx_0, dnx_algo_template_t vsq_rate_cls_a_c);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_get
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_get_cb)(
    int unit, uint32 vsq_rate_cls_a_c_idx_0, dnx_algo_template_t *vsq_rate_cls_a_c);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_create
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_create_cb)(
    int unit, uint32 vsq_rate_cls_a_c_idx_0, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_exchange
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_exchange_cb)(
    int unit, uint32 vsq_rate_cls_a_c_idx_0, uint32 flags, const dnx_ingress_congestion_queue_vsq_rate_class_info_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_profile_data_get
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_profile_data_get_cb)(
    int unit, uint32 vsq_rate_cls_a_c_idx_0, int profile, int *ref_count, dnx_ingress_congestion_queue_vsq_rate_class_info_t *profile_data);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_set
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_set_cb)(
    int unit, uint32 vsq_rate_cls_d_idx_0, dnx_algo_template_t vsq_rate_cls_d);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_get
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_get_cb)(
    int unit, uint32 vsq_rate_cls_d_idx_0, dnx_algo_template_t *vsq_rate_cls_d);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_alloc
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_create
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_create_cb)(
    int unit, uint32 vsq_rate_cls_d_idx_0, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_exchange
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_exchange_cb)(
    int unit, uint32 vsq_rate_cls_d_idx_0, uint32 flags, const dnx_ingress_congestion_queue_vsq_rate_class_info_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_profile_data_get
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_profile_data_get_cb)(
    int unit, uint32 vsq_rate_cls_d_idx_0, int profile, int *ref_count, dnx_ingress_congestion_queue_vsq_rate_class_info_t *profile_data);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_set
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_set_cb)(
    int unit, uint32 vsq_rate_cls_source_port_idx_0, dnx_algo_template_t vsq_rate_cls_source_port);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_get
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_get_cb)(
    int unit, uint32 vsq_rate_cls_source_port_idx_0, dnx_algo_template_t *vsq_rate_cls_source_port);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_alloc
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_create
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_create_cb)(
    int unit, uint32 vsq_rate_cls_source_port_idx_0, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_exchange
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_exchange_cb)(
    int unit, uint32 vsq_rate_cls_source_port_idx_0, uint32 flags, const dnx_ingress_congestion_src_port_vsq_rate_class_info_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_profile_data_get
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_profile_data_get_cb)(
    int unit, uint32 vsq_rate_cls_source_port_idx_0, int profile, int *ref_count, dnx_ingress_congestion_src_port_vsq_rate_class_info_t *profile_data);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_set
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_set_cb)(
    int unit, uint32 vsq_rate_cls_pg_idx_0, dnx_algo_template_t vsq_rate_cls_pg);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_get
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_get_cb)(
    int unit, uint32 vsq_rate_cls_pg_idx_0, dnx_algo_template_t *vsq_rate_cls_pg);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_alloc
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_create
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_create_cb)(
    int unit, uint32 vsq_rate_cls_pg_idx_0, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_exchange
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_exchange_cb)(
    int unit, uint32 vsq_rate_cls_pg_idx_0, uint32 flags, const dnx_ingress_congestion_pg_vsq_rate_class_info_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_profile_data_get
 */
typedef int (*dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_profile_data_get_cb)(
    int unit, uint32 vsq_rate_cls_pg_idx_0, int profile, int *ref_count, dnx_ingress_congestion_pg_vsq_rate_class_info_t *profile_data);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_pg_tc_mapping_set
 */
typedef int (*dnx_ingress_congestion_db_vsq_pg_tc_mapping_set_cb)(
    int unit, uint32 vsq_pg_tc_mapping_idx_0, dnx_algo_template_t vsq_pg_tc_mapping);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_pg_tc_mapping_get
 */
typedef int (*dnx_ingress_congestion_db_vsq_pg_tc_mapping_get_cb)(
    int unit, uint32 vsq_pg_tc_mapping_idx_0, dnx_algo_template_t *vsq_pg_tc_mapping);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_pg_tc_mapping_alloc
 */
typedef int (*dnx_ingress_congestion_db_vsq_pg_tc_mapping_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_pg_tc_mapping_create
 */
typedef int (*dnx_ingress_congestion_db_vsq_pg_tc_mapping_create_cb)(
    int unit, uint32 vsq_pg_tc_mapping_idx_0, dnx_algo_template_create_data_t * data, void *extra_arguments);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_pg_tc_mapping_exchange
 */
typedef int (*dnx_ingress_congestion_db_vsq_pg_tc_mapping_exchange_cb)(
    int unit, uint32 vsq_pg_tc_mapping_idx_0, uint32 flags, const dnx_ingress_congestion_vsq_tc_pg_mapping_t *profile_data, int old_profile, const void *extra_arguments, int *new_profile, uint8 *first_reference, uint8 *last_reference);

/**
 * implemented by: dnx_ingress_congestion_db_vsq_pg_tc_mapping_profile_data_get
 */
typedef int (*dnx_ingress_congestion_db_vsq_pg_tc_mapping_profile_data_get_cb)(
    int unit, uint32 vsq_pg_tc_mapping_idx_0, int profile, int *ref_count, dnx_ingress_congestion_vsq_tc_pg_mapping_t *profile_data);

/**
 * implemented by: dnx_ingress_congestion_db_dram_thresholds_set
 */
typedef int (*dnx_ingress_congestion_db_dram_thresholds_set_cb)(
    int unit, uint32 dram_thresholds_idx_0, CONST dnx_cosq_ingress_dram_thresholds_t *dram_thresholds);

/**
 * implemented by: dnx_ingress_congestion_db_dram_thresholds_get
 */
typedef int (*dnx_ingress_congestion_db_dram_thresholds_get_cb)(
    int unit, uint32 dram_thresholds_idx_0, dnx_cosq_ingress_dram_thresholds_t *dram_thresholds);

/**
 * implemented by: dnx_ingress_congestion_db_dram_thresholds_alloc
 */
typedef int (*dnx_ingress_congestion_db_dram_thresholds_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_fadt_tail_drop_thresholds_alloc
 */
typedef int (*dnx_ingress_congestion_db_fadt_tail_drop_thresholds_alloc_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_fadt_tail_drop_thresholds_resource_dp_set
 */
typedef int (*dnx_ingress_congestion_db_fadt_tail_drop_thresholds_resource_dp_set_cb)(
    int unit, uint32 fadt_tail_drop_thresholds_idx_0, uint32 resource_idx_0, uint32 dp_idx_0, bcm_cosq_fadt_threshold_t dp);

/**
 * implemented by: dnx_ingress_congestion_db_fadt_tail_drop_thresholds_resource_dp_get
 */
typedef int (*dnx_ingress_congestion_db_fadt_tail_drop_thresholds_resource_dp_get_cb)(
    int unit, uint32 fadt_tail_drop_thresholds_idx_0, uint32 resource_idx_0, uint32 dp_idx_0, bcm_cosq_fadt_threshold_t *dp);

/**
 * implemented by: dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_create
 */
typedef int (*dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_create_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_is_created
 */
typedef int (*dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_is_created_cb)(
    int unit, uint8 *is_created);

/**
 * implemented by: dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_take
 */
typedef int (*dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_take_cb)(
    int unit, int usec);

/**
 * implemented by: dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_give
 */
typedef int (*dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_give_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_sync_manager_fadt_mutex_create
 */
typedef int (*dnx_ingress_congestion_db_sync_manager_fadt_mutex_create_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_sync_manager_fadt_mutex_is_created
 */
typedef int (*dnx_ingress_congestion_db_sync_manager_fadt_mutex_is_created_cb)(
    int unit, uint8 *is_created);

/**
 * implemented by: dnx_ingress_congestion_db_sync_manager_fadt_mutex_take
 */
typedef int (*dnx_ingress_congestion_db_sync_manager_fadt_mutex_take_cb)(
    int unit, int usec);

/**
 * implemented by: dnx_ingress_congestion_db_sync_manager_fadt_mutex_give
 */
typedef int (*dnx_ingress_congestion_db_sync_manager_fadt_mutex_give_cb)(
    int unit);

/**
 * implemented by: dnx_ingress_congestion_db_sync_manager_dram_in_use_set
 */
typedef int (*dnx_ingress_congestion_db_sync_manager_dram_in_use_set_cb)(
    int unit, int dram_in_use);

/**
 * implemented by: dnx_ingress_congestion_db_sync_manager_dram_in_use_get
 */
typedef int (*dnx_ingress_congestion_db_sync_manager_dram_in_use_get_cb)(
    int unit, int *dram_in_use);

/*
 * STRUCTs
 */

/**
 * This structure holds the access functions for the variable ref_count
 */
typedef struct {
    dnx_ingress_congestion_db_voq_rate_class_ref_count_set_cb set;
    dnx_ingress_congestion_db_voq_rate_class_ref_count_get_cb get;
    dnx_ingress_congestion_db_voq_rate_class_ref_count_inc_cb inc;
    dnx_ingress_congestion_db_voq_rate_class_ref_count_dec_cb dec;
    dnx_ingress_congestion_db_voq_rate_class_ref_count_alloc_cb alloc;
} dnx_ingress_congestion_db_voq_rate_class_ref_count_cbs;

/**
 * This structure holds the access functions for the variable is_created
 */
typedef struct {
    dnx_ingress_congestion_db_voq_rate_class_is_created_set_cb set;
    dnx_ingress_congestion_db_voq_rate_class_is_created_get_cb get;
    dnx_ingress_congestion_db_voq_rate_class_is_created_alloc_cb alloc;
} dnx_ingress_congestion_db_voq_rate_class_is_created_cbs;

/**
 * This structure holds the access functions for the variable dnx_ingress_congestion_voq_rate_class_info_t
 */
typedef struct {
    /**
     * Access struct for ref_count
     */
    dnx_ingress_congestion_db_voq_rate_class_ref_count_cbs ref_count;
    /**
     * Access struct for is_created
     */
    dnx_ingress_congestion_db_voq_rate_class_is_created_cbs is_created;
} dnx_ingress_congestion_db_voq_rate_class_cbs;

/**
 * This structure holds the access functions for the variable voq_guaranteed
 */
typedef struct {
    dnx_ingress_congestion_db_guaranteed_voq_guaranteed_set_cb set;
    dnx_ingress_congestion_db_guaranteed_voq_guaranteed_get_cb get;
} dnx_ingress_congestion_db_guaranteed_voq_guaranteed_cbs;

/**
 * This structure holds the access functions for the variable vsq_guaranteed
 */
typedef struct {
    dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_set_cb set;
    dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_get_cb get;
    dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_inc_cb inc;
    dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_dec_cb dec;
} dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_cbs;

/**
 * This structure holds the access functions for the variable dnx_ingress_congestion_guaranteed_t
 */
typedef struct {
    /**
     * Access struct for voq_guaranteed
     */
    dnx_ingress_congestion_db_guaranteed_voq_guaranteed_cbs voq_guaranteed;
    /**
     * Access struct for vsq_guaranteed
     */
    dnx_ingress_congestion_db_guaranteed_vsq_guaranteed_cbs vsq_guaranteed;
} dnx_ingress_congestion_db_guaranteed_cbs;

/**
 * This structure holds the access functions for the variable src_port
 */
typedef struct {
    dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_src_port_set_cb set;
    dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_src_port_get_cb get;
} dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_src_port_cbs;

/**
 * This structure holds the access functions for the variable dnx_ingress_congestion_vsq_e_mapping_info_t
 */
typedef struct {
    dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_alloc_cb alloc;
    /**
     * Access struct for src_port
     */
    dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_src_port_cbs src_port;
} dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_cbs;

/**
 * This structure holds the access functions for the variable src_port
 */
typedef struct {
    dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_src_port_set_cb set;
    dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_src_port_get_cb get;
} dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_src_port_cbs;

/**
 * This structure holds the access functions for the variable dnx_ingress_congestion_vsq_f_mapping_info_t
 */
typedef struct {
    dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_alloc_cb alloc;
    /**
     * Access struct for src_port
     */
    dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_src_port_cbs src_port;
} dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_cbs;

/**
 * This structure holds the access functions for the variable lossless_pool_id
 */
typedef struct {
    dnx_ingress_congestion_db_port_based_vsq_lossless_pool_id_set_cb set;
    dnx_ingress_congestion_db_port_based_vsq_lossless_pool_id_get_cb get;
} dnx_ingress_congestion_db_port_based_vsq_lossless_pool_id_cbs;

/**
 * This structure holds the access functions for the variable dnx_ingress_congestion_pb_vsq_info_t
 */
typedef struct {
    dnx_ingress_congestion_db_port_based_vsq_alloc_cb alloc;
    /**
     * Access struct for vsq_e_mapping
     */
    dnx_ingress_congestion_db_port_based_vsq_vsq_e_mapping_cbs vsq_e_mapping;
    /**
     * Access struct for vsq_f_mapping
     */
    dnx_ingress_congestion_db_port_based_vsq_vsq_f_mapping_cbs vsq_f_mapping;
    /**
     * Access struct for lossless_pool_id
     */
    dnx_ingress_congestion_db_port_based_vsq_lossless_pool_id_cbs lossless_pool_id;
} dnx_ingress_congestion_db_port_based_vsq_cbs;

/**
 * This structure holds the access functions for the variable pg_numq
 */
typedef struct {
    dnx_ingress_congestion_db_pg_numq_set_cb set;
    dnx_ingress_congestion_db_pg_numq_get_cb get;
} dnx_ingress_congestion_db_pg_numq_cbs;

/**
 * This structure holds the access functions for the variable vsqe_id
 */
typedef struct {
    dnx_ingress_congestion_db_algo_res_vsqe_id_set_cb set;
    dnx_ingress_congestion_db_algo_res_vsqe_id_get_cb get;
    dnx_ingress_congestion_db_algo_res_vsqe_id_alloc_cb alloc;
    dnx_ingress_congestion_db_algo_res_vsqe_id_create_cb create;
    dnx_ingress_congestion_db_algo_res_vsqe_id_allocate_single_cb allocate_single;
    dnx_ingress_congestion_db_algo_res_vsqe_id_free_single_cb free_single;
} dnx_ingress_congestion_db_algo_res_vsqe_id_cbs;

/**
 * This structure holds the access functions for the variable vsqf_id
 */
typedef struct {
    dnx_ingress_congestion_db_algo_res_vsqf_id_set_cb set;
    dnx_ingress_congestion_db_algo_res_vsqf_id_get_cb get;
    dnx_ingress_congestion_db_algo_res_vsqf_id_alloc_cb alloc;
    dnx_ingress_congestion_db_algo_res_vsqf_id_create_cb create;
    dnx_ingress_congestion_db_algo_res_vsqf_id_allocate_several_cb allocate_several;
    dnx_ingress_congestion_db_algo_res_vsqf_id_free_several_cb free_several;
} dnx_ingress_congestion_db_algo_res_vsqf_id_cbs;

/**
 * This structure holds the access functions for the variable dnx_ingress_congestion_res_mngr_t
 */
typedef struct {
    /**
     * Access struct for vsqe_id
     */
    dnx_ingress_congestion_db_algo_res_vsqe_id_cbs vsqe_id;
    /**
     * Access struct for vsqf_id
     */
    dnx_ingress_congestion_db_algo_res_vsqf_id_cbs vsqf_id;
} dnx_ingress_congestion_db_algo_res_cbs;

/**
 * This structure holds the access functions for the variable vsq_rate_cls_a_c
 */
typedef struct {
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_set_cb set;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_get_cb get;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_create_cb create;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_exchange_cb exchange;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_profile_data_get_cb profile_data_get;
} dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_cbs;

/**
 * This structure holds the access functions for the variable vsq_rate_cls_d
 */
typedef struct {
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_set_cb set;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_get_cb get;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_alloc_cb alloc;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_create_cb create;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_exchange_cb exchange;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_profile_data_get_cb profile_data_get;
} dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_cbs;

/**
 * This structure holds the access functions for the variable vsq_rate_cls_source_port
 */
typedef struct {
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_set_cb set;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_get_cb get;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_alloc_cb alloc;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_create_cb create;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_exchange_cb exchange;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_profile_data_get_cb profile_data_get;
} dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_cbs;

/**
 * This structure holds the access functions for the variable vsq_rate_cls_pg
 */
typedef struct {
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_set_cb set;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_get_cb get;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_alloc_cb alloc;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_create_cb create;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_exchange_cb exchange;
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_profile_data_get_cb profile_data_get;
} dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_cbs;

/**
 * This structure holds the access functions for the variable dnx_ingress_congestion_rate_class_template_t
 */
typedef struct {
    /**
     * Access struct for vsq_rate_cls_a_c
     */
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_a_c_cbs vsq_rate_cls_a_c;
    /**
     * Access struct for vsq_rate_cls_d
     */
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_d_cbs vsq_rate_cls_d;
    /**
     * Access struct for vsq_rate_cls_source_port
     */
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_source_port_cbs vsq_rate_cls_source_port;
    /**
     * Access struct for vsq_rate_cls_pg
     */
    dnx_ingress_congestion_db_vsq_rate_class_vsq_rate_cls_pg_cbs vsq_rate_cls_pg;
} dnx_ingress_congestion_db_vsq_rate_class_cbs;

/**
 * This structure holds the access functions for the variable vsq_pg_tc_mapping
 */
typedef struct {
    dnx_ingress_congestion_db_vsq_pg_tc_mapping_set_cb set;
    dnx_ingress_congestion_db_vsq_pg_tc_mapping_get_cb get;
    dnx_ingress_congestion_db_vsq_pg_tc_mapping_alloc_cb alloc;
    dnx_ingress_congestion_db_vsq_pg_tc_mapping_create_cb create;
    dnx_ingress_congestion_db_vsq_pg_tc_mapping_exchange_cb exchange;
    dnx_ingress_congestion_db_vsq_pg_tc_mapping_profile_data_get_cb profile_data_get;
} dnx_ingress_congestion_db_vsq_pg_tc_mapping_cbs;

/**
 * This structure holds the access functions for the variable dnx_cosq_ingress_dram_thresholds_t
 */
typedef struct {
    dnx_ingress_congestion_db_dram_thresholds_set_cb set;
    dnx_ingress_congestion_db_dram_thresholds_get_cb get;
    dnx_ingress_congestion_db_dram_thresholds_alloc_cb alloc;
} dnx_ingress_congestion_db_dram_thresholds_cbs;

/**
 * This structure holds the access functions for the variable dp
 */
typedef struct {
    dnx_ingress_congestion_db_fadt_tail_drop_thresholds_resource_dp_set_cb set;
    dnx_ingress_congestion_db_fadt_tail_drop_thresholds_resource_dp_get_cb get;
} dnx_ingress_congestion_db_fadt_tail_drop_thresholds_resource_dp_cbs;

/**
 * This structure holds the access functions for the variable dnx_cosq_ingress_fadt_tail_drop_thresholds_per_resource_t
 */
typedef struct {
    /**
     * Access struct for dp
     */
    dnx_ingress_congestion_db_fadt_tail_drop_thresholds_resource_dp_cbs dp;
} dnx_ingress_congestion_db_fadt_tail_drop_thresholds_resource_cbs;

/**
 * This structure holds the access functions for the variable dnx_cosq_ingress_fadt_tail_drop_thresholds_t
 */
typedef struct {
    dnx_ingress_congestion_db_fadt_tail_drop_thresholds_alloc_cb alloc;
    /**
     * Access struct for resource
     */
    dnx_ingress_congestion_db_fadt_tail_drop_thresholds_resource_cbs resource;
} dnx_ingress_congestion_db_fadt_tail_drop_thresholds_cbs;

/**
 * This structure holds the access functions for the variable dram_bound_mutex
 */
typedef struct {
    dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_create_cb create;
    dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_is_created_cb is_created;
    dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_take_cb take;
    dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_give_cb give;
} dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_cbs;

/**
 * This structure holds the access functions for the variable fadt_mutex
 */
typedef struct {
    dnx_ingress_congestion_db_sync_manager_fadt_mutex_create_cb create;
    dnx_ingress_congestion_db_sync_manager_fadt_mutex_is_created_cb is_created;
    dnx_ingress_congestion_db_sync_manager_fadt_mutex_take_cb take;
    dnx_ingress_congestion_db_sync_manager_fadt_mutex_give_cb give;
} dnx_ingress_congestion_db_sync_manager_fadt_mutex_cbs;

/**
 * This structure holds the access functions for the variable dram_in_use
 */
typedef struct {
    dnx_ingress_congestion_db_sync_manager_dram_in_use_set_cb set;
    dnx_ingress_congestion_db_sync_manager_dram_in_use_get_cb get;
} dnx_ingress_congestion_db_sync_manager_dram_in_use_cbs;

/**
 * This structure holds the access functions for the variable dnx_cosq_ingress_sync_manager_t
 */
typedef struct {
    /**
     * Access struct for dram_bound_mutex
     */
    dnx_ingress_congestion_db_sync_manager_dram_bound_mutex_cbs dram_bound_mutex;
    /**
     * Access struct for fadt_mutex
     */
    dnx_ingress_congestion_db_sync_manager_fadt_mutex_cbs fadt_mutex;
    /**
     * Access struct for dram_in_use
     */
    dnx_ingress_congestion_db_sync_manager_dram_in_use_cbs dram_in_use;
} dnx_ingress_congestion_db_sync_manager_cbs;

/**
 * This structure holds the access functions for the variable dnx_ingress_congestion_db_t
 */
typedef struct {
    dnx_ingress_congestion_db_is_init_cb is_init;
    dnx_ingress_congestion_db_init_cb init;
    /**
     * Access struct for voq_rate_class
     */
    dnx_ingress_congestion_db_voq_rate_class_cbs voq_rate_class;
    /**
     * Access struct for guaranteed
     */
    dnx_ingress_congestion_db_guaranteed_cbs guaranteed;
    /**
     * Access struct for port_based_vsq
     */
    dnx_ingress_congestion_db_port_based_vsq_cbs port_based_vsq;
    /**
     * Access struct for pg_numq
     */
    dnx_ingress_congestion_db_pg_numq_cbs pg_numq;
    /**
     * Access struct for algo_res
     */
    dnx_ingress_congestion_db_algo_res_cbs algo_res;
    /**
     * Access struct for vsq_rate_class
     */
    dnx_ingress_congestion_db_vsq_rate_class_cbs vsq_rate_class;
    /**
     * Access struct for vsq_pg_tc_mapping
     */
    dnx_ingress_congestion_db_vsq_pg_tc_mapping_cbs vsq_pg_tc_mapping;
    /**
     * Access struct for dram_thresholds
     */
    dnx_ingress_congestion_db_dram_thresholds_cbs dram_thresholds;
    /**
     * Access struct for fadt_tail_drop_thresholds
     */
    dnx_ingress_congestion_db_fadt_tail_drop_thresholds_cbs fadt_tail_drop_thresholds;
    /**
     * Access struct for sync_manager
     */
    dnx_ingress_congestion_db_sync_manager_cbs sync_manager;
} dnx_ingress_congestion_db_cbs;

/*
 * Global Variables
 */

/*
 * Global Variables
 */

extern dnx_ingress_congestion_db_cbs dnx_ingress_congestion_db;

#endif /* __DNX_INGRESS_CONGESTION_ACCESS_H__ */
