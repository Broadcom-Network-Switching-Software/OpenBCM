
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_LED_H_

#define _DNX_DATA_LED_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_led.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_led_init(
    int unit);







typedef enum
{
    dnx_data_led_general_flexe_in_scan_chain_is_adjusted,

    
    _dnx_data_led_general_feature_nof
} dnx_data_led_general_feature_e;



typedef int(
    *dnx_data_led_general_feature_get_f) (
    int unit,
    dnx_data_led_general_feature_e feature);


typedef uint32(
    *dnx_data_led_general_last_port_on_scan_chain_get_f) (
    int unit);


typedef uint32(
    *dnx_data_led_general_m0ss_freq_mhz_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_led_general_feature_get_f feature_get;
    
    dnx_data_led_general_last_port_on_scan_chain_get_f last_port_on_scan_chain_get;
    
    dnx_data_led_general_m0ss_freq_mhz_get_f m0ss_freq_mhz_get;
} dnx_data_if_led_general_t;





typedef struct
{
    
    dnx_data_if_led_general_t general;
} dnx_data_if_led_t;




extern dnx_data_if_led_t dnx_data_led;


#endif 

