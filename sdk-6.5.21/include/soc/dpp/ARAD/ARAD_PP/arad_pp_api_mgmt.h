
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_API_MGMT_INCLUDED__

#define __ARAD_PP_API_MGMT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mgmt.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>


























uint32
  arad_pp_mgmt_operation_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode
  );


uint32
  arad_pp_mgmt_operation_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_PP_MGMT_OPERATION_MODE *op_mode
  );


uint32
  arad_pp_mgmt_device_close(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_pp_mgmt_elk_mode_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  );


uint32
  arad_pp_mgmt_elk_mode_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT ARAD_PP_MGMT_ELK_MODE *elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  );


uint32
  arad_pp_mgmt_use_elk_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  );


uint32
  arad_pp_mgmt_use_elk_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT ARAD_PP_MGMT_LKP_TYPE *lkp_type,
    SOC_SAND_OUT uint8           *use_elk
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


