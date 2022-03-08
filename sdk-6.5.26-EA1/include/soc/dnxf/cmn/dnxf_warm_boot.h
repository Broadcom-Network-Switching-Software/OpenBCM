/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * DNXF WARM BOOT H
 */

#ifndef _SOC_DNXF_WARM_BOOT_H
#define _SOC_DNXF_WARM_BOOT_H

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <soc/error.h>
#include <soc/sand/shrextend/shrextend_error.h>

shr_error_e soc_dnxf_ha_init(
    int unit);

shr_error_e soc_dnxf_ha_deinit(
    int unit);

shr_error_e soc_dnxf_sw_state_alloc(
    int unit);

shr_error_e soc_dnxf_warm_boot_engine_init_buffer_struct(
    int unit,
    int buffer_id);

shr_error_e soc_dnxf_warm_boot_buffer_id_create(
    int unit,
    int buffer_id);

shr_error_e soc_dnxf_sw_state_init(
    int unit);

shr_error_e soc_dnxf_sw_state_deinit(
    int unit);

#endif
