
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_FIELD_ITMH_PPH_INIT_H_INCLUDED

#define APPL_REF_FIELD_ITMH_PPH_INIT_H_INCLUDED

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

#define ITMH_PPH_NOF_FG                      11

typedef shr_error_e(
    *field_itmh_pph_config_cb) (
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info);

typedef struct
{
    char *name;
    int context_ids;
    field_itmh_pph_config_cb config_cb;
} field_itmh_pph_fg_info_t;

shr_error_e appl_dnx_field_itmh_init(
    int unit);

shr_error_e appl_dnx_field_itmh_pph_init(
    int unit);

#endif
