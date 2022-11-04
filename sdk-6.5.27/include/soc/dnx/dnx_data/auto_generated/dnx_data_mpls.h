
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_MPLS_H_

#define _DNX_DATA_MPLS_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_mpls.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_mpls_init(
    int unit);







typedef enum
{
    dnx_data_mpls_general_mpls_term_1_or_2_labels,
    dnx_data_mpls_general_mpls_term_till_bos,
    dnx_data_mpls_general_mpls_ioam_d_flag_stamping,
    dnx_data_mpls_general_stack_termination_indication,
    dnx_data_mpls_general_support_alternate_marking,
    dnx_data_mpls_general_fhei_init_v1,
    dnx_data_mpls_general_per_port_ilm_lookup_supported,
    dnx_data_mpls_general_enc2_implicit_null_w_ahp_supported,
    dnx_data_mpls_general_configurable_fhei_supported,
    dnx_data_mpls_general_mpls_raw_vtt_handling_supported,
    dnx_data_mpls_general_mpls_terminator_force_commit,
    dnx_data_mpls_general_per_intf_lookup_supported,
    dnx_data_mpls_general_coupling_label_lookup_supported,
    dnx_data_mpls_general_tandem_supported,

    
    _dnx_data_mpls_general_feature_nof
} dnx_data_mpls_general_feature_e;



typedef int(
    *dnx_data_mpls_general_feature_get_f) (
    int unit,
    dnx_data_mpls_general_feature_e feature);


typedef uint32(
    *dnx_data_mpls_general_nof_mpls_termination_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_nof_mpls_push_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_nof_bits_mpls_label_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_log_nof_mpls_range_elements_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_nof_mpls_range_elements_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_mpls_special_label_encap_handling_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_mpls_control_word_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_mpls_second_stage_parser_handling_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_mpls_control_word_flag_bits_lsb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_fhei_mps_type_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_mpls_speculative_learning_handling_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_gach_with_flow_label_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_max_nof_mpls_term_stages_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mpls_general_nof_php_gport_lif_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_mpls_general_feature_get_f feature_get;
    
    dnx_data_mpls_general_nof_mpls_termination_profiles_get_f nof_mpls_termination_profiles_get;
    
    dnx_data_mpls_general_nof_mpls_push_profiles_get_f nof_mpls_push_profiles_get;
    
    dnx_data_mpls_general_nof_bits_mpls_label_get_f nof_bits_mpls_label_get;
    
    dnx_data_mpls_general_log_nof_mpls_range_elements_get_f log_nof_mpls_range_elements_get;
    
    dnx_data_mpls_general_nof_mpls_range_elements_get_f nof_mpls_range_elements_get;
    
    dnx_data_mpls_general_mpls_special_label_encap_handling_get_f mpls_special_label_encap_handling_get;
    
    dnx_data_mpls_general_mpls_control_word_supported_get_f mpls_control_word_supported_get;
    
    dnx_data_mpls_general_mpls_second_stage_parser_handling_get_f mpls_second_stage_parser_handling_get;
    
    dnx_data_mpls_general_mpls_control_word_flag_bits_lsb_get_f mpls_control_word_flag_bits_lsb_get;
    
    dnx_data_mpls_general_fhei_mps_type_nof_bits_get_f fhei_mps_type_nof_bits_get;
    
    dnx_data_mpls_general_mpls_speculative_learning_handling_get_f mpls_speculative_learning_handling_get;
    
    dnx_data_mpls_general_gach_with_flow_label_supported_get_f gach_with_flow_label_supported_get;
    
    dnx_data_mpls_general_max_nof_mpls_term_stages_get_f max_nof_mpls_term_stages_get;
    
    dnx_data_mpls_general_nof_php_gport_lif_get_f nof_php_gport_lif_get;
} dnx_data_if_mpls_general_t;





typedef struct
{
    
    dnx_data_if_mpls_general_t general;
} dnx_data_if_mpls_t;




extern dnx_data_if_mpls_t dnx_data_mpls;


#endif 

