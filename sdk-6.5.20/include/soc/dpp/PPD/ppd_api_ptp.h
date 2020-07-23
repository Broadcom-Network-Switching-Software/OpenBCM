/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_PTP_INCLUDED__

#define __SOC_PPD_API_PTP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPC/ppc_api_ptp.h>














typedef enum
{
  
  

  SOC_PPD_PTP_INIT = SOC_PPD_PROC_DESC_BASE_PTP_FIRST,
  SOC_PPD_PTP_PORT_SET,
  SOC_PPD_PTP_PORT_GET,
  
  SOC_PPD_PTP_PROCEDURE_DESC_LAST

} SOC_PPD_PTP_PROCEDURE_DESC;












uint32
  soc_ppd_ptp_init(
    SOC_SAND_IN  int                   unit
  );


uint32
  soc_ppd_ptp_port_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PTP_PORT_INFO                  *info,
    SOC_SAND_IN  SOC_PPC_PTP_IN_PP_PORT_PROFILE        profile
  );


uint32
  soc_ppd_ptp_port_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT SOC_PPC_PTP_IN_PP_PORT_PROFILE         *profile
  );


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
