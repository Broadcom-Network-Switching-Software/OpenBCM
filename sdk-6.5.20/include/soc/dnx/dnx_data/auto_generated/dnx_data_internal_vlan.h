

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_VLAN_H_

#define _DNX_DATA_INTERNAL_VLAN_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_vlan_submodule_vlan_translate,
    dnx_data_vlan_submodule_llvp,

    
    _dnx_data_vlan_submodule_nof
} dnx_data_vlan_submodule_e;








int dnx_data_vlan_vlan_translate_feature_get(
    int unit,
    dnx_data_vlan_vlan_translate_feature_e feature);



typedef enum
{
    dnx_data_vlan_vlan_translate_define_nof_vlan_tag_formats,
    dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_profiles,
    dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_profiles,
    dnx_data_vlan_vlan_translate_define_nof_vlan_range_entries,
    dnx_data_vlan_vlan_translate_define_nof_vlan_ranges_per_entry,
    dnx_data_vlan_vlan_translate_define_egress_tpid_outer_packet_index,
    dnx_data_vlan_vlan_translate_define_egress_tpid_inner_packet_index,
    dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_action_ids,
    dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_action_ids,

    
    _dnx_data_vlan_vlan_translate_define_nof
} dnx_data_vlan_vlan_translate_define_e;



uint32 dnx_data_vlan_vlan_translate_nof_vlan_tag_formats_get(
    int unit);


uint32 dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_profiles_get(
    int unit);


uint32 dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_profiles_get(
    int unit);


uint32 dnx_data_vlan_vlan_translate_nof_vlan_range_entries_get(
    int unit);


uint32 dnx_data_vlan_vlan_translate_nof_vlan_ranges_per_entry_get(
    int unit);


uint32 dnx_data_vlan_vlan_translate_egress_tpid_outer_packet_index_get(
    int unit);


uint32 dnx_data_vlan_vlan_translate_egress_tpid_inner_packet_index_get(
    int unit);


uint32 dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_action_ids_get(
    int unit);


uint32 dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_action_ids_get(
    int unit);



typedef enum
{

    
    _dnx_data_vlan_vlan_translate_table_nof
} dnx_data_vlan_vlan_translate_table_e;









int dnx_data_vlan_llvp_feature_get(
    int unit,
    dnx_data_vlan_llvp_feature_e feature);



typedef enum
{
    dnx_data_vlan_llvp_define_ext_vcp_en,

    
    _dnx_data_vlan_llvp_define_nof
} dnx_data_vlan_llvp_define_e;



uint32 dnx_data_vlan_llvp_ext_vcp_en_get(
    int unit);



typedef enum
{

    
    _dnx_data_vlan_llvp_table_nof
} dnx_data_vlan_llvp_table_e;






shr_error_e dnx_data_vlan_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

