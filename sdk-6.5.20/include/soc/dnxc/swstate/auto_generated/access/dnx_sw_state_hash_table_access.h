
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SW_STATE_HASH_TABLE_ACCESS_H__
#define __DNX_SW_STATE_HASH_TABLE_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/auto_generated/types/dnx_sw_state_hash_table_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>



shr_error_e
dnx_sw_state_hash_table_print_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, dnx_sw_state_hash_table_print_cb * cb);



#endif 
