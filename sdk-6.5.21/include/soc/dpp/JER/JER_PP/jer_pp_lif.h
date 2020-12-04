/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_JER_PP_LIF_INCLUDED__

#define __SOC_JER_PP_LIF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>


























soc_error_t
soc_jer_lif_glem_access_entry_add(int unit, int global_lif_index, int egress_lif_index);


soc_error_t
soc_jer_lif_glem_access_entry_remove(int unit, int global_lif_index);


soc_error_t
soc_jer_lif_glem_access_entry_by_key_get(int unit, int global_lif_id, int *egress_lif_id, uint8 *accessed, uint8 *is_found);

soc_error_t
soc_jer_lif_init(int unit);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


