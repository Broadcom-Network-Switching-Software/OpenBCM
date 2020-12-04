

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_FAILOVER_H_

#define _DNX_DATA_FAILOVER_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_failover.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_failover_init(
    int unit);







typedef enum
{

    
    _dnx_data_failover_path_select_feature_nof
} dnx_data_failover_path_select_feature_e;



typedef int(
    *dnx_data_failover_path_select_feature_get_f) (
    int unit,
    dnx_data_failover_path_select_feature_e feature);


typedef uint32(
    *dnx_data_failover_path_select_egress_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_failover_path_select_ingress_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_failover_path_select_fec_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_failover_path_select_egr_no_protection_get_f) (
    int unit);


typedef uint32(
    *dnx_data_failover_path_select_ing_no_protection_get_f) (
    int unit);


typedef uint32(
    *dnx_data_failover_path_select_fec_no_protection_get_f) (
    int unit);


typedef uint32(
    *dnx_data_failover_path_select_fec_facility_protection_get_f) (
    int unit);


typedef uint32(
    *dnx_data_failover_path_select_nof_fec_path_select_banks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_failover_path_select_fec_bank_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_failover_path_select_in_lif_protection_pointer_nof_msb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_failover_path_select_in_lif_protection_pointer_nof_lsb_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_failover_path_select_feature_get_f feature_get;
    
    dnx_data_failover_path_select_egress_size_get_f egress_size_get;
    
    dnx_data_failover_path_select_ingress_size_get_f ingress_size_get;
    
    dnx_data_failover_path_select_fec_size_get_f fec_size_get;
    
    dnx_data_failover_path_select_egr_no_protection_get_f egr_no_protection_get;
    
    dnx_data_failover_path_select_ing_no_protection_get_f ing_no_protection_get;
    
    dnx_data_failover_path_select_fec_no_protection_get_f fec_no_protection_get;
    
    dnx_data_failover_path_select_fec_facility_protection_get_f fec_facility_protection_get;
    
    dnx_data_failover_path_select_nof_fec_path_select_banks_get_f nof_fec_path_select_banks_get;
    
    dnx_data_failover_path_select_fec_bank_size_get_f fec_bank_size_get;
    
    dnx_data_failover_path_select_in_lif_protection_pointer_nof_msb_get_f in_lif_protection_pointer_nof_msb_get;
    
    dnx_data_failover_path_select_in_lif_protection_pointer_nof_lsb_get_f in_lif_protection_pointer_nof_lsb_get;
} dnx_data_if_failover_path_select_t;







typedef enum
{
    
    dnx_data_failover_facility_accelerated_mode_enable,

    
    _dnx_data_failover_facility_feature_nof
} dnx_data_failover_facility_feature_e;



typedef int(
    *dnx_data_failover_facility_feature_get_f) (
    int unit,
    dnx_data_failover_facility_feature_e feature);



typedef struct
{
    
    dnx_data_failover_facility_feature_get_f feature_get;
} dnx_data_if_failover_facility_t;





typedef struct
{
    
    dnx_data_if_failover_path_select_t path_select;
    
    dnx_data_if_failover_facility_t facility;
} dnx_data_if_failover_t;




extern dnx_data_if_failover_t dnx_data_failover;


#endif 

