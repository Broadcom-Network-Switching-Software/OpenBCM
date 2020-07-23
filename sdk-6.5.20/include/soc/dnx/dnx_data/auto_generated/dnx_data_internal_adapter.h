

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_ADAPTER_H_

#define _DNX_DATA_INTERNAL_ADAPTER_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_adapter_submodule_tx,
    dnx_data_adapter_submodule_rx,
    dnx_data_adapter_submodule_general,
    dnx_data_adapter_submodule_reg_mem_access,

    
    _dnx_data_adapter_submodule_nof
} dnx_data_adapter_submodule_e;








int dnx_data_adapter_tx_feature_get(
    int unit,
    dnx_data_adapter_tx_feature_e feature);



typedef enum
{
    dnx_data_adapter_tx_define_loopback_enable,

    
    _dnx_data_adapter_tx_define_nof
} dnx_data_adapter_tx_define_e;



uint32 dnx_data_adapter_tx_loopback_enable_get(
    int unit);



typedef enum
{

    
    _dnx_data_adapter_tx_table_nof
} dnx_data_adapter_tx_table_e;









int dnx_data_adapter_rx_feature_get(
    int unit,
    dnx_data_adapter_rx_feature_e feature);



typedef enum
{
    dnx_data_adapter_rx_define_packet_header_signal_id,
    dnx_data_adapter_rx_define_constant_header_size,

    
    _dnx_data_adapter_rx_define_nof
} dnx_data_adapter_rx_define_e;



uint32 dnx_data_adapter_rx_packet_header_signal_id_get(
    int unit);


uint32 dnx_data_adapter_rx_constant_header_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_adapter_rx_table_nof
} dnx_data_adapter_rx_table_e;









int dnx_data_adapter_general_feature_get(
    int unit,
    dnx_data_adapter_general_feature_e feature);



typedef enum
{
    dnx_data_adapter_general_define_lib_ver,

    
    _dnx_data_adapter_general_define_nof
} dnx_data_adapter_general_define_e;



uint32 dnx_data_adapter_general_lib_ver_get(
    int unit);



typedef enum
{

    
    _dnx_data_adapter_general_table_nof
} dnx_data_adapter_general_table_e;









int dnx_data_adapter_reg_mem_access_feature_get(
    int unit,
    dnx_data_adapter_reg_mem_access_feature_e feature);



typedef enum
{
    dnx_data_adapter_reg_mem_access_define_do_collect_enable,

    
    _dnx_data_adapter_reg_mem_access_define_nof
} dnx_data_adapter_reg_mem_access_define_e;



uint32 dnx_data_adapter_reg_mem_access_do_collect_enable_get(
    int unit);



typedef enum
{

    
    _dnx_data_adapter_reg_mem_access_table_nof
} dnx_data_adapter_reg_mem_access_table_e;






shr_error_e dnx_data_adapter_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

