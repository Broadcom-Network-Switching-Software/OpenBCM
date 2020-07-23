
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __KBP_FWD_TCAM_ACCESS_MAPPER_TYPES_H__
#define __KBP_FWD_TCAM_ACCESS_MAPPER_TYPES_H__

#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>



typedef enum {
    DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV4_MC = 0x0,
    DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV6_MC = 0x1,
    DNX_KBP_MAX_NOF_TCAM_HASH_TABLE_INDICES = 0x2
} dnx_kbp_fwd_tcam_hash_table_index_e;



typedef struct {
    uint32 key[DNX_KBP_MAX_FWD_KEY_SIZE_IN_WORDS];
    uint32 key_mask[DNX_KBP_MAX_FWD_KEY_SIZE_IN_WORDS];
} dnx_kbp_fwd_tcam_access_hash_key_t;


typedef struct {
    
    sw_state_htbl_t key_2_entry_id_hash[DNX_KBP_MAX_NOF_TCAM_HASH_TABLE_INDICES];
} kbp_fwd_tcam_access_mapper_t;

#endif  

#endif 
