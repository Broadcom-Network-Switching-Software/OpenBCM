

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_STACK_H_

#define _DNX_DATA_INTERNAL_STACK_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stack.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_stack_submodule_general,

    
    _dnx_data_stack_submodule_nof
} dnx_data_stack_submodule_e;








int dnx_data_stack_general_feature_get(
    int unit,
    dnx_data_stack_general_feature_e feature);



typedef enum
{
    dnx_data_stack_general_define_nof_tm_domains_max,
    dnx_data_stack_general_define_dest_port_base_queue_min,
    dnx_data_stack_general_define_dest_port_base_queue_max,
    dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_min,
    dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_max,
    dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_all,
    dnx_data_stack_general_define_lag_stack_fec_resolve_entry_min,
    dnx_data_stack_general_define_lag_stack_fec_resolve_entry_max,
    dnx_data_stack_general_define_lag_stack_fec_resolve_entry_all,
    dnx_data_stack_general_define_lag_domain_min,
    dnx_data_stack_general_define_lag_domain_max,
    dnx_data_stack_general_define_trunk_entry_per_tmd_bit_num,
    dnx_data_stack_general_define_stacking_enable,

    
    _dnx_data_stack_general_define_nof
} dnx_data_stack_general_define_e;



uint32 dnx_data_stack_general_nof_tm_domains_max_get(
    int unit);


uint32 dnx_data_stack_general_dest_port_base_queue_min_get(
    int unit);


uint32 dnx_data_stack_general_dest_port_base_queue_max_get(
    int unit);


uint32 dnx_data_stack_general_lag_stack_trunk_resolve_entry_min_get(
    int unit);


uint32 dnx_data_stack_general_lag_stack_trunk_resolve_entry_max_get(
    int unit);


uint32 dnx_data_stack_general_lag_stack_trunk_resolve_entry_all_get(
    int unit);


uint32 dnx_data_stack_general_lag_stack_fec_resolve_entry_min_get(
    int unit);


uint32 dnx_data_stack_general_lag_stack_fec_resolve_entry_max_get(
    int unit);


uint32 dnx_data_stack_general_lag_stack_fec_resolve_entry_all_get(
    int unit);


uint32 dnx_data_stack_general_lag_domain_min_get(
    int unit);


uint32 dnx_data_stack_general_lag_domain_max_get(
    int unit);


uint32 dnx_data_stack_general_trunk_entry_per_tmd_bit_num_get(
    int unit);


uint32 dnx_data_stack_general_stacking_enable_get(
    int unit);



typedef enum
{

    
    _dnx_data_stack_general_table_nof
} dnx_data_stack_general_table_e;






shr_error_e dnx_data_stack_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

