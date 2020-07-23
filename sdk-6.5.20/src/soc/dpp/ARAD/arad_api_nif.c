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

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_nif.h>

#include <soc/dpp/ARAD/arad_api_ports.h>

#include <soc/dpp/ARAD/arad_general.h>


























ARAD_NIF_TYPE
  arad_nif_id2type(
    SOC_SAND_IN  ARAD_INTERFACE_ID  arad_nif_id
  )
{
  ARAD_NIF_TYPE
    nif_type;

  if (ARAD_NIF_IS_TYPE_ID(XAUI, arad_nif_id))
  {
    nif_type = ARAD_NIF_TYPE_XAUI;
  }
  else if (ARAD_NIF_IS_TYPE_ID(RXAUI, arad_nif_id))
  {
    nif_type = ARAD_NIF_TYPE_RXAUI;
  }
  else if (ARAD_NIF_IS_TYPE_ID(SGMII, arad_nif_id))
  {
    nif_type = ARAD_NIF_TYPE_SGMII;
  }
  else if (ARAD_NIF_IS_TYPE_ID(ILKN, arad_nif_id))
  {
    nif_type = ARAD_NIF_TYPE_ILKN;
  }
  else if (ARAD_NIF_IS_TYPE_ID(10GBASE_R, arad_nif_id))
  {
    nif_type = ARAD_NIF_TYPE_10GBASE_R;
  }
  else if (ARAD_NIF_IS_TYPE_ID(CGE, arad_nif_id))
  {
    nif_type = ARAD_NIF_TYPE_100G_CGE;
  }
  else if (ARAD_NIF_IS_TYPE_ID(XLGE, arad_nif_id))
  {
    nif_type = ARAD_NIF_TYPE_40G_XLGE;
  }
  else if (ARAD_NIF_IS_TYPE_ID(TM_INTERNAL_PKT, arad_nif_id))
  {
    nif_type = ARAD_NIF_TYPE_TM_INTERNAL_PKT;
  }
  else
  {
    nif_type = ARAD_NIF_TYPE_NONE;
  }

  return nif_type;
}


ARAD_INTERFACE_ID
  arad_nif_type2id(
    SOC_SAND_IN ARAD_NIF_TYPE arad_nif_type,
    SOC_SAND_IN uint32 internal_id
  )
{
  ARAD_INTERFACE_ID
    nif_id;
  
  switch(arad_nif_type) {
    case ARAD_NIF_TYPE_XAUI:
      nif_id = ARAD_NIF_ID(XAUI, internal_id);
        break;
    case ARAD_NIF_TYPE_RXAUI:
      nif_id = ARAD_NIF_ID(RXAUI, internal_id);
        break;
    case ARAD_NIF_TYPE_SGMII:
      nif_id = ARAD_NIF_ID(SGMII, internal_id);
        break;
    case ARAD_NIF_TYPE_ILKN:
      nif_id = ARAD_NIF_ID(ILKN, internal_id);
        break;
    default:
      nif_id = ARAD_NIF_ID_NONE;
    }

  return nif_id;

}




ARAD_INTERFACE_ID
  arad_nif_offset2nif_id(
    SOC_SAND_IN  ARAD_NIF_TYPE       nif_type,
    SOC_SAND_IN  uint32         nif_offset
  )
{
  ARAD_INTERFACE_ID
    nif_id;

  if (nif_offset >= ARAD_NIF_NOF_NIFS)
  {
    nif_id = ARAD_NIF_ID_NONE;
  }
  else
  {
    switch(nif_type) {
    case ARAD_NIF_TYPE_XAUI:
      nif_id = ARAD_NIF_ID(XAUI, nif_offset);
        break;
    case ARAD_NIF_TYPE_RXAUI:
      nif_id = ARAD_NIF_ID(RXAUI, nif_offset);
        break;
    case ARAD_NIF_TYPE_SGMII:
      nif_id = ARAD_NIF_ID(SGMII, nif_offset);
        break;
    case ARAD_NIF_TYPE_ILKN:
      nif_id = ARAD_NIF_ID(ILKN, nif_offset);
        break;
    default:
      nif_id = ARAD_NIF_ID_NONE;
    }
  }

  return nif_id;
}


uint32
  arad_nif_counter_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t                  port,
    SOC_SAND_IN  ARAD_NIF_COUNTER_TYPE       counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT              *counter_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_COUNTER_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE; 

  SOC_SAND_CHECK_NULL_INPUT(counter_val);

  res = arad_nif_counter_ndx_verify(
          unit,
          port,
          counter_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_nif_counter_get_unsafe(
          unit,
          port,
          counter_type,
          counter_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_counter_get()", 0, 0);
}


uint32
  arad_nif_all_counters_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT             counter_val[ARAD_NIF_NOF_COUNTERS]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_ALL_COUNTERS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE; 

  res = arad_nif_all_counters_ndx_verify(
          unit,
          port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_nif_all_counters_get_unsafe(
          unit,
          port,
          non_data_also,
          counter_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_all_counters_get()", 0, 0);
}


uint32
  arad_nif_all_nifs_all_counters_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                     counters_val[ARAD_NIF_NOF_NIFS][ARAD_NIF_NOF_COUNTERS]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_ALL_NIFS_ALL_COUNTERS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE; 

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_nif_all_nifs_all_counters_get_unsafe(
          unit,
          non_data_also,
          counters_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_all_nifs_all_counters_get()", 0, 0);
}


uint32 arad_nif_synce_clk_sel_port_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_IN  soc_port_t                  port)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE; 

  res = arad_nif_synce_clk_sel_port_verify(
          unit,
          synce_cfg_num,
          port      
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_nif_synce_clk_sel_port_set_unsafe(
          unit,
          synce_cfg_num,
          port      
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_sel_port_set()", synce_cfg_num, port);
}

uint32 arad_nif_synce_clk_sel_port_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_OUT soc_port_t                  *port)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE; 

  SOC_SAND_CHECK_NULL_INPUT(port);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_nif_synce_clk_sel_port_get_unsafe(
          unit,
          synce_cfg_num,
          port      
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_sel_port_get()", synce_cfg_num, 0x0);
}


uint32 arad_nif_synce_clk_squelch_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   synce_cfg_num,
    SOC_SAND_IN  int                      enable)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_nif_synce_clk_cfg_num_verify(
          unit,
          synce_cfg_num);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_nif_synce_clk_squelch_set_unsafe(
          unit,
          synce_cfg_num,
          enable
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_squelch_set()", synce_cfg_num, enable);
}

uint32 arad_nif_synce_clk_squelch_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   synce_cfg_num,
    SOC_SAND_OUT int                      *enable)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = arad_nif_synce_clk_cfg_num_verify(
          unit,
          synce_cfg_num);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_nif_synce_clk_squelch_get_unsafe(
          unit,
          synce_cfg_num,
          enable
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_squelch_get()", synce_cfg_num, 0x0);
}


#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_NIF_TYPE_to_string(
    SOC_SAND_IN  ARAD_NIF_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_NIF_TYPE_NONE:
    str = "none";
  break;
  case ARAD_NIF_TYPE_XAUI:
    str = "xaui";
  break;
  case ARAD_NIF_TYPE_SGMII:
    str = "sgmii";
  break;
  case ARAD_NIF_TYPE_RXAUI:
    str = "rxaui";
  break;
  case ARAD_NIF_TYPE_ILKN:
    str = "ilkn";
  break;
  default:
    str = " Unknown";
  }
  return str;
}
#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

