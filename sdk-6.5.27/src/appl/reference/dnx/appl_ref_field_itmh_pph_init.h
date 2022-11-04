
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_FIELD_ITMH_PPH_INIT_H_INCLUDED

#define APPL_REF_FIELD_ITMH_PPH_INIT_H_INCLUDED

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

shr_error_e appl_dnx_field_itmh_init(
    int unit);

shr_error_e appl_dnx_field_itmh_pph_init(
    int unit);

#endif
