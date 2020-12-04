
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_RANGE_TYPES_H__
#define __DNX_FIELD_RANGE_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



typedef enum {
    
    DNX_FIELD_RANGE_TYPE_INVALID = -1,
    
    DNX_FIELD_RANGE_TYPE_FIRST = 0,
    
    DNX_FIELD_RANGE_TYPE_L4_SRC_PORT = DNX_FIELD_RANGE_TYPE_FIRST,
    
    DNX_FIELD_RANGE_TYPE_L4_DST_PORT,
    
    DNX_FIELD_RANGE_TYPE_OUT_LIF,
    
    DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE,
    
    DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE,
    
    DNX_FIELD_RANGE_TYPE_IN_TTL,
    
    DNX_FIELD_RANGE_TYPE_FFC1_LOW,
    
    DNX_FIELD_RANGE_TYPE_FFC1_HIGH,
    
    DNX_FIELD_RANGE_TYPE_FFC2_LOW,
    
    DNX_FIELD_RANGE_TYPE_FFC2_HIGH,
    
    DNX_FIELD_RANGE_TYPE_NOF
} dnx_field_range_type_e;




typedef struct {
    
    dnx_field_qual_t dnx_qual;
    
    dnx_field_qual_attach_info_t qual_info;
} dnx_field_range_type_qual_info_t;


typedef struct {
    
    dnx_field_range_type_e ext_l4_ops_range_types[DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES];
    
    dnx_field_range_type_qual_info_t ext_l4_ops_ffc_quals[DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES];
} dnx_field_range_sw_db_t;


#endif 
