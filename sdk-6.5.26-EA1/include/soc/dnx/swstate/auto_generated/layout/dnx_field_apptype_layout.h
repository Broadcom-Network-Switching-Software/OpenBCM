
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_APPTYPE_LAYOUT_H__
#define __DNX_FIELD_APPTYPE_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_field_apptype_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_FIELD_APPTYPE_MODULE_ID, 0),
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO,
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO,
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS,
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS,
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP,
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID,
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT,
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE,
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID,
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS,
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME,
    
    DNX_SW_STATE_DNX_FIELD_APPTYPE_SW_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_field_apptype_node_id_e;

#endif 
