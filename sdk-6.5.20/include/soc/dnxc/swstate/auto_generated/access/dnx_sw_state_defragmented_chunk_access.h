
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SW_STATE_DEFRAGMENTED_CHUNK_ACCESS_H__
#define __DNX_SW_STATE_DEFRAGMENTED_CHUNK_ACCESS_H__

#ifdef BCM_DNX_SUPPORT
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/auto_generated/types/dnx_sw_state_defragmented_chunk_types.h>
#include <bcm_int/dnx/algo/consistent_hashing/consistent_hashing_manager.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>



shr_error_e
sw_state_defragmented_cunk_move_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, sw_state_defragmented_cunk_move_cb * cb);


#endif  

#endif 
