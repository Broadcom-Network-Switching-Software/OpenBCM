

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_STACK_H_

#define _DNX_DATA_STACK_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_stack.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_stack_init(
    int unit);







typedef enum
{

    
    _dnx_data_stack_general_feature_nof
} dnx_data_stack_general_feature_e;



typedef int(
    *dnx_data_stack_general_feature_get_f) (
    int unit,
    dnx_data_stack_general_feature_e feature);


typedef uint32(
    *dnx_data_stack_general_nof_tm_domains_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_dest_port_base_queue_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_dest_port_base_queue_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_lag_stack_trunk_resolve_entry_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_lag_stack_trunk_resolve_entry_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_lag_stack_trunk_resolve_entry_all_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_lag_stack_fec_resolve_entry_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_lag_stack_fec_resolve_entry_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_lag_stack_fec_resolve_entry_all_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_lag_domain_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_lag_domain_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_trunk_entry_per_tmd_bit_num_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stack_general_stacking_enable_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_stack_general_feature_get_f feature_get;
    
    dnx_data_stack_general_nof_tm_domains_max_get_f nof_tm_domains_max_get;
    
    dnx_data_stack_general_dest_port_base_queue_min_get_f dest_port_base_queue_min_get;
    
    dnx_data_stack_general_dest_port_base_queue_max_get_f dest_port_base_queue_max_get;
    
    dnx_data_stack_general_lag_stack_trunk_resolve_entry_min_get_f lag_stack_trunk_resolve_entry_min_get;
    
    dnx_data_stack_general_lag_stack_trunk_resolve_entry_max_get_f lag_stack_trunk_resolve_entry_max_get;
    
    dnx_data_stack_general_lag_stack_trunk_resolve_entry_all_get_f lag_stack_trunk_resolve_entry_all_get;
    
    dnx_data_stack_general_lag_stack_fec_resolve_entry_min_get_f lag_stack_fec_resolve_entry_min_get;
    
    dnx_data_stack_general_lag_stack_fec_resolve_entry_max_get_f lag_stack_fec_resolve_entry_max_get;
    
    dnx_data_stack_general_lag_stack_fec_resolve_entry_all_get_f lag_stack_fec_resolve_entry_all_get;
    
    dnx_data_stack_general_lag_domain_min_get_f lag_domain_min_get;
    
    dnx_data_stack_general_lag_domain_max_get_f lag_domain_max_get;
    
    dnx_data_stack_general_trunk_entry_per_tmd_bit_num_get_f trunk_entry_per_tmd_bit_num_get;
    
    dnx_data_stack_general_stacking_enable_get_f stacking_enable_get;
} dnx_data_if_stack_general_t;





typedef struct
{
    
    dnx_data_if_stack_general_t general;
} dnx_data_if_stack_t;




extern dnx_data_if_stack_t dnx_data_stack;


#endif 

