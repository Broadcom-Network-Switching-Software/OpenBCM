/*
 * $Id: dnx_pp_programmability_utils.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef DNX_PP_PROGRAMMABILITY_UTILS_H_INCLUDED

#define DNX_PP_PROGRAMMABILITY_UTILS_H_INCLUDED

#define DNX_PP_NON_STD_1_DEFAULT_NOF_ELEMENTS      2048

shr_error_e dnx_pp_prgm_device_image_name_get(
    int unit,
    char **device_image_name);

shr_error_e dnx_pp_prgm_device_image_name_legacy_get(
    int unit,
    char **device_image_name);

shr_error_e dnx_pp_prgm_default_image_check(
    int unit,
    uint8 *is_default);

shr_error_e dnx_pp_prgm_is_standard_image(
    int unit,
    char *image_to_check,
    uint8 *is_standard);

shr_error_e dnx_pp_prgm_current_image_check(
    int unit,
    char *image_to_check,
    uint8 *is_image);

shr_error_e dnx_pp_prgm_is_vendor_standard_image(
    int unit,
    uint8 *is_vendor);

shr_error_e dnx_pp_prgm_is_custom_standard_image(
    int unit,
    uint8 *is_custom);

#endif
