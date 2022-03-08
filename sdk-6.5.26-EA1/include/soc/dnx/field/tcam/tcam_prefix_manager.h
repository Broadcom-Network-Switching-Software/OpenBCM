
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_PREFIX_MANAGER_H_INCLUDED

#define TCAM_PREFIX_MANAGER_H_INCLUDED

#include <soc/dnx/field/tcam/tcam_entities.h>

#define DNX_FIELD_TCAM_PREFIX_VAL_AUTO 0x512

shr_error_e dnx_field_tcam_prefix_set(
    int unit,
    uint32 handler_id,
    int banks_count,
    int bank_ids[],
    bcm_core_t core_ids[]);

shr_error_e dnx_field_tcam_prefix_bank_alloc(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id);

shr_error_e dnx_field_tcam_prefix_free(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id);

shr_error_e dnx_field_tcam_prefix_is_free(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
    uint8 *is_free);

#endif
