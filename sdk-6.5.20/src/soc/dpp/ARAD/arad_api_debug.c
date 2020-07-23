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

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_api_debug.h>
#include <soc/dpp/ARAD/arad_debug.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_api_general.h>











#define SOC_SAND_TAKE_ALL_DMA_LOCKS(unit) \
    if(SOC_CONTROL(unit)->tableDmaTimeout){ \
        TABLE_DMA_LOCK(unit); \
        TSLAM_DMA_LOCK(unit); \
        SOC_SBUSDMA_DM_LOCK(unit); \
    }

#define SOC_SAND_GIVE_ALL_DMA_LOCKS(unit) \
    if(SOC_CONTROL(unit)->tableDmaTimeout){ \
        SOC_SBUSDMA_DM_UNLOCK(unit); \
        TSLAM_DMA_UNLOCK(unit); \
        TABLE_DMA_UNLOCK(unit); \
    }
















uint32
  arad_dbg_autocredit_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_AUTOCREDIT_INFO *info,
    SOC_SAND_OUT uint32                  *exact_rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_AUTOCREDIT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_rate);

  res = arad_dbg_autocredit_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_dbg_autocredit_set_unsafe(
          unit,
          info,
          exact_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_autocredit_set()",0,0);
}



uint32
  arad_dbg_autocredit_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_DBG_AUTOCREDIT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_AUTOCREDIT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_dbg_autocredit_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_autocredit_get()",0,0);
}


uint32
  arad_dbg_ingr_reset(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_INGR_RESET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_dbg_ingr_reset_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_ingr_reset()",0,0);
}


uint32
  arad_dbg_dev_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_RST_DOMAIN      rst_domain
  )
{
  uint32 res = SOC_SAND_OK;
  soc_control_t *soc = SOC_CONTROL(unit);

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_DEV_RESET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_dbg_dev_reset_verify(
          rst_domain
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

#ifdef BCM_SBUSDMA_SUPPORT
  SOC_SAND_TAKE_ALL_DMA_LOCKS(unit);
#endif

  res = arad_dbg_dev_reset_unsafe(
          unit,
          rst_domain
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
#ifdef BCM_SBUSDMA_SUPPORT
  SOC_SAND_GIVE_ALL_DMA_LOCKS(unit);
#endif
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_dbg_dev_reset()", 0, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

