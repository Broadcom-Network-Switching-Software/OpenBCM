

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_SYNCE_H_

#define _DNX_DATA_SYNCE_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_synce.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_synce_init(
    int unit);






typedef struct
{
    
    bcm_port_t source_clock_port;
    
    uint32 squelch_enable;
    
    uint32 output_clock_sel;
} dnx_data_synce_general_cfg_t;


typedef struct
{
    
    uint32 chain_id;
} dnx_data_synce_general_nmg_chain_map_t;



typedef enum
{
    
    dnx_data_synce_general_synce_no_sdm_mode,
    
    dnx_data_synce_general_synce_nmg_chain_select,

    
    _dnx_data_synce_general_feature_nof
} dnx_data_synce_general_feature_e;



typedef int(
    *dnx_data_synce_general_feature_get_f) (
    int unit,
    dnx_data_synce_general_feature_e feature);


typedef uint32(
    *dnx_data_synce_general_nof_plls_get_f) (
    int unit);


typedef uint32(
    *dnx_data_synce_general_nif_div_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_synce_general_nif_div_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_synce_general_fabric_div_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_synce_general_fabric_div_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_synce_general_nof_clk_blocks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_synce_general_nof_fabric_clk_blocks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_synce_general_output_clk_mode_get_f) (
    int unit);


typedef const dnx_data_synce_general_cfg_t *(
    *dnx_data_synce_general_cfg_get_f) (
    int unit,
    int synce_index);


typedef const dnx_data_synce_general_nmg_chain_map_t *(
    *dnx_data_synce_general_nmg_chain_map_get_f) (
    int unit,
    int ethu_index);



typedef struct
{
    
    dnx_data_synce_general_feature_get_f feature_get;
    
    dnx_data_synce_general_nof_plls_get_f nof_plls_get;
    
    dnx_data_synce_general_nif_div_min_get_f nif_div_min_get;
    
    dnx_data_synce_general_nif_div_max_get_f nif_div_max_get;
    
    dnx_data_synce_general_fabric_div_min_get_f fabric_div_min_get;
    
    dnx_data_synce_general_fabric_div_max_get_f fabric_div_max_get;
    
    dnx_data_synce_general_nof_clk_blocks_get_f nof_clk_blocks_get;
    
    dnx_data_synce_general_nof_fabric_clk_blocks_get_f nof_fabric_clk_blocks_get;
    
    dnx_data_synce_general_output_clk_mode_get_f output_clk_mode_get;
    
    dnx_data_synce_general_cfg_get_f cfg_get;
    
    dnxc_data_table_info_get_f cfg_info_get;
    
    dnx_data_synce_general_nmg_chain_map_get_f nmg_chain_map_get;
    
    dnxc_data_table_info_get_f nmg_chain_map_info_get;
} dnx_data_if_synce_general_t;





typedef struct
{
    
    dnx_data_if_synce_general_t general;
} dnx_data_if_synce_t;




extern dnx_data_if_synce_t dnx_data_synce;


#endif 

