
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_MANAGER_TYPES_H__
#define __DNX_FIELD_TCAM_MANAGER_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>


typedef shr_error_e (*dnx_field_tcam_context_sharing_handlers_get_p)(int unit, uint32 tcam_handler_id, uint32 tcam_handler_ids[]);




typedef enum {
    DNX_FIELD_TCAM_HANDLER_MODE_INVALID = -1,
    
    DNX_FIELD_TCAM_HANDLER_MODE_DEFAULT = 0,
    
    DNX_FIELD_TCAM_HANDLER_MODE_NOF_MODES = 1
} dnx_field_tcam_handler_mode_e;


typedef enum {
    DNX_FIELD_TCAM_HANDLER_STATE_INVALID = -1,
    
    DNX_FIELD_TCAM_HANDLER_STATE_CLOSE = 0,
    
    DNX_FIELD_TCAM_HANDLER_STATE_OPEN = 1,
    
    DNX_FIELD_TCAM_HANDLER_STATE_NOF_STATES = 2
} dnx_field_tcam_handler_state_e;




typedef struct {
    
    dnx_field_tcam_handler_mode_e mode;
    
    dnx_field_tcam_handler_state_e state;
} dnx_field_tcam_handler_t;


typedef struct {
    
    dnx_field_tcam_handler_t* tcam_handlers;
} dnx_field_tcam_manager_sw_t;


#endif 
