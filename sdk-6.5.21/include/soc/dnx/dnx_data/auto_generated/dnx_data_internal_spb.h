

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_SPB_H_

#define _DNX_DATA_INTERNAL_SPB_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_spb.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_spb_submodule_ocb,

    
    _dnx_data_spb_submodule_nof
} dnx_data_spb_submodule_e;








int dnx_data_spb_ocb_feature_get(
    int unit,
    dnx_data_spb_ocb_feature_e feature);



typedef enum
{
    dnx_data_spb_ocb_define_nof_blocks,

    
    _dnx_data_spb_ocb_define_nof
} dnx_data_spb_ocb_define_e;



uint32 dnx_data_spb_ocb_nof_blocks_get(
    int unit);



typedef enum
{
    dnx_data_spb_ocb_table_last_in_chain,

    
    _dnx_data_spb_ocb_table_nof
} dnx_data_spb_ocb_table_e;



const dnx_data_spb_ocb_last_in_chain_t * dnx_data_spb_ocb_last_in_chain_get(
    int unit,
    int ocb_index);



shr_error_e dnx_data_spb_ocb_last_in_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_spb_ocb_last_in_chain_info_get(
    int unit);



shr_error_e dnx_data_spb_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

