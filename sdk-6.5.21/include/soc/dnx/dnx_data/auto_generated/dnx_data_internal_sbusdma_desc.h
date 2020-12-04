

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_SBUSDMA_DESC_H_

#define _DNX_DATA_INTERNAL_SBUSDMA_DESC_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sbusdma_desc.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_sbusdma_desc_submodule_global,

    
    _dnx_data_sbusdma_desc_submodule_nof
} dnx_data_sbusdma_desc_submodule_e;








int dnx_data_sbusdma_desc_global_feature_get(
    int unit,
    dnx_data_sbusdma_desc_global_feature_e feature);



typedef enum
{
    dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_chain_length_max,
    dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_buff_size_kb,
    dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_timeout_usec,

    
    _dnx_data_sbusdma_desc_global_define_nof
} dnx_data_sbusdma_desc_global_define_e;



uint32 dnx_data_sbusdma_desc_global_dma_desc_aggregator_chain_length_max_get(
    int unit);


uint32 dnx_data_sbusdma_desc_global_dma_desc_aggregator_buff_size_kb_get(
    int unit);


uint32 dnx_data_sbusdma_desc_global_dma_desc_aggregator_timeout_usec_get(
    int unit);



typedef enum
{
    dnx_data_sbusdma_desc_global_table_enable_module_desc_dma,

    
    _dnx_data_sbusdma_desc_global_table_nof
} dnx_data_sbusdma_desc_global_table_e;



const dnx_data_sbusdma_desc_global_enable_module_desc_dma_t * dnx_data_sbusdma_desc_global_enable_module_desc_dma_get(
    int unit,
    int module_enum_val);



shr_error_e dnx_data_sbusdma_desc_global_enable_module_desc_dma_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_sbusdma_desc_global_enable_module_desc_dma_info_get(
    int unit);



shr_error_e dnx_data_sbusdma_desc_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

