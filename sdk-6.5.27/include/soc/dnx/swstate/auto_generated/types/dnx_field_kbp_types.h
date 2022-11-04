
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_KBP_TYPES_H__
#define __DNX_FIELD_KBP_TYPES_H__

#ifdef INCLUDE_KBP
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>


#define DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT SAL_UINT8_NOF_BITS

#define DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID ((uint8)(-1))

#define DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY (45)



typedef struct {
    uint8 qual_idx[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT];
    dnx_field_group_t fg_id;
} dnx_field_kbp_segment_info_t;

typedef struct {
    dnx_field_kbp_segment_info_t segment_info[DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint8 nof_fwd_segments;
} dnx_field_kbp_master_key_info_t;

typedef struct {
    dnx_field_kbp_master_key_info_t master_key_info;
    uint8 is_valid;
} dnx_field_kbp_opcode_info_t;

typedef struct {
    dnx_field_kbp_opcode_info_t* opcode_info;
} dnx_field_kbp_sw_t;

#endif  

#endif 
