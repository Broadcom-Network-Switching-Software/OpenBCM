/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_SLB


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_slb.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_slb.h>

#ifdef BCM_88660_A0
#define PPD_API_SLB_INTERNAL_STD_IMPL(suffix, args) \
{ \
  uint32 res = SOC_SAND_OK; \
  SOC_SAND_INIT_ERROR_DEFINITIONS(0); \
  SOC_SAND_CHECK_DRIVER_AND_DEVICE; \
  res = arad_pp_api_impl_ ## suffix ## _verify args; \
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); \
  SOC_SAND_TAKE_DEVICE_SEMAPHORE; \
  res = arad_pp_api_impl_ ## suffix ## _unsafe args; \
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore); \
exit_semaphore: \
  SOC_SAND_GIVE_DEVICE_SEMAPHORE; \
  ARAD_PP_DO_NOTHING_AND_EXIT; \
exit: \
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0); \
}
#else 
#define PPD_API_SLB_INTERNAL_STD_IMPL(suffix, args) \
{ \
  uint32 res = SOC_SAND_OK; \
  SOC_SAND_INIT_ERROR_DEFINITIONS(0); \
  SOC_SAND_CHECK_DRIVER_AND_DEVICE; \
  SOC_SAND_SET_ERROR_CODE(0, 10, exit); \
  ARAD_PP_DO_NOTHING_AND_EXIT; \
exit: \
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0); \
}
#endif 

#define PPD_API_SLB_INTERNAL_PREFIX(suffix) arad_pp_api_impl_ ## suffix
#define PPD_API_SLB_INTERNAL_FUNCTIONS_ONLY


#include <soc/dpp/PPD/ppd_api_slb.h>

#undef PPD_API_SLB_INTERNAL_FUNCTIONS_ONLY
#undef PPD_API_SLB_INTERNAL_PREFIX
#undef PPD_API_SLB_INTERNAL_STD_IMPL


#include <soc/dpp/SAND/Utils/sand_footer.h>

