/** \file res_mngr_callbacks.c
 *
 * Callbacks functions for resource manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESOURCEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_callbacks.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_advanced_algorithms.h>
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>
#include <bcm_int/dnx/algo/field/ffc_alloc_mngr.h>
#include <bcm_int/dnx/algo/failover/algo_failover.h>
#include <bcm_int/dnx/algo/multicast/algo_multicast.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <soc/dnx/mdb.h>
#include "../l3/algo_l3_ecmp.h"
#include "../l3/algo_l3_fec.h"
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>

/*
 * }
 */
const dnx_algo_res_advanced_alogrithm_cb_t Resource_callbacks_map[DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_COUNT] = {
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC
     */
    {resource_tag_bitmap_create,
     resource_tag_bitmap_allocate_single,
     resource_tag_bitmap_is_allocated,
     resource_tag_bitmap_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     resource_tag_bitmap_clear,
     resource_tag_bitmap_allocate_several,
     resource_tag_bitmap_free_several,
     NULL,
     sw_state_resource_nof_allocated_elements_in_range_get,
     resource_tag_bitmap_tag_set,
     resource_tag_bitmap_tag_get},
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_LOCAL_OUTLIF
     */
    {dnx_algo_local_outlif_resource_create,
     dnx_algo_local_outlif_resource_allocate,
     dnx_algo_local_outlif_resource_is_allocated,
     NULL,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     NULL,
     resource_tag_bitmap_allocate_several,
     resource_tag_bitmap_free_several,
     NULL,
     sw_state_resource_nof_allocated_elements_in_range_get,
     resource_tag_bitmap_tag_set,
     NULL},
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_LOCAL_INLIF
     */
    {dnx_lif_local_inlif_resource_create,
     dnx_lif_local_inlif_resource_allocate,
     dnx_lif_local_inlif_resource_is_allocated,
     dnx_lif_local_inlif_resource_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     NULL,
     NULL,
     dnx_lif_local_inlif_resource_free_several,
     NULL,
     sw_state_resource_nof_allocated_elements_in_range_get,
     NULL,
     NULL},
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_IQM
     */
    {dnx_cosq_ipq_alloc_mngr_queue_qrtt_bitmap_create,
     dnx_cosq_ipq_alloc_mngr_queue_qrtt_bitmap_allocate,
     resource_tag_bitmap_is_allocated,
     dnx_cosq_ipq_alloc_mngr_queue_qrtt_bitmap_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     NULL,
     NULL,
     NULL,
     NULL,
     sw_state_resource_nof_allocated_elements_in_range_get,
     NULL,
     NULL},
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_SCH
     */
    {dnx_algo_sch_bitmap_create,
     dnx_algo_sch_bitmap_allocate,
     dnx_algo_sch_bitmap_is_allocated,
     dnx_algo_sch_bitmap_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     NULL, /** clear */
     NULL, /** allocate_several */
     NULL, /** free_several */
     NULL,
     sw_state_resource_nof_allocated_elements_in_range_get,
     NULL, /** bitmap_tag_set */
     NULL}, /** bitmap_tag_get */
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_FFC
     */
    {dnx_algo_ffc_ipmf_1_bitmap_create,
     dnx_algo_ffc_ipmf_1_bitmap_allocate,
     resource_tag_bitmap_is_allocated,
     resource_tag_bitmap_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL},
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_FAILOVER_FEC_PATH_SELECT
     */
    {dnx_algo_failover_fec_path_select_bitmap_create,
     dnx_algo_failover_fec_path_select_bitmap_allocate,
     resource_tag_bitmap_is_allocated,
     dnx_algo_failover_fec_path_select_bitmap_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     NULL,
     dnx_algo_failover_fec_path_select_bitmap_several_allocate,
     dnx_algo_failover_fec_path_select_bitmap_several_free,
     NULL,
     sw_state_resource_nof_allocated_elements_in_range_get,
     NULL,
     NULL},
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_ECMP
     */
    {dnx_algo_l3_ecmp_bitmap_create,
     dnx_algo_l3_ecmp_bitmap_allocate,
     resource_tag_bitmap_is_allocated,
     dnx_algo_l3_ecmp_bitmap_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL},
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_FEC
     */
    {dnx_algo_l3_fec_bitmap_create,
     dnx_algo_l3_fec_bitmap_allocate,
     resource_tag_bitmap_is_allocated,
     dnx_algo_l3_fec_bitmap_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL},
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_GLOBAL_LIF
     */
    {dnx_algo_global_lif_res_create,
     dnx_algo_global_lif_res_allocate,
     resource_tag_bitmap_is_allocated,
     resource_tag_bitmap_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL},
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_PORT_TPID_TAG_STRUCT
     */
    {dnx_port_tpid_tag_struct_hw_resource_create,
     dnx_port_tpid_tag_struct_hw_resource_allocate,
     resource_tag_bitmap_is_allocated,
     resource_tag_bitmap_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     resource_tag_bitmap_clear,
     resource_tag_bitmap_allocate_several,
     resource_tag_bitmap_free_several,
     NULL},
    /*
     * DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_MULTICAST_MCDB
     */
    {dnx_algo_multicast_bitmap_create,
     resource_tag_bitmap_allocate_single,
     resource_tag_bitmap_is_allocated,
     resource_tag_bitmap_free,
     resource_tag_bitmap_destroy,
     resource_tag_bitmap_nof_free_elements_get,
     resource_tag_bitmap_clear,
     resource_tag_bitmap_allocate_several,
     resource_tag_bitmap_free_several,
     NULL,
     sw_state_resource_nof_allocated_elements_in_range_get,
     resource_tag_bitmap_tag_set,
     resource_tag_bitmap_tag_get},
};
