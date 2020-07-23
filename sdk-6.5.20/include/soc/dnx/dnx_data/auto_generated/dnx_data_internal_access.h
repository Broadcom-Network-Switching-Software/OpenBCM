

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_ACCESS_H_

#define _DNX_DATA_INTERNAL_ACCESS_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_access.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_access_submodule_mdio,
    dnx_data_access_submodule_rcpu,
    dnx_data_access_submodule_blocks,

    
    _dnx_data_access_submodule_nof
} dnx_data_access_submodule_e;








int dnx_data_access_mdio_feature_get(
    int unit,
    dnx_data_access_mdio_feature_e feature);



typedef enum
{
    dnx_data_access_mdio_define_ext_divisor,
    dnx_data_access_mdio_define_int_divisor,
    dnx_data_access_mdio_define_int_div_out_delay,
    dnx_data_access_mdio_define_ext_div_out_delay,

    
    _dnx_data_access_mdio_define_nof
} dnx_data_access_mdio_define_e;



uint32 dnx_data_access_mdio_ext_divisor_get(
    int unit);


uint32 dnx_data_access_mdio_int_divisor_get(
    int unit);


uint32 dnx_data_access_mdio_int_div_out_delay_get(
    int unit);


uint32 dnx_data_access_mdio_ext_div_out_delay_get(
    int unit);



typedef enum
{

    
    _dnx_data_access_mdio_table_nof
} dnx_data_access_mdio_table_e;









int dnx_data_access_rcpu_feature_get(
    int unit,
    dnx_data_access_rcpu_feature_e feature);



typedef enum
{

    
    _dnx_data_access_rcpu_define_nof
} dnx_data_access_rcpu_define_e;




typedef enum
{
    dnx_data_access_rcpu_table_rx,

    
    _dnx_data_access_rcpu_table_nof
} dnx_data_access_rcpu_table_e;



const dnx_data_access_rcpu_rx_t * dnx_data_access_rcpu_rx_get(
    int unit);



shr_error_e dnx_data_access_rcpu_rx_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_access_rcpu_rx_info_get(
    int unit);






int dnx_data_access_blocks_feature_get(
    int unit,
    dnx_data_access_blocks_feature_e feature);



typedef enum
{

    
    _dnx_data_access_blocks_define_nof
} dnx_data_access_blocks_define_e;




typedef enum
{
    dnx_data_access_blocks_table_override,
    dnx_data_access_blocks_table_soft_init,

    
    _dnx_data_access_blocks_table_nof
} dnx_data_access_blocks_table_e;



const dnx_data_access_blocks_override_t * dnx_data_access_blocks_override_get(
    int unit,
    int index);


const dnx_data_access_blocks_soft_init_t * dnx_data_access_blocks_soft_init_get(
    int unit);



shr_error_e dnx_data_access_blocks_override_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_access_blocks_soft_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_access_blocks_override_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_access_blocks_soft_init_info_get(
    int unit);



shr_error_e dnx_data_access_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

