/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_DIAGNOSTICS_INCLUDED__

#define __ARAD_DIAGNOSTICS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_general.h>

#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>


























uint32
  arad_diag_last_packet_info_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_OUT ARAD_DIAG_LAST_PACKET_INFO *last_packet
  );

uint32
  arad_diag_last_packet_info_get_verify(
    SOC_SAND_IN  int                unit
  );



uint32
  arad_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint8                               enable
  );


uint32
  arad_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT uint8                                *enable
  );

#if ARAD_DEBUG


uint32
  arad_diag_signals_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  int      core,
    SOC_SAND_IN  uint32   flags
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



