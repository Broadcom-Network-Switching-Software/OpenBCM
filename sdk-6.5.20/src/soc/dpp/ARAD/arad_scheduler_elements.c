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



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_scheduler_element_converts.h>
#include <soc/dpp/ARAD/arad_scheduler_elements.h>
#include <soc/dpp/ARAD/arad_scheduler_end2end.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_scheduler_flows.h>

#include <soc/dpp/mbcm.h>












#define ARAD_SCH_DUAL_SPOUSE_ID(flow_id) ( ((flow_id)&0x1)?((flow_id)-1):((flow_id)+1))

















uint32
  arad_sch_se_state_get(
    SOC_SAND_IN     int                unit,
    SOC_SAND_IN     int                core,
    SOC_SAND_IN     ARAD_SCH_SE_ID     se_ndx,
    SOC_SAND_OUT    uint8              *is_se_enabled
  )
{
  uint32
    offset = 0,
    idx = 0,
    res = SOC_SAND_OK;
  ARAD_SCH_SEM_TBL_DATA
    sem_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_STATE_GET);
  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  offset  = se_ndx/8;
  idx = se_ndx%8;

  res = arad_sch_sem_tbl_get_unsafe(
          unit, core,
          offset,
          &sem_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *is_se_enabled = (uint8)SOC_SAND_GET_BIT(sem_tbl_data.sch_enable, idx);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_se_state_get()",se_ndx,0);
}


uint32
  arad_sch_se_state_set(
    SOC_SAND_IN     int                unit,
    SOC_SAND_IN     int                core,
    SOC_SAND_IN     ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN     uint8                is_se_enabled
  )
{
  uint32
    offset = 0,
    idx = 0,
    old_max_burst = 0,
    old_max_burst2 = 0,
    temp_max,
    fsf_nof_subflows = 0,
    res = SOC_SAND_OK;
  ARAD_SCH_SEM_TBL_DATA
    sem_tbl_data;
  uint32
    bit_val_to_set = 0;
  ARAD_SCH_FLOW_ID
    flow_id = 0,
    flow_id_2 = 0;
  ARAD_SCH_GLOBAL_PER1K_INFO
    global_per1k_info;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_STATE_SET);
  

  res = arad_sch_se_id_verify_unsafe(
          unit,
          se_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  offset  = se_ndx/8;
  idx = se_ndx%8;

  res = arad_sch_sem_tbl_get_unsafe(
          unit, core,
          offset,
          &sem_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  bit_val_to_set = (is_se_enabled)?0x1:0x0;

  if (SOC_SAND_GET_BIT(sem_tbl_data.sch_enable,idx) != bit_val_to_set)
  {

    
    flow_id = arad_sch_se2flow_id(se_ndx);

    
    res = arad_sch_flow_nof_subflows_get(
            unit, core,
            flow_id,
            &fsf_nof_subflows
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    res = arad_sch_flow_max_burst_save_and_set(unit, core, flow_id, 0, &old_max_burst);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (fsf_nof_subflows == 2) 
    {
        res = arad_sch_per1k_info_get_unsafe(
          unit, core,
          ARAD_SCH_FLOW_TO_1K_ID(flow_id),
          &global_per1k_info
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        flow_id_2 = ARAD_SCH_SUB_FLOW_OTHER_FLOW(flow_id, global_per1k_info.is_odd_even);
        
        res = arad_sch_flow_max_burst_save_and_set(unit, core, flow_id_2, 0, &old_max_burst2);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    SOC_SAND_SET_BIT(sem_tbl_data.sch_enable,bit_val_to_set,idx);
     res = arad_sch_sem_tbl_set_unsafe(
          unit, core,
          offset,
          &sem_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
    
    
    res = arad_sch_flow_max_burst_save_and_set(unit, core, flow_id, old_max_burst, &temp_max);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (fsf_nof_subflows == 2) 
    {
        res = arad_sch_flow_max_burst_save_and_set(unit, core, flow_id_2, old_max_burst2, &temp_max);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_se_state_set()",0,0);
}


uint32
  arad_sch_se_dual_shaper_get(
    SOC_SAND_IN     int                unit,
    SOC_SAND_IN     int                core,
    SOC_SAND_IN     ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN     ARAD_SCH_SE_TYPE        se_type,
    SOC_SAND_OUT    uint8                *is_dual_shaper
  )
{
  uint32
    se_offset_ndx,
    offset = 0,
    idx = 0,
    res = SOC_SAND_OK;
  ARAD_SCH_DSM_TBL_DATA
    dsm_tbl_data;
  ARAD_SCH_FLOW_ID
    flow_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_DUAL_SHAPER_GET);

  if (se_type == ARAD_SCH_SE_TYPE_CL)
  {
    se_offset_ndx = se_ndx - ARAD_CL_SE_ID_MIN;
  }
  else 
  {
    
    flow_id = arad_sch_se2flow_id(se_ndx);

    
    flow_id = ARAD_SCH_FLOW_BASE_QRTT_ID(flow_id);
    se_offset_ndx = arad_sch_flow2se_id(unit, flow_id) - ARAD_CL_SE_ID_MIN;
  }

  offset  = se_offset_ndx / 16;
  idx = se_offset_ndx % 16;

  res = arad_sch_dsm_tbl_get_unsafe(
          unit, core,
          offset,
          &dsm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *is_dual_shaper = (uint8)SOC_SAND_GET_BIT(dsm_tbl_data.dual_shaper_ena, idx);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_se_dual_get()",0,0);
}



uint32
  arad_sch_se_dual_shaper_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     ARAD_SCH_FLOW_ID          se_ndx,
    SOC_SAND_IN     uint8                   is_dual_shaper
  )
{
  uint32
    se_offset_ndx,
    offset = 0,
    idx = 0,
    res = SOC_SAND_OK;
  ARAD_SCH_DSM_TBL_DATA
    dsm_tbl_data;
  uint32
    bit_val_to_set = 0;
  ARAD_SCH_FLOW_ID
    flow_id;
  ARAD_SCH_SE_TYPE
    se_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_DUAL_SHAPER_SET);

  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  
  se_type = arad_sch_se_get_type_by_id(se_ndx);
  if (se_type == ARAD_SCH_SE_TYPE_CL)
  {
    se_offset_ndx = se_ndx - ARAD_CL_SE_ID_MIN;
  }
  else 
  {
    
    flow_id = arad_sch_se2flow_id(se_ndx);

    
    flow_id = ARAD_SCH_FLOW_BASE_QRTT_ID(flow_id);
    se_offset_ndx = arad_sch_flow2se_id(unit, flow_id) - ARAD_CL_SE_ID_MIN;
  }

  offset = se_offset_ndx / 16;
  idx = se_offset_ndx % 16;

  res = arad_sch_dsm_tbl_get_unsafe(
          unit, core,
          offset,
          &dsm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  bit_val_to_set = (is_dual_shaper) ? 0x1 : 0x0;

  if (SOC_SAND_GET_BIT(dsm_tbl_data.dual_shaper_ena,idx) != bit_val_to_set)
  {
    SOC_SAND_SET_BIT(dsm_tbl_data.dual_shaper_ena,bit_val_to_set,idx);
    res = arad_sch_dsm_tbl_set_unsafe(
            unit, core,
            offset,
            &dsm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_se_dual_shaper_set()",0,0);
}


uint32
  arad_sch_se_config_get(
    SOC_SAND_IN   int           unit,
    SOC_SAND_IN   int           core,
    SOC_SAND_IN   ARAD_SCH_SE_ID     se_ndx,
    SOC_SAND_IN   ARAD_SCH_SE_TYPE   se_type,
    SOC_SAND_OUT  ARAD_SCH_SE_INFO  *se
    )
{
  uint32
    offset = 0,
    res;
  ARAD_SCH_SE_HR_MODE
    hr_mode;
  uint8
    is_dual_shaper;
  ARAD_SCH_SHC_TBL_DATA
    shc_tbl_data;
  ARAD_SCH_SCC_TBL_DATA
    scc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_CONFIG_GET);

  se->id = se_ndx;
  se->type = se_type;

  switch(se_type)
    {
    case ARAD_SCH_SE_TYPE_HR:
      
      
      offset  = se_ndx - ARAD_HR_SE_ID_MIN;
      res = arad_sch_shc_tbl_get_unsafe(
              unit, core,
              offset,
              &shc_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = arad_sch_INTERNAL_HR_MODE_to_HR_MODE_convert(
              shc_tbl_data.hrmode,
              &hr_mode
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      se->type_info.hr.mode = hr_mode;
      break;
    case ARAD_SCH_SE_TYPE_CL:
      
      
      offset  = se_ndx - ARAD_CL_SE_ID_MIN;
      res = arad_sch_scc_tbl_get_unsafe(
              unit, core,
              offset,
              &scc_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      se->type_info.cl.id = scc_tbl_data.clsch_type;
      break;
    case ARAD_SCH_SE_TYPE_FQ:
        break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_SCH_SE_TYPE_UNDEFINED_ERR, 40, exit);
      break;
    }

    res = arad_sch_se_dual_shaper_get(
            unit, core,
            se_ndx,
            se_type,
            &is_dual_shaper
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    se->is_dual = is_dual_shaper;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_se_config_get", 0, 0);
}


uint32
  arad_sch_se_config_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int              core,
    SOC_SAND_IN  ARAD_SCH_SE_INFO     *se,
    SOC_SAND_IN  uint32              nof_subflows
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    internal_hr_mode,
    offset;
  ARAD_SCH_SE_ID
    spouse_se_id;
  ARAD_SCH_FLOW_ID
    se_flow_id,
    spouse_flow_id;
  uint8
    is_composite_agg_0_1,
    dual_state,
    spouse_id_is_enabled;
  ARAD_SCH_SHC_TBL_DATA
    shc_tbl_data;
  ARAD_SCH_SCC_TBL_DATA
    scc_tbl_data;
  ARAD_SCH_GLOBAL_PER1K_INFO
    global_per1k_info;
  uint32
    fsf_nof_subflows;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_CONFIG_SET);

  se_flow_id = arad_sch_se2flow_id(se->id);
  spouse_flow_id = ARAD_SCH_DUAL_SPOUSE_ID(se_flow_id);
  spouse_se_id = arad_sch_flow2se_id(unit, spouse_flow_id);

  res = arad_sch_per1k_info_get_unsafe(
          unit, core,
          ARAD_SCH_FLOW_TO_1K_ID(se_flow_id),
          &global_per1k_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  if (se->state == ARAD_SCH_SE_STATE_DISABLE)
  {
    res = arad_sch_flow_nof_subflows_get(
            unit, core,
            se_flow_id,
            &fsf_nof_subflows
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    is_composite_agg_0_1 = SOC_SAND_NUM2BOOL((fsf_nof_subflows == 2) && (global_per1k_info.is_odd_even));
    if (is_composite_agg_0_1 && ARAD_SCH_COMPOSITE_IS_SECOND_SUBFLOW(se_flow_id, TRUE))
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_SCH_ILLEGAL_COMPOSITE_AGGREGATE_ERR, 11, exit);
    }

    res = arad_sch_se_state_set(
            unit, core,
            se->id,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (is_composite_agg_0_1)
    {
      res = arad_sch_se_state_set(
              unit, core,
              spouse_se_id,
              FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }
  else
  {
    is_composite_agg_0_1 = SOC_SAND_NUM2BOOL((nof_subflows == 2) && (global_per1k_info.is_odd_even));
    if (is_composite_agg_0_1 && ARAD_SCH_COMPOSITE_IS_SECOND_SUBFLOW(se_flow_id, TRUE))
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_SCH_ILLEGAL_COMPOSITE_AGGREGATE_ERR, 13, exit);
    }
    if (is_composite_agg_0_1 && se->is_dual)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_SCH_COMPOSITE_AGGREGATE_DUAL_SHAPER_ERR, 16, exit);
    }

    switch(se->type)
    {
    case ARAD_SCH_SE_TYPE_HR:
      

      
      offset = se->id - ARAD_HR_SE_ID_MIN;
      res = arad_sch_shc_tbl_get_unsafe(
              unit, core,
              offset,
              &shc_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = arad_sch_HR_MODE_to_INTERNAL_HR_MODE_convert(
              se->type_info.hr.mode,
              &internal_hr_mode
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if (shc_tbl_data.hrmode != internal_hr_mode)
      {
        shc_tbl_data.hrmode = internal_hr_mode;

        res = arad_sch_shc_tbl_set_unsafe(
              unit, core,
              offset,
              &shc_tbl_data
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      }

      break;
    case ARAD_SCH_SE_TYPE_CL:
      

      
      offset  = se->id - ARAD_CL_SE_ID_MIN;

      scc_tbl_data.clsch_type = se->type_info.cl.id;
      res = arad_sch_scc_tbl_set_unsafe(
              unit, core,
              offset,
              &scc_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      break;
    case ARAD_SCH_SE_TYPE_FQ:
        break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_SCH_SE_TYPE_UNDEFINED_ERR, 40, exit);
      break;
    }
    res = arad_sch_se_state_get(
            unit, core,
            spouse_se_id,
            &spouse_id_is_enabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    
    dual_state = SOC_SAND_NUM2BOOL(spouse_id_is_enabled ? se->is_dual : FALSE);
    res = arad_sch_se_dual_shaper_set(
            unit, core,
            se->id,
            dual_state
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = arad_sch_se_state_set(
          unit, core,
          se->id,
          TRUE
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    if (is_composite_agg_0_1)
    {
      res = arad_sch_se_state_set(
            unit, core,
            spouse_se_id,
            TRUE
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
  } 

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_se_config_set", 0, 0);
}



uint32
  arad_sch_se_id_and_type_match_verify(
    ARAD_SCH_SE_ID                   se_id,
    ARAD_SCH_SE_TYPE                 se_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_SCH_SE_ID_AND_TYPE_MATCH_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_se_id_and_type_match_verify()",0,0);
}




uint32
  arad_sch_class_type_params_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_CL_CLASS_TYPE_ID cl_type_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_CL_CLASS_INFO *class_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_CLASS_TYPE_PARAMS_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_class_type_params_verify()",0,0);
}


uint32
  arad_sch_class_type_params_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_CL_CLASS_TYPE_ID cl_type_ndx,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_INFO *class_type
  )
{
  uint32
    offset,
    res;
  ARAD_SCH_SCT_TBL_DATA
    sct_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_CLASS_TYPE_PARAMS_GET_UNSAFE);

  offset = cl_type_ndx;

  res = arad_sch_sct_tbl_get_unsafe(
          unit, core,
          offset,
          &sct_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  class_type->id = cl_type_ndx;

  
  res = arad_sch_INTERNAL_CLASS_TYPE_to_CLASS_TYPE_convert(
          &sct_tbl_data,
          class_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_class_type_params_get_unsafe()",0,0);
}


uint32
  arad_sch_class_type_params_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_CL_CLASS_INFO *class_type,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_INFO *exact_class_type
  )
{
  uint32
    offset,
    res;
  ARAD_SCH_SCT_TBL_DATA
    sct_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_CLASS_TYPE_PARAMS_SET_UNSAFE);

  
  res = arad_sch_CLASS_TYPE_to_INTERNAL_CLASS_TYPE_convert(
          unit,
          class_type,
          &sct_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  
  offset = class_type->id;

  res = arad_sch_sct_tbl_set_unsafe(
          unit, core,
          offset,
          &sct_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  res = arad_sch_sct_tbl_get_unsafe(
          unit, core,
          offset,
          &sct_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);


  
  res = arad_sch_INTERNAL_CLASS_TYPE_to_CLASS_TYPE_convert(
          &sct_tbl_data,
          exact_class_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_class_type_params_set_unsafe()",0,0);
}


uint32
  arad_sch_class_type_params_table_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_SE_CL_CLASS_TABLE *sct
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_CLASS_TYPE_PARAMS_TABLE_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_class_type_params_table_verify()",0,0);
}


uint32
  arad_sch_class_type_params_table_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_CL_CLASS_TABLE *sct,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_TABLE *exact_sct
  )
{
  uint32
    res,
    sct_index;
  const ARAD_SCH_SE_CL_CLASS_INFO
      *sct_entry;
  ARAD_SCH_SE_CL_CLASS_INFO
      *exact_sct_entry;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_UNSAFE);

  
  exact_sct->nof_class_types = 0;
  for (sct_index = 0; sct_index < sct->nof_class_types; ++sct_index)
  {
    sct_entry = &sct->class_types[sct_index];
    exact_sct_entry = &exact_sct->class_types[sct_index];
    res = arad_sch_class_type_params_set_unsafe(
            unit, core,
            sct_entry,
            exact_sct_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);
    exact_sct->nof_class_types++;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_class_type_params_table_set_unsafe()",0,0);
}


uint32
  arad_sch_class_type_params_table_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_TABLE *sct
  )
{
  uint32
    res,
    sct_index;
  ARAD_SCH_SE_CL_CLASS_INFO
      sct_entry;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_CLASS_TYPE_PARAMS_TABLE_GET_UNSAFE);

  arad_ARAD_SCH_SE_CL_CLASS_INFO_clear(&sct_entry);

  sct->nof_class_types = ARAD_SCH_NOF_CLASS_TYPES;
  
  for (sct_index = 0; sct_index < ARAD_SCH_NOF_CLASS_TYPES; ++sct_index)
  {
    res = arad_sch_class_type_params_get_unsafe(
            unit, core,
            sct_index,
            &sct_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);
    ARAD_COPY(&(sct->class_types[sct_index]), &sct_entry, ARAD_SCH_SE_CL_CLASS_INFO, 1);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_class_type_params_table_get_unsafe()",0,0);
}




uint32
  arad_sch_se_verify_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_SCH_SE_INFO      *se
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_VERIFY_UNSAFE);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_se_verify_unsafe()",unit,se->id);
}

uint32
  arad_sch_se_get_unsafe(
    SOC_SAND_IN   int         unit,
    SOC_SAND_IN   int         core,
    SOC_SAND_IN   ARAD_SCH_SE_ID    se_ndx,
    SOC_SAND_OUT  ARAD_SCH_SE_INFO  *se
  )
{
  uint32
    res;
  uint8
    is_sched_enabled = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  se->id = se_ndx;
  se->type = arad_sch_se_get_type_by_id(se_ndx);
  
  
  res = arad_sch_se_state_get(
          unit, core,
          se_ndx,
          &is_sched_enabled
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  se->state = is_sched_enabled ?
                       ARAD_SCH_SE_STATE_ENABLE :
                       ARAD_SCH_SE_STATE_DISABLE;

  
  res = arad_sch_se_config_get(
          unit, core,
          se_ndx,
          se->type,
          se
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_se_get_unsafe()",0,0);
}



uint32
  arad_sch_se_set_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int              core,
    SOC_SAND_IN  ARAD_SCH_SE_INFO      *se,
    SOC_SAND_IN  uint32              nof_subflows
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_SET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  res = arad_sch_se_config_set(
          unit, core,
          se,
          nof_subflows
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_se_set_unsafe()",unit,se->id);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

