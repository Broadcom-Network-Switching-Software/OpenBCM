

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_DEBUG_H_

#define _DNX_DATA_DEBUG_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_debug.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_debug_init(
    int unit);






typedef struct
{
    
    int valid;
    
    int line_nof;
    
    int width;
} dnx_data_debug_mem_params_t;



typedef enum
{

    
    _dnx_data_debug_mem_feature_nof
} dnx_data_debug_mem_feature_e;



typedef int(
    *dnx_data_debug_mem_feature_get_f) (
    int unit,
    dnx_data_debug_mem_feature_e feature);


typedef const dnx_data_debug_mem_params_t *(
    *dnx_data_debug_mem_params_get_f) (
    int unit,
    int block);



typedef struct
{
    
    dnx_data_debug_mem_feature_get_f feature_get;
    
    dnx_data_debug_mem_params_get_f params_get;
    
    dnxc_data_table_info_get_f params_info_get;
} dnx_data_if_debug_mem_t;







typedef enum
{
    
    dnx_data_debug_feature_no_hit_bit_on_mdb_access,
    dnx_data_debug_feature_pem_vt_init_fix,
    
    dnx_data_debug_feature_iparser_signals_valid,
    
    dnx_data_debug_feature_fwd_action_signals_from_lbp_valid,

    
    _dnx_data_debug_feature_feature_nof
} dnx_data_debug_feature_feature_e;



typedef int(
    *dnx_data_debug_feature_feature_get_f) (
    int unit,
    dnx_data_debug_feature_feature_e feature);



typedef struct
{
    
    dnx_data_debug_feature_feature_get_f feature_get;
} dnx_data_if_debug_feature_t;







typedef enum
{

    
    _dnx_data_debug_kleap_feature_nof
} dnx_data_debug_kleap_feature_e;



typedef int(
    *dnx_data_debug_kleap_feature_get_f) (
    int unit,
    dnx_data_debug_kleap_feature_e feature);


typedef uint32(
    *dnx_data_debug_kleap_fwd1_gen_data_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_debug_kleap_fwd2_gen_data_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_debug_kleap_feature_get_f feature_get;
    
    dnx_data_debug_kleap_fwd1_gen_data_size_get_f fwd1_gen_data_size_get;
    
    dnx_data_debug_kleap_fwd2_gen_data_size_get_f fwd2_gen_data_size_get;
} dnx_data_if_debug_kleap_t;







typedef enum
{

    
    _dnx_data_debug_general_feature_nof
} dnx_data_debug_general_feature_e;



typedef int(
    *dnx_data_debug_general_feature_get_f) (
    int unit,
    dnx_data_debug_general_feature_e feature);


typedef uint32(
    *dnx_data_debug_general_global_visibility_get_f) (
    int unit);


typedef uint32(
    *dnx_data_debug_general_etpp_eop_and_sop_dec_above_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_debug_general_egress_port_mc_visibility_get_f) (
    int unit);


typedef uint32(
    *dnx_data_debug_general_mdb_debug_signals_get_f) (
    int unit);


typedef uint32(
    *dnx_data_debug_general_mem_array_index_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_debug_general_feature_get_f feature_get;
    
    dnx_data_debug_general_global_visibility_get_f global_visibility_get;
    
    dnx_data_debug_general_etpp_eop_and_sop_dec_above_threshold_get_f etpp_eop_and_sop_dec_above_threshold_get;
    
    dnx_data_debug_general_egress_port_mc_visibility_get_f egress_port_mc_visibility_get;
    
    dnx_data_debug_general_mdb_debug_signals_get_f mdb_debug_signals_get;
    
    dnx_data_debug_general_mem_array_index_get_f mem_array_index_get;
} dnx_data_if_debug_general_t;





typedef struct
{
    
    dnx_data_if_debug_mem_t mem;
    
    dnx_data_if_debug_feature_t feature;
    
    dnx_data_if_debug_kleap_t kleap;
    
    dnx_data_if_debug_general_t general;
} dnx_data_if_debug_t;




extern dnx_data_if_debug_t dnx_data_debug;


#endif 

