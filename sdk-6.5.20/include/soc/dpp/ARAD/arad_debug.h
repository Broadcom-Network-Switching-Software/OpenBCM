/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_DEBUG_INCLUDED__

#define __ARAD_DEBUG_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_debug.h>



























uint32
  arad_dbg_autocredit_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_AUTOCREDIT_INFO *info,
    SOC_SAND_OUT uint32                  *exact_rate
  );



uint32
  arad_dbg_autocredit_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_AUTOCREDIT_INFO *info
  );



uint32
  arad_dbg_autocredit_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_DBG_AUTOCREDIT_INFO *info
  );



uint32
  arad_dbg_egress_shaping_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );



uint32
  arad_dbg_egress_shaping_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  );



uint32
  arad_dbg_flow_control_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );



uint32
  arad_dbg_flow_control_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  );


uint32
  arad_dbg_ingr_reset_unsafe(
    SOC_SAND_IN  int                 unit
  );

 
uint32
  arad_dbg_dev_reset_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_RST_DOMAIN      rst_domain
  );

uint32
  arad_dbg_dev_reset_verify(
    SOC_SAND_IN  ARAD_DBG_RST_DOMAIN      rst_domain
  );
  

uint32
  arad_dbg_eci_access_tst_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 nof_k_iters,
    SOC_SAND_IN  uint8                 use_dflt_tst_reg,
    SOC_SAND_IN  uint32                  reg1_addr_longs,
    SOC_SAND_IN  uint32                  reg2_addr_longs,
    SOC_SAND_OUT uint8                 *is_valid
  );


uint32
  arad_dbg_sch_reset_unsafe(
    SOC_SAND_IN  int unit
  );
#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif
