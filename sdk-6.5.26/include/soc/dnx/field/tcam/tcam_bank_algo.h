
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_BANK_ALGO_H_INCLUDED

#define TCAM_BANK_ALGO_H_INCLUDED

#include <soc/dnx/field/tcam/tcam_entities.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_bank_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>

shr_error_e dnx_field_tcam_bank_algo_check_context_collision(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
    uint8 *collide);

shr_error_e dnx_field_tcam_bank_algo_get_owner_stage(
    int unit,
    int bank_id,
    bcm_core_t core_id,
    dnx_field_tcam_stage_e * bank_owner_stage);

shr_error_e dnx_field_tcam_bank_algo_get_handler_node_in_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
    sw_state_ll_node_t * handler_node);

shr_error_e dnx_field_tcam_bank_algo_handler_in_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
    uint8 *handler_in_bank);

shr_error_e dnx_field_tcam_bank_algo_find_id(
    int unit,
    uint32 handler_id,
    bcm_core_t core_id,
    int *bank_id);

shr_error_e dnx_field_tcam_bank_algo_bank_contains_half_key_handler(
    int unit,
    int bank_id,
    bcm_core_t core_id,
    uint8 *contains_half_key_handler);

#endif
