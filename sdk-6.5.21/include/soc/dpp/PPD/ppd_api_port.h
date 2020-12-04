/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_PORT_INCLUDED__

#define __SOC_PPD_API_PORT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_port.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_PORT_INFO_SET = SOC_PPD_PROC_DESC_BASE_PORT_FIRST,
  SOC_PPD_PORT_INFO_SET_PRINT,
  SOC_PPD_PORT_INFO_GET,
  SOC_PPD_PORT_INFO_GET_PRINT,
  SOC_PPD_PORT_STP_STATE_SET,
  SOC_PPD_PORT_STP_STATE_SET_PRINT,
  SOC_PPD_PORT_STP_STATE_GET,
  SOC_PPD_PORT_STP_STATE_GET_PRINT,
  SOC_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_SET,
  SOC_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_SET_PRINT,
  SOC_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_GET,
  SOC_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_GET_PRINT,
  SOC_PPD_PORT_GET_PROCS_PTR,
  
  
  SOC_PPD_PORT_PROCEDURE_DESC_LAST
} SOC_PPD_PORT_PROCEDURE_DESC;










uint32
  soc_ppd_port_info_set(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PORT_INFO                           *port_info
  );


uint32
  soc_ppd_port_info_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_PORT_INFO                           *port_info
  );


uint32
  soc_ppd_port_stp_state_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                topology_id_ndx,
    SOC_SAND_IN  SOC_PPC_PORT_STP_STATE                      stp_state
  );


uint32
  soc_ppd_port_stp_state_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                topology_id_ndx,
    SOC_SAND_OUT SOC_PPC_PORT_STP_STATE                      *stp_state
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

