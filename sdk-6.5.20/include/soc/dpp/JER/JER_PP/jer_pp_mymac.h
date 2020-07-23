/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_JER_PP_MYMAC_INCLUDED__

#define __SOC_JER_PP_MYMAC_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>


























soc_error_t 
soc_jer_mymac_2nd_mymac_init(int unit);


soc_error_t 
soc_jer_mymac_protocol_group_set(int unit, uint32 protocols, uint32 group);


soc_error_t 
soc_jer_mymac_protocol_group_get_protocol_by_group(int unit, uint8 group, uint32 *protocols);


soc_error_t 
soc_jer_mymac_protocol_group_get_group_by_protocols(int unit, uint32 protocols, uint8 *group);

soc_error_t
soc_jer_mymac_vrrp_tcam_info_set(int unit,SOC_PPC_VRRP_CAM_INFO *info);

soc_error_t
soc_jer_mymac_vrrp_tcam_info_delete(int unit, uint8 cam_index);




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

