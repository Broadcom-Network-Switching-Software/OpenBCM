
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_DEFRAG_H_INCLUDED

#define TCAM_DEFRAG_H_INCLUDED

#include <soc/dnx/field/tcam/tcam_location_manager.h>

#define DNX_FIELD_TCAM_DEFRAG_MODE_DEFAULT 0

shr_error_e dnx_field_tcam_defrag(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location);

shr_error_e dnx_field_tcam_defrag_mode(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location,
    int mode);

shr_error_e dnx_field_tcam_defrag_bank_evacuate(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[],
    bcm_core_t core_ids[]);

#endif
