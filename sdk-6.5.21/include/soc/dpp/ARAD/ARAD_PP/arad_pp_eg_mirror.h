/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_EG_MIRROR_INCLUDED__

#define __ARAD_PP_EG_MIRROR_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_port.h>
#include <soc/dpp/PPC/ppc_api_eg_mirror.h>






#define ARAD_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES (7)









typedef enum
{
  
  ARAD_PP_EG_MIRROR_PORT_VLAN_ADD = ARAD_PP_PROC_DESC_BASE_EG_MIRROR_FIRST,
  ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_PRINT,
  ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE,
  ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_PRINT,
  ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_VLAN_GET,
  ARAD_PP_EG_MIRROR_PORT_VLAN_GET_PRINT,
  ARAD_PP_EG_MIRROR_PORT_VLAN_GET_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_VLAN_GET_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_DFLT_SET,
  ARAD_PP_EG_MIRROR_PORT_DFLT_SET_PRINT,
  ARAD_PP_EG_MIRROR_PORT_DFLT_SET_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_DFLT_SET_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_DFLT_GET,
  ARAD_PP_EG_MIRROR_PORT_APPL_SET,
  ARAD_PP_EG_MIRROR_PORT_APPL_GET,
  ARAD_PP_EG_MIRROR_PORT_APPL_SET_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_APPL_GET_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_DFLT_GET_PRINT,
  ARAD_PP_EG_MIRROR_PORT_DFLT_GET_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_DFLT_GET_UNSAFE,
  SOC_PPC_EG_MIRROR_PORT_INFO_GET,
  SOC_PPC_EG_MIRROR_PORT_INFO_GET_PRINT,
  SOC_PPC_EG_MIRROR_PORT_INFO_GET_VERIFY,
  SOC_PPC_EG_MIRROR_PORT_INFO_GET_UNSAFE,
  ARAD_PP_EG_MIRROR_GET_PROCS_PTR,
  ARAD_PP_EG_MIRROR_GET_ERRS_PTR,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET_UNSAFE,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET_VERIFY,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET_UNSAFE,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET_VERIFY,
  

  
  ARAD_PP_EG_MIRROR_PROCEDURE_DESC_LAST
} ARAD_PP_EG_MIRROR_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_EG_MIRROR_FIRST,
  ARAD_PP_EG_MIRROR_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_MIRROR_RECYCLE_COMMAND_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_MIRROR_TRAP_CODE_OUT_OF_RANGE_ERR,  
  

  
  ARAD_PP_EG_MIRROR_ERR_LAST
} ARAD_PP_EG_MIRROR_ERR;











uint32
  arad_pp_eg_mirror_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_eg_mirror_port_vlan_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                        pp_port,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success,
    SOC_SAND_IN dpp_outbound_mirror_config_t        *config

  );

uint32
  arad_pp_eg_mirror_port_vlan_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint8                                 enable_mirror

  );


uint32
  arad_pp_eg_mirror_port_vlan_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

uint32
  arad_pp_eg_mirror_port_vlan_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );


uint32
  arad_pp_eg_mirror_port_vlan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT dpp_outbound_mirror_config_t          *config
  );

uint32
  arad_pp_eg_mirror_port_vlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );


uint32
  arad_pp_eg_mirror_port_dflt_set_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_IN  SOC_PPC_PORT                       pp_port,
    SOC_SAND_IN dpp_outbound_mirror_config_t        *config

  );


uint32
  arad_pp_eg_mirror_port_appl_set_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  SOC_PPC_PORT   local_port, 
    SOC_SAND_IN  uint8         enable
  );

uint32
  arad_pp_eg_mirror_port_dflt_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );

uint32
  arad_pp_eg_mirror_port_dflt_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );


uint32
  arad_pp_eg_mirror_port_dflt_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT dpp_outbound_mirror_config_t        *config
  );


uint32
  arad_pp_eg_mirror_port_appl_get_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  SOC_PPC_PORT  local_port_ndx,
    SOC_SAND_OUT uint8         *is_enabled 
  );


uint32
  arad_pp_eg_mirror_recycle_command_trap_set_unsafe(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  uint32        recycle_command, 
    SOC_SAND_IN  uint32        trap_code, 
    SOC_SAND_IN  uint32        snoop_strength,
    SOC_SAND_IN  uint32        forward_strengh
  );

uint32
  arad_pp_eg_mirror_recycle_command_trap_set_verify(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  uint32        recycle_command, 
    SOC_SAND_IN  uint32        trap_code, 
    SOC_SAND_IN  uint32        snoop_strength,
    SOC_SAND_IN  uint32        forward_strengh
  );

uint32
  arad_pp_eg_mirror_recycle_command_trap_get_unsafe(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  uint32        recycle_command, 
    SOC_SAND_OUT  uint32       *trap_code, 
    SOC_SAND_OUT  uint32       *snoop_strength
  );

uint32
  arad_pp_eg_mirror_recycle_command_trap_get_verify(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  uint32        recycle_command 
  );

uint32
  arad_pp_eg_mirror_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );


uint32
  arad_pp_eg_mirror_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                        pp_port,
    SOC_SAND_OUT SOC_PPC_EG_MIRROR_PORT_INFO            *info
  );



uint32
  SOC_PPC_EG_MIRROR_PORT_DFLT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_MIRROR_PORT_DFLT_INFO *info
  );

uint32
  SOC_PPC_EG_MIRROR_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_MIRROR_PORT_INFO *info
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
