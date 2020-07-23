

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_SBUSDMA_DESC_H_

#define _DNX_DATA_SBUSDMA_DESC_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_sbusdma_desc.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_sbusdma_desc_init(
    int unit);






typedef struct
{
    
    uint32 enable;
} dnx_data_sbusdma_desc_global_enable_module_desc_dma_t;



typedef enum
{
    
    dnx_data_sbusdma_desc_global_desc_dma,
    
    dnx_data_sbusdma_desc_global_force_enable,

    
    _dnx_data_sbusdma_desc_global_feature_nof
} dnx_data_sbusdma_desc_global_feature_e;



typedef int(
    *dnx_data_sbusdma_desc_global_feature_get_f) (
    int unit,
    dnx_data_sbusdma_desc_global_feature_e feature);


typedef uint32(
    *dnx_data_sbusdma_desc_global_dma_desc_aggregator_chain_length_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sbusdma_desc_global_dma_desc_aggregator_buff_size_kb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sbusdma_desc_global_dma_desc_aggregator_timeout_usec_get_f) (
    int unit);


typedef const dnx_data_sbusdma_desc_global_enable_module_desc_dma_t *(
    *dnx_data_sbusdma_desc_global_enable_module_desc_dma_get_f) (
    int unit,
    int module_enum_val);



typedef struct
{
    
    dnx_data_sbusdma_desc_global_feature_get_f feature_get;
    
    dnx_data_sbusdma_desc_global_dma_desc_aggregator_chain_length_max_get_f dma_desc_aggregator_chain_length_max_get;
    
    dnx_data_sbusdma_desc_global_dma_desc_aggregator_buff_size_kb_get_f dma_desc_aggregator_buff_size_kb_get;
    
    dnx_data_sbusdma_desc_global_dma_desc_aggregator_timeout_usec_get_f dma_desc_aggregator_timeout_usec_get;
    
    dnx_data_sbusdma_desc_global_enable_module_desc_dma_get_f enable_module_desc_dma_get;
    
    dnxc_data_table_info_get_f enable_module_desc_dma_info_get;
} dnx_data_if_sbusdma_desc_global_t;





typedef struct
{
    
    dnx_data_if_sbusdma_desc_global_t global;
} dnx_data_if_sbusdma_desc_t;




extern dnx_data_if_sbusdma_desc_t dnx_data_sbusdma_desc;


#endif 

