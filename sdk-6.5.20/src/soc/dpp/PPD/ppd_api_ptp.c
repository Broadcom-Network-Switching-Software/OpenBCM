/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PTP



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ptp.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_api_framework.h>

#include <shared/swstate/access/sw_state_access.h>






















uint32
  soc_ppd_ptp_init(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PTP_INIT);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_ptp_init_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_ppd_ptp_init()", 0, 0);
}



uint32
  soc_ppd_ptp_port_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PTP_PORT_INFO                  *info,
    SOC_SAND_IN  SOC_PPC_PTP_IN_PP_PORT_PROFILE         profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PTP_PORT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_ptp_port_set_verify(
          unit,
          local_port_ndx,
          info,
          profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_ptp_port_set_unsafe(
          unit,
          local_port_ndx,
          info,
          profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  
  res = sw_state_access[unit].dpp.soc.arad.pp.ptp.ptp_profile.set(unit, local_port_ndx, profile);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_ppd_ptp_port_set()", 0, 0);
}



uint32
  soc_ppd_ptp_port_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT SOC_PPC_PTP_IN_PP_PORT_PROFILE         *profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PTP_PORT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_NOF_LOCAL_PORTS(unit), ARAD_FAP_PORT_ID_INVALID_ERR, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.ptp.ptp_profile.get(unit, local_port_ndx, profile);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_ppd_ptp_port_get()", 0, 0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>



