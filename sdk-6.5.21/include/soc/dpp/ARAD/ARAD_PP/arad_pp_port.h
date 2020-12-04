/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_PPC_PORT_INCLUDED__

#define __SOC_PPC_PORT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_port.h>













typedef enum
{
  
  SOC_PPC_PORT_INFO_SET = ARAD_PP_PROC_DESC_BASE_PORT_FIRST,
  SOC_PPC_PORT_INFO_SET_PRINT,
  SOC_PPC_PORT_INFO_SET_UNSAFE,
  SOC_PPC_PORT_INFO_SET_VERIFY,
  SOC_PPC_PORT_INFO_GET,
  SOC_PPC_PORT_INFO_GET_PRINT,
  SOC_PPC_PORT_INFO_GET_VERIFY,
  SOC_PPC_PORT_INFO_GET_UNSAFE,
  SOC_PPC_PORT_STP_STATE_SET,
  SOC_PPC_PORT_STP_STATE_SET_PRINT,
  SOC_PPC_PORT_STP_STATE_SET_UNSAFE,
  SOC_PPC_PORT_STP_STATE_SET_VERIFY,
  SOC_PPC_PORT_STP_STATE_GET,
  SOC_PPC_PORT_STP_STATE_GET_PRINT,
  SOC_PPC_PORT_STP_STATE_GET_VERIFY,
  SOC_PPC_PORT_STP_STATE_GET_UNSAFE,
  SOC_PPC_PORT_GET_PROCS_PTR,
  SOC_PPC_PORT_GET_ERRS_PTR,
  
   SOC_PPC_PORTS_REGS_INIT,
   SOC_PPC_PORTS_INIT,

  
  SOC_PPC_PORT_PROCEDURE_DESC_LAST
} SOC_PPC_PORT_PROCEDURE_DESC;

typedef enum
{
  
  SOC_PPC_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_PORT_FIRST,
  SOC_PPC_PORT_STP_STATE_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_SYS_PHY_PORT_ID_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_DIRECTION_NDX_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_LEARN_DEST_TYPE_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_PORT_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_TPID_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_PORT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_ORIENTATION_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_ETHER_TYPE_BASED_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_MTU_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_DA_NOT_FOUND_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_VT_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PPC_PORT_VSI_PROFILE_OUT_OF_RANGE_ERR,
  

  
  SOC_PPC_PORT_ERR_LAST
} SOC_PPC_PORT_ERR;











uint32
  arad_pp_port_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );



uint32
  arad_pp_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PORT_INFO                           *port_info
  );


uint32
  arad_pp_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PORT_INFO                           *port_info
  );

uint32
  arad_pp_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );


uint32
  arad_pp_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_PORT_INFO                           *port_info
  );


uint32
  arad_pp_port_stp_state_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_IN  SOC_PPC_PORT_STP_STATE                      stp_state
  );

uint32
  arad_pp_port_stp_state_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_IN  SOC_PPC_PORT_STP_STATE                      stp_state
  );

uint32
  arad_pp_port_stp_state_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx
  );


uint32
  arad_pp_port_stp_state_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_OUT SOC_PPC_PORT_STP_STATE                      *stp_state
  );
uint32
  SOC_PPC_PORT_INFO_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_PORT_INFO *info
  );

soc_error_t arad_pp_port_property_set(int unit, int core, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 value);
soc_error_t arad_pp_port_property_get(int unit, int core, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 *value);


soc_error_t arad_pp_port_additional_tpids_set(int unit, SOC_PPC_ADDITIONAL_TPID_VALUES *additional_tpids);


soc_error_t arad_pp_port_additional_tpids_get(int unit, SOC_PPC_ADDITIONAL_TPID_VALUES *additional_tpids);




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
