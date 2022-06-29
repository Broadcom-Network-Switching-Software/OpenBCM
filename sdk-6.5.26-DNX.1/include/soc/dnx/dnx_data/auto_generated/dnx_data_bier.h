
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_BIER_H_

#define _DNX_DATA_BIER_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_bier.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_bier_init(
    int unit);







typedef enum
{
    dnx_data_bier_params_hw_supported,
    dnx_data_bier_params_is_supported,
    dnx_data_bier_params_egress_only,
    dnx_data_bier_params_ingress_only,
    dnx_data_bier_params_ingress_bfr_pos_msb_start,
    dnx_data_bier_params_bfr_bit_str_in_mcdb,

    
    _dnx_data_bier_params_feature_nof
} dnx_data_bier_params_feature_e;



typedef int(
    *dnx_data_bier_params_feature_get_f) (
    int unit,
    dnx_data_bier_params_feature_e feature);


typedef uint32(
    *dnx_data_bier_params_bitstring_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bier_params_bfr_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bier_params_bank_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bier_params_bundle_nof_banks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bier_params_bundle_nof_banks_net_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bier_params_nof_egr_table_copies_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bier_params_mcdb_copies_per_entry_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bier_params_nof_bfr_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_bier_params_bier_mcdb_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_bier_params_feature_get_f feature_get;
    
    dnx_data_bier_params_bitstring_size_get_f bitstring_size_get;
    
    dnx_data_bier_params_bfr_entry_size_get_f bfr_entry_size_get;
    
    dnx_data_bier_params_bank_entries_get_f bank_entries_get;
    
    dnx_data_bier_params_bundle_nof_banks_get_f bundle_nof_banks_get;
    
    dnx_data_bier_params_bundle_nof_banks_net_get_f bundle_nof_banks_net_get;
    
    dnx_data_bier_params_nof_egr_table_copies_get_f nof_egr_table_copies_get;
    
    dnx_data_bier_params_mcdb_copies_per_entry_get_f mcdb_copies_per_entry_get;
    
    dnx_data_bier_params_nof_bfr_entries_get_f nof_bfr_entries_get;
    
    dnx_data_bier_params_bier_mcdb_size_get_f bier_mcdb_size_get;
} dnx_data_if_bier_params_t;





typedef struct
{
    
    dnx_data_if_bier_params_t params;
} dnx_data_if_bier_t;




extern dnx_data_if_bier_t dnx_data_bier;


#endif 

