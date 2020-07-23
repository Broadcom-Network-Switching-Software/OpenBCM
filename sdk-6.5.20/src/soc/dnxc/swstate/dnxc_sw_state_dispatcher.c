
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/dnxc/swstate/sw_state_features.h>




#include <assert.h>
#include <shared/bsl.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>
#include <soc/dnxc/swstate/dnxc_sw_state_common.h>
#include <soc/dnxc/swstate/types/sw_state_mutex_plain.h>
#include <soc/dnxc/swstate/types/sw_state_mutex_wb.h>
#include <soc/dnxc/swstate/types/sw_state_sem_plain.h>
#include <soc/dnxc/swstate/types/sw_state_sem_wb.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>




#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL


dnxc_sw_state_dispatcher_cb dnxc_sw_state_dispatcher[SOC_MAX_NUM_DEVICES][NOF_MODULE_ID];


shr_error_e dnxc_sw_state_dispatcher_module_init(
    int unit,
    uint32 module_id,
    dnxc_sw_state_implementation_e implementation)
{
#if defined(BCM_WARM_BOOT_SUPPORT)
    if (dnxc_sw_state_is_warmboot_supported_get(unit) && implementation == DNXC_SW_STATE_IMPLEMENTATION_WB) {
        dnxc_sw_state_dispatcher[unit][module_id].module_init = dnxc_sw_state_module_init_wb;
        dnxc_sw_state_dispatcher[unit][module_id].module_deinit = dnxc_sw_state_module_deinit_wb;
        dnxc_sw_state_dispatcher[unit][module_id].alloc = dnxc_sw_state_alloc_wb;
        dnxc_sw_state_dispatcher[unit][module_id].free = dnxc_sw_state_free_wb;
        dnxc_sw_state_dispatcher[unit][module_id].alloc_size = dnxc_sw_state_alloc_size_wb;
        dnxc_sw_state_dispatcher[unit][module_id].memcpy = dnxc_sw_state_memcpy_common;
        dnxc_sw_state_dispatcher[unit][module_id].memset = dnxc_sw_state_memset_common;
        dnxc_sw_state_dispatcher[unit][module_id].counter_inc = dnxc_sw_state_counter_inc_common;
        dnxc_sw_state_dispatcher[unit][module_id].counter_dec = dnxc_sw_state_counter_dec_common;
        dnxc_sw_state_dispatcher[unit][module_id].mutex_create = dnxc_sw_state_mutex_create_wb;
        dnxc_sw_state_dispatcher[unit][module_id].mutex_destroy = dnxc_sw_state_mutex_destroy_wb;
        dnxc_sw_state_dispatcher[unit][module_id].mutex_take = dnxc_sw_state_mutex_take_wb;
        dnxc_sw_state_dispatcher[unit][module_id].mutex_give = dnxc_sw_state_mutex_give_wb;
        dnxc_sw_state_dispatcher[unit][module_id].sem_create = dnxc_sw_state_sem_create_wb;
        dnxc_sw_state_dispatcher[unit][module_id].sem_destroy = dnxc_sw_state_sem_destroy_wb;
        dnxc_sw_state_dispatcher[unit][module_id].sem_take = dnxc_sw_state_sem_take_wb;
        dnxc_sw_state_dispatcher[unit][module_id].sem_give = dnxc_sw_state_sem_give_wb;
        dnxc_sw_state_dispatcher[unit][module_id].start_measurement_point = dnxc_sw_state_wb_start_size_measurement_point;
        dnxc_sw_state_dispatcher[unit][module_id].size_measurement_get = dnxc_sw_state_wb_size_measurement_get;
        dnxc_sw_state_dispatcher[unit][module_id].calc_pointer = dnxc_sw_state_wb_calc_pointer;
        dnxc_sw_state_dispatcher[unit][module_id].calc_dpointer = dnxc_sw_state_wb_calc_double_pointer;
        dnxc_sw_state_dispatcher[unit][module_id].calc_offset = dnxc_sw_state_wb_calc_offset;
        dnxc_sw_state_dispatcher[unit][module_id].calc_offset_from_dptr = dnxc_sw_state_wb_calc_offset_from_dptr;
        return SOC_E_NONE;
    }
#endif
     dnxc_sw_state_dispatcher[unit][module_id].mutex_create = dnxc_sw_state_mutex_create;
     dnxc_sw_state_dispatcher[unit][module_id].mutex_destroy = dnxc_sw_state_mutex_destroy;
     dnxc_sw_state_dispatcher[unit][module_id].mutex_take = dnxc_sw_state_mutex_take;
     dnxc_sw_state_dispatcher[unit][module_id].mutex_give = dnxc_sw_state_mutex_give;
     dnxc_sw_state_dispatcher[unit][module_id].sem_create = dnxc_sw_state_sem_create;
     dnxc_sw_state_dispatcher[unit][module_id].sem_destroy = dnxc_sw_state_sem_destroy;
     dnxc_sw_state_dispatcher[unit][module_id].sem_take = dnxc_sw_state_sem_take;
     dnxc_sw_state_dispatcher[unit][module_id].sem_give = dnxc_sw_state_sem_give;
     dnxc_sw_state_dispatcher[unit][module_id].module_init = dnxc_sw_state_module_init_plain;
     dnxc_sw_state_dispatcher[unit][module_id].module_deinit = dnxc_sw_state_module_deinit_plain;
     dnxc_sw_state_dispatcher[unit][module_id].alloc = dnxc_sw_state_alloc_plain;
     dnxc_sw_state_dispatcher[unit][module_id].free = dnxc_sw_state_free_plain;
     dnxc_sw_state_dispatcher[unit][module_id].alloc_size = dnxc_sw_state_alloc_size_plain;
     dnxc_sw_state_dispatcher[unit][module_id].memcpy = dnxc_sw_state_memcpy_common;
     dnxc_sw_state_dispatcher[unit][module_id].memset = dnxc_sw_state_memset_common;
     dnxc_sw_state_dispatcher[unit][module_id].counter_inc = dnxc_sw_state_counter_inc_common;
     dnxc_sw_state_dispatcher[unit][module_id].counter_dec = dnxc_sw_state_counter_dec_common;
     dnxc_sw_state_dispatcher[unit][module_id].start_measurement_point = dnxc_sw_state_plain_start_size_measurement_point;
     dnxc_sw_state_dispatcher[unit][module_id].size_measurement_get = dnxc_sw_state_plain_size_measurement_get;
     dnxc_sw_state_dispatcher[unit][module_id].calc_pointer = dnxc_sw_state_plain_calc_pointer;
     dnxc_sw_state_dispatcher[unit][module_id].calc_dpointer = dnxc_sw_state_plain_calc_double_pointer;
     dnxc_sw_state_dispatcher[unit][module_id].calc_offset = dnxc_sw_state_plain_calc_offset;
     dnxc_sw_state_dispatcher[unit][module_id].calc_offset_from_dptr = dnxc_sw_state_plain_calc_offset_from_dptr;
    return SOC_E_NONE;
}


shr_error_e dnxc_sw_state_dispatcher_module_deinit(int unit)
{
    return SOC_E_NONE;
}

#undef _ERR_MSG_MODULE_NAME
