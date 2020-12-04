

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_BIER_H_

#define _DNX_DATA_INTERNAL_BIER_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bier.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_bier_submodule_params,

    
    _dnx_data_bier_submodule_nof
} dnx_data_bier_submodule_e;








int dnx_data_bier_params_feature_get(
    int unit,
    dnx_data_bier_params_feature_e feature);



typedef enum
{
    dnx_data_bier_params_define_bitstring_size,
    dnx_data_bier_params_define_bfr_entry_size,
    dnx_data_bier_params_define_bank_entries,
    dnx_data_bier_params_define_bundle_nof_banks,
    dnx_data_bier_params_define_bundle_nof_banks_net,
    dnx_data_bier_params_define_egress_only,
    dnx_data_bier_params_define_nof_egr_table_copies,
    dnx_data_bier_params_define_ingress_bfr_pos_msb_start,
    dnx_data_bier_params_define_nof_bfr_entries,

    
    _dnx_data_bier_params_define_nof
} dnx_data_bier_params_define_e;



uint32 dnx_data_bier_params_bitstring_size_get(
    int unit);


uint32 dnx_data_bier_params_bfr_entry_size_get(
    int unit);


uint32 dnx_data_bier_params_bank_entries_get(
    int unit);


uint32 dnx_data_bier_params_bundle_nof_banks_get(
    int unit);


uint32 dnx_data_bier_params_bundle_nof_banks_net_get(
    int unit);


uint32 dnx_data_bier_params_egress_only_get(
    int unit);


uint32 dnx_data_bier_params_nof_egr_table_copies_get(
    int unit);


uint32 dnx_data_bier_params_ingress_bfr_pos_msb_start_get(
    int unit);


uint32 dnx_data_bier_params_nof_bfr_entries_get(
    int unit);



typedef enum
{

    
    _dnx_data_bier_params_table_nof
} dnx_data_bier_params_table_e;






shr_error_e dnx_data_bier_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

