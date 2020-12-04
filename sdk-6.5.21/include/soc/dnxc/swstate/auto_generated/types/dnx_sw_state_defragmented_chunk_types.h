
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SW_STATE_DEFRAGMENTED_CHUNK_TYPES_H__
#define __DNX_SW_STATE_DEFRAGMENTED_CHUNK_TYPES_H__

#ifdef BCM_DNX_SUPPORT
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm_int/dnx/algo/consistent_hashing/consistent_hashing_manager.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <soc/dnxc/swstate/types/sw_state_defragmented_chunk.h>


typedef int (*sw_state_defragmented_cunk_move_cb)          (int unit, uint32 source_offset, uint32 destination_offset, uint32 piece_size, void *move_cb_additional_info)  ;



#endif  

#endif 
