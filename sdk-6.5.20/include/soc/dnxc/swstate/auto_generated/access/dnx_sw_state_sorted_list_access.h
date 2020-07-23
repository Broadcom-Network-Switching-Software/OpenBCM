
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SW_STATE_SORTED_LIST_ACCESS_H__
#define __DNX_SW_STATE_SORTED_LIST_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/auto_generated/types/dnx_sw_state_sorted_list_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>



shr_error_e
dnx_sw_state_sorted_list_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, dnx_sw_state_sorted_list_cb * cb);



#endif 
