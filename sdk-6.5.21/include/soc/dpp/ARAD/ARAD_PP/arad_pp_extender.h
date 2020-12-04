/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_PP_EXTENDER_INCLUDED__

#define __ARAD_PP_EXTENDER_INCLUDED__





#include <soc/dcmn/error.h>
#include <soc/dpp/PPC/ppc_api_extender.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
















typedef struct arad_pp_extender_info_s {
    uint16                        etag_ethertype; 
} arad_pp_extender_info_t;










soc_error_t arad_pp_extender_init(int unit);
soc_error_t arad_pp_extender_deinit(int unit);

soc_error_t arad_pp_extender_port_info_set(int unit, SOC_PPC_PORT port, SOC_PPC_EXTENDER_PORT_INFO *port_info);
soc_error_t arad_pp_extender_port_info_get(int unit, SOC_PPC_PORT port, SOC_PPC_EXTENDER_PORT_INFO *port_info);


soc_error_t arad_pp_extender_global_etag_ethertype_set(int unit, uint16 etag_tpid);
soc_error_t arad_pp_extender_global_etag_ethertype_get(int unit, uint16 *etag_tpid);

soc_error_t arad_pp_extender_eve_etag_format_set(int unit, uint32 edit_profile, uint8 is_extender);
soc_error_t arad_pp_extender_eve_etag_format_get(int unit, uint32 edit_profile, uint8 *is_extender);


#endif


