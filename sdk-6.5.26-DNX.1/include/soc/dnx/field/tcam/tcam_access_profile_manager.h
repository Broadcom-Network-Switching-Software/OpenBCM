
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_ACCESS_PROFILE_MANAGER_H_INCLUDED

#define TCAM_ACCESS_PROFILE_MANAGER_H_INCLUDED

#include <soc/dnx/field/tcam/tcam_prefix_manager.h>

#define FIELD_TCAM_ACCESS_PROFILE_ID_AUTO     -1
#define FIELD_TCAM_ACCESS_PROFILE_ID_INVALID  0x1023

shr_error_e dnx_field_tcam_access_profile_alloc(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    int access_profile_id);

shr_error_e dnx_field_tcam_access_profile_bank_add(
    int unit,
    uint32 handler_id,
    int bank_id,
    bcm_core_t core_id);

shr_error_e dnx_field_tcam_access_profile_bank_remove(
    int unit,
    uint32 handler_id,
    int bank_id,
    bcm_core_t core_id);

shr_error_e dnx_field_tcam_access_profile_free(
    int unit,
    uint32 handler_id);

shr_error_e dnx_field_tcam_access_profile_banks_bmp_get(
    int unit,
    uint32 handler_id,
    bcm_core_t core_id,
    uint32 *banks_bmp);

shr_error_e dnx_field_tcam_access_profile_banks_sorted_get(
    int unit,
    uint32 handler_id,
    bcm_core_t core_id,
    int *bank_ids,
    int *bank_count);

shr_error_e dnx_field_tcam_access_profile_id_get(
    int unit,
    uint32 handler_id,
    int *access_profile_id);

shr_error_e dnx_field_tcam_access_profile_intersect(
    int unit,
    uint32 handler_id1,
    uint32 handler_id2,
    uint8 *intersect);

shr_error_e dnx_field_tcam_access_profile_prefix_write(
    int unit,
    uint32 handler_id,
    bcm_core_t core_id,
    uint32 prefix_value);

shr_error_e dnx_field_tcam_access_profile_reserve(
    int unit,
    int access_profile_id);

#endif
