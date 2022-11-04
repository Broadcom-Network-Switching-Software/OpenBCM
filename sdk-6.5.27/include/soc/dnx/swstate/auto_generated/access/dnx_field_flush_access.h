
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_FLUSH_ACCESS_H__
#define __DNX_FIELD_FLUSH_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_field_flush_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>


typedef int (*dnx_field_flush_sw_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_field_flush_sw_init_cb)(
    int unit);

typedef int (*dnx_field_flush_sw_flush_profile_alloc_cb)(
    int unit);

typedef int (*dnx_field_flush_sw_flush_profile_mapped_fg_set_cb)(
    int unit, uint32 flush_profile_idx_0, dnx_field_group_t mapped_fg);

typedef int (*dnx_field_flush_sw_flush_profile_mapped_fg_get_cb)(
    int unit, uint32 flush_profile_idx_0, dnx_field_group_t *mapped_fg);



typedef struct {
    dnx_field_flush_sw_flush_profile_mapped_fg_set_cb set;
    dnx_field_flush_sw_flush_profile_mapped_fg_get_cb get;
} dnx_field_flush_sw_flush_profile_mapped_fg_cbs;

typedef struct {
    dnx_field_flush_sw_flush_profile_alloc_cb alloc;
    dnx_field_flush_sw_flush_profile_mapped_fg_cbs mapped_fg;
} dnx_field_flush_sw_flush_profile_cbs;

typedef struct {
    dnx_field_flush_sw_is_init_cb is_init;
    dnx_field_flush_sw_init_cb init;
    dnx_field_flush_sw_flush_profile_cbs flush_profile;
} dnx_field_flush_sw_cbs;





extern dnx_field_flush_sw_cbs dnx_field_flush_sw;

#endif 
