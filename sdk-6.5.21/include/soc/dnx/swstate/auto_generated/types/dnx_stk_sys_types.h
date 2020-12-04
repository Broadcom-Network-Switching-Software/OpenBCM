
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_STK_SYS_TYPES_H__
#define __DNX_STK_SYS_TYPES_H__

#ifdef BCM_DNX_SUPPORT
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>



typedef struct {
    
    int nof_fap_ids;
    
    int module_enable_done;
} dnx_stk_sys_db_t;

#endif  

#endif 
