

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_DRAM_HBMC_HBMCMONITOR_H_INCLUDED

#define _BCM_DNX_DRAM_HBMC_HBMCMONITOR_H_INCLUDED


#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>


shr_error_e dnx_hbmc_temp_log(
    int unit,
    void *user_data);


shr_error_e dnx_hbmc_temp_monitor(
    int unit,
    void *user_data);


shr_error_e dnx_hbmc_traffic_to_hbm_restore(
    int unit);


shr_error_e dnx_hbmc_traffic_to_hbm_stop(
    int unit);


shr_error_e dnx_hbmc_traffic_is_empty_get(
    int unit,
    uint8 *hbm_is_empty);


shr_error_e dnx_hbmc_temp_stub_set(
    int unit,
    int nof_elements,
    uint32 *temp_arr);

#endif 
