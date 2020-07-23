#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_mgmt.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/mbcm.h>







#define ARAD_PA_FMC_RATE_DELTA_CONST(is_for_ips)               (102) 

  
#define ARAD_FMC_RATE_DELTA_CONST(is_for_ips)             ((is_for_ips == TRUE)? ARAD_PA_FMC_RATE_DELTA_CONST(is_for_ips) : 7)











typedef SOC_SAND_RET (*ARAD_SEND_MESSAGE_TO_QUEUE_FUNC)(SOC_SAND_IN  uint32 msg);













uint8
  arad_is_multicast_id_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multicast_id
  )
{
  if (multicast_id <= ARAD_MAX_MC_ID(unit))
  {
    return TRUE;
  }
  return FALSE;
}

uint8
  arad_is_queue_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                queue
  )
{

  
  if (queue < SOC_DPP_DEFS_GET(unit, nof_queues))
  {
    return TRUE;
  }
  return FALSE;
}

uint8
  arad_is_flow_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                flow
  )
{
  uint8
    is_in_range;

  is_in_range = flow < (SOC_DPP_DEFS_GET(unit, nof_flows));

  return is_in_range;
}


uint32
  arad_intern_rate2clock(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  rate_kbps,
    SOC_SAND_IN  uint8 is_for_ips,
    SOC_SAND_OUT uint32  *clk_interval
  )
{
  uint32
    res;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    cr_size = 0,
    ticks_per_sec = 0,
    interval = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INTERN_RATE2CLOCK);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
    res,10,exit,ARAD_GET_ERR_TEXT_001,MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &cr_size))) ;

  ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

  if (rate_kbps == 0)
  {
    *clk_interval = 0;
  }
  else
  {
    ret = soc_sand_kbits_per_sec_to_clocks(
            rate_kbps,
            cr_size  * 8 ,
            ticks_per_sec,
            &interval
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

    *clk_interval = interval - ARAD_FMC_RATE_DELTA_CONST(is_for_ips);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_intern_rate2clock()",0,0);
}


uint32
  arad_intern_clock2rate(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  clk_interval,
    SOC_SAND_IN  uint8 is_for_ips,
    SOC_SAND_OUT uint32  *rate_kbps
  )
{
  uint32
    res;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    cr_size = 0,
    ticks_per_sec = 0,
    interval = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INTERN_CLOCK2RATE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
    res,15,exit,ARAD_GET_ERR_TEXT_001,MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &cr_size))) ;

  ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

  if (clk_interval == 0)
  {
    *rate_kbps = 0;
  }
  else
  {
    interval = (clk_interval + ARAD_FMC_RATE_DELTA_CONST(is_for_ips));

    ret = soc_sand_clocks_to_kbits_per_sec(
            interval,
            cr_size * 8 ,
            ticks_per_sec,
            rate_kbps
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_intern_clock2rate()",0,0);
}





#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
