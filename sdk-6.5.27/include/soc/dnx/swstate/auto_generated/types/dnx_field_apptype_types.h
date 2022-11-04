
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_APPTYPE_TYPES_H__
#define __DNX_FIELD_APPTYPE_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>


#define DNX_FIELD_APPTYPE_SWSTATE_ACL_CONTEXT_INVALID ((uint8)(DNX_FIELD_CONTEXT_ID_INVALID))



typedef struct {
    uint8 is_valid;
    uint8 apptype;
    uint8 cs_profile_id;
    uint8 nof_fwd2_context_ids;
    uint8 fwd2_context_ids[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    uint8 valid_for_kbp;
} dnx_field_predef_opcode_apptype_info_t;

typedef struct {
    uint8 apptype;
    uint8 acl_context;
    uint8 base_opcode;
    uint8 profile_id;
    sw_state_string_t name[DBAL_MAX_STRING_LENGTH];
} dnx_field_user_opcode_info_t;

typedef struct {
    dnx_field_predef_opcode_apptype_info_t opcode_predef_info[DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF];
    dnx_field_user_opcode_info_t user_def_info[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF];
} dnx_field_apptype_sw_t;


#endif 
