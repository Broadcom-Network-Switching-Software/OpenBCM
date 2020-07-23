

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_BFD_H_

#define _DNX_DATA_BFD_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_bfd.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_bfd_init(
    int unit);







typedef enum
{
    dnx_data_bfd_general_configurable_single_hop_tos,
    
    dnx_data_bfd_general_extended_sip_support,
    
    dnx_data_bfd_general_vxlan_support,

    
    _dnx_data_bfd_general_feature_nof
} dnx_data_bfd_general_feature_e;



typedef int(
    *dnx_data_bfd_general_feature_get_f) (
    int unit,
    dnx_data_bfd_general_feature_e feature);


typedef uint32(
    *dnx_data_bfd_general_nof_bits_bfd_endpoints_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_nof_endpoints_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_nof_dips_for_bfd_multihop_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_nof_bits_dips_for_bfd_multihop_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_nof_bfd_server_trap_codes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_nof_profiles_for_tx_rate_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_nof_bits_bfd_server_trap_codes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_rmep_index_db_ipv4_const_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_rmep_index_db_mpls_const_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_rmep_index_db_pwe_const_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_nof_req_int_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_nof_bits_your_discr_to_lif_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bfd_general_oamp_bfd_cw_valid_dis_supp_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_bfd_general_feature_get_f feature_get;
    
    dnx_data_bfd_general_nof_bits_bfd_endpoints_get_f nof_bits_bfd_endpoints_get;
    
    dnx_data_bfd_general_nof_endpoints_get_f nof_endpoints_get;
    
    dnx_data_bfd_general_nof_dips_for_bfd_multihop_get_f nof_dips_for_bfd_multihop_get;
    
    dnx_data_bfd_general_nof_bits_dips_for_bfd_multihop_get_f nof_bits_dips_for_bfd_multihop_get;
    
    dnx_data_bfd_general_nof_bfd_server_trap_codes_get_f nof_bfd_server_trap_codes_get;
    
    dnx_data_bfd_general_nof_profiles_for_tx_rate_get_f nof_profiles_for_tx_rate_get;
    
    dnx_data_bfd_general_nof_bits_bfd_server_trap_codes_get_f nof_bits_bfd_server_trap_codes_get;
    
    dnx_data_bfd_general_rmep_index_db_ipv4_const_get_f rmep_index_db_ipv4_const_get;
    
    dnx_data_bfd_general_rmep_index_db_mpls_const_get_f rmep_index_db_mpls_const_get;
    
    dnx_data_bfd_general_rmep_index_db_pwe_const_get_f rmep_index_db_pwe_const_get;
    
    dnx_data_bfd_general_nof_req_int_profiles_get_f nof_req_int_profiles_get;
    
    dnx_data_bfd_general_nof_bits_your_discr_to_lif_get_f nof_bits_your_discr_to_lif_get;
    
    dnx_data_bfd_general_oamp_bfd_cw_valid_dis_supp_get_f oamp_bfd_cw_valid_dis_supp_get;
} dnx_data_if_bfd_general_t;







typedef enum
{
    dnx_data_bfd_feature_detect_multiplier_limitation,
    dnx_data_bfd_feature_ptch_oam_offset_limitation,

    
    _dnx_data_bfd_feature_feature_nof
} dnx_data_bfd_feature_feature_e;



typedef int(
    *dnx_data_bfd_feature_feature_get_f) (
    int unit,
    dnx_data_bfd_feature_feature_e feature);



typedef struct
{
    
    dnx_data_bfd_feature_feature_get_f feature_get;
} dnx_data_if_bfd_feature_t;





typedef struct
{
    
    dnx_data_if_bfd_general_t general;
    
    dnx_data_if_bfd_feature_t feature;
} dnx_data_if_bfd_t;




extern dnx_data_if_bfd_t dnx_data_bfd;


#endif 

