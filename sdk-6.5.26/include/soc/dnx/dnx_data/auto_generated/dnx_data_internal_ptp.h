
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_PTP_H_

#define _DNX_DATA_INTERNAL_PTP_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ptp.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_ptp_submodule_general,
    dnx_data_ptp_submodule_ptp_firmware,

    
    _dnx_data_ptp_submodule_nof
} dnx_data_ptp_submodule_e;








int dnx_data_ptp_general_feature_get(
    int unit,
    dnx_data_ptp_general_feature_e feature);



typedef enum
{
    dnx_data_ptp_general_define_nof_port_profiles,

    
    _dnx_data_ptp_general_define_nof
} dnx_data_ptp_general_define_e;



uint32 dnx_data_ptp_general_nof_port_profiles_get(
    int unit);



typedef enum
{

    
    _dnx_data_ptp_general_table_nof
} dnx_data_ptp_general_table_e;









int dnx_data_ptp_ptp_firmware_feature_get(
    int unit,
    dnx_data_ptp_ptp_firmware_feature_e feature);



typedef enum
{
    dnx_data_ptp_ptp_firmware_define_ptp_cosq_port,

    
    _dnx_data_ptp_ptp_firmware_define_nof
} dnx_data_ptp_ptp_firmware_define_e;



uint32 dnx_data_ptp_ptp_firmware_ptp_cosq_port_get(
    int unit);



typedef enum
{

    
    _dnx_data_ptp_ptp_firmware_table_nof
} dnx_data_ptp_ptp_firmware_table_e;






shr_error_e dnx_data_ptp_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

