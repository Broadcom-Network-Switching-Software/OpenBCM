/**
* \file        tcam_entities.c
*
* This file implements all the TCAM entities functions.
 */
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/dnx/field/tcam/tcam_entities.h>
#include <bcm_int/dnx/field/tcam/tcam_prefix_manager.h>

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

shr_error_e
dnx_field_tcam_database_t_init(
    int unit,
    dnx_field_tcam_database_t * db)
{
    SHR_FUNC_INIT_VARS(unit);

    db->prefix_size = 0;
    db->prefix_value = DNX_FIELD_TCAM_PREFIX_VAL_AUTO;
    db->bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO;
    db->nof_banks_to_alloc = 0;
    db->nof_restricted_tcam_handlers = 0;
    db->actual_action_size = 0;
    db->action_size = 0;
    db->key_size = 0;
    db->is_direct = FALSE;

    db->stage = DNX_FIELD_TCAM_STAGE_IPMF1;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
