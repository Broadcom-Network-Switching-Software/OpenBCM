

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_BFD_H_

#define _DNX_DATA_INTERNAL_BFD_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_bfd_submodule_general,
    dnx_data_bfd_submodule_feature,

    
    _dnx_data_bfd_submodule_nof
} dnx_data_bfd_submodule_e;








int dnx_data_bfd_general_feature_get(
    int unit,
    dnx_data_bfd_general_feature_e feature);



typedef enum
{
    dnx_data_bfd_general_define_nof_bits_bfd_endpoints,
    dnx_data_bfd_general_define_nof_endpoints,
    dnx_data_bfd_general_define_nof_dips_for_bfd_multihop,
    dnx_data_bfd_general_define_nof_bits_dips_for_bfd_multihop,
    dnx_data_bfd_general_define_nof_bfd_server_trap_codes,
    dnx_data_bfd_general_define_nof_profiles_for_tx_rate,
    dnx_data_bfd_general_define_nof_bits_bfd_server_trap_codes,
    dnx_data_bfd_general_define_rmep_index_db_ipv4_const,
    dnx_data_bfd_general_define_rmep_index_db_mpls_const,
    dnx_data_bfd_general_define_rmep_index_db_pwe_const,
    dnx_data_bfd_general_define_nof_req_int_profiles,
    dnx_data_bfd_general_define_nof_bits_your_discr_to_lif,
    dnx_data_bfd_general_define_oamp_bfd_cw_valid_dis_supp,

    
    _dnx_data_bfd_general_define_nof
} dnx_data_bfd_general_define_e;



uint32 dnx_data_bfd_general_nof_bits_bfd_endpoints_get(
    int unit);


uint32 dnx_data_bfd_general_nof_endpoints_get(
    int unit);


uint32 dnx_data_bfd_general_nof_dips_for_bfd_multihop_get(
    int unit);


uint32 dnx_data_bfd_general_nof_bits_dips_for_bfd_multihop_get(
    int unit);


uint32 dnx_data_bfd_general_nof_bfd_server_trap_codes_get(
    int unit);


uint32 dnx_data_bfd_general_nof_profiles_for_tx_rate_get(
    int unit);


uint32 dnx_data_bfd_general_nof_bits_bfd_server_trap_codes_get(
    int unit);


uint32 dnx_data_bfd_general_rmep_index_db_ipv4_const_get(
    int unit);


uint32 dnx_data_bfd_general_rmep_index_db_mpls_const_get(
    int unit);


uint32 dnx_data_bfd_general_rmep_index_db_pwe_const_get(
    int unit);


uint32 dnx_data_bfd_general_nof_req_int_profiles_get(
    int unit);


uint32 dnx_data_bfd_general_nof_bits_your_discr_to_lif_get(
    int unit);


uint32 dnx_data_bfd_general_oamp_bfd_cw_valid_dis_supp_get(
    int unit);



typedef enum
{

    
    _dnx_data_bfd_general_table_nof
} dnx_data_bfd_general_table_e;









int dnx_data_bfd_feature_feature_get(
    int unit,
    dnx_data_bfd_feature_feature_e feature);



typedef enum
{

    
    _dnx_data_bfd_feature_define_nof
} dnx_data_bfd_feature_define_e;




typedef enum
{

    
    _dnx_data_bfd_feature_table_nof
} dnx_data_bfd_feature_table_e;






shr_error_e dnx_data_bfd_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

