#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_CHIP


#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>



























uint32
  arad_chip_defines_init(int unit)
{
    uint8 is_allocated;
    soc_error_t rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_INIT);


    if (!SOC_WARM_BOOT(unit)) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.chip_definitions.is_allocated(unit, &is_allocated);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 5, exit);
        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.chip_definitions.alloc(unit);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
        }

        rv = sw_state_access[unit].dpp.soc.arad.tm.chip_definitions.ticks_per_sec.set(unit, ARAD_DFLT_TICKS_PER_SEC);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in chip_defines_initialize()",0,0);
}






uint32
  arad_chip_time_to_ticks(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        time_value,
    SOC_SAND_IN  uint8       is_nano,
    SOC_SAND_IN  uint32       result_granularity,
    SOC_SAND_IN  uint8       is_round_up,
    SOC_SAND_OUT uint32        *result
  )
{
  uint32
    granularity,
    ticks,
    kilo_ticks_per_sec,
    reminder;
  SOC_SAND_U64
    val1,
    val2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CHIP_TIME_TO_TICKS);
  SOC_SAND_CHECK_NULL_INPUT(result);

  kilo_ticks_per_sec = arad_chip_kilo_ticks_per_sec_get(unit);
  
  granularity = (is_nano)?(result_granularity*1000000):(result_granularity*1000);

  if (result_granularity == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_DIV_BY_ZERO_ERR, 10, exit);
  }

  soc_sand_u64_multiply_longs(time_value, kilo_ticks_per_sec, &val1);
  reminder = soc_sand_u64_devide_u64_long(&val1, granularity, &val2);

  if (val2.arr[1] != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 12, exit);
  }

  ticks = val2.arr[0];

  if ((is_round_up) && (reminder != 0))
  {
    ticks += 1;
  }

  *result = ticks;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_chip_time_to_ticks()",0,0);
}


uint32
  arad_ticks_to_time(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        ticks_value,
    SOC_SAND_IN  uint8       is_nano,
    SOC_SAND_IN  uint32       result_granularity,
    SOC_SAND_OUT uint32        *result
  )
{
  uint32
    kilo_ticks_per_sec;
  uint32
    granularity;
  SOC_SAND_U64
    val1,
    val2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CHIP_TICKS_TO_TIME);
  SOC_SAND_CHECK_NULL_INPUT(result);

  
  kilo_ticks_per_sec = arad_chip_kilo_ticks_per_sec_get(unit);

  granularity = (is_nano)?(result_granularity * 1000*1000):result_granularity*1000;

  soc_sand_u64_multiply_longs(ticks_value, granularity, &val1);
  soc_sand_u64_devide_u64_long(&val1, kilo_ticks_per_sec, &val2);

  if (val2.arr[1] != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 12, exit);
  }

  *result =  val2.arr[0];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ticks_to_time()",0,0);
}

uint32
  arad_chip_ticks_per_sec_get(
    SOC_SAND_IN int unit
  )
{
    uint32 val;
    int rv;

    rv = sw_state_access[unit].dpp.soc.arad.tm.chip_definitions.ticks_per_sec.get(unit, &val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error at arad_chip_ticks_per_sec_get")));
    }

    return val;
}

void
  arad_chip_kilo_ticks_per_sec_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  clck_freq_khz
  )
{
    int rv;

    rv = sw_state_access[unit].dpp.soc.arad.tm.chip_definitions.ticks_per_sec.set(unit, clck_freq_khz * 1000);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error at arad_chip_ticks_per_sec_set")));
    }
}

uint32
  arad_chip_kilo_ticks_per_sec_get(
    SOC_SAND_IN int unit
  )
{
  uint32
    tps;

  tps = arad_chip_ticks_per_sec_get(unit);
  return SOC_SAND_DIV_ROUND_DOWN(tps, 1000);
}

uint32
  arad_chip_mega_ticks_per_sec_get(
    SOC_SAND_IN int unit
  )
{
  uint32
    tps;

  tps = arad_chip_ticks_per_sec_get(unit);
  return SOC_SAND_DIV_ROUND_DOWN(tps, (1000*1000));
}

 




void
  arad_ARAD_REG_INFO_clear(
    SOC_SAND_OUT ARAD_REG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_REG_INFO));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
