/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_EG_MIRROR_INCLUDED__

#define __SOC_PPD_API_EG_MIRROR_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_eg_mirror.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_EG_MIRROR_PORT_VLAN_ADD = SOC_PPD_PROC_DESC_BASE_EG_MIRROR_FIRST,
  SOC_PPD_EG_MIRROR_PORT_VLAN_ADD_PRINT,
  SOC_PPD_EG_MIRROR_PORT_VLAN_REMOVE,
  SOC_PPD_EG_MIRROR_PORT_VLAN_REMOVE_PRINT,
  SOC_PPD_EG_MIRROR_PORT_VLAN_GET,
  SOC_PPD_EG_MIRROR_PORT_VLAN_GET_PRINT,
  SOC_PPD_EG_MIRROR_PORT_DFLT_SET,
  SOC_PPD_EG_MIRROR_PORT_DFLT_SET_PRINT,
  SOC_PPD_EG_MIRROR_PORT_DFLT_GET,
  SOC_PPD_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET,
  SOC_PPD_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET,  
  SOC_PPD_EG_MIRROR_PORT_DFLT_GET_PRINT,
  SOC_PPD_EG_MIRROR_PORT_INFO_GET,  
  SOC_PPD_EG_MIRROR_PORT_INFO_GET_PRINT,  
  SOC_PPD_EG_MIRROR_GET_PROCS_PTR,
  
  
  SOC_PPD_EG_MIRROR_PROCEDURE_DESC_LAST
} SOC_PPD_EG_MIRROR_PROCEDURE_DESC;










uint32
  soc_ppd_eg_mirror_port_vlan_add(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN dpp_outbound_mirror_config_t        *config,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_eg_mirror_port_vlan_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx
  );


uint32
  soc_ppd_eg_mirror_port_vlan_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_OUT dpp_outbound_mirror_config_t        *config
  );


uint32
  soc_ppd_eg_mirror_port_dflt_set(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                      pp_port,
    SOC_SAND_IN  dpp_outbound_mirror_config_t       *config

  );


uint32
  soc_ppd_eg_mirror_port_dflt_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT  dpp_outbound_mirror_config_t       *config
  );


uint32
  soc_ppd_eg_mirror_port_appl_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  SOC_PPC_PORT  local_port_ndx,
    SOC_SAND_IN  uint8     appl_mirroring_info
  );


uint32
  soc_ppd_eg_mirror_port_appl_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  SOC_PPC_PORT  local_port_ndx,
    SOC_SAND_OUT uint8     *appl_mirroring_info
  );


uint32
  soc_ppd_eg_mirror_port_info_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  int    core_id,
    SOC_SAND_IN  SOC_PPC_PORT  local_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_MIRROR_PORT_INFO *info
  );



uint32
  soc_ppd_eg_mirror_recycle_command_trap_set(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  uint32        recycle_command, 
    SOC_SAND_IN  uint32        trap_code, 
    SOC_SAND_IN  uint32        snoop_strength,
    SOC_SAND_IN  uint32        forward_strengh 
  );

uint32
  soc_ppd_eg_mirror_recycle_command_trap_get(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  uint32        recycle_command, 
    SOC_SAND_OUT  uint32       *trap_code,
    SOC_SAND_OUT  uint32       *snoop_strength
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

