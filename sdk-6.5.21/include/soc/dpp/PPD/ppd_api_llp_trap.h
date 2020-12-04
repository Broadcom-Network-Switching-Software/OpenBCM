/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LLP_TRAP_INCLUDED__

#define __SOC_PPD_API_LLP_TRAP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_llp_trap.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_LLP_TRAP_PORT_INFO_SET = SOC_PPD_PROC_DESC_BASE_LLP_TRAP_FIRST,
  SOC_PPD_LLP_TRAP_PORT_INFO_SET_PRINT,
  SOC_PPD_LLP_TRAP_PORT_INFO_GET,
  SOC_PPD_LLP_TRAP_PORT_INFO_GET_PRINT,
  SOC_PPD_LLP_TRAP_ARP_INFO_SET,
  SOC_PPD_LLP_TRAP_ARP_INFO_SET_PRINT,
  SOC_PPD_LLP_TRAP_ARP_INFO_GET,
  SOC_PPD_LLP_TRAP_ARP_INFO_GET_PRINT,
  SOC_PPD_LLP_TRAP_RESERVED_MC_INFO_SET,
  SOC_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_PRINT,
  SOC_PPD_LLP_TRAP_RESERVED_MC_INFO_GET,
  SOC_PPD_LLP_TRAP_RESERVED_MC_INFO_GET_PRINT,
  SOC_PPD_LLP_TRAP_PROG_TRAP_INFO_SET,
  SOC_PPD_LLP_TRAP_PROG_TRAP_INFO_SET_PRINT,
  SOC_PPD_LLP_TRAP_PROG_TRAP_INFO_GET,
  SOC_PPD_LLP_TRAP_PROG_TRAP_INFO_GET_PRINT,
  SOC_PPD_LLP_TRAP_GET_PROCS_PTR,
  
  
  SOC_PPD_LLP_TRAP_PROCEDURE_DESC_LAST
} SOC_PPD_LLP_TRAP_PROCEDURE_DESC;










uint32
  soc_ppd_llp_trap_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_INFO                  *port_info
  );


uint32
  soc_ppd_llp_trap_port_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PORT_INFO                  *port_info
  );


uint32
  soc_ppd_llp_trap_arp_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_ARP_INFO                   *arp_info
  );


uint32
  soc_ppd_llp_trap_arp_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_ARP_INFO                   *arp_info
  );


uint32
  soc_ppd_llp_trap_reserved_mc_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                      *trap_action
  );


uint32
  soc_ppd_llp_trap_reserved_mc_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE                      *trap_action
  );


uint32
  soc_ppd_llp_trap_prog_trap_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                prog_trap_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual
  );


uint32
  soc_ppd_llp_trap_prog_trap_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                prog_trap_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

