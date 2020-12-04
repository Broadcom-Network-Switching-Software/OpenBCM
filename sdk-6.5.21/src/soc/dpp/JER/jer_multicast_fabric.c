#include <soc/mcm/memregs.h>
#if defined(BCM_88675_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_multicast_fabric.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/port_sw_db.h>

























uint32
  jer_mult_fabric_enhanced_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                  *queue_range
  )
{
  int
    res;
  uint64
    reg_value;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(queue_range);
  COMPILER_64_SET(reg_value, 0, 0);
  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
      LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Core %d out of range\n"), core_id));
      SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (queue_range->start > (SOC_DPP_DEFS_GET(unit, nof_queues) - 1)) {
    LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Queue start %d out of range\n"), queue_range->start));
    SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (queue_range->end > (SOC_DPP_DEFS_GET(unit, nof_queues) - 1)) {
    LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Queue end %d out of range\n"), queue_range->end));
    SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }

  if (queue_range->start > queue_range->end) {
      LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Queue start %d is higher than Queue end %d\n"), queue_range->start, queue_range->end));
      SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }

  if (SOC_IS_QAX(unit)) { 
          res = READ_IPS_FMC_QNUM_RANGEr(unit, SOC_CORE_ALL, &reg_value);
          SOCDNX_IF_ERR_EXIT(res);

          soc_reg64_field32_set(unit, IPS_FMC_QNUM_RANGEr, &reg_value, FMC_QNUM_LOWf, queue_range->start);
          soc_reg64_field32_set(unit, IPS_FMC_QNUM_RANGEr, &reg_value, FMC_QNUM_HIGHf, queue_range->end);

          res = WRITE_IPS_FMC_QNUM_RANGEr(unit, SOC_CORE_ALL, reg_value);
          SOCDNX_IF_ERR_EXIT(res);
  } else {

      if (core_id == 0 || core_id == SOC_CORE_ALL) {
          res = READ_ECI_GLOBAL_FMC_0r(unit, &reg_value);
          SOCDNX_IF_ERR_EXIT(res);

          soc_reg64_field32_set(unit, ECI_GLOBAL_FMC_0r, &reg_value, FMC_QNUM_LOW_0f, queue_range->start);
          soc_reg64_field32_set(unit, ECI_GLOBAL_FMC_0r, &reg_value, FMC_QNUM_HIGH_0f, queue_range->end);

          res = WRITE_ECI_GLOBAL_FMC_0r(unit, reg_value);
          SOCDNX_IF_ERR_EXIT(res);
      }
      if (core_id == 1 || core_id == SOC_CORE_ALL) {
          res = READ_ECI_GLOBAL_FMC_1r(unit, &reg_value);
          SOCDNX_IF_ERR_EXIT(res);

          soc_reg64_field32_set(unit, ECI_GLOBAL_FMC_1r, &reg_value, FMC_QNUM_LOW_1f, queue_range->start);
          soc_reg64_field32_set(unit, ECI_GLOBAL_FMC_1r, &reg_value, FMC_QNUM_HIGH_1f, queue_range->end);

          res = WRITE_ECI_GLOBAL_FMC_1r(unit, reg_value);
          SOCDNX_IF_ERR_EXIT(res);
      }
  }

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  jer_mult_fabric_enhanced_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_INOUT SOC_SAND_U32_RANGE                *queue_range
  )
{
  int
    res;
  uint64
    reg_value;
  
  SOCDNX_INIT_FUNC_DEFS;
  COMPILER_64_SET(reg_value, 0, 0);
  SOCDNX_NULL_CHECK(queue_range);
  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
      LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Core %d out of range\n"), core_id));
      SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }


  if (SOC_IS_QAX(unit)) { 
      res = READ_IPS_FMC_QNUM_RANGEr(unit, SOC_CORE_ALL, &reg_value);
      SOCDNX_IF_ERR_EXIT(res);

      queue_range->start = soc_reg64_field_get(unit, IPS_FMC_QNUM_RANGEr, reg_value, FMC_QNUM_LOWf);
      queue_range->end = soc_reg64_field_get(unit, IPS_FMC_QNUM_RANGEr, reg_value, FMC_QNUM_HIGHf);
  } else {
      if (core_id == 0 || core_id == SOC_CORE_ALL) {
          res = READ_ECI_GLOBAL_FMC_0r(unit, &reg_value);
          SOCDNX_IF_ERR_EXIT(res);

          queue_range->start = soc_reg64_field32_get(unit, ECI_GLOBAL_FMC_0r, reg_value, FMC_QNUM_LOW_0f);
          queue_range->end = soc_reg64_field32_get(unit, ECI_GLOBAL_FMC_0r, reg_value, FMC_QNUM_HIGH_0f);
      } else if (core_id == 1) {
          res = READ_ECI_GLOBAL_FMC_1r(unit, &reg_value);
          SOCDNX_IF_ERR_EXIT(res);

          queue_range->start = soc_reg64_field32_get(unit, ECI_GLOBAL_FMC_1r, reg_value, FMC_QNUM_LOW_1f);
          queue_range->end = soc_reg64_field32_get(unit, ECI_GLOBAL_FMC_1r, reg_value, FMC_QNUM_HIGH_1f);
      } 
  }

exit:
  SOCDNX_FUNC_RETURN;
}
#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
