

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INGR_REASSEMBLY_H_

#define _DNX_DATA_INGR_REASSEMBLY_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ingr_reassembly.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_ingr_reassembly_init(
    int unit);






typedef struct
{
    
    uint32 start_index;
} dnx_data_ingr_reassembly_context_context_map_t;



typedef enum
{
    
    dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities,

    
    _dnx_data_ingr_reassembly_context_feature_nof
} dnx_data_ingr_reassembly_context_feature_e;



typedef int(
    *dnx_data_ingr_reassembly_context_feature_get_f) (
    int unit,
    dnx_data_ingr_reassembly_context_feature_e feature);


typedef uint32(
    *dnx_data_ingr_reassembly_context_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_context_invalid_context_get_f) (
    int unit);


typedef const dnx_data_ingr_reassembly_context_context_map_t *(
    *dnx_data_ingr_reassembly_context_context_map_get_f) (
    int unit,
    int port_type);



typedef struct
{
    
    dnx_data_ingr_reassembly_context_feature_get_f feature_get;
    
    dnx_data_ingr_reassembly_context_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_ingr_reassembly_context_invalid_context_get_f invalid_context_get;
    
    dnx_data_ingr_reassembly_context_context_map_get_f context_map_get;
    
    dnxc_data_table_info_get_f context_map_info_get;
} dnx_data_if_ingr_reassembly_context_t;







typedef enum
{
    
    dnx_data_ingr_reassembly_ilkn_is_supported,

    
    _dnx_data_ingr_reassembly_ilkn_feature_nof
} dnx_data_ingr_reassembly_ilkn_feature_e;



typedef int(
    *dnx_data_ingr_reassembly_ilkn_feature_get_f) (
    int unit,
    dnx_data_ingr_reassembly_ilkn_feature_e feature);



typedef struct
{
    
    dnx_data_ingr_reassembly_ilkn_feature_get_f feature_get;
} dnx_data_if_ingr_reassembly_ilkn_t;







typedef enum
{

    
    _dnx_data_ingr_reassembly_rcy_feature_nof
} dnx_data_ingr_reassembly_rcy_feature_e;



typedef int(
    *dnx_data_ingr_reassembly_rcy_feature_get_f) (
    int unit,
    dnx_data_ingr_reassembly_rcy_feature_e feature);


typedef uint32(
    *dnx_data_ingr_reassembly_rcy_nof_interfaces_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_ingr_reassembly_rcy_feature_get_f feature_get;
    
    dnx_data_ingr_reassembly_rcy_nof_interfaces_get_f nof_interfaces_get;
} dnx_data_if_ingr_reassembly_rcy_t;







typedef enum
{

    
    _dnx_data_ingr_reassembly_priority_feature_nof
} dnx_data_ingr_reassembly_priority_feature_e;



typedef int(
    *dnx_data_ingr_reassembly_priority_feature_get_f) (
    int unit,
    dnx_data_ingr_reassembly_priority_feature_e feature);


typedef uint32(
    *dnx_data_ingr_reassembly_priority_nif_eth_priorities_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_priority_mirror_priorities_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_priority_rcy_priorities_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_priority_priorities_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_priority_cgm_priorities_nof_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_ingr_reassembly_priority_feature_get_f feature_get;
    
    dnx_data_ingr_reassembly_priority_nif_eth_priorities_nof_get_f nif_eth_priorities_nof_get;
    
    dnx_data_ingr_reassembly_priority_mirror_priorities_nof_get_f mirror_priorities_nof_get;
    
    dnx_data_ingr_reassembly_priority_rcy_priorities_nof_get_f rcy_priorities_nof_get;
    
    dnx_data_ingr_reassembly_priority_priorities_nof_get_f priorities_nof_get;
    
    dnx_data_ingr_reassembly_priority_cgm_priorities_nof_get_f cgm_priorities_nof_get;
} dnx_data_if_ingr_reassembly_priority_t;







typedef enum
{

    
    _dnx_data_ingr_reassembly_dbal_feature_nof
} dnx_data_ingr_reassembly_dbal_feature_e;



typedef int(
    *dnx_data_ingr_reassembly_dbal_feature_get_f) (
    int unit,
    dnx_data_ingr_reassembly_dbal_feature_e feature);


typedef uint32(
    *dnx_data_ingr_reassembly_dbal_reassembly_context_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_dbal_interface_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_dbal_interface_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_dbal_context_map_base_address_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_dbal_context_map_base_address_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_dbal_context_map_base_offest_ext_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_dbal_interleaved_interface_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_reassembly_dbal_interleaved_interface_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_ingr_reassembly_dbal_feature_get_f feature_get;
    
    dnx_data_ingr_reassembly_dbal_reassembly_context_bits_get_f reassembly_context_bits_get;
    
    dnx_data_ingr_reassembly_dbal_interface_bits_get_f interface_bits_get;
    
    dnx_data_ingr_reassembly_dbal_interface_max_get_f interface_max_get;
    
    dnx_data_ingr_reassembly_dbal_context_map_base_address_bits_get_f context_map_base_address_bits_get;
    
    dnx_data_ingr_reassembly_dbal_context_map_base_address_max_get_f context_map_base_address_max_get;
    
    dnx_data_ingr_reassembly_dbal_context_map_base_offest_ext_max_get_f context_map_base_offest_ext_max_get;
    
    dnx_data_ingr_reassembly_dbal_interleaved_interface_max_get_f interleaved_interface_max_get;
    
    dnx_data_ingr_reassembly_dbal_interleaved_interface_bits_get_f interleaved_interface_bits_get;
} dnx_data_if_ingr_reassembly_dbal_t;





typedef struct
{
    
    dnx_data_if_ingr_reassembly_context_t context;
    
    dnx_data_if_ingr_reassembly_ilkn_t ilkn;
    
    dnx_data_if_ingr_reassembly_rcy_t rcy;
    
    dnx_data_if_ingr_reassembly_priority_t priority;
    
    dnx_data_if_ingr_reassembly_dbal_t dbal;
} dnx_data_if_ingr_reassembly_t;




extern dnx_data_if_ingr_reassembly_t dnx_data_ingr_reassembly;


#endif 

