

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_ACCESS_H_

#define _DNX_DATA_ACCESS_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <soc/chip.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_access.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_access_init(
    int unit);







typedef enum
{

    
    _dnx_data_access_mdio_feature_nof
} dnx_data_access_mdio_feature_e;



typedef int(
    *dnx_data_access_mdio_feature_get_f) (
    int unit,
    dnx_data_access_mdio_feature_e feature);


typedef uint32(
    *dnx_data_access_mdio_ext_divisor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_access_mdio_int_divisor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_access_mdio_int_div_out_delay_get_f) (
    int unit);


typedef uint32(
    *dnx_data_access_mdio_ext_div_out_delay_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_access_mdio_feature_get_f feature_get;
    
    dnx_data_access_mdio_ext_divisor_get_f ext_divisor_get;
    
    dnx_data_access_mdio_int_divisor_get_f int_divisor_get;
    
    dnx_data_access_mdio_int_div_out_delay_get_f int_div_out_delay_get;
    
    dnx_data_access_mdio_ext_div_out_delay_get_f ext_div_out_delay_get;
} dnx_data_if_access_mdio_t;






typedef struct
{
    
    bcm_pbmp_t pbmp;
} dnx_data_access_rcpu_rx_t;



typedef enum
{

    
    _dnx_data_access_rcpu_feature_nof
} dnx_data_access_rcpu_feature_e;



typedef int(
    *dnx_data_access_rcpu_feature_get_f) (
    int unit,
    dnx_data_access_rcpu_feature_e feature);


typedef const dnx_data_access_rcpu_rx_t *(
    *dnx_data_access_rcpu_rx_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_access_rcpu_feature_get_f feature_get;
    
    dnx_data_access_rcpu_rx_get_f rx_get;
    
    dnxc_data_table_info_get_f rx_info_get;
} dnx_data_if_access_rcpu_t;






typedef struct
{
    
    char *block_type;
    
    int block_instance;
    
    int value;
} dnx_data_access_blocks_override_t;


typedef struct
{
    
    soc_pbmp_t trigger_value;
} dnx_data_access_blocks_soft_init_t;



typedef enum
{

    
    _dnx_data_access_blocks_feature_nof
} dnx_data_access_blocks_feature_e;



typedef int(
    *dnx_data_access_blocks_feature_get_f) (
    int unit,
    dnx_data_access_blocks_feature_e feature);


typedef const dnx_data_access_blocks_override_t *(
    *dnx_data_access_blocks_override_get_f) (
    int unit,
    int index);


typedef const dnx_data_access_blocks_soft_init_t *(
    *dnx_data_access_blocks_soft_init_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_access_blocks_feature_get_f feature_get;
    
    dnx_data_access_blocks_override_get_f override_get;
    
    dnxc_data_table_info_get_f override_info_get;
    
    dnx_data_access_blocks_soft_init_get_f soft_init_get;
    
    dnxc_data_table_info_get_f soft_init_info_get;
} dnx_data_if_access_blocks_t;





typedef struct
{
    
    dnx_data_if_access_mdio_t mdio;
    
    dnx_data_if_access_rcpu_t rcpu;
    
    dnx_data_if_access_blocks_t blocks;
} dnx_data_if_access_t;




extern dnx_data_if_access_t dnx_data_access;


#endif 

