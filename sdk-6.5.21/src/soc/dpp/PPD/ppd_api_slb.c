/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPD



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_slb.h>











#include <soc/dpp/PPD/ppd_api_slb.h>

#define ARAD_PP_DEVICE_CALL(func, params)            \
    do                                            \
    {                                             \
      res = arad_pp_##func params;                     \
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);      \
    } while(0)


#define PPD_API_SLB_INTERNAL_STD_IMPL(suffix, args) \
{ \
  uint32 res = SOC_SAND_OK; \
  SOC_SAND_INIT_ERROR_DEFINITIONS(0); \
  SOC_SAND_CHECK_DRIVER_AND_DEVICE; \
  ARAD_PP_DEVICE_CALL(api_impl_ ## suffix, args); \
  SOC_PPD_DO_NOTHING_AND_EXIT; \
exit: \
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0); \
}

#define PPD_API_SLB_INTERNAL_PREFIX(suffix) soc_ppd_ ## suffix
#define PPD_API_SLB_INTERNAL_FUNCTIONS_ONLY


#include <soc/dpp/PPD/ppd_api_slb.h>

#undef PPD_API_SLB_INTERNAL_FUNCTIONS_ONLY
#undef PPD_API_SLB_INTERNAL_PREFIX
#undef PPD_API_SLB_INTERNAL_STD_IMPL

#if SOC_PPC_DEBUG_IS_LVL1

#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>

