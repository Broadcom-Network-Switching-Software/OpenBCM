
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_PLL_H_

#define _DNX_DATA_PLL_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/pll/pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pll.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_pll_init(
    int unit);






typedef struct
{
    int in_freq;
    int out_freq;
} dnx_data_pll_general_nif_pll_t;


typedef struct
{
    int in_freq;
    int out_freq;
} dnx_data_pll_general_fabric_pll_t;



typedef enum
{
    dnx_data_pll_general_ts_freq_lock,
    dnx_data_pll_general_ts_refclk_source_internal,
    dnx_data_pll_general_is_supported,

    
    _dnx_data_pll_general_feature_nof
} dnx_data_pll_general_feature_e;



typedef int(
    *dnx_data_pll_general_feature_get_f) (
    int unit,
    dnx_data_pll_general_feature_e feature);


typedef uint32(
    *dnx_data_pll_general_ts_ptp_fref_get_f) (
    int unit);


typedef const dnx_data_pll_general_nif_pll_t *(
    *dnx_data_pll_general_nif_pll_get_f) (
    int unit,
    int pll_index);


typedef const dnx_data_pll_general_fabric_pll_t *(
    *dnx_data_pll_general_fabric_pll_get_f) (
    int unit,
    int pll_index);



typedef struct
{
    
    dnx_data_pll_general_feature_get_f feature_get;
    
    dnx_data_pll_general_ts_ptp_fref_get_f ts_ptp_fref_get;
    
    dnx_data_pll_general_nif_pll_get_f nif_pll_get;
    
    dnxc_data_table_info_get_f nif_pll_info_get;
    
    dnx_data_pll_general_fabric_pll_get_f fabric_pll_get;
    
    dnxc_data_table_info_get_f fabric_pll_info_get;
} dnx_data_if_pll_general_t;






typedef struct
{
    int pll1_id;
    int valid;
    int pdiv;
    int ndiv_int;
    int ch0_mdiv;
    int ch1_mdiv;
    int ch2_mdiv;
    int ch3_mdiv;
    int ch4_mdiv;
    int ch5_mdiv;
    int output_cml_en;
    int refclk_source_sel;
    int refclk;
} dnx_data_pll_pll1_config_t;



typedef enum
{

    
    _dnx_data_pll_pll1_feature_nof
} dnx_data_pll_pll1_feature_e;



typedef int(
    *dnx_data_pll_pll1_feature_get_f) (
    int unit,
    dnx_data_pll_pll1_feature_e feature);


typedef uint32(
    *dnx_data_pll_pll1_nof_pll1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll1_ts_pll1_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll1_bs_pll1_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll1_fab_pll1_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll1_nif_pll1_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll1_flexe_pll1_id_get_f) (
    int unit);


typedef const dnx_data_pll_pll1_config_t *(
    *dnx_data_pll_pll1_config_get_f) (
    int unit,
    int pll1_type);



typedef struct
{
    
    dnx_data_pll_pll1_feature_get_f feature_get;
    
    dnx_data_pll_pll1_nof_pll1_get_f nof_pll1_get;
    
    dnx_data_pll_pll1_ts_pll1_id_get_f ts_pll1_id_get;
    
    dnx_data_pll_pll1_bs_pll1_id_get_f bs_pll1_id_get;
    
    dnx_data_pll_pll1_fab_pll1_id_get_f fab_pll1_id_get;
    
    dnx_data_pll_pll1_nif_pll1_id_get_f nif_pll1_id_get;
    
    dnx_data_pll_pll1_flexe_pll1_id_get_f flexe_pll1_id_get;
    
    dnx_data_pll_pll1_config_get_f config_get;
    
    dnxc_data_table_info_get_f config_info_get;
} dnx_data_if_pll_pll1_t;






typedef struct
{
    int pll3_id;
    int valid;
    int ch1_mdiv;
    int ch2_mdiv;
    int ch3_mdiv;
    int ch4_mdiv;
    int ch5_mdiv;
} dnx_data_pll_pll3_config_t;



typedef enum
{

    
    _dnx_data_pll_pll3_feature_nof
} dnx_data_pll_pll3_feature_e;



typedef int(
    *dnx_data_pll_pll3_feature_get_f) (
    int unit,
    dnx_data_pll_pll3_feature_e feature);


typedef uint32(
    *dnx_data_pll_pll3_nof_pll3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_vco_clock_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_kp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_ki_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_en_ctrl_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_en_ctrl_byp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_route_ctr_byp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_ref_clock_125_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_ref_clock_156_25_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_ref_clock_312_5_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_nif0_pll3_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_nif1_pll3_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_mas0_pll3_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll3_mas1_pll3_id_get_f) (
    int unit);


typedef const dnx_data_pll_pll3_config_t *(
    *dnx_data_pll_pll3_config_get_f) (
    int unit,
    int pll3_type);



typedef struct
{
    
    dnx_data_pll_pll3_feature_get_f feature_get;
    
    dnx_data_pll_pll3_nof_pll3_get_f nof_pll3_get;
    
    dnx_data_pll_pll3_vco_clock_get_f vco_clock_get;
    
    dnx_data_pll_pll3_kp_get_f kp_get;
    
    dnx_data_pll_pll3_ki_get_f ki_get;
    
    dnx_data_pll_pll3_en_ctrl_get_f en_ctrl_get;
    
    dnx_data_pll_pll3_en_ctrl_byp_get_f en_ctrl_byp_get;
    
    dnx_data_pll_pll3_route_ctr_byp_get_f route_ctr_byp_get;
    
    dnx_data_pll_pll3_ref_clock_125_get_f ref_clock_125_get;
    
    dnx_data_pll_pll3_ref_clock_156_25_get_f ref_clock_156_25_get;
    
    dnx_data_pll_pll3_ref_clock_312_5_get_f ref_clock_312_5_get;
    
    dnx_data_pll_pll3_nif0_pll3_id_get_f nif0_pll3_id_get;
    
    dnx_data_pll_pll3_nif1_pll3_id_get_f nif1_pll3_id_get;
    
    dnx_data_pll_pll3_mas0_pll3_id_get_f mas0_pll3_id_get;
    
    dnx_data_pll_pll3_mas1_pll3_id_get_f mas1_pll3_id_get;
    
    dnx_data_pll_pll3_config_get_f config_get;
    
    dnxc_data_table_info_get_f config_info_get;
} dnx_data_if_pll_pll3_t;






typedef struct
{
    int pll4_id;
    int valid;
    int ref_clk_src_sel;
    int ref_clk;
    int dll_ref_pdiv;
    int dll_post_en;
    int aux_post_en;
    int pdiv;
    int ndiv_frac_mode;
    int ndiv_p;
    int ndiv_q;
    int ndiv_int;
    int d2c_en;
    int ch0_mdiv;
    int ch1_mdiv;
    int ch2_mdiv;
    int ch3_mdiv;
    int ch6_mdiv;
} dnx_data_pll_pll4_config_t;



typedef enum
{
    dnx_data_pll_pll4_pll_in_frac_mode,
    dnx_data_pll_pll4_extra_post_div_dis,
    dnx_data_pll_pll4_bs_pll_holdover_support,
    dnx_data_pll_pll4_ts_pll_dither_support,
    dnx_data_pll_pll4_bs_pll_holdover_enable,
    dnx_data_pll_pll4_ts_pll_dither_enable,

    
    _dnx_data_pll_pll4_feature_nof
} dnx_data_pll_pll4_feature_e;



typedef int(
    *dnx_data_pll_pll4_feature_get_f) (
    int unit,
    dnx_data_pll_pll4_feature_e feature);


typedef uint32(
    *dnx_data_pll_pll4_nof_pll4_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll4_ts_pll4_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll4_bs_pll4_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll4_nif_pll4_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll4_fab_pll4_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll4_nif_tsc_pll4_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll4_framer_pll4_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pll_pll4_sar_pll4_id_get_f) (
    int unit);


typedef const dnx_data_pll_pll4_config_t *(
    *dnx_data_pll_pll4_config_get_f) (
    int unit,
    int pll4_type);



typedef struct
{
    
    dnx_data_pll_pll4_feature_get_f feature_get;
    
    dnx_data_pll_pll4_nof_pll4_get_f nof_pll4_get;
    
    dnx_data_pll_pll4_ts_pll4_id_get_f ts_pll4_id_get;
    
    dnx_data_pll_pll4_bs_pll4_id_get_f bs_pll4_id_get;
    
    dnx_data_pll_pll4_nif_pll4_id_get_f nif_pll4_id_get;
    
    dnx_data_pll_pll4_fab_pll4_id_get_f fab_pll4_id_get;
    
    dnx_data_pll_pll4_nif_tsc_pll4_id_get_f nif_tsc_pll4_id_get;
    
    dnx_data_pll_pll4_framer_pll4_id_get_f framer_pll4_id_get;
    
    dnx_data_pll_pll4_sar_pll4_id_get_f sar_pll4_id_get;
    
    dnx_data_pll_pll4_config_get_f config_get;
    
    dnxc_data_table_info_get_f config_info_get;
} dnx_data_if_pll_pll4_t;







typedef enum
{
    dnx_data_pll_synce_pll_present,

    
    _dnx_data_pll_synce_pll_feature_nof
} dnx_data_pll_synce_pll_feature_e;



typedef int(
    *dnx_data_pll_synce_pll_feature_get_f) (
    int unit,
    dnx_data_pll_synce_pll_feature_e feature);



typedef struct
{
    
    dnx_data_pll_synce_pll_feature_get_f feature_get;
} dnx_data_if_pll_synce_pll_t;





typedef struct
{
    
    dnx_data_if_pll_general_t general;
    
    dnx_data_if_pll_pll1_t pll1;
    
    dnx_data_if_pll_pll3_t pll3;
    
    dnx_data_if_pll_pll4_t pll4;
    
    dnx_data_if_pll_synce_pll_t synce_pll;
} dnx_data_if_pll_t;




extern dnx_data_if_pll_t dnx_data_pll;


#endif 

