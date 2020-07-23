/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_PTP_INCLUDED__

#define __ARAD_PP_PTP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_ptp.h>
#include <soc/dpp/dpp_config_defs.h>





#define ARAD_PP_PTP_UDP_ENCAP_PORT1 319
#define ARAD_PP_PTP_UDP_ENCAP_PORT2 320
#define ARAD_PP_BFD_SINGLE_HOP_UDP_PORT 3784
#define ARAD_PP_BFD_MICRO_UDP_PORT 6784
#define ARAD_PP_BFD_MULTI_HOP_UDP_PORT 4784
#define ARAD_PP_GTP_U_UDP_PORT 2152
#define ARAD_PP_GTP_C_UDP_PORT 2123
#define	ARAD_PP_L2TP_ENCAP_PORT			1701








typedef enum
{
  
  ARAD_PP_PTP_INIT = ARAD_PP_PROC_DESC_BASE_PTP_FIRST,
  ARAD_PP_PTP_INIT_UNSAFE,
  ARAD_PP_PTP_PORT_SET,
  ARAD_PP_PTP_PORT_SET_UNSAFE,
  ARAD_PP_PTP_PORT_GET,
  ARAD_PP_PTP_PORT_GET_UNSAFE,
  ARAD_PP_PTP_PORT_SET_ACTION_PROFILE,
  
   
  ARAD_PP_PTP_PORT_SET_VERIFY,

  
  ARAD_PP_PTP_PROCEDURE_DESC_LAST
} ARAD_PP_PTP_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_PTP_ACTION_PROFILE_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_PTP_FIRST,

  

  ARAD_PP_PTP_USER_TRAP_ID_OUT_OF_RANGE_ERR,
  
  
  ARAD_PP_PTP_ERR_LAST
} ARAD_PP_PTP_ERR;

typedef struct {
    SOC_PPC_PTP_IN_PP_PORT_PROFILE ptp_profile[SOC_DPP_DEFS_MAX(NOF_LOCAL_PORTS)];
} ARAD_PP_PTP_SW_STATE;










uint32
  arad_pp_ptp_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_ptp_port_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                        local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PTP_PORT_INFO               *info,
    SOC_SAND_IN  SOC_PPC_PTP_IN_PP_PORT_PROFILE      profile
  );


uint32
  arad_pp_ptp_port_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PTP_PORT_INFO                  *info,
    SOC_SAND_IN  SOC_PPC_PTP_IN_PP_PORT_PROFILE         profile
  );


uint32
  arad_pp_ptp_port_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT SOC_PPC_PTP_IN_PP_PORT_PROFILE         *profile
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
 arad_pp_ptp_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
arad_pp_ptp_get_errs_ptr(void);


soc_error_t
  arad_pp_ptp_p2p_delay_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_PORT                   local_port_ndx,
    SOC_SAND_IN  int                            value
  );


soc_error_t
  arad_pp_ptp_p2p_delay_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_PORT                   local_port_ndx,
    SOC_SAND_OUT int*                           value
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


