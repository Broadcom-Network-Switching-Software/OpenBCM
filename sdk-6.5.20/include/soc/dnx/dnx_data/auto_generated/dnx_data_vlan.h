

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_VLAN_H_

#define _DNX_DATA_VLAN_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_vlan.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_vlan_init(
    int unit);







typedef enum
{

    
    _dnx_data_vlan_vlan_translate_feature_nof
} dnx_data_vlan_vlan_translate_feature_e;



typedef int(
    *dnx_data_vlan_vlan_translate_feature_get_f) (
    int unit,
    dnx_data_vlan_vlan_translate_feature_e feature);


typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_vlan_tag_formats_get_f) (
    int unit);


typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_vlan_range_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_vlan_ranges_per_entry_get_f) (
    int unit);


typedef uint32(
    *dnx_data_vlan_vlan_translate_egress_tpid_outer_packet_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_vlan_vlan_translate_egress_tpid_inner_packet_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_action_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_action_ids_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_vlan_vlan_translate_feature_get_f feature_get;
    
    dnx_data_vlan_vlan_translate_nof_vlan_tag_formats_get_f nof_vlan_tag_formats_get;
    
    dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_profiles_get_f nof_ingress_vlan_edit_profiles_get;
    
    dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_profiles_get_f nof_egress_vlan_edit_profiles_get;
    
    dnx_data_vlan_vlan_translate_nof_vlan_range_entries_get_f nof_vlan_range_entries_get;
    
    dnx_data_vlan_vlan_translate_nof_vlan_ranges_per_entry_get_f nof_vlan_ranges_per_entry_get;
    
    dnx_data_vlan_vlan_translate_egress_tpid_outer_packet_index_get_f egress_tpid_outer_packet_index_get;
    
    dnx_data_vlan_vlan_translate_egress_tpid_inner_packet_index_get_f egress_tpid_inner_packet_index_get;
    
    dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_action_ids_get_f nof_ingress_vlan_edit_action_ids_get;
    
    dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_action_ids_get_f nof_egress_vlan_edit_action_ids_get;
} dnx_data_if_vlan_vlan_translate_t;







typedef enum
{

    
    _dnx_data_vlan_llvp_feature_nof
} dnx_data_vlan_llvp_feature_e;



typedef int(
    *dnx_data_vlan_llvp_feature_get_f) (
    int unit,
    dnx_data_vlan_llvp_feature_e feature);


typedef uint32(
    *dnx_data_vlan_llvp_ext_vcp_en_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_vlan_llvp_feature_get_f feature_get;
    
    dnx_data_vlan_llvp_ext_vcp_en_get_f ext_vcp_en_get;
} dnx_data_if_vlan_llvp_t;





typedef struct
{
    
    dnx_data_if_vlan_vlan_translate_t vlan_translate;
    
    dnx_data_if_vlan_llvp_t llvp;
} dnx_data_if_vlan_t;




extern dnx_data_if_vlan_t dnx_data_vlan;


#endif 

