/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LLP_TRAP_INCLUDED__

#define __ARAD_PP_LLP_TRAP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_llp_trap.h>





#define ARAD_PP_LLP_TRAP_NOF_UD_L3_PROTOCOLS (7)









typedef enum
{
  
  SOC_PPC_LLP_TRAP_PORT_INFO_SET = ARAD_PP_PROC_DESC_BASE_LLP_TRAP_FIRST,
  SOC_PPC_LLP_TRAP_PORT_INFO_SET_PRINT,
  SOC_PPC_LLP_TRAP_PORT_INFO_SET_UNSAFE,
  SOC_PPC_LLP_TRAP_PORT_INFO_SET_VERIFY,
  SOC_PPC_LLP_TRAP_PORT_INFO_GET,
  SOC_PPC_LLP_TRAP_PORT_INFO_GET_PRINT,
  SOC_PPC_LLP_TRAP_PORT_INFO_GET_VERIFY,
  SOC_PPC_LLP_TRAP_PORT_INFO_GET_UNSAFE,
  SOC_PPC_LLP_TRAP_ARP_INFO_SET,
  SOC_PPC_LLP_TRAP_ARP_INFO_SET_PRINT,
  SOC_PPC_LLP_TRAP_ARP_INFO_SET_UNSAFE,
  SOC_PPC_LLP_TRAP_ARP_INFO_SET_VERIFY,
  SOC_PPC_LLP_TRAP_ARP_INFO_GET,
  SOC_PPC_LLP_TRAP_ARP_INFO_GET_PRINT,
  SOC_PPC_LLP_TRAP_ARP_INFO_GET_VERIFY,
  SOC_PPC_LLP_TRAP_ARP_INFO_GET_UNSAFE,
  ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET,
  ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET_PRINT,
  ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET_UNSAFE,
  ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET_VERIFY,
  ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET,
  ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET_PRINT,
  ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET_VERIFY,
  ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET_UNSAFE,
  ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET,
  ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET_PRINT,
  ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET_UNSAFE,
  ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET_VERIFY,
  ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET,
  ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET_PRINT,
  ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET_VERIFY,
  ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET_UNSAFE,
  ARAD_PP_LLP_TRAP_GET_PROCS_PTR,
  ARAD_PP_LLP_TRAP_GET_ERRS_PTR,
  

  ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_L3_PRTCL_PROCCESS,

  
  ARAD_PP_LLP_TRAP_PROCEDURE_DESC_LAST
} ARAD_PP_LLP_TRAP_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LLP_TRAP_PROG_TRAP_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LLP_TRAP_FIRST,
  ARAD_PP_LLP_TRAP_RESERVED_MC_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_DEST_MAC_NOF_BITS_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_SUB_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_SUB_TYPE_BITMAP_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_IP_PROTOCOL_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_SRC_PORT_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_SRC_PORT_BITMAP_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_DEST_PORT_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_DEST_PORT_BITMAP_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_ENABLE_BITMAP_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_INVERSE_BITMAP_OUT_OF_RANGE_ERR,
  

  ARAD_PP_ACTION_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_ACTION_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_L3_PROTOCOL_EXCEEDS_CAPACITY_ERR,
  ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_TO_NDX_MISMATCH_ERR,
  ARAD_PP_LLP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_TRAP_FAILED_TO_ALLOCATE_ETHER_TYPE_ERR,

  
  ARAD_PP_LLP_TRAP_ERR_LAST
} ARAD_PP_LLP_TRAP_ERR;









uint32
  arad_pp_llp_trap_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_llp_trap_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_INFO                  *port_info
  );

uint32
  arad_pp_llp_trap_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_INFO                  *port_info
  );

uint32
  arad_pp_llp_trap_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );


uint32
  arad_pp_llp_trap_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PORT_INFO                  *port_info
  );


uint32
  arad_pp_llp_trap_arp_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_ARP_INFO                   *arp_info
  );

uint32
  arad_pp_llp_trap_arp_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_ARP_INFO                   *arp_info
  );

uint32
  arad_pp_llp_trap_arp_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_llp_trap_arp_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_ARP_INFO                   *arp_info
  );


uint32
  arad_pp_llp_trap_reserved_mc_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                      *trap_action
  );

uint32
  arad_pp_llp_trap_reserved_mc_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                      *trap_action
  );

uint32
  arad_pp_llp_trap_reserved_mc_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key
  );


uint32
  arad_pp_llp_trap_reserved_mc_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE                      *trap_action
  );


uint32
  arad_pp_llp_trap_prog_trap_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  prog_trap_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual
  );

uint32
  arad_pp_llp_trap_prog_trap_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  prog_trap_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual
  );

uint32
  arad_pp_llp_trap_prog_trap_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  prog_trap_ndx
  );


uint32
  arad_pp_llp_trap_prog_trap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  prog_trap_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_trap_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_trap_get_errs_ptr(void);

uint32
  SOC_PPC_LLP_TRAP_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_INFO *info
  );

uint32
  SOC_PPC_LLP_TRAP_ARP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_ARP_INFO *info
  );

uint32
  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY *info
  );

uint32
  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO *info
  );

uint32
  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO *info
  );

uint32
  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO *info
  );

uint32
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

