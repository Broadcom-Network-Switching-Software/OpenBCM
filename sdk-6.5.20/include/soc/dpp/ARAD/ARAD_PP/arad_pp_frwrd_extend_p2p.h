
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FRWRD_EXT_P2P_INCLUDED__

#define __ARAD_PP_FRWRD_EXT_P2P_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/PPC/ppc_api_general.h>
























soc_error_t
  arad_pp_frwrd_extend_p2p_lem_entry_add(int unit, 
                                         SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info,
                                         SOC_PPC_FRWRD_DECISION_INFO *frwrd_info);


soc_error_t
  arad_pp_frwrd_extend_p2p_lem_entry_remove(int unit, 
                                         SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info);

soc_error_t
  arad_pp_frwrd_extend_p2p_lem_entry_get(int unit, 
                                         SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info,
                                         SOC_PPC_FRWRD_DECISION_INFO *frwrd_info,
                                         uint8 * found);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

