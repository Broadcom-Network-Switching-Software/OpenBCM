

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_ARR_H_

#define _DNX_DATA_INTERNAL_ARR_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_arr.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_arr_submodule_prefix,
    dnx_data_arr_submodule_max_nof_entries,
    dnx_data_arr_submodule_formats,

    
    _dnx_data_arr_submodule_nof
} dnx_data_arr_submodule_e;








int dnx_data_arr_prefix_feature_get(
    int unit,
    dnx_data_arr_prefix_feature_e feature);



typedef enum
{
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_0,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_1,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_2,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_3,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_4,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_5,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_6,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_7,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_8,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_9,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_10,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_11,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_12,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_13,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_14,
    dnx_data_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_15,
    dnx_data_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_0,
    dnx_data_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_2,
    dnx_data_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_3,
    dnx_data_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_4,
    dnx_data_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_5,
    dnx_data_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_6,
    dnx_data_arr_prefix_define_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_0,
    dnx_data_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_0,
    dnx_data_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_1,
    dnx_data_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_2,
    dnx_data_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_3,
    dnx_data_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_4,
    dnx_data_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_5,
    dnx_data_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_6,
    dnx_data_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_7,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_0,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_1,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_2,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_3,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_4,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_5,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_6,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_7,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_8,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_10,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_11,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_12,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_13,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_14,
    dnx_data_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_15,
    dnx_data_arr_prefix_define_IRPP_VSI_FORMATS___prefix_0,
    dnx_data_arr_prefix_define_IRPP_VSI_FORMATS___prefix_1,
    dnx_data_arr_prefix_define_IRPP_VSI_FORMATS___prefix_2,
    dnx_data_arr_prefix_define_IRPP_VSI_FORMATS___prefix_3,
    dnx_data_arr_prefix_define_IRPP_VSI_FORMATS___prefix_4,
    dnx_data_arr_prefix_define_IRPP_VSI_FORMATS___prefix_5,
    dnx_data_arr_prefix_define_IRPP_VSI_FORMATS___prefix_6,
    dnx_data_arr_prefix_define_IRPP_VSI_FORMATS___prefix_7,

    
    _dnx_data_arr_prefix_define_nof
} dnx_data_arr_prefix_define_e;



uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_0_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_1_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_2_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_3_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_4_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_5_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_6_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_7_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_8_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_9_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_10_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_11_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_12_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_13_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_14_get(
    int unit);


uint32 dnx_data_arr_prefix_ETPP_ETPS_FORMATS___prefix_15_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_0_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_2_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_3_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_4_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_5_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_6_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_0_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_0_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_1_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_2_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_3_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_4_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_5_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_6_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_7_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_0_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_1_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_2_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_3_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_4_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_5_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_6_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_7_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_8_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_10_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_11_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_12_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_13_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_14_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_15_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_VSI_FORMATS___prefix_0_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_VSI_FORMATS___prefix_1_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_VSI_FORMATS___prefix_2_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_VSI_FORMATS___prefix_3_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_VSI_FORMATS___prefix_4_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_VSI_FORMATS___prefix_5_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_VSI_FORMATS___prefix_6_get(
    int unit);


uint32 dnx_data_arr_prefix_IRPP_VSI_FORMATS___prefix_7_get(
    int unit);



typedef enum
{

    
    _dnx_data_arr_prefix_table_nof
} dnx_data_arr_prefix_table_e;









int dnx_data_arr_max_nof_entries_feature_get(
    int unit,
    dnx_data_arr_max_nof_entries_feature_e feature);



typedef enum
{
    dnx_data_arr_max_nof_entries_define_AGE_PROFILE_ARR,
    dnx_data_arr_max_nof_entries_define_ETPP_ENCAP_1_ARR_ToS,
    dnx_data_arr_max_nof_entries_define_ETPP_ENCAP_2_ARR_ToS,
    dnx_data_arr_max_nof_entries_define_ETPP_ENCAP_3_ARR_ToS,
    dnx_data_arr_max_nof_entries_define_ETPP_ENCAP_4_ARR_ToS,
    dnx_data_arr_max_nof_entries_define_ETPP_ENCAP_5_ARR_ToS,
    dnx_data_arr_max_nof_entries_define_ETPP_ENCAP_ARR_ToS_plus_2,
    dnx_data_arr_max_nof_entries_define_ETPP_FWD_ARR_ToS,
    dnx_data_arr_max_nof_entries_define_ETPP_FWD_ARR_ToS_plus_1,
    dnx_data_arr_max_nof_entries_define_ETPP_Preprocessing_ARR,
    dnx_data_arr_max_nof_entries_define_ETPP_Termination_1_ARR,
    dnx_data_arr_max_nof_entries_define_ETPP_Termination_2_ARR,
    dnx_data_arr_max_nof_entries_define_IPPB_ACCEPTED_ARR,
    dnx_data_arr_max_nof_entries_define_IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR,
    dnx_data_arr_max_nof_entries_define_IPPB_EXPECTED_ARR,
    dnx_data_arr_max_nof_entries_define_IRPP_FEC_ARR,
    dnx_data_arr_max_nof_entries_define_IRPP_FWD1_FWD_ARR,
    dnx_data_arr_max_nof_entries_define_IRPP_FWD1_RPF_ARR,
    dnx_data_arr_max_nof_entries_define_IRPP_SUPER_FEC_ARR,
    dnx_data_arr_max_nof_entries_define_IRPP_VSI_ARR,
    dnx_data_arr_max_nof_entries_define_IRPP_VTT_ARR,
    dnx_data_arr_max_nof_entries_define_ITPP_MACT_LEARN_JR1_TRANSLATE_ARR,
    dnx_data_arr_max_nof_entries_define_PPMC_ERPP_ARR,
    dnx_data_arr_max_nof_entries_define_USER_ALLOCATION,

    
    _dnx_data_arr_max_nof_entries_define_nof
} dnx_data_arr_max_nof_entries_define_e;



uint32 dnx_data_arr_max_nof_entries_AGE_PROFILE_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_ENCAP_1_ARR_ToS_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_ENCAP_2_ARR_ToS_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_ENCAP_3_ARR_ToS_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_ENCAP_4_ARR_ToS_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_ENCAP_5_ARR_ToS_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_ENCAP_ARR_ToS_plus_2_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_FWD_ARR_ToS_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_FWD_ARR_ToS_plus_1_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_Preprocessing_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_Termination_1_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ETPP_Termination_2_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_IPPB_ACCEPTED_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_IPPB_EXPECTED_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_IRPP_FEC_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_IRPP_FWD1_FWD_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_IRPP_FWD1_RPF_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_IRPP_SUPER_FEC_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_IRPP_VSI_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_IRPP_VTT_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_ITPP_MACT_LEARN_JR1_TRANSLATE_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_PPMC_ERPP_ARR_get(
    int unit);


uint32 dnx_data_arr_max_nof_entries_USER_ALLOCATION_get(
    int unit);



typedef enum
{

    
    _dnx_data_arr_max_nof_entries_table_nof
} dnx_data_arr_max_nof_entries_table_e;









int dnx_data_arr_formats_feature_get(
    int unit,
    dnx_data_arr_formats_feature_e feature);



typedef enum
{

    
    _dnx_data_arr_formats_define_nof
} dnx_data_arr_formats_define_e;




typedef enum
{
    dnx_data_arr_formats_table_files,
    dnx_data_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG,
    dnx_data_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0,
    dnx_data_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1,
    dnx_data_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION,
    dnx_data_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array,
    dnx_data_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION,
    dnx_data_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array,
    dnx_data_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION,
    dnx_data_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG,
    dnx_data_arr_formats_table_MACT_MRQ_ARR_CFG,
    dnx_data_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG,
    dnx_data_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array,
    dnx_data_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array,
    dnx_data_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array,
    dnx_data_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array,
    dnx_data_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1,
    dnx_data_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array,
    dnx_data_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array,
    dnx_data_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE,
    dnx_data_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG,

    
    _dnx_data_arr_formats_table_nof
} dnx_data_arr_formats_table_e;



const dnx_data_arr_formats_files_t * dnx_data_arr_formats_files_get(
    int unit);


const dnx_data_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t * dnx_data_arr_formats_MACT_AGE_PROFILE_ARR_CFG_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t * dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t * dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t * dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t * dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_get(
    int unit,
    int table_index,
    int index);


const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t * dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_get(
    int unit,
    int index);


const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t * dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(
    int unit,
    int table_index,
    int index);


const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t * dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_get(
    int unit,
    int index);


const dnx_data_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t * dnx_data_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_get(
    int unit,
    int index);


const dnx_data_arr_formats_MACT_MRQ_ARR_CFG_t * dnx_data_arr_formats_MACT_MRQ_ARR_CFG_get(
    int unit,
    int index);


const dnx_data_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t * dnx_data_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_get(
    int unit,
    int index);


const dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t * dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index);


const dnx_data_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t * dnx_data_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index);


const dnx_data_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t * dnx_data_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index);


const dnx_data_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t * dnx_data_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index);


const dnx_data_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t * dnx_data_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_get(
    int unit,
    int index);


const dnx_data_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t * dnx_data_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t * dnx_data_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index);


const dnx_data_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t * dnx_data_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index);


const dnx_data_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t * dnx_data_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t * dnx_data_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_get(
    int unit,
    int index);


const dnx_data_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t * dnx_data_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_get(
    int unit,
    int index);



shr_error_e dnx_data_arr_formats_files_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_MACT_AGE_PROFILE_ARR_CFG_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_MACT_MRQ_ARR_CFG_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_arr_formats_files_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_MACT_AGE_PROFILE_ARR_CFG_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_MACT_MRQ_ARR_CFG_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_info_get(
    int unit);



shr_error_e dnx_data_arr_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

