/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_API_DIAGNOSTICS_INCLUDED__

#define __ARAD_API_DIAGNOSTICS_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>





typedef SOC_TMC_DIAG_LAST_PACKET_INFO		ARAD_DIAG_LAST_PACKET_INFO;             
#define ARAD_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_ARAD

#if ARAD_DEBUG


uint32
  arad_diag_signals_dump(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  int      core,
    SOC_SAND_IN  uint32   flags
  );
  
#endif 


uint32
  arad_diag_last_packet_info_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_OUT ARAD_DIAG_LAST_PACKET_INFO *last_packet
  );


void
  ARAD_DIAG_LAST_PACKET_INFO_clear(
    SOC_SAND_OUT ARAD_DIAG_LAST_PACKET_INFO *info
  );


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


