/** \file template_mngr_callbacks.c
 *
 * Callbacks functions for template manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/algo/rx/trap_mtu_profile_allocation.h>
#include <bcm_int/dnx/algo/rx/trap_etpp_lif_profile_allocation.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_even_distribution.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/types/example_temp_mngr_types.h>

/*
 * Print callback for the algo template ctest.
 */
void
dnx_algo_template_test_print_cb(
    int unit,
    const void *data)
{
    template_mngr_example_data_t *template_data = (template_mngr_example_data_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "element_8", template_data->element_8, NULL,
                                   "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "element_16", template_data->element_16, NULL,
                                   "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "element_32", template_data->element_32, NULL,
                                   "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "elemrnt_int", template_data->element_int, NULL,
                                   "0x%x");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * }
 */
/* *INDENT-OFF* */
const dnx_algo_template_advanced_alogrithm_cb_t Template_callbacks_map_sw[DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_COUNT] = {
    /*
     * DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC
     */
    {multi_set_create,
     multi_set_member_add,
     NULL,
     multi_set_get_by_index,
     multi_set_member_lookup,
     multi_set_member_remove_by_index_multiple,
     multi_set_destroy,
     NULL,
     multi_set_clear,
     NULL,
     NULL,
     NULL,
     NULL},
     /*
      * DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SMART_TEMPLATE
      */
     {dnx_algo_smart_template_create,
     dnx_algo_smart_template_allocate,
     NULL,
     multi_set_get_by_index,
     multi_set_member_lookup,
     dnx_algo_smart_template_free,
     NULL,
     NULL,
     dnx_algo_smart_template_clear,
     NULL,
     NULL,
     dnx_algo_smart_template_tag_set,
     dnx_algo_smart_template_tag_get},
     /*
      * DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_L3_SOURCE_TABLE
      */
     {dnx_algo_l3_source_address_table_create,
      dnx_algo_l3_source_address_table_allocate,
      NULL,
      multi_set_get_by_index,
      multi_set_member_lookup,
      dnx_algo_l3_source_address_table_free,
      NULL,
      NULL,
      dnx_algo_l3_source_address_table_clear,
      NULL,
      NULL,
      NULL,
      NULL},
    /*
     * DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_TRAP_ETPP_LIF_PROFILE
     */
    {multi_set_create,
     dnx_algo_rx_trap_etpp_lif_profile_allocate,
     NULL,
     multi_set_get_by_index,
     multi_set_member_lookup,
     multi_set_member_remove_by_index_multiple,
     multi_set_destroy,
     NULL,
     multi_set_clear,
     NULL,
     NULL,
     NULL,
     NULL},
    /*
     * DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SMART_TEMPLATE_EVEN_DISTRIBUTION
     */
    {dnx_algo_smart_template_create,
     dnx_algo_template_mngr_even_distribution_allocate,
     NULL,
     multi_set_get_by_index,
     multi_set_member_lookup,
     dnx_algo_template_mngr_even_distribution_free,
     NULL,
     NULL,
     dnx_algo_smart_template_clear,
     NULL,
     NULL,
     dnx_algo_smart_template_tag_set,
     dnx_algo_smart_template_tag_get},
};
/* *INDENT-ON* */
