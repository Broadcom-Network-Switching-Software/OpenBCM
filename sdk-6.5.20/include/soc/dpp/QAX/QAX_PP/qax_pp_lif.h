
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __QAX_PP_LIF_INCLUDED__

#define __QAX_PP_LIF_INCLUDED__





#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>



soc_error_t qax_pp_lif_default_native_ac_outlif_init(int unit); 


soc_error_t qax_pp_lif_default_native_ac_outlif_set(int unit, uint32 local_out_lif_id); 


soc_error_t qax_pp_lif_default_native_ac_outlif_get(int unit, uint32* local_out_lif_id); 


#endif 


