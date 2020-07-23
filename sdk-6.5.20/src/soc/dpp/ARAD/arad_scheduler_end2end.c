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
#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/cosq.h>

#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_callback_handles.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>


#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_scheduler_end2end.h>
#include <soc/dpp/ARAD/arad_scheduler_flows.h>
#include <soc/dpp/ARAD/arad_scheduler_ports.h>
#include <soc/dpp/ARAD/arad_scheduler_flow_converts.h>
#include <soc/dpp/ARAD/arad_scheduler_device.h>
#include <soc/dpp/ARAD/arad_scheduler_elements.h>
#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_sw_db.h>

#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>

#include <bcm_int/dpp/cosq.h>
#include <soc/dpp/mbcm.h>







#define ARAD_SCH_CALENDER_ACCESS_PERIOD (3)





















uint32
  arad_scheduler_end2end_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res,
    data,
    residue,
    field_val,
    index;
  int
    core;

 SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCHEDULER_END2END_REGS_INIT);
  
 SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {   
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, core, 0, SUB_FLOW_ENABLEf,  0x1));
     
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,WRITE_SCH_SMP_BACK_UP_MESSAGESr(unit, core, 0x10ff));
     
     
     for(index = 0; index <= 255; index++) {
         SOC_SAND_SOC_IF_ERROR_RETURN(res, 2, exit, WRITE_SCH_CIR_SHAPER_CALENDAR_CSCm(unit, SCH_BLOCK(unit,core), index, &index));
         SOC_SAND_SOC_IF_ERROR_RETURN(res, 3, exit, WRITE_SCH_PIR_SHAPER_CALENDAR_PSCm(unit, SCH_BLOCK(unit,core), index, &index));
     }

     
     SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_SCH_CIR_SHAPERS_CONFIGURATIONr(unit, core, &data));

     
     residue = 0;
     field_val = (ARAD_SCH_CALENDER_ACCESS_PERIOD << 4) + residue;
     soc_reg_field_set(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, &data, CIR_SHAPERS_CAL_ACCESS_PERIODf, field_val);


     
     field_val = ARAD_NOF_TCG_IDS - 1;
     soc_reg_field_set(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, &data, CIR_SHAPERS_CAL_LENGTHf, field_val);
     SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_SCH_CIR_SHAPERS_CONFIGURATIONr(unit, core, data));

     if(SOC_IS_JERICHO(unit))
     {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 21, exit, WRITE_SCH_CIR_SHAPERS_CONFIGURATION_1r(unit, core, data));
     }
     SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_SCH_PIR_SHAPERS_CONFIGURATIONr(unit, core, &data));

     
     residue = 0;
     field_val = (ARAD_SCH_CALENDER_ACCESS_PERIOD << 4) + residue;
     soc_reg_field_set(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, &data, PIR_SHAPERS_CAL_ACCESS_PERIODf, field_val);

     
     field_val = ARAD_HR_SE_ID_MAX - ARAD_HR_SE_ID_MIN;
     soc_reg_field_set(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, &data, PIR_SHAPERS_CAL_LENGTHf, field_val);
     SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_SCH_PIR_SHAPERS_CONFIGURATIONr(unit, core, data)); 

     if(SOC_IS_JERICHO(unit))
     {
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_SCH_PIR_SHAPERS_CONFIGURATION_1r(unit, core, data));
     }

     
     if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
         SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,WRITE_SCH_REG_10Br(unit,  0x3FF7));
     }

#ifdef PLISIM
     if (!SAL_BOOT_PLISIM)
#endif
     {
         uint32 entry = 0x7FFFFFF;
          
         res = arad_fill_table_with_entry(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, SCH_BLOCK(unit,core), &entry);
         SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

         res = arad_fill_table_with_entry(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, SCH_BLOCK(unit,core), &entry);
         SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
     }
 }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_scheduler_end2end_regs_init()",0,0);

}




uint32
  arad_scheduler_end2end_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    port_i,
    weight_i,
    ind,
    flags;
  ARAD_SCH_GLOBAL_PER1K_INFO
    *global_per1k_info = NULL;
  ARAD_SCH_SE_CL_CLASS_TABLE
    *cl_class_table = NULL,
    *exact_cl_class_table = NULL;
  ARAD_SCH_PORT_INFO
    *sch_port_info = NULL;
  ARAD_SCH_IF_WEIGHTS
    *weights = NULL;
  uint32 
    tm_port;
  int 
    core;
  soc_pbmp_t
    ports_bm;
    

 SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCHEDULER_END2END_INIT);

   res = arad_sch_flow_ipf_config_mode_set_unsafe(
          unit,
          ARAD_SCH_NOF_FLOW_IPF_CONFIG_MODES
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  res = arad_scheduler_end2end_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_ALLOC(global_per1k_info, ARAD_SCH_GLOBAL_PER1K_INFO, 1, "global_per1k_info");
  BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {
    ARAD_CLEAR_STRUCT(global_per1k_info, ARAD_SCH_GLOBAL_PER1K_INFO);
    global_per1k_info->is_cl_cir = FALSE;
    global_per1k_info->is_interdigitated = FALSE;
    global_per1k_info->is_odd_even = FALSE;
    for (ind = ARAD_SCH_FLOW_BASE_AGGR_FLOW_ID; ind <= ARAD_SCH_MAX_FLOW_ID; ind += 1024)
    {
      res = arad_sch_per1k_info_set_unsafe(
            unit, core,
            ARAD_SCH_FLOW_TO_1K_ID(ind),
            global_per1k_info
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

  ARAD_ALLOC(cl_class_table, ARAD_SCH_SE_CL_CLASS_TABLE, 1, "cl_class_table");
  ARAD_ALLOC(exact_cl_class_table, ARAD_SCH_SE_CL_CLASS_TABLE, 1, "exact_cl_class_table");
  BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {
    ARAD_CLEAR_STRUCT(cl_class_table, ARAD_SCH_SE_CL_CLASS_TABLE);
    ARAD_CLEAR_STRUCT(exact_cl_class_table, ARAD_SCH_SE_CL_CLASS_TABLE);
    res =
      arad_sch_class_type_params_table_set(
          unit, core,
          cl_class_table,
          exact_cl_class_table
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  ARAD_ALLOC_AND_CLEAR_STRUCT(sch_port_info, ARAD_SCH_PORT_INFO,"sch_port_info");
  ARAD_ALLOC_AND_CLEAR_STRUCT(weights, ARAD_SCH_IF_WEIGHTS,"weights");
  sch_port_info->enable = FALSE;
  sch_port_info->group = ARAD_SCH_GROUP_AUTO;
  for (ind = 0; ind < ARAD_NOF_TRAFFIC_CLASSES; ind++) 
  {
    sch_port_info->hr_modes[ind] = ARAD_SCH_HR_MODE_NONE;
  }
  
  sch_port_info->lowest_hp_class = ARAD_SCH_PORT_LOWEST_HP_HR_CLASS_EF1;
  sch_port_info->max_expected_rate = ARAD_SCH_PORT_MAX_EXPECTED_RATE_AUTO;

  res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  SOC_PBMP_ITER(ports_bm, port_i)
  {
    res = soc_port_sw_db_flags_get(unit, port_i, &flags);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit);
    if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags))) {
      res = soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

      res = arad_sch_port_sched_set_unsafe(
            unit, core,
            tm_port,
            sch_port_info
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }

  for (weight_i = 0; weight_i < ARAD_SCH_NOF_IF_WEIGHTS; weight_i++)
  {
    weights->weight[weight_i].id = weight_i;
    weights->weight[weight_i].val = 0x1;
  }
  weights->nof_enties = ARAD_SCH_NOF_IF_WEIGHTS;

  res = arad_sch_if_weight_conf_set_unsafe(
          unit,
          weights
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  if (global_per1k_info) {
      ARAD_FREE(global_per1k_info);
  }
  ARAD_FREE(cl_class_table);
  ARAD_FREE(exact_cl_class_table);
  ARAD_FREE(sch_port_info);
  ARAD_FREE(weights);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_scheduler_end2end_init()",0,0);
}

STATIC uint32
  arad_sch_group_to_se_assign(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core,
    SOC_SAND_IN  ARAD_SCH_SE_ID      father_se_ndx,
    SOC_SAND_OUT ARAD_SCH_GROUP      *group
  )
{
  uint32
    res = 0;
  ARAD_SCH_GROUP
    grp,
    father_grp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_GROUP_TO_SE_ASSIGN);

  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  res = arad_sch_se_group_get(
          unit, core,
          father_se_ndx,
          &father_grp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  grp = (father_grp + 1) % ARAD_SCH_NOF_GROUPS;

  *group = grp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_group_to_se_assign()",0,0);
}


uint32
  arad_sch_se_group_get(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     ARAD_SCH_SE_ID        se_ndx,
    SOC_SAND_OUT    ARAD_SCH_GROUP*  group
  )
{
  uint32
    offset,
    idx,
    res;
  ARAD_SCH_FGM_TBL_DATA
    fgm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_GROUP_GET);

  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  offset  = se_ndx/8;
  idx = se_ndx%8;

  
  res = arad_sch_fgm_tbl_get_unsafe(
          unit, core,
          offset,
          &fgm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *group = fgm_tbl_data.flow_group[idx];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_se_group_get()",0,0);
}



uint32
  arad_sch_se_group_set(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     ARAD_SCH_SE_ID        se_ndx,
    SOC_SAND_IN     ARAD_SCH_GROUP        group
  )
{
  uint32
    offset,
    idx,
    hr_ndx,
    field_val,
    data = 0,
    res;  
  ARAD_SCH_FGM_TBL_DATA
    fgm_tbl_data;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SE_GROUP_SET); 

  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  offset  = se_ndx/8;
  idx     = se_ndx%8;

  
  res = arad_sch_fgm_tbl_get_unsafe(
          unit, core,
          offset,
          &fgm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (fgm_tbl_data.flow_group[idx] != (uint32)group)
  {
    fgm_tbl_data.flow_group[idx] = group;
    res = arad_sch_fgm_tbl_set_unsafe(
          unit, core,
          offset,
          &fgm_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  

  
  if (SOC_SAND_IS_VAL_IN_RANGE(
       se_ndx, ARAD_HR_SE_ID_MIN, ARAD_HR_SE_ID_MIN + ARAD_SCH_MAX_PORT_ID))
  {
    hr_ndx = se_ndx - ARAD_HR_SE_ID_MIN;
    offset = ARAD_REG_IDX_GET(hr_ndx, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE); 
    idx = ARAD_FLD_IDX_GET(hr_ndx, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, READ_SCH_PORT_GROUP_PFGMm(unit, SCH_BLOCK(unit,core), offset, &data));

    field_val = soc_SCH_PORT_GROUP_PFGMm_field32_get(unit,&data,PORT_GROUPf);

    res = soc_sand_bitstream_set_any_field(&group,idx * 2,2,&field_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    soc_SCH_PORT_GROUP_PFGMm_field32_set(unit,&data,PORT_GROUPf,field_val);  
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, WRITE_SCH_PORT_GROUP_PFGMm(unit, SCH_BLOCK(unit,core), offset, &field_val));       
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_se_group_set()",0,0);
}



uint32
  arad_sch_slow_max_rates_verify(
    SOC_SAND_IN  int                 unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SLOW_MAX_RATES_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_slow_max_rates_verify()",0,0);
}


uint32
  arad_sch_slow_max_rates_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_IN  int                 slow_rate_val
  )
{
  uint32
    slow_fld_val,
    res, reg_val32;
  ARAD_SCH_SUBFLOW
    sub_flow;
  ARAD_SCH_INTERNAL_SUB_FLOW_DESC
    internal_sub_flow;
  soc_field_info_t
    peak_rate_man_fld, 
    peak_rate_exp_fld;


    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SLOW_MAX_RATES_SET_UNSAFE);

    arad_ARAD_SCH_SUBFLOW_clear(unit, &sub_flow);

   
    ARAD_TBL_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_MAN_EVENf, &peak_rate_man_fld);
    ARAD_TBL_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_EXP_EVENf, &peak_rate_exp_fld);

    sub_flow.shaper.max_rate = slow_rate_val;

    res = arad_sch_to_internal_subflow_shaper_convert(unit, &sub_flow, &internal_sub_flow, TRUE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    slow_fld_val = 0;
    slow_fld_val |= ARAD_FLD_IN_PLACE(internal_sub_flow.peak_rate_exp, peak_rate_exp_fld);
    slow_fld_val |= ARAD_FLD_IN_PLACE(internal_sub_flow.peak_rate_man, peak_rate_man_fld);
   
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 50, exit, ARAD_REG_ACCESS_ERR, READ_SCH_SHAPER_CONFIGURATION_REGISTER_1r(unit, SOC_CORE_ALL, &reg_val32));
  
    soc_reg_field_set(unit, SCH_SHAPER_CONFIGURATION_REGISTER_1r, &reg_val32, (slow_rate_type == 1)? SHAPER_SLOW_RATE_1f:SHAPER_SLOW_RATE_2f, slow_fld_val);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 60, exit, ARAD_REG_ACCESS_ERR, WRITE_SCH_SHAPER_CONFIGURATION_REGISTER_1r(unit, SOC_CORE_ALL, reg_val32));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_slow_max_rates_set_unsafe()",0,0);
}



uint32
  arad_sch_slow_max_rates_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_OUT int      *slow_rate_val
  )
{
  uint32
    slow_fld_val,
    res,
    reg_val32;
  ARAD_SCH_SUBFLOW
    sub_flow;
  ARAD_SCH_INTERNAL_SUB_FLOW_DESC
    internal_sub_flow;
  soc_field_info_t
    peak_rate_man_fld, 
    peak_rate_exp_fld;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SLOW_MAX_RATES_GET_UNSAFE);

  
  internal_sub_flow.max_burst = 0;
  internal_sub_flow.slow_rate_index = 0;

  
    ARAD_TBL_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_MAN_EVENf, &peak_rate_man_fld);
    ARAD_TBL_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_EXP_EVENf, &peak_rate_exp_fld);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, READ_SCH_SHAPER_CONFIGURATION_REGISTER_1r(unit, SOC_CORE_ALL, &reg_val32));
    slow_fld_val = soc_reg_field_get(unit, SCH_SHAPER_CONFIGURATION_REGISTER_1r, reg_val32, (slow_rate_type == 1)? SHAPER_SLOW_RATE_1f:SHAPER_SLOW_RATE_2f);

    internal_sub_flow.peak_rate_exp = ARAD_FLD_FROM_PLACE(slow_fld_val, peak_rate_exp_fld);
    internal_sub_flow.peak_rate_man = ARAD_FLD_FROM_PLACE(slow_fld_val, peak_rate_man_fld);

    
    res = arad_sch_from_internal_subflow_shaper_convert(unit, &internal_sub_flow, &sub_flow);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    *slow_rate_val = sub_flow.shaper.max_rate;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_slow_max_rates_get_unsafe()",0,0);
}


uint32
  arad_sch_aggregate_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_INFO        *se,
    SOC_SAND_IN  ARAD_SCH_FLOW           *flow
  )
{
  ARAD_SCH_FLOW_ID flow_ndx ;
	
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_AGGREGATE_VERIFY);

  flow_ndx = arad_sch_se2flow_id(se_ndx);

  if (flow_ndx != flow->sub_flow[0].id)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_AGGR_SE_AND_FLOW_ID_MISMATCH_ERR, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_aggregate_verify()",se_ndx,0);
}


uint32
  arad_sch_aggregate_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_INFO        *se,
    SOC_SAND_IN  ARAD_SCH_FLOW           *flow
  )
{
  uint32
    res;
  uint32
    sub_flow_i,
    nof_subflows = 0;
  ARAD_SCH_GROUP
    group = ARAD_SCH_GROUP_LAST;
  ARAD_SCH_SE_ID
    subflow_se_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_AGGREGATE_SET_UNSAFE);

  
  if(se->state == ARAD_SCH_SE_STATE_ENABLE)
  {
    for (sub_flow_i = 0; sub_flow_i < ARAD_SCH_NOF_SUB_FLOWS; sub_flow_i++)
    {
      if (flow->sub_flow[sub_flow_i].is_valid)
      {
        ++nof_subflows;
      }
    }

    res = arad_sch_se_set_unsafe(
            unit, core,
            se,
            nof_subflows
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    if (se->group == ARAD_SCH_GROUP_AUTO)
    {
      res = arad_sch_group_to_se_assign(
        unit, core,
        flow->sub_flow[0].credit_source.id,
        &group
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      group = se->group;
    }

    res = arad_sch_se_group_set(
            unit, core,
            se->id,
            group
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    if (nof_subflows > 1)
    {
      subflow_se_id = arad_sch_flow2se_id(unit,
                        flow->sub_flow[1].id
                      );
      if (ARAD_SCH_INDICATED_SE_ID_IS_VALID(subflow_se_id))
      {
        res = arad_sch_se_group_set(
            unit, core,
            subflow_se_id,
            group
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
    }

    

    
    res = arad_sch_flow_set_unsafe(
            unit, core,
            flow->sub_flow[0].id,
            flow
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }
  else 
  {
    for (sub_flow_i = 0; sub_flow_i < ARAD_SCH_NOF_SUB_FLOWS; sub_flow_i++)
    {
      if (flow->sub_flow[sub_flow_i].is_valid)
      {
        ++nof_subflows;
      }
    }

    res = arad_sch_se_set_unsafe(
            unit, core,
            se,
            nof_subflows
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = arad_sch_flow_set_unsafe(
            unit, core,
            flow->sub_flow[0].id,
            flow
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_aggregate_set_unsafe()",0,0);
}


uint32
  arad_sch_aggregate_group_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_INFO        *se,
    SOC_SAND_IN  ARAD_SCH_FLOW           *flow
  )
{
  uint32
    res;
  uint32
    sub_flow_i,
    nof_subflows = 0;
  ARAD_SCH_GROUP
    group = ARAD_SCH_GROUP_LAST;
  ARAD_SCH_SE_ID
    subflow_se_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_AGGREGATE_GROUP_SET_UNSAFE);

  if(se->state == ARAD_SCH_SE_STATE_ENABLE)
  {
    for (sub_flow_i = 0; sub_flow_i < ARAD_SCH_NOF_SUB_FLOWS; sub_flow_i++)
    {
      if (flow->sub_flow[sub_flow_i].is_valid)
      {
        ++nof_subflows;
      }
    }

    
    if (se->group == ARAD_SCH_GROUP_AUTO)
    {
      res = arad_sch_group_to_se_assign(
        unit, core,
        flow->sub_flow[0].credit_source.id,
        &group
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      group = se->group;
    }
    res = arad_sch_se_group_set(
            unit, core,
            se->id,
            group
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    if (nof_subflows > 1)
    {
      subflow_se_id = arad_sch_flow2se_id(unit,
                        flow->sub_flow[1].id
                      );
      if (ARAD_SCH_INDICATED_SE_ID_IS_VALID(subflow_se_id))
      {
        res = arad_sch_se_group_set(
            unit, core,
            subflow_se_id,
            group
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
    }

    

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_aggregate_group_set_unsafe",0,0);
}


uint32
  arad_sch_aggregate_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_OUT ARAD_SCH_SE_INFO        *se,
    SOC_SAND_OUT ARAD_SCH_FLOW           *flow
  )
{
  uint32
    res;
  ARAD_SCH_FLOW_ID
    flow_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_AGGREGATE_GET_UNSAFE);

  flow_ndx = arad_sch_se2flow_id(
               se_ndx
             );

  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  res = arad_sch_se_get_unsafe(
          unit, core,
          se_ndx,
          se
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_sch_flow_get_unsafe(
          unit, core,
          flow_ndx,
          flow
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_aggregate_get_unsafe()",0,0);
}


uint32
  arad_sch_per1k_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_IN  ARAD_SCH_GLOBAL_PER1K_INFO *per1k_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PER1K_INFO_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_per1k_info_verify()",0,0);
}

static int __regions_cache_was_allocated[SOC_MAX_NUM_DEVICES] = {0};
static uint8 *__regions_is_cl_cir[SOC_MAX_NUM_DEVICES] = {NULL};
static uint8 *__regions_is_interdigitated[SOC_MAX_NUM_DEVICES] = {NULL};
static int *__regions_p_is_oddEven[SOC_MAX_NUM_DEVICES] = {NULL};

STATIC int
arad_sch_allocate_cache_for_per1k(SOC_SAND_IN int unit)
{
  int ret ;
  int size, counter, total_size ;
  int *int_p ;

  ret = 1 ;
  __regions_is_cl_cir[unit] = NULL ;
  __regions_is_interdigitated[unit] = NULL ;
  __regions_p_is_oddEven[unit] = NULL ;
  size =
    ((SOC_DPP_DEFS_GET(unit,nof_flows_per_pipe) / 1024) * SOC_DPP_DEFS_GET(unit,nof_cores)) ;
  total_size = sizeof(*(__regions_is_cl_cir[unit])) * size ;
  __regions_is_cl_cir[unit] = sal_alloc(total_size,"per1k_cache") ;
  if (__regions_is_cl_cir[unit] == NULL)
  {
    goto exit ;
  }
  total_size = sizeof(*(__regions_is_interdigitated[unit])) * size ;
  __regions_is_interdigitated[unit] = sal_alloc(total_size,"per1k_cache") ;
  if (__regions_is_interdigitated[unit] == NULL)
  {
    goto exit ;
  }
  
  sal_memset(__regions_is_interdigitated[unit], -1, total_size);

  total_size = sizeof(*(__regions_p_is_oddEven[unit])) * size ;
  __regions_p_is_oddEven[unit] = sal_alloc(total_size,"per1k_cache") ;
  if (__regions_p_is_oddEven[unit] == NULL)
  {
    goto exit ;
  }
  
  int_p = __regions_p_is_oddEven[unit] ;
  for (counter = 0 ; counter < size ; counter++)
  {
    *int_p++ = (int)(-1) ;
  }
  ret = 0 ;
  __regions_cache_was_allocated[unit] = 1 ;
  
exit:
  if (ret)
  {
    __regions_cache_was_allocated[unit] = 0 ;
    if (__regions_is_cl_cir[unit])
    {
      sal_free(__regions_is_cl_cir[unit]) ;
    }
    if (__regions_is_interdigitated[unit])
    {
      sal_free(__regions_is_interdigitated[unit]) ;
    }
    if (__regions_p_is_oddEven[unit])
    {
      sal_free(__regions_p_is_oddEven[unit]) ;
    }
    __regions_is_cl_cir[unit] = NULL ;
    __regions_is_interdigitated[unit] = NULL ;
    __regions_p_is_oddEven[unit] = NULL ;
  }
  return (ret) ;
}

void
arad_sch_deallocate_cache_for_per1k(int unit)
{
    __regions_cache_was_allocated[unit] = 0 ;

    if (__regions_is_cl_cir[unit])
    {
      sal_free(__regions_is_cl_cir[unit]) ;
    }

    if (__regions_is_interdigitated[unit])
    {
      sal_free(__regions_is_interdigitated[unit]) ;
    }

    if (__regions_p_is_oddEven[unit])
    {
      sal_free(__regions_p_is_oddEven[unit]) ;
    }

    __regions_is_cl_cir[unit] = NULL ;
    __regions_is_interdigitated[unit] = NULL ;
    __regions_p_is_oddEven[unit] = NULL ;
}


uint32
  arad_sch_per1k_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_IN  ARAD_SCH_GLOBAL_PER1K_INFO *per1k_info
  )
{
  uint32
    res,
    reg_val = 0;
  uint8
    is_cl_cir         = FALSE,
    is_odd_even       = FALSE,
    is_interdigitated = FALSE;
  uint32
    k_idx,
    k_idx_arr_i,
    k_idx_i;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PER1K_INFO_SET_UNSAFE);

  {
    if (!__regions_cache_was_allocated[unit])
    {
      int err ;

      err = arad_sch_allocate_cache_for_per1k(unit) ;
      if(err)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_INTERRUPT_INSUFFICIENT_MEMORY_ERR, 15, exit);
      }
    }
    
    {
      int table_index ;

      table_index = k_flow_ndx * SOC_DPP_DEFS_GET(unit,nof_cores) + core;
      if ((per1k_info->is_interdigitated == __regions_is_interdigitated[unit][table_index]) &&
                     (per1k_info->is_odd_even == __regions_p_is_oddEven[unit][table_index]) &&
                     (per1k_info->is_cl_cir == __regions_is_cl_cir[unit][table_index]))
      {
        goto exit ;
      }
    }
  }
    
  k_idx = (k_flow_ndx - ARAD_SCH_FLOW_TO_1K_ID(ARAD_SCH_FLOW_BASE_AGGR_FLOW_ID));
  k_idx_arr_i =(k_idx < 32) ? 0 : 1;
  k_idx_i = k_idx - (32 * k_idx_arr_i);
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_FSF_COMPOSITE_CONFIGURATIONr, core, k_idx_arr_i , FSF_COMP_ODD_EVEN_Nf, &(reg_val)));

  is_odd_even = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx_i);

  if(per1k_info->is_odd_even != is_odd_even)
  {
    SOC_SAND_SET_BIT(reg_val, per1k_info->is_odd_even, k_idx_i);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_FSF_COMPOSITE_CONFIGURATIONr, core, k_idx_arr_i, FSF_COMP_ODD_EVEN_Nf,  reg_val));
  }
  
  
   SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SELECT_FLOW_TO_QUEUE_MAPPINGr, core, k_idx_arr_i, INTER_DIG_Nf, &(reg_val)));

  is_interdigitated = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx_i);

  if(per1k_info->is_interdigitated != is_interdigitated)
  {
    SOC_SAND_SET_BIT(reg_val, per1k_info->is_interdigitated, k_idx_i);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SELECT_FLOW_TO_QUEUE_MAPPINGr, core, k_idx_arr_i, INTER_DIG_Nf,  reg_val));
  }
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERSr, core,  k_idx_arr_i, SWITCH_CIR_EIR_Nf, &reg_val));

  is_cl_cir = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx_i);

  if(per1k_info->is_cl_cir != is_cl_cir)
  {
    SOC_SAND_SET_BIT(reg_val, per1k_info->is_cl_cir, k_idx_i);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERSr, core, k_idx_arr_i, SWITCH_CIR_EIR_Nf,  reg_val));
  }
  
  {
    int table_index ;

    table_index = k_flow_ndx * SOC_DPP_DEFS_GET(unit,nof_cores) + core;
    __regions_is_interdigitated[unit][table_index] = per1k_info->is_interdigitated ;
    __regions_p_is_oddEven[unit][table_index] = per1k_info->is_odd_even ;
    __regions_is_cl_cir[unit][table_index] = per1k_info->is_cl_cir ;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_per1k_info_set_unsafe()",0,0);
}


uint32
  arad_sch_per1k_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_OUT ARAD_SCH_GLOBAL_PER1K_INFO *per1k_info
  )
{
  uint32
    reg_val,
    res = SOC_SAND_OK;
  uint8
    is_cl_cir         = FALSE,
    is_odd_even       = FALSE,
    is_interdigitated = FALSE;
  uint32
    k_idx,
    k_idx_arr_i,
    k_idx_i;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PER1K_INFO_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(core, SOC_DPP_DEFS_GET(unit, nof_cores) , ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,15,exit);

  
  {
    if (!__regions_cache_was_allocated[unit])
    {
      int err ;

      err = arad_sch_allocate_cache_for_per1k(unit) ;
      if(err)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_INTERRUPT_INSUFFICIENT_MEMORY_ERR, 15, exit);
      }
    }
  }

  if (!ARAD_SCH_1K_FLOWS_IS_IN_AGGR_RANGE(k_flow_ndx))
  {
    arad_ARAD_SCH_GLOBAL_PER1K_INFO_clear(per1k_info);
  }
  else
  {
    int already_loaded ;

    already_loaded = 0 ;
    {
      int table_index ;

      table_index = k_flow_ndx * SOC_DPP_DEFS_GET(unit,nof_cores) + core;
      if (__regions_p_is_oddEven[unit][table_index] != -1)
      {
        per1k_info->is_interdigitated = __regions_is_interdigitated[unit][table_index];
        per1k_info->is_odd_even = __regions_p_is_oddEven[unit][table_index];
        per1k_info->is_cl_cir = __regions_is_cl_cir[unit][table_index];
        already_loaded = 1 ;
      }
    }
    if (!already_loaded)
    {
      k_idx = (k_flow_ndx - ARAD_SCH_FLOW_TO_1K_ID(ARAD_SCH_FLOW_BASE_AGGR_FLOW_ID));
      k_idx_arr_i =(k_idx < 32) ? 0 : 1;
      k_idx_i = k_idx - (32 * k_idx_arr_i);
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_FSF_COMPOSITE_CONFIGURATIONr, core,  k_idx_arr_i, FSF_COMP_ODD_EVEN_Nf, &(reg_val)));
  
      is_odd_even = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx_i);
  
      per1k_info->is_odd_even = is_odd_even;
      
      
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SELECT_FLOW_TO_QUEUE_MAPPINGr, core,  k_idx_arr_i, INTER_DIG_Nf, &(reg_val)));
  
      is_interdigitated = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx_i);
  
      per1k_info->is_interdigitated = is_interdigitated;
      
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERSr, core,  k_idx_arr_i, SWITCH_CIR_EIR_Nf, &reg_val));
  
      is_cl_cir = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx_i);
  
      per1k_info->is_cl_cir = is_cl_cir;
      
      {
        int table_index ;

        table_index = k_flow_ndx * SOC_DPP_DEFS_GET(unit,nof_cores) + core;
        __regions_is_interdigitated[unit][table_index] = per1k_info->is_interdigitated ;
        __regions_p_is_oddEven[unit][table_index] = per1k_info->is_odd_even ;
        __regions_is_cl_cir[unit][table_index] = per1k_info->is_cl_cir ;
      }
    }
  }
exit:

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_per1k_info_get_unsafe()",0,0);
}


uint32
  arad_sch_nof_quartets_to_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_interdigitated,
    SOC_SAND_IN  uint8                 is_composite,
    SOC_SAND_OUT uint32                 *nof_quartets_to_map
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_NOF_QUARTETS_TO_MAP_GET);

  switch(is_interdigitated)
  {
  case FALSE:
    if (is_composite == FALSE)
    {
      *nof_quartets_to_map = 1;
    }
    else
    {
      *nof_quartets_to_map = 2;
    }
    break;
  case TRUE:
    if (is_composite == FALSE)
    {
      *nof_quartets_to_map = 2;
    }
    else
    {
      *nof_quartets_to_map = 4;
    }
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_FLOW_TO_Q_INVALID_GLOBAL_CONF_ERR, 10, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_nof_quartets_to_map_get()", 0, 0);
}


uint32
  arad_sch_flow_to_queue_mapping_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_IN  uint32                 nof_quartets_to_map,
    SOC_SAND_IN  ARAD_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  )
{
  uint32
    offset,
    res;
  ARAD_SCH_FLOW_ID
    flow_ndx;
  uint32
    k_flow_ndx,
    quartets_to_map_calculated = 0;
  ARAD_SCH_GLOBAL_PER1K_INFO
    per1k_info;
  ARAD_SCH_FFM_TBL_DATA
    ffm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FLOW_TO_QUEUE_MAPPING_VERIFY);

  flow_ndx = ARAD_SCH_QRTT_TO_FLOW_ID(quartet_ndx);
  k_flow_ndx = ARAD_SCH_FLOW_TO_1K_ID(flow_ndx);

  res = arad_sch_per1k_info_get_unsafe(
          unit, core,
          k_flow_ndx,
          &per1k_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (per1k_info.is_odd_even == FALSE)
  {
    
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_FLOW_TO_Q_ODD_EVEN_IS_FALSE_ERR, 40, exit);
  }

  res = arad_sch_nof_quartets_to_map_get(
          unit,
          per1k_info.is_interdigitated,
          quartet_flow_info->is_composite,
          &quartets_to_map_calculated
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (quartets_to_map_calculated != nof_quartets_to_map)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_FLOW_TO_Q_NOF_QUARTETS_MISMATCH_ERR, 60, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    quartet_flow_info->fip_id, ARAD_MAX_FAP_ID,
    ARAD_FAP_PORT_ID_INVALID_ERR , 65, exit
  );

  
  {
    offset = quartet_ndx / 2;
    res = arad_sch_ffm_tbl_get_unsafe(
          unit, core,
          offset,
          &ffm_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    
    if ((ffm_tbl_data.device_number != SOC_TMC_MAX_FAP_ID) && (quartet_flow_info->fip_id != SOC_TMC_MAX_FAP_ID))
    {
      if (ffm_tbl_data.device_number != quartet_flow_info->fip_id)
      { 
      	if (!SOC_IS_QUX(unit)) {
        SOC_SAND_SET_ERROR_CODE(ARAD_SCH_FLOW_TO_FIP_SECOND_QUARTET_MISMATCH_ERR, 80, exit);
      }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_flow_to_queue_mapping_verify()",0,0);
}



uint32
  arad_sch_flow_to_queue_mapping_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_IN  uint32                 nof_quartets_to_map,
    SOC_SAND_IN  ARAD_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  )
{
  uint32
    offset,
    quartet_i,
    res;
  ARAD_SCH_FQM_TBL_DATA
    fqm_tbl_data;
  ARAD_SCH_FFM_TBL_DATA
    ffm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FLOW_TO_QUEUE_MAPPING_SET_UNSAFE);

  for (quartet_i = 0; quartet_i < nof_quartets_to_map; ++quartet_i)
  {
    
    offset  = quartet_ndx + quartet_i;

    res = arad_sch_fqm_tbl_get_unsafe(
            unit, core,
            offset,
            &fqm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    fqm_tbl_data.base_queue_num = quartet_flow_info->base_q_qrtt_id;
    fqm_tbl_data.sub_flow_mode = quartet_flow_info->is_composite;

    res = arad_sch_fqm_tbl_set_unsafe(
            unit, core,
            offset,
            &fqm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    

    if (nof_quartets_to_map == 1)
    {
      
      int update_table ;

      update_table = 1 ;
      if ((quartet_flow_info->fip_id == SOC_TMC_MAX_FAP_ID) &&
                          (quartet_flow_info->other_quartet_is_valid))
      {
        
        update_table = 0 ;
      }
      if (update_table)
      {
        
        offset = quartet_ndx / 2 ;
        ffm_tbl_data.device_number = quartet_flow_info->fip_id ;
        res =
          arad_sch_ffm_tbl_set_unsafe(
            unit, core,
            offset,
            &ffm_tbl_data
          ) ;
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        
      }
    }
    else
    {
    if (((quartet_ndx + quartet_i) % 2) == 0)
    {
      
      offset = (quartet_ndx + quartet_i) / 2;

      ffm_tbl_data.device_number = quartet_flow_info->fip_id;

      res = arad_sch_ffm_tbl_set_unsafe(
              unit, core,
              offset,
              &ffm_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
      
      }
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_flow_to_queue_mapping_set_unsafe()",0,0);
}


uint32
  arad_sch_flow_to_queue_mapping_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_OUT ARAD_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  )
{
  uint32
    offset,
    res;
  ARAD_SCH_FQM_TBL_DATA
    fqm_tbl_data;
  ARAD_SCH_FFM_TBL_DATA
    ffm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FLOW_TO_QUEUE_MAPPING_GET_UNSAFE);

  
  offset  = quartet_ndx;

  res = arad_sch_fqm_tbl_get_unsafe(
          unit, core,
          offset,
          &fqm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  quartet_flow_info->base_q_qrtt_id = fqm_tbl_data.base_queue_num;
  quartet_flow_info->is_composite = (uint8)fqm_tbl_data.sub_flow_mode;
  

  
  offset = quartet_ndx/2;

  res = arad_sch_ffm_tbl_get_unsafe(
          unit, core,
          offset,
          &ffm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  quartet_flow_info->fip_id = ffm_tbl_data.device_number;

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_flow_to_queue_mapping_get_unsafe()",0,0);
}



uint8
  arad_sch_is_flow_id_se_id(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID        flow_id
  )
{
  ARAD_SCH_SE_ID
    se_id = 0;

  se_id = arad_sch_flow2se_id(unit, flow_id);

  return (ARAD_SCH_INDICATED_SE_ID_IS_VALID(se_id))?TRUE : FALSE;
}


uint32
  arad_sch_flow_delete_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID         flow_ndx
  )
{
  uint32
      offset, 
    res = SOC_SAND_OK;
  ARAD_SCH_FDMS_TBL_DATA
    sch_fdms_tbl_data;
  ARAD_SCH_SHDS_TBL_DATA
    shds_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FLOW_DELETE_UNSAFE);

  
  
  offset  = flow_ndx / 2;
  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_shds_tbl_get_unsafe, (unit, core, offset, &shds_tbl_data)) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (flow_ndx % 2 == 0)
  {
    shds_tbl_data.max_burst_even = 0x0;
    shds_tbl_data.max_burst_update_even = 0x1;
  }
  else
  {
    shds_tbl_data.max_burst_odd = 0x0;
    shds_tbl_data.max_burst_update_odd = 0x1;
  }

  
  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_shds_tbl_set_unsafe, (unit, core, offset, &shds_tbl_data)) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  ARAD_DEVICE_CHECK(unit, exit);
  
  sch_fdms_tbl_data.cos = 0;
  sch_fdms_tbl_data.hrsel_dual = 0;
  sch_fdms_tbl_data.sch_number = 0;
  res = arad_sch_fdms_tbl_set_unsafe(
          unit, core,
          flow_ndx,
          &sch_fdms_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_flow_delete_unsafe()",flow_ndx,0);
}

uint32
  arad_sch_flow_is_deleted_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID         flow_ndx,
    SOC_SAND_OUT uint8                 *flow_is_reset
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_SCH_FDMS_TBL_DATA
    sch_fdms_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FLOW_IS_RESET_GET_UNSAFE);

  res = arad_sch_fdms_tbl_get_unsafe(
          unit, core,
          flow_ndx,
          &sch_fdms_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *flow_is_reset = (sch_fdms_tbl_data.cos == 0) ? TRUE : FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_flow_is_deleted_get_unsafe()",flow_ndx,0);
}

#if ARAD_DEBUG

uint32
  arad_flow_status_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  uint32               flow_id,
    SOC_SAND_OUT uint32               *credit_rate
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    credit_cnt;
  uint32
    credit_worth;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FLOW_STATUS_INFO_GET);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_1r, SOC_CORE_ALL, 0, FILTER_FLOWf,  flow_id));



  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, SOC_CORE_ALL, 0, FILTER_BY_FLOWf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, SOC_CORE_ALL, 0, FILTER_BY_SUB_FLOWf,  0x0));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, SOC_CORE_ALL, 0, FILTER_DEST_FAPf,  0x0));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,READ_SCH_DBG_CREDIT_COUNTERr(unit, core, &credit_cnt));

  soc_sand_os_task_delay_milisec(1008);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_SCH_DBG_CREDIT_COUNTERr(unit, core, &credit_cnt));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
    res,50,exit,ARAD_GET_ERR_TEXT_001,MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth))) ;

  *credit_rate = credit_cnt * ((SOC_SAND_NOF_BITS_IN_CHAR * credit_worth) / ARAD_RATE_1K);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_flow_status_info_get()",flow_id,0);
}

uint32
  arad_port_status_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  uint32               port_id,
    SOC_SAND_IN  uint32               priority_ndx,
    SOC_SAND_OUT uint32               *credit_rate,
    SOC_SAND_OUT uint32               *fc_cnt,
    SOC_SAND_OUT uint32               *fc_percent
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    credit_cnt,
    crd_reg_val,
    fc_reg_val,
    fld_val;
  uint32
    credit_worth,  
    base_port_tc,
    offset;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
   
  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, port_id, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  offset = base_port_tc + priority_ndx;


 
  crd_reg_val = 0;
  fld_val = offset;

  ARAD_FLD_TO_REG(SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATIONr, DVS_FILTER_PORTf, fld_val, crd_reg_val, 10, exit);

  fld_val = 0x1;
  ARAD_FLD_TO_REG(SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATIONr, CNT_BY_PORTf, fld_val, crd_reg_val, 20, exit);

  fld_val = 0x0;
  ARAD_FLD_TO_REG(SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATIONr, CNT_BY_NIFf, fld_val, crd_reg_val, 21, exit);

 
  fc_reg_val = 0;
  fld_val = offset;
  ARAD_FLD_TO_REG(SCH_DBG_DVS_FC_COUNTERS_CONFIGURATIONr, FC_CNT_PORTf, fld_val, fc_reg_val, 30, exit);

  fld_val = 0x1;
  ARAD_FLD_TO_REG(SCH_DBG_DVS_FC_COUNTERS_CONFIGURATIONr, CNT_PORT_FCf, fld_val, fc_reg_val, 40, exit);

 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATIONr(unit, core, crd_reg_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,WRITE_SCH_DBG_DVS_FC_COUNTERS_CONFIGURATIONr(unit, core, fc_reg_val));
 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,READ_SCH_DVS_CREDIT_COUNTERr(unit, core, &credit_cnt));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,READ_SCH_DBG_DVS_FLOW_CONTROL_COUNTERr(unit, core, fc_cnt));
 
  soc_sand_os_task_delay_milisec(1008);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,READ_SCH_DVS_CREDIT_COUNTERr(unit, core, &credit_cnt));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,READ_SCH_DBG_DVS_FLOW_CONTROL_COUNTERr(unit, core, fc_cnt));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
    res,120,exit,ARAD_GET_ERR_TEXT_001,MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth))) ;

  *credit_rate = credit_cnt * ((SOC_SAND_NOF_BITS_IN_CHAR * credit_worth) / ARAD_RATE_1K);


  if (*fc_cnt != 0)
  {
    *fc_percent = SOC_SAND_DIV_ROUND_UP((*fc_cnt * 100), arad_chip_ticks_per_sec_get(unit));
    SOC_SAND_LIMIT_FROM_ABOVE(*fc_percent, 100);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_status_info_get()",port_id,priority_ndx);

}

uint32
  arad_agg_status_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  uint32               se_id,
    SOC_SAND_OUT uint32               *credit_rate,
    SOC_SAND_OUT uint32               *overflow


  )
{
  uint32
    credit_cnt,
    res=0;
  uint32
    credit_worth;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_AGG_STATUS_INFO_GET);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_DBG_CML_CREDIT_SCHEDULER_COUNTER_CONFIGURATIONr, core, 0, FILTER_SCH_MASKf,  0x7fff));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_DBG_CML_CREDIT_SCHEDULER_COUNTER_CONFIGURATIONr, core, 0, FILTER_SCHf,  se_id));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  0x30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_DBG_CML_SCHEDULER_COUNTERr, core, 0, CML_SCH_CREDIT_CNTf, &credit_cnt));

  soc_sand_os_task_delay_milisec(1008);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  0x40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_DBG_CML_SCHEDULER_COUNTERr, core, 0, CML_SCH_CREDIT_CNTf, &credit_cnt));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  0x45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_DBG_CML_SCHEDULER_COUNTERr, core, 0, CML_SCH_CREDIT_OVFf, overflow));

  if (SOC_SAND_NUM2BOOL(*overflow) != TRUE)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
      res,50,exit,ARAD_GET_ERR_TEXT_001,MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth))) ;
    *credit_rate = credit_cnt * ((SOC_SAND_NOF_BITS_IN_CHAR * credit_worth)/ARAD_RATE_1K);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_agg_status_info_get()",se_id,0);
}

uint32
arad_flow_and_up_info_get_unsafe(
        SOC_SAND_IN     int                             unit,
        SOC_SAND_IN     int                             core,
        SOC_SAND_IN     uint32                             flow_id,
        SOC_SAND_IN     uint32                             reterive_status,
        SOC_SAND_INOUT  ARAD_SCH_FLOW_AND_UP_INFO           *flow_and_up_info
)
{
    uint32
    res = SOC_SAND_OK;
    uint32
    credit_source_i;
    ARAD_OFP_RATES_INTERFACE_SHPR_INFO
    *dummy_shaper = NULL;
    ARAD_SCH_SE_CL_CLASS_INFO class_type;
    ARAD_SCH_FLOW_AND_UP_PORT_INFO *port_sch_info;
    ARAD_SCH_FLOW_AND_UP_SE_INFO   *se_sch_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_ALLOC_AND_CLEAR_STRUCT(dummy_shaper, ARAD_OFP_RATES_INTERFACE_SHPR_INFO,"dummy_shaper");


    if (flow_and_up_info->credit_sources_nof == 0)
    {
        
        res = arad_sch_flow_get_unsafe(
                unit, core,
                flow_id,
                &(flow_and_up_info->sch_consumer)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if(reterive_status)
        {
            res = arad_flow_status_info_get(unit, core, flow_id, &(flow_and_up_info->credit_rate));
            SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
        }
        

        if((flow_and_up_info->sch_consumer).sub_flow[0].is_valid == TRUE)
        {
            flow_and_up_info->credit_sources[0] = (flow_and_up_info->sch_consumer).sub_flow[0].credit_source.id;
            flow_and_up_info->credit_sources_nof++;
        }
        if((flow_and_up_info->sch_consumer).sub_flow[1].is_valid == TRUE)
        {
            flow_and_up_info->credit_sources[1] = (flow_and_up_info->sch_consumer).sub_flow[1].credit_source.id;
            flow_and_up_info->credit_sources_nof++;
        }
    }

    for(credit_source_i = 0; credit_source_i < (flow_and_up_info->credit_sources_nof); ++credit_source_i)
    {

        res = arad_sch_se2port_tc_id(
            unit, core,
                (flow_and_up_info->credit_sources)[credit_source_i],
                &(flow_and_up_info->sch_port_id[credit_source_i]),
                &(flow_and_up_info->sch_priority_ndx[credit_source_i]));
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

        if(arad_sch_is_port_id_valid(unit, flow_and_up_info->sch_port_id[credit_source_i]))
        {
            flow_and_up_info->is_port_sch[credit_source_i] = 1;
            port_sch_info = &((flow_and_up_info->sch_union_info[credit_source_i]).port_sch_info);
            arad_ARAD_SCH_FLOW_AND_UP_PORT_INFO_clear(port_sch_info);

            res = arad_sch_port_sched_get_unsafe(
                    unit,
                    core,
                    flow_and_up_info->sch_port_id[credit_source_i],
                    &(port_sch_info->port_info)
            );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            
            res = arad_ofp_rates_sch_single_port_rate_hw_get(unit, core, flow_and_up_info->sch_port_id[credit_source_i], &(port_sch_info->ofp_rate_info.sch_rate));
            if (res != SOC_E_NONE) {
                SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR,25,exit);
            }
            
            res = arad_ofp_rates_egq_single_port_rate_hw_get(unit,core,flow_and_up_info->sch_port_id[credit_source_i], &(port_sch_info->ofp_rate_info.egq_rate));
            if (res != SOC_E_NONE) {
                SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR,27,exit);
            }
            
            res = arad_ofp_rates_single_port_max_burst_get(unit,core,flow_and_up_info->sch_port_id[credit_source_i],&(port_sch_info->ofp_rate_info.max_burst));
            if (res != SOC_E_NONE) {
                SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR,29,exit);
            }

            
            port_sch_info->ofp_rate_info.port_id = flow_and_up_info->sch_port_id[credit_source_i];

            if(reterive_status)
            {
                arad_port_status_info_get(unit,
                        core,
                        flow_and_up_info->sch_port_id[credit_source_i],
                        flow_and_up_info->sch_priority_ndx[credit_source_i],
                        &(port_sch_info->credit_rate),
                        &(port_sch_info->fc_cnt),
                        &(port_sch_info->fc_percent)
                );
            }
        } else if(arad_sch_is_se_id_valid(unit,flow_and_up_info->credit_sources[credit_source_i]))
        {
            flow_and_up_info->is_port_sch[credit_source_i] = 0;

            
            se_sch_info = &((flow_and_up_info->sch_union_info[credit_source_i]).se_sch_info);
            arad_ARAD_SCH_FLOW_AND_UP_SE_INFO_clear(unit, se_sch_info);

            (se_sch_info->se_info).id = flow_and_up_info->credit_sources[credit_source_i];

            res = arad_sch_aggregate_get_unsafe(
                    unit, core,
                    (se_sch_info->se_info).id,
                    &(se_sch_info->se_info),
                    &(se_sch_info->sch_consumer)
            );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            if (se_sch_info->se_info.type == ARAD_SCH_SE_TYPE_CL){
                res = arad_sch_class_type_params_get_unsafe(unit, core, se_sch_info->se_info.type_info.cl.id, &class_type);
                SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);
                se_sch_info->cl_mode = class_type.weight_mode;
                res = arad_sch_flow_ipf_config_mode_get(unit, &(se_sch_info->ipf_mode));
                SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
            }
            if(reterive_status)
            {
                res = arad_agg_status_info_get(unit,
                        core,
                        (se_sch_info->se_info).id,
                        &(se_sch_info->credit_rate),
                        &(se_sch_info->credit_rate_overflow));
                SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

            }
            if((se_sch_info->sch_consumer).sub_flow[0].is_valid == TRUE && (flow_and_up_info->next_level_credit_sources_nof < SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES))
            {
                flow_and_up_info->next_level_credit_sources[flow_and_up_info->next_level_credit_sources_nof] = (se_sch_info->sch_consumer).sub_flow[0].credit_source.id;
                (flow_and_up_info->next_level_credit_sources_nof)++;
            }
            if((se_sch_info->sch_consumer).sub_flow[1].is_valid == TRUE && (flow_and_up_info->next_level_credit_sources_nof < SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES))
            {
                (flow_and_up_info->next_level_credit_sources)[(flow_and_up_info->next_level_credit_sources_nof)] = (se_sch_info->sch_consumer).sub_flow[1].credit_source.id;
                (flow_and_up_info->next_level_credit_sources_nof)++;
            }
        }
        else
        {
            SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_SE_ID_ERR,40,exit);
            
        }
    }
#ifdef ARAD_PRINT_FLOW_AND_UP_PRINT_DRM_AND_MAL_RATES
    if ( (flow_and_up_info->next_level_credit_sources_nof) == 0)
    {
        ARAD_INTERFACE_ID   if_id;
        uint32              source_if_id;
        uint32              dsp_pp_i;
        SOC_TMC_OFP_RATES_TBL_INFO *ofp_rates_tbl = &flow_and_up_info->ofp_rates_table;

        res = arad_port_to_interface_map_get(
                unit,
                core,
                flow_and_up_info->sch_port_id[credit_source_i],
                &if_id,
                NULL
        );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 52, exit);

        
        for (ofp_rate_tbl->nof_valid_entries = 0, dsp_pp_i = 0; dsp_pp_i < ARAD_NOF_FAP_PORTS; ++dsp_pp_i)
        {
            res = arad_port_to_interface_map_get(
                    unit,
                    core,
                    dsp_pp_i,
                    &if_id,
                    NULL
            );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

            if (if_id == source_if_id)
            {
                res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, dsp_pp_i, &base_q_pair);
                SOC_SAND_SOC_IF_ERROR_RETURN(res,63,exit);

                if (base_q_pair == ARAD_NOF_FAP_PORTS)
                {
                    SOC_SAND_SET_ERROR_CODE(ARAD_OFP_RATES_INVALID_PORT_ID_ERR, 55, exit);
                }
                
                res = arad_ofp_rates_sch_single_port_rate_hw_get(unit,core, dsp_pp_i,&ofp_rate_tbl->rates[flow_and_up_info->ofp_rates_tbl.nof_valid_entries].sch_rate);
                if (res != SOC_E_NONE) {
                    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR,65,exit);
                }
                
                res = arad_ofp_rates_egq_single_port_rate_hw_get(unit,core,dsp_pp_i,&ofp_rate_tbl->rates[flow_and_up_info->ofp_rates_tbl.nof_valid_entries].egq_rate);
                if (res != SOC_E_NONE) {
                    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR,75,exit);
                }
                
                res = arad_ofp_rates_single_port_max_burst_get(unit,core,dsp_pp_i,&ofp_rate_tbl->rates[flow_and_up_info->ofp_rates_tbl.nof_valid_entries].max_burst);
                if (res != SOC_E_NONE) {
                    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR,85,exit);
                }

                ofp_rates_tbl->rates[ofp_rate_tbl->nof_valid_entries++].port_id = dsp_pp_i;
            }
        }

        for (dsp_pp_i = ofp_rate_tbl->nof_valid_entries; dsp_pp_i < ARAD_EGR_NOF_BASE_Q_PAIRS; ++dsp_pp_i)
        {
            arad_ARAD_OFP_RATE_INFO_clear(ofp_rate_tbl->rates + dsp_pp_i);
        }
        flow_and_up_info->ofp_rate_valid = 1;
    }
#endif
    exit:

    ARAD_FREE(dummy_shaper);
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_flow_and_up_info_get_unsafe()",0,0);
}

#endif



#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif 
