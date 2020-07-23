

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_FAILOVER_H_

#define _DNX_DATA_INTERNAL_FAILOVER_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_failover_submodule_path_select,
    dnx_data_failover_submodule_facility,

    
    _dnx_data_failover_submodule_nof
} dnx_data_failover_submodule_e;








int dnx_data_failover_path_select_feature_get(
    int unit,
    dnx_data_failover_path_select_feature_e feature);



typedef enum
{
    dnx_data_failover_path_select_define_egress_size,
    dnx_data_failover_path_select_define_ingress_size,
    dnx_data_failover_path_select_define_fec_size,
    dnx_data_failover_path_select_define_egr_no_protection,
    dnx_data_failover_path_select_define_ing_no_protection,
    dnx_data_failover_path_select_define_fec_no_protection,
    dnx_data_failover_path_select_define_fec_facility_protection,
    dnx_data_failover_path_select_define_nof_fec_path_select_banks,
    dnx_data_failover_path_select_define_fec_bank_size,
    dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_msb,
    dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_lsb,

    
    _dnx_data_failover_path_select_define_nof
} dnx_data_failover_path_select_define_e;



uint32 dnx_data_failover_path_select_egress_size_get(
    int unit);


uint32 dnx_data_failover_path_select_ingress_size_get(
    int unit);


uint32 dnx_data_failover_path_select_fec_size_get(
    int unit);


uint32 dnx_data_failover_path_select_egr_no_protection_get(
    int unit);


uint32 dnx_data_failover_path_select_ing_no_protection_get(
    int unit);


uint32 dnx_data_failover_path_select_fec_no_protection_get(
    int unit);


uint32 dnx_data_failover_path_select_fec_facility_protection_get(
    int unit);


uint32 dnx_data_failover_path_select_nof_fec_path_select_banks_get(
    int unit);


uint32 dnx_data_failover_path_select_fec_bank_size_get(
    int unit);


uint32 dnx_data_failover_path_select_in_lif_protection_pointer_nof_msb_get(
    int unit);


uint32 dnx_data_failover_path_select_in_lif_protection_pointer_nof_lsb_get(
    int unit);



typedef enum
{

    
    _dnx_data_failover_path_select_table_nof
} dnx_data_failover_path_select_table_e;









int dnx_data_failover_facility_feature_get(
    int unit,
    dnx_data_failover_facility_feature_e feature);



typedef enum
{

    
    _dnx_data_failover_facility_define_nof
} dnx_data_failover_facility_define_e;




typedef enum
{

    
    _dnx_data_failover_facility_table_nof
} dnx_data_failover_facility_table_e;






shr_error_e dnx_data_failover_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

