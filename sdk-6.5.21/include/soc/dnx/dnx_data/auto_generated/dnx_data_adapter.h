

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_ADAPTER_H_

#define _DNX_DATA_ADAPTER_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_adapter.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_adapter_init(
    int unit);







typedef enum
{

    
    _dnx_data_adapter_tx_feature_nof
} dnx_data_adapter_tx_feature_e;



typedef int(
    *dnx_data_adapter_tx_feature_get_f) (
    int unit,
    dnx_data_adapter_tx_feature_e feature);


typedef uint32(
    *dnx_data_adapter_tx_loopback_enable_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_adapter_tx_feature_get_f feature_get;
    
    dnx_data_adapter_tx_loopback_enable_get_f loopback_enable_get;
} dnx_data_if_adapter_tx_t;







typedef enum
{

    
    _dnx_data_adapter_rx_feature_nof
} dnx_data_adapter_rx_feature_e;



typedef int(
    *dnx_data_adapter_rx_feature_get_f) (
    int unit,
    dnx_data_adapter_rx_feature_e feature);


typedef uint32(
    *dnx_data_adapter_rx_packet_header_signal_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_rx_constant_header_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_adapter_rx_feature_get_f feature_get;
    
    dnx_data_adapter_rx_packet_header_signal_id_get_f packet_header_signal_id_get;
    
    dnx_data_adapter_rx_constant_header_size_get_f constant_header_size_get;
} dnx_data_if_adapter_rx_t;







typedef enum
{

    
    _dnx_data_adapter_general_feature_nof
} dnx_data_adapter_general_feature_e;



typedef int(
    *dnx_data_adapter_general_feature_get_f) (
    int unit,
    dnx_data_adapter_general_feature_e feature);


typedef uint32(
    *dnx_data_adapter_general_lib_ver_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_adapter_general_feature_get_f feature_get;
    
    dnx_data_adapter_general_lib_ver_get_f lib_ver_get;
} dnx_data_if_adapter_general_t;







typedef enum
{

    
    _dnx_data_adapter_reg_mem_access_feature_nof
} dnx_data_adapter_reg_mem_access_feature_e;



typedef int(
    *dnx_data_adapter_reg_mem_access_feature_get_f) (
    int unit,
    dnx_data_adapter_reg_mem_access_feature_e feature);


typedef uint32(
    *dnx_data_adapter_reg_mem_access_do_collect_enable_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_adapter_reg_mem_access_feature_get_f feature_get;
    
    dnx_data_adapter_reg_mem_access_do_collect_enable_get_f do_collect_enable_get;
} dnx_data_if_adapter_reg_mem_access_t;





typedef struct
{
    
    dnx_data_if_adapter_tx_t tx;
    
    dnx_data_if_adapter_rx_t rx;
    
    dnx_data_if_adapter_general_t general;
    
    dnx_data_if_adapter_reg_mem_access_t reg_mem_access;
} dnx_data_if_adapter_t;




extern dnx_data_if_adapter_t dnx_data_adapter;


#endif 

