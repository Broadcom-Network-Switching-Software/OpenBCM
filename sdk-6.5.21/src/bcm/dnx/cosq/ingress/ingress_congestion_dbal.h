/** \file src/bcm/dnx/cosq/ingress/ingress_congestion_dbal.h
 * 
 *
 * DBAL access API of ingress congestion inside cosq module
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _INGRESS_CONGESTION_DBAL_H_INCLUDED_
#define _INGRESS_CONGESTION_DBAL_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include "ingress_congestion.h"

/**
 * WRED dbal table keys
 */
typedef struct
{
    int rate_class;
    int dp;
    int core_id;
    int pool_id;
} dnx_ingress_congestion_dbal_wred_key_t;

/**
 * WRED parameters as configured to HW
 */
typedef struct
{
    int wred_en;
    uint32 min_thresh;
    uint32 max_thresh;
    uint32 c1;
    uint32 c2;
    uint32 c3;
    int ignore_packet_size;
} dnx_ingress_congestion_wred_hw_params_t;

/**
 * WRED avgr parameters
 */
typedef struct
{
    int avrg_en;
    uint32 avrg_weight;
} dnx_ingress_congestion_wred_avrg_params_t;

/**
 * VSQ-E resource allocation parameters
 */
typedef struct
{
    uint32 guaranteed;          /* reserved amount of the resource. */
    uint32 shared_pool;         /* maximum amount of the resource in the shared pool. */
    uint32 headroom;            /* maximum amount of the resource in the lossless headroom. */
} dnx_ingress_congestion_vsq_e_resource_alloc_params_t;

/**
 * VSQ-F resource allocation parameters
 */
typedef struct
{
    uint32 guaranteed;          /* reserved amount of the resource. */
    dnx_cosq_fadt_threshold_t shared_pool_fadt; /* fadt threshold for shared pool */
    uint32 nominal_headroom;    /* nominal size of headroom */
    uint32 headroom_extension;  /* size in shared which can be used for headroom */
    uint32 max_headroom;        /* maximum size which can be used for headroom -- headroom + shared */
} dnx_ingress_congestion_vsq_f_resource_alloc_params_t;

/**
 * Global VSQ resource allocation parameters
 */
typedef struct
{
    uint32 shared_pool;         /* maximum amount of the resource in the shared pool. */
    uint32 nominal_headroom;    /* maximum amount of the resource in the lossless headroom. */
    uint32 max_headroom;        /* headroom + extension */
} dnx_ingress_congestion_global_vsq_resource_alloc_params_t;

/**
 *  reject mask
 */
typedef struct
{
    int mask[DNX_INGRESS_CONGESTION_REJECT_BIT_NOF];
} dnx_ingress_congestion_reject_mask_t;

typedef union
{
    struct
    {
        int sram_pds_in_guaranteed;
        int sram_buffers_in_guaranteed;
        int words_in_guaranteed;
    } guaranteed;

    struct
    {
        int admit_profile;
        int is_lossless;
    } port_based_vsq;

    int pp_admit_profile;
} dnx_ingress_congestion_reject_mask_key_t;

/**
 * \brief - write VOQ FADT drop to HW
 */
shr_error_e dnx_ingress_congestion_dbal_voq_fadt_drop_hw_set(
    int unit,
    dbal_tables_e table_id,
    int rate_class,
    int dp,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief - read VOQ FADT drop from HW
 */
shr_error_e dnx_ingress_congestion_dbal_voq_fadt_drop_hw_get(
    int unit,
    dbal_tables_e table_id,
    int rate_class,
    int dp,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief - write WRED to HW
 */
shr_error_e dnx_ingress_congestion_dbal_wred_hw_set(
    int unit,
    dbal_tables_e table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    dnx_ingress_congestion_wred_hw_params_t * params);

/**
 * \brief - read WRED from HW
 */
shr_error_e dnx_ingress_congestion_dbal_wred_hw_get(
    int unit,
    dbal_tables_e table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    dnx_ingress_congestion_wred_hw_params_t * params);

/**
 * \brief - read WRED enable for all DPs of the rate class (and ECN for VOQ) from HW
 */
shr_error_e dnx_ingress_congestion_dbal_wred_enable_hw_get(
    int unit,
    dbal_tables_e table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    int *enable);

/**
 * \brief - write WRED average weight to HW
 */
shr_error_e dnx_ingress_congestion_dbal_wred_weight_hw_set(
    int unit,
    dbal_tables_e table_id,
    int core_id,
    int rate_class,
    dnx_ingress_congestion_wred_avrg_params_t * avrg_params);

/**
 * \brief - read WRED average weight to HW
 */
shr_error_e dnx_ingress_congestion_dbal_wred_weight_hw_get(
    int unit,
    dbal_tables_e table_id,
    int core_id,
    int rate_class,
    dnx_ingress_congestion_wred_avrg_params_t * avrg_params);

/**
 * \brief - write hysteresis FC threshold to HW
 */
shr_error_e dnx_ingress_congestion_dbal_hyst_fc_hw_set(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int pool,
    int rate_class,
    dnx_cosq_hyst_threshold_t * fc);

/**
 * \brief - read hysteresis FC threshold from HW
 */
shr_error_e dnx_ingress_congestion_dbal_hyst_fc_hw_get(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int pool,
    int rate_class,
    dnx_cosq_hyst_threshold_t * fc);

/**
 * \brief - write FADT FC threshold (VSQ-F) to HW
 */
shr_error_e dnx_ingress_congestion_dbal_vsq_fadt_fc_hw_set(
    int unit,
    int core,
    dbal_tables_e table_id,
    int rate_class,
    dnx_cosq_fadt_hyst_threshold_t * fadt_fc);

/**
 * \brief - read  FADT FC (VSQ-f) from HW
 */
shr_error_e dnx_ingress_congestion_dbal_vsq_fadt_fc_hw_get(
    int unit,
    int core,
    dbal_tables_e table_id,
    int rate_class,
    dnx_cosq_fadt_hyst_threshold_t * fadt_fc);

/**
 * \brief - write port to VSQ-E and PG base mapping
 */
shr_error_e dnx_ingress_congestion_dbal_port_to_src_vsq_map_set(
    int unit,
    int core,
    int port,
    int vsq_e,
    int pg_base);

/**
 * \brief - read port to VSQ-E and PG base mapping
 */
shr_error_e dnx_ingress_congestion_dbal_port_to_src_vsq_map_get(
    int unit,
    int core,
    int port,
    int *vsq_e,
    int *pg_base);

/**
 * \brief - clear port to VSQ-E and PG base mapping
 */
shr_error_e dnx_ingress_congestion_dbal_port_to_src_vsq_map_clear(
    int unit,
    int core,
    int port);

/**
 * \brief - return whether vsq-e/f for specified port enabled
 */
shr_error_e dnx_ingress_congestion_dbal_port_to_src_vsq_enable_get(
    int unit,
    int core,
    int port,
    int *enable);

/**
 * \brief -
 * map port to TC-PG profile
 */
shr_error_e dnx_ingress_congestion_dbal_port_tc_pg_profile_set(
    int unit,
    int core,
    int port,
    int tc_pg_profile);

/**
 * \brief -
 * get port to TC-PG profile mapping
 */
shr_error_e dnx_ingress_congestion_dbal_port_tc_pg_profile_get(
    int unit,
    int core,
    int port,
    int *tc_pg_profile);

/**
 * \brief -
 * configure TC->PG mapping per profile
 */
shr_error_e dnx_ingress_congestion_dbal_tc_pg_map_set(
    int unit,
    int core,
    int tc_pg_profile,
    int tc,
    int pg);

/**
 * \brief -
 * get TC->PG mapping per profile
 */
shr_error_e dnx_ingress_congestion_dbal_tc_pg_map_get(
    int unit,
    int core,
    int tc_pg_profile,
    int tc,
    int *pg);

/**
 * \brief -
 * configure PG params
 */
shr_error_e dnx_ingress_congestion_dbal_pg_params_set(
    int unit,
    int core,
    int pg,
    dnx_ingress_congestion_pg_params_t * pg_params);

/**
 * \brief -
 * set PG use port guaranteed property
 */
shr_error_e dnx_ingress_congestion_dbal_pg_use_port_guaranteed_set(
    int unit,
    int core,
    int pg,
    int use_port_guaranteed);

/**
 * \brief -
 * get PG use port guaranteed property
 */
shr_error_e dnx_ingress_congestion_dbal_pg_use_port_guaranteed_get(
    int unit,
    int core,
    int pg,
    int *use_port_guaranteed);

/**
 * \brief -
 * Set VSQ-E resource allocation
 */
shr_error_e dnx_ingress_congestion_dbal_vsq_e_resource_alloc_hw_set(
    int unit,
    int core,
    dbal_tables_e table_id,
    uint32 rt_cls,
    int pool,
    int dp,
    dnx_ingress_congestion_vsq_e_resource_alloc_params_t * resource_alloc_params);

/**
 * \brief -
 * Get VSQ-E resource allocation
 */
shr_error_e dnx_ingress_congestion_dbal_vsq_e_resource_alloc_hw_get(
    int unit,
    int core,
    dbal_tables_e table_id,
    uint32 rt_cls,
    int pool,
    int dp,
    dnx_ingress_congestion_vsq_e_resource_alloc_params_t * resource_alloc_params);

/**
 * \brief -
 * Set VSQ-F resource allocation
 */
shr_error_e dnx_ingress_congestion_dbal_vsq_f_resource_alloc_hw_set(
    int unit,
    int core,
    dbal_tables_e table_id,
    uint32 rt_cls,
    int dp,
    dnx_ingress_congestion_vsq_f_resource_alloc_params_t * resource_alloc_params);

/**
 * \brief -
 * Get VSQ-F resource allocation
 */
shr_error_e dnx_ingress_congestion_dbal_vsq_f_resource_alloc_hw_get(
    int unit,
    int core,
    dbal_tables_e table_id,
    uint32 rt_cls,
    int dp,
    dnx_ingress_congestion_vsq_f_resource_alloc_params_t * resource_alloc_params);

/**
 * \brief -
 * Set global VSQ resource allocation
 */
shr_error_e dnx_ingress_congestion_dbal_global_vsq_resource_alloc_hw_set(
    int unit,
    int core,
    dbal_tables_e table_id,
    int pool_id,
    int is_lossless,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t * resource_alloc_params);

/**
 * \brief -
 * Get global VSQ resource allocation
 */
shr_error_e dnx_ingress_congestion_dbal_global_vsq_resource_alloc_hw_get(
    int unit,
    int core,
    dbal_tables_e table_id,
    int pool_id,
    int is_lossless,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t * resource_alloc_params);

/*
 * -- Global Thresholds  --
 */
shr_error_e dnx_ingress_congestion_dbal_vsq_shared_threshold_hw_set(
    int unit,
    int pool_id,
    int dp,
    dnx_ingress_congestion_resource_type_e resource_type,
    uint32 set_threshold,
    uint32 clear_threshold);

shr_error_e dnx_ingress_congestion_dbal_vsq_shared_threshold_hw_get(
    int unit,
    int pool_id,
    int dp,
    dnx_ingress_congestion_resource_type_e resource_type,
    uint32 *set_threshold,
    uint32 *clear_threshold);

shr_error_e dnx_ingress_congestion_dbal_global_free_sram_threshold_hw_set(
    int unit,
    int index /* either dp or tc */ ,
    int is_dp /* true if index is dp */ ,
    dnx_ingress_congestion_global_sram_resource_t resource_type,
    uint32 set_threshold,
    uint32 clear_threshold);

shr_error_e dnx_ingress_congestion_dbal_global_free_sram_threshold_hw_get(
    int unit,
    int index /* either dp or tc */ ,
    int is_dp /* true if index is dp */ ,
    dnx_ingress_congestion_global_sram_resource_t resource_type,
    uint32 *set_threshold,
    uint32 *clear_threshold);

shr_error_e dnx_ingress_congestion_dbal_global_free_dram_threshold_hw_set(
    int unit,
    int index /* either dp or tc */ ,
    int is_dp /* true if index is dp */ ,
    uint32 set_threshold,
    uint32 clear_threshold);

shr_error_e dnx_ingress_congestion_dbal_global_free_dram_threshold_hw_get(
    int unit,
    int index /* either dp or tc */ ,
    int is_dp /* true if index is dp */ ,
    uint32 *set_threshold,
    uint32 *clear_threshold);

shr_error_e dnx_ingress_congestion_dbal_cmd_fifo_sram_reject_threshold_hw_set(
    int unit,
    dnx_ingress_congestion_cmd_fifo_type_t fifo_id,
    bcm_cosq_forward_decision_type_t fwd_action,
    dnx_ingress_congestion_global_sram_resource_t resource,
    uint32 set_threshold,
    uint32 clear_threshold);

shr_error_e dnx_ingress_congestion_dbal_cmd_fifo_sram_reject_threshold_hw_get(
    int unit,
    dnx_ingress_congestion_cmd_fifo_type_t fifo_id,
    bcm_cosq_forward_decision_type_t fwd_action,
    dnx_ingress_congestion_global_sram_resource_t resource,
    uint32 *set_threshold,
    uint32 *clear_threshold);

/**
 * \brief - Set rate class profile for a VOQ
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] voq - voq index
 * \param [in] rate_class - rate class profile
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_voq_rate_class_set(
    int unit,
    int core,
    uint32 voq,
    uint32 rate_class);

/**
 * \brief - Set traffic class for a VOQ
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] voq - voq index
 * \param [in] traffic_class - traffic class profile
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_voq_traffic_class_set(
    int unit,
    int core,
    uint32 voq,
    uint32 traffic_class);

/**
 * \brief - Set connection class profile for a VOQ
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] voq - voq index
 * \param [in] connection_class - connection class profile
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_voq_connection_class_set(
    int unit,
    int core,
    uint32 voq,
    uint32 connection_class);

/**
 * \brief - clear voq info (rate class, connection class, )
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] voq - voq index
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_voq_info_clear(
    int unit,
    int core,
    uint32 voq);

/**
 * \brief - Get reject status threshold for free resource level per DP
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] dbal_table_id - dbal table id. there is a DBAL table per resource type
 * \param [in] dp - dp (0-3).
 * \param [out] set_thr - threshold to set reject status.
 * \param [out] clear_thr - threshold to clear reject status.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_voq_shared_res_threshold_get(
    int unit,
    int core,
    dbal_tables_e dbal_table_id,
    uint32 dp,
    uint32 *set_thr,
    uint32 *clear_thr);

/**
 * \brief - Set reject status threshold for free resource level per DP
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] dbal_table_id - dbal table id. there is a DBAL table per resource type
 * \param [in] dp - dp (0-3).
 * \param [in] set_thr - threshold to set reject status.
 * \param [in] clear_thr - threshold to clear reject status.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_voq_shared_res_threshold_set(
    int unit,
    int core,
    dbal_tables_e dbal_table_id,
    uint32 dp,
    uint32 set_thr,
    uint32 clear_thr);

/**
 * \brief - Set FADT thresholds to HW
 *
 * \param [in] unit - chip unit id.
 * \param [in] table_id - dbal table id. there is a DBAL table per resource type
 * \param [in] rate_class - rate class profile id.
 * \param [in] is_resource_range - indication if the threshold include free resources limit (means the FADT algorithm is used only between free-res-min and free-res-max).
 * \param [in] fadt - fadt thresholds values.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_fadt_hw_set(
    int unit,
    dbal_tables_e table_id,
    int rate_class,
    int is_resource_range,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief - Get FADT thresholds from HW
 *
 * \param [in] unit - chip unit id.
 * \param [in] table_id - dbal table id. there is a DBAL table per resource type
 * \param [in] rate_class - rate class profile id.
 * \param [in] is_resource_range - indication if the threshold include free resources limit (means the FADT algorithm is used only between free-res-min and free-res-max).
 * \param [out] fadt - fadt thresholds values.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_fadt_hw_get(
    int unit,
    dbal_tables_e table_id,
    int rate_class,
    int is_resource_range,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief - Get next congested PB VSQ PG id and resource from 'voq congestion fifo'
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id
 * \param [out] is_valid - Indication whether the retrieved congestion entry is valid (fifo not empty).
 * \param [out] resource_type -Type of congested resource (Total words, SRAM buffers, SRAM PDs)
 * \param [out] pg_id - congested PG id.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_notification_fifo_pb_vsq_pg_hw_get(
    int unit,
    bcm_core_t core,
    uint32 *is_valid,
    dbal_enum_value_field_congestion_notification_resource_type_e * resource_type,
    uint32 *pg_id);

/**
 * \brief - Get next congested PB VSQ LLFC id and resource from 'voq congestion fifo'
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id
 * \param [out] is_valid - Indication whether the retrieved congestion entry is valid (fifo not empty).
 * \param [out] resource_type -Type of congested resource (Total words, SRAM buffers, SRAM PDs)
 * \param [out] port_index - congested port index.
 * \param [out] pool_id - congested pool ID.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_notification_fifo_pb_vsq_llfc_hw_get(
    int unit,
    bcm_core_t core,
    uint32 *is_valid,
    dbal_enum_value_field_congestion_notification_resource_type_e * resource_type,
    uint32 *port_index,
    uint32 *pool_id);

/**
 * \brief - Get next congested voq id and resource from 'voq congestion fifo'
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id
 * \param [out] is_valid - Indication whether the retrieved congestion entry is valid (fifo not empty).
 * \param [out] resource_type -Type of congested resource (Total words, SRAM buffers, SRAM PDs)
 * \param [out] voq_id - congested voq id (queue index).
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_notification_fifo_voq_hw_get(
    int unit,
    bcm_core_t core,
    uint32 *is_valid,
    dbal_enum_value_field_congestion_notification_resource_type_e * resource_type,
    uint32 *voq_id);

/**
 * \brief - Set DBAL recovery threshold to HW
 *
 * \param [in] unit - chip unit id.
 * \param [in] rate_class - rate class profile id.
 * \param [in] recovery_threshold - recovery threshold value in HW resolution.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_dram_recovery_set(
    int unit,
    int rate_class,
    uint32 recovery_threshold);

/**
 * \brief - Get DBAL recovery threshold from HW
 *
 * \param [in] unit - chip unit id.
 * \param [in] rate_class - rate class profile id.
 * \param [out] recovery_threshold - recovery threshold value in HW resolution.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_dram_recovery_get(
    int unit,
    int rate_class,
    uint32 *recovery_threshold);

/**
 * \brief
 * set rate class as DRAM block eligible for all rate classes
 */
shr_error_e dnx_ingress_congestion_dbal_dram_block_eligible_set_all(
    int unit,
    int dram_block_eligible);

/**
 * \brief - Set rate class Dram usage mode to HW
 *
 * \param [in] unit - chip unit id.
 * \param [in] rate_class - rate class profile id.
 * \param [in] dbal_usage_mode - dram usage mode - normal (SRAM+DRAM), SRAM only, DRAM only.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_dram_use_mode_set(
    int unit,
    int rate_class,
    dbal_enum_value_field_dram_use_mode_e dbal_usage_mode);

/**
 * \brief - Get rate class Dram usage mode from HW
 *
 * \param [in] unit - chip unit id.
 * \param [in] rate_class - rate class profile id.
 * \param [out] dbal_usage_mode - dram usage mode - normal (SRAM+DRAM), SRAM only, DRAM only.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_dram_use_mode_get(
    int unit,
    int rate_class,
    dbal_enum_value_field_dram_use_mode_e * dbal_usage_mode);

/**
 * \brief - set a range of reassembly contexts to override PP ports in the TM command
 *
 * \param [in] unit - chip unit id.
 * \param [in] reassembly_ctxt_min - first reassembly context to set.
 * \param [in] reassembly_ctxt_max - last reassembly context to set.
 * \param [in] enable - enable override indication.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_override_pp_port_with_reassembly_context_enable_range_set(
    int unit,
    int reassembly_ctxt_min,
    int reassembly_ctxt_max,
    uint32 enable);

/**
 * \brief - set a single reassembly contexts to override PP ports in the TM command
 *
 * \param [in] unit - chip unit id.
 * \param [in] reassembly_ctxt - reassembly context to set.
 * \param [in] enable - enable override indication.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_override_pp_port_with_reassembly_context_enable_set(
    int unit,
    int reassembly_ctxt,
    uint32 enable);

/**
 * \brief - Get indication if reassembly contexts override the PP ports in the TM command
 *
 * \param [in] unit - chip unit id.
 * \param [in] reassembly_ctxt - reassembly context to check.
 * \param [in] enable - enable override indication.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_override_pp_port_with_reassembly_context_enable_get(
    int unit,
    int reassembly_ctxt,
    uint32 *enable);

/**
 * \brief - Set ingress congestion init settings - HW values which are not controlled with APIs.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_init_settings_config_set(
    int unit);

/**
 * \brief - Set minimal DP to discard. all packets with equal or greater DP will be discarded
 *
 * \param [in] unit - chip unit id.
 * \param [in] dp_discard - minimal DP to discard.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_global_dp_discard_set(
    int unit,
    uint32 dp_discard);

/**
 * \brief - Get global minimal DP discard. all packets with equal or greater DP will be discarded
 *
 * \param [in] unit - chip unit id.
 * \param [out] dp_discard - minimal DP to discard.
 *
 * \return shr_error_e
 */
shr_error_e dnx_ingress_congestion_dbal_global_dp_discard_get(
    int unit,
    uint32 *dp_discard);

/**
 * \brief
 *   Set admission preferences per DP - guarantee or admit.
 *   if guarantee_over_admit is set- in case some statistic admission test fail (for example WRED),
 *   but the queue size is under the guaranteed, the packet will enter. Otherwise, every admission test failure will lea to packet dropped.
 *
 * \param [in] unit - unit.
 * \param [in] dp - dp 0-3.
 * \param [in] is_guarantee_over_admit - indication whether to respect guaranteed size over admission tests.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dbal_guarantee_over_admit_set(
    int unit,
    int dp,
    uint32 is_guarantee_over_admit);

/**
 * \brief
 *   Get admission preferences per DP - guarantee or admit.
 *
 * \param [in] unit - unit.
 * \param [in] dp - dp 0-3.
 * \param [out] is_guarantee_over_admit - indication whether to respect guaranteed size over admission tests.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ingress_congestion_dbal_guarantee_over_admit_set
 */
shr_error_e dnx_ingress_congestion_dbal_guarantee_over_admit_get(
    int unit,
    int dp,
    uint32 *is_guarantee_over_admit);

/**
 * \brief -
 * set vsq words reject mapping
 */
shr_error_e dnx_ingress_congestion_dbal_vsq_words_reject_map_set(
    int unit,
    int admit_profile,
    const dnx_data_ingr_congestion_init_vsq_words_rjct_map_t * reject_reasons,
    int reject);

/**
 * \brief -
 * set vsq SRAM reject mapping
 */
shr_error_e dnx_ingress_congestion_dbal_vsq_sram_reject_map_set(
    int unit,
    int admit_profile,
    const dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t * reject_reasons,
    int reject);

/**
 * \brief -
 * set reject mask
 */
shr_error_e dnx_ingress_congestion_dbal_reject_mask_set(
    int unit,
    dbal_tables_e dbal_table_id,
    dnx_ingress_congestion_reject_mask_key_t * key,
    dnx_ingress_congestion_reject_mask_t * mask);

/**
 * \brief -
 * set dram block mask
 */
shr_error_e dnx_ingress_congestion_dbal_dram_block_mask_set(
    int unit,
    int dp,
    uint32 mask[]);

/**
 * \brief -
 * set dram soft block default parameters
 */
shr_error_e dnx_ingress_congestion_dbal_dram_soft_block_defaults_set(
    int unit);

/**
 * \brief -
 * set TAR FIFO size(s)
 */
shr_error_e dnx_ingress_congestion_dbal_fifo_size_set(
    int unit,
    uint32 fifo_size);

/**
 * \brief - dbal function to set mirror on drop group info (reject bitmap and type)
 *
 * \param [in] unit - The unit number.
 * \param [in] flags - flags.
 * \param [in] group_id - drop group id.
 * \param [in] group_info - mirror on drop group info.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dbal_mirror_on_drop_group_set(
    int unit,
    uint32 flags,
    int group_id,
    bcm_cosq_mirror_on_drop_group_info_t * group_info);

/**
 * \brief - dbal function to get mirror on drop group info (reject bitmap and type)
 *
 * \param [in] unit - The unit number.
 * \param [in] flags - flags.
 * \param [in] group_id - drop group id.
 * \param [out] group_info - mirror on drop group info.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dbal_mirror_on_drop_group_get(
    int unit,
    uint32 flags,
    int group_id,
    bcm_cosq_mirror_on_drop_group_info_t * group_info);

/**
 * \brief - Low level function to set mirror on drop aging time per group type
 *
 * \param [in] unit - unit.
 * \param [in] type - Group type (bcmCosqControlMirrorOnDropAgingGlobal, bcmCosqControlMirrorOnDropAgingVoq,
 *                     bcmCosqControlMirrorOnDropAgingVsq)
 * \param [out] arg - Aging time in microseconds units
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dbal_mirror_on_drop_aging_set(
    int unit,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief - Low level function to set mirror on drop aging time per group type
 *
 * \param [in] unit - unit.
 * \param [in] type - Group type (bcmCosqControlMirrorOnDropAgingGlobal, bcmCosqControlMirrorOnDropAgingVoq,
 *                     bcmCosqControlMirrorOnDropAgingVsq)
 * \param [out] arg - Aging time in microseconds units
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dbal_mirror_on_drop_aging_get(
    int unit,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief - Init HW defaults for mirror on drop  (shaper)
 *
 * \param [in] unit - unit.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dbal_mirror_on_drop_init(
    int unit);

shr_error_e dnx_ingress_congestion_dbal_threshold_encode(
    int unit,
    uint32 threshold,
    uint32 *encoded_threshold);

/**
 * \brief - write ECN marking threshold to HW
 *
 * \param [in] unit - unit.
 * \param [in] core - device core id.
 * \param [in] resource - resource type
 * \param [in] set_threshold - threshold value to be set in the dbal
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dbal_ecn_marking_threshold_hw_set(
    int unit,
    int core,
    bcm_cosq_resource_t resource,
    uint32 set_threshold);

/**
 * \brief - read ECN marking threshold from HW
 *
 * \param [in] unit - unit.
 * \param [in] core - device core id.
 * \param [in] resource - resource type
 * \param [out] set_threshold - threshold value that will be get from the dbal
 * \param [out] clear_threshold - threshold value that will be get from the dbal
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_dbal_ecn_marking_threshold_hw_get(
    int unit,
    int core,
    bcm_cosq_resource_t resource,
    uint32 *set_threshold,
    uint32 *clear_threshold);
#endif
