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

#define _ERR_MSG_MODULE_NAME BSL_SOC_CNT



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_cnt.h>
#include <soc/dpp/ARAD/arad_cnt.h>






















uint32
  arad_cnt_dma_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   dma_ndx,
	SOC_SAND_IN  uint8                 channel,
	SOC_SAND_IN  int                   cache_length
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DMA_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = arad_cnt_dma_set_verify(
          unit,
          dma_ndx,
		  channel,
          cache_length
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_cnt_dma_set_unsafe(
          unit,
          dma_ndx,
		  channel,
          cache_length
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_dma_set()", 0, 0);
}


uint32
  arad_cnt_dma_unset(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   dma_ndx,
	SOC_SAND_IN  uint8                 channel
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DMA_UNSET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = arad_cnt_dma_unset_verify(
          unit,
          dma_ndx,
		  channel
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_cnt_dma_unset_unsafe(
          unit,
          dma_ndx,
		  channel
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_dma_unset()", 0, 0);
}

uint32
  arad_cnt_counters_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_COUNTERS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_cnt_counters_set_verify(
          unit,
          processor_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_cnt_counters_set_unsafe(
          unit,
          processor_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_counters_set()", 0, 0);
}


uint32
  arad_cnt_counters_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT ARAD_CNT_COUNTERS_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_COUNTERS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_cnt_counters_get_verify(
          unit,
          processor_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_cnt_counters_get_unsafe(
          unit,
          processor_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_counters_get()", 0, 0);
}


uint32
  arad_cnt_status_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT ARAD_CNT_STATUS               *proc_status
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(proc_status);

  res = arad_cnt_status_get_verify(
          unit,
          processor_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_cnt_status_get_unsafe(
          unit,
          processor_ndx,
          proc_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_status_get()", 0, 0);
}


uint32
  arad_cnt_algorithmic_read(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         dma_ndx,
	SOC_SAND_IN  uint8                         channel,
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO     *counter_info_array,
    SOC_SAND_OUT ARAD_CNT_RESULT_ARR           *result_arr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_ALGORITHMIC_READ);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(result_arr);
  SOC_SAND_CHECK_NULL_INPUT(counter_info_array);

  res = arad_cnt_algorithmic_read_verify(unit, dma_ndx, channel);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit_semaphore);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_cnt_algorithmic_read_unsafe(
          unit,
          dma_ndx,
		  channel,
          counter_info_array,
          result_arr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_algorithmic_read()", 0, 0);
}


uint32
  arad_cnt_direct_read(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO         *counter_info,
    SOC_SAND_IN  uint32                    counter_ndx,
    SOC_SAND_OUT ARAD_CNT_RESULT               *read_rslt
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DIRECT_READ);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(read_rslt);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_cnt_direct_read_unsafe(
          unit,
          processor_ndx,
          counter_info,
          counter_ndx,
          read_rslt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_direct_read()", 0, counter_ndx);
}


uint32
  arad_cnt_q2cnt_id(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT uint32                   *counter_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_Q2CNT_ID);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(counter_ndx);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_cnt_q2cnt_id_unsafe(
          unit,
          processor_ndx,
          queue_ndx,
          counter_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_q2cnt_id()", queue_ndx, *counter_ndx);
}



uint32
  arad_cnt_meter_hdr_compensation_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_IN  int                        counter_adjust_type,
    SOC_SAND_IN  int32                    hdr_compensation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_METER_HDR_COMPENSATION_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_cnt_meter_hdr_compensation_set_verify(
          unit,
          port_ndx,
          direction,
          hdr_compensation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_cnt_meter_hdr_compensation_set_unsafe(
          unit,
          core_id,
          port_ndx,
          direction,
          counter_adjust_type,
          hdr_compensation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_meter_hdr_compensation_set()", port_ndx, 0);
}


uint32
  arad_cnt_meter_hdr_compensation_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_IN  int                        counter_adjust_type,
    SOC_SAND_OUT int32                    *hdr_compensation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_METER_HDR_COMPENSATION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(hdr_compensation);

  res = arad_cnt_meter_hdr_compensation_get_verify(
          unit,
          port_ndx,
          direction
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_cnt_meter_hdr_compensation_get_unsafe(
          unit,
          core_id,
          port_ndx,
          direction,
          counter_adjust_type,
          hdr_compensation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_meter_hdr_compensation_get()", port_ndx, 0);
}

void
  ARAD_CNT_COUNTERS_INFO_clear(
     SOC_SAND_IN  int                    unit,
     SOC_SAND_OUT ARAD_CNT_COUNTERS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNT_COUNTERS_INFO_clear(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_CNT_OVERFLOW_clear(
    SOC_SAND_OUT ARAD_CNT_OVERFLOW *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNT_OVERFLOW_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_CNT_STATUS_clear(
    SOC_SAND_OUT ARAD_CNT_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNT_STATUS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_CNT_RESULT_clear(
    SOC_SAND_OUT ARAD_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNT_RESULT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_CNT_RESULT_ARR_clear(
    SOC_SAND_OUT ARAD_CNT_RESULT_ARR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNT_RESULT_ARR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_CNT_PROCESSOR_ID_to_string(
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID enum_val
  )
{
  return SOC_TMC_CNT_PROCESSOR_ID_to_string(enum_val);
}

const char*
  ARAD_CNT_MODE_STATISTICS_to_string(
    SOC_SAND_IN  ARAD_CNT_MODE_STATISTICS enum_val
  )
{
  return SOC_TMC_CNT_MODE_STATISTICS_to_string(enum_val);
}

const char*
  ARAD_CNT_MODE_EG_to_string(
    SOC_SAND_IN  ARAD_CNT_MODE_EG enum_val
  )
{
  return SOC_TMC_CNT_MODE_EG_to_string(enum_val.resolution);
}

const char*
  ARAD_CNT_SRC_TYPE_to_string(
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE enum_val
  )
{
  return SOC_TMC_CNT_SRC_TYPE_to_string(enum_val);
}

const char*
  ARAD_CNT_Q_SET_SIZE_to_string(
    SOC_SAND_IN  ARAD_CNT_Q_SET_SIZE enum_val
  )
{
  return SOC_TMC_CNT_Q_SET_SIZE_to_string(enum_val);
}

void
  ARAD_CNT_MODE_EG_print(
    SOC_SAND_IN  ARAD_CNT_MODE_EG *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_TMC_CNT_MODE_EG_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  ARAD_CNT_COUNTERS_INFO_print(
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNT_COUNTERS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_CNT_OVERFLOW_print(
    SOC_SAND_IN  ARAD_CNT_OVERFLOW *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNT_OVERFLOW_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_CNT_STATUS_print(
    SOC_SAND_IN  ARAD_CNT_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNT_STATUS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_CNT_RESULT_print(
    SOC_SAND_IN  ARAD_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNT_RESULT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_CNT_RESULT_ARR_print(
    SOC_SAND_IN  ARAD_CNT_RESULT_ARR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNT_RESULT_ARR_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

