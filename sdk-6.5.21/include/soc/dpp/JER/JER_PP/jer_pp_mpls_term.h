/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_JER_PP_MPLS_TERM_INCLUDED__

#define __SOC_JER_PP_MPLS_TERM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <shared/swstate/sw_state.h>





#define JER_MPLS_TERM  sw_state_access[unit].dpp.soc.jericho.pp.mpls_term








typedef struct jer_pp_mpls_term_s {
    SOC_PPC_LIF_ID local_mldp_dummy_lif_id[2]; 
} jer_pp_mpls_term_t; 











soc_error_t soc_jer_pp_mpls_termination_spacial_labels_init(int unit);


soc_error_t soc_jer_pp_mpls_termination_l4b_l5l_init(int unit);


soc_error_t soc_jer_pp_mpls_termination_range_action_set(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *range_action_info,uint32 flags);
soc_error_t soc_jer_pp_mpls_termination_range_action_get(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *range_action_info);


soc_error_t soc_jer_pp_mpls_termination_range_profile_set(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *range_profile_info);
soc_error_t soc_jer_pp_mpls_termination_range_profile_get(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *range_profile_info);


soc_error_t soc_jer_pp_mpls_termination_vccv_type_ttl1_oam_classification_set(int unit, uint8 vccv_type_ttl1_oam_classification_enabled);
soc_error_t soc_jer_pp_mpls_termination_vccv_type_ttl1_oam_classification_get(int unit, uint8 * vccv_type_ttl1_oam_classification_enabled);

soc_error_t soc_jer_pp_mpls_termination_first_label_bit_map_set(int unit, uint32 termination_bit_map);
soc_error_t soc_jer_pp_mpls_termination_first_label_bit_map_get(int unit, uint32 * termination_bit_map);
soc_error_t soc_jer_pp_mpls_termination_first_label_bit_map_init(int unit, uint32 termination_bit_map);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
