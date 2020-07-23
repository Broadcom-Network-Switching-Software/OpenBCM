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

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ

#include <soc/mem.h>




#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/mbcm.h>

#include <soc/dpp/ARAD/arad_scheduler_ports.h>
#include <soc/dpp/ARAD/arad_scheduler_elements.h>
#include <soc/dpp/ARAD/arad_scheduler_end2end.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_mgmt.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>

#include <soc/dpp/port_sw_db.h>

#include <shared/swstate/access/sw_state_access.h>





#define ARAD_SCH_TCG_NDX_DEFAULT                    (0)
#define ARAD_SCH_SINGLE_MEMBER_TCG_START            (4)
#define ARAD_SCH_SINGLE_MEMBER_TCG_END              (7)
#define ARAD_SCH_TCG_WEIGHT_MIN                     (0)
#define ARAD_SCH_TCG_WEIGHT_MAX                     (1023)





















STATIC uint32
  arad_sch_hr_lowest_hp_class_select_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core,
    SOC_SAND_IN  uint32                       tm_port,
    SOC_SAND_OUT ARAD_SCH_PORT_LOWEST_HP_HR_CLASS  *hp_class_conf_idx
  )
{
  uint32
    offset,
    res;
  ARAD_SCH_SHC_TBL_DATA
    shc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_HR_LOWEST_HP_CLASS_SELECT_GET);

  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  

  offset  = tm_port;

  res = arad_sch_shc_tbl_get_unsafe(
          unit, core,
          offset,
          &shc_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *hp_class_conf_idx = shc_tbl_data.hrmask_type;

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_hr_lowest_hp_class_select_get", 0, 0);
}

STATIC uint32
  arad_sch_port_force_fc_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core,
    SOC_SAND_IN  uint32                       base_hr,
    SOC_SAND_IN  uint32                       nof_priorities,
    SOC_SAND_IN  int                          enable
  )
{
      uint32 start_idx, reg_idx, fld_val = 0, res = SOC_SAND_OK;
      int priority_i;

      SOC_SAND_INIT_ERROR_DEFINITIONS(0);

      reg_idx = ARAD_REG_IDX_GET(base_hr, ARAD_SCH_PORT_NOF_PORTS_PER_FORCE_FC_REG_LINE);

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 900, exit, READ_SCH_FORCE_PORT_FC_REGISTERr(unit, core, reg_idx, &fld_val));

      start_idx = ARAD_FLD_IDX_GET(base_hr, ARAD_SCH_PORT_NOF_PORTS_PER_FORCE_FC_REG_LINE);

      
      for (priority_i = 0; priority_i < nof_priorities; priority_i++)
      {
        
         
        SOC_SAND_SET_BIT(fld_val, enable, start_idx+priority_i);
      }

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 905, exit, WRITE_SCH_FORCE_PORT_FC_REGISTERr(unit, core, reg_idx, fld_val));

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_port_tc_force_fc_set", 0, 0);
}

uint32
  arad_sch_hr_to_port_assign_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core,
    SOC_SAND_IN  uint32        tm_port,
    SOC_SAND_IN  uint8         is_port_hr
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    start_idx,
    reg_idx,
    fld_val = 0,
    is_port_hr_val;
  uint32
    base_port_tc,
    nof_priorities,
    priority_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_HR_TO_PORT_ASSIGN_SET); 

  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

  is_port_hr_val = SOC_SAND_BOOL2NUM(is_port_hr);

  
  if (!is_port_hr_val)
  {
      res = arad_sch_port_force_fc_set(unit, core, base_port_tc, nof_priorities, 1);
      SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);
  }

  
  {
      fld_val = 0;
      reg_idx = ARAD_REG_IDX_GET(base_port_tc, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_SCH_PORT_ENABLE_PORTENm(unit,  SCH_BLOCK(unit, core), reg_idx, &fld_val));
      
      start_idx = ARAD_FLD_IDX_GET(base_port_tc, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
       
      
      for (priority_i = 0; priority_i < nof_priorities; priority_i++)
      {
        
                       
        SOC_SAND_SET_BIT(fld_val, is_port_hr_val, start_idx+priority_i);
      }
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_SCH_PORT_ENABLE_PORTENm(unit, SCH_BLOCK(unit,core), reg_idx, &fld_val));
  }


  
  if (is_port_hr_val)
  {
      res = arad_sch_port_force_fc_set(unit, core, base_port_tc, nof_priorities, 0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);
  }


exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_hr_to_port_assign_set", 0, 0);
}


uint32
  arad_sch_hr_tcg_map_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core,
    SOC_SAND_IN  ARAD_SCH_SE_ID      se_id,
    SOC_SAND_IN  ARAD_TCG_NDX        tcg_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    start_idx,
    table_ndx,
    data,
    fld_val = 0;
  uint32
    hr_ndx,
    dummy_tc,
    port_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_HR_TO_PORT_ASSIGN_SET); 

  res = arad_sch_se_id_verify_unsafe(
          unit,
          se_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_SAND_IS_VAL_OUT_OF_RANGE(
       se_id, ARAD_HR_SE_ID_MIN, ARAD_HR_SE_ID_MIN + ARAD_SCH_MAX_PORT_ID))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_SE_HR_ID_ERR, 20, exit);
  }

  
  res = arad_sch_se2port_tc_id_get_unsafe(
          unit, core,
          se_id,
          &port_id,
          &dummy_tc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (port_id == ARAD_SCH_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 35, exit);
  }

  
  hr_ndx = se_id - ARAD_HR_SE_ID_MIN;

  table_ndx = ARAD_REG_IDX_GET(hr_ndx, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  start_idx = ARAD_FLD_IDX_GET(hr_ndx, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_SCH_PORT_SCHEDULER_MAP_PSMm(unit, SCH_BLOCK(unit,core), table_ndx, &data));
  fld_val = soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_get(unit,&data,TC_PG_MAPf);

  res = soc_sand_bitstream_set_any_field(&tcg_ndx,start_idx * ARAD_NOF_TCG_IN_BITS,ARAD_NOF_TCG_IN_BITS,&fld_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_set(unit,&data,TC_PG_MAPf,fld_val);  
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_SCH_PORT_SCHEDULER_MAP_PSMm(unit, SCH_BLOCK(unit,core), table_ndx, &data));    

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_hr_tcg_map_set", 0, 0);
}


static uint32
  arad_sch_hr_tcg_map_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core,
    SOC_SAND_IN  ARAD_SCH_SE_ID      se_id,
    SOC_SAND_OUT ARAD_TCG_NDX       *tcg_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    start_idx,
    table_ndx,
    data,
    fld_val = 0;
  uint32
    hr_ndx,
    dummy_tc,
    port_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

  res = arad_sch_se_id_verify_unsafe(
          unit,
          se_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_SAND_IS_VAL_OUT_OF_RANGE(
       se_id, ARAD_HR_SE_ID_MIN, ARAD_HR_SE_ID_MIN + ARAD_SCH_MAX_PORT_ID))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_SE_HR_ID_ERR, 20, exit);
  }

  
  res = arad_sch_se2port_tc_id_get_unsafe(
          unit, core,
          se_id,
          &port_id,
          &dummy_tc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (port_id == ARAD_SCH_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 35, exit);
  }

  
  hr_ndx = se_id - ARAD_HR_SE_ID_MIN;

  table_ndx = ARAD_REG_IDX_GET(hr_ndx, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  start_idx = ARAD_FLD_IDX_GET(hr_ndx, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_SCH_PORT_SCHEDULER_MAP_PSMm(unit, SCH_BLOCK(unit,core), table_ndx, &data));
  fld_val = soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_get(unit,&data,TC_PG_MAPf);

  res = soc_sand_bitstream_get_any_field(&fld_val,start_idx * ARAD_NOF_TCG_IN_BITS,ARAD_NOF_TCG_IN_BITS,tcg_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
 
exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_hr_tcg_map_get", 0, 0);
}


uint32
  arad_sch_hr_to_port_assign_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core,
    SOC_SAND_IN  uint32        tm_port,
    SOC_SAND_OUT uint8         *is_port_hr
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_idx,
    reg_idx,
    fld_val = 0,
    is_port_hr_val;
  uint32
    base_port_tc;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_HR_TO_PORT_ASSIGN_GET);

  res = arad_sch_port_id_verify_unsafe(
          unit,
          tm_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  reg_idx = ARAD_REG_IDX_GET(base_port_tc, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  fld_idx = ARAD_FLD_IDX_GET(base_port_tc, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1110, exit, READ_SCH_PORT_ENABLE_PORTENm(unit, SCH_BLOCK(unit, core), reg_idx, &fld_val));

  
  is_port_hr_val = SOC_SAND_GET_BIT(fld_val, fld_idx);
  *is_port_hr = SOC_SAND_NUM2BOOL(is_port_hr_val);

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_hr_to_port_assign_get", 0, 0);
}



uint32
  arad_sch_port_sched_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_SCH_PORT_INFO  *port_info
  )
{
  uint32
    res;
  uint32
    priority_i,
    nof_priorities,
    tcg_i;
  uint8
    is_one_member;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_SCHED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_sch_port_id_verify_unsafe(
          unit,
          tm_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
      port_info->lowest_hp_class, SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_LAST, SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_LAST,
      ARAD_SCH_HP_CLASS_OUT_OF_RANGE_ERR, 30, exit
    );

  SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(
      port_info->hr_mode, SOC_TMC_SCH_SE_HR_MODE_LAST,
      ARAD_SCH_HR_MODE_INVALID_ERR, 35, exit
    );


  for (priority_i = 0; priority_i < nof_priorities; priority_i++)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
        port_info->hr_modes[priority_i], SOC_TMC_SCH_HR_MODE_NONE, SOC_TMC_SCH_HR_MODE_ENHANCED_PRIO_WFQ,
        ARAD_SCH_HR_MODE_INVALID_ERR, 35, exit
      );    
  }

  if (nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT)
  {
    
    for (priority_i = 0; priority_i < nof_priorities; priority_i++) 
    {
      
      
      SOC_SAND_ERR_IF_OUT_OF_RANGE(
          port_info->tcg_ndx[priority_i], ARAD_TCG_MIN, ARAD_TCG_MAX, 
          ARAD_TCG_OUT_OF_RANGE_ERR, 35, exit
        );
    }

    
    for (tcg_i = ARAD_SCH_SINGLE_MEMBER_TCG_START; tcg_i <= ARAD_SCH_SINGLE_MEMBER_TCG_END; tcg_i++)
    {
      is_one_member = FALSE;
      for (priority_i = 0; priority_i < nof_priorities; priority_i++) 
      {
        if (port_info->tcg_ndx[priority_i] == tcg_i)
        {
          if (is_one_member)
          {
            
            SOC_SAND_SET_ERROR_CODE(ARAD_TCG_SINGLE_MEMBER_ERR, 100+tcg_i, exit);
          }
          else
          {
            is_one_member = TRUE;
          }
        }     
      }
    }
    
  }
  else
  {
    
    for (priority_i = 0; priority_i < nof_priorities; priority_i++) {
      SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(
        port_info->tcg_ndx[priority_i], ARAD_SCH_TCG_NDX_DEFAULT,
        ARAD_TCG_NOT_SUPPORTED_ERR, 110, exit
      );
    }
  }

  if (port_info->enable == FALSE)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      port_info->max_expected_rate, ARAD_IF_MAX_RATE_MBPS(unit),
      ARAD_SCH_SE_PORT_RATE_OUT_OF_RANGE_ERR, 140, exit
    );
  }
  else
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      port_info->max_expected_rate, 1, ARAD_IF_MAX_RATE_MBPS(unit),
      ARAD_SCH_SE_PORT_RATE_OUT_OF_RANGE_ERR, 150, exit
    );
  }

  if (port_info->group == ARAD_SCH_GROUP_NONE)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_GROUP_ERR, 160, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_sched_verify()",0,0);
}


uint32
  arad_sch_port_tcg_weight_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX        tcg_ndx,
    SOC_SAND_IN  ARAD_SCH_TCG_WEIGHT *tcg_weight
  )
{
  uint32
    res,
    data,
    is_tcg_weight_val,
    field_val;
  uint32
    base_port_tc,
    ps,
    nof_priorities;
  soc_reg_above_64_val_t
    data_above_64;
  soc_field_t
    field_name;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_TCG_WEIGHT_SET_UNSAFE);
  
  SOC_REG_ABOVE_64_CLEAR(data_above_64);


  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

  ps = ARAD_BASE_PORT_TC2PS(base_port_tc);

  
  res = arad_sch_port_force_fc_set(unit, core, base_port_tc, nof_priorities, 1);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_SCH_PORT_SCHEDULER_MAP_PSMm(unit, SCH_BLOCK(unit, core), ps, &data));
  field_val = soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_get(unit,&data,PG_WFQ_VALIDf);
 
  is_tcg_weight_val = tcg_weight->tcg_weight_valid ? 1:0; 
  SOC_SAND_SET_BIT(field_val, is_tcg_weight_val, tcg_ndx);
  
  soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_set(unit,&data,PG_WFQ_VALIDf,field_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_SCH_PORT_SCHEDULER_MAP_PSMm(unit, SCH_BLOCK(unit, core), ps, &data));    
  

  
  if (tcg_weight->tcg_weight_valid)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_SCH_PORT_SCHEDULER_WEIGHTS_PSWm(unit, SCH_BLOCK(unit, core), ps, data_above_64));
    switch (tcg_ndx)
    {
    case 0:
      field_name = WFQ_PG_0_WEIGHTf;
      break;
    case 1:
      field_name = WFQ_PG_1_WEIGHTf;
      break;
    case 2:
      field_name = WFQ_PG_2_WEIGHTf;
      break;
    case 3:
      field_name = WFQ_PG_3_WEIGHTf;
      break;
    case 4:
      field_name = WFQ_PG_4_WEIGHTf;
      break;
    case 5:
      field_name = WFQ_PG_5_WEIGHTf;
      break;
    case 6:
      field_name = WFQ_PG_6_WEIGHTf;
      break;
    case 7:
      field_name = WFQ_PG_7_WEIGHTf;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_TCG_OUT_OF_RANGE_ERR, 50, exit);
    }

    field_val = tcg_weight->tcg_weight;

    soc_SCH_PORT_SCHEDULER_WEIGHTS_PSWm_field32_set(unit,data_above_64,field_name,field_val);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_SCH_PORT_SCHEDULER_WEIGHTS_PSWm(unit, SCH_BLOCK(unit, core), ps, data_above_64));    

  }

  
  res = arad_sch_port_force_fc_set(unit, core, base_port_tc, nof_priorities, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_tcg_weight_set_unsafe()",tm_port,tcg_ndx);
}

uint32
  arad_sch_port_tcg_weight_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX        tcg_ndx,
    SOC_SAND_OUT ARAD_SCH_TCG_WEIGHT *tcg_weight
  )
{
  uint32
    res,
    data,
    is_tcg_weight_val,
    field_val;
  uint32
    base_port_tc,
    ps;
  soc_reg_above_64_val_t
    data_above_64;
  soc_field_t
    field_name;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_TCG_WEIGHT_GET_UNSAFE);
  
  SOC_REG_ABOVE_64_CLEAR(data_above_64);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  ps = ARAD_BASE_PORT_TC2PS(base_port_tc);

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_SCH_PORT_SCHEDULER_MAP_PSMm(unit, SCH_BLOCK(unit, core), ps, &data));
  field_val = soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_get(unit,&data,PG_WFQ_VALIDf);
 
  is_tcg_weight_val = SOC_SAND_GET_BIT(field_val, tcg_ndx);
  tcg_weight->tcg_weight_valid = SOC_SAND_NUM2BOOL(is_tcg_weight_val);
  

  
  if (tcg_weight->tcg_weight_valid)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_SCH_PORT_SCHEDULER_WEIGHTS_PSWm(unit, SCH_BLOCK(unit, core), ps, data_above_64));
    switch (tcg_ndx)
    {
    case 0:
      field_name = WFQ_PG_0_WEIGHTf;
      break;
    case 1:
      field_name = WFQ_PG_1_WEIGHTf;
      break;
    case 2:
      field_name = WFQ_PG_2_WEIGHTf;
      break;
    case 3:
      field_name = WFQ_PG_3_WEIGHTf;
      break;
    case 4:
      field_name = WFQ_PG_4_WEIGHTf;
      break;
    case 5:
      field_name = WFQ_PG_5_WEIGHTf;
      break;
    case 6:
      field_name = WFQ_PG_6_WEIGHTf;
      break;
    case 7:
      field_name = WFQ_PG_7_WEIGHTf;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_TCG_OUT_OF_RANGE_ERR, 50, exit);
    }

    field_val = soc_SCH_PORT_SCHEDULER_WEIGHTS_PSWm_field32_get(unit,data_above_64,field_name);
    tcg_weight->tcg_weight = field_val;
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_tcg_weight_get_unsafe()",tm_port,tcg_ndx);
}

uint32
  arad_sch_port_tcg_weight_set_verify_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  ARAD_SCH_TCG_WEIGHT       *tcg_weight
  )
{
  uint32
    res;
  uint32
    base_port_tc,
    nof_priorities;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_TCG_WEIGHT_SET_VERIFY_UNSAFE);
  
  
  res = arad_sch_port_id_verify_unsafe(
          unit,
          tm_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  if (base_port_tc == ARAD_SCH_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 17, exit)
  }

  
  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 19, exit);

  if (!(nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TCG_NOT_SUPPORTED_ERR, 20, exit);
  }

  


  SOC_SAND_ERR_IF_OUT_OF_RANGE(
          tcg_ndx, ARAD_TCG_MIN, ARAD_TCG_MAX, 
          ARAD_TCG_OUT_OF_RANGE_ERR, 35, exit
        );

  
  if (tcg_weight->tcg_weight_valid)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
            tcg_weight->tcg_weight, ARAD_SCH_TCG_WEIGHT_MIN, ARAD_SCH_TCG_WEIGHT_MAX, 
            ARAD_SCH_TCG_WEIGHT_OUT_OF_RANGE_ERR, 35, exit
          );
  }
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_tcg_weight_set_verify_unsafe()",tm_port,tcg_ndx);
}

uint32
  arad_sch_port_tcg_weight_get_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX        tcg_ndx,
    SOC_SAND_OUT ARAD_SCH_TCG_WEIGHT *tcg_weight
  )
{
  uint32
    res;
  uint32
    base_port_tc,
    nof_priorities;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_TCG_WEIGHT_GET_VERIFY_UNSAFE);
  
  
  res = arad_sch_port_id_verify_unsafe(
          unit,
          tm_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  if (base_port_tc == ARAD_SCH_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 17, exit)
  }

  

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 19, exit);

  if (!(nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TCG_NOT_SUPPORTED_ERR, 20, exit);
  }

  


  SOC_SAND_ERR_IF_OUT_OF_RANGE(
          tcg_ndx, ARAD_TCG_MIN, ARAD_TCG_MAX, 
          ARAD_TCG_OUT_OF_RANGE_ERR, 35, exit
        );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_tcg_weight_get_verify_unsafe()",tm_port, tcg_ndx);
}


uint32
  arad_sch_port_sched_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_SCH_PORT_INFO  *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_SCH_PORT_HP_CLASS_INFO
    hp_class_info;
  ARAD_SCH_SE_INFO
    se;
  uint32 
    nof_priorities,
    priority_i,
    group;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_SCHED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_sch_port_id_verify_unsafe(
          unit,
          tm_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 13, exit);
      
  
  if (port_info->enable == FALSE)
  {
    res = arad_sch_hr_to_port_assign_set(
            unit, core,
            tm_port,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

        
    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
      res = arad_sch_port_tc2se_id(
              unit, core,
              tm_port,
              priority_i,
              &(se.id)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      res = arad_sch_se_state_set(
            unit, core,
            se.id,
            FALSE
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

      if (nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT)
      {
        
        res = arad_sch_hr_tcg_map_set(
                unit,
                core,
                se.id,
                ARAD_SCH_TCG_NDX_DEFAULT
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
      }

    }
  }
  else
  {

    
    res = arad_sch_port_hr_group_get(unit, core, tm_port, &group); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
      res = arad_sch_port_tc2se_id(
              unit, core,
              tm_port,
              priority_i,
              &(se.id)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      se.state = (port_info->enable == TRUE)?ARAD_SCH_SE_STATE_ENABLE:ARAD_SCH_SE_STATE_DISABLE;
      se.is_dual = FALSE;
      se.type = ARAD_SCH_SE_TYPE_HR;
      se.type_info.hr.mode = port_info->hr_modes[priority_i];

      
      se.group = ARAD_SCH_GROUP_NONE;

      res = arad_sch_se_set_unsafe(
              unit, core,
              &se,
              1
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
      
      
      res = arad_sch_se_group_set(unit, core, se.id, group);
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

      
      if (nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT)
      {
        res = arad_sch_hr_tcg_map_set(
                unit,
                core,
                se.id,
                port_info->tcg_ndx[priority_i]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
      }
    }

    arad_ARAD_SCH_PORT_HP_CLASS_INFO_clear(&hp_class_info);

  

    res = arad_sch_hr_to_port_assign_set(
            unit, core,
            tm_port,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_sched_set_unsafe()",0,0);
}




uint32
  arad_sch_port_sched_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT ARAD_SCH_PORT_INFO  *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_SCH_SE_INFO
    se;
  ARAD_SCH_PORT_LOWEST_HP_HR_CLASS
    hp_class_select_idx = 0;
  ARAD_SCH_PORT_HP_CLASS_INFO
    hp_class_info;
  uint8
    is_port_hr;
  uint32
    nof_priorities,
    priority_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_SCHED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_sch_port_id_verify_unsafe(
          unit,
          tm_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  res = arad_sch_hr_to_port_assign_get(unit, core, tm_port, &is_port_hr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  port_info->enable = is_port_hr;

  if (is_port_hr)
  {
    res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
      res = arad_sch_port_tc2se_id(unit, core,tm_port,priority_i,&(se.id));
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      
      res = arad_sch_se_get_unsafe(
              unit, core,
              se.id,
              &se
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

      if ((se.state == ARAD_SCH_SE_STATE_DISABLE) && (is_port_hr == TRUE))
      {
        port_info->enable = FALSE;
      }
      
      if (se.type != ARAD_SCH_SE_TYPE_HR)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_SCH_SE_PORT_SE_TYPE_NOT_HR_ERR, 30, exit);
      }

      port_info->hr_modes[priority_i] = se.type_info.hr.mode;
      
      if (nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT)
      {
        res = arad_sch_hr_tcg_map_get(
                unit, core,
                se.id,
                &(port_info->tcg_ndx[priority_i])
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
      }
    }
  } 

  res = arad_sch_hr_lowest_hp_class_select_get(
          unit, core,
          tm_port,
          &hp_class_select_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  arad_ARAD_SCH_PORT_HP_CLASS_INFO_clear(&hp_class_info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_sched_get_unsafe()",0,0);
}

uint32
  arad_sch_se2port_tc_id_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  ARAD_SCH_SE_ID    se_id,
    SOC_SAND_OUT ARAD_SCH_PORT_ID  *port_id,
    SOC_SAND_OUT uint32            *tc 
  )
{
  uint32
    port_ndx,
    base_port_tc,
    port_tc_to_check,
    nof_priorities,
    res,
    tm_port,
    flags;
  soc_pbmp_t
    ports_bm;
  int
    loc_core ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_SAND_IS_VAL_OUT_OF_RANGE(
       se_id, ARAD_HR_SE_ID_MIN, ARAD_HR_SE_ID_MIN + ARAD_SCH_MAX_PORT_ID))
  {
    *port_id = ARAD_SCH_PORT_ID_INVALID;
  }
  else
  {
    *port_id = ARAD_SCH_PORT_ID_INVALID;
    port_tc_to_check = se_id - ARAD_HR_SE_ID_MIN;

    
    res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_PBMP_ITER(ports_bm, port_ndx)
    {
        res = soc_port_sw_db_flags_get(unit, port_ndx, &flags);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);

        if (!(SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))) {
            res = soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &tm_port, &loc_core);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 13, exit);

            res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, loc_core, tm_port, &base_port_tc);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

            res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, loc_core, tm_port, &nof_priorities); 
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

            if (SOC_SAND_IS_VAL_IN_RANGE(port_tc_to_check,base_port_tc,base_port_tc+nof_priorities-1))
            {
                if (loc_core == core)
                {
                    
                    *tc = port_tc_to_check - base_port_tc;
                    *port_id = tm_port;
                    break;
                }
            }
        }
    }    
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_se2port_tc_id_get_unsafe()",se_id,0);
}


uint32
  arad_sch_port_tc2se_id_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  uint32            tm_port,
    SOC_SAND_IN  uint32            tc,
    SOC_SAND_OUT ARAD_SCH_SE_ID    *se_id
  )
{
  uint32
    base_port_tc,
    nof_priorities,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if (tm_port == ARAD_SCH_PORT_ID_INVALID || 
      tm_port > ARAD_SCH_MAX_PORT_ID ||
      base_port_tc == ARAD_SCH_PORT_ID_INVALID ||
      tc >= nof_priorities)
  {
    *se_id = ARAD_SCH_SE_ID_INVALID;
  }
  else
  {
    *se_id = base_port_tc + tc + ARAD_HR_SE_ID_MIN;
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_tc2se_id_get_unsafe()",tm_port,tc);
}


int
  arad_sch_port_rate_kbits_per_sec_to_qaunta(
    SOC_SAND_IN       int       unit,
    SOC_SAND_IN       uint32    rate,     
    SOC_SAND_IN       uint32    credit_div,
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   quanta  
  )
{
  uint32
    calc2,
    calc;
  uint32 credit_worth;

  SOCDNX_INIT_FUNC_DEFS;

  if (NULL == quanta)
  {
    _rv = SOC_SAND_NULL_POINTER_ERR ;
    goto exit ;
  }
  if (0 == rate)
  {
    
    *quanta = 0;
    goto exit ;
  }
  
  SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth)));

  
   
  calc = SOC_SAND_DIV_ROUND(ticks_per_sec*SOC_SAND_NOF_BITS_IN_CHAR,1000);
  calc = SOC_SAND_DIV_ROUND(credit_worth * calc, credit_div); 
  
  calc2 = SOC_SAND_DIV_ROUND_UP(rate,calc); 
  if(calc2 != 0){
      *quanta = calc2;
  }
  else{
      *quanta = 1;
  }

exit:
   SOCDNX_FUNC_RETURN;
}


int
  arad_sch_port_qunta_to_rate_kbits_per_sec(
    SOC_SAND_IN       int       unit,
    SOC_SAND_IN       uint32    quanta, 
    SOC_SAND_IN       uint32    credit_div,   
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   rate      
  )
{
  SOC_SAND_U64
    calc2;
  uint32
    calc,
    tmp;
  uint32 credit_worth;

  SOCDNX_INIT_FUNC_DEFS;

  if (NULL == rate)
  {
    _rv = SOC_SAND_NULL_POINTER_ERR ;
    goto exit ;
  }
  if (0 == quanta)
  {
    
    _rv = SOC_SAND_DIV_BY_ZERO_ERR ;
    goto exit ;
  }
  
  SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth)));

  
  
  calc = SOC_SAND_DIV_ROUND(ticks_per_sec*SOC_SAND_NOF_BITS_IN_CHAR,1000);
  calc = SOC_SAND_DIV_ROUND(credit_worth * calc, credit_div);                     
  soc_sand_u64_multiply_longs(calc, quanta, &calc2);
  if (soc_sand_u64_to_long(&calc2, &tmp))
  {
    
    _rv = SOC_SAND_OVERFLOW_ERR ;
    goto exit ;
  }
  *rate = tmp;
exit:
   SOCDNX_FUNC_RETURN;
}
 


uint32
  arad_sch_port_priority_shaper_rate_set_unsafe(
    SOC_SAND_IN     int                  unit,
    SOC_SAND_IN     int                  core,
    SOC_SAND_IN     uint32               tm_port,
    SOC_SAND_IN     uint32               priority_ndx,
    SOC_SAND_IN     uint32               rate
  )
{
  uint32
    res,
    quanta,
    nof_ticks,
    quanta_nof_bits,
    credit_div,  
    tbl_data;
  uint32
    base_port_tc,
    offset;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_PRIORITY_SHAPER_RATE_SET_UNSAFE);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  res = arad_sch_calendar_info_get(unit, core, 0, 1, &credit_div, &nof_ticks, &quanta_nof_bits);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);


  
  if (0 == rate)
  {
    quanta = 0;
  }
  else
  {    
    
    res = arad_sch_port_rate_kbits_per_sec_to_qaunta(
            unit,
            rate,
            credit_div,
            nof_ticks,
            &quanta
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    SOC_SAND_LIMIT_FROM_ABOVE(quanta, SOC_SAND_BITS_MASK(quanta_nof_bits-1,0));
  }

  offset = base_port_tc + priority_ndx;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit,SCH_BLOCK(unit,core),offset,&tbl_data));
  soc_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm_field32_set(unit,&tbl_data,QUANTA_TO_ADDf,quanta);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1140, exit, WRITE_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit,SCH_BLOCK(unit,core),offset,&tbl_data));

  
  res = arad_sw_db_sch_port_tcg_rate_set(unit, core, offset, rate, 1);
  SOC_SAND_CHECK_FUNC_RESULT(res, 550, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_priority_shaper_rate_set_unsafe()",0,0);
}

uint32 
arad_sch_port_priority_shaper_max_burst_set_unsafe(
    SOC_SAND_IN     int                  unit,
    SOC_SAND_IN     int                  core,
    SOC_SAND_IN     uint32               tm_port,
    SOC_SAND_IN     uint32               priority_ndx,
    SOC_SAND_IN     uint32               burst
  )
{
    uint32 base_port_tc, offset, tbl_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));

    offset = base_port_tc + priority_ndx;

    SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit,SCH_BLOCK(unit,core),offset,&tbl_data));
    soc_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm_field32_set(unit,&tbl_data,MAX_BURSTf,burst);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit,SCH_BLOCK(unit,core),offset,&tbl_data));
        
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_sch_port_priority_shaper_hw_set_unsafe(
   SOC_SAND_IN   int    unit,
   SOC_SAND_IN   int    core)
{

    uint32 offset, mem_val, quanta, credit_div, nof_ticks, quanta_nof_bits;
    int rate;
    uint8 valid;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_sch_calendar_info_get(unit, core, 0, 1, &credit_div, &nof_ticks, &quanta_nof_bits));

    for (offset = 0 ; offset < ARAD_EGR_NOF_Q_PAIRS; offset++)
    {
        SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_priority_port_rate_get(unit, core, offset, &rate, &valid));
        if (valid > 0)
        {
            if (rate == 0)
            {
                quanta = 0;
            }
            else
            {    
                SOCDNX_IF_ERR_EXIT(arad_sch_port_rate_kbits_per_sec_to_qaunta(unit,rate,credit_div,nof_ticks,&quanta));
                SOC_SAND_LIMIT_FROM_ABOVE(quanta, SOC_SAND_BITS_MASK(quanta_nof_bits-1,0));
            }

            SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit,SCH_BLOCK(unit,core),offset,&mem_val));
            soc_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm_field32_set(unit,&mem_val,QUANTA_TO_ADDf,quanta);
            SOCDNX_IF_ERR_EXIT(WRITE_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit,SCH_BLOCK(unit,core),offset,&mem_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_sch_port_priority_shaper_rate_get_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     uint32            priority_ndx,    
    SOC_SAND_OUT    ARAD_SCH_PORT_PRIORITY_RATE_INFO *info
  )
{
  uint32
    res,
    offset,
    quanta,
    quanta_nof_bits,
    nof_ticks,
    rate_internal,
    credit_div,
    tbl_data;
  uint32
    base_port_tc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_PRIORITY_SHAPER_RATE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  arad_ARAD_SCH_PORT_PRIORITY_RATE_INFO_clear(info);  
 
  res= arad_sch_calendar_info_get(unit, core, 0, 1, &credit_div, &nof_ticks, &quanta_nof_bits);
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);
    
  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

  offset = base_port_tc + priority_ndx;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit,SCH_BLOCK(unit,core),offset,&tbl_data));
    quanta = soc_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm_field32_get(unit,&tbl_data,QUANTA_TO_ADDf);
    info->max_burst = soc_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm_field32_get(unit,&tbl_data,MAX_BURSTf);

  if (0 == quanta)
  {
    rate_internal = 0;
  }
  else
  {
    res = arad_sch_port_qunta_to_rate_kbits_per_sec(
            unit,
            quanta,
            credit_div,
            nof_ticks,
            &rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }
  info->rate = rate_internal;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_priority_shaper_rate_get_unsafe()",0,0);
}


uint32
  arad_sch_tcg_shaper_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,
    SOC_SAND_IN     int               rate
  )
{
  uint32
    res,
    quanta,
    nof_ticks,
    quanta_nof_bits,
    credit_div,
    tbl_data;
  uint32
    base_port_tc,
    offset;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_PRIORITY_SHAPER_RATE_SET_UNSAFE);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port,&base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  res = arad_sch_calendar_info_get(unit, core, 0, 0, &credit_div, &nof_ticks, &quanta_nof_bits);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  
  if (0 == rate)
  {
    quanta = 0;
  }
  else
  {    
    
    res = arad_sch_port_rate_kbits_per_sec_to_qaunta(
            unit,
            rate,
            credit_div,
            nof_ticks,
            &quanta
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    SOC_SAND_LIMIT_FROM_ABOVE(quanta, SOC_SAND_BITS_MASK(quanta_nof_bits-1,0));
  }

  offset = ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit,SCH_BLOCK(unit,core),offset,&tbl_data));
  soc_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm_field32_set(unit,&tbl_data,QUANTA_TO_ADDf,quanta);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1140, exit, WRITE_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit,SCH_BLOCK(unit,core),offset,&tbl_data));
  
  res = arad_sw_db_sch_port_tcg_rate_set(unit, core, offset, rate, 1);
  SOC_SAND_CHECK_FUNC_RESULT(res, 550, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_tcg_shaper_rate_set_unsafe()",tm_port,tcg_ndx);
}

uint32
arad_sch_tcg_shaper_max_burst_set_unsafe(
    SOC_SAND_IN     int                  unit,
    SOC_SAND_IN     int                  core,
    SOC_SAND_IN     uint32               tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX         tcg_ndx,
    SOC_SAND_IN     uint32               burst
  )
{
    uint32 base_port_tc, offset, tbl_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));

    offset = ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx); 

    SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit,SCH_BLOCK(unit,core),offset,&tbl_data));
    soc_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm_field32_set(unit,&tbl_data,MAX_BURSTf,burst);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit,SCH_BLOCK(unit,core),offset,&tbl_data));
        
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_sch_tcg_shaper_rate_get_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,    
    SOC_SAND_OUT    ARAD_SCH_TCG_RATE_INFO *info
  )
{
  uint32
    res,
    offset,
    quanta,
    nof_ticks,
    rate_internal,
    credit_div,    
    tbl_data,
    quanta_nof_bits,
    base_port_tc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_PRIORITY_SHAPER_RATE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  arad_ARAD_SCH_TCG_RATE_INFO_clear(info);  

  res= arad_sch_calendar_info_get(unit, core, 0, 0, &credit_div, &nof_ticks, &quanta_nof_bits);
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

  offset = ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit,SCH_BLOCK(unit,core),offset,&tbl_data));
  quanta = soc_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm_field32_get(unit,&tbl_data,QUANTA_TO_ADDf);
  info->max_burst = soc_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm_field32_get(unit,&tbl_data,MAX_BURSTf);

  if (0 == quanta)
  {
    rate_internal = 0;
  }
  else
  {
    res = arad_sch_port_qunta_to_rate_kbits_per_sec(
            unit,
            quanta,
            credit_div,
            nof_ticks,
            &rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }
  info->rate = rate_internal;  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_tcg_shaper_rate_get_unsafe()",tm_port,tcg_ndx);
}

void
  arad_ARAD_SCH_PORT_PRIORITY_RATE_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_PORT_PRIORITY_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_SCH_PORT_PRIORITY_RATE_INFO));
    
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_SCH_TCG_RATE_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_TCG_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_SCH_TCG_RATE_INFO));
    
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


int 
arad_sch_e2e_interface_allocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    )
{
    return SOC_E_NONE;
}


int 
arad_sch_e2e_interface_deallocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    )
{
    return SOC_E_NONE;
}


uint32
arad_sch_calendar_info_get(int unit, int core, int hr_calendar_num ,  int is_priority_rate_calendar ,
                          uint32 *credit_div, uint32 *nof_ticks, uint32 *quanta_nof_bits)
{
    uint32 reg_val, field_val, device_ticks_per_sec, access_period, cal_length, divider;
    SOCDNX_INIT_FUNC_DEFS;

    if (is_priority_rate_calendar)
    {
        *quanta_nof_bits = soc_mem_field_length(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, QUANTA_TO_ADDf);
    }
    else
    {
        *quanta_nof_bits = soc_mem_field_length(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, QUANTA_TO_ADDf);
    }

    if (hr_calendar_num == 0) 
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_REBOUNDED_CREDIT_CONFIGURATIONr(unit, core, &reg_val));
        divider = soc_reg_field_get(unit, SCH_REBOUNDED_CREDIT_CONFIGURATIONr, reg_val, REBOUNDED_CREDIT_WORTHf);    

        if (is_priority_rate_calendar)
        {
            SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_CONFIGURATIONr(unit, core, &reg_val));
            cal_length = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, reg_val, PIR_SHAPERS_CAL_LENGTHf) + 1;
            field_val = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, reg_val, PIR_SHAPERS_CAL_ACCESS_PERIODf);

        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_CONFIGURATIONr(unit, core, &reg_val));
            cal_length = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, reg_val, CIR_SHAPERS_CAL_LENGTHf) +1;
            field_val = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, reg_val, CIR_SHAPERS_CAL_ACCESS_PERIODf);
        }       
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_REBOUNDED_CREDIT_CONFIGURATION_1r(unit, core, &reg_val));
        divider = soc_reg_field_get(unit, SCH_REBOUNDED_CREDIT_CONFIGURATION_1r, reg_val, REBOUNDED_CREDIT_WORTH_1f);

        if (is_priority_rate_calendar)
        {
            SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_CONFIGURATION_1r(unit, core, &reg_val));
            cal_length = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATION_1r, reg_val, PIR_SHAPERS_CAL_LENGTH_1f) + 1;
            field_val = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATION_1r, reg_val, PIR_SHAPERS_CAL_ACCESS_PERIOD_1f);
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_CONFIGURATION_1r(unit, core, &reg_val));
            cal_length = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATION_1r, reg_val, CIR_SHAPERS_CAL_LENGTH_1f) + 1;
            field_val = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATION_1r, reg_val, CIR_SHAPERS_CAL_ACCESS_PERIOD_1f);
        }
    }

    access_period = (field_val >> 4);
          
    *credit_div = divider;

    device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);
    *nof_ticks = SOC_SAND_DIV_ROUND_UP(device_ticks_per_sec,(cal_length*access_period));

exit:
    SOCDNX_FUNC_RETURN;
}


int
arad_sch_port_sched_min_bw_group_get(
    SOC_SAND_IN     int                  unit,
    SOC_SAND_IN     int                  core,
    SOC_SAND_OUT    uint32               *group
  )
{

    int i, res;
    int min_bw = 0, group_bw = 0;

    SOCDNX_INIT_FUNC_DEFS;

    *group = 0;
    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.groups_bw.get(unit, core, *group, &group_bw);
    SOCDNX_IF_ERR_EXIT(res);
    min_bw = group_bw;

    for (i = 0; i < ARAD_SCH_NOF_GROUPS ; i++) {
        res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.groups_bw.get(unit, core, i, &group_bw);
        SOCDNX_IF_ERR_EXIT(res);
        if (group_bw < min_bw) {
            min_bw = group_bw;
            *group = i;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_sch_port_hr_group_get(
    SOC_SAND_IN     int                  unit,
    SOC_SAND_IN     int                  core,
    SOC_SAND_IN     uint32               tm_port,
    SOC_SAND_OUT    uint32               *group
  )
{
    uint32 interface_max_rate, res, base_q_pair, se_id;
    int group_rate, hr_rate;
    soc_port_t port;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &interface_max_rate));

    
    res = arad_sch_port_tc2se_id(unit, core, tm_port, 0, &se_id);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.hr_group_bw.get(unit, core, base_q_pair, &hr_rate);
    SOCDNX_IF_ERR_EXIT(res);

    
    if (hr_rate) {
        res = arad_sch_se_group_get(unit, core, se_id, group);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.groups_bw.get(unit, core, *group, &group_rate);
        SOCDNX_IF_ERR_EXIT(res);

        if ((group_rate - hr_rate) > 0) {
            group_rate = group_rate - hr_rate;
        } else {
            group_rate = 0;
        }
        res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.groups_bw.set(unit, core, *group, group_rate);
        SOCDNX_IF_ERR_EXIT(res);
    }

    
    SOCDNX_IF_ERR_EXIT(arad_sch_port_sched_min_bw_group_get(unit, core, group));
    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.groups_bw.get(unit, core, *group, &group_rate);
    SOCDNX_IF_ERR_EXIT(res);
    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.groups_bw.set(unit, core, *group, (group_rate + interface_max_rate));
    SOCDNX_IF_ERR_EXIT(res);
    sw_state_access[unit].dpp.soc.arad.tm.tm_info.hr_group_bw.set(unit, core, base_q_pair, interface_max_rate);
    SOCDNX_IF_ERR_EXIT(res);


exit:
    SOCDNX_FUNC_RETURN;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

