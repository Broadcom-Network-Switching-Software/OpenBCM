/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INGRESS




#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_ingress_scheduler.h>

#include <soc/dpp/ARAD/arad_reg_access.h>

#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>

#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>

#include <soc/mcm/memregs.h>






#define ARAD_CONVERSION_TEST_REGRESSION_STEP 9999
#define ARAD_CONVERSION_TEST_MAX_RATE_DIFF 250000

#define ARAD_ING_SCH_CLOS_NOF_SHAPERS (ARAD_ING_SCH_CLOS_NOF_GLOBAL_SHAPERS + ARAD_ING_SCH_CLOS_NOF_HP_SHAPERS + ARAD_ING_SCH_CLOS_NOF_LP_SHAPERS)


#define ARAD_IPT_MC_SLOW_START_TIMER_MAX    (17)





















STATIC uint32
  arad_ingress_scheduler_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_REGS_INIT);

  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_ENABLEf,  0 ));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_5_SLOW_START_ENABLEf,  0 ));
 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_CFG_TIMER_PERIOD_0f,  ARAD_IPT_MC_SLOW_START_TIMER_MAX ));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_CFG_TIMER_PERIOD_1f,  ARAD_IPT_MC_SLOW_START_TIMER_MAX ));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_5_SLOW_START_CFG_TIMER_PERIOD_0f,  ARAD_IPT_MC_SLOW_START_TIMER_MAX ));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_5_SLOW_START_CFG_TIMER_PERIOD_1f,  ARAD_IPT_MC_SLOW_START_TIMER_MAX ));
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_regs_init()",0,0);
}


soc_error_t
  arad_ingress_scheduler_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_ingress_scheduler_regs_init(
          unit
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:    
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_ingress_scheduler_mesh_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO   *exact_mesh_info
  )
{
  uint32
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_MESH_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(mesh_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_mesh_info);

  SOC_SAND_MAGIC_NUM_VERIFY(mesh_info);
  SOC_SAND_MAGIC_NUM_VERIFY(exact_mesh_info);

  for (index = 0;index < ARAD_ING_SCH_NUM_OF_CONTEXTS;index++)
  {
    if (mesh_info->contexts[index].weight > ARAD_ING_SCH_MAX_WEIGHT_VALUE)
    {
      
      SOC_SAND_SET_ERROR_CODE(ARAD_ING_SCH_WEIGHT_OUT_OF_RANGE_ERR, 10, exit);
    }
    if (mesh_info->contexts[index].shaper.max_burst > ARAD_ING_SCH_MAX_MAX_CREDIT_VALUE)
    {
      
      SOC_SAND_SET_ERROR_CODE(ARAD_ING_SCH_MAX_CREDIT_OUT_OF_RANGE_ERR, 20, exit);
    }
    if (mesh_info->contexts[index].id > ARAD_ING_SCH_MAX_ID_VALUE)
    {
      
      SOC_SAND_SET_ERROR_CODE(ARAD_ING_SCH_MESH_ID_OUT_OF_RANGE_ERR, 30, exit);
    }
  }
  if (mesh_info->total_rate_shaper.max_burst > ARAD_ING_SCH_MAX_MAX_CREDIT_VALUE)
  {
    
    SOC_SAND_SET_ERROR_CODE(ARAD_ING_SCH_MAX_CREDIT_OUT_OF_RANGE_ERR, 40, exit);
  }
  if (mesh_info->nof_entries > ARAD_ING_SCH_MAX_NOF_ENTRIES)
  {
    
    SOC_SAND_SET_ERROR_CODE(ARAD_ING_SCH_MESH_NOF_ENTRIES_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_mesh_verify()",0,0);
}


uint32
  arad_ingress_scheduler_mesh_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO   *exact_mesh_info
  )
{
  uint32
    res,
    exact_total_max_rate,
    exact_max_rate;
  uint32
    index;
  ARAD_ING_SCH_MESH_CONTEXT_INFO
    current_context;
   
  reg_field 
      wfq_weights [ARAD_ING_SCH_NUM_OF_CONTEXTS],
      shaper_max_crdts[ARAD_ING_SCH_NUM_OF_CONTEXTS],
      shaper_delays[ARAD_ING_SCH_NUM_OF_CONTEXTS],
      shaper_cals[ARAD_ING_SCH_NUM_OF_CONTEXTS];
  
  reg_field max_credit, delay, cal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_MESH_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mesh_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_mesh_info);

  res = arad_ingress_scheduler_mesh_reg_flds_db_get(
          unit,
          wfq_weights,
          shaper_max_crdts,
          shaper_delays,
          shaper_cals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

  soc_sand_os_memcpy(
    exact_mesh_info,
    mesh_info,
    sizeof(ARAD_ING_SCH_MESH_INFO)
  );

  for (index=0;index<(mesh_info->nof_entries);index++)
  {
    current_context = mesh_info->contexts[index];

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, wfq_weights[current_context.id].reg, REG_PORT_ANY, 0, wfq_weights[current_context.id].field,  current_context.weight));

    
    res = arad_ingress_scheduler_shaper_values_set(
            unit,
            TRUE,
            &(current_context.shaper),
            &(shaper_max_crdts[current_context.id]),
            &(shaper_delays[current_context.id]),
            &(shaper_cals[current_context.id]),
            &exact_max_rate
          );

    SOC_SAND_CHECK_FUNC_RESULT(res,30,exit);

    exact_mesh_info->contexts[index].shaper.max_rate = exact_max_rate;

  }

  
  max_credit.reg = IPT_SHAPER_8_MAX_CREDITr;
  max_credit.field = SHAPER_8_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_8_DELAYr;
  delay.field = SHAPER_8_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_8_CALr;      
  cal.field = SHAPER_8_CALf;
  cal.index = 0;
    
  res = arad_ingress_scheduler_shaper_values_set(
          unit,
          TRUE,
          &mesh_info->total_rate_shaper,
          &max_credit,
          &delay,
          &cal,
          &exact_total_max_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,40,exit);

  exact_mesh_info->total_rate_shaper.max_rate = exact_total_max_rate;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_mesh_set_unsafe()",0,0);
}


uint32
  arad_ingress_scheduler_mesh_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO   *mesh_info
  )
{
  uint32
    res,
    index;
  ARAD_ING_SCH_MESH_CONTEXT_INFO
    current_context;
  ARAD_ING_SCH_SHAPER
    current_shaper;
  reg_field
    wfq_weights[ARAD_ING_SCH_NUM_OF_CONTEXTS],
    shaper_max_crdts[ARAD_ING_SCH_NUM_OF_CONTEXTS],
    shaper_delays[ARAD_ING_SCH_NUM_OF_CONTEXTS],
    shaper_cals[ARAD_ING_SCH_NUM_OF_CONTEXTS];
   
  reg_field max_credit, delay, cal;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_MESH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mesh_info);
  
  arad_ARAD_ING_SCH_MESH_INFO_clear(mesh_info);

  res = arad_ingress_scheduler_mesh_reg_flds_db_get(
          unit,
          wfq_weights,
          shaper_max_crdts,
          shaper_delays,
          shaper_cals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

  for (index=0;index<ARAD_ING_SCH_NUM_OF_CONTEXTS;index++)
  {
    current_context.id = index;

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, wfq_weights[index].reg, REG_PORT_ANY, 0, wfq_weights[index].field, &current_context.weight));

    
    res = arad_ingress_scheduler_shaper_values_get(
            unit,
            TRUE,
            &(shaper_max_crdts[index]),
            &(shaper_delays[index]),
            &(shaper_cals[index]),
            &(current_context.shaper)
          );

    SOC_SAND_CHECK_FUNC_RESULT(res,30,exit);

    soc_sand_os_memcpy(
      &(mesh_info->contexts[index]),
      &(current_context),
      sizeof(ARAD_ING_SCH_MESH_CONTEXT_INFO)
    );
  }

  
  max_credit.reg = IPT_SHAPER_8_MAX_CREDITr;
  max_credit.field = SHAPER_8_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_8_DELAYr;
  delay.field = SHAPER_8_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_8_CALr;      
  cal.field = SHAPER_8_CALf;
  cal.index = 0;
  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &current_shaper
        );

  SOC_SAND_CHECK_FUNC_RESULT(res,40,exit);

  soc_sand_os_memcpy(
    &(mesh_info->total_rate_shaper),
    &(current_shaper),
    sizeof(ARAD_ING_SCH_SHAPER)
  );
  mesh_info->nof_entries = ARAD_ING_SCH_NUM_OF_CONTEXTS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_mesh_get_unsafe()",0,0);
}



uint32
  arad_ingress_scheduler_shaper_values_get(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   int                 is_delay_2_clocks,
    SOC_SAND_IN   reg_field              *max_credit,
    SOC_SAND_IN   reg_field              *delay,
    SOC_SAND_IN   reg_field              *cal,
    SOC_SAND_OUT  ARAD_ING_SCH_SHAPER    *shaper
  )
{

  uint32
    res;
  uint32
    shaper_delay_2_clocks,
    shaper_cal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_SHAPER_VALUES_GET);

  SOC_SAND_CHECK_NULL_INPUT(shaper);

  

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, max_credit->reg, REG_PORT_ANY, max_credit->index, max_credit->field, &shaper->max_burst));

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, delay->reg, REG_PORT_ANY, delay->index, delay->field, &shaper_delay_2_clocks));

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, cal->reg, REG_PORT_ANY, cal->index, cal->field, &shaper_cal));

  
  if (is_delay_2_clocks) 
  {
      shaper_delay_2_clocks *=2;
  }

  res = arad_ingress_scheduler_delay_cal_to_max_rate_form(
          unit,
          shaper_delay_2_clocks,
          shaper_cal,
          &(shaper->max_rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,4,exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_shaper_values_get()",0,0);
}



uint32
  arad_ingress_scheduler_delay_cal_to_max_rate_form(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint32          shaper_delay,
    SOC_SAND_IN  uint32          shaper_cal,
    SOC_SAND_OUT uint32          *max_rate
  )
{
  uint32
    res,
    device_ticks_per_sec,
    exact_rate_in_kbits_per_sec;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_DELAY_CAL_TO_MAX_RATE_FORM);

  SOC_SAND_CHECK_NULL_INPUT(max_rate);

  device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

  if (shaper_cal == 0)
  {
    exact_rate_in_kbits_per_sec = 0;
  }
  else
  {
    res = soc_sand_clocks_to_kbits_per_sec(
            shaper_delay,
            shaper_cal,
            device_ticks_per_sec,
            &exact_rate_in_kbits_per_sec
            );
    if (res == SOC_SAND_OVERFLOW_ERR)
    {
      
      exact_rate_in_kbits_per_sec = ARAD_ING_SCH_MAX_MAX_CREDIT_VALUE;
    }
  }

  *max_rate = exact_rate_in_kbits_per_sec;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_delay_cal_to_max_rate_form()",0,0);
}


uint32
  arad_ingress_scheduler_mesh_reg_flds_db_get(
    SOC_SAND_IN  int       unit,
    reg_field          wfq_weights[ARAD_ING_SCH_NUM_OF_CONTEXTS],
    reg_field          shaper_max_crdts[ARAD_ING_SCH_NUM_OF_CONTEXTS],
    reg_field          shaper_delays[ARAD_ING_SCH_NUM_OF_CONTEXTS],
    reg_field          shaper_cals[ARAD_ING_SCH_NUM_OF_CONTEXTS]
    )
{ 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_MESH_REG_FLDS_DB_GET);

  
  
  
  wfq_weights[0].field = WFQ_0_WEIGHTf;
  wfq_weights[0].reg = IPT_WFQ_WEIGHT_0r;
  wfq_weights[0].index = 0;
  wfq_weights[1].field = WFQ_1_WEIGHTf;
  wfq_weights[1].reg = IPT_WFQ_WEIGHT_0r;
  wfq_weights[1].index = 0;
  wfq_weights[2].field = WFQ_2_WEIGHTf;
  wfq_weights[2].reg = IPT_WFQ_WEIGHT_0r;
  wfq_weights[2].index = 0;
  wfq_weights[3].field = WFQ_3_WEIGHTf;
  wfq_weights[3].reg = IPT_WFQ_WEIGHT_0r;
  wfq_weights[3].index = 0;

  wfq_weights[4].field = WFQ_4_WEIGHTf;
  wfq_weights[4].reg = IPT_WFQ_WEIGHT_1r;
  wfq_weights[4].index = 0;
  wfq_weights[5].field = WFQ_5_WEIGHTf;
  wfq_weights[5].reg = IPT_WFQ_WEIGHT_1r;
  wfq_weights[5].index = 0;
  wfq_weights[6].field = WFQ_6_WEIGHTf;
  wfq_weights[6].reg = IPT_WFQ_WEIGHT_1r;
  wfq_weights[6].index = 0;
  wfq_weights[7].field = WFQ_7_WEIGHTf;
  wfq_weights[7].reg = IPT_WFQ_WEIGHT_1r;
  wfq_weights[7].index = 0;
  
  
  shaper_max_crdts[0].field = SHAPER_0_MAX_CREDITf;
  shaper_max_crdts[0].reg = IPT_SHAPER_01_MAX_CREDITr;
  shaper_max_crdts[0].index = 0;
  shaper_max_crdts[1].field = SHAPER_1_MAX_CREDITf;
  shaper_max_crdts[1].reg = IPT_SHAPER_01_MAX_CREDITr;
  shaper_max_crdts[1].index = 0;
  
  shaper_max_crdts[2].field = SHAPER_2_MAX_CREDITf;
  shaper_max_crdts[2].reg = IPT_SHAPER_23_MAX_CREDITr;
  shaper_max_crdts[2].index = 0;
  shaper_max_crdts[3].field = SHAPER_3_MAX_CREDITf;
  shaper_max_crdts[3].reg = IPT_SHAPER_23_MAX_CREDITr;  
  shaper_max_crdts[3].index = 0;

  shaper_max_crdts[4].field = SHAPER_4_MAX_CREDITf;
  shaper_max_crdts[4].reg = IPT_SHAPER_45_MAX_CREDITr;
  shaper_max_crdts[4].index = 0;
  shaper_max_crdts[5].field = SHAPER_5_MAX_CREDITf;
  shaper_max_crdts[5].reg = IPT_SHAPER_45_MAX_CREDITr;
  shaper_max_crdts[5].index = 0;

  shaper_max_crdts[6].field = SHAPER_6_MAX_CREDITf;
  shaper_max_crdts[6].reg = IPT_SHAPER_67_MAX_CREDITr;
  shaper_max_crdts[6].index = 0;
  shaper_max_crdts[7].field = SHAPER_7_MAX_CREDITf;
  shaper_max_crdts[7].reg = IPT_SHAPER_67_MAX_CREDITr;
  shaper_max_crdts[7].index = 0;    

  
  shaper_delays[0].field = SHAPER_0_DELAYf;
  shaper_delays[0].reg = IPT_SHAPER_01_DELAYr;
  shaper_delays[0].index = 0;
  shaper_delays[1].field = SHAPER_1_DELAYf;
  shaper_delays[1].reg = IPT_SHAPER_01_DELAYr;
  shaper_delays[1].index = 0;

  shaper_delays[2].field = SHAPER_2_DELAYf;
  shaper_delays[2].reg = IPT_SHAPER_23_DELAYr;
  shaper_delays[2].index = 0;
  shaper_delays[3].field = SHAPER_3_DELAYf;
  shaper_delays[3].reg = IPT_SHAPER_23_DELAYr;
  shaper_delays[3].index = 0;

  shaper_delays[4].field = SHAPER_4_DELAYf;
  shaper_delays[4].reg = IPT_SHAPER_45_DELAYr;
  shaper_delays[4].index = 0;
  shaper_delays[5].field = SHAPER_5_DELAYf;
  shaper_delays[5].reg = IPT_SHAPER_45_DELAYr;
  shaper_delays[5].index = 0;
  
  shaper_delays[6].field = SHAPER_6_DELAYf;
  shaper_delays[6].reg = IPT_SHAPER_67_DELAYr;
  shaper_delays[6].index = 0;
  shaper_delays[7].field = SHAPER_7_DELAYf;
  shaper_delays[7].reg = IPT_SHAPER_67_DELAYr;
  shaper_delays[7].index = 0;

   
  shaper_cals[0].field = SHAPER_0_CALf;
  shaper_cals[0].reg = IPT_SHAPER_01_CALr;
  shaper_cals[0].index= 0;
  shaper_cals[1].field = SHAPER_1_CALf;
  shaper_cals[1].reg = IPT_SHAPER_01_CALr;
  shaper_cals[1].index= 0;

  shaper_cals[2].field = SHAPER_2_CALf;
  shaper_cals[2].reg = IPT_SHAPER_23_CALr;
  shaper_cals[2].index= 0;
  shaper_cals[3].field = SHAPER_3_CALf;
  shaper_cals[3].reg = IPT_SHAPER_23_CALr;
  shaper_cals[3].index= 0;

  shaper_cals[4].field = SHAPER_4_CALf;
  shaper_cals[4].reg = IPT_SHAPER_45_CALr;
  shaper_cals[4].index= 0;
  shaper_cals[5].field = SHAPER_5_CALf;
  shaper_cals[5].reg = IPT_SHAPER_45_CALr;
  shaper_cals[5].index= 0;

  shaper_cals[6].field = SHAPER_6_CALf;
  shaper_cals[6].reg = IPT_SHAPER_67_CALr;
  shaper_cals[6].index= 0;
  shaper_cals[7].field = SHAPER_7_CALf;
  shaper_cals[7].reg = IPT_SHAPER_67_CALr;
  shaper_cals[7].index= 0;

  

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_mesh_reg_flds_db_get()",0,0);
}


uint32
  arad_ingress_scheduler_clos_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *exact_clos_info
  )
{
  uint32
    index,
    weights[ARAD_ING_SCH_NUM_OF_CONTEXTS],
    shapers_max_burst[ARAD_ING_SCH_CLOS_NOF_SHAPERS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_clos_info);

  SOC_SAND_MAGIC_NUM_VERIFY(clos_info);
  SOC_SAND_MAGIC_NUM_VERIFY(exact_clos_info);

  if(clos_info->weights.global_hp.weight2 != 1) {
      
      SOC_SAND_SET_ERROR_CODE(ARAD_ING_SCH_WEIGHT_OUT_OF_RANGE_ERR, 1, exit);
  }

  weights[0] = clos_info->weights.fabric_lp.weight1;
  weights[1] = clos_info->weights.fabric_lp.weight2;
  weights[2] = clos_info->weights.fabric_hp.weight1;
  weights[3] = clos_info->weights.fabric_hp.weight2;
  weights[4] = clos_info->weights.global_lp.weight1;
  weights[5] = clos_info->weights.global_lp.weight2;
  weights[6] = clos_info->weights.global_hp.weight1;
  weights[7] = clos_info->weights.global_hp.weight2;

  shapers_max_burst[0] = clos_info->shapers.local.max_burst;
  shapers_max_burst[1] = clos_info->shapers.fabric.max_burst;
  shapers_max_burst[2] = clos_info->shapers.hp.local.max_burst;
  shapers_max_burst[3] = clos_info->shapers.hp.fabric_unicast.max_burst;
  shapers_max_burst[4] = clos_info->shapers.hp.fabric_multicast.max_burst;
  shapers_max_burst[5] = clos_info->shapers.lp.fabric_multicast.max_burst;
  shapers_max_burst[6] = clos_info->shapers.lp.fabric_unicast.max_burst;

  for (index = 0 ;index < ARAD_ING_SCH_NUM_OF_CONTEXTS ; ++index)
  {
    if (weights[index] > ARAD_ING_SCH_MAX_WEIGHT_VALUE)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_ING_SCH_WEIGHT_OUT_OF_RANGE_ERR, 10, exit);
    }
  }
  for (index = 0 ;index < ARAD_ING_SCH_CLOS_NOF_SHAPERS; ++index)
  {
    if (shapers_max_burst[index] > ARAD_ING_SCH_MAX_MAX_CREDIT_VALUE)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_ING_SCH_MAX_CREDIT_OUT_OF_RANGE_ERR, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_verify()",0,0);
}


uint32
  arad_ingress_scheduler_clos_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *exact_clos_info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_clos_info);

  soc_sand_os_memcpy(
    exact_clos_info,
    clos_info,
    sizeof(ARAD_ING_SCH_CLOS_INFO)
    );
  
  res = arad_ingress_scheduler_clos_weights_set(
          unit,
          clos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
  
  res = arad_ingress_scheduler_clos_global_shapers_set(
          unit,
          clos_info,
          exact_clos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

  
  res = arad_ingress_scheduler_clos_hp_shapers_set(
          unit,
          clos_info,
          exact_clos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,3,exit);  
  
  
  res = arad_ingress_scheduler_clos_lp_shapers_set(
          unit,
          clos_info,
          exact_clos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,4,exit);  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_set_unsafe()",0,0);
}



uint32
   arad_ingress_scheduler_clos_weights_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO*  clos_info
    )
{

  uint32
    res;

   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_WEIGHTS_SET);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2 ,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_WFQ_WEIGHT_0r, REG_PORT_ANY, 0, WFQ_2_WEIGHTf,  clos_info->weights.global_hp.weight1));
  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6 ,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_WFQ_WEIGHT_0r, REG_PORT_ANY, 0, WFQ_0_WEIGHTf,  clos_info->weights.fabric_hp.weight1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  8 ,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_WFQ_WEIGHT_0r, REG_PORT_ANY, 0, WFQ_1_WEIGHTf,  clos_info->weights.fabric_hp.weight2));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_WFQ_WEIGHT_0r, REG_PORT_ANY, 0, WFQ_3_WEIGHTf,  clos_info->weights.fabric_lp.weight1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_WFQ_WEIGHT_1r, REG_PORT_ANY, 0, WFQ_4_WEIGHTf,  clos_info->weights.fabric_lp.weight2));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_WFQ_WEIGHT_1r, REG_PORT_ANY, 0, WFQ_5_WEIGHTf,  clos_info->weights.global_lp.weight1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_WFQ_WEIGHT_1r, REG_PORT_ANY, 0, WFQ_6_WEIGHTf,  clos_info->weights.global_lp.weight2));
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_weights_set()",0,0);
}


uint32
   arad_ingress_scheduler_clos_global_shapers_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info,
     SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *exact_clos_info
   )
{

  uint32
    res,
    exact_local_max_rate,
    exact_fab_max_rate;
   
  reg_field max_credit, delay, cal;   

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_GLOBAL_SHAPERS_SET);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_clos_info);

  
  

  
  max_credit.reg = IPT_SHAPER_01_MAX_CREDITr;
  max_credit.field = SHAPER_0_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_01_DELAYr;
  delay.field = SHAPER_0_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_01_CALr;      
  cal.field = SHAPER_0_CALf;
  cal.index = 0;
    
  res = arad_ingress_scheduler_shaper_values_set(
        unit,
        TRUE,
        &(clos_info->shapers.local),
        &max_credit,
        &delay,
        &cal,
        &exact_local_max_rate
        );

  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
  exact_clos_info->shapers.local.max_rate = exact_local_max_rate;

  max_credit.field = SHAPER_1_MAX_CREDITf;
  delay.field = SHAPER_1_DELAYf;
  cal.field = SHAPER_1_CALf;
  
  
  res = arad_ingress_scheduler_shaper_values_set(
          unit,
          TRUE,
          &(clos_info->shapers.fabric),
          &max_credit,
          &delay,
          &cal,
          &exact_fab_max_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);
  exact_clos_info->shapers.fabric.max_rate = exact_fab_max_rate;

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_global_shapers_set()",0,0);
}


uint32
  arad_ingress_scheduler_clos_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *clos_info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);

  arad_ARAD_ING_SCH_CLOS_INFO_clear(clos_info);

  
  res = arad_ingress_scheduler_clos_weights_get(
          unit,
          clos_info
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = arad_ingress_scheduler_clos_global_shapers_get(
          unit,
          clos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit);

  
  res = arad_ingress_scheduler_clos_hp_shapers_get(
          unit,
          clos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,30,exit);
  
  
  res = arad_ingress_scheduler_clos_lp_shapers_get(
          unit,
          clos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,40,exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_get_unsafe()",0,0);
}


uint32
   arad_ingress_scheduler_clos_weights_get(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_OUT  ARAD_ING_SCH_CLOS_INFO   *clos_info
    )
{

  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_WEIGHTS_GET);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  0,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_WFQ_WEIGHT_0r, REG_PORT_ANY, 0, WFQ_2_WEIGHTf, &clos_info->weights.global_hp.weight1));
  clos_info->weights.global_hp.weight2 = 1; 

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_WFQ_WEIGHT_0r, REG_PORT_ANY, 0, WFQ_0_WEIGHTf, &clos_info->weights.fabric_hp.weight1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_WFQ_WEIGHT_0r, REG_PORT_ANY, 0, WFQ_1_WEIGHTf, &clos_info->weights.fabric_hp.weight2));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  8,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_WFQ_WEIGHT_0r, REG_PORT_ANY, 0, WFQ_3_WEIGHTf, &clos_info->weights.fabric_lp.weight1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_WFQ_WEIGHT_1r, REG_PORT_ANY, 0, WFQ_4_WEIGHTf, &clos_info->weights.fabric_lp.weight2));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_WFQ_WEIGHT_1r, REG_PORT_ANY, 0, WFQ_5_WEIGHTf, &clos_info->weights.global_lp.weight1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_WFQ_WEIGHT_1r, REG_PORT_ANY, 0, WFQ_6_WEIGHTf, &clos_info->weights.global_lp.weight2));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_weights_get()",0,0);
}


uint32
   arad_ingress_scheduler_clos_global_shapers_get(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_OUT  ARAD_ING_SCH_CLOS_INFO   *clos_info
    )
{
  uint32
    res;
   
  reg_field max_credit, delay, cal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_GLOBAL_SHAPERS_GET);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);

  

  
  max_credit.reg = IPT_SHAPER_01_MAX_CREDITr;
  max_credit.field = SHAPER_0_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_01_DELAYr;
  delay.field = SHAPER_0_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_01_CALr;
  cal.field = SHAPER_0_CALf;
  cal.index = 0;
  
  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &(clos_info->shapers.local)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);

  
  max_credit.field = SHAPER_1_MAX_CREDITf;
  delay.field = SHAPER_1_DELAYf;
  cal.field = SHAPER_1_CALf;
  
  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &(clos_info->shapers.fabric)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_global_shapers_get()",0,0);
}


uint32
  arad_ingress_scheduler_shaper_values_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int                  is_delay_2_clocks,
    SOC_SAND_IN  ARAD_ING_SCH_SHAPER     *shaper,
    SOC_SAND_IN  reg_field               *max_credit,
    SOC_SAND_IN  reg_field               *delay,
    SOC_SAND_IN  reg_field               *cal,
    SOC_SAND_OUT uint32                  *exact_max_rate
  )
{

  uint32
    res;

  uint32
    shaper_delay_2_clocks,
    shaper_cal,
    exact;
  ARAD_ING_SCH_SHAPER
    out_shaper_struct;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_SHAPER_VALUES_SET);

  SOC_SAND_CHECK_NULL_INPUT(shaper);

  

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, max_credit->reg, REG_PORT_ANY, max_credit->index, max_credit->field,  shaper->max_burst));

  if(shaper->max_rate != SOC_TMC_ING_SCH_DONT_TOUCH) {
      
      res = arad_ingress_scheduler_rate_to_delay_cal_form(
           unit,
           shaper->max_rate,
           is_delay_2_clocks,
           &shaper_delay_2_clocks,
           &shaper_cal,
           &exact
      );
      SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, delay->reg, REG_PORT_ANY, delay->index, delay->field,  shaper_delay_2_clocks));

      

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, cal->reg, REG_PORT_ANY, cal->index, cal->field,  shaper_cal));

      *exact_max_rate = exact;
  } else {
      out_shaper_struct=*shaper;
      res = arad_ingress_scheduler_shaper_values_get(
          unit,
          is_delay_2_clocks,
          max_credit,
          delay,
          cal,
          &out_shaper_struct
      );
      SOC_SAND_CHECK_FUNC_RESULT(res,6,exit);
      *exact_max_rate = out_shaper_struct.max_rate;
  }

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_shaper_values_set()",0,0);
}


uint32
  arad_ingress_scheduler_rate_to_delay_cal_form(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32             max_rate,
    SOC_SAND_IN  int                is_delay_2_clocks,
    SOC_SAND_OUT uint32*            shaper_delay_2_clocks,
    SOC_SAND_OUT uint32*            shaper_cal,
    SOC_SAND_OUT uint32*            exact_max_rate
  )
{
  uint32
    res,
    device_ticks_per_sec,
    delay_value,
    cal_value,
    exact_cal_value_long,
    exact_rate_in_kbits_per_sec,
    divider;
  uint8
    cal_and_delay_in_range = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_RATE_TO_DELAY_CAL_FORM);

  SOC_SAND_CHECK_NULL_INPUT(shaper_delay_2_clocks);
  SOC_SAND_CHECK_NULL_INPUT(shaper_cal);

  SOC_SAND_CHECK_NULL_INPUT(exact_max_rate);

  cal_value = ARAD_ING_SCH_FIRST_CAL_VAL;

  if(is_delay_2_clocks) {
      divider = 2;
  } else {
      divider = 1;
  }

  device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

  if (0 == max_rate)
  {
    *shaper_cal = ARAD_ING_SCH_MIN_CAL_VAL;
    *shaper_delay_2_clocks = ARAD_ING_SCH_MAX_DELAY_VAL;
    *exact_max_rate = 0;
    goto exit;
  }

  while ((cal_value > 1) && \
    (cal_value < ARAD_ING_SCH_MAX_CAL_VAL) && \
    !(cal_and_delay_in_range)
  )
  {
    res = soc_sand_kbits_per_sec_to_clocks(
            max_rate,
            cal_value,
            device_ticks_per_sec,
            &delay_value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    if (delay_value > (ARAD_ING_SCH_MAX_DELAY_VAL*divider))
    {
      cal_value /= 2;
    }
    else if (delay_value < (ARAD_ING_SCH_MIN_DELAY_VAL*divider))
    {
      cal_value *= 2;
      if (cal_value > ARAD_ING_SCH_MAX_CAL_VAL)
      {
        cal_value = ARAD_ING_SCH_MAX_CAL_VAL;
      }
    }
    else
    {
      cal_and_delay_in_range = TRUE;
    }
  }

  res = soc_sand_kbits_per_sec_to_clocks(
          max_rate,
          cal_value,
          device_ticks_per_sec,
          &delay_value
         );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit);

  if(delay_value > (ARAD_ING_SCH_MAX_DELAY_VAL*divider))
  {
    delay_value = (ARAD_ING_SCH_MAX_DELAY_VAL*divider);
  }

  
  if(is_delay_2_clocks) {
      delay_value = (((delay_value + 1) / 2)*2);
  }

  res = soc_sand_clocks_to_kbits_per_sec(
          delay_value,
          cal_value,
          device_ticks_per_sec,
          &exact_rate_in_kbits_per_sec
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  arad_ingress_scheduler_exact_cal_value(
          cal_value,
          max_rate,
          exact_rate_in_kbits_per_sec,
          &exact_cal_value_long
        );

  if (exact_cal_value_long > ARAD_ING_SCH_MAX_CAL_VAL)
  {
    exact_cal_value_long = ARAD_ING_SCH_MAX_CAL_VAL;
    SOC_SAND_SET_ERROR_CODE(ARAD_ING_SCH_EXACT_CAL_LARGER_THAN_MAXIMUM_VALUE_ERR, 40, exit);
  }

  res = soc_sand_clocks_to_kbits_per_sec(
          delay_value,
          exact_cal_value_long,
          device_ticks_per_sec,
          &exact_rate_in_kbits_per_sec
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  *shaper_delay_2_clocks = delay_value / divider;
  *shaper_cal = exact_cal_value_long;
  *exact_max_rate = exact_rate_in_kbits_per_sec;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_rate_to_delay_cal_form()",0,0);
}


uint32
  arad_ingress_scheduler_exact_cal_value(
    SOC_SAND_IN uint32  cal_value,
    SOC_SAND_IN uint32  max_rate,
    SOC_SAND_IN uint32  exact_rate_in_kbits_per_sec,
    SOC_SAND_OUT uint32 *exact_cal_value_long
  )
{

  SOC_SAND_U64
    tmp_result,
    exact_cal_value_u64,
    round_helper;

  soc_sand_u64_clear(&tmp_result);

  soc_sand_u64_clear(&exact_cal_value_u64);

  soc_sand_u64_multiply_longs(
    cal_value,
    max_rate,
    &tmp_result
  );

  soc_sand_u64_devide_u64_long(
    &tmp_result,
    exact_rate_in_kbits_per_sec,
    &exact_cal_value_u64
  );

  soc_sand_u64_to_long(
    &exact_cal_value_u64,
    exact_cal_value_long
  );

  soc_sand_u64_multiply_longs(
    *exact_cal_value_long,
    exact_rate_in_kbits_per_sec,
    &round_helper
  );

  if(soc_sand_u64_is_bigger(&tmp_result,&round_helper))
  {
    *exact_cal_value_long = *exact_cal_value_long + 1;
  }

  if(*exact_cal_value_long == 0)
  {
    *exact_cal_value_long = 1;
  }

  return 0;
}


uint8
  arad_ingress_scheduler_conversion_test(
    SOC_SAND_IN uint8 is_regression,
    SOC_SAND_IN uint8 silent
  )
{

  uint32
    ret,
    index,
    rate = ARAD_ING_SCH_MAX_RATE_MIN,
    tmp_delay,
    tmp_cal,
    exact,
    err_percent,
    max_err_percent=0;

  uint8
    pass = TRUE;

   for (index = ARAD_ING_SCH_MAX_RATE_MIN; index < ARAD_ING_SCH_MAX_RATE_MAX; index = (index + 1))
   {
     if(rate > 1000 && is_regression)
     {
       index += ARAD_CONVERSION_TEST_REGRESSION_STEP;
     }
     rate = index;

     ret = arad_ingress_scheduler_rate_to_delay_cal_form(
       0,
       rate,
       TRUE,
       &tmp_delay,
       &tmp_cal,
       &exact
       );

     if(soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
     {
       if (!silent)
       {
         LOG_INFO(BSL_LS_SOC_INGRESS,
                  (BSL_META("arad_ingress_scheduler_conversion_test:"
                            "arad_ingress_scheduler_rate_to_delay_cal_form FAIL (100)"
                            "\n\r"
                      )));
       }
       pass = FALSE;
       goto exit;
     }

     if(exact < rate)
     {
       if (!silent)
       {
         LOG_INFO(BSL_LS_SOC_INGRESS,
                  (BSL_META("arad_ingress_scheduler_conversion_test: FAIL (200)"
                            "exact rate value is smaller than rate"
                            " \n\r"
                      )));
       }
       pass = FALSE;
       goto exit;
     }

     if((exact - rate) > ARAD_CONVERSION_TEST_MAX_RATE_DIFF)
     {
       if (!silent)
       {
         LOG_INFO(BSL_LS_SOC_INGRESS,
                  (BSL_META("arad_ingress_scheduler_conversion_test: FAIL (300)"
                            "difference between exact_rate and rate is OUT OF LIMIT"
                            "\n\r"
                      )));
       }
       pass = FALSE;
       goto exit;
     }

     err_percent = (((exact - rate)*10000)/rate);
     if (err_percent > 10)
     {
       if (!silent)
       {
         LOG_INFO(BSL_LS_SOC_INGRESS,
                  (BSL_META("arad_ingress_scheduler_conversion_test: FAIL (400)"
                            "error percentage is OUT OF LIMIT"
                            "\n\r"
                      )));
       }
        pass = FALSE;
        goto exit;
     }
     if(max_err_percent < err_percent)
     {
       max_err_percent = err_percent;
     }
   }
   if (!silent)
   {
     LOG_INFO(BSL_LS_SOC_INGRESS,
              (BSL_META("arad_ingress_scheduler_conversion_test:"
                        "\n\r"
                        "max_err_percent =  %u . %02u \n\r"),
               max_err_percent/100,
               max_err_percent%100
               ));
   }

exit:
   return pass;
}


uint32
  arad_ingress_scheduler_clos_hp_shapers_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *exact_clos_info
    )
{

  uint32
    res,
    exact_local_max_rate,
    exact_fabm_max_rate,
    exact_fabu_max_rate;
  
  reg_field max_credit, delay, cal;
  ARAD_ING_SCH_SHAPER shaper_temp;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_HP_SHAPERS_SET);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_clos_info);

  

  
  max_credit.reg = IPT_SHAPER_23_MAX_CREDITr;
  max_credit.field = SHAPER_2_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_23_DELAYr;
  delay.field = SHAPER_2_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_23_CALr;
  cal.field = SHAPER_2_CALf;
  cal.index = 0;
  
  res = arad_ingress_scheduler_shaper_values_set(
    unit,
    TRUE,
    &(clos_info->shapers.hp.local),
    &max_credit,
    &delay,
    &cal,
    &exact_local_max_rate
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
  exact_clos_info->shapers.hp.local.max_rate = exact_local_max_rate;

  
  max_credit.field = SHAPER_3_MAX_CREDITf;
  delay.field = SHAPER_3_DELAYf;
  cal.field = SHAPER_3_CALf;
  
  res = arad_ingress_scheduler_shaper_values_set(
    unit,
    TRUE,
    &(clos_info->shapers.hp.fabric_unicast),
    &max_credit,
    &delay,
    &cal,
    &exact_fabu_max_rate
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);
  exact_clos_info->shapers.hp.fabric_unicast.max_rate = exact_fabu_max_rate;

  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_45_DELAYr;
  delay.field = SHAPER_4_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_45_CALr;
  cal.field = SHAPER_4_CALf;
  cal.index = 0;
  
  res = arad_ingress_scheduler_shaper_values_set(
    unit,
    TRUE,
    &(clos_info->shapers.hp.fabric_multicast),
    &max_credit,
    &delay,
    &cal,
    &exact_fabm_max_rate
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,3,exit);
  exact_clos_info->shapers.hp.fabric_multicast.max_rate = exact_fabm_max_rate;

  
  
  if (clos_info->shapers.hp.fabric_multicast.slow_start_enable != SOC_TMC_ING_SCH_DONT_TOUCH)
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  7,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_ENABLEf,  clos_info->shapers.hp.fabric_multicast.slow_start_enable ? 1  : 0 ));
  }
  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_4_SLOW_START_DELAYr;
  delay.field = SHAPER_4_SLOW_START_DELAY_0f;
  delay.index = 0;
  cal.reg = IPT_SHAPER_4_SLOW_START_CALr;
  cal.field = SHAPER_4_SLOW_START_CAL_0f;
  cal.index = 0;

  arad_ARAD_ING_SCH_SHAPER_clear(&shaper_temp);
  if (clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_0  == SOC_TMC_ING_SCH_DONT_TOUCH) {
      shaper_temp.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
      shaper_temp.max_burst = exact_clos_info->shapers.hp.fabric_multicast.max_burst;
  } else {
      shaper_temp.max_rate = exact_clos_info->shapers.hp.fabric_multicast.max_rate * clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_0 / 100;
      shaper_temp.max_burst = exact_clos_info->shapers.hp.fabric_multicast.max_burst;
  }
  res = arad_ingress_scheduler_shaper_values_set(
     unit,
     TRUE,
     &shaper_temp,
     &max_credit,
     &delay,
     &cal,
     &exact_fabm_max_rate
  );
  SOC_SAND_CHECK_FUNC_RESULT(res,5,exit);
  if (exact_clos_info->shapers.hp.fabric_multicast.max_rate != 0)
  {
      exact_clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_0  = exact_fabm_max_rate * 100 / exact_clos_info->shapers.hp.fabric_multicast.max_rate;
  } else {
      exact_clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_0  = 0;
  }
  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_4_SLOW_START_DELAYr;
  delay.field = SHAPER_4_SLOW_START_DELAY_1f;
  delay.index = 0;
  cal.reg = IPT_SHAPER_4_SLOW_START_CALr;
  cal.field = SHAPER_4_SLOW_START_CAL_1f;
  cal.index = 0;

  arad_ARAD_ING_SCH_SHAPER_clear(&shaper_temp);
  if (clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_1  == SOC_TMC_ING_SCH_DONT_TOUCH) {
      shaper_temp.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
      shaper_temp.max_burst = exact_clos_info->shapers.hp.fabric_multicast.max_burst;
  } else {
      shaper_temp.max_rate = exact_clos_info->shapers.hp.fabric_multicast.max_rate * clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_1 / 100;
      shaper_temp.max_burst = exact_clos_info->shapers.hp.fabric_multicast.max_burst;
  }
  res = arad_ingress_scheduler_shaper_values_set(
     unit,
     TRUE,
     &shaper_temp,
     &max_credit,
     &delay,
     &cal,
     &exact_fabm_max_rate
  );
  SOC_SAND_CHECK_FUNC_RESULT(res,6,exit);
  if (exact_clos_info->shapers.hp.fabric_multicast.max_rate != 0)
  {
      exact_clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_1  = exact_fabm_max_rate * 100 / exact_clos_info->shapers.hp.fabric_multicast.max_rate;
  } else {
      exact_clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_1  = 0;
  }


  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_hp_shapers_set()",0,0);
}



uint32
  arad_ingress_scheduler_clos_hp_shapers_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  ARAD_ING_SCH_CLOS_INFO   *clos_info
  )
{
  uint32
    res;

  reg_field max_credit, delay, cal;
  ARAD_ING_SCH_SHAPER shaper_temp;
  uint32 slow_start_enable;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_HP_SHAPERS_GET);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);

  

  
  max_credit.reg = IPT_SHAPER_23_MAX_CREDITr;
  max_credit.field = SHAPER_2_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_23_DELAYr;
  delay.field = SHAPER_2_DELAYf;
  delay.index = 0;;
  cal.reg = IPT_SHAPER_23_CALr;
  cal.field = SHAPER_2_CALf;
  cal.index = 0;
  
  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &(clos_info->shapers.hp.local)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);

  
  max_credit.field = SHAPER_3_MAX_CREDITf;
  delay.field = SHAPER_3_DELAYf;
  cal.field = SHAPER_3_CALf;

  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &(clos_info->shapers.hp.fabric_unicast)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_45_DELAYr;
  delay.field = SHAPER_4_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_45_CALr;
  cal.field = SHAPER_4_CALf;
  cal.index = 0;

  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &(clos_info->shapers.hp.fabric_multicast)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,3,exit);

  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_ENABLEf, &slow_start_enable));
  clos_info->shapers.hp.fabric_multicast.slow_start_enable = (slow_start_enable == 0) ? 0 : 1;
  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_4_SLOW_START_DELAYr;
  delay.field = SHAPER_4_SLOW_START_DELAY_0f;
  delay.index = 0;
  cal.reg = IPT_SHAPER_4_SLOW_START_CALr;
  cal.field = SHAPER_4_SLOW_START_CAL_0f;
  cal.index = 0;

  arad_ARAD_ING_SCH_SHAPER_clear(&shaper_temp);
  res = arad_ingress_scheduler_shaper_values_get(
    unit,
    TRUE,
    &max_credit,
    &delay,
    &cal,
    &shaper_temp
  );
  SOC_SAND_CHECK_FUNC_RESULT(res,5,exit);
  if (clos_info->shapers.hp.fabric_multicast.max_rate != 0)
  {
      clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_0 = shaper_temp.max_rate * 100 / clos_info->shapers.hp.fabric_multicast.max_rate;
  } else {
      clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_0 = 0;
  }

  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_4_SLOW_START_DELAYr;
  delay.field = SHAPER_4_SLOW_START_DELAY_1f;
  delay.index = 0;
  cal.reg = IPT_SHAPER_4_SLOW_START_CALr;
  cal.field = SHAPER_4_SLOW_START_CAL_1f;
  cal.index = 0;

  arad_ARAD_ING_SCH_SHAPER_clear(&shaper_temp);
  res = arad_ingress_scheduler_shaper_values_get(
    unit,
    TRUE,
    &max_credit,
    &delay,
    &cal,
    &shaper_temp
  );
  SOC_SAND_CHECK_FUNC_RESULT(res,5,exit);
  if (clos_info->shapers.hp.fabric_multicast.max_rate != 0)
  {
      clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_1 = shaper_temp.max_rate * 100 / clos_info->shapers.hp.fabric_multicast.max_rate;
  } else {
      clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_1 = 0;
  }
  
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_hp_shapers_get()",0,0);
}


uint32
  arad_ingress_scheduler_clos_lp_shapers_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *exact_clos_info
    )
{

  uint32
    res,
    exact_fabm_max_rate,
    exact_fabu_max_rate;
  
  reg_field max_credit, delay, cal;
  ARAD_ING_SCH_SHAPER shaper_temp;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_LP_SHAPERS_SET);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_clos_info);

  

  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_5_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_45_DELAYr;
  delay.field = SHAPER_5_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_45_CALr;
  cal.field = SHAPER_5_CALf;
  cal.index = 0;
  
  res = arad_ingress_scheduler_shaper_values_set(
    unit,
    TRUE,
    &(clos_info->shapers.lp.fabric_multicast),
    &max_credit,
    &delay,
    &cal,
    &exact_fabm_max_rate
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
  exact_clos_info->shapers.lp.fabric_multicast.max_rate = exact_fabm_max_rate;

  
  
  if (clos_info->shapers.lp.fabric_multicast.slow_start_enable  != SOC_TMC_ING_SCH_DONT_TOUCH) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  7,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_5_SLOW_START_ENABLEf,  clos_info->shapers.lp.fabric_multicast.slow_start_enable ? 1  : 0 ));
  } 

  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_5_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_5_SLOW_START_DELAYr;
  delay.field = SHAPER_5_SLOW_START_DELAY_0f;
  delay.index = 0;
  cal.reg = IPT_SHAPER_5_SLOW_START_CALr;
  cal.field = SHAPER_5_SLOW_START_CAL_0f;
  cal.index = 0;

  arad_ARAD_ING_SCH_SHAPER_clear(&shaper_temp);
  if (clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_0 == SOC_TMC_ING_SCH_DONT_TOUCH)
  {
      shaper_temp.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
      shaper_temp.max_burst = exact_clos_info->shapers.lp.fabric_multicast.max_burst;
  } else {
      shaper_temp.max_rate = exact_clos_info->shapers.lp.fabric_multicast.max_rate * clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_0 / 100;
      shaper_temp.max_burst = exact_clos_info->shapers.lp.fabric_multicast.max_burst;
  }
  res = arad_ingress_scheduler_shaper_values_set(
     unit,
     TRUE,
     &shaper_temp,
     &max_credit,
     &delay,
     &cal,
     &exact_fabm_max_rate
  );
  SOC_SAND_CHECK_FUNC_RESULT(res,5,exit);
  if (exact_clos_info->shapers.lp.fabric_multicast.max_rate != 0)
  {
      exact_clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_0  = exact_fabm_max_rate * 100 / exact_clos_info->shapers.lp.fabric_multicast.max_rate;
  } else {
      exact_clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_0  = 0;
  }

  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_5_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_5_SLOW_START_DELAYr;
  delay.field = SHAPER_5_SLOW_START_DELAY_1f;
  delay.index = 0;
  cal.reg = IPT_SHAPER_5_SLOW_START_CALr;
  cal.field = SHAPER_5_SLOW_START_CAL_1f;
  cal.index = 0;

  arad_ARAD_ING_SCH_SHAPER_clear(&shaper_temp);
  if (clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_1 == SOC_TMC_ING_SCH_DONT_TOUCH)
  {
      shaper_temp.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
      shaper_temp.max_burst = exact_clos_info->shapers.lp.fabric_multicast.max_burst;
  } else {
      shaper_temp.max_rate = exact_clos_info->shapers.lp.fabric_multicast.max_rate * clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_1 / 100;
      shaper_temp.max_burst = exact_clos_info->shapers.lp.fabric_multicast.max_burst;
  }
  res = arad_ingress_scheduler_shaper_values_set(
       unit,
       TRUE,
       &shaper_temp,
       &max_credit,
       &delay,
       &cal,
       &exact_fabm_max_rate
  );
  SOC_SAND_CHECK_FUNC_RESULT(res,6,exit);
  if (exact_clos_info->shapers.lp.fabric_multicast.max_rate != 0)
  {
      exact_clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_1  = exact_fabm_max_rate * 100 / exact_clos_info->shapers.lp.fabric_multicast.max_rate;
  } else {
      exact_clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_1 = 0;
  }

  


  
  max_credit.reg = IPT_SHAPER_67_MAX_CREDITr;
  max_credit.field = SHAPER_6_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_67_DELAYr;
  delay.field = SHAPER_6_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_67_CALr;
  cal.field = SHAPER_6_CALf;
  cal.index = 0;
  
  res = arad_ingress_scheduler_shaper_values_set(
    unit,
    TRUE,
    &(clos_info->shapers.lp.fabric_unicast),
    &max_credit,
    &delay,
    &cal,
    &exact_fabu_max_rate
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);
  exact_clos_info->shapers.lp.fabric_unicast.max_rate = exact_fabu_max_rate;

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_lp_shapers_set()",0,0);
}



uint32
  arad_ingress_scheduler_clos_lp_shapers_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  ARAD_ING_SCH_CLOS_INFO   *clos_info
  )
{
  uint32
    res;
  ARAD_ING_SCH_SHAPER shaper_temp;
  reg_field max_credit, delay, cal;
  uint32 slow_start_enable;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_CLOS_LP_SHAPERS_GET);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);

  

  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_5_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_45_DELAYr;
  delay.field = SHAPER_5_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_45_CALr;
  cal.field = SHAPER_5_CALf;
  cal.index = 0;

  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &(clos_info->shapers.lp.fabric_multicast)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);


  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_5_SLOW_START_ENABLEf, &slow_start_enable));
  clos_info->shapers.lp.fabric_multicast.slow_start_enable = (slow_start_enable == 0) ? 0 : 1;
  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_5_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_5_SLOW_START_DELAYr;
  delay.field = SHAPER_5_SLOW_START_DELAY_0f;
  delay.index = 0;
  cal.reg = IPT_SHAPER_5_SLOW_START_CALr;
  cal.field = SHAPER_5_SLOW_START_CAL_0f;
  cal.index = 0;

  arad_ARAD_ING_SCH_SHAPER_clear(&shaper_temp);
  res = arad_ingress_scheduler_shaper_values_get(
        unit,
        TRUE,
        &max_credit,
        &delay,
        &cal,
        &shaper_temp
      );
  SOC_SAND_CHECK_FUNC_RESULT(res,5,exit);
  if (clos_info->shapers.lp.fabric_multicast.max_rate != 0)
  {
      clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_0 = shaper_temp.max_rate * 100 / clos_info->shapers.lp.fabric_multicast.max_rate;
  } else {
      clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_0 = 0;
  }

  
  max_credit.reg = IPT_SHAPER_45_MAX_CREDITr;
  max_credit.field = SHAPER_5_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_5_SLOW_START_DELAYr;
  delay.field = SHAPER_5_SLOW_START_DELAY_1f;
  delay.index = 0;
  cal.reg = IPT_SHAPER_5_SLOW_START_CALr;
  cal.field = SHAPER_5_SLOW_START_CAL_1f;
  cal.index = 0;

  arad_ARAD_ING_SCH_SHAPER_clear(&shaper_temp);
  res = arad_ingress_scheduler_shaper_values_get(
        unit,
        TRUE,
        &max_credit,
        &delay,
        &cal,
        &shaper_temp
      );
  SOC_SAND_CHECK_FUNC_RESULT(res,6,exit);
  if (clos_info->shapers.lp.fabric_multicast.max_rate != 0)
  {
      clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_1 = shaper_temp.max_rate * 100 / clos_info->shapers.lp.fabric_multicast.max_rate;
  } else {
      clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_1 = 0;
  }

  
  max_credit.reg = IPT_SHAPER_67_MAX_CREDITr;
  max_credit.field = SHAPER_6_MAX_CREDITf;
  max_credit.index = 0;
  delay.reg = IPT_SHAPER_67_DELAYr;
  delay.field = SHAPER_6_DELAYf;
  delay.index = 0;
  cal.reg = IPT_SHAPER_67_CALr;
  cal.field = SHAPER_6_CALf;
  cal.index = 0;
  
  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &(clos_info->shapers.lp.fabric_unicast)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);
  
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_clos_lp_shapers_get()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

