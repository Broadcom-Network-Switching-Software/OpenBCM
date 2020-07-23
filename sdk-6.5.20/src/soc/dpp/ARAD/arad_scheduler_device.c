#include <shared/bsl.h>

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

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ
#include <soc/mem.h>



#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/port_sw_db.h>


#include <soc/dpp/ARAD/arad_scheduler_device.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_mgmt.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/dpp_config_imp_defs.h>







#define ARAD_SCH_DEV_RATE_INTERVAL_RESOLUTION 128
#define ARAD_SCH_CHNL_NIF_SPEED_UP (4)




 








uint32
  arad_sch_if_shaper_rate_verify(
    SOC_SAND_IN     int           unit,
    SOC_SAND_IN     ARAD_INTERFACE_ID  if_ndx,
    SOC_SAND_IN     uint32            rate
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_IF_SHAPER_RATE_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_if_shaper_rate_verify()",0,0);
}

int
  arad_sch_if_shaper_rate_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core,
    SOC_SAND_IN  uint32        tm_port,
    SOC_SAND_OUT uint32        *if_rate
  )
{
    uint32
        res,
        rate_internal,
        credit_rate,
        device_ticks_per_sec,
        credit_worth;
    uint32
        offset,
        sch_offset,
        egress_offset;
    uint8
        fast_traversal = FALSE,
        is_channelized;
    soc_reg_above_64_val_t
        tbl_data;

    SOCDNX_INIT_FUNC_DEFS;

    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset));
    SOCDNX_IF_ERR_EXIT(res);

    
    SOC_REG_ABOVE_64_CLEAR(tbl_data);
    SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &tbl_data));
    sch_offset = soc_SCH_FC_MAP_FCMm_field32_get(unit,&tbl_data,FC_MAP_FCMf);

    if (ARAD_SCH_IS_CHNIF_ID(unit, sch_offset))
    {
        is_channelized = TRUE;
        offset = sch_offset;
    }
    else
    {
        is_channelized = FALSE;
        fast_traversal = TRUE;
        offset = ARAD_SCH_OFFSET_TO_NON_CHANNELIZED_OFFSET(unit, sch_offset);
    }      

    
    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth));
    SOCDNX_IF_ERR_EXIT(res);

    
    if (is_channelized)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_CH_NIF_RATES_CONFIGURATION_CNRCm(unit, SCH_BLOCK(unit, core), offset, tbl_data));
        credit_rate = soc_SCH_CH_NIF_RATES_CONFIGURATION_CNRCm_field32_get(unit, tbl_data, CH_NI_FXX_MAX_CR_RATEf);
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit, SCH_BLOCK(unit, core), offset, tbl_data));
        credit_rate = soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_get(unit, tbl_data, PORT_NIF_MAX_CR_RATEf);
    }
  
    if (0 == credit_rate)
    {
        rate_internal = 0;
    }
    else
    {
        device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

        res = soc_sand_clocks_to_kbits_per_sec(
            credit_rate,
            credit_worth,
            device_ticks_per_sec,
            &rate_internal
          );
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (fast_traversal)
        {
            rate_internal = rate_internal * ARAD_SCH_DEV_RATE_INTERVAL_RESOLUTION;
        }

        if(is_channelized) {
            rate_internal = rate_internal * ARAD_SCH_CHNL_NIF_SPEED_UP;
        }
    }

    *if_rate = rate_internal;

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_sch_if_shaper_rate_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     uint32                tm_port,
    SOC_SAND_IN     uint32                rate
  )
{
    uint32
        res,
        credit_rate,
        device_ticks_per_sec,
        rate_internal = rate,
        credit_worth,
        credit_rate_nof_bits;
    uint32
        offset,
        egress_offset,
        sch_offset;
    uint8
        is_channelized;
    soc_reg_above_64_val_t
        tbl_data;

    SOCDNX_INIT_FUNC_DEFS;

    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset));
    SOCDNX_IF_ERR_EXIT(res);

    
    SOC_REG_ABOVE_64_CLEAR(tbl_data);
    SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &tbl_data));
    sch_offset = soc_SCH_FC_MAP_FCMm_field32_get(unit,&tbl_data,FC_MAP_FCMf);

    if (ARAD_SCH_IS_CHNIF_ID(unit, sch_offset))
    {
        is_channelized = TRUE;
        offset = sch_offset;
        
        credit_rate_nof_bits = soc_mem_field_length(unit, SCH_CH_NIF_RATES_CONFIGURATION_CNRCm, CH_NI_FXX_MAX_CR_RATEf);
        rate_internal = rate;
    }
    else
    {
        is_channelized = FALSE;
        offset = ARAD_SCH_OFFSET_TO_NON_CHANNELIZED_OFFSET(unit, sch_offset);
        
        credit_rate_nof_bits = soc_mem_field_length(unit, SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm, PORT_NIF_MAX_CR_RATEf);
        rate_internal = SOC_SAND_DIV_ROUND_UP(rate, ARAD_SCH_DEV_RATE_INTERVAL_RESOLUTION);
    }      

    
    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth));
    SOCDNX_IF_ERR_EXIT(res);

    device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

    
    if (0 == rate_internal)
    {
        credit_rate = 0;
    }
    else
    {
        res = soc_sand_kbits_per_sec_to_clocks(
            rate_internal,
            credit_worth,
            device_ticks_per_sec,
            &credit_rate
          );
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if(is_channelized) {
            credit_rate = credit_rate * ARAD_SCH_CHNL_NIF_SPEED_UP;
        }

        
        
        SOC_SAND_LIMIT_FROM_ABOVE(credit_rate, SOC_SAND_BITS_MASK((credit_rate_nof_bits-1),0));

        if (!is_channelized){
            SOC_SAND_LIMIT_FROM_BELOW(credit_rate, ARAD_SCH_DEV_RATE_INTERVAL_RESOLUTION);
        }
    }

    
    if (is_channelized)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_CH_NIF_RATES_CONFIGURATION_CNRCm(unit, SCH_BLOCK(unit, core), offset, tbl_data));
        soc_SCH_CH_NIF_RATES_CONFIGURATION_CNRCm_field32_set(unit,tbl_data,CH_NI_FXX_MAX_CR_RATEf,credit_rate);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_CH_NIF_RATES_CONFIGURATION_CNRCm(unit, SCH_BLOCK(unit, core), offset, tbl_data));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit, SCH_BLOCK(unit, core), offset, tbl_data));
        soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_set(unit,tbl_data,PORT_NIF_MAX_CR_RATEf,credit_rate);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit, SCH_BLOCK(unit, core), offset, tbl_data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_sch_if_weight_conf_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_IF_WEIGHTS      *if_weights
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_IF_WEIGHT_CONF_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_if_weight_conf_verify()",0,0);
}


uint32
  arad_sch_if_weight_conf_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_IF_WEIGHTS      *if_weights
  )
{
  uint32
    res,
    weight_reg_idx;
  uint32
    entry_idx,
    weight_idx;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_IF_WEIGHT_CONF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(if_weights);
  

  for (entry_idx = 0; entry_idx < if_weights->nof_enties; entry_idx++)
  {
    weight_idx = if_weights->weight[entry_idx].id;

    
    weight_reg_idx = weight_idx/2;

    if ((weight_idx%2) == 0)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_DVS_WEIGHT_CONFIGr, SOC_CORE_ALL,  weight_reg_idx, WFQ_WEIGHT_XXf,  if_weights->weight[entry_idx].val));
    }
    else
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_DVS_WEIGHT_CONFIGr, SOC_CORE_ALL,  weight_reg_idx, WFQ_WEIGHT_XX_PLUS_1f,  if_weights->weight[entry_idx].val));
    }
    

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_if_weight_conf_set_unsafe()",0,0);
}


uint32
  arad_sch_if_weight_conf_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_SCH_IF_WEIGHTS      *if_weights
  )
{
  uint32
    res,
    weight_reg_idx;
  uint32
    weight_idx,
    entry_idx;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_IF_WEIGHT_CONF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(if_weights);
  

  if_weights->nof_enties = ARAD_SCH_NOF_IF_WEIGHTS;

  for (entry_idx = 0; entry_idx < if_weights->nof_enties; entry_idx++)
  {
    weight_idx = entry_idx;
    
    SOC_SAND_ERR_IF_ABOVE_MAX(
      weight_idx, ARAD_SCH_NOF_IF_WEIGHTS,
      ARAD_SCH_INTERFACE_WEIGHT_INDEX_OUT_OF_RANGE_ERR, 20, exit
    );

    if_weights->weight[entry_idx].id = entry_idx;
    
    weight_reg_idx = weight_idx/2;

    if ((weight_idx%2) == 0)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_DVS_WEIGHT_CONFIGr, SOC_CORE_ALL,  weight_reg_idx, WFQ_WEIGHT_XXf, &if_weights->weight[entry_idx].val));
    }
    else
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_DVS_WEIGHT_CONFIGr, SOC_CORE_ALL,  weight_reg_idx, WFQ_WEIGHT_XX_PLUS_1f, &if_weights->weight[entry_idx].val));
    }
    
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_if_weight_conf_get_unsafe()",0,0);
}


uint32
  arad_sch_device_if_weight_idx_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              weight_index
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DEVICE_IF_WEIGHT_IDX_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_device_if_weight_idx_verify()",0,0);
}


uint32
  arad_sch_device_if_weight_idx_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_OUT uint32              *weight_index
  )
{
  uint32
    weight_idx_result,
    res,
    data,
    tbl_data;
  uint32
    egress_offset,
    sch_offset,
    offset,
    tm_port;
  uint8
    is_channelized;
  int
    core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DEVICE_IF_WEIGHT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(weight_index);


  res = soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);  

  
  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);


  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1260, exit, READ_SCH_FC_MAP_FCMm(unit,SCH_BLOCK(unit, core),egress_offset,&data));
  sch_offset = soc_SCH_FC_MAP_FCMm_field32_get(unit,&data,FC_MAP_FCMf);

  if (ARAD_SCH_IS_CHNIF_ID(unit, sch_offset))
  {
    is_channelized = TRUE;
    offset = sch_offset;
  }
  else
  {
    is_channelized = FALSE;
    offset = ARAD_SCH_OFFSET_TO_NON_CHANNELIZED_OFFSET(unit, sch_offset);
  }     
  
  
  if (is_channelized)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1270, exit, READ_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm(unit, SCH_BLOCK(unit, core), offset, &tbl_data));
    weight_idx_result = soc_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm_field32_get(unit, &tbl_data, CH_NI_FXX_WEIGHTf);
  }
  else
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1280, exit, READ_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),offset,&tbl_data));
    weight_idx_result = soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_get(unit, &tbl_data, PORT_NIF_WEIGHTf);
  } 

  *weight_index = weight_idx_result;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_device_if_weight_idx_get_unsafe()",0,0);
}

uint32
  arad_sch_device_if_weight_idx_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_IN  uint32              weight_index
  )
{
  uint32
    res,
    tbl_data,
    data;
  uint32
    egress_offset,
    sch_offset,
    offset,
    tm_port;
  uint8
    is_channelized;
  int
    core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DEVICE_IF_WEIGHT_SET_UNSAFE);

  res = soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
  
  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);


  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1380, exit, READ_SCH_FC_MAP_FCMm(unit,SCH_BLOCK(unit, core),egress_offset,&data));
  sch_offset = soc_SCH_FC_MAP_FCMm_field32_get(unit,&data,FC_MAP_FCMf);

  if (ARAD_SCH_IS_CHNIF_ID(unit, sch_offset))
  {
    is_channelized = TRUE;
    offset = sch_offset;
  }
  else
  {
    is_channelized = FALSE;
    offset = ARAD_SCH_OFFSET_TO_NON_CHANNELIZED_OFFSET(unit, sch_offset);
  }     
  
  
  if (is_channelized)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1390, exit, READ_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm(unit, SCH_BLOCK(unit, core), offset, &tbl_data));
    soc_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm_field32_set(unit, &tbl_data, CH_NI_FXX_WEIGHTf,weight_index);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1400, exit, WRITE_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm(unit, SCH_BLOCK(unit, core), offset, &tbl_data));
  }
  else
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1410, exit, READ_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),offset,&tbl_data));
    soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_set(unit, &tbl_data, PORT_NIF_WEIGHTf,weight_index);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1420, exit, WRITE_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),offset,&tbl_data));
  } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_device_if_weight_idx_set_unsafe()",0,0);
}


uint32
  arad_sch_device_rate_entry_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              rci_level_ndx,
    SOC_SAND_IN  uint32              nof_active_links_ndx,
    SOC_SAND_IN  uint32              rate
  )
{
  uint32
    interval_in_clock_128_th,
    calc,
    offset,
    credit_worth,
    res;
  ARAD_SCH_DRM_TBL_DATA
    drm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DEVICE_RATE_ENTRY_SET_UNSAFE);
  
  res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_credit_worth_get,(unit, &credit_worth))) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);
  

  if (0 == rate)
  {
    interval_in_clock_128_th = 0;
  }
  else
  {
    calc = (credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) *
           (arad_chip_mega_ticks_per_sec_get(unit) * ARAD_SCH_DEVICE_RATE_INTERVAL_RESOLUTION);
    calc = SOC_SAND_DIV_ROUND(calc, rate);
    interval_in_clock_128_th = calc;
  }

  SOC_SAND_LIMIT_FROM_ABOVE(interval_in_clock_128_th, ARAD_SCH_DEVICE_RATE_INTERVAL_MAX);

  if (interval_in_clock_128_th != 0)
  {
    SOC_SAND_LIMIT_FROM_BELOW(interval_in_clock_128_th, ARAD_SCH_DEVICE_RATE_INTERVAL_MIN);
  }

  offset = (nof_active_links_ndx * SOC_DPP_DEFS_GET(unit, nof_rci_levels)) + rci_level_ndx;
  drm_tbl_data.device_rate = interval_in_clock_128_th;

  
  res = arad_sch_drm_tbl_set_unsafe(
          unit,
          offset,
          &drm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_device_rate_entry_set_unsafe()",0,0);
}



uint32
  arad_sch_device_rate_entry_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              rci_level_ndx,
    SOC_SAND_IN  uint32              nof_active_links_ndx,
    SOC_SAND_OUT uint32              *rate
  )
{
  uint32
    interval_in_clock_128_th,
    calc,
    offset,
  credit_worth,
    res;
  ARAD_SCH_DRM_TBL_DATA
    drm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DEVICE_RATE_ENTRY_GET_UNSAFE);

  

  offset = (nof_active_links_ndx * SOC_DPP_DEFS_GET(unit, nof_rci_levels)) + rci_level_ndx;

  res = arad_sch_drm_tbl_get_unsafe(
          unit,
          offset,
          &drm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  interval_in_clock_128_th = drm_tbl_data.device_rate;
  
  if (0 == interval_in_clock_128_th)
  {
    *rate = 0;
  }
  else
  {
    
    res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_credit_worth_get,(unit, &credit_worth))) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

    calc = (credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) *
           (arad_chip_mega_ticks_per_sec_get(unit) * ARAD_SCH_DEVICE_RATE_INTERVAL_RESOLUTION);
    calc = SOC_SAND_DIV_ROUND(calc, interval_in_clock_128_th);
    *rate = calc;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_device_rate_entry_get_unsafe()",0,0);
}


uint32
  arad_sch_ch_if_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            sch_offset,
    SOC_SAND_IN     uint32            rate
  )
{
    uint32
        res,
        rate_internal,
        credit_rate,
        device_ticks_per_sec,
        credit_worth,
        credit_rate_nof_bits;
    soc_reg_above_64_val_t
        tbl_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(tbl_data);

    if (!ARAD_SCH_IS_CHNIF_ID(unit, sch_offset))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("SCH interface %d (core %d) isn't part of channalized interface"), sch_offset, core));
    }

    rate_internal = SOC_SAND_DIV_ROUND_UP(rate, ARAD_SCH_DEV_RATE_INTERVAL_RESOLUTION);

    
    if (0 == rate_internal)
    {
        credit_rate = 0;
    }
    else
    {
        
        res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth));
        SOCDNX_IF_ERR_EXIT(res);

        device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

        res = soc_sand_kbits_per_sec_to_clocks(
                rate_internal,
                credit_worth,
                device_ticks_per_sec,
                &credit_rate
              );
        SOCDNX_SAND_IF_ERR_EXIT(res);

        credit_rate_nof_bits = soc_mem_field_length(unit, SCH_CH_NIF_RATES_CONFIGURATION_CNRCm, CH_NI_FXX_SUM_OF_PORTSf);
        
        
        SOC_SAND_LIMIT_FROM_ABOVE(credit_rate, SOC_SAND_BITS_MASK((credit_rate_nof_bits-1),0));
        SOC_SAND_LIMIT_FROM_BELOW(credit_rate, ARAD_SCH_DEV_RATE_INTERVAL_RESOLUTION);
    }

    SOCDNX_IF_ERR_EXIT(READ_SCH_CH_NIF_RATES_CONFIGURATION_CNRCm(unit,SCH_BLOCK(unit, core), sch_offset,&tbl_data));
    soc_SCH_CH_NIF_RATES_CONFIGURATION_CNRCm_field32_set(unit,&tbl_data,CH_NI_FXX_SUM_OF_PORTSf,credit_rate);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_CH_NIF_RATES_CONFIGURATION_CNRCm(unit,SCH_BLOCK(unit, core), sch_offset,&tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_sch_ch_if_rate_get_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            sch_offset,
    SOC_SAND_OUT    uint32            *rate
  )
{
    uint32
        res,
        rate_internal,
        credit_rate,
        device_ticks_per_sec,
        credit_worth;
    soc_reg_above_64_val_t
        tbl_data;

    SOCDNX_INIT_FUNC_DEFS;

    if (!ARAD_SCH_IS_CHNIF_ID(unit, sch_offset))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("SCH interface %d (core %d) isn't part of channalized interface"), sch_offset, core));
    }

    SOCDNX_IF_ERR_EXIT(READ_SCH_CH_NIF_RATES_CONFIGURATION_CNRCm(unit, SCH_BLOCK(unit, core), sch_offset, &tbl_data));
    credit_rate = soc_SCH_CH_NIF_RATES_CONFIGURATION_CNRCm_field32_get(unit, &tbl_data,CH_NI_FXX_SUM_OF_PORTSf);

    if(credit_rate == 0) {
        *rate = 0;
    } 
    else 
    {
        
        res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth));
        SOCDNX_IF_ERR_EXIT(res);

        device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

        res = soc_sand_clocks_to_kbits_per_sec(
            credit_rate, 
            credit_worth,   
            device_ticks_per_sec,
            &rate_internal     
            );
        SOCDNX_SAND_IF_ERR_EXIT(res);

        *rate = rate_internal * ARAD_SCH_DEV_RATE_INTERVAL_RESOLUTION;
    }


exit:
    SOCDNX_FUNC_RETURN;
}




#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif 
