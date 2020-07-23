
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_MANAGER_ACCESS_H__
#define __DNX_FIELD_TCAM_MANAGER_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



typedef int (*dnx_field_tcam_manager_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_tcam_manager_sw_init_cb)(
    int unit);


typedef int (*dnx_field_tcam_manager_sw_tcam_handlers_set_cb)(
    int unit, uint32 tcam_handlers_idx_0, CONST dnx_field_tcam_handler_t *tcam_handlers);


typedef int (*dnx_field_tcam_manager_sw_tcam_handlers_get_cb)(
    int unit, uint32 tcam_handlers_idx_0, dnx_field_tcam_handler_t *tcam_handlers);


typedef int (*dnx_field_tcam_manager_sw_tcam_handlers_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_manager_sw_tcam_handlers_mode_set_cb)(
    int unit, uint32 tcam_handlers_idx_0, dnx_field_tcam_handler_mode_e mode);


typedef int (*dnx_field_tcam_manager_sw_tcam_handlers_mode_get_cb)(
    int unit, uint32 tcam_handlers_idx_0, dnx_field_tcam_handler_mode_e *mode);


typedef int (*dnx_field_tcam_manager_sw_tcam_handlers_state_set_cb)(
    int unit, uint32 tcam_handlers_idx_0, dnx_field_tcam_handler_state_e state);


typedef int (*dnx_field_tcam_manager_sw_tcam_handlers_state_get_cb)(
    int unit, uint32 tcam_handlers_idx_0, dnx_field_tcam_handler_state_e *state);




typedef struct {
    dnx_field_tcam_manager_sw_tcam_handlers_mode_set_cb set;
    dnx_field_tcam_manager_sw_tcam_handlers_mode_get_cb get;
} dnx_field_tcam_manager_sw_tcam_handlers_mode_cbs;


typedef struct {
    dnx_field_tcam_manager_sw_tcam_handlers_state_set_cb set;
    dnx_field_tcam_manager_sw_tcam_handlers_state_get_cb get;
} dnx_field_tcam_manager_sw_tcam_handlers_state_cbs;


typedef struct {
    dnx_field_tcam_manager_sw_tcam_handlers_set_cb set;
    dnx_field_tcam_manager_sw_tcam_handlers_get_cb get;
    dnx_field_tcam_manager_sw_tcam_handlers_alloc_cb alloc;
    
    dnx_field_tcam_manager_sw_tcam_handlers_mode_cbs mode;
    
    dnx_field_tcam_manager_sw_tcam_handlers_state_cbs state;
} dnx_field_tcam_manager_sw_tcam_handlers_cbs;


typedef struct {
    dnx_field_tcam_manager_sw_is_init_cb is_init;
    dnx_field_tcam_manager_sw_init_cb init;
    
    dnx_field_tcam_manager_sw_tcam_handlers_cbs tcam_handlers;
} dnx_field_tcam_manager_sw_cbs;






const char *
dnx_field_tcam_handler_mode_e_get_name(dnx_field_tcam_handler_mode_e value);



const char *
dnx_field_tcam_handler_state_e_get_name(dnx_field_tcam_handler_state_e value);




extern dnx_field_tcam_manager_sw_cbs dnx_field_tcam_manager_sw;

#endif 
