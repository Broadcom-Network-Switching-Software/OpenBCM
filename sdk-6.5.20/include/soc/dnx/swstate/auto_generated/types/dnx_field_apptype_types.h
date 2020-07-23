
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_APPTYPE_TYPES_H__
#define __DNX_FIELD_APPTYPE_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>



typedef enum {
    
    DNX_FIELD_APPTYPE_FLAG_WITH_ID = 0x1,
    DNX_FIELD_APPTYPE_FLAG_NOF
} dnx_field_apptype_flags_e;




typedef struct {
    
    uint8 nof_context_ids;
    
    uint8 context_ids[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    
    uint8 valid_for_kbp;
} dnx_field_predef_apptype_info_t;


typedef struct {
    
    uint8 opcode_id;
    
    uint8 acl_context;
    
    uint8 base_apptype;
    
    dnx_field_apptype_flags_e flags;
    
    sw_state_string_t name[DBAL_MAX_STRING_LENGTH];
    
    uint8 profile_id;
} dnx_field_user_apptype_info_t;


typedef struct {
    
    dnx_field_predef_apptype_info_t predef_info[bcmFieldAppTypeCount];
    
    dnx_field_user_apptype_info_t user_def_info[DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF];
} dnx_field_apptype_sw_t;


#endif 
