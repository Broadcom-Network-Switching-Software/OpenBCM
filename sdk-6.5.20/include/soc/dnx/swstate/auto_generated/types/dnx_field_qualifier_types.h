
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_QUALIFIER_TYPES_H__
#define __DNX_FIELD_QUALIFIER_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



typedef enum {
    
    DNX_FIELD_QUALIFIER_FLAG_WITH_ID = 0x1,
    
    DNX_FIELD_QUALIFIER_FLAG_NOF
} dnx_field_qual_flags_e;




typedef struct {
    
    uint32 valid;
    
    dbal_fields_e field_id;
    
    uint32 size;
    
    int bcm_id;
    
    uint32 ref_count;
    
    dnx_field_qual_flags_e flags;
} dnx_field_user_qual_info_t;


typedef struct {
    
    dnx_field_user_qual_info_t info[DNX_DATA_MAX_FIELD_QUAL_USER_NOF];
} dnx_field_qual_sw_db_t;


#endif 
