
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __QAX_PP_EG_ENCAP_INCLUDED__

#define __QAX_PP_EG_ENCAP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>




                     















soc_error_t soc_qax_eg_encap_additional_label_profile_set(int unit, SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO *additional_label_profile_info, int profile_index);

soc_error_t soc_qax_eg_encap_additional_label_profile_get(int unit, SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO *additional_label_profile_info, int profile_index);



uint32 soc_qax_eg_encap_header_compensation_per_cud_set(int unit, int cud_msb, int value);

uint32 soc_qax_eg_encap_header_compensation_per_cud_get(int unit, int cud_msb, int *value);


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


