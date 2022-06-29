
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef FIELD_TCAM_H_INCLUDED

#define FIELD_TCAM_H_INCLUDED

#include <soc/sand/shrextend/shrextend_error.h>

#define DNX_FIELD_TCAM_SHADOW_KEEP  (1)

#define DNX_FIELD_TCAM_SHADOW_READ  (1 && DNX_FIELD_TCAM_SHADOW_KEEP)

shr_error_e dnx_field_tcam_sw_state_init(
    int unit);

#endif
