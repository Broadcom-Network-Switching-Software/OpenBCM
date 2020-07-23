

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_SPB_H_

#define _DNX_DATA_SPB_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_spb.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_spb_init(
    int unit);






typedef struct
{
    
    int is_last_in_chain;
} dnx_data_spb_ocb_last_in_chain_t;



typedef enum
{
    
    dnx_data_spb_ocb_blocks_in_ring,

    
    _dnx_data_spb_ocb_feature_nof
} dnx_data_spb_ocb_feature_e;



typedef int(
    *dnx_data_spb_ocb_feature_get_f) (
    int unit,
    dnx_data_spb_ocb_feature_e feature);


typedef uint32(
    *dnx_data_spb_ocb_nof_blocks_get_f) (
    int unit);


typedef const dnx_data_spb_ocb_last_in_chain_t *(
    *dnx_data_spb_ocb_last_in_chain_get_f) (
    int unit,
    int ocb_index);



typedef struct
{
    
    dnx_data_spb_ocb_feature_get_f feature_get;
    
    dnx_data_spb_ocb_nof_blocks_get_f nof_blocks_get;
    
    dnx_data_spb_ocb_last_in_chain_get_f last_in_chain_get;
    
    dnxc_data_table_info_get_f last_in_chain_info_get;
} dnx_data_if_spb_ocb_t;





typedef struct
{
    
    dnx_data_if_spb_ocb_t ocb;
} dnx_data_if_spb_t;




extern dnx_data_if_spb_t dnx_data_spb;


#endif 

