

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_ARR_NAT_H_

#define _DNX_DATA_INTERNAL_ARR_NAT_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_arr_nat.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_arr_nat_submodule_prefix,

    
    _dnx_data_arr_nat_submodule_nof
} dnx_data_arr_nat_submodule_e;








int dnx_data_arr_nat_prefix_feature_get(
    int unit,
    dnx_data_arr_nat_prefix_feature_e feature);



typedef enum
{
    dnx_data_arr_nat_prefix_define_IRPP_VSI_FORMATS___prefix_0,
    dnx_data_arr_nat_prefix_define_IRPP_VSI_FORMATS___prefix_1,
    dnx_data_arr_nat_prefix_define_IRPP_VSI_FORMATS___prefix_2,
    dnx_data_arr_nat_prefix_define_IRPP_VSI_FORMATS___prefix_4,
    dnx_data_arr_nat_prefix_define_IRPP_VSI_FORMATS___prefix_5,
    dnx_data_arr_nat_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_0,
    dnx_data_arr_nat_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_1,
    dnx_data_arr_nat_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_2,
    dnx_data_arr_nat_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_3,
    dnx_data_arr_nat_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_4,
    dnx_data_arr_nat_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_5,
    dnx_data_arr_nat_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_6,
    dnx_data_arr_nat_prefix_define_IRPP_IN_LIF_FORMATS___prefix_0,
    dnx_data_arr_nat_prefix_define_IRPP_IN_LIF_FORMATS___prefix_13,
    dnx_data_arr_nat_prefix_define_IRPP_IN_LIF_FORMATS___prefix_7,
    dnx_data_arr_nat_prefix_define_ETPP_ETPS_FORMATS___prefix_0,
    dnx_data_arr_nat_prefix_define_ETPP_ETPS_FORMATS___prefix_13,
    dnx_data_arr_nat_prefix_define_ETPP_ETPS_FORMATS___prefix_14,

    
    _dnx_data_arr_nat_prefix_define_nof
} dnx_data_arr_nat_prefix_define_e;



uint32 dnx_data_arr_nat_prefix_IRPP_VSI_FORMATS___prefix_0_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_VSI_FORMATS___prefix_1_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_VSI_FORMATS___prefix_2_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_VSI_FORMATS___prefix_4_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_VSI_FORMATS___prefix_5_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_FWD_STAGE_FORMATS___prefix_0_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_FWD_STAGE_FORMATS___prefix_1_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_FWD_STAGE_FORMATS___prefix_2_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_FWD_STAGE_FORMATS___prefix_3_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_FWD_STAGE_FORMATS___prefix_4_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_FWD_STAGE_FORMATS___prefix_5_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_FWD_STAGE_FORMATS___prefix_6_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_IN_LIF_FORMATS___prefix_0_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_IN_LIF_FORMATS___prefix_13_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_IRPP_IN_LIF_FORMATS___prefix_7_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_ETPP_ETPS_FORMATS___prefix_0_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_ETPP_ETPS_FORMATS___prefix_13_get(
    int unit);


uint32 dnx_data_arr_nat_prefix_ETPP_ETPS_FORMATS___prefix_14_get(
    int unit);



typedef enum
{

    
    _dnx_data_arr_nat_prefix_table_nof
} dnx_data_arr_nat_prefix_table_e;






shr_error_e dnx_data_arr_nat_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

