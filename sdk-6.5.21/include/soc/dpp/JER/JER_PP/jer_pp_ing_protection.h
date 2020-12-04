/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_JER_PP_ING_PROTECTION_INCLUDED__

#define __SOC_JER_PP_ING_PROTECTION_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
























soc_error_t soc_jer_pp_ing_protection_init(
   int unit);

soc_error_t soc_jer_ingress_protection_state_set(
   int unit,
   uint32 protection_ndx,
   uint8 path_state);

soc_error_t soc_jer_ingress_protection_state_get(
   int unit,
   uint32 protection_ndx,
   uint8 *path_state);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

