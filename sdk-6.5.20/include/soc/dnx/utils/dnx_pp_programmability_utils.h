/*
 * $Id: dnx_pp_programmability_utils.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_PP_PROGRAMMABILITY_UTILS_H_INCLUDED

#define DNX_PP_PROGRAMMABILITY_UTILS_H_INCLUDED

shr_error_e dnx_pp_prgm_device_image_name_get(
    int unit,
    char **device_image_name);

shr_error_e dnx_pp_prgm_device_image_name_legacy_get(
    int unit,
    char **device_image_name);

shr_error_e dnx_pp_prgm_default_image_check(
    int unit,
    uint8 *is_default);

shr_error_e dnx_pp_prgm_current_image_check(
    int unit,
    char *image_to_check,
    uint8 *is_image);
#endif
